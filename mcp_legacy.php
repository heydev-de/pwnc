<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   MCP LEGACY CLASS
//==================================================================================================

//modern mcp (2026-07-28) in, legacy mcp (initialize and session, e.g. 2025-11-25) out
//oauth: resource and authorization server metadata and endpoints are mirrored, resource rewritten
//usage: mcp_legacy.php?url=<legacy endpoint>
//authorization is detected: a server that answers anonymously but publishes oauth metadata gets a 401 challenge,
//so the client asks for authorization; override with &auth=none|token|oauth
//redirect uris of the same origin with a query go through a relay, some authorization servers mangle the query

//preload mcp class
require_once(__DIR__ . "/pwnc/#system/sys.mcp.inc");

class mcp_legacy
{

//constants
//--------------------------------------------------------------------------------------------------

    const VERSION    = "2025-11-25";                          //version offered to the upstream
    const URL        = '~^https?://[^\x00-\x20\x7F]+$~i';     //absolute http url without control characters
    const TOKEN      = '~^[\x21-\x7E]{1,200}$~';              //session id and version
    const HEADER     = ["Content-Type: application/json",
                        "Accept: application/json, text/event-stream"];
    const LIMIT_BODY = 16777216;                              //response size
    const LIMIT_META = 1048576;                               //metadata size
    const TIME_READ  = 30;                                    //seconds without data
    const TIME_TOTAL = 120;                                   //seconds per upstream request
    const TIME_PROBE = 4;                                     //seconds without data while detecting oauth
    const TIME_DETECT = 10;                                   //seconds for the whole detection
    const CACHE_TTL  = 86400;                                 //seconds until a session cache file is purged
    const LOG_LIMIT  = 262144;                                //bytes until the log is rotated

//properties
//--------------------------------------------------------------------------------------------------

    private $input  = "";    //request body
    private $json   = NULL;  //request message
    private $target = "";    //upstream endpoint
    private $route  = NULL;  //oauth endpoint
    private $auth   = NULL;  //authorization header
    private $need   = "";    //authorization override, none, token or oauth
    private $back   = "";    //redirect uri of the callback relay
    private $until  = NULL;  //deadline for optional upstream requests
    private $self   = "";    //url of this script
    private $head   = [];    //last upstream header
    private $last   = NULL;  //last upstream message
    private $info   = NULL;  //client identification

//run
//--------------------------------------------------------------------------------------------------

    function run()
    {
        ini_set("display_errors", "0"); //diagnostics never belong into the response

        try
        {
            $this->handle();
        }
        catch (\Throwable $e)
        {
            $this->log("error " . get_class($e) . ": " . $e->getMessage());
            $this->rpc_error("Internal error.", 500);
        };
    }

//--------------------------------------------------------------------------------------------------

    private function handle()
    {
        $this->input = (string)file_get_contents("php://input");
        $this->json  = json_decode($this->input);
        $this->auth  = mcp_http_header("authorization");
        $this->self  = self::self_url();

        $url          = $_GET["url"] ?? "";
        $this->target = is_string($url) ? $url : "";
        $this->need   = in_array($_GET["auth"] ?? "", ["none", "token", "oauth"], TRUE) ? $_GET["auth"] : "";

        //oauth endpoint, target is part of the path
        $path = (string)substr((string)parse_url($_SERVER["REQUEST_URI"] ?? "", PHP_URL_PATH),
                               strlen($_SERVER["SCRIPT_NAME"] ?? ""));

        if (($_GET["oauth"] ?? "") === "resource")
        {
            $this->route = "resource";
        }
        elseif (preg_match("~^/([-\w]+)/(\.well-known/openid-configuration|authorize|token|register)$~", $path, $match))
        {
            $this->route  = $match[2];
            $this->target = $this->unwrap($match[1]);
        }
        elseif (preg_match("~^/([-\w]+)/callback/([-\w]+)$~", $path, $match))
        {
            $this->route  = "callback";
            $this->target = $this->unwrap($match[1]);
            $this->back   = self::decode($match[2]);
        };

        //method
        $allow = in_array($this->route, [NULL, "token", "register"], TRUE) ? "POST" : "GET";
        if (($_SERVER["REQUEST_METHOD"] ?? "") !== $allow)
        {
            header("Allow: $allow");
            $this->reply(["error" => "Method not allowed."], 405);
        };

        //target
        if (! self::safe($this->target)) $this->reply(["error" => "Bad request."], 400);

        if ($this->route === NULL) $this->mcp();
        else                       $this->oauth();
    }

//--------------------------------------------------------------------------------------------------
//   MCP
//--------------------------------------------------------------------------------------------------

