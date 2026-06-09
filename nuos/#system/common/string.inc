<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

function strtoalphanum($string,
                       $replacement = " ")
{
    $string = htmlentities_decode($string);
    $string = preg_replace("/[^\p{L}\p{N}]+/u", $replacement, $string);

    if (nstre($replacement))
    {
        $_replacement = preg_quote($replacement, "/");
        $string       = preg_replace(["/^$_replacement+|$_replacement+$/u", "/$_replacement+/u"],
                                     ["",                                   $replacement       ],
                                     $string);
    };

    return $string;
};

//--------------------------------------------------------------------------------------------------

function strtonum($string)
{
    $search = "/(-?[0-9" . preg_quote(CMS_L_THOUSAND_SEPARATOR . CMS_L_DECIMAL_SEPARATOR, "/") . "]+)/iu";
    if (! preg_match($search, $string, $string)) return FALSE;

    $string = $string[0];
    $string = str_replace(CMS_L_THOUSAND_SEPARATOR, "", $string);
    $string = str_replace(CMS_L_DECIMAL_SEPARATOR, ".", $string);

    return floatval($string);
};

//--------------------------------------------------------------------------------------------------

function stripspaces($string,
                     $preserve_newlines = FALSE,
                     $limit_empty_lines = TRUE)
{
    if ($preserve_newlines)
    {
        $search  = ["/^\s+|\s+$/u", "/[^\S\n]+/u", "/ *(\n+) */u"];
        $replace = ["",             " "          , "\\1"         ];

        if ($limit_empty_lines)
        {
            $search[]  = "/\n{3,}/u";
            $replace[] = "\n\n";
        };

        return preg_replace($search, $replace, $string);
    };

    $search  = ["/^\s+|\s+$/u", "/\s+/u"];
    $replace = ["",             " "     ];

    return preg_replace($search, $replace, $string);
};

//--------------------------------------------------------------------------------------------------

function nl2br($string)
{
    $string = str_replace("\r\n", "\n", $string);
    $string = preg_replace("/(?<=\n)\n/", "<br class=\"multiple\">", $string);

    return str_replace("\n", "<br>", $string);
};

//--------------------------------------------------------------------------------------------------

function limitstr($string,
                  $length    = 255,
                  $separator = ",")
{
    $return = NULL;
    $part   = explode($separator, $string);

    foreach ($part AS $value)
    {
        if ($value = utf8_trim($value))
        {
            if ($return) $value = $separator . $value;
            if (utf8_strlen($return . $value) > $length) break;
            $return .= $value;
        };
    };

    return utf8_substr($return, 0, $length);
};

//--------------------------------------------------------------------------------------------------

function first_paragraph($string)
{
    return preg_match("/^\V+/u", $string, $_string) ? $_string[0] : $string;
};

//--------------------------------------------------------------------------------------------------

function first_words($string,
                     $length   = 250,
                     $ellipsis = " …")
{
    $length = (int)$length;
    if (utf8_strlen($string) <= $length) return $string;

    $length = max($length - 2, 1);
    preg_match("/^(.{1,$length}(?=" . CMS_REGEX_SEPARATOR . ")|.{1,$length})/su", $string, $string);

    return $string[1] . $ellipsis;
};

//--------------------------------------------------------------------------------------------------

function zerofill($string,
                  $length)
{
    return $string . str_repeat("\0", $length - strlen($string));
};

//--------------------------------------------------------------------------------------------------

function stre($string)
{
    return (string)$string === "";
};

//--------------------------------------------------------------------------------------------------

function nstre($string)
{
    return (string)$string !== "";
};

//--------------------------------------------------------------------------------------------------

function streq($string1,
               $string2)
{
    return (string)$string1 === (string)$string2;
};

//--------------------------------------------------------------------------------------------------

function nstreq($string1,
                $string2)
{
    return (string)$string1 !== (string)$string2;
};

//--------------------------------------------------------------------------------------------------

function strieq($string1,
                $string2)
{
    return preg_match("/" . preg_quote($string1, "/") . "/iu", $string2);
};

//--------------------------------------------------------------------------------------------------

function substri_count($source,
                       $string)
{
    return preg_match_all("/" . preg_quote($string, "/") . "/iu", $source, $array);
};

//--------------------------------------------------------------------------------------------------

function verify_email($email)
{
    return preg_match(
        "/^.+" .
        "@" .
        "[0-9a-zA-ZäöüÄÖÜ][\-\.0-9a-zA-ZäöüÄÖÜ]{1,60}[0-9a-zA-ZäöüÄÖÜ]" .
        "\." .
        "[a-zA-Z]{2,4}$/u",
        $email);
};

//--------------------------------------------------------------------------------------------------

function unique_id($count = 8)
{
    static $string = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    $return = "";

    for ($i = 0; $i < $count; $i++)
        $return .= $string[mt_rand(0, 61)];

    return $return;
};

//--------------------------------------------------------------------------------------------------

function strabridge($string,
                    $length  = 50,
                    $cut_end = FALSE)
{
    $_length       = utf8_strlen($string);
    if ($_length <= $length) return $string;

    if ($cut_end) return utf8_substr($string, 0, $length - 2) . " …";

    $length       -= 3;
    $length_start  = round($length * 65 / 100, 0);
    $length_end    = $length - $length_start;

    return utf8_substr($string, 0, $length_start) .
           " … " .
           utf8_substr($string, -$length_end);
};

//--------------------------------------------------------------------------------------------------

function generate_pseudonym($seed      = NULL,
                            $syllables = 3)
{
    $array = [
        "a",  "i",   "u",   "e",  "o",
        "ka", "ki",  "ku",  "ke", "ko",
        "sa", "shi", "su",  "se", "so",
        "ta", "chi", "tsu", "te", "to",
        "na", "ni",  "nu",  "ne", "no",
        "ha", "hi",  "fu",  "he", "ho",
        "ma", "mi",  "mu",  "me", "mo",
        "ya", "yu",  "yo",
        "ra", "ri",  "ru",  "re", "ro",
        "wa", "wo"];

    if ($flag = nstre($seed)) srand(crc32($seed));

    $name = "";
    for ($i = 1; $i < $syllables; $i++)
        $name .= $array[array_rand($array)];

    $array[]  = "n";
    $name    .= $array[array_rand($array)];

    if ($flag) srand();

    return ucfirst($name);
};

//--------------------------------------------------------------------------------------------------

function strtocolor($string,
                    $lightness = 75,
                    $span      = TRUE,
                    $diff_min  = 0)
{
    static $hue_prev = NULL;

    $hue = sprintf("%u", djb2($string)) % 360;

    if (isset($hue_prev))
    {
        $diff = $hue - $hue_prev;
        if ($diff > 180)      $diff -= 360;
        elseif ($diff < -180) $diff += 360;

        $abs = abs($diff);
        if ($abs < $diff_min)
        {
            $delta  = $diff_min - $abs;
            $hue   += ($diff >= 0) ? $delta : -$delta;
            $hue    = ($hue + 360) % 360;
        };
    };

    $hue_prev = $hue;
    $color    = sprintf("hsl(%d,%d%%,%d%%)", $hue, $lightness, $lightness);

    return $span ?
           "<span style=\"COLOR:" . x($color) . "\">" .
           x($string) .
           "</span>" :
           $color;
};