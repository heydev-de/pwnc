<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

function image($url,
               $width   = NULL,
               $height  = NULL,
               $alt     = NULL,
               $style   = NULL,
               $defer   = FALSE,
               $preview = TRUE)
{
    $return              = ""; //noscript fallback
    if ($defer) $_return = ""; //javascript deferred loading

    //prepare image (scaling, caching, ...)
    $_url = image_process($url, $width, $height, NULL, FALSE, $preview);
    list($_width, $_height) = getimagesize($_url); //processed size

    //adapt aspect ratio if processing failed
    if (($width > 0) && ($width < $_width))
    {
        $_height = (int)(($width * $_height + $_width - 1) / $_width);
        $_width  = $width;
    };

    if (($height > 0) && ($height < $_height))
    {
        $_width  = (int)(($height * $_width + $_height - 1) / $_height);
        $_height = $height;
    };

    $return              .= " src=\"" .            x($_url) .    "\"";
    if ($defer) $_return .= " src=\"" .            x("data:image/svg+xml," .
                                                     r("<svg xmlns=\"http://www.w3.org/2000/svg\" " .
                                                            "width=\"" . x($_width) . "\" " .
                                                            "height=\"" . x($_height) . "\">" .
                                                       "</svg>")) . "\"" .
                            " data-defer-src=\"" . x($_url) .    "\"";
    $return              .= " width=\"" .          x($_width) .  "\"";
    if ($defer) $_return .= " width=\"" .          x($_width) .  "\"";
    $return              .= " height=\"" .         x($_height) . "\"";
    if ($defer) $_return .= " height=\"" .         x($_height) . "\"";

    //generate alternative resolution sources
    $__url = analyze_url($url);
    if (
         ($__url !== FALSE)
         &&
         preg_match("/^(?:https?|ftp)$/", $__url["scheme"])
         &&
         preg_match("/^(?:jpe?g|png|webp)$/i", $__url["extension"])
       )
    {
        $resolution    = [160, 320, 480, 640, 960, 1280, 1600, 1920, 3840, 7680]; //resolution list
        list($__width) = getimagesize($url);                                      //original size
        $srcset        = $_url . " " . $_width . "w,";
        $width_retina  = $_width * 2;

        foreach ($resolution AS $value)
        {
            if ($width_retina < $value) break; //exceeds target size
            if ($__width < $value)      break; //original is too small
            if (
                 ($_width !== FALSE)
                 &&
                 (abs($_width - $value) < ($value * 0.05))
               )
                continue; //already in range

            //prepare alternative resolution image (scaling, caching, ...)
            $__url   = image_process($url, $value, NULL, NULL, FALSE, $preview);
            $srcset .= x($__url . " " . $value . "w,");
        };

        $return              .= " srcset=\"" . x(substr($srcset, 0, -1)) . "\"" .
                                " sizes=\"100vw\"";
        if ($defer) $_return .= " data-defer-srcset=\"" . x(substr($srcset, 0, -1)) . "\"";
    };

    $return                      .= " alt=\"" .   x($alt) .   "\"";
    if ($defer) $_return         .= " alt=\"" .   x($alt) .   "\"";

    if (nstre($style))
    {
        $return              .= " style=\"" . x($style) . "\"";
        if ($defer) $_return .= " style=\"" . x($style) . "\"";
    };

    return $defer ?
           "<noscript>" .
           "<img$return>" .
           "</noscript>" .
           "<img$_return>" :
           "<img$return>";
};

//--------------------------------------------------------------------------------------------------

function image_exists($value)
{
    return is_file(CMS_IMAGES_PATH . "$value.webp") ||
           is_file(CMS_IMAGES_PATH . "$value.jpg")  ||
           is_file(CMS_IMAGES_PATH . "$value.png")  ||
           is_file(CMS_IMAGES_PATH . "$value.gif")  ||
           is_file(CMS_IMAGES_PATH . "$value.svg");
};

//--------------------------------------------------------------------------------------------------

