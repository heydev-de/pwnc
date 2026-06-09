<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   UTF8 CLASS
//==================================================================================================

define("CMS_UTF8_CHARSET_UTF_8",       "utf-8");
define("CMS_UTF8_CHARSET_ISO_8859_1",  "iso-8859-1");
define("CMS_UTF8_CHARSET_ISO_8859_2",  "iso-8859-2");
define("CMS_UTF8_CHARSET_ISO_8859_3",  "iso-8859-3");
define("CMS_UTF8_CHARSET_ISO_8859_4",  "iso-8859-4");
define("CMS_UTF8_CHARSET_ISO_8859_5",  "iso-8859-5");
define("CMS_UTF8_CHARSET_ISO_8859_6",  "iso-8859-6");
define("CMS_UTF8_CHARSET_ISO_8859_7",  "iso-8859-7");
define("CMS_UTF8_CHARSET_ISO_8859_8",  "iso-8859-8");
define("CMS_UTF8_CHARSET_ISO_8859_9",  "iso-8859-9");
define("CMS_UTF8_CHARSET_ISO_8859_10", "iso-8859-10");
define("CMS_UTF8_CHARSET_ISO_8859_11", "iso-8859-11");
define("CMS_UTF8_CHARSET_ISO_8859_13", "iso-8859-13");
define("CMS_UTF8_CHARSET_ISO_8859_14", "iso-8859-14");
define("CMS_UTF8_CHARSET_ISO_8859_15", "iso-8859-15");
define("CMS_UTF8_CHARSET_ISO_8859_16", "iso-8859-16");

//related functions
//--------------------------------------------------------------------------------------------------

function utf8_convert($value,
                      $charset = CMS_UTF8_CHARSET_ISO_8859_1)
{
    //prefer mb function
    if (function_exists("mb_convert_encoding"))
        return mb_convert_encoding($value, "UTF-8", $charset);

    static $map = NULL;

    if (! isset($map[$charset]))
    {
        $charset = strtolower(trim($charset));

        switch ($charset)
        {
        case "iso-8859-1":  $file = "iso_8859_1.inc";  break;
        case "iso-8859-2":  $file = "iso_8859_2.inc";  break;
        case "iso-8859-3":  $file = "iso_8859_3.inc";  break;
        case "iso-8859-4":  $file = "iso_8859_4.inc";  break;
        case "iso-8859-5":  $file = "iso_8859_5.inc";  break;
        case "iso-8859-6":  $file = "iso_8859_6.inc";  break;
        case "iso-8859-7":  $file = "iso_8859_7.inc";  break;
        case "iso-8859-8":  $file = "iso_8859_8.inc";  break;
        case "iso-8859-9":  $file = "iso_8859_9.inc";  break;
        case "iso-8859-10": $file = "iso_8859_10.inc"; break;
        case "iso-8859-11": $file = "iso_8859_11.inc"; break;
        case "iso-8859-13": $file = "iso_8859_13.inc"; break;
        case "iso-8859-14": $file = "iso_8859_14.inc"; break;
        case "iso-8859-15": $file = "iso_8859_15.inc"; break;
        case "iso-8859-16": $file = "iso_8859_16.inc"; break;
        default: return $value;
        };

        $map[$charset] = require(CMS_SYSTEM_PATH . "utf8/$file");
    };

    return strtr($value, $map[$charset]);
};

//--------------------------------------------------------------------------------------------------

