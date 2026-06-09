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
ifc_permission(["" => CMS_L_ACCESS]);

cms_cache_init($object, "insert." . CMS_USER . ".object");
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
        $data->set_buffer([["name" => CMS_L_IFC_INSERT_002]]);
        $_insert_object = $data->append();

        if ($data->save())
        {
            $insert_object = $_insert_object;
            $ifc_response  = CMS_MSG_DONE;
            break;
        };

        $ifc_response = CMS_MSG_ERROR;
        break;

    case "insert_save":
//..................................................................................................

        //keep name if none is given
        if (stre($ifc_param1 = utf8_trim($ifc_param1)))
            $ifc_param1 = $data->get($insert_object, "name");

        $data->set(["name" => $ifc_param1,
                    "html" => isset($ifc_param2),
                    "code" => $ifc_param3],
                   $insert_object);

        $ifc_response = $data->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
        break;

    case "insert_delete":
//..................................................................................................

        if (empty($_list)) break; //no selection

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

            $ifc_response = CMS_MSG_DONE;
            break;
        };

        $ifc_response = CMS_MSG_ERROR;
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
    $menu = [CMS_L_COMMAND_ADD .             "|insert/command_add"    => "insert_add",
             CMS_L_COMMAND_DELETE_SELECTED . "|insert/command_delete" => "#insert_delete",
             CMS_L_COMMAND_CLOSE .           "|insert/command_cancel" => FALSE];

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   $menu,
                   ["object"        => $object,
                    "list"          => $list,
                    "insert_object" => $insert_object],
                   NULL,
                   CMS_L_IFC_INSERT_004);

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
         CMS_L_IFC_INSERT_004 .
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
    $ifc->set(CMS_L_ALL,    "button", "javascript:ifc_list_activate(\"_list\");");
    $ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert(\"_list\");");
    $ifc->set(CMS_L_NONE,   "button", "javascript:ifc_list_deactivate(\"_list\");");
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
             CMS_L_IFC_INSERT_005 .
             "</th>" .
             "</tr>" .

        //settings
             "<tr>" .
             "<td>");

        //name
        $ifc->set(CMS_L_NAME,           "text 40 80",             $data->get($insert_object, "name"));

        //html
        $ifc->set(CMS_L_IFC_INSERT_006, "checkbox b", TRUE,       $data->get($insert_object, "html"));

        //code
        $ifc->set(CMS_L_IFC_INSERT_007, "texteditor 60x15 bilrt", $data->get($insert_object, "code"));

        echo("<div style=\"TEXT-ALIGN:right\">");

        //display button
        $ifc->set(CMS_L_COMMAND_SHOW,
                  "button",
                  "javascript:load_page('" .
                  q(cms_url(["ifc_page"      => CMS_IFC_PAGE,
                             "ifc_message"   => "insert_display",
                             "insert_object" => $insert_object])) . "');");

        //save button
        $ifc->set(CMS_L_COMMAND_SAVE,
                  "button",
                  "insert_save");

        echo("</div>" .
             "</td>" .
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
            $ifc_response = CMS_MSG_DONE;
            break;
        };
    };

    $ifc_response = CMS_MSG_ERROR;

case "add":
//..................................................................................................

    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object" => $object,
                    "list"   => $list],
                   "_add",
                   CMS_L_COMMAND_ADD);

    $ifc->set(CMS_L_NAME, "text 40 256 b");

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
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "save":
//..................................................................................................

    $data = new data("#system/insert");

    $data->set($insert_object, $object, "insert");

    $ifc_response = $data->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
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
$menu = [CMS_L_COMMAND_ADD .             "|insert/command_add"    => "add",
         CMS_L_IFC_INSERT_004 .          "|insert/command_code"   => "insert",
         CMS_L_COMMAND_DELETE_SELECTED . "|insert/command_delete" => "#delete"];

//display
$ifc = new ifc($ifc_response,
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
     CMS_L_IFC_INSERT_008 .
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
$ifc->set(CMS_L_ALL,    "button", "javascript:ifc_list_activate();");
$ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert();");
$ifc->set(CMS_L_NONE,   "button", "javascript:ifc_list_deactivate();");
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
         CMS_L_IFC_INSERT_005 .
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

    $ifc->set(CMS_L_IFC_INSERT_003, "label");
    $ifc->set($array,               "select 40 b", $insert, NULL, "insert_object");

    echo("<div style=\"TEXT-ALIGN:right\">");

    //display button
    if ($insert)
        $ifc->set(CMS_L_COMMAND_SHOW,
                  "button",
                  "javascript:load_page('" .
                  q(cms_url(["ifc_page"      => CMS_IFC_PAGE,
                             "ifc_message"   => "insert_display",
                             "insert_object" => $insert])) . "');");

    //save button
    $ifc->set(CMS_L_COMMAND_SAVE,
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