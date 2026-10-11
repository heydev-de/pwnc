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

$agent_process_object = function($_object) use (&$operator, &$object, &$owner, &$user, &$id)
{
    //extract object parts
    [$owner, $user, $id] = array_pad(preg_split("/\./", $_object, 3, PREG_SPLIT_NO_EMPTY), 3, "");

    //no owner selected
    if (stre($owner))
    {
        $owner = CMS_SUPERUSER; //default
    }

    //only operators can access other owners
    elseif (! $operator && nstreq($owner, CMS_SUPERUSER))
    {
        $owner = CMS_SUPERUSER;
        $user  = $id = "";
    }

    elseif (

    //user selected
             nstre($user)
             &&

    //not operator
             ! $operator
             &&

    //no permission to access user
             ! cms_permission("agent.$user", NULL, NULL)
           )
    {
        $user = $id = "";
    };

    //construct and store object in permanent cache
    $_object = $owner . (nstre($user) ? ".$user" . (nstre($id) ? ".$id" : "") : "");
    if ($_object === $object) return;

    $object = $_object;
    cms_cache("agent." . CMS_USER . ".object", $_object, TRUE);
};

//load library
if (! cms_load("agent")) ifc_inactive($ifc_page);

//check operator status
$operator = cms_permission("operator");

//retrieve selected object from permanent cache
cms_cache_init($object, "agent." . CMS_USER . ".object", CMS_SUPERUSER, TRUE);
$agent_process_object($object);

//==================================================================================================
//   MESSAGE HANDLING
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "select":
//..................................................................................................

    $agent_process_object($ifc_param ??= CMS_SUPERUSER);
    break;

