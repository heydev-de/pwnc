<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   DESKTOP CLASS
//==================================================================================================

define("CMS_DESKTOP_TYPE_NONE",          0);
define("CMS_DESKTOP_TYPE_LINK",          1);
define("CMS_DESKTOP_TYPE_NOTE",          2);
define("CMS_DESKTOP_TYPE_APPOINTMENT",   4);
define("CMS_DESKTOP_TYPE_ADDRESS",       8);
define("CMS_DESKTOP_TYPE_CONTAINER",    16);
define("CMS_DESKTOP_TYPE_MAILBOX",      32);
define("CMS_DESKTOP_TYPE_FILE",         64);
define("CMS_DESKTOP_TYPE_ALL",         255);

class desktop
{

//properties
//--------------------------------------------------------------------------------------------------

    public $user = NULL;
    public $data = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($user = NULL)
    {
        if ($user === NULL) $user = CMS_SUPERUSER;
        $this->user = $user;

        $this->data = new data("#desktop/" . encode_filename($user) . "/desktop");

        $flag = 0;

        $this->data->move("first");

        while (
                ($flag !== 3)
                &&
                ($key = $this->data->move("next"))
              )
        {
            switch ($this->data->get($key, "#subtype"))
            {
            case "mailbox":     $flag |= 1; break;
            case "appointment": $flag |= 2; break;
            case "address":     $flag |= 4; break;
            };
        };

        if (! flag($flag, 1))
        {
            $this->data->set_buffer(
                [["name"        => t("in_box"),
                  "#type"       => "container",
                  "#subtype"    => "mailbox",
                  "quickaccess" => TRUE],
                 ["#type"       => "/container"]]);
            $this->data->insert();
        };

        if (! flag($flag, 2))
        {
            $this->data->set_buffer(
                [["name"        => t("appointments"),
                  "#type"       => "container",
                  "#subtype"    => "appointment",
                  "quickaccess" => TRUE],
                 ["#type"       => "/container"]]);
            $this->data->insert();
        };

        if (! flag($flag, 4))
        {
            $this->data->set_buffer(
                [["name"        => t("contacts"),
                  "#type"       => "container",
                  "#subtype"    => "address",
                  "quickaccess" => TRUE],
                 ["#type"       => "/container"]]);
            $this->data->insert();
        };

        if ($flag !== 7) $this->data->save();
    }

//methods
//--------------------------------------------------------------------------------------------------

    function object_get($index,
                        $property)
    {
        return $this->data->get($index, $property);
    }

//--------------------------------------------------------------------------------------------------

    function object_set($index,
                        $property,
                        $value = NULL)
    {
        return $this->data->get($index) ?
               $this->data->set($value, $index, $property) :
               FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function save()
    {
        return $this->data->save();
    }

//--------------------------------------------------------------------------------------------------

    function create_object($index,
                           $type,
                           $name)
    {
        $color = strtocolor($name, 25, FALSE);

        if (streq($type, "container"))
            $this->data->set_buffer(
                [["name"  => $name,
                  "color" => $color,
                  "#type" => "container"],
                 ["#type" => "/container"]]);
        else
            $this->data->set_buffer(
                [["name"  => $name,
                  "color" => $color,
                  "#type" => $type]]);

        if (
             ($return = $this->data->insert($index))
             &&
             $this->data->save()
           )
            return $return;

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function move_object($source,
                         $target)
    {
        if (
             $this->data->cut($source)
             &&
             (($return = $this->data->insert($target)) !== FALSE)
             &&
             $this->data->save()
           )
            return $return;

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function delete_object($index)
    {
        //collect file paths
        if ($array = $this->get_children($index, ["mailbox", "file"]))
        {
            foreach ($array AS $key => $value)
                $array[$key] = streq($value, "file") ?
                               $this->file_path($key) :
                               $this->object_path($key);
        };

        if (! $this->data->del($index)) return FALSE;
        if (! $this->data->save())      return FALSE;

        //cleanup
        if ($array && cms_load("filemanager"))
        {
            foreach ($array AS $value) filemanager_delete($value);
        };

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function object_type($index)
    {
        return $this->data->get($index, "#type");
    }

//--------------------------------------------------------------------------------------------------

    function object_path($index)
    {
        return CMS_DATA_PATH . "#desktop/" .
               encode_filename($this->user) . "/" .
               encode_filename($index) . "/";
    }

//--------------------------------------------------------------------------------------------------

    function file_path($index)
    {
        $extension = $this->object_get($index, "extension");
        return $this->object_path("file") .
               encode_filename($index . (stre($extension) ? "" : ".$extension"));
    }

//--------------------------------------------------------------------------------------------------

    function get_parent($index)
    {
        return $this->data->move("parent", $index);
    }

//--------------------------------------------------------------------------------------------------

    function get_children($index,
                         $filter = NULL)
    {
        if (($key = $this->data->move("to", $index)) === NULL) return NULL;

        $result = [];
        $depth  = 0;

        do
        {
            $type = $this->data->get($key, "#type");

            switch ($type)
            {
            case "container":  $depth++; break;
            case "/container": $depth--; break;
            };

            if ($filter === NULL || in_array($type, $filter)) $result[$key] = $type;
        }
        while (($depth > 0) && ($key = $this->data->move("next")));

        return $result;
    }
};