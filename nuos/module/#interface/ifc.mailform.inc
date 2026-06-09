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
define("CMS_MAILFORM_PERMISSION_OPERATOR", "operator");
ifc_permission([""                               => CMS_L_ACCESS,
                CMS_MAILFORM_PERMISSION_OPERATOR => CMS_L_OPERATOR]);

if (! isset($object)) $object = (string)cms_cache("mailform." . CMS_USER . ".object");

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "select":
//..................................................................................................

    $object = $ifc_param;
    break;

case "display":
//..................................................................................................

    if ($object && cms_load("template"))
        template_preview(
            "<?php\n" .
            "\$GLOBALS[\"mailform_form\"] = \"$object\";\n" .
            "cms_application(\"mailform\");\n" .
            "?>",
            FALSE, NULL, FALSE);
    exit();

case "add_receiver":
case "add_element":
//..................................................................................................

    $ifc_param = $object; //target

    switch (CMS_IFC_MESSAGE)
    {
    case "add_receiver":

        $ifc_param1 = CMS_L_IFC_MAILFORM_001; //name
        break;

    case "add_element":

        $ifc_param1 = CMS_L_IFC_MAILFORM_009; //name
        $ifc_param2 = "text";                 //subtype
        break;
    };

case "add_receiver_target":
case "add_element_target":
//..................................................................................................

    $data     = new data("#system/mailform");
    $flexview = new flexview();

    $flexview->import_data($data);

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   [CMS_L_COMMAND_CANCEL . "|mailform/command_cancel" => FALSE],
                   ["object" => $object],
                   NULL,
                   streq(substr(CMS_IFC_MESSAGE, 0, 13), "add_receiver") ?
                   CMS_L_IFC_MAILFORM_010 : CMS_L_IFC_MAILFORM_002);

    ifc_table_open();
    echo("<colgroup>" .
         "<col style=\"WIDTH:0\">" .
         "<col>" .
         "</colgroup>" .

         "<tr>" .

    //data
         "<th>" .
         CMS_L_IFC_MAILFORM_038 .
         "</th>" .

    //target
         "<th>" .
         CMS_L_IFC_MAILFORM_039 .
         "</th>" .
         "</tr>" .
         "<tr>" .

    //data
         "<td>");

    switch (CMS_IFC_MESSAGE)
    {
    case "add_receiver":
    case "add_receiver_target":

        //name
        $ifc->set(CMS_L_NAME, "text 50 256 l",  $ifc_param1);
        break;

    case "add_element":
    case "add_element_target":

        //name
        $ifc->set(CMS_L_NAME, "text 50 256 bl", $ifc_param1);

        //type
        $ifc->set(CMS_L_IFC_MAILFORM_003, "label");
        $ifc->set([CMS_L_IFC_MAILFORM_004 => "checkbox",
                   CMS_L_IFC_MAILFORM_005 => "radio",
                   CMS_L_IFC_MAILFORM_006 => "select",
                   CMS_L_IFC_MAILFORM_007 => "text",
                   CMS_L_IFC_MAILFORM_008 => "textarea",
                   CMS_L_IFC_MAILFORM_047 => "code",
                   CMS_L_IFC_MAILFORM_029 => "hidden",
                   CMS_L_IFC_MAILFORM_033 => "pagebreak"],
                  "select 50",
                  $ifc_param2);
        break;
    };

    echo("</td>" .

    //target
         "<td class=\"target\">");

    $flexview->set_encoding_function(__NAMESPACE__ . "\\qr");

    switch (CMS_IFC_MESSAGE)
    {
    case "add_receiver":
    case "add_receiver_target":

        $flexview->show_target(
            $ifc_param,
            "javascript:ifc_post('add_receiver_target','%index%');",
            "javascript:ifc_post('add_receiver_insert','%index%');",
            "javascript:ifc_post('add_receiver_append','%index%');",
            "name",
            0,
            TRUE,                 //insert type list
            NULL,                 //insert subtype list
            ["base"      => TRUE, //append type list
             "container" => TRUE]);
        break;

    case "add_element":
    case "add_element_target":

        $flexview->show_target(
            $ifc_param,
            "javascript:ifc_post('add_element_target','%index%');",
            "javascript:ifc_post('add_element_insert','%index%');",
            "javascript:ifc_post('add_element_append','%index%');",
            "name",
            0,
            ["container" => TRUE], //insert type list
            NULL,                  //insert subtype list
            ["checkbox"  => TRUE,  //append type list
             "radio"     => TRUE,
             "select"    => TRUE,
             "text"      => TRUE,
             "textarea"  => TRUE,
             "code"      => TRUE,
             "hidden"    => TRUE,
             "pagebreak" => TRUE]);
        break;
    };

    echo("</td>" .
         "</tr>");
    ifc_table_close();

    $ifc->close();

