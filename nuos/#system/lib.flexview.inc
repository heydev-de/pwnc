<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   FLEXVIEW CLASS
//==================================================================================================

define("CMS_FLEXVIEW_ENTRY_TYPE_NONE",  0);
define("CMS_FLEXVIEW_ENTRY_TYPE_BASE",  1);
define("CMS_FLEXVIEW_ENTRY_TYPE_ENTRY", 2);
define("CMS_FLEXVIEW_ENTRY_TYPE_END",   3);

class flexview_entry
{

//properties
//--------------------------------------------------------------------------------------------------

    public $type        = CMS_FLEXVIEW_ENTRY_TYPE_NONE;
    public $index       = NULL;
    public $parent      = NULL;
    public $position    = 0;
    public $count       = 0;
    public $subcount    = 0;
    public $indentation = 0;
    public $open        = FALSE;
};

class flexview
{

//properties
//--------------------------------------------------------------------------------------------------

    public $object              = ["" => ["#data" => ["#type" => "base"]]];
    public $icon_default        = NULL;
    public $value_function      = NULL;
    public $encoding_function   = NULL;
    public $display_function    = NULL;
    public $display             = "%checkbox%<a[ href=\"%action%\"][ class=\"%class%\"]>%mark%[%icon% ]%name%</a>";
    public $index               = "";
    public $checkbox_identifier = NULL;
    public $checkbox_index      = 0;
    public $checkbox_list       = NULL;
    public $mark                = NULL;
    public $icon_custom         = NULL;
    public $action              = "%index%";
    public $name_key            = "name";
    public $image_button_key    = "image_button";
    public $image_hover_key     = "image_hover";
    public $image_active_key    = "image_active";
    public $description_key     = "description";
    public $base                = "";
    public $param               = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct()
    {
        $data                             = new data("#system/flexview.icon");
        $application                      = cms_application();
        $this->icon_default["#base"]      = $data->get($application, "#base");
        $this->icon_default["entry"]      = $data->get($application, "entry");
        $this->icon_default["container"]  = $data->get($application, "container");
        $this->icon_default["+container"] = $data->get($application, "+container");
        if (! $this->icon_default["#base"])      $this->icon_default["#base"]      = "flexview/icon_base";
        if (! $this->icon_default["entry"])      $this->icon_default["entry"]      = "flexview/icon_entry";
        if (! $this->icon_default["container"])  $this->icon_default["container"]  = "flexview/icon_entrycontainer";
        if (! $this->icon_default["+container"]) $this->icon_default["+container"] = "flexview/icon_entryopen";
        $this->encoding_function          = __NAMESPACE__ . "\\x";
    }

//methods
//--------------------------------------------------------------------------------------------------

    function set_value_function($callback_function)
    {
        if (is_callable($callback_function)) $this->value_function = $callback_function;
    }

//--------------------------------------------------------------------------------------------------

    function set_encoding_function($callback_function)
    {
        if (is_callable($callback_function)) $this->encoding_function = $callback_function;
    }

//--------------------------------------------------------------------------------------------------

    function set_display_function($callback_function)
    {
        if (is_callable($callback_function)) $this->display_function = $callback_function;
    }

//--------------------------------------------------------------------------------------------------

    function set_index($value)
    {
        if (! isset($this->object[$value]))
        {
            if (! isset($this->object[$this->base])) $this->base = "";
            $value = $this->base;
        };

        $this->index = $value;
    }

//--------------------------------------------------------------------------------------------------

    function set_checkbox_identifier($value)
    {
        $this->checkbox_identifier = $value;
        $this->checkbox_index      = 0;
    }

//--------------------------------------------------------------------------------------------------

    function set_checkbox_list($value)
    {
        $this->checkbox_list = is_array($value) ? array_flip($value) : NULL;
    }

//--------------------------------------------------------------------------------------------------

