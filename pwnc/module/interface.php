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

//load libraries
cms_load("ifc", TRUE);

function ifc_module_list()
{
    if (($hdir = opendir(CMS_INTERFACE_PATH)) === FALSE) return []; //error

    $array = [];

    while ($file_object = readdir($hdir))
    {
        if (
             is_file(CMS_INTERFACE_PATH . $file_object)
             &&
             preg_match("/^ifc\.(.*)\.inc$/", $file_object, $file_object)
             &&
             cms_permission("interface." . $file_object[1], NULL, NULL)
           )
        {
            $array[$file_object[1]] = ifc_page_title($file_object[1]) ?: $file_object[1];
        };
    };

    closedir($hdir);

    if (! $order = file(CMS_INTERFACE_PATH . "order", FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES)) return $array;

    $return = [];

    foreach ($order AS $value)
    {
        $value = rtrim($value);
        if ($value === "-")
        {
            $return[unique_id()] = "-";
        }
        elseif (isset($array[$value]))
        {
            $return[$value] = $array[$value];
            unset($array[$value]);
        };
    };

    return $return + $array;
};

//get modules
$ifc_page = ifc_module_list();

//execute selected module
if (CMS_IFC_PAGE && isset($ifc_page[CMS_IFC_PAGE]))
{
    cms_set_cookie(["cms_ifc_page" => CMS_IFC_PAGE]);
    if (substr_count(CMS_IFC_OPTION ?? "", "external")) $ifc_page = NULL;
    require(CMS_INTERFACE_PATH . "ifc." . CMS_IFC_PAGE . ".inc");
}

//no module selected
else
{
    //generate and download mcp client bundle
    if (
         (CMS_IFC_MESSAGE === "mcpb")
         &&
         (($file = mcp_bundle()) !== FALSE)
       )
    {
        download($file, "PWNC " . CMS_DOMAIN);
        exit();
    };

    //show default page
    ifc_default($ifc_page);
};