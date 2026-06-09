<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   DESKTOP INTERFACE
//==================================================================================================

cms_load("core_resource", TRUE);

define("CMS_DESKTOP_IMS_STATUS_NONE", 0);
define("CMS_DESKTOP_IMS_STATUS_SENT", 1);

init($filter_user);
init($filter_thread);

//create core object
$ims = new core_resource(
    CMS_DATA_PATH . "#desktop/ims.core",
    ["id"       => "string[8]",
     "thread"   => "string[8]",
     "time"     => "string[20]",
     "owner"    => "_string[40]",
     "receiver" => "_string[40]",
     "sender"   => "_string[40]",
     "status"   => "byte",
     "text"     => "_string[1500]",
     "hash"     => "string[32]"]);

//get receiver list
$list_receiver = NULL;
$data          = new data("#system/permission");

$data->move("first");
while ($key = $data->move("next"))
{
    if (! permission_is_user($key)) continue;
    $_key = substr($key, 5);

    if (
         nstreq($_key, DESKTOP_USER)
         &&
         cms_permission(CMS_APPLICATION, NULL, NULL, $_key)
       )
        $list_receiver[$data->get($key, "name")] = $_key;
};

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "filter_user":
//..................................................................................................

    $filter_user = $ifc_param;
    break;

case "filter_thread":
//..................................................................................................

    $filter_thread = $ifc_param;
    break;

case "filter_reset":
//..................................................................................................

    $filter_user   = NULL;
    $filter_thread = NULL;
    break;

case "_reply":
case "_send":
//..................................................................................................

    if (isset($ifc_param1))
    {
        if ($ifc_param2)
        {
            $time = time();
            $hash = hash32($thread . $time . DESKTOP_USER . $ifc_param2);

            foreach ($ifc_param1 AS $key => $value)
            {
                $ims->seek(["owner" => NULL], TRUE);
                $ims->set(
                    ["id"       => unique_id(8),
                     "thread"   => $thread,
                     "time"     => $time,
                     "owner"    => $value,
                     "receiver" => $value,
                     "sender"   => DESKTOP_USER,
                     "status"   => CMS_DESKTOP_IMS_STATUS_NONE,
                     "text"     => $ifc_param2,
                     "hash"     => $hash]);

                $ims->seek(["owner" => NULL], TRUE);
                $ims->set(
                    ["id"       => unique_id(8),
                     "thread"   => $thread,
                     "time"     => $time,
                     "owner"    => DESKTOP_USER,
                     "receiver" => $value,
                     "sender"   => DESKTOP_USER,
                     "status"   => CMS_DESKTOP_IMS_STATUS_SENT,
                     "text"     => $ifc_param2,
                     "hash"     => $hash]);

                //create notification file
                if ($hfile = fopen(DESKTOP_PATH . "ims.flag", "wb"))
                {
                    flock($hfile, LOCK_EX);
                    fwrite($hfile, 1);
                    fclose($hfile);
                };
            };

            $ifc_response = CMS_MSG_DONE;
            break;
        };
    }
    else
    {
        $ifc_param1 = NULL;
    };

    $ifc_response = CMS_MSG_ERROR;

