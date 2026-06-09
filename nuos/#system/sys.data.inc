<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   DATA CLASS
//==================================================================================================

//related functions
//--------------------------------------------------------------------------------------------------

function data_sort(&$data,
                   $property,
                   $key = NULL)
{
    //build hierarchical tree
    $array = [];
    $stack = [0];
    $count = 0;

    $data->_move("first");
    while (
            ($_key = $data->_move("next"))
            &&
            ($count >= 0)
          )
    {
        switch ($data->_get($_key, "#type"))
        {
        case "container":
//..................................................................................................

            $array[$_key]["#!"]           = l($data->_get($_key, $property));
            $array[$stack[$count]][$_key] = &$array[$_key];
            $stack[++$count]              = $_key;
            break;

        default:
//..................................................................................................

            $array[$_key]["#!"]           = l($data->_get($_key, $property));
            $array[$stack[$count]][$_key] = &$array[$_key];
            break;

        case "/container":
//..................................................................................................

            $array[$stack[$count]]["#/"]  = $_key;
            unset($stack[$count]);
            $count--;
        };
    };

    $array[0]["#/"] = TRUE;

    //sort hierarchical tree
    $stack = [];
    $key   = $data->remove_prefix($key);
    $_key  = stre($key) ? 0 : $key;
    $count = [$_key => 0];

    reset($array[$_key]);
    while (TRUE)
    {
        while (($__key = key($array[$_key])) !== NULL)
        {
            next($array[$_key]);

            if ($__key === "#!") continue;
            if ($__key === "#/")
            {
                if ($count[$_key]) uasort($array[$_key], __NAMESPACE__ . "\\_data_sort");
                break;
            };

            $count[$_key]++;

            if (isset($array[$__key]["#/"]))
            {
                array_push($stack, $_key);
                $_key = $__key;
                $count[$_key] = 0;
                reset($array[$_key]);
            };
        };

        unset($count[$_key]);
        $_key = array_pop($stack);
        if ($_key === NULL) break;
    };

    unset($array[0]["#/"]);

    //construct data based on sorted order
    $stack = [];
    $_key  = 0;
    $_data = NULL;

    reset($array[0]);
    while (TRUE)
    {
        while (($__key = key($array[$_key])) !== NULL)
        {
            next($array[$_key]);

            if ($__key === "#!") continue;
            if ($__key === "#/") continue;

            $_data[$__key] = $data->_get($__key);

            if (isset($array[$__key]["#/"]))
            {
                array_push($stack, $_key);
                $_key = $__key;
                reset($array[$_key]);
            };
        };

        if (isset($array[$_key]["#/"]))
        {
            $__key         = $array[$_key]["#/"];
            $_data[$__key] = $data->_get($__key);
        };

        $_key = array_pop($stack);
        if ($_key === NULL) break;
    };

    $data->data = $_data;
};

//--------------------------------------------------------------------------------------------------

function _data_sort($value1, $value2)
{
    if (! isset($value1["#!"])) return 0;
    if (! isset($value2["#!"])) return 0;
    return utf8_strnatcasecmp($value1["#!"], $value2["#!"]);
};

class data
{

//properties
//--------------------------------------------------------------------------------------------------

    public $file          = NULL;
    public $data          = NULL;
    public $buffer        = NULL;
    public $password      = NULL;
    public $prefix        = NULL;
    public $prefix_length = 0;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($name     = NULL,
                         $password = NULL,
                         $prefix   = NULL)
    {
        if (nstre($name))
        {
            $this->open($name, $password, $prefix);
            return;
        };

        $this->set_password($password);
        $this->set_prefix($prefix);
    }

//methods
//--------------------------------------------------------------------------------------------------