function utf8_detect($value)
{
    //empty string
    if (stre($value)) return TRUE;

    //check byte order mark
    if (strncmp($value, "\xEF\xBB\xBF", 3) === 0) return TRUE;

    //prefer mb function
    if (function_exists("mb_check_encoding"))
        return mb_check_encoding($value, "UTF-8");

    //regex for shorter strings due to backtrack limit
    if (strlen($value) < 1048576)
        return preg_match(
            "/^(?:" .
            "[\\x00-\\x7F]|" .                           //ascii
            "[\\xC2-\\xDF][\\x80-\\xBF]|" .              //non overlong 2 byte character
            "\\xE0[\\xA0-\\xBF][\\x80-\\xBF]|" .         //excluding overlong character
            "[\\xE1-\\xEC\\xEE\\xEF][\\x80-\\xBF]{2}|" . //straight 3 byte character
            "\\xED[\\x80-\\x9F][\\x80-\\xBF]|" .         //excluding surrogate character
            "\\xF0[\\x90-\\xBF][\\x80-\\xBF]{2}|" .      //planes 1 to 3
            "[\\xF1-\\xF3][\\x80-\\xBF]{3}|" .           //planes 4 to 15
            "\\xF4[\\x80-\\x8F][\\x80-\\xBF]{2}" .       //plane 16
            ")*$/",
            $value);

    //bit level check
    $i = 0;
    $c = strlen($value);

    while ($i < $c)
    {
        $byte = ord($value[$i++]);

        //ascii
        if (($byte & 0x80) === 0x00)
        {
            continue;
        }

        //2 byte character
        elseif (($byte & 0xE0) === 0xC0)
        {
            $byte = ord($value[$i++] ?? "");
            if (($byte & 0xC0) !== 0x80) return FALSE;
        }

        //3 byte character
        elseif (($byte & 0xF0) === 0xE0)
        {
            $byte = ord($value[$i++] ?? "");
            if (($byte & 0xC0) !== 0x80) return FALSE;

            $byte = ord($value[$i++] ?? "");
            if (($byte & 0xC0) !== 0x80) return FALSE;
        }

        //4 byte character
        elseif (($byte & 0xF8) === 0xF0)
        {
            $byte = ord($value[$i++] ?? "");
            if (($byte & 0xC0) !== 0x80) return FALSE;

            $byte = ord($value[$i++] ?? "");
            if (($byte & 0xC0) !== 0x80) return FALSE;

            $byte = ord($value[$i++] ?? "");
            if (($byte & 0xC0) !== 0x80) return FALSE;
        }

        //invalid character
        else
        {
            return FALSE;
        };
    };

    return TRUE;
};

//--------------------------------------------------------------------------------------------------

function utf8_chr($value)
{
    //prefer mb function
    if (function_exists("mb_chr")) return mb_chr($value, "UTF-8");

    if ($value < 128)     return chr(  $value);                     //ascii
    if ($value < 2048)    return chr(( $value >>  6)       + 192) . //2 byte character
                                 chr(( $value        & 63) + 128);
    if ($value < 65536)   return chr(( $value >> 12)       + 224) . //3 byte character
                                 chr((($value >>  6) & 63) + 128) .
                                 chr(( $value        & 63) + 128);
    if ($value < 2097152) return chr(( $value >> 18)       + 240) . //4 byte character
                                 chr((($value >> 12) & 63) + 128) .
                                 chr((($value >>  6) & 63) + 128) .
                                 chr(( $value        & 63) + 128);
    return FALSE;                                                   //invalid character
};

//--------------------------------------------------------------------------------------------------

function utf8_ord($value)
{
    //empty string
    if (nstre($value)) return FALSE;

    //prefer mb function
    if (function_exists("mb_ord")) return mb_ord($value, "UTF-8");

    $_value = ord($value[0]);
    if ($_value < 128) return $_value;                      //ascii
    if ($_value < 192) return FALSE;                        //invalid character
    if ($_value < 224) return ($_value        & 31) << 6  | //2 byte character
                              (ord($value[1]) & 63);
    if ($_value < 240) return ($_value        & 15) << 12 | //3 byte character
                              (ord($value[1]) & 63) << 6  |
                              (ord($value[2]) & 63);
    if ($_value < 248) return ($_value        & 15) << 18 | //4 byte character
                              (ord($value[1]) & 63) << 12 |
                              (ord($value[2]) & 63) << 6  |
                              (ord($value[3]) & 63);
    return FALSE;                                           //invalid character
};

//--------------------------------------------------------------------------------------------------

function utf8_substr($value,
                     $offset,
                     $count = NULL)
{
    if ($count === 0) return "";

    //prefer mb function
    if (function_exists("mb_substr")) return mb_substr($value, $offset, $count, "UTF-8");

    $length_byte = strlen($value);
    if ($length_byte === 0) return "";

    $f = function($offset_char,
                  &$length_char = NULL,
                  $offset_byte = 0)
         use ($value,
              $length_byte)
    {
        $length_char = 0;

        while ($offset_byte < $length_byte)
        {
            if ($length_char === $offset_char) return $offset_byte;

            $ord = ord($value[$offset_byte]);
            if     ($ord < 128) $offset_byte += 1;
            elseif ($ord < 224) $offset_byte += 2;
            elseif ($ord < 240) $offset_byte += 3;
            elseif ($ord < 248) $offset_byte += 4;
            else                $offset_byte += 1;

            $length_char++;
        };

        return $offset_byte;
    };

    //determine total number of characters
    $length_char = 0;
    if (($flag = ($offset < 0)) || ($count < 0))
    {
        $f(PHP_INT_MAX, $length_char);
        if ($flag) $offset += $length_char;
    };

    //determine start byte position
    $offset_byte = $f($offset);

    //early return up to end
    if ($count === NULL) return substr($value, $offset_byte);

    //compute end character position
    if ($count < 0)  $count += $length_char - $offset;
    if ($count <= 0) return "";

    //determine end byte position
    $count_byte = $f($count, $null, $offset_byte) - $offset_byte;

    return substr($value, $offset_byte, $count_byte);
};

