/*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

var dd_object      = null;
var dd_vehicle     = null;
var dd_touched     = null;
var dd_callback    = null;
var dd_left        = 0;
var dd_top         = 0;
var dd_nofx        = false;
var dd_flag        = false;
var dd_scroll_flag = false;
var dd_scroll_x    = 0;
var dd_scroll_y    = 0;

function dd_register(object,
                     type,
                     accept,
                     fixed = false,
                     nofx  = false)
{
    if (typeof object !== "object") object = document.getElementById(object);
    if (object === null)            return false;

    //property
    object.dd_enabled = true;
    object.dd_type    = type;
    object.dd_accept  = accept;
    object.dd_fixed   = fixed;
    object.dd_nofx    = nofx;

    //event
    object.ondragstart   = () => false;
    object.onselectstart = () => false;

    //set title
    if (object.title)    return true; //already set
    if (object.dd_fixed) { if (accept) object.title = "✥ Drop"; }
    else if (accept)                   object.title = "✥ Drag & Drop";
    else                               object.title = "✥ Drag";

    return true;
};

/*
   set callback function

   example:

   function my_callback_function(my_event, my_source, my_target) { ... };
   dd_set_callback(my_callback_function);

   possible values for my_event:

   "activate",        "dblclick", "select",
   "beforedragstart", "dragstart",
   "dragover",        "drag",
   "dropon",          "dropon_alt",
   "drop",            "drop_alt"
*/

function dd_set_callback(callback)
{
    dd_callback = callback;
};

function dd_get_object()
{
    var object = fx_pointer_object();
    if (! object) return null;

    do
    {
        if (object.dd_enabled) return (object === dd_object) ? null : object;
    }
    while (object = object.parentElement);

    return null;
};

function dd_move_vehicle()
{
    fx_move(dd_vehicle, fx_mouse_window_x + 10, fx_mouse_window_y + 5);
};

function dd_scroll()
{
    dd_scroll_flag = true;

    if ((dd_object === null) || ((dd_scroll_x === 0) && (dd_scroll_y === 0)))
    {
        dd_scroll_flag = false;
        return;
    };

    //store position
    var left = fx_window_left;
    var top  = fx_window_top;

    //scroll
    fx_scroll_container.scrollBy({
        left     : dd_scroll_x,
        top      : dd_scroll_y,
        behavior : "instant"});

    //force reflow
    dd_vehicle.getBoundingClientRect();

    //check position change
    fx_update_window_position();

    if ((fx_window_left === left) && (fx_window_top === top))
    {
        dd_scroll_flag = false;
        return;
    };

    //update position
    dd_move_vehicle();

    fx_animation_frame(dd_scroll, 25);
};

