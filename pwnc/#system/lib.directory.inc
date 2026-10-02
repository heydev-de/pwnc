<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   DIRECTORY CLASS
//==================================================================================================

//related functions
//--------------------------------------------------------------------------------------------------

function directory_get_canonical($index = NULL)
{
    if (stre($index)) $index = 0;
    $language = nstre(CMS_LANGUAGE) ? CMS_LANGUAGE . "." : "";
    $map      = new map("#system/" . $language . "directory.canonical");
    return $map->get_value($index);
};

//--------------------------------------------------------------------------------------------------

function directory_flexview_display_function($flexview,
                                             $index,
                                             $open)
{
    if ($flag = streq($index, $flexview->index)) $open = TRUE;

    $title       = x($flexview->get_name($index));
    $name        = $title;
    $hidden      = ! empty($flexview->object[$index]["#data"]["hidden"]);
    $placeholder = ! empty($flexview->object[$index]["#data"]["placeholder"]);
    $used        = ! empty($flexview->object[$index]["#data"]["used"]);

    if (isset($flexview->object[$index]["#data"]["#subtype"]))
    {
        $type = $flexview->object[$index]["#data"]["#subtype"];
        if ($open && isset($flexview->icon_custom["+$type"])) $icon = $flexview->icon_custom["+$type"];
        elseif (isset($flexview->icon_custom[$type]))         $icon = $flexview->icon_custom[$type];
    };

    if ($_flag = ! isset($icon)) $container   = count($flexview->object[$index]) > 2;

    if ($hidden)
    {
        if ($placeholder)
        {
            if ($used)
            {
                if ($_flag) $icon = $container ? ($open ? "open_hpu" : "container_hpu") : "_hpu";
                $name = "<span class=\"green\">($name)</span>";
            }
            else
            {
                if ($_flag) $icon = $container ? ($open ? "open_hp" : "container_hp") : "_hp";
                $name = "<span class=\"red\">($name)</span>";
            };
        }
        else
        {
            if ($_flag) $icon = $container ? ($open ? "open_h" : "container_h") : "_h";
            $name = "<span>($name)</span>";
        };
    }
    elseif ($placeholder)
    {
        if ($used)
        {
            if ($_flag) $icon = $container ? ($open ? "open_pu" : "container_pu") : "_pu";
            $name = "<span class=\"green\">$name</span>";
        }
        else
        {
            if ($_flag) $icon = $container ? ($open ? "open_p" : "container_p") : "_p";
            $name = "<span class=\"red\">$name</span>";
        };
    }
    elseif ($_flag)
    {
        $icon  = $flexview->get_icon($index, $open);
        $_flag = 2;
    };

    $display = $flexview->display;
    $array   = ["index"    => x($index),
                "action"   => x($flexview->get_action($index)),
                "title"    => $title,
                "checkbox" => (strpos($flexview->display, "%checkbox%") !== FALSE) ?
                              $flexview->get_checkbox($index) : "",
                "mark"     => (($temp = $flexview->get_mark($index)) !== NULL) ?
                              image($temp) : "",
                "icon"     => $_flag ? image(($_flag === 2) ? $icon : "directory/icon_entry" . $icon) : image($icon, 16, 16),
                "name"     => $flag ? "<strong>$name</strong>" : $name,
                "class"    => $open ? "active" : ""];

    foreach ($array AS $key => $value)
        $display = nstre($value) ?
                   preg_replace("/(\\[([^\\[\\]]*))?%$key%(([^\\[\\]]*)\\])?/", "\${2}$value\\4", $display) :
                   preg_replace("/(\\[[^\\[\\]]*)?%$key%([^\\[\\]]*\\])?/",     "",               $display);

    echo($display);
};

//--------------------------------------------------------------------------------------------------

