<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   HTML CLASS
//==================================================================================================

//constants
define("CMS_HTML_GET_FIRST", 1);
define("CMS_HTML_GET_NEXT",  2);

//related functions
//--------------------------------------------------------------------------------------------------

function html_page_info($url)
{
    $html = new html($url);

    if (stre($html->file)) return FALSE;

    //check for basic html document structure
    if (! preg_match("/^.*?<html(?:\s[^>]*)?>" .
                     ".*?<head(?:\s[^>]*)?>.*?<\/head\s*>" .
                     ".*?<body(?:\s[^>]*)?>/isu",
                     $html->file))
        return FALSE;

    //get meta data
    $html->reset();
    while ($buffer = $html->get(CMS_HTML_GET_NEXT, "meta", FALSE))
    {
        if (
             (
               isset($buffer["#attribute"]["name"])
               ||
               isset($buffer["#attribute"]["http-equiv"])
             )
             &&
             isset($buffer["#attribute"]["content"])
           )
        {
            $key   = isset($buffer["#attribute"]["name"]) ?
                     $buffer["#attribute"]["name"] :
                     $buffer["#attribute"]["http-equiv"];
            $key   = stripspaces(utf8_strtolower(htmlentities_decode($key)));
            $value = stripspaces(htmlentities_decode($buffer["#attribute"]["content"]));

            if (isset($return["meta"][$key]))
                $return["meta"][$key] .= ", $value";
            else
                $return["meta"][$key]  = $value;
        };
    };

    //get title
    $buffer          = $html->get(CMS_HTML_GET_FIRST, "title");
    $return["title"] = stripspaces(htmlentities_decode($buffer["#pcdata"]));

    //get content
    $h1   = []; //heading 1
    $h2   = []; //heading 2
    $h3   = []; //heading 3
    $copy = []; //copy (w/o heading and side content)
    $text = []; //heading + copy
    $side = []; //side content

    //move to body element
    $html->get(CMS_HTML_GET_FIRST, "body", FALSE);
    $position = $html->position;

    while ($buffer = $html->get(CMS_HTML_GET_NEXT, "h[1-3]|aside|footer|nav"))
    {
        //fetch part between specified elements
        $_buffer  = substr($html->file, $position, $buffer["#offset"] - $position);
        $__buffer = htmltoplain($_buffer, FALSE, TRUE);
        if (nstre($__buffer)) $copy[] = $__buffer;
        $__buffer = htmltoplain($_buffer, FALSE, TRUE, TRUE);
        if (nstre($__buffer)) $text[] = $__buffer;
        $position = $html->position;

        //preprocess
        switch ($buffer["#element"])
        {
        case "h1":
        case "h2":
        case "h3":

            $_buffer = htmltoplain($buffer["#pcdata"], FALSE, TRUE, TRUE);
            break;

        case "aside":
        case "footer":
        case "nav":

            $_buffer = htmltoplain($buffer["#pcdata"], FALSE, TRUE);
            break;
        };

        if (stre($_buffer)) continue; //no content

        //add to content fields
        switch ($buffer["#element"])
        {
        case "h1":

            $h1[] = $text[] = $_buffer;
            break;

        case "h2":

            $h2[] = $text[] = $_buffer;
            break;

        case "h3":

            $h3[] = $text[] = $_buffer;
            break;

        case "aside":
        case "footer":
        case "nav":

            $side[] = $_buffer;
            break;
        };
    };

    //fetch rest
    $buffer   = substr($html->file, $position);
    $_buffer  = htmltoplain($buffer, FALSE, TRUE);
    if (nstre($_buffer)) $copy[] = $_buffer;
    $_buffer  = htmltoplain($buffer, FALSE, TRUE, TRUE);
    if (nstre($_buffer)) $text[] = $_buffer;

    //generate final content field
    $return["h1"]   = implode(" ",   $h1);
    $return["h2"]   = implode(" ",   $h2);
    $return["h3"]   = implode(" ",   $h3);
    $return["copy"] = implode(" ",   $copy);
    $return["text"] = implode(" — ", $text);
    $return["side"] = implode(" ",   $side);

    //get document base
    if (
         ($buffer = $html->get(CMS_HTML_GET_FIRST, "base", NULL))
         &&
         isset($buffer["#attribute"]["href"])
       )
    {
        $_url = htmlentities_decode($buffer["#attribute"]["href"]);
        $url  = absolute_path($url, $_url);
    };

    //get links
    $return["links"] = [];

    $html->reset();
    while ($buffer = $html->get(CMS_HTML_GET_NEXT, "a"))
    {
        if (! isset($buffer["#attribute"]["href"])) continue; //no target address

        if (
             isset($buffer["#attribute"]["rel"])
             &&
             preg_match("/\bnofollow\b/i", $buffer["#attribute"]["rel"])
           )
            continue; //ignore link

        $_url = htmlentities_decode($buffer["#attribute"]["href"]);

        //verify address
        if (($_buffer = analyze_url($_url)) === FALSE) continue; //invalid url

        switch ($_buffer["scheme"])
        {
        case "":
        case "http":
        case "https":

            //add to list
            $_url              = absolute_path($url, $_url);
            $return["links"][] = ["url"  => $_url,
                                  "text" => $buffer["#pcdata"]];
            break;
        };
    };

    return $return;
};