case "display":
//..................................................................................................

    if (stre($id)) exit();

    $path = CMS_AGENT_DATA_PATH . encode_filename($owner) . "/" .
            encode_filename($user) . "/" . encode_filename($id) . ".json";
    if (! is_file($path)) exit();

    $hfile = fopen($path, "rb");
    if ($hfile === FALSE) exit();

    flock($hfile, LOCK_SH);

    $array    = [];
    $pending  = [];
    $boundary = "";
    $from     = max((int)($from ?? -1), -1);
    $to       = max((int)($to   ?? -1), -1);
    $i        = 0;

    //format token usage
    $usage_format = function($usage)
    {
        if (! array_sum($usage)) return "";
        return format_number($usage["input"] ?? 0) .
               (($usage["cache"] ?? 0) ? " (" . t("tokens_cache") . ": " . format_number($usage["cache"]) . ")" : "") .
               " ➞ " . format_number($usage["output"] ?? 0) . " " . t("tokens");
    };

    //prescan lines before displayed range to rebuild pending list
    $prescan = function($line) use (&$pending, &$boundary)
    {
        if (! preg_match("/\"type\":\"(message|result)\"/", substr($line, 0, 100))) return;

        //no tool call
        if (strpos($line, "\"tools\"") === FALSE)
        {
            if (strpos($line, "\"total\":") !== FALSE) $boundary = $line; //reply
            return;
        };

        //decode line
        $json = json_decode($line, TRUE);
        if (! is_array($json)) return;

        //message
        if (($json["type"] ?? "") === "message")
        {
            foreach ((array)($json["tools"] ?? []) AS $entry)
            {
                if (! isset($entry["result_path"])) $pending[$entry["id"]] = $entry["name"];
            };

            return;
        };

        //result
        foreach ((array)($json["tools"] ?? []) AS $entry)
            unset($pending[$entry["id"] ?? ""]);
    };

    //render detail block
    $display_detail = function($class, $label, $text, $info = "")
    {
        echo("<details class=\"agent-$class\">" .
             "<summary>" .
             x($label) .
             (nstre($info) ? " (" . x($info) . ")" : "") .
             "</summary>" .
             nl2br(x($text)) .
             "</details>");
    };

    //render tool call with result
    $display_tool = function($name, $text, $error) use ($display_detail)
    {
        $display_detail(
            $error ? "error" : "result",
            (stre($name) ? t("tool_call") : $name),
            $text,
            format_bytesize(strlen($text)));
    };

    $usage_total = "";
    while (($line = fgets($hfile)) !== FALSE)
    {
        //latest total usage
        if (strpos($line, "\"total\":") !== FALSE) $usage_total = $line;

        if (($to !== -1) && ($i > $to)) continue; //outside range

        if (($from === -1) || ($i >= $from))
        {
            $array[$i] = $line;
            if (($from === -1) && (count($array) > 50))
            {
                $prescan($array[array_key_first($array)]);
                unset($array[array_key_first($array)]);
            };
        }

        //preceding lines
        else
        {
            $prescan($line);
        };

        $i++;
    };

    fclose($hfile);

    if (blank($array)) exit(); //nothing new

    //total usage of previous turn
    $usage_prev = json_decode($boundary, TRUE)["total"] ?? [];

    //return delivered range lines plus status
    echo(array_key_first($array) . "\n" .
         array_key_last($array) . "\n" .
         $usage_format(json_decode($usage_total, TRUE)["total"] ?? []) . "\n");

    foreach ($array AS $value)
    {
        $json = json_decode($value, TRUE);
        if (! is_array($json)) continue; //entry corrupted

        $type = $json["type"];
        $time = (int)$json["time"];
        $from = $json["from"] ?? "";         //system, tool, user.{username}
        $role = $json["data"]["role"] ?? ""; //system, user, assistant, model, tool
        $text = agent::extract_text($json["data"] ?? NULL, $json["text_path"] ?? "");

        //extract context
        $context = "";
        if (preg_match("/^(.*?)" . CMS_AGENT_CONTEXT_SEPARATOR . "(.*)$/su", $text, $match))
        {
            $text    = $match[1];
            $context = $match[2];
        };

        switch ($type)
        {
        case "message":
//..................................................................................................

            //system prompt
            if (streq($role, "system")) { $display_detail("system", t("system_prompt"), $text); break; };

            //reasoning
            $reasoning = (string)($json["reasoning"] ?? "");
            if (nstre($reasoning)) $display_detail("reasoning", t("reasoning"), $reasoning);

            //context
            if (nstre($context)) $display_detail("context", t("context"), $context);

            //tool calls
            foreach ((array)($json["tools"] ?? []) AS $entry)
            {
                //provider executes mcp
                if (isset($entry["result_path"]))
                {
                    $display_tool(
                        $entry["name"],
                        agent::extract_text(
                            array_get_path($json["data"] ?? NULL, $entry["result_path"]),
                            $entry["text_path"] ?? ""),
                        $entry["error"] ?? FALSE);
                    continue;
                };

                //pwnc executes mcp
                $pending[$entry["id"]] = $entry["name"];
            };

            //token usage
            $usage = "";
            if (isset($json["total"]) && ! isset($json["tools"]))
            {
                $delta = [];
                foreach ($json["total"] AS $key => $value)
                    $delta[$key] = $value - ($usage_prev[$key] ?? 0);

                $usage      = $usage_format($delta);
                $usage_prev = $json["total"];
            };

            if (stre($text)) break;

            echo("<div class=\"" . (streq($role, "user") ? "agent-message" : "agent-response") . "\">" .
                 "<div class=\"agent-name\">" .
                 x(permission_get_name(permission_remove_prefix($from), $from)) .
                 "</div>" .
                 (stre($usage) ? "" :
                 "<div class=\"agent-usage\">" .
                 x($usage) .
                 "</div>") .
                 "<div class=\"agent-date\">" .
                 date(t("short_datetime_format"), $time) .
                 "</div>" .
                 "<div class=\"agent-text text\">" .
                 nl2br(x($text)) .
                 "</div>" .
                 "</div>");
            break;

        case "result":
//..................................................................................................

            $path = $json["text_path"] ?? "";
            $pos  = strpos($path, "/*"); //marks group

            //ungrouped
            if ($pos === FALSE)
            {
                $blocks = [$json["data"] ?? NULL];
            }

            //grouped
            else
            {
                $blocks = array_get_path($json["data"] ?? NULL, substr($path, 0, $pos));
                $path   = substr($path, $pos + 3);
                if (! is_array($blocks)) $blocks = [];
            };

            $list = $json["tools"] ?? [];
            foreach ($blocks AS $index => $block)
            {
                //match result to call by id
                $id = $list[$index]["id"] ?? "";
                $display_tool(
                    $pending[$id] ?? "",
                    agent::extract_text($block, $path),
                    $list[$index]["error"] ?? FALSE);
                unset($pending[$id]);
            };
            break;

        case "stop":
//..................................................................................................

            echo("<div class=\"agent-" . ($json["data"]["error"] ? "error" : "stop") . "\">" .
                 x($json["data"]["reason"]) .
                 "</div>");
        };
    };

    exit();