case "add_receiver_insert":
case "add_receiver_append":
//..................................................................................................

    //set name if none is given
    if (! language_get($ifc_param1, FALSE))
        $ifc_param1 = language_set($ifc_param1, CMS_L_IFC_MAILFORM_001, FALSE);

    $data = new data("#system/mailform");
    $data->set_buffer(
        [["name"  => $ifc_param1,
          "#type" => "container"],
         ["#type" => "/container"]]);

    switch (CMS_IFC_MESSAGE)
    {
    case "add_receiver_insert":

        $_object = $data->insert($ifc_param);
        break;

    case "add_receiver_append":

        $_object = $data->append($ifc_param);
        break;
    };

    //create template
    if (cms_load("template"))
    {
        $template           = new template();
        $template->operator = TRUE;
        $template_index     = $template->add(
            language_get($ifc_param1, FALSE), //name
            NULL,                             //category
            NULL,                             //page
            "<CMS:nocache>" .                 //code
            "<?php\n" .
            "\$GLOBALS[\"mailform_form\"] = \"$_object\";\n" .
            "cms_application(\"mailform\");\n" .
            "?>" .
            "</CMS:nocache>");

        if ($template_index !== FALSE)
            $data->set($template_index, $_object, "template");
    };

    if ($data->save())
    {
        $object       = $_object;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    if (! blank($template_index)) $template->delete($template_index);
    $ifc_response = CMS_MSG_ERROR;
    break;

case "add_element_insert":
case "add_element_append":
//..................................................................................................

    //set name in default language if not given
    if (! language_get($ifc_param1, FALSE))
        $ifc_param1 = language_set($ifc_param1, CMS_L_IFC_MAILFORM_009, FALSE);

    //set default values
    switch ($ifc_param2)
    {
    case "code":

        $code = "<input id=\"%id%\"\n" .
                "       name=\"%name%\"\n" .
                "       type=\"text\"\n" .
                "       value=\"%value%\"\n" .
                "       placeholder=\"%label%\">";
        break;

    default:

        $code = NULL;
        break;
    };

    $data = new data("#system/mailform");
    $data->set_buffer(
        [["name"  => $ifc_param1,
          "#type" => $ifc_param2,
          "code"  => $code]]);

    switch (CMS_IFC_MESSAGE)
    {
    case "add_element_insert":

        $_object = $data->insert($ifc_param);
        break;

    case "add_element_append":

        $_object = $data->append($ifc_param);
        break;
    };

    if ($data->save())
    {
        $object       = $_object;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "save":
//..................................................................................................

    //set name in default language if not given
    if (! language_get($ifc_param1, FALSE))
        $ifc_param1 = language_set($ifc_param1, CMS_L_UNKNOWN, FALSE);

    $data = new data("#system/mailform");
    $type = $data->get($object, "#type");

    $data->set($ifc_param1, $object, "name");

    switch ($type)
    {
    case "container":
//..................................................................................................

        $data->set($ifc_param2,        $object, "description");
        $data->set($ifc_param3,        $object, "email");
        $data->set($ifc_param4,        $object, "url");
        $data->set($ifc_param5,        $object, "confirmation");
        $data->set($ifc_param6,        $object, "submit");
        $data->set(isset($ifc_param7), $object, "captcha");

        //update template
        if (
             ($template_index = $data->get($object, "template"))
             &&
             cms_load("template")
           )
        {
            $template           = new template();
            $template->operator = TRUE;

            if ($array = $template->get($template_index))
                $template->set($template_index,                                        //index
                               language_get($ifc_param1, FALSE),                       //name
                               isset($array["category"]) ? $array["category"] : NULL,  //category
                               isset($array["page"])     ? $array["page"]     : NULL); //page
        };

        break;

    case "checkbox":
    case "radio":
//..................................................................................................

        $data->set($ifc_param2,         $object, "id");
        $data->set($ifc_param3,         $object, "description");
        $data->set($ifc_param4,         $object, "option");
        $data->set($ifc_param5,         $object, "column");
        $data->set(isset($ifc_param6),  $object, "required");
        $data->set(isset($ifc_param7),  $object, "secret");
        break;

    case "select":
//..................................................................................................

        $data->set($ifc_param2,         $object, "id");
        $data->set($ifc_param3,         $object, "description");
        $data->set($ifc_param4,         $object, "option");
        $data->set(isset($ifc_param5),  $object, "required");
        $data->set(isset($ifc_param6),  $object, "secret");
        break;

    case "text":
//..................................................................................................

        $data->set($ifc_param2,         $object, "id");
        $data->set($ifc_param3,         $object, "description");
        $data->set((int)$ifc_param4,    $object, "width");
        $data->set((int)$ifc_param5,    $object, "length");
        $data->set($ifc_param6,         $object, "default");
        $data->set(empty($ifc_param9) ? $ifc_param7 : $ifc_param9,
                   $object,
                   "match");
        $data->set(isset($ifc_param8),  $object, "required");
        $data->set(isset($ifc_param10), $object, "confirm");
        $data->set(isset($ifc_param11), $object, "secret");
        break;

    case "textarea":
//..................................................................................................

        $data->set($ifc_param2,         $object, "id");
        $data->set($ifc_param3,         $object, "description");
        $data->set((int)$ifc_param4,    $object, "width");
        $data->set((int)$ifc_param5,    $object, "height");
        $data->set($ifc_param6,         $object, "default");
        $data->set(isset($ifc_param7),  $object, "required");
        $data->set(isset($ifc_param8),  $object, "secret");
        break;

    case "code":
//..................................................................................................

        $data->set($ifc_param2,         $object, "id");
        $data->set($ifc_param3,         $object, "description");
        $data->set($ifc_param4,         $object, "code");
        $data->set(isset($ifc_param5),  $object, "codeonly");
        $data->set($ifc_param6,         $object, "default");
        $data->set(empty($ifc_param9) ? $ifc_param7 : $ifc_param9,
                   $object,
                   "match");
        $data->set(isset($ifc_param8),  $object, "required");
        $data->set(isset($ifc_param10), $object, "secret");
        break;

    case "hidden":
//..................................................................................................

        $data->set($ifc_param2,         $object, "id");
        $data->set($ifc_param3,         $object, "default");
        $data->set(isset($ifc_param4),  $object, "confirm");
        $data->set(isset($ifc_param5),  $object, "secret");
        break;
    };

    if ($data->save())
    {
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "copy_insert":
case "copy_append":
case "cut_insert":
case "cut_append":
//..................................................................................................

    $data      = new data("#system/mailform");
    $_data     = clone $data; //use clone of data structure as source
    $ifc_param = explode(",", $ifc_param);
    $value     = $ifc_param[0];
    $ifc_param = $ifc_param[1];

    //load template extension
    if ($flag_template = cms_load("template"))
    {
        $template           = new template();
        $template->operator = TRUE;
    };

    $source_is_container = $data->is_container($value);
    $target_is_container = $data->is_container($ifc_param);
    $flag_copy           = FALSE;

    //copy / cut
    switch (CMS_IFC_MESSAGE)
    {
    case "copy_insert":

        //containers may not be inserted
        if ($source_is_container) break 2;

    case "copy_append":

        //containers may only be appended to containers
        if ($source_is_container && (! $target_is_container)) break 2;

        //buffer section in clone
        $_data->copy($value);
        $flag_copy = TRUE;
        break;

    case "cut_insert":

        //containers may not be inserted
        if ($source_is_container) break 2;

    case "cut_append":

        //containers may only be appended to containers
        if ($source_is_container && (! $target_is_container)) break 2;

        //target is contained in source
        if ($data->is_child($ifc_param, $value))
        {
            //buffer section in clone
            $_data->copy($value);
            $flag_copy = TRUE;
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

    //create template on copy
    if (
         $source_is_container
         &&
         $flag_template
         &&
         $flag_copy
       )
    {
        $template_index = $template->add(
            $data->get($_object, "name"), //name
            NULL,                         //category
            NULL,                         //base
            "<CMS:nocache>" .             //code
            "<?php\n" .
            "\$GLOBALS[\"mailform_form\"] = \"$_object\";\n" .
            "cms_application(\"mailform\");\n" .
            "?>" .
            "</CMS:nocache>");

        if ($template_index !== FALSE)
            $data->set($template_index, $_object, "template");
    };

    //save changes
    if ($data->save())
    {
        //set target as selection
        switch (CMS_IFC_MESSAGE)
        {
        case "copy_insert":
        case "cut_insert":

            $object = $ifc_param;
            break;

        case "copy_append":
        case "cut_append":

            $object = $data->move("parent", $ifc_param);
            break;
        };

        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "del":
//..................................................................................................

    $data    = new data("#system/mailform");
    $stack   = [];
    $_object = $object;

    //get path to object
    while ($_object = $data->move("parent", $_object))
        array_unshift($stack, $_object);
    array_unshift($stack, 0);

    //delete
    $template = $data->get($ifc_param, "template");
    $data->del($ifc_param);

    if ($data->save())
    {
        //select last existing object in path
        while (! $data->get($object))
        {
            $object = array_pop($stack);
            if ($object === 0) break;
        };

        //delete templates
        if ($template && cms_load("template"))
        {
            $_template           = new template();
            $_template->operator = TRUE;
            $_template->delete($template);
        };

        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "config":
//..................................................................................................

    if (! cms_permission(CMS_MAILFORM_PERMISSION_OPERATOR)) break; //no permission

    //get values
    if (streq(CMS_IFC_MESSAGE, "config"))
    {
        $system     = new system();
        $ifc_param1 = $system->getval("email", "address");
        $ifc_param2 = $system->getval("email", "reply_to");
        $ifc_param3 = $system->getval("email", "method");
        $ifc_param4 = $system->getval("email", "smtp");
        $ifc_param5 = $system->getval("email", "username");
        $ifc_param6 = $system->getval("email", "password");
    };

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object" => $object],
                   "_config",
                   CMS_L_IFC_MAILFORM_040);

    $ifc->set(CMS_L_IFC_MAILFORM_042, "text 40 b",   $ifc_param1 ? $ifc_param1 : CMS_EMAIL_AGENT);
    $ifc->set(CMS_L_IFC_MAILFORM_061, "text 40 b",   $ifc_param2);
    $ifc->set(CMS_L_IFC_MAILFORM_052, "radio b",     "mail", streq($ifc_param3, "mail"));
    $ifc->set(CMS_L_IFC_MAILFORM_054, "title");
    $ifc->set(CMS_L_IFC_MAILFORM_053, "radio b",     NULL,   stre($ifc_param3));
    $ifc->set(CMS_L_IFC_MAILFORM_041, "text 40 b",   $ifc_param4);
    $ifc->set(CMS_L_IFC_MAILFORM_043, "text 40 b",   $ifc_param5);
    $ifc->set(CMS_L_IFC_MAILFORM_044, "password 40", $ifc_param6);

    $ifc->close();

case "_config":
//..................................................................................................

    if (! cms_permission(CMS_MAILFORM_PERMISSION_OPERATOR)) break; //no permission

    //set values
    $system = new system();
    $system->setval($ifc_param1, "email", "address");
    $system->setval($ifc_param2, "email", "reply_to");
    $system->setval($ifc_param3, "email", "method");
    $system->setval($ifc_param4, "email", "smtp");
    $system->setval($ifc_param5, "email", "username");
    $system->setval($ifc_param6, "email", "password");

    $ifc_response = $system->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "test":
//..................................................................................................

    if (! cms_permission(CMS_MAILFORM_PERMISSION_OPERATOR)) break; //no permission

    $ifc_response = CMS_MSG_ERROR;

    if (! cms_load("smtp")) break;

    $smtp = new smtp();

    if (! $smtp->enabled)
    {
        $ifc_response = CMS_MSG_ERROR . x($smtp->response);
    }

    elseif (cms_load("mime"))
    {
        //generate test message
        $mime = new mime();
        $mime->add_text(CMS_L_IFC_MAILFORM_055,  //subject
                        CMS_L_IFC_MAILFORM_060); //body

        //get receiver
        $reply_to = (new system())->getval("email", "reply_to");
        $to       = nstre($reply_to) ? $reply_to : CMS_EMAIL_AGENT;

        //send test message
        $ifc_response =
            $smtp->send(
                $mime,           //mime
                CMS_EMAIL_AGENT, //from
                $to,             //to
                NULL,            //cc
                NULL,            //bcc
                $reply_to) ?     //reply_to
            CMS_MSG_DONE :
            CMS_MSG_ERROR . x($smtp->response);

        $smtp->quit();
    };

    break;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

//cache selected object permanently
if (CMS_IFC_MESSAGE !== "") cms_cache("mailform." . CMS_USER . ".object", $object, TRUE);

$data           = new data("#system/mailform");
$type           = $data->get($object, "#type");
$flag_pagebreak = nstreq($type, "pagebreak");
$flexview       = new flexview();

$flexview->import_data($data);

//menu
$menu = [CMS_L_IFC_MAILFORM_010 . "|mailform/command_add_form"        => "add_receiver",
         CMS_L_IFC_MAILFORM_002 . "|mailform/command_add_element"     => "add_element"];
if (cms_permission(CMS_MAILFORM_PERMISSION_OPERATOR))
{
    $menu[CMS_L_IFC_MAILFORM_040 . "|mailform/command_configuration"] =  "config";
    $menu[CMS_L_IFC_MAILFORM_045 . "|mailform/command_test"]          =  "test";
};

//display
$ifc = new ifc($ifc_response,
               $ifc_page,
               $menu,
               ["object" => $object]);

echo("<table class=\"layout\">" .

     "<colgroup>" .
     "<col>" .
     (($object && $flag_pagebreak) ? "<col style=\"WIDTH:0\">" : "") .
     "</colgroup>" .

     "<tr>" .
     "<td>");

ifc_table_open();

//header
echo("<tr>" .
     "<th>" .
     CMS_L_IFC_MAILFORM .
     "</th>" .
     "</tr>" .

//folder
     "<tr>" .
     "<td>");

?><script>

function mailform_flexview_event(event,
                                 source,
                                 target)
{
    var source_id = "";
    var target_id = "";
    var action    = "";

    if (source !== null)
        source_id = source.getAttribute("data-fv-hir-index");

    if (target !== null)
    {
        target_id = target.getAttribute("data-fv-hir-index");
        action    = target.classList.contains("fv-hir-source") ? "insert" : "append";
    };

    switch (event)
    {
    case "dropon":

        if (target.id === "trashbin")     ifc_post("del", source_id);
        else if (source_id !== target_id) ifc_post("cut_" + action, source_id + "," + target_id);
        break;

    case "dropon_alt":

        ifc_post("copy_" + action, source_id + "," + target_id);
        break;
    };
};

</script><?php

$flexview->set_encoding_function(__NAMESPACE__ . "\\qr");
$flexview->show_hierarchy(
    $object,
    "javascript:ifc_post('select','%index%');",
    "name",
    NULL,
    ["container" => "mailform/icon_form",
     "checkbox"  => "mailform/icon_checkbox",
     "radio"     => "mailform/icon_radio",
     "select"    => "mailform/icon_select",
     "text"      => "mailform/icon_text",
     "textarea"  => "mailform/icon_textarea",
     "code"      => "mailform/icon_code",
     "hidden"    => "mailform/icon_hidden",
     "pagebreak" => "mailform/icon_pagebreak"],
    "",
    "mailform_flexview_event",
    ["base"      => ["index"  => 512, "insert" =>   1, "append" =>   1],
     "container" => ["index"  =>   1, "insert" => 510, "append" =>   1],
     "checkbox"  => ["index"  =>   2, "insert" =>   0, "append" => 510],
     "radio"     => ["index"  =>   4, "insert" =>   0, "append" => 510],
     "select"    => ["index"  =>   8, "insert" =>   0, "append" => 510],
     "text"      => ["index"  =>  16, "insert" =>   0, "append" => 510],
     "textarea"  => ["index"  =>  32, "insert" =>   0, "append" => 510],
     "code"      => ["index"  =>  64, "insert" =>   0, "append" => 510],
     "hidden"    => ["index"  => 128, "insert" =>   0, "append" => 510],
     "pagebreak" => ["index"  => 256, "insert" =>   0, "append" => 510]]);

echo("</td>" .
     "</tr>" .

//trash bin
     "<tr>" .
     "<td id=\"trashbin\" " .
         "title=\"" . CMS_L_IFC_MAILFORM_059 . "\" " .
         "class=\"varied\">" .
     image("mailform/command_delete") . " " . CMS_L_COMMAND_DELETE .
     jscript("dd_register(document.getElementById(\"trashbin\"), 0, 511, true);") .
     "</td>" .
     "</tr>");
ifc_table_close();

echo("</td>");

//data
if ($object && $flag_pagebreak)
{
    //data
    echo("<td>");

    ifc_table_open();
    echo("<tr>" .
         "<th>");

    //field type
    $array = ["checkbox"  => CMS_L_IFC_MAILFORM_004,
              "radio"     => CMS_L_IFC_MAILFORM_005,
              "select"    => CMS_L_IFC_MAILFORM_006,
              "text"      => CMS_L_IFC_MAILFORM_007,
              "textarea"  => CMS_L_IFC_MAILFORM_008,
              "code"      => CMS_L_IFC_MAILFORM_047,
              "hidden"    => CMS_L_IFC_MAILFORM_029,
              "pagebreak" => CMS_L_IFC_MAILFORM_033];
    echo((isset($array[$type]) ? $array[$type] : CMS_L_IFC_MAILFORM_012) .

         "</th>" .
         "</tr>" .
         "<tr>" .
         "<td>");

    //get active form object
    $_object = $object;

    while (! $data->is_container($_object))
    {
        $_object = $data->move("parent", $_object);
        if (! $_object) break;
    };

    //name
    $ifc->set(CMS_L_NAME, "text 50 256 bl", $data->get($object, "name"));

    echo("<div style=\"TEXT-ALIGN:right\">");

    //get active form object
    $_object = $object;

    while (! $data->is_container($_object))
    {
        $_object = $data->move("parent", $_object);
        if (! $_object) break;
    };

    //display button
    if ($_object)
        $ifc->set(CMS_L_COMMAND_SHOW,
                  "button",
                  "javascript:load_page('" .
                  q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                             "ifc_message" => "display",
                             "ifc_option"  => "external",
                             "object"      => $_object])) . "');");

    //save button
    $ifc->set(CMS_L_COMMAND_SAVE, "button", "save");
    echo("</div>");

    //field id
    if (nstreq($type, "container"))
        $ifc->set(CMS_L_IFC_MAILFORM_030, "text 50 256 b",         $data->get($object, "id"));

    switch ($type)
    {
    case "container":
//..................................................................................................

        //description
        $ifc->set(CMS_L_IFC_MAILFORM_025, "texteditor 50x8 bilrt", $data->get($object, "description"));

        //receiver address
        $ifc->set(CMS_L_IFC_MAILFORM_013, "text 50 256 bl",        $data->get($object, "email"));

        //target address
        $ifc->set(CMS_L_IFC_MAILFORM_027, "text 50 256 bl",        $data->get($object, "url"));

        //confirmation text
        $ifc->set(CMS_L_IFC_MAILFORM_028, "info b");
        $ifc->set(CMS_L_IFC_MAILFORM_026, "texteditor 50x8 bilrt", $data->get($object, "confirmation"));

        //submit button
        $ifc->set(CMS_L_IFC_MAILFORM_050, "text 50 256 bl",        $data->get($object, "submit"));

        //captcha
        $ifc->set(CMS_L_IFC_MAILFORM_046, "checkbox b", TRUE,      $data->get($object, "captcha"));
        break;

    case "checkbox":
    case "radio":
//..................................................................................................

        //description
        $ifc->set(CMS_L_IFC_MAILFORM_025, "texteditor 50x8 bilrt", $data->get($object, "description"));

        //option list
        $ifc->set(CMS_L_IFC_MAILFORM_014, "textarea 50x16 bl",     $data->get($object, "option"));
        $ifc->set(CMS_L_IFC_MAILFORM_015, "info b");
        $ifc->set(CMS_L_IFC_MAILFORM_034, "label");
        $ifc->set([1 => 1,
                   2 => 2,
                   3 => 3,
                   4 => 4],
                  "select :",
                  $data->get($object, "column"));

        //required flag
        $ifc->set(CMS_L_IFC_MAILFORM_023, "checkbox b", TRUE,      $data->get($object, "required"));

        //do not send data in confirmation flag
        $ifc->set(CMS_L_IFC_MAILFORM_056, "checkbox", TRUE,        $data->get($object, "secret"));
        break;

    case "select":
//..................................................................................................

        //description
        $ifc->set(CMS_L_IFC_MAILFORM_025, "texteditor 50x8 bilrt", $data->get($object, "description"));

        //option list
        $ifc->set(CMS_L_IFC_MAILFORM_014, "textarea 50x16 bl",     $data->get($object, "option"));
        $ifc->set(CMS_L_IFC_MAILFORM_015, "info b");

        //required flag
        $ifc->set(CMS_L_IFC_MAILFORM_023, "checkbox b", TRUE,      $data->get($object, "required"));

        //do not send data in confirmation flag
        $ifc->set(CMS_L_IFC_MAILFORM_056, "checkbox", TRUE,        $data->get($object, "secret"));
        break;

    case "text":
//..................................................................................................

        //description
        $ifc->set(CMS_L_IFC_MAILFORM_025, "texteditor 50x8 bilrt", $data->get($object, "description"));

        //field width in characters
        $ifc->set(CMS_L_IFC_MAILFORM_016, "text 3 3 :",            $data->get($object, "width"));

        //maximum text length
        $ifc->set(CMS_L_IFC_MAILFORM_017, "text 3 3 :b",           $data->get($object, "length"));

        //default value
        $ifc->set(CMS_L_IFC_MAILFORM_018, "text 50 256 bl",        $data->get($object, "default"));

        //prefab verifications
        $array = [" "                    => "",
                  CMS_L_IFC_MAILFORM_020 => "/^[\p{L}\p{P}\p{Zs}]*$/u",                   //name
                  CMS_L_IFC_MAILFORM_058 => "/^[\p{L}\p{P}\p{Zs}\p{Nd}]*$/u",             //street
                  CMS_L_IFC_MAILFORM_022 => "/^[ \/\(\)0-9\+\-]*$/",                      //phone
                  CMS_L_IFC_MAILFORM_021 => "/^.+@.+\..+$/",                              //email
                  CMS_L_IFC_MAILFORM_057 => "/^(?:[!#$&-;=?-[\]_a-z~]|%[0-9a-f]{2})*$/i", //url
                  CMS_L_IFC_MAILFORM_031 => "/^\p{Nd}*$/u"];                              //number
        $match = $data->get($object, "match");
        $flag  = in_array($match, $array);

        $ifc->set(CMS_L_IFC_MAILFORM_019, "label");
        $ifc->set($array,                 "select",                $flag ? $match : NULL);

        //required flag
        $ifc->set(CMS_L_IFC_MAILFORM_023, "checkbox b", TRUE,      $data->get($object, "required"));

        //custom verification
        $ifc->set(CMS_L_IFC_MAILFORM_035, "text 50 b",             $flag ? NULL   : $match);

        //send confirmation flag, interprets field as email address
        $ifc->set(CMS_L_IFC_MAILFORM_032, "checkbox b", TRUE,      $data->get($object, "confirm"));

        //do not send data in confirmation flag
        $ifc->set(CMS_L_IFC_MAILFORM_056, "checkbox", TRUE,        $data->get($object, "secret"));
        break;

    case "textarea":
//..................................................................................................

        //description
        $ifc->set(CMS_L_IFC_MAILFORM_025, "texteditor 50x8 bilrt", $data->get($object, "description"));

        //field width in characters
        $ifc->set(CMS_L_IFC_MAILFORM_016, "text 3 3 :",            $data->get($object, "width"));

        //field height in characters
        $ifc->set(CMS_L_IFC_MAILFORM_024, "text 3 3 :b",           $data->get($object, "height"));

        //default value
        $ifc->set(CMS_L_IFC_MAILFORM_018, "textarea 50x8 bl",      $data->get($object, "default"));

        //required flag
        $ifc->set(CMS_L_IFC_MAILFORM_023, "checkbox b", TRUE,      $data->get($object, "required"));

        //do not send data in confirmation flag
        $ifc->set(CMS_L_IFC_MAILFORM_056, "checkbox", TRUE,        $data->get($object, "secret"));
        break;

    case "code":
//..................................................................................................

        //description
        $ifc->set(CMS_L_IFC_MAILFORM_025, "texteditor 50x8 bilrt", $data->get($object, "description"));

        //code
        $ifc->set(CMS_L_IFC_MAILFORM_048, "textarea 50x16 bl",     $data->get($object, "code"));
        $ifc->set(CMS_L_IFC_MAILFORM_049, "info b");

        //hidden flag
        $ifc->set(CMS_L_IFC_MAILFORM_051, "checkbox b", TRUE,      $data->get($object, "codeonly"));

        //default value
        $ifc->set(CMS_L_IFC_MAILFORM_018, "textarea 50x8 bl",      $data->get($object, "default"));

        //prefab verifications
        $array = [" "                    => "",
                  CMS_L_IFC_MAILFORM_020 => "/^[\p{L}\p{P}\p{Zs}]*$/u",                   //name
                  CMS_L_IFC_MAILFORM_058 => "/^[\p{L}\p{P}\p{Zs}\p{Nd}]*$/u",             //street
                  CMS_L_IFC_MAILFORM_022 => "/^[ \/\(\)0-9\+\-]*$/",                      //phone
                  CMS_L_IFC_MAILFORM_021 => "/^.+@.+\..+$/",                              //email
                  CMS_L_IFC_MAILFORM_057 => "/^(?:[!#$&-;=?-[\]_a-z~]|%[0-9a-f]{2})*$/i", //url
                  CMS_L_IFC_MAILFORM_031 => "/^\p{Nd}*$/u"];                              //number
        $match = $data->get($object, "match");
        $flag  = in_array($match, $array);

        $ifc->set(CMS_L_IFC_MAILFORM_019, "label");
        $ifc->set($array,                 "select",                $flag ? $match : NULL);

        //required flag
        $ifc->set(CMS_L_IFC_MAILFORM_023, "checkbox b", TRUE,      $data->get($object, "required"));

        //custom verification
        $ifc->set(CMS_L_IFC_MAILFORM_035, "text 50 b",             $flag ? NULL   : $match);

        //do not send data in confirmation flag
        $ifc->set(CMS_L_IFC_MAILFORM_056, "checkbox", TRUE,        $data->get($object, "secret"));
        break;

    case "hidden":
//..................................................................................................

        //default value
        $ifc->set(CMS_L_IFC_MAILFORM_018, "textarea 50x8 b",       $data->get($object, "default"));

        //send confirmation flag, interprets field as email address
        $ifc->set(CMS_L_IFC_MAILFORM_032, "checkbox b", TRUE,      $data->get($object, "confirm"));

        //do not send data in confirmation flag
        $ifc->set(CMS_L_IFC_MAILFORM_056, "checkbox", TRUE,        $data->get($object, "secret"));
        break;
    };

    echo("</td>" .
         "</tr>");
    ifc_table_close();

    echo("</td>");
};
echo("</tr>");
ifc_table_close();

$ifc->close();