class html
{

//properties
//--------------------------------------------------------------------------------------------------

    public $file     = NULL;
    public $position = 0;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($url)
    {
        //load library
        if (! cms_load("http")) return;

        //retrieve data
        $hfile      = http_fopen($url);
        if ($hfile === FALSE) return; //no connection
        $this->file = http_fetch_data($hfile);

        if ($this->file === FALSE)
        {
            $this->file = NULL;
            return;
        };

        //convert character set
        if (! utf8_detect($this->file)) //not utf-8
        {
            $charset = CMS_UTF8_CHARSET_ISO_8859_1; //assume default

            if (preg_match_all("/<meta\s+([^<>]*)\s*\/?>/i", $this->file, $array))
            {
                foreach ($array[1] AS $value)
                {
                    if (preg_match_all("/([^\s\"'>\/=]+)\s*(?:=\s*(?:\"([^\"]*)|\'([^']*)|([^\s>]*)))?/", $value, $_value))
                    {
                        $attribute = [];
                        for ($i = 0, $c = count($_value[1]); $i < $c; $i++)
                            $attribute[strtolower($_value[1][$i])] =
                                htmlentities_decode($_value[2][$i] . $_value[3][$i] . $_value[4][$i]);

                        //meta charset
                        if (isset($attribute["charset"]) && nstre($attribute["charset"]))
                        {
                            $charset = $attribute["charset"];
                            break;
                        };

                        //meta http-equiv content-type
                        if (
                             isset($attribute["http-equiv"]) && streq($attribute["http-equiv"], "content-type")
                             &&
                             isset($attribute["content"]) && nstre($attribute["content"])
                             &&
                             cms_load("mime")
                           )
                        {
                            $content = mime_extract_rfc2822_header($attribute["content"]);
                            if (isset($content["charset"]) && nstre($content["charset"]))
                            {
                                $charset = $content["charset"];
                                break;
                            };
                        };
                    };
                };
            };

            //convert
            $this->file = utf8_convert($this->file, $charset);
        };

        //normalize
        $this->file = utf8_normalize($this->file);

        //remove noise
        $search     = ["/<!--.*?-->/su",                            //comment
                       "/<script(?:\s[^>]*)?>.*?<\/script\s*>/isu", //script
                       "/<style(?:\s[^>]*)?>.*?<\/style\s*>/isu"];  //style
        $this->file = preg_replace($search, "", $this->file);
    }

//methods
//--------------------------------------------------------------------------------------------------

