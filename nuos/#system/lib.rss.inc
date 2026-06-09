<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   RSS CLASS
//==================================================================================================

//related functions
//--------------------------------------------------------------------------------------------------

function rss_get_default()
{
    $data  = new data("#system/rss");
    $value = NULL;

    $data->move("first");
    while ($key = $data->move("next"))
    {
        if ($data->get($key, "default"))
            $value .= "/$key/";
    };

    return $value;
};

class rss
{

//properties
//--------------------------------------------------------------------------------------------------

    public $data = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct()
    {
        $this->data = new data("#system/rss");
    }

//methods
//--------------------------------------------------------------------------------------------------

    function add_channel($name,
                         $description,
                         $link,
                         $image           = NULL,
                         $category        = NULL,
                         $default         = NULL)
    {
        $this->data->set_buffer([["name"            => $name,
                                  "description"     => $description,
                                  "link"            => $link,
                                  "image"           => $image,
                                  "category"        => $category,
                                  "default"         => (bool)$default]]);

        return $this->data->insert();
    }

//--------------------------------------------------------------------------------------------------

    function set_channel($index,
                         $name,
                         $description,
                         $link,
                         $image           = NULL,
                         $category        = NULL,
                         $default         = NULL)
    {
        $this->data->set($name,            $index, "name");
        $this->data->set($description,     $index, "description");
        $this->data->set($link,            $index, "link");
        $this->data->set($image,           $index, "image");
        $this->data->set($category,        $index, "category");
        $this->data->set((bool)$default,   $index, "default");

        return $index;
    }

//--------------------------------------------------------------------------------------------------

    function del_channel($index)
    {
        return $this->data->del($index);
    }

//--------------------------------------------------------------------------------------------------

    function save()
    {
        return $this->data->save();
    }
};