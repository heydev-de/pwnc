<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

if (stre(CMS_LANGUAGE_ENABLED)) return;

$data     = new data("#system/language");
$map      = new map("#system/language.image");
$language = explode(",", CMS_LANGUAGE_ENABLED);
$flag     = defined("CMS_CONTENT_DIRECTORY_INDEX") && (CMS_CONTENT_DIRECTORY_INDEX !== 0);

foreach ($language AS $value)
{
    if (streq($value, CMS_LANGUAGE)) continue; //exclude active language

    while (TRUE)
    {
        //directory entry
        while ($flag)
        {
            $map = new map("#system/$value.directory");
            $url = $map->get_value(CMS_CONTENT_DIRECTORY_INDEX);
            if ($url === NULL) break;
            $url = cms_url(CMS_ROOT_URL . $url);
            break 2;
        };

        //no directory entry
        $url = cms_url(["cms_select_language" => $value]);
        break;
    };

    $name = $data->get($value, "name");

    echo("<a href=\"" . x($url) . "\">");

    if (
         (($image = $map->get_value($value)) !== NULL)
         &&
         is_file(CMS_DATA_PATH . "language/$image")
       )
         echo(image(CMS_DATA_URL . "language/$image", NULL, NULL, $name) . " ");

    echo(x($name) .
         "</a>");
};