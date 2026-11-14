<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

define("IMAGETYPE_SVG", 100);

// temporary compatibility function
if (version_compare(PHP_VERSION, "8.0.0", ">="))
{
    function imagedestroy(...$args) {};
};

function image($url,
               $width         = NULL,
               $height        = NULL,
               $alt           = NULL,
               $attribute     = NULL, //(array)          -> attribute list
                                      //["" => (string)] -> pass through
                                      //(string)         -> style attribute
                                      //TRUE             -> return data
               $defer         = FALSE,
               $preview       = TRUE) //TRUE             -> generate preview
                                      //FALSE            -> no preview
                                      //NULL             -> no processing
{
    $output = "";              //noscript fallback
    if ($defer) $_output = ""; //javascript deferred loading

    //return data flag
    if ($return = $attribute === TRUE) $attribute = [];

    $process = $preview !== NULL;

    //source image data
    $url_main    = $url;
    $path_source = $size = $width_source = $height_source = NULL;

    //use no_image.svg on error
    $error = function() use (&$url, &$alt, &$path_source, &$size, &$width_source, &$height_source)
    {
        $url         = CMS_IMAGES_URL . "no_image.svg";
        $alt         = "";
        $path_source = image_path($url);
        $size        = getimagesize($path_source) ?: [300, 250];
        list($width_source, $height_source) = $size;
    };

//..................................................................................................

    //process short format

    static $cache = [];

    if (! isset($cache[$url]) && (image_processor::cache_path($url) === FALSE)) //invalid url
    {
        $cache[$url] = FALSE;

        //look for local image in supported format
        foreach (["svg", "png"] AS $ext)
        {
            if (is_file(CMS_IMAGES_PATH . "$url.$ext"))
            {
                $cache[$url] = $ext;
                break;
            };
        };
    };

    if (isset($cache[$url]))
    {
        if ($cache[$url] !== FALSE)
        {
            $path          = $url . "." . $cache[$url];
            $path_source   = CMS_IMAGES_PATH . $path;
            $url_main      = CMS_IMAGES_URL . $path;
            $alt         ??= "";
            $process       = FALSE;
        }
        else
        {
            $error(); //no valid image
        };
    };

//..................................................................................................

    //fetch remote file and cache locally

    //local path, needed for the size even when nothing is processed
    $path_source ??= image_path($url);

    if ($process && ($path_source === FALSE)) //remote
    {
        if (

        //generate cache file path
             (($path_source = image_processor::cache_path($url)) === FALSE)
             ||

        //synchronize remote file
             (image_processor::cache_remote($url, $path_source) === FALSE)
           )
            $error(); //invalid url or fetch error
    };

//..................................................................................................

    //get image size
    if (($size = getimagesize($path_source)) === FALSE) $error();
    list($width_source, $height_source) = $size;

    //get contained size
    $resolution = image_processor::resolution($width_source, $height_source, $width, $height);
    list($width_main, $height_main) = reset($resolution);

//..................................................................................................

    //process alternative resolutions

    if ($process)
    {
        //resolution list
        $resolution = [$width_main];

        //create alternative resolution list
        if (! isset($attribute["srcset"])) //srcset attrib overrides
        {
            $_resolution  = [160, 320, 480, 768, 1024, 1280, 1440, 1920, 2560, 3840, 7680];
            $width_retina = $width_main * 2;

            foreach ($_resolution AS $value)
            {
                if ($width_retina < $value) break;                         //exceeds target size
                if ($width_source < $value) break;                         //original too small
                if (abs($width_main - $value) < ($value * 0.05)) continue; //already in range
                $resolution[] = $value;
            };
        };

        //process resolutions
        $list = image_process($url, $resolution, NULL, NULL, FALSE, $preview);

        //resolve main url
        if ($multiple = is_array($list))
        {
            $url_main = $url; //fallback
            foreach ($list AS $_url => $_width)
                if ($_width === $width_main) { $url_main = $_url; break; };
        }
        else
        {
            $url_main = $list;
        };
    };

//..................................................................................................

    //generate output

    //inline style
    if (is_string($attribute)) $attribute = ["style" => $attribute];

    //source url
    $output              .= " src=\"" .            x($url_main) . "\"";
    if ($defer) $_output .= " src=\"" .            x("data:image/svg+xml," .
                                                     r("<svg xmlns=\"http://www.w3.org/2000/svg\" " .
                                                            "width=\"" . x($width_main) . "\" " .
                                                            "height=\"" . x($height_main) . "\">" .
                                                       "</svg>")) . "\"" .
                            " data-defer-src=\"" . x($url_main) . "\"";

    unset($attribute["src"]); //remove from attribute list

    //alternative resolution sources
    if ($process && $multiple && ! isset($attribute["srcset"])) //srcset attrib overrides
    {
        $srcset = "";
        foreach ($list AS $_url => $_width) $srcset .= x($_url . " " . $_width . "w,");
        $srcset = substr($srcset, 0, -1);

        $output              .= " srcset=\"" . x($srcset) . "\"" .
                                " sizes=\"" . x($attribute["sizes"] ?? "100vw") . "\"";
        if ($defer) $_output .= " data-defer-srcset=\"" . x($srcset) . "\"";

        //remove from attribute list
        unset($attribute["srcset"]);
        unset($attribute["sizes"]);
    };

    //image size
    $attribute["width"]  ??= $width_main;
    $attribute["height"] ??= $height_main;

    //text alternative
    $attribute["alt"] ??= $alt ?? file_name($path_source);

    //return data
    if ($return)
        return ["url"    => $url_main,
                "path"   => $path_source,
                "width"  => $attribute["width"],
                "height" => $attribute["height"],
                "alt"    => $attribute["alt"]];

    //output attributes
    $attrib = "";
    foreach ($attribute AS $key => $value)
    {
        if ($value === NULL)     continue;
        if (stre($key . $value)) continue;
        if (stre($value))        $attrib .= " $key";
        elseif (stre($key))      $attrib .= $value;
        else                     $attrib .= " $key=\"" . x($value) . "\"";
    };

    //return image string
    return $defer ?
           "<noscript>" .
           "<img$output$attrib>" .
           "</noscript>" .
           "<img$_output$attrib>" :
           "<img$output$attrib>";
};

