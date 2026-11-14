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

class map
{

//properties
//--------------------------------------------------------------------------------------------------

    public $file  = NULL;
    public $key   = [];
    public $value = [];

    //deferred save versioning
    private static $version = [];
    private $_version       = [];

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($name = NULL)
    {
        if ($name) $this->open($name);
    }

//destructor
//--------------------------------------------------------------------------------------------------

    function __destruct()
    {
        foreach ($this->_version AS $name => $version)
        {
            if ((self::$version[$name] ?? NULL) === $version)
                $this->save($name, TRUE);
        };
    }

//methods
//--------------------------------------------------------------------------------------------------

    function open($name)
    {
        $this->file = CMS_DATA_PATH . "$name.map";

        //retrieve from temporary cache
        if (($cache = cms_cache("map.v2." . $this->file)) !== NULL)
        {
            [$this->key, $this->value] = $cache;
            return TRUE;
        };

        //retrieve from file
        $this->key   = [];
        $this->value = [];

        if (
             is_file($this->file)
             &&
             (($hfile = fopen($this->file, "rb")) !== FALSE)
           )
        {
            flock($hfile, LOCK_SH);
            stream_set_read_buffer($hfile, 65536);

            while (! feof($hfile))
            {
                if (($key   = fgets($hfile)) === FALSE) break;
                if (($value = fgets($hfile)) === FALSE) break;

                if (stre($key = rtrim($key, "\r\n"))) continue;
                $value = rtrim($value, "\r\n");

                $this->key[$key]     = $value;
                $this->value[$value] = $key;
            };

            fclose($hfile);
        };

        //temporary caching
        cms_cache("map.v2." . $this->file, [$this->key, $this->value]);

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function save($name  = NULL,
                  $force = FALSE)
    {
        if (nstre($name)) $this->file = CMS_DATA_PATH . "$name.map";
        else              $name       = substr($this->file, strlen(CMS_DATA_PATH), -4);

        $path = dirname($this->file);
        if (! mkpath($path)) return FALSE;

        //memorize for saving
        if (! $force)
        {
            if ((is_file($this->file) && is_writable($this->file)) || is_writable($path))
            {
                //increase global version and store locally
                self::$version[$name]  ??= 0;
                $this->_version[$name]   = ++self::$version[$name];

                //temporary caching
                cms_cache("map.v2." . $this->file, [$this->key, $this->value]);
                return TRUE;
            };

            return FALSE;
        };

        unset($this->_version[$name]); //remove deferral

        //write to temporary file
        $temp = "$path/" . unique_id() . ".tmp";
        if (($hfile = fopen($temp, "wb")) === FALSE) return FALSE;

        flock($hfile, LOCK_EX);
        stream_set_write_buffer($hfile, 65536);

        foreach ($this->key AS $key => $value)
        {
            if (stre($key)) continue;
            fwrite($hfile, "$key\n$value\n");
        };

        fclose($hfile);

        //atomic replace
        if (! rename($temp, $this->file))
        {
            unlink($temp);
            return FALSE;
        };

        //temporary caching
        cms_cache("map.v2." . $this->file, [$this->key, $this->value]);

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function set($key,
                 $value)
    {
        $key   = (string)$key;
        $value = (string)$value;

        if (isset($this->key[$key]))
        {
            $_value = $this->key[$key];
            if (($this->value[$_value] ?? NULL) === $key)
                unset($this->value[$_value]);
        };

        $this->key[$key]     = $value;
        $this->value[$value] = $key;

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function get_value($key)
    {
        return $this->key[$key] ?? NULL;
    }

//--------------------------------------------------------------------------------------------------

    function get_value_list()
    {
        return array_values($this->key);
    }

//--------------------------------------------------------------------------------------------------

    function get_key($value)
    {
        return $this->value[$value] ?? NULL;
    }

//--------------------------------------------------------------------------------------------------

    function del_key($key)
    {
        $key = (string)$key;

        if (isset($this->key[$key]))
        {
            $value = $this->key[$key];

            unset($this->key[$key]);
            if (($this->value[$value] ?? NULL) === $key)
                unset($this->value[$value]);
        };
    }

//--------------------------------------------------------------------------------------------------

    function del_value($value)
    {
        $value = (string)$value;

        if (isset($this->value[$value]))
        {
            $key = $this->value[$value];

            unset($this->value[$value]);
            unset($this->key[$key]);
        };
    }
};