    function get_attributes($string)
    {
        $return   = FALSE;
        $position = 0;

        while (TRUE)
        {
            //attribute = "value"
            if (preg_match("/[\p{C}\p{Z}]*([^\p{C}\p{Z}=]+)[\p{C}\p{Z}]*=[\p{C}\p{Z}]*\"([^\"]*)\"/Uu",
                           $string,
                           $array,
                           PREG_OFFSET_CAPTURE,
                           $position))
            {
                $return[$array[1][0]] = $array[2][0];
                $position             = $array[0][1] + strlen($array[0][0]);
                continue;
            };

            //attribute = 'value'
            if (preg_match("/[\p{C}\p{Z}]*([^\p{C}\p{Z}=]+)[\p{C}\p{Z}]*=[\p{C}\p{Z}]*'([^']*)'/Uu",
                           $string,
                           $array,
                           PREG_OFFSET_CAPTURE,
                           $position))
            {
                $return[$array[1][0]] = $array[2][0];
                $position             = $array[0][1] + strlen($array[0][0]);
                continue;
            };

            //attribute = value
            if (preg_match("/[\p{C}\p{Z}]*([^\p{C}\p{Z}=]+)[\p{C}\p{Z}]*=[\p{C}\p{Z}]*([^\p{C}\p{Z}]*)/Uu",
                           $string,
                           $array,
                           PREG_OFFSET_CAPTURE,
                           $position))
            {
                $return[$array[1][0]] = $array[2][0];
                $position             = $array[0][1] + strlen($array[0][0]);
                continue;
            };

            //attribute
            if (preg_match("/[\p{C}\p{Z}]*([^\p{C}\p{Z}]+)/Uu",
                           $string,
                           $array,
                           PREG_OFFSET_CAPTURE,
                           $position))
            {
                $return[$array[1][0]] = TRUE;
                $position             = $array[0][1] + strlen($array[0][0]);
                continue;
            };

            break;
        };

        return $return;
    }

//--------------------------------------------------------------------------------------------------

    function get($option        = CMS_HTML_GET_NEXT,
                 $element       = NULL,
                 $get_pcdata    = TRUE,
                 $ignore_nested = TRUE)
    {
        if ($this->file === NULL)          return FALSE;
        if ($option == CMS_HTML_GET_FIRST) $this->position = 0;
        if ($element === NULL)             $element        = "[0-9a-z]+";

        //search for opening element
        if (! preg_match("/<(" . $element . ")(?:[\p{C}\p{Z}]+(.*))?(\/?)>/isUu",
                         $this->file,
                         $array,
                         PREG_OFFSET_CAPTURE,
                         $this->position))
            return NULL;

        $_element             = preg_quote($array[1][0]); //matched element name
        $return["#element"]   = utf8_strtolower($array[1][0]);
        $return["#attribute"] = $this->get_attributes($array[2][0]);
        $return["#pcdata"]    = "";
        $return["#offset"]    = $array[0][1];
        $position             = strlen($array[0][0]);     //length of element
        $_position            = $array[0][1] + $position; //offset + length of element
        $this->position       = $_position;
        $flag                 = TRUE;

        //empty element or pcdata not required
        if ($array[3][0] || (! $get_pcdata)) return $return;

        //search for closing element
        while (preg_match("/<\/[\p{C}\p{Z}]*" . $_element . "[\p{C}\p{Z}]*>/isUu",
                          $this->file,
                          $array,
                          PREG_OFFSET_CAPTURE,
                          $_position))
        {
            $pcdata = substr($this->file, $_position, $array[0][1] - $_position);

            //check for nested elements balance
            if ($depth = preg_match_all("/<" . $_element . "(?:[\p{C}\p{Z}]+.*)?(?<!\/)>/isUu",
                                        $pcdata,
                                        $_array,
                                        PREG_OFFSET_CAPTURE))
            {
                if ($flag) { $position = $_array[0][0][1]; $flag = FALSE; };

                $_depth = preg_match_all("/<\/[\p{C}\p{Z}]*" . $_element . "[\p{C}\p{Z}]*>/isUu",
                                         $pcdata);

                if ($depth > $_depth)
                {
                    $_position = $array[0][1] + strlen($array[0][0]);
                    continue;
                };
            };

            $return["#pcdata"] = $pcdata;

            //set position
            if (! $ignore_nested) $this->position += $position;
            else                  $this->position  = $array[0][1] + strlen($array[0][0]);

            return $return;
        };

        //no closing element
        $return["#pcdata"] = substr($this->file, $_position);
        $this->position    = strlen($this->file);

        return $return;
    }

//--------------------------------------------------------------------------------------------------

    function reset()
    {
        if ($this->file === NULL) return FALSE;

        $this->position = 0;

        return TRUE;
    }
};