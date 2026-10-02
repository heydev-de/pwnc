<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

function each(&$array)
{
    $key = key($array);
    if ($key === NULL) return FALSE;

    $value = current($array);
    next($array);

    return [0 => $key, 1 => $value, "key" => $key, "value" => $value];
};

//--------------------------------------------------------------------------------------------------

function init(&$variable,
              $default_value = NULL)
{
    if (blank($variable)) $variable = $default_value;

    return $variable;
};

//--------------------------------------------------------------------------------------------------

function blank(&$variable)
{
    if (! isset($variable)) return TRUE;

    switch (gettype($variable))
    {
    case "boolean":
    case "integer":
    case "double":
    case "string":

        return (string)$variable === "";
    };

    return empty($variable);
};

//--------------------------------------------------------------------------------------------------

function yesno($boolean)
{
    return $boolean ? t("yes") : t("no");
};

//--------------------------------------------------------------------------------------------------

function option($boolean,
                $value)
{
    return $boolean ? $value : NULL;
};

//--------------------------------------------------------------------------------------------------

function get_mime_type($filename_or_extension = NULL)
{
    static $list = NULL;

    //load extension type list
    if ($list === NULL)
    {
        $array = file(
            CMS_PATH . "mimetype",
            FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);

        $list = [];
        foreach ($array AS $value)
        {
            list($extension, $type) = explode(" ", $value, 2);
            $list[$extension]       = $type;
        };
    };

    //return list
    if ($filename_or_extension === NULL) return $list;

    //get file extension
    if (strpos($filename_or_extension, ".") !== FALSE)
        $filename_or_extension = file_extension($filename_or_extension);

    //return mime type
    return $list[$filename_or_extension] ?? "application/octet-stream";
};

//--------------------------------------------------------------------------------------------------

function get_mime_list()
{
    $list   = get_mime_type();
    $return = [];

    foreach ($list AS $extension => $type)
    {
        $icon = get_mime_icon($type, NULL);
        if ($icon !== NULL) $return[$extension] = $icon;
    };

    return $return;
};

//--------------------------------------------------------------------------------------------------

function get_mime_icon($type,
                       $default = "mimetype/default")
{
    $_type = "mimetype/$type";
    if (image_exists($_type)) return $_type;

    if (($pos = strpos($type, "/")) !== FALSE)
    {
        $_type = "mimetype/" . substr($type, 0, $pos);
        if (image_exists($_type)) return $_type;
    };

    return $default;
};

//--------------------------------------------------------------------------------------------------

