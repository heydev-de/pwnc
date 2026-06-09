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

require("../pwnc.inc");

(function() {

global $chat_display,
       $chat_name,
       $core_control_object,
       $core_control_command,
       $core_control_value,
       $chat_data;

cms_load("core", TRUE);

define("CMS_CHAT_TIMEOUT",                30); //timeout in seconds
define("CMS_CHAT_REFRESH",              5000); //refresh rate in milliseconds
define("CMS_CHAT_AWAY_TIMER_TIMEOUT", 600000); //away timeout in milliseconds
define("CMS_CHAT_AWAY_TIMER_REFRESH",  30000); //away timer refresh rate in milliseconds

switch ($chat_display)
{
default:
//..................................................................................................

    $core = new core(CMS_CHAT_TIMEOUT);
    $core->update_index(TRUE); //update user list removing oneself on timeout

    //error - probably banned - or still connected
    if ((! $core->enabled) || $core->index->seek(["guid" => $core->guid]))
    {
        //display message and try again later
        echo(CMS_DOCTYPE_HTML .
             "<html>" .
             "<head>" .
             CMS_HTML_HEADER . CMS_JAVASCRIPT . CMS_STYLESHEET .
             "</head>" .
             "<body class=\"" . x(CMS_CLASS) . "\">" .

        //title
             "<h1>" .
             CMS_L_MOD_CHAT_014 .
             "</h1>" .

        //message
             ($core->enabled ?
             CMS_L_MOD_CHAT_001 .
             jscript("setTimeout(\"location.replace(\\\"" . q(q(cms_url())) . "\\\");\", 10000);") :
             CMS_L_MOD_CHAT_019) .
             "</body>" .
             "</html>");

        exit();
    };

    //try to log in
    if ($core->connect($chat_name))
    {
        $profile = $core->get_profile();

        //log
        $log = new log();
        $log->access("connected", $profile["name"]);
        $log->user($profile["name"]);

        header("Location: " . cms_url(["chat_display" => "interface"]), TRUE, 302);
        exit();
    };

    echo(CMS_DOCTYPE_HTML .
         "<html>" .
         "<head>" .
         CMS_HTML_HEADER . CMS_JAVASCRIPT . CMS_STYLESHEET .
         "</head>" .
         "<body class=\"" . x(CMS_CLASS) . "\">");

    //insert
    insert("top");

    echo("<noscript>" .
         "<div class=\"response-error\">" .
         CMS_L_NOSCRIPT .
         "</div>" .
         "</noscript>" .

    //title
         "<h1>" .
         CMS_L_MOD_CHAT_014 .
         "</h1>" .

         "<form name=\"chat_login\" " .
               "method=\"post\" " .
               "action=\"" . x(cms_url()) . "\">");

    //name taken message
    if (nstre($chat_name))
        echo("<div class=\"response\">" .
             CMS_L_MOD_CHAT_003 .
             "</div>");

    //name
    echo("<fieldset>" .
         "<legend>" .
         CMS_L_MOD_CHAT_008 .
         "</legend>" .
         "<input name=\"chat_name\" " .
                "type=\"text\" " .
                "value=\"" . x($chat_name) . "\" " .
                "maxlength=\"40\">" .
         "</fieldset>" .

    //connect button
         "<fieldset>" .
         "<input name=\"chat_submit\" " .
                "type=\"submit\" " .
                "value=\"" . CMS_L_COMMAND_CONNECT . "\">" .
         "</fieldset>" .
         "</form>" .

    //identification
         "<div class=\"p\">" .
         CMS_L_MOD_CHAT_013 .
         "</div>");

    cms_application("identification");
    echo(jscript("document.chat_login.chat_name.select();" .
                 "document.chat_login.chat_name.focus();"));

    //insert
    insert("bottom");

    echo("</body>" .
         "</html>");

    exit();

case "interface":
//..................................................................................................

    $core = new core(CMS_CHAT_TIMEOUT);
    $core->update_index();

    //error
    if ((! $core->enabled) || (! $core->index->seek(["guid" => $core->guid])))
    {
        header("Location: " . cms_url(["chat_display" => "disconnected"]), TRUE, 302);
        exit();
    };

    echo(CMS_DOCTYPE_HTML .
         "<html>" .
         "<head>" .
         CMS_HTML_HEADER . CMS_JAVASCRIPT . CMS_STYLESHEET .
         "</head>" .
         "<body class=\"" . x(CMS_CLASS) . "-interface\" " .
               "onload=\"chat_onload();\">" .

    //control status
         "<input id=\"chat-control-switch\" " .
                "type=\"checkbox\" " .
                "onchange=\"return chat_control_switch(this.checked);\">" .

    //menu
         "<div id=\"chat-menu\">" .

    //away button
         "<a href=\"#\" " .
            "title=\"/away\" " .
            "onclick=\"return chat_replace('/away',1);\">" .
         CMS_L_MOD_CHAT_009 .
         "</a>" .

    //disconnect button
         "<a href=\"#\" " .
            "onclick=\"return chat_disconnect();\">" .
         CMS_L_MOD_CHAT_017 .
         "</a>" .

    //control button
         "<label for=\"chat-control-switch\">" .
         CMS_L_MOD_CHAT_012 .
         "</label>" .
         "</div>" . //chat-menu

    //output
         "<div id=\"chat-output\">" .
         "</div>" . //chat-output

    //input
         "<div id=\"chat-input\">");

    //emoticons
    $list = [];
    $path = CMS_IMAGES_PATH . "emoticon";

    if (is_dir($path) && ($hdir = opendir($path)))
    {
        echo("<input id=\"chat-emoticon-trigger\" " .
                    "type=\"checkbox\">" .
             "<div id=\"chat-emoticon\">");

        while (($file = readdir($hdir)) !== FALSE)
        {
            if (! is_file("$path/$file"))                              continue; //no file
            if (! preg_match("/\.(gif|jpe?g|png|svg|webp)$/i", $file)) continue; //no image

            $name        = base64_decode(preg_replace("/\.[^\.]+$/", "", $file));
            $_name       = x($name);
            $list[$name] = $file;

            //emoticon button
            echo("<a href=\"#\" " .
                    "onclick=\"return chat_append(' " . qx($name) . " ');\" " .
                    "title=\"$_name\">" .
                 "<img src=\"" . x(CMS_IMAGES_URL . "emoticon/" . r($file)) . "\" " .
                      "alt=\"$_name\">" .
                 "</a>");
        };

        closedir($hdir);
        echo("</div>"); //chat-emoticon
    };

    echo("<form method=\"post\" " .
               "action=\"#\" " .
               "onsubmit=\"return chat_send();\">" .

    //text
         "<input id=\"chat-data\" " .
                "type=\"text\" " .
                "maxlength=\"1024\">" .

    //emoticons display switch
         "<label id=\"chat-emoticon-switch\" " .
                "for=\"chat-emoticon-trigger\">" .
         CMS_L_MOD_CHAT_018 .
         "</label>" .
         "</form>" .
         "</div>" . //chat-input

    //control
         "<div id=\"chat-control\">" .

    //control head
         "<div id=\"chat-control-head\">" .

    //back button
         "<label for=\"chat-control-switch\">" .
         CMS_L_MOD_CHAT_007 .
         "</label>" .

    //settings
         "<div id=\"chat-settings\">" .

    //focus switch
         "<label>" .
         "<input id=\"chat-focus-enabled\" " .
                "type=\"checkbox\">" .
         CMS_L_MOD_CHAT_011 .
         "</label>" .

    //sound switch
         "<label>" .
         "<input id=\"chat-sound-enabled\" " .
                "type=\"checkbox\" " .
                "checked>" .
         CMS_L_MOD_CHAT_006 .
         "</label>" .

    //away switch
         "<label>" .
         "<input id=\"chat-away-timer-enabled\" " .
                "type=\"checkbox\" " .
                "checked>" .
         CMS_L_MOD_CHAT_016 .
         "</label>" .

    //refresh
         "<a href=\"#\" " .
            "onclick=\"return chat_control_refresh(1);\">" .
         CMS_L_COMMAND_REFRESH .
         "</a>" .
         "</div>" . //chat-settings
         "</div>" . //chat-control-head

         "<iframe id=\"chat-control-iframe\" " .
                 "src=\"" . x(cms_url(["chat_display" => "control"])) . "\">" .
         "</iframe>" .
         "</div>"); //chat-control

?><script>

function chat_onload()
{
    chat_receive();
    chat_away_timer();
    document.getElementById("chat-data").focus();
    fx_event_listen(window, "message", chat_control_message);
};

var chat_control_refresh_enabled = false;

function chat_control_message(event)
{
    switch (event.data)
    {
    case "chat_control_submit":

        document.getElementById("chat-control-switch").checked = false;
        break;

    case "chat_control_refresh_enable":

        chat_control_refresh_enabled = true;
        break;

    case "chat_control_refresh_disable":

        chat_control_refresh_enabled = false;
        break;

    case "chat_control_receive":

        chat_receive(1);
        break;
    };
};

function chat_control_refresh(override = false)
{
    if ((! override) && (! chat_control_refresh_enabled)) return false;
    var object   = document.getElementById("chat-control-iframe").contentWindow;
    var location = object.location;
    location.replace(location.href);
    object.focus();
    return false;
};

function chat_control_switch(open)
{
    open ? chat_control_refresh() : document.getElementById("chat-data").focus();
    return false;
};

function chat_control_profile(guid)
{
    var url = "<?php echo(q(cms_url(["chat_display"        => "control",
                                     "core_control_object" => "\x1B%guid%"])));?>";
    guid    = ((guid === "<?php echo(q($core->guid));?>") ? "profile:" : "user:") + guid;

    var object = document.getElementById("chat-control-iframe").contentWindow;
    object.location.replace(url.replace("%guid%", encodeURIComponent(guid)));
    object.focus();

    object = document.getElementById("chat-control-switch");
    if (! object.checked) object.checked = true;

    return false;
};

