<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   CAPTCHA CLASS
//==================================================================================================

class captcha
{

//properties
//--------------------------------------------------------------------------------------------------

    public $secret  = NULL;
    public $length  = 4;
    public $enabled = FALSE;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct()
    {
        //check requirements
        if (! extension_loaded("gd"))       return;
        $array = gd_info();
        if (! isset($array["PNG Support"])) return;
        if (! $array["PNG Support"])        return;

        //load secret
        $date         = cms_cache("captcha.date");
        $_date        = gmdate("Y-m-d");
        $this->secret = cms_cache("captcha.secret");

        //check if secret is outdated
        if (nstreq($date, $_date) || ($this->secret === NULL))
        {
            //create new secret
            $this->secret = unique_id();
            cms_cache("captcha.date", $_date, TRUE);
            cms_cache("captcha.secret", $this->secret, TRUE);
        };

        $this->enabled = TRUE;
    }

//methods
//--------------------------------------------------------------------------------------------------

    function create()
    {
        if (! $this->enabled) return FALSE;

        //create captcha string
        $key    = strtolower(unique_id($this->length));

        //create verification code
        $code   = hash32("$key|" . $this->secret);

        //create image file
        $size   = 70;
        $width  = $this->length * $size;
        $height = $size;
        $image  = imagecreate($width, $height);

        //allocate basic colors
        $white  = imagecolorallocate($image, 0xFF, 0xFF, 0xFF);
        $black  = imagecolorallocate($image, 0x00, 0x00, 0x00);

        for ($i = 0; $i < $this->length; $i++)
        {
            //load character image
            $char   = $key[$i];
            $_image = imagecreatefrompng(CMS_IMAGES_PATH . "captcha/$char.png");

            //rotate randomly by a maximum of 45 degree to any direction
            if (function_exists("imagerotate"))
            {
                $color  = (imagecolorstotal($_image) > 255) ?
                          imagecolorresolve($_image, 0xFF, 0xFF, 0xFF) :
                          imagecolorallocate($_image, 0xFF, 0xFF, 0xFF);
                $_image = imagerotate($_image, mt_rand(-45, 45), $color);
            };

            //copy character into captcha image
            $_width  = imagesx($_image);
            $_height = imagesy($_image);
            $x       = ($size - $_width)  / 2;
            $y       = ($size - $_height) / 2;
            imagecopy($image, $_image, $i * $size + $x, $y, 0, 0, $_width, $_height);
            imagedestroy($_image);
        };

        //add obfuscation
        /*
        $_width  = $width  / 2;
        $_height = $height / 2;

        for ($i = 0; $i < 4; $i++)
        {
            //generate random box
            $x  = mt_rand(0, $_width);
            $y  = mt_rand(0, $_height);
            $_x = mt_rand($_width, $width);
            $_y = mt_rand($_height, $height);

            for ($__y = $y; $__y < $_y; $__y++)
            {
                for ($__x = $x; $__x < $_x; $__x++)
                {
                    //retrieve color at position
                    $color = imagecolorat($image, $__x, $__y);
                    $color = imagecolorsforindex($image, $color);

                    //invert color
                    $color["red"]   = 0xFF ^ $color["red"];
                    $color["green"] = 0xFF ^ $color["green"];
                    $color["blue"]  = 0xFF ^ $color["blue"];

                    //set inverted color at position
                    $color = (imagecolorstotal($image) > 255) ?
                             imagecolorresolve($image, $color["red"], $color["green"], $color["blue"]) :
                             imagecolorallocate($image, $color["red"], $color["green"], $color["blue"]);
                    imagesetpixel($image, $__x, $__y, $color);
                };
            };
        };
        */

        //add obfuscation
        $x  = 0;
        $y  = $height / 2;
        $_x = $width;
        $_y = $height;

        for ($__y = $y; $__y < $_y; $__y++)
        {
            for ($__x = $x; $__x < $_x; $__x++)
            {
                //retrieve color at position
                $color = imagecolorat($image, $__x, $__y);
                $color = imagecolorsforindex($image, $color);

                //invert color
                $color["red"]   = 0xFF ^ $color["red"];
                $color["green"] = 0xFF ^ $color["green"];
                $color["blue"]  = 0xFF ^ $color["blue"];

                //set inverted color at position
                $color = (imagecolorstotal($image) > 255) ?
                         imagecolorresolve($image, $color["red"], $color["green"], $color["blue"]) :
                         imagecolorallocate($image, $color["red"], $color["green"], $color["blue"]);
                imagesetpixel($image, $__x, $__y, $color);
            };
        };

        //reduce to pure black and white only
        //or bots could use intermediate shades
        //to recognize character shapes
        for ($y = 0; $y < $height; $y++)
        {
            for ($x = 0; $x < $width; $x++)
            {
                //retrieve color at position
                $color = imagecolorat($image, $x, $y);
                $color = imagecolorsforindex($image, $color);

                //use either pure white or black
                $mean  = ($color["red"] + $color["green"] + $color["blue"]) / 3;
                $color = ($mean > 0x80) ? $white : $black;

                //set color at position
                imagesetpixel($image, $x, $y, $color);
            };
        };

        //generate image
        ob_start();
        imagepng($image);
        imagedestroy($image);

        return ["code"   => $code,
                "image"  => "data:image/png;base64," . base64_encode(ob_get_clean()),
                "width"  => $width,
                "height" => $height];
    }

//--------------------------------------------------------------------------------------------------

    function verify($key,
                    $code)
    {
        if (! $this->enabled) return TRUE;

        return streq(hash32(strtolower($key) . "|" . $this->secret), $code);
    }
};