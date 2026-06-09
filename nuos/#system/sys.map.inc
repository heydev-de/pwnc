<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   MAP CLASS
//==================================================================================================

//constants
define("CMS_MAP_INDEX_KEY",   0);
define("CMS_MAP_INDEX_VALUE", 1);
define("CMS_MAP_DATA_KEY",    2);
define("CMS_MAP_DATA_VALUE",  3);

class map
{

//properties
//--------------------------------------------------------------------------------------------------

    public $file = NULL;
    public $data = [CMS_MAP_INDEX_KEY   => [],
                    CMS_MAP_INDEX_VALUE => [],
                    CMS_MAP_DATA_KEY    => [],
                    CMS_MAP_DATA_VALUE  => []];

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($name = NULL)
    {
        if ($name) $this->open($name);
    }

//methods
//--------------------------------------------------------------------------------------------------

    function open($name)
    {
        $this->file = CMS_DATA_PATH . "$name.map";

        //retrieve from temporary cache
        if (($this->data = cms_cache("map." . $this->file)) !== NULL) return TRUE;

        //retrieve from file
        $this->data = [CMS_MAP_INDEX_KEY   => [],
                       CMS_MAP_INDEX_VALUE => [],
                       CMS_MAP_DATA_KEY    => [],
                       CMS_MAP_DATA_VALUE  => []];

        if (
             is_file($this->file)
             &&
             (($hfile = fopen($this->file, "rb")) !== FALSE)
           )
        {
            flock($hfile, LOCK_SH);

            $i = 1;

            while (
                    (! feof($hfile))
                    &&
                    (($key = rtrim(fgets($hfile, 1024))) !== FALSE)
                    &&
                    (($value = rtrim(fgets($hfile, 1024))) !== FALSE)
                  )
            {
                $this->data[CMS_MAP_INDEX_KEY][crc32($key)]     = $i;
                $this->data[CMS_MAP_INDEX_VALUE][crc32($value)] = $i;
                $this->data[CMS_MAP_DATA_KEY][$i]               = $key;
                $this->data[CMS_MAP_DATA_VALUE][$i++]           = $value;
            };

            fclose($hfile);
        };

        //temporary caching
        cms_cache("map." . $this->file, $this->data);

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function save($name = NULL)
    {
        if (nstre($name)) $this->file = CMS_DATA_PATH . "$name.map";

        $path = dirname($this->file);

        if (! mkpath($path))                               return FALSE;
        if (($hfile = fopen($this->file, "wb")) === FALSE) return FALSE;

        flock($hfile, LOCK_EX);

        foreach ($this->data[CMS_MAP_DATA_KEY] AS $key => $value)
        {
            if (stre($value)) continue;
            fwrite($hfile, "$value\n" . $this->data[CMS_MAP_DATA_VALUE][$key] . "\n");
        };

        fclose($hfile);

        //temporary caching
        cms_cache("map." . $this->file, $this->data);

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function set($key,
                 $value)
    {
        $_key   = crc32($key);
        $_value = crc32($value);

        if (isset($this->data[CMS_MAP_INDEX_KEY][$_key]))
        {
            $index   = $this->data[CMS_MAP_INDEX_KEY][$_key];
            $__value = crc32($this->data[CMS_MAP_DATA_VALUE][$index]);
            unset($this->data[CMS_MAP_INDEX_VALUE][$__value]);
        }
        else
        {
            end($this->data[CMS_MAP_DATA_KEY]);
            $index = key($this->data[CMS_MAP_DATA_KEY]) + 1;
        };

        $this->data[CMS_MAP_INDEX_KEY][$_key]     = $index;
        $this->data[CMS_MAP_INDEX_VALUE][$_value] = $index;
        $this->data[CMS_MAP_DATA_KEY][$index]     = $key;
        $this->data[CMS_MAP_DATA_VALUE][$index]   = $value;

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function get_value($key)
    {
        $key = crc32($key);

        return isset($this->data[CMS_MAP_INDEX_KEY][$key]) ?
               $this->data[CMS_MAP_DATA_VALUE][$this->data[CMS_MAP_INDEX_KEY][$key]] :
               NULL;
    }

//--------------------------------------------------------------------------------------------------

    function get_value_list()
    {
        return array_values($this->data[CMS_MAP_DATA_VALUE]);
    }

//--------------------------------------------------------------------------------------------------

    function get_key($value)
    {
        $value = crc32($value);

        return isset($this->data[CMS_MAP_INDEX_VALUE][$value]) ?
               $this->data[CMS_MAP_DATA_KEY][$this->data[CMS_MAP_INDEX_VALUE][$value]] :
               NULL;
    }

//--------------------------------------------------------------------------------------------------

    function del_key($key)
    {
        $key = crc32($key);

        if (isset($this->data[CMS_MAP_INDEX_KEY][$key]))
        {
            $index = $this->data[CMS_MAP_INDEX_KEY][$key];
            $value = crc32($this->data[CMS_MAP_DATA_VALUE][$index]);

            unset($this->data[CMS_MAP_INDEX_KEY][$key]);
            unset($this->data[CMS_MAP_INDEX_VALUE][$value]);
            unset($this->data[CMS_MAP_DATA_KEY][$index]);
            unset($this->data[CMS_MAP_DATA_VALUE][$index]);
        };
    }

//--------------------------------------------------------------------------------------------------

    function del_value($value)
    {
        $value = crc32($value);

        if (isset($this->data[CMS_MAP_INDEX_VALUE][$value]))
        {
            $index = $this->data[CMS_MAP_INDEX_VALUE][$value];
            $key   = crc32($this->data[CMS_MAP_DATA_KEY][$index]);

            unset($this->data[CMS_MAP_INDEX_KEY][$key]);
            unset($this->data[CMS_MAP_INDEX_VALUE][$value]);
            unset($this->data[CMS_MAP_DATA_KEY][$index]);
            unset($this->data[CMS_MAP_DATA_VALUE][$index]);
        };
    }
};