case "send":
//..................................................................................................

    if (stre($user))       exit();
    if (blank($ifc_param)) exit();

    $agent = new agent($user, $owner, $id);

    //filter context separator
    $ifc_param = str_replace(CMS_AGENT_CONTEXT_SEPARATOR, "", $ifc_param);

    if (nstre($context)) $ifc_param = $ifc_param . CMS_AGENT_CONTEXT_SEPARATOR . $context;
    echo($agent->send_message($ifc_param, CMS_USER) ?
         "1\n" . (stre($id) ? "$owner.$user." . $agent->id : "") :
         "0\n" . x($agent->error ?? ""));
    exit();

case "interrupt":
//..................................................................................................

    if (nstre($id)) echo(agent::interrupt($owner, $user, $id) ? t("msg_done") : t("msg_error"));
    exit();

case "delete":
//..................................................................................................

    if (stre($id) || ! $operator) break;

    if (agent::delete($owner, $user, $id))
    {
        $agent_process_object("$owner.$user");
        $ifc_response = t("msg_done");
        break;
    };

    $ifc_response = t("msg_error");
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

//permission
ifc_permission([""         => t("access"),
                "operator" => t("operator")]);

//display
//..................................................................................................

$ifc = new ifc(
    $ifc_response,
    $ifc_page,
    NULL,
    ["context" => $context ?? NULL]);

echo("<div class=\"agent-layout\">" .
     "<div>");

//owner selection
//..................................................................................................

if ($operator)
{
    $array = [CMS_SUPERUSER => TRUE];

    //collect owners
    if (
         is_dir(CMS_AGENT_DATA_PATH)
         &&
         (($hdir = opendir(CMS_AGENT_DATA_PATH)) !== FALSE)
       )
    {
        while (($file_object = readdir($hdir)) !== FALSE)
        {
            if (($file_object === ".") || ($file_object === "..")) continue;
            if (! is_dir(CMS_AGENT_DATA_PATH . $file_object))      continue;

            $value = decode_filename($file_object);
            if (isset($array[$value])) continue;

            $array[$value] = TRUE;
        };

        closedir($hdir);
    };

    //display
    $name = permission_get_name($owner, $owner);
    ifc_popover_open((stre($owner) ? t("user") : $name) . "|agent/icon_owner");
    echo("<div class=\"label\">");

    foreach ($array AS $value => $_)
    {
        $name = x(permission_get_name($value, $value));
        if ($value === $owner) $name = "<strong>$name</strong>";
        echo("<a href=\"javascript:ifc_post('select','" . qx($value) . "');\">" .
             $name .
             "</a><br>");
    };

    echo("</div>");
    ifc_popover_close();
};

//user selection
//..................................................................................................

$array     = [];
$has_agent = FALSE;

