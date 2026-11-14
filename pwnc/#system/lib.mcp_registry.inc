<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   MCP REGISTRY CLASS
//==================================================================================================

class mcp_registry
{

//properties
//--------------------------------------------------------------------------------------------------

    public $data    = NULL; //server list
    private $client = [];   //client objects
    private $option = NULL; //client options

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($option = NULL)
    {
        cms_load("mcp_client");
        $this->data   = new data("#system/mcp.server");
        $this->option = $option;
    }

//--------------------------------------------------------------------------------------------------
//   SERVER
//--------------------------------------------------------------------------------------------------

    function list($user)
    {
        $this->load();

        $array = [];
        foreach (array_keys($this->data->get() ?? []) AS $id)
        {
            if (! cms_permission("mcp.$id", NULL, NULL, $user)) continue; //no access
            if (($token = $this->token($id)) === FALSE)         continue; //no token
            $array[$id] = $this->client($id, $token);
        };

        return $array;
    }

//--------------------------------------------------------------------------------------------------

    function client($id,
                    $token = NULL)
    {
        $client = $this->client[$id] ?? NULL;
        $url    = $this->data->get($id, "url") ?? "";

        //new or changed server or authorization
        if (($client?->url !== $url) || ($client?->token !== $token))
        {
            $cache_key = "mcp.$id." . hash64((string)$token);
            $this->client[$id] = new mcp_client($url, $token,
                ["cache_key" => $cache_key] + (array)$this->option);
        };

        return $this->client[$id];
    }

//--------------------------------------------------------------------------------------------------
//   SETUP
//--------------------------------------------------------------------------------------------------

    function probe($id)
    {
        $client = $this->client($id);
        $result = $client->auth_discover();

        //reset
        foreach (["auth_type", "auto", "scope",
                  "endpoint", "auth_method",
                  "refresh_token", "expire",
                  "tool_count", "tool_size",
                  "error"]
                 AS $key)
            $this->data->del($id, $key);

        if ($result === FALSE) return $this->error($id, $client->error);

        //adopt canonical url
        $this->data->set($result["resource"],      $id, "url");
        $this->data->set($result["auth"],          $id, "auth_type");
        $this->data->set($result["scope"] ?? NULL, $id, "scope");

        switch ($result["auth"])
        {
        case "token":

            break; //keep token

        case "oauth":

            //automatic registration
            $metadata = $result["metadata"];
            if (
                 nstre($metadata["registration_endpoint"] ?? "")
                 ||
                 (
                   ($metadata["client_id_metadata_document_supported"] ?? FALSE)
                   &&
                   (strncasecmp(CMS_MODULES_URL, "https://", 8) === 0)
                 )
               )
            {
                $this->data->set(TRUE, $id, "auto");
                $this->data->del($id, "client_id");
                $this->data->del($id, "client_secret");
            }
            else
            {
                $this->data->del($id, "auto");
            };

        default:

            $this->data->del($id, "token"); //remove token
        };

        return $this->measure($id);
    }

//--------------------------------------------------------------------------------------------------

    function measure($id,
                     $list = NULL)
    {
        //optionally retrieve tool list
        if ($list === NULL)
        {
            $token = $this->token($id);
            $list  = ($token === FALSE) ? FALSE : $this->client($id, $token)->tools_list();
        };

        //declaration size
        $size = 0;
        foreach (($list ?: []) AS $tool)
            $size += strlen(json_encode(
                array_intersect_key($tool, ["name" => 0, "description" => 0, "inputSchema" => 0]),
                JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE));

        $this->data->set(($list === FALSE) ? NULL : count($list), $id, "tool_count");
        $this->data->set(($list === FALSE) ? NULL : $size,        $id, "tool_size");

        return $this->data->save();
    }

//--------------------------------------------------------------------------------------------------

