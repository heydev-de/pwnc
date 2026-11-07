<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   MCP CLIENT CLASS
//==================================================================================================

class mcp_client
{

//properties
//--------------------------------------------------------------------------------------------------

    public $url            = NULL;         //endpoint
    public $token          = NULL;         //bearer token
    public $header         = [];           //last http header
    public $error          = NULL;         //last failure

    //configuration
    private $info          = [];           //client identification
    private $capability    = [];           //client capabilities
    private $cache_key     = NULL;         //private cache identifier
    private $timeout_total = 30;           //seconds
    private $timeout_chunk = 30;           //seconds
    private $page_limit    = 100;          //pages per list

    //request
    private $status        = 0;            //last http status
    private static $id     = 0;            //request counter

    const VERSION          = "2026-07-28"; //mcp version

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($url,
                         $token  = NULL,
                         $option = NULL)
    {
        $this->url   = $url;
        $this->token = $token;
        $this->info  = ["name"    => CMS_SOFTWARE,
                        "version" => CMS_VERSION];

        if (isset($option["timeout_total"])) $this->timeout_total = $option["timeout_total"];
        if (isset($option["timeout_chunk"])) $this->timeout_chunk = $option["timeout_chunk"];
        if (isset($option["capability"]))    $this->capability    = $option["capability"];
        if (isset($option["cache_key"]))     $this->cache_key     = $option["cache_key"];
    }

//methods
//--------------------------------------------------------------------------------------------------

    private function send_request($method,
                                  $param  = NULL,
                                  $option = NULL)
    {
        $this->header = [];
        $this->status = 0;
        $this->error  = NULL;

        //load library
        if (! cms_load("http")) return $this->error("Internal error.");

        //per request protocol metadata
        $param["_meta"] = (array)($option["meta"] ?? []) + ($param["_meta"] ?? []) +
            ["io.modelcontextprotocol/protocolVersion"    => self::VERSION,
             "io.modelcontextprotocol/clientInfo"         => $this->info,
             "io.modelcontextprotocol/clientCapabilities" => (object)$this->capability];

        //mirror in http header
        switch ($method)
        {
        case "tools/call":
        case "prompts/get":

            $name = $param["name"] ?? NULL;
            break;

        case "resources/read":

            $name = $param["uri"] ?? NULL;
            break;

        default:

            $name = NULL;
        };

        $id   = ++self::$id;
        $body = json_encode(
            ["jsonrpc" => "2.0",
             "id"      => $id,
             "method"  => $method,
             "params"  => $param],
            JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE);

        //send
        $reply = http_request(
            $this->url,
            ["method"        => "POST",
             "header"        => $this->build_header($method, $name, $param, $option),
             "body"          => $body,
             "timeout_total" => $this->timeout_total,
             "timeout_chunk" => $this->timeout_chunk,
             "limit"         => $option["limit"] ?? NULL],
            $header);
        if ($reply === FALSE) return $this->error("No response.");

        //authorization failed
        $this->header = $header;
        $this->status = (int)$header["#status"];
        if (in_array($this->status, [401, 403], TRUE))
            return $this->error("HTTP " . $this->status . ".");

        //result for this request, error with any id
        $json = $this->get_response($reply, $header["content-type"] ?? "", $id);
        if (! is_array($json) || (! isset($json["error"]) && (($json["id"] ?? NULL) !== $id)))
            return $this->error("Invalid response.");

        //protocol error
        if (isset($json["error"]))
            return $this->error((string)($json["error"]["message"] ?? "Error") .
                                " (" . (int)($json["error"]["code"] ?? 0) . ")");

        //get result
        $result = $json["result"] ?? NULL;
        if (! is_array($result)) return $this->error("No result.");

        //absent result type means complete
        switch ($result["resultType"] ?? "complete")
        {
        case "complete":

            break;

        case "input_required":

            return $this->error("Additional input required.");

        default:

            return $this->error("Unsupported result type.");
        };

        return $result;
    }

//--------------------------------------------------------------------------------------------------