function directory_get_select()
{
    $data = new data("#system/directory");

    $array["[" . t("start") . "]"] = NULL;
    $indentation                   = 0;

    $data->move("first");
    while (($key = $data->move("next")) !== NULL)
    {
        $name = l($data->get($key, "name"));
        if (stre($name)) $name = t("unknown");

        switch ($data->get($key, "#type"))
        {
        case "container":
//..................................................................................................

            if ($data->get($key, "url") === NULL) $name = "($name)";

            $name  = str_repeat("\xE2\x80\x83", $indentation) . $name;
            $_name = $name;
            $i     = 1;

            while (isset($array[$_name]))
                $_name = $name . " (" . $i++ . ")";

            $array[$_name] = $key;
            $indentation++;
            break;

        case "/container":
//..................................................................................................

            $indentation--;
        };
    };

    return $array;
};

//--------------------------------------------------------------------------------------------------

function directory_get_type()
{
    $data  = new data("#system/directory.type");
    $array = [];

    $data->move("first");
    while(($key = $data->move("next")) !== NULL)
    {
        if (($format = $data->get($key, "format"))  !== NULL)
            $array[$key]    = CMS_DATA_URL . "directory.type/" . r("$key$format");

        if (($format = $data->get($key, "+format")) !== NULL)
            $array["+$key"] = CMS_DATA_URL . "directory.type/" . r("+$key$format");
    };

    return $array;
};

//--------------------------------------------------------------------------------------------------

function directory_get_type_select()
{
    $data  = new data("#system/directory.type");
    $array = [];

    $data->move("first");
    while (($key = $data->move("next")) !== NULL)
    {
        $name  = $data->get($key, "name") ?? t("unknown");
        $_name = $name;
        $i     = 1;

        while (isset($array[$_name]))
            $_name = "$name (" . $i++ . ")";

        $array[$_name] = $key;
    };

    ksort($array, SORT_NATURAL | SORT_FLAG_CASE);

    $_array = ["" => NULL];
    foreach ($array AS $key => $value)
    {
        if (($format = $data->get($value, "format")) !== NULL)
            $key = strtr($key, ["|" => "\\|"]) .
                   "|" . CMS_DATA_PATH . "directory.type/" . $value . $format;

        $_array[$key] = $value;
    };

    return $_array;
};

//--------------------------------------------------------------------------------------------------

function directory_get_visible($index = NULL)
{
    if (($index === NULL) && defined("CMS_CONTENT_DIRECTORY_INDEX"))
        $index = CMS_CONTENT_DIRECTORY_INDEX;

    $data   = new data("#system/directory");
    if ($data->get($index) === NULL) return 0; //entry does not exist
    $stack  = [0 => FALSE];
    $key    = 0;
    $hidden = FALSE;

    $data->move("first");
    while (($_key = $data->move("next")) !== NULL)
    {
        switch ($data->get($_key, "#type"))
        {
        case "container":
//..................................................................................................

            array_push($stack, $hidden);
            $hidden |= (bool)$data->get($_key, "hidden");
            if (! $hidden)            $key = $_key;
            if (streq($_key, $index)) break 2;
            break;

        case "/container":
//..................................................................................................

            $hidden  = array_pop($stack);
        };
    };

    return $key;
};

//--------------------------------------------------------------------------------------------------

function directory_value($index)
{
    $language = nstre(CMS_LANGUAGE) ? CMS_LANGUAGE . "." : "";
    $map      = new map("#system/" . $language . "directory");
    $value    = $map->get_value($index) ?? "";
    $param    = cms_param();

    if (nstre($param))
    {
        $i = strrpos($value, "#");
        if ($i !== FALSE) $value  = substr_replace($value, $param, $i, 0);
        else              $value .= $param;
    };

    return CMS_ROOT_URL . $value;
};

//--------------------------------------------------------------------------------------------------

function directory_get_flexview($remove_hidden = TRUE)
{
    if (! cms_load("flexview")) return FALSE;

    $flexview = new flexview();
    $data     = new data("#system/directory");
    $array    = [0 => 0];
    $count    = 0;

    $flexview->set_value_function(__NAMESPACE__ . "\\directory_value");

    $data->move("first");
    while (($key = $data->move("next")) !== NULL)
    {
        switch ($data->get($key, "#type"))
        {
        case "container":
//..................................................................................................

            if (
                 (
                   $remove_hidden
                   &&
                   ($data->get($key, "hidden") !== NULL)
                 )
                 ||
                 (
                   ($data->get($key, "placeholder") !== NULL)
                   &&
                   ($data->get($key, "used") === FALSE)
                 )
                 ||
                 stre($name = l($data->get($key, "name")))
               )
            {
                $data->del($key);
            }
            else
            {
                $flexview->set($key, $data->get($key), $array[$count]);
                $array[++$count] = $key;
            };

            break;

        case "/container":
//..................................................................................................

            unset($array[$count]);
            $count--;
        };
    };

    return $flexview;
};