case "reply":
case "send":
//..................................................................................................

    if (! $list_receiver)
    {
        $ifc_response = CMS_L_DESKTOP_IMS_004;
        break;
    };

    switch (CMS_IFC_MESSAGE)
    {
    case "reply":
//..................................................................................................

        if ($ims->seek(["id" => $ifc_param]))
        {
            $value      = $ims->get();
            $thread     = $value["thread"];
            $text       = $value["text"];
            $ifc_param1 = [$value["sender"]];
            $ifc_param2 = NULL;
            $message    = "_" . CMS_IFC_MESSAGE;
            break;
        };

    case "send":
//..................................................................................................

        $thread     = unique_id(8);
        $text       = NULL;
        $ifc_param1 = NULL;
        $ifc_param2 = NULL;
        $message    = "_" . CMS_IFC_MESSAGE;
        break;

    case "_reply":
//..................................................................................................

        if ($ims->seek(["id" => $ifc_param]))
        {
            $value   = $ims->get();
            $text    = $value["text"];
            $message = CMS_IFC_MESSAGE;
            break;
        };

    case "_send":
//..................................................................................................

        $text    = NULL;
        $message = CMS_IFC_MESSAGE;
        break;
    };

    //display
    $ifc = new ifc($ifc_response,
                   NULL,
                   TRUE,
                   array_merge(
                       $param,
                       ["ifc_param"     => $ifc_param,
                        "filter_user"   => $filter_user,
                        "filter_thread" => $filter_thread,
                        "thread"        => $thread]),
                   $message,
                   CMS_L_DESKTOP_IMS_001);

    echo("<table class=\"layout\">" .
         "<colgroup>" .
         "<col style=\"WIDTH:0\">" .
         "<col>" .
         "</colgroup>" .

         "<tr>" .
         "<td>");
    $ifc->set(CMS_L_DESKTOP_IMS_002, "label");
    ksort($list_receiver, SORT_NATURAL | SORT_FLAG_CASE);
    $ifc->set($list_receiver,        "multiselect 20x14",     $ifc_param1);
    echo("</td>" .

         "<td>");
    $ifc->set($text,                 "info b");
    $ifc->set(CMS_L_DESKTOP_IMS_003, "textarea 55x20 1500 w", $ifc_param2);
    echo("</td>" .
         "</tr>" .
         "</table>");

    $ifc->close();

case "delete":
//..................................................................................................

    if (empty($list)) break; //no selection

    $list   = array_flip($list);
    $filter = ["owner" => DESKTOP_USER];
    if ($filter_thread) $filter["thread"] = $filter_thread;

    $ims->reset();
    while ($ims->next($filter))
    {
        $value = $ims->get();

        if (isset($list[$value["hash"]]))
        {
            if ($value["status"] & CMS_DESKTOP_IMS_STATUS_SENT)
            {
                if (
                     (! $filter_user)
                     ||
                     streq($value["receiver"], $filter_user)
                   )
                    $ims->del();
            }
            elseif (
                     (! $filter_user)
                     ||
                     streq($value["sender"], $filter_user)
                   )
                $ims->del();
        };
    };

    $ifc_response = CMS_MSG_DONE;
    break;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

//remove notification file
$path = DESKTOP_PATH . "ims.flag";
if (is_file($path)) unlink($path);

//menu
$menu = [CMS_L_COMMAND_REFRESH . "|desktop/command_refresh" => NULL];
if ($list_receiver)                 $menu[CMS_L_DESKTOP_IMS_001  . "|desktop/command_create"] = "send";
$menu[CMS_L_COMMAND_DELETE_SELECTED . "|desktop/command_delete"] =  "delete";
if ($filter_user || $filter_thread) $menu[CMS_L_COMMAND_PREVIOUS . "|desktop/command_cancel"] = "filter_reset";

//display
$ifc = new ifc($ifc_response,
               NULL,
               $menu,
               array_merge(
                   $param,
                   ["filter_user"   => $filter_user,
                    "filter_thread" => $filter_thread]),
               NULL,
               CMS_L_DESKTOP_IMS_012);

//receive messages
$filter    = ["owner" => DESKTOP_USER];
if ($filter_thread) $filter["thread"] = $filter_thread;
$message   = NULL;
$list_hash = NULL;

$ims->reset();
while ($ims->next($filter))
{
    $value = $ims->get();

    if ($value["status"] & CMS_DESKTOP_IMS_STATUS_SENT)
    {
        if (
             (! $filter_user)
             ||
             streq($value["receiver"], $filter_user)
           )
        {
            $hash               = $value["hash"];
            if (! isset($list_hash[$hash]))
                $message[$value["time"] . $value["id"]] = $value;
            $list_hash[$hash][] = $value["receiver"];
        };
    }
    elseif (
             (! $filter_user)
             ||
             streq($value["sender"], $filter_user)
           )
    {
        $message[$value["time"] . $value["id"]] = $value;
    };
};