    private function mcp()
    {
        $json = $this->json;

        if (! is_object($json) || ! is_string($json->method ?? NULL)) $this->reply(["error" => "Bad request."], 400);

        //token or oauth required, the upstream would answer the tool list anonymously
        if ((($this->need === "token") || ($this->need === "oauth")) && ($this->auth === NULL))
            $this->deny($this->need === "oauth");

        //client identification for the upstream
        $info       = $json->params->_meta->{"io.modelcontextprotocol/clientInfo"} ?? NULL;
        $this->info = is_object($info) ? $info : (object)["name" => "pwnc", "version" => "1"];

        //notification
        if (($json->id ?? NULL) === NULL)
        {
            http_response_code(202);
            exit();
        };

        //discover from initialize result
        if ($json->method === "server/discover")
        {
            if (($state = $this->init()) === NULL) $this->fail();

            $result = $state["result"];

            $this->reply(
                ["jsonrpc" => "2.0",
                 "id"      => $json->id,
                 "result"  => [
                     "_meta"             => ["io.modelcontextprotocol/serverInfo" => $result->serverInfo ?? (object)[]],
                     "resultType"        => "complete",
                     "ttlMs"             => 60000,
                     "cacheScope"        => "private",
                     "supportedVersions" => ["2026-07-28"],
                     "capabilities"      => $result->capabilities ?? (object)[]]
                     + (isset($result->instructions) ? ["instructions" => $result->instructions] : [])]);
        };

        //request without modern metadata
        $request = (object)["jsonrpc" => "2.0",
                            "id"      => $json->id,
                            "method"  => $json->method];

        if (is_object($params = $json->params ?? NULL))
        {
            if (isset($params->_meta) && is_object($params->_meta))
            {
                foreach (array_keys(get_object_vars($params->_meta)) AS $key)
                {
                    if (strncmp($key, "io.modelcontextprotocol/", 24) === 0) unset($params->_meta->$key);
                };

                if (! get_object_vars($params->_meta)) unset($params->_meta);
            };

            $request->params = $params;
        };

        //session
        $state   = $this->cache_load();
        $fresh   = ($state === NULL);
        $checked = ($state["checked"] ?? FALSE) === TRUE;
        if ($fresh && (($state = $this->init()) === NULL)) $this->fail(); //an upstream that answers 401 is relayed as it is

        //oauth server that answers anonymously, the client would never get to ask for authorization
        if (($this->need === "") && ($this->auth === NULL) && ! $checked)
        {
            if (($found = $this->detect()) !== NULL) $this->deny(TRUE, $found["scope"]);

            $checked = TRUE;
            $this->cache_save(["session" => $state["session"], "version" => $state["version"], "checked" => TRUE]);
        };

        //forward
        $message = $this->send($request, $state["session"], $state["version"]);

        //session gone, start over once
        $status = $this->head["#status"] ?? 0;
        if (! $fresh && (($status === 404) || (($status === 400) && ($message === NULL))))
        {
            if (($state = $this->init($checked)) === NULL) $this->fail();
            $message = $this->send($request, $state["session"], $state["version"]);
        };

        if ($message === NULL) $this->fail();

        $this->reply($message);
    }

//--------------------------------------------------------------------------------------------------