//--------------------------------------------------------------------------------------------------

function image_process($url,
                       $width        = NULL,
                       $height       = NULL,
                       $pref_type    = NULL,
                       $ignore_cache = FALSE,
                       $preview      = TRUE)
{
    static $format = ["webp", "jpg", "png", "gif", "svg"];

    $single = ! (is_array($width) || is_array($height));

    //verify url
    $cache_path = image_processor::cache_path($url);
    if ($cache_path === FALSE) return $url;

    //create cache directory
    if (! mkpath(dirname($cache_path))) return $url;

    //source path
    $path_source = image_path($url);

    //cache remote source locally
    if ($path_source === FALSE)
    {
        $result = image_processor::cache_remote($url, $cache_path);
        if ($result === FALSE) return $url;          //error
        if ($result === TRUE)  $ignore_cache = TRUE; //updated
        $path_source = $cache_path;
    };

    //no processing
    if (in_array(file_extension($path_source), ["gif", "svg"])) return $url;

    //daemon mode
    $preview = $preview && (new system())->getval("image", "daemon");

    //load image data
    if (

    //reading failed or file corrupt
         (($data = getimagesize($path_source)) === FALSE)
         ||

    //animation not supported
         $data["anim"]
       )
        return $url;

    list($_width, $_height, $type) = $data;
    $alpha = $data["alpha"];

    //get list of target resolutions
    $resolution = image_processor::resolution($_width, $_height, $width, $height);

    //determine output format
    $type_target = image_processor::determine_format($pref_type, $alpha);
    if ($type_target === FALSE) return $url; //no supported target format
    $ext_target = image_processor::$type_to_ext[$type_target];

//..................................................................................................

    //chained progressive downscale
    $image_source = FALSE;
    $mtime_source = filemtime($path_source);
    $preview_path = NULL;
    $return       = [];
    $daemon       = FALSE;

    foreach ($resolution AS $size)
    {
        list($_width, $_height) = $size;

        //set file name
        $path_target = image_processor::cache_path($url, $_width, $_height, $ext_target);
        $url_target  = image_processor::cache_url($path_target);

        //skip existing image
        if (($preview || ! $ignore_cache) && is_file($path_target))
        {
            $mtime = filemtime($path_target);
            $flag  = $preview && ($mtime === 1);

            if ($flag || ($mtime >= $mtime_source))
            {
                $return[$url_target] = $_width;
                $daemon |= $flag;
                continue;
            };
        };

        //preview
        if ($preview)
        {
            //preview size override
            $size = [0       => $_width,
                     1       => $_height,
                     2       => $type,
                     3       => "width=\"$_width\" height=\"$_height\"",
                     "mime"  => get_mime_type($ext_target),
                     "alpha" => $alpha,
                     "anim"  => FALSE];
            cms_cache("image.size.$path_target", [1 => $size], TRUE);

            //first iteration
            if ($preview_path === NULL)
            {
                $width_original = $_width;
                list($_width, $_height) = image_processor::contain($_width, $_height, 50, 50);
            }

            //2nd+ iteration
            else
            {
                copy($preview_path, $path_target);
                touch($path_target, 1);
                $return[$url_target] = $_width;
                $daemon = TRUE;
                continue;
            };
        };

        //load source image
        if ($image_source === FALSE)
        {
            $image_source = image_processor::load($path_source, $type);
            if ($image_source === FALSE) { $daemon = FALSE; break; }; //loading failed

            //temporary file suffix
            $temp = unique_id();
        };

        set_time_limit(600); //10 minutes

        //create target image
        $image_target = imagecreatetruecolor($_width, $_height);

        //transparency
        if ($alpha)
        {
            imagesavealpha($image_target, TRUE);
            imagealphablending($image_target, FALSE);
            $transparent = imagecolorallocatealpha($image_target, 0, 0, 0, 127);
            imagefilledrectangle($image_target, 0, 0, $_width - 1, $_height - 1, $transparent);
        };

        //rescale
        imagecopyresampled(
            $image_target, $image_source,
            0, 0, 0, 0,
            $_width, $_height,
            imagesx($image_source), imagesy($image_source));

        //encode
        $path_temp = "$path_target.$temp";

        switch ($type_target)
        {
        case IMAGETYPE_WEBP:

            $quality = image_processor::compute_quality($_width, $_height, $type_target);
            imagewebp($image_target, $path_temp, $quality);
            break;

        case IMAGETYPE_JPEG:

            $quality = image_processor::compute_quality($_width, $_height, $type_target);
            imageinterlace($image_target, TRUE);
            imagejpeg($image_target, $path_temp, $quality);
            break;

        default:

            imagepng($image_target, $path_temp);
            break;
        };

        if (is_file($path_temp)) //success
        {
            //swap temp into live file
            if (! rename($path_temp, $path_target))
            {
                //failed
                unlink($path_temp); //delete temp file
            }

            //preview processing
            elseif ($preview)
            {
                //add to return list using original width
                $return[$url_target] = $width_original;

                //set scaled image as preview
                $preview_path = $path_target;

                //make stale to override cache strategy
                touch($path_target, 1);

                //activate daemon
                $daemon = TRUE;
            }

            //final processing
            else
            {
                //add to return list
                $return[$url_target] = $_width;
            };
        };

        imagedestroy($image_source);

        //use scaled image as new source
        $image_source = $image_target;
    };

    if ($image_source !== FALSE) imagedestroy($image_source);

    //create daemon task
    if ($daemon)
    {
        $buffer = sprintf("image_process(%s, %s, %s, %s, FALSE, FALSE);",
                          var_export($url, TRUE),
                          var_export($width, TRUE),
                          var_export($height, TRUE),
                          var_export($pref_type, TRUE));
        $id = "image." . $url . "." . hash32(serialize([$width, $height, $pref_type]));
        cms_daemon($buffer, $id, 0, "Image processing: " . rawurldecode($url));
    };

    return $single ? (array_key_first($return) ?? $url) : $return;
};