var chat_receive_time = 0;

function chat_receive(override = false)
{
    if (! override)
    {
        var delay = <?php echo(CMS_CHAT_REFRESH);?> - new Date().getTime() + chat_receive_time;

        if (delay > 0)
        {
            setTimeout(chat_receive, delay);
            return;
        };

        setTimeout(chat_receive, <?php echo(CMS_CHAT_REFRESH);?>);
    };

    asr_send("<?php echo(q(cms_url(["chat_display" => "receive"])));?>", _chat_receive);
};

function _chat_receive(value)
{
    chat_receive_time = new Date().getTime();
    if (value === "") return;
    value = JSON.parse(value);

    for (var i = 0, c = value.length; i < c; i++)
    {
        switch (value[i][0])
        {
        default:
        case "m": chat_message(value[i][1], value[i][2], value[i][3], value[i][4], value[i][5]); break;
        case "p": chat_private(value[i][1], value[i][2], value[i][3], value[i][4], value[i][5], value[i][6]); break;
        case "s": chat_system(value[i][1]); break;
        case "i": chat_info(value[i][1]); break;
        case "d": chat_disconnect();
        };
    };
};

function chat_send()
{
    var value = document.getElementById("chat-data").value;
    if (value === "") return false;
    var url = "<?php echo(q(cms_url(["chat_display" => "send",
                                     "chat_data"    => "\x1B%data%"])));?>";
    asr_send(url.replace("%data%", encodeURIComponent(value)), _chat_send);
    return false;
};