    private function init($checked = FALSE)
    {
        //initialize
        $message = $this->send((object)[
            "jsonrpc" => "2.0",
            "id"      => 1,
            "method"  => "initialize",
            "params"  => ["protocolVersion" => self::VERSION,
                          "capabilities"    => (object)[],
                          "clientInfo"      => $this->info]]);

        $version = $message->result->protocolVersion ?? NULL;
        if (! is_string($version) || ! preg_match(self::TOKEN, $version)) return NULL;

        $session = $this->head["mcp-session-id"] ?? NULL;
        if (($session !== NULL) && ! preg_match(self::TOKEN, $session)) $session = NULL;

        //confirm
        $this->send((object)["jsonrpc" => "2.0", "method" => "notifications/initialized"], $session, $version);

        $state = ["session" => $session, "version" => $version];
        $this->cache_save($checked ? $state + ["checked" => TRUE] : $state);

        return $state + ["result" => $message->result];
    }

//--------------------------------------------------------------------------------------------------

    private function send($data,
                          $session = NULL,
                          $version = NULL)
    {
        $header = self::HEADER;

        if ($this->auth !== NULL) $header[] = "Authorization: $this->auth";
        if ($session !== NULL)    $header[] = "Mcp-Session-Id: $session";
        if ($version !== NULL)    $header[] = "MCP-Protocol-Version: $version";

        //send
        $this->last = NULL;
        $body = $this->fetch(($data->method ?? "?") . (($this->auth === NULL) ? " anonymous" : ""),
                             $this->target,
                             $header,
                             self::encode_json($data),
                             self::LIMIT_BODY);
        if ($body === FALSE) return NULL; //no response

        //collect messages
        $list = [];
        if (stripos($this->head["content-type"] ?? "", "text/event-stream") === 0)
        {
            foreach (explode("\n\n", preg_replace("/\r\n|\r/", "\n", $body)) AS $event)
            {
                $array = [];

                foreach (explode("\n", $event) AS $line)
                {
                    if (strncasecmp($line, "data:", 5) !== 0) continue;

                    $line    = substr($line, 5);
                    $array[] = (strncmp($line, " ", 1) === 0) ? substr($line, 1) : $line;
                };

                if ($array) $list[] = json_decode(implode("\n", $array));
            };
        }
        else $list[] = json_decode($body);

        //find response
        foreach ($list AS $message)
        {
            if (! is_object($message) || ! (isset($message->result) || isset($message->error))) continue;

            $this->last = $message;
            if (($message->id ?? NULL) === ($data->id ?? NULL)) return $message;
        };

        return NULL;
    }

//--------------------------------------------------------------------------------------------------

    private function fail()
    {
        $status = $this->head["#status"] ?? 0;

        //relay authorization failure
        if (in_array($status, [401, 403], TRUE))
        {
            //challenge points to this script, the resource would not match otherwise
            $challenge = $this->head["www-authenticate"] ?? "";
            if (preg_match('/resource_metadata="[^"]*"/i', $challenge))
                header("WWW-Authenticate: " .
                       preg_replace('/resource_metadata="[^"]*"/i',
                                    'resource_metadata="' . $this->resource() . '&oauth=resource"',
                                    $challenge));

            http_response_code($status);
            exit();
        };

        $message = $this->last->error->message ?? NULL;
        $this->rpc_error(is_string($message) ? $message : ($this->head["#error"] ?? "No valid response from upstream."));
    }

//--------------------------------------------------------------------------------------------------
//   OAUTH
//--------------------------------------------------------------------------------------------------