//--------------------------------------------------------------------------------------------------

function utf8_clean_edges($value)
{
    //mb_strcut is avoided as it prioritizes character integrity
    //over strict byte-limit adherence at the offset

    return preg_replace(
        ["/^[\\x80-\\xBF]+/",
         "/[\\xC0-\\xDF]$" .                    //2 byte character
         "|[\\xE0-\\xEF][\\x80-\\xBF]?$" .      //3 byte character
         "|[\\xF0-\\xF4][\\x80-\\xBF]{0,2}$/"], //4 byte character
        "",
        $value);
};

//--------------------------------------------------------------------------------------------------

function utf8_strcut($value,
                     $offset,
                     $count = NULL)
{
    //mb_strcut is avoided as it prioritizes character integrity
    //over strict byte-limit adherence at the offset

    return utf8_clean_edges(
        ($count === NULL) ?
        substr($value, $offset) : //length not nullable in php < 8.0
        substr($value, $offset, $count));
};

//--------------------------------------------------------------------------------------------------

function utf8_strlen($value)
{
    //prefer mb function
    if (function_exists("mb_strlen")) return mb_strlen($value, "UTF-8");

    return preg_match_all("/./u", $value);
};

//--------------------------------------------------------------------------------------------------

function utf8_strtoupper($value)
{
    //prefer mb function
    if (function_exists("mb_strtoupper")) return mb_strtoupper($value, "UTF-8");

    static $data = NULL;
    if (! $data) $data = require(CMS_SYSTEM_PATH . "utf8/simple_uppercase_mapping.inc");

    return strtr($value, $data);
};

//--------------------------------------------------------------------------------------------------

function utf8_strtolower($value)
{
    //prefer mb function
    if (function_exists("mb_strtolower")) return mb_strtolower($value, "UTF-8");

    static $data = NULL;
    if (! $data) $data = require(CMS_SYSTEM_PATH . "utf8/simple_lowercase_mapping.inc");

    return strtr($value, $data);
};

//--------------------------------------------------------------------------------------------------

function utf8_ucfirst($value)
{
    return utf8_strtoupper(utf8_substr($value, 0, 1)) . utf8_substr($value, 1);
};

//--------------------------------------------------------------------------------------------------

function utf8_ucwords($value)
{
    return preg_replace_callback(

    //not preceded by letter, number or currency
        "/(?<![\p{L}\p{N}\p{Sc}])" .

    //and not last letter
        "(\p{L}(?!" . CMS_REGEX_SEPARATOR . "|$)" .

    //or preceded by math symbol, separator or joint
        "|(?<=^|[\p{Sm}\p{Z}]|" . CMS_REGEX_JOINT . ")\p{L})/u",

        function($value) { return utf8_ucfirst($value[0]); },
        $value);
};

//--------------------------------------------------------------------------------------------------

function utf8_ltrim($value)
{
    return preg_replace("/^[\s\p{Z}\\0\x0B]+/u", "", $value);
};

//--------------------------------------------------------------------------------------------------

function utf8_rtrim($value)
{
    return preg_replace("/[\s\p{Z}\\0\x0B]+$/u", "", $value);
};

//--------------------------------------------------------------------------------------------------

function utf8_trim($value)
{
    return preg_replace("/^[\s\p{Z}\\0\x0B]+|[\s\p{Z}\\0\x0B]+$/u", "", $value);
};

//--------------------------------------------------------------------------------------------------

function utf8_strcasecmp($value1,
                         $value2)
{
    $value1 = utf8_strtolower($value1);
    $value2 = utf8_strtolower($value2);

    return strcmp($value1, $value2);
};

//--------------------------------------------------------------------------------------------------

function utf8_strnatcasecmp($value1,
                            $value2)
{
    $value1 = utf8_strtolower($value1);
    $value2 = utf8_strtolower($value2);

    return strnatcmp($value1, $value2);
};

//--------------------------------------------------------------------------------------------------