function _chat_send()
{
    var object   = document.getElementById("chat-data");
    object.value = "";
    object.focus();
    chat_away_timer_reset();
    chat_receive(1);
};

function chat_append(text)
{
    var object    = document.getElementById("chat-data");
    object.value += text;
    object.focus();
    document.getElementById("chat-emoticon-trigger").checked = false;
    return false;
};

function chat_replace(text) //arguments: 1 => post
{
    var object   = document.getElementById("chat-data");
    object.value = text;
    object.focus();
    if (arguments.length > 1) chat_send();
    return false;
};

function chat_write(type,
                    guid,
                    name,
                    color,
                    image,
                    text,
                    meta)
{
    var output = document.getElementById("chat-output");
    var scroll = Math.abs(output.scrollHeight - output.clientHeight - output.scrollTop) < 10;
    type = (type == "") ?
           "chat-message" :
           "chat-message-" + type;
    name = (name == "") ?
           "" :
           "<a href=\"#\" " +
              "class=\"chat-message-name\" " +
              "onclick=\"return chat_control_profile('" + addslashes(htmlspecialchars(guid)) + "');\">" +
           htmlspecialchars(name) +
           "<\/a>";
    if (color != "")
    {
        var _color = (parseInt(color.substr(1, 2), 16) * 3 +
                      parseInt(color.substr(3, 2), 16) * 6 +
                      parseInt(color.substr(5, 2), 16)) < 1500 ?
                     "#ffffff" : "#000000";
        color      = " style=\"BACKGROUND-COLOR:" + color + ";" +
                              "BORDER-COLOR:" + _color + ";" +
                              "COLOR:" + _color + "\"";
    };
    image = (image == "") ?
        "" :
        "<img src=\"" + htmlspecialchars(image) + "\" " +
             "alt=\"\" " +
             "class=\"chat-message-image\">";
    text  = htmlspecialchars(text);
    text  = chat_link(text);
    text  = chat_emoticon(text);
    meta  = (meta == "") ?
        "" :
        "<div class=\"chat-message-meta\">" +
        meta +
        "<\/div>";
    output.insertAdjacentHTML(
        "beforeend",
        "<div class=\"" + type + "\"" + color + ">" +
        image +
        meta +
        name +
        "<div class=\"chat-message-text\">" +
        text +
        "<\/div>" +
        "<\/div>");
    if (scroll) output.scrollBy(0, 99999999);
};

