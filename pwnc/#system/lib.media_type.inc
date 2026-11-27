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

    $array[t("automatic")] = NULL;

    $data->move("first");
    while ($key = $data->move("next"))
    {
        $name = $data->get($key, "name") ?? "";
        $array[$name] = $key;
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
        $this->type     = cms_cache("media_type.type");
        if ($this->type === NULL) $this->update_type();

        mkpath(CMS_DATA_PATH . "#media.type");
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
        $path = CMS_DATA_PATH . "#media.type/" . encode_filename("$index.htm");
        if (! write_file($path, $code ?? "")) return FALSE;

        //write entry
        $this->data->set(
            ["name" => $name,
             "type" => $type],
            $index);

        //save
        if ($this->save()) return TRUE;
        unlink($path);
        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function get_index($type)
    {
        $type = utf8_strtolower($type);
        return $this->type[$type] ?? FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function get_code($index)
    {
        return read_file(CMS_DATA_PATH . "#media.type/" . encode_filename("$index.htm"));
    }

//--------------------------------------------------------------------------------------------------

    function delete($index)
    {
        if (! $this->operator) return FALSE;

        if (! is_array($index)) $index = [$index];

        //delete entries
        foreach ($index AS $value) $this->data->del($value);
        if (! $this->save()) return FALSE;

        //delete code files
        foreach ($index AS $value)
        {
            $path = CMS_DATA_PATH . "#media.type/" . encode_filename("$value.htm");
            if (is_file($path)) unlink($path);
        };

        return TRUE;
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
        if (stre($code = l($this->get_code($index))))
        {
            //fallback type identification
            $_type = file_extension($url);

            if (
                 stre($index = $this->get_index($_type))
                 ||
                 stre($code  = l($this->get_code($index)))
               )
                $code = "<object[ id=\"%id%\"] " .
                                "data=\"%src%\" " .
                                "type=\"%type%\"" .
                              "[ width=\"%width%\"]" .
                              "[ height=\"%height%\"]" .
                              "[ standby=\"" . sprintf(t("resource_loading"), "%title%") . "\"]" .
                              "[ class=\"%class%\"]" .
                              "[ style=\"%style%\"]>" .
                        sprintf(t("err_resource_not_displayable"), "%src%", "%alt%") .
                        "</object>";
        };

        //type
        $type ??= get_mime_type($url);

        //replacement list
        $array = ["id"     => $id,
                  "src"    => $url,
                  "type"   => $type,
                  "width"  => $width,
                  "height" => $height,
                  "alt"    => $alt,
                  "title"  => $title,
                  "class"  => $class,
                  "style"  => $style];

        return replace_placeholder($code, $array);
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
            if (stre($type)) continue;

            foreach (explode(",", $type) AS $value)
            {
                $value = utf8_trim($value);
                $value = utf8_strtolower($value);
                if (isset($this->type[$value])) continue;
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