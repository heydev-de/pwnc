<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   TEMPLATE CLASS
//==================================================================================================

//permission
define("CMS_TEMPLATE_PERMISSION_OPERATOR", "operator");

//constants
define("CMS_TEMPLATE_CACHE_SEPARATOR",     "\x1C"); //ascii file separator

//option flag
define("CMS_TEMPLATE_OPTION_NONE",                  0);
define("CMS_TEMPLATE_OPTION_HREF",                  1);
define("CMS_TEMPLATE_OPTION_PLUGIN",                2);
define("CMS_TEMPLATE_OPTION_TEXT",                  4);
define("CMS_TEMPLATE_OPTION_VALUE",                 8);
define("CMS_TEMPLATE_OPTION_DOWNLOAD",             16);
define("CMS_TEMPLATE_OPTION_IMAGE",                32);
define("CMS_TEMPLATE_OPTION_THUMBNAIL",            64);
define("CMS_TEMPLATE_OPTION_MEDIA",               128);
define("CMS_TEMPLATE_OPTION_TEMPLATE",            256);
define("CMS_TEMPLATE_OPTION_GROUP",               512);
define("CMS_TEMPLATE_OPTION_REPEAT",             1024);
define("CMS_TEMPLATE_OPTION_SHIFT",              2048);
define("CMS_TEMPLATE_OPTION_CALT",               4096);
define("CMS_TEMPLATE_OPTION_CBLOCK",             8192);
define("CMS_TEMPLATE_OPTION_DEBUG",             16384);
define("CMS_TEMPLATE_OPTION_SWITCH",            32768);
define("CMS_TEMPLATE_OPTION_LAYOUT",       CMS_TEMPLATE_OPTION_VALUE     |
                                           CMS_TEMPLATE_OPTION_TEMPLATE  |
                                           CMS_TEMPLATE_OPTION_GROUP     |
                                           CMS_TEMPLATE_OPTION_REPEAT    |
                                           CMS_TEMPLATE_OPTION_SHIFT     |
                                           CMS_TEMPLATE_OPTION_CBLOCK    |
                                           CMS_TEMPLATE_OPTION_SWITCH);
define("CMS_TEMPLATE_OPTION_EDIT",         CMS_TEMPLATE_OPTION_HREF      |
                                           CMS_TEMPLATE_OPTION_PLUGIN    |
                                           CMS_TEMPLATE_OPTION_TEXT      |
                                           CMS_TEMPLATE_OPTION_DOWNLOAD  |
                                           CMS_TEMPLATE_OPTION_IMAGE     |
                                           CMS_TEMPLATE_OPTION_THUMBNAIL |
                                           CMS_TEMPLATE_OPTION_MEDIA     |
                                           CMS_TEMPLATE_OPTION_CBLOCK);
define("CMS_TEMPLATE_OPTION_ALL",          CMS_TEMPLATE_OPTION_LAYOUT    |
                                           CMS_TEMPLATE_OPTION_EDIT);

define("CMS_TEMPLATE_ACTION",                       0);
define("CMS_TEMPLATE_CONTROL",                      1);
define("CMS_TEMPLATE_CODE",                         2);
define("CMS_TEMPLATE_IMAGE",                        3);
define("CMS_TEMPLATE_COMMAND",                      4);
define("CMS_TEMPLATE_SWITCH",                       5);

//type
define("CMS_TEMPLATE_TYPE_NONE",                    0);
define("CMS_TEMPLATE_TYPE_HEAD",                    1);
define("CMS_TEMPLATE_TYPE_HREF",                    2);
define("CMS_TEMPLATE_TYPE_PLUGIN",                  4);
define("CMS_TEMPLATE_TYPE_TEXT",                    8);
define("CMS_TEMPLATE_TYPE_VALUE",                  16);
define("CMS_TEMPLATE_TYPE_DOWNLOAD",               32);
define("CMS_TEMPLATE_TYPE_IMAGE",                  64);
define("CMS_TEMPLATE_TYPE_THUMBNAIL",             128);
define("CMS_TEMPLATE_TYPE_MEDIA",                 256);
define("CMS_TEMPLATE_TYPE_TEMPLATE",              512);
define("CMS_TEMPLATE_TYPE_GROUP",                1024);
define("CMS_TEMPLATE_TYPE_REPEAT",               2048);
define("CMS_TEMPLATE_TYPE_SHIFT",                4096);
define("CMS_TEMPLATE_TYPE_MENU",                 8192);
define("CMS_TEMPLATE_TYPE_CBLOCK",              16384);
define("CMS_TEMPLATE_TYPE_CALT",                32768);
define("CMS_TEMPLATE_TYPE_BASE",                65536);
define("CMS_TEMPLATE_TYPE_NAMESPACE",          131072);
define("CMS_TEMPLATE_TYPE_NOCACHE",            262144);
define("CMS_TEMPLATE_TYPE_CONTROL",            524288);
define("CMS_TEMPLATE_TYPE_BACKLINK",          1048576);
define("CMS_TEMPLATE_TYPE_DEBUG",             2097152);
define("CMS_TEMPLATE_TYPE_STYLESHEET",        4194304);
define("CMS_TEMPLATE_TYPE_SWITCH",            8388608);
define("CMS_TEMPLATE_TYPE_CEDIT",            16777216);
define("CMS_TEMPLATE_TYPE_CNOEDIT",          33554432);
define("CMS_TEMPLATE_TYPE_JAVASCRIPT",       67108864);
define("CMS_TEMPLATE_TYPE_ALL",            4294967295);

//type filter
define("CMS_TEMPLATE_TYPE_EDIT",           CMS_TEMPLATE_TYPE_HREF      |
                                           CMS_TEMPLATE_TYPE_PLUGIN    |
                                           CMS_TEMPLATE_TYPE_TEXT      |
                                           CMS_TEMPLATE_TYPE_VALUE     |
                                           CMS_TEMPLATE_TYPE_DOWNLOAD  |
                                           CMS_TEMPLATE_TYPE_IMAGE     |
                                           CMS_TEMPLATE_TYPE_THUMBNAIL |
                                           CMS_TEMPLATE_TYPE_MEDIA     |
                                           CMS_TEMPLATE_TYPE_TEMPLATE  |
                                           CMS_TEMPLATE_TYPE_GROUP     |
                                           CMS_TEMPLATE_TYPE_REPEAT    |
                                           CMS_TEMPLATE_TYPE_SHIFT     |
                                           CMS_TEMPLATE_TYPE_SWITCH);
define("CMS_TEMPLATE_TYPE_SPAN",           CMS_TEMPLATE_TYPE_HREF      |
                                           CMS_TEMPLATE_TYPE_DOWNLOAD  |
                                           CMS_TEMPLATE_TYPE_GROUP     |
                                           CMS_TEMPLATE_TYPE_REPEAT    |
                                           CMS_TEMPLATE_TYPE_SHIFT     |
                                           CMS_TEMPLATE_TYPE_CBLOCK    |
                                           CMS_TEMPLATE_TYPE_CALT      |
                                           CMS_TEMPLATE_TYPE_BASE      |
                                           CMS_TEMPLATE_TYPE_NAMESPACE |
                                           CMS_TEMPLATE_TYPE_NOCACHE   |
                                           CMS_TEMPLATE_TYPE_CEDIT     |
                                           CMS_TEMPLATE_TYPE_CNOEDIT);
define("CMS_TEMPLATE_TYPE_PATH",           CMS_TEMPLATE_TYPE_TEMPLATE  |
                                           CMS_TEMPLATE_TYPE_GROUP     |
                                           CMS_TEMPLATE_TYPE_REPEAT    |
                                           CMS_TEMPLATE_TYPE_SHIFT     |
                                           CMS_TEMPLATE_TYPE_BASE      |
                                           CMS_TEMPLATE_TYPE_NAMESPACE);

//command
define("CMS_TEMPLATE_COMMAND_NONE",                 0);
define("CMS_TEMPLATE_COMMAND_BUFFER",               1);
define("CMS_TEMPLATE_COMMAND_PASTE",                2);
define("CMS_TEMPLATE_COMMAND_SWAP",                 4);
define("CMS_TEMPLATE_COMMAND_KICK1",                8);
define("CMS_TEMPLATE_COMMAND_KICK2",               16);
define("CMS_TEMPLATE_COMMAND_DROP1",               32);
define("CMS_TEMPLATE_COMMAND_DROP2",               64);
define("CMS_TEMPLATE_COMMAND_RELEASE",            128);
define("CMS_TEMPLATE_COMMAND_REFERENCE",          256);
define("CMS_TEMPLATE_COMMAND_EXPORT",             512);
define("CMS_TEMPLATE_COMMAND_CLEAR",             1024);
define("CMS_TEMPLATE_COMMAND_DRAGDROP1",         2048);
define("CMS_TEMPLATE_COMMAND_DRAGDROP2",         4096);
define("CMS_TEMPLATE_COMMAND_ALL",         4294967295);

//structure
define("CMS_TEMPLATE_STRUCTURE_INDEX",              0);
define("CMS_TEMPLATE_STRUCTURE_PATH",               1);
define("CMS_TEMPLATE_STRUCTURE_PARENT",             2);
define("CMS_TEMPLATE_STRUCTURE_TYPE",               3);

//asset type
define("CMS_TEMPLATE_ASSET_TYPE_CODE",              1);
define("CMS_TEMPLATE_ASSET_TYPE_STYLESHEET",        2);
define("CMS_TEMPLATE_ASSET_TYPE_JAVASCRIPT",        3);

//related functions
//--------------------------------------------------------------------------------------------------

function template_get_array($page = NULL)
{
    $data  = new data("#system/template");
    $array = ["" => []];

    $data->move("first");
    while ($key = $data->move("next"))
    {
        if ($page !== NULL)
        {
            if ($data->get($key, "page"))
            {
                if ($page === FALSE) continue;
            }
            else
            {
                if ($page === TRUE)  continue;
            };
        };

        $category = (string)$data->get($key, "category");
        $name     = l($data->get($key, "name"));
        if (stre($name)) $name = $key;

        $_name = $name;
        $i     = 1;
        while (isset($array[$category][$_name]))
            $_name = $name . " (" . $i++ . ")";

        $array[$category][$_name] = $key;
    };

    ksort_recursive($array, SORT_NATURAL | SORT_FLAG_CASE);

    return $array;
};

//--------------------------------------------------------------------------------------------------

function template_get_select($page = NULL)
{
    $data  = new data("#system/template");
    $array = ["" => ""];

    $data->move("first");
    while ($key = $data->move("next"))
    {
        if ($page !== NULL)
        {
            if ($data->get($key, "page"))
            {
                if ($page === FALSE) continue;
            }
            else
            {
                if ($page === TRUE)  continue;
            };
        };

        $category = (string)$data->get($key, "category");
        $array[$category] = $category;
    };

    ksort($array, SORT_NATURAL | SORT_FLAG_CASE);

    return $array;
};

//--------------------------------------------------------------------------------------------------

function template_get_attribute($string,
                                $name = NULL)
{
    $flag  = stre($name);
    $name  = $flag ? "[^\s\"'\/>=]+" : preg_quote(utf8_strtolower($name), "/");
    $regex = "/\s($name)\s*(?:=\s*" .                    //name
             "(?:\"([^\"]*)|\'([^']*)|([^\s\/>]*)))?/i"; //value

    if (! preg_match_all($regex, $string, $_string)) return NULL;

    $array = [];
    for ($i = 0, $c = count($_string[0]); $i < $c; $i++)
        $array[utf8_strtolower($_string[1][$i])] =
            htmlentities_decode($_string[2][$i] . $_string[3][$i] . $_string[4][$i]);

    return $flag ? $array : ($array[$name] ?? NULL);
};

//--------------------------------------------------------------------------------------------------

function template_set_attribute($string,
                                $name,
                                $value)
{
    $string = template_remove_attribute($string, $name);

    return preg_replace_callback(
        "/^<[^\s\/>]+/",
        function($match) use ($name, $value)
        {
            return $match[0] . " $name=\"$value\"";
        },
        $string);
};

//--------------------------------------------------------------------------------------------------

function template_remove_attribute($string,
                                   $name = NULL)
{
    $name  = stre($name) ? "[^\s\"'\/=>]+" : preg_quote(utf8_strtolower($name), "/");
    $regex = "/\s+$name\s*(?:=\s*" .                 //name
             "(?:\"[^\"]*\"|\'[^']*\'|[^\s\/>]*))?/i"; //value

    return preg_replace($regex, "", $string);
};

//--------------------------------------------------------------------------------------------------

function template_parse_reference($value,
                                  $analyze = FALSE)
{
    //handle deprecated format
    if (preg_match("/^((?:directory|content)(?!:\/\/)|address):(.+)$/s", $value, $_value))
        $value = streq($_value[1], "address") ? $_value[2] : $_value[1] . "://" . $_value[2];

    //analyze
    if ($value = analyze_url($value))
    {
        if ($analyze) return $value;
        $type  = $value["scheme"];
        $index = $value["host"];
        $url   = $value["url"];
    }

    //invalid
    else
    {
        return FALSE;
    };

    //process reference
    $array = ["name"        => NULL,
              "description" => NULL,
              "url"         => NULL];

    switch ($type)
    {
    case "directory":
//..................................................................................................

        $directory            = new data("#system/directory");
        $array["name"]        = l($directory->get($index, "name"));
        $value                = l($directory->get($index, "description"));
        $value                = remove_format($value, TRUE, TRUE);
        $value                = htmltoplain($value);
        $array["description"] = $value;

        break;

    case "content":
//..................................................................................................

        if (! cms_load("content")) break;
        $query = "SELECT " . CMS_DB_CONTENT_TITLE . ", " .
                             CMS_DB_CONTENT_DESCRIPTION . " " .
                 "FROM " .   CMS_DB_CONTENT . " " .
                 "WHERE " .  CMS_DB_CONTENT_INDEX .
                 "='" .      sqlesc($index) . "' " .
                 "LIMIT 1";
        if (
             (($result = mysql_query($query)) !== FALSE)
             &&
             is_array($resultrow = mysql_fetch_row($result))
           )
        {
            $array["name"]        = l($resultrow[0]);
            $value                = l($resultrow[1]);
            $value                = remove_format($value, TRUE, TRUE);
            $value                = htmltoplain($value);
            $array["description"] = $value;
        };

        break;

    default:
    case "address":
//..................................................................................................

        break;
    };

    $array["url"] = translate_url($url);

    if (stre($array["name"])) $array["name"] = $array["url"];
    return $array;
};

//--------------------------------------------------------------------------------------------------

