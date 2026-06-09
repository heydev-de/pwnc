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

if (blank($file) || (! is_file($file))) $file = CMS_DATA_PATH . "#system/system.dat";

$data = new data($file);

if (blank($object) || (! $data->has($object))) $object = 0;
init($list, []);
init($target);
init($status);

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "select":
//..................................................................................................

    $object = $ifc_param;
    $list   = [$object];
    $status = NULL;
    break;

case "save":
//..................................................................................................

    //add attribute
    if ($ifc_param1) $data->set($ifc_param2, $object, $ifc_param1);

    //change attributes
    $i = 3;
    while (isset(${"attribute$i"}))
    {
        $data->set(${"ifc_param$i"}, $object, ${"attribute$i"});
        $i++;
    };

    //save changes
    if ($data->save())
    {
        //remove entry if no attributes are left
        if (! $data->has($object))
        {
            $_object = $data->move("parent", $object);
            $data->del($object);
            $object = ($_object === FALSE) ? 0 : $_object;
        };

        $list         = [$object];
        $status       = NULL;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "add":
//..................................................................................................

    $ifc_param  = $object; //target
    $ifc_param1 = NULL;    //key
    $ifc_param2 = NULL;    //container flag

case "add_target":
//..................................................................................................

    //add name property
    while ($key = $data->move("next"))
    {
        if (! $data->get($key, "name"))
            $data->set($key, $key, "name");
    };

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   [CMS_L_COMMAND_CANCEL . "|system/command_cancel" => FALSE],
                   ["object" => $object,
                    "list"   => $list],
                   NULL,
                   CMS_L_COMMAND_ADD);

    ifc_table_open();
    echo("<colgroup>" .
         "<col style=\"WIDTH:0\">" .
         "<col>" .
         "</colgroup>" .

         "<tr>" .

    //data
         "<th>" .
         CMS_L_IFC_SYSTEM_006 .
         "</th>" .

    //target
         "<th>" .
         CMS_L_IFC_SYSTEM_008 .
         "</th>" .
         "</tr>" .
         "<tr>" .

    //data
         "<td>");

    //key
    $ifc->set(CMS_L_IFC_SYSTEM_009, "text 40 b",      $ifc_param1);

    //container flag
    $ifc->set(CMS_L_IFC_SYSTEM_002, "checkbox", TRUE, isset($ifc_param2));
    echo("</td>" .

    //target
         "<td class=\"target\">");
    $flexview = new flexview();
    $flexview->import_data($data);
    $flexview->set_encoding_function(__NAMESPACE__ . "\\qr");
    $flexview->show_target(
        $ifc_param,
        "javascript:ifc_post('add_target','%index%');",
        "javascript:ifc_post('add_insert','%index%');",
        "javascript:ifc_post('add_append','%index%');");
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    $ifc->close();

