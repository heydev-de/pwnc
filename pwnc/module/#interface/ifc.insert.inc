<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   INTERFACE
//==================================================================================================

//load libraries
if (! cms_load("flexview")) ifc_inactive($ifc_page);

//permission
ifc_permission(["" => t("access")]);

//retrieve selected object from permanent cache
cms_cache_init($object, "insert." . CMS_USER . ".object");

//initialize variables
$list ??= isset($object) ? [$object] : [];

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "insert":
case "insert_select":
case "insert_display":
case "insert_add":
case "insert_save":
case "insert_delete":
//..................................................................................................

    $data = new data("#system/insert.code");

    switch (CMS_IFC_MESSAGE)
    {
    case "insert_select":
//..................................................................................................

        $insert_object = $ifc_param;
        break;

    case "insert_display":
//..................................................................................................

        $code = l($data->get($insert_object, "code"));
        preview($data->get($insert_object, "html") ? $code : parse_text($code));
        exit();

    case "insert_add":
//..................................................................................................

        //create new entry at top
        $data->set_buffer([["name" => t("new_insert")]]);
        $_insert_object = $data->append();

        if ($data->save())
        {
            $insert_object = $_insert_object;
            $ifc_response  = t("msg_done");
            break;
        };

        $ifc_response = t("msg_error");
        break;

    case "insert_save":
//..................................................................................................

        //keep name if none is given
        if (stre($ifc_param1 = utf8_trim($ifc_param1)))
            $ifc_param1 = $data->get($insert_object, "name");

        $html = (int)$ifc_tab_1 === 2;
        $data->set(["name" => $ifc_param1,
                    "html" => $html,
                    "code" => $html ? $ifc_param3 : $ifc_param2],
                   $insert_object);

        $ifc_response = $data->save() ? t("msg_done") : t("msg_error");
        break;

    case "insert_delete":
//..................................................................................................

        if (blank($_list)) break; //no selection

        foreach ($_list AS $value) $data->del($value);

        if ($data->save())
        {
            //remove deleted objects from positions
            $data_insert = new data("#system/insert");

            $data_insert->move("first");
            while ($key = $data_insert->move("next"))
            {
                $_key = $data_insert->get($key, "insert");
                if ($data->get($_key) === NULL) $data_insert->del($key, "insert");
            };

            $data_insert->save();

            //deselect object if deleted
            if (in_array($insert_object, $_list)) $insert_object = "";

            $ifc_response = t("msg_done");
            break;
        };

        $ifc_response = t("msg_error");
        break;
    };

    if (blank($insert_object))
    {
        //select assigned type
        if ($object) $insert_object = $data->get($object, "insert");

        //select first type
        if (stre($insert_object)) $insert_object = $data->move("first");
    };

    //menu
    $menu = [t("command_add") .             "|insert/command_add"    => "insert_add",
             t("command_delete_selected") . "|insert/command_delete" => "#insert_delete",
             t("command_close") .           "|insert/command_cancel" => FALSE];

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   $menu,
                   ["object"        => $object,
                    "list"          => $list,
                    "insert_object" => $insert_object],
                   NULL,
                   t("inserts"));

    echo("<table class=\"layout\">" .

         "<colgroup>" .
         "<col>" .
         (nstre($insert_object) ? "<col style=\"WIDTH:0\">" : "") .
         "</colgroup>" .

         "<tr>" .
         "<td>");

    ifc_table_open();
    echo("<colgroup>" .
         "<col style=\"WIDTH:0\">" .
         "<col>" .
         "</colgroup>" .

    //header
         "<tr>" .

    //selection
         "<td class=\"select\">" .
         "</td>" .

    //name
         "<th>" .
         t("inserts") .
         "</th>" .
         "</tr>");

    data_sort($data, "name");

    $data->move("first");
    while ($key = $data->move("next"))
    {
        $name   = x($data->get($key, "name"));
        if (streq($key, $insert_object)) $name = "<strong>$name</strong>";
        $varied = ifc_varied();

        echo("<tr>" .

        //selection
             "<td class=\"select\">");
        $ifc->set(NULL, "checkbox", $key, streq($key, $insert_object), "_list[]");
        echo("</td>" .

        //name
             "<td$varied>" .
             "<a href=\"javascript:ifc_post('insert_select','" . qx($key) . "');\">" .
             image("insert/icon_insert") . " $name" .
             "</a>" .
             "</td>" .
             "</tr>");
    };

    //selection control
    echo("<tr>" .
         "<td colspan=\"2\" " .
             "class=\"select\" " .
             "style=\"TEXT-ALIGN:right\">");
    $ifc->set(t("all"),    "button", "javascript:ifc_list_activate(\"_list\");");
    $ifc->set(t("invert"), "button", "javascript:ifc_list_invert(\"_list\");");
    $ifc->set(t("none"),   "button", "javascript:ifc_list_deactivate(\"_list\");");
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    echo("</td>");

    if (nstre($insert_object))
    {
        //data
        echo("<td>");

        ifc_table_open();

        //header
        echo("<tr>" .
             "<th>" .
             t("settings") .
             "</th>" .
             "</tr>" .

        //settings
             "<tr>" .
             "<td>");

        //name
        $ifc->set(t("name"), "text 40 80", $data->get($insert_object, "name"));

        //display button
        $ifc->set(t("command_show"),
                  "button",
                  "javascript:load_page('" .
                  q(cms_url(["ifc_page"      => CMS_IFC_PAGE,
                             "ifc_message"   => "insert_display",
                             "insert_object" => $insert_object])) . "');");

        //save button
        $ifc->set(t("command_save"), "button b", "insert_save");

        //content
        init($ifc_tab_1, $data->get($insert_object, "html") ? 2 : 1);

        //text
        ifc_tab_open(t("content"));

        $ifc->set(NULL, "texteditor 60x15 ilrt", $data->get($insert_object, "code"));

        //html
        ifc_tab_next(t("html"));

        $ifc->set(NULL, "code_html 60x15 l", $data->get($insert_object, "code"));

        ifc_tab_close();

?><script>

//sync text and html input
fx_event_listen([ifc_object("ifc_tab_1"), ifc_object("ifc_tab_1", 1)], "change", () =>
{
    const value       = parseInt(ifc_get("ifc_tab_1") || ifc_get("ifc_tab_1", 1));
    const flag        = value === 1;
    const from_source = ifc_object(flag ? "ifc_param3"   : "ifc_param2");
    const from_target = ifc_object(flag ? "l_ifc_param3" : "l_ifc_param2");
    const to_source   = ifc_object(flag ? "ifc_param2"   : "ifc_param3");
    const to_target   = ifc_object(flag ? "l_ifc_param2" : "l_ifc_param3");
    ifc_copy(from_source, to_source);
    if (from_target && (from_target.language !== to_target.language))
        ifc_language_select(to_source, to_target, from_target.language);
});

</script><?php

        echo("</td>" .
             "</tr>");
        ifc_table_close();

        echo("</td>");
    };
    echo("</tr>" .
         "</table>");

    $ifc->close();

