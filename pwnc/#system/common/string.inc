<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

function replace_placeholder($string,
                             $replacement,
                             $encoding_function = __NAMESPACE__ . "\\x")
{
    if (! is_array($replacement)) $replacement = [$replacement];

    //no encoding
    if (! is_callable($encoding_function)) $encoding_function = NULL;

    foreach ($replacement AS $key => $value)
    {
        $key    = preg_quote($key, "/");
        $string = nstre($value) ?
            preg_replace_callback(
                "/(\\[([^\\[\\]]*))?%$key%(([^\\[\\]]*)\\])?/",
                function($match) use ($value, $encoding_function)
                {
                    return ($match[2] ?? "") .
                           (($encoding_function === NULL) ? $value : $encoding_function($value)) .
                           ($match[4] ?? "");
                },
                $string) :
            preg_replace(
                "/(?:\\[[^\\[\\]]*)?%$key%(?:[^\\[\\]]*\\])?/",
                "", $string);
    };

    return $string;
};

//--------------------------------------------------------------------------------------------------

function strtoalphanum($string,
                       $replacement = " ")
{
    $string = htmlentities_decode((string)$string);
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
    $string = (string)$string;
    $search = "/(-?[0-9" . preg_quote(t("thousand_separator") . t("decimal_separator"), "/") . "]+)/iu";
    if (! preg_match($search, $string, $string)) return FALSE;

    $string = $string[0];
    $string = str_replace(t("thousand_separator"), "", $string);
    $string = str_replace(t("decimal_separator"), ".", $string);

    return floatval($string);
};

//--------------------------------------------------------------------------------------------------

function stripspaces($string,
                     $preserve_newlines = FALSE,
                     $limit_empty_lines = TRUE)
{
    $string = (string)$string;

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
    $string = str_replace("\r\n", "\n", (string)$string);
    $string = preg_replace("/(?<=\n)\n/", "<br class=\"multiple\">", $string);

    return str_replace("\n", "<br>", $string);
};

//--------------------------------------------------------------------------------------------------

function limitstr($string,
                  $length    = 255,
                  $separator = ",")
{
    $part   = explode($separator, (string)$string);
    $return = "";

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
    $string = (string)$string;
    return preg_match("/^\V+/u", $string, $_string) ? $_string[0] : $string;
};

//--------------------------------------------------------------------------------------------------

function first_words($string,
                     $length   = 250,
                     $ellipsis = " …")
{
    $string = (string)$string;
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
    $string = (string)$string;
    $length = (int)$length - strlen($string);
    return $string . ($length > 0 ? str_repeat("\0", $length) : "");
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
    return preg_match("/^" . preg_quote((string)$string1, "/") . "$/iu", (string)$string2);
};

//--------------------------------------------------------------------------------------------------

function substri_count($source,
                       $string)
{
    return preg_match_all("/" . preg_quote((string)$string, "/") . "/iu", $source, $array);
};

//--------------------------------------------------------------------------------------------------