function chat_message(guid,
                      name,
                      color,
                      image,
                      text)
{
    chat_write("", guid, name, color, image, text, "");
    chat_focus();
    chat_notify("<?php echo(q(CMS_L_MOD_CHAT_005));?>".replace("%s", name));
    chat_sound("<?php echo(q(CMS_SOUNDS_URL));?>chat/message.mp3");
};

function chat_private(guid,
                      name,
                      color,
                      image,
                      text,
                      meta)
{
    var _meta = "";
    meta      = meta.split("\n");
    var c     = meta.length;
    if (c > 1)
    {
        for (var i = 0; i < c; i += 2)
            _meta += "<a href=\"#\" " +
                        "onclick=\"return chat_control_profile('" + addslashes(htmlspecialchars(meta[i])) + "');\">" +
                     "@" + htmlspecialchars(meta[i + 1]) +
                     "</a>";
    }
    else
    {
        _meta = "<?php echo(qx(CMS_L_MOD_CHAT_002));?>";
    };

    chat_write("private", guid, name, color, image, text, _meta);
    chat_focus();
    chat_notify("<?php echo(q(CMS_L_MOD_CHAT_005));?>".replace("%s", name));
    chat_sound("<?php echo(q(CMS_SOUNDS_URL));?>chat/private.mp3");
};

function chat_system(text)
{
    chat_write("system", "", "", "", "", text, "");
    chat_focus();
    chat_notify(text);
    chat_sound("<?php echo(q(CMS_SOUNDS_URL));?>chat/system.mp3");
};

function chat_info(text)
{
    chat_write("info", "", "", "", "", text, "");
};

