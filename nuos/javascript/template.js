/*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

var tp_ctrl_opt_value   = 0;
var tp_ctrl_opt_img_url = "";

function tp_event()
{
    var list   = document.querySelectorAll(".tp-dd, .tp-dd100");
    var object = null;
    for (object of list)
    {
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
    for (object of list)
    {
        fx_event_listen(
            object,
            "contextmenu",
            function(e) { if (e.pointerType === "touch") e.preventDefault(); },
            false);
    };

    //suppress tooltip text in settings
    list = document.querySelectorAll(".module-settings");
    for (object of list) object.title = "";
    return;
};

function tp_beforedragstart()
{
    //hide control pad
    fx_style(document.getElementById("tp-marker"), "display", "none", true);
    fx_style(document.getElementById("tp-ctrl"), "display", "none", true);

    //remove focus
    document.activeElement.blur();
};

function tp_drop()
{
    //show control pad
    fx_style(document.getElementById("tp-marker"), "display", false);
    fx_style(document.getElementById("tp-ctrl"), "display", false);
};

function tp_ctrl_opt_set(value)
{
    tp_ctrl_opt_value = value;
    var object = document.getElementById("tp-ctrl-opt-apply");
    if (object !== null) object.click();
};

function tp_ctrl_opt_switch(value)
{
    if ((tp_ctrl_opt_value & value) == 0)
        tp_ctrl_opt_value |= value;
    else
        tp_ctrl_opt_value &= ~value;
    tp_ctrl_opt_img();
};

function tp_ctrl_opt_apply(url)
{
    url = url.replace(/%value%/, tp_ctrl_opt_value);
    url = url.replace(/%left%/,  fx_position_left());
    url = url.replace(/%top%/,   fx_position_top());
    location.replace(url);
};

function tp_ctrl_opt_img()
{
    var array1 = [     4,       8,    32768,      32,          64,     128,      1,         16,        2,        256,     512,     1024,    2048,     8192,   4096,   16384];
    var array2 = ["text", "value", "switch", "image", "thumbnail", "media", "href", "download", "plugin", "template", "group", "repeat", "shift", "cblock", "calt", "debug"];

    for (var i = 0; i < 16; i++)
        fx_change_image(document.getElementById("tp-ctrl-opt-" + i).firstElementChild,
                        tp_ctrl_opt_img_url + "button_" + array2[i] +
                        (((tp_ctrl_opt_value & array1[i]) == 0) ? "_disabled" : "") + ".svg");
};

function tp_flp(id)
{
    var object = document.getElementById("tp-dd-" + id);
    if (object === null) return false;

    if ((fx_keyboard_key === 16) || //shift
        (fx_keyboard_key === 17) || //ctrl
        (fx_keyboard_key === 18))   //alt
    {
        var list    = document.getElementsByClassName("tp-dd100");
        var _object = null;
        var parent  = null;
        var flag    = false;
        var _flag   = false;

        for (var i = 0; i < list.length; i++)
        {
            _object = list.item(i);
            _flag   = false;

            if (flag)
            {
                parent = _object;

                while ((parent = parent.parentElement) !== null)
                {
                    if (parent === object)
                    {
                        _flag = true;
                        break;
                    };
                };
            }
            else
            {
                if (_object === object)
                {
                    flag = _flag = true;
                }
                else
                {
                    parent = object;

                    while ((parent = parent.parentElement) !== null)
                    {
                        if (parent === _object)
                        {
                            _flag = true;
                            break;
                        };
                    };
                };
            };

            _flag ? _object.removeAttribute("data-tp-flp-on") :
                _object.setAttribute("data-tp-flp-on", 1);
        };

        fx_scrollto(object);
    }
    else
    {
        object.hasAttribute("data-tp-flp-on") ?
        object.removeAttribute("data-tp-flp-on") :
        object.setAttribute("data-tp-flp-on", 1);
    };

    tp_flp_store();
    return false;
};

function tp_flp_store()
{
    var list    = document.getElementsByClassName("tp-dd100");
    var object  = null;
    var id      = "";
    var value   = "/";

    for (var i = 0; i < list.length; i++)
    {
        object  = list.item(i);
        if (! object.hasAttribute("data-tp-flp-on")) continue;
        id      = object.id.substr(6);
        value  += id + "/";
    };

    setcookie("cms_tp_flp_value", value);
};

function tp_flp_restore(content_index)
{
    setTimeout(() => document.documentElement.classList.add("tp-flp-restored"), 50);

    //check page id
    var value = getcookie("cms_tp_flp_id");
    setcookie("cms_tp_flp_id", content_index);
    if (value !== content_index) { delcookie("cms_tp_flp_value"); return; };

    value      = getcookie("cms_tp_flp_value");
    var list   = document.getElementsByClassName("tp-dd100");
    var object = null;
    var id     = "";
    var flag   = false;

    for (var i = 0; i < list.length; i++)
    {
        object = list.item(i);
        id     = object.id.substr(6);
        flag   = value.indexOf("/" + id + "/") === -1;
        (flag) ? object.removeAttribute("data-tp-flp-on") : object.setAttribute("data-tp-flp-on", 1);
    };
};