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
init($before);
init($after);

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
    $_key = permission_remove_prefix($key);

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
        //check receivers
        if ($ifc_param2 && ($ifc_param1 = array_intersect($ifc_param1, $list_receiver ?? [])))
        {
            foreach ($ifc_param1 AS $key => $value)
            {
                $call  = cms_cache("mcp.call." . DESKTOP_USER . ".$thread");
                $relay = $call && streq($call["sender"], $value);

                //notify via messenger only
                notify($value, $ifc_param2, DESKTOP_USER, $thread, ! $relay);

                //relay reply to caller
                if ($relay && cms_load("agent"))
                    agent::answer_call($call["callback"], DESKTOP_USER, $call["id"], $ifc_param2);
            };

            $ifc_response = t("msg_done");
            break;
        };
    }
    else
    {
        $ifc_param1 = NULL;
    };

    $ifc_response = t("msg_error");

case "reply":
case "send":
//..................................................................................................

    if (! $list_receiver)
    {
        $ifc_response = t("no_recipients_available");
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
    $ifc = new ifc(
        $ifc_response,
        NULL,
        TRUE,
        array_merge(
            $param,
            ["ifc_param"     => $ifc_param,
             "filter_user"   => $filter_user,
             "filter_thread" => $filter_thread,
             "thread"        => $thread]),
            $message,
            t("compose_message"));

    ifc_popover_open(t("recipient"));
    ksort($list_receiver, SORT_NATURAL | SORT_FLAG_CASE);
    $ifc->set($list_receiver, "multiselect 20x14", $ifc_param1);
    ifc_popover_close();

    if (nstre($text))
        echo("<br>" .
             "<div class=\"text\">" .
             url2link(x($text)) .
             "</div>");

    $ifc->set(t("message"), "textarea 0x20 1500 w", $ifc_param2);

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
                     ! $filter_user
                     ||
                     streq($value["receiver"], $filter_user)
                   )
                    $ims->del();
            }
            elseif (
                     ! $filter_user
                     ||
                     streq($value["sender"], $filter_user)
                   )
                $ims->del();
        };
    };

    $ifc_response = t("msg_done");
    break;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

//remove notification file
$path = DESKTOP_PATH . "ims.flag";
if (! $before)
{
    if (is_file($path)) unlink($path);
    elseif ($after)     exit(); //skip
};

//display
if ($async = streq(CMS_IFC_MESSAGE, "load"))
{
    ob_start();
    $ifc = new ifc();
    ob_end_clean();
}
else
{
    //menu
    $menu = [t("command_refresh") . "|desktop/command_refresh" => NULL];
    if ($list_receiver)                 $menu[t("compose_message") . "|desktop/command_create"] = "send";
    $menu[t("command_delete_selected") . "|desktop/command_delete"] = "delete";
    if ($filter_user || $filter_thread) $menu[t("command_previous") . "|desktop/command_cancel"] = "filter_reset";

    $ifc = new ifc(
        $ifc_response,
        NULL,
        $menu,
        array_merge(
            $param,
            ["filter_user"   => $filter_user,
             "filter_thread" => $filter_thread]),
        NULL,
        t("messenger"));
};

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
             ! $filter_user
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
             ! $filter_user
             ||
             streq($value["sender"], $filter_user)
           )
    {
        $message[$value["time"] . $value["id"]] = $value;
    };
};

if (! $message)
{
    $ifc->set(t("no_messages_available"), "info");
    $ifc->close();
};

//filter
if ($before) $message = array_filter($message, fn($key) => $key < $before, ARRAY_FILTER_USE_KEY);
if ($after)  $message = array_filter($message, fn($key) => $key > $after,  ARRAY_FILTER_USE_KEY);

//sort by date
krsort($message);

//retrieve page
$size    = $after ? PHP_INT_MAX : 50;
$load    = count($message) > $size;
$message = array_slice($message, 0, $size, TRUE);

if (! $async)
{
    ifc_table_open("varied");
    echo("<colgroup>" .
         "<col style=\"WIDTH:0\">" .
         "<col style=\"WIDTH:200px\">" .
         "<col>" .
         "</colgroup>");
};

