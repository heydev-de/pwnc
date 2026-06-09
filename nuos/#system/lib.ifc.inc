<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   IFC CLASS
//==================================================================================================

global $ifc_message,       //control message
       $ifc_page,          //selected page
       $ifc_option,        //'external' => externally opened windows
       $ifc_select,        //ifc fieldname of return value
       $ifc_select_action, //url of return call, %return% will be replaced by value
       $ifc_response,      //response message: CMS_MSG_DONE, CMS_MSG_ERROR, {text}
       $ifc_param;         //default parameter

//constants
define("CMS_IFC_MESSAGE",           $ifc_message);
define("CMS_IFC_PAGE",              CMS_INSTANCE);
define("CMS_IFC_OPTION",            $ifc_option);
define("CMS_IFC_SELECT",            $ifc_select);
define("CMS_IFC_SELECT_ACTION",     $ifc_select_action);
define("CMS_IFC_INPUT_PLACEHOLDER", CMS_L_IFC_012 . " …");

//related functions
//--------------------------------------------------------------------------------------------------

function ifc_permission($array = NULL)
{
    static $permission = NULL;

    if ($array === NULL) return $permission;

    $permission = (is_array($array) && (count($array) > 0)) ? $array : NULL;
};

//--------------------------------------------------------------------------------------------------

function ifc_available($module)
{
    return is_file(CMS_MODULES_PATH . "#interface/ifc.$module.inc");
};

//--------------------------------------------------------------------------------------------------

function ifc_default($ifc_page = NULL)
{
    ifc_permission(["" => CMS_L_ACCESS]);

    $ifc = new ifc(NULL, $ifc_page, NULL);

    echo("<div>" .
         image("avatar", 110, NULL, NULL, "MARGIN-BOTTOM:10px") . "<br>" .
         "<strong>" .
         x(CMS_SOFTWARE) . " " .
         x(CMS_VERSION) .
         "</strong><br>" .
         x(CMS_COPYRIGHT) . ", " .
         "<a href=\"" . x(CMS_HOMEPAGE) . "\" " .
            "title=\"" . x(CMS) . "\" " .
            "onclick=\"window.open(this.href);return false;\">" .
         x(CMS_HOMEPAGE) .
         "</a><br>" .
         "PHP " . x(phpversion()) . " (" . x(php_sapi_name()) . "), " . x(php_uname()) . "<br><br>");

    //update check
    if (
         cms_permission("interface.setup")
         &&
         (! (cms_daemon_exists("update") || cms_daemon_exists("backup")))
         &&
         cms_load("update")
       )
    {
        cms_error_silent();
        $version = (new update())->available();
        cms_error_silent(FALSE);

        //display update button
        if (nstre($version))
            echo("<a id=\"ifc-update\" " .
                    "href=\"" . x(cms_url(["ifc_page" => "setup"])) . "\">" .
                 sprintf(CMS_L_IFC_011, x($version)) .
                 "</a><br>");
    };

    echo(CMS_L_IFC_013 .
         "<div id=\"ifc-license\">" .
         x(file_get_contents(CMS_PATH . "license.txt")) .
         "</div>" .
         "</div>");

    $ifc->close();
};

//--------------------------------------------------------------------------------------------------

function ifc_inactive($ifc_page = NULL)
{
    $ifc = new ifc(CMS_L_IFC_001, $ifc_page, NULL);
    $ifc->close();
};

//--------------------------------------------------------------------------------------------------

function ifc_table_open($class = NULL)
{
    echo("<table" . (nstre($class) ? " class=\"$class\"" : "") . ">");
};

//--------------------------------------------------------------------------------------------------

function ifc_table_close()
{
    echo("</table>");
};

//--------------------------------------------------------------------------------------------------

function ifc_tab_open($label,
                      $command = NULL)
{
    static $level = [];
    static $count = 0;

    $i = count($level);

    switch($command)
    {
    default:
//..................................................................................................

        $level[++$i] = [0 => ++$count, 1 => 0];
        $flag        = TRUE;

        echo("<div class=\"tab\">");

    case "next":
//..................................................................................................

        $level[$i][1]++;

        $name  = "ifc_tab_" . $level[$i][0];
        $value = $level[$i][1];
        global $$name;

        $flag = isset($$name) ?
                streq($$name, $value) :
                isset($flag);

        echo("<label>" .
             "<input name=\"$name\" " .
                    "type=\"radio\"" .
                    "value=\"$value\" " .
                    ($flag ? " checked" : "") . ">" .
             ifc_parse_label($label)[2] .
             "</label>" .
             "<div>");
        break;

    case "close":
//..................................................................................................

        array_pop($level);
    };
};

//--------------------------------------------------------------------------------------------------

function ifc_tab_next($label)
{
    echo("</div>");
    ifc_tab_open($label, "next");
}

//--------------------------------------------------------------------------------------------------

function ifc_tab_close()
{
    echo("</div>" .
         "</div>");
    ifc_tab_open("", "close");
};

//--------------------------------------------------------------------------------------------------

function ifc_popover_open($label)
{
    static $count = 0;

    $id = "ifc-popover-" . ++$count;

    echo("<button type=\"button\" " .
                 "popovertarget=\"$id\" " .
                 "class=\"popover-toggle\">" .
         ifc_parse_label($label)[2] .
         "</button>" .

         "<div id=\"$id\" " .
              "popover " .
              "class=\"popover\">" .
         "<div class=\"popover-backdrop\">" .
         "</div>" .
         "<div class=\"popover-content\">" .
         "<button type=\"button\" " .
                 "popovertarget=\"$id\" " .
                 "popovertargetaction=\"hide\" " .
                 "class=\"popover-close\">" .
         CMS_L_COMMAND_CLOSE .
         "</button>");
};

//--------------------------------------------------------------------------------------------------

function ifc_popover_close()
{
    echo("</div>" .
         "</div>");
};

//--------------------------------------------------------------------------------------------------

function ifc_close_external()
{
    echo(CMS_DOCTYPE_HTML .
         "<html lang=\"" . x(CMS_LANGUAGE) . "\">" .
         "<head>" .
         "<title>" .
         x(CMS_DOMAIN) . ": " . CMS_L_IFC_003 .
         "</title>");

?><script>

this.close();

</script><?php

    echo("</head>" .
         "<body>" .
         "</body>" .
         "</html>");

    exit();
};

//--------------------------------------------------------------------------------------------------

function ifc_varied($option = NULL,
                    $index  = 0)
{
    return class_varied($option, $index);
};

//--------------------------------------------------------------------------------------------------