//--------------------------------------------------------------------------------------------------

function image_data($url)
{
    return image($url, NULL, NULL, NULL, TRUE, FALSE, NULL);
};

//--------------------------------------------------------------------------------------------------

function image_exists($value)
{
    foreach (["svg", "png"] AS $ext)
        if (is_file(CMS_IMAGES_PATH . "$value.$ext")) return TRUE;

    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function image_path($url)
{
    static $cache    = [];
    static $len_url  = NULL;
    static $len_path = NULL;

    //use cache
    if (isset($cache[$url])) return $cache[$url];

    $len_url  ??= strlen(CMS_ROOT_URL);
    $len_path ??= strlen(CMS_ROOT_PATH);

    //url starts with local root
    if (! strncmp($url, CMS_ROOT_URL, $len_url))
    {
        $path = rawurldecode(substr($url, $len_url));
        $path = resolve_path(CMS_ROOT_PATH, $path);
        if (strncmp($path, CMS_ROOT_PATH, $len_path)) return FALSE; //outside root
        if (is_file($path)) return $cache[$url] = $path;
    };

    return $cache[$url] = FALSE;
};

//--------------------------------------------------------------------------------------------------

function getimagesize($file,
                      &$imageinfo = NULL)
{
    //get local path if applicable
    if (($temp = image_path($file)) !== FALSE) $file = $temp;

    //cache key
    $cache_key = "image.size.$file";

    //delete cache
    if (! is_file($file))
    {
        cms_cache_delete($cache_key, TRUE);
        return FALSE;
    };

    //retrieve cache
    $cache = cms_cache($cache_key);
    $time  = filemtime($file) ?: 0;
    if (isset($cache) && (($cache[0] ?? 0) >= $time)) return $cache[1] ?? FALSE;

    //cache function
    $cache = function($value, $alpha_anim = NULL) use ($cache_key, $time)
    {
        if ($alpha_anim === NULL) //svg
        {
            $value[0]      = (int)round($value[0]);
            $value[1]      = (int)round($value[1]);
            $value[2]      = IMAGETYPE_SVG;
            $value[3]      = "width=\"" .  $value[0] . "\" " .
                             "height=\"" . $value[1] . "\"";
            $value["mime"] = "image/svg+xml";
            $alpha_anim    = [FALSE, FALSE];
        };

        $value["alpha"] = $alpha_anim[0];
        $value["anim"]  = $alpha_anim[1];

        cms_cache($cache_key, [$time, $value], TRUE);
        return $value;
    };

    //svg
    if (streq(file_extension($file), "svg"))
    {
        //should be available
        if (! extension_loaded("SimpleXML")) return $cache(FALSE); //unavailable

        //open
        $xml = simplexml_load_file($file);
        if ($xml === FALSE) return $cache(FALSE); //unreadable

        //get size
        $width  = isset($xml["width"])  ? dimension_to_px($xml["width"])  : 0;
        $height = isset($xml["height"]) ? dimension_to_px($xml["height"]) : 0;

        if ($width && $height) return $cache([$width, $height]);

        //viewbox
        if (
             isset($xml["viewBox"])
             &&
             ($value = preg_split("/[\s,]+/", $xml["viewBox"], 4, PREG_SPLIT_NO_EMPTY))
             &&
             (count($value) === 4)
           )
        {
            //viewbox size
            $_width  = (float)$value[2];
            $_height = (float)$value[3];
            if (($_width <= 0) || ($_height <= 0)) return $cache(FALSE); //negative size

            //fit to viewbox
            if ($width)  return $cache([$width, $width  / $_width  * $_height]);
            if ($height) return $cache([$height / $_height * $_width, $height]);

            return $cache([$_width, $_height]);
        };

        return $cache(FALSE); //invalid size
    };

    //read size and type
    $size = \getimagesize($file, $imageinfo);
    if ($size === FALSE) return $cache(FALSE); //error

    //read alpha and anim settings
    $alpha_anim = image_processor::alpha_anim($file, $size[2]) ?: [FALSE, FALSE];

    return $cache($size, $alpha_anim);
};

class image_processor
{

//properties
//--------------------------------------------------------------------------------------------------

    static $width_max   = NULL;
    static $height_max  = NULL;
    static $pref_type   = NULL;
    static $format      =
        [IMAGETYPE_WEBP => "WebP Support",
         IMAGETYPE_JPEG => "JPEG Support",
         IMAGETYPE_PNG  => "PNG Support"];
    static $ext_to_type =
        ["webp"         => IMAGETYPE_WEBP,
         "jpg"          => IMAGETYPE_JPEG,
         "png"          => IMAGETYPE_PNG,
         "gif"          => IMAGETYPE_GIF,
         "svg"          => IMAGETYPE_SVG];
    static $type_to_ext =
        [IMAGETYPE_WEBP => "webp",
         IMAGETYPE_JPEG => "jpg",
         IMAGETYPE_PNG  => "png",
         IMAGETYPE_GIF  => "gif",
         IMAGETYPE_SVG  => "svg"];

//methods
//--------------------------------------------------------------------------------------------------

    static function resolution($source_width,
                               $source_height,
                               $target_width  = NULL,
                               $target_height = NULL)
    {
        static $cache = [];

        $cache_key = serialize([$target_width, $target_height]);
        if (isset($cache[$source_width][$source_height][$cache_key]))
            return $cache[$source_width][$source_height][$cache_key];

        //determine maximum dimensions
        if (self::$width_max === NULL)
        {
            $max = (new system())->getval("image", "resolution") ?? "";

            //system configurating
            if (preg_match("/^(\d+)x(\d+)$/", $max, $_max))
            {
                self::$width_max  = (int)$_max[1];
                self::$height_max = (int)$_max[2];
            }

            //fallback
            else
            {
                self::$width_max  = 1920;
                self::$height_max = 0;
            };
        };

        //source dimensions
        $_width  = max((int)$source_width,  1);
        $_height = max((int)$source_height, 1);

        //build target resolution list
        $list    = [];
        $w_array = is_array($target_width);
        $h_array = is_array($target_height);
        $w_int   = is_int($target_width);
        $h_int   = is_int($target_height);

        if ($w_array || $h_array)
        {
            $c = max($w_array ? count($target_width)  : 1,
                     $h_array ? count($target_height) : 1);

            for ($i = 0; $i < $c; $i++)
            {
                $list[] = [$w_array ? ($target_width[$i]  ?? 0) : ($w_int ? $target_width  : 0),
                           $h_array ? ($target_height[$i] ?? 0) : ($h_int ? $target_height : 0)];
            };
        }
        else
        {
            $list[] = [(int)$target_width, (int)$target_height];
        };

        $resolution = [];

        //apply containment size
        foreach ($list AS $size)
        {
            list($width, $height) = $size;

            //complement using aspect ratio
            if (($width + $height) === 0) { $width  = $_width;
                                            $height = $_height; }
            elseif ($width  === 0)          $width  = (int)(($height * $_width  + $_height - 1) / $_height);
            elseif ($height === 0)          $height = (int)(($width  * $_height + $_width  - 1) / $_width);

            //constrain size
            if (self::$width_max)  $width  = min($width,  self::$width_max);
            if (self::$height_max) $height = min($height, self::$height_max);

            //contain image keeping aspect ratio
            if (($width * $_height) < ($height * $_width))
            {
                $width  = min($width, $_width);
                $height = (int)(($width * $_height + $_width - 1) / $_width);
            }
            else
            {
                $height = min($height, $_height);
                $width  = (int)(($height * $_width + $_height - 1) / $_height);
            };

            $resolution[$width * $height] = [$width, $height];
        };

        //sort in downscale order
        krsort($resolution);

        return $cache[$source_width][$source_height][$cache_key] = $resolution;
    }

//--------------------------------------------------------------------------------------------------

    static function alpha_anim($path,
                               $type)
    {
        switch ($type)
        {
        case IMAGETYPE_PNG:

            $png_type = read_file($path, 1, 25);
            if (stre($png_type)) break;
            return [in_array(ord($png_type), [4, 6], TRUE), FALSE];

        case IMAGETYPE_WEBP:

            $header = read_file($path, 21);
            if (stre($header)) break;

            $header = unpack("a4riff/L1filesize/a4webp/a4format/L1chunksize/C1chunk", $header);
            if ($header === FALSE) break;

            switch ($header["format"])
            {
            case "VP8 ": $alpha = FALSE; $anim = FALSE; break;
            case "VP8L": $alpha = TRUE;  $anim = FALSE; break;
            case "VP8X": $alpha = flag($header["chunk"], 0b00010000);
                         $anim  = flag($header["chunk"], 0b00000010);
                         break;
            default:     break 2; //invalid format
            };

            return [$alpha, $anim];
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    static function load($path,
                         $type)
    {
        set_time_limit(600); //10 minutes

        $image = FALSE;

        //suppress warnings on corrupted images
        $error = set_error_handler(function() { return TRUE; });

        switch ($type)
        {
        case IMAGETYPE_JPEG:
//..................................................................................................

            $image = imagecreatefromjpeg($path);
            break;

        case IMAGETYPE_PNG:
//..................................................................................................

            $image = imagecreatefrompng($path);
            break;

        case IMAGETYPE_WEBP:
//..................................................................................................

            $image = imagecreatefromwebp($path);
            break;
        };

        //reinstate error handling
        set_error_handler($error);

        return $image;
    }

//--------------------------------------------------------------------------------------------------

    static function determine_format($pref_type = NULL,
                                     $alpha     = FALSE)
    {
        static $cache = [];
        static $check = FALSE;

        //preferred output format
        $pref_type ??= self::$pref_type;
        if ($pref_type === NULL)
        {
            $ext       = (new system())->getval("image", "preference") ?? "";
            $pref_type = self::$pref_type = (self::$ext_to_type[$ext] ?? IMAGETYPE_WEBP);
        };

        if (isset($cache[$pref_type][$alpha])) return $cache[$pref_type][$alpha];

        //get supported formats
        if ($check)
        {
            $format = self::$format;
        }

        //detect supported formats
        else
        {
            $check  = TRUE;
            $format = [];
            $info   = extension_loaded("gd") ? gd_info() : [];

            foreach (self::$format AS $key => $value)
            {
                if ($info[$value] ?? FALSE) $format[$key] = TRUE;
            };

            self::$format = $format;
        };

        //put preference at beginning
        if (isset($format[$pref_type])) $format = [$pref_type => TRUE] + $format;

        if ($alpha) //transparency
        {
            //remove non-alpha formats
            $format = array_intersect_key(
                $format, [IMAGETYPE_WEBP => TRUE, IMAGETYPE_PNG => TRUE]);

            //fallback
            if (! $format && (self::$format[IMAGETYPE_WEBP] ?? FALSE))
                $format[IMAGETYPE_WEBP] = TRUE;
        };

        return $cache[$pref_type][$alpha] = (array_key_first($format) ?? FALSE);
    }

//--------------------------------------------------------------------------------------------------

    static function compute_quality($width,
                                    $height,
                                    $type)
    {
        static $cache = [];

        if (isset($cache[$width][$height][$type])) return $cache[$width][$height][$type];

        //quality
        static $quality = [IMAGETYPE_WEBP => ["min" => 90, "mid" => 90, "max" => 95],
                           IMAGETYPE_JPEG => ["min" => 85, "mid" => 85, "max" => 90]];

        //resolution
        static $rmin = 5760; //sqrt(7680 * 4320)
        static $rmid = 1440; //sqrt(1920 * 1080)
        static $rmax =  240; //sqrt( 320 *  180)

        //quality points for image format
        $q    = $quality[$type];
        $qmin = $q["min"];
        $qmid = $q["mid"];
        $qmax = $q["max"];

        //image diagonal
        $diagonal = sqrt($width * $height);
        $diagonal = max($rmax, min($rmin, $diagonal)); //clamp

        //normalize t
        $t    = ($diagonal - $rmax) / ($rmin - $rmax);
        $tmid = ($rmid     - $rmax) / ($rmin - $rmax);

        //control point
        $d = 2 * (1 - $tmid) * $tmid; //denominator
        $c = ($d === 0.0) ? $qmid : ($qmid - pow(1 - $tmid, 2) * $qmax - pow($tmid, 2) * $qmin) / $d;

        //quadratic bezier interpolation
        $u     = 1 - $t; //complementary parameter
        $value = $u * $u * $qmax + 2 * $u * $t * $c + $t * $t * $qmin;

        return $cache[$width][$height][$type] = (int)round($value);
    }

//--------------------------------------------------------------------------------------------------

    static function cache_path($url,
                               $width  = NULL,
                               $height = NULL,
                               $ext    = NULL)
    {
        static $cache = [];

        $width  = (string)$width;
        $height = (string)$height;

        //use cache
        if (isset($cache[$url][$width][$height]))
            return $cache[$url][$width][$height];

        $data = analyze_url($url);

        if (

        //invalid url
             ($data === FALSE)
             ||

        //unsupported protocol
             ! preg_match("/^(?:https?|ftp)$/", $data["scheme"] ?? "")
             ||

        //unsupported format
             ! preg_match("/^(?:webp|jpe?g|png|gif|svg)$/i", $data["extension"] ?? "")
           )
            return $cache[$url][$width][$height] = FALSE;

        $hash       = hash32($url);
        $dir        = substr($hash, 0, 4);
        $hash       = substr($hash, 4, 8);
        $filename   = rawurldecode($data["filename"]);
        $filename   = stringtofilename($filename);
        $size       = nstre($width . $height) ? "-" . $width . "x" . $height : "";
        $ext      ??= ($data["extension"] === "jpeg") ? "jpg" : $data["extension"];
        $ext        = nstre($ext) ? ".$ext" : "";
        $path       = CMS_DATA_PATH . "image/cache/$dir/$filename$size-$hash$ext";

        return $cache[$url][$width][$height] = $path;
    }

//--------------------------------------------------------------------------------------------------

    static function cache_url($path)
    {
        static $length = NULL;
        $length ??= strlen(CMS_DATA_PATH);

        if (strncmp($path, CMS_DATA_PATH, $length)) return FALSE;
        return CMS_DATA_URL . cms_path_urlencode(substr($path, $length));
    }

//--------------------------------------------------------------------------------------------------

    static function cache_remote($url,
                                 $path,
                                 $ttl = 600) //10 minutes
    {
        $cache_key = "image.time.$url";
        $time_now  = time();

        if (

        //image is cached
             ($flag = is_file($path))
             &&

        //cooldown check
             ((int)cms_cache($cache_key) >= ($time_now - $ttl))
           )
            return NULL; //still within cooldown time

        //activate new cooldown
        cms_cache($cache_key, $time_now, TRUE);

        if ($flag) //cached
        {
            if (
                 cms_load("http")
                 &&
                 (($header = http_header($url)) !== FALSE)
                 &&
                 ($header["#status"] < 500)
               )
            {
                //source not available
                if (in_array((int)($header["#status"] ?? 0),
                             [401,  //unauthorized
                              403,  //forbidden
                              404,  //not found
                              410], //gone
                             TRUE))
                {
                    //remove cache
                    unlink($path);
                    return FALSE;
                };

                //source modified
                if (
                     (strtotime($header["last-modified"] ?? "") > filemtime($path))
                     &&
                     retrieve_file($url, $path) //update cache
                   )
                    return TRUE;
            };

            return NULL; //keep existing
        }

        //(re-)fill cache
        elseif (mkpath(dirname($path)) && retrieve_file($url, $path))
        {
            return TRUE; //cached
        };

        return FALSE; //error
    }

//--------------------------------------------------------------------------------------------------

    static function contain($width,
                            $height,
                            $width_max,
                            $height_max)
    {
        if (($width * $height_max) > ($height * $width_max))
        {
            $_width = $width;
            $width  = min($width, $width_max);
            $height = (int)(($width * $height + $_width - 1) / $_width);
        }
        else
        {
            $_height = $height;
            $height  = min($height, $height_max);
            $width   = (int)(($height * $width + $_height - 1) / $_height);
        };

        return [$width, $height];
    }
};