    private function oauth()
    {
        $issuer = $this->issuer();

        //resource metadata needs no upstream
        if ($this->route === "resource")
        {
            $result = ["resource"                 => $this->resource(),
                       "authorization_servers"    => [$issuer],
                       "bearer_methods_supported" => ["header"]];

            //oauth flag, no challenge carries the scope
            $scope = ($this->need === "oauth") ? ($this->upstream_resource()->scopes_supported ?? NULL) : NULL;
            if (is_array($scope)) $result["scopes_supported"] = array_values(array_filter($scope, "is_string"));

            $this->reply($result);
        };

        //callback relay, the client gets its redirect uri back with a clean query
        if ($this->route === "callback")
        {
            $query = (string)($_SERVER["QUERY_STRING"] ?? "");

            //same origin only, anything else would be an open redirect
            if (! self::safe($this->back) || (self::origin($this->back) !== self::origin($this->self)) || preg_match('/[\x00-\x20\x7F]/', $query))
                $this->reply(["error" => "Bad request."], 400);

            //issuer identification, the client knows the bridge as authorization server
            if (preg_match('/(^|&)iss=/', $query) && (($upstream = $this->discover()) !== NULL))
                $query = $this->swap_iss($query, $upstream["metadata"]->issuer);

            header("Location: $this->back" . (($query === "") ? "" : ((strpos($this->back, "?") === FALSE) ? "?" : "&") . $query), TRUE, 302);
            exit();
        };

        if (($upstream = $this->discover()) === NULL) $this->reply(["error" => "No authorization server."], 502);

        $metadata = $upstream["metadata"];

        switch ($this->route)
        {
        case ".well-known/openid-configuration":
//..................................................................................................

            $result = ["issuer"                 => $issuer,
                       "authorization_endpoint" => "$issuer/authorize",
                       "token_endpoint"         => "$issuer/token"];

            if (self::endpoint($metadata, "registration_endpoint") !== NULL)
                $result["registration_endpoint"] = "$issuer/register";

            //capabilities
            foreach (["response_types_supported", "grant_types_supported", "code_challenge_methods_supported",
                      "token_endpoint_auth_methods_supported", "scopes_supported"]
                     AS $key)
            {
                if (is_array($metadata->$key ?? NULL)) $result[$key] = array_values(array_filter($metadata->$key, "is_string"));
            };

            //client id metadata document fixes the redirect uri and the issuer of the response, registration goes through the bridge
            if (is_bool($metadata->client_id_metadata_document_supported ?? NULL) && ! isset($result["registration_endpoint"]))
                $result["client_id_metadata_document_supported"] = $metadata->client_id_metadata_document_supported;

            if (is_bool($metadata->authorization_response_iss_parameter_supported ?? NULL))
                $result["authorization_response_iss_parameter_supported"] = $metadata->authorization_response_iss_parameter_supported;

            $this->reply($result);

        case "authorize":
//..................................................................................................

            //browser redirect with upstream resource
            if (($url = self::endpoint($metadata, "authorization_endpoint")) === NULL)
                $this->reply(["error" => "No authorization endpoint."], 502);

            $query = $this->swap_redirect(self::swap_resource((string)($_SERVER["QUERY_STRING"] ?? ""), $upstream["resource"]));
            if (preg_match('/[\x00-\x20\x7F]/', $query)) $this->reply(["error" => "Bad request."], 400);

            header("Location: $url" . (($query === "") ? "" : ((strpos($url, "?") === FALSE) ? "?" : "&") . $query), TRUE, 302);
            exit();

        case "token":
        case "register":
//..................................................................................................

            $url = self::endpoint($metadata, ($this->route === "token") ? "token_endpoint" : "registration_endpoint");
            if ($url === NULL) $this->reply(["error" => "Endpoint not supported."], 404);

            $header = ($this->route === "token") ?
                      ["Content-Type: application/x-www-form-urlencoded", "Accept: application/json"] :
                      ["Content-Type: application/json", "Accept: application/json"];
            if ($this->auth !== NULL) $header[] = "Authorization: $this->auth";

            //redirect uri of the relay
            $body = $this->input;
            if ($this->route === "token")
            {
                $body = $this->swap_redirect(self::swap_resource($body, $upstream["resource"]));
            }
            elseif (is_object($json = json_decode($body)) && is_array($json->redirect_uris ?? NULL))
            {
                $array = array_map([$this, "redirect_uri"], $json->redirect_uris);

                //body stays as it is without a change
                if ($array !== $json->redirect_uris)
                {
                    $json->redirect_uris = $array;
                    $body                = self::encode_json($json);
                };
            };

            $this->relay($this->fetch($this->route, $url, $header, $body, self::LIMIT_META));
        };
    }

//--------------------------------------------------------------------------------------------------

