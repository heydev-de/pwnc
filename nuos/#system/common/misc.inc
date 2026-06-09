<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

if (version_compare(PHP_VERSION, "7.2.0", ">="))
{
function each(&$array)
{
    $key = key($array);
    if ($key === NULL) return FALSE;

    $value = current($array);
    next($array);

    return [0 => $key, 1 => $value, "key" => $key, "value" => $value];
};
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
    return $boolean ? CMS_L_COMMON_008 : CMS_L_COMMON_009;
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
    static $list = FALSE;

    //load extension type list
    if ($list === FALSE)
    {
        $array = file(CMS_PATH . "mimetype",
                      FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);

        foreach ($array AS $value)
        {
            list($extension, $type) = explode(" ", $value, 2);
            $list[$extension]       = $type;
        };
    };

    //return list
    if ($filename_or_extension === NULL) return $list;

    //get file extension
    if (! preg_match("/^[0-9a-z]+$/i", $filename_or_extension))
        $filename_or_extension = file_extension($filename_or_extension);

    //return mime type
    return isset($list[$filename_or_extension]) ?
           $list[$filename_or_extension] :
           "application/octet-stream";
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
    $url = "mimetype/$type";
    if (image_exists($url)) return $url;

    $url = "mimetype/" . substr($type, 0, strpos($type, "/"));
    if (image_exists($url)) return $url;

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
            $return .= ($byte & pow(2, $_i)) ? "1" : "0";
    };

    return $length ? substr($return, -$length) : $return;
};

//--------------------------------------------------------------------------------------------------

function set_time_limit($time)
{
    if ((! ini_get("safe_mode")) && ($time > ini_get("max_execution_time")))
        \set_time_limit($time);
};

//--------------------------------------------------------------------------------------------------

function anonymize_ip($address)
{
    if ($address = inet_pton($address))
    {
        $length = strlen($address) / 4;
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
         "<title>" . CMS_L_COMMON_028 . "</title>" .
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
         "<title>" . CMS_L_COMMON_028 . "</title>" .
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
         CMS_L_COMMON_029 .
         "</iframe>" .
         "</body>" .
         "</html>");
};

//--------------------------------------------------------------------------------------------------

function force_flush($size = 65536)
{
    echo("<script type=\"text/plain\">" .
         random_bytes($size) .
         "</script>");

    while (ob_get_level()) ob_end_flush();
    flush();
};