//--------------------------------------------------------------------------------------------------

function directory_create_filesystem($language = CMS_LANGUAGE_ENABLED)
{
    clearstatcache(TRUE);

    //revert logged entries
    if (! directory_remove_filesystem()) return FALSE;

    //initialize
    $data           = new data("#system/directory");
    $key            = $data->move("first");
    $path           = [];
    $map_path       = [];
    $canonical      = [];
    $_canonical     = $data->get($key, "canonical");
    $list_url       = ["index.php" => 0];
    $list_index     = [0 => 0];
    $list_host      = [];
    $list_canonical = [];
    $list_content   = [];
    $list_depth     = [];
    $list_length    = [];
    $reference      = [];
    $language       = stre($language) ? [0] : explode(",", $language);
    $directory      = [];
    $length         = strlen(CMS_ROOT_PATH);
    $content        = substr(CMS_MODULES_PATH, $length) . "content.php";
    $error          = FALSE;

    //initialize by language
    foreach ($language AS $value)
    {
        //path
        $path[$value]     = ($value === 0) ? [] : [stringtofilename($value)];
        $map_path[$value] = new map();
        $map_path[$value]->set("0", "index.php");

        //canonical
        if (stre($__canonical = language_get($_canonical, $value)))
            $__canonical = CMS_ROOT_URL;
        elseif (nstre($host = parse_url($__canonical, PHP_URL_HOST)))
            $list_host[strtolower($host)] = TRUE; //add host

        $canonical[$value]      = $__canonical;
        $list_canonical[$value] = [0 => $__canonical];
        $list_depth[$value]     = [0 => substr_count($__canonical, "/")];
        $list_length[$value]    = [0 => strlen($__canonical)];
    };

    //get auxiliary files
    $auxiliary = [];
    $path_aux  = CMS_DATA_PATH . "#directory/auxiliary/";

    if (
         is_dir($path_aux)
         &&
         (($hdir = opendir($path_aux)) !== FALSE)
       )
    {
        while (($file_object = readdir($hdir)) !== FALSE)
        {
            if (! is_file($path_aux . $file_object)) continue;
            $auxiliary[] = $file_object;
        };

        closedir($hdir);
    };

//..................................................................................................

    //create index file
    if (! write_file(CMS_ROOT_PATH . "index.php",
                     "<?php " .
                     "/* generated by " . CMS_SOFTWARE . " - do not edit */ " .
                     "namespace cms; " .
                     "require(\"$content\");"))
        return FALSE;

    //open log
    if (($hfile = fopen(CMS_DATA_PATH . "#system/directory.log", "wb")) === FALSE) return FALSE;

    //lock also prevents concurrent execution
    flock($hfile, LOCK_EX);
    stream_set_write_buffer($hfile, 0);

    while (($key = $data->move("next")) !== NULL)
    {
        $type = $data->get($key, "#type");

        switch ($type)
        {
        case "container":

            //skip unused placeholder entries
            if (
                 ($data->get($key, "placeholder") !== NULL)
                 &&
                 ($data->get($key, "used") === NULL)
               )
            {
                $depth = 1;

                while (($depth > 0) && (($key = $data->move("next")) !== NULL))
                {
                    $type = $data->get($key, "#type");

                    switch ($type)
                    {
                    case "container":  $depth++; break;
                    case "/container": $depth--; break;
                    };
                };

                break;
            };

            //proceed with entry
            $name       = $data->get($key, "name");
            $url        = $data->get($key, "url") ?? "";
            $_path      = $data->get($key, "path");
            $_canonical = $data->get($key, "canonical");
            $_content   = "";
            $fragment   = "";

            if ($_url = analyze_url($url))
            {
                switch ($_url["scheme"])
                {
                case "directory":

                    $reference[$key] = [$_url["host"], $_url["fragment"]];
                    break;

                case "content":

                    $_content = $_url["host"];
                    $fragment = $_url["fragment"];
                    break;
                };
            };

            foreach ($path AS $_key => $value)
            {
                if ($_key === 0) $_key = FALSE;

                //retrieve entry name
                $_name = language_get($name, $_key);
                $_name = stringtofilename($_name);

                //add entry name to path
                $path[$_key][] = $_name;
                $depth         = count($path[$_key]);

                //override path
                if (nstre($__path = language_get($_path, $_key)))
                {
                    $__path = explode("/", $__path);
                    $depth  = count($__path);
                    $value  = [];
                    $_name  = array_pop($__path);
                    $_name  = stringtofilename($_name);
                    $_name  = strtolower($_name);

                    foreach ($__path AS $__key => $_value)
                    {
                        $_value  = stringtofilename($_value);
                        $_value  = strtolower($_value);
                        $value[] = $_value;
                    };
                };

                //override canonical
                if (nstre($__canonical = language_get($_canonical, $_key)))
                {
                    //add host
                    if (nstre($host = parse_url($__canonical, PHP_URL_HOST)))
                        $list_host[strtolower($host)] = TRUE;

                    $canonical[$_key] = $__canonical;
                }
                else
                {
                    $__canonical = $canonical[$_key];
                };

                //create path to file
                if ($depth > 1)
                {
                    $buffer = implode($value) . "/";

                    if (! isset($directory[$buffer]))
                    {
                        $buffer = "";

                        foreach ($value AS $_value)
                        {
                            $buffer .= "$_value/";
                            if (isset($directory[$buffer])) continue;

                            $_buffer = unique_filename(CMS_ROOT_PATH . $buffer);
                            $buffer  = substr($_buffer, $length);

                            //create directory
                            if (! mkdir($_buffer)) { $error = TRUE; break 4; };

                            //write log entry
                            fwrite($hfile, "$buffer\n");

                            //memorize directory
                            $directory[$buffer] = TRUE;

                            //copy auxiliary files
                            foreach ($auxiliary AS $_value)
                            {
                                if (copy($path_aux . $_value, $_buffer . $_value))

                                    //write log entry
                                    fwrite($hfile, $buffer . $_value . "\n");
                            };
                        };
                    };
                }
                else
                {
                    $buffer = "";
                };

                //generate unique name
                $_buffer = unique_filename(CMS_ROOT_PATH . $buffer . "$_name.php");
                $buffer  = substr($_buffer, $length);

                //create file
                if (! write_file($_buffer,
                                 "<?php " .
                                 "/* generated by " . CMS_SOFTWARE . " - do not edit */ " .
                                 "namespace cms; " .
                                 "\$_GET[\"content_directory_index\"] = \"$key\"; " .
                                 "\$_GET[\"content_display\"] = \"directory\"; " .
                                 "\$_GET[\"content_index\"] = NULL; " .
                                 "\$_GET[\"cms_select_language\"] = \"$_key\"; " .
                                 "require(\"" . str_repeat("../", $depth - 1) . $content . "\");"))
                {
                    $error = TRUE;
                    break 3;
                };

                //map entry path
                $__buffer = cms_path_urlencode($buffer);
                $map_path[$_key]->set($key, $__buffer . (($fragment !== NULL) ? "#$fragment" : ""));

                //write log entry
                fwrite($hfile, "$buffer\n");

                //canonical
                $__canonical = absolute_path($__canonical, $__buffer);
                $depth       = substr_count($__canonical, "/");
                $_length     = strlen($__canonical);

                if (isset($list_url[$url])) //repeating url
                {
                    $__key            = $list_url[$url];
                    $list_index[$key] = $__key;

                    if (isset($list_canonical[$_key][$__key]))
                    {
                        if ($list_depth[$_key][$__key]  <  $depth)   continue;
                        if ($list_length[$_key][$__key] <= $_length) continue;
                    };
                }
                else //new url occurrence
                {
                    $__key            = count($list_url);
                    $list_url[$url]   = $__key;
                    $list_index[$key] = $__key;
                };

                $list_canonical[$_key][$__key] = $__canonical;
                $list_content[$_content]       = $__key;
                $list_depth[$_key][$__key]     = $depth;
                $list_length[$_key][$__key]    = $_length;
            };

            break;

        case "/container":

            foreach ($path AS $_key => $_)
                array_pop($path[$_key]);
        };
    };

//..................................................................................................

    //directory dereferencing
    foreach ($reference AS $key => $value)
    {
        $list = [$key => TRUE];

        while (isset($reference[$value[0]]))
        {
            //check for loop
            if (isset($list[$value[0]])) continue 2;
            $list[$value[0]] = TRUE;

            $value = $reference[$value[0]];
        };

        //map entry
        foreach ($language AS $_value)
        {
            //path
            $__value = $map_path[$_value]->get_value($value[0]);
            if ($__value === NULL) continue;

            //fragment
            if ($value[1] !== NULL)
            {
                $i = strrpos($__value, "#");
                if ($i !== FALSE) $__value = substr($__value, 0, $i);
                $__value .= "#" . $value[1];
            };

            $map_path[$_value]->set($key, $__value);
        };

        //canonical
        if (isset($list_index[$value[0]]))
            $list_index[$key] = $list_index[$value[0]];
    };

//..................................................................................................

    //save maps
    foreach ($language AS $value)
    {
        //path
        $_value = ($value !== 0) ? "$value." : "";
        $map_path[$value]->save("#system/" . $_value . "directory");

        //canonical
        $map_canonical = new map();
        foreach ($list_index AS $key => $__value)
            $map_canonical->set($key, $list_canonical[$value][$__value]);
        $map_canonical->save("#system/" . $_value . "directory.canonical");

        //content
        $map_content = new map();
        foreach ($list_content AS $key => $__value)
            if (nstre($key)) $map_content->set($key, $list_canonical[$value][$__value]);
        $map_content->save("#system/" . $_value . "directory.content");
    };

    //save host list
    $path = CMS_DATA_PATH . "#system/host.txt";
    if ($list_host)
    {
        $list_host[CMS_DOMAIN] = TRUE; //add current host
        write_file($path, implode("\n", array_keys($list_host)));
    }
    elseif (is_file($path))
    {
        unlink($path);
    };

    //close log
    fclose($hfile);

    return ! $error;
};