    private function upstream_resource($challenge = TRUE,
                                       $wait      = self::TIME_READ)
    {
        //challenge of anonymous request
        if ($challenge) $this->fetch("discover", $this->target, self::HEADER, '{"jsonrpc":"2.0","id":1,"method":"tools/list"}', self::LIMIT_META, $wait);

        $origin = self::origin($this->target);
        $dir    = rtrim((string)parse_url($this->target, PHP_URL_PATH), "/");
        $list   = [];

        if ($challenge && preg_match('/resource_metadata="([^"]+)"/i', $this->head["www-authenticate"] ?? "", $match) && self::safe($match[1]))
            $list[] = $match[1];

        $list[] = "$origin/.well-known/oauth-protected-resource$dir";
        $list[] = "$origin/.well-known/oauth-protected-resource";

        //protected resource metadata
        $resource = NULL;
        foreach ($list AS $url)
        {
            $json = json_decode((string)$this->fetch("resource", $url, ["Accept: application/json"], NULL, self::LIMIT_META, $wait));

            if (is_object($json) && is_string($json->resource ?? NULL))
            {
                $resource = $json;
                break;
            };
        };

        return $resource;
    }

//--------------------------------------------------------------------------------------------------

    private function discover($challenge = TRUE,
                              $wait      = self::TIME_READ)
    {
        if (($resource = $this->upstream_resource($challenge, $wait)) === NULL) return NULL;

        //authorization server metadata
        foreach (array_slice((array)($resource->authorization_servers ?? []), 0, 3) AS $issuer)
        {
            if (! self::safe($issuer)) continue;

            $base = self::origin($issuer);
            $dir  = rtrim((string)parse_url($issuer, PHP_URL_PATH), "/");

            foreach (($dir === "") ?
                     ["$base/.well-known/oauth-authorization-server",
                      "$base/.well-known/openid-configuration"] :
                     ["$base/.well-known/oauth-authorization-server$dir",
                      "$base/.well-known/openid-configuration$dir",
                      "$base$dir/.well-known/openid-configuration"]
                     AS $url)
            {
                $metadata = json_decode((string)$this->fetch("metadata", $url, ["Accept: application/json"], NULL, self::LIMIT_META, $wait));

                if (is_object($metadata) && (($metadata->issuer ?? NULL) === $issuer))
                    return ["resource" => $resource->resource,
                            "metadata" => $metadata,
                            "scope"    => implode(" ", array_filter((array)($resource->scopes_supported ?? []), "is_string"))];
            };
        };

        return NULL;
    }

//--------------------------------------------------------------------------------------------------

    private function relay($body)
    {
        $status = (int)($this->head["#status"] ?? 0);
        $json   = is_string($body) ? json_decode($body) : NULL;

        //json only, an upstream cannot place html or script on this origin
        if (! is_object($json) && ! is_array($json))
        {
            $body   = json_encode(["error"             => "server_error",
                                   "error_description" => "Invalid response from the authorization server."]);
            $status = (($status >= 400) && ($status < 600)) ? $status : 502;
        };

        http_response_code($status ?: 502);
        header("Content-Type: application/json; charset=utf-8");
        header("Cache-Control: no-store, private");
        header("X-Content-Type-Options: nosniff");
        echo($body);
        exit();
    }

//--------------------------------------------------------------------------------------------------
//   HTTP
//--------------------------------------------------------------------------------------------------