    function get_checkbox($index)
    {
        static $flag = TRUE;

        if (! $this->checkbox_identifier) return NULL;

        if ($flag)
        {

//javascript

?><script>

function flexview_checkbox_load(name)
{
    var input   = document.getElementsByName(name)[0];
    var label   = input.parentElement;
    var image   = input.nextElementSibling;
    var checked = input.checked;

    label.onmouseenter  = function()
    {
        if ((fx_keyboard_key == 16) || //shift
            (fx_keyboard_key == 17) || //ctrl
            (fx_keyboard_key == 18))   //alt
            this.firstElementChild.click();
    };

    label.className     = checked ? "fv-cb-on" : "fv-cb-off";
    input.onclick       = function() { flexview_checkbox_click(this); };
    input.className     = "fv-cb";
    image.className     = "fv-cb-space";
    image.style.display = "";
};

function flexview_checkbox_click(trigger)
{
    var label       = trigger.parentElement;
    var checked     = trigger.checked;
    label.className = checked ? "fv-cb-on" : "fv-cb-off";
};

</script><?php

            $flag = FALSE;
        };

        $name    = $this->checkbox_identifier . "[" . $this->checkbox_index++ . "]";
        $checked = isset($this->checkbox_list[$index]);
        $index   = $this->value_function ? call_user_func($this->value_function, $index) : $index;

        return "<label>" .
               "<input name=\"" . x($name) . "\" " .
                      "type=\"checkbox\" " .
                      "value=\"" . x($index) . "\"" .
                      ($checked ? " checked>" : ">") .
               "<img src=\"" . x("data:image/svg+xml," .
                                 r("<svg xmlns=\"http://www.w3.org/2000/svg\">" .
                                   "</svg>")) . "\" " .
                    "alt=\"\" " .
                    "style=\"DISPLAY:none\">" .
               "</label>" .
               jscript("flexview_checkbox_load(\"" . q($name) . "\");");
    }

//--------------------------------------------------------------------------------------------------

    function set_mark($value)
    {
        $this->mark = $value;
    }

//--------------------------------------------------------------------------------------------------

    function get_mark($index)
    {
        return isset($this->mark[$index]) ? $this->mark[$index] : NULL;
    }

//--------------------------------------------------------------------------------------------------

    function set_icon($value)
    {
        $this->icon_custom = $value;
    }

//--------------------------------------------------------------------------------------------------

    function get_icon($index,
                      $open = FALSE)
    {
        if ($this->icon_custom === FALSE) return NULL;

        if (streq($index, $this->base))
        {
            if (isset($this->icon_custom["#base"]))           return $this->icon_custom["#base"];
            return $this->icon_default["#base"];
        };

        $open |= streq($index, $this->index);

        if (isset($this->object[$index]["#data"]["#subtype"]))
        {
            $type = $this->object[$index]["#data"]["#subtype"];
            if ($open && isset($this->icon_custom["+$type"])) return $this->icon_custom["+$type"];
            if (isset($this->icon_custom[$type]))             return $this->icon_custom[$type];
        };

        if (isset($this->object[$index]["#data"]["#type"]))
        {
            $type = $this->object[$index]["#data"]["#type"];
            if ($open && isset($this->icon_custom["+$type"])) return $this->icon_custom["+$type"];
            if (isset($this->icon_custom[$type]))             return $this->icon_custom[$type];
        };

        if (count($this->object[$index]) > 2)
        {
            if ($open)                                        return $this->icon_default["+container"];
            return $this->icon_default["container"];
        };

        return $this->icon_default["entry"];
    }

//--------------------------------------------------------------------------------------------------

    function set_action($value)
    {
        $this->action = $value;
    }

//--------------------------------------------------------------------------------------------------

    function get_action($index)
    {
        return str_replace("%index%", $this->get_value($index), $this->action);
    }

//--------------------------------------------------------------------------------------------------

    function set_name_key($value)
    {
        $this->name_key = $value;
    }

//--------------------------------------------------------------------------------------------------

    function get_name($index)
    {
        if (
             isset($this->object[$index]["#data"][$this->name_key])
             &&
             ($name = l($this->object[$index]["#data"][$this->name_key]))
           )
            return $name;

        return streq($index, $this->base) ? CMS_L_FLEXVIEW_002 : CMS_L_UNKNOWN;
    }

//--------------------------------------------------------------------------------------------------

    function set_image_key($button,
                           $hover,
                           $active)
    {
        $this->image_button_key = $button;
        $this->image_hover_key  = $hover;
        $this->image_active_key = $active;
    }

//--------------------------------------------------------------------------------------------------

    function get_image($index,
                       $open = FALSE)
    {
        if (
             $open
             &&
             isset($this->object[$index]["#data"][$this->image_active_key])
             &&
             ($image = l($this->object[$index]["#data"][$this->image_active_key]))
           )
            return translate_url($image);

        if (
             isset($this->object[$index]["#data"][$this->image_button_key])
             &&
             ($image = l($this->object[$index]["#data"][$this->image_button_key]))
           )
            return translate_url($image);

        return NULL;
    }

//--------------------------------------------------------------------------------------------------

    function set_base($value)
    {
        if (! isset($this->object[$value])) $value = "";
        $this->base = $value;
    }

//--------------------------------------------------------------------------------------------------

