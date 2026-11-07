<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   MODULE
//==================================================================================================

if (\defined(__NAMESPACE__ . "\\CMS_MCP")) return; //prevent self-integration

(function() {

//helper function
$resolve = function($target)
{
    if ($target === "") return FALSE; //no target

    $url = parse_url($target);

    //check host
    if (
         isset($url["host"])
         &&
         (strcasecmp($url["host"], $_SERVER["HTTP_HOST"] ?? "") !== 0)
       )
        return FALSE; //invalid

    //check scheme
    if (isset($url["scheme"]))
    {
        $scheme = preg_match("/^(?:|off)$/i", $_SERVER["HTTPS"] ?? "") ? "http" : "https";
        if (strcasecmp($url["scheme"], $scheme) !== 0) return FALSE; //invalid
    };

    //check path
    $path = ($url["path"] ?? "");
    if ($path === "") return FALSE; //invalid

    //resolve root
    $root = substr(__FILE__, 0, -strlen($_SERVER["SCRIPT_NAME"]));
    if (($root = realpath($root)) === FALSE) return FALSE;
    $root = str_replace("\\", "/", $root) . "/";

    //resolve path
    $path = $root . ltrim($path, "/\\");
    if (is_dir($path)) $path = rtrim($path, "/\\") . "/index.php"; //directory index
    if (($path = realpath($path)) === FALSE) return FALSE; //invalid

    //must be executable script
    if (substr($path, -4) !== ".php") return FALSE;

    //check containment
    $path = str_replace("\\", "/", $path);
    if (strpos($path, $root) !== 0) return FALSE; //outside root

    return $path;
};

//..................................................................................................

//preload mcp class
require_once(dirname(__DIR__) . "/#system/sys.mcp.inc");

//retrieve and parse json
mcp::$json = json_decode((string)file_get_contents("php://input"), TRUE);

//oauth message
$message = $_REQUEST["mcp_message"] ?? $_REQUEST["response_type"] ?? $_REQUEST["grant_type"] ?? "";

//determine oauth message by url
if ($message === "")
{
    $php_self = $_SERVER["PHP_SELF"] ?? "";

    foreach (["/.well-known/oauth-protected-resource" => "metadata",
              "/.well-known/openid-configuration"     => "oauth"]
             AS $key => $value)
    {
        if (substr($php_self, -strlen($key)) === $key)
        {
            $message = $value;
            break;
        };
    };
};

if (

//no oauth message
     ($message !== "")
     ||

//no mcp headers
     (
       (mcp_http_header("mcp-protocol-version") === NULL)
       &&
       (mcp_http_header("mcp-method") === NULL)
     )
   )
{
    require("../pwnc.inc");

    //authorization required
    if ($message === "")
    {
        http_response_code(401); //unauthorized
        header("WWW-Authenticate: Bearer resource_metadata=" .
               "\"" . CMS_MODULES_URL . "mcp.php?mcp_message=metadata\"");
        exit();
    };

    //helper functions
    $response = function($value,
                         $http_code = 400)
    {
        while (ob_get_level()) ob_end_clean();
        http_response_code($http_code);
        header_remove();
        header("Cache-Control: no-store, private");
        header("Content-Type: application/json; charset=utf-8");
        echo(json_encode(is_array($value) ? $value : ["error" => $value]));
        exit();
    };

    $verify_resource = function($value)
    {
        if (stre($value)) return TRUE;
        $url = parse_url($value);
        if (($url === FALSE) || isset($url["fragment"])) return FALSE;
        return cms_build_url($url) === CMS_ACTIVE_URL;
    };

    $request = function($key,
                        $default = "")
    {
        return is_string($value = $_REQUEST[$key] ?? $default) ? $value : "";
    };

    $error = NULL;

    //input data
    $client_id    = $request("client_id");
    $redirect_uri = $request("redirect_uri");
    $state        = $request("state");

    switch ($message)
    {
    case "metadata":
//..................................................................................................

        $response(
            ["type"                     => "oauth-resource-metadata",
             "resource"                 => CMS_ACTIVE_URL,
             "authorization_servers"    => [CMS_ACTIVE_URL],
             "scopes_supported"         => ["mcp"],
             "bearer_methods_supported" => ["header"]],
            200);

    case "oauth":
//..................................................................................................

        $response(
            ["issuer"                                => CMS_ACTIVE_URL,
             "authorization_endpoint"                => CMS_ACTIVE_URL,
             "token_endpoint"                        => CMS_ACTIVE_URL,
             "registration_endpoint"                 => CMS_ACTIVE_URL . "?mcp_message=register",
             "scopes_supported"                      => ["mcp"],
             "response_types_supported"              => ["code"],
             "grant_types_supported"                 => ["authorization_code"],
             "code_challenge_methods_supported"      => ["S256", "plain"],
             "token_endpoint_auth_methods_supported" => ["none"],
             "protected_resources"                   => [CMS_ACTIVE_URL]],
            200);

    case "register":
//..................................................................................................

        $data = json_decode((string)file_get_contents("php://input"), TRUE);

        $response(
            ["client_id"                  => bin2hex(random_bytes(32)),
             "client_id_issued_at"        => time(),
             "redirect_uris"              => $data["redirect_uris"] ?? [],
             "token_endpoint_auth_method" => "none",
             "grant_types"                => ["authorization_code"],
             "response_types"             => ["code"]],
            201);

    case "client":
//..................................................................................................

        $response(
            ["client_id"                  => CMS_ACTIVE_URL . "?mcp_message=client",
             "redirect_uris"              => [CMS_ACTIVE_URL . "?mcp_message=callback"],
             "token_endpoint_auth_method" => "none",
             "grant_types"                => ["authorization_code", "refresh_token"],
             "response_types"             => ["code"],
             "client_name"                => CMS_SOFTWARE . " (" . CMS_DOMAIN . ")"],
            200);

    case "callback":
//..................................................................................................

        $error = cms_load("mcp_registry") ? (new mcp_registry())->callback($_GET) : "Internal error.";

        echo(CMS_DOCTYPE_HTML .
             "<html>" .
             "<head>" .
             CMS_HTML_HEADER . CMS_STYLESHEET .
             "</head>" .

             "<body class=\"" . x(CMS_CLASS) . "\">" .
             "<div>" .
             "<h1>" .
             t("authorization") .
             "</h1>" .

             (nstre($error) ?
             "<div class=\"response-error\">" .
             x($error) .
             "</div>" :
             "<div class=\"response-success\">" .
             t("connection_established") .
             "</div>") .

             "</div>" .
             "</body>" .
             "</html>");
        exit();

    case "cancel":
//..................................................................................................

        header("Location: " .
                cms_url($redirect_uri,
                        ["error" => "access_denied",
                         "state" => $state]),
                TRUE, 303);
        exit();

    case "confirm":
    case "code":
//..................................................................................................

        //input data
        $code_challenge        = $request("code_challenge");
        $code_challenge_method = $request("code_challenge_method", "plain");

        //verify parameters
        if (
             stre($client_id)
             ||
             (($url = parse_url($redirect_uri)) === FALSE)
             ||
             stre($url["scheme"] ?? "")
             ||
             stre($url["host"] ?? "")
             ||
             stre($code_challenge)
             ||
             ! in_array($code_challenge_method, ["S256", "plain"], TRUE)
           )
            $error = t("the_request_is_invalid");

        switch (($error === NULL) ? $message : "")
        {
        case "code":

            if (! $verify_resource($request("resource"))) $error = t("the_request_is_invalid");
            break;

        case "confirm":

            $token = $request("token");

            //verify token
            if ((new data("#system/permission.token"))->get(hash64($token), "user") === NULL)
            {
                $error = t("err_api_key_invalid");
                break;
            };

            //generate token retrieval code
            $code = bin2hex(random_bytes(32));

            //store data
            cms_cache(
                "mcp.oauth2.$code",
                ["token"                 => encrypt($token, $code),
                 "client_id"             => $client_id,
                 "redirect_uri"          => $redirect_uri,
                 "code_challenge"        => $code_challenge,
                 "code_challenge_method" => $code_challenge_method],
                TRUE,
                60);

            header("Location: " .
                   cms_url($redirect_uri,
                           ["code"  => $code,
                            "state" => $state]),
                   TRUE, 303);
            exit();
        };

//..................................................................................................

        //api key input form
        echo(CMS_DOCTYPE_HTML .
             "<html>" .
             "<head>" .
             CMS_HTML_HEADER . CMS_STYLESHEET .
             "</head>" .

             "<body class=\"" . x(CMS_CLASS) . "\">" .
             "<div>" .
             "<h1>" .
             t("authorization") .
             "</h1>");

        //display error
        if ($error !== NULL)
        {
            echo("<div class=\"response-error\">" .
                 x($error) .
                 "</div>");

            if ($message === "code")
            {
                echo("</div>" .
                     "</body>" .
                     "</html>");
                exit();
            };
        };

        echo("<form method=\"post\" " .
                   "action=\"" . x(cms_url()) . "\">" .

             "<input name=\"client_id\" " .
                    "type=\"hidden\" " .
                    "value=\"" . x($client_id) . "\">" .
             "<input name=\"redirect_uri\" " .
                    "type=\"hidden\" " .
                    "value=\"" . x($redirect_uri) . "\">" .
             "<input name=\"state\" " .
                    "type=\"hidden\" " .
                    "value=\"" . x($state) . "\">" .
             "<input name=\"code_challenge\" ".
                    "type=\"hidden\" " .
                    "value=\"" . x($code_challenge) . "\">" .
             "<input name=\"code_challenge_method\" " .
                    "type=\"hidden\" " .
                    "value=\"" . x($code_challenge_method) . "\">" .

             "<div class=\"p\">" .
             t("hint_api_key") .
             "</div>" .

             "<label>" .
             t("api_key") . "<br>" .
             "<input id=\"mcp-token\" " .
                    "name=\"token\" " .
                    "type=\"text\">" .
             "</label><br>" .

             "<button name=\"mcp_message\" " .
                     "type=\"submit\" " .
                     "value=\"cancel\">" .
             t("command_cancel") .
             "</button>" .

             "<button name=\"mcp_message\" " .
                     "type=\"submit\" " .
                     "value=\"confirm\">" .
             t("authorize") .
             "</button>" .

             "</form>" .
             "</div>" .
             "</body>" .
             "</html>");
        exit();

    case "authorization_code":
//..................................................................................................

        //input data
        $code          = $request("code");
        $code_verifier = $request("code_verifier");

        //retrieve stored data
        $data = cms_cache_notouch("mcp.oauth2.$code");
        if (blank($data)) $response("invalid_grant");
        cms_cache_delete("mcp.oauth2.$code");

        //verify client id and redirect uri
        if (
             nstreq($client_id, $data["client_id"])
             ||
             nstreq($redirect_uri, $data["redirect_uri"])
           )
            $response("invalid_grant");

        //verify resource indicator
        if (! $verify_resource($request("resource")))
            $response("invalid_target");

        //pkce validation
        if (stre($code_challenge = $data["code_challenge"] ?? ""))
            $response("invalid_grant");

        if (streq($data["code_challenge_method"], "S256"))
            $code_verifier = base64url(hash("sha256", $code_verifier, TRUE));

        if (nstreq($code_challenge, $code_verifier))
            $response("invalid_grant");

        $permission = new permission();
        $token      = decrypt($data["token"], $code);
        $user       = $permission->get_user($token);
        if ($user === NULL) $response("invalid_grant");
        $expire     = (int)$permission->data->get("user.$user", "token_expire") - time();

        //return access token
        $response(
            ["access_token" => $token,
             "token_type"   => "Bearer",
             "scope"        => "mcp"]
            + (($expire > 0) ? ["expires_in" => $expire] : []),
            200);
    };

    http_response_code(400);
    exit();
};

define(__NAMESPACE__ . "\\CMS_MCP", TRUE);

//get target
$target = mcp::$json["params"]["arguments"]["url"] ?? NULL;
if (! is_string($target)) $target = dirname($_SERVER["SCRIPT_NAME"] ?? "") . "/desktop.php"; //default

//resolve path
$script = NULL;
if (($path = $resolve($target)) === FALSE)
{
    //use http proxy for external or static resource
    $path   = __DIR__ . "/http.php";
    $script = dirname($_SERVER["SCRIPT_NAME"] ?? "") . "/http.php";
};

//rewrite request context
$url   = parse_url($target);
$_path = "/" . ltrim($url["path"] ?? "", "/");
$query = $url["query"] ?? "";

$_SERVER["SCRIPT_NAME"]     = $script ?? $_path;
$_SERVER["SCRIPT_FILENAME"] = $path;
$_SERVER["QUERY_STRING"]    = $query;
$_SERVER["REQUEST_URI"]     = $_path . (($query !== "") ? "?$query" : "");

$_GET     = []; if ($query !== "") parse_str($query, $_GET);
$_REQUEST = $_GET;

//hand over target
define(__NAMESPACE__ . "\\CMS_MCP_TARGET", $path);

})();

//load target in global scope
chdir(dirname(CMS_MCP_TARGET));
require(CMS_MCP_TARGET);