//--------------------------------------------------------------------------------------------------

function directory_remove_filesystem()
{
    $path_old = CMS_ROOT_PATH . "directory.log";
    $path_new = CMS_DATA_PATH . "#system/directory.log";

    //migrate legacy log
    if (is_file($path_old)) rename($path_old, $path_new);

    //lock file to prevent concurrent execution
    if (($hfile = fopen($path_new, "c+")) === FALSE) return FALSE;
    flock($hfile, LOCK_EX);

    $array = [];
    while (($buffer = fgets($hfile)) !== FALSE)
    {
        if (stre($buffer = rtrim($buffer, "\r\n"))) continue; //important
        $array[] = $buffer;
    };

    $path  = end($array);
    $error = FALSE;

    while ($path !== FALSE)
    {
        $path = CMS_ROOT_PATH . $path;

        if (file_exists($path))
        {
            switch (filetype($path))
            {
            case "file":

                $error |= ! unlink($path);
                break;

            case "dir":

                $error |= ! rmdir($path);
                break;
            };
        };

        $path = prev($array);
    };

    if (! $error)
    {
        rewind($hfile);
        ftruncate($hfile, 0);
    };

    fclose($hfile);
    return ! $error;
};

class directory
{

//properties
//--------------------------------------------------------------------------------------------------