function template_read_plugin($url)
{
    if (cms_load("http") && (($hfile = http_fopen($url)) !== FALSE))
        return http_fetch_data($hfile);

    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function template_preview($index_or_code,
                          $is_index = TRUE,
                          $document = NULL,
                          $inert    = TRUE)
{
    //define dummy actions
    $action = [CMS_TEMPLATE_ACTION         =>
              [CMS_TEMPLATE_TYPE_HREF      => [CMS_TEMPLATE_CODE  => "javascript:;",
                                               CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_href.svg"],
               CMS_TEMPLATE_TYPE_PLUGIN    => [CMS_TEMPLATE_CODE  => "javascript:;",
                                               CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_plugin.svg"],
               CMS_TEMPLATE_TYPE_TEXT      => [CMS_TEMPLATE_CODE  => "javascript:;",
                                               CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_text.svg"],
               CMS_TEMPLATE_TYPE_VALUE     => [CMS_TEMPLATE_CODE  => "javascript:;",
                                               CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_value.svg"],
               CMS_TEMPLATE_TYPE_DOWNLOAD  => [CMS_TEMPLATE_CODE  => "javascript:;",
                                               CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_download.svg"],
               CMS_TEMPLATE_TYPE_IMAGE     => [CMS_TEMPLATE_CODE  => "javascript:;",
                                               CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_image.svg"],
               CMS_TEMPLATE_TYPE_THUMBNAIL => [CMS_TEMPLATE_CODE  => "javascript:;",
                                               CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_thumbnail.svg"],
               CMS_TEMPLATE_TYPE_MEDIA     => [CMS_TEMPLATE_CODE  => "javascript:;",
                                               CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_media.svg"],
               CMS_TEMPLATE_TYPE_TEMPLATE  => [CMS_TEMPLATE_CODE  => "javascript:;",
                                               CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_template.svg"],
               CMS_TEMPLATE_TYPE_GROUP     => [CMS_TEMPLATE_CODE  => "javascript:;",
                                               CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_group.svg"],
               CMS_TEMPLATE_TYPE_REPEAT    => [CMS_TEMPLATE_CODE  => "javascript:;",
                                               CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_repeat.svg"],
               CMS_TEMPLATE_TYPE_SHIFT     => [CMS_TEMPLATE_CODE  => "javascript:;",
                                               CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_shift.svg"],
               CMS_TEMPLATE_TYPE_SWITCH    => [CMS_TEMPLATE_CODE  => "javascript:;",
                                               CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_switch.svg"],
               CMS_TEMPLATE_TYPE_CBLOCK    => TRUE]];

    //create document if not given
    if ($document === NULL)
    {
        if (! cms_load("document")) return;
        $document = new document();
    };

    //let the template engine know it is a preview
    define("CMS_TEMPLATE_PREVIEW", TRUE);

    $template = new template();

    //preview based on given template index
    if ($is_index)
    {
        //base template
        if ($template->data->get($index_or_code, "page"))
        {
            //parse
            $buffer = $template->parse(
                $document,          //document
                $index_or_code,     //index
                CMS_L_TEMPLATE_002, //title
                NULL,               //description
                NULL,               //keyword
                $action);           //action
            echo($inert ? preview_inert($buffer) : $buffer);
            return;
        };

        //modular template
        $document->set("preview", "template", $index_or_code);
        $index_or_code = "<CMS:template id=\"preview\"/>";
    };

    //embed code in simple page frame
    $text =
        CMS_DOCTYPE_HTML .
        "<html>" .
        "<head>" .
        "<CMS:head/>" .
        stylesheet(CMS_URL . "stylesheet/template.css", FALSE) .
        "</head>" .
        "<body>" .
        $index_or_code .
        "</body>" .
        "</html>";

    $buffer = $template->parse_code(
        $document,          //document
        $text,              //template
        CMS_L_TEMPLATE_002, //title
        NULL,               //description
        NULL,               //keyword
        $action);           //action
    echo($inert ? preview_inert($buffer) : $buffer);
};

//--------------------------------------------------------------------------------------------------

function template_error($code,
                        $message,
                        $path = NULL,
                        $line = NULL)
{
    $path  = "embedded PHP script";
    $index = $GLOBALS["template_error_index"];

    if (nstre($index))
    {
        $data = new data("#system/template");
        $name = l($data->get($index, "name"));

        if (nstre($name))
        {
            $path .= " \"$name\"";

            switch (CMS_LANGUAGE)
            {
            default:

                $_path = CMS_DATA_PATH . "#template/" . CMS_LANGUAGE . "$index.htm";
                if (is_file($_path)) { $path .= " ($_path)"; break; };

            case "":

                $_path = CMS_DATA_PATH . "#template/$index.htm";
                if (is_file($_path)) { $path .= " ($_path)"; break; };
            };
        };
    };

    return cms_error(
        $code,
        $message,
        $path,
        $GLOBALS["template_error_line"] + $line - 1,
        (bool)CMS_TEMPLATE_EDIT);
};

class template
{

//properties
//--------------------------------------------------------------------------------------------------

    public $data         = NULL;
    public $operator     = NULL;
    public $compat_mode  = TRUE;

    public $tlist        = [""                          => CMS_TEMPLATE_TYPE_NONE,
                            "head"                      => CMS_TEMPLATE_TYPE_HEAD,
                            "href"                      => CMS_TEMPLATE_TYPE_HREF,
                            "plugin"                    => CMS_TEMPLATE_TYPE_PLUGIN,
                            "text"                      => CMS_TEMPLATE_TYPE_TEXT,
                            "value"                     => CMS_TEMPLATE_TYPE_VALUE,
                            "download"                  => CMS_TEMPLATE_TYPE_DOWNLOAD,
                            "image"                     => CMS_TEMPLATE_TYPE_IMAGE,
                            "thumbnail"                 => CMS_TEMPLATE_TYPE_THUMBNAIL,
                            "media"                     => CMS_TEMPLATE_TYPE_MEDIA,
                            "template"                  => CMS_TEMPLATE_TYPE_TEMPLATE,
                            "group"                     => CMS_TEMPLATE_TYPE_GROUP,
                            "repeat"                    => CMS_TEMPLATE_TYPE_REPEAT,
                            "shift"                     => CMS_TEMPLATE_TYPE_SHIFT,
                            "menu"                      => CMS_TEMPLATE_TYPE_MENU,
                            "cblock"                    => CMS_TEMPLATE_TYPE_CBLOCK,
                            "calt"                      => CMS_TEMPLATE_TYPE_CALT,
                            "base"                      => CMS_TEMPLATE_TYPE_BASE,
                            "namespace"                 => CMS_TEMPLATE_TYPE_NAMESPACE,
                            "nocache"                   => CMS_TEMPLATE_TYPE_NOCACHE,
                            "control"                   => CMS_TEMPLATE_TYPE_CONTROL,
                            "backlink"                  => CMS_TEMPLATE_TYPE_BACKLINK,
                            "stylesheet"                => CMS_TEMPLATE_TYPE_STYLESHEET,
                            "javascript"                => CMS_TEMPLATE_TYPE_JAVASCRIPT,
                            "switch"                    => CMS_TEMPLATE_TYPE_SWITCH,
                            "edit"                      => CMS_TEMPLATE_TYPE_CEDIT,
                            "noedit"                    => CMS_TEMPLATE_TYPE_CNOEDIT];
    public $tname        = [CMS_TEMPLATE_TYPE_NONE      => CMS_L_UNKNOWN,
                            CMS_TEMPLATE_TYPE_HREF      => CMS_L_TEMPLATE_039,
                            CMS_TEMPLATE_TYPE_PLUGIN    => CMS_L_TEMPLATE_003,
                            CMS_TEMPLATE_TYPE_TEXT      => CMS_L_TEMPLATE_029,
                            CMS_TEMPLATE_TYPE_VALUE     => CMS_L_TEMPLATE_043,
                            CMS_TEMPLATE_TYPE_DOWNLOAD  => CMS_L_TEMPLATE_030,
                            CMS_TEMPLATE_TYPE_IMAGE     => CMS_L_TEMPLATE_031,
                            CMS_TEMPLATE_TYPE_THUMBNAIL => CMS_L_TEMPLATE_032,
                            CMS_TEMPLATE_TYPE_MEDIA     => CMS_L_TEMPLATE_033,
                            CMS_TEMPLATE_TYPE_TEMPLATE  => CMS_L_TEMPLATE_034,
                            CMS_TEMPLATE_TYPE_GROUP     => CMS_L_TEMPLATE_035,
                            CMS_TEMPLATE_TYPE_REPEAT    => CMS_L_TEMPLATE_036,
                            CMS_TEMPLATE_TYPE_SHIFT     => CMS_L_TEMPLATE_037,
                            CMS_TEMPLATE_TYPE_SWITCH    => CMS_L_TEMPLATE_038];
    public $toption      = [CMS_TEMPLATE_TYPE_HREF      => CMS_TEMPLATE_OPTION_HREF,
                            CMS_TEMPLATE_TYPE_PLUGIN    => CMS_TEMPLATE_OPTION_PLUGIN,
                            CMS_TEMPLATE_TYPE_TEXT      => CMS_TEMPLATE_OPTION_TEXT,
                            CMS_TEMPLATE_TYPE_VALUE     => CMS_TEMPLATE_OPTION_VALUE,
                            CMS_TEMPLATE_TYPE_DOWNLOAD  => CMS_TEMPLATE_OPTION_DOWNLOAD,
                            CMS_TEMPLATE_TYPE_IMAGE     => CMS_TEMPLATE_OPTION_IMAGE,
                            CMS_TEMPLATE_TYPE_THUMBNAIL => CMS_TEMPLATE_OPTION_THUMBNAIL,
                            CMS_TEMPLATE_TYPE_MEDIA     => CMS_TEMPLATE_OPTION_MEDIA,
                            CMS_TEMPLATE_TYPE_TEMPLATE  => CMS_TEMPLATE_OPTION_TEMPLATE,
                            CMS_TEMPLATE_TYPE_GROUP     => CMS_TEMPLATE_OPTION_GROUP,
                            CMS_TEMPLATE_TYPE_REPEAT    => CMS_TEMPLATE_OPTION_REPEAT,
                            CMS_TEMPLATE_TYPE_SHIFT     => CMS_TEMPLATE_OPTION_SHIFT,
                            CMS_TEMPLATE_TYPE_SWITCH    => CMS_TEMPLATE_OPTION_SWITCH];
    public $action       = NULL;
    public $image        = NULL;
    public $media        = NULL;
    public $download     = NULL;
    public $structure_id = NULL;
    public $parent_id    = NULL;
    public $execute_vars = [];

    //meta data
    public $title        = ""; //document title
    public $description  = ""; //document description
    public $keyword      = ""; //document keywords
    public $header       = ""; //additional header insert
    public $query_data   = []; //querystring data

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct()
    {
        $this->data        = new data("#system/template");
        $this->operator    = cms_permission(CMS_TEMPLATE_PERMISSION_OPERATOR);
        $this->compat_mode = stre((new system())->getval("template", "version"));

        mkpath(CMS_DATA_PATH . "#template");
        mkpath(CMS_DATA_PATH . "template/stylesheet");
        mkpath(CMS_DATA_PATH . "template/javascript");
    }

//methods
//--------------------------------------------------------------------------------------------------

    function add($name,
                 $category   = NULL,
                 $page       = NULL,
                 $code       = NULL,
                 $stylesheet = NULL,
                 $javascript = NULL)
    {
        if (! $this->operator) return FALSE;

        //set default name if none given
        if (stre(language_get($name, FALSE)))
            $name = language_set($name, CMS_L_TEMPLATE_001, FALSE);

        //prepare data for insert
        $this->data->set_buffer(
            [["name"     => $name,
              "category" => $category,
              "page"     => (bool)$page]]);

        //add data and save
        if (
             ($return = $this->data->insert())
             &&
             $this->set_code($return, $code)
             &&
             $this->set_stylesheet($return, $stylesheet)
             &&
             $this->set_javascript($return, $javascript)
             &&
             $this->data->save()
           )
            return $return;

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function set($index,
                 $name     = NULL,
                 $category = NULL,
                 $page     = NULL)
    {
        if (! $this->operator) return FALSE;

        //set default name if none given
        if (stre(language_get($name, FALSE)))
            $name = language_set($name, CMS_L_TEMPLATE_001, FALSE);

        //update dataset
        $this->data->set(
            ["name"     => $name,
             "category" => $category,
             "page"     => (bool)$page],
            $index);

        //save
        return $this->data->save();
    }

//--------------------------------------------------------------------------------------------------

    function get($index)
    {
        return $this->data->get($index);
    }

//--------------------------------------------------------------------------------------------------

    private function asset_path($type)
    {
        switch ($type)
        {
        case CMS_TEMPLATE_ASSET_TYPE_CODE:       return "#template/%s%s.htm";
        case CMS_TEMPLATE_ASSET_TYPE_STYLESHEET: return "template/stylesheet/%s%s.css";
        case CMS_TEMPLATE_ASSET_TYPE_JAVASCRIPT: return "template/javascript/%s%s.js";
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    private function set_asset($index,
                               &$text,
                               $type)
    {
        if (! $this->operator) return FALSE;

        $path = $this->asset_path($type);
        if ($path === FALSE) return FALSE;

        $array = language_get_array($text);
        $error = FALSE;

        foreach ($array AS $key => $value)
        {
            if (nstre($key)) $key .= ".";

            $_path = CMS_DATA_PATH . sprintf($path, $key, $index);

            //delete file
            if (! $value)
            {
                if (is_file($_path)) $error = ! unlink($_path);
                continue;
            };

            //write file
            if ($hfile = fopen($_path, "wb"))
            {
                flock($hfile, LOCK_EX);
                fwrite($hfile, $value);
                fclose($hfile);
            }
            else
            {
                $error = TRUE;
            };
        };

        return ! $error;
    }

//--------------------------------------------------------------------------------------------------

    private function get_asset($index,
                               $language,
                               $check,
                               $type)
    {
        $path = $this->asset_path($type);
        if ($path === FALSE) return FALSE;

        //multiple language string
        if ($language === FALSE)
        {
            $return   = NULL;
            $language = CMS_LANGUAGE_ENABLED ?
                        array_merge([""], explode(",", CMS_LANGUAGE_ENABLED)) :
                        [""];

            foreach ($language AS $key => $value)
            {
                //retrieve from file
                $key    = nstre($value) ? "$value." : "";
                $text   = read_file(CMS_DATA_PATH . sprintf($path, $key, $index));
                $return = language_set($return, $text, $value);
            };

            return $return;
        };

        //active language string
        $language ??= CMS_LANGUAGE;
        if (nstre($language)) $language .= ".";

        //retrieve from file
        $_path = sprintf($path, $language, $index);
        $text  = $check ? is_file(CMS_DATA_PATH . $_path) : read_file(CMS_DATA_PATH . $_path);

        //fallback
        if (nstre($language) && stre($text))
        {
            //retrieve from file
            $_path = sprintf($path, "", $index);
            $text  = $check ? is_file(CMS_DATA_PATH . $_path) : read_file(CMS_DATA_PATH . $_path);
        };

        return $check ? (nstre($text) ? $_path : FALSE) : $text;
    }

//--------------------------------------------------------------------------------------------------

    function set_code($index,
                      $text)
    {
        if ($this->set_asset($index, $text, CMS_TEMPLATE_ASSET_TYPE_CODE))
        {
            //invalidate content cache
            touch(CMS_DATA_PATH . "#system/template.dat");

            return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function get_code($index,
                      $language = NULL)
    {
        //multiple language string
        if ($language === FALSE)
            $this->get_asset($index, FALSE, FALSE, CMS_TEMPLATE_ASSET_TYPE_CODE);

        //active language string
        $_language = $language ?? CMS_LANGUAGE;
        if (nstre($_language)) $_language .= ".";

        //retrieve from temporary cache
        if (($text = cms_cache("template.$_language$index")) !== NULL) return $text;

        $text = $this->get_asset($index, $language, FALSE, CMS_TEMPLATE_ASSET_TYPE_CODE);

        //temporary caching
        cms_cache("template.$_language$index", $text);

        return $text;
    }

//--------------------------------------------------------------------------------------------------

    function set_stylesheet($index,
                            $text)
    {
        return $this->set_asset($index, $text, CMS_TEMPLATE_ASSET_TYPE_STYLESHEET);
    }

//--------------------------------------------------------------------------------------------------

    function get_stylesheet($index,
                            $language = NULL,
                            $check    = FALSE)
    {
        return $this->get_asset($index, $language, $check, CMS_TEMPLATE_ASSET_TYPE_STYLESHEET);
    }

//--------------------------------------------------------------------------------------------------

    function set_javascript($index,
                            $text)
    {
        return $this->set_asset($index, $text, CMS_TEMPLATE_ASSET_TYPE_JAVASCRIPT);
    }

//--------------------------------------------------------------------------------------------------

    function get_javascript($index,
                            $language = NULL,
                            $check    = FALSE)
    {
        return $this->get_asset($index, $language, $check, CMS_TEMPLATE_ASSET_TYPE_JAVASCRIPT);
    }

//--------------------------------------------------------------------------------------------------

    function delete($index)
    {
        if (! $this->operator) return FALSE;
        if (! $index)          return FALSE;

        $language = CMS_LANGUAGE_ENABLED ?
                    array_merge([""], explode(",", CMS_LANGUAGE_ENABLED)) :
                    [""];

        foreach ($language AS $key => $value)
        {
            $key = $value ? "$value." : "";

            if (is_file(CMS_DATA_PATH . "#template/$key$index.htm"))
            {
                if (! unlink(CMS_DATA_PATH . "#template/$key$index.htm")) return FALSE;

                //remove from temporary cache
                cms_cache("template.$key$index", "");
            };

            //delete stylesheet
            if (is_file(CMS_DATA_PATH . "template/stylesheet/$key$index.css"))
                unlink(CMS_DATA_PATH . "template/stylesheet/$key$index.css");

            //delete javascript
            if (is_file(CMS_DATA_PATH . "template/javascript/$key$index.js"))
                unlink(CMS_DATA_PATH . "template/javascript/$key$index.js");
        };

        $this->data->del($index);

        return $this->data->save();
    }

//--------------------------------------------------------------------------------------------------

    function parse(&$document,
                   $index,
                   $title       = NULL,
                   $description = NULL,
                   $keyword     = NULL,
                   $action      = NULL,
                   $header      = NULL,
                   $base_id     = NULL, //do not change for page template
                   $cache       = FALSE)
    {
        $array = [];

        //link stylesheet of employed template
        if (($file = $this->get_stylesheet($index, NULL, TRUE)) !== FALSE)
            $array[] = stylesheet(CMS_DATA_URL . $file, FALSE);

        //link javascript of employed template
        if (($file = $this->get_javascript($index, NULL, TRUE)) !== FALSE)
            $array[] = javascript(CMS_DATA_URL . $file, FALSE);

        $this->action      = $action;
        $this->title       = l($title);
        $this->description = l($description);
        $this->keyword     = l($keyword);
        $this->header      = implode("\n", $array) . (nstre($header) ? "\n$header" : "");

        return $this->_parse($document, $this->get_code($index), $base_id, $cache, $index);
    }

//--------------------------------------------------------------------------------------------------

    function parse_code(&$document,
                        $template,
                        $title       = NULL,
                        $description = NULL,
                        $keyword     = NULL,
                        $action      = NULL,
                        $header      = NULL,
                        $base_id     = NULL) //do not change for page template
    {
        $this->action      = $action;
        $this->title       = l($title);
        $this->description = l($description);
        $this->keyword     = l($keyword);
        $this->header      = $header;

        return $this->_parse($document, $template, $base_id);
    }

//--------------------------------------------------------------------------------------------------

    private function _parse(&$document,              //data source
                            $template,               //template source code
                            $base_id        = NULL,  //do not change for page template
                            $cache          = FALSE, //true = caching, false = no caching, -1 = finish preprocessed nocache span
                            $template_index = NULL)  //do not change for page template
    {
        //static variables
        static $template_cache = [];    //template data:
                                        //[0] => template code
                                        //[1] => tokenized php
                                        //[2] => php offset
                                        //[3] => template name
                                        //[4] => stylesheet path
                                        //[5] => javascript path
        static $template_depth = -1;    //template nesting counter
        static $preview        = FALSE; //preview mode
        static $debug          = FALSE; //debug mode
        static $base_url       = NULL;  //base url
        static $stylesheet     = NULL;  //page stylesheet
        static $title_wrapper  = NULL;  //page title wrapper
        static $insert         = -1;    //header insert position

        //add path separator
        if (nstre($base_id)) $base_id .= ".";

        //increase template nesting counter
        $template_depth++;

        //initialize
        if (! $template_depth) //zero on first pass
        {
            //load image extension
            if ((! $this->image) && cms_load("image"))
                $this->image = new image();

            //load media extension
            if ((! $this->media) && cms_load("media"))
                $this->media = new media();

            //load download extension
            if ((! $this->download) && cms_load("download"))
                $this->download = new download();

            //static variables
            $template_cache = [];
            $preview        = defined("CMS_TEMPLATE_PREVIEW");
            $debug          = (! $cache) && isset($this->action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_DEBUG]);
            $base_url       = NULL;
            $stylesheet     = NULL;
            $title_wrapper  = NULL;
            $insert         = -1;

            //misplaced but solves it centrally
            if (! defined("CMS_CONTENT_META_ROBOTS")) define("CMS_CONTENT_META_ROBOTS", "noindex, nofollow");

            //scripting constants
            if (! defined("CMS_TEMPLATE"))        define("CMS_TEMPLATE",      TRUE);
            if (! defined("CMS_TEMPLATE_EDIT"))   define("CMS_TEMPLATE_EDIT", (! $cache) && isset($this->action[CMS_TEMPLATE_ACTION]));
            if (! defined("CMS_TEMPLATE_OPTION")) define("CMS_TEMPLATE_OPTION",
                (isset($this->action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_HREF])      * CMS_TEMPLATE_OPTION_HREF)      |
                (isset($this->action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_PLUGIN])    * CMS_TEMPLATE_OPTION_PLUGIN)    |
                (isset($this->action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_TEXT])      * CMS_TEMPLATE_OPTION_TEXT)      |
                (isset($this->action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_VALUE])     * CMS_TEMPLATE_OPTION_VALUE)     |
                (isset($this->action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_DOWNLOAD])  * CMS_TEMPLATE_OPTION_DOWNLOAD)  |
                (isset($this->action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_IMAGE])     * CMS_TEMPLATE_OPTION_IMAGE)     |
                (isset($this->action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_THUMBNAIL]) * CMS_TEMPLATE_OPTION_THUMBNAIL) |
                (isset($this->action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_MEDIA])     * CMS_TEMPLATE_OPTION_MEDIA)     |
                (isset($this->action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_TEMPLATE])  * CMS_TEMPLATE_OPTION_TEMPLATE)  |
                (isset($this->action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_GROUP])     * CMS_TEMPLATE_OPTION_GROUP)     |
                (isset($this->action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_REPEAT])    * CMS_TEMPLATE_OPTION_REPEAT)    |
                (isset($this->action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_SHIFT])     * CMS_TEMPLATE_OPTION_SHIFT)     |
                (isset($this->action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_CBLOCK])    * CMS_TEMPLATE_OPTION_CBLOCK)    |
                (isset($this->action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_CALT])      * CMS_TEMPLATE_OPTION_CALT)      |
                (isset($this->action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_SWITCH])    * CMS_TEMPLATE_OPTION_SWITCH)    |
                ($debug                                                                 * CMS_TEMPLATE_OPTION_DEBUG));
        };

        //replace tokens in template code
        $template_index = (string)$template_index;

        if ($cache !== -1)
        {
            if (! isset($template_cache[$template_index][0]))
                $template_cache[$template_index][0] = parse_token($template);

            $template = &$template_cache[$template_index][0];
        };

        if (stre($template)) return FALSE; //template is empty

        //parse php
        if (! isset($template_cache[$template_index][1]))
        {
            $template_cache[$template_index][1] = token_get_all($template);
            $template_cache[$template_index][2] = [];

            $list    = &$template_cache[$template_index][1];
            $offset  = &$template_cache[$template_index][2];
            $_offset = 0;

            foreach ($list AS $key => $value)
            {
                if (! is_array($value))
                {
                    $_offset += strlen($value);
                    continue;
                };

                if (($value[0] === T_OPEN_TAG) || ($value[0] === T_OPEN_TAG_WITH_ECHO))
                    $offset[$_offset] = $key;

                $_offset += strlen($value[1]);
            };

            unset($list, $offset);
        };

        //initialize local variables
        $offset          = 0;                 //primary text position
        $_offset         = 0;                 //secondary text position
        $length          = strlen($template); //template code length

        $auto_id         = 0;                 //automatic element index
        $_base_id        = $base_id;          //base index working copy
        $no_id           = TRUE;              //no explicit id given

        $auto_id_stack   = [];                //automatic element index stack
        $span_stack      = [];                //open span stack
        $group_stack     = [];                //group data stack
        $repeat_stack    = [];                //repeat data stack
        $shift_stack     = [];                //shift data stack
        $base_stack      = [];                //base data stack
        $namespace_stack = [];                //namespace data stack
        $debug_stack     = [];                //debug information stack
        $temp            = crc32_base62((defined("CMS_CONTENT_INDEX") ? CMS_CONTENT_INDEX : 0) . ":$base_id");
        $temp_stack      = [$temp];           //temporary random index stack

        $cblock_depth    = 0;                 //conditional block depth
        $cblock_limit    = 0;                 //conditional block limit
        $calt_depth      = 0;                 //conditional alternative depth
        $calt_limit      = 0;                 //conditional alternative limit
        $nocache_depth   = 0;                 //nocache depth
        $nocache_buffer  = "";                //nocache buffer
        $nocache_insert  = 0;                 //nocache insert position
        $edit_depth      = 0;                 //edit depth
        $edit_limit      = 0;                 //edit limit
        $noedit_depth    = 0;                 //noedit depth
        $noedit_limit    = 0;                 //noedit limit

        $output          = "";                //output buffer

        //process template code
        while (
                ($offset < $length) //not at end of template code
                &&
                (($offset = strpos($template, "<", $offset)) !== FALSE) //seek next opening tag
              )
        {
            $suppressed = $cblock_limit || $calt_limit || $edit_limit || $noedit_limit;

            if (! $suppressed)
            {
                //get preliminary unparsed data
                $buffer = substr($template, $_offset, $offset - $_offset);

                //append to nocache buffer
                if ($nocache_depth) $nocache_buffer .= $buffer;

                //append to output
                $output .= $buffer;
            };

//..................................................................................................

            //php code
            if (isset($template_cache[$template_index][2][$offset]))
            {
                $list     = &$template_cache[$template_index][1];
                $flag     = FALSE;   //output flag
                $_length  = 0;       //length of opening php tag including trailing whitespace
                $__length = 0;       //length of closing php tag
                $_offset  = $offset; //set offset to start of php code
                $result   = TRUE;

                for ($i = $template_cache[$template_index][2][$offset], $c = count($list); $i < $c; $i++)
                {
                    $value = $list[$i];

                    if (is_array($value))
                    {
                        $_offset += strlen($value[1]);

                        switch ($value[0])
                        {
                        case T_OPEN_TAG_WITH_ECHO:

                            $flag = TRUE;

                        case T_OPEN_TAG:

                            $_length = strlen(rtrim($value[1]));
                            break;

                        case T_CLOSE_TAG:

                            $__length = strlen($value[1]);
                            break 2;
                        };

                        continue;
                    };

                    $_offset += strlen($value);
                };

                unset($list);

                //not suppressed
                if (! $suppressed)
                {
                    //append unparsed source code if within nocache block
                    if ($nocache_depth) $nocache_buffer .= substr($template, $offset, $_offset - $offset);

                    //get php source code
                    $__offset = $offset + $_length;
                    $buffer   = substr($template, $__offset, $_offset - $__offset - $__length);

                    //apply short output
                    if ($flag) $buffer = "echo $buffer;";

                    //set current template index
                    $GLOBALS["template_error_index"] = $template_index;

                    //set number of lines preceding script block
                    $GLOBALS["template_error_line"] = ($offset > 0) ? substr_count($template, "\n", 0, $offset) + 1 : 1;

                    //start new output buffer
                    ob_start();

                    //check for syntax errors
                    if (eval("return TRUE; $buffer") === FALSE)
                    {
                        //get error information
                        $error = error_get_last();

                        //display error
                        template_error($error["type"], $error["message"], NULL, $error["line"]);
                    }

                    //execute script
                    else
                    {
                        //execute script
                        $result = $this->execute($buffer, $document, $base_id, $_base_id, end($temp_stack));
                    };

                    //append function output
                    $output .= ob_get_clean();
                };

                //continue after php code
                $offset = $_offset;

                //skip processing of remaining template if script returns false explicitly
                if ($result === FALSE) $offset = $_offset = $length;
            }

//..................................................................................................

            //start tag
            elseif (
                     (($offset + 5) < $length)        //considers upcoming strpos
                     &&
                     ($template[$offset + 1] === "C") //fast check of initial tag characters
                     &&
                     ($template[$offset + 2] === "M")
                     &&
                     ($template[$offset + 3] === "S")
                     &&
                     ($template[$offset + 4] === ":")
                   )
            {
                if (($_offset = strpos($template, ">", $offset + 5)) !== FALSE) //seek tag end
                {
                    $_offset++; //advance position behind tag
                    $string = substr($template, $offset, $_offset - $offset); //get tag code

                    //verify tag
                    if (
                         preg_match("/^<CMS:([a-z]+)(?:\s|\/|>)/", $string, $_string) //extensive check
                         &&
                         isset($this->tlist[$_string[1]]) //check if element is listed
                       )
                    {
                        //append unparsed element code to nocache buffer
                        if ($nocache_depth) $nocache_buffer .= $string;

                        $buffer = ""; //reset output buffer

                        //template type
                        $_ttype = $_string[1];           //literal
                        $ttype  = $this->tlist[$_ttype]; //numeric

                        //switch -> value
                        if ($ttype === CMS_TEMPLATE_TYPE_SWITCH)    $_ttype = "value";

                        //thumbnail -> image
                        if ($ttype === CMS_TEMPLATE_TYPE_THUMBNAIL) $_ttype = "image";

                        //extract attributes
                        $attribute = template_get_attribute($string);

                        //store and reset automatic index on spannable path extending element
                        if (
                             (! $this->compat_mode) //skip for older versions
                             &&
                             ($ttype & CMS_TEMPLATE_TYPE_SPAN & CMS_TEMPLATE_TYPE_PATH)
                           )
                        {
                            $auto_id_stack[] = $auto_id;
                            $auto_id         = 0;
                        };

                        //id
                        $no_id = ! $this->parse_id(
                            $attribute["id"] ?? NULL,
                            $base_id,
                            $_base_id,
                            $auto_id,
                            $id,   //data index
                            $_id); //path index

                        //generate unique element index
                        $index = crc32_base62($_id);

                        //reset content name
                        $name  = NULL;

                        if ($ttype & CMS_TEMPLATE_TYPE_EDIT) //element is editable (can have data assigned)
                        {
                            //default value
                            if (isset($attribute["default"]))
                                $document->set_default($id, $_ttype, $attribute["default"]);

                            //edit option
                            $edit    = (! isset($attribute["edit"])) ||
                                       (! preg_match("/^\s*(?:off|no|false)\s*$/i", $attribute["edit"]));

                            //element title
                            $_title  = $attribute["title"] ?? $this->tname[$ttype];

                            //content value
                            $_cvalue = $document->get($id, $_ttype);
                            $cvalue  = stre($_cvalue) ? NULL : l($_cvalue);
                        }

                        else //not an editable element
                        {
                            $edit    = FALSE; //edit option
                            $_title  = NULL;  //element title
                            $_cvalue = NULL;  //raw content value
                            $cvalue  = NULL;  //content value
                        };

                        //set if element is displayed for editing
                        $edit_display =

                        //editing active
                            CMS_TEMPLATE_EDIT
                            &&

                        //editing is enabled for current element
                            $edit
                            &&

                        //not suppressed
                            (! $suppressed)
                            &&

                        //current element is selected for editing, implies action set
                            ($this->toption[$ttype] & CMS_TEMPLATE_OPTION);

                        //close open href/download tags before elements that are displayed for editing
                        if ($edit_display && (! $preview))
                        {
                            $array = [CMS_TEMPLATE_TYPE_HREF, CMS_TEMPLATE_TYPE_DOWNLOAD];

                            foreach ($array as $value)
                            {
                                if (! isset($span_stack[$value])) continue;

                                $i = count($span_stack[$value]);
                                while ($i && ($span_stack[$value][--$i] === FALSE))
                                {
                                    //display button to activate link
                                    $output .= "<img src=\"" . x(CMS_IMAGES_URL) . "content/button_open.svg\" " .
                                                    "alt=\"\">" .
                                               "</a>";

                                    //mark done
                                    $span_stack[$value][$i] = 0;
                                };
                            };
                        };

                        //memorize spannable element as open
                        if ($ttype & CMS_TEMPLATE_TYPE_SPAN)
                        {
                            $span_stack[$ttype] ??= [];
                            $value = $edit_display ? TRUE : ($suppressed ? 0 : FALSE);
                            array_push($span_stack[$ttype], $value); //add to span stack
                        };

                        //process element by type
                        switch ($ttype)
                        {
                        case CMS_TEMPLATE_TYPE_HEAD:
//..................................................................................................

                            /*
                                <CMS:head [base="{url}"]
                                          [stylesheet="on|yes|true|off|no|false|{url}"]
                                          [title="{text}"]/>
                            */

                            //base url
                            if (
                                 isset($attribute["base"])
                                 &&
                                 (! preg_match("/^\s*(?:on|yes|true)\s*$/i", $attribute["base"]))
                               )
                            {
                                //use custom base url if given
                                if (! preg_match("/^\s*(?:off|no|false)\s*$/i", $attribute["base"]))
                                    $base_url = $attribute["base"];
                            }

                            //use default base url
                            else
                            {
                                $base_url = CMS_ROOT_URL;
                            };

                            //page stylesheet
                            if (
                                 isset($attribute["stylesheet"])
                                 &&
                                 (! preg_match("/^\s*(?:on|yes|true)\s*$/i", $attribute["stylesheet"]))
                               )
                            {
                                //use custom stylesheet if given
                                if (! preg_match("/^\s*(?:off|no|false)\s*$/i", $attribute["stylesheet"]))
                                    $stylesheet = stylesheet($attribute["stylesheet"], FALSE);
                            }

                            //use default stylesheet
                            else
                            {
                                $stylesheet = CMS_STYLESHEET;
                            };

                            //title
                            if (isset($attribute["title"]))
                                $title_wrapper = $attribute["title"];

                            //set insert position
                            $insert = strlen($output);
                            break;

                        case CMS_TEMPLATE_TYPE_CONTROL:
//..................................................................................................

                            /*
                                <CMS:control/>
                            */

                            if (! CMS_TEMPLATE_EDIT) break; //skip if not in edit mode

                            //no control functions defined
                            if (! isset($this->action[CMS_TEMPLATE_CONTROL])) break;

                            $buffer .= "<input id=\"tp-ctrl-switch\" " .
                                              "type=\"checkbox\" " .
                                              "value=\"1\">";

                            //control marker
                            while (TRUE)
                            {
                                //display document info
                                while (defined("CMS_CONTENT_INDEX"))
                                {
                                    $query = "SELECT " . CMS_DB_CONTENT_WRITER_TIME . ", " .
                                                         CMS_DB_CONTENT_TIME . ", " .
                                                         CMS_DB_CONTENT_BUFFER_TITLE . " " .
                                             "FROM " .   CMS_DB_CONTENT . " " .
                                             "WHERE " .  CMS_DB_CONTENT_INDEX .
                                             "='" .      sqlesc(CMS_CONTENT_INDEX) . "' " .
                                             "LIMIT 1";

                                    if (
                                         (($result = mysql_query($query)) === FALSE)
                                         ||
                                         (! is_array($resultrow = mysql_fetch_row($result)))
                                       )
                                        break; //error or no record

                                    //up to date
                                    if ($resultrow[0] === $resultrow[1])
                                        $buffer .= "<label id=\"tp-marker\" " .
                                                          "for=\"tp-ctrl-switch\" " .
                                                          "onmouseenter=\"this.click();\">" .
                                                   "(" . x(CMS_CONTENT_INDEX) . ") " . x(l($resultrow[2])) .
                                                   "</label>";

                                    //edited
                                    else
                                        $buffer .= "<label id=\"tp-marker\" " .
                                                          "for=\"tp-ctrl-switch\" " .
                                                          "class=\"tp-mod\" " .
                                                          "onmouseenter=\"this.click();\">" .
                                                   "(" . x(CMS_CONTENT_INDEX) . ") " . x(l($resultrow[2])) . " " .
                                                   "(" . CMS_L_TEMPLATE_049 . " " . x(friendly_date($resultrow[0])) . ")" .
                                                   "</label>";
                                    break 2;
                                };

                                //fallback
                                $buffer .= "<label id=\"tp-marker\" " .
                                                  "for=\"tp-ctrl-switch\" " .
                                                  "onmouseenter=\"this.click();\">" .
                                           CMS_L_TEMPLATE_048 .
                                           "</label>";
                                break;
                            };

                            //control pad
                            $buffer .= "<div id=\"tp-ctrl\">";

                            //display element edit switch
                            if (isset($this->action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_SWITCH][CMS_TEMPLATE_CODE]))
                            {
                                //presets
                                $buffer .= "<div class=\"tp-ctrl-title\">" .
                                           CMS_L_TEMPLATE_017 .
                                           "</div>" .

                                           "<div id=\"tp-ctrl-opt-set\">";

                                //view modes
                                $array = [[CMS_TEMPLATE_OPTION_ALL,    "f", "F", "command_view_full",   CMS_L_TEMPLATE_012],
                                          [CMS_TEMPLATE_OPTION_LAYOUT, "l", "L", "command_view_layout", CMS_L_TEMPLATE_015],
                                          [CMS_TEMPLATE_OPTION_EDIT,   "c", "C", "command_view_edit",   CMS_L_TEMPLATE_016],
                                          [CMS_TEMPLATE_OPTION_NONE,   "v", "V", "command_view",        CMS_L_TEMPLATE_013]];

                                foreach ($array as $key => $value)
                                {
                                    $_value  = (CMS_TEMPLATE_OPTION === $value[0]) ? " class=\"tp-ctrl-opt-set-active\"" : "";
                                    $buffer .= "<button type=\"button\" " .
                                                       "onclick=\"tp_ctrl_opt_set(" . $value[0] . ");\" " .
                                                       "accesskey=\"" . $value[1] . "\" " .
                                                       "title=\"[Alt] + [" . $value[2] . "]\"" . $_value . ">" .
                                               image("template/" . $value[3]) . " " . $value[4] .
                                               "</button>";
                                };

                                $buffer .= "</div>" .

                                //elements
                                           "<div class=\"tp-ctrl-title\">" .
                                           CMS_L_TEMPLATE_051 .
                                           "</div>" .

                                           "<div id=\"tp-ctrl-opt-switch\">";

                                //option switches
                                $array = [[CMS_TEMPLATE_OPTION_TEXT,      CMS_L_TEMPLATE_007, "button_text"],
                                          [CMS_TEMPLATE_OPTION_VALUE,     CMS_L_TEMPLATE_044, "button_value"],
                                          [CMS_TEMPLATE_OPTION_SWITCH,    CMS_L_TEMPLATE_055, "button_switch"],
                                          [CMS_TEMPLATE_OPTION_IMAGE,     CMS_L_TEMPLATE_009, "button_image"],
                                          [CMS_TEMPLATE_OPTION_THUMBNAIL, CMS_L_TEMPLATE_010, "button_thumbnail"],
                                          [CMS_TEMPLATE_OPTION_MEDIA,     CMS_L_TEMPLATE_021, "button_media"],
                                          [CMS_TEMPLATE_OPTION_HREF,      CMS_L_TEMPLATE_005, "button_href"],
                                          [CMS_TEMPLATE_OPTION_DOWNLOAD,  CMS_L_TEMPLATE_008, "button_download"],
                                          [CMS_TEMPLATE_OPTION_PLUGIN,    CMS_L_TEMPLATE_006, "button_plugin"],
                                          [CMS_TEMPLATE_OPTION_TEMPLATE,  CMS_L_TEMPLATE_011, "button_template"],
                                          [CMS_TEMPLATE_OPTION_GROUP,     CMS_L_TEMPLATE_028, "button_group"],
                                          [CMS_TEMPLATE_OPTION_REPEAT,    CMS_L_TEMPLATE_020, "button_repeat"],
                                          [CMS_TEMPLATE_OPTION_SHIFT,     CMS_L_TEMPLATE_022, "button_shift"],
                                          [CMS_TEMPLATE_OPTION_CBLOCK,    CMS_L_TEMPLATE_004, "button_cblock"],
                                          [CMS_TEMPLATE_OPTION_CALT,      CMS_L_TEMPLATE_023, "button_calt"],
                                          [CMS_TEMPLATE_OPTION_DEBUG,     CMS_L_TEMPLATE_047, "button_debug", $debug ? TRUE : NULL]];

                                foreach ($array as $key => $value)
                                {
                                    $_value  = ($value[3] ?? CMS_TEMPLATE_OPTION & $value[0]) ? "" : "_disabled";
                                    $buffer .= "<button id=\"tp-ctrl-opt-$key\" " .
                                                       "type=\"button\" " .
                                                       "onclick=\"tp_ctrl_opt_switch(" . $value[0] . ");\" " .
                                                       "title=\"" . $value[1] . "\">" .
                                               image("content/" . $value[2] . $_value) .
                                               "</button>";
                                };

                                //apply option
                                $buffer .= "<button id=\"tp-ctrl-opt-apply\" " .
                                                   "type=\"button\" " .
                                                   "onclick=\"" . x($this->action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_SWITCH][CMS_TEMPLATE_CODE]) . "\">" .
                                           CMS_L_COMMAND_CONFIRM .
                                           "</button>" .

                                           "</div>" .

                                //set current selection
                                           jscript("tp_ctrl_opt_value = " . CMS_TEMPLATE_OPTION . ";" .

                                //set path to images
                                                   "tp_ctrl_opt_img_url = \"" . q(CMS_IMAGES_URL) . "content/\";");
                            };

                            //extended commands
                            if (
                                 isset($this->action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_COMMAND])
                                 &&
                                 is_array($this->action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_COMMAND])
                               )
                            {
                                $buffer .= "<div class=\"tp-ctrl-title\">" .
                                           CMS_L_TEMPLATE_048 .
                                           "</div>" .
                                           "<div id=\"tp-ctrl-cmd\">";
                                $array   = $this->action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_COMMAND];

                                //process commands and display as button
                                foreach ($array AS $key => $value)
                                    $buffer .= "<button type=\"button\" " .
                                                       "onclick=\"" . x($value) . "\">" .
                                               x($key) .
                                               "</button>";

                                $buffer .= "</div>";
                            };

                            //language selector
                            if (CMS_LANGUAGE_ENABLED)
                            {
                                $map       = new map("#system/language.image");
                                $buffer   .= "<div id=\"tp-ctrl-language\">" .
                                             CMS_L_TEMPLATE_052 . ": ";
                                $language  = explode(",", CMS_LANGUAGE_ENABLED);

                                foreach ($language AS $value)
                                {
                                    $url     = cms_url(["cms_select_language" => $value]);
                                    $buffer .= "<a href=\"" . x($url) . "\" " .
                                                  "title=\"" . x($value) . "\"" .
                                                   (streq($value, CMS_LANGUAGE) ?
                                                 " class=\"tp-ctrl-language-active\">" : ">") .
                                               image(((($image = $map->get_value($value)) !== NULL) &&
                                                     is_file(CMS_DATA_PATH . "language/$image")) ?
                                                     CMS_DATA_URL . "language/$image" :
                                                     "language/default",
                                                     16, 12) .
                                               "</a>";
                                };

                                $buffer .= "</div>";
                            };

                            //permission editor
                            if (defined("CMS_CONTENT_INDEX"))
                            {
                                $_buffer = permission(
                                    [CMS_CONTENT_PERMISSION_READER . "." . CMS_CONTENT_INDEX => CMS_L_READ,
                                     CMS_CONTENT_PERMISSION_WRITER . "." . CMS_CONTENT_INDEX => CMS_L_WRITE],
                                    TRUE);

                                if (nstre($_buffer))
                                    $buffer .= "<div id=\"tp-ctrl-permission\">" .
                                               $_buffer .
                                               "</div>";
                            };

                            $buffer .= "</div>"; //control pad

                            break;

                        case CMS_TEMPLATE_TYPE_MENU:
//..................................................................................................

                            /*
                                <CMS:menu [base="{id}"]
                                          [level="{number}|+{number}|-{number}"]
                                          [depth="off|no|false|{number}"]
                                          [filter="normal|open|active|active-open"]
                                          [show-icons="on|yes|true|off|no|false"]
                                          [show-images="on|yes|true|off|no|false"]
                                          [show-description="on|yes|true|off|no|false"]
                                          [show-hidden="on|yes|true|off|no|false"]
                                          [exclude="{id}[ {id}]"]/>
                            */

                            if ($suppressed)        break; //conditionally suppressed
                            if (! cms_load("menu")) break; //library unavailable

                            $base             = $attribute["base"] ?? 0;
                            $level            = $attribute["level"] ?? 0;
                            $depth            = (isset($attribute["depth"]) &&
                                                (! preg_match("/^\s*(?:off|no|false)\s*$/i", $attribute["depth"]))) ?
                                                $attribute["depth"] : NULL;
                            $filter           = $attribute["filter"] ?? NULL;
                            $show_icons       = isset($attribute["show-icons"]) &&
                                                preg_match("/^\s*(?:on|yes|true)\s*$/i", $attribute["show-icons"]);
                            $show_images      = isset($attribute["show-images"]) &&
                                                preg_match("/^\s*(?:on|yes|true)\s*$/i", $attribute["show-images"]);
                            $show_description = isset($attribute["show-description"]) &&
                                                preg_match("/^\s*(?:on|yes|true)\s*$/i", $attribute["show-description"]);
                            $show_hidden      = isset($attribute["show-hidden"]) &&
                                                preg_match("/^\s*(?:on|yes|true)\s*$/i", $attribute["show-hidden"]);
                            $exclude          = $attribute["exclude"] ?? 0;

                            ob_start();
                            new menu(
                                defined("CMS_CONTENT_DIRECTORY_INDEX") ? CMS_CONTENT_DIRECTORY_INDEX : 0,
                                $base,
                                $level,
                                $depth,
                                $filter,
                                $show_icons,
                                $show_images,
                                $show_description,
                                $show_hidden,
                                $exclude);
                            $buffer = ob_get_clean();

                            break;

                        case CMS_TEMPLATE_TYPE_BACKLINK:
//..................................................................................................

                            /*
                                <CMS:backlink/>
                            */

                            if ($suppressed)     break; //conditionally suppressed
                            if ($template_depth) break; //not the base template

                            //replace with backlink code
                            $buffer .= "<!-- backlink -->" .
                                       "<a href=\"" . x(CMS_HOMEPAGE) . "\">" .
                                       x(CMS_SOFTWARE) .
                                       "</a>";

                            break;

                        case CMS_TEMPLATE_TYPE_STYLESHEET:
//..................................................................................................

                            /*
                                <CMS:stylesheet default="{identifier}"/>
                            */

                            if ($suppressed) break; //conditionally suppressed

                            //get identifier
                            if (blank($attribute["default"])) break; //not set
                            $cvalue = $attribute["default"];

                            //stylesheet already employed
                            if (isset($template_cache[$cvalue][4])) break; //skip

                            //link stylesheet of employed template
                            $file = FALSE;

                            if (
                                 ($_name = l($this->data->get($cvalue, "name")))
                                 &&
                                 (($file = $this->get_stylesheet($cvalue, NULL, TRUE)) !== FALSE)
                               )
                                $buffer .= stylesheet(CMS_DATA_URL . $file, FALSE);

                            //add to list of employed stylesheets
                            $template_cache[$cvalue][4] = $file;

                            break;

                        case CMS_TEMPLATE_TYPE_JAVASCRIPT:
//..................................................................................................

                            /*
                                <CMS:javascript default="{identifier}"/>
                            */

                            if ($suppressed) break; //conditionally suppressed

                            //get identifier
                            if (blank($attribute["default"])) break; //not set
                            $cvalue = $attribute["default"];

                            //javascript already employed
                            if (isset($template_cache[$cvalue][5])) break; //skip

                            //link javascript of employed template
                            $file = FALSE;

                            if (
                                 ($_name = l($this->data->get($cvalue, "name")))
                                 &&
                                 (($file = $this->get_javascript($cvalue, NULL, TRUE)) !== FALSE)
                               )
                                $buffer .= javascript(CMS_DATA_URL . $file, FALSE);

                            //add to list of employed javascripts
                            $template_cache[$cvalue][5] = $file;

                            break;

                        case CMS_TEMPLATE_TYPE_CBLOCK:
//..................................................................................................

                            /*
                                <CMS:cblock [idref="+{number}|-{number}|{alphanumeric_.}"]
                                            [regex="{pcre}"]>

                                ...

                                </CMS:cblock>
                            */

                            //do not apply if conditional blocks are flagged to be displayed
                            //debug mode overrides in order to retrieve idref
                            $flag = CMS_TEMPLATE_OPTION & CMS_TEMPLATE_OPTION_CBLOCK;
                            if ($flag && (! $debug)) break;

                            //idref
                            $this->parse_id(
                                $attribute["idref"] ?? NULL,
                                $base_id,
                                $_base_id,
                                $auto_id,
                                $idref); //data index

                            if ($calt_limit)   break; //suppressed by alternative block
                            if ($edit_limit)   break; //suppressed by non-editable block
                            if ($noedit_limit) break; //suppressed by non-editable block

                            //do not apply if conditional blocks are flagged to be displayed
                            if ($flag) break;

                            //increase block nesting count
                            $cblock_depth++;

                            //not suppressed already
                            while (! $cblock_limit)
                            {
                                $regex = $attribute["regex"] ?? NULL;
                                $value = l($document->get($idref));

                                //referenced element value matches regex
                                if (nstre($regex))
                                {
                                    if (preg_match($regex, $value)) break; //no suppression
                                }

                                //referenced element has value
                                elseif (nstre($value))
                                {
                                    break; //no suppression
                                };

                                //set suppression
                                $cblock_limit = $cblock_depth;
                                break;
                            };

                            break;

                        case CMS_TEMPLATE_TYPE_CALT:
//..................................................................................................

                            /*
                                <CMS:calt [idref="+{number}|-{number}|{alphanumeric_.}"]>

                                ...

                                </CMS:calt>
                            */

                            //do not apply if alternative blocks are flagged to be displayed
                            //debug mode overrides in order to retrieve idref
                            $flag = CMS_TEMPLATE_OPTION & CMS_TEMPLATE_OPTION_CALT;
                            if ($flag && (! $debug)) break;

                            //idref
                            $this->parse_id(
                                $attribute["idref"] ?? NULL,
                                $base_id,
                                $_base_id,
                                $auto_id,
                                $idref); //data index

                            if ($flag)         break; //flagged to be displayed
                            if ($cblock_limit) break; //suppressed by conditional block
                            if ($edit_limit)   break; //suppressed by non-editable block
                            if ($noedit_limit) break; //suppressed by non-editable block

                            //increase block nesting count
                            $calt_depth++;

                            if ($calt_limit) break; //already suppressed

                            if (

                            //conditional blocks are flagged to be displayed
                                 (CMS_TEMPLATE_OPTION & CMS_TEMPLATE_OPTION_CBLOCK)
                                 ||

                            //or referenced element has value
                                 nstre(l($document->get($idref)))
                               )

                                //set suppression
                                $calt_limit = $calt_depth;

                            break;


                        case CMS_TEMPLATE_TYPE_GROUP:
//..................................................................................................

                            /*
                                <CMS:group [id="+{number}|-{number}|{alphanumeric_.}"]
                                           [edit="on|yes|true|off|no|false"]
                                           [title="{text}"]>

                                ...

                                </CMS:group>
                            */

                            if ($suppressed) break; //conditionally suppressed

                            //put data on group stack
                            array_push($group_stack, $_base_id); //backup base index

                            $_base_id = "$_id."; //set base index

                            //open group span when editing
                            if ($edit_display)
                                $buffer .= "<div id=\"tp-grp-$index\" " .
                                                "class=\"tp-grp\">";

                            //set name to display
                            if (isset($attribute["title"])) $name = $attribute["title"];

                            break;

                        case CMS_TEMPLATE_TYPE_BASE:
//..................................................................................................

                            /*
                                <CMS:base [id="+{number}|-{number}|{alphanumeric_.}"]
                                          [scope="local|global"]>

                                ...

                                </CMS:base>
                            */

                            //advance automatic index if no explicit id is given
                            if ($no_id) $auto_id++;

                            //put data on base stack
                            array_push(
                                $base_stack,
                                ["auto_id"  => $auto_id,    //backup automatic index
                                 "base_id"  => $base_id,    //backup base index
                                 "_base_id" => $_base_id]); //backup base index

                            $auto_id  = 0;       //set automatic index
                            $base_id  = "$_id."; //set base index
                            $_base_id = "$_id."; //set base index

                            //put data on temporary random index stack
                            $_temp = preg_match("/^\s*global\s*$/i", $attribute["scope"] ?? NULL) ? $temp :
                                     crc32_base62((defined("CMS_CONTENT_INDEX") ? CMS_CONTENT_INDEX : 0) . ":$base_id");
                            array_push($temp_stack, $_temp);

                            break;

                        case CMS_TEMPLATE_TYPE_NAMESPACE:
//..................................................................................................

                            /*
                                <CMS:namespace [id="+{number}|-{number}|{alphanumeric_.}"]
                                               [auto-id="contain"]>

                                ...

                                </CMS:namespace>
                            */

                            //contain automatic index
                            $_auto_id = NULL;
                            if (
                                 isset($attribute["auto-id"])
                                 &&
                                 preg_match("/^\s*contain\s*$/i", $attribute["auto-id"])
                               )
                            {
                                $_auto_id = $auto_id;
                                $auto_id  = 0;
                            };

                            //put data on namespace stack
                            array_push(
                                $namespace_stack,
                                ["auto_id"  => $_auto_id,   //backup automatic index
                                 "_base_id" => $_base_id]); //backup base index

                            $_base_id = "$_id."; //set base index

                            break;

                        case CMS_TEMPLATE_TYPE_REPEAT:
//..................................................................................................

                            /*
                                <CMS:repeat [id="+{number}|-{number}|{alphanumeric_.}"]
                                            [default="{number}"]
                                            [edit="on|yes|true|off|no|false"]
                                            [title="{text}"]>

                                ...

                                </CMS:repeat>
                            */

                            //determine repeat count
                            $cvalue = max((int)$cvalue, 1); //at least one repetition

                            //put data on repeat stack
                            array_push(
                                $repeat_stack,
                                ["count"   => $cvalue,     //repeat count
                                 "offset"  => $_offset,    //repeat offset
                                 "base_id" => $_base_id]); //backup base index

                            $_base_id = "$_id."; //set base index

                            //display value when editing
                            if ($edit_display)
                                $buffer .= "<div class=\"tp-value\">" .
                                           "<span>" .
                                           x(strabridge($_title, 15, TRUE)) .
                                           "</span>" .
                                           (stre($cvalue) ? CMS_L_TEMPLATE_050 : x(strabridge($cvalue, 25))) .
                                           "</div>";

                            break;

                        case CMS_TEMPLATE_TYPE_SHIFT:
//..................................................................................................

                            /*
                                <CMS:shift [id="+{number}|-{number}|{alphanumeric_.}"]
                                           [default="{number}"]
                                           [edit="on|yes|true|off|no|false"]
                                           [title="{text}"]>

                                ...

                                </CMS:shift>
                            */

                            if ($suppressed) break; //conditionally suppressed

                            //determine shift count
                            $cvalue = max((int)$cvalue, 1); //fallback value is one

                            //put data on shift stack
                            array_push($shift_stack, $_base_id); //backup base index

                            $_base_id = "$_id."; //set base index

                            break;

                        case CMS_TEMPLATE_TYPE_TEMPLATE:
//..................................................................................................

                            /*
                                <CMS:template [id="+{number}|-{number}|{alphanumeric_.}"]
                                              [default="{identifier}"]
                                              [edit="on|yes|true|off|no|false"]
                                              [title="{text}"]/>
                            */

                            if (streq("$id.", $base_id)) break; //prevent infinite loop
                            if ($template_depth >= 50)   break; //prevent circular recursion
                            if ($suppressed)             break; //conditionally suppressed

                            //no value
                            if (stre($cvalue))
                            {
                                //set name to display
                                if (isset($attribute["title"])) $name = $attribute["title"];
                                break; //skip
                            };

                            //template already employed
                            if (isset($template_cache[$cvalue][3]))
                            {
                                //set name to display
                                $name = $template_cache[$cvalue][3];
                            }

                            //template not yet employed
                            else
                            {
                                $name  = l($this->data->get($cvalue, "name"));
                                $file  = $template_cache[$cvalue][4] ?? NULL;
                                $_file = $template_cache[$cvalue][5] ?? NULL;

                                if (nstre($name)) //template exists
                                {
                                    //link stylesheet of employed template
                                    if (
                                         ($file === NULL)
                                         &&
                                         (($file = $this->get_stylesheet($cvalue, NULL, TRUE)) !== FALSE)
                                       )
                                        $buffer .= stylesheet(CMS_DATA_URL . $file, FALSE);

                                    //link javascript of employed template
                                    if (
                                         ($_file === NULL)
                                         &&
                                         (($_file = $this->get_javascript($cvalue, NULL, TRUE)) !== FALSE)
                                       )
                                        $buffer .= javascript(CMS_DATA_URL . $_file, FALSE);
                                };

                                //add to list of employed assets
                                $template_cache[$cvalue][3] = $name;
                                $template_cache[$cvalue][4] = $file  ?? FALSE;
                                $template_cache[$cvalue][5] = $_file ?? FALSE;
                            };

                            //template not found
                            if (stre($name))
                            {
                                $name = CMS_L_MISSING;
                                break; //skip
                            };

                            //open group span when editing
                            if ($edit_display)
                                $buffer .= "<div id=\"tp-grp-$index\" " .
                                                "class=\"tp-grp\">";

                            //recursively call parser function to process subtemplate
                            $buffer .= $this->_parse(
                                $document,                                   //document
                                isset($template_cache[$cvalue][0]) ?         //template
                                NULL : $this->get_code($cvalue),
                                $_id,                                        //base id
                                ($cache === TRUE) && ($nocache_depth === 0), //cache
                                $cvalue);                                    //template index

                            //close group span when editing
                            if ($edit_display)
                                $buffer .= "</div>";

                            break;

                        case CMS_TEMPLATE_TYPE_TEXT:
//..................................................................................................

                            /*
                                <CMS:text [id="+{number}|-{number}|{alphanumeric_.}"]
                                          [default="{text}"]
                                          [edit="on|yes|true|off|no|false"]
                                          [title="{text}"]/>
                            */

                            if ($suppressed) break; //conditionally suppressed

                            //parse text if given
                            if (nstre($cvalue))
                                $buffer .= parse_text($cvalue, TRUE);

                            //or output element title when editing
                            elseif ($edit_display)
                                $buffer .= "<span class=\"tp-placeholder\">" .
                                           x($_title) .
                                           "</span>";

                            break;

                        case CMS_TEMPLATE_TYPE_VALUE:
//..................................................................................................

                            /*
                                <CMS:value [id="+{number}|-{number}|{alphanumeric_.}"]
                                           [default="{text}"]
                                           [regex="{text}"]
                                           [edit="on|yes|true|off|no|false"]
                                           [title="{text}"]/>
                            */

                            if ($suppressed) break; //conditionally suppressed
                            if (! $edit)     break; //edit option disabled

                            //display value when editing
                            if ($edit_display)
                                $buffer .= "<div class=\"tp-value\">" .
                                           "<span>" .
                                           x(strabridge($_title, 15, TRUE)) .
                                           "</span>" .
                                           (stre($cvalue) ? CMS_L_TEMPLATE_050 : x(strabridge($cvalue, 25))) .
                                           "</div>";

                            break;

                        case CMS_TEMPLATE_TYPE_SWITCH:
//..................................................................................................

                            /*
                                <CMS:switch [id="+{number}|-{number}|{alphanumeric_.}"]
                                            [default="{text}"]
                                            [edit="on|yes|true|off|no|false"]
                                            [title="{text}"]/>
                            */

                            if ($suppressed) break; //conditionally suppressed
                            if (! $edit)     break; //edit option disabled

                            //display value when editing
                            if ($edit_display)
                            {
                                $flag    = stre($cvalue);
                                $buffer .= "<div class=\"tp-switch-" . ($flag ? "off" : "on") . "\">" .
                                           "<span>" .
                                           x(strabridge($_title, 15, TRUE)) .
                                           "</span>" .
                                           ($flag ? CMS_L_TEMPLATE_053 : CMS_L_TEMPLATE_054) .
                                           "</div>";
                            };

                            break;

                        case CMS_TEMPLATE_TYPE_HREF:
//..................................................................................................

                            /*
                                <CMS:href [id="+{number}|-{number}|{alphanumeric_.}"]
                                          [_id="{text}"]
                                          [default="{url}"]
                                          [target="{identifier}"]
                                          [rel="{text}"]
                                          [edit="on|yes|true|off|no|false"]
                                          [title="{text}"]
                                          [class="{text}"]
                                          [style="{text}"]>

                                ...

                                </CMS:href>
                            */

                            if ($suppressed) break; //conditionally suppressed
                            if ($preview)    break; //suppressed by preview

                            //parse document value
                            if (nstre($cvalue) && (($array = template_parse_reference($cvalue)) !== FALSE))
                            {
                                $attribute["_title"] ??= $array["description"];

                                //html link
                                $buffer .= $edit_display ?
                                           "<a href=\"" . x($array["url"]) . "\" " .
                                              "title=\"" . CMS_L_TEMPLATE_019 . "\">" :
                                           "<a" . $this->attribute($attribute, ["href" => $array["url"]]) . ">";
                            }

                            //no or invalid value
                            elseif (! $edit_display)
                            {
                                //close element span
                                array_pop($span_stack[$ttype]);
                            };

                            break;

                        case CMS_TEMPLATE_TYPE_IMAGE:
//..................................................................................................

                            /*
                                <CMS:image [id="+{number}|-{number}|{alphanumeric_.}"]
                                           [_id="{text}"]
                                           [default="{url}"]
                                           [width="{width}[%]"]
                                           [height="{height}[%]"]
                                           [alt="{text}"]
                                           [edit="on|yes|true|off|no|false"]
                                           [title="{text}"]
                                           [class="{text}"]
                                           [style="{text}"]/>
                            */

                            if ($suppressed) break; //conditionally suppressed

                            //set name to display
                            if (isset($attribute["title"])) $name = $attribute["title"];

                            if ($cvalue)
                            {
                                //process image address
                                $location               = translate_url($cvalue);

                                //prepare image (scaling, caching, ...)
                                $width                  = $attribute["width"]  ?? NULL;
                                $height                 = $attribute["height"] ?? NULL;
                                $attribute["src"]       = $_location = image_process($location, $width, $height);
                                list($_width, $_height) = getimagesize($_location); //processed size

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

                                if (
                                     (($__location = analyze_url($location)) !== FALSE)
                                     &&
                                     preg_match("/(?:jpe?g|png|webp)$/i", $__location["extension"])
                                   )
                                {
                                    //generate alternative resolution sources
                                    $resolution    = [160, 320, 480, 640, 960, 1280, 1600, 1920, 3840, 7680]; //resolution list
                                    list($__width) = getimagesize($location);                                 //original size
                                    $srcset        = $_location . " " . $_width . "w,";
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
                                        $__location  = image_process($location, $value);
                                        $srcset     .= x($__location . " " . $value . "w,");
                                    };

                                    //set srcset attribute
                                    $attribute["srcset"] = substr($srcset, 0, -1);
                                };

                                //alt text
                                $attribute["alt"] ??=
                                    stre($temp = l($this->image->data->get($cvalue, "name"))) ?
                                    pathinfo($cvalue, PATHINFO_FILENAME) : $temp;

                                //html image reference fallback
                                $array   = ["width"  => $_width,
                                            "height" => $_height,
                                            "sizes"  => isset($attribute["srcset"]) ? "100vw" : NULL];
                                $buffer .= "<noscript>" .
                                           "<img" . $this->attribute($attribute, $array, "_id") . ">" .
                                           "</noscript>";

                                //html image reference lazy load
                                $attribute["data-defer-src"] = $attribute["src"];
                                $attribute["src"]            = "data:image/svg+xml," .
                                                               r("<svg xmlns=\"http://www.w3.org/2000/svg\" " .
                                                                      "width=\"" . x($_width) . "\" " .
                                                                      "height=\"" . x($_height) . "\">" .
                                                                 "</svg>");
                                if (isset($attribute["srcset"]))
                                {
                                    $attribute["data-defer-srcset"] = $attribute["srcset"];
                                    unset($attribute["srcset"]);
                                };
                                if (isset($attribute["sizes"]))
                                {
                                    $attribute["data-defer-sizes"]  = $attribute["sizes"];
                                    unset($attribute["sizes"]);
                                };

                                $buffer .= "<img" . $this->attribute($attribute, ["width" => $_width, "height" => $_height]) . ">";
                            }

                            //no value
                            else
                            {
                                if (! CMS_TEMPLATE_OPTION) break; //output mode, no display

                                //prepare dummy display
                                $attribute["width"]  ??= round(($attribute["height"] ?? 100) / 100 * 150);
                                $attribute["height"] ??= round($attribute["width"] / 150 * 100);
                                $attribute["src"]      = "data:image/svg+xml," .
                                                         r("<svg xmlns=\"http://www.w3.org/2000/svg\" " .
                                                                "width=\"" . x($attribute["width"]) . "\" " .
                                                                "height=\"" . x($attribute["height"]) . "\">" .
                                                           "</svg>");
                                $attribute["alt"]      = "";
                                $attribute["style"]    = "BACKGROUND:url('" . qx(CMS_IMAGES_URL) . "no_image.svg') center/cover";

                                //html image reference
                                $buffer .= "<img" . $this->attribute($attribute) . ">";
                            };

                            break;

                        case CMS_TEMPLATE_TYPE_THUMBNAIL:
//..................................................................................................

                            /*
                                <CMS:thumbnail [id="+{number}|-{number}|{alphanumeric_.}"]
                                               [_id="{text}"]
                                               [default="{url}"]
                                               [twidth="{width}[%]"]
                                               [theight="{height}[%]"]
                                               [width="{width}[%]"]
                                               [height="{height}[%]"]
                                               [alt="{text}"]
                                               [edit="on|yes|true|off|no|false"]
                                               [title="{text}"]
                                               [style="{text}"]/>
                            */

                            if ($suppressed) break; //conditionally suppressed

                            //set name to display
                            if (isset($attribute["title"])) $name = $attribute["title"];

                            if ($cvalue)
                            {
                                //process image address
                                $location               = translate_url($cvalue);

                                //prepare thumbnail image
                                $_attribute             = ["width"  => $twidth    = $attribute["twidth"]  ?? NULL,
                                                           "height" => $theight   = $attribute["theight"] ?? NULL,
                                                           "src"    => $tlocation = image_process($location, $twidth, $theight)];
                                list($_width, $_height) = getimagesize($tlocation);        //processed size
                                list($__width)          = $size = getimagesize($location); //original size
                                $flag                   = ! isset($attribute["data-size"]);

                                //adapt aspect ratio if processing failed
                                if (($twidth > 0) && ($twidth < $_width))
                                {
                                    $_height = (int)(($twidth * $_height + $_width - 1) / $_width);
                                    $_width  = $twidth;
                                };

                                if (($theight > 0) && ($theight < $_height))
                                {
                                    $_width  = (int)(($theight * $_width + $_height - 1) / $_height);
                                    $_height = $theight;
                                };

                                if (
                                     (($_location = analyze_url($location)) !== FALSE)
                                     &&
                                     preg_match("/(?:jpe?g|png|webp)$/i", $_location["extension"])
                                   )
                                {
                                    //generate alternative resolution sources
                                    $resolution   = [160, 320, 480, 640, 960, 1280, 1600, 1920, 3840, 7680]; //resolution list
                                    $srcset       = $tlocation . " " . $_width . "w,";
                                    $width_retina = $_width * 2;

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
                                        $_location  = image_process($location, $value);
                                        $srcset    .= x($_location . " " . $value . "w,");
                                    };

                                    //set srcset attribute
                                    $_attribute["srcset"] = substr($srcset, 0, -1);
                                };

                                //prepare zoom image
                                $width  = $attribute["width"]  ?? NULL;
                                $height = $attribute["height"] ?? NULL;

                                if (

                                //width, height or both are given
                                     (((! $width) && $height) || preg_match("/^\d+$/", $width))
                                     &&
                                     (((! $height) && $width) || preg_match("/^\d+$/", $height))
                                   )
                                {
                                    //prepare zoom image (scaling, caching, ...)
                                    $location = image_process($location, $width, $height);
                                    if ($flag) $size = getimagesize($location);
                                };

                                //image size
                                if ($flag && ($size !== FALSE)) $attribute["data-size"] = implode("x", array_slice($size, 0, 2));

                                //alt text
                                $_attribute["alt"] ??=
                                    stre($temp = l($this->image->data->get($cvalue, "name"))) ?
                                    pathinfo($cvalue, PATHINFO_FILENAME) : $temp;

                                //html link to zoom image
                                $buffer .= "<a" . $this->attribute($attribute,
                                                                   ["href" => $location, "_title" => $_attribute["alt"]],
                                                                   ["twidth", "theight", "width", "height", "alt"]) . ">";

                                //html thumbnail image reference fallback
                                $array   = ["width"  => $_width,
                                            "height" => $_height,
                                            "sizes"  => isset($_attribute["srcset"]) ? "100vw" : NULL];
                                $buffer .= "<noscript>" .
                                           "<img" . $this->attribute($_attribute, $array, "_id") . ">" .
                                           "</noscript>";

                                //html thumbnail image reference lazy load
                                $_attribute["data-defer-src"] = $_attribute["src"];
                                $_attribute["src"]            = "data:image/svg+xml," .
                                                                r("<svg xmlns=\"http://www.w3.org/2000/svg\" " .
                                                                       "width=\"" . x($_width) . "\" " .
                                                                       "height=\"" . x($_height) . "\">" .
                                                                  "</svg>");
                                if (isset($_attribute["srcset"]))
                                {
                                    $_attribute["data-defer-srcset"] = $_attribute["srcset"];
                                    unset($_attribute["srcset"]);
                                };
                                if (isset($_attribute["sizes"]))
                                {
                                    $_attribute["data-defer-sizes"]  = $_attribute["sizes"];
                                    unset($_attribute["sizes"]);
                                };

                                $buffer .= "<img" . $this->attribute($_attribute, ["width" => $_width, "height" => $_height]) . ">" .
                                           "</a>";
                            }

                            //no value
                            else
                            {
                                if (! CMS_TEMPLATE_OPTION) break; //output mode, no display

                                //prepare dummy display
                                $twidth     = $attribute["twidth"]  ?? round(($attribute["theight"] ?? 100) / 100 * 150);
                                $theight    = $attribute["theight"] ?? round($twidth / 150 * 100);
                                $location   = NULL;
                                $tlocation  = "data:image/svg+xml," .
                                              r("<svg xmlns=\"http://www.w3.org/2000/svg\" " .
                                                     "width=\"" . x($twidth) . "\" " .
                                                     "height=\"" . x($theight) . "\">" .
                                                "</svg>");
                                $alt        = "";
                                $style      = "BACKGROUND:url('" . qx(CMS_IMAGES_URL) . "no_image.svg') center/cover";

                                $_attribute = ["src"    => $tlocation,
                                               "width"  => $twidth,
                                               "height" => $theight,
                                               "alt"    => $alt,
                                               "style"  => $style];

                                //html link to zoom image
                                $buffer .= "<a" . $this->attribute($attribute,
                                                                   ["href" => $location],
                                                                   ["twidth", "theight", "width", "height", "alt"]) . ">" .

                                //html thumbnail image reference
                                           "<img" . $this->attribute($_attribute) . ">" .
                                           "</a>";
                            };

                            break;

                        case CMS_TEMPLATE_TYPE_MEDIA:
//..................................................................................................

                            /*
                                <CMS:media [id="+{number}|-{number}|{alphanumeric_.}"]
                                           [_id="{text}"]
                                           [default="{url}"]
                                           [width="{width}[%]"]
                                           [height="{height}[%]"]
                                           [alt="{text}"]
                                           [edit="on|yes|true|off|no|false"]
                                           [title="{text}"]
                                           [class="{text}"]
                                           [style="{text}"]/>
                            */

                            if ($suppressed) break; //conditionally suppressed

                            //set name to display
                            if (isset($attribute["title"])) $name = $attribute["title"];

                            //attributes
                            $__id   = isset($attribute["_id"])    ? x($attribute["_id"])    : NULL;
                            $width  = isset($attribute["width"])  ? x($attribute["width"])  : NULL;
                            $height = isset($attribute["height"]) ? x($attribute["height"]) : NULL;
                            $alt    = isset($attribute["alt"])    ? x($attribute["alt"])    : NULL;
                            $class  = isset($attribute["class"])  ? x($attribute["class"])  : NULL;
                            $style  = isset($attribute["style"])  ? x($attribute["style"])  : NULL;

                            if ($cvalue)
                            {
                                $location = $cvalue;

                                //alt text
                                if (! $alt) $alt = x(l($this->media->data->get($cvalue, "name")));
                            }

                            //no value
                            else
                            {
                                if (! CMS_TEMPLATE_OPTION) break; //output mode, no display

                                //prepare dummy display
                                $width    ??= ($height ?? 100) / 100 * 150;
                                $height   ??= $width / 150 * 100;
                                $location   = "data:image/svg+xml," .
                                              r("<svg xmlns=\"http://www.w3.org/2000/svg\" " .
                                                     "width=\"" . x($width) . "\" " .
                                                     "height=\"" . x($height) . "\">" .
                                                "</svg>");
                                $alt        = $_title;
                                $style      = "BACKGROUND:url('" . qx(CMS_IMAGES_URL) . "no_image.svg') center/cover";
                            };

                            //generate media display code
                            $buffer .= $this->media->parse(
                                $location, //address
                                $__id,     //id
                                $width,    //width
                                $height,   //height
                                $alt,      //alt
                                $alt,      //title
                                $class,    //class
                                $style);   //style

                            break;

                        case CMS_TEMPLATE_TYPE_DOWNLOAD:
//..................................................................................................

                            /*
                                <CMS:download [id="+{number}|-{number}|{alphanumeric_.}"]
                                              [_id="{text}"]
                                              [default="{identifier}"]
                                              [edit="on|yes|true|off|no|false"]
                                              [title="{text}"]
                                              [class="{text}"]
                                              [style="{text}"]>

                                ...

                                </CMS:download>
                            */

                            if ($suppressed) break; //conditionally suppressed
                            if ($preview)    break; //suppressed by preview

                            if ($cvalue)
                            {
                                //compatibility fallback
                                $location = (strncmp($cvalue, "download://", 11) === 0) ?
                                            $cvalue : "download://$cvalue";

                                //address of download
                                $location = translate_url($location);

                                //title text
                                $attribute["_title"] ??=
                                    sprintf(CMS_L_TEMPLATE_056,
                                            l($this->download->data->get($cvalue, "name")));

                                //html link to download
                                $buffer .= "<a" . $this->attribute($attribute, ["href" => $location]) . ">";
                            }

                            //no value
                            elseif (! $edit_display)
                            {
                                //close element span
                                array_pop($span_stack[$ttype]);
                            };

                            break;

                        case CMS_TEMPLATE_TYPE_PLUGIN:
//..................................................................................................

                            /*
                                <CMS:plugin [id="+{number}|-{number}|{alphanumeric_.}"]
                                            [default="{url}"]
                                            [edit="on|yes|true|off|no|false"]
                                            [title="{text}"]/>
                            */

                            if ($suppressed) break; //conditionally suppressed

                            //retrieve data from given address
                            if (nstre($cvalue))
                                $buffer .= template_read_plugin(translate_url($cvalue));

                            break;

                        case CMS_TEMPLATE_TYPE_NOCACHE:
//..................................................................................................

                            /*
                                <CMS:nocache>

                                ...

                                </CMS:nocache>
                            */

                            if ($suppressed) break; //conditionally suppressed
                            if (! $cache)    break; //do not generate cache version

                            //if not within uncached span
                            if (! $nocache_depth++)
                            {
                                //initialize nocache buffer
                                $nocache_buffer = substr($base_id, 0, -1) .
                                                  CMS_TEMPLATE_CACHE_SEPARATOR;

                                //store cache insert position
                                $nocache_insert = strlen($output);
                            };

                            break;

                        case CMS_TEMPLATE_TYPE_CEDIT:
//..................................................................................................

                            /*
                                <CMS:edit [option="href plugin text value
                                                   download image thumbnail
                                                   media template group
                                                   repeat shift switch"]>

                                ...

                                </CMS:edit>
                            */

                            if ($cblock_limit) break; //suppressed by conditional block
                            if ($calt_limit)   break; //suppressed by alternative block
                            if ($noedit_limit) break; //suppressed by non-editable block

                            //increase block nesting count
                            $edit_depth++;

                            if ($edit_limit) break; //already suppressed

                            //specific
                            if (isset($attribute["option"]))
                            {
                                $option = preg_split("/\s+/", $attribute["option"]);

                                foreach ($option AS $value)
                                {
                                    //check if type exists
                                    if (! isset($this->tlist[$value]))   continue;
                                    $value = $this->tlist[$value];

                                    //check if option exists
                                    if (! isset($this->toption[$value])) continue;
                                    $value = $this->toption[$value];

                                    //check for option not matching
                                    if (CMS_TEMPLATE_OPTION & $value)    break 2;
                                };
                            }

                            //general
                            elseif (CMS_TEMPLATE_OPTION & ~CMS_TEMPLATE_OPTION_DEBUG) break; //editing enabled

                            //set suppression
                            $edit_limit = $edit_depth;

                            break;

                        case CMS_TEMPLATE_TYPE_CNOEDIT:
//..................................................................................................

                            /*
                                <CMS:noedit>

                                ...

                                </CMS:noedit>
                            */

                            if ($cblock_limit) break; //suppressed by conditional block
                            if ($calt_limit)   break; //suppressed by alternative block
                            if ($edit_limit)   break; //suppressed by editable block

                            //increase block nesting count
                            $noedit_depth++;

                            if ($noedit_limit) break; //already suppressed

                            //editing enabled
                            if (CMS_TEMPLATE_OPTION & ~CMS_TEMPLATE_OPTION_DEBUG)

                                //set suppression
                                $noedit_limit = $noedit_depth;

                            break;
                        };

                        //display editing controls
                        if ($ttype & CMS_TEMPLATE_TYPE_EDIT) //element is editable
                        {
                            if ($edit_display)
                            {
                                //set edit options
                                $edit_flag = CMS_TEMPLATE_COMMAND_BUFFER  |
                                             CMS_TEMPLATE_COMMAND_PASTE   |
                                             CMS_TEMPLATE_COMMAND_SWAP    |
                                             CMS_TEMPLATE_COMMAND_KICK1   |
                                             CMS_TEMPLATE_COMMAND_KICK2   |
                                             CMS_TEMPLATE_COMMAND_DROP1   |
                                             CMS_TEMPLATE_COMMAND_DROP2   |
                                             CMS_TEMPLATE_COMMAND_RELEASE |
                                             CMS_TEMPLATE_COMMAND_REFERENCE;

                                //value is set
                                if (nstre($_cvalue))
                                {
                                    //export
                                    if (

                                    //element is template
                                         ($ttype === CMS_TEMPLATE_TYPE_TEMPLATE)
                                         &&

                                    //value is set for current language
                                         nstre($cvalue)
                                       )
                                        $edit_flag |= CMS_TEMPLATE_COMMAND_EXPORT;

                                    //clear
                                    $edit_flag |= CMS_TEMPLATE_COMMAND_CLEAR;
                                };

                                //collect url parameters
                                $_action   = $this->action[CMS_TEMPLATE_ACTION][$ttype][CMS_TEMPLATE_CODE];
                                $replace   = ["%index%"     => $id,
                                              "%path%"      => $_id,
                                              "%value%"     => (strpos($_action, "%value%") !== FALSE) ? $_cvalue : NULL,
                                              "%type%"      => $_ttype,
                                              "%reference%" => $document->get_reference($_id, $_ttype),
                                              "%id%"        => $index];
                                $__ttype   = ($ttype & (CMS_TEMPLATE_TYPE_VALUE | CMS_TEMPLATE_TYPE_SWITCH))     ?
                                             CMS_TEMPLATE_TYPE_VALUE | CMS_TEMPLATE_TYPE_SWITCH                  :
                                             (($ttype & (CMS_TEMPLATE_TYPE_IMAGE | CMS_TEMPLATE_TYPE_THUMBNAIL)) ?
                                             CMS_TEMPLATE_TYPE_IMAGE | CMS_TEMPLATE_TYPE_THUMBNAIL               :
                                             $ttype);
                                $empty     = stre($buffer);
                                $image     = $this->action[CMS_TEMPLATE_ACTION][$ttype][CMS_TEMPLATE_IMAGE];
                                $reference = nstre($replace["%reference%"]);
                                if ($reference && ($replace["%reference%"] === TRUE)) $replace["%reference%"] = "";

                                //create editing container and functions
                                $_buffer = "<div class=\"tp-dd" . (($ttype & (CMS_TEMPLATE_TYPE_TEMPLATE | CMS_TEMPLATE_TYPE_GROUP)) ? "100" : "") . ($reference ? " tp-ref" : "") . "\" " .
                                           ($preview ?

                                //open dummy container
                                                "data-tp-type=\"" . x($replace["%type%"]) . "\">" :

                                //open dragdrop container
                                                "id=\"tp-dd-" . x($index) . "\" " .
                                                "title=\"" . CMS_L_TEMPLATE_045 . "\" " .
                                                "data-tp-index=\"" . x($replace["%index%"]) . "\" " .
                                                "data-tp-path=\"" . x($replace["%path%"]) . "\" " .
                                                "data-tp-value=\"" . x($replace["%value%"]) . "\" " .
                                                "data-tp-type=\"" . x($replace["%type%"]) . "\" " .
                                                "data-tp-reference=\"" . x($replace["%reference%"]) . "\" " .
                                                "data-tp-id=\"" . x($replace["%id%"]) . "\" " .
                                                "data-tp-empty=\"" . ($empty ? "true" : "false") . "\" " .
                                                "data-tp-dd-type=\"" . x($ttype) . "\" " .
                                                "data-tp-dd-accept=\"" . x($__ttype) . "\">") .

                                //open extended editing functions container
                                           "<div class=\"tp-edt\">";

                                //flip switch
                                if ((! $preview) && ($ttype & (CMS_TEMPLATE_TYPE_TEMPLATE | CMS_TEMPLATE_TYPE_GROUP)))
                                    $_buffer .= "<button type=\"button\" " .
                                                        "onclick=\"return tp_flp('" . qx($index) . "');\" " .
                                                        "title=\"" . CMS_L_TEMPLATE_046 . "\" " .
                                                        "class=\"tp-flp\">" .
                                                "</button>";

                                //code for primary button that invokes element action
                                $replace  = array_map(__NAMESPACE__ . "\\r", $replace);
                                $search   = array_keys($replace);
                                $_buffer .= "<button type=\"button\" " .
                                                    "id=\"tp-edt-a-" . x($index) . "\" " .
                                                    "onclick=\"" . x(str_replace($search, $replace, $_action)) . "\" " .
                                                    "title=\"" . x($_id) . "\" " .
                                                    "data-title=\"" . x($_title) . ($reference ? " (" . CMS_L_TEMPLATE_042 . ")" : "") . "\">" .
                                             image($image) .
                                             "</button>";

                                if (! $preview)
                                {
                                    //extended editing functions that invoke additional actions
                                    //displayed as a panel that expands when touching primary button
                                    $_buffer .= "<div id=\"tp-edt-" . x($index) . "\" " .
                                                     "title=\"\" " .
                                                     "class=\"tp-edt-dsp\">";

                                    //set up activation button
                                    if (

                                    //element is href or download
                                         ($ttype & (CMS_TEMPLATE_TYPE_HREF | CMS_TEMPLATE_TYPE_DOWNLOAD))
                                         &&

                                    //element display code has been generated
                                         (! $empty)
                                       )
                                    {
                                        $_buffer .= $buffer .
                                                    "<img src=\"" . x(CMS_IMAGES_URL) . "content/button_open.svg\" " .
                                                         "alt=\"\">" .
                                                    "</a>";
                                        $buffer   = "";
                                    };

                                    //structure actions
                                    $array = [[CMS_TEMPLATE_COMMAND_BUFFER,    CMS_TEMPLATE_COMMAND_BUFFER,    "button_buffer",    CMS_L_COMMAND_BUFFER],
                                              [CMS_TEMPLATE_COMMAND_PASTE,     CMS_TEMPLATE_COMMAND_PASTE,     "button_paste",     CMS_L_COMMAND_PASTE],
                                              [CMS_TEMPLATE_COMMAND_SWAP,      CMS_TEMPLATE_COMMAND_SWAP,      "button_swap",      CMS_L_TEMPLATE_018,   TRUE],
                                              [CMS_TEMPLATE_COMMAND_KICK1,     CMS_TEMPLATE_COMMAND_KICK1,     "button_kick1",     CMS_L_TEMPLATE_025],
                                              [CMS_TEMPLATE_COMMAND_KICK2,     CMS_TEMPLATE_COMMAND_KICK2,     "button_kick2",     CMS_L_TEMPLATE_024],
                                              [CMS_TEMPLATE_COMMAND_DROP1,     CMS_TEMPLATE_COMMAND_DROP1,     "button_drop1",     CMS_L_TEMPLATE_026],
                                              [CMS_TEMPLATE_COMMAND_DROP2,     CMS_TEMPLATE_COMMAND_DROP2,     "button_drop2",     CMS_L_TEMPLATE_027,   TRUE],
                                              [CMS_TEMPLATE_COMMAND_RELEASE,   CMS_TEMPLATE_COMMAND_RELEASE,   "button_release",   CMS_L_TEMPLATE_040],
                                              [CMS_TEMPLATE_COMMAND_REFERENCE, CMS_TEMPLATE_COMMAND_REFERENCE, "button_reference", CMS_L_TEMPLATE_041],
                                              [CMS_TEMPLATE_COMMAND_EXPORT,    CMS_TEMPLATE_COMMAND_EXPORT,    "button_export",    CMS_L_COMMAND_EXPORT, TRUE],
                                              [CMS_TEMPLATE_COMMAND_CLEAR,     CMS_TEMPLATE_COMMAND_CLEAR,     "button_clear",     CMS_L_COMMAND_DELETE]];

                                    foreach ($array as $value)
                                    {
                                        if (
                                             ($edit_flag & $value[0])
                                             &&
                                             isset($this->action[CMS_TEMPLATE_COMMAND][$value[1]])
                                           )
                                        {
                                            $command  = str_replace($search, $replace, $this->action[CMS_TEMPLATE_COMMAND][$value[1]]);
                                            $_buffer .= "<button type=\"button\" " .
                                                                "onclick=\"" . x($command) . "\" " .
                                                                "title=\"" . $value[3] . "\">" .
                                                         image("content/" . $value[2]) .
                                                         "</button>";
                                        };
                                        if (isset($value[4])) $_buffer .= "<br>";
                                    };

                                    $_buffer .= "</div>";
                                };

                                //display content name
                                if (nstre($name))
                                    $_buffer .= "<div class=\"tp-name\">" .
                                                x($name) .
                                                "</div>";

                                $_buffer .= "</div>";           //close extended editing functions container
                                $buffer   = $_buffer . $buffer; //insert before content

                                //close dragdrop container if element is not spannable
                                if ($ttype & ~CMS_TEMPLATE_TYPE_SPAN) $buffer .= "</div>";
                            };

                            $auto_id++; //advance automatic index
                        };

                        //debug information
                        if ($debug)
                        {
                            //element name
                            $info = $_ttype;

                            //add id
                            if ($ttype & CMS_TEMPLATE_TYPE_EDIT)
                                $info .= " id:$id";

                            //add idref
                            elseif (
                                     ($ttype & (CMS_TEMPLATE_TYPE_CBLOCK | CMS_TEMPLATE_TYPE_CALT))
                                     &&
                                     isset($idref)
                                   )
                                $info .= " idref:$idref";

                            //add element start marker
                            $buffer = "<!-- #debug# $info -->$buffer";

                            //closing tag expected later
                            if ($ttype & CMS_TEMPLATE_TYPE_SPAN)
                            {
                                //memorize spannable element as open
                                $debug_stack[$ttype] ??= [];
                                array_push($debug_stack[$ttype], $info); //add to debug stack
                            }

                            //implicit closing tag
                            else
                            {
                                //add element end marker
                                $buffer .= "<!-- #debug# /$info -->";
                            };
                        };

                        //append parsed element code to output
                        $output .= $buffer;
                    };

                    //continue after tag
                    $offset = $_offset;
                };
            }