function utf8_strspn($value,
                     $mask,
                     $start  = NULL,
                     $length = NULL)
{
    if ($start !== NULL || $length !== NULL)
        $value = utf8_substr($value, $start, $length);

    $mask = preg_quote($mask, "/");
    $mask = str_replace(["-", "^"], ["\\-", "\\^"], $mask);

    return preg_match("/^[" . $mask . "]+/u", $value, $value) ?
           utf8_strlen($value[0]) : 0;
};

//--------------------------------------------------------------------------------------------------

function utf8_wordwrap($value,
                       $length = 75,
                       $break  = "\n",
                       $cut    = FALSE)
{
    $length  = max(1, min((int)$length, 65535));
    $search  = ".{1," . $length . "}(?:[\s\p{Z}]|$)|" .          //before separator or at end
               ".{1," . $length . "}[\p{Pd}\p{Pe}\p{Pf}\p{Po}]"; //after punctuation
    $search .= $cut ? "|.{" . $length . "}" : "|.+";             //hard break / no break

    preg_match_all("/$search/u", $value, $value);
    return implode($break, $value[0]);
};

//--------------------------------------------------------------------------------------------------

function utf8_normalize($value,
                        $compose = TRUE)
{
    //ascii
    if (! preg_match("/[^\\x00-\\x7F]/", $value)) return $value;

    //prefer normalizer class
    if (class_exists("\Normalizer"))
    {
        $result = \Normalizer::normalize($value, $compose ? \Normalizer::FORM_C : \Normalizer::FORM_D);
        if ($result !== FALSE) return $result;
    };

    static $data = NULL;
    if (! $data) $data = [0 => require(CMS_SYSTEM_PATH . "utf8/nfd_quick_check.inc"),
                          1 => require(CMS_SYSTEM_PATH . "utf8/nfc_quick_check.inc")];

    $result = "";
    $buffer = "";
    $class  = 0;
    $_class = 0;
    $i      = 0;
    $_i     = -1;
    $c      = strlen($value);

    while ($i < $c)
    {
        $char = utf8::get_character($value, $i);
        if ($char === FALSE) continue; //invalid

        if (isset($data[$compose ? 1 : 0][$char])) //nfc quick check not passed
        {
            if ((! $compose) && (($_char = utf8::decompose($char)) !== FALSE)) $char = $_char;

            $buffer .= $char;
        }
        else //nfc quick check passed
        {
            $class   = utf8::get_canonical_combining_class($char);

            if ($class === 0) //starter
            {
                if ($buffer !== "")
                {
                    if ($_i > 0)
                    {
                        $buffer = utf8_substr($result, $_i) . $buffer;
                        $result = utf8_substr($result, 0, $_i);
                    }
                    else
                    {
                        $buffer = $result . $buffer;
                        $result = "";
                    };

                    if ($compose) //compose
                    {
                        $buffer  = utf8_normalize($buffer, FALSE);
                        $result .= utf8::compose($buffer);
                    }
                    else //decompose
                    {
                        $result .= utf8::sort($buffer);
                    };

                    $buffer = "";
                    $_i     = -1;
                };

                if ($_i === -1) $_i = max(utf8_strlen($result), 0);
                $result .= $char;
            }
            elseif (($buffer === "") && ($class >= $_class)) //canonically ordered
            {
                $result .= $char;
            }
            else
            {
                $buffer .= $char;
            };

            $_class = $class;
        };
    };

    if ($buffer !== "")
    {
        if ($_i > 0)
        {
            $buffer = utf8_substr($result, $_i) . $buffer;
            $result = utf8_substr($result, 0, $_i);
        }
        else
        {
            $buffer = $result . $buffer;
            $result = "";
        };

        if ($compose) //compose
        {
            $buffer  = utf8_normalize($buffer, FALSE);
            $result .= utf8::compose($buffer);
        }
        else //decompose
        {
            $result .= utf8::sort($buffer);
        };
    };

    return $result;
};

class utf8
{

//methods
//--------------------------------------------------------------------------------------------------

