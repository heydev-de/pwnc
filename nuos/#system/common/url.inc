<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

function translate_url($address,
                       $param          = NULL,
                       $language       = CMS_LANGUAGE,
                       $omit_cms_param = FALSE)
{
    /*
      expands internally used identifiers to full url:

      directory://{index}(?{query})(#{fragment})
      content://{index}(?{query})(#{fragment})
      image://{index}
      media://{index}
      download://{index}
    */

    if (preg_match("/^(javascript|mailto):/i", $address)) return $address;

    //complete address
    $address = absolute_path(CMS_ACTIVE_URL, $address);

    //break down address
    $_address = analyze_url($address);
    if ($_address === FALSE) return $address;

    //convert to valid cms_param parameter value
    $omit_cms_param = $omit_cms_param ?: NULL;

    switch ($_address["scheme"])
    {
    case "http":
    case "https":
    case "directory":
    case "content":
//..................................................................................................

        //break down querystring
        if (nstre($_address["query"])) parse_str($_address["query"], $_param);
        else                           $_param = [];

        //extend query parameters
        $param = is_array($param) ? array_replace_recursive($_param, $param) : $_param;
    };

    $flag = FALSE;

    switch ($_address["scheme"])
    {
    case "http":
    case "https":
//..................................................................................................

        $external = strcasecmp($_address["host"], CMS_DOMAIN) !== 0;
        $query    = $external ? querystring($param) : cms_param($param, $omit_cms_param);
        $_address["query"] = substr($query, 1);
        return cms_build_url($_address);

    case "directory":
//..................................................................................................

        $flag = TRUE;

    case "content":
//..................................................................................................

        $_language = nstre($language) ? "$language." : "";

        if ($flag)
        {
            $map       = new map("#system/" . $_language . "directory");
            $index     = $_address["host"];
            $__address = CMS_ROOT_URL . $map->get_value($index);
        }
        else
        {
            $map       = new map("#system/" . $_language . "directory.content");
            $index     = $_address["host"];
            $__address = $map->get_value($index);
        };

        if (
             ($__address !== NULL)
             &&
             (($__address = analyze_url($__address)) !== FALSE)
           )
        {
            $__address["fragment"] = $_address["fragment"] ?? $__address["fragment"];
            $__address["query"]    = substr(cms_param($param, $omit_cms_param), 1);
            return cms_build_url($__address);
        };

        if ($flag)
        {
            $param["content_directory_index"] = $index;
            $param["content_display"]         = "directory";
        }
        else
        {
            $param["content_index"] = $index;
        };

        $param["cms_language"] = $language;
        $fragment              = ($_address["fragment"] !== NULL) ?
                                 "#" . $_address["fragment"] : "";

        return CMS_MODULES_URL . "content.php" .
               cms_param($param, $omit_cms_param) .
               $fragment;

    case "image":
//..................................................................................................

        if (! cms_load("image")) break;

        $image = new image();
        if ($image->data->get($address) === NULL) return image_process("no_image");

        //internal linkage
        $value = $image->data->get($address, "filename");
        if ($value === NULL)
        {
            //external linkage
            $value = $image->data->get($address, "url");
            if ($value !== NULL) return $value;

            //fallback
            $value = $_address["host"];
        };

        return CMS_DATA_URL . "image/" . r($value);

    case "media":
//..................................................................................................

        if (! cms_load("media")) break;

        $media = new media();
        if ($media->data->get($address) === NULL) break;

        //internal linkage
        $value = $media->data->get($address, "filename");
        if ($value === NULL)
        {
            //external linkage
            $value = $media->data->get($address, "url");
            if ($value !== NULL) return $value;

            //fallback
            $value = $_address["host"];
        };

        return CMS_DATA_URL . "media/" . r($value);

    case "download":
//..................................................................................................

        if (! cms_load("download")) break;

        $download = new download();
        if ($download->data->get($address) === NULL)
        {
            //compatibility fallback
            if ($download->data->get($_address["host"]) === NULL) break;
            $address = $_address["host"];
        };

        return CMS_MODULES_URL . "download.php" .
               cms_param(["download_index" => $address,
                          "download_start" => 1],
                         TRUE);
    };

    return $address;
};

//--------------------------------------------------------------------------------------------------

function analyze_url($address)
{
    if (($array = parse_url($address)) === FALSE) return FALSE;

    if (stre($array["path"] ?? ""))
    {
        $path = [];
    }
    else
    {
        $path = pathinfo($array["path"]);
        $path["dirname"] = str_replace("\\", "/", $path["dirname"]);
        if ($path["dirname"] === ".") $path["dirname"] = "";
    };

    return array_merge(
        ["url"      => $address,
         "scheme"   => NULL, "user"     => NULL, "pass"     => NULL, "host"      => NULL,
         "port"     => NULL, "path"     => NULL, "query"    => NULL, "fragment"  => NULL,
         "relative" => blank($array["scheme"]),
         "dirname"  => NULL, "basename" => NULL, "filename" => NULL, "extension" => NULL],
        $array,
        $path);
};

//--------------------------------------------------------------------------------------------------

function absolute_path($source,
                       $target)
{
    if (($_target = analyze_url($target)) === FALSE) return FALSE;
    if ($_target["relative"] === FALSE)              return $target;
    if (($_source = analyze_url($source)) === FALSE) return FALSE;
    if ($_source["relative"] !== FALSE)              return FALSE;

    $_source["query"]    = $_target["query"];
    $_source["fragment"] = $_target["fragment"];

    if (substr($_target["path"], 0, 1) === "/")
    {
        $_source["path"] = $_target["path"];
        return cms_build_url($_source);
    };

    if (nstre($_target["path"]))
    {
        if (substr($_source["path"], 0, 1) === "/")  $_source["path"] = substr($_source["path"], 1);
        if (substr($_source["path"], -1, 1) !== "/") $_source["path"] = str_replace("\\", "/", dirname($_source["path"]));
        else                                         $_source["path"] = substr($_source["path"], 0, -1);
        if ($_source["path"] === ".")                $_source["path"] = "";

        $source_path = explode("/", $_source["path"]);
        $target_path = explode("/", $_target["path"]);

        foreach ($target_path AS $key => $value)
        {
            if ($value === "..")    array_pop($source_path);
            elseif ($value !== ".") array_push($source_path, $value);
        };

        $_source["path"] = implode("/", $source_path);
    };

    return cms_build_url($_source);
};

//--------------------------------------------------------------------------------------------------

function querystring($param,
                     $alter = NULL)
{
    if (! is_array($param)) return FALSE;
    if (is_array($alter))   $param = array_replace_recursive($param, $alter);

    return cms_param($param, TRUE, TRUE);
};

//--------------------------------------------------------------------------------------------------

function u($address = NULL,
           $param   = NULL)
{
    if (is_array($address))
    {
        $param   = $address;
        $address = CMS_ACTIVE_URL;
    }
    else
    {
        if (! is_array($param))    $param   = [];
        if (! is_string($address)) $address = CMS_ACTIVE_URL;
    };

    return $address . cms_param($param, TRUE, TRUE);
};

//--------------------------------------------------------------------------------------------------

function qu($address = NULL,
            $param   = NULL)
{
    return q(u($address, $param));
}

//--------------------------------------------------------------------------------------------------

function import_querystring($address)
{
    $address = parse_url($address);
    if (! isset($address["query"])) return FALSE;

    parse_str($address["query"], $param);

    foreach ($param AS $key => $value)
    {
        $value         = cms_utf8_normalize($value);
        $_GET[$key]    = $value;
        $GLOBALS[$key] = $value;
    };

    return TRUE;
};