    public $data = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct()
    {
        $this->data = new data("#system/directory");
    }

//methods
//--------------------------------------------------------------------------------------------------

    function append($key,
                    $name,
                    $description  = "",
                    $url          = "",
                    $subtype      = "",
                    $hidden       = FALSE,
                    $placeholder  = FALSE,
                    $dynamic      = TRUE,
                    $image_button = "",
                    $image_hover  = "",
                    $image_active = "",
                    $path         = "",
                    $canonical    = "")
    {
        $this->data->set_buffer(
            [["name"         => $name,
              "#type"        => "container",
              "#subtype"     => $subtype,
              "description"  => $description,
              "url"          => $url,
              "hidden"       => (bool)$hidden,
              "placeholder"  => (bool)$placeholder,
              "dynamic"      => (bool)$dynamic,
              "image_button" => $image_button,
              "image_hover"  => $image_hover,
              "image_active" => $image_active,
              "path"         => $path,
              "canonical"    => $canonical],
             ["#type"        => "/container"]]);

        return $this->data->append($key);
    }

//--------------------------------------------------------------------------------------------------

    function insert($key,
                    $name,
                    $description  = "",
                    $url          = "",
                    $subtype      = "",
                    $hidden       = FALSE,
                    $placeholder  = FALSE,
                    $dynamic      = TRUE,
                    $image_button = "",
                    $image_hover  = "",
                    $image_active = "",
                    $path         = "",
                    $canonical    = "")
    {
        $this->data->set_buffer(
            [["name"         => $name,
              "#type"        => "container",
              "#subtype"     => $subtype,
              "description"  => $description,
              "url"          => $url,
              "hidden"       => (bool)$hidden,
              "placeholder"  => (bool)$placeholder,
              "dynamic"      => (bool)$dynamic,
              "image_button" => $image_button,
              "image_hover"  => $image_hover,
              "image_active" => $image_active,
              "path"         => $path,
              "canonical"    => $canonical],
             ["#type"        => "/container"]]);

        return $this->data->insert($key);
    }

//--------------------------------------------------------------------------------------------------