//collect agents
if (streq($owner, CMS_SUPERUSER)) //own account
{
    $data = new data("#system/permission");
    $data->move("first");

    while ($key = $data->move("next"))
    {
        if (! permission_is_user($key)) continue;

        $value = permission_remove_prefix($key);
        if (! permission_is_agent($value)) continue;

        if (cms_permission("agent.$value", NULL, NULL)) $array[$value] = TRUE;
    };
};

//collect directories
if (
     $operator
     &&
     is_dir($path = CMS_AGENT_DATA_PATH . encode_filename($owner) . "/")
     &&
     (($hdir = opendir($path)) !== FALSE)
   )
{
    while (($file_object = readdir($hdir)) !== FALSE)
    {
        if (($file_object === ".") || ($file_object === "..")) continue;
        if (! is_dir($path . $file_object))                    continue;

        $array[decode_filename($file_object)] = TRUE;
    };

    closedir($hdir);
};

//display
if ($array)
{
    $name = permission_get_name($user, $user);
    ifc_popover_open((stre($user) ? t("agent") : $name) . "|agent/icon_user");
    echo("<div class=\"label\">");

    foreach ($array AS $value => $_)
    {
        $name = x(permission_get_name($value, $value));
        if ($value === $user) $name = "<strong>$name</strong>";
        echo("<a href=\"javascript:ifc_post('select','" . qx("$owner.$value") . "');\">" .
             $name .
             "</a><br>");
    };

    echo("</div>");
    ifc_popover_close();

    $has_agent = TRUE;
};

//chat selection
//..................................................................................................

if (
     nstre($user)
     &&
     is_dir($path = CMS_AGENT_DATA_PATH . encode_filename($owner) . "/" . encode_filename($user) . "/")
     &&
     (($hdir = opendir($path)) !== FALSE)
   )
{
    //collect
    $heap = new class extends \SplMinHeap
    {
        protected function compare(mixed $a, mixed $b): int
        {
            return $b[0] <=> $a[0];
        }
    };

    while (($file_object = readdir($hdir)) !== FALSE)
    {
        if (($file_object === ".") || ($file_object === "..")) continue;
        if (! is_file($path . $file_object))                   continue;
        if (! preg_match("/^(.+)\.json$/i", $file_object, $m)) continue;

        $heap->insert([filemtime($path . $file_object), $m[1]]);
        if ($heap->count() > 100) $heap->extract();
    };

    closedir($hdir);

    //display
    if ($heap->count() > 0)
    {
        ifc_popover_open(t("chat") . "|agent/icon_run");

        $ifc->set(t("command_new"), "button b", "javascript:ifc_post('select','" . qx("$owner.$user.") . "');");
        echo("<div class=\"label\">");

        foreach (array_reverse(iterator_to_array($heap, FALSE)) AS $value)
        {
            $name = x($value[1] . " (" . friendly_date($value[0]) . ")");
            if ($id === $value[1]) $name = "<strong>$name</strong>";
            echo("<a href=\"javascript:ifc_post('select','" . qx("$owner.$user." . $value[1]) . "');\">" .
                 $name .
                 "</a><br>");
        };

        echo("</div>");
        ifc_popover_close();
    };
};

$ifc->param("object", $object);

//no chat
if (stre($id))
{
    echo("<br>");
    $ifc->set(
        $has_agent ?
        (stre($user) ? t("chat_hint_select_agent") : t("chat_hint_message")) :
        t("chat_hint_no_agents"),
        "info");
}

//chat selected
else
{

?><script>

function agent_run_interrupt()
{
    asr_send("<?php echo(q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "interrupt",
                                    "object"      => $object])));?>",
             (value) => ifc_response(value));
};

</script><?php

    //interrupt button
    $ifc->set(t("interrupt_run") . "|agent/command_cancel", "button", "javascript:agent_run_interrupt();");

    //delete button
    if ($operator) $ifc->set(t("delete_chat") . "|agent/command_delete", "button", "#delete");
};