    private function fetch($label,
                           $url,
                           $header  = [],
                           $content = NULL,
                           $limit   = self::LIMIT_META,
                           $wait    = self::TIME_READ)
    {
        $this->head = ["#status" => 0];
        $body       = FALSE;
        $total      = ($wait === self::TIME_READ) ? self::TIME_TOTAL : $wait * 2;

        //optional request out of time
        if (($this->until !== NULL) && (time() >= $this->until)) return FALSE;

        $stream = @fopen($url,
                         "r",
                         FALSE,
                         stream_context_create(
                             ["http" => ["method"          => ($content === NULL) ? "GET" : "POST",
                                         "header"          => implode("\r\n", $header),
                                         "content"         => (string)$content,
                                         "user_agent"      => "mcp_legacy",
                                         "ignore_errors"   => TRUE,
                                         "follow_location" => 0,
                                         "timeout"         => $wait]]));

        if ($stream !== FALSE)
        {
            stream_set_timeout($stream, $wait);

            $time = $idle = time();
            $body = "";
            while (! feof($stream))
            {
                $data = fread($stream, 65536);

                if ($data === FALSE)                       { $body = FALSE; $this->head["#error"] = "Upstream read failed."; break; };
                if ((time() - $time) > $total)             { $body = FALSE; $this->head["#error"] = "Upstream too slow.";    break; };

                //no data
                if ($data === "")
                {
                    if ((time() - $idle) > $wait) { $body = FALSE; $this->head["#error"] = "Upstream timeout."; break; };
                    continue;
                };

                $idle  = time();
                $body .= $data;
                if (strlen($body) > $limit) { $body = FALSE; $this->head["#error"] = "Upstream response too large."; break; };
            };

            //response header
            foreach ((array)(stream_get_meta_data($stream)["wrapper_data"] ?? []) AS $line)
            {
                if (! is_string($line)) continue;

                if (preg_match("/^HTTP\/\S+\s+(\d+)/", $line, $match)) $this->head["#status"] = (int)$match[1];
                elseif ($offset = strpos($line, ":")) $this->head[strtolower(substr($line, 0, $offset))] =
                                                          trim(substr($line, $offset + 1));
            };

            fclose($stream);
        };

        //log exchange, never the token and only failed responses with their content
        $failed = ($body === FALSE) || ($this->head["#status"] === 0) || ($this->head["#status"] >= 400);
        $this->log(substr($label, 0, 60) . " " . parse_url($url, PHP_URL_HOST) .
                   " status=" . $this->head["#status"] .
                   " challenge=" . substr($this->head["www-authenticate"] ?? "-", 0, 200) .
                   (($failed && ! in_array($label, ["token", "register"], TRUE)) ?
                    " body=" . substr(preg_replace("/\s+/", " ", (string)$body), 0, 300) : ""));

        return $body;
    }

//--------------------------------------------------------------------------------------------------

    private function log($text)
    {
        $file = __DIR__ . "/data/#log/mcp_legacy.txt";
        $line = date("Y-m-d H:i:s") . " " . preg_replace('/[^\x20-\x7E]/', "?", $text) . "\n";

        //rotate
        if (@filesize($file) > self::LOG_LIMIT) @rename($file, "$file.1");

        @file_put_contents($file, $line, FILE_APPEND);
    }

//--------------------------------------------------------------------------------------------------

    private function reply($value,
                           $http_code = 200)
    {
        http_response_code($http_code);
        header("Content-Type: application/json; charset=utf-8");
        header("Cache-Control: no-store, private");
        header("X-Content-Type-Options: nosniff");
        echo(self::encode_json($value));
        exit();
    }

//--------------------------------------------------------------------------------------------------

