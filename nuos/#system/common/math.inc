<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

function diffpercent($value1,
                     $value2)
{
    if (! $value1) return FALSE;

    $difference = abs(100 - (100 / $value1 * $value2));
    if ($value2 < $value1) $difference *= -1;

    return number_format($difference, 1, CMS_L_DECIMAL_SEPARATOR, NULL) . "%";
};

//--------------------------------------------------------------------------------------------------

function format_number($value)
{
    $count = fmod($value, 1) ? 2 : 0;
    return number_format($value, $count, CMS_L_DECIMAL_SEPARATOR, CMS_L_THOUSAND_SEPARATOR);
};

//--------------------------------------------------------------------------------------------------

function format_bytesize($value)
{
    if ($value < 1024)
        return "$value Byte";

    if ($value < 1048576)
        return number_format($value / 1024, 2, CMS_L_DECIMAL_SEPARATOR, NULL) . " KB";

    if ($value < 1073741824)
        return number_format($value / 1048576, 2, CMS_L_DECIMAL_SEPARATOR, NULL) . " MB";

    return number_format($value / 1073741824, 2, CMS_L_DECIMAL_SEPARATOR, NULL) . " GB";
};

//--------------------------------------------------------------------------------------------------

function flag($bitmask,
              $flag)
{
    return (bool)($bitmask & $flag);
};

//--------------------------------------------------------------------------------------------------

function sgn($value)
{
    if($value > 0) return  1;
    if($value < 0) return -1;
    return 0;
};

//--------------------------------------------------------------------------------------------------

function dimension_to_px($value)
{
    static $unit =
        ["cap"  => 16,
         "ch"   => 16 / 2,
         "cm"   => 16 * 6 / 2.54,
         "em"   => 16,
         "ex"   => 16 / 2,
         "ic"   => 16,
         "in"   => 16 * 6,
         "lh"   => 16 * 1.2,
         "mm"   => 16 * 6 / 2.54 / 10,
         "pc"   => 16,
         "pt"   => 16 * 6 / 72,
         "px"   => 1,
         "Q"    => 16 * 6 / 2.54 / 40,
         "rcap" => 16,
         "rch"  => 16 / 2,
         "rem"  => 16,
         "rex"  => 16 / 2,
         "ric"  => 16,
         "rlh"  => 16 * 1.2];

    if (! preg_match("/^(\d*(?:\.\d*)?)([a-z]*)$/i", $value, $_value)) return FALSE;

    if (stre($_value[1]))         return FALSE;
    if (stre($_value[2]))         return (int)round($_value[1]);
    if (isset($unit[$_value[2]])) return (int)round((float)$_value[1] * $unit[$_value[2]]);

    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function base62($value)
{
    $char   = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    $result = "";

    while ($value > 0)
    {
        $temp   = $value % 62;
        $result = $char[$temp] . $result;
        $value  = (int)($value / 62);
    };

    return $result;
};

//--------------------------------------------------------------------------------------------------

function hamming_distance($string1,
                          $string2)
{
    $c = max(strlen($string1), strlen($string2));
    $n = 0;

    for ($i = 0; $i < $c; $i++)
        $n += (($string1[$i] ?? "") !== ($string2[$i] ?? ""));

    return $n;
};