echo("</div>" .

//chat display
//..................................................................................................

     "<div id=\"agent-display\">" .
     "<button id=\"agent-speak\" " .
             "type=\"button\">" .
     "</button>" .
     "</div>");

?><script>

var agent_display       = document.getElementById("agent-display");
var agent_display_from  = -1;
var agent_display_to    = -1;
var agent_display_busy  = false;
var agent_display_timer = null;
var agent_sent_list     = [];
var agent_sent_last     = null;
var agent_speak         = () => {};

function agent_sent(request)
{
    const index = agent_sent_list.indexOf(request);
    if (index >= 0) agent_sent_list.splice(index, 1);
    if ((request === agent_sent_last) || (! agent_sent_list.length))
        document.getElementById("agent-input")?.classList.remove("agent-sending");
};

function agent_display_load(from, to, callback)
{
    asr_send("<?php echo(q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "display",
                                    "object"      => $object,
                                    "from"        => "\x1B%from%",
                                    "to"          => "\x1B%to%"])));?>"
             .replace("%from%", from)
             .replace("%to%",   to),
             (value) =>
    {
        if (! value) { callback(); return; };

        const pos1 = value.indexOf("\n");
        const pos2 = value.indexOf("\n", pos1 + 1);
        const pos3 = value.indexOf("\n", pos2 + 1);

        from = parseInt(value.substring(0, pos1));
        to   = parseInt(value.substring(pos1 + 1, pos2));

        agent_display_from = (agent_display_from === -1) ? from : Math.min(agent_display_from, from);
        agent_display_to   = (agent_display_to   === -1) ? to   : Math.max(agent_display_to,   to);

        const status = document.getElementById("agent-status");
        if (status) status.textContent = value.substring(pos2 + 1, pos3);

        callback(value.substring(pos3 + 1));
    });
};

function agent_display_init()
{
    agent_display_load(-1, -1, (html) =>
    {
        if (html)
        {
            agent_display.insertAdjacentHTML("beforeend", html);
            agent_marked();
            agent_speak(false);
            agent_display_scroll("instant");

            //firefox workaround
            fx_style(agent_display, "display", "block");
        };

        agent_display_reveal();
        setTimeout(agent_display_update, 5000);
    });
};

function agent_display_update()
{
    clearTimeout(agent_display_timer);
    if (document.hidden) return;

    agent_display_load(agent_display_to + 1, -1, (html) =>
    {
        if (html)
        {
            const scroll = (agent_display.scrollHeight - agent_display.scrollTop) <= (agent_display.clientHeight + 5);
            agent_display.insertAdjacentHTML("beforeend", html);
            agent_marked();
            agent_speak(true);
            (html.match(/class="agent-message"/g) || []).forEach(() => agent_sent(agent_sent_list[0]));
            if (scroll) agent_display_scroll();
        };

        agent_display_timer = setTimeout(agent_display_update, 5000);
    });
};

fx_event_listen(document, "visibilitychange", () => { if (! document.hidden) agent_display_update(); });

function agent_display_scroll(behavior = "smooth")
{
    var item = agent_display.lastElementChild;
    if (item && (item.className === "agent-stop")) item = item.previousElementSibling;
    if (! item) return;

    item.scrollIntoView({behavior, block: "start", inline: "nearest", container: "nearest"});
};

function agent_display_reveal()
{
    if (agent_display_busy)            return;
    if (agent_display.scrollTop > 100) return;
    if (agent_display_from <= 0)       return;

    agent_display_busy = true;

    agent_display_load(-1, agent_display_from - 1, (html) =>
    {
        agent_display_busy = false;
        if (! html) return;

        var height = agent_display.scrollHeight;
        agent_display.firstElementChild.insertAdjacentHTML("afterend", html);
        agent_marked();
        agent_speak(false);
        agent_display.scrollTop += agent_display.scrollHeight - height;

        if (agent_display.scrollHeight <= agent_display.clientHeight)
            setTimeout(agent_display_reveal, 1000);
    });
};

