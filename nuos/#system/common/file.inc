<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

function mkpath($path)
{
    if (is_dir($path)) return TRUE;

    $path  = explode("/", preg_replace("/^" . preg_quote(CMS_ROOT_PATH, "/") . "/iu", "", $path));
    $_path = CMS_ROOT_PATH;

    foreach ($path AS $key => $value)
    {
        if (stre($value)) continue;

        $_path .= $value;

        if (! (is_dir($_path) || mkdir($_path))) return FALSE;

        $_path .= "/";
    };

    return TRUE;
};

//--------------------------------------------------------------------------------------------------

function read_file($file)
{
    if (is_file($file) && ($hfile = fopen($file, "rb")))
    {
        flock($hfile, LOCK_SH);
        $buffer = stream_get_contents($hfile);
        fclose($hfile);

        return $buffer;
    };

    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function download($file,
                  $name = NULL)
{
    if (! is_file($file)) return FALSE; //not found
    if (headers_sent())   return FALSE;

    cms_error_silent();
    ignore_user_abort(FALSE);

    //check etag
    $mod_time = filemtime($file);
    $filesize = filesize($file);
    $etag     = "\"" . $mod_time . "." . $filesize . "\"";

    if (strpos($_SERVER["HTTP_IF_NONE_MATCH"] ?? "", $etag) !== FALSE)
    {
        http_response_code(304); //not modified
        exit(); //don't send body
    };

    //open file
    if (($hfile = fopen($file, "rb")) === FALSE) return FALSE;
    flock($hfile, LOCK_SH);

    //discard output buffering
    while (ob_get_level()) ob_end_clean();

    //send headers
    $extension = file_extension($file);
    $name      = (stre($name) ? file_name($file) : stringtofilename($name)) . "." . $extension;
    $charset   = streq($extension, "xls") ? "utf-16le" : "utf-8";

    header("Content-Disposition: attachment; filename=\"" . addcslashes($name, "\"\\") . "\"");
    header("Content-Length: $filesize");
    header("Content-Type: " . get_mime_type($file) . "; charset=$charset");
    header("Last-Modified: " . gmdate("D, d M Y H:i:s", $mod_time) . " GMT");
    header("ETag: $etag");

    //send file in chunks
    $size = 0;

    do
    {
        set_time_limit(30);
        $buffer = fread($hfile, 524288);
        echo($buffer);
        flush();

        $size += strlen($buffer);
    }
    while (! feof($hfile));

    ignore_user_abort(TRUE);

    return fclose($hfile) ? $size : FALSE;
};

//--------------------------------------------------------------------------------------------------

function ansi_transliteration($string)
{
    //ansi transliteration
    static $translation =
        ["À" => "A", "Á" => "A",  "Â" => "A", "Ã" => "A", "Ä" => "Ae", "Å" => "A", "Æ" => "Ae",
         "Ç" => "C",
         "È" => "E", "É" => "E",  "Ê" => "E", "Ë" => "E",
         "Ì" => "I", "Í" => "I",  "Î" => "I", "Ï" => "I",
         "Ð" => "D", "Ñ" => "N",
         "Ò" => "O", "Ó" => "O",  "Ô" => "O", "Õ" => "O", "Ö" => "Oe", "Ø" => "Oe",
         "Ù" => "U", "Ú" => "U",  "Û" => "U", "Ü" => "Ue",
         "Ý" => "Y", "Þ" => "Th", "ß" => "ss",

         "à" => "a", "á" => "a",  "â" => "a", "ã" => "a", "ä" => "ae", "å" => "a", "æ" => "ae",
         "ç" => "c",
         "è" => "e", "é" => "e",  "ê" => "e", "ë" => "e",
         "ì" => "i", "í" => "i",  "î" => "i", "ï" => "i",
         "ð" => "d", "ñ" => "n",
         "ò" => "o", "ó" => "o",  "ô" => "o", "õ" => "o", "ö" => "oe", "ø" => "oe",
         "ù" => "u", "ú" => "u",  "û" => "u", "ü" => "ue",
         "ý" => "y", "þ" => "th", "ÿ" => "y"];

    return strtr($string, $translation);
};

//--------------------------------------------------------------------------------------------------

function stringtofilename($string,
                          $replacement = "-")
{
    $string  = ansi_transliteration($string);

    //strip everything but letters and numbers
    $search  = ["/^[^\p{L}\p{N}]+/u", "/[^\p{L}\p{N}]+$/u", "/[^\p{L}\p{N}]+/u"];
    $replace = ["",                   "",                   $replacement];

    $string  = preg_replace($search, $replace, $string);

    //convert to lowercase
    $string  = utf8_strtolower($string);

    return $string;
};

//--------------------------------------------------------------------------------------------------

function basename($path,
                  $suffix = NULL)
{
    //remove trailing path separators
    $path = rtrim($path, "/");

    //remove preceding path
    $value = (($value = strrchr($path, "/")) === FALSE) ? $path : substr($value, 1);

    //remove suffix
    if ($suffix !== NULL)
    {
        $length = strlen($suffix);
        if (streq(substr($value, -$length), $suffix))
            $value = substr($value, 0, -$length);
    };

    return $value;
};

//--------------------------------------------------------------------------------------------------

function file_size($path)
{
    //local
    if (
         stream_is_local($path)
         &&
         realpath($path)
       )
        return filesize($path);

    //remote
    if (
         ($value = get_headers($path, TRUE))
         &&
         isset($value["Content-Length"])
       )
        return (int)$value["Content-Length"];

    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function file_name($path,
                   $extension = FALSE)
{
    static $array = [];

    if (! isset($array[$path]))
    {
        $value        = (realpath($path) && stream_is_local($path)) ?
                        pathinfo($path) : analyze_url($path);
        $array[$path] = ($value === FALSE) ?
                        FALSE :
                        [0 => $value["filename"],
                         1 => isset($value["extension"]) ?
                              utf8_strtolower($value["extension"]) : ""];
    };

    if ($array[$path] === FALSE) return FALSE;
    return $extension ? $array[$path][1] : $array[$path][0];
};

//--------------------------------------------------------------------------------------------------

function file_extension($path)
{
    return file_name($path, TRUE);
};

//--------------------------------------------------------------------------------------------------

function safe_filename($string)
{
    static $char = ".0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    if (strspn($string, $char) === strlen($string)) return $string;

    $hash   = hash("crc32b", $string);
    $string = ansi_transliteration($string);
    $string = trim(preg_replace("/[^\.0-9A-Z_a-z]+/u", "_", $string), "._");

    return substr($string, 0, 246) . "-" . $hash;
};

//--------------------------------------------------------------------------------------------------

function retrieve_file($source_url,
                       $target_path,
                       $timeout = 60)
{
    $temp    = $target_path . "." . unique_id();
    $context = stream_context_create(
        ["http" => ["timeout"         => $timeout,
                    "header"          => "User-Agent: " . CMS_USER_AGENT . "\r\n",
                    "follow_location" => TRUE,
                    "ignore_errors"   => FALSE]]);

    set_time_limit($timeout + 60);

    if (
         copy($source_url, $temp, $context)
         &&
         rename($temp, $target_path)
       )
        return TRUE;

    if (is_file($temp)) unlink($temp);

    return FALSE;
};