    function display($index,
                     $open = FALSE)
    {
        if ($flag = streq($index, $this->index)) $open = TRUE;

        if ($this->display_function)
        {
            call_user_func($this->display_function, $this, $index, $open);
            return;
        };

        $display   = $this->display;
        $name      = x($this->get_name($index));
        $container = count($this->object[$index]) > 2;

        $array     = ["index"             => x($index),
                      "action"            => $this->get_action($index),
                      "image_url"         => x(($temp = $this->get_image($index, $open)) ? $temp : NULL),
                      "image"             => $temp ? image($temp) : NULL,
                      "image_button_url"  => x($temp = (isset($this->object[$index]["#data"][$this->image_button_key])   ?
                                             translate_url(l($this->object[$index]["#data"][$this->image_button_key])) : NULL)),
                      "image_button"      => $temp ? image($temp) : NULL,
                      "image_hover_url"   => x($temp = (isset($this->object[$index]["#data"][$this->image_hover_key])    ?
                                             translate_url(l($this->object[$index]["#data"][$this->image_hover_key]))  : NULL)),
                      "image_hover"       => $temp ? image($temp) : NULL,
                      "image_active_url"  => x($temp = (isset($this->object[$index]["#data"][$this->image_active_key])   ?
                                             translate_url(l($this->object[$index]["#data"][$this->image_active_key])) : NULL)),
                      "image_hover"       => $temp ? image($temp) : NULL,
                      "checkbox"          => (strpos($this->display, "%checkbox%") === FALSE) ?
                                             NULL : $this->get_checkbox($index),
                      "mark"              => ($temp = $this->get_mark($index)) ?
                                             image($temp) : NULL,
                      "icon"              => ($temp = $this->get_icon($index, $open)) ?
                                             image($temp) : NULL,
                      "name"              => $flag ? "<strong>$name</strong>" : $name,
                      "description"       => isset($this->object[$index]["#data"][$this->description_key]) ?
                                             parse_text(l($this->object[$index]["#data"][$this->description_key])) : NULL,
                      "description_plain" => isset($this->object[$index]["#data"][$this->description_key]) ?
                                             x(htmltoplain(remove_format(l($this->object[$index]["#data"][$this->description_key]), TRUE, TRUE))) : NULL,
                      "class"             => ltrim(($open      ? "active"     : NULL) .
                                                   ($container ? " container" : NULL))];

        foreach ($array AS $key => $value)
            $display = stre($value) ?
                       preg_replace("/(\\[[^\\[\\]]*)?%$key%([^\\[\\]]*\\])?/",     "",               $display) :
                       preg_replace("/(\\[([^\\[\\]]*))?%$key%(([^\\[\\]]*)\\])?/", "\${2}$value\\4", $display);

        echo($display);
    }

//--------------------------------------------------------------------------------------------------

    function get_value($index)
    {
        if ($this->value_function)    $index = call_user_func($this->value_function,    $index);
        if ($this->encoding_function) $index = call_user_func($this->encoding_function, $index);

        return $index;
    }

//--------------------------------------------------------------------------------------------------

    function set($index,
                 $data   = NULL,
                 $parent = "")
    {
        $this->object[$index]["#data"]   = $data;
        $this->object[$index]["#parent"] = $parent;
        $this->object[$parent][$index]   = &$this->object[$index];
    }

//--------------------------------------------------------------------------------------------------

    function get_path()
    {
        $index = $this->index;
        $path  = [0 => $index];

        while (isset($this->object[$index]["#parent"]))
        {
            $index  = $this->object[$index]["#parent"];
            if (streq($index, $this->base)) return array_reverse($path);
            $path[] = $index;
        };

        return [];
    }

//--------------------------------------------------------------------------------------------------

    function import_data(&$data)
    {
        $array = [0 => ""];
        $count = 0;

        $data->move("first");
        while ($key = $data->move("next"))
        {
            switch ($data->get($key, "#type"))
            {
            case "container":
//..................................................................................................

                $this->set($key, $data->get($key), $array[$count]);
                $array[++$count] = $key;
                break;

            default:
//..................................................................................................

                $this->set($key, $data->get($key), $array[$count]);
                break;

            case "/container":
//..................................................................................................

                unset($array[$count]);
                $count--;
            };
        };
    }

//--------------------------------------------------------------------------------------------------

    function import_database(&$result,
                             $index_key  = "id",
                             $parent_key = "container")
    {
        while ($array = mysql_fetch_assoc($result))
        {
            if ($index = $array[$index_key])
            {
                unset($array[$index_key]);

                if ($parent_key)
                {
                    $parent = $array[$parent_key];
                    unset($array[$parent_key]);
                }
                else
                {
                    $parent = "";
                };
            };

            $this->set($index, $array, $parent);
        };
    }

//--------------------------------------------------------------------------------------------------

