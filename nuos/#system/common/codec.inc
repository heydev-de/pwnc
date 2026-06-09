<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

function encrypt($string,
                 $password)
{
    //generate salt
    $salt = random_bytes(SODIUM_CRYPTO_PWHASH_SALTBYTES);

    //create key from password
    $key = sodium_crypto_pwhash(
        SODIUM_CRYPTO_SECRETBOX_KEYBYTES,
        $password,
        $salt,
        SODIUM_CRYPTO_PWHASH_OPSLIMIT_INTERACTIVE,
        SODIUM_CRYPTO_PWHASH_MEMLIMIT_INTERACTIVE);

    //generate nonce (number used once)
    $nonce = random_bytes(SODIUM_CRYPTO_SECRETBOX_NONCEBYTES);

    //encrypt
    $string = sodium_crypto_secretbox($string, $nonce, $key);

    //generate hmac for integrity protection
    $hmac = sodium_crypto_auth($nonce . $string, $key);

    //concatenate parts
    return base64_encode($salt . $nonce . $hmac . $string);
};

//--------------------------------------------------------------------------------------------------

function decrypt($string,
                 $password)
{
    $string = base64_decode($string);

    //extract parts
    $salt   = substr($string, $i  = 0,                                  SODIUM_CRYPTO_PWHASH_SALTBYTES);
    $nonce  = substr($string, $i += SODIUM_CRYPTO_PWHASH_SALTBYTES,     SODIUM_CRYPTO_SECRETBOX_NONCEBYTES);
    $hmac   = substr($string, $i += SODIUM_CRYPTO_SECRETBOX_NONCEBYTES, SODIUM_CRYPTO_AUTH_BYTES);
    $string = substr($string, $i += SODIUM_CRYPTO_AUTH_BYTES);

    //create key from password
    $key = sodium_crypto_pwhash(
        SODIUM_CRYPTO_SECRETBOX_KEYBYTES,
        $password,
        $salt,
        SODIUM_CRYPTO_PWHASH_OPSLIMIT_INTERACTIVE,
        SODIUM_CRYPTO_PWHASH_MEMLIMIT_INTERACTIVE);

    //check integrity
    if (! sodium_crypto_auth_verify($hmac, $nonce . $string, $key)) return NULL;

    //decrypt
    return sodium_crypto_secretbox_open($string, $nonce, $key);
};

//--------------------------------------------------------------------------------------------------

function rc4encrypt($string,
                    $password)
{
    if (! $password) return NULL;

    $string          = (string)$string;
    $key             = [];
    $state           = [];
    $return          = "";

    $string_length   = strlen($string);
    $password_length = strlen($password);

    for ($i = 0; $i < 256; $i++)
    {
        $key[$i]   = ord($password[$i % $password_length]);
        $state[$i] = $i;
    };

    for ($i = 0, $j = 0; $i < 256; $i++)
    {
        $j         = ($j + $state[$i] + $key[$i]) % 256;
        $k         = $state[$i];
        $state[$i] = $state[$j];
        $state[$j] = $k;
    };

    for ($i = 0, $j = 0, $k = 0; $i < $string_length; $i++)
    {
        $k          = ($k + 1)          % 256;
        $j          = ($j + $state[$k]) % 256;

        $l          = $state[$k];
        $state[$k]  = $state[$j];
        $state[$j]  = $l;

        $l          = $state[($state[$k] + $state[$j]) % 256];
        $return    .= chr(ord($string[$i]) ^ $l);
    };

    return $return;
};

//--------------------------------------------------------------------------------------------------

function rc4decrypt($string,
                    $password)
{
    return rc4encrypt($string, $password);
};

//--------------------------------------------------------------------------------------------------

function encchr($string)
{
    return strtr(
        $string,
        ["\0" => "[chr0]",
         "\n" => "[chr10]",
         "\r" => "[chr13]",
         ";"  => "[chr59]",
         "="  => "[chr61]",
         "["  => "[chr91]",
         "\\" => "[chr92]"]);
};

//--------------------------------------------------------------------------------------------------

function decchr($string)
{
    return strtr(
        $string,
        ["[chr0]"  => "\0",
         "[chr10]" => "\n",
         "[chr13]" => "\r",
         "[chr59]" => ";",
         "[chr61]" => "=",
         "[chr91]" => "[",
         "[chr92]" => "\\"]);
};

//--------------------------------------------------------------------------------------------------

function sqlesc($value,
                $escape_backticks = FALSE)
{
    switch (gettype($value))
    {
    case "boolean":

        return $value ? "1" : "";

    case "integer":
    case "double":

        return $value;

    case "string":

        return $escape_backticks ?
               str_replace("`", "``", $value) :
               strtr($value,
                     ["\0"   => "\\0",
                      "\n"   => "\\n",
                      "\r"   => "\\r",
                      "\\"   => "\\\\",
                      "'"    => "\\'",
                      "\""   => "\\\"",
                      "\x1A" => "\\Z"]);

    case "array":

        foreach ($value AS $key => $_value)
            $value[$key] = sqlesc($_value, $escape_backticks);

        return $value;

    default:
    //case "object":
    //case "resource":
    //case "resource (closed)":
    //case "unknown type":
    };

    return "";
};

//--------------------------------------------------------------------------------------------------