function image_quality($width,
                       $height,
                       $extension)
{
    //quality
    static $quality = ["webp" => ["min" => 90, "mid" => 90, "max" => 95],
                       "jpg"  => ["min" => 85, "mid" => 85, "max" => 90]];

    //resolution
    static $rmin = 5760; //sqrt(7680 * 4320)
    static $rmid = 1440; //sqrt(1920 * 1080)
    static $rmax =  240; //sqrt( 320 *  180)

    //quality points for image format
    $q    = $quality[$extension];
    $qmin = $q["min"];
    $qmid = $q["mid"];
    $qmax = $q["max"];

    //image diagonal
    $diagonal = sqrt($width * $height);
    $diagonal = max($rmax, min($rmin, $diagonal)); //clamp

    //normalize t
    $t    = ($diagonal - $rmax) / ($rmin - $rmax);
    $tmid = ($rmid - $rmax) / ($rmin - $rmax);

    //control point
    $d = 2 * (1 - $tmid) * $tmid; //denominator
    $c = ($d == 0) ? $qmid : ($qmid - pow(1 - $tmid, 2) * $qmax - pow($tmid, 2) * $qmin) / $d;

    //quadratic bezier interpolation
    $u     = 1 - $t; //complementary parameter
    $value = $u * $u * $qmax + 2 * $u * $t * $c + $t * $t * $qmin;

    return (int)round($value);
};

//--------------------------------------------------------------------------------------------------