    function show_custom($callback_function)
    {
        $key = $this->base;

        $flexview_entry        = new flexview_entry();
        $flexview_entry->type  = CMS_FLEXVIEW_ENTRY_TYPE_BASE;
        $flexview_entry->index = $key;

        if (isset($this->object[$key]["#parent"]))
        {
            $flexview_entry->parent   = $this->object[$key]["#parent"];
            $flexview_entry->subcount = count($this->object[$key]) - 2;
        }
        else
        {
            $flexview_entry->parent   = NULL;
            $flexview_entry->subcount = count($this->object[$key]);
        };

        $flexview_entry->position    = 0;
        $flexview_entry->count       = 0;
        $flexview_entry->indentation = 0;
        $flexview_entry->open        = TRUE;

        $position    = [0 => 0];
        $count       = [0 => $flexview_entry->subcount];
        $indentation = 0;
        $path        = $this->get_path();

        if (! call_user_func($callback_function, $flexview_entry))
        {
            reset($this->object[$key]);
            while (TRUE)
            {
                while (($_key = key($this->object[$key])) !== NULL)
                {
                    next($this->object[$key]);

                    if (preg_match("/^#(?:data|parent)$/", $_key)) continue;

                    $position[$indentation]++;

                    $flexview_entry->type        = CMS_FLEXVIEW_ENTRY_TYPE_ENTRY;
                    $flexview_entry->index       = $_key;
                    $flexview_entry->parent      = $key;
                    $flexview_entry->position    = $position[$indentation];
                    $flexview_entry->count       = $count[$indentation];
                    $flexview_entry->subcount    = count($this->object[$_key]) - 2;
                    $flexview_entry->indentation = $indentation;
                    $flexview_entry->open        = isset($path[$indentation]) &&
                                                   streq($path[$indentation], $_key);

                    if (! call_user_func($callback_function, $flexview_entry))
                    {
                        $key                      = $_key;
                        $position[++$indentation] = 0;
                        $count[$indentation]      = $flexview_entry->subcount;

                        reset($this->object[$key]);
                    };
                };

                if (! $indentation--) break;

                $key = $this->object[$key]["#parent"];
            };
        };

        $flexview_entry->type        = CMS_FLEXVIEW_ENTRY_TYPE_END;
        $flexview_entry->index       = NULL;
        $flexview_entry->parent      = NULL;
        $flexview_entry->position    = 0;
        $flexview_entry->count       = 0;
        $flexview_entry->subcount    = 0;
        $flexview_entry->indentation = 0;
        $flexview_entry->open        = FALSE;

        call_user_func($callback_function, $flexview_entry);
    }

//--------------------------------------------------------------------------------------------------

    function show_hierarchy($index                   = "",
                            $action                  = NULL,
                            $name_key                = "name",
                            $mark                    = NULL,
                            $icon                    = NULL,
                            $base                    = "",
                            $dragdrop_event_function = NULL,
                            $dragdrop_type_accept    = NULL)
    {
        static $flag = TRUE;

        $this->set_base($base);
        $this->set_index($index);
        $this->set_action($action);
        $this->set_name_key($name_key);
        $this->set_mark($mark);
        $this->set_icon($icon);
        $this->param["dragdrop_type_accept"] = $dragdrop_type_accept;

        if ($flag)
        {

?><script>

function flexview_hierarchy_load(id)
{
    var object = document.getElementById(id);
    var list   = object.getElementsByClassName("fv-hir-source");
    var i      = 0;
    var item   = null;
    var c      = list.length;
    var type   = 0;
    var accept = 0;

    for (; i < c; i++)
    {
        item   = list.item(i);
        type   = parseInt(item.getAttribute("data-fv-hir-type"));
        accept = parseInt(item.getAttribute("data-fv-hir-accept"));
        dd_register(item, type, accept);
    };

    list = object.getElementsByClassName("fv-hir-target");
    c    = list.length;

    for (i = 0; i < c; i++)
    {
        item   = list.item(i);
        accept = parseInt(item.getAttribute("data-fv-hir-accept"));
        dd_register(item, 0, accept, true);
    };

    dd_set_callback(<?php echo($dragdrop_event_function);?>);
};

</script><?php

            $flag = FALSE;
        };

        $this->show_custom([$this, "_show_hierarchy"]);
    }

//--------------------------------------------------------------------------------------------------