//..................................................................................................

            //closing tag
            elseif (
                     (($offset + 6) < $length)        //considers upcoming strpos
                     &&
                     ($template[$offset + 1] === "/") //fast check of initial tag characters
                     &&
                     ($template[$offset + 2] === "C")
                     &&
                     ($template[$offset + 3] === "M")
                     &&
                     ($template[$offset + 4] === "S")
                     &&
                     ($template[$offset + 5] === ":")
                   )
            {
                if (($_offset = strpos($template, ">", $offset + 6)) !== FALSE) //seek tag end
                {
                    $_offset++; //advance position after tag
                    $string = substr($template, $offset, $_offset - $offset); //get closing tag code

                    //verify tag
                    if (
                         preg_match("/^<\/CMS:([a-z]+)\s*>$/", $string, $_string) //extensive check
                         &&
                         isset($this->tlist[$_string[1]]) //check if element is listed
                       )
                    {
                        //template type
                        $_ttype = $_string[1];           //literal
                        $ttype  = $this->tlist[$_ttype]; //numeric

                        //append unparsed element code to nocache buffer
                        if ($nocache_depth && ($ttype !== CMS_TEMPLATE_TYPE_NOCACHE))
                            $nocache_buffer .= $string;

                        if (
                             ($ttype & CMS_TEMPLATE_TYPE_SPAN)                           //spannable element
                             &&
                             isset($span_stack[$ttype])                                  //element is open
                             &&
                             (($edit_display = array_pop($span_stack[$ttype])) !== NULL) //get editing state and remove from span list
                           )
                        {
                            $buffer = "";

                            //process element by type
                            switch ($ttype)
                            {
                            case CMS_TEMPLATE_TYPE_CBLOCK:
//..................................................................................................

                                if ($calt_limit)   break; //suppressed by alternative block
                                if ($edit_limit)   break; //suppressed by non-editable block
                                if ($noedit_limit) break; //suppressed by non-editable block

                                if (

                                //within conditional block
                                     $cblock_depth
                                     &&

                                //decrease block nesting count
                                //to fall below limit
                                     (--$cblock_depth < $cblock_limit)
                                   )

                                    //abolish suppression
                                    $cblock_limit = 0;

                                break;

                            case CMS_TEMPLATE_TYPE_CALT:
//..................................................................................................

                                if ($cblock_limit) break; //suppressed by alternative block
                                if ($edit_limit)   break; //suppressed by non-editable block
                                if ($noedit_limit) break; //suppressed by non-editable block

                                if (

                                //within alternative block
                                     $calt_depth
                                     &&

                                //decrease block nesting count
                                //to fall below limit
                                     (--$calt_depth < $calt_limit)
                                   )

                                    //abolish suppression
                                    $calt_limit = 0;

                                break;

                            case CMS_TEMPLATE_TYPE_GROUP:
//..................................................................................................

                                if ($suppressed) break; //conditionally suppressed

                                //retrieve data from group stack
                                if (count($group_stack) === 0) break; //no group data
                                $_base_id = array_pop($group_stack);  //restore base index

                                //close group span when editing
                                if ($edit_display) $buffer .= "</div>";

                                break;

                            case CMS_TEMPLATE_TYPE_BASE:
//..................................................................................................

                                //retrieve data from base stack
                                if (count($base_stack) === 0) break; //no base data
                                $array    = array_pop($base_stack);
                                $auto_id  = $array["auto_id"];  //restore automatic index
                                $base_id  = $array["base_id"];  //restore base index
                                $_base_id = $array["_base_id"]; //restore base index

                                //remove data from temporary random index stack
                                array_pop($temp_stack);

                                break;

                            case CMS_TEMPLATE_TYPE_NAMESPACE:
//..................................................................................................

                                //retrieve data from namespace stack
                                if (count($namespace_stack) === 0) break; //no namespace data
                                $array    = array_pop($namespace_stack);
                                if ($array["auto_id"] !== NULL) $auto_id = $array["auto_id"]; //restore automatic index
                                $_base_id = $array["_base_id"];                               //restore base index

                                break;

                            case CMS_TEMPLATE_TYPE_REPEAT:
//..................................................................................................

                                //retrieve data from repeat stack
                                if (count($repeat_stack) === 0) break; //no repeat task
                                $array = array_pop($repeat_stack);
                                $count = --$array["count"];

                                //decrease repeat count
                                if ($count === 0) //end repeat task
                                {
                                    $_base_id = $array["base_id"]; //restore base index
                                    break;
                                };

                                //jump back
                                $_offset = $array["offset"];

                                //put updated repeat task on stack
                                array_push($repeat_stack, $array);

                                //memorize spannable element as open
                                array_push($span_stack[CMS_TEMPLATE_TYPE_REPEAT], $edit_display);
                                $edit_display = FALSE; //set false so dragdrop container will not be closed yet

                                break;

                            case CMS_TEMPLATE_TYPE_SHIFT:
//..................................................................................................

                                if ($suppressed) break; //conditionally suppressed

                                //retrieve data from shift stack
                                if (count($shift_stack) === 0) break; //no shift data
                                $_base_id = array_pop($shift_stack);  //restore base index

                                break;

                            case CMS_TEMPLATE_TYPE_HREF:
                            case CMS_TEMPLATE_TYPE_DOWNLOAD:
//..................................................................................................

                                if ($suppressed)   break; //conditionally suppressed
                                if ($edit_display) break; //skip if element is displayed for editing
                                if ($preview)      break; //suppressed by preview

                                //close html link
                                $buffer .= "</a>";

                                break;

                            case CMS_TEMPLATE_TYPE_NOCACHE:
//..................................................................................................

                                if ($suppressed) break; //conditionally suppressed
                                if (! $cache)    break; //do not generate cache version

                                //not within uncached span
                                if ($nocache_depth && (! --$nocache_depth))

                                    //inject nocache buffer
                                    $output = substr($output, 0, $nocache_insert) .
                                              CMS_TEMPLATE_CACHE_SEPARATOR .
                                              $nocache_buffer .
                                              CMS_TEMPLATE_CACHE_SEPARATOR .
                                              substr($output, $nocache_insert) .
                                              CMS_TEMPLATE_CACHE_SEPARATOR;

                                break;

                            case CMS_TEMPLATE_TYPE_CEDIT:
//..................................................................................................

                                if ($cblock_limit) break; //suppressed by conditional block
                                if ($calt_limit)   break; //suppressed by alternative block
                                if ($noedit_limit) break; //suppressed by non-editable block

                                if (

                                //within edit block
                                     $edit_depth
                                     &&

                                //decrease block nesting count
                                //to fall below limit
                                     (--$edit_depth < $edit_limit)
                                   )

                                    //abolish suppression
                                    $edit_limit = 0;

                                break;

                            case CMS_TEMPLATE_TYPE_CNOEDIT:
//..................................................................................................

                                if ($cblock_limit)         break; //suppressed by conditional block
                                if ($calt_limit)           break; //suppressed by alternative block
                                if ($edit_limit)           break; //suppressed by editable block
                                if (! CMS_TEMPLATE_OPTION) break; //not editing

                                if (

                                //within noedit block
                                     $noedit_depth
                                     &&

                                //decrease block nesting count
                                //to fall below limit
                                     (--$noedit_depth < $noedit_limit)
                                   )

                                    //abolish suppression
                                    $noedit_limit = 0;

                                break;
                            };

                            //close dragdrop container
                            if ($edit_display) $buffer .= "</div>";

                            //append parsed element code to output
                            $output .= $buffer;

                            //restore automatic index on spannable path extending element
                            if (
                                 (! $this->compat_mode)
                                 &&
                                 ($ttype & CMS_TEMPLATE_TYPE_PATH)
                                 &&
                                 (count($auto_id_stack) > 0)
                               )
                                $auto_id = array_pop($auto_id_stack);
                        };

                        //debug information
                        if ($debug)
                        {
                            //no open tag
                            if (
                                 (! isset($debug_stack[$ttype]))
                                 ||
                                 (count($debug_stack[$ttype]) === 0)
                               )
                            {
                                //error
                                $output .= "<!-- #debug# error: redundant closing tag /$_ttype -->";
                            }

                            //repeat
                            elseif ($ttype & CMS_TEMPLATE_TYPE_REPEAT)
                            {
                                //repetitions remaining
                                if ($count > 0)
                                {
                                    //retrieve data from debug stack
                                    $info    = end($debug_stack[$ttype]);

                                    //add element end marker
                                    $output .= "<!-- #debug# /$info ($count left) -->" .

                                    //add element start marker
                                               "<!-- #debug# $info -->";
                                }

                                //final repetition
                                else
                                {
                                    //retrieve data from debug stack
                                    $info    = array_pop($debug_stack[$ttype]);

                                    //add element end marker
                                    $output .= "<!-- #debug# /$info (final) -->";
                                };
                            }

                            //other
                            else
                            {
                                //retrieve data from debug stack
                                $info    = array_pop($debug_stack[$ttype]);

                                //add element end marker
                                $output .= "<!-- #debug# /$info -->";
                            };
                        };
                    };

                    //continue after closing tag
                    $offset = $_offset;
                };
            }

            //unparsed data
            else
            {
                //proceed after current position
                $_offset = $offset;
                $offset++;
            };
        };

        //append offcut to output
        $output .= substr($template, $_offset);

        //decrease template nesting counter
        $template_depth--;

        //base level
        if ($template_depth === -1)
        {
            //insert meta data into head
            if ($insert >= 0)
            {
                //html meta data

                //character set
                $buffer  = "<meta charset=\"utf-8\">";

                //title
                $value   = ($title_wrapper === NULL) ? $this->title : sprintf($title_wrapper, $this->title);
                $value   = stripspaces($value);
                $buffer .= "\n<title>" .
                           x($value) .
                           "</title>";

                //description
                $value   = remove_format($this->description, TRUE, TRUE);
                $value   = stripspaces($value);
                $buffer .= "\n<meta name=\"description\" " .
                                   "content=\"" . x($value) . "\">";

                //keywords
                $value   = explode(",", $this->keyword);
                $value   = array_map(__NAMESPACE__ . "\\stripspaces", $value);
                $value   = array_filter($value, __NAMESPACE__ . "\\nstre");
                $value   = array_map(__NAMESPACE__ . "\\utf8_strtolower", $value);
                $value   = array_unique($value);
                $value   = implode(", ", $value);
                $buffer .= "\n<meta name=\"keywords\" " .
                                   "content=\"" . x($value) . "\">" .

                //generating software --- DO NOT MODIFY OR REMOVE ---
                           "\n<meta name=\"generator\" " .
                                   "content=\"" . x(CMS_IDENTIFIER) . "\">";

                //base url
                if ($base_url !== NULL) $buffer .= "\n<base href=\"" . x($base_url) . "\">";

                //directory entry
                if (defined("CMS_CONTENT_DIRECTORY_INDEX") && (CMS_CONTENT_DIRECTORY_INDEX !== 0))
                {
                    //canonical
                    $language = nstre(CMS_LANGUAGE) ? CMS_LANGUAGE . "." : "";
                    $map      = new map("#system/" . $language . "directory.canonical");
                    $query    = querystring($this->query_data);

                    if (($url = $map->get_value(CMS_CONTENT_DIRECTORY_INDEX)) !== NULL)
                        $buffer .= "\n<link rel=\"canonical\" " .
                                           "href=\"" . x($url . $query) . "\">";

                    //alternate, canonical only
                    if (streq(translate_url("directory://" . CMS_CONTENT_DIRECTORY_INDEX), $url))
                    {
                        $language = explode(",", CMS_LANGUAGE_ENABLED);

                        foreach ($language AS $value)
                        {
                            $map = new map("#system/$value.directory.canonical");

                            if (($url = $map->get_value(CMS_CONTENT_DIRECTORY_INDEX)) !== NULL)
                            {
                                $buffer .= "\n<link rel=\"alternate\" " .
                                                   "href=\"" . x($url . $query) . "\" " .
                                                   "hreflang=\"" . x($value) . "\">";

                                if (streq($value, CMS_LANGUAGE_DEFAULT) && nstreq($value, "x-default"))
                                    $buffer .= "\n<link rel=\"alternate\" " .
                                                       "href=\"" . x($url . $query) . "\" " .
                                                       "hreflang=\"x-default\">";
                            };
                        };
                    };
                }

                //no directory entry
                elseif (defined("CMS_CONTENT_INDEX") && cms_load("directory"))
                {
                    $url      = substr(CMS_MODULES_URL, strlen(CMS_ROOT_URL)) . "content.php";
                    $url      = absolute_path(directory_get_canonical(), $url);
                    $language = explode(",", CMS_LANGUAGE_ENABLED, 2);

                    //canonical
                    $_language  = (count($language) > 1) ? CMS_LANGUAGE : NULL;
                    $array      = ["content_index"       => CMS_CONTENT_INDEX,
                                   "cms_select_language" => $_language];
                    $_url       = $url . querystring($this->query_data, $array);
                    $buffer    .= "\n<link rel=\"canonical\" " .
                                          "href=\"" . x($_url) . "\">";

                    //alternate
                    foreach ($language AS $value)
                    {
                        $array   = ["content_index"       => CMS_CONTENT_INDEX,
                                    "cms_select_language" => $value];
                        $_url    = $url . querystring($this->query_data, $array);
                        $buffer .= "\n<link rel=\"alternate\" " .
                                           "href=\"" . x($url) . "\" " .
                                           "hreflang=\"" . x($value) . "\">";

                        if (streq($value, CMS_LANGUAGE_DEFAULT) && nstreq($value, "x-default"))
                            $buffer .= "\n<link rel=\"alternate\" " .
                                               "href=\"" . x($url) . "\" " .
                                               "hreflang=\"x-default\">";
                    };
                };

                //page stylesheet
                if ($stylesheet !== NULL) $buffer .= "\n$stylesheet";

                //preload employed assets
                foreach ($template_cache as $key => $value)
                {
                    //stylesheet
                    if (isset($value[4]) && ($value[4] !== FALSE))
                        $buffer .= "\n<link rel=\"preload\" " .
                                           "href=\"" . x(CMS_DATA_URL . $value[4]) . "\" " .
                                           "as=\"style\">";

                    //javascript
                    if (isset($value[5]) && ($value[5] !== FALSE))
                        $buffer .= "\n<link rel=\"preload\" " .
                                           "href=\"" . x(CMS_DATA_URL . $value[5]) . "\" " .
                                           "as=\"script\">";
                };

                //template editing stylesheet
                if (CMS_TEMPLATE_EDIT)
                    $buffer .= "\n" . stylesheet(CMS_URL . "stylesheet/template.css", FALSE);

                //default javascript libraries
                $_buffer  = x(CMS_JAVASCRIPT_URL);
                $buffer  .= "\n<script src=\"" . $_buffer . "common.js\">" .
                            "</script>" .
                            "\n<script src=\"" . $_buffer . "fx.js\">" .
                            "</script>" .
                            "\n<script src=\"" . $_buffer . "defer.js\" defer>" .
                            "</script>" .
                            "<noscript>" .
                            "<style>" .
                            "IMG[data-defer-src]" .
                            "{" .
                            "DISPLAY: none !important;" .
                            "}" .
                            "</style>" .
                            "</noscript>";

                //javascript libraries required for editing
                if (CMS_TEMPLATE_EDIT && (! $preview))
                {
                    $buffer .= "\n<script src=\"" . $_buffer . "template.js\">" .
                               "</script>";

                    //not output mode
                    if (CMS_TEMPLATE_OPTION)
                    {
                        $buffer .= "\n<script src=\"" . $_buffer . "dragdrop.js\">" .
                                   "</script>" .

                        //dragdrop action
                                   "<script>" .

                                   "function tp_dd_event(event, source, target)" .
                                   "{" .
                                   "switch (event)" .
                                   "{" .

                        //element is doubleclicked
                                   "case \"dblclick\":" .

                                   "var id     = source.getAttribute(\"data-tp-id\");" .
                                   "var object = document.getElementById(\"tp-edt-a-\" + id);" .
                                   "if (object !== null) object.click();" .
                                   "break;" .

                        //element is about to be dragged
                                   "case \"beforedragstart\":" .

                                   "tp_beforedragstart();" .
                                   "break;";

                        //element is dropped on target
                        if (isset($this->action[CMS_TEMPLATE_COMMAND][CMS_TEMPLATE_COMMAND_DRAGDROP1]))
                        {
                            $buffer .=
                               "case \"dropon\":" .

                               "tp_drop();" .
                               "new Function(\"" . q($this->action[CMS_TEMPLATE_COMMAND][CMS_TEMPLATE_COMMAND_DRAGDROP1]) . "\"" .
                               ".replace(/%index%/g,     encodeURIComponent(source.getAttribute(\"data-tp-index\")))" .
                               ".replace(/%path%/g,      encodeURIComponent(source.getAttribute(\"data-tp-path\")))" .
                               ".replace(/%value%/g,     encodeURIComponent(target.getAttribute(\"data-tp-path\")))" .
                               ".replace(/%type%/g,      encodeURIComponent(source.getAttribute(\"data-tp-type\")))" .
                               ".replace(/%reference%/g, encodeURIComponent(source.getAttribute(\"data-tp-reference\")))" .
                               ".replace(/%id%/g,        encodeURIComponent(target.getAttribute(\"data-tp-id\"))))();" .
                               "break;";
                        };

                        //element is dropped on target modified with shift/ctrl/alt
                        if (isset($this->action[CMS_TEMPLATE_COMMAND][CMS_TEMPLATE_COMMAND_DRAGDROP2]))
                        {
                            $buffer .=
                               "case \"dropon_alt\":" .

                               "tp_drop();" .
                               "new Function(\"" . q($this->action[CMS_TEMPLATE_COMMAND][CMS_TEMPLATE_COMMAND_DRAGDROP2]) . "\"" .
                               ".replace(/%index%/g,     encodeURIComponent(source.getAttribute(\"data-tp-index\")))" .
                               ".replace(/%path%/g,      encodeURIComponent(source.getAttribute(\"data-tp-path\")))" .
                               ".replace(/%value%/g,     encodeURIComponent(target.getAttribute(\"data-tp-path\")))" .
                               ".replace(/%type%/g,      encodeURIComponent(source.getAttribute(\"data-tp-type\")))" .
                               ".replace(/%reference%/g, encodeURIComponent(source.getAttribute(\"data-tp-reference\")))" .
                               ".replace(/%id%/g,        encodeURIComponent(target.getAttribute(\"data-tp-id\"))))();" .
                               "break;";
                        };

                        $buffer .= "case \"drop\":" .
                                   "case \"drop_alt\":" .

                                   "tp_drop();" .
                                   "};" .
                                   "};" .

                        //register event callback function
                                    "dd_set_callback(tp_dd_event);" .

                        //display scroll area on touch screen devices
                                    "document.documentElement.classList.add(\"tp-touchbar\");" .
                                    "</script>";
                    };
                };

                //additional header information
                if (nstre($this->header))
                    $buffer .= "\n" . $this->header;

                if ($debug) $insert += 21; //add debug string length

                //insert
                $output = substr($output, 0, $insert) .
                          $buffer .
                          substr($output, $insert);
            };
        };

        //return parsed data
        return $output;
    }

