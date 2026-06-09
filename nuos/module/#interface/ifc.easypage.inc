<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   MODULE
//==================================================================================================

//load libraries
if (! cms_load("content"))   ifc_inactive($ifc_page);
if (! cms_load("directory")) ifc_inactive($ifc_page);
if (! cms_load("flexview"))  ifc_inactive($ifc_page);
if (! cms_load("template"))  ifc_inactive($ifc_page);

//permission
ifc_permission(["" => CMS_L_ACCESS]);

$content = new content();
if (! $content->enabled) ifc_inactive($ifc_page);

if (! isset($object)) $object = cms_cache("directory." . CMS_USER . ".object");

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "select":
//..................................................................................................

    $object = $ifc_param;
    break;

case "add":
//..................................................................................................

    $ifc_param  = $object;
    $ifc_param1 = CMS_L_IFC_EASYPAGE_007;
    $ifc_param2 = cms_cache("directory." . CMS_USER . ".hidden") ? TRUE : NULL;
    $ifc_param3 = (string)cms_cache("template." . CMS_USER . ".page");

case "add_target":
//..................................................................................................

    $data     = new data("#system/directory");
    $flexview = new flexview();

    $flexview->import_data($data);

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   [CMS_L_COMMAND_CANCEL . "|easypage/command_cancel" => FALSE],
                   ["object" => $object],
                   NULL,
                   CMS_L_COMMAND_ADD);

?><script>

function easypage_template_preview()
{
    var value = ifc_get("ifc_param3");
    if (value) load_page("<?php
        echo(str_replace("%replace%",
                         "\"+encodeURIComponent(value)+\"",
                         q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "template_preview",
                                    "object"      => "\x1B%replace%"],
                                   TRUE))));?>");
};