    function _show_hierarchy($flexview_entry)
    {
        static $indentation = -1;
        static $instance    = 0;

        $type_index    = 0;
        $accept_insert = 0;
        $accept_append = 0;

        if ($this->param["dragdrop_type_accept"] === NULL)
        {
            $type_index    = 1;
            $accept_insert = 1;
            $accept_append = 1;
        }
        elseif (isset($this->object[$flexview_entry->index]["#data"]["#type"]))
        {
            $_type = $this->object[$flexview_entry->index]["#data"]["#type"];

            if (isset($this->param["dragdrop_type_accept"][$_type]))
            {
                if (isset($this->param["dragdrop_type_accept"][$_type]["index"]))
                    $type_index    = $this->param["dragdrop_type_accept"][$_type]["index"];
                if (isset($this->param["dragdrop_type_accept"][$_type]["insert"]))
                    $accept_insert = $this->param["dragdrop_type_accept"][$_type]["insert"];
                if (isset($this->param["dragdrop_type_accept"][$_type]["append"]))
                    $accept_append = $this->param["dragdrop_type_accept"][$_type]["append"];
            };

            if (isset($this->object[$flexview_entry->index]["#data"]["#subtype"]))
            {
                $subtype = $this->object[$flexview_entry->index]["#data"]["#subtype"];
                if (isset($this->param["dragdrop_type_accept"]["$_type/$subtype"]["index"]))
                    $type_index    = $this->param["dragdrop_type_accept"][$_type]["index"];
                if (isset($this->param["dragdrop_type_accept"]["$_type/$subtype"]["insert"]))
                    $accept_insert = $this->param["dragdrop_type_accept"][$_type]["insert"];
                if (isset($this->param["dragdrop_type_accept"]["$_type/$subtype"]["append"]))
                    $accept_append = $this->param["dragdrop_type_accept"][$_type]["append"];
            };
        };

        switch ($flexview_entry->type) //the type of event that is passed
        {
        case CMS_FLEXVIEW_ENTRY_TYPE_BASE: //before first entry
//..................................................................................................

            //empty
            if (! $flexview_entry->subcount)
            {
                echo(CMS_L_FLEXVIEW_001 . "<br>");
                return TRUE;
            };

            //reset
            $indentation  = -1;
            $instance    +=  1;

            echo("<div id=\"fv-hir-" . x($instance) . "\" " .
                      "class=\"fv-hir\">");

            //display
            $this->display = "<a[ href=\"%action%\"]" .
                               "[ class=\"%class%\"]>" .
                             "%mark%[%icon% ]%name%" .
                             "</a>";

            //display base
            $this->display($flexview_entry->index);
            echo("<div data-fv-hir-index=\"" . x($flexview_entry->index) . "\" " .
                      "data-fv-hir-accept=\"" . x($accept_append) . "\" " .
                      "class=\"fv-hir-target\">" .
                 "</div>");

            return;

        case CMS_FLEXVIEW_ENTRY_TYPE_ENTRY: //current entry
//..................................................................................................

            //start sublist
            if ($indentation < $flexview_entry->indentation)
            {
                echo("<div class=\"fv-hir-level\">");
                $indentation = $flexview_entry->indentation;
            }

            //end sublist
            else
            {
                for (; $indentation > $flexview_entry->indentation; $indentation--)
                    echo("</div>");

                //separate entry
                echo("</div>" .
                     "<div class=\"fv-hir-level\">");
            };

            //display entry
            if ($flexview_entry->subcount > 0)
                echo("<input id=\"fv-hir-open-" . x($flexview_entry->index) . "\" " .
                            "type=\"checkbox\" " .
                            "value=\"1\"" .
                            ($flexview_entry->open ? " checked" : "") . ">" .
                     "<label for=\"fv-hir-open-" . x($flexview_entry->index) . "\">" .
                     "</label>");
            echo("<div data-fv-hir-index=\"" . x($flexview_entry->index) . "\" " .
                      "data-fv-hir-type=\"" . x($type_index) . "\" " .
                      "data-fv-hir-accept=\"" . x($accept_insert) . "\" " .
                      "class=\"fv-hir-source\">");
            $this->display($flexview_entry->index);
            echo("</div>" .
                 "<div data-fv-hir-index=\"" . x($flexview_entry->index) . "\" " .
                      "data-fv-hir-accept=\"" . x($accept_append) . "\" " .
                      "class=\"fv-hir-target\">" .
                 "</div>");

            return; //display subsequent entries

        case CMS_FLEXVIEW_ENTRY_TYPE_END: //after last entry
//..................................................................................................

            //end sublist
            for (; $indentation >= 0; $indentation--)
                echo("</div>");

            echo("</div>" .
                 jscript("flexview_hierarchy_load(\"fv-hir-" . x($instance) . "\");"));
        };
    }

//--------------------------------------------------------------------------------------------------

    function show_tree($index    = "",
                       $action   = NULL,
                       $name_key = "name",
                       $mark     = NULL,
                       $icon     = NULL,
                       $base     = "")
    {
        $this->set_base($base);
        $this->set_index($index);
        $this->set_action($action);
        $this->set_name_key($name_key);
        $this->set_mark($mark);
        $this->set_icon($icon);

        $this->show_custom([$this, "_show_tree"]);
    }

//--------------------------------------------------------------------------------------------------