//--------------------------------------------------------------------------------------------------

    function export(&$document,
                    $index,
                    $base_id      = NULL, //do not change for page template
                    $return_asset = FALSE)
    {
        static $template_list  = []; //list of employed templates
        static $template_depth = -1; //template nesting counter
        static $depth          = []; //depth count

        //get template code
        if (
             (stre($index) || stre($template = $this->get_code($index)))
             &&
             nstre($index = $document->get($base_id, "template"))
           )
            $template = $this->get_code($index);

        if (stre($template)) return FALSE; //no template code

        //add path separator
        if (nstre($base_id)) $base_id .= ".";

        //increase template nesting counter
        $template_depth++;

        //initialize
        if (! $template_depth) //zero on first pass
        {
            $template_list = []; //reset list of employed templates
            $depth         = []; //reset depth count
        };

        $offset          = 0;                 //primary text position
        $_offset         = 0;                 //secondary text position
        $length          = strlen($template); //template code length

        $auto_id         = 0;                 //automatic element index
        $_base_id        = $base_id;          //base index working copy
        $no_id           = TRUE;              //no explicit id given

        $auto_id_stack   = [];                //automatic element index stack
        $group_stack     = [];                //group data stack
        $base_stack      = [];                //base data stack
        $namespace_stack = [];                //namespace data stack
        $repeat_stack    = [];                //repeat data stack
        $shift_stack     = [];                //shift data stack

        $cblock_stack    = [];                //conditional block stack
        $calt_depth      = 0;                 //conditional alternative depth
        $calt_limit      = 0;                 //conditional alternative limit

        $output          = NULL;              //output buffer

        //add to list of employed templates
        $template_list[$index] = TRUE;

        //link stylesheet of employed template
        //every stylesheet is added and sorted out later when parsing
        //because we can not know yet which path will be displayed
        if (! $return_asset)
            $output .= "<CMS:stylesheet default=\"" . x($index) . "\"/>" .
                       "<CMS:javascript default=\"" . x($index) . "\"/>";

        //process template code
        while (
                ($offset < $length) //not at end of template code
                &&
                (($offset = strpos($template, "<", $offset)) !== FALSE) //seek next opening tag
              )
        {
            //append preliminary unparsed data to output if not suppressed
            if (! $calt_limit) $output .= substr($template, $_offset, $offset - $_offset);

            //start tag
            if (
                 (($offset + 5) < $length)        //considers upcoming strpos
                 &&
                 ($template[$offset + 1] === "C") //fast check of initial tag characters
                 &&
                 ($template[$offset + 2] === "M")
                 &&
                 ($template[$offset + 3] === "S")
                 &&
                 ($template[$offset + 4] === ":")
               )
            {
                if (($_offset = strpos($template, ">", $offset + 5)) !== FALSE) //seek tag end
                {
                    $_offset++; //advance position after tag
                    $string = substr($template, $offset, $_offset - $offset); //get tag code

                    //verify tag
                    if (
                         preg_match("/^<CMS:([a-z]+)(?:\s|\/|>)/", $string, $_string) //extensive check
                         &&
                         isset($this->tlist[$_string[1]]) //check if element is listed
                       )
                    {
                        //template type
                        $_ttype = $_string[1];           //literal
                        $ttype  = $this->tlist[$_ttype]; //numeric

                        //store and reset automatic index on spannable path extending element
                        if (
                             (! $this->compat_mode) //skip for older versions
                             &&
                             ($ttype & CMS_TEMPLATE_TYPE_SPAN & CMS_TEMPLATE_TYPE_PATH)
                           )
                        {
                            $auto_id_stack[] = $auto_id;
                            $auto_id         = 0;
                        };

                        //id
                        $no_id = ! $this->parse_id(
                            template_get_attribute($string, "id"),
                            $base_id,
                            $_base_id,
                            $auto_id,
                            $_id,  //data index
                            $__id, //path index
                            $id);  //element index

                        //get value
                        switch ($ttype)
                        {
                        case CMS_TEMPLATE_TYPE_REPEAT:
                        case CMS_TEMPLATE_TYPE_TEMPLATE:
//..................................................................................................

                            //default value
                            $value = template_get_attribute($string, "default");
                            if (isset($value)) $document->set_default($_id, $_ttype, $value);

                            //content value
                            $value  = $document->get($_id, $_ttype);
                            $cvalue = nstre($value) ? l($value) : NULL;

                            break;
                        };

                        //process element by type
                        switch ($ttype)
                        {
                        case CMS_TEMPLATE_TYPE_HEAD:
//..................................................................................................

                            //pass through
                            $output .= $string;

                            if (! $return_asset)
                            {
                                //link stylesheet of employed template
                                if (($file = $this->get_stylesheet($index, NULL, TRUE)) !== FALSE)
                                    $output .= "\n" . stylesheet(CMS_DATA_URL . $file, FALSE);

                                //link javascript of employed template
                                if (($file = $this->get_javascript($index, NULL, TRUE)) !== FALSE)
                                    $output .= "\n" . javascript(CMS_DATA_URL . $file, FALSE);
                            };

                            break;

                        case CMS_TEMPLATE_TYPE_CBLOCK:
//..................................................................................................

                            if ($calt_limit) break; //suppressed by alternative block

                            //idref
                            $this->parse_id(
                                template_get_attribute($string, "idref"),
                                $base_id,
                                $_base_id,
                                $auto_id,
                                $_idref,  //data index
                                $__idref, //path index (not used)
                                $idref);  //element index

                            //referenced element is a template and value is set
                            $value = l($document->get($_idref, "template"));
                            while (nstre($value))
                            {
                                //regex is set
                                $regex = template_get_attribute($string, "regex");
                                if (nstre($regex))
                                {
                                    //no match -> exit while and adopt element
                                    if (! preg_match($regex, $value)) break;
                                };

                                //mark associated closing tag to be discarded
                                array_push($cblock_stack, FALSE);
                                break 2; //exit switch
                            };

                            //referenced element is not a template
                            //or value is not set or regex does not match

                            //set reference index and adopt conditional block
                            $output .= template_set_attribute($string, "idref", $idref);

                            //mark associated closing tag to be applied
                            array_push($cblock_stack, TRUE);

                            break;

                        case CMS_TEMPLATE_TYPE_CALT:
//..................................................................................................

                            //idref
                            $this->parse_id(
                                template_get_attribute($string, "idref"),
                                $base_id,
                                $_base_id,
                                $auto_id,
                                $_idref,  //data index
                                $__idref, //path index (not used)
                                $idref);  //element index

                            //increase block nesting count
                            $calt_depth++;

                            if ($calt_limit) break; //suppressed by alternative block

                            //referenced element is a template and value is set
                            if (nstre(l($document->get($_idref, "template"))))

                                //set suppression
                                $calt_limit = $calt_depth;

                            //referenced element is not a template or no value is set
                            else

                                //set reference index and adopt alternative block
                                $output .= template_set_attribute($string, "idref", $idref);

                            break;

                        case CMS_TEMPLATE_TYPE_GROUP:
//..................................................................................................

                            if ($calt_limit) break; //suppressed by alternative block

                            //put data on group stack
                            array_push($group_stack, $_base_id); //backup base index

                            $_base_id = "$__id."; //set base index

                            //set index and adopt element
                            $output .= template_set_attribute($string, "id", $id);

                            break;

                        case CMS_TEMPLATE_TYPE_BASE:
//..................................................................................................

                            //advance automatic index if no explicit id is given
                            if ($no_id) $auto_id++;

                            //put data on base stack
                            array_push(
                                $base_stack,
                                ["auto_id"  => $auto_id,    //backup automatic index
                                 "base_id"  => $base_id,    //backup base index
                                 "_base_id" => $_base_id]); //backup base index

                            $auto_id  = 0;        //set automatic index
                            $base_id  = "$__id."; //set base index
                            $_base_id = "$__id."; //set base index

                            if ($calt_limit) break; //suppressed by alternative block

                            //set index and adopt element
                            $output .= template_set_attribute($string, "id", $id);

                            break;

                        case CMS_TEMPLATE_TYPE_NAMESPACE:
//..................................................................................................

                            //contain automatic index
                            $_auto_id = NULL;
                            if (preg_match("/(?:^|\s)contain(?:$|\s)/i",
                                           template_get_attribute($string, "auto-id")))
                            {
                                $_auto_id = $auto_id;
                                $auto_id  = 0;
                            };

                            //put data on namespace stack
                            array_push(
                                $namespace_stack,
                                ["auto_id"  => $_auto_id,   //backup automatic index
                                 "_base_id" => $_base_id]); //backup base index

                            $_base_id = "$__id."; //set base index

                            if ($calt_limit) break; //suppressed by alternative block

                            //set index and adopt element
                            $output .= template_set_attribute($string, "id", $id);

                            break;

                        case CMS_TEMPLATE_TYPE_REPEAT:
//..................................................................................................

                            $cvalue = max((int)$cvalue, 1); //at least one repetition

                            //put data on repeat stack
                            array_push(
                                $repeat_stack,
                                ["count"   => $cvalue,     //repeat count
                                 "offset"  => $_offset,    //repeat offset
                                 "base_id" => $_base_id]); //backup base index

                            $_base_id = "$__id."; //set base index

                            if ($calt_limit) break; //suppressed by alternative block

                            //replace with namespace
                            $output .= "<CMS:namespace id=\"$id\">";

                            break;

                        case CMS_TEMPLATE_TYPE_SHIFT:
//..................................................................................................

                            if ($calt_limit) break; //suppressed by alternative block

                            //put data on shift stack
                            array_push($shift_stack, $_base_id); //backup base index

                            $_base_id  = "$__id."; //set base index

                            //replace with namespace
                            $output   .= "<CMS:namespace id=\"$id\">";

                            break;

                        case CMS_TEMPLATE_TYPE_TEMPLATE:
//..................................................................................................

                            if ($calt_limit)              break; //suppressed by alternative block
                            if (streq("$_id.", $base_id)) break; //prevent infinite loop
                            if ($template_depth >= 50)    break; //prevent circular recursion

                            //subtemplate is given
                            if ($cvalue)
                            {
                                //add template start marker
                                $name    = x(l($this->data->get($cvalue, "name")));
                                $name    = preg_replace("/-{2,}/", "-", $name);
                                $output .= "<!-- template $name -->" .

                                //recursively call export function to process subtemplate
                                           "<CMS:base id=\"$id\">" .
                                           $this->export($document,       //document
                                                         $cvalue,         //index
                                                         $__id,           //base_id
                                                         $return_asset) . //return asset
                                           "</CMS:base>" .

                                //add template end marker
                                           "<!-- /template $name -->";
                                break;
                            };

                            //set index and adopt element
                            $output .= template_set_attribute($string, "id", $id);

                            break;

                        case CMS_TEMPLATE_TYPE_NOCACHE:
                        case CMS_TEMPLATE_TYPE_CEDIT:
                        case CMS_TEMPLATE_TYPE_CNOEDIT:
//..................................................................................................

                            if ($calt_limit) break; //suppressed by alternative block

                            if (! isset($depth[$ttype])) $depth[$ttype] = 0; //init

                            //if not already within span
                            if (! $depth[$ttype]++)

                                //adopt block
                                $output .= $string;

                            break;

                        default:
//..................................................................................................

                            if ($calt_limit) break; //suppressed by alternative block

                            $output .= ($ttype & CMS_TEMPLATE_TYPE_EDIT) ?          //editable
                                       template_set_attribute($string, "id", $id) : //set index and adopt element
                                       $string;                                     //just pass through

                            break;
                        };

                        //advance automatic index if element is editable
                        if ($ttype & CMS_TEMPLATE_TYPE_EDIT) $auto_id++;
                    };

                    //continue after tag
                    $offset = $_offset;
                };
            }

            //closing tag
            elseif (
                     (($offset + 6) < $length)        //considers upcoming strpos
                     &&
                     ($template[$offset + 1] === "/") //fast check of initial tag characters
                     &&
                     ($template[$offset + 2] === "C")
                     &&
                     ($template[$offset + 3] === "M")
                     &&
                     ($template[$offset + 4] === "S")
                     &&
                     ($template[$offset + 5] === ":")
                   )
            {
                if (($_offset = strpos($template, ">", $offset + 6)) !== FALSE) //seek tag end
                {
                    $_offset++; //advance position after tag
                    $string = substr($template, $offset, $_offset - $offset); //get closing tag code

                    //verify tag
                    if (
                         preg_match("/^<\/CMS:([a-z]+)\s*>$/", $string, $_string) //extensive check
                         &&
                         isset($this->tlist[$_string[1]]) //check if element is listed
                       )
                    {
                        //template type
                        $ttype = $this->tlist[$_string[1]];

                        if ($ttype & CMS_TEMPLATE_TYPE_SPAN) //spannable element
                        {
                            //process element by type
                            switch ($ttype)
                            {
                            case CMS_TEMPLATE_TYPE_CBLOCK:
//..................................................................................................

                                if ($calt_limit) break; //suppressed by alternative block

                                //pass through closing tag if marked to be adopted
                                if (array_pop($cblock_stack)) $output .= $string;

                                break;

                            case CMS_TEMPLATE_TYPE_CALT:
//..................................................................................................

                                //pass through closing tag if not suppressed
                                if (! $calt_limit) $output .= $string;

                                if (

                                //within alternative block
                                     $calt_depth
                                     &&

                                //decrease block nesting count
                                //to fall below limit
                                     (--$calt_depth < $calt_limit)
                                   )

                                    //abolish suppression
                                    $calt_limit = 0;

                                break;

                            case CMS_TEMPLATE_TYPE_GROUP:
//..................................................................................................

                                if ($calt_limit) break; //suppressed by alternative block

                                //retrieve data from group stack
                                if (count($group_stack) === 0) break; //no group data
                                $_base_id = array_pop($group_stack); //restore base index

                                //pass through closing tag
                                $output .= $string;

                                break;

                            case CMS_TEMPLATE_TYPE_BASE:
//..................................................................................................

                                //retrieve data from base stack
                                if (count($base_stack) === 0) break; //no base data
                                $array    = array_pop($base_stack);
                                $auto_id  = $array["auto_id"];  //restore automatic index
                                $base_id  = $array["base_id"];  //restore base index
                                $_base_id = $array["_base_id"]; //restore base index

                                if ($calt_limit) break; //suppressed by alternative block

                                //just pass through
                                $output .= $string;

                                break;

                            case CMS_TEMPLATE_TYPE_NAMESPACE:
//..................................................................................................

                                //retrieve data from namespace stack
                                if (count($namespace_stack) === 0) break; //no namespace data
                                $array    = array_pop($namespace_stack);
                                if ($array["auto_id"] !== NULL) $auto_id = $array["auto_id"]; //restore automatic index
                                $_base_id = $array["_base_id"];                               //restore base index

                                if ($calt_limit) break; //suppressed by alternative block

                                //just pass through
                                $output .= $string;

                                break;

                            case CMS_TEMPLATE_TYPE_REPEAT:
//..................................................................................................

                                //retrieve data from repeat stack
                                if (count($repeat_stack) === 0) break; //no repeat task
                                $array = array_pop($repeat_stack);

                                //decrease repeat count
                                if (--$array["count"] === 0) //end repeat task
                                {
                                    $_base_id = $array["base_id"]; //restore base index

                                    if ($calt_limit) break; //suppressed by alternative block

                                    //replace with namespace
                                    $output .= "</CMS:namespace>";
                                    break;
                                };

                                //jump back
                                $_offset = $array["offset"];

                                //put updated repeat task on stack
                                array_push($repeat_stack, $array);

                                break;

                            case CMS_TEMPLATE_TYPE_SHIFT:
//..................................................................................................

                                if ($calt_limit) break; //suppressed by alternative block

                                //retrieve data from shift stack
                                if (count($shift_stack) === 0) break; //no shift data
                                $_base_id = array_pop($shift_stack); //restore base index

                                //replace with namespace
                                $output .= "</CMS:namespace>";

                                break;

                            case CMS_TEMPLATE_TYPE_NOCACHE:
                            case CMS_TEMPLATE_TYPE_CEDIT:
                            case CMS_TEMPLATE_TYPE_CNOEDIT:
//..................................................................................................

                                if ($calt_limit)             break; //suppressed by alternative block
                                if (! isset($depth[$ttype])) break; //not within span

                                //span ends
                                if ($depth[$ttype] && (! --$depth[$ttype]))

                                    //adopt block end
                                    $output .= $string;

                                break;

                            default:
//..................................................................................................

                                if ($calt_limit) break; //suppressed by alternative block

                                //just pass through
                                $output .= $string;

                                break;
                            };
                        };

                        //restore automatic index on spannable path extending element
                        if (
                             (! $this->compat_mode)
                             &&
                             ($ttype & CMS_TEMPLATE_TYPE_PATH)
                             &&
                             (count($auto_id_stack) > 0)
                           )
                            $auto_id = array_pop($auto_id_stack);
                    };

                    //continue after closing tag
                    $offset = $_offset;
                };
            }

            //unparsed data
            else
            {
                //proceed after current position
                $_offset = $offset;
                $offset++;
            };
        };

        //append offcut to output
        $output .= substr($template, $_offset);

        //decrease template nesting counter
        $template_depth--;

        //base level
        if ($return_asset && ($template_depth === -1))
        {
            //combine stylesheets of employed templates
            $stylesheet = "";
            $javascript = "";

            foreach ($template_list AS $key => $_)
            {
                $name   = l($this->data->get($key, "name"));
                $name   = utf8_strtoupper($name);
                $name   = str_replace("*/", "", $name);
                $buffer = str_repeat("=", 93);
                $buffer = "/*===" . $buffer . "==\n" .
                          "     " . $name .     "\n" .
                          "=====" . $buffer . "*/\n";

                if (nstre($file = $this->get_stylesheet($key, NULL)))
                    $stylesheet .= "$buffer\n$file\n\n";

                if (nstre($file = $this->get_javascript($key, NULL)))
                    $javascript .= "$buffer\n$file\n\n";
            };

            $stylesheet = substr($stylesheet, 0, -2);
            $javascript = substr($javascript, 0, -2);

            return ["code" => $output, "stylesheet" => $stylesheet, "javascript" => $javascript];
        };

        //return parsed data
        return $output;
    }