fx_event_listen(agent_display, "scroll", agent_display_reveal);

function agent_marked()
{
    if (typeof marked === "undefined") return;

    agent_display.querySelectorAll(".agent-text:not([data-marked])").forEach((el) =>
    {
        el.dataset.marked = true;
        el.innerHTML      = marked.parse(el.innerHTML.replace(/<br[^>]*>/g, "\n"));
    });
};

load_script("pwnc/javascript/marked.js", () =>
{
    marked.use({ walkTokens : (token) => { if (token.type === "code") token.escaped = true; },
                 renderer   : { codespan({text}) { return "<code>" + text + "</code>"; }}});

    agent_marked();
    agent_display_init();
});

</script><?php

//message input
//..................................................................................................

echo("<div>");

if (nstre($user) && ($operator || cms_permission("agent.$user", NULL, NULL)))
{
    echo("<textarea id=\"agent-input\">" .
         "</textarea>" .
         "<div id=\"agent-glow\">" .
         "</div>" .
         "<button id=\"agent-speech\" " .
                 "type=\"button\">" .
         "</button>" .
         "<button id=\"agent-send\" " .
                 "type=\"button\" " .
                 "tabindex=\"-1\">" .
         "</button>" .
         "<div id=\"agent-status\">" .
         "</div>");

    header("Permissions-Policy: microphone=(self), on-device-speech-recognition=(self)");

?><script>

var agent_speech  = document.getElementById("agent-speech");
var agent_input   = document.getElementById("agent-input");
var agent_send    = document.getElementById("agent-send");
var agent_context = document.forms["ifc"].elements["context"];

function agent_get_context()
{
    if ((! window.opener) || window.opener.closed) return "";
    try { var doc = window.opener.document; }
    catch (e) { return ""; };

    const list = [];
    doc.querySelectorAll("FORM[name=\"ifc\"] > INPUT[type=\"hidden\"]:not([data-agent-info=\"ignore/omit\"])")
       .forEach((o) => { list.push(o.name + ": " + o.value); });

    return doc.URL + (list.length ? "\n" + list.join("\n") : "");
};

fx_event_listen(agent_input, "input", function()
{
    if (this.selectionEnd !== this.value.length) return;
    this.scrollTop = this.scrollHeight;
    fx_animation_frame(() => this.scrollTop = this.scrollHeight);
});

fx_event_listen(agent_input, "keydown", function()
{
    if ((event.key !== "Enter") || event.shiftKey) return;

    event.preventDefault();
    agent_send.click();
}, false);

fx_event_listen(agent_send, "click", function()
{
    if (agent_input.value.trim() === "") return;

    const text        = agent_input.value;
    const context     = agent_get_context();
    agent_input.value = "";
    const request     = agent_sent_last = {};

    agent_sent_list.push(request);
    agent_input.classList.add("agent-sending");
    agent_input.dispatchEvent(new Event("send"));

    asr_send("<?php echo(q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "send",
                                    "ifc_param"   => "\x1B%param%",
                                    "object"      => $object,
                                    "context"     => "\x1B%context%",])));?>"
             .replace("%param%",   encodeURIComponent(text))
             .replace("%context%", encodeURIComponent((context === agent_context.value) ? "" : context)),
             (value) =>
    {
        value        = value || "";
        const pos    = value.indexOf("\n");
        const result = value.substring(pos + 1);

        if (value.substring(0, pos) !== "1")
        {
            ifc_response("<?php echo(q(t("msg_error")));?>" + result);
            if (value.substring(0, pos) === "0") agent_sent(request);

            //restore text
            const { selectionStart: start, selectionEnd: end, selectionDirection: dir, scrollHeight: height } = agent_input;
            agent_input.setRangeText(text, 0, 0);
            agent_input.setSelectionRange(start + text.length, end + text.length, dir);
            agent_input.scrollTop += agent_input.scrollHeight - height;
            return;
        };

        agent_context.value = context;
        if (result) ifc_post("select", result);
    });
});