    private function get_response($body,
                                  $type,
                                  $id)
    {
        //no event stream
        if (stripos((string)$type, "text/event-stream") !== 0)
            return json_decode($body, TRUE);

        //collect event data
        $array = [];
        foreach (preg_split("/\r\n|\r|\n/", $body) AS $line)
        {
            //comment
            if (strncmp($line, ":", 1) === 0) continue;

            //data field
            if (strncasecmp($line, "data:", 5) === 0)
            {
                $line    = substr($line, 5);
                $array[] = (strncmp($line, " ", 1) === 0) ? substr($line, 1) : $line;
                continue;
            };

            //other field
            if ($line !== "") continue;

            //end of event
            if (blank($array)) continue;

            $json  = json_decode(implode("\n", $array), TRUE);
            $array = []; //reset

            //verify response id
            if (($json["id"] ?? NULL) === $id) return $json;
        };

        //unterminated final event
        if (blank($array)) return NULL;

        $json = json_decode(implode("\n", $array), TRUE);

        return (($json["id"] ?? NULL) === $id) ? $json : NULL;
    }

//--------------------------------------------------------------------------------------------------

    private function build_header($method,
                                  $name   = NULL,
                                  $param  = NULL,
                                  $option = NULL)
    {
        $header = ["Content-Type"         => "application/json",
                   "Accept"               => "application/json, text/event-stream",
                   "MCP-Protocol-Version" => self::VERSION,
                   "MCP-Method"           => $method];

        if (nstre($this->token)) $header["Authorization"] = "Bearer " . $this->token;
        if (nstre($name))        $header["MCP-Name"]      = self::encode_value($name);

        //mirror parameters in http header
        foreach ((array)($option["mirror"] ?? []) AS $path => $_name)
        {
            $value = array_get_path((array)($param["arguments"] ?? []), $path);
            if ($value !== NULL) $header["MCP-Param-$_name"] = self::encode_value($value);
        };

        return $header;
    }

//--------------------------------------------------------------------------------------------------