//--------------------------------------------------------------------------------------------------

    function structure(&$document,
                       $index,
                       $base_id = NULL) //do not change for page template
    {
        static $template_depth = -1; //template nesting counter

        //get template code
        $template = $this->get_code($index);
        if (stre($template)) return FALSE; //no template code

        //add path separator
        if (nstre($base_id)) $base_id .= ".";

        //increase template nesting counter
        $template_depth++;

        //initialize
        if (! $template_depth) //zero on first pass
        {
            $this->structure_id = 1; //set structure index
            $this->parent_id    = 0; //set parent index
        };

        $offset          = 0;                 //primary text position
        $_offset         = 0;                 //secondary text position
        $length          = strlen($template); //template code length

        $auto_id         = 0;                 //automatic element index
        $_base_id        = $base_id;          //base index working copy
        $no_id           = TRUE;              //no explicit id given

        $auto_id_stack   = [];                //automatic element index stack
        $parent_stack    = [];                //parent stack
        $group_stack     = [];                //group data stack
        $base_stack      = [];                //base data stack
        $namespace_stack = [];                //namespace data stack
        $repeat_stack    = [];                //repeat data stack
        $shift_stack     = [];                //shift data stack

        $structure       = [];                //structure

        //process template code
        while (
                ($offset < $length) //not at end of template code
                &&
                (($offset = strpos($template, "<", $offset)) !== FALSE) //seek next opening tag
              )
        {
            //start tag
            if (
                 (($offset + 5) < $length)        //considers upcoming strpos
                 &&
                 ($template[$offset + 1] === "C") //fast check of initial tag characters
                 &&
                 ($template[$offset + 2] === "M")
                 &&
                 ($template[$offset + 3] === "S")
                 &&
                 ($template[$offset + 4] === ":")
               )
            {
                if (($_offset = strpos($template, ">", $offset + 5)) !== FALSE) //seek tag end
                {
                    $_offset++; //advance position after tag
                    $string = substr($template, $offset, $_offset - $offset); //get tag code

                    //verify tag
                    if (
                         preg_match("/^<CMS:([a-z]+)(?:\s|\/|>)/", $string, $_string) //extensive check
                         &&
                         isset($this->tlist[$_string[1]]) //check if element is listed
                       )
                    {
                        //template type
                        $_ttype = $_string[1];           //literal
                        $ttype  = $this->tlist[$_ttype]; //numeric

                        //switch equals value
                        if ($ttype === CMS_TEMPLATE_TYPE_SWITCH)    $_ttype = "value";

                        //thumbnail equals image
                        if ($ttype === CMS_TEMPLATE_TYPE_THUMBNAIL) $_ttype = "image";

                        //editable or path extending element
                        if ($ttype & (CMS_TEMPLATE_TYPE_EDIT | CMS_TEMPLATE_TYPE_PATH))
                        {
                            //store and reset automatic index on spannable path extending element
                            if (
                                 (! $this->compat_mode) //skip for older versions
                                 &&
                                 ($ttype & CMS_TEMPLATE_TYPE_SPAN & CMS_TEMPLATE_TYPE_PATH)
                               )
                            {
                                $auto_id_stack[] = $auto_id;
                                $auto_id         = 0;
                            };

                            //id
                            $no_id = ! $this->parse_id(
                                template_get_attribute($string, "id"),
                                $base_id,
                                $_base_id,
                                $auto_id,
                                $id,   //data index
                                $_id); //path index

                            //add element to structure
                            $structure[$this->structure_id] =
                                [CMS_TEMPLATE_STRUCTURE_INDEX  => $id,
                                 CMS_TEMPLATE_STRUCTURE_PATH   => $_id,
                                 CMS_TEMPLATE_STRUCTURE_PARENT => $this->parent_id,
                                 CMS_TEMPLATE_STRUCTURE_TYPE   => $_ttype];

                            if ($ttype & CMS_TEMPLATE_TYPE_PATH) //path extending element
                            {
                                //put current parent on stack
                                array_push($parent_stack, $this->parent_id);

                                //set current element as parent
                                $this->parent_id = $this->structure_id;
                            };

                            //increase structure index
                            $this->structure_id++;
                        };

                        //get value
                        switch ($ttype)
                        {
                        case CMS_TEMPLATE_TYPE_REPEAT:
                        case CMS_TEMPLATE_TYPE_TEMPLATE:
//..................................................................................................

                            //default value
                            $value = template_get_attribute($string, "default");
                            if (isset($value)) $document->set_default($id, $_ttype, $value);

                            //content value
                            $value  = $document->get($id, $_ttype);
                            $cvalue = nstre($value) ? l($value) : NULL;

                            break;
                        };

                        //process element by type
                        switch ($ttype)
                        {
                        case CMS_TEMPLATE_TYPE_GROUP:
//..................................................................................................

                            //put data on group stack
                            array_push($group_stack, $_base_id); //backup base index

                            $_base_id = "$_id."; //set base index

                            break;

                        case CMS_TEMPLATE_TYPE_BASE:
//..................................................................................................

                            //advance automatic index if no explicit id is given
                            if ($no_id) $auto_id++;

                            //put data on base stack
                            array_push(
                                $base_stack,
                                ["auto_id"  => $auto_id,    //backup automatic index
                                 "base_id"  => $base_id,    //backup base index
                                 "_base_id" => $_base_id]); //backup base index

                            $auto_id  = 0;       //set automatic index
                            $base_id  = "$_id."; //set base index
                            $_base_id = "$_id."; //set base index

                            break;

                        case CMS_TEMPLATE_TYPE_NAMESPACE:
//..................................................................................................

                            //contain automatic index
                            $_auto_id = NULL;
                            if (preg_match("/(?:^|\s)contain(?:$|\s)/i",
                                           template_get_attribute($string, "auto-id")))
                            {
                                $_auto_id = $auto_id;
                                $auto_id  = 0;
                            };

                            //put data on namespace stack
                            array_push(
                                $namespace_stack,
                                ["auto_id"  => $_auto_id,   //backup automatic index
                                 "_base_id" => $_base_id]); //backup base index

                            $_base_id = "$_id."; //set base index

                            break;

                        case CMS_TEMPLATE_TYPE_REPEAT:
//..................................................................................................

                            $cvalue = max((int)$cvalue, 1); //at least one repetition

                            //put data on repeat stack
                            array_push(
                                $repeat_stack,
                                ["count"   => $cvalue,     //repeat count
                                 "offset"  => $_offset,    //repeat offset
                                 "base_id" => $_base_id]); //backup base index

                            $_base_id = "$_id."; //set base index

                            break;

                        case CMS_TEMPLATE_TYPE_SHIFT:
//..................................................................................................

                            //put data on shift stack
                            array_push($shift_stack, $_base_id); //backup base index

                            $_base_id = "$_id."; //set base index

                            break;

                        case CMS_TEMPLATE_TYPE_TEMPLATE:
//..................................................................................................

                            if (streq("$id.", $base_id)) break; //prevent infinite loop
                            if ($template_depth >= 50)   break; //prevent circular recursion

                            //recursively call structure function to process subtemplate
                            if ($array = $this->structure($document, $cvalue, $_id))

                                //integrate substructure
                                $structure += $array;

                            //retrieve previous parent index from stack
                            //to be done here because there is no closing tag
                            $this->parent_id = array_pop($parent_stack);

                            break;
                        };

                        //advance automatic index if element is editable
                        if ($ttype & CMS_TEMPLATE_TYPE_EDIT) $auto_id++;
                    };

                    //continue after tag
                    $offset = $_offset;
                };
            }

            //closing tag
            elseif (
                     (($offset + 6) < $length)        //considers upcoming strpos
                     &&
                     ($template[$offset + 1] === "/") //fast check of initial tag characters
                     &&
                     ($template[$offset + 2] === "C")
                     &&
                     ($template[$offset + 3] === "M")
                     &&
                     ($template[$offset + 4] === "S")
                     &&
                     ($template[$offset + 5] === ":")
                   )
            {
                if (($_offset = strpos($template, ">", $offset + 6)) !== FALSE) //seek tag end
                {
                    $_offset++; //advance position after tag
                    $string = substr($template, $offset, $_offset - $offset); //get closing tag code

                    //verify tag
                    if (
                         preg_match("/^<\/CMS:([a-z]+)\s*>$/", $string, $_string) //extensive check
                         &&
                         isset($this->tlist[$_string[1]]) //check if element is listed
                       )
                    {
                        //template type
                        $ttype = $this->tlist[$_string[1]]; //numeric

                        if ($ttype & CMS_TEMPLATE_TYPE_SPAN) //spannable element
                        {
                            //process element by type
                            switch ($ttype)
                            {
                            case CMS_TEMPLATE_TYPE_GROUP:
//..................................................................................................

                                //retrieve data from group stack
                                if (count($group_stack) === 0) break; //no group data
                                $_base_id = array_pop($group_stack);  //restore base index

                                //retrieve previous parent index from stack
                                $this->parent_id = array_pop($parent_stack);

                                break;

                            case CMS_TEMPLATE_TYPE_BASE:
//..................................................................................................

                                //retrieve data from base stack
                                if (count($base_stack) === 0) break; //no base data
                                $array    = array_pop($base_stack);
                                $auto_id  = $array["auto_id"];  //restore automatic index
                                $base_id  = $array["base_id"];  //restore base index
                                $_base_id = $array["_base_id"]; //restore base index

                                break;

                            case CMS_TEMPLATE_TYPE_NAMESPACE:
//..................................................................................................

                                //retrieve data from namespace stack
                                if (count($namespace_stack) === 0) break; //no namespace data
                                $array    = array_pop($namespace_stack);
                                if ($array["auto_id"] !== NULL) $auto_id = $array["auto_id"]; //restore automatic index
                                $_base_id = $array["_base_id"];                               //restore base index

                                break;

                            case CMS_TEMPLATE_TYPE_REPEAT:
//..................................................................................................

                                //retrieve data from repeat stack
                                if (count($repeat_stack) === 0) break; //no repeat task
                                $array = array_pop($repeat_stack);

                                //decrease repeat count
                                if (--$array["count"] === 0) //end repeat task
                                {
                                    $_base_id = $array["base_id"]; //restore base index

                                    //retrieve previous parent index from stack
                                    $this->parent_id = array_pop($parent_stack);
                                    break;
                                };

                                //jump back
                                $_offset = $array["offset"];

                                //put updated repeat task on stack
                                array_push($repeat_stack, $array);

                                break;

                            case CMS_TEMPLATE_TYPE_SHIFT:
//..................................................................................................

                                //retrieve data from shift stack
                                if (count($shift_stack) === 0) break; //no shift data
                                $_base_id = array_pop($shift_stack);  //restore base index

                                //retrieve previous parent index from stack
                                $this->parent_id = array_pop($parent_stack);

                                break;

                            default:
//..................................................................................................

                                //no path extending element
                                if (! ($ttype & CMS_TEMPLATE_TYPE_PATH)) break;

                                //retrieve previous parent index from stack
                                $this->parent_id = array_pop($parent_stack);

                                break;
                            };
                        };

                        //restore automatic index on spannable path extending element
                        if (
                             (! $this->compat_mode)
                             &&
                             ($ttype & CMS_TEMPLATE_TYPE_PATH)
                             &&
                             (count($auto_id_stack) > 0)
                           )
                            $auto_id = array_pop($auto_id_stack);
                    };

                    //continue after closing tag
                    $offset = $_offset;
                };
            }

            //unparsed data
            else
            {
                //proceed after current position
                $_offset = $offset;
                $offset++;
            };
        };

        //decrease template nesting counter
        $template_depth--;

        return $structure;
    }

