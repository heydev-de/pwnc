<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//should never happen if software is installed correctly
if (! cms_load("directory"))                  return;

//do not display on empty selection
if (! defined("CMS_CONTENT_DIRECTORY_INDEX")) return;
if (! CMS_CONTENT_DIRECTORY_INDEX)            return;

//get display object containing the directory tree structure
$flexview = directory_get_flexview(FALSE);

//set currently selected directory entry
$index = CMS_CONTENT_DIRECTORY_INDEX;
$flexview->set_index($index);

//set base entry of directory branch to be displayed
//all elements outside that branch are ignored
$flexview->set_base(0);

//structured data
$list = ["@context"        => "https://schema.org",
         "@type"           => "BreadcrumbList",
         "itemListElement" => []];
$pos  = 1;

//retrieve home page
foreach ($flexview->object[0] AS $key => $nothing)
{
    //skip indexes not representing an entry
    if (preg_match("/^#(?:data|parent)$/", $key)) continue;

    //skip empty entries
    if (! isset($flexview->object[$key]["#data"]["url"])) continue;

    //add to structured data
    $list["itemListElement"][] =
        ["@type"    => "ListItem",
         "position" => $pos++,
         "name"     => $flexview->get_name($key),
         "item"     => $flexview->get_action($key)];
    break;
};

//get list of elements connecting base entry with selected entry (inclusive)
$path = $flexview->get_path();

foreach ($path AS $value)
{
    //skip home page
    if ($value === $key) continue;

    //skip empty entries
    if (! isset($flexview->object[$value]["#data"]["url"])) continue;

    //add to structured data
    $list["itemListElement"][] =
        ["@type"    => "ListItem",
         "position" => $pos++,
         "name"     => $flexview->get_name($value),
         "item"     => $flexview->get_action($value)];
};

//output structured data
echo("<script type=\"application/ld+json\">" .
     json_encode($list) .
     "</script>");