    private static function map_header($schema,
                                       $path,
                                       &$map,
                                       &$seen = [])
    {
        $property = $schema["properties"] ?? NULL;
        if (! is_array($property)) return TRUE;

        foreach ($property AS $key => $value)
        {
            if (! is_array($value)) continue;

            $_path = ($path === "") ? (string)$key : "$path/$key";

            if (isset($value["x-mcp-header"]))
            {
                $name = $value["x-mcp-header"];
                if (! is_string($name)) return FALSE;

                //name must be non empty http field token
                if (! preg_match("/^[!#$%&'*+-.^_`|~[:alnum:]]+$/", $name)) return FALSE;

                //name must be unique
                $_name = strtolower($name);
                if (isset($seen[$_name])) return FALSE;
                $seen[$_name] = TRUE;

                //primitive only, fractional number excluded
                if (! in_array($value["type"] ?? NULL, ["boolean", "integer", "string"], TRUE)) return FALSE;

                $map[$_path] = $name;
            };

            //nested object
            if (! self::map_header($value, $_path, $map, $seen)) return FALSE;
        };

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------
//   SERVER
//--------------------------------------------------------------------------------------------------

    function server_discover()
    {
        return $this->send_request("server/discover");
    }

//--------------------------------------------------------------------------------------------------
//   TOOLS
//--------------------------------------------------------------------------------------------------

    function tools_list($option = NULL)
    {
        $key = "mcp_client.tools_list." . hash64($this->url);

        //retrieve from cache
        if ($this->cache_key !== NULL)
        {
            $time = time();
            foreach (["$key." . $this->cache_key, $key] AS $_key)
            {
                if ((cms_cache_time($_key) + CMS_CACHE_TTL) > $time)
                    return cms_cache_notouch($_key);
            };
        };

        $array  = [];
        $cursor = NULL;
        $ttl    = PHP_INT_MAX;
        $public = TRUE;

        for ($i = 0; $i < $this->page_limit; $i++)
        {
            $param  = ($cursor === NULL) ? NULL : ["cursor" => $cursor];
            $result = $this->send_request("tools/list", $param, $option);
            if ($result === FALSE) return FALSE;

            //collect tool definitions
            foreach ((array)($result["tools"] ?? []) AS $tool)
            {
                //named tool with object schema
                $schema = $tool["inputSchema"] ?? [];
                if (! is_string($tool["name"] ?? NULL) || stre($tool["name"]))  continue; //invalid name
                if (! is_array($schema) || ($schema && array_is_list($schema))) continue; //no json object
                if (($schema["type"] ?? "object") !== "object")                 continue; //invalid type

                $map = [];
                if (! self::map_header($schema, "", $map)) continue; //invalid http header annotation

                $tool["schema"] = $schema + ["type" => "object"];
                $tool["header"] = $map;
                $array[]        = $tool;
            };

            //use lowest ttl and prefer private caching
            $ttl    = min($ttl, intdiv((int)($result["ttlMs"] ?? 0), 1000));
            $public = $public && (($result["cacheScope"] ?? "private") === "public");

            //next page
            $cursor = $result["nextCursor"] ?? NULL;
            if (! is_string($cursor)) break;
        };

        //cache tool list
        if (($this->cache_key !== NULL) && ($ttl > 0))
        {
            if (! $public) $key .= "." . $this->cache_key;
            cms_cache($key, $array, TRUE, $ttl);
        };

        return $array;
    }

//--------------------------------------------------------------------------------------------------

    function tools_call($name,
                        $argument = NULL,
                        $option   = NULL)
    {
        return $this->send_request(
            "tools/call",
            ["name"      => $name,
             "arguments" => (object)(array)$argument],
            $option);
    }

//--------------------------------------------------------------------------------------------------
//   RESOURCES
//--------------------------------------------------------------------------------------------------

    function resources_list($option = NULL)
    {
        return $this->send_request("resources/list", NULL, $option);
    }

//--------------------------------------------------------------------------------------------------

    function resources_read($uri,
                            $option = NULL)
    {
        return $this->send_request("resources/read", ["uri" => $uri], $option);
    }

//--------------------------------------------------------------------------------------------------
//   PROMPTS
//--------------------------------------------------------------------------------------------------

    function prompts_list($option = NULL)
    {
        return $this->send_request("prompts/list", NULL, $option);
    }

//--------------------------------------------------------------------------------------------------

    function prompts_get($name,
                         $argument = NULL,
                         $option   = NULL)
    {
        return $this->send_request(
            "prompts/get",
            ["name"      => $name,
             "arguments" => (object)(array)$argument],
            $option);
    }

//--------------------------------------------------------------------------------------------------
//   AUTHORIZATION
//--------------------------------------------------------------------------------------------------

    function auth_discover()
    {
        //anonymous
        $token       = $this->token;
        $this->token = NULL;
        $result      = $this->send_request("tools/list");
        $this->token = $token;

        if ($result !== FALSE)     return ["auth" => "none", "resource" => $this->url];
        if ($this->status !== 401) return FALSE;

        //challenge and origin
        $challenge = parse_http_param($this->header["www-authenticate"] ?? "");
        $origin    = normalize_url($this->url, TRUE);
        $path      = rtrim(parse_url($this->url, PHP_URL_PATH) ?? "", "/");

        //protected resource metadata
        $resource = NULL;
        $list     = array_filter(
            [$challenge["resource_metadata"] ?? "",
             ($path !== "") ? "$origin/.well-known/oauth-protected-resource$path" : "",
             "$origin/.well-known/oauth-protected-resource"],
            __NAMESPACE__ . "\\nstre");

        foreach ($list AS $url)
        {
            $resource = self::json_rpc($url);
            if (is_string($resource["resource"] ?? NULL)) break;
            $resource = NULL;
        };

        //static token
        if ($resource === NULL) return ["auth" => "token", "resource" => $this->url];

        //resource must share origin
        if (normalize_url($resource["resource"], TRUE) !== $origin)
            return $this->error("Resource mismatch.");

        //authorization server metadata
        foreach (array_filter((array)($resource["authorization_servers"] ?? []), "is_string") AS $issuer)
        {
            //metadata urls
            $base = normalize_url($issuer, TRUE);
            $path = rtrim(parse_url((string)$issuer, PHP_URL_PATH) ?? "", "/");
            $list = ($path === "") ?
                    ["$base/.well-known/oauth-authorization-server",
                     "$base/.well-known/openid-configuration"] :
                    ["$base/.well-known/oauth-authorization-server$path",
                     "$base/.well-known/openid-configuration$path",
                     "$base$path/.well-known/openid-configuration"];

            foreach ($list AS $url)
            {
                $metadata = self::json_rpc($url);

                if (

                //issuer matches
                     (($metadata["issuer"] ?? NULL) === $issuer)
                     &&

                //has absolute endpoints
                     ! in_array(FALSE, array_map(
                         fn($url) => is_string($url) && nstre(normalize_url($url, TRUE)),
                         [$metadata["authorization_endpoint"] ?? NULL,
                          $metadata["token_endpoint"]         ?? NULL]),
                         TRUE)
                     &&

                //pkce s256 supported
                     in_array("S256", (array)($metadata["code_challenge_methods_supported"] ?? []), TRUE)
                   )
                {
                    //challenge scope or resource supported scopes
                    $scope = array_filter((array)($resource["scopes_supported"] ?? []), "is_string"); //string only
                    $scope = $challenge["scope"] ?? implode(" ", $scope);
                    $scope = preg_split("/\s+/", $scope, -1, PREG_SPLIT_NO_EMPTY);

                    //refresh token
                    if (in_array("offline_access", (array)($metadata["scopes_supported"] ?? []), TRUE))
                        $scope[] = "offline_access";

                    //oauth
                    return ["auth"     => "oauth",
                            "resource" => $resource["resource"],
                            "scope"    => implode(" ", array_unique($scope)),
                            "metadata" => $metadata];
                };
            };
        };

        //error
        return $this->error("Incompatible authorization server.");
    }

//--------------------------------------------------------------------------------------------------

    function auth_register($metadata,
                           $redirect,
                           $document,
                           $auth = NULL)
    {
        //pre-registered client
        if (nstre($auth["client_id"] ?? ""))
        {
            $supported = (array)($metadata["token_endpoint_auth_methods_supported"] ?? ["client_secret_basic"]);

            if (stre($auth["client_secret"] ?? ""))                    $method = "none";
            elseif (in_array("client_secret_basic", $supported, TRUE)) $method = "client_secret_basic";
            else                                                       $method = "client_secret_post";

            return $auth + ["auth_method" => $method];
        };

        //client id metadata document
        if (
             ($metadata["client_id_metadata_document_supported"] ?? FALSE)
             &&
             (strncasecmp($document, "https://", 8) === 0)
           )
            return ["client_id" => $document, "auth_method" => "none"];

        //no registration endpoint
        if (stre($metadata["registration_endpoint"] ?? ""))
            return $this->error("Client registration not supported.");

        //dynamic client registration
        $json = (array)self::json_rpc(
            $metadata["registration_endpoint"],
            ["Content-Type" => "application/json"],
            json_encode(
                ["redirect_uris"              => [$redirect],
                 "token_endpoint_auth_method" => "none",
                 "grant_types"                => ["authorization_code", "refresh_token"],
                 "response_types"             => ["code"],
                 "client_name"                => CMS_SOFTWARE . " (" . CMS_DOMAIN . ")",
                 "application_type"           => "web"],
                JSON_UNESCAPED_SLASHES));
        $json = array_filter($json, "is_string"); //string only

        //registered client
        if (nstre($json["client_id"] ?? ""))
            return ["client_id"     => $json["client_id"],
                    "client_secret" => $json["client_secret"] ?? NULL,
                    "auth_method"   => $json["token_endpoint_auth_method"] ??
                                       (isset($json["client_secret"]) ? "client_secret_basic" : "none")];

        //error
        $error = $json["error_description"] ?? $json["error"] ?? NULL;
        return $this->error(is_string($error) ? $error : "Client registration failed.");
    }

//--------------------------------------------------------------------------------------------------

    static function auth_url($metadata,
                             $client_id,
                             $param)
    {
        //random state and verifier
        $state    = bin2hex(random_bytes(16));
        $verifier = base64url(random_bytes(32));

        //pkce s256
        $param = ["response_type"         => "code",
                  "client_id"             => $client_id,
                  "state"                 => $state,
                  "code_challenge"        => base64url(hash("sha256", $verifier, TRUE)),
                  "code_challenge_method" => "S256"] +
                  $param;

        //authorization url
        $address = parse_url((string)($metadata["authorization_endpoint"] ?? "")) ?: [];

        //merge querystrings
        parse_str($address["query"] ?? "", $query);
        $address["query"] = substr(querystring($query, $param), 1);

        return ["url"      => cms_build_url($address),
                "state"    => $state,
                "verifier" => $verifier];
    }

//--------------------------------------------------------------------------------------------------

    function auth_token($endpoint,
                        $auth,
                        $param)
    {
        //form request
        $header = ["Content-Type" => "application/x-www-form-urlencoded"];

        //authentication
        switch ($auth["auth_method"] ?? "none")
        {
        case "client_secret_basic":

            $header["Authorization"] = "Basic " .
                base64_encode(urlencode($auth["client_id"]) . ":" .
                              urlencode($auth["client_secret"] ?? ""));
            break;

        case "client_secret_post":

            $param["client_secret"] = $auth["client_secret"] ?? "";

        default:

            $param["client_id"] = $auth["client_id"];
        };

        //send request
        $json = (array)self::json_rpc($endpoint, $header, http_build_query($param));
        $json = array_filter($json, "is_scalar"); //scalar only

        //bearer token
        if (
             nstre($json["access_token"] ?? "")
             &&
             strieq("bearer", $json["token_type"] ?? "bearer")
           )
            return $json;

        //error
        $this->error = $json["error_description"] ?? $json["error"] ??
                       (isset($json["access_token"]) ? "Unsupported token type." : "Token request failed.");

        //invalid grant rejected, other failure retried
        return (($json["error"] ?? NULL) === "invalid_grant") ? FALSE : NULL;
    }

//--------------------------------------------------------------------------------------------------
//   HELPER
//--------------------------------------------------------------------------------------------------

    private static function json_rpc($url,
                                     $header = [],
                                     $body   = NULL)
    {
        if (! cms_load("http")) return NULL;

        $reply = http_request(
            $url,
            ["header" => $header + ["Accept" => "application/json"],
             "body"   => $body]);
        $json  = json_decode($reply, TRUE);

        return is_array($json) ? $json : NULL;
    }

//--------------------------------------------------------------------------------------------------

    static function to_text($result)
    {
        $text = fn($value) => is_scalar($value) ? (string)$value : ""; //scalar only

        $array = [];
        foreach ((array)($result["content"] ?? []) AS $block)
        {
            switch ($block["type"] ?? "")
            {
            case "text":

                $array[] = $text($block["text"] ?? "");
                break;

            case "resource":

                $resource = $block["resource"] ?? [];
                $array[]  = isset($resource["text"]) ?
                            $text($resource["text"]) :
                            $text($resource["uri"] ?? "") . " (" .
                            $text($resource["mimeType"] ?? "application/octet-stream") . ", binary)";
                break;

            case "resource_link":

                $array[] = $text($block["uri"] ?? "");
            };
        };

        return implode("\n", $array);
    }

//--------------------------------------------------------------------------------------------------

    static function encode_value($value)
    {
        if (is_bool($value)) return $value ? "true" : "false";

        $value = (string)$value;
        return preg_match("/^\s|[^\x20-\x7E]|^=\?base64\?.*\?=$|\s$/", $value) ?
               "=?base64?" . base64_encode($value) . "?=" : $value;
    }

//--------------------------------------------------------------------------------------------------

    private function error($message)
    {
        $this->error = $message;
        return FALSE;
    }
};