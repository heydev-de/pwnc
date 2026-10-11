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
    if (is_dir($path))      return TRUE;
    if (file_exists($path)) return is_dir($path);

    $length = strlen(CMS_ROOT_PATH);
    if (! strncmp($path, CMS_ROOT_PATH, $length)) $path = substr($path, $length);

    $path  = explode("/", $path);
    $_path = CMS_ROOT_PATH;

    foreach ($path AS $key => $value)
    {
        if (stre($value)) continue;

        $_path .= $value;

        if (is_dir($_path));
        elseif (! mkdir($_path) && ! is_dir($_path)) return FALSE;

        $_path .= "/";
    };

    return TRUE;
};

//--------------------------------------------------------------------------------------------------

function real_path($path)
{
    $regex  = "~(?:(?:^|[/\\\\])\.)?[/\\\\](?:\.(?:[/\\\\]|$))?+~";
    $prefix = strspn($path, "/\\", 0, 1) ? "/" : "";
    $suffix = strspn(substr($path, -1), "/\\", 0, 1) ? "/" : "";
    $path   = preg_split($regex, $path, -1, PREG_SPLIT_NO_EMPTY);
    $return = [];

    foreach ($path AS $value)
    {
        switch ($value)
        {
        case "":
        case ".":

            break;

        case "..":

            array_pop($return);
            break;

        default:

            array_push($return, $value);
            break;
        };
    };

    $return = implode("/", $return);
    return stre($return) ? $prefix : $prefix . $return . $suffix;
};

//--------------------------------------------------------------------------------------------------

function resolve_path($source,
                      $target)
{
    //target is absolute
    if (strspn($target, "/\\", 0, 1))
    {
        $prefix = "/";
        $is_dir = TRUE;
        $source = [];
    }

    //target is relative
    else
    {
        $source = real_path($source);
        $prefix = strspn($source, "/\\", 0, 1) ? "/" : "";
        $is_dir = preg_match("~(?:^|[/\\\\])\.{1,2}|[/\\\\]$~", $source);
        $source = preg_split("~/~", $source, -1, PREG_SPLIT_NO_EMPTY);
    };

    $suffix = strspn(substr($target, -1), "/\\", 0, 1) ? "/" : "";
    $regex  = "~(?:(?:^|[/\\\\])\.)?[/\\\\](?:\.(?:[/\\\\]|$))?+~";
    $target = preg_split($regex, $target, -1, PREG_SPLIT_NO_EMPTY);

    //target set
    if ($target)
    {
        if (! $is_dir) array_pop($source); //remove file

        foreach ($target AS $value)
        {
            switch ($value)
            {
            case "":
            case ".":

                break;

            case "..":

                array_pop($source);
                break;

            default:

                array_push($source, $value);
                break;
            };
        };
    }

    //target empty
    else
    {
        $suffix = $is_dir ? "/" : "";
    };

    $path = implode("/", $source);
    return stre($path) ? $prefix : $prefix . $path . $suffix;
};

//--------------------------------------------------------------------------------------------------