function verify_email($email)
{
    $regex =
        "/^" .

    //local identifier
        "(" .

    //quoted
            "\"" .
            "(?:" .
                "[^\\x00\\x0A\\x0D\"\\\\]" .
                "|" .
                "\\\\[^\\x00]" . //escaping
            ")+" .
            "\"" .

            "|" .

    //unquoted
            "(?:" .
                "(?<!^|\.)\." . //dot not following dot
                "|" .
                "[^\\x00-\\x1F \"(),.:;<>@\[\\\\\]\\x7F]" .
            ")+" .

            "(?<!\.)" . //may not end on dot
        ")" .

        "@" .

        "(?:" .

    //ip
            "\[(?:(IPv6:)([0-9a-f.:]+)|([0-9.]+))\]" .

            "|" .

    //domain
            "(" .
                "(?:" .
                    "[^\\x00-\\x1F !\"#$%&'()*+,\-.\/:;<=>?@[\\\\\]^_`{|}~\\x7F]" .
                    "(?:" .
                        "[^\\x00-\\x1F !\"#$%&'()*+,.\/:;<=>?@[\\\\\]^_`{|}~\\x7F]*" .
                        "[^\\x00-\\x1F !\"#$%&'()*+,\-.\/:;<=>?@[\\\\\]^_`{|}~\\x7F]" .
                    ")?" .

                    "\." .
                ")+" .

    //tld
                "[^\\x00-\\x1F !\"#$%&'()*+,\-.\/0-9:;<=>?@[\\\\\]^_`{|}~\\x7F]" .
                "[^\\x00-\\x1F !\"#$%&'()*+,.\/:;<=>?@[\\\\\]^_`{|}~\\x7F]*" .
                "[^\\x00-\\x1F !\"#$%&'()*+,\-.\/0-9:;<=>?@[\\\\\]^_`{|}~\\x7F]" .
            ")" .
        ")" .

        "$/iDu";

    if (! preg_match($regex, (string)$email, $match)) return FALSE;

    //check local identifier length
    $length = strlen($match[1]);
    if ($length > 64) return FALSE;

    $total = $length + 1; //identifier@

    //check ip
    $ipv6 = nstre($match[2]);
    $ip   = $ipv6 ? $match[3] : $match[4];
    if (nstre($ip))
    {
        if ((strpos($ip, ":") === FALSE) === $ipv6) return FALSE;
        if (! verify_ip($ip))                       return FALSE;

        $length = strlen($ip) + ($ipv6 ? 7 : 2);
        if ($length > 255) return FALSE;

        $total += $length;
        return $total <= 254;
    };

    //check domain
    foreach (explode(".", $match[5]) AS $segment)
    {
        $segment = punycode($segment);
        if (substr($segment, 2, 3) === "---") return FALSE;

        $length  = strlen($segment);
        if ($length > 63) return FALSE;

        $total += $length + 1;
    };

    return $total < 255;
};

//--------------------------------------------------------------------------------------------------

function verify_ip($ip)
{
    //implementation of abnf from rfc 3986
    $dec_octet   = "(?:\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])";
    $ipv4address = "(?:$dec_octet\.){3}$dec_octet";
    $h16         = "[0-9a-f]{1,4}";
    $ls32        = "(?:$h16:$h16|$ipv4address)";
    $ipv6address = "(?:" .
                        "(?:" .
                            "(?:$h16:){6}" .
                            "|" .
                            "::(?:$h16:){5}" .
                            "|" .
                            "(?:$h16)?::(?:$h16:){4}" .
                            "|" .
                            "(?:(?:$h16:){0,1}$h16)?::(?:$h16:){3}" .
                            "|" .
                            "(?:(?:$h16:){0,2}$h16)?::(?:$h16:){2}" .
                            "|" .
                            "(?:(?:$h16:){0,3}$h16)?::$h16:" .
                            "|" .
                            "(?:(?:$h16:){0,4}$h16)?::" .
                        ")" .
                        "$ls32" .
                        "|" .
                        "(?:" .
                            "(?:(?:$h16:){0,5}$h16)?::$h16" .
                            "|" .
                            "(?:($h16:){0,6}$h16)?::" .
                        ")" .
                        "|" .
                        "$ipv4address" . //additional ipv4 support
                    ")";

    return (bool)preg_match("/^$ipv6address$/iD", (string)$ip);
};

//--------------------------------------------------------------------------------------------------

function unique_id($count = 8)
{
    static $string = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    $return = "";

    for ($i = 0; $i < $count; $i++)
        $return .= $string[random_int(0, 61)];

    return $return;
};

//--------------------------------------------------------------------------------------------------

function reference_code($count        = 6,
                        $chunk_length = 3)
{
    static $string = "346789ABCDEFGHJKLMNPQRTUVWXY";

    $return = "";

    for ($i = 0; $i < $count; $i++)
    {
        if (($chunk_length > 0) && ($i !== 0) && (($i % $chunk_length) === 0)) $return .= "-";
        $return .= $string[mt_rand(0, 27)];
    };

    return $return;
};

//--------------------------------------------------------------------------------------------------

function strabridge($string,
                    $length  = 50,
                    $cut_end = FALSE)
{
    $string  = (string)$string;
    $_length = utf8_strlen($string);
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

    $string = (string)$string;
    $hue    = sprintf("%u", djb2($string)) % 360;

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