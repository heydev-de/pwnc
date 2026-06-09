<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   CORE_RESOURCE CLASS
//==================================================================================================

class core_resource
{

//properties
//--------------------------------------------------------------------------------------------------

    public $file             = "";
    public $hfile            = NULL;
    public $field_type       = NULL;
    public $field_offset     = NULL;
    public $field_length     = NULL;
    public $recordset_length = 0;
    public $offset           = 0;
    public $zero             = "";
    public $lock             = [];

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($file,
                         $structure)
    {
        $offset = 0;

        foreach ($structure AS $key => $value)
        {
            if (preg_match("/^(string|_string|byte)(?:\[([0-9]+)\])?$/", $value, $_value))
            {
                switch ($_value[1])
                {
                case "string":

                    $length = ($_value[2] !== "") ? (int)$_value[2]     : 1;
                    break;

                case "_string": //utf-8

                    $length = ($_value[2] !== "") ? (int)$_value[2] * 4 : 4;
                    break;

                case "byte":

                    $length = 1;
                    break;
                };

                $this->field_type[$key]    = $_value[1];
                $this->field_offset[$key]  = $offset;
                $this->field_length[$key]  = $length;
                $offset                   += $length;
            };
        };

        $this->recordset_length = $offset;
        $this->zero             = str_repeat("\0", $this->recordset_length);
        $this->file             = $file;

        if (is_file($file))
        {
            $this->hfile = fopen($file, "r+b");
        }
        else
        {
            mkpath(dirname($file));
            $this->hfile = fopen($file, "w+b");
        };

        stream_set_write_buffer($this->hfile, 0);

        register_shutdown_function([$this, "close"]);
    }

//destructor
//--------------------------------------------------------------------------------------------------

    function __destruct()
    {
        $this->close();
    }

//methods
//--------------------------------------------------------------------------------------------------

    function current()
    {
        return max(0, $this->offset);
    }

//--------------------------------------------------------------------------------------------------

    function reset()
    {
        $this->offset = -$this->recordset_length;
    }

//--------------------------------------------------------------------------------------------------

    function next($filter = NULL,
                   $reset  = FALSE,
                   $limit  = NULL)
    {
        $filesize = filesize($this->file);
        $limit    = ($limit === NULL) ? $filesize : min($limit, $filesize);
        $count    = is_array($filter) ? count($filter) : 0;
        $result   = FALSE;

        if ($reset) $this->reset();

        $this->lock();
        while ($this->offset < $limit)
        {
            $this->offset += $this->recordset_length;

            if ($count > 0)
            {
                $value  = $this->get();
                $_count = 0;

                foreach ($filter AS $key => $_value)
                {
                    if (
                         (($_value === TRUE) && ((string)$value[$key] !== ""))
                         ||
                         (in_array($_value, [NULL, FALSE], TRUE) && ((string)$value[$key] === ""))
                         ||
                         ((string)$_value === (string)$value[$key])
                       )
                        $_count++;
                    else
                        break;
                };

                if ($count === $_count) { $result = TRUE; break; };
            }
            else
            {
                $result = TRUE;
                break;
            };
        };

        $this->unlock();
        return $result;
    }

//--------------------------------------------------------------------------------------------------

    function seek($filter,
                  $next = FALSE)
    {
        $this->offset = $this->current();
        $offset       = $this->offset;
        if ($next) $this->offset += $this->recordset_length;

        $this->lock();
        $result = $this->next($filter) || $this->next($filter, TRUE, $offset);
        $this->unlock();

        return $result;
    }

//--------------------------------------------------------------------------------------------------

    function get($key = NULL,
                 $raw = FALSE)
    {
        $offset = $this->current();

        $this->lock();
        fseek($this->hfile, $offset);
        $value = fread($this->hfile, $this->recordset_length);
        $this->unlock();

        if ($value === FALSE) return NULL;

        if ($key !== NULL)
        {
            if (isset($this->field_type[$key]))
            {
                $value = substr($value,
                                $this->field_offset[$key],
                                $this->field_length[$key]);
                return $raw ? $value : $this->field_decode($key, $value);
            };

            return NULL;
        };

        $array = [];
        foreach ($this->field_type as $key => $_)
        {
            $_value = substr($value,
                             $this->field_offset[$key],
                             $this->field_length[$key]);
            $array[$key] = $raw ? $_value : $this->field_decode($key, $_value);
        };

        return $array;
    }

//--------------------------------------------------------------------------------------------------

    function set($value)
    {
        $offset = $this->current();
        $_value = $this->get(NULL, TRUE) ?? [];
        $_value = $value + $_value;

        $array = [];
        foreach ($this->field_length AS $key => $length)
            $array[] = pack("a" . $length,
                            isset($_value[$key]) ?
                            $this->field_encode($key, $_value[$key]) :
                            "");

        $this->lock(TRUE);
        fseek($this->hfile, $offset);
        fwrite($this->hfile, implode("", $array));
        $this->unlock();

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function del()
    {
        $this->lock(TRUE);
        fseek($this->hfile, $this->current());
        fwrite($this->hfile, $this->zero);
        $this->unlock();

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function field_encode($key,
                          $value)
    {
        switch ($this->field_type[$key])
        {
        case "string":
        case "_string": return $value;
        case "byte":    return chr($value);
        };

        return $value;
    }

//--------------------------------------------------------------------------------------------------

    function field_decode($key,
                          $value)
    {
        switch ($this->field_type[$key])
        {
        case "string":
        case "_string": return rtrim($value, "\0");
        case "byte":    return ord($value);
        };

        return $value;
    }

//--------------------------------------------------------------------------------------------------

    function lock($exclusive = FALSE)
    {
        if ($exclusive)
        {
            if (end($this->lock) !== 1) flock($this->hfile, LOCK_EX);
        }
        elseif (end($this->lock) === FALSE)
        {
            flock($this->hfile, LOCK_SH);
        };

        $this->lock[] = (int)$exclusive;
    }

//--------------------------------------------------------------------------------------------------

    function unlock()
    {
        $state = array_pop($this->lock);
        if ($state === NULL) return;

        $_state = end($this->lock);
        if ($_state === FALSE)
            flock($this->hfile, LOCK_UN);

        elseif (($state === 1) && ($_state === 0))
            flock($this->hfile, LOCK_SH);
    }

//--------------------------------------------------------------------------------------------------

    function close()
    {
        if (is_resource($this->hfile)) fclose($this->hfile);
    }
};