    function _show_tree($flexview_entry)
    {
        static $indentation = "";

        switch ($flexview_entry->type)
        {
        case CMS_FLEXVIEW_ENTRY_TYPE_BASE:
//..................................................................................................

            //empty
            if (! $flexview_entry->subcount)
            {
                echo(CMS_L_FLEXVIEW_001 . "<br>");
                return TRUE;
            };

            //display
            $this->display = "<a[ href=\"%action%\"]" .
                               "[ class=\"%class%\"]>" .
                             "%mark%[%icon% ]%name%" .
                             "</a><br>";

            //display base
            $this->display($flexview_entry->index);

            $this->display = "%checkbox%" .
                             "<a[ href=\"%action%\"]" .
                               "[ class=\"%class%\"]>" .
                             "%mark%[%icon% ]%name%" .
                             "</a><br>";

            return;

        case CMS_FLEXVIEW_ENTRY_TYPE_ENTRY:
//..................................................................................................

            //indentation
            if ($flexview_entry->indentation)
            {
                for ($i = 1; $i < $flexview_entry->indentation; $i++)
                    echo($indentation[$i] ?
                         "<img src=\"" . x(CMS_IMAGES_URL . "flexview/tree_indentation.svg") . "\" " .
                              "alt=\"\" " .
                              "class=\"fv-tree\">" :
                         $this->space());

                $image = ($flexview_entry->position == $flexview_entry->count) ?
                         CMS_IMAGES_URL . "flexview/tree_branchclose.svg" :
                         CMS_IMAGES_URL . "flexview/tree_branchopen.svg";
                echo("<img src=\"" . x($image) . "\" " .
                          "alt=\"\" " .
                          "class=\"fv-tree\">");
            };

            //open
            if ($flexview_entry->open && $flexview_entry->subcount)
                $indentation .= ($flexview_entry->position == $flexview_entry->count) ? "0" : "1";

            //display
            $this->display($flexview_entry->index, $flexview_entry->open);

            return ! $flexview_entry->open;

        case CMS_FLEXVIEW_ENTRY_TYPE_END:
//..................................................................................................

        };
    }

//--------------------------------------------------------------------------------------------------

    function show_target($index          = "",
                         $action         = NULL,
                         $action_insert  = NULL,
                         $action_append  = NULL,
                         $name_key       = "name",
                         $base           = "",
                         $type_insert    = NULL,
                         $subtype_insert = NULL,
                         $type_append    = NULL,
                         $subtype_append = NULL)
    {
        $this->set_base($base);
        $this->set_index($index);
        $this->set_action($action);
        $this->param["action_insert"]  = $action_insert;
        $this->param["action_append"]  = $action_append;
        $this->set_name_key($name_key);
        $this->param["type_insert"]    = $type_insert;
        $this->param["subtype_insert"] = $subtype_insert;
        $this->param["type_append"]    = $type_append;
        $this->param["subtype_append"] = $subtype_append;

        $this->show_custom([$this, "_show_target"]);
    }

//--------------------------------------------------------------------------------------------------

