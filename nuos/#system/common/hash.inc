<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

function fingerprint($string,
                     $guarantee_threshold = 8,
                     $noise_threshold     = 5)
{
    //hash algorithm constants
    $table = 65537;
    $base  = 257;

    //adjust parameters
    if ($noise_threshold > $guarantee_threshold)
        $noise_threshold = $guarantee_threshold;

    $window_size = $guarantee_threshold - $noise_threshold + 1;
    $kgram_size  = $noise_threshold;

    $c  = strlen($string);
    if ($c < $kgram_size)  $kgram_size  = $c;
    $c -= $kgram_size;
    if ($c < $window_size) $window_size = $c;

    //compute constant base ^ (kgram_size - 1)
    $const1 = 1;
    for ($i = 1; $i < $kgram_size; $i++)
        $const1 = ($const1 * $base) % $table;

    //compute constant base * table
    $const2 = $base * $table;

    //compute initial hash
    $hash = ord($string[0]);
    for ($i = 1; $i < $kgram_size; $i++)
           $hash = ($hash * $base + ord($string[$i])) % $table;

    //text too short for winnowing => return initial hash
    if ($window_size === 0)
        return [0 => $hash];

    //fill initial window
    $window = [0 => $hash];
    for ($i = 0; $i < $window_size; $i++)
    {
        $hash = ($hash - ord($string[$i]) * $const1 + $const2) % $table;
        $hash = ($hash * $base + ord($string[$i + $kgram_size])) % $table;

        $window[] = $hash;
    };

    $pos    = $window_size - 1;
    $min    = $pos;
    $result = [];

    while (TRUE)
    {
        //next window
        if ($min === $pos)
        {
            //scan back for rightmost minimum in previous window
            $_pos = $pos;

            do
            {
                if (--$_pos < 0)                    $_pos += $window_size;
                if ($window[$_pos] < $window[$min]) $min   = $_pos;
            }
            while ($_pos != $pos);

            $result[] = $window[$min];
        }

        //current window
        else
        {
            if ($window[$pos] < $window[$min])
            {
                $min      = $pos;
                $result[] = $window[$min];
            };
        };

        if ($i >= $c)                break;
        if (++$pos === $window_size) $pos = 0;

        //compute next hash
        $hash = ($hash - ord($string[$i]) * $const1 + $const2) % $table;
        $hash = ($hash * $base + ord($string[$i + $kgram_size])) % $table;

        $window[$pos] = $hash;

        $i++;
    };

    return $result;
};

//--------------------------------------------------------------------------------------------------

function hmac_md5($string,
                  $key)
{
    if (strlen($key) > 64) $key = pack("H*", md5($key));
    $key  = str_pad($key, 64, "\0");

    $ipad = $key ^ str_repeat("\x36", 64);
    $opad = $key ^ str_repeat("\x5C", 64);

    return md5($opad . pack("H*", md5($ipad . $string)));
};

//--------------------------------------------------------------------------------------------------

function simhash($string)
{
    //initialze histogram
    $histogram = array_fill(0, 64, 0);

    //traverse string
    for ($i = 0, $c = strlen($string); $i < $c; $i++)
    {
        //generate shingle hash
        $hash  = md5(substr($string, $i, 16));
        $hash1 = hexdec(substr($hash, 0, 8));
        $hash2 = hexdec(substr($hash, 8, 8));
        $hash3 = hexdec(substr($hash, 16, 8));
        $hash4 = hexdec(substr($hash, 24, 8));

        //traverse hash bits
        for ($_i = 0; $_i < 32; $_i++)
        {
            $bitmask              = 1 << $_i;
            $histogram[$_i]      += (($hash1 & $bitmask) === 0) ? -1 : 1;
            $histogram[$_i + 32] += (($hash2 & $bitmask) === 0) ? -1 : 1;
            $histogram[$_i]      += (($hash3 & $bitmask) === 0) ? -1 : 1;
            $histogram[$_i + 32] += (($hash4 & $bitmask) === 0) ? -1 : 1;
        };
    };

    //generate result hash
    $hash = [];
    for ($i = 0; $i < 64; $i++) $hash[] = ($histogram[$i] > 0) ? "1" : "0";

    return implode("", $hash);
};

//--------------------------------------------------------------------------------------------------

function crc32_base62($string)
{
    return base62(sprintf("%u", crc32($string)));
};

//--------------------------------------------------------------------------------------------------

function hash32($string,
                $binary = FALSE)
{
    return hash("ripemd128", $string, $binary);
};

//--------------------------------------------------------------------------------------------------

function hash64($string,
                $binary = FALSE)
{
    return hash("sha256", $string, $binary);
};

//--------------------------------------------------------------------------------------------------

function djb2($string)
{
    $hash = 5381;

    for ($i = 0, $c = strlen($string); $i < $c; $i++)
        $hash = ((($hash << 5) & 0xFFFFFFFF) + $hash + ord($string[$i])) & 0xFFFFFFFF;

    return $hash;
};