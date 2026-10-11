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

    public $data               = [];    //data
    public $buffer             = NULL;  //insert buffer
    private $path              = NULL;  //path
    private $password          = NULL;  //encryption password
    private $prefix            = NULL;  //key prefix
    private $prefix_length     = 0;     //length of key prefix
    private $data_orig         = NULL;  //original data
    private $path_orig         = NULL;  //original path
    private static $cache      = [];    //caching
    private static $file_state = [];    //file content
    private static $defer      = [];    //deferred save registry
    private static $shutdown   = FALSE; //shutdown execution flag

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

//destructor
//--------------------------------------------------------------------------------------------------

    function __destruct()
    {
        if (
             ($this->path !== NULL)
             &&
             isset(self::$defer[$this->path][spl_object_id($this)])
           )
            $this->save(NULL, TRUE);
    }

//methods
//--------------------------------------------------------------------------------------------------

    function open($name,
                  $password = NULL,
                  $prefix   = NULL)
    {
        $this->path = streq(substr($name, -4), ".dat") ?
                      $name : CMS_DATA_PATH . "$name.dat";
        $this->set_password($password);
        $this->set_prefix($prefix);

        //use cache
        if ($this->data = self::$cache[$this->path] ?? NULL)
        {
            $this->snapshot();
            return TRUE;
        };

        //retrieve from file
        $this->data = NULL;

        if (
             is_file($this->path)
             &&
             (($hfile = fopen($this->path, "rb")) !== FALSE)
           )
        {
            stream_set_read_buffer($hfile, 65536);
            $this->data = $this->parse($hfile, $hash);
            fclose($hfile);

            self::$file_state[$this->path] = ["data" => $this->data, "hash" => $hash];
        };

        //caching
        self::$cache[$this->path] = $this->data;

        $this->snapshot();

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    private function parse($hfile,
                           &$hash = NULL)
    {
        $data    = NULL;
        $context = hash_init("xxh3");
        $time    = time();
        $i       = 0;

        while (! feof($hfile))
        {
            //retrieve dataset
            $value = fgets($hfile);
            if ($value === FALSE) break;

            hash_update($context, $value);

            //index
            $offset = strpos($value, ":=");
            if ($offset === FALSE) continue;
            $key    = decchr(substr($value, 0, $offset));
            $key    = $this->remove_prefix($key);
            if (stre($key))        continue;

            //data
            $value = substr($value, $offset + 2);
            $value = rtrim($value, "\r\n");
            $value = explode(";", $value);

            foreach ($value AS $_value)
            {
                $offset = strpos($_value, "=");
                $_key   = decchr(substr($_value, 0, $offset));
                $_value = decchr(substr($_value, $offset + 1));
                if ($this->password !== NULL)
                    $_value = decrypt($_value, $this->password);

                $data[$key][$_key] = $_value;
            };

            //type
            $type = isset($data[$key]["#type"]) ?
                $data[$key]["#type"] : "";

            //automatic expiry
            if ($i > 0)
            {
                unset($data[$key]);

                switch ($type)
                {
                case "container":  $i++; break;
                case "/container": $i--; break;
                };

                continue;
            };

            if (
                 isset($data[$key]["#expire"])
                 &&
                 ($data[$key]["#expire"] < $time)
               )
            {
                unset($data[$key]);
                if (streq($type, "container")) $i = 1;
            };
        };

        $hash = hash_final($context);

        return $data;
    }

//--------------------------------------------------------------------------------------------------

    private function snapshot()
    {
        $this->data_orig = $this->data;
        $this->path_orig = $this->path;
    }

//--------------------------------------------------------------------------------------------------

    function save($name      = NULL,
                  $immediate = FALSE)
    {
        //set file name
        $path = stre($name) ? $this->path :
                (streq(substr($name, -4), ".dat") ? $name : CMS_DATA_PATH . "$name.dat");

        //different save target -> remove deferral
        if (($this->path !== NULL) && ($path !== $this->path)) $this->defer_remove();

        $this->path = $path;

        //folder
        $dir = dirname($path);
        if (! mkpath($dir)) return FALSE;

        //same file
        $merge = $path === $this->path_orig;

        //no change
        if ($merge && ($this->data === $this->data_orig)) return TRUE;

        //memorize for deferred saving
        if (! $immediate)
        {
            if ((is_file($path) && is_writable($path)) || is_writable($dir))
            {
                //register deferral
                self::$defer[$path][spl_object_id($this)] = $this;
                self::$cache[$path]                       = $this->data;

                //write at shutdown
                if (! self::$shutdown)
                {
                    self::$shutdown = TRUE;
                    register_shutdown_function([self::class, "defer_apply"]);
                };

                return TRUE;
            };

            return FALSE;
        };

        //remove deferral
        $this->defer_remove();

        //establish lock
        if (! cms_lock($path)) return FALSE;

        try
        {
            $data = $this->data;
            $file = NULL;
            $hash = hash("xxh3", "");

            if ($merge)
            {
                //read file
                $state = self::$file_state[$path] ?? NULL;

                if (is_file($path) && (($hfile = fopen($path, "rb")) !== FALSE))
                {
                    $context = hash_init("xxh3");
                    hash_update_stream($context, $hfile);
                    $hash = hash_final($context);

                    if (($state["hash"] ?? NULL) === $hash)
                    {
                        $file = $state["data"];
                    }
                    else
                    {
                        rewind($hfile);
                        $file = $this->parse($hfile, $hash);
                    };

                    fclose($hfile);
                };

                //merge if changed
                if ($file !== $this->data_orig)
                    $data = self::merge($this->data_orig, $this->data, $file);
            };

            //write if changed
            if (! $merge || ($data !== $file))
            {
                //write temp file
                $temp = dirname($path) . "/" . unique_id() . ".tmp";
                if (($hfile = fopen($temp, "wb")) === FALSE) return FALSE;

                flock($hfile, LOCK_EX);
                stream_set_write_buffer($hfile, 0);

                //stream lines in chunks
                $context = hash_init("xxh3");
                $buffer  = "";
                $error   = FALSE;

                foreach ((array)$data AS $key => $value)
                {
                    if (stre($key))         continue; //no key
                    if (! is_array($value)) continue; //no values

                    //values
                    $line = "";
                    foreach ($value AS $_key => $_value)
                    {
                        if (stre($_value)) continue; //empty value

                        //encryption
                        if ($this->password !== NULL)
                            $_value = encrypt((string)$_value, $this->password);

                        //property / value pair
                        $line .= encchr($_key) . "=" . encchr($_value) . ";";
                    };

                    //add line
                    if ($line === "") continue;
                    $buffer .= encchr($key) . ":=" . substr($line, 0, -1) . "\n";

                    if (strlen($buffer) < 65536) continue;

                    //write buffer
                    if (fwrite($hfile, $buffer) !== strlen($buffer))
                    {
                        $error = TRUE;
                        break;
                    };

                    hash_update($context, $buffer);
                    $buffer = "";
                };

                //write remaining buffer
                if (! $error) $error = fwrite($hfile, $buffer) !== strlen($buffer);

                hash_update($context, $buffer);
                $hash = hash_final($context);

                fclose($hfile);

                //replace file
                if ($error || ! rename($temp, $path))
                {
                    unlink($temp);
                    return FALSE;
                };
            };
        }
        finally
        {
            //release lock
            cms_unlock($path);
        };

        //update state
        $this->data = $data;

        $this->snapshot();

        self::$cache[$path]      = $this->data;
        self::$file_state[$path] = ["data" => $this->data, "hash" => $hash];

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    static function merge($orig,
                          $data,
                          $file)
    {
        $orig = (array)$orig;
        $data = (array)$data;
        $file = (array)$file;
        $work = new self;

        //remove deleted entries
        foreach ([[&$file, &$data], [&$data, &$file]] AS [&$v1, &$v2])
        {
            $work->data = &$v1;
            foreach (array_diff_key($orig, $v2) AS $key => $_) $work->_del($key);
        };

        unset($v1, $v2);

//..................................................................................................

        //data changes

        foreach ($data AS $key => $value)
        {
            //new entry
            if (! isset($orig[$key]))
            {
                $file[$key] = array_replace($file[$key] ?? [], $value);
                continue;
            };

            if ($value === $orig[$key]) continue; //not changed

            //add properties
            foreach ($value AS $_key => $_value)
            {
                if (($orig[$key][$_key] ?? NULL) !== $_value)
                    $file[$key][$_key] = $_value;
            };

            //remove properties
            foreach ($orig[$key] AS $_key => $_)
            {
                if (! isset($value[$_key]))
                    unset($file[$key][$_key]);
            };
        };

//..................................................................................................

        //positional changes

        //get original position of previously existing entries
        $order_orig = array_flip(array_keys($orig));
        $order_data = [];

        foreach ($data AS $key => $_)
        {
            if (isset($order_orig[$key])) $order_data[$key] = $order_orig[$key];
        };

        //moved or added entries
        $delta = array_diff_key($data, longest_increasing($order_data));
        if (! $delta) return $file; //not changed

        $prev = ""; //predecessor
        $next = []; //changed entry after predecessor

        foreach ($data AS $key => $_)
        {
            if (isset($delta[$key])) $next[$prev] = $key; //changed

            //set predecessor
            $prev = $key;
        };

        //unmoved entries in file order followed by changed entries
        $merged = [];

        foreach (array_merge([""], array_keys(array_diff_key($file, $delta))) AS $key)
        {
            if ($key !== "") $merged[$key] = $file[$key];

            for ($_key = $next[$key] ?? NULL; $_key !== NULL; $_key = $next[$_key] ?? NULL)
                $merged[$_key] = $file[$_key];
        };

        return $merged;
    }

//--------------------------------------------------------------------------------------------------

    private function defer_remove()
    {
        unset(self::$defer[$this->path][spl_object_id($this)]);
        if (empty(self::$defer[$this->path])) unset(self::$defer[$this->path]);
    }

//--------------------------------------------------------------------------------------------------

    static function defer_apply($file = NULL)
    {
        if (nstre($file))
        {
            if (! isset(self::$defer[$file])) return TRUE;
            $list = [$file];
        }
        else
        {
            $list = array_keys(self::$defer);
        };

        $result = TRUE;
        foreach ($list AS $file)
        {
            $defer = self::$defer[$file] ?? [];
            unset(self::$defer[$file]);

            foreach ($defer AS $object)
                if (! $object->save($file, TRUE)) $result = FALSE;
        };

        return $result;
    }

//--------------------------------------------------------------------------------------------------

    static function defer_relocate($from,
                                   $to)
    {
        foreach ([&self::$cache, &self::$file_state] AS &$value)
        {
            if (isset($value[$from])) $value[$to] = $value[$from];
            else                      unset($value[$to]);
            unset($value[$from]);
        };
    }

//--------------------------------------------------------------------------------------------------

    static function defer_discard($file)
    {
        unset(self::$cache[$file], self::$file_state[$file], self::$defer[$file]);
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

    private function _set($value    = NULL,
                          $key      = NULL,
                          $property = NULL)
    {
        if (! is_array($value) && stre($value))
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

    private function _has($key      = NULL,
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

    private function _del($key       = NULL,
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

                $type = $this->data[$key]["#type"] ?? "";
                unset($this->data[$key]);
                if (nstreq($type, "container")) return TRUE;

                $this->_move("to", $key);
                $depth = 1;
                while (
                        ($depth > 0)
                        &&
                        (($key = $this->_move("next")) !== NULL)
                      )
                {
                    if (! isset($this->data[$key]["#type"])) continue;

                    switch ($this->data[$key]["#type"])
                    {
                    case "container":  $depth++; break;
                    case "/container": $depth--; break;
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

    function move($target = "current", //"current", "first", "last", "prev", "next", "to", "parent"
                  $key    = NULL)
    {
        if ($key !== NULL) $key = $this->remove_prefix($key); //remove prefix
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

    function seek($condition)
    {
        $condition = array_map([$this, "remove_prefix"], $condition); //remove prefix
        return $this->apply_prefix($this->_seek($condition));
    }

//--------------------------------------------------------------------------------------------------

    private function _seek($condition)
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

    function is_container($key)
    {
        $key = $this->remove_prefix($key); //remove prefix
        return $this->_is_container($key);
    }

//--------------------------------------------------------------------------------------------------

    private function _is_container($key)
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

    private function _is_child($key,
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

    private function _has_children($key)
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

    private function _cut($key)
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

    private function _insert($key = NULL)
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

    private function _append($key = NULL)
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

    private function _copy($key,
                           $action = NULL)
    {
        if (streq($action, "append"))
        {
            if ($this->buffer === NULL) return FALSE;
            elseif (stre($key))         return $this->_insert(0);
        };

        if (! preg_match("/^(append|del)$/", $action ?? "")) $this->buffer = NULL;
        if (! isset($this->data[$key]))                      return FALSE;

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

        if (nstre($value))
            $this->prefix = (string)$value;
        elseif (isset($this->path) && isset($prefix[$this->path]))
            $this->prefix = $prefix[$this->path];
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