    static function build()
    {
        //composition exclusions
        if ($hfile_in = fopen(CMS_PATH . "unicode/CompositionExclusions.txt", "rb"))
        {
            $exclusion = [];

            while ($value = utf8::ucd_get_record($hfile_in))
            {
                $value = utf8::ucd_extract_code_point($value[0]);

                foreach ($value AS $_value)
                    $exclusion[$_value] = TRUE;
            };

            fclose($hfile_in);
        };

        //unicode data
        if (! $hfile_in = fopen(CMS_PATH . "unicode/UnicodeData.txt", "rb")) return FALSE;

        //canonical combining class
        $hfile_out1 = fopen(CMS_SYSTEM_PATH . "utf8/canonical_combining_class.inc", "wb");
        fwrite($hfile_out1, "<?php\nreturn [\n");

        //canonical decomposition mapping
        $hfile_out2 = fopen(CMS_SYSTEM_PATH . "utf8/canonical_decomposition_mapping.inc", "wb");
        fwrite($hfile_out2, "<?php\nreturn [\n");

        //canonical composition mapping
        $hfile_out3 = fopen(CMS_SYSTEM_PATH . "utf8/canonical_composition_mapping.inc", "wb");
        fwrite($hfile_out3, "<?php\nreturn [\n");

        //simple uppercase mapping
        $hfile_out4 = fopen(CMS_SYSTEM_PATH . "utf8/simple_uppercase_mapping.inc", "wb");
        fwrite($hfile_out4, "<?php\nreturn [\n");

        //simple lowercase mapping
        $hfile_out5 = fopen(CMS_SYSTEM_PATH . "utf8/simple_lowercase_mapping.inc", "wb");
        fwrite($hfile_out5, "<?php\nreturn [\n");

        $flag_cc    = FALSE;
        $flag_dm    = FALSE;
        $flag_cm    = FALSE;
        $flag_uc    = FALSE;
        $flag_lc    = FALSE;

        while ($value = utf8::ucd_get_record($hfile_in))
        {
            $code                      = utf8::ucd_extract_code_point($value[0]);
            $code                      = reset($code);
            //$name                      = $value[1];
            //$general_category          = $value[2];
            $canonical_combining_class = $value[3];
            //$bidi_class                = $value[4];
            $decomposition_mapping     = $value[5];
            //$decimal_digit_value       = $value[6];
            //$digit_value               = $value[7];
            //$numeric_value             = $value[8];
            //$bidi_mirrored             = $value[9];
            //$unicode_1_name            = $value[10];
            //$iso_comment               = $value[11];
            $simple_uppercase_mapping  = $value[12];
            $simple_lowercase_mapping  = $value[13];
            //$simple_titlecase_mapping  = $value[14];

            //canonical combining class
            if ($canonical_combining_class)
            {
                $buffer = "'$code' => $canonical_combining_class";
                if ($flag_cc) $buffer  = ",\n$buffer";
                else          $flag_cc = TRUE;
                fwrite($hfile_out1, $buffer);
            };

            //canonical decomposition mapping / canonical composition mapping
            if ($decomposition_mapping && ($decomposition_mapping[0] !== "<"))
            {
                $decomposition_mapping = implode("", utf8::ucd_extract_code_point($decomposition_mapping));

                //canonical decomposition mapping
                $buffer = "'$code' => '$decomposition_mapping'";
                if ($flag_dm) $buffer  = ",\n$buffer";
                else          $flag_dm = TRUE;
                fwrite($hfile_out2, $buffer);

                //canonical composition mapping
                if (! isset($exclusion[$code]))
                {
                    $buffer = "'$decomposition_mapping' => '$code'";
                    if ($flag_cm) $buffer  = ",\n$buffer";
                    else          $flag_cm = TRUE;
                    fwrite($hfile_out3, $buffer);
                };
            };

            //simple uppercase mapping
            if ($simple_uppercase_mapping)
            {
                $simple_uppercase_mapping = utf8::ucd_extract_code_point($simple_uppercase_mapping);
                $simple_uppercase_mapping = reset($simple_uppercase_mapping);

                $buffer = "'$code' => '$simple_uppercase_mapping'";
                if ($flag_uc) $buffer  = ",\n$buffer";
                else          $flag_uc = TRUE;
                fwrite($hfile_out4, $buffer);
            };

            //simple lowercase mapping
            if ($simple_lowercase_mapping)
            {
                $simple_lowercase_mapping = utf8::ucd_extract_code_point($simple_lowercase_mapping);
                $simple_lowercase_mapping = reset($simple_lowercase_mapping);

                $buffer = "'$code' => '$simple_lowercase_mapping'";
                if ($flag_lc) $buffer  = ",\n$buffer";
                else          $flag_lc = TRUE;
                fwrite($hfile_out5, $buffer);
            };
        };

        fclose($hfile_in);

        //canonical combining class
        fwrite($hfile_out1, "];");
        fclose($hfile_out1);

        //simple uppercase mapping
        fwrite($hfile_out4, "];");
        fclose($hfile_out4);

        //simple lowercase mapping
        fwrite($hfile_out5, "];");
        fclose($hfile_out5);

//..................................................................................................

        //hangul
        $sbase  = 0xAC00;
        $lbase  = 0x1100;
        $vbase  = 0x1161;
        $tbase  = 0x11A7;
        $lcount = 19;
        $vcount = 21;
        $tcount = 28;
        $ncount = 588;   //vcount * tcount;
        $scount = 11172; //lcount * ncount;

        for ($sindex = 0; $sindex < $scount; $sindex++)
        {
            $l       = $lbase +  $sindex / $ncount;
            $v       = $vbase + ($sindex % $ncount) / $tcount;
            $t       = $tbase +  $sindex % $tcount;

            $code    = utf8_chr($sindex + $sbase);
            $buffer  = utf8_chr($l) . utf8_chr($v);
            if ($t !== $tbase) $buffer .= utf8_chr($t);

            //hangul decomposition mapping
            $_buffer = "'$code' => '$buffer'";
            if ($flag_dm) $_buffer = ",\n$_buffer";
            else          $flag_dm = TRUE;
            fwrite($hfile_out2, $_buffer);

            //hangul composition mapping
            if (! isset($exclusion[$code]))
            {
                $_buffer = "'$buffer' => '$code'";
                if ($flag_cm) $_buffer = ",\n$_buffer";
                else          $flag_cm = TRUE;
                fwrite($hfile_out3, $_buffer);
            };
        };

        //hangul decomposition mapping
        fwrite($hfile_out2, "];");
        fclose($hfile_out2);

        //hangul composition mapping
        fwrite($hfile_out3, "];");
        fclose($hfile_out3);

//..................................................................................................

        //derived normalization props
        if (! $hfile_in = fopen(CMS_PATH . "unicode/DerivedNormalizationProps.txt", "rb")) return FALSE;

        //nfd quick check
        $hfile_out1 = fopen(CMS_SYSTEM_PATH . "utf8/nfd_quick_check.inc", "wb");
        fwrite($hfile_out1, "<?php\nreturn [\n");

        //nfc quick check
        $hfile_out2 = fopen(CMS_SYSTEM_PATH . "utf8/nfc_quick_check.inc", "wb");
        fwrite($hfile_out2, "<?php\nreturn [\n");

        $flag_qc    = FALSE;
        $flag_qd    = FALSE;

        while ($value = utf8::ucd_get_record($hfile_in))
        {
            if (! isset($value[1])) continue;

            if ($value[1] === "NFD_QC")
            {
                $code = utf8::ucd_extract_code_point($value[0]);

                foreach ($code AS $_code)
                {
                    $buffer = "'$_code' => '" . $value[2] . "'";
                    if ($flag_qc) $buffer  = ",\n$buffer";
                    else          $flag_qc = TRUE;
                    fwrite($hfile_out1, $buffer);
                };
            }
            elseif ($value[1] === "NFC_QC")
            {
                $code = utf8::ucd_extract_code_point($value[0]);

                foreach ($code AS $_code)
                {
                    $buffer = "'$_code' => '" . $value[2] . "'";
                    if ($flag_qd) $buffer  = ",\n$buffer";
                    else          $flag_qd = TRUE;
                    fwrite($hfile_out2, $buffer);
                };
            };
        };

        fclose($hfile_in);

        //nfd quick check
        fwrite($hfile_out1, "];");
        fclose($hfile_out1);

        //nfc quick check
        fwrite($hfile_out2, "];");
        fclose($hfile_out2);

//..................................................................................................

        //iso mapping
        $array = ["8859-1.TXT"  => "iso_8859_1.inc",  "8859-2.TXT"  => "iso_8859_2.inc",
                  "8859-3.TXT"  => "iso_8859_3.inc",  "8859-4.TXT"  => "iso_8859_4.inc",
                  "8859-5.TXT"  => "iso_8859_5.inc",  "8859-6.TXT"  => "iso_8859_6.inc",
                  "8859-7.TXT"  => "iso_8859_7.inc",  "8859-8.TXT"  => "iso_8859_8.inc",
                  "8859-9.TXT"  => "iso_8859_9.inc",  "8859-10.TXT" => "iso_8859_10.inc",
                  "8859-11.TXT" => "iso_8859_11.inc",
                  "8859-13.TXT" => "iso_8859_13.inc", "8859-14.TXT" => "iso_8859_14.inc",
                  "8859-15.TXT" => "iso_8859_15.inc", "8859-16.TXT" => "iso_8859_16.inc"];

        foreach ($array AS $key => $value)
        {
            if ($hfile_in = fopen(CMS_PATH . "unicode/$key", "rb"))
            {
                if ($hfile_out = fopen(CMS_SYSTEM_PATH . "utf8/$value", "wb"))
                {
                    fwrite($hfile_out, "<?php\nreturn [\n");

                    $flag = FALSE;

                    while (! feof($hfile_in))
                    {
                        $value = fgets($hfile_in, 1024);

                        //strip comment
                        if (($i = strpos($value, "#")) !== FALSE)
                            $value = substr($value, 0, $i);

                        //get columns
                        $value = explode("\t", $value);
                        $value = array_map("trim", $value);

                        //line not empty
                        if (isset($value[0]) && isset($value[1]))
                        {
                            $value[0] = hexdec($value[0]);

                            if ($value[0] > 127)
                            {
                                $value[0] =      chr(       $value[0] );
                                $value[1] = utf8_chr(hexdec($value[1]));
                                $buffer   = "'" . $value[0] . "' => '" . $value[1] . "'";
                                if ($flag) $buffer = ",\n$buffer";
                                else       $flag   = TRUE;
                                fwrite($hfile_out, $buffer);
                            };
                        };
                    };

                    fwrite($hfile_out, "];");
                    fclose($hfile_out);
                };

                fclose($hfile_in);
            };
        };

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    static function test($limit = 20000)
    {
        //normalization test
        if (! $hfile = fopen(CMS_PATH . "unicode/NormalizationTest.txt", "rb")) return FALSE;

        $line   = 0;
        $error1 = 0;
        $error2 = 0;

        while (
            (($limit === 0) || ($line++ < $limit))
            &&
            ($value = utf8::ucd_get_record($hfile))
        )
        {
            if ($value[0][0] === "@")
            {
                echo($value[0] . "\n");
                continue;
            };

            $nfc     = [];
            $_error1 = $error1;

            for ($i = 0; $i <= 4; $i++)
            {
                $value[$i] = implode("", utf8::ucd_extract_code_point($value[$i]));
                $nfc[$i]   = utf8_normalize($value[$i]);
            };

            if ($value[1] !== $nfc[0]) echo("error " . (++$error1) . " on line $line: c2 '" . $value[1] . "' != NFC(c1) '" . $nfc[0] . "'\n");
            if ($value[1] !== $nfc[1]) echo("error " . (++$error1) . " on line $line: c2 '" . $value[1] . "' != NFC(c2) '" . $nfc[1] . "'\n");
            if ($value[1] !== $nfc[2]) echo("error " . (++$error1) . " on line $line: c2 '" . $value[1] . "' != NFC(c3) '" . $nfc[2] . "'\n");
            if ($value[3] !== $nfc[3]) echo("error " . (++$error1) . " on line $line: c4 '" . $value[3] . "' != NFC(c4) '" . $nfc[3] . "'\n");
            if ($value[3] !== $nfc[4]) echo("error " . (++$error1) . " on line $line: c4 '" . $value[3] . "' != NFC(c5) '" . $nfc[4] . "'\n");

            if ($error1 !== $_error1) $error2++;
        };

        fclose($hfile);

        echo("$error1 error(s) on $error2 line(s)");
        return (bool)$error1;
    }

//--------------------------------------------------------------------------------------------------

    static function get_canonical_combining_class($value)
    {
        static $data = NULL;
        if (! $data) $data = require(CMS_SYSTEM_PATH . "utf8/canonical_combining_class.inc");

        return isset($data[$value]) ? $data[$value] : 0;
    }

//--------------------------------------------------------------------------------------------------

    static function decompose(&$value)
    {
        utf8::_decompose($value, $result);
        return $result;
    }

//--------------------------------------------------------------------------------------------------

    static function _decompose($value,
                               &$result)
    {
        $i = 0;
        $c = strlen($value);

        while ($i < $c)
        {
            $char    = utf8::get_character($value, $i);
            if ($char === FALSE) continue; //invalid

            $_char   = utf8::__decompose($char);
            $result .= ($_char === FALSE) ? $char : utf8::_decompose($_char, $result);
        };
    }

//--------------------------------------------------------------------------------------------------

    static function __decompose($value)
    {
        static $data = NULL;
        if (! $data) $data = require(CMS_SYSTEM_PATH . "utf8/canonical_decomposition_mapping.inc");

        return isset($data[$value]) ? $data[$value] : FALSE;
    }

//--------------------------------------------------------------------------------------------------

    static function sort($value)
    {
        $result = "";
        $array  = [];
        $count  = 0;
        $i      = 0;
        $c      = strlen($value);

        while ($i < $c)
        {
            $char = utf8::get_character($value, $i);

            if ($char !== FALSE) //valid
            {
                $class = utf8::get_canonical_combining_class($char);

                if ($class === 0) //starter
                {
                    ksort($array);
                    $result .= implode("", $array) . $char;
                    $array   = [];
                    $count   = 0;
                }
                else //non-starter
                {
                    $array[$class * 1000 + $count++] = $char;
                };
            };
        };

        ksort($array);
        return $result . implode("", $array);
    }

//--------------------------------------------------------------------------------------------------

    static function compose($value)
    {
        $result     = "";
        $array      = [];
        $i          = 0;
        $c          = strlen($value);
        if ($c === 0)      return;

        $starter    = utf8::get_character($value, $i);
        $nonstarter = "";
        $_class     = utf8::get_canonical_combining_class($starter);
        if ($_class !== 0) $_class = 255;

        while ($i < $c)
        {
            $char  = utf8::get_character($value, $i);
            if ($char === FALSE) continue; //invalid

            $class = utf8::get_canonical_combining_class($char);

            if (($class === 0) && ($_class === 0)) //starter sequence
            {
                if (($composite = utf8::_compose($starter . $char)) !== FALSE) //hangul lv
                {
                    //prefetch character
                    $_i    = $i;
                    $_char = utf8::get_character($value, $i);

                    if (($_composite = utf8::_compose($starter . $char . $_char)) !== FALSE) //hangul lvt
                    {
                        $starter = $_composite;
                    }
                    else
                    {
                        $starter = $composite;
                        $i       = $_i; //reset position
                    };
                }
                else
                {
                    $result     .= $starter;
                    $starter     = $char;
                    $nonstarter  = "";
                    $_class      = 0;
                };
            }
            elseif (
                     (($_class === 0) || ($class > $_class)) //not blocked
                     &&
                     (($composite = utf8::_compose($starter . $char)) !== FALSE)
                   )
            {
                $starter     = $composite;
            }
            else
            {
                $nonstarter .= $char;
                $_class      = $class;
            };
        };

        return $result . $starter . $nonstarter;
    }

//--------------------------------------------------------------------------------------------------

    static function _compose($value)
    {
        static $data = NULL;
        if (! $data) $data = require(CMS_SYSTEM_PATH . "utf8/canonical_composition_mapping.inc");

        return isset($data[$value]) ? $data[$value] : FALSE;
    }

//--------------------------------------------------------------------------------------------------

    static function get_character(&$value,
                                  &$offset)
    {
        if (preg_match(
            "/(?:" .
            "[\\x00-\\x7F]|" .                           //ascii character
            "[\\xC2-\\xDF][\\x80-\\xBF]|" .              //non overlong 2 byte character
            "\\xE0[\\xA0-\\xBF][\\x80-\\xBF]|" .         //excluding overlong character
            "[\\xE1-\\xEC\\xEE\\xEF][\\x80-\\xBF]{2}|" . //straight 3 byte character
            "\\xED[\\x80-\\x9F][\\x80-\\xBF]|" .         //excluding surrogate character
            "\\xF0[\\x90-\\xBF][\\x80-\\xBF]{2}|" .      //planes 1 to 3
            "[\\xF1-\\xF3][\\x80-\\xBF]{3}|" .           //planes 4 to 15
            "\\xF4[\\x80-\\x8F][\\x80-\\xBF]{2}|" .      //plane 16
            "(.)" .                                      //invalid character
            ")/S",
            $value,
            $result,
            NULL,
            $offset))
        {
            $offset += strlen($result[0]);
            return isset($result[1]) ? FALSE : $result[0];
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    static function ucd_get_record($hfile)
    {
        if (! $hfile) return FALSE;

        while (! feof($hfile))
        {
            $value = fgets($hfile, 1024);

            //strip comment
            if (($i = strpos($value, "#")) !== FALSE)
                $value = substr($value, 0, $i);

            //get columns
            $value = explode(";", $value);
            $value = array_map("trim", $value);

            //line not empty
            if (reset($value) !== "") return $value;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    static function ucd_extract_code_point($value)
    {
        $result = [];
        $value  = explode(" ", $value);

        foreach ($value AS $_value)
        {
            $__value = explode("..", $_value);

            //code point range
            if (count($__value) === 2)
            {
                $i = hexdec($__value[0]);
                $c = hexdec($__value[1]);

                while ($i <= $c) $result[] = utf8_chr($i++);
            }

            //code point
            else
            {
                $result[] = utf8_chr(hexdec($_value));
            };
        };

        return $result;
    }
};