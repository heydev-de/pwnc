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

    public $url           = NULL; //endpoint
    public $token         = NULL; //bearer token
    public $info          = [];   //client identification
    public $capability    = [];   //client capabilities
    public $error         = NULL; //last failure
    public $timeout_total = 30;   //seconds
    public $timeout_chunk = 30;   //seconds

    static $id    = 0;            //request counter
    const VERSION = "2026-07-28"; //mcp version

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($url,
                         $token,
                         $option = NULL)
    {
        $this->url   = $url;
        $this->token = $token;
        $this->info  = ["name"    => CMS_SOFTWARE,
                        "version" => CMS_VERSION];

        if (isset($option["timeout_total"])) $this->timeout_total = $option["timeout_total"];
        if (isset($option["timeout_chunk"])) $this->timeout_chunk = $option["timeout_chunk"];
        if (isset($option["capability"]))    $this->capability    = $option["capability"];
    }

//methods
//--------------------------------------------------------------------------------------------------

    private function send_request($method,
                                  $param  = NULL,
                                  $option = NULL)
    {
        $this->error = NULL;
        $param       = (array)$param;

        //load library
        if (! cms_load("http")) { $this->error = "Internal error."; return FALSE; };

        //per request protocol metadata
        $param["_meta"] = (array)($option["meta"] ?? []) + ($param["_meta"] ?? []) +
            ["io.modelcontextprotocol/protocolVersion"    => self::VERSION,
             "io.modelcontextprotocol/clientInfo"         => $this->info,
             "io.modelcontextprotocol/clientCapabilities" => (object)$this->capability];

        //value mirrored into Mcp-Name, by method
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

        $body = json_encode(
            ["jsonrpc" => "2.0",
             "id"      => ++self::$id,
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
        if ($reply === FALSE) { $this->error = "No response."; return FALSE; };

        $json = $this->get_response($reply, $header["content-type"] ?? "");
        if (! is_array($json)) { $this->error = "Invalid response."; return FALSE; };

        //protocol error
        if (isset($json["error"]))
        {
            $this->error = (string)($json["error"]["message"] ?? "Error") .
                           " (" . (int)($json["error"]["code"] ?? 0) . ")";
            return FALSE;
        };

        //get result
        $result = $json["result"] ?? NULL;
        if (! is_array($result)) { $this->error = "No result."; return FALSE; };

        //absent result type means complete
        switch ($result["resultType"] ?? "complete")
        {
        case "complete":

            break;

        case "input_required":

            $this->error = "Additional input required.";
            return FALSE;

        default:

            $this->error = "Unsupported result type.";
            return FALSE;
        };

        return $result;
    }

//--------------------------------------------------------------------------------------------------

    private function get_response($body,
                                  $type)
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

            //response must have id
            if (isset($json["id"])) return $json;
        };

        //unterminated final event
        if (blank($array)) return NULL;

        $json = json_decode(implode("\n", $array), TRUE);

        return isset($json["id"]) ? $json : NULL;
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
                                       &$seen)
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
                if (! preg_match("/^[!#$%&'*+-.^_`|~[:alnum:]]+$/", (string)$name)) return FALSE;

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
        $result = $this->send_request("tools/list", NULL, $option);
        if ($result === FALSE) return FALSE;

        //collect tool definitions
        $array = [];
        foreach ($result["tools"] ?? [] AS $tool)
        {
            if (! isset($tool["name"])) continue;

            //invalid annotation invalidates definition
            $map    = $seen = [];
            $header = self::map_header((array)($tool["inputSchema"] ?? []), "", $map, $seen) ? $map : FALSE;
            if ($header === FALSE) continue;

            $tool["schema"] = $tool["inputSchema"] ?? ["type" => "object"];
            $tool["header"] = $header;
            $array[]        = $tool;
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
//   HELPER
//--------------------------------------------------------------------------------------------------

    static function to_text($result)
    {
        $array = [];
        foreach ($result["content"] ?? [] AS $block)
        {
            switch ($block["type"] ?? "")
            {
            case "text":

                $array[] = (string)($block["text"] ?? "");
                break;

            case "resource":

                $resource = $block["resource"] ?? [];
                $array[]  = isset($resource["text"]) ?
                            (string)$resource["text"] :
                            ($resource["uri"] ?? "") . " (" .
                            ($resource["mimeType"] ?? "application/octet-stream") . ", binary)";
                break;

            case "resource_link":

                $array[] = (string)($block["uri"] ?? "");
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
};