/*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

var mv_object        = null;
var mv_left          = 0;
var mv_top           = 0;
var mv_offset_left   = 0;
var mv_offset_top    = 0;
var mv_fixed         = false;
var mv_flag          = false;
var mv_scroll_flag   = false;
var mv_scroll_x      = 0;
var mv_scroll_y      = 0;

function mv_register(object)
{
    if (typeof object != "object") object = document.getElementById(object);
    if (object === null)           return false;

    //event
    object.ondragstart   = () => false;
    object.onselectstart = () => false;

    //mark object
    object.mv_enabled = true;

    //set title
    if (! object.title) object.title = "✥ Move";

    return true;
};

function mv_get_object()
{
    var object = fx_pointer_object();
    if (! object) return null;

    do
    {
        if (object.mv_enabled) return (object === mv_object) ? null : object;
    }
    while (object = object.parentElement);

    return null;
};

function mv_move_object()
{
    //position
    var x = fx_mouse_x - mv_offset_left;
    var y = fx_mouse_y - mv_offset_top;

    //adjust fixed element
    if (mv_fixed)
    {
        x -= fx_window_left;
        y -= fx_window_top;
    };

    fx_move(mv_object, x, y);
};

function mv_scroll()
{
    mv_scroll_flag = true;

    if ((mv_object === null) || ((mv_scroll_x === 0) && (mv_scroll_y === 0)))
    {
        mv_scroll_flag = false;
        return;
    };

    //store position
    var left = fx_window_left;
    var top  = fx_window_top;

    //scroll
    fx_scroll_container.scrollBy({
        left     : mv_scroll_x,
        top      : mv_scroll_y,
        behavior : "instant"});

    //check position change
    var display = fx_style(mv_object, "display");
    fx_style(mv_object, "display", "none", true);
    fx_update_window_position();
    fx_style(mv_object, "display", display);

    if ((fx_window_left === left) && (fx_window_top  === top))
    {
        mv_scroll_flag = false;
        return;
    };

    //update position
    mv_move_object();

    fx_animation_frame(mv_scroll, 25);
};

function mv_event(event)
{
    switch (event)
    {
    case "mousedown":
//..................................................................................................

        if (mv_object !== null) return; //object already selected
        if (fx_mouse_key !== 1) return; //not left mouse button

        var object = mv_get_object();
        if (object === null)    return; //no target

        //global
        mv_left        = fx_mouse_x;
        mv_top         = fx_mouse_y;
        mv_offset_left = fx_mouse_x - fx_left(object, true);
        mv_offset_top  = fx_mouse_y - fx_top(object,  true);
        mv_fixed       = fx_style(object, "position") === "fixed";

        //adjust fixed element
        if (mv_fixed)
        {
            mv_offset_left -= fx_window_left;
            mv_offset_top  -= fx_window_top;
        };

        //set object
        mv_object = object;

        //disable scrolling
        fx_noscroll();
        break;

    case "mousemove":
//..................................................................................................

        if (mv_object === null)  return; //no object

        //perform move action
        if (mv_flag)
        {
            //scroll at page border
            if (fx_mouse_window_x < 100)
                mv_scroll_x = fx_mouse_window_x - 100;
            else if (fx_mouse_window_x > (fx_window_width - 100))
                mv_scroll_x = fx_mouse_window_x - fx_window_width + 100;
            else
                mv_scroll_x = 0;

            if (fx_mouse_window_y < 100)
                mv_scroll_y = fx_mouse_window_y - 100;
            else if (fx_mouse_window_y > (fx_window_height - 100))
                mv_scroll_y = fx_mouse_window_y - fx_window_height + 100;
            else
                mv_scroll_y = 0;

            if (! mv_scroll_flag) mv_scroll();
        }

        //start move action
        else
        {
            //element has not been moved away from initial position yet
            if (Math.sqrt(Math.pow(fx_mouse_x - mv_left, 2) +
                          Math.pow(fx_mouse_y - mv_top,  2)) <= 10.0)
                return;

            //set move flag
            mv_flag = true;

            //set cursor
            fx_style(document.documentElement, "cursor", "move", true);

            //disable responsivity
            fx_pointer_block();
        };

        //update position
        mv_move_object();
        break;

    case "mouseup":
    case "mouseleave":
//..................................................................................................

        if (mv_object === null) return; //no object

        //clear source
        mv_object = null;

        //clear move flag
        mv_flag = false;

        //reenable scrolling
        fx_noscroll(false);

        //reset cursor
        fx_style(document.documentElement, "cursor", false);

        //reenable responsivity
        fx_pointer_block(false);
    };
};

fx_register_callback(mv_event);