function ifc_parse_label($text)
{
    $url = NULL;
    $img = NULL;

    //icon
    if (
         (($pos = strrpos($text, "|")) !== FALSE)
         &&
         (($pos === 0) || ($text[$pos - 1] !== "\\"))
       )
    {
        $path   = substr($text, $pos + 1);
        $text   = substr($text, 0, $pos);
        $length = strlen(CMS_ROOT_PATH);
        $flag   = FALSE;

        //fully specified local path
        if (strncmp($path, CMS_ROOT_PATH, $length) === 0)
        {
            if (is_file($path))
                $url = image_process(CMS_ROOT_URL . substr($path, $length), 16, 16);
        }

        //short specifier
        else
        {
            foreach (["svg", "png"] AS $extension)
            {
                $_path  = "$path.$extension";
                $__path = CMS_IMAGES_PATH . $_path;

                if (is_file($__path))
                {
                    $url = CMS_IMAGES_URL . $_path;
                    break;
                };
            };
        };

        //image code
        if (isset($url))
            $img = "<img src=\"" . x($url) . "\" " .
                        "width=\"16\" " .
                        "height=\"16\" " .
                        "alt=\"\">";
    };

    $text = strtr($text, ["\\|" => "|"]);

    return [$text, $url, trim("$img $text"), $img];
};

class ifc
{

//properties
//--------------------------------------------------------------------------------------------------