function q($string,
           $escape_closing_tag = TRUE,
           $binary             = FALSE)
{
    static $array = NULL;

    if ($array === NULL)
    {
        $array = ["\0"   => "\\0",   "\x01" => "\\x01", "\x02" => "\\x02", "\x03" => "\\x03",
                  "\x04" => "\\x04", "\x05" => "\\x05", "\x06" => "\\x06", "\x07" => "\\x07",
                  "\x08" => "\\b",   "\t"   => "\\t",   "\n"   => "\\n",   "\x0B" => "\\x0B",
                  "\f"   => "\\f",   "\r"   => "\\r",   "\x0E" => "\\x0E", "\x0F" => "\\x0F",
                  "\x10" => "\\x10", "\x11" => "\\x11", "\x12" => "\\x12", "\x13" => "\\x13",
                  "\x14" => "\\x14", "\x15" => "\\x15", "\x16" => "\\x16", "\x17" => "\\x17",
                  "\x18" => "\\x18", "\x19" => "\\x19", "\x1A" => "\\x1A", "\x1B" => "\\x1B",
                  "\x1C" => "\\x1C", "\x1D" => "\\x1D", "\x1E" => "\\x1E", "\x1F" => "\\x1F",
                  "\""   => "\\\"",  "'"    => "\\'",   "\\"   => "\\\\"];
        for ($i = 127; $i <= 255; $i++) $array[chr($i)] = sprintf("\\x%02X", $i);
    };

    //binary encoding
    if ($binary)
    {
        $string = strtr($string, $array);
    }

    //utf-16 encoding
    else
    {
        $buffer = [];
        $string = (string)$string;
        $c      = strlen($string);
        $i      = 0;

        while ($i < $c)
        {
            $chr = $string[$i++];
            $ord = ord($chr);

            while (($ord >= 192) && ($ord < 248))
            {
                if ($ord < 224) //2 byte character
                {
                    if (($c - $i) < 1) break;
                    $v = ($ord               & 31) << 6  |
                         (ord($string[$i++]) & 63);
                    $buffer[] = sprintf("\\u%04X", $v);
                }
                elseif ($ord < 240) //3 byte character
                {
                    if (($c - $i) < 2) break;
                    $v = ($ord               & 15) << 12 |
                         (ord($string[$i++]) & 63) << 6  |
                         (ord($string[$i++]) & 63);
                    $buffer[] = sprintf("\\u%04X", $v);
                }
                else //4 byte character
                {
                    if (($c - $i) < 3) break;
                    //* using & 15 instead of & 7 provides binary data integrity
                    $v = (($ord               & 15) << 18 | //*
                          (ord($string[$i++]) & 63) << 12 |
                          (ord($string[$i++]) & 63) << 6  |
                          (ord($string[$i++]) & 63))
                         - 0x10000;
                    $h = 0xD800 + ($v >> 10);
                    $l = 0xDC00 + ($v & 0x3FF);
                    $buffer[] = sprintf("\\u%04X\\u%04X", $h, $l);
                };

                continue 2;
            };

            //ascii
            $buffer[] = isset($array[$chr]) ? $array[$chr] : $chr;
        };

        $string = implode("", $buffer);
    };

    if ($escape_closing_tag) $string = str_replace("</", "<\/", $string);
    return $string;
};

//--------------------------------------------------------------------------------------------------

function qb($string,
            $escape_closing_tag = TRUE)
{
    return q($string, $escape_closing_tag, TRUE);
};

//--------------------------------------------------------------------------------------------------

function r($string)
{
    return rawurlencode($string);
};

//--------------------------------------------------------------------------------------------------

function x($string)
{
    return xmlspecialchars($string);
};

//--------------------------------------------------------------------------------------------------

function rq($string)
{
    return q(rawurlencode($string), FALSE);
};

//--------------------------------------------------------------------------------------------------

function qr($string)
{
    return rawurlencode(q($string, FALSE));
};

//--------------------------------------------------------------------------------------------------

function qx($string)
{
    return xmlspecialchars(q($string, FALSE));
};

//--------------------------------------------------------------------------------------------------

function rx($string)
{
    return xmlspecialchars(rawurlencode($string));
};

//--------------------------------------------------------------------------------------------------

function qrx($string)
{
    return xmlspecialchars(rawurlencode(q($string, FALSE)));
};

//--------------------------------------------------------------------------------------------------

function xmlspecialchars($string,
                         $decode = NULL)
{
    static $array_encode = ["\"" => "&quot;", "'" => "&apos;", "&" => "&amp;", "<" => "&lt;", ">" => "&gt;"];
    static $array_decode = ["&quot;" => "\"", "&apos;" => "'", "&amp;" => "&", "&lt;" => "<", "&gt;" => ">"];

    return strtr($string, $decode ? $array_decode : $array_encode);
};

//--------------------------------------------------------------------------------------------------

function htmlentities_decode($string)
{
    static $translation = NULL;

    //replace numeric entities
    $string = preg_replace_callback(
        "/&#x([0-9a-f]+);/i",
        function($value) { return utf8_chr(hexdec($value[1])); },
        $string);
    $string = preg_replace_callback(
        "/&#([0-9]+);/",
        function($value) { return utf8_chr($value[1]); },
        $string);

    //replace literal entities
    if ($translation === NULL)
    {
        $translation  = [];
        $_translation = get_html_translation_table(HTML_ENTITIES);

        foreach ($_translation AS $key => $value)
            $translation[$value] = utf8_encode($key);
    };

    return strtr($string, $translation);
};

//--------------------------------------------------------------------------------------------------

function escape($string)
{
    return preg_replace_callback(
        "/(.)/",
        function($value) { return "%" . bin2hex($value[1]); },
        $string);
};