    function _show_target($flexview_entry)
    {
        static $indentation = -1;
        static $instance    = 0;

        //verify insert target type
        $action_insert = (bool)$this->param["action_insert"];

        if ($action_insert)
        {
            if ($this->param["type_insert"])
            {
                if (isset($this->object[$flexview_entry->index]["#data"]["#type"]))
                {
                    $type           = $this->object[$flexview_entry->index]["#data"]["#type"];
                    $action_insert  = isset($this->param["type_insert"][$type]) &&
                                      $this->param["type_insert"][$type];
                }
                else
                {
                    $action_insert  = FALSE;
                };
            };

            if ($action_insert && $this->param["subtype_insert"])
            {
                if (isset($this->object[$flexview_entry->index]["#data"]["#subtype"]))
                {
                    $subtype        = $this->object[$flexview_entry->index]["#data"]["#subtype"];
                    $action_insert |= isset($this->param["subtype_insert"][$subtype]) &&
                                      $this->param["subtype_insert"][$subtype];
                }
                else
                {
                    $action_insert  = FALSE;
                };
            };
        };

        //verify append target type
        $action_append = (bool)$this->param["action_append"];

        if ($action_append)
        {
            if ($this->param["type_append"])
            {
                if (isset($this->object[$flexview_entry->index]["#data"]["#type"]))
                {
                    $type           = $this->object[$flexview_entry->index]["#data"]["#type"];
                    $action_append  = isset($this->param["type_append"][$type]) &&
                                      $this->param["type_append"][$type];
                }
                else
                {
                    $action_append  = FALSE;
                };
            };

            if ($action_append && $this->param["subtype_append"])
            {
                if (isset($this->object[$flexview_entry->index]["#data"]["#subtype"]))
                {
                    $subtype        = $this->object[$flexview_entry->index]["#data"]["#subtype"];
                    $action_append |= isset($this->param["subtype_append"][$subtype]) &&
                                      $this->param["subtype_append"][$subtype];
                }
                else
                {
                    $action_append  = FALSE;
                };
            };
        };

        switch ($flexview_entry->type)
        {
        case CMS_FLEXVIEW_ENTRY_TYPE_BASE:
//..................................................................................................

            //reset
            $indentation  = -1;
            $instance    += 1;

            echo("<div id=\"fv-target-" . x($instance) . "\" " .
                      "class=\"fv-target\">" .

            //base
                 "<div class=\"fv-target-item\">");

            //display
            $this->display = "<a[ href=\"%action%\"]" .
                               "[ class=\"%class%\"]>" .
                             "[%icon% ]%name%" .
                             "</a><br>";
            $this->display($flexview_entry->index);

            //action
            if ($action_insert || $action_append)
            {
                //display
                $this->display = "<a[ href=\"%action%\"]" .
                                    " class=\"fv-target-append\">" .
                                 CMS_L_FLEXVIEW_003 .
                                 "</a><br>";
                $action        = $this->action;
                $_action       = $action_insert ? "action_insert" : "action_append";
                $this->set_action($this->param[$_action]);
                $this->display($flexview_entry->index);
                $this->set_action($action);
            };

            echo("</div>");

            return;

        case CMS_FLEXVIEW_ENTRY_TYPE_ENTRY:
//..................................................................................................

            //start sublist
            if ($indentation < $flexview_entry->indentation)
            {
                echo("<div class=\"fv-target-level\">");
                $indentation = $flexview_entry->indentation;
            }

            //end sublist
            else
            {
                for (; $indentation > $flexview_entry->indentation; $indentation--)
                    echo("</div>");

                //separate entry
                echo("</div>" .
                     "<div class=\"fv-target-level\">");
            };

            //toggle container
            if ($flexview_entry->subcount > 0)
                echo("<input id=\"fv-target-open-" . x($flexview_entry->index) . "\" " .
                            "type=\"checkbox\" " .
                            "value=\"1\"" .
                            ($flexview_entry->open ? " checked" : "") . ">" .
                     "<label for=\"fv-target-open-" . x($flexview_entry->index) . "\">" .
                     "</label>");

            //entry
            echo("<div class=\"fv-target-item\">");

            //display
            $this->display = "<a[ href=\"%action%\"]" .
                               "[ class=\"%class%\"]>" .
                             "[%icon% ]%name%" .
                             "</a><br>";
            $this->display($flexview_entry->index);

            //display insert action
            if ($action_insert)
            {
                $this->display = "<a[ href=\"%action%\"]" .
                                    " class=\"fv-target-insert\">" .
                                 CMS_L_FLEXVIEW_003 .
                                 "</a><br>";
                $action        = $this->action;
                $this->set_action($this->param["action_insert"]);
                $this->display($flexview_entry->index);
                $this->set_action($action);
            };

            //display append action
            if ($action_append)
            {
                $this->display = "<a[ href=\"%action%\"]" .
                                    " class=\"fv-target-append\">" .
                                 CMS_L_FLEXVIEW_003 .
                                 "</a><br>";
                $action        = $this->action;
                $this->set_action($this->param["action_append"]);
                $this->display($flexview_entry->index);
                $this->set_action($action);
            };

            echo("</div>");

            return; //display subsequent entries

        case CMS_FLEXVIEW_ENTRY_TYPE_END:
//..................................................................................................

            //end sublist
            for (; $indentation >= 0; $indentation--)
                echo("</div>");

            echo("</div>");
        };
    }

//--------------------------------------------------------------------------------------------------

    function show_path($index     = "",
                       $action    = NULL,
                       $name_key  = "name",
                       $delimiter = "›",
                       $base      = "")
    {
        $this->set_base($base);
        $this->set_index($index);
        $this->set_action($action);
        $this->set_name_key($name_key);

        if (streq($this->index, $this->base)) return;

        //path
        $path = $this->get_path();

        //base
        $this->display = "<a[ href=\"%action%\"]" .
                           "[ class=\"%class%\"]>" .
                         "%mark%[%icon% ]%name%" .
                         "</a>";

        $this->display($this->base);

        //display
        $this->display = " " . x($delimiter) . " " .
                         "<a[ href=\"%action%\"]" .
                           "[ class=\"%class%\"]>" .
                         "%mark%[%icon% ]%name%" .
                         "</a>";

        foreach ($path AS $key => $value)
            $this->display($value);
    }

//--------------------------------------------------------------------------------------------------