function in_array_recursive($needle,
                            $haystack,
                            $strict = FALSE)
{
    $iterator = new \RecursiveIteratorIterator(new \RecursiveArrayIterator($haystack));

    foreach ($iterator AS $element)
    {
        if ($strict)
        {
            if ($element === $needle) return TRUE;
        }
        elseif ($element == $needle)
        {
            return TRUE;
        };
    };

    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function ksort_recursive(&$array,
                         $flag = SORT_REGULAR)
{
    ksort($array, $flag);

    foreach ($array AS $key => $value)
    {
        if (is_array($value))
            ksort_recursive($array[$key], $flag);
    };
};

//--------------------------------------------------------------------------------------------------

function bitstring($value,
                   $length = NULL)
{
    $return = "";

    for ($i = 0, $c = strlen($value); $i < $c; $i++)
    {
        $byte = ord(substr($value, $i, 1));

        for ($_i = 7; $_i >= 0; $_i--)
            $return .= ($byte & (1 << $_i)) ? "1" : "0";
    };

    return ($length !== NULL) ? substr($return, -(int)$length) : $return;
};

//--------------------------------------------------------------------------------------------------

function set_time_limit($time)
{
    if ($time > ini_get("max_execution_time")) \set_time_limit($time);
};

//--------------------------------------------------------------------------------------------------

function anonymize_ip($address)
{
    if ($address = inet_pton($address))
    {
        $length = (int)(strlen($address) / 4);
        return inet_ntop(substr($address, 0, $length * 3) . str_repeat(chr(0), $length));
    };

    return "0.0.0.0";
};

//--------------------------------------------------------------------------------------------------

function preview($content_code,
                 $stylesheet = NULL,
                 $body_class = "preview")
{
    if (stre($stylesheet)) $stylesheet = CMS_DATA_URL . "stylesheet.css";
    $_url = x(CMS_JAVASCRIPT_URL);

    echo(CMS_DOCTYPE_HTML .
         "<html>" .
         "<head>" .
         "<meta charset=\"utf-8\">" .
         "<title>" . t("preview") . "</title>" .
         "<meta name=\"robots\" " .
               "content=\"noindex, nofollow\">" .
         "<base href=\"" . x(CMS_ROOT_URL) . "\">" .
         "\n<script src=\"" . $_url . "common.js\">" .
         "</script>" .
         "\n<script src=\"" . $_url . "fx.js\">" .
         "</script>" .
         "\n<script src=\"" . $_url . "defer.js\" defer>" .
         "</script>" .
         "<noscript>" .
         "<style>" .
         "IMG[data-defer-src] { DISPLAY: none !important; }" .
         "</style>" .
         "</noscript>" .
         "<link rel=\"stylesheet\" " .
               "href=\"" . x($stylesheet) . "\">" .
         "</head>" .
         "<body class=\"" . x($body_class) . "\">" .
         $content_code .
         "</body>" .
         "</html>");
};

//--------------------------------------------------------------------------------------------------

function preview_inert($content_code)
{
    echo(CMS_DOCTYPE_HTML .
         "<html>" .
         "<head>" .
         "<meta charset=\"utf-8\">" .
         "<title>" . t("preview") . "</title>" .
         "<meta name=\"robots\" " .
               "content=\"noindex, nofollow\">");

?><style>

BODY
{
    MARGIN: 0;
}

IFRAME
{
    BORDER: none;
    HEIGHT: 100%;
    POSITION: fixed;
    WIDTH: 100%;
}

</style><?php

    echo("</head>" .
         "<body>" .
         "<iframe srcdoc=\"" . x($content_code) . "\" " .
                 "sandbox=\"allow-same-origin\" " .
                 "onload=\"this.contentDocument.documentElement.setAttribute('inert','');\">" .
         t("preview_unavailable") .
         "</iframe>" .
         "</body>" .
         "</html>");
};

//--------------------------------------------------------------------------------------------------

function force_flush($size = 65536)
{
    if (! headers_sent())
    {
        header("Cache-Control: no-transform");
        header("Content-Encoding: none");
        header("X-Accel-Buffering: no");
    };

    while (ob_get_level()) ob_end_flush();

    echo("<script type=\"text/plain\">" .
         str_replace(["\0", "<"], "_", random_bytes($size)) .
         "</script>");

    flush();
};

//--------------------------------------------------------------------------------------------------

function notify($user,
                $text,
                $sender = NULL)
{
    if (! cms_load("core_resource")) return FALSE;

    //create core object
    $ims = new core_resource(
        CMS_DATA_PATH . "#desktop/ims.core",
        ["id"       => "string[8]",
         "thread"   => "string[8]",
         "time"     => "string[20]",
         "owner"    => "_string[40]",
         "receiver" => "_string[40]",
         "sender"   => "_string[40]",
         "status"   => "byte",
         "text"     => "_string[1500]",
         "hash"     => "string[32]"]);

    //prepare data
    $thread   = unique_id(8);
    $time     = time();
    $sender ??= $user;
    $text     = utf8_substr($text, 0, 1500);
    $hash     = hash32($thread . $time . $sender . $text);

    //create message
    $ims->seek(["owner" => NULL], TRUE);
    $ims->set(
        ["id"       => unique_id(8),
         "thread"   => $thread,
         "time"     => $time,
         "owner"    => $user,
         "receiver" => $user,
         "sender"   => $sender,
         "status"   => 0,
         "text"     => $text,
         "hash"     => $hash]);

    //create notification file
    write_file(CMS_DATA_PATH . "#desktop/" . encode_filename($user) . "/ims.flag", "1");

    return TRUE;
};

//--------------------------------------------------------------------------------------------------

function notify_admin($text)
{
    return notify("admin", t("system_message") . ": $text");
};

//--------------------------------------------------------------------------------------------------

function array_get_path($array,
                        $path,
                        $delimiter = "/")
{
    //multiple paths
    if (is_array($path))
    {
        foreach ($path AS $_path)
        {
            $value = array_get_path($array, $_path, $delimiter);
            if ($value !== NULL) return $value;
        };

        return NULL;
    };

    if (stre($path)) return $array; //root

    //find value at path
    foreach (explode($delimiter, $path) AS $key)
    {
        if (! is_array($array))    return NULL;
        if (! isset($array[$key])) return NULL;
        $array = $array[$key];
    };

    return $array;
};

//--------------------------------------------------------------------------------------------------

function array_set_path(&$array,
                        $path,
                        $value,
                        $delimiter = "/")
{
    if (stre($path)) { $array = $value; return TRUE; }; //root

    $key  = explode($delimiter, $path);
    $last = array_pop($key);
    $ref  = &$array;

    foreach ($key AS $_key)
    {
        if (! isset($ref[$_key]))        $ref[$_key] = [];
        elseif (! is_array($ref[$_key])) return FALSE;
        $ref = &$ref[$_key];
    };

    $ref[$last] = $value;
    return TRUE;
};