    function open($name,
                  $password = NULL,
                  $prefix   = NULL)
    {
        $this->file = streq(substr($name, -4), ".dat") ?
                      $name : CMS_DATA_PATH . "$name.dat";
        $this->set_password($password);
        $this->set_prefix($prefix);

        //retrieve from temporary cache
        if ($this->data = cms_cache("data." . $this->file)) return TRUE;

        //retrieve from file
        $this->data = NULL;

        if (
             is_file($this->file)
             &&
             (($hfile = fopen($this->file, "rb")) !== FALSE)
           )
        {
            flock($hfile, LOCK_SH);

            $time = time();
            $i    = 0;

            while (! feof($hfile))
            {
                //retrieve dataset
                $value = fgets($hfile);

                //index
                $offset = strpos($value, ":=");
                if ($offset === FALSE) continue;
                $key    = decchr(substr($value, 0, $offset));
                $key    = $this->remove_prefix($key);
                if (stre($key))        continue;

                //data
                $value = substr($value, $offset + 2);
                $value = rtrim($value, "\n\r");
                $value = explode(";", $value);

                foreach ($value AS $_value)
                {
                    $offset = strpos($_value, "=");
                    $_key   = decchr(substr($_value, 0, $offset));
                    $_value = decchr(substr($_value, $offset + 1));
                    if ($this->password !== NULL)
                        $_value = decrypt($_value, $this->password);

                    $this->data[$key][$_key] = $_value;
                };

                //type
                $type = isset($this->data[$key]["#type"]) ?
                    $this->data[$key]["#type"] : "";

                //automatic expiration
                if ($i > 0)
                {
                    unset($this->data[$key]);

                    switch ($type)
                    {
                    case "container":  $i++; break;
                    case "/container": $i--; break;
                    };

                    continue;
                };

                if (
                     isset($this->data[$key]["#expire"])
                     &&
                     ($this->data[$key]["#expire"] < $time)
                   )
                {
                    unset($this->data[$key]);

                    //erase user data
                    if (
                         streq($this->file, CMS_DATA_PATH . "#system/permission.dat")
                         &&
                         permission_is_user($key)
                       )
                        permission_delete(substr($key, 5));

                    if (streq($type, "container")) $i = 1;
                };
            };

            fclose($hfile);
        };

        //temporary caching
        cms_cache("data." . $this->file, $this->data);

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function save($name = NULL)
    {
        //set file name
        if (nstre($name))
            $this->file = streq(substr($name, -4), ".dat") ?
                          $name : CMS_DATA_PATH . "$name.dat";

        //path
        $path = dirname($this->file);
        if (! mkpath($path)) return FALSE;

        //file
        $hfile = fopen($this->file, "wb");
        if ($hfile === FALSE) return FALSE;

        flock($hfile, LOCK_EX);

        //process data
        if (is_array($this->data))
        {
            foreach ($this->data AS $key => $value)
            {
                if (stre($key))         continue; //no key
                if (! is_array($value)) continue; //no values

                //key
                $buffer = encchr($key) . ":=";

                //values
                foreach ($value AS $_key => $_value)
                {
                    if (stre($_value)) continue; //empty value

                    //encryption
                    if ($this->password !== NULL)
                        $_value = encrypt((string)$_value, $this->password);

                    //property / value pair
                    $buffer .= encchr($_key) . "=" . encchr($_value) . ";";
                };

                fwrite($hfile, substr($buffer, 0, -1) . "\n");
            };
        };

        fclose($hfile);

        //temporary caching
        cms_cache("data." . $this->file, $this->data);

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function set($value    = NULL,
                 $key      = NULL,
                 $property = NULL)
    {
        $key = $this->remove_prefix($key); //remove prefix
        return $this->_set($value, $key, $property);
    }

//--------------------------------------------------------------------------------------------------

    function _set($value    = NULL,
                  $key      = NULL,
                  $property = NULL)
    {
        if ((! is_array($value)) && stre($value))
        {
            $this->_del($key, $property);
            return;
        };

        if (stre($key) && stre($property)) $this->data                  = (array)$value;
        elseif (stre($property))           $this->data[$key]            = (array)$value;
        else                               $this->data[$key][$property] = (string)$value;
    }

//--------------------------------------------------------------------------------------------------

    function get($key      = NULL,
                 $property = NULL)
    {
        $key = $this->remove_prefix($key); //remove prefix
        return $this->_get($key, $property);
    }

//--------------------------------------------------------------------------------------------------

    function _get($key      = NULL,
                  $property = NULL)
    {
        if (stre($key) && stre($property))                   return $this->data;
        elseif (stre($property) && isset($this->data[$key])) return $this->data[$key];
        elseif (isset($this->data[$key][$property]))         return $this->data[$key][$property];
        return NULL;
    }

//--------------------------------------------------------------------------------------------------

    function has($key      = NULL,
                 $property = NULL)
    {
        $key = $this->remove_prefix($key); //remove prefix
        return $this->_has($key, $property);
    }

//--------------------------------------------------------------------------------------------------

    function _has($key      = NULL,
                  $property = NULL)
    {
        if (stre($key) && stre($property))                   return $this->data !== NULL;
        elseif (stre($property) && isset($this->data[$key])) return (bool)count($this->data[$key]);
        elseif (isset($this->data[$key][$property]))         return nstre($this->data[$key][$property]);
        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function del($key       = NULL,
                 $property  = NULL,
                 $recursive = TRUE)
    {
        $key = $this->remove_prefix($key); //remove prefix
        return $this->_del($key, $property, $recursive);
    }

//--------------------------------------------------------------------------------------------------

    function _del($key       = NULL,
                  $property  = NULL,
                  $recursive = TRUE)
    {
        if ($this->data === NULL) return FALSE;

        if (stre($property))
        {
            if (stre($key))
            {
                $this->data = [];
            }
            elseif (isset($this->data[$key]))
            {
                if ($recursive) return $this->_copy($key, "del");

                $this->_move("to", $key);
                unset($this->data[$key]);

                $depth = 1;
                while (
                        ($depth > 0)
                        &&
                        (($key = $this->_move("next")) !== NULL)
                      )
                {
                    if (isset($this->data[$key]["#type"]))
                    {
                        switch ($this->data[$key]["#type"])
                        {
                        case "container":  $depth++; break;
                        case "/container": $depth--; break;
                        };
                    };
                };

                unset($this->data[$key]);
            };
        }
        elseif (isset($this->data[$key][$property]))
        {
            unset($this->data[$key][$property]);
        };

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function set_buffer($array)
    {
        $this->buffer = NULL;

        if (! is_array($array)) return FALSE;

        foreach ($array AS $value)
            $this->buffer[unique_id()] = $value;

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function cut($key)
    {
        $key = $this->remove_prefix($key); //remove prefix
        return $this->_cut($key);
    }

//--------------------------------------------------------------------------------------------------

    function _cut($key)
    {
        return $this->_copy($key, "cut");
    }

//--------------------------------------------------------------------------------------------------

    function insert($key = NULL)
    {
        $key = $this->remove_prefix($key); //remove prefix
        return $this->apply_prefix($this->_insert($key));
    }

//--------------------------------------------------------------------------------------------------

    function _insert($key = NULL)
    {
        if ($this->buffer === NULL) return FALSE;

        if (stre($key)) $key = 0;
        $buffer = [];
        $_key   = 0;
        $return = NULL;

        $this->_move("first");
        do
        {
            $buffer[$_key] = $this->_get($_key);
            if (streq($_key, $key))
            {
                foreach ($this->buffer AS $_key => $_value)
                {
                    $buffer[$_key] = $_value;
                    $return ??= $_key;
                };
            };
        }
        while (($_key = $this->_move("next")) !== NULL);

        unset($buffer[0]);
        $this->data = $buffer;

        return $return ?? TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function append($key = NULL)
    {
        $key = $this->remove_prefix($key); //remove prefix
        return $this->apply_prefix($this->_append($key));
    }

//--------------------------------------------------------------------------------------------------

    function _append($key = NULL)
    {
        return $this->_copy($key, "append");
    }

//--------------------------------------------------------------------------------------------------

    function copy($key,
                  $action = NULL)
    {
        $key = $this->remove_prefix($key); //remove prefix
        return $this->apply_prefix($this->_copy($key, $action));
    }

//--------------------------------------------------------------------------------------------------

    function _copy($key,
                   $action = NULL)
    {
        if (streq($action, "append"))
        {
            if ($this->buffer === NULL) return FALSE;
            elseif (stre($key))         return $this->_insert(0);
        };

        if (! preg_match("/^(append|del)$/", $action)) $this->buffer = NULL;
        if (! isset($this->data[$key]))                return FALSE;

        $depth = 0;
        $key   = $this->_move("to", $key);

        do
        {
            if (isset($this->data[$key]["#type"]))
            {
                switch ($this->data[$key]["#type"])
                {
                case "container":  $depth++; break;
                case "/container": $depth--; break;
                };
            };

            switch ($action)
            {
            default:
//..................................................................................................

                $this->buffer[unique_id()] = $this->data[$key];
                break;

            case "cut":
//..................................................................................................

                $this->buffer[$key] = $this->data[$key];

            case "del":
//..................................................................................................

                unset($this->data[$key]);

            case "append":
//..................................................................................................

            };
        }
        while (($depth > 0) && ($key = $this->_move("next")));

        if (streq($action, "append")) return $this->_insert($key);
        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function seek($condition)
    {
        $condition = array_map([$this, "remove_prefix"], $condition); //remove prefix
        return $this->apply_prefix($this->_seek($condition));
    }

//--------------------------------------------------------------------------------------------------

    function _seek($condition)
    {
        $this->_move("first");
        while (($key = $this->_move("next")) !== NULL)
        {
            foreach ($condition AS $_key => $value)
            {
                if (nstreq($this->_get($key, $_key), $value))
                {
                    $key = NULL;
                    break;
                };
            };

            if ($key !== NULL) return $key;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function move($target = "current", //"current", "first", "last", "prev", "next", "to", "parent"
                  $key    = NULL)
    {
        $key = $this->remove_prefix($key); //remove prefix
        return $this->apply_prefix($this->_move($target, $key));
    }

//--------------------------------------------------------------------------------------------------

    function _move($target = "current",
                   $key    = NULL)
    {
        if ($this->data === NULL) return NULL;

        switch ($target)
        {
        default:
        case "current":
//..................................................................................................

            $_key = key($this->data);
            break;

        case "first":
//..................................................................................................

            reset($this->data);
            $_key = key($this->data);
            break;

        case "last":
//..................................................................................................

            end($this->data);
            $_key = key($this->data);
            break;

        case "prev":
//..................................................................................................

            $_key = key($this->data);
            prev($this->data);
            break;

        case "next":
//..................................................................................................

            $_key = key($this->data);
            next($this->data);
            break;

        case "to":
//..................................................................................................

            reset($this->data);
            while (($_key = key($this->data)) !== NULL)
            {
                next($this->data);
                if (streq($key, $_key)) break;
            };
            break;

        case "parent":
//..................................................................................................

            $array = [];

            reset($this->data);
            while (($_key = key($this->data)) !== NULL)
            {
                next($this->data);
                if (streq($key, $_key)) break;

                switch ($this->_get($_key, "#type"))
                {
                case "container":

                    array_push($array, $_key);
                    break;

                case "/container":

                    array_pop($array);
                };
            };

            $_key = end($array);
        };

        return $_key;
    }

//--------------------------------------------------------------------------------------------------

    function is_container($key)
    {
        $key = $this->remove_prefix($key); //remove prefix
        return $this->_is_container($key);
    }

//--------------------------------------------------------------------------------------------------

    function _is_container($key)
    {
        if (stre($key)) return TRUE;
        return streq($this->_get($key, "#type"), "container");
    }

//--------------------------------------------------------------------------------------------------

    function is_child($key,
                      $parent)
    {
        //remove prefix
        $key    = $this->remove_prefix($key);
        $parent = $this->remove_prefix($parent);

        return $this->_is_child($key, $parent);
    }

//--------------------------------------------------------------------------------------------------

    function _is_child($key,
                       $parent)
    {
        if (stre($key))                                         return FALSE;
        if (streq($key, $parent))                               return TRUE;
        if (nstreq($this->_get($parent, "#type"), "container")) return FALSE;

        $depth = 0;

        foreach ($this->data AS $_key => $value)
        {
            if (streq($_key, $key)) return (bool)$depth;

            if (streq($_key, $parent) || ($depth > 0))
            {
                switch ($this->_get($_key, "#type"))
                {
                case "container":

                    $depth++;
                    break;

                case "/container":

                    if (--$depth === 0) return FALSE;
                    break;
                };
            };
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function has_children($key)
    {
        $key = $this->remove_prefix($key); //remove prefix
        return $this->_has_children($key);
    }

//--------------------------------------------------------------------------------------------------

    function _has_children($key)
    {
        if (stre($key))                                      return FALSE;
        if (nstreq($this->_get($key, "#type"), "container")) return FALSE;

        $this->_move("to", $key);
        $_key = $this->_move("next");
        if ($_key === NULL) return FALSE;
        $type = $this->_get($_key, "#type");

        return nstreq($type, "/container");
    }

//--------------------------------------------------------------------------------------------------

    function set_password($value = NULL)
    {
        $this->password = nstre($value) ? hash64($value) : NULL;
    }

//--------------------------------------------------------------------------------------------------

    function set_prefix($value = NULL)
    {
        static $prefix = [CMS_DATA_PATH . "#system/image.dat"    => "image://",
                          CMS_DATA_PATH . "#system/media.dat"    => "media://",
                          CMS_DATA_PATH . "#system/download.dat" => "download://"];

        if (nstre($value))                   $this->prefix = (string)$value;
        elseif (isset($prefix[$this->file])) $this->prefix = $prefix[$this->file];
        $this->prefix_length = strlen((string)$this->prefix);
    }

//--------------------------------------------------------------------------------------------------

    function apply_prefix($key)
    {
        return is_string($key) ? $this->prefix . $key : $key;
    }

//--------------------------------------------------------------------------------------------------

    function remove_prefix($key)
    {
        if ($this->prefix_length === 0) return $key; //no prefix
        return (strncmp($key, $this->prefix, $this->prefix_length) === 0) ?
               substr($key, $this->prefix_length) : $key;
    }
};