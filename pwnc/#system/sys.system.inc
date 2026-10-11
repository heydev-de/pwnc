<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   SYSTEM CLASS
//==================================================================================================

class system
{

//properties
//--------------------------------------------------------------------------------------------------

    public $data = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct()
    {
        static $file = NULL;

        if ($file === NULL)
            $file = is_file(CMS_DATA_PATH . "#system/system.dev.dat") ?
                    "#system/system.dev" : "#system/system";

        $this->data = new data($file);
    }

//methods
//--------------------------------------------------------------------------------------------------

    function getval($property,
                    $option = "value")
    {
        return $this->data->get($property, $option);
    }

//--------------------------------------------------------------------------------------------------

    function setval($value    = NULL,
                    $property = NULL,
                    $option   = "value")
    {
        $this->data->set($value, $property, $option);
    }

//--------------------------------------------------------------------------------------------------

    function save()
    {
        return $this->data->save();
    }
};