    public $has_page_menu = FALSE;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($ifc_response         = NULL,
                         $ifc_page             = NULL,
                         $menu                 = TRUE,
                         $param                = NULL,
                         $message              = NULL,
                         $subpage              = NULL,
                         $content_container_id = NULL)
    {
        //title
        $title = isset($ifc_page[CMS_IFC_PAGE]) ? $ifc_page[CMS_IFC_PAGE] : "";
        if (stre($title))
        {
            $constant = "CMS_L_IFC_" . strtoupper(CMS_IFC_PAGE);
            if (defined($constant)) $title = constant($constant);
            if (stre($title))
            {
                $title = CMS_IFC_PAGE;
                if (stre($title)) $title = ucfirst(CMS_APPLICATION);
            };
        };
        if (nstre($subpage)) $title .= " > " . $subpage;

        //saved position
        global $ifc_prev,
               $ifc_left,
               $ifc_top;

        //same page/subpage -> restore position
        if (streq($_title = hash32($title), $ifc_prev))
        {
            $ifc_left = (float)$ifc_left;
            $ifc_top  = (float)$ifc_top;
        }

        //different page/subpage -> reset position
        else
        {
            $ifc_left = $ifc_top = 0;
        };

        //default menu
        if (! is_array($menu))
        {
            if ($menu)
                $menu = [CMS_L_COMMAND_CONFIRM . "|" . CMS_IFC_EDITION . "/command_confirm" => "javascript:ifc_submit();",
                         CMS_L_COMMAND_CANCEL .  "|" . CMS_IFC_EDITION . "/command_cancel"  => NULL];
            elseif ($menu === FALSE)
                $menu = [CMS_L_COMMAND_CLOSE .   "|" . CMS_IFC_EDITION . "/command_cancel"  => NULL];
        };

        //build page
        echo(CMS_DOCTYPE_HTML .
             "<html lang=\"" . x(CMS_LANGUAGE) . "\" " .
                   "style=\"VISIBILITY:hidden\">" .
             "<head>" .
             "<meta charset=\"utf-8\">" .
             "<title>" .
             x(CMS_DOMAIN) . ": " . x($title) .
             "</title>" .
             "<meta name=\"robots\" " .
                   "content=\"noindex, nofollow\">" .
             "<meta name=\"viewport\" " .
                   "content=\"width=device-width, " .
                             "initial-scale=1.0, " .
                             "minimum-scale=0.5, " .
                             "maximum-scale=5.0, " .
                             "user-scalable=yes, " .
                             "interactive-widget=resizes-visual\">" .
             "<meta name=\"theme-color\" " .
                   "content=\"#293131\">" .
             "<meta name=\"format-detection\" " .
                   "content=\"address=no, email=no, telephone=no\">" .
             "<base href=\"" . x(CMS_ROOT_URL) . "\">" .
             CMS_JAVASCRIPT .
             "<link rel=\"manifest\" " .
                   "href=\"/pwnc/manifest.json\">" .
             "<link rel=\"stylesheet\" " .
                   "href=\"" . x(CMS_URL) . "stylesheet/" . CMS_IFC_EDITION . ".css\">" .
             "<link rel=\"shortcut icon\" " .
                   "type=\"image/svg+xml\" " .
                   "href=\"" . x(CMS_URL) . "favicon.svg\">" .
             "<link rel=\"icon\" " .
                   "href=\"/pwnc/image/pwnc-512.png\" " .
                   "sizes=\"512x512\">" .
             "<link rel=\"icon\" " .
                   "href=\"/pwnc/image/pwnc-192.png\" " .
                   "sizes=\"192x192\">" .
             "<script src=\"" . x(CMS_JAVASCRIPT_URL) . "ifc.js\">" .
             "</script>" .
             jscript("fx_register_callback(ifc_loading_event);") .
             "</head>" .

             "<body class=\"" . x(CMS_APPLICATION) . (nstre(CMS_INSTANCE) ? "-" . x(CMS_INSTANCE) : "") . "\">");

?><script>

fx_event_listen("document_load", () =>
{
    //scroll to active interface
    while (true)
    {
        let container = document.getElementById("ifc-control");
        if (! container) break;

        let element = container.querySelector("A.active");
        if (! element) break;

        let container_rect = container.getBoundingClientRect();
        let element_rect   = element.getBoundingClientRect();
        let threshold      = element_rect.bottom + 3 * element_rect.height;
        if (container_rect.bottom >= threshold) break;

        container.scrollBy({
            top: threshold - container_rect.bottom,
            behavior: "auto"
        });

        break;
    };

    ifc_show_upload_progress(document.forms["ifc"]);
    fx_style(document.documentElement, "visibility", "visible");
});

fx_event_listen("window_load", () =>
{
    //restore scroll position
    fx_scroll_container.scrollTo(<?php echo(qx($ifc_left));?>, <?php echo(qx($ifc_top));?>);

    if (! window.first_load) fx_adjust_window();

    //colorize option elements
    document.querySelectorAll("OPTION, DIV.select LABEL").forEach((elem) =>
    {
        elem.style.color = strtocolor(elem.value || elem.querySelector("INPUT")?.value || elem.textContent, 75, 40);
    });
});

if ("serviceWorker" in navigator)
    fx_event_listen("window_load", () =>
    {
        navigator.serviceWorker.register("/pwnc/service-worker.js");
    });

</script><?php

        echo("<form name=\"ifc\" " .
                   "action=\"" . x(cms_url($_GET)) . "\" " .
                   "method=\"post\" " .
                   "enctype=\"multipart/form-data\" " .
                   "accept-charset=\"utf-8\">" .

        //page parameter
             "<input name=\"ifc_page\" " .
                    "type=\"hidden\" " .
                    "value=\"" . x(CMS_IFC_PAGE) . "\">" .

        //message parameter
             "<input name=\"ifc_message\" " .
                    "type=\"hidden\" " .
                    "value=\"" . x($message) . "\">" .

        //current page/subpage hash
             "<input name=\"ifc_prev\" " .
                    "type=\"hidden\" " .
                    "value=\"" . x($_title) . "\">" .

        //left scroll position
             "<input name=\"ifc_left\" " .
                    "type=\"hidden\" " .
                    "value=\"0\">" .

        //top scroll position
             "<input name=\"ifc_top\" " .
                    "type=\"hidden\" " .
                    "value=\"0\">");

        //constant parameters
        $content_offset = 5;

        if (is_array($param))
        {
            if (! isset($param["ifc_param"])) $this->param("ifc_param");
            $content_offset += $this->param($param);
        }
        elseif (nstre($param))
        {
            $content_offset += $this->param("ifc_param", $param);
        }
        else
        {
            $this->param("ifc_param");
        };

        //response
        echo("<div id=\"ifc-response\">" .
             $ifc_response .
             "</div>" .
             "<button id=\"ifc-response-close\" " .
                     "type=\"button\" " .
                     "onclick=\"ifc_object('ifc-response').className='hidden';\">" .
             CMS_L_COMMAND_CLOSE .
             "</button>");

?><script>

fx_event_listen(document, "mousedown", (e) =>
{
    var object = ifc_object("ifc-response");
    if (! object.contains(e.target))
        object.className = "hidden";
});

</script><?php

        if ($this->has_page_menu = is_array($ifc_page))
        {
            //control
            echo("<div id=\"ifc-control\">" .

            //logo
                 "<a id=\"ifc-logo\" " .
                    "href=\"" . x(CMS_HOMEPAGE) . "\" " .
                    "title=\"" . x(CMS) . "\" " .
                    "onclick=\"window.open(this.href);return false;\">" .
                 image(CMS_IFC_EDITION . "/logo") .
                 image(CMS_IFC_EDITION . "/icon") .
                 "</a>" .

            //control head
                 "<div id=\"ifc-control-head\">" .

            //desktop
                 (cms_permission("desktop", NULL, NULL) ?
                 "<a href=\"" . x(cms_url(CMS_MODULES_URL . "desktop.php")) . "\">" .
                 image(CMS_IFC_EDITION . "/icon_desktop") . " " . CMS_L_IFC_006 .
                 "</a>" : "") .

            //website
                 "<a href=\"" . x(cms_url(CMS_ROOT_URL)) . "\">" .
                 image(CMS_IFC_EDITION . "/icon_website") . " " . CMS_L_IFC_008 .
                 "</a>" .
                 "</div>" .

            //control body
                 "<div id=\"ifc-control-body\">");

            $flag = FALSE;

            foreach ($ifc_page AS $key => $value)
            {
                //skip hidden interface pages (starting with '#')
                if (preg_match("/^#/", $value)) continue;

                //separator
                if ($value === "-")
                {
                    $flag = TRUE;
                    continue;
                };

                if ($flag)
                {
                    echo("<hr>");
                    $flag = FALSE;
                };

                $image = image_exists("interface/icon_$key") ?
                         "icon_$key" : "icon_unknown";
                $value = x($value);

                //select button
                echo("<a href=\"" . x(cms_url(["ifc_message" => "ifc_select_page",
                                               "ifc_param"   => $key])) . "\"" .
                         (streq($key, CMS_IFC_PAGE) ? " class=\"active\"" : "") . ">" .
                     image("interface/$image") . " $value" .
                     "</a>");
            };

            echo("</div>" .

            //control foot
                 "<div id=\"ifc-control-foot\">" .

            //logout
                 "<a href=\"" . x(CMS_MODULES_URL . "identification.php" .
                                  cms_param(["cms_logout" => 1], NULL, TRUE)) . "\">" .
                 image(CMS_IFC_EDITION . "/icon_logout") . " " . CMS_L_IFC_007 .
                 "</a>" .
                 "</div>");

            //language
            if (CMS_LANGUAGE_ENABLED)
            {
                $map      = new map("#system/language.image");
                $language = explode(",", CMS_LANGUAGE_ENABLED);

                echo("<div id=\"ifc-language\">");

                foreach ($language AS $key => $value)
                {
                    $url = cms_url(["ifc_page"            => CMS_IFC_PAGE,
                                    "cms_select_language" => $value]);

                    echo("<a href=\"" . x($url) . "\" " .
                            "title=\"" . x($value) . "\" " .
                            "class=\"language-" . (streq($value, CMS_LANGUAGE) ? "on" : "off") . "\">" .
                         image(((($image = $map->get_value($value)) !== NULL) &&
                               is_file(CMS_DATA_PATH . "language/$image")) ?
                               CMS_DATA_URL . "language/$image" :
                               "language/default",
                               16, 12) .
                         "</a>");
                };

                echo("</div>");
            };

            //expand button
            echo("<a id=\"ifc-control-open\" " .
                    "href=\"javascript:;\">" .
                 "…" .
                 "</a>" .
                 "</div>");

            if ($content_container_id === NULL) $content_container_id = "ifc-content";
        }
        else
        {
            if ($content_container_id === NULL) $content_container_id = "ifc-content-full";
        };

        echo("<div id=\"" . x($content_container_id) . ($menu ? "" : "-no-menu") . "\">");

        //permission
        if (
             cms_permission("interface.permission", NULL, NULL)
             &&
             is_array($array = ifc_permission())
           )
        {
            $application = cms_application();

            echo("<div id=\"ifc-overlay\">" .
                 "<input id=\"ifc-permission-switch\" " .
                        "type=\"checkbox\" " .
                        "value=\"1\">" .
                 "<label id=\"ifc-permission-close\" " .
                        "for=\"ifc-permission-switch\">" .
                 "</label>" .
                 "<div id=\"ifc-permission\">" .
                 "<label for=\"ifc-permission-switch\" " .
                        "title=\"" . CMS_L_IFC_009 . "\">" .
                 image("ifc/icon_permission") .
                 "</label>");

            foreach ($array AS $key => $value)
            {
                if ($key) $key = ".$key";
                echo("<a href=\"javascript:load_page('" .
                         qx(cms_url(CMS_MODULES_URL . "interface.php",
                                    ["ifc_page"    => "permission",
                                     "ifc_message" => "_set",
                                     "ifc_option"  => "external",
                                     "ifc_param1"  => $application . $key],
                                    TRUE)) . "');\" " .
                        "title=\"" . x($application . $key) . "\">" .
                     x($value) .
                     "</a>");
            };

            echo("</div>" .
                 "</div>");
        };

        //menu selection
        if ($menu)
        {
            echo("<div id=\"ifc-command\">");

            //subpage
            if ($subpage)
            {
                $subpage = x($subpage);
                echo("<div id=\"ifc-subpage\" " .
                          "title=\"$subpage\">" .
                     $subpage .
                     "</div>");
            };

            $array    = [0 => [], []];
            $charlist = "";
            $i        = 0;

            foreach ($menu AS $key => $value)
            {
                //parse menu text
                [$key, , , $img] = ifc_parse_label($key);

                //security check
                if (streq(substr($value, 0, 1), "#"))
                {
                    $array[0][$i] = q($key);
                    $value        = substr($value, 1);
                }
                else
                {
                    $array[0][$i] = NULL;
                };

                //javascript
                if (streq(substr($value, 0, 11), "javascript:"))
                    $array[1][$i] = q(substr($value, 11));

                //message
                elseif (nstre($value))
                    $array[1][$i] = q("ifc_post('" . q($value) . "');");

                //cancel
                else
                    $array[1][$i] = q("ifc_cancel($content_offset);");

                //accesskey
                $_key = $key;

                while (TRUE)
                {
                    $_i   = strspn($_key, $charlist);
                    $char = substr($_key, $_i, 1);
                    $_key = substr($_key, $_i + 1);

                    if (preg_match("/[a-z]/i", $char))
                    {
                        $_i        = strlen($key) - strlen($_key) - 1;
                        $charlist .= strtolower($char) . strtoupper($char);
                        $key       = x(substr($key, 0, $_i)) . "<strong>$char</strong>" . x($_key);
                        break;
                    };

                    if (stre($_key))
                    {
                        $key       = x($key);
                        $char      = "";
                        break;
                    };
                };

                echo("<a href=\"javascript:ifc_select_menu($i);\" " .
                        "accesskey=\"$char\">" .
                     (isset($img) ? "$img " : "") . $key .
                     "</a>");

                $i++;
            };

            echo(jscript("var ifc_menu = [[\"" . implode("\", \"", $array[0]) . "\"], " .
                                         "[\"" . implode("\", \"", $array[1]) . "\"]];") .

            //expand button
                 "<a id=\"ifc-command-open\" " .
                    "href=\"javascript:;\">" .
                 "</a>" .
                 "</div>");
        };

        echo("<noscript>" .
             "<div class=\"error\">" .
             CMS_L_NOSCRIPT .
             "</div>" .
             "</noscript>");
    }
//methods

//--------------------------------------------------------------------------------------------------