    function show_column($index    = "",
                         $action   = NULL,
                         $name_key = "name",
                         $mark     = NULL,
                         $icon     = NULL,
                         $base     = "")
    {
        $this->set_base($base);
        $this->set_index($index);
        $this->set_action($action);
        $this->set_name_key($name_key);
        $this->set_mark($mark);
        $this->set_icon($icon);

        $this->_show_column();
    }

//--------------------------------------------------------------------------------------------------

    function _show_column()
    {
        //empty
        if (count($this->object) == 1)
        {
            echo(CMS_L_FLEXVIEW_001 . "<br>");
            return;
        };

        $index = $this->index;

        //base
        if (nstreq($index, $this->base))
        {
            $this->display = "<a[ href=\"%action%\"]" .
                                " class=\"fv-col[ %class%]\">" .
                             "%mark%[%icon% ]%name%" .
                             "</a>";

            $this->display($this->base);

            //path
            if (count($this->object[$index]) < 3)
                $index = $this->object[$index]["#parent"];

            if (nstreq($index, $this->base))
            {
                $_index = $index;
                $path   = [0 => $_index];

                while (
                        isset($this->object[$_index]["#parent"])
                        &&
                        nstreq($_index = $this->object[$_index]["#parent"], $this->base)
                      )
                    $path[] = $_index;

                $path = array_reverse($path);

                $this->display = "<a[ href=\"%action%\"]" .
                                    " class=\"fv-col[ %class%]\">" .
                                 "%mark%[%icon% ]%name%" .
                                 "</a>";

                foreach ($path AS $key => $value)
                    $this->display($value, TRUE);
            };

            echo("<br>");
        };

        //display
        $this->display = "<div class=\"fv-col\">" .
                         "%checkbox%" .
                         "<a[ href=\"%action%\"]" .
                           "[ class=\"%class%\"]>" .
                         "%mark%[%icon% ]%name%" .
                         "</a>" .
                         "</div>";

        foreach ($this->object[$index] AS $key => $_)
        {
            if (! preg_match("/^#(?:data|parent)$/", $key))
                $this->display($key);
        };

        echo("<br style=\"CLEAR:both\">");
    }

//--------------------------------------------------------------------------------------------------

    function show_folder($index    = "",
                         $action   = NULL,
                         $name_key = "name",
                         $mark     = NULL,
                         $icon     = NULL,
                         $base     = "")
    {
        $this->set_base($base);
        $this->set_index($index);
        $this->set_action($action);
        $this->set_name_key($name_key);
        $this->set_mark($mark);
        $this->set_icon($icon);

        $this->_show_folder();
    }

//--------------------------------------------------------------------------------------------------

    function _show_folder()
    {
        //empty
        if (count($this->object) == 1)
        {
            echo(CMS_L_FLEXVIEW_001 . "<br>");
            return;
        };

        $index = $this->index;

        //base
        if (nstreq($index, $this->base))
        {
            $this->display = "<a[ href=\"%action%\"]" .
                               "[ class=\"%class%\"]>" .
                             "%mark%[%icon% ]%name%" .
                             "</a><br>";

            $this->display($this->base);

            //path
            if (count($this->object[$index]) < 3)
                $index = $this->object[$index]["#parent"];

            if (nstreq($index, $this->base))
            {
                $_index = $index;
                $path   = [0 => $_index];

                while (
                        isset($this->object[$_index]["#parent"])
                        &&
                        nstreq($_index = $this->object[$_index]["#parent"], $this->base)
                      )
                    $path[] = $_index;

                $path = array_reverse($path);

                foreach ($path AS $key => $value)
                    $this->display($value, TRUE);
            };
        };

        //display
        $this->display = "%checkbox%" .
                         "<a[ href=\"%action%\"]" .
                           "[ class=\"%class%\"]>" .
                         "%mark%[%icon% ]%name%" .
                         "</a><br>";

        foreach ($this->object[$index] AS $key => $_)
        {
            if (! preg_match("/^#(?:data|parent)$/", $key))
                $this->display($key);
        };
    }

//--------------------------------------------------------------------------------------------------

    function space($count = 1,
                   $size  = 18)
    {
        return "<img src=\"" . x("data:image/svg+xml," .
                                 r("<svg xmlns=\"http://www.w3.org/2000/svg\" " .
                                      "width=\"" . x($size * $count) . "\" " .
                                      "height=\"" . x($size) . "\">" .
                                   "</svg>")) . "\" " .
                    "alt=\" \" " .
                    "class=\"fv-space\">";
    }
};