case "add_insert":
case "add_append":
//..................................................................................................

    //set key if none is given
    if (stre($ifc_param1)) $ifc_param1 = unique_id();

    $data->buffer = empty($ifc_param2) ?

    //no container
                    [$ifc_param1    => ["name" => $ifc_param1]] :

    //container
                    [$ifc_param1    => ["name"  => $ifc_param1,
                                        "#type" => "container"],
                     "/$ifc_param1" => ["#type" => "/container"]];

    $ifc_param1   = $data->insert($ifc_param);

    //save changes
    if ($data->save())
    {
        $object       = $ifc_param1;
        $list         = [$object];
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "copy":
case "cut":
//..................................................................................................

    $target = $object;
    $status = CMS_IFC_MESSAGE;
    break;

case "target":
//..................................................................................................

    $target = $ifc_param;
    break;

case "copy_insert":
case "copy_append":
case "cut_insert":
case "cut_append":
//..................................................................................................

    if (empty($list)) break; //no selection;

    $_data = clone $data; //use clone of data structure as source
    $_list = NULL;

    //transverse reversely
    $list  = array_reverse($list);

    foreach ($list AS $value)
    {
        //copy / cut
        switch (CMS_IFC_MESSAGE)
        {
        case "copy_insert":
        case "copy_append":

            //buffer section in clone
            $_data->copy($value);
            break;

        case "cut_insert":
        case "cut_append":

            //target is contained in source
            if ($data->is_child($ifc_param, $value))
            {
                //buffer section in clone
                $_data->copy($value);
            }
            else
            {
                //remove section in master
                $data->del($value);

                //buffer section in clone
                $_data->cut($value);
            };
            break;
        };

        //transmit buffer from clone to master
        $data->buffer = $_data->buffer;

        //paste
        switch (CMS_IFC_MESSAGE)
        {
        case "copy_insert":
        case "cut_insert":

            //insert buffer in master
            $_object = $data->insert($ifc_param);
            break;

        case "copy_append":
        case "cut_append":

            //append buffer in master
            $_object = $data->append($ifc_param);
            break;
        };
    };

    //save changes
    if ($data->save())
    {
        $object       = $_object;
        $list         = [$object];
        $status       = NULL;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "delete":
//..................................................................................................

    if (empty($list)) break; //no selection;

    $stack   = [];
    $_object = $object;

    //get path to object
    while ($_object = $data->move("parent", $_object))
        array_unshift($stack, $_object);
    array_unshift($stack, 0);

    //delete
    foreach ($list AS $value) $data->del($value);

    //save changes
    if ($data->save())
    {
        //select last existing object in path
        while (! $data->get($object))
        {
            $object = array_pop($stack);
            if ($object === 0) break;
        };

        $list         = [$object];
        $status       = NULL;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "filemanager":
//..................................................................................................

    if (cms_load("plist"))
    {
        //add access
        $plist = new plist("#system/filemanager.recent");
        $plist->add(substr($file, strlen(CMS_ROOT_PATH)), TRUE);
        $plist->remove(10, 0);
    };
    break;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

//menu
$menu = [CMS_L_COMMAND_ADD .             "|system/command_add"    => "add",
         CMS_L_COMMAND_COPY .            "|system/command_copy"   => "copy",
         CMS_L_COMMAND_CUT .             "|system/command_cut"    => "cut",
         CMS_L_COMMAND_DELETE_SELECTED . "|system/command_delete" => "#delete"];

//add name property
$data->move("first");
while ($key = $data->move("next"))
{
    if (! $data->get($key, "name"))
        $data->set($key, $key, "name");
};

//temporary caching
cms_cache("data." . $data->file, $data->data);

//display
$ifc = new ifc($ifc_response,
               $ifc_page,
               $menu,
               ["object" => $object,
                "status" => $status,
                "file"   => $file]);

echo("<table class=\"layout\">" .

     "<colgroup>" .
     "<col>" .
     ($status ? "<col style=\"WIDTH:200px\">" : "") .
     ($object ? "<col style=\"WIDTH:0\">"     : "") .
     "</colgroup>" .

     "<tr>" .
     "<td>");

ifc_table_open();

//header
echo("<tr>" .
     "<th>" .
     x($file) .
     "</th>" .
     "</tr>" .

//treeview
     "<tr>" .
     "<td>");
$flexview = new flexview();
$flexview->import_data($data);
$flexview->set_encoding_function(__NAMESPACE__ . "\\qr");
$flexview->set_checkbox_identifier("list");
$flexview->set_checkbox_list($list);
$flexview->show_tree(
    $object,                                         //index
    "javascript:ifc_post('select','%index%');",      //action
    "name",                                          //name key
    NULL,                                            //mark
    ["container"  => "flexview/icon_entrycontainer", //icon
     "+container" => "flexview/icon_entryopen"]);
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

if ($status)
{
    //target
    echo("<td>");

    ifc_table_open();
    echo("<tr>" .
         "<th>" .
         (streq($status, "copy") ? CMS_L_COMMAND_COPY : CMS_L_COMMAND_CUT) .
         "</th>" .
         "</tr>" .
         "<tr>" .
         "<td class=\"target\">");
    $status = qr($status);
    $flexview->show_target(
        $target,
        "javascript:ifc_post('target','%index%');",
        "javascript:ifc_post('" . $status . "_insert','%index%');",
        "javascript:ifc_post('" . $status . "_append','%index%');");
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    echo("</td>");
};

//data
if ($object)
{
    //data
    echo("<td>");

    ifc_table_open();
    echo("<tr>" .
         "<th>" .
         x($object) .
         "</th>" .
         "</tr>" .
         "<tr>" .
         "<td>");

    //new attribute name
    $ifc->set(CMS_L_IFC_SYSTEM_001, "text 50 b");

    //new attribute value
    $ifc->set(CMS_L_IFC_SYSTEM_007, "textarea 50x4 bl");

    //apply button
    echo("<div style=\"TEXT-ALIGN:right\">");
    $ifc->set(CMS_L_COMMAND_SAVE,   "button", "save");
    echo("</div>");

    //display attributes
    $array = $data->get($object);
    $i     = 3;

    //order by attribute name
    ksort($array, SORT_NATURAL | SORT_FLAG_CASE);

    foreach ($array AS $key => $value)
    {
        //container
        if (streq($key, "#type")) continue;

        //attribute name
        $ifc->param("attribute" . $i++, $key);

        //attribute value
        $ifc->set($key, "textarea 50x4 bl", $value);
    };
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    echo("</td>");
};
echo("</tr>" .
     "</table>");

$ifc->close();