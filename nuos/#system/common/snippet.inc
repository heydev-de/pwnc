<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

function permission($array,
                    $return = FALSE)
{
    //hide when in editing view mode
    if (
         (! $return)
         &&
         defined("CMS_TEMPLATE_OPTION")
         &&
         (CMS_TEMPLATE_OPTION === CMS_TEMPLATE_OPTION_NONE)
       )
        return;

    if (! cms_permission("interface.permission", NULL, NULL)) return;

    $application = cms_application(TRUE, TRUE);
    $buffer      = "<div title=\"" . CMS_L_COMMON_010 . "\" " .
                        "class=\"permission\">" .
                   image("permission/icon_permission") . " " .
                   x(cms_application()) . ": ";

    foreach ($array AS $key => $value)
    {
        if ($key) $key = ".$key";
        $buffer .= "<a href=\"" . x(cms_url(CMS_MODULES_URL . "interface.php",
                                            ["ifc_page"    => "permission",
                                             "ifc_message" => "_set",
                                             "ifc_option"  => "external",
                                             "ifc_param1"  => $application . $key],
                                             TRUE)) . "\" " .
                      "title=\"" . x($application . $key) . "\" " .
                      "onclick=\"load_page(this.href);return false;\">" .
                   x($value) .
                   "</a>" .
                   " | ";
    };

    $buffer = substr($buffer, 0, -3) . "</div>";
    if ($return) return $buffer;
    echo($buffer);
};

//--------------------------------------------------------------------------------------------------

function insert($position = NULL)
{
    //hide when in editing view mode
    if (defined("CMS_TEMPLATE_OPTION") &&
        CMS_TEMPLATE_OPTION === CMS_TEMPLATE_OPTION_NONE)
        return;

    $key = cms_application();
    if ($position) $key .= ".$position";

    $data   = new data("#system/insert");
    $insert = $data->get($key, "insert");

    if (
         cms_permission("interface.insert", NULL, NULL)
         &&

    //only display in editing mode
         (
           (! defined("CMS_TEMPLATE_OPTION"))
           ||
           CMS_TEMPLATE_OPTION
         )
       )
        echo("<div>" .
             "<a href=\"" . x(cms_url(CMS_MODULES_URL . "interface.php",
                                      ["ifc_page"    => "insert",
                                       "ifc_message" => "_add",
                                       "ifc_option"  => "external",
                                       "ifc_param1"  => $key,
                                       "object"      => "\x1B",
                                       TRUE])) . "\" " .
                "title=\"" . CMS_L_COMMON_011 . "\" " .
                "onclick=\"load_page(this.href);return false;\">" .
             image("insert/button_edit") .
             "</a>" .
             "</div>");

    if ($insert)
    {
        $data = new data("#system/insert.code");
        $code = l($data->get($insert, "code"));

        if (nstre($code))
            echo($data->get($insert, "html") ?
                 parse_token($code) :
                 "<div>" . parse_text($code) . "</div>");
    };
};

//--------------------------------------------------------------------------------------------------

function class_varied($option = NULL,
                      $index  = 0)
{
    static $flag  = NULL;

    $index        = (int)$index;
    if (($option === TRUE) || (! isset($flag[$index]))) $flag[$index] = TRUE;
    $flag[$index] = ! $flag[$index];

    if (is_string($option)) return $flag[$index] ? " class=\"varied $option\"" : " class=\"$option\"";
    if ($flag[$index])      return " class=\"varied\"";
    return "";
};

//--------------------------------------------------------------------------------------------------

function jscript($code)
{
    return "<script>" .
           str_replace("</", "<\/", $code) .
           "</script>";
};

//--------------------------------------------------------------------------------------------------

function stylesheet($url,
                    $async = TRUE)
{
    $url = x($url);
    return ($async ?
           "<link rel=\"preload\" " .
                 "href=\"$url\" " .
                 "as=\"style\" " .
                 "onload=\"this.onload=null;this.rel='stylesheet';\">" .
           "<noscript>" : "") .
           "<link rel=\"stylesheet\" " .
                 "href=\"$url\">" .
           ($async ?
           "</noscript>" : "");
};

//--------------------------------------------------------------------------------------------------

function javascript($url,
                    $async = TRUE,
                    $defer = FALSE)
{
    $url = x($url);
    return "<script src=\"$url\"" .
                    ($async ? " async" : "") .
                    ($defer ? " defer" : "") . ">" .
           "</script>";
};

//--------------------------------------------------------------------------------------------------

