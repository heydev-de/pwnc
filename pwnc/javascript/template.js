/*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

var tp_l_title = "";
var tp_l_flip  = "";
var tp_l_sync  = "";

var tp_ctrl_opt_value = 0;
var tp_ctrl_opt_data  = [];

var tp_code    = {};
var tp_command = {};
var tp_dd      = {};

function tp_event()
{
    tp_build(); //hydrate

    if (typeof dd_set_callback === "function") //edit mode
    {
        dd_set_callback(tp_dd_event);
        document.documentElement.classList.add("tp-touchbar");
    };

    let list = document.querySelectorAll(".tp-dd, .tp-dd100");
    for (let object of list)
    {
        const type = object.dataset.tpDdType;
        dd_register(object.id, type, tp_code[type][2]);

        fx_event_listen(
            object,
            "mouseover",
            function(e) { e.stopPropagation(); this.setAttribute("data-tp-hover", 1); });
        fx_event_listen(
            object,
            "mouseout",
            function(e) { e.stopPropagation(); this.removeAttribute("data-tp-hover"); });
    };

    //suppress context menu on touch screen
    list = document.querySelectorAll(".tp-edt > BUTTON");
    for (let object of list)
        fx_event_listen(
            object,
            "contextmenu",
            function(e) { if (e.pointerType === "touch") e.preventDefault(); },
            false);

    //suppress tooltip
    list = document.querySelectorAll(".module-settings");
    for (let object of list) object.title = "";
};

function tp_dd_event(event,
                     source,
                     target)
{
    switch (event)
    {
    case "dblclick":

        document.getElementById("tp-edt-a-" + source.dataset.tpId).click();
        break;

    case "beforedragstart":

        tp_dd_beforedragstart();
        break;

    case "dropon":
    case "dropon_alt":

        tp_dd_drop();

        var action = tp_dd[(event === "dropon") ? "move" : "duplicate"];
        if (action) (new Function(tp_action(action, source, target)))();
        break;

    case "drop":
    case "drop_alt":

        tp_dd_drop();
    };
};

function tp_ctrl_display(value)
{
    for (const id of ["tp-marker", "tp-ctrl"])
        fx_style(id, "display", value, value !== false);
};

function tp_dd_beforedragstart()
{
    tp_ctrl_display("none");
    document.activeElement.blur();
};

function tp_dd_drop()
{
    tp_ctrl_display(false);
};

function tp_build()
{
    const list = document.querySelectorAll(":is(.tp-dd, .tp-dd100)[data-tp-cmd]");

    for (let node of list)
    {
        const id   = node.dataset.tpId;
        node.title = tp_l_title;

        node.dataset.tpValue     ??= "";
        node.dataset.tpReference ??= "";

        //element container
        const container     = document.createElement("div");
        container.className = "tp-edt";

        //toggle visibility
        if (node.classList.contains("tp-dd100"))
        {
            const flip     = document.createElement("button");
            flip.type      = "button";
            flip.title     = tp_l_flip;
            flip.className = "tp-flp";
            flip.onclick   = function(e)
            {
                //same guards as the command buttons
                e.preventDefault();
                e.stopPropagation();

                tp_flp(id);
            };
            container.appendChild(flip);
        };

        //primary command
        const code = tp_code[node.dataset.tpDdType];

        if (code)
            container.appendChild(tp_button(
                node, code[0], code[1], node.dataset.tpPath, "tp-edt-a-" + id,
                (node.dataset.tpTitle ?? node.dataset.tpType) +
                (node.dataset.tpReference ? " (" + tp_l_sync + ")" : "")));

        //extended commands
        const command     = node.dataset.tpCmd.split(" ");
        const display     = document.createElement("div");
        display.id        = "tp-edt-" + id;
        display.title     = "";
        display.className = "tp-edt-dsp";

        for (const [name, value] of Object.entries(tp_command))
        {
            if (command.includes(name))
            {
                display.appendChild(tp_button(node, value[0], value[2], value[1]));
                if (value[3]) display.appendChild(document.createElement("br"));
            };
        };

        //reposition activation link
        if (["href", "download"].includes(node.dataset.tpType))
        {
            const link = node.querySelector(":scope > A");
            if (link) display.insertBefore(link, display.firstChild);
        };

        container.appendChild(display);

        //display name
        if (node.dataset.tpName)
        {
            const name       = document.createElement("div");
            name.className   = "tp-name";
            name.textContent = node.dataset.tpName;
            container.appendChild(name);
        };

        node.insertBefore(container, node.firstChild);
    };

    const container = document.getElementById("tp-ctrl-opt-switch");
    const apply     = document.getElementById("tp-ctrl-opt-apply");

    tp_ctrl_opt_data.forEach(([type, , title], i) =>
    {
        const button   = document.createElement("button");
        button.id      = "tp-ctrl-opt-" + i;
        button.type    = "button";
        button.title   = title;
        button.onclick = () => tp_ctrl_opt_switch(type);
        button.appendChild(document.createElement("img"));
        container.insertBefore(button, apply);
    });

    tp_ctrl_opt_update();
};

function tp_button(object,
                   action,
                   image,
                   title,
                   id         = null,
                   data_title = null)
{
    //button
    const button = document.createElement("button");
    button.type  = "button";
    button.title = title;
    if (id         !== null) button.id            = id;
    if (data_title !== null) button.dataset.title = data_title;

    //image
    const img  = document.createElement("img");
    img.src    = image[0];
    img.width  = image[1];
    img.height = image[2];
    img.alt    = image[3];
    button.appendChild(img);

    //action
    const func     = new Function(tp_action(action, object));
    button.onclick = function(e)
    {
        e.preventDefault();
        e.stopPropagation();

        func();
    };

    return button;
};

function tp_action(code,
                   source,
                   target = source)
{
    const value = (target === source) ? source.dataset.tpValue : target.dataset.tpPath;
    return code
        .replace(/%index%/g,     encodeURIComponent(source.dataset.tpIndex))
        .replace(/%path%/g,      encodeURIComponent(source.dataset.tpPath))
        .replace(/%type%/g,      encodeURIComponent(source.dataset.tpType))
        .replace(/%reference%/g, encodeURIComponent(source.dataset.tpReference))
        .replace(/%value%/g,     encodeURIComponent(value))
        .replace(/%id%/g,        encodeURIComponent(target.dataset.tpId));
};

function tp_ctrl_opt_update()
{
    tp_ctrl_opt_data.forEach(([type, , image_on, image_off], i) =>
    {
        fx_change_image(
            document.getElementById("tp-ctrl-opt-" + i).firstElementChild,
            (tp_ctrl_opt_value & type) ? image_on : image_off);
    });
};

function tp_ctrl_opt_set(value)
{
    tp_ctrl_opt_value = value;
    document.getElementById("tp-ctrl-opt-apply").click();
};

function tp_ctrl_opt_switch(value)
{
    tp_ctrl_opt_value ^= value;
    tp_ctrl_opt_update();
};

function tp_ctrl_opt_apply(url)
{
    location.replace(url
        .replace("%value%", tp_ctrl_opt_value)
        .replace("%left%",  fx_position_left())
        .replace("%top%",   fx_position_top()));
};

function tp_flp(id)
{
    const object = document.getElementById("tp-dd-" + id);

    //shift, ctrl, alt
    if ([16, 17, 18].includes(fx_keyboard_key))
    {
        for (const node of document.getElementsByClassName("tp-dd100"))
            node.toggleAttribute("data-tp-flp-on", ! (node.contains(object) || object.contains(node)));
        fx_scrollto(object);
    }
    else if (! object.toggleAttribute("data-tp-flp-on"))
    {
        fx_scrollto(object);
    };

    tp_flp_store();
};

function tp_flp_store()
{
    const list  = document.querySelectorAll(".tp-dd100[data-tp-flp-on]");
    const id    = Array.from(list, node => node.dataset.tpId);
    const value = id.length ? "/" + id.join("/") + "/" : "";
    setcookie("cms_tp_flp_value", value);
};

function tp_flp_restore(content_index)
{
    setTimeout(() => document.documentElement.classList.add("tp-flp-restored"), 50);

    //check page id
    let value = getcookie("cms_tp_flp_id");
    setcookie("cms_tp_flp_id", content_index);
    if (value !== content_index) { delcookie("cms_tp_flp_value"); return; };

    value      = getcookie("cms_tp_flp_value");
    const list = document.getElementsByClassName("tp-dd100");
    for (const node of list)
        node.toggleAttribute("data-tp-flp-on", value.includes("/" + node.dataset.tpId + "/"));
};