function read_file($file,
                   $length = NULL,
                   $offset = NULL)
{
    if (is_file($file) && ($hfile = fopen($file, "rb")))
    {
        flock($hfile, LOCK_SH);
        $buffer = stream_get_contents($hfile, $length ?? -1, $offset ?? -1);
        fclose($hfile);

        return $buffer;
    };

    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function write_file($file,
                    $data)
{
    $path = dirname($file);
    if (! mkpath($path)) return FALSE;

    $temp = "$path/" . unique_id() . ".tmp";
    if (file_put_contents($temp, $data, LOCK_EX) === FALSE) return FALSE;

    if (! rename($temp, $file))
    {
        unlink($temp);
        return FALSE;
    };

    return TRUE;
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

function encode_filename($string,
                         $maxlength = 255)
{
    $maxlength  = max($maxlength, 0);
    $_maxlength = max($maxlength - 10, 0);
    $length     = 0;
    $_length    = 0;
    $flag       = FALSE;
    $win        = "^(?:aux|com[1-9]|con|lpt[1-9]|nul|prn)(?=\.|$)"; //invalid windows file names
    $regex      = "/(%s)|([^ \-0-9_a-z\\x{0080}-\\x{10FFFF}])|(.)/iu";
    $func       = function($match) use (&$length, &$flag, $maxlength)
    {
        if ($flag) return "";

        if ($match[1] === ".")    $char = ".";
        elseif ($match[1] !== "") $char = "%" . bin2hex($match[1][0]) . substr($match[1], 1);
        elseif ($match[2] !== "") $char = r($match[2]);
        else                      $char = $match[3];

        $length += strlen($char);
        if ($length > $maxlength) { $flag = TRUE; return ""; };
        return $char;
    };

    //no extension
    if (($pos = strrpos($string, ".")) < 1)
    {
        $_string = preg_replace_callback(sprintf($regex, $win), $func, $string);
        if ($flag) $_string = utf8_strcut($_string, 0, $_maxlength) .
                              "[" . hash("crc32b", $string) . "]";
        return ($_string === ".") ? "%2E" : $_string;
    };

    $extension = preg_replace_callback(sprintf($regex, "\."), $func, substr($string, $pos));
    $_string   = preg_replace_callback(sprintf($regex, $win), $func, substr($string, 0, $pos));
    if ($flag) $_string = utf8_strcut($_string, 0, $_maxlength - strlen($extension)) .
                          "[" . hash("crc32b", $string) . "]";

    return (($_string . $extension) === "..") ? "%2E%2E" : $_string . $extension;
};

//--------------------------------------------------------------------------------------------------

function decode_filename($string)
{
    $string = preg_replace("/\[[0-9a-f]{8}\](?=\.[^.]*$|$)/", "", $string, 1); //remove hash
    return preg_replace_callback(
        "/%([0-9a-fA-F]{2})/",
        function($match) { return hex2bin($match[1]); },
        $string);
};

//--------------------------------------------------------------------------------------------------

function unique_filename($path,
                         $exception       = NULL,
                         $reuse_directory = FALSE)
{
    if (! file_exists($path))                          return $path;
    if (isset($exception) && streq($path, $exception)) return $path;
    $is_dir = substr($path, -1) === "/";
    if ($is_dir && $reuse_directory && is_dir($path))  return $path;

    if ($is_dir) //directory
    {
        $_path     = rtrim($path, "/");
        $extension = "/";
    }
    else
    {
        $_path     = dirname($path) . "/" . file_name($path);
        $extension = file_extension($path);
        if (nstre($extension)) $extension = "." . $extension;
    };

    $i = 0;

    do
    {
        $path = $_path . "-" . ++$i . $extension;
        if (isset($exception) && streq($path, $exception)) break;
        if ($is_dir && $reuse_directory && is_dir($path))  break;
    }
    while (file_exists($path));

    return $path;
};

//--------------------------------------------------------------------------------------------------

function pathinfo($path,
                  $flags = PATHINFO_ALL)
{
    /*
       implementation that can tell files from dirs
    */

    preg_match("~^(.*?(?:\.{1,2}$)?)(([^/\\\\]*?)(?:\.([^\./\\\\]*))?)?$~", $path, $_path);

    $array = ["dirname"   => $_path[1] ?? "",
              "basename"  => $_path[2] ?? "",
              "filename"  => $_path[3] ?? "",
              "extension" => $_path[4] ?? ""];

    //mimic original behavior
    if ($flags === PATHINFO_ALL)     return $array;
    if ($flags & PATHINFO_DIRNAME)   return $array["dirname"];
    if ($flags & PATHINFO_BASENAME)  return $array["basename"];
    if ($flags & PATHINFO_EXTENSION) return $array["extension"];
    if ($flags & PATHINFO_FILENAME)  return $array["filename"];
    return "";
};

//--------------------------------------------------------------------------------------------------

function basename($path,
                  $suffix = NULL)
{
    $suffix = nstre($suffix) ? preg_quote($suffix, "~") : "";
    return preg_replace("~.*?([^/\\\\]*)$suffix?[/\\\\]*$~", "\\1", $path);
};

//--------------------------------------------------------------------------------------------------

function file_name($path,
                   $extension = FALSE)
{
    static $cache = [];

    if (! isset($cache[$path]))
    {
        $value        = stream_is_local($path) ?
                        pathinfo($path) : analyze_url($path);
        $cache[$path] = ($value === FALSE) ?
                        FALSE :
                        [0 => $value["filename"],
                         1 => isset($value["extension"]) ?
                              utf8_strtolower($value["extension"]) : ""];
    };

    if ($cache[$path] === FALSE) return FALSE;
    return $extension ? $cache[$path][1] : $cache[$path][0];
};

//--------------------------------------------------------------------------------------------------

function file_extension($path)
{
    return file_name($path, TRUE);
};

//--------------------------------------------------------------------------------------------------

function file_size($path)
{
    //local
    if (stream_is_local($path))
        return is_file($path) ? filesize($path) : FALSE;

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

function retrieve_file($source_url,
                       $target_path,
                       $timeout = 60)
{
    if (stream_is_local($source_url)) return FALSE;

    $temp    = $target_path . "." . unique_id();
    $context = stream_context_create(
        ["http" => ["timeout"         => $timeout,
                    "header"          => "User-Agent: " . CMS_USER_AGENT . "\r\n",
                    "follow_location" => TRUE,
                    "ignore_errors"   => FALSE]]);

    set_time_limit($timeout + 60);

    if (
         \copy($source_url, $temp, $context)
         &&
         rename($temp, $target_path)
       )
        return TRUE;

    if (is_file($temp)) unlink($temp);

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
    if (($mod_time = filemtime($file)) === FALSE) return FALSE;
    if (($filesize = filesize($file))  === FALSE) return FALSE;
    $etag = "\"" . $mod_time . "." . $filesize . "\"";

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

    //buffer for agents
    if (CMS_MCP_ACTIVE) ob_start();

    //send headers
    $extension = file_extension($file);
    $name      = (stre($name)             ? file_name($file) : stringtofilename($name)) .
                 (stre($extension)        ? ""               : ".$extension");
    $charset   = streq($extension, "xls") ? "utf-16le"       : "utf-8";

    header("Content-Disposition: attachment; filename=" . quote_http_param($name));
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
        if ($buffer === FALSE) break;
        echo($buffer);
        flush();

        $size += strlen($buffer);
    }
    while (! feof($hfile));

    ignore_user_abort(TRUE);

    return fclose($hfile) ? $size : FALSE;
};