var agent_language = navigator.language;
try
{
    const locale = new Intl.Locale(document.documentElement.lang || navigator.language).maximize();
    if (locale.region) agent_language = locale.language + "-" + locale.region;
}
catch (e) {};

//speech input
if (window.isSecureContext && navigator.mediaDevices)
{
    let SpeechRecognition = (window.SpeechRecognition || window.webkitSpeechRecognition);

    if (SpeechRecognition)
    {
        //query availability
        const check = async (o) =>
        {
            try { return await SpeechRecognition.available(o); }
            catch (e) { return "unavailable"; };
        };

        const option = (typeof SpeechRecognition.available === "function") ?
                       [{ langs: [agent_language], processLocally: true, quality: "dictation" },
                        { langs: [agent_language], processLocally: true, quality: "command"   }] :
                       null;

        //initial button state
        (option ?
        check(option[0])
            .then((v) => ((v === "unavailable") ? check(option[1]) : v))
            .then((v) => ((v === "unavailable") ? check({ langs: [agent_language] }) : v)) :
        Promise.resolve("available"))
            .then((v) => { agent_speech.className = "agent-" + v; });

        let recognition = null;
        let locally     = null;
        let active      = false;
        let success     = false;
        let count       = 0;
        let start       = 0;
        let value       = "";

        fx_event_listen(agent_input, "send", () =>
        {
            start = count;
            value = "";

            if (active)
            {
                try { recognition.abort(); } catch (e) {};
                recognition = null;

                active = false;
                agent_speech.classList.remove("agent-active");
            };
        });

        fx_event_listen(agent_speech, "click", async function()
        {
            //stop recording
            if (active)
            {
                const _recognition = recognition;
                const _success     = success;
                const _locally     = locally;
                let   flag         = false;

                _recognition.onend = () => { flag = true; };
                _recognition.stop();

                setTimeout(() =>
                {
                    if (flag) return;
                    try { _recognition.abort(); } catch (e) {};
                    if (_locally && (! _success)) locally = false;
                }, 1500);

                active = false;
                agent_speech.classList.remove("agent-active");
                return;
            };

            //resolve on-device status
            let status = "unavailable";

            if (option && (locally !== false))
            {
                for (const o of option)
                {
                    status = await check(o);

                    if ((status === "downloadable") || (status === "downloading"))
                    {
                        agent_speech.className = "agent-downloading";

                        //install language pack
                        try { if (await SpeechRecognition.install(o)) status = await check(o); }
                        catch (e) { status = "unavailable"; };
                    };

                    if (status === "available") break;
                };

                locally = (status === "available") ? true : null;
            };

            //cloud fallback
            if (! locally) status = option ? await check({ langs: [agent_language] }) : "available";

            agent_speech.className = "agent-" + status;

            recognition                = new SpeechRecognition();
            recognition.continuous     = true;
            recognition.interimResults = true;
            recognition.lang           = agent_language;
            recognition.processLocally = locally;

            recognition.onstart = (event) =>
            {
                if (event.target !== recognition) return;

                active = true;
                count  = 0;
                start  = 0;
                value  = agent_input.value;

                if (! value.match(/^$|\s$/)) value += " ";
                agent_speech.classList.add("agent-active");
                agent_input.dispatchEvent(new Event("record"));
            };

            recognition.onresult = (event) =>
            {
                if (event.target !== recognition) return;

                success  = true;
                count    = event.results.length;
                let text = "";
                let prev = "";

                for (let i = start; i < count; i++)
                {
                    const regexp  = new RegExp("^" + RegExp.escape(prev));
                    prev  = event.results[i][0].transcript;
                    text += prev.replace(regexp, "");
                };

                agent_input.value = value + text;
            };

            recognition.onend = (event) =>
            {
                if (event.target !== recognition || ! active) return;

                try { recognition.start(); }
                catch (e) { active = false; agent_speech.classList.remove("agent-active"); };
            };

            recognition.onerror = (event) =>
            {
                if (event.target !== recognition) return;

                active = false;
                agent_speech.classList.remove("agent-active");
                ifc_response("<?php echo(q(t("msg_error")));?>" + (event.message || event.error));
            };

            success = false;

            try { recognition.start(); }
            catch (e) { ifc_response("<?php echo(q(t("msg_error")));?>" + (e.message || e.name)); };
        });
    };
};