if (! $message)
{
    $ifc->set(CMS_L_DESKTOP_IMS_005, "info");
    $ifc->close();
};

ifc_table_open();
echo("<colgroup>" .
     "<col style=\"WIDTH:0\">" .
     "<col style=\"WIDTH:200px\">" .
     "<col>" .
     "</colgroup>");

krsort($message);
foreach ($message AS $value)
{
    $varied = ifc_varied();

    //sent message
    if ($value["status"] & CMS_DESKTOP_IMS_STATUS_SENT)
    {
        if (! isset($list_hash[$value["hash"]])) continue;

        //selection
        echo("<tr>" .
             "<td class=\"select\">");
        $ifc->set(NULL, "checkbox", $value["hash"], FALSE, "list[]");
        echo("</td>" .

        //meta data
             "<td$varied>" .
             x(friendly_date($value["time"])) . " " . CMS_L_DESKTOP_IMS_013 . ":");

        $receiver = $list_hash[$value["hash"]];

        foreach ($receiver AS $_value)
            echo("<br>" .
                 "<a href=\"javascript:ifc_post('filter_user','" . qx($_value) . "');\" " .
                    "title=\"" . CMS_L_DESKTOP_IMS_009 . "\">" .
                 image("desktop/icon_receiver") . " " .
                 (($_receiver = $data->get("user.$_value", "name")) ? $_receiver : $_value) .
                 "</a>");

        echo("</td>" .

        //content
             "<td$varied>" .
             "<a href=\"javascript:ifc_post('filter_thread','" . qx($value["thread"]) . "');\" " .
                "title=\"" . CMS_L_DESKTOP_IMS_010 . "\" " .
                "class=\"br\">" .
             image("desktop/icon_message_sent") . " " . nl2br(x($value["text"])) .
             "</a>" .
             "</td>" .
             "</tr>");

        unset($list_hash[$value["hash"]]);
    }

    //received message
    else
    {
        $sender  = $value["sender"];
        $_sender = ($_sender = $data->get("user.$sender", "name")) ? $_sender : $sender;
        $class   = stre($varied) ? "related" : "related2";

        //selection
        echo("<tr>" .
             "<td class=\"select\">");
        $ifc->set(NULL, "checkbox", $value["hash"], FALSE, "list[]");
        echo("</td>" .

        //meta data
             "<td class=\"$class\">" .
             x(friendly_date($value["time"])) . " " . CMS_L_DESKTOP_IMS_014 . ":<br>" .
             "<a href=\"javascript:ifc_post('filter_user','" . qx($sender) . "');\" " .
            "title=\"" . CMS_L_DESKTOP_IMS_009 . "\">" .
        image("desktop/icon_receiver") . " " . x($_sender) .
        "</a>" .
        "</td>" .

        //content
             "<td class=\"$class\">" .
             "<a href=\"javascript:ifc_post('filter_thread','" . qx($value["thread"]) . "');\" " .
                "title=\"" . CMS_L_DESKTOP_IMS_010 . "\" " .
                "class=\"br\">" .
             image("desktop/icon_message") . " " . nl2br(x($value["text"])) .
             "</a>");
        if (nstreq($sender, DESKTOP_USER))
            echo("<br>" .
                 "<a href=\"javascript:ifc_post('reply','" . qx($value["id"]) . "');\">" .
                 image("desktop/command_create") . " " . CMS_L_DESKTOP_IMS_011 .
                 "</a>");
        echo("</td>" .
             "</tr>");
    };
};

//selection control
echo("<tr>" .
     "<td colspan=\"3\" " .
         "class=\"select\" " .
         "style=\"TEXT-ALIGN:right\">");
$ifc->set(CMS_L_ALL,    "button", "javascript:ifc_list_activate();");
$ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert();");
$ifc->set(CMS_L_NONE,   "button", "javascript:ifc_list_deactivate();");
echo("</td>" .
     "</tr>");
ifc_table_close();

$ifc->close();