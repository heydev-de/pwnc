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
cms_load("content",   TRUE);
cms_load("directory", TRUE);
cms_load("rss",       TRUE);

//send rss content type header
header("Content-Type: application/rss+xml; charset=utf-8");

$rss = new rss();

//selected channel
if (blank($rss_channel))             return; //no channel
if (! $rss->data->get($rss_channel)) return; //invalid channel

//number of items to display
$rss_limit = isset($rss_limit) ? max((int)$rss_limit, 1) : NULL;

//sorting order
switch(isset($rss_order) ? $rss_order : NULL)
{
case "modified":

    $order     = CMS_DB_CONTENT_TIME;
    break;

default:

    $rss_order = NULL;

case "published":

    $order     = CMS_DB_CONTENT_PUBLISHER_TIME;
    break;
};

//caching
$cache_key  = "rss.$rss_channel.$rss_limit.$rss_order." . CMS_LANGUAGE;
$cache_time = cms_cache_time($cache_key);
$time       = time();

if (($cache_time !== FALSE) && ($cache_time > ($time - 60))) //60 seconds delay
{
    echo(cms_cache_notouch($cache_key));
    exit();
};

//start output
$buffer  = "<?xml version=\"1.0\" " .
                 "encoding=\"utf-8\"?>" .
           "<rss version=\"2.0\" " .
                "xmlns:atom=\"http://www.w3.org/2005/Atom\">" .
           "<channel>" .
           "<atom:link href=\"" . x(u(["rss_channel" => $rss_channel,
                                       "rss_limit"   => $rss_limit,
                                       "rss_order"   => $rss_order])) . "\" " .
                      "rel=\"self\" " .
                      "type=\"application/rss+xml\"/>";

//title
$title   = l($rss->data->get($rss_channel, "name"));
$buffer .= "<title>" . x($title) . "</title>";

//link
$link    = l($rss->data->get($rss_channel, "link"));
$buffer .= "<link>" . x($link) . "</link>";

//description
$value   = l($rss->data->get($rss_channel, "description"));
$buffer .= "<description>" . x($value) . "</description>";

//language
$buffer .= "<language>" . x(CMS_LANGUAGE) . "</language>";

//publication date
new mysql(); //establish database connection
$result = mysql_query(
    "SELECT " .   CMS_DB_CONTENT_PUBLISHER_TIME . " " .
    "FROM " .     CMS_DB_CONTENT . " " .
    "WHERE " .    CMS_DB_CONTENT_STATUS .
    "='" .        CMS_CONTENT_STATUS_PUBLICATION . "' " .
    "AND " .      CMS_DB_CONTENT_CHANNEL . " " .
    "LIKE '%/" .  sqlesc($rss_channel) . "/%' " .
    "ORDER BY " . CMS_DB_CONTENT_PUBLISHER_TIME . " DESC " .
    "LIMIT 1");
if (mysql_num_rows($result))
{
    $value   = date("r", mysql_result($result, 0));
    $buffer .= "<pubDate>" . x($value) . "</pubDate>";
};

//last build date
$value   = date("r");
$buffer .= "<lastBuildDate>" . x($value) . "</lastBuildDate>";

//category
if ($value = l($rss->data->get($rss_channel, "category")))
    $buffer .= "<category>" . x($value) . "</category>";

//generator
$buffer .= "<generator>" . x(CMS_IDENTIFIER) . "</generator>";

//image
$value   = ($value = l($rss->data->get($rss_channel, "image"))) ?
           translate_url($value) : CMS_IMAGES_URL . "content/rss.svg";
$buffer .= "<image>" .
           "<url>" .   x($value) . "</url>" .
           "<title>" . x($title) . "</title>" .
           "<link>" .  x($link) .  "</link>" .
           "</image>";

//items
$directory = new directory();
$_time     = $time - 259200; //3 days

$result = mysql_query(
    "SELECT " .   CMS_DB_CONTENT_INDEX . ", " .
                  CMS_DB_CONTENT_TIME . ", " .
                  CMS_DB_CONTENT_PUBLISHER_TIME . ", " .
                  CMS_DB_CONTENT_TITLE . ", " .
                  CMS_DB_CONTENT_DESCRIPTION . ", " .
                  CMS_DB_CONTENT_IMAGE . " " .
    "FROM " .     CMS_DB_CONTENT . " " .
    "WHERE " .    CMS_DB_CONTENT_STATUS .
    "='" .        CMS_CONTENT_STATUS_PUBLICATION . "' " .
    "AND " .      CMS_DB_CONTENT_CHANNEL . " " .
    "LIKE '%/" .  sqlesc($rss_channel) . "/%' " .
    "ORDER BY " . $order . " DESC " .
    (($rss_limit > 0) ? " LIMIT " . sqlesc($rss_limit) : ""));