    function param($param,
                   $value = NULL)
    {
        $count = 0;

        if (is_array($param))
        {
            foreach ($param AS $key => $value)
                $count += $this->param($key, $value);
        }
        elseif (is_array($value))
        {
            foreach ($value AS $key => $_value)
                $count += $this->param($param . "[" . $key . "]", $_value);
        }
        else
        {
            echo("<input name=\"" . x($param) . "\" " .
                        "type=\"hidden\" " .
                        "value=\"" . x($value) . "\">");
            $count++;
        };

        return $count;
    }

//--------------------------------------------------------------------------------------------------

    function set($text     = NULL,
                 $type     = "new text 40 60 b",

                 /*
                  * * - display text as title attribute (button option)
                  * : - inline
                  * b - linebreak after
                  * c - checked
                  * d - disabled
                  * f - full size
                  * i - image (texteditor option)
                  * l - language
                  * n - nowrap
                  * r - reference (texteditor option)
                  * t - token (texteditor option)
                  * w - full width
                  */

                 $value    = NULL,
                 $checked  = NULL,
                 $name     = NULL,
                 $language = NULL)
    {
        static $index        = 0,
               $file_index   = 0,
               $radio_index  = 0,
               $_radio_index = 0,
               $radio_name   = NULL,
               $prev_type    = NULL,
               $label        = NULL;

        $_break = NULL;
        $affix  = [];

        //settings
        //                1        2           3           4          5
        //                new      type        length      maximum    parameter
        //                radio                            length
        if (preg_match("/^(new)?\s*([a-z_]+)\s*([0-9x]*)\s*([0-9]*)\s*([\*:bcdfilnrtw]*)$/", $type, $array) !== 1) return;

        //radio reset
        $new = (bool)$array[1];

        //type
        $type = $array[2];

        //length
        if ($array[3])
        {
            //extract horizontal/vertical length
            if (preg_match("/^([0-9]+)x([0-9]+)$/", $array[3], $_array))
            {
                $size   = (int)$_array[1];
                $height = (int)$_array[2];
            }
            else
            {
                $size   = (int)$array[3];
                $height = 1;
            };
        }
        else
        {
            $size   = 1;
            $height = 1;
        };

        //maximum length
        $maxlength = (int)$array[4];

        //display text as title attribute
        $title  = strpos($array[5], "*") !== FALSE;

        //break after label text
        $break = (strpos($array[5], ":") !== FALSE) ? "" : "<br>";

        //parameter
        if (strpos($array[5], "b") !== FALSE) $affix[]  = "<br>";
        elseif ($type === "description")      $text    .= ":";
        $checked   = $checked || (strpos($array[5], "c") !== FALSE);
        $disabled  = strpos($array[5], "d") !== FALSE;
        $fullsize  = strpos($array[5], "f") !== FALSE;
        $image     = (strpos($array[5], "i") !== FALSE) &&
                     cms_available("image") &&
                     ifc_available("image") &&
                     cms_permission("interface.image", NULL, NULL);
        $_language = strpos($array[5], "l") !== FALSE;
        $nowrap    = strpos($array[5], "n") !== FALSE;
        $reference = (strpos($array[5], "r") !== FALSE) &&
                     cms_available("content") &&
                     ifc_available("content") &&
                     cms_permission("interface.content", NULL, NULL);
        $token     = (strpos($array[5], "t") !== FALSE) &&
                     cms_available("token") &&
                     ifc_available("token") &&
                     cms_permission("interface.token", NULL, NULL);
        $fullwidth = strpos($array[5], "w") !== FALSE;

        //language
        if ($_language)
        {
            //extract languages
            if ($language === NULL)
                $language = nstre(CMS_LANGUAGE_ENABLED) ?
                            explode(",", "," . CMS_LANGUAGE_ENABLED) : [];
            else
                $language = explode(",", $language);

            //check active language
            $__language = in_array(CMS_LANGUAGE, $language) ?
                          CMS_LANGUAGE : reset($language);
        };

        //label
        if (is_string($text))
        {
            [$text, $url, $_label, $img] = ifc_parse_label($text);
            if ($title) $_label = $img;
        }
        elseif ($label !== NULL)
        {
            [$label, $url, $_label, $img] = ifc_parse_label($label);
            if ($title) $_label = $img;
        }
        else
        {
            [$_label, $url, $img] = [NULL, NULL, NULL];
        };

        $label = NULL;

        //output
        switch ($type)
        {
        case "button":
//..................................................................................................

            $_language = FALSE; //disable language

            if (streq(substr($value, 0, 11), "javascript:"))
                $value = substr($value, 11);
            elseif ($value)
                $value = "ifc_post('" . q($value) . "');";
            else
                $value = "ifc_submit();";

            //element
            echo("<button type=\"button\" " .
                         "onclick=\"" . x($value) . "\"" .
                         ($title     ? " title=\"" . x($text) . "\"" : "") .
                         ($disabled  ? " disabled"                   : "") .
                         ($fullsize  ? " class=\"fullsize\""         :
                         ($fullwidth ? " class=\"fullwidth\""        : "")) .
                         (in_array($text, [CMS_L_COMMAND_CONFIRM, CMS_L_COMMAND_SAVE]) ?
                                       " style=\"FONT-WEIGHT:bold\"" : "") . ">" .
                 $_label .
                 "</button>");
            break;

        case "password":
//..................................................................................................

            $_language = FALSE; //disable language

        case "text":
//..................................................................................................

            //generate element name
            if (stre($name))
            {
                $index++;
                $name = "ifc_param$index";
            };

            //language
            if ($_language)
            {
                $_name  = "l_$name";
                $_value = "";
            }
            else
            {
                $_name  = $name;
                $_value = $value;
            };

            //full size/width
            if ($fullsize || $fullwidth)
            {
                $class   = $fullsize ? "fullsize" : "fullwidth";
                echo("<div class=\"$class\">");
                $affix[] = "</div>";
            };

            //label
            if (nstre($_label))
            {
                echo("<label class=\"element\">" .
                     "<span>" .
                     $_label .
                     "</span>$break");
                $affix[] = "</label>";
            };

            //element
            echo("<input name=\"" . x($_name) . "\" " .
                        "type=\"$type\" " .
                        "value=\"" . x($_value) . "\" " .
                        "placeholder=\"" . CMS_IFC_INPUT_PLACEHOLDER . "\" " .
                        "style=\"--width:" . ($size * 10 + 20) . "px\"" .
                        ($title     ? " title=\"" . x($text) . "\""                              : "") .
                        ($maxlength ? " maxlength=\"$maxlength\""                                : "") .
                        ($disabled  ? " disabled"                                                : "") .
                        ($_language ? " onblur=\"ifc_language_save(this,'" . qx($name) . "');\"" : "") . ">" .
                 (($size <= 10) ? "" :
                 "<button type=\"button\" " .
                         "class=\"ifc-delete\" " .
                         "onclick=\"ifc_del(this.previousSibling);\">" .
                 CMS_L_COMMAND_DELETE .
                 "</button>"));
            break;

        case "date":
//..................................................................................................

            //generate element name
            if (stre($name))
            {
                $index++;
                $name = "ifc_param$index";
            };

            $_language = FALSE; //disable language

            //full size/width
            if ($fullsize || $fullwidth)
            {
                $class   = $fullsize ? "fullsize" : "fullwidth";
                echo("<div class=\"$class\">");
                $affix[] = "</div>";
            };

            //label
            if (nstre($_label))
            {
                echo("<label class=\"element\">" .
                     "<span>" .
                     $_label .
                     "</span>$break");
                $affix[] = "</label>";
            };

            //sanitize value
            if (is_string($value))  $_value = date("Y-m-d", strtotime($value) ?: NULL);
            elseif (is_int($value)) $_value = date("Y-m-d", max($value, 0));
            else                    $_value = date("Y-m-d");

            //element
            echo("<input name=\"" . x($name) . "\" " .
                        "type=\"date\" " .
                        "value=\"" . x($_value) . "\"" .
                        ($title    ? " title=\"" . x($text) . "\"" : "") .
                        ($disabled ? " disabled "                  : "") . ">");
            break;

        case "datetime":
//..................................................................................................

            //generate element name
            if (stre($name))
            {
                $index++;
                $name = "ifc_param$index";
            };

            $_language = FALSE; //disable language

            //full size/width
            if ($fullsize || $fullwidth)
            {
                $class   = $fullsize ? "fullsize" : "fullwidth";
                echo("<div class=\"$class\">");
                $affix[] = "</div>";
            };

            //label
            if (nstre($_label))
            {
                echo("<label class=\"element\">" .
                     "<span>" .
                     $_label .
                     "</span>$break");
                $affix[] = "</label>";
            };

            //sanitize value
            if (is_string($value))  $_value = date("Y-m-d\TH:i", strtotime($value) ?: NULL);
            elseif (is_int($value)) $_value = date("Y-m-d\TH:i", max($value, 0));
            else                    $_value = date("Y-m-d\TH:i");

            //element
            echo("<input name=\"" . x($name) . "\" " .
                        "type=\"datetime-local\" " .
                        "value=\"" . x($_value) . "\"" .
                        ($title    ? " title=\"" . x($text) . "\"" : "") .
                        ($disabled ? " disabled"                   : "") . ">");
            break;

        case "file":
//..................................................................................................

            //generate element name
            if (stre($name))
            {
                $file_index++;
                $name = "ifc_file$file_index";
            };

            $_language = FALSE; //disable language

            //full size/width
            if ($fullsize || $fullwidth)
            {
                $class   = $fullsize ? "fullsize" : "fullwidth";
                echo("<div class=\"$class\">");
                $affix[] = "</div>";
            };

            //label
            if (nstre($_label))
            {
                echo("<label class=\"element\">" .
                     "<span>" .
                     $_label .
                     "</span>$break");
                $affix[] = "</label>";
            };

            //element
            echo("<input name=\"" . x($name) . "\" " .
                        "type=\"file\" " .
                        "style=\"--width:" . ($size * 10 + 20) . "px\"" .
                        ($title    ? " title=\"" . x($text) . "\"" : "") .
                        ($disabled ? " disabled"                   : "") . ">");
            break;

        case "multifile":
//..................................................................................................

            //generate element name
            if (stre($name))
            {
                $file_index++;
                $name = "ifc_file$file_index";
            };

            $_language = FALSE; //disable language

            //full size/width
            if ($fullsize || $fullwidth)
            {
                $class   = $fullsize ? "fullsize" : "fullwidth";
                echo("<div class=\"$class\">");
                $affix[] = "</div>";
            };

            //label
            if (nstre($_label))
            {
                echo("<label class=\"element\">" .
                     "<span>" .
                     $_label .
                     "</span>$break");
                $affix[] = "</label>";
            };

            //element
            echo("<input name=\"" . x($name) . "[]\" " .
                        "type=\"file\" " .
                        "multiple " .
                        "style=\"--width:" . ($size * 10 + 20) . "px\"" .
                        ($title    ? " title=\"" . x($text) . "\"" : "") .
                        ($disabled ? " disabled"                   : "") . ">");
            break;

        case "texteditor":
//..................................................................................................

            //generate element name
            if (stre($name))
            {
                $index++;
                $name = "ifc_param$index";
            };

            $_name = ($_language ? "l" : "x") . "_$name";

            //full size/width
            if ($fullsize || $fullwidth)
            {
                $class   = $fullsize ? "fullsize" : "fullwidth";
                echo("<div class=\"$class\">");
                $affix[] = "</div>";
            };

            //label
            if (nstre($_label))
            {
                echo("<label class=\"element\" " .
                            "onclick=\"ifc_focus('" . x($_name) . "');\">" .
                     "<span>" .
                     $_label .
                     "</span>$break");
                $affix[] = "</label>";
            };

            $extension = "";

            //image selector
            if ($image)
            {
                $buffer     = "load_page('" .
                              q(cms_url(["ifc_page"          => "image",
                                         "ifc_option"        => "external",
                                         "ifc_select"        => "_object",
                                         "ifc_select_action" =>
                                             "javascript:this.opener.textcontrol_set(" .
                                             "'#" . q($_name) . "'," .
                                             "'#image_left'," .
                                             "'%return%');"],
                                        TRUE)) . "');";
                $extension .= "<a href=\"javascript:;\" " .
                                 "title=\"image\" " .
                                 "onclick=\"" . x($buffer) . "\">" .
                              image("textcontrol/button_image_external") .
                              "</a>";
            };

            //link selector
            if ($reference)
            {
                $buffer     = "load_page('" .
                              q(cms_url(["ifc_page"          => "content",
                                         "ifc_message"       => "edit_href",
                                         "ifc_option"        => "external",
                                         "ifc_select"        => "_value",
                                         "ifc_select_action" =>
                                             "javascript:this.opener.textcontrol_set(" .
                                             "'#" . q($_name) . "'," .
                                             "'#link'," .
                                             "'%return%');"],
                                        TRUE)) . "');";
                $extension .= "<a href=\"javascript:;\" " .
                                 "title=\"link\" " .
                                 "onclick=\"" . x($buffer) . "\">" .
                              image("textcontrol/button_link_external") .
                              "</a>";
            };

            //token selector
            if ($token)
            {
                $buffer     = "load_page('" .
                              q(cms_url(["ifc_page"          => "token",
                                         "ifc_option"        => "external",
                                         "ifc_select"        => "token",
                                         "ifc_select_action" =>
                                             "javascript:this.opener.textcontrol_set(" .
                                             "'#" . q($_name) . "'," .
                                             "'#token'," .
                                             "'%return%');"],
                                        TRUE)) . "');";
                $extension .= "<a href=\"javascript:;\" " .
                                 "title=\"token\" " .
                                 "onclick=\"" . x($buffer) . "\">" .
                              image("textcontrol/button_token_external") .
                              "</a>";
            };

            //element
            echo(jscript("textcontrol(" .
                         "\"#" . q($_name) . "\", " .
                         "\"" . q(CMS_IMAGES_URL) . "\"" .
                         (nstre($extension) ? ", \"" . q($extension) . "\"" : "") . ");") .
                 "<span class=\"code-wrapper\"" .
                       "style=\"--width:" . ($size * 10 + 20) . "px;" .
                               "--height:" . ($height * 25 + 10) . "px\">" .
                 "<code id=\"" . x($_name) . "\" " .
                       "title=\"" . ($title ? x($text) : CMS_L_IFC_010) . "\" " .
                       "contenteditable=\"plaintext-only\" " .
                       "class=\"hl-format\" " .
                       "onblur=\"this.parentElement.removeAttribute('data-focus');" .
                                 ($_language ? "ifc_language_save(this,'" . qx($name) . "');" :
                                "ifc_set('" . qx($name) . "',this.value);") . "\" " .
                       "onchange=\"ifc_highlight(this,40);\" " .
                       "onfocus=\"this.parentElement.setAttribute('data-focus','');\" " .
                       "onkeydown=\"return ifc_keydown(event);\" " .
                       "onkeyup=\"ifc_highlight(this,40);\"" .
                       ($nowrap ? " style=\"WHITE-SPACE:pre\"" : "") . ">" .
                 "</code>" .
                 "</span>" .
                 jscript("ifc_contenteditable_init(\"" . q($_name) . "\");"));

            //hidden form submission element
            if (! $_language)
                echo("<input name=\"" . x($name) . "\" " .
                            "type=\"hidden\" " .
                            "value=\"" . x($value) . "\">" .
                     jscript("ifc_copy(\"" . q($name) . "\", \"" . q($_name) . "\");"));
            break;

        case "textarea":
//..................................................................................................

            //generate element name
            if (stre($name))
            {
                $index++;
                $name = "ifc_param$index";
            };

            //language
            if ($_language)
            {
                $_name  = "l_$name";
                $_value = "";
            }
            else
            {
                $_name  = $name;
                $_value = $value;
            };

            //full size/width
            if ($fullsize || $fullwidth)
            {
                $class   = $fullsize ? "fullsize" : "fullwidth";
                echo("<div class=\"$class\">");
                $affix[] = "</div>";
            };

            //label
            if (nstre($_label))
            {
                echo("<label class=\"element\">" .
                     "<span>" .
                     $_label .
                     "</span>$break");
                $affix[] = "</label>";
            };

            //element
            echo("<textarea name=\"" . x($_name) . "\" " .
                           "onkeydown=\"return ifc_keydown(event);\" " .
                           "style=\"--width:" . ($size * 10 + 20) . "px;" .
                                   "--height:" . ($height * 20) . "px" .
                           ($nowrap    ? ";WHITE-SPACE:pre"                                         : "") . "\"" .
                           ($title     ? " title=\"" . ($title ? x($text) : CMS_L_IFC_010) . "\""   : "") .
                           ($maxlength ? " onkeyup=\"ifc_limit(this,$maxlength);\""                 : "") .
                           ($disabled  ? " disabled"                                                : "") .
                           ($_language ? " onblur=\"ifc_language_save(this,'" . qx($name) . "');\"" : "") . ">" .
                 x($_value) .
                 "</textarea>");
            break;

        case "code":
        case "code_html":
        case "code_php":
        case "code_style":
        case "code_script":
//..................................................................................................

            $_array    = ["code"        => [0,  "false", "html"],
                          "code_html"   => [0,  "true",  "html"],
                          "code_php"    => [0,  "6",     "html"],
                          "code_style"  => [10, "true",  "style"],
                          "code_script" => [20, "true",  "script"]];
            $mode      = $_array[$type][0];
            $no_insert = $_array[$type][1];
            $class     = $_array[$type][2];

            //generate element name
            if (stre($name))
            {
                $index++;
                $name = "ifc_param$index";
            };

            $_name = ($_language ? "l" : "x") . "_$name";

            //full size/width
            if ($fullsize || $fullwidth)
            {
                $class   = $fullsize ? "fullsize" : "fullwidth";
                echo("<div class=\"$class\">");
                $affix[] = "</div>";
            };

            //label
            if (nstre($_label))
            {
                echo("<label class=\"element\" " .
                            "onclick=\"ifc_focus('" . x($_name) . "');\">" .
                     "<span>" .
                     $_label .
                     "</span>$break");
                $affix[] = "</label>";
            };

            //element
            echo("<span class=\"code-wrapper\" " .
                       "style=\"--width:" . ($size * 10 + 20) . "px;" .
                               "--height:" . ($height * 20) . "px\">" .
                 "<code id=\"" . x($_name) . "\" " .
                       "title=\"" . ($title ? x($text) : CMS_L_IFC_010) . "\" " .
                       "contenteditable=\"plaintext-only\" " .
                       "class=\"hl-" . x($class) . "\" " .
                       "onblur=\"this.parentElement.removeAttribute('data-focus');" .
                                 ($_language ? "ifc_language_save(this,'" . qx($name) . "');" :
                                "ifc_set('" . qx($name) . "',this.value);") . "\" " .
                       "onchange=\"ifc_highlight(this," . qx($mode) . "," . qx($no_insert) . ");\" " .
                       "onfocus=\"this.parentElement.setAttribute('data-focus','');\" " .
                       "onkeydown=\"return ifc_keydown(event);\" " .
                       "onkeyup=\"ifc_highlight(this," . qx($mode) . "," . qx($no_insert) . ");\">" .
                 "</code>" .
                 "</span>" .
                 jscript("ifc_contenteditable_init(\"" . q($_name) . "\");"));

            //hidden form submission element
            if (! $_language)
                echo("<input name=\"" . x($name) . "\" " .
                            "type=\"hidden\" " .
                            "value=\"" . x($value) . "\">" .
                     jscript("var object = ifc_object(\"" . q($_name) . "\");" .
                             "object[\"ifc_state_nosave\"] = true;" .
                             "ifc_copy(\"" . q($name) . "\", object);" .
                             "delete object[\"ifc_state_nosave\"];"));
            break;

        case "checkbox":
//..................................................................................................

            //generate element name
            if (stre($name))
            {
                $index++;
                $name = "ifc_param$index";
            };

            $_language = FALSE; //disable language

            //label
            if (nstre($_label))
            {
                $class = $fullwidth ? " fullwidth" : "";
                echo("<label class=\"element$class\">");
                $affix[] = "<span>" .
                           $_label .
                           "</span>" .
                           "</label>";
            };

            //element
            echo("<input name=\"" . x($name) . "\" " .
                        "type=\"checkbox\" " .
                        "value=\"" . x($value) . "\"" .
                        ($title    ? " title=\"" . x($text) . "\"" : "") .
                        ($checked  ? " checked"                    : "") .
                        ($disabled ? " disabled"                   : "") . ">");
            break;

        case "select":
        case "multiselect":
//..................................................................................................

            //generate element name
            if (stre($name))
            {
                $index++;
                $name = "ifc_param$index";
            };

            $_language = FALSE; //disable language

            //full size/width
            if ($fullsize || $fullwidth)
            {
                $class = $fullsize ? "fullsize" : "fullwidth";
                echo("<div class=\"$class\">");
                $affix[] = "</div>";
            };

            //label
            if (nstre($_label))
            {
                echo("<label class=\"element\">" .
                     "<span>" .
                     $_label .
                     "</span>$break");
                $affix[] = "</label>";
            };

            //element
            switch ($_type = $type . (($height > 1) ? "-list" : ""))
            {
            case "multiselect":

                echo("<select name=\"" . x($name) . "[]\" " .
                             "multiple");
                break;

            case "select":

                echo("<select name=\"" . x($name) . "\"");
                break;

            case "multiselect-list":
            case "select-list":

                echo("<div id=\"" . x($name) . "\" " .
                          "data-size=\"$height\" " .
                          "class=\"select\"");
                $__type = streq($_type, "multiselect-list") ?
                          "checkbox" : "radio";
                break;
            };

            echo(($title      ? " title=\"" . x($text) . "\""                    : "") .
                 (($size > 1) ? " style=\"--width:" . ($size * 10 + 20) . "px\"" : "") . ">");

            switch ($_type)
            {
            case "multiselect":
            case "select":

                echo("<button>" .
                     "<selectedcontent>" .
                     "</selectedcontent>" .
                     "</button>");
            };

            //prepare options
            if (! is_array($text))
            {
                $text = (string)$text;
                $text = ($text === "") ? [] : [$text => $text];
            };

            //prepare value
            if (! is_array($value))
            {
                $value = (string)$value;
                $value = ($value === "") ? [] : [$value];
            };

            //add missing options
            foreach ($value AS $_value)
            {
                if (nstre($_value) && (! in_array($_value, $text)))
                    $text[CMS_L_IFC_005 . ": $_value"] = $_value;
            };

            //display options
            foreach ($text AS $key => $_value)
            {
                //label
                [$key, $url, $_label, $img] = ifc_parse_label($key);

                //cut to size
                if ($size > 1)
                {
                    $_size = utf8_strlen($key);
                    if ($_size > $size) $key = strabridge($key, $size);
                };

                //selected
                $flag = in_array($_value, $value);

                switch ($_type)
                {
                case "multiselect":
                case "select":

                    echo("<option value=\"" . x($_value) . "\"" .
                                  ($flag ? " selected" : "") . ">" .
                         $_label .
                         "</option>");
                    break;

                case "multiselect-list":
                case "select-list":

                    echo("<label>" .
                         "<input name=\"" . x($name) . "[]\" " .
                                "type=\"$__type\" " .
                                "value=\"" . x($_value) . "\"" .
                                ($flag ? " checked>" : ">") .
                         $_label .
                         "</label>");
                    break;
                };
            };

            switch ($_type)
            {
            case "multiselect":
            case "select":

                echo("</select>");
                break;

            case "multiselect-list":
            case "select-list":

                echo("</div>" .
                     jscript("ifc_custom_select(\"" . q($name) . "\");"));
                break;
            };

            break;

        case "list":
//..................................................................................................

            //generate element name
            if (stre($name))
            {
                $index++;
                $name = "ifc_param$index";
            };

            $_language = FALSE; //disable language

            //full size/width
            if ($fullsize || $fullwidth)
            {
                $class   = $fullsize ? "fullsize" : "fullwidth";
                echo("<div class=\"$class\">");
                $affix[] = "</div>";
            };

            //label
            if (nstre($_label))
            {
                echo("<label class=\"element\">" .
                     "<span>" .
                     $_label .
                     "</span>$break");
                $affix[] = "</label>";
            };

            //element
            echo("<input name=\"" .x($name) . "\" " .
                        "type=\"text\" " .
                        "value=\"" . x($value) . "\" " .
                        "list=\"" . x($name) . "\" " .
                        "style=\"--width:" . ($size * 10 + 20) . "px\" " .
                        "onfocus=\"ifc_del(this);\"" .
                        ($title     ? " title=\"" . x($text) . "\"" : "") .
                        ($maxlength ? " maxlength=\"$maxlength\""   : "") .
                        ($disabled  ? " disabled"                   : "") . ">" .
                 "<datalist id=\"" . x($name) . "\">");

            if ((! is_array($text)) || (! count($text)))
                $text = [" " => NULL];

            //display options
            foreach ($text AS $key => $_value)
            {
                echo("<option value=\"" . x($_value) . "\">" .
                     x($key) .
                     "</option>");
            };

            echo("</datalist>");
            break;

        case "radio":
//..................................................................................................

            //generate element name
            if (stre($name))
            {
                $name = "ifc_param";

                //new group
                if (
                     $new
                     ||
                     ($radio_index === 0)
                     ||
                     nstreq($name, $radio_name)
                   )
                {
                    $index++;
                    $radio_index  = $index;
                    $_radio_index = 0;
                    $radio_name   = $name;
                };

                $name .= $radio_index;
            }
            elseif (nstreq($name, $radio_name))
            {
                $_radio_index = 0;
                $radio_name   = $name;
            };

            $_language = FALSE; //disable language

            //label
            if (nstre($_label))
            {
                $class = $fullwidth ? " fullwidth" : "";
                echo("<label class=\"element$class\">");
                $affix[] = "<span>" .
                           $_label .
                           "</span>" .
                           "</label>";
            };

            //element
            echo("<input name=\"" . x($name) . "\" " .
                        "type=\"radio\" " .
                        "value=\"" . x($value) . "\"" .
                        ($title    ? " title=\"" . x($text) . "\"" : "") .
                        ($checked  ? " checked"                    : "") .
                        ($disabled ? " disabled"                   : "") . ">");

            $_radio_index++;
            break;

        case "title":
//..................................................................................................

            if (stre($_label)) return;

            $_language = FALSE; //disable language

            //element
            echo("<div class=\"title\">" .
                 $_label .
                 "</div>");
            break;

        case "label":
//..................................................................................................

            //memorize label
            $label = $text;
            return;

        case "description":
//..................................................................................................

            if (stre($_label)) return;

            $_language = FALSE; //disable language

            //element
            echo("<div class=\"label\">" .
                 $_label .
                 "</div>");
            break;

        case "info":
//..................................................................................................

            if (stre($_label)) return;

            $_language = FALSE; //disable language

            //element
            echo("<div class=\"info\">" .
                 $_label .
                 "</div>");
            break;

        case "dummy":
//..................................................................................................

            $index += $size; //skip one or more elements
            return;

        case "default":
//..................................................................................................

            return; //invalid
        };

        //language selector
        if ($_language)
        {
            //use default if explicit language value is empty
            if (stre(language_get($value, $__language, TRUE)) && in_array("", $language)) $__language = "";

            //combined language form submission element
            echo("<input name=\"" . x($name) . "\" " .
                        "type=\"hidden\" " .
                        "value=\"" . x($value) . "\" " .
                        "data-l=\"" . x($_name) . "\">" .

            //load value into editing area
                 jscript("ifc_language_select(\"" . q($name) . "\", \"" . q($_name) . "\", \"" . q($__language) . "\");"));

            //language buttons
            if (count($language) > 0)
            {
                $map = new map("#system/language.image");

                echo("<span class=\"language\">");

                foreach ($language AS $value)
                {
                    $flag = nstre($value);
                    echo("<a id=\"" . x($name) . ":" . x($value) . "\" " .
                            "href=\"javascript:;\" " .
                            "title=\"" . ($flag ? x($value) : CMS_L_LANGUAGE_DEFAULT) . "\" " .
                            "onclick=\"return ifc_language_select('" . qx($name) . "','" . qx($_name) . "','" . qx($value) . "');\" " .
                            "class=\"language-" . (streq($__language, $value) ? "on" : "off") . "\">" .
                         image(($flag &&
                               (($image = $map->get_value($value)) !== NULL) &&
                               is_file(CMS_DATA_PATH . "language/$image")) ?
                               CMS_DATA_URL . "language/$image" :
                               "language/default",
                               16, 12) .
                         "</a>");
                };

                echo("</span>");
            };
        };

        //append affix
        echo(implode(array_reverse($affix)));

        //memorize type
        $prev_type = $type;
    }

//--------------------------------------------------------------------------------------------------

