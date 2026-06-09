/*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

var defer_done = false;

function defer_process()
{
    if (defer_done) return;
    defer_done = true;

    fx_update_window_position();
    fx_update_window_size();

    var list      = [];
    var _list     = [];

    var object    = null;
    var visible   = false;
    var ratio     = 0;
    var width     = 0;
    var flag      = false;

    var page_x1   = fx_window_left;
    var page_y1   = fx_window_top;
    var page_x2   = page_x1 + fx_window_width;
    var page_y2   = page_y1 + fx_window_height;

    var object_x1 = 0;
    var object_y1 = 0;
    var object_x2 = 0;
    var object_y2 = 0;

    list = Array.from(document.querySelectorAll(
        "AUDIO[data-defer-src], " +
        "IFRAME[data-defer-src], " +
        "IMG[data-defer-src], " +
        "VIDEO[data-defer-src]"));

    for (object of list)
    {
        visible = false;
        width   = 0;

        object_x1 = fx_offset_left(object);

        if (object_x1 < page_x2)
        {
            object_y1 = fx_offset_top(object);

            if (object_y1 < page_y2)
            {
                object_x2 = object_x1 + fx_width(object);

                if (object_x2 > page_x1)
                {
                    object_y2 = object_y1 + fx_height(object);

                    if (object_y2 > page_y1) visible = true;
                };
            };
        };

        if (! object.hasAttribute("data-defer-sizes"))
        {
            ratio = object.hasAttribute("height") ? (object.getAttribute("width") / object.getAttribute("height")) : 0;
            width = Math.max(ratio * object.height, object.width);
        };

        _list.push({ object: object, visible: visible, width: width });
    };

    for (object of _list)
    {
        visible = object.visible;
        width   = object.width;
        object  = object.object;
        flag    = object.hasAttribute("data-defer-srcset");

        if (flag || object.hasAttribute("srcset"))
        {
            if (width === 0)
            {
                object.setAttribute("sizes", object.getAttribute("data-defer-sizes"));
                object.removeAttribute("data-defer-sizes");
            }
            else
            {
                object.setAttribute("sizes", width + "px");
            };
        };

        if (flag)
        {
            object.setAttribute("srcset", object.getAttribute("data-defer-srcset"));
            object.removeAttribute("data-defer-srcset");
        };

        object.setAttribute("src", object.getAttribute("data-defer-src"));
        object.removeAttribute("data-defer-src");

        object.setAttribute("fetchpriority", visible ? "high" : "low");
        object.setAttribute("loading", visible ? "eager" : "lazy");
    };
};

if (document.readyState === "loading")
    fx_event_listen("document_load", defer_process);
else
    fx_event_listen("window_load", defer_process);