//--------------------------------------------------------------------------------------------------

    function create_cache(&$document,
                          $index,
                          $title       = NULL,
                          $description = NULL,
                          $keyword     = NULL,
                          $action      = NULL,
                          $header      = NULL,
                          &$is_dynamic = FALSE)
    {
        $buffer = $this->parse(
            $document,    //document
            $index,       //index
            $title,       //title
            $description, //description
            $keyword,     //keyword
            $action,      //action
            $header,      //header
            NULL,         //base id
            TRUE);        //cache

        $buffer  = explode(CMS_TEMPLATE_CACHE_SEPARATOR, $buffer);
        $cache   = [];
        $output  = [];
        $base_id = NULL;
        $state   = 0;

        foreach ($buffer AS $value)
        {
            switch ($state)
            {
            case 0: //cached

                $cache[]  = $value;
                $output[] = $value;
                $state++;
                break;

            case 2: //uncached

                $is_dynamic = TRUE;

            case 1: //base id

                $cache[] = $value;
                $state++;
                break;

            case 3: //processed

                $output[] = $value;
                $state    = 0;
            };
        };

        return ["cache"  => implode(CMS_TEMPLATE_CACHE_SEPARATOR, $cache),
                "output" => implode("", $output)];
    }

//--------------------------------------------------------------------------------------------------

    function process_cache(&$document,
                           $template,
                           $title       = NULL,
                           $description = NULL,
                           $keyword     = NULL,
                           $action      = NULL,
                           $header      = NULL,
                           &$is_dynamic = FALSE)
    {
        $this->action      = $action;
        $this->title       = l($title);
        $this->description = l($description);
        $this->keyword     = l($keyword);
        $this->header      = $header;

        $template = explode(CMS_TEMPLATE_CACHE_SEPARATOR, $template);
        $output   = [];
        $base_id  = NULL;
        $state    = 0;

        foreach ($template AS $value)
        {
            switch ($state)
            {
            case 0: //cached

                $output[] = $value;
                $state++;
                break;

            case 1: //base id

                $base_id = $value;
                $state++;
                break;

            case 2: //uncached

                $output[]   = $this->_parse($document, $value, $base_id, -1);
                $state      = 0;
                $is_dynamic = TRUE;
            };
        };

        return implode("", $output);
    }