foreach ($message AS $key => $value)
{
    //sent message
    if ($value["status"] & CMS_DESKTOP_IMS_STATUS_SENT)
    {
        if (! isset($list_hash[$value["hash"]])) continue;

        //selection
        echo("<tr data-ims-key=\"" . x($key) . "\">" .
             "<td class=\"select\">");
        $ifc->set(NULL, "checkbox", $value["hash"], FALSE, "list[]");
        echo("</td>" .

        //meta data
             "<td>" .
             x(friendly_date($value["time"])) . " " . t("to") . ":");

        $receiver = $list_hash[$value["hash"]];

        foreach ($receiver AS $_value)
            echo("<br>" .
                 "<a href=\"javascript:ifc_post('filter_user','" . qx($_value) . "');\" " .
                    "title=\"" . t("show_user_correspondence") . "\">" .
                 image("desktop/icon_receiver") . " " .
                 (($_receiver = $data->get("user.$_value", "name")) ? $_receiver : $_value) .
                 "</a>");

        echo("</td>" .

        //content
             "<td class=\"br\">" .
             "<a href=\"javascript:ifc_post('filter_thread','" . qx($value["thread"]) . "');\" " .
                "title=\"" . t("show_successional_messages") . "\">" .
             image("desktop/icon_message_sent") .
             "</a> " .
             nl2br(url2link(x($value["text"]))) .
             "</td>" .
             "</tr>");

        unset($list_hash[$value["hash"]]);
    }

    //received message
    else
    {
        $sender  = $value["sender"];
        $_sender = ($_sender = $data->get("user.$sender", "name")) ? $_sender : $sender;

        //selection
        echo("<tr data-ims-key=\"" . x($key) . "\">" .
             "<td class=\"select\">");
        $ifc->set(NULL, "checkbox", $value["hash"], FALSE, "list[]");
        echo("</td>" .

        //meta data
             "<td class=\"related\">" .
             x(friendly_date($value["time"])) . " " . t("from") . ":<br>" .
             "<a href=\"javascript:ifc_post('filter_user','" . qx($sender) . "');\" " .
                "title=\"" . t("show_user_correspondence") . "\">" .
        image("desktop/icon_receiver") . " " . x($_sender) .
        "</a>" .
        "</td>" .

        //content
             "<td class=\"related br\">" .
             "<a href=\"javascript:ifc_post('filter_thread','" . qx($value["thread"]) . "');\" " .
                "title=\"" . t("show_successional_messages") . "\">" .
             image("desktop/icon_message") .
             "</a> " .
             nl2br(url2link(x($value["text"]))));
        if ($list_receiver && in_array($sender, $list_receiver))
            echo("<br>" .
                 "<a href=\"javascript:ifc_post('reply','" . qx($value["id"]) . "');\">" .
                 image("desktop/command_create") . " " . t("reply") .
                 "</a>");
        echo("</td>" .
             "</tr>");
    };
};

//older messages remaining
if ($load)
    echo("<tr data-ims-load>" .
         "<td colspan=\"3\">" .
         t("loading") .
         "</td>" .
         "</tr>");

if ($async) exit();

//selection control
echo("<tr>" .
     "<td colspan=\"3\" " .
         "class=\"select\" " .
         "style=\"TEXT-ALIGN:right\">");
$ifc->set(t("all"),    "button", "javascript:ifc_list_activate();");
$ifc->set(t("invert"), "button", "javascript:ifc_list_invert();");
$ifc->set(t("none"),   "button", "javascript:ifc_list_deactivate();");
echo("</td>" .
     "</tr>");
ifc_table_close();

?><script>

function ims_load(before,
                  after,
                  callback)
{
    asr_send(
        "<?php echo(q(cms_url(array_merge(
            $param,
            ["desktop_display" => "interface",
             "ifc_message"     => "load",
             "filter_user"     => $filter_user,
             "filter_thread"   => $filter_thread,
             "before"          => "\x1B%before%",
             "after"           => "\x1B%after%"]))));?>"
        .replace("%before%", encodeURIComponent(before))
        .replace("%after%",  encodeURIComponent(after)),
        callback);
};

const ims_observer = new IntersectionObserver(function(list)
{
    list.forEach((object) =>
    {
        if (! object.isIntersecting) return;

        const target = object.target;
        this.unobserve(target);

        ims_load(target.previousElementSibling.dataset.imsKey, "", (value) =>
        {
            if (value === false) return;

            target.insertAdjacentHTML("beforebegin", value);
            target.remove();

            document.querySelectorAll("[data-ims-load]").forEach((marker) => this.observe(marker));
        });
    });
});

document.querySelectorAll("[data-ims-load]").forEach((marker) => ims_observer.observe(marker));

var ims_timer = null;

function ims_update()
{
    clearTimeout(ims_timer);
    if (document.hidden) return;

    const first = document.querySelector("[data-ims-key]");

    ims_load("", first.dataset.imsKey, (value) =>
    {
        if (value && (first === document.querySelector("[data-ims-key]")))
        {
            const top = first.getBoundingClientRect().top;
            first.insertAdjacentHTML("beforebegin", value);
            fx_scroll_container.scrollBy(0, first.getBoundingClientRect().top - top);
            ifc_response("<?php echo(q(t("new_message")));?>");
        };

        ims_timer = setTimeout(ims_update, 10000);
    });
};

fx_event_listen(document, "visibilitychange", () => { if (! document.hidden) ims_update(); });
ims_timer = setTimeout(ims_update, 10000);

</script><?php

$ifc->close();