case "select":
//..................................................................................................

    $object = $ifc_param;
    $list   = [$object];
    break;

case "_add":
//..................................................................................................

    if (nstre($ifc_param1 = utf8_trim($ifc_param1)))
    {
        $data  = new data("#system/insert");
        $array = explode(".", "$object.$ifc_param1");
        $value = NULL;

        foreach ($array AS $_value)
        {
            if (stre($_value)) continue;

            $__value = $value;
            if ($value === NULL) $value  = $_value;
            else                 $value .= ".$_value";

            if ($data->get($value) === NULL)
            {
                $data->buffer = [$value    => ["name"  => $_value,
                                               "#type" => "container"],
                                 "/$value" => ["#type" => "/container"]];
                $data->insert($__value);
            };
        };

        if ($data->save())
        {
            $object       = $value;
            $list         = [$value];
            $ifc_response = t("msg_done");
            break;
        };
    };

    $ifc_response = t("msg_error");

case "add":
//..................................................................................................

    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object" => $object,
                    "list"   => $list],
                   "_add",
                   t("command_add"));

    $ifc->set(t("name"), "text 40 256 b");

    $ifc->close();

case "delete":
//..................................................................................................

    if (count($list) === 0) break; //no selection;

    $data    = new data("#system/insert");
    $stack   = [];
    $_object = $object;

    //get path to object
    while (($_object = $data->move("parent", $_object)) !== FALSE)
        array_unshift($stack, $_object);
    array_unshift($stack, "");

    //delete
    foreach ($list AS $value) $data->del($value);

    if ($data->save())
    {
        //select last existing object in path
        while ($data->get($object) === NULL)
        {
            $object = array_pop($stack);
            if (stre($object)) break;
        };

        $list         = [$object];
        $ifc_response = t("msg_done");
        break;
    };

    $ifc_response = t("msg_error");
    break;