    function dummy($count = 1)
    {
        $this->set(NULL, "dummy $count");
    }

//--------------------------------------------------------------------------------------------------

    function close()
    {
        echo("</div>" .
             "</form>");

        //return value
        if (CMS_IFC_SELECT)
        {

?><script>

var ifc_return_opener_url = this.opener.document.URL;

function ifc_return(value = null,
                    title = null)
{
    if (this.opener && (! this.opener.closed) && (this.opener.document.URL === ifc_return_opener_url))
    {
        var object = ifc_object("<?php echo(q(CMS_IFC_SELECT));?>");
        if (value === null) value = ifc_get(object);
        if (title === null) title = ifc_title(object);
        var left   = this.opener.fx_position_left();
        var _top   = this.opener.fx_position_top();
<?php

            if (streq(substr(CMS_IFC_SELECT_ACTION, 0, 11), "javascript:"))
            {

?>
        var action = "<?php echo(substr(q(CMS_IFC_SELECT_ACTION), 11));?>";
        action     = action.replace(/%return%/, addslashes(value));
        action     = action.replace(/%title%/,  addslashes(title));
        action     = action.replace(/%left%/,   left);
        action     = action.replace(/%top%/,    _top);
        try { (new Function(action))(); } catch (e) {};
<?php

            }
            else
            {

?>
        var action = "<?php echo(q(CMS_IFC_SELECT_ACTION));?>";
        action     = action.replace(/%return%/, encodeURIComponent(value));
        action     = action.replace(/%title%/,  encodeURIComponent(title));
        action     = action.replace(/%left%/,   left);
        action     = action.replace(/%top%/,    _top);
        this.opener.location.replace(action);
<?php

            };

?>
    };

    if (this === top) this.close();
};

function ifc_return_close()
{
    if ((! this.opener) || this.opener.closed || (this.opener.document.URL !== ifc_return_opener_url))
    {
        this.close();
        return;
    };
};

function ifc_return_clear()
{
    ifc_return("");
};

if (
     (ifc_object("<?php echo(q(CMS_IFC_SELECT));?>") === null)
     &&
     (this === top)
   )
    ifc_return_close();

</script><?php

        };

        echo("</body>" .
             "</html>");
        exit();
    }
};