function dd_event(event)
{
    switch (event)
    {
    case "mousedown":
//..................................................................................................

        if (dd_object !== null) return; //source already selected
        if (fx_mouse_key !== 1) return; //not left mouse button

        var object = dd_get_object();
        if (object === null)    return; //no source

        //global
        dd_left = fx_mouse_x;
        dd_top  = fx_mouse_y;
        dd_flag = (fx_keyboard_key === 16) || //shift
                  (fx_keyboard_key === 17) || //ctrl
                  (fx_keyboard_key === 18);   //alt

        //set source
        dd_object = object;

        //disable scrolling
        fx_noscroll();
        break;

    case "mousemove":
//..................................................................................................

        if (dd_object === null) return; //no source
        if (dd_object.dd_fixed) return; //fixed

        //start drag action
        if (dd_vehicle === null)
        {
            //element has not been moved away from initial position yet
            if (Math.sqrt(Math.pow(fx_mouse_x - dd_left, 2) +
                          Math.pow(fx_mouse_y - dd_top,  2)) <= 10.0)
                return;

            //callback
            if (dd_callback !== null) dd_callback("beforedragstart", dd_object, null);

            //create vehicle
            dd_vehicle           = document.createElement("div");
            dd_vehicle.className = "dd-vehicle";
            //var rect             = document.body.getBoundingClientRect();

            Object.assign(dd_vehicle.style, {
                contain        : "content",
                height         : "auto",
                //marginLeft     : "-" + rect.left + "px",
                //marginTop      : "-" + rect.top + "px",
                maxHeight      : "200px",
                maxWidth       : "300px",
                opacity        : "0.75",
                overflow       : "clip",
                pointerEvents  : "none",
                position       : "fixed",
                userSelect     : "none",
                width          : "max-content",
                zIndex         : "10000"});

            document.body.appendChild(dd_vehicle);

            //clone selected object
            var clone = dd_object.cloneNode(true);

            //remove id attributes
            [clone, ...clone.querySelectorAll("[id]")].forEach(object => object.removeAttribute("id"));

            //remove scripts
            clone.querySelectorAll("script").forEach(object => object.remove());

            //fill vehicle
            dd_vehicle.appendChild(clone);

            //correct first element
            if (fx_style(dd_vehicle.firstChild, "position") === "absolute")
                fx_style(dd_vehicle.firstChild, "position", "static", true);

            //set cursor
            fx_style(document.documentElement, "cursor", "move", true);

            //disable responsivity
            fx_pointer_block();

            //callback
            if (dd_callback !== null) dd_callback("dragstart", dd_object, null);
        }

        //perform drag action
        else
        {
            //scroll at page border
            if (fx_mouse_window_x < 100)
                dd_scroll_x = fx_mouse_window_x - 100;
            else if (fx_mouse_window_x > (fx_window_width - 100))
                dd_scroll_x = fx_mouse_window_x - fx_window_width + 100;
            else
                dd_scroll_x = 0;

            if (fx_mouse_window_y < 100)
                dd_scroll_y = fx_mouse_window_y - 100;
            else if (fx_mouse_window_y > (fx_window_height - 100))
                dd_scroll_y = fx_mouse_window_y - fx_window_height + 100;
            else
                dd_scroll_y = 0;

            if (! dd_scroll_flag) dd_scroll();

            var object = dd_get_object();

            //clear touched element
            if (
                 (dd_touched !== null)
                 &&
                 (object !== dd_touched)
               )
            {
                if (! dd_nofx) dd_touched.classList.remove("dd-active");
                dd_touched = null;
            };

            //perform drag over action
            if (
                 (object !== null) //object found
                 &&
                 ((object.dd_accept & dd_object.dd_type) !== 0) //potential target
               )
            {
                //set cursor
                fx_style(document.documentElement, "cursor", "default", true);

                //set touched element
                if (object !== dd_touched)
                {
                    dd_touched = object;
                    dd_nofx    = object.dd_nofx;

                    if (! dd_nofx) dd_touched.classList.add("dd-active");
                };

                //callback
                if (dd_callback !== null) dd_callback("dragover", dd_object, object);
            }

            //default
            else
            {
                //set cursor
                fx_style(document.documentElement, "cursor", (object === null) ? "move" : "no-drop", true);

                //callback
                if (dd_callback !== null) dd_callback("drag", dd_object, null);
            };
        };

        //update position
        dd_move_vehicle();
        break;

    case "mouseup":
//..................................................................................................

        if (dd_object === null) return; //no source

        //callback
        if (dd_callback !== null)
        {
            //element is at initial position
            if (
                 (dd_vehicle === null)
                 ||
                 (Math.sqrt(Math.pow(fx_mouse_x - dd_left, 2) +
                            Math.pow(fx_mouse_y - dd_top,  2)) <= 10.0)
               )
            {
                dd_callback(dd_flag ? "select" : "activate", dd_object, null);
            }

            //element was moved away from initial position
            else
            {
                if (dd_object.dd_fixed) break; //fixed

                var object = dd_get_object();

                if ((object !== null) && ((object.dd_accept & dd_object.dd_type) !== 0))
                    dd_callback(dd_flag ? "dropon_alt" : "dropon", dd_object, object);
                else
                    dd_callback(dd_flag ? "drop_alt"   : "drop",   dd_object, null);
            };

        };

    case "mouseleave":
//..................................................................................................

        if (dd_object === null) return; //no source

        //clear source
        dd_object = null;

        //clear vehicle
        if (dd_vehicle !== null)
        {
            document.body.removeChild(dd_vehicle);
            dd_vehicle = null;
        };

        //clear touched element
        if (dd_touched !== null)
        {
            if (! dd_nofx) dd_touched.classList.remove("dd-active");
            dd_touched = null;
        };

        //reenable scrolling
        fx_noscroll(false);

        //reset cursor
        fx_style(document.documentElement, "cursor", false);

        //reenable responsivity
        fx_pointer_block(false);
        break;

    case "dblclick":
//..................................................................................................

        if (fx_mouse_key !== 1) return; //not left mouse button

        var object = dd_get_object();
        if (object === null) return; //no target

        dd_callback("dblclick", object, null);
    };
};

fx_register_callback(dd_event);