while ($resultrow = mysql_fetch_assoc($result))
{
    $index   = $resultrow[CMS_DB_CONTENT_INDEX];
    $buffer .= "<item>";

    //title
    $value = l($resultrow[CMS_DB_CONTENT_TITLE]);

    if ($resultrow[CMS_DB_CONTENT_PUBLISHER_TIME] >= $resultrow[CMS_DB_CONTENT_TIME])
    {
        $__time  = $resultrow[CMS_DB_CONTENT_PUBLISHER_TIME];
        $_buffer = CMS_L_MOD_RSS_001;
    }
    else
    {
        $__time  = $resultrow[CMS_DB_CONTENT_TIME];
        $_buffer = CMS_L_MOD_RSS_002;
    };

    if ($__time > $_time)
        $value .= " ($_buffer " . friendly_date($__time) . ")";

    $buffer .= "<title>" . x($value) . "</title>";

    //link
    $buffer .= "<link>" .
               x(translate_url("content://$index",
                               NULL,
                               CMS_LANGUAGE,
                               TRUE)) .
               "</link>";

    //description
    $value   = parse_text(l($resultrow[CMS_DB_CONTENT_DESCRIPTION]));
    $buffer .= "<description>" . x($value) . "</description>";

    //category
    $array      = [];
    $stack_flag = [];
    $stack_key  = [];
    $stack_name = [];
    $link       = FALSE;
    $length     = strlen($index) + 10;

    $directory->data->move("first");
    while ($key = $directory->data->move("next"))
    {
        switch ($directory->data->get($key, "#type"))
        {
        case "container":
//..................................................................................................

            $url = $directory->data->get($key, "url");

            if (
                 streq(substr($url, 0, $length), "content://$index")
                 &&
                 (
                   stre($url = substr($url, $length, 1))
                   ||
                   ($url === "?")
                   ||
                   ($url === "#")
                 )
               )
            {
                $_key = end($stack_key);
                if (($_key !== FALSE) && (! isset($array[$_key])))
                    $array[$_key] = implode("/", $stack_name);

                if ($link === FALSE)
                    $link         = translate_url("directory://$key");
            };

            $flag = stre($url) || $directory->data->get($key, "hidden");
            array_push($stack_flag, $flag);
            if (! $flag)
            {
                array_push($stack_key,  $key);
                array_push($stack_name, l($directory->data->get($key, "name")));
            };
            break;

        case "/container":
//..................................................................................................

            if (! array_pop($stack_flag))
            {
                array_pop($stack_key);
                array_pop($stack_name);
            };
            break;
        };
    };

    foreach ($array AS $key => $value)
    {
        $key     = ($key === 0) ? CMS_ROOT_URL : translate_url("directory://$key");
        $buffer .= "<category domain=\"" . x($key) . "\">" .
                   x($value) .
                   "</category>";
    };

    //enclosure
    $value = l($resultrow[CMS_DB_CONTENT_IMAGE]);

    if (nstre($value))
    {
        $url = translate_url($value);
        $url = image_process($url, 500, 500);

        //local
        if (($path = image_path($url)) !== FALSE)
        {
            $length = filesize($path);
            $type   = get_mime_type($path);
        }

        //remote
        elseif ($value = get_headers($url))
        {
            $length = isset($value["Content-Length"]) ? $value["Content-Length"] : FALSE;
            $type   = isset($value["Content-Length"]) ? $value["Content-Type"]   : FALSE;
        };

        if (($length !== FALSE) && ($type !== FALSE))
            $buffer .= "<enclosure url=\"" .    x($url) .    "\" " .
                                  "length=\"" . x($length) . "\" " .
                                  "type=\"" .   x($type) .   "\"/>";
    };

    //guid
    $buffer .= "<guid isPermaLink=\"false\">" . x($index) . "</guid>";

    //publication date
    $value   = date("r", $resultrow[CMS_DB_CONTENT_PUBLISHER_TIME]);
    $buffer .= "<pubDate>" . x($value) . "</pubDate>" .

               "</item>";
};

$buffer .= "</channel>" .
           "</rss>";

//cache permanently
cms_cache($cache_key, $buffer, TRUE);

//output
echo($buffer);