    function connect($id)
    {
        $entry  = $this->data->get($id) ?? [];
        $client = $this->client($id);

        $result = $client->auth_discover();
        if (($result["auth"] ?? "") !== "oauth")
            return $this->error($id, ($result === FALSE) ? $client->error : "OAuth not supported.");

        $metadata = $result["metadata"];

        //entered client, else client registered at same authorization server
        $auth = NULL;
        if (isset($entry["client_id"]))
        {
            if (! isset($entry["auto"]))
                $auth = ["client_id"     => $entry["client_id"],
                         "client_secret" => $entry["client_secret"] ?? NULL];

            elseif (streq($entry["endpoint"] ?? "", $metadata["token_endpoint"] ?? ""))
                $auth = ["client_id"     => $entry["client_id"],
                         "client_secret" => $entry["client_secret"] ?? NULL,
                         "auth_method"   => $entry["auth_method"] ?? "none"];
        };

        $redirect = CMS_MODULES_URL . "mcp.php?mcp_message=callback";
        $auth     = $client->auth_register(
            $metadata,
            $redirect,
            CMS_MODULES_URL . "mcp.php?mcp_message=client",
            $auth);
        if ($auth === FALSE) return $this->error($id, $client->error);

        //discovered scope joined with stepped up scope
        $scope = self::scope_merge($result["scope"], $entry["scope"] ?? "");
        $param = mcp_client::auth_url(
            $metadata,
            $auth["client_id"],
            ["redirect_uri" => $redirect,
             "resource"     => $result["resource"],
             "scope"        => $scope]);

        //pending authorization
        cms_cache(
            "agent.oauth." . $param["state"],
            ["name"            => $id,
             "url"             => $result["resource"],
             "issuer"          => $metadata["issuer"],
             "issuer_required" => $metadata["authorization_response_iss_parameter_supported"] ?? FALSE,
             "endpoint"        => $metadata["token_endpoint"] ?? "",
             "auth"            => $auth,
             "verifier"        => $param["verifier"]],
            TRUE,
            600);

        //store client
        $this->data->set($result["resource"],               $id, "url");
        $this->data->set($scope,                            $id, "scope");
        $this->data->set($metadata["token_endpoint"] ?? "", $id, "endpoint");
        $this->data->set($auth["client_id"],                $id, "client_id");
        $this->data->set($auth["client_secret"] ?? NULL,    $id, "client_secret");
        $this->data->set($auth["auth_method"],              $id, "auth_method");
        $this->data->del($id, "error");

        return $this->data->save() ? $param["url"] : FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function callback($param)
    {
        $param = array_filter((array)$param, "is_string"); //string only
        $key   = "agent.oauth." . ($param["state"] ?? "");

        //consume state
        cms_lock("agent.oauth");
        $pending = cms_cache_notouch($key);
        $expire  = cms_cache_time($key) + CMS_CACHE_TTL;
        cms_cache_delete($key);
        cms_unlock("agent.oauth");

        //unknown or expired state
        if (blank($pending) || ($expire <= time())) return "Invalid state.";

        $id = $pending["name"];
        if (nstreq($this->data->get($id, "url"), $pending["url"]))
            return "Server changed."; //error

        //issuer identification error
        if (isset($param["iss"]) ?
            nstreq($param["iss"], $pending["issuer"]) :
            $pending["issuer_required"])
        {
            $error = "Issuer mismatch.";
        }

        //error
        elseif (isset($param["error"]))
        {
            $error = $param["error_description"] ?? $param["error"];
        }

        //exchange authorization code
        else
        {
            $client = $this->client($id);
            $token  = $client->auth_token(
                $pending["endpoint"],
                $pending["auth"],
                ["grant_type"    => "authorization_code",
                 "code"          => $param["code"] ?? "",
                 "redirect_uri"  => CMS_MODULES_URL . "mcp.php?mcp_message=callback",
                 "code_verifier" => $pending["verifier"],
                 "resource"      => $pending["url"]]);
            if ($token) $this->token_save($id, $token);
            else        $error = $client->error;
        };

        $this->data->set($error ??= NULL, $id, "error");
        $this->data->save(NULL, TRUE);

        //tools of new authorization
        if ($error === NULL) $this->measure($id);

        return $error;
    }

//--------------------------------------------------------------------------------------------------
//   TOKEN
//--------------------------------------------------------------------------------------------------

    function token($id,
                   $header = NULL)
    {
        $entry = $this->data->get($id);
        if (blank($entry) || isset($entry["disabled"])) return FALSE;

        //check authorization type
        switch ($entry["auth_type"] ?? "")
        {
        case "none":  return "";
        case "token": return $entry["token"] ?? FALSE;
        case "oauth": break;
        default:      return FALSE;
        };

        $status    = (int)($header["#status"] ?? 0);
        $challenge = parse_http_param($header["www-authenticate"] ?? "");

        //insufficient scope
        if (($status === 403) && streq($challenge["error"] ?? "", "insufficient_scope"))
        {
            //merge scope
            $scope = self::scope_merge($entry["scope"] ?? "", $challenge["scope"] ?? "");
            $this->data->set($scope, $id, "scope");
            $this->data->set("Insufficient scope.", $id, "error");
            $this->data->save(NULL, TRUE);
            return FALSE;
        };

        $token = $entry["token"] ?? "";
        if (stre($token)) return FALSE;

        //not due
        $expire = $entry["expire"] ?? 0;
        if (($status !== 401) && (! $expire || ($expire > time() + 60))) return $token;

        //refresh
        cms_lock($key = "mcp.server.$id");
        $this->load(); //reload
        $entry = $this->data->get($id) ?? [];

        //already refreshed
        if (nstreq($entry["token"] ?? "", $token))
        {
            $token = $entry["token"] ?? FALSE;
        }

        //no refresh token
        elseif (! isset($entry["refresh_token"]))
        {
            $token = FALSE;
            $this->token_save($id, []);
            $this->data->set("Authorization expired.", $id, "error");
        }

        //refresh token
        else
        {
            $client   = $this->client($id);
            $response = $client->auth_token(
                $entry["endpoint"] ?? "",
                ["client_id"     => $entry["client_id"] ?? "",
                 "client_secret" => $entry["client_secret"] ?? NULL,
                 "auth_method"   => $entry["auth_method"] ?? "none"],
                ["grant_type"    => "refresh_token",
                 "refresh_token" => $entry["refresh_token"],
                 "resource"      => $entry["url"]]);

            //refreshed
            if ($response)
            {
                $this->token_save($id, $response + ["refresh_token" => $entry["refresh_token"]]);
                $this->data->del($id, "error");
            }

            //invalid grant removed
            else
            {
                if ($response === FALSE) $this->token_save($id, []);
                $this->data->set($client->error, $id, "error");
            };

            $token = $response["access_token"] ?? FALSE;
        };

        $this->data->save(NULL, TRUE);
        cms_unlock($key);

        return $token;
    }

//--------------------------------------------------------------------------------------------------

    private function token_save($id,
                                $response)
    {
        $expire = is_numeric($response["expires_in"] ?? NULL) ?
                  time() + (int)$response["expires_in"] : NULL;

        $this->data->set($response["access_token"]  ?? NULL, $id, "token");
        $this->data->set($response["refresh_token"] ?? NULL, $id, "refresh_token");
        $this->data->set($expire,                            $id, "expire");
    }

//--------------------------------------------------------------------------------------------------
//   HELPER
//--------------------------------------------------------------------------------------------------

    private function load()
    {
        data::defer_discard(CMS_DATA_PATH . "#system/mcp.server.dat");
        $this->data = new data("#system/mcp.server");
    }

//--------------------------------------------------------------------------------------------------

    private static function scope_merge($a,
                                        $b)
    {
        $array = preg_split("/\s+/", "$a $b", -1, PREG_SPLIT_NO_EMPTY);
        return implode(" ", array_unique($array));
    }

//--------------------------------------------------------------------------------------------------

    private function error($id,
                           $message)
    {
        $this->data->set($message, $id, "error");
        $this->data->save();
        return FALSE;
    }
};