    private function rpc_error($message,
                               $http_code = 502)
    {
        $this->reply(
            ["jsonrpc" => "2.0",
             "id"      => is_object($this->json) ? ($this->json->id ?? NULL) : NULL,
             "error"   => ["code" => -32603, "message" => (string)$message]],
            $http_code);
    }

//--------------------------------------------------------------------------------------------------
//   SESSION CACHE
//--------------------------------------------------------------------------------------------------

    private function cache_file()
    {
        return sys_get_temp_dir() . "/mcp_legacy_" . hash("sha256", "$this->target\0$this->auth");
    }

//--------------------------------------------------------------------------------------------------

    private function cache_load()
    {
        $state   = json_decode((string)@file_get_contents($this->cache_file()), TRUE);
        $version = is_array($state) ? ($state["version"] ?? NULL) : NULL;
        $session = is_array($state) ? ($state["session"] ?? NULL) : NULL;

        if (! is_string($version) || ! preg_match(self::TOKEN, $version)) return NULL;
        if (($session !== NULL) && (! is_string($session) || ! preg_match(self::TOKEN, $session))) return NULL;

        return ["session" => $session, "version" => $version, "checked" => ($state["checked"] ?? NULL) === TRUE];
    }

//--------------------------------------------------------------------------------------------------

    private function cache_save($state)
    {
        $file = $this->cache_file();

        @file_put_contents($file, json_encode($state), LOCK_EX);
        @chmod($file, 0600);

        //purge stale files
        foreach (glob(sys_get_temp_dir() . "/mcp_legacy_*") ?: [] AS $_file)
        {
            if (preg_match('/mcp_legacy_[0-9a-f]{64}$/', $_file) && ((time() - (int)@filemtime($_file)) > self::CACHE_TTL))
                @unlink($_file);
        };
    }

//--------------------------------------------------------------------------------------------------
//   HELPER
//--------------------------------------------------------------------------------------------------

    private function detect()
    {
        //oauth metadata only, an upstream that hangs or fails is treated like one without oauth
        $this->until = time() + self::TIME_DETECT;
        $found       = $this->discover(FALSE, self::TIME_PROBE);
        $this->until = NULL;

        $this->log("detect " . parse_url($this->target, PHP_URL_HOST) . " oauth=" . (($found === NULL) ? "no" : "yes"));
        if ($found === NULL) return NULL;

        //the scope goes into a quoted challenge parameter
        return ["scope" => trim((string)preg_replace('/[^\x20\x21\x23-\x5B\x5D-\x7E]/', "", $found["scope"]))];
    }

//--------------------------------------------------------------------------------------------------

    private function deny($oauth,
                          $scope = "")
    {
        header("WWW-Authenticate: Bearer" .
               ($oauth ?
                ' resource_metadata="' . $this->resource() . '&oauth=resource"' . (($scope === "") ? "" : ', scope="' . $scope . '"') :
                ""));
        $this->reply(["error" => "Authorization required."], 401);
    }

//--------------------------------------------------------------------------------------------------

    private function unwrap($value)
    {
        $value = self::decode($value);

        //issuers of an earlier version carry a flag
        return (strncmp($value, "oauth|", 6) === 0) ? substr($value, 6) : $value;
    }

//--------------------------------------------------------------------------------------------------

    private function issuer()
    {
        return $this->self . "/" . self::encode($this->target);
    }

//--------------------------------------------------------------------------------------------------

    private function redirect_uri($uri)
    {
        //a query string in the redirect uri is mangled by some authorization servers, the relay restores it,
        //only redirect uris of this origin, the relay does not leave it
        return (is_string($uri) && (strpos($uri, "?") !== FALSE) && self::safe($uri) && (self::origin($uri) === self::origin($this->self))) ?
               $this->issuer() . "/callback/" . self::encode($uri) : $uri;
    }

//--------------------------------------------------------------------------------------------------