function select($option,
                $preset    = NULL,
                $name      = NULL,
                $set_value = FALSE,
                $disabled  = FALSE,
                $height    = NULL)
{
    if (! is_array($option)) return FALSE;

    if ($height != NULL)     $name .= "[]";

    echo("<select" .
         (nstre($name)   ? " name=\"" . x($name) . "\""              : "") .
         (nstre($height) ? " size=\"" . (int)$height . "\" multiple" : "") .
         ($disabled      ? " disabled"                               : "") . ">");

    foreach ($option AS $key => $value)
    {
        $_value   = $set_value                  ? $value      : $key;
        $selected = is_array($preset) ?
                    (in_array($_value, $preset) ? " selected" : "") :
                    (streq($_value, $preset)    ? " selected" : "");

        echo("<option value=\"" . x($_value) . "\"$selected>" .
             x($key) .
             "</option>");
    };
    echo("</select>");

    return TRUE;
};

//--------------------------------------------------------------------------------------------------

function info($text)
{
    return "<div>" .
           image("icon_info") . "&nbsp;" .
           "<strong>" .
           $text .
           "</strong>" .
           "</div>";
};

//--------------------------------------------------------------------------------------------------

function alert($text)
{
    return "<div>" .
           image("icon_alert") . "&nbsp;" .
           "<strong style=\"COLOR:#ff0000\">" .
           $text .
           "</strong>" .
           "</div>";
};

//--------------------------------------------------------------------------------------------------

function pagination($url,
                    $page,
                    $count,
                    $next   = CMS_L_COMMAND_NEXT,
                    $class  = NULL,
                    $offset = 0)
{
    if ($count <= 1) return;
    $count   += $offset - 1;
    $page     = max($offset, min((int)$page, $count));
    $start    = max($page - 2, $offset);
    $end      = min($page + 2, $count);
    $_offset  = $offset + 1;

    echo("<nav" . (nstre($class) ? " class=\"" . x($class) . "\"" : "") . ">");

    //previous command
    if ($page > $offset)
        echo("<a href=\"" . x(str_replace("%page%", $page - 1, $url)) . "\" " .
                "rel=\"prev\" " .
                "title=\"" . CMS_L_COMMAND_PREVIOUS . "\">" .
             "‹" .
             "</a> ");

    //start
    if ($start > $_offset)
        echo("<a href=\"" . x(str_replace("%page%", $offset, $url)) . "\" " .
                "rel=\"first\" " .
                "title=\"" . CMS_L_COMMAND_START . "\">" .
             "1" .
             "</a>" .
             " … ");

    //page number
    for ($i = ($start > $_offset) ? $start : $offset; $i <= $end; $i++)
    {
        $_i = $i - $offset + 1;
        echo("<a href=\"" . x(str_replace("%page%", $i, $url)) . "\">" .
             (($i === $page) ? "<strong>" . $_i . "</strong>" : $_i) .
             "</a>" .
             (($i < $end) ? " | " : ""));
    };

    //ellipsis
    if ($end < $count)      echo(" … ");
    elseif ($page < $count) echo(" | ");

    //next command
    if ($page < $count)
        echo("<a href=\"" . x(str_replace("%page%", $page + 1, $url)) . "\" " .
                "rel=\"next\" " .
                "title=\"" . CMS_L_COMMAND_NEXT . "\">" .
             x($next) . "&nbsp;›" .
             "</a>");

    echo("</nav>");
};

//--------------------------------------------------------------------------------------------------

function language_selector($language = NULL,
                           $url      = "javascript:console.log('%language%');",
                           $encoding = NULL, //default: q()
                           $width    = 16,
                           $height   = 12)
{
    if (stre(CMS_LANGUAGE_ENABLED)) return;

    $encoding  ??= __NAMESPACE__ ."\\q";
    $map         = new map("#system/language.image");
    $_language   = explode(",", CMS_LANGUAGE_ENABLED);
    $_url        = str_replace("%language%", "", $url);
    $class       = "language-" . (stre($language) ? "on" : "off");
    $image       = "language/default";

    echo("<div class=\"language\">" .

         "<a href=\"" . x($_url) . "\" " .
            "title=\"" . CMS_L_LANGUAGE_DEFAULT . "\" " .
            "class=\"$class\">" .
         image($image, $width, $height) .
         "</a>");

    foreach ($_language AS $value)
    {
        $_value = is_callable($encoding) ? ($encoding)($value) : x($value);
        $_url   = str_replace("%language%", $_value, $url);
        $class  = "language-" . (streq($language, $value) ? "on" : "off");
        $image  = $map->get_value($value);
        $image  = (($image !== NULL) && is_file(CMS_DATA_PATH . "language/$image")) ?
                  CMS_DATA_URL . "language/$image" : "language/default";

        echo("<a href=\"" . x($_url) . "\" " .
                "title=\"" . x($value) . "\" " .
                "class=\"$class\">" .
             image($image, $width, $height) .
             "</a>");
    };

    echo("</div>");
};