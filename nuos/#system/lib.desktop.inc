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
        if ($user == NULL) $user = CMS_SUPERUSER;
        $this->user = $user;

        $this->data = new data("#desktop/" . safe_filename($user) . "/desktop");

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
                [["name"        => CMS_L_DESKTOP_001,
                  "#type"       => "container",
                  "#subtype"    => "mailbox",
                  "quickaccess" => TRUE],
                 ["#type"       => "/container"]]);
            $this->data->insert();
        };

        if (! flag($flag, 2))
        {
            $this->data->set_buffer(
                [["name"        => CMS_L_DESKTOP_002,
                  "#type"       => "container",
                  "#subtype"    => "appointment",
                  "quickaccess" => TRUE],
                 ["#type"       => "/container"]]);
            $this->data->insert();
        };

        if (! flag($flag, 4))
        {
            $this->data->set_buffer(
                [["name"        => CMS_L_DESKTOP_003,
                  "#type"       => "container",
                  "#subtype"    => "address",
                  "quickaccess" => TRUE],
                 ["#type"       => "/container"]]);
            $this->data->insert();
        };

        if ($flag != 7) $this->data->save();
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
        $type = $this->data->get($index, "#type");

        if (! $this->data->del($index)) return FALSE;
        if (! $this->data->save())      return FALSE;

        //cleanup
        switch ($type)
        {
        case "mailbox":

            if (cms_load("filemanager"))
            {
                $path = CMS_DATA_PATH . "#desktop/" . safe_filename($this->user) . "/$index/";
                filemanager_delete($path);
            };
            break;

        default:

            break;
        };

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function object_type($index)
    {
        return $this->data->get($index, "#type");
    }

//--------------------------------------------------------------------------------------------------

    function get_parent($index)
    {
        return $this->data->move("parent", $index);
    }
};