case "save":
//..................................................................................................

    $data = new data("#system/insert");

    $data->set($insert_object, $object, "insert");

    $ifc_response = $data->save() ? t("msg_done") : t("msg_error");
    break;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

//load data
$data = new data("#system/insert");
if ($data->get($object) === NULL) $object = ""; //check selected object

//cache selected object permanently
if (nstre(CMS_IFC_MESSAGE)) cms_cache("insert." . CMS_USER . ".object", $object, TRUE);

//menu
$menu = [t("command_add") .             "|insert/command_add"    => "add",
         t("inserts") .                 "|insert/command_code"   => "insert",
         t("command_delete_selected") . "|insert/command_delete" => "#delete"];

//display
$ifc = new ifc(
    $ifc_response,
    $ifc_page,
    $menu,
    ["object" => $object]);

echo("<table class=\"layout\">" .

     "<colgroup>" .
     "<col>" .
     (nstre($object) ? "<col style=\"WIDTH:0\">" : "") .
     "</colgroup>" .

     "<tr>" .
     "<td>");

ifc_table_open();

//header
echo("<tr>" .
     "<th>" .
     t("position") .
     "</th>" .
     "</tr>" .

//position
     "<tr>" .
     "<td>");
$flexview = new flexview();
$flexview->import_data($data);
$flexview->set_encoding_function(__NAMESPACE__ . "\\qr");
$flexview->set_checkbox_identifier("list");
$flexview->set_checkbox_list($list);
$flexview->show_tree(
    $object,                                    //index
    "javascript:ifc_post('select','%index%');", //action
    "name");                                    //name key
echo("</td>" .
     "</tr>" .

//selection control
     "<tr>" .
     "<td class=\"select\" " .
         "style=\"TEXT-ALIGN:right\">");
$ifc->set(t("all"),    "button", "javascript:ifc_list_activate();");
$ifc->set(t("invert"), "button", "javascript:ifc_list_invert();");
$ifc->set(t("none"),   "button", "javascript:ifc_list_deactivate();");
echo("</td>" .
     "</tr>");
ifc_table_close();

echo("</td>");

//data
if (nstre($object))
{
    //data
    echo("<td>");

    ifc_table_open();

    //header
    echo("<tr>" .
         "<th>" .
         t("settings") .
         "</th>" .
         "</tr>" .

    //settings
         "<tr>" .
         "<td>");

    //collect inserts
    $_data = new data("#system/insert.code");
    $array = [" " => NULL];

    $_data->move("first");
    while ($key = $_data->move("next"))
    {
        $name  = $_data->get($key, "name");
        $_name = $name;
        $i     = 1;

        while (isset($array[$_name])) $_name = "$name (" . $i++ . ")";

        $array[$_name] = $key;
    };

    ksort($array, SORT_NATURAL | SORT_FLAG_CASE);

    //insert selection
    $insert = $data->get($object, "insert");

    $ifc->set(t("insert"), "label");
    $ifc->set($array,      "select 40 b", $insert, NULL, "insert_object");

    echo("<div style=\"TEXT-ALIGN:right\">");

    //display button
    if ($insert)
        $ifc->set(t("command_show"),
                  "button",
                  "javascript:load_page('" .
                  q(cms_url(["ifc_page"      => CMS_IFC_PAGE,
                             "ifc_message"   => "insert_display",
                             "insert_object" => $insert])) . "');");

    //save button
    $ifc->set(t("command_save"),
              "button",
              "save");

    echo("</div>" .
         "</td>" .
         "</tr>");
    ifc_table_close();

    echo("</td>");
};
echo("</tr>" .
     "</table>");

$ifc->close();