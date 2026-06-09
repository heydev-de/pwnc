<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   PLIST CLASS
//==================================================================================================

class plist
{

//properties
//--------------------------------------------------------------------------------------------------

    public $hfile         = NULL;
    public $record_length = NULL;
    public $buffer_length = 65536;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($name,
                         $record_length = 500)
    {
        $file = streq(substr($name, -6), ".lst") ?
            $name : CMS_DATA_PATH . "$name.lst";

        if (! is_file($file)) mkpath(dirname($file));
        $this->hfile = fopen($file, "c+b");

        $this->record_length = $record_length;
    }

//methods
//--------------------------------------------------------------------------------------------------

    function add($value,
                 $remove_existing = FALSE)
    {
        if (! $this->hfile) return FALSE;

        flock($this->hfile, LOCK_EX);

        $value  = substr($value, 0, $this->record_length);
        $value  = str_pad($value, $this->record_length, "\0", STR_PAD_RIGHT);
        $hash   = pack("N", crc32($value));
        $length = $this->record_length + 4;
        $end    = (int)(fstat($this->hfile)["size"] / $length) * $length;

        if ($remove_existing)
        {
            $n = 0;

            for ($pos = $end - $length; $pos >= 0; $pos -= $length)
            {
                fseek($this->hfile, $pos);
                $_hash = fread($this->hfile, 4);
                if ($_hash !== $hash) continue;

                $_value = fread($this->hfile, $this->record_length);
                if ($_value !== $value) continue;

                $n = 0;

                while (TRUE)
                {
                    $_pos = $pos + ($n++ * $this->buffer_length);

                    fseek($this->hfile, $_pos + $length);
                    $_value = fread($this->hfile, $this->buffer_length);
                    if (($_value === FALSE) || ($_value === "")) break;

                    fseek($this->hfile, $_pos);
                    fwrite($this->hfile, $_value);
                };

                $end -= $length;
            };

            if ($n) ftruncate($this->hfile, $end);
        };

        fseek($this->hfile, 0, SEEK_END);
        fwrite($this->hfile, $hash . $value);

        flock($this->hfile, LOCK_UN);
        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function get($offset = 0,
                 $length = 1,
                 $remove = FALSE)
    {
        if (! $this->hfile) return FALSE;

        flock($this->hfile, LOCK_EX);

        $array   = [];
        $_length = $this->record_length + 4;
        $offset  = max($offset, 0);
        $_offset = (int)(fstat($this->hfile)["size"] / $_length - 1) - $offset;
        $n       = 0;

        for ($pos = $_offset * $_length; $pos >= 0; $pos -= $_length)
        {
            fseek($this->hfile, $pos + 4);
            $value = fread($this->hfile, $this->record_length);

            $array[] = rtrim($value, "\0");

            $n++;
            if ($length <= 0)   continue;
            if ($n === $length) break;
        };

        if ($remove) $this->remove($offset, $n);

        flock($this->hfile, LOCK_UN);
        return $array;
    }

//--------------------------------------------------------------------------------------------------

    function length()
    {
        if (! $this->hfile) return FALSE;

        return (int)(fstat($this->hfile)["size"] / ($this->record_length + 4));
    }

//--------------------------------------------------------------------------------------------------

    function remove($offset,
                    $length = 1)
    {
        if (! $this->hfile) return FALSE;

        $flag = flock($this->hfile, LOCK_EX);

        $_length = $this->record_length + 4;
        $end     = (int)(fstat($this->hfile)["size"] / $_length);
        $offset  = max($end - max($offset, 0), 0);
        $_offset = $offset * $_length;
        $length  = ($length > 0) ? min($length, $offset) : $offset;
        $size    = $length * $_length;
        $n       = 0;

        while (TRUE)
        {
            $pos = $_offset + ($n++ * $this->buffer_length);

            fseek($this->hfile, $pos);
            $value = fread($this->hfile, $this->buffer_length);
            if (($value === FALSE) || ($value === "")) break;

            fseek($this->hfile, $pos - $size);
            fwrite($this->hfile, $value);
        };

        ftruncate($this->hfile, ($end - $length) * $_length);

        if ($flag) flock($this->hfile, LOCK_UN);
        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function __destruct()
    {
        if ($this->hfile) fclose($this->hfile);
    }
};