    private function swap_iss($query,
                              $from)
    {
        $array = [];

        //only the issuer of the upstream, anything else stays and fails at the client
        foreach (explode("&", $query) AS $pair)
        {
            $part    = explode("=", $pair, 2);
            $array[] = ((urldecode($part[0]) === "iss") && (urldecode($part[1] ?? "") === $from)) ?
                       "iss=" . rawurlencode($this->issuer()) : $pair;
        };

        return implode("&", $array);
    }

//--------------------------------------------------------------------------------------------------

    private function swap_redirect($query)
    {
        $pairs = ($query === "") ? [] : explode("&", $query);
        $array = [];

        //client id metadata document, its redirect uris are fixed
        foreach ($pairs AS $pair)
        {
            $part = explode("=", $pair, 2);
            if ((urldecode($part[0]) === "client_id") && preg_match("~^https?://~i", urldecode($part[1] ?? ""))) return $query;
        };

        foreach ($pairs AS $pair)
        {
            $part    = explode("=", $pair, 2);
            $uri     = urldecode($part[1] ?? "");
            $array[] = ((urldecode($part[0]) === "redirect_uri") && isset($part[1]) && (($new = $this->redirect_uri($uri)) !== $uri)) ?
                       "redirect_uri=" . rawurlencode($new) : $pair;
        };

        return implode("&", $array);
    }

//--------------------------------------------------------------------------------------------------

    private function resource()
    {
        //flag stays, the client adopts the resource as its server url
        return $this->self . "?url=" . rawurlencode($this->target) . (($this->need === "") ? "" : "&auth=$this->need");
    }

//--------------------------------------------------------------------------------------------------

    private static function safe($url)
    {
        return is_string($url) && preg_match(self::URL, $url) && is_string(parse_url($url, PHP_URL_HOST));
    }

//--------------------------------------------------------------------------------------------------

    private static function endpoint($metadata,
                                     $name)
    {
        $url = $metadata->$name ?? NULL;
        return self::safe($url) ? $url : NULL;
    }

//--------------------------------------------------------------------------------------------------

    private static function origin($url)
    {
        return parse_url($url, PHP_URL_SCHEME) . "://" . parse_url($url, PHP_URL_HOST) .
               (($port = parse_url($url, PHP_URL_PORT)) ? ":$port" : "");
    }

//--------------------------------------------------------------------------------------------------

    private static function self_url()
    {
        $https = ! preg_match("/^(?:|off)$/i", $_SERVER["HTTPS"] ?? "") ||
                 (strncasecmp($_SERVER["HTTP_X_FORWARDED_PROTO"] ?? "", "https", 5) === 0);
        $host  = $_SERVER["HTTP_HOST"] ?? "";
        if (! preg_match('/^[-.:\[\]\w]+$/', $host)) $host = $_SERVER["SERVER_NAME"] ?? "";

        return ($https ? "https" : "http") . "://$host" . ($_SERVER["SCRIPT_NAME"] ?? "");
    }

//--------------------------------------------------------------------------------------------------

    private static function swap_resource($query,
                                          $value)
    {
        $array = [];

        //other pairs stay as they are, names with dots or brackets included
        foreach (($query === "") ? [] : explode("&", $query) AS $pair)
            $array[] = (urldecode(explode("=", $pair, 2)[0]) === "resource") ? "resource=" . rawurlencode($value) : $pair;

        return implode("&", $array);
    }

//--------------------------------------------------------------------------------------------------

    private static function encode($value)
    {
        return rtrim(strtr(base64_encode($value), "+/", "-_"), "=");
    }

//--------------------------------------------------------------------------------------------------

    private static function decode($value)
    {
        return (string)base64_decode(strtr($value, "-_", "+/"));
    }

//--------------------------------------------------------------------------------------------------

    private static function encode_json($value)
    {
        return json_encode($value, JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE | JSON_PRESERVE_ZERO_FRACTION | JSON_PARTIAL_OUTPUT_ON_ERROR);
    }
}

//==================================================================================================

(new mcp_legacy())->run();