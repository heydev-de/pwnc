/*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

//==================================================================================================
//   INITIALIZATION
//==================================================================================================

var agent_speech         = document.getElementById("agent-speech");
var agent_input          = document.getElementById("agent-input");
var agent_send           = document.getElementById("agent-send");
var agent_attach         = document.getElementById("agent-attach");
var agent_context        = document.forms["ifc"].elements["context"];
var agent_attach_display = document.getElementById("agent-file-display");
var agent_attach_input   = document.createElement("input");
var agent_attach_list    = [];

agent_attach_input.type     = "file";
agent_attach_input.multiple = true;

//==================================================================================================
//   CONTEXT
//==================================================================================================

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

//==================================================================================================
//   ATTACHMENT
//==================================================================================================

function agent_is_file(e)
{
    return e.dataTransfer && e.dataTransfer.types.includes("Files");
};

function agent_attach_render()
{
    agent_attach_display.replaceChildren(...agent_attach_list.map((file, index) =>
    {
        const item   = document.createElement("span");
        const remove = document.createElement("button");

        item.textContent   = file.name;
        remove.type        = "button";
        remove.textContent = "×";

        fx_event_listen(remove, "click", () =>
        {
            agent_attach_list.splice(index, 1);
            agent_attach_render();
        });

        item.append(remove);
        return item;
    }));
};

function agent_attach_add(list)
{
    list = agent_attach_list.concat(Array.from(list));

    //check size limit
    if (agent_attach_limit && (list.reduce((total, file) => total + file.size, 0) > agent_attach_limit))
    {
        ifc_response(agent_text.msg_error + agent_text.err_upload_limit);
        return;
    };

    agent_attach_list = list;
    agent_attach_render();
};

fx_event_listen(agent_attach, "click", () =>
    agent_attach_input.click());

fx_event_listen(agent_attach_input, "change", () =>
{
    agent_attach_add(agent_attach_input.files);
    agent_attach_input.value = "";
});

//file drop
fx_event_listen(agent_input, "dragover", (e) =>
{
    if (! agent_is_file(e)) return;

    e.preventDefault();
    agent_input.classList.add("agent-drop");
}, false);

fx_event_listen(agent_input, "dragleave", () =>
    agent_input.classList.remove("agent-drop"));

fx_event_listen(agent_input, "drop", (e) =>
{
    if (! agent_is_file(e)) return;

    e.preventDefault();
    agent_input.classList.remove("agent-drop");
    agent_attach_add(e.dataTransfer.files);
}, false);

//==================================================================================================
//   INPUT
//==================================================================================================

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

//==================================================================================================
//   SEND
//==================================================================================================

fx_event_listen(agent_send, "click", function()
{
    if ((agent_input.value.trim() === "") && ! agent_attach_list.length) return;

    const text    = agent_input.value;
    const file    = agent_attach_list;
    const context = agent_get_context();
    const request = agent_sent_last = {};

    agent_input.value = "";
    agent_attach_list = [];
    agent_attach_render();

    agent_sent_list.push(request);
    agent_input.classList.add("agent-sending");
    agent_input.dispatchEvent(new Event("send"));

    //request data
    const data = { ifc_param: text, context: (context === agent_context.value) ? "" : context };
    let   body = data;
    if (file.length)
    {
        body = new FormData();
        Object.entries(data).forEach(([key, value]) => body.append(key, value));
        file.forEach((item) => body.append("ifc_file1[]", item));
    };

    asr_send(
        agent_send_url,
        (value) =>
        {
            value        = value || "";
            const pos    = value.indexOf("\n");
            const result = value.substring(pos + 1);

            if (value.substring(0, pos) !== "1")
            {
                ifc_response(agent_text.msg_error + result);
                if (value.substring(0, pos) === "0") agent_sent(request);

                //restore attachments
                agent_attach_list = file.concat(agent_attach_list);
                agent_attach_render();

                //restore text
                const { selectionStart:     start,
                        selectionEnd:       end,
                        selectionDirection: dir,
                        scrollHeight:       height } = agent_input;
                agent_input.setRangeText(text, 0, 0);
                agent_input.setSelectionRange(start + text.length, end + text.length, dir);
                agent_input.scrollTop += agent_input.scrollHeight - height;
                return;
            };

            agent_context.value = context;
            const target = result.split("\n", 1)[0];
            if (target) s(target);
        },
        body);
});

//==================================================================================================
//   LANGUAGE
//==================================================================================================

var agent_language = navigator.language;
try
{
    const locale = new Intl.Locale(document.documentElement.lang || navigator.language).maximize();
    if (locale.region) agent_language = locale.language + "-" + locale.region;
}
catch (e) {};

//==================================================================================================
//   SPEECH INPUT
//==================================================================================================

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

        //update text
        const input_update = (text = agent_input.value) =>
        {
            start = count;
            value = agent_input.value = text;
            if (! value.match(/^$|\s$/)) value += " ";
        };

        //voice commands
        let   command_timer = null;
        const command_wake  = agent_text.voice_command_wake + " ";
        const command_list  = [
            {
                cmd: agent_text.voice_command_send,
                run: () => agent_send.click()
            }, {
                cmd: agent_text.voice_command_stop,
                run: () => recognition_stop()
            }, {
                cmd: agent_text.voice_command_delete_sentence,
                run: () => input_update(agent_input.value.trimEnd().replace(/^(.*\p{Sentence_Terminal}\s)?.*$/su, "$1"))
            }, {
                cmd: agent_text.voice_command_delete,
                run: () => input_update("")
            }].map((v) => (v.cmd = (command_wake + v.cmd).toLowerCase(), v));

        const command_check = () =>
        {
            clearTimeout(command_timer);

            const text    = agent_input.value.trimEnd();
            const text_lc = text.toLowerCase();
            const command = command_list.find((v) => text_lc.endsWith(v.cmd));
            if (! command) return;

            command_timer = setTimeout(() =>
            {
                if (agent_input.value.trimEnd() !== text) return;
                input_update(text.substring(0, text.length - command.cmd.length).trimEnd());
                command.run();
            }, 1500);
        };

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

        const recognition_stop = () =>
        {
            if (! active) return;
            clearTimeout(command_timer);

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
        };

        fx_event_listen(agent_input, "input", () =>
        {
            if (active) input_update();
        });

        fx_event_listen(agent_input, "send", () =>
        {
            clearTimeout(command_timer);

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
            if (active) { recognition_stop(); return; };

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
                    const transcript = event.results[i][0].transcript;
                    text += transcript.startsWith(prev) ? transcript.slice(prev.length) : transcript;
                    prev  = transcript;
                };

                agent_input.value = value + text;
                command_check();
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

                clearTimeout(command_timer);

                active = false;
                agent_speech.classList.remove("agent-active");
                ifc_response(agent_text.msg_error + (event.message || event.error));
            };

            success = false;

            try { recognition.start(); }
            catch (e) { ifc_response(agent_text.msg_error + (e.message || e.name)); };
        });
    };
};

//==================================================================================================
//   SPEECH OUTPUT
//==================================================================================================

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
                //utterance.rate  = 1.5;

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