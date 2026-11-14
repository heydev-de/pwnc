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
cms_load("content", TRUE);

//send sitemap content type header
header("Content-Type: application/xml; charset=utf-8");

//caching
$cache_key  = "sitemap";
$cache_time = cms_cache_time($cache_key);
$time       = time();

if (($cache_time !== FALSE) && ($cache_time > ($time - 60))) //60 seconds delay
{
    echo(cms_cache_notouch($cache_key));
    exit();
};

//retrieve language specific mapping
$map      = [];
$language = stre(CMS_LANGUAGE_ENABLED) ? [0] : explode(",", CMS_LANGUAGE_ENABLED);

foreach ($language AS $value)
{
    $_value      = ($value !== 0) ? "$value." : "";
    $map[$value] = new map("#system/" . $_value . "directory.content");
};

//start output
$buffer = "<?xml version=\"1.0\" " .
                "encoding=\"utf-8\"?>\n" .
          "<urlset xmlns=\"http://www.sitemaps.org/schemas/sitemap/0.9\" " .
                  "xmlns:xhtml=\"http://www.w3.org/1999/xhtml\">\n";

//retrieve publications
new mysql(); //establish database connection
$result = mysql_query(
    "SELECT " .   CMS_DB_CONTENT_INDEX . ", " .
                  CMS_DB_CONTENT_TIME . " " .
    "FROM " .     CMS_DB_CONTENT . " " .
    "WHERE " .    CMS_DB_CONTENT_STATUS .
    "='" .        CMS_CONTENT_STATUS_PUBLICATION . "' " .
    "AND NOT " .  CMS_DB_CONTENT_FLAG . " " .
    "& " .       (CMS_CONTENT_FLAG_SITEMAP_EXCLUDE | CMS_CONTENT_FLAG_META_ROBOTS_NOINDEX) . " " .
    "ORDER BY " . CMS_DB_CONTENT_TIME . " DESC");

//process publications
while ($resultrow = mysql_fetch_assoc($result))
{
    $index   = $resultrow[CMS_DB_CONTENT_INDEX];
    $date    = x(date("Y-m-d", $resultrow[CMS_DB_CONTENT_TIME]));
    $_buffer = "";

    foreach ($language AS $value)
    {
        $url = $map[$value]->get_value($index);
        if ($url === NULL) continue; //no canonical url in directory

        //primary url
        $buffer .= "<url>\n" .
                   "<loc>" . x($url) . "</loc>\n" .
                   "<lastmod>$date</lastmod>\n";

        if ($value !== 0) //multilingual
        {
            if ($_buffer === "")
            {
                foreach ($language AS $_value)
                {
                    //alternate url
                    $url = $map[$_value]->get_value($index);
                    if ($url === NULL) continue; //no canonical url in directory

                    $url      = x($url);
                    $_buffer .= "<xhtml:link rel=\"alternate\" " .
                                            "href=\"$url\" " .
                                            "hreflang=\"" . x($_value) . "\"/>\n";

                    //fallback url
                    if (streq($_value, CMS_LANGUAGE_DEFAULT) && nstreq($_value, "x-default"))
                        $_buffer .= "<xhtml:link rel=\"alternate\" " .
                                                "href=\"$url\" " .
                                                "hreflang=\"x-default\"/>\n";
                };
            };

            $buffer .= $_buffer;
        };

        $buffer .= "</url>\n";
    };
};

$buffer .= "</urlset>";

//cache permanently
cms_cache($cache_key, $buffer, TRUE);

//output
echo($buffer);