function image_process($url,
                       $width               = NULL,
                       $height              = NULL,
                       $preference_override = NULL,
                       $ignore_cache        = FALSE,
                       $preview             = TRUE)
{
    $system = new system();
    $_url   = $url;

    //preferred format
    $preference = $preference_override ?? $system->getval("image", "preference");
    if (stre($preference)) $preference = "webp";

    $format = ["webp", "jpg", "png", "gif", "svg"]; //supported image formats in order of priority
    $format = array_diff($format, [$preference]);   //remove preference
    array_unshift($format, $preference);            //put preference at beginning (highest priority)

    //verify url
    $array = analyze_url($url);
    if (
         ($array === FALSE)
         ||
         (! preg_match("/^(?:https?|ftp)$/", $array["scheme"]))
         ||
         (! preg_match("/^(?:gif|jpe?g|png|svg|webp)$/i", $array["extension"]))
       )
    {
        //check for local image
        foreach ($format AS $extension)
        {
            if (is_file(CMS_IMAGES_PATH . "$url.$extension"))
                return CMS_IMAGES_URL . "$url.$extension";
        };

        return $url;
    };

    //image extension
    $extension = $array["extension"];
    if (streq($extension, "jpeg")) $extension = "jpg";

    //check for local source file
    $count = strlen(CMS_ROOT_URL);
    $local = FALSE;

    if (streq(substr($url, 0, $count), CMS_ROOT_URL))
    {
        $path_source = substr($url, $count);
        $path_source = CMS_ROOT_PATH . rawurldecode($path_source); //source file path
        $local       = is_file($path_source);
    };

    //generate directory and file name
    $hash     = hash32($url);
    $dir      = substr($hash, 0, 4);
    $hash     = substr($hash, 4, 8);
    $filename = rawurldecode($array["filename"]);
    $filename = stringtofilename($filename);

    //create cache directory
    if (! mkpath(CMS_DATA_PATH . "image/cache/$dir/")) return $url;

    //retrieve external source
    if (! $local)
    {
        $_filename   = "$filename-$hash.$extension";
        $path_source = CMS_DATA_PATH . "image/cache/$dir/$_filename";

        //local copy exists
        if (is_file($path_source))
        {
            $key   = "image.time.$url";
            $time  = (int)cms_cache($key);
            $_time = time();

            if ($time < ($_time - 3600)) //once per hour
            {
                cms_cache($key, $_time, TRUE);

                if (
                     cms_load("http")
                     &&
                     (($array = http_header($url)) !== FALSE)
                     &&
                     (strtotime($array["last-modified"] ?? "") > filemtime($path_source)) //expired
                     &&
                     retrieve_file($url, $path_source)
                   )
                    $ignore_cache = TRUE;
            };
        }

        //create local copy
        elseif (retrieve_file($url, $path_source))
        {
            $ignore_cache = TRUE;
        }

        //fail
        else
        {
            return $url;
        };

        //local copy location
        $_url = CMS_DATA_URL . "image/cache/" . r($dir) . "/" . r($_filename);
    };

    //no processing
    switch ($extension)
    {
    case "gif":
    case "svg":

        return $_url;
    };

    //image dimensions
    $width  = max((int)$width,  0);
    $height = max((int)$height, 0);

    //get image size limit
    $resolution = $system->getval("image", "resolution");
    if (preg_match("/^(\d+)x(\d+)$/", $resolution, $_resolution))
    {
        $width_max  = (int)$_resolution[1];
        $height_max = (int)$_resolution[2];
    }
    else //default
    {
        $width_max  = 1920;
        $height_max = 1080;
    };

    //get original image size
    if (($size = \getimagesize($path_source)) === FALSE) return $_url;
    list($_width, $_height, $type) = $size;

    //set original size
    if (($width + $height) === 0) { $width  = $_width;
                                    $height = $_height; }

    //complement using aspect ratio
    elseif ($width === 0)           $width  = (int)(($height * $_width  + $_height - 1) / $_height);
    elseif ($height === 0)          $height = (int)(($width  * $_height + $_width  - 1) / $_width);

    //constrain size
    $width  = min($width,  $width_max);
    //$height = min($height, $height_max);

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

    //set final file name
    $filename .= "-$width" . "x$height-$hash";

    //seek cached image
    $path_cache = CMS_DATA_PATH . "image/cache/$dir/$filename";

    if (! $ignore_cache)
    {
        $time  = filemtime($path_source);
        $cache = FALSE;

        //traverse in declining priority
        foreach ($format AS $_extension)
        {
            $_path_cache = "$path_cache.$_extension";

            if (! is_file($_path_cache)) continue; //not found

            if (filemtime($_path_cache) < $time) //expired
            {
                unlink($_path_cache);
                continue;
            };

            if ($cache) continue; //already found

            $extension = $_extension;
            $cache     = TRUE;
        };

        //found cached file
        if ($cache) return CMS_DATA_URL . "image/cache/" . r($dir) . "/" . r($filename) . ".$extension";
    };

    //check real file format
    switch ($type)
    {
    default:             return $_url;
    case IMAGETYPE_JPEG: $extension = "jpg"; break;
    case IMAGETYPE_PNG:  $extension = "png"; break;
    case IMAGETYPE_WEBP: $extension = "webp";
    };

    //prepare background processing
    if ($preview = ($preview && $system->getval("image", "daemon")))
        $daemon = sprintf("image_process(\"%s\", %u, %u, %s, TRUE, FALSE);",
                          addslashes($url),
                          $width,
                          $height,
                          ($preference_override === NULL) ? "NULL" :
                          "\"" . addslashes((string)$preference_override) . "\"");

    //generate preview
    if ($preview)
    {
        $_url       = image_process($url, 50, 50, $preference_override, FALSE, FALSE);
        $_extension = pathinfo($_url, PATHINFO_EXTENSION);
        $path       = image_path($_url);

        if (($path !== FALSE) && copy($path, "$path_cache.$_extension"))
        {
            $_url = CMS_DATA_URL . "image/cache/" . r($dir) . "/" . r($filename) . ".$_extension";

            //temporarily store preview size
            cms_cache("image.size.$_url", $width . "x" . $height, TRUE);

            //daemonize image processing
            cms_daemon("$daemon\n" .
                       sprintf("cms_cache(\"%s\", \"\", TRUE);",
                               "image.size." . addslashes($_url)),
                       "image.$_url",
                       0,
                       "Image processing: " . rawurldecode($url) . " ➔ $width x $height px");

            return $_url;
        };
    };

    set_time_limit(600); //10 minutes

    //gd support
    $gd_info = extension_loaded("gd") ? gd_info() : NULL;

    //temporary file suffix
    $temp = unique_id();

    //temporary file path
    $path_temp = FALSE;

    switch ($extension)
    {
    case "jpg":
//..................................................................................................

        //rescale image using gd library
        if (isset($gd_info["JPEG Support"]) && $gd_info["JPEG Support"])
        {
            //create target image and read source
            $image  = imagecreatetruecolor($width, $height);
            $_image = imagecreatefromjpeg($path_source);

            //scale
            imagecopyresampled($image, $_image,
                               0,      0,       0,       0,
                               $width, $height, $_width, $_height);

            imagedestroy($_image);

            while (TRUE)
            {
                switch ($preference)
                {
                case "webp":

                    //save as webp if supported
                    if (isset($gd_info["WebP Support"]) && $gd_info["WebP Support"])
                    {
                        $extension = "webp";
                        $path_temp = "$path_cache.$extension.$temp";
                        $quality   = image_quality($width, $height, $extension);
                        imagewebp($image, $path_temp, $quality);
                        break 2;
                    };

                    break; //fall back to jpg

                case "png":

                    //save as png if supported
                    if (isset($gd_info["PNG Support"]) && $gd_info["PNG Support"])
                    {
                        $extension = "png";
                        $path_temp = "$path_cache.$extension.$temp";
                        imagepng($image, $path_temp);
                        break 2;
                    };
                };

                //save as jpg
                $extension = "jpg";
                $path_temp = "$path_cache.$extension.$temp";
                $quality   = image_quality($width, $height, $extension);
                imageinterlace($image, TRUE);
                imagejpeg($image, $path_temp, $quality);
                break;
            };

            imagedestroy($image);
            break;
        };

        //rescale image using cgi fallback
        if (preg_match("/^(lin|win)/i", PHP_OS, $os))
        {
            $os = strtolower($os[1]);

            switch ($os)
            {
            case "lin":

                if (PHP_INT_SIZE === 8) $os = "l64";
                break;

            case "win":

                break;
            };

            //scale and overwrite image
            $extension = "jpg";
            $path_temp = "$path_cache.$extension.$temp";
            exec(CMS_PATH . "!bin/jpeg.$os.cgi " .
                 "\"$path_source\" $width $height \"$path_temp\"",
                 $output, $return_var);
        };

        break;

    case "png":
//..................................................................................................

        //check png support
        if (! isset($gd_info["PNG Support"])) break;
        if (! $gd_info["PNG Support"])        break;

        //create target image and read source
        $image  = imagecreatetruecolor($width, $height);
        $error  = set_error_handler(function() { return TRUE; });
        $_image = imagecreatefrompng($path_source);
        set_error_handler($error);

        //check for alpha channel
        $alpha = imagecolortransparent($_image) !== -1;
        if (! $alpha)
        {
            $png_type = ord(file_get_contents($path_source, FALSE, NULL, 25, 1));
            $alpha    = in_array($png_type, [4, 6], TRUE);
        };

        if ($alpha)
        {
            imagesavealpha($image, TRUE);
            imagealphablending($image, FALSE);
            $transparent = imagecolorallocatealpha($image, 0, 0, 0, 127);
            imagefilledrectangle($image, 0, 0, $width - 1, $height - 1, $transparent);
        };

        //rescale image using gd library
        imagecopyresampled($image, $_image,
                           0,      0,       0,       0,
                           $width, $height, $_width, $_height);

        imagedestroy($_image);

        switch ($preference)
        {
        case "webp": $preference = ["webp", "jpg", "png"]; break;
        default:     $preference = ["jpg", "webp", "png"]; break;
        };

        foreach ($preference AS $value)
        {
            switch ($value)
            {
            case "webp":

                //save as webp if supported
                if (isset($gd_info["WebP Support"]) && $gd_info["WebP Support"])
                {
                    $extension = "webp";
                    $path_temp = "$path_cache.$extension.$temp";
                    $quality   = image_quality($width, $height, $extension);
                    imagewebp($image, $path_temp, $quality);
                    break 2;
                };

                break;

            case "jpg":

                //save as jpg if supported and no alpha channel required
                if ((! $alpha) && isset($gd_info["JPEG Support"]) && $gd_info["JPEG Support"])
                {
                    $extension = "jpg";
                    $path_temp = "$path_cache.$extension.$temp";
                    $quality   = image_quality($width, $height, $extension);
                    imageinterlace($image, TRUE);
                    imagejpeg($image, $path_temp, $quality);
                    break 2;
                };

                break;

            default:

                //save as png
                $extension = "png";
                $path_temp = "$path_cache.$extension.$temp";
                imagepng($image, $path_temp);
            };
        };

        //free image memory
        imagedestroy($image);
        break;

    case "webp":
//..................................................................................................

        //check webp support
        if (! isset($gd_info["WebP Support"])) break;
        if (! $gd_info["WebP Support"])        break;

        //create target image and read source
        $image  = imagecreatetruecolor($width, $height);
        $_image = imagecreatefromwebp($path_source);

        //check for extended features
        $header  = file_get_contents($path_source, FALSE, NULL, 0, 21);
        $_header = unpack("a4riff/L1filesize/a4webp/a4format/L1chunksize/C1chunk", $header);

        switch ($_header["format"])
        {
        case "VP8 ":

            $alpha = FALSE;
            $anim  = FALSE;
            break;

        case "VP8L":

            $alpha = TRUE;
            $anim  = FALSE;
            break;

        case "VP8X":

            $alpha = flag($_header["chunk"], 0b00010000);
            $anim  = flag($_header["chunk"], 0b00000010);
            break;

        default:

            break 2; //unsupported format
        };

        if ($anim) break; //animation not supported

        if ($alpha)
        {
            imagesavealpha($image, TRUE);
            imagealphablending($image, FALSE);
            $transparent = imagecolorallocatealpha($image, 0, 0, 0, 127);
            imagefilledrectangle($image, 0, 0, $width - 1, $height - 1, $transparent);
        };

        //rescale image using gd library
        imagecopyresampled($image, $_image,
                           0,      0,       0,       0,
                           $width, $height, $_width, $_height);

        imagedestroy($_image);

        while (TRUE)
        {
            switch ($preference)
            {
            case "jpg":

                //save as jpg if supported and no alpha channel required
                if ((! $alpha) && isset($gd_info["JPEG Support"]) && $gd_info["JPEG Support"])
                {
                    $extension = "jpg";
                    $path_temp = "$path_cache.$extension.$temp";
                    $quality   = image_quality($width, $height, $extension);
                    imageinterlace($image, TRUE);
                    imagejpeg($image, $path_temp, $quality);
                    break 2;
                };

                break; //fall back to webp

            case "png":

                //save as png if supported
                if (isset($gd_info["PNG Support"]) && $gd_info["PNG Support"])
                {
                    $extension = "png";
                    $path_temp = "$path_cache.$extension.$temp";
                    imagepng($image, $path_temp);
                    break 2;
                };
            };

            //save as webp
            $extension = "webp";
            $path_temp = "$path_cache.$extension.$temp";
            $quality   = image_quality($width, $height, $extension);
            imagewebp($image, $path_temp, $quality);
            break;
        };

        //free image memory
        imagedestroy($image);
        break;
    };

    //swap temp into live file
    if (
         ($path_temp !== FALSE)
         &&
         is_file($path_temp)
         &&
         rename($path_temp, "$path_cache.$extension")
       )
        return CMS_DATA_URL . "image/cache/" . r($dir) . "/" . r($filename) . ".$extension";

    return $url;
};