function chat_link(text)
{
    text = text.replace(
        /(^|\s)(www\.\S+)(?=\s|$)/gi,
        "$1https://$2");
    text = text.replace(
        /(^|\s)((?:ftp|https?):\/\/\S+\.(?:gif|jpe?g|png|webp))(?=\s|$)/gi,
        "$1" +
        "<a href=\"$2\" " +
           "onclick=\"window.open(this.href);return false;\">" +
        "<img src=\"$2\" " +
             "alt=\"\" " +
             "style=\"MAX-HEIGHT:200px;" +
                     "MAX-WIDTH:200px\">" +
        "<\/a>");
    text = text.replace(
        /(^|\s)((?:ftp|https?):\/\/[^\s"]+)(?=\s|$)/gi,
        "$1" +
        "<a href=\"$2\" " +
           "onclick=\"window.open(this.href);return false;\">" +
        "$2" +
        "<\/a>");
    return text;
};

function chat_emoticon(text)
{
    return text<?php
    foreach ($list AS $key => $value)
        echo(".replace(/(^|\\s)" . preg_quote(x($key), "/") . "(?=\\s|$)/g, " .
             "\"$1<img src=\\\"" . qx(CMS_IMAGES_URL . "emoticon/" . r($value)) . "\\\" " .
                      "alt=\\\"" . qx($key) . "\\\">\")");?>;
};

function chat_focus()
{
    if (! document.getElementById("chat-focus-enabled").checked) return;
    top.focus();
};

var chat_notify_text  = "";
var chat_notify_count = 0;

function chat_notify(text)
{
    chat_notify_text = text;

    if (chat_notify_count === 0)
    {
        chat_notify_count = 4;
        _chat_notify();
        return;
    };

    chat_notify_count = 4;
};

function _chat_notify()
{
    top.document.title = chat_notify_text;
    if (--chat_notify_count <= 0) return;
    setTimeout("top.document.title=\"…\";", 250);
    setTimeout(_chat_notify, 750);
};

var chat_audio_playing = false;

function chat_sound(file)
{
    if (chat_audio_playing) return;
    if (! document.getElementById("chat-sound-enabled").checked) return;

    var f_play = function()
    {
        audio.play();
    };

    var f_end = function()
    {
        fx_event_remove(audio, "canplay", f_play);
        fx_event_remove(audio, "ended", f_end);
        fx_event_remove(audio, "error", f_end);
        audio = null;
        chat_audio_playing = false;
    };

    var audio = new Audio(file);
    fx_event_listen(audio, "canplay", f_play);
    fx_event_listen(audio, "ended", f_end);
    fx_event_listen(audio, "error", f_end);
    chat_audio_playing = true;
};

var chat_away_timer_value = <?php echo(CMS_CHAT_AWAY_TIMER_TIMEOUT);?>;

function chat_away_timer()
{
    if (document.getElementById("chat-away-timer-enabled").checked)
    {
        if (chat_away_timer_value >= 0)
        {
            chat_away_timer_value -= <?php echo(CMS_CHAT_AWAY_TIMER_REFRESH);?>;
            if (chat_away_timer_value < 0) chat_replace("/away", true)
        };
    }
    else
    {
        chat_away_timer_value = <?php echo(CMS_CHAT_AWAY_TIMER_TIMEOUT);?>;
    };

    setTimeout(chat_away_timer, <?php echo(CMS_CHAT_AWAY_TIMER_REFRESH);?>);
};

function chat_away_timer_reset()
{
    chat_away_timer_value = <?php echo(CMS_CHAT_AWAY_TIMER_TIMEOUT);?>;
};

function chat_disconnect()
{
    location.replace("<?php echo(q(cms_url(["chat_display" => "disconnect"])));?>");
    return false;
};

</script><?php

    echo("</body>" .
         "</html>");

    exit();

case "control":
//..................................................................................................

    if (! cms_permission(CMS_CORE_PERMISSION_CONTROL)) exit();

    cms_load("core_control", TRUE);

    $core = new core(CMS_CHAT_TIMEOUT);
    $core->update_index();

    //error
    if ((! $core->enabled) || (! $core->index->seek(["guid" => $core->guid]))) exit();

    echo(CMS_DOCTYPE_HTML .
         "<html>" .
         "<head>" .
         CMS_HTML_HEADER . CMS_JAVASCRIPT . CMS_STYLESHEET .
         "</head>" .
         "<body class=\"" . x(CMS_CLASS) . "-control\">");

    //insert
    insert("control_top");

    //set default display for frame
    cms_param("control", "chat_display");

    //load control element
    core_control(
        $core,
        $core_control_object,
        $core_control_command,
        $core_control_value);

    //insert
    insert("control_bottom");

    //permission
    permission([""                           => CMS_L_ACCESS,
                CMS_CORE_PERMISSION_CONTROL  => CMS_L_MOD_CHAT_012,
                CMS_CORE_PERMISSION_OPERATOR => CMS_L_OPERATOR]);

    echo("</body>" .
         "</html>");

    exit();

case "send":
//..................................................................................................

    $core = new core(CMS_CHAT_TIMEOUT);
    $core->update_index();

    //error
    if ((! $core->enabled) || (! $core->index->seek(["guid" => $core->guid]))) exit();

    //no data
    if (stre($data = utf8_trim($chat_data))) exit();

    //input commands
    switch ($data)
    {
    case "afk":
    case "away":
    case "brb":
    case "/afk":
    case "/away":
    case "/brb":

        $core->status(CMS_CORE_STATUS_ABSENT);
        break;

    case "/bye":
    case "/disconnect":
    case "/exit":
    case "/logout":
    case "/off":
    case "/quit":

        if ($core->index->seek(["guid" => $core->guid])) $core->disconnect();
        break;

    default:

        $core->send($data);
        break;
    };

    exit();

case "receive":
//..................................................................................................

    $core = new core(CMS_CHAT_TIMEOUT);
    $core->update_index();

    //error
    if ((! $core->enabled) || (! $core->index->seek(["guid" => $core->guid])))
    {
        echo("[[\"d\"]]");
        exit();
    };

    if (! $data = $core->receive()) exit(); //no data

    $flag = FALSE;
    echo("[");

    foreach ($data AS $key => $value)
    {
        $array = [];

        //system message
        if (flag($value["status"], CMS_CORE_DATA_SYSTEM))
        {
            $array = ["s", $value["data"]];
        }

        //private message
        elseif (flag($value["status"], CMS_CORE_DATA_PRIVATE))
        {
            //get private receivers
            if (flag($value["status"], CMS_CORE_DATA_PRIVATE_META))
            {
                $profile  = $core->get_profile($value["guid"]);
                $meta     = "";

                $receiver = explode("\n", $value["data"]);

                foreach ($receiver AS $guid)
                {
                    $profile  = $core->get_profile($guid);
                    $meta    .= $guid . "\n" .
                                (stre($profile["name"]) ? CMS_L_UNKNOWN : $profile["name"]) . "\n";
                };

                $meta = substr($meta, 0, -1);
                continue;
            }

            //get private data
            elseif (flag($value["status"], CMS_CORE_DATA_PRIVATE_DATA))
            {
                $profile = $core->get_profile($value["guid"]);
                $array   = ["p",
                            $value["guid"],
                            stre($profile["name"]) ? CMS_L_UNKNOWN : $profile["name"],
                            $profile["color"],
                            stre($profile["image"]) ? "" : image_process(CMS_DATA_URL . "core/" . r($profile["image"]), 35),
                            $value["data"],
                            $meta];
            }

            //get spy receivers
            elseif (flag($value["status"], CMS_CORE_DATA_SPY_META))
            {
                $profile  = $core->get_profile($value["guid"]);
                $meta     = stre($profile["name"]) ? CMS_L_UNKNOWN : $profile["name"];

                $receiver = explode("\n", $value["data"]);

                foreach ($receiver AS $guid)
                {
                    $profile  = $core->get_profile($guid);
                    $meta    .= " @" . (stre($profile["name"]) ? CMS_L_UNKNOWN : $profile["name"]);
                };

                continue;
            }

            //get spy data
            elseif (flag($value["status"], CMS_CORE_DATA_SPY_DATA))
            {
                $array = ["i", $meta . ": " . $value["data"]];
            }

            //display
            else
            {
                $profile = $core->get_profile($value["guid"]);
                $array   = ["p",
                            $value["guid"],
                            stre($profile["name"]) ? CMS_L_UNKNOWN : $profile["name"],
                            $profile["color"],
                            stre($profile["image"]) ? "" : image_process(CMS_DATA_URL . "core/" . r($profile["image"]), 35),
                            $value["data"],
                            ""];
            };
        }

        //default message
        else
        {
            $profile = $core->get_profile($value["guid"]);
            $array   = ["m",
                        $value["guid"],
                        stre($profile["name"]) ? CMS_L_UNKNOWN : $profile["name"],
                        $profile["color"],
                        stre($profile["image"]) ? "" : image_process(CMS_DATA_URL . "core/" . r($profile["image"]), 35),
                        $value["data"]];
        };

        if ($flag) echo(", ");
        else       $flag = TRUE;
        echo(json_encode($array));
    };

    echo("]");
    exit();

case "disconnect":
//..................................................................................................

    $core = new core(CMS_CHAT_TIMEOUT);
    $core->update_index();

    //disconnect
    if ($core->enabled && $core->index->seek(["guid" => $core->guid])) $core->disconnect();

case "disconnected":
//..................................................................................................

    echo(CMS_DOCTYPE_HTML .
         "<html>" .
         "<head>" .
         CMS_HTML_HEADER . CMS_JAVASCRIPT . CMS_STYLESHEET .
         "</head>" .
         "<body class=\"" . x(CMS_CLASS) . "-disconnected\">" .
         "<div>" .

    //title
         "<h1>" .
         CMS_L_MOD_CHAT_014 .
         "</h1>" .

    //message
         "<p>" .
         CMS_L_MOD_CHAT_004 .
         "</p>" .

    //reconnect button
         "<a href=\"" . x(cms_url()) . "\" " .
            "class=\"button\">" .
         CMS_L_MOD_CHAT_015 .
         "</a>" .
         "</div>" .
         "</body>" .
         "</html>");

    exit();
};

})();