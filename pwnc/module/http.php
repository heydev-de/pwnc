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

require("../pwnc.inc");

(function() {

if (! CMS_MCP_ACTIVE)   exit(); //mcp only
if (! cms_load("http")) mcp::send_error("Internal error.");

//retrieve target url
$url = mcp::$json["params"]["arguments"]["url"] ?? "";
$url = absolute_path(CMS_ROOT_URL, $url);
if ($url === FALSE) mcp::send_error("Invalid `url`.");

//post data
$data = ((mcp::$json["params"]["name"] ?? "") === "post") ?
        (mcp::$json["params"]["arguments"]["form_data"] ?? []) :
        NULL;

//send data and retrieve headers
$hfile = http_fopen($url, $data, $header, ["limit" => CMS_HTTP_LIMIT + 1]);
if ($hfile === FALSE) mcp::send_error("Connection failed.");

//fetch body data
$buffer = http_fetch_data($hfile, $error);

//no data
if ($buffer === FALSE)
{
    $error = "content unavailable ($error)";
}

//limit exceeded
elseif (strlen($buffer) > CMS_HTTP_LIMIT)
{
    //cut at character boundary
    $buffer = utf8_strcut($buffer, 0, CMS_HTTP_LIMIT);

    //text
    if (utf8_detect($buffer))
        $buffer .= "\n~~~ truncated at limit of " . format_bytesize(CMS_HTTP_LIMIT) . " ~~~";

    //binary
    else
        $error = "content omitted, exceeds limit of " . format_bytesize(CMS_HTTP_LIMIT);
};

//replace content
if ($error !== NULL)
{
    if (isset($header["content-type"])) $header["x-original-content-type"] = $header["content-type"];
    $header["content-type"] = "text/plain; charset=utf-8";
    $buffer = "~~~ $error ~~~";
};

//forward response headers
header_remove();
foreach ($header AS $key => $value)
{
    //process set-cookie header
    if (is_array($value))
    {
        foreach ($value AS $_value) header("$key: $_value", FALSE);
        continue;
    };

    header("$key: $value");
};

//output content
echo($buffer);

})();