//--------------------------------------------------------------------------------------------------

function image_path($url)
{
    $count = strlen(CMS_ROOT_URL);

    if (streq(substr($url, 0, $count), CMS_ROOT_URL))
    {
        $url = substr($url, $count);
        $url = CMS_ROOT_PATH . rawurldecode($url);
        if (is_file($url)) return $url;
    };

    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function image_type($path)
{
    if (! is_file($path)) return FALSE;

    if (streq(file_extension($path), "svg")) return "svg";

    $type = function_exists("exif_imagetype") ?
        exif_imagetype($path) :
        (\getimagesize($path)[2] ?? NULL);

    switch ($type)
    {
    case IMAGETYPE_GIF:  return "gif";
    case IMAGETYPE_JPEG: return "jpg";
    case IMAGETYPE_PNG:  return "png";
    case IMAGETYPE_WEBP: return "webp";
    };

    return NULL;
};

//--------------------------------------------------------------------------------------------------

function getimagesize($filename,
                      &$imageinfo = NULL,
                      $preview    = NULL)
{
    //return preview size
    if (nstre($value = cms_cache("image.size.$filename"))) return explode("x", $value);

    //get local path if applicable
    if (($_filename = image_path($filename)) !== FALSE) $filename = $_filename;

    //svg
    if (streq(file_extension($filename), "svg"))
    {
        if (! extension_loaded("SimpleXML")) return [300, 250];

        $xml = simplexml_load_file($filename);
        if ($xml === FALSE) return FALSE;

        $width  = isset($xml["width"])  ? dimension_to_px($xml["width"])  : 0;
        $height = isset($xml["height"]) ? dimension_to_px($xml["height"]) : 0;
        if ($width && $height) return [(int)round($width), (int)round($height)];

        if (
             isset($xml["viewBox"])
             &&
             ($value = preg_split("/[\s,]+/", $xml["viewBox"], 4, PREG_SPLIT_NO_EMPTY))
             &&
             (count($value) === 4)
           )
        {
            $_width  = (float)$value[2];
            $_height = (float)$value[3];
            if (($_width <= 0) || ($_height <= 0)) return FALSE;

            if ($width)  return [(int)round($width), (int)round($width  / $_width  * $_height)];
            if ($height) return [(int)round($height / $_height * $_width), (int)round($height)];
            /*
            if ($_width >= $_height) return [300, (int)round($_height * 300 / $_width)];
            else                     return [(int)round($_width * 250 / $_height), 250];
            */
            return [(int)round($_width), (int)round($_height)];
        };
    };

    $return = \getimagesize($filename, $imageinfo);
    return ($return === FALSE) ? ["300", "250"] : $return;
};