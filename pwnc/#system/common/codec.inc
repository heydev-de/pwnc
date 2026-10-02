<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

if (extension_loaded("sodium"))
{

function secretbox_key($password,
                       $salt = NULL)
{
    static $list_salt = [];
    static $list_key  = [];

    $flag = isset($salt);

    //generate salt
    $salt ??= $list_salt[$password] ??= random_bytes(SODIUM_CRYPTO_PWHASH_SALTBYTES);

    //create key from password
    $index = $password . $salt;
    $list_key[$index] ??= sodium_crypto_pwhash(
        SODIUM_CRYPTO_SECRETBOX_KEYBYTES,
        $password,
        $salt,
        SODIUM_CRYPTO_PWHASH_OPSLIMIT_INTERACTIVE,
        SODIUM_CRYPTO_PWHASH_MEMLIMIT_INTERACTIVE);

    return $flag ? $list_key[$index] : [$list_salt[$password], $list_key[$index]];
};

//--------------------------------------------------------------------------------------------------

function encrypt($string,
                 $password)
{
    $key = secretbox_key($password);

    //generate nonce (number used once)
    $nonce = random_bytes(SODIUM_CRYPTO_SECRETBOX_NONCEBYTES);

    //encrypt
    $string = sodium_crypto_secretbox($string, $nonce, $key[1]);

    //concatenate parts
    return base64_encode($key[0] . $nonce . $string);
};

//--------------------------------------------------------------------------------------------------

function decrypt($string,
                 $password)
{
    $string = base64_decode($string);

    //extract parts
    $salt   = substr($string, $i  = 0,                              SODIUM_CRYPTO_PWHASH_SALTBYTES);
    $nonce  = substr($string, $i += SODIUM_CRYPTO_PWHASH_SALTBYTES, SODIUM_CRYPTO_SECRETBOX_NONCEBYTES);
    $string = substr($string, $i += SODIUM_CRYPTO_SECRETBOX_NONCEBYTES);

    //decrypt
    return sodium_crypto_secretbox_open($string, $nonce, secretbox_key($password, $salt));
};

}
else
{

function encrypt($string, $password) { return $string; };
function decrypt($string, $password) { return $string; };

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
        for ($i = 127; $i <= 255; $i++) $array[chr($i)] = sprintf("\\x%02x", $i);
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

            if (($ord >= 192) && ($ord < 248))
            {
                if ($ord < 224) //2 byte character
                {
                    if (($c - $i) < 1) break;
                    $v = ($ord               & 31) << 6  |
                         (ord($string[$i++]) & 63);
                     $buffer[] = sprintf("\\u%04x", $v);
                }
                elseif ($ord < 240) //3 byte character
                {
                    if (($c - $i) < 2) break;
                    $v = ($ord               & 15) << 12 |
                         (ord($string[$i++]) & 63) << 6  |
                         (ord($string[$i++]) & 63);
                    $buffer[] = sprintf("\\u%04x", $v);
                }
                else //4 byte character
                {
                    if (($c - $i) < 3) break;
                    $v = (($ord               & 15) << 18 |
                          (ord($string[$i++]) & 63) << 12 |
                          (ord($string[$i++]) & 63) << 6  |
                          (ord($string[$i++]) & 63))
                         - 0x10000;
                    $h = 0xD800 + ($v >> 10);
                    $l = 0xDC00 + ($v & 0x3FF);
                     $buffer[] = sprintf("\\u%04x\\u%04x", $h, $l);
                };

                continue;
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
    return rawurlencode((string)$string);
};

//--------------------------------------------------------------------------------------------------

function x($string)
{
    return xmlspecialchars($string);
};

//--------------------------------------------------------------------------------------------------

function rq($string)
{
    return q(rawurlencode((string)$string), FALSE);
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
    return xmlspecialchars(rawurlencode((string)$string));
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

    return strtr((string)$string, $decode ? $array_decode : $array_encode);
};

//--------------------------------------------------------------------------------------------------

function htmlentities_decode($string)
{
    static $func1 = NULL;
    static $func2 = NULL;

    //decode numeric entities
    $func1  ??= function($v) { return (($v = utf8_chr(hexdec($v[1]))) === FALSE) ? " " : (($v === "&") ? "&amp;" : $v); };
    $string   = preg_replace_callback("/&#x([0-9a-f]+);/i", $func1, $string);
    $func2  ??= function($v) { return (($v = utf8_chr($v[1])) === FALSE) ? " " : (($v === "&") ? "&amp;" : $v); };
    $string   = preg_replace_callback("/&#([0-9]+);/", $func2, $string);

    //decode remaining entities
    $string = html_entity_decode($string, ENT_QUOTES | ENT_SUBSTITUTE | ENT_HTML5, "UTF-8");

    //replace substitutes
    return str_replace("\\xEF\\xBF\\xBD", " ", $string);
};

//--------------------------------------------------------------------------------------------------

function quote_http_param($string)
{
    $string = preg_replace("/[\\x00-\\x1F\\x7F]/", "", $string);
    return preg_match("/^[!#$%&'*+\-.^_`|~0-9a-zA-Z]+$/", $string) ?
           $string : "\"" . addcslashes($string, "\"\\") . "\"";
};

//--------------------------------------------------------------------------------------------------

function punycode($string)
{
    if (! preg_match("/[^\\x00-\\x7F]/", $string)) return $string;
    if (! utf8_detect($string))                  return $string;

    preg_match_all('/./su', $string, $array);

    $char    = "abcdefghijklmnopqrstuvwxyz0123456789";
    $_array  = [];
    $output  = "";

    foreach($array[0] AS $_char)
    {
        $code     = utf8_ord($_char);
        $_array[] = $code;

        if ($code < 128) $output .= $_char;
    };

    $c_ascii = strlen($output);
    $c_utf8  = count($_array);

    if ($c_ascii > 0) $output .= "-";

    $curr   = 128;
    $delta  = 0;
    $bias   = 72;
    $c_done = $c_ascii;

    while ($c_done < $c_utf8)
    {
        $next = PHP_INT_MAX;

        foreach ($_array AS $code)
        {
            if (($code >= $curr) && ($code < $next)) $next = $code;
        };

        $delta += ($next - $curr) * ($c_done + 1);
        $curr   = $next;

        foreach ($_array AS $code)
        {
            if ($code !== $curr)
            {
                if ($code < $curr) $delta++;
                continue;
            };

            $delta_remain = $delta;
            for ($step = 36; ; $step += 36)
            {
                $thresh = ($step <= $bias) ? 1 : (($step >= ($bias + 26)) ? 26 : $step - $bias);
                if ($delta_remain < $thresh) break;

                $i             = $thresh + ($delta_remain - $thresh) % (36 - $thresh);
                $output       .= $char[$i];
                $delta_remain  = intdiv($delta_remain - $thresh, 36 - $thresh);
            };

            $output .= $char[$delta_remain];

            $delta_bias  = ($c_done === $c_ascii) ? intdiv($delta, 700) : intdiv($delta, 2);
            $delta_bias += intdiv($delta_bias, ++$c_done);
            $bias        = 0;

            while ($delta_bias > 455)
            {
                $delta_bias = intdiv($delta_bias, 35);
                $bias += 36;
            };

            $bias  += intdiv(36 * $delta_bias, $delta_bias + 38);
            $delta  = 0;
        };

        $delta++;
        $curr++;
    };

    return "xn--" . $output;
};