    function set($key,
                 $name         = NULL,
                 $description  = NULL,
                 $url          = NULL,
                 $subtype      = NULL,
                 $hidden       = NULL,
                 $placeholder  = NULL,
                 $dynamic      = NULL,
                 $image_button = NULL,
                 $image_hover  = NULL,
                 $image_active = NULL,
                 $path         = NULL,
                 $canonical    = NULL)
    {
        if ($this->data->get($key) === NULL) return FALSE;

        if ($name         !== NULL) $this->data->set($name,              $key, "name");
        if ($description  !== NULL) $this->data->set($description,       $key, "description");
        if ($url          !== NULL) $this->data->set($url,               $key, "url");
        if ($subtype      !== NULL) $this->data->set($subtype,           $key, "#subtype");
        if ($hidden       !== NULL) $this->data->set((bool)$hidden,      $key, "hidden");
        if ($placeholder  !== NULL) $this->data->set((bool)$placeholder, $key, "placeholder");
        if ($dynamic      !== NULL) $this->data->set((bool)$dynamic,     $key, "dynamic");
        if ($image_button !== NULL) $this->data->set($image_button,      $key, "image_button");
        if ($image_hover  !== NULL) $this->data->set($image_hover,       $key, "image_hover");
        if ($image_active !== NULL) $this->data->set($image_active,      $key, "image_active");
        if ($path         !== NULL) $this->data->set($path,              $key, "path");
        if ($canonical    !== NULL) $this->data->set($canonical,         $key, "canonical");

        return $key;
    }

//--------------------------------------------------------------------------------------------------

    function del($key)
    {
        return $this->data->del($key);
    }

//--------------------------------------------------------------------------------------------------

    function parse_placeholder()
    {
        $array = [];
        $depth = 0;
        $limit = 0;

        $key   = $this->data->move("first");
        while ($key !== NULL)
        {
            switch ($this->data->get($key, "#type"))
            {
            case "container":

                $array[++$depth] = $key;
                if ($this->data->get($key, "url") !== NULL)             $limit = $depth;
                break;

            case "/container":

                $key             = $array[$depth];
                if ($depth > $limit)                                    $this->data->del($key, "used");
                elseif ($this->data->get($key, "placeholder") !== NULL) $this->data->set(TRUE, $key, "used");
                else                                                    $this->data->del($key, "used");
                if (--$depth < $limit)                                  $limit = $depth;
            };

            $key = $this->data->move("next");
        };
    }

//--------------------------------------------------------------------------------------------------

    function save()
    {
        $this->parse_placeholder();

        return $this->data->save() && directory_create_filesystem();
    }
};