//speech output
if (window.speechSynthesis && Intl.Segmenter)
{
    const synth      = window.speechSynthesis;
    const sentence   = new Intl.Segmenter(agent_language, { granularity: "sentence" });
    const word       = new Intl.Segmenter(agent_language, { granularity: "word" });
    const queue      = new Set();
    const button     = document.getElementById("agent-speak");
    let enabled      = getcookie("cms_agent_speak_enabled") === "1";
    button.className = "agent-available" + (enabled ? " agent-enabled" : "");

    const stop = (unlock = false) =>
    {
        synth.cancel();
        queue.clear();
        button.className = "agent-available" + (enabled ? " agent-enabled" : "");
        if (unlock && enabled) synth.speak(new SpeechSynthesisUtterance(""));
    };

    agent_speak = (utter) =>
    {
        agent_display.querySelectorAll(".agent-response > .agent-text:not([data-spoken])").forEach((el) =>
        {
            el.dataset.spoken = true;
            if (! utter || ! enabled || agent_speech.classList.contains("agent-active")) return;

            const clone = el.cloneNode(true);
            clone.querySelectorAll("pre, table").forEach((o) => { o.remove(); });
            clone.querySelectorAll("br").forEach((o) => { o.append("\n"); });
            const text = clone.textContent.replace(/https?:\/\/([^\s?#]+)\S*/g, "$1");
            const list = [];

            for (const paragraph of text.split(/\n+/))
            {
                let buffer = "";

                for (const _sentence of sentence.segment(paragraph))
                {
                    const part = (_sentence.segment.length > 200) ?
                                 word.segment(_sentence.segment) : [_sentence];

                    for (const _part of part)
                    {
                        if (buffer && ((buffer.length + _part.segment.length) > 200))
                        {
                            list.push(buffer);
                            buffer = "";
                        };

                        buffer += _part.segment;
                    };
                };

                if (buffer) list.push(buffer);
            };

            const language = agent_language.split("-")[0].toLowerCase();
            const score    = (v) =>
                ((v.lang.replace("_", "-") === agent_language) ? 4 : 0) +
                (v.localService ? 2 : 0) +
                (v.default ? 1 : 0);
            const voice    = synth.getVoices()
                .filter((v)  => (v.lang.replace("_", "-").split("-")[0].toLowerCase() === language))
                .sort((a, b) => (score(b) - score(a)))[0] || null;

            for (const phrase of list.filter((s) => /[\p{L}\p{N}]/u.test(s)))
            {
                const utterance = new SpeechSynthesisUtterance(phrase);
                utterance.lang  = agent_language;
                utterance.voice = voice;
                utterance.rate  = 1.5;

                utterance.onend = utterance.onerror = () =>
                {
                    queue.delete(utterance);
                    if (! queue.size) button.className = "agent-available" + (enabled ? " agent-enabled" : "");
                };

                synth.speak(utterance);
                queue.add(utterance);
                button.className = "agent-available agent-active";
            };
        });
    };

    fx_event_listen(button, "click", () =>
    {
        enabled = ! enabled;
        setcookie("cms_agent_speak_enabled", enabled ? "1" : "0");
        stop(true);
    });

    fx_event_listen(agent_input, "send",   () => stop(true));
    fx_event_listen(agent_input, "record", () => stop());
};

</script><?php

};

echo("</div>");

$ifc->close();