</script><?php

    ifc_table_open();
    echo("<colgroup>" .
         "<col style=\"WIDTH:0\">" .
         "<col>" .
         "</colgroup>" .

         "<tr>" .

    //data
         "<th>" .
         CMS_L_IFC_EASYPAGE_003 .
         "</th>" .

    //target
         "<th>" .
         CMS_L_IFC_EASYPAGE_006 .
         "</th>" .
         "</tr>" .
         "<tr>" .

    //data
         "<td>");
    $ifc->set(CMS_L_IFC_EASYPAGE_022,    "info b");

    //title
    $ifc->set(CMS_L_IFC_EASYPAGE_009,    "text 50 256 bl",   $ifc_param1);

    //hidden
    $ifc->set(CMS_L_IFC_EASYPAGE_011,    "checkbox b", TRUE, isset($ifc_param2));

    //template
    $ifc->set(CMS_L_IFC_EASYPAGE_015,    "label");
    $ifc->set(content_template_select(), "select 40",        $ifc_param3);
    $ifc->set(CMS_L_COMMAND_SHOW,        "button",           "javascript:easypage_template_preview();");
    echo("</td>" .

    //target
         "<td class=\"target\">");
    $ifc->set(CMS_L_IFC_EASYPAGE_021,    "info b");
    $flexview->set_encoding_function(__NAMESPACE__ . "\\qr");
    $flexview->set_display_function(__NAMESPACE__ . "\\directory_flexview_display_function");
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

    //set name if none is given
    if (! language_get($ifc_param1, FALSE))
        $ifc_param1 = language_set($ifc_param1, CMS_L_UNKNOWN, FALSE);

    $content->writer = TRUE; //temporary permission

    //create
    if ($index = $content->create($ifc_param1, $ifc_param3))
    {
        //temporary permission
        $content->editor    = TRUE;
        $content->publisher = TRUE;

        //publish, implies authorisation
        if (($index = $content->publish($index,                     //index
                                        NULL,                       //comment
                                        0,                          //time_publish
                                        0,                          //time_withdraw
                                        $ifc_param,                 //directory_index
                                        substr(CMS_IFC_MESSAGE, 4), //directory_action
                                        $ifc_param1))               //directory_title
             !== FALSE)
        {
            //link publication
            $object    = $index;
            $directory = new directory();

            $directory->data->set(isset($ifc_param2), $object, "hidden");
            $directory->data->del($object, "placeholder");

            if ($directory->data->save())
            {
                cms_cache("directory." . CMS_USER . ".hidden", isset($ifc_param2), TRUE);
                cms_cache("template." . CMS_USER . ".page",    $ifc_param3, TRUE);
                $ifc_response = CMS_MSG_DONE;
                break;
            };
        };
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "save":
//..................................................................................................

    $directory = new directory();

    $directory->data->set($ifc_param1,        $object, "name");
    $directory->data->set(isset($ifc_param2), $object, "hidden");

    if ($directory->save())
    {
        //set name if none is given
        if (! language_get($ifc_param1, FALSE))
            $ifc_param1 = language_set($ifc_param1, CMS_L_UNKNOWN, FALSE);

        $url = $directory->data->get($object, "url");

        //entry links content
        if (streq(substr($url, 0, 10), "content://"))
        {
            //get content index
            $index      = substr($url, 10);
            $index      = substr($index, 0, strspn($index, "1234567890"));
            $index      = sqlesc($index);
            $ifc_param3 = sqlesc($ifc_param3);
            $ifc_param4 = sqlesc($ifc_param4);
            $ifc_param5 = sqlesc($ifc_param5);

            //update linked content
            mysql_query("UPDATE " . CMS_DB_CONTENT . " " .
                        "SET " .    CMS_DB_CONTENT_TITLE .              "='$ifc_param3', " .
                                    CMS_DB_CONTENT_DESCRIPTION .        "='$ifc_param4', " .
                                    CMS_DB_CONTENT_KEYWORD .            "='$ifc_param5', " .
                                    CMS_DB_CONTENT_BUFFER_TITLE .       "='$ifc_param3', " .
                                    CMS_DB_CONTENT_BUFFER_DESCRIPTION . "='$ifc_param4', " .
                                    CMS_DB_CONTENT_BUFFER_KEYWORD .     "='$ifc_param5' " .
                        "WHERE " .  CMS_DB_CONTENT_INDEX .              "='$index' " .
                        "LIMIT 1");
        };

        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "delete":
//..................................................................................................

    $directory      = new directory();
    $stack          = [];
    $_object        = $object;
    $array_document = NULL;
    $array_schedule = NULL;
    $depth          = 0;
    $error          = TRUE;

    //get path to object
    while ($_object = $directory->data->move("parent", $_object))
        array_unshift($stack, $_object);
    array_unshift($stack, 0);

    //collect linked content and schedules in directory branch
    $key = $directory->data->move("to", $ifc_param);
    while ($key)
    {
        $type = $directory->data->get($key, "#type");
        switch ($type)
        {
        case "container":

            $depth++;

        default:

            $url = $directory->data->get($key, "url");
            if (streq(substr($url, 0, 10), "content://"))
            {
                //get content index
                $index = substr($url, 10);
                $index = substr($index, 0, strspn($index, "1234567890"));

                $array_document[$index]       = TRUE;
                $array_schedule[sqlesc($key)] = TRUE;
            };

            break;

        case "/container":

            if (--$depth <= 0) break 2;
            break;
        };

        $key = $directory->data->move("next");
    };

    //delete branch
    $directory->data->del($ifc_param);

    //save changes
    if ($directory->save())
    {
        $error = FALSE;

        //select last existing object in path
        while (! $directory->data->get($object))
        {
            $object = array_pop($stack);
            if ($object === 0) break;
        };
    };

    if (is_array($array_document))
    {
        //go through directory tree and remove linked content from collection
        $directory->data->move("first");
        while ($key = $directory->data->move("next"))
        {
            $type = $directory->data->get($key, "#type");
            if (streq($type, "/container")) continue; //skip closing containers

            $url  = $directory->data->get($key, "url");
            if (streq(substr($url, 0, 10), "content://"))
            {
                //get content index
                $index = substr($url, 10);
                $index = substr($index, 0, strspn($index, "1234567890"));

                //collection match
                if (isset($array_document[$index]))
                    unset($array_document[$index]); //remove
            };
        };

        //go through remaining content and delete entries from database
        if (count($array_document))
        {
            $temp               = $content->publisher;
            $content->publisher = TRUE; //temporary permission

            foreach ($array_document AS $key => $_)
                $error |= ! $content->delete($key, TRUE, TRUE);

            $content->publisher = $temp;
        };
    };

    //delete schedule entries
    if (is_array($array_schedule))
        $error |= ! mysql_query("DELETE FROM " . CMS_DB_CONTENT_SCHEDULE . " " .
                                "WHERE " .       CMS_DB_CONTENT_SCHEDULE_TYPE . " " .
                                "IN ('" .        CMS_CONTENT_SCHEDULE_TYPE_PUBLISH . "', " .
                                "'" .            CMS_CONTENT_SCHEDULE_TYPE_WITHDRAW . "') " .
                                "AND " .         CMS_DB_CONTENT_SCHEDULE_VALUE_1 . " " .
                                "IN ('" .        implode("', '", array_keys($array_schedule)) . "')");

    $ifc_response = $error ? CMS_MSG_ERROR : CMS_MSG_DONE;
    break;

case "insert":
case "append":
//..................................................................................................

    $directory = new directory();
    $ifc_param = explode(",", $ifc_param);
    $value     = $ifc_param[0];
    $ifc_param = $ifc_param[1];

    //target may not be below selected entry
    if (! $directory->data->is_child($ifc_param, $value))
    {
        //cut and paste
        $directory->data->cut($value);
        $_object = streq(CMS_IFC_MESSAGE, "append") ?
                   $directory->data->append($ifc_param) :
                   $directory->data->insert($ifc_param);

        if ($directory->save())
        {
            $object       = $_object;
            $ifc_response = CMS_MSG_DONE;
            break;
        };
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "template_preview":
//..................................................................................................

    template_preview($object);
    exit();
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

//cache selected object permanently
if (CMS_IFC_MESSAGE !== "") cms_cache("directory." . CMS_USER . ".object", $object, TRUE);

$data     = new data("#system/directory");
$flexview = new flexview();

$flexview->import_data($data);

//menu
$menu = [CMS_L_COMMAND_ADD . "|easypage/command_create" => "add"];
if ($object)
{
    $url  = $data->get($object, "url");
    $flag = FALSE;

    if (streq(substr($url, 0, 10), "content://"))
    {
        //get content index
        $index = substr($url, 10);
        $index = substr($index, 0, strspn($index, "1234567890"));

        //retrieve keywords from database
        if (
             ($result = mysql_query("SELECT " . CMS_DB_CONTENT_TITLE . ", " .
                                                CMS_DB_CONTENT_DESCRIPTION . ", " .
                                                CMS_DB_CONTENT_KEYWORD . " " .
                                    "FROM " .   CMS_DB_CONTENT . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX . "='" . sqlesc($index) . "' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
           )
        {
            $flag        = TRUE;
            $title       = $resultrow[CMS_DB_CONTENT_TITLE];
            $description = $resultrow[CMS_DB_CONTENT_DESCRIPTION];
            $keyword     = $resultrow[CMS_DB_CONTENT_KEYWORD];
        };
    };

    if (! $flag)
    {
        $title       = "";
        $description = $data->get($object, "description");
    };
};

//display
$ifc = new ifc($ifc_response,
               $ifc_page,
               $menu,
               ["object" => $object]);

echo("<table class=\"layout\">" .

     "<colgroup>" .
     "<col>" .
     ($object ? "<col style=\"WIDTH:0\">" : "") .
     "</colgroup>" .

     "<tr>" .
     "<td>");

ifc_table_open();

//header
echo("<tr>" .
     "<th>" .
     CMS_L_IFC_EASYPAGE_001 .
     "</th>" .
     "</tr>" .

//directory
     "<tr>" .
     "<td>");

?><script>

function directory_easypage_event(event,
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

    if (source_id === target_id) return;

    switch (event)
    {
    case "dropon":

        if (target.id === "trashbin")     ifc_post("delete", source_id);
        else if (source_id !== target_id) ifc_post(action, source_id + "," + target_id);
        break;
    };
};

</script><?php

$ifc->set(CMS_L_IFC_EASYPAGE_010, "info b");
$flexview->set_encoding_function(__NAMESPACE__ . "\\qr");
$flexview->set_display_function(__NAMESPACE__ . "\\directory_flexview_display_function");
$flexview->show_hierarchy(
    $object,                                    //index
    "javascript:ifc_post('select','%index%');", //action
    "name",                                     //name key
    NULL,                                       //mark
    directory_get_type(),                       //icon
    "",                                         //base
    "directory_easypage_event");                //dragdrop_event_function
echo("</td>" .
     "</tr>" .

//trash bin
     "<tr>" .
     "<td id=\"trashbin\" " .
         "title=\"" . CMS_L_IFC_EASYPAGE_004 . "\" " .
         "class=\"varied\">" .
     image("easypage/command_delete") . " " . CMS_L_COMMAND_DELETE .
     jscript("dd_register(document.getElementById(\"trashbin\"), 0, 1, true);") .
     "</td>" .
     "</tr>");
ifc_table_close();

echo("</td>");

if ($object)
{
    //data
    echo("<td>");

    ifc_table_open();

    //header
    echo("<tr>" .
         "<th>" .
         CMS_L_IFC_EASYPAGE_002 .
         "</th>" .
         "</tr>" .

    //settings
         "<tr>" .
         "<td>");
    $ifc->set(CMS_L_IFC_EASYPAGE_008,            "info b");

    //name
    $ifc->set(CMS_L_IFC_EASYPAGE_009,            "text 50 256 bl",                   $data->get($object, "name"));

    echo("<div style=\"FLOAT:right\">");

    //edit button
    $url = translate_url(
        "directory://$object",
         ["content_option" => CMS_TEMPLATE_OPTION_ALL],
         CMS_LANGUAGE,
         TRUE);
    $ifc->set(CMS_L_COMMAND_EDIT,                "button",                           "javascript:load_page('" . q($url) . "');");

    //save button
    $ifc->set(CMS_L_COMMAND_SAVE,                "button b",                         "save");

    echo("</div>");

    //hidden
    $ifc->set(CMS_L_IFC_EASYPAGE_011,            "checkbox b", TRUE,                 $data->get($object, "hidden"));

    //meta data
    if ($flag) $ifc->set(CMS_L_IFC_EASYPAGE_014, "title");

    //page title
    $ifc->set(CMS_L_IFC_EASYPAGE_013,            $flag ? "text 50 256 bl" : "dummy", $title);

    //description
    $ifc->set(CMS_L_DESCRIPTION,                 "texteditor 50x8 2048 bilrt",       $description);

    //keywords
    if ($flag) $ifc->set(CMS_L_IFC_EASYPAGE_005, "text 50 256 l",                    $keyword);
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    echo("</td>");
};
echo("</tr>" .
     "</table>");

$ifc->close();