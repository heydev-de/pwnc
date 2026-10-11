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
$url = mcp::$json["params"]["arguments"]["target_url"] ?? "";
$url = absolute_path(CMS_ROOT_URL, $url);
if ($url === FALSE) mcp::send_error("Invalid `target_url`.");

//post data
$data = ((mcp::$json["params"]["name"] ?? "") === "post") ?
        (mcp::$json["params"]["arguments"]["form_data"] ?? []) :
        NULL;

//send data and retrieve headers
$hfile = http_fopen($url, $data, $header);
if ($hfile === FALSE) mcp::send_error("Connection failed.");

//fetch body data
$buffer = http_fetch_data($hfile);
if ($buffer === FALSE) mcp::send_error("No response.");

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

//output body
echo($buffer);

})();