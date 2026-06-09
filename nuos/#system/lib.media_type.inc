<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   MEDIA TYPE CLASS
//==================================================================================================

//permission
define("CMS_MEDIA_TYPE_PERMISSION_OPERATOR", "operator");

//related functions
//--------------------------------------------------------------------------------------------------

function media_type_get_select()
{
    $data = new data("#system/media.type");

    $array[CMS_L_MEDIA_TYPE_001] = NULL;

    $data->move("first");
    while ($key = $data->move("next"))
    {
        $name                       = $data->get($key, "name");
        $array[$name ? $name : " "] = $key;
    };

    ksort($array, SORT_NATURAL | SORT_FLAG_CASE);

    return $array;
};

class media_type
{

//properties
//--------------------------------------------------------------------------------------------------

    public $data     = NULL;
    public $operator = NULL;
    public $type     = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct()
    {
        $this->data     = new data("#system/media.type");
        $this->operator = cms_permission(CMS_MEDIA_TYPE_PERMISSION_OPERATOR);

        mkpath(CMS_DATA_PATH . "#media.type");
        if (! $this->type = cms_cache("media_type.type")) $this->update_type();
    }

//methods
//--------------------------------------------------------------------------------------------------

    function add($name,
                 $type = NULL,
                 $code = NULL)
    {
        if (! $this->operator) return FALSE;

        $index = unique_id();
        return $this->set($index, $name, $type, $code) ? $index : FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function set($index,
                 $name,
                 $type = NULL,
                 $code = NULL)
    {
        if (! $this->operator) return FALSE;

        //write code
        if (! $hfile = fopen(CMS_DATA_PATH . "#media.type/$index.htm", "wb")) return FALSE;
        flock($hfile, LOCK_EX);
        fwrite($hfile, $code);
        fclose($hfile);

        //write entry
        $this->data->set(
            ["name" => $name,
             "type" => $type],
            $index);

        //save
        return $this->save();
    }

//--------------------------------------------------------------------------------------------------

    function get_index($type)
    {
        $type = utf8_strtolower($type);

        if (! isset($this->type[$type])) return FALSE;
        return $this->type[$type];
    }

//--------------------------------------------------------------------------------------------------

    function get_code($index)
    {
        return ($code = read_file(CMS_DATA_PATH . "#media.type/$index.htm")) ? $code : FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function delete($index)
    {
        if (! $this->operator) return FALSE;

        if (! is_array($index)) $index = [$index];
        foreach ($index AS $value)
        {
            if (! $this->data->del($value)) continue;
            if (is_file(CMS_DATA_PATH . "#media.type/$value.htm"))
                unlink(CMS_DATA_PATH . "#media.type/$value.htm");
        };

        return $this->save();
    }

//--------------------------------------------------------------------------------------------------

    function parse($index,
                   $url,
                   $id     = NULL,
                   $type   = NULL,
                   $width  = NULL,
                   $height = NULL,
                   $alt    = NULL,
                   $title  = NULL,
                   $class  = NULL,
                   $style  = NULL)
    {
        //code
        if (! $code = l($this->get_code($index)))
        {
            //fallback type identification
            $_type = file_extension($url);

            if (
                 (! $index = $this->get_index($_type))
                 ||
                 (! $code  = l($this->get_code($index)))
               )
                $code = "<object[ id=\"%id%\"] " .
                                "data=\"%src%\" " .
                                "type=\"%type%\"" .
                              "[ width=\"%width%\"]" .
                              "[ height=\"%height%\"]" .
                              "[ standby=\"" . sprintf(CMS_L_MEDIA_TYPE_002, "%title%") . "\"]" .
                              "[ class=\"%class%\"]" .
                              "[ style=\"%style%\"]>" .
                        sprintf(CMS_L_MEDIA_TYPE_003, "%src%", "%alt%") .
                        "</object>";
        };

        //type
        if (! $type) $type = get_mime_type($url);

        //replace placeholder
        $array = ["id"     => $id,
                  "src"    => $url,
                  "type"   => $type,
                  "width"  => $width,
                  "height" => $height,
                  "alt"    => $alt,
                  "title"  => $title,
                  "class"  => $class,
                  "style"  => $style];

        foreach ($array AS $key => $value)
            $code = nstre($value) ?
                preg_replace("/(\\[([^\\[\\]]*))?%$key%(([^\\[\\]]*)\\])?/",
                             "\${2}$value\\4",
                             $code) :
                preg_replace("/(?:\\[[^\\[\\]]*)?%$key%(?:[^\\[\\]]*\\])?/",
                             "",
                             $code);

        return $code;
    }

//--------------------------------------------------------------------------------------------------

    function update_type()
    {
        $this->type = [];

        data_sort($this->data, "name");

        $this->data->move("first");
        while ($key = $this->data->move("next"))
        {
            $type = $this->data->get($key, "type");
            $type = explode(",", $type);

            foreach ($type AS $value)
            {
                $value = utf8_trim($value);
                $value = utf8_strtolower($value);
                $this->type[$value] = $key;
            };
        };

        cms_cache("media_type.type", $this->type);
    }

//--------------------------------------------------------------------------------------------------

    function save()
    {
        if (! $this->operator) return FALSE;

        $this->update_type();
        return $this->data->save();
    }
};