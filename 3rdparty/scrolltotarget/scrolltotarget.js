/*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

function scrolltotarget_init()
{
    var list       = document.getElementsByTagName("a");
    var object     = null;
    var data       = "";
    var fragment   = "";
    var url_source = "";
    var url_target = "";
    var c          = list.length;
    var func       = function(e) { e.preventDefault(); scrolltotarget(this); };

    for (var i = 0; i < c; i++)
    {
        object = list.item(i);

        if (object.hasAttribute("data-scrolltotarget"))
        {
            data = object.getAttribute("data-scrolltotarget");
            if (data.match(/(^|\s)(off|no|false)(\s|$)/i)) continue;
        };

        fragment = object.href.replace(/^.*#/, "");

        if (fragment != "")
        {
            url_source = window.location.href.replace(/#.*$/, "");
            url_target = object.href.replace(/#.*$/, "");

            if (url_target == "")
                object.href = url_source + "#" + fragment;

            if ((url_target == "") || (url_target == url_source))
                fx_event_listen(object, "click", func, false);
        };
    };
};

function scrolltotarget(object)
{
    var id     = object.href.substr(object.href.lastIndexOf("#") + 1);
    var target = document.getElementById(id);

    fx_scrollto(target);
};