//--------------------------------------------------------------------------------------------------

    function execute($cms_template_code,
                     $cms_template_document,
                     $cms_template_base_id,
                     $cms_template_path_id,
                     $cms_template_temp_id)
    {
        //temporarily replace error handler
        set_error_handler(__NAMESPACE__ . "\\template_error");

        //import variables
        if (
             isset($this->execute_vars[$cms_template_temp_id])
             &&
             is_array($this->execute_vars[$cms_template_temp_id])
           )
            extract($this->execute_vars[$cms_template_temp_id], EXTR_SKIP);

        //execute code
        $cms_template_return = eval("namespace " . __NAMESPACE__ . ";$cms_template_code");

        //store variables
        $this->execute_vars[$cms_template_temp_id] = get_defined_vars();

        //restore error handler
        restore_error_handler();

        return $cms_template_return;
    }

//--------------------------------------------------------------------------------------------------

    function attribute($param,
                       $alter = NULL,
                       $skip  = NULL)
    {
        if (! is_array($param)) return FALSE;
        if (is_array($alter))   $param = array_merge($param, $alter);
        if (! is_array($skip))  $skip  = [$skip];

        $_skip  = ["id", "idref", "default", "edit", "title"];
        $return = "";

        ksort($param);
        foreach ($param AS $key => $value)
        {
            if (stre($key))                     continue;
            if ($value === NULL)                continue;
            if (isset($param["_$key"]))         continue;
            if (in_array($key, $skip))          continue;
            if (in_array($key, $_skip))         continue;
            if (streq(substr($key, 0, 1), "_")) $key = substr($key, 1);
            $return .= " $key=\"" . x($value) . "\"";
        };

        return $return;
    }

//--------------------------------------------------------------------------------------------------

    function parse_id($string,
                      $id_base,
                      $id_path,
                      $id_auto,
                      &$id_data_new = NULL,
                      &$id_path_new = NULL,
                      &$id_elem_new = NULL)
    {
        while (TRUE)
        {
            while (nstre($string))
            {
                //process relative automatic indexes
                $id_elem_new = preg_replace_callback(
                    "/(?<=^|\.)(\+|-)([0-9]+)(?=$|\.)/",
                    function($a) use (&$id_auto) { return $id_auto + (($a[1] === "+") ? $a[2] : -$a[2]); },
                    $string);

                //verify id
                if (! preg_match("/^(\.{0,2})([0-9a-zA-Z_\.]+?)(\.?)$/", $id_elem_new, $string)) break;

                $id_data_new = $string[2];
                $valid       = TRUE;

                //process prefix modifier
                switch ($string[1])
                {
                case ".": //relative to base

                    if ($string[3] === ".")
                    {
                        $id_data_new = $id_base . $id_data_new;
                        $id_path_new = $id_data_new;
                    }
                    else
                    {
                        $id_path_new = $id_path . $id_data_new;
                        $id_data_new = $id_base . $id_data_new;
                    };
                    break;

                case "..": //absolute

                    $id_path_new = ($string[3] === ".") ? $id_data_new : $id_path . $id_data_new;
                    break;

                default: //relative

                    $id_data_new = $id_path . $id_data_new;
                    $id_path_new = $id_data_new;
                    break;
                };

                break 2;
            };

            //relative using automatic index
            $id_elem_new = $id_auto;
            $id_data_new = $id_path . $id_auto;
            $id_path_new = $id_data_new;
            $valid       = FALSE;
            break;
        };

        return $valid;
    }
};