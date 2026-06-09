/*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

//==================================================================================================
//   ANIMATION
//==================================================================================================

function fx_animation_frame(callback,
                            delay = 0)
{
    if (typeof callback === "string") callback = new Function(callback);

    if (delay > 0) return setTimeout(function() { requestAnimationFrame(callback); }, delay);
    return requestAnimationFrame(callback);
};

//==================================================================================================
//   ELEMENT MANIPULATION
//==================================================================================================

function fx_move(object,
                 left,
                 top)
{
    if (typeof object !== "object") object = document.getElementById(object);
    if (object === null)            return;

    left      = Math.floor(left);
    top       = Math.floor(top);
    var style = object.style;
    var _left = parseInt(style.left) || 0;
    var _top  = parseInt(style.top) || 0;

    if (_left !== left) style.left = left + "px";
    if (_top !== top)   style.top  = top + "px";
};

function fx_style(object,
                  property,
                  value    = null,
                  priority = false)
{
    if (typeof object !== "object") object = document.getElementById(object);
    if (object === null)            return false;

    //remove value
    if ((value === "") || (value === false))
    {
        object.style.removeProperty(property);
        return true;
    };

    //set value
    if (value !== null)
    {
        object.style.setProperty(property, value, priority ? "important" : "");
        return true;
    };

    //retrieve value
    return getComputedStyle(object).getPropertyValue(property);
};

function fx_visible(object,
                    set = null)
{
    if (typeof object !== "object") object = document.getElementById(object);
    if (object === null)            return false;

    if (set !== null) return fx_style(object, "visibility", set ? "visible" : "hidden");
    return fx_style(object, "visibility") === "visible";
};

function fx_change_image(object,
                         image_url)
{
    if (typeof object !== "object") object = document.getElementById(object);
    if (object === null)            return;

    if (object.tagName === "IMG") object.src = image_url;
};

//==================================================================================================
//   WINDOW MANIPULATION
//==================================================================================================

function fx_scrollto(object)
{
    if (typeof object !== "object") object = document.getElementById(object);
    if (object === null)            return;

    //object.scrollIntoView({ behavior: "smooth", block: "start" });

    var _object = object;
    while (_object !== fx_scroll_container)
    {
        //if (fx_style(_object, "position") === "fixed") return;
        if ((_object = _object.offsetParent) === null) return;
    };

    var frames      = 200;
    var interval    = 5;
    var damping     = interval / 2;
    var pifraction  = Math.PI / frames;
    var delay       = 0;

    var page_left   = fx_window_left;
    var page_top    = fx_window_top;
    var target_left = fx_left(object);
    var target_top  = fx_top(object) - fx_window_height / 5;
    var leap_left   = (target_left - page_left) / frames;
    var leap_top    = (target_top - page_top) / frames;

    for (var frame = 0; frame < frames; frame++)
    {
        delay     += interval + damping * Math.sin(Math.PI + pifraction * frame);
        page_left += leap_left;
        page_top  += leap_top;

        fx_animation_frame(() => fx_scroll_container.scrollTo(page_left, page_top), delay);
    };
};

function fx_adjust_window(object = window)
{
    if ((object !== object.window) || (! object.opener)) return;

    var size = fx_document_size(object);
    if (! size) return;

    var w_current = object.outerWidth;
    var h_current = object.outerHeight;

    var w_screen  = screen.availWidth;
    var h_screen  = screen.availHeight;

    var w_target  = Math.min(w_screen, Math.max(900, w_current, size.width  + w_current - object.innerWidth));
    var h_target  = Math.min(h_screen, Math.max(600, h_current, size.height + h_current - object.innerHeight));

    var l_target  = Math.max(0, Math.min(object.screenX, w_screen - w_target));
    var t_target  = Math.max(0, Math.min(object.screenY, h_screen - h_target));

    try {
        if ((w_target !== w_current) || (h_target !== h_current))
            object.resizeTo(w_target, h_target);

        if ((l_target !== object.screenX) || (t_target !== object.screenY))
            object.moveTo(l_target, t_target);
    } catch(e) {};

    object.focus();
};

//==================================================================================================
//   ELEMENT POSITIONING
//==================================================================================================

function fx_left(object,
                 relative = false)
{
    if (typeof object !== "object") object = document.getElementById(object);
    if (object === null)            return 0;

    if (relative) return object.offsetLeft;
    return fx_offset_left(object, true);
};

function fx_top(object,
                relative = false)
{
    if (typeof object !== "object") object = document.getElementById(object);
    if (object === null)            return 0;

    if (relative) return object.offsetTop;
    return fx_offset_top(object, true);
};

function fx_offset_left(object,
                        no_cropping = false)
{
    if (typeof object !== "object") object = document.getElementById(object);
    if (object === null)            return 0;

    var rect = object.getBoundingClientRect();
    var left = rect.left;

    if (no_cropping) return left + fx_window_left;

    while ((object = object.parentElement) !== null)
    {
        rect = object.getBoundingClientRect();
        left = Math.max(left, rect.left);
    };

    return left + fx_window_left;
};

function fx_offset_top(object,
                       no_cropping = false)
{
    if (typeof object !== "object") object = document.getElementById(object);
    if (object === null)            return 0;

    var rect = object.getBoundingClientRect();
    var top  = rect.top;

    if (no_cropping) return top + fx_window_top;

    while ((object = object.parentElement) !== null)
    {
        rect = object.getBoundingClientRect();
        top  = Math.max(top, rect.top);
    };

    return top + fx_window_top;
};

//==================================================================================================
//   ELEMENT DIMENSIONS
//==================================================================================================

function fx_width(object,
                  no_cropping = false)
{
    if (typeof object !== "object") object = document.getElementById(object);
    if (object === null)            return 0;

    var rect = object.getBoundingClientRect();

    if (no_cropping) return rect.width;

    var left  = rect.left;
    var right = rect.right;

    while ((object = object.parentElement) !== null)
    {
        rect  = object.getBoundingClientRect();
        left  = Math.max(left, rect.left);
        right = Math.min(right, rect.right);
    };

    return Math.max(0, right - left);
};

function fx_height(object,
                   no_cropping = false)
{
    if (typeof object !== "object") object = document.getElementById(object);
    if (object === null)            return 0;

    var rect = object.getBoundingClientRect();

    if (no_cropping) return rect.height;

    var top    = rect.top;
    var bottom = rect.bottom;

    while ((object = object.parentElement) !== null)
    {
        rect   = object.getBoundingClientRect();
        top    = Math.max(top, rect.top);
        bottom = Math.min(bottom, rect.bottom);
    };

    return Math.max(0, bottom - top);
};

//==================================================================================================
//   WINDOW POSITIONING
//==================================================================================================

function fx_position_left()
{
    return parseFloat((100 / fx_document_width * fx_window_left).toFixed(2));
};

function fx_position_top()
{
    return parseFloat((100 / fx_document_height * fx_window_top).toFixed(2));
};

//==================================================================================================
//   DOCUMENT DIMENSIONS
//==================================================================================================

function fx_document_size(object = window)
{
    var body = object.document.body;
    if (! body) return null;

    var max_x = 0, max_y = 0, flag = false;

    var style = object.getComputedStyle(body);
    var _left = parseFloat(style.marginLeft) || 0;
    var _top  = parseFloat(style.marginTop)  || 0;
    var stack = [{ parent : body, left : _left, top : _top }];

    while (stack.length > 0)
    {
        var current  = stack.pop();
        var children = current.parent.children;
        var length   = children.length;

        for (var i = 0; i < length; i++)
        {
            var element = children[i];
            if (! element.offsetParent) continue;

            var _left  = current.left + element.offsetLeft;
            var _top   = current.top  + element.offsetTop;
            var width  = element.offsetWidth;
            var height = element.offsetHeight;

            var temp = _left + width;
            if (temp > max_x) max_x = temp;

            temp = _top + height;
            if (temp > max_y) max_y = temp;

            flag = true;

            if (element.childElementCount === 0) continue;

            var style = object.getComputedStyle(element);
            if ((style.transform !== "none") && (style.transform !== "")) continue;

            if ((style.width === "auto") || (style.height === "auto"))
                stack.push({ parent : element,
                             left   : _left,
                             top    : _top });
        };
    };

    return flag ?
           { width  : Math.ceil(max_x),
             height : Math.ceil(max_y) } :
           null;
};

//==================================================================================================
//   SWIPE FUNCTIONALITY
//==================================================================================================-

function fx_swipe(object,
                  callback)
{
    if (typeof object != "object") object = document.getElementById(object);
    if (object == null)            return;

    var active = false;
    var ignore = false;
    var x1     = 0, y1 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0;

    var execute_swipe = function()
    {
        active = false;

        var diff_x = x2 - x1;
        var diff_y = y2 - y1;
        var abs_x  = Math.abs(diff_x);
        var abs_y  = Math.abs(diff_y);
        var dir    = "";

        if ((abs_x > abs_y) && (abs_x > 20)) dir = (diff_x > 0) ? "r" : "l";
        else if (abs_y > 20)                 dir = (diff_y > 0) ? "d" : "u";

        if ((dir !== "") && (typeof callback === "function"))
        {
            ignore = true;
            requestAnimationFrame(function() { callback(object, dir); });
        };
    };

    var correct_direction = function()
    {
        var diff_x = x2 - x1;
        var diff_y = y2 - y1;
        var abs_x  = Math.abs(diff_x);
        var abs_y  = Math.abs(diff_y);

        if (abs_x < x3) x1 = x2;
        if (abs_y < y3) y1 = y2;

        x3 = abs_x;
        y3 = abs_y;
    };

    fx_event_listen(object, "mousedown", function(e)
    {
        active  = true;
        ignore  = false;
        x1 = x2 = fx_mouse_x;
        y1 = y2 = fx_mouse_y;
    });

    fx_event_listen(object, "mousemove", function(e)
    {
        if (! active) return;
        x2 = fx_mouse_x;
        y2 = fx_mouse_y;
        correct_direction();
    });

    fx_event_listen(object, "dragstart", function(e)
    {
        e.preventDefault();
    }, false);

    fx_event_listen(object, "selectstart", function(e)
    {
        e.preventDefault();
    }, false);

    fx_event_listen(object, "mouseup", function(e)
    {
        execute_swipe();
    });

    fx_event_listen(object, "mouseleave", function(e)
    {
        if (! active) return;
        execute_swipe();
    });

    fx_event_listen(object, "click", function(e)
    {
        if (ignore) e.preventDefault();
    }, false);

    fx_event_listen(object, "touchstart", function(e)
    {
        x1 = x2 = fx_mouse_x;
        y1 = y2 = fx_mouse_y;
    });

    fx_event_listen(object, "touchmove", function(e)
    {
        x2 = fx_mouse_x;
        y2 = fx_mouse_y;
        correct_direction();
    });

    fx_event_listen(object, "touchend", function(e)
    {
        execute_swipe();
    });
};

//==================================================================================================
//   MOVE/PINCH FUNCTIONALITY
//==================================================================================================

function fx_move_zoom(object,
                      callback)
{
    if (typeof object != "object") object = document.getElementById(object);
    if (object == null)            return;

    var active  = false;
    var noclick = false;
    var nomove  = false;
    var x1      = 0, y1  = 0, x2  = 0, y2  = 0; //pointer 1
    var _x1     = 0, _y1 = 0, _x2 = 0, _y2 = 0; //pointer 2
    var z       = 0, zx  = 0, zy  = 0;          //zoom
    var vx      = 0, vy  = 0;                   //vector

    var execute_move_zoom = function()
    {
        var _vx = nomove ? 0 : x2 - x1;
        var _vy = nomove ? 0 : y2 - y1;

        if ((_vx === 0) && (_vy === 0) && (z === 0)) return;

        if (typeof callback === "function") callback(object, _vx, _vy, z, zx, zy);

        noclick = true;
        x1      = x2;
        y1      = y2;
        _x1     = _x2;
        _y1     = _y2;
        z       = 0;

        if (active) { vx = _vx; vy = _vy; };
    };

    var execute_flick = function()
    {
        if (active) return;

        var _vx = Math.pow(Math.abs(vx), 1.25);
        var _vy = Math.pow(Math.abs(vy), 1.25);

        if ((_vx < 1) && (_vy < 1)) return;

        x1 -= _vx * Math.sign(vx);
        y1 -= _vy * Math.sign(vy);

        execute_move_zoom();

        vx /= 1.25;
        vy /= 1.25;

        setTimeout(execute_flick, 50);
    };

    var clear_vector = function()
    {
        if (active) vx = vy = 0;
    };

    fx_event_listen(object, "click", function(e)
    {
        if (noclick) e.preventDefault();
    }, false);

    fx_event_listen(object, "mousedown", function(e)
    {
        active  = true;
        noclick = false;
        x1      = x2 = fx_mouse_x;
        y1      = y2 = fx_mouse_y;
    });

    fx_event_listen(object, "mousemove", function(e)
    {
        if (! active) return;

        x2 = fx_mouse_x;
        y2 = fx_mouse_y;

        execute_move_zoom();
        setTimeout(clear_vector, 250);
    });

    fx_event_listen(object, "dragstart", function(e)
    {
        e.preventDefault();
    }, false);

    fx_event_listen(object, "selectstart", function(e)
    {
        e.preventDefault();
    }, false);

    fx_event_listen(object, "mouseup", function(e)
    {
        active = false;
        execute_flick();
    });

    fx_event_listen(object, "mouseleave", function(e)
    {
        active = false;
    });

    fx_event_listen(object, "wheel", function(e)
    {
        e.preventDefault();

        z  = e.wheelDelta;
        zx = fx_mouse_x - fx_left(this);
        zy = fx_mouse_y - fx_top(this);

        execute_move_zoom();
    }, false);

    fx_event_listen(object, "touchstart", function(e)
    {
        if (! active) noclick = false;

        active = true;
        x1     = x2 = _x1 = _x2 = fx_touch1_x;
        y1     = y2 = _y1 = _y2 = fx_touch1_y;

        if (fx_touch2_x !== null)
        {
            noclick = true;
            nomove  = true;
            _x1     = _x2 = fx_touch2_x;
            _y1     = _y2 = fx_touch2_y;
        };
    });

    fx_event_listen(object, "touchmove", function(e)
    {
        e.preventDefault();

        x2 = _x2 = fx_touch1_x;
        y2 = _y2 = fx_touch1_y;

        if (fx_touch2_x !== null)
        {
            _x2 = fx_touch2_x;
            _y2 = fx_touch2_y;

            var d1 = Math.sqrt(Math.pow(x1 - _x1, 2) + Math.pow(y1 - _y1, 2));
            var d2 = Math.sqrt(Math.pow(x2 - _x2, 2) + Math.pow(y2 - _y2, 2));

            z  = d2 - d1;
            zx = (x2 + _x2) / 2 - fx_left(this);
            zy = (y2 + _y2) / 2 - fx_top(this);
        };

        execute_move_zoom();
        setTimeout(clear_vector, 250);
    }, false);

    fx_event_listen(object, "touchend", function(e)
    {
        if (e.touches.length > 0) return;

        active = false;

        if (nomove) nomove = false;
        else        execute_flick();
    });

    fx_event_listen(object, "touchcancel", function(e)
    {
        if (e.touches.length > 0) return;

        active = false;
        nomove = false;
    });
};

//==================================================================================================
//   MISC
//==================================================================================================

function fx_pointer_block(set = true)
{
    var style = document.getElementById("fx_pointer_block");

    if (set)
    {
        if (! style)
        {
            style    = document.createElement("style");
            style.id = "fx_pointer_block";
            style.textContent =
                "HTML:root * { " +
                "POINTER-EVENTS: none !important; " +
                "TOUCH-ACTION: none !important; " +
                "USER-SELECT: none !important; }";
            document.head.appendChild(style);
        };

        return;
    };

    if (style) style.parentNode.removeChild(style);
};

function fx_pointer_object()
{
    var style = document.getElementById("fx_pointer_block");
    if (style) style.disabled = true;

    var object = document.elementFromPoint(fx_mouse_window_x, fx_mouse_window_y);

    if (style) style.disabled = false;

    return object;
};

//==================================================================================================
//   GLOBAL EVENT VALUES
//==================================================================================================

var fx_document_width   = 0;
var fx_document_height  = 0;
var fx_window_left      = 0;
var fx_window_top       = 0;
var fx_window_width     = 0;
var fx_window_height    = 0;

var fx_mouse_key        = false;
var fx_mouse_x          = 0;
var fx_mouse_y          = 0;
var fx_mouse_window_x   = 0;
var fx_mouse_window_y   = 0;
var fx_touch1_x         = 0;
var fx_touch1_y         = 0;
var fx_touch1_window_x  = 0;
var fx_touch1_window_y  = 0;
var fx_touch2_x         = 0;
var fx_touch2_y         = 0;
var fx_touch2_window_x  = 0;
var fx_touch2_window_y  = 0;
var fx_keyboard_key     = false;

var fx_event_object     = null;
var fx_scroll_container = null;

//==================================================================================================
//   EVENT UPDATE FUNCTIONS
//==================================================================================================

function fx_update_window_position()
{
    if (! fx_scroll_container) return;

    var left, top;

    if (fx_scroll_container === window)
    {
        left = fx_scroll_container.scrollX;
        top  = fx_scroll_container.scrollY;
    }
    else
    {
        left = fx_scroll_container.scrollLeft;
        top  = fx_scroll_container.scrollTop;
    };

    var _left = left - fx_window_left;
    var _top  = top  - fx_window_top;

    fx_mouse_x  += _left;
    fx_mouse_y  += _top;
    fx_touch1_x += _left;
    fx_touch1_y += _top;

    if (fx_touch2_x !== null)
    {
        fx_touch2_x += _left;
        fx_touch2_y += _top;
    };

    fx_window_left = left;
    fx_window_top  = top;
};

function fx_update_window_size()
{
    if (! fx_scroll_container) return;

    if (fx_scroll_container === window)
    {
        fx_document_width  = document.documentElement.scrollWidth;
        fx_document_height = document.documentElement.scrollHeight;
    }
    else
    {
        fx_document_width  = fx_scroll_container.scrollWidth;
        fx_document_height = fx_scroll_container.scrollHeight;
    };

    fx_window_width  = document.documentElement.clientWidth;
    fx_window_height = document.documentElement.clientHeight;
};

function fx_update_mouse_position(e)
{
    fx_touch1_window_x = fx_mouse_window_x = e.clientX;
    fx_touch1_window_y = fx_mouse_window_y = e.clientY;
    fx_touch1_x        = fx_mouse_x        = fx_mouse_window_x + fx_window_left;
    fx_touch1_y        = fx_mouse_y        = fx_mouse_window_y + fx_window_top;
    fx_touch2_window_x = fx_touch2_x       = null;
    fx_touch2_window_y = fx_touch2_y       = null;
};

function fx_update_touch_position(e)
{
    fx_touch1_window_x = fx_mouse_window_x = e.touches[0].clientX;
    fx_touch1_window_y = fx_mouse_window_y = e.touches[0].clientY;
    fx_touch1_x        = fx_mouse_x        = fx_touch1_window_x + fx_window_left;
    fx_touch1_y        = fx_mouse_y        = fx_touch1_window_y + fx_window_top;
    fx_touch2_window_x = fx_touch2_x       = null;
    fx_touch2_window_y = fx_touch2_y       = null;

    if (e.touches.length > 1)
    {
        fx_touch2_window_x = e.touches[1].clientX;
        fx_touch2_window_y = e.touches[1].clientY;
        fx_touch2_x        = fx_touch2_window_x + fx_window_left;
        fx_touch2_y        = fx_touch2_window_y + fx_window_top;
    };
};

//==================================================================================================
//   EVENT SETTINGS
//==================================================================================================

var fx_noscroll_flag   = false;
var fx_nodebounce_flag = true;
var fx_noaniframe_flag = false;

function fx_noscroll(set = true)
{
    fx_noscroll_flag = set;
};

function fx_nodebounce(set = true)
{
    fx_nodebounce_flag = set;
};

function fx_noaniframe(set = true)
{
    fx_noaniframe_flag = set;
};

//==================================================================================================
//   EVENT MANAGEMENT
//==================================================================================================

var fx_touch_time     = -1;
var fx_touch_x        = 0;
var fx_touch_y        = 0;
var fx_touch_window_x = 0;
var fx_touch_window_y = 0;
var fx_touch_count    = 0;
var fx_touch_timer    = null;
const fx_touch_detail = 42;

function fx_event_callback(event, e)
{
    switch (event)
    {
    case "window_load":

        fx_update_window_position();
        fx_update_window_size();
        break;

    case "window_scroll":

        fx_update_window_position();
        break;

    case "window_resize":

        fx_update_window_position();
        fx_update_window_size();
        break;

    case "window_unload":

        break;

    case "document_load":

        fx_update_window_position();
        fx_update_window_size();
        break;

    case "mousedown":

        if (e.touches)
        {
            fx_update_touch_position(e);
            fx_mouse_key      = 1;
            fx_touch_time     = Date.now();
            fx_touch_x        = fx_touch1_x;
            fx_touch_y        = fx_touch1_y;
            fx_touch_window_x = fx_touch1_window_x;
            fx_touch_window_y = fx_touch1_window_y;

            fx_ghost_buster_zuul = false;

            var object = e.target;
            while (object)
            {
                //direct input element
                if (object.matches(":is(CODE:read-write, INPUT, LABEL, SELECT, TEXTAREA)"))
                {
                    fx_ghost_buster_zuul = true;
                    break;
                };

                object.focus({ preventScroll: true });
                if (document.activeElement === object) break;

                object = object.parentElement;
            };

            if (! object) document.activeElement.blur();

            if (fx_ghost_buster_zuul) break;
            if (! navigator.vibrate)  break;

            setTimeout((time) =>
            {
                if (
                     (fx_touch_count === 0)
                     &&
                     (time === fx_touch_time)
                     &&
                     (Math.sqrt(Math.pow(fx_mouse_x - fx_touch_x, 2) +
                                Math.pow(fx_mouse_y - fx_touch_y, 2)) <= 10.0)
                   )
                    navigator.vibrate(50);
            }, 250, fx_touch_time);
            break;
        };

        fx_update_mouse_position(e);
        fx_mouse_key = e.button + 1;
        break;

    case "mousemove":

        if (e.touches)
        {
            fx_update_touch_position(e);
            break;
        };

        fx_update_mouse_position(e);
        break;

    case "mouseup":

        if (! e.touches) break;

        if (fx_touch_timer !== null)
        {
            clearTimeout(fx_touch_timer);
            fx_touch_timer = null;
        };

        if (
             fx_ghost_buster_zuul
             ||
             (Math.sqrt(Math.pow(fx_mouse_x - fx_touch_x, 2) +
                        Math.pow(fx_mouse_y - fx_touch_y, 2)) > 10.0)
           )
        {
            fx_touch_time  = -1;
            fx_touch_count = 0;
            break;
        };

        if ((Date.now() - fx_touch_time) >= 250)
        {
            /*
               need to check for a better way
               to make module settings visible
               on touch screen devices
            */

            let list = [e.target, ...e.target.querySelectorAll("*")];
            for (let object of list)
            {
                object.focus({ preventScroll: true });
                if (document.activeElement === object) break;
            };
            break;
        };

        fx_touch_count++

        fx_touch_timer = setTimeout(function()
        {
            let event = new MouseEvent(
                (fx_touch_count > 1) ? "dblclick" : "click", {
                bubbles    : true,
                cancelable : true,
                clientX    : fx_touch_window_x,
                clientY    : fx_touch_window_y,
                detail     : fx_touch_detail,
                view       : window});
            e.target.dispatchEvent(event);
            fx_touch_timer = null;
            fx_touch_time  = -1;
            fx_touch_count = 0;
        }, 250);
        break;

    case "mouseleave":

        if (! e.touches) break;

        if (fx_touch_timer !== null)
        {
            clearTimeout(fx_touch_timer);
            fx_touch_timer = null;
        };

        fx_touch_time  = -1;
        fx_touch_count = 0;
        break;

    case "dblclick":

        fx_mouse_key = e.button + 1;
        break;

    case "keydown":

        fx_keyboard_key = e.keyCode;
        break;

    case "keypress":
    case "keyup":
    };
};

var fx_callback   = [fx_event_callback];
var fx_event      = [];
var fx_event_slot = -1;

function fx_event_raise(event, e)
{
    var context = this;

    if (fx_noaniframe_flag)
    {
        _fx_event_raise.call(context, event, e)
        return;
    };

    requestAnimationFrame(() => _fx_event_raise.call(context, event, e));
};

function _fx_event_raise(event, e)
{
    if (fx_event_slot >= 9) return;

    var callback = fx_callback[0];
    var list     = fx_callback.slice(1);
    var slot     = ++fx_event_slot;
    var data     = [];
    var _event   = null;
    var context  = this;
    var once     = ["window_load", "document_load"].includes(event);

    fx_event_object = e;
    callback.call(context, event, e);

    //disable ghost trap
    var shadow_of_zuul   = fx_ghost_buster_zuul;
    fx_ghost_buster_zuul = true;

    fx_event[slot] = [
        fx_document_width,  fx_document_height,
        fx_window_left,     fx_window_top,
        fx_window_width,    fx_window_height,
        fx_mouse_key,
        fx_mouse_x,         fx_mouse_y,
        fx_mouse_window_x,  fx_mouse_window_y,
        fx_touch1_x,        fx_touch1_y,
        fx_touch1_window_x, fx_touch1_window_y,
        fx_touch2_x,        fx_touch2_y,
        fx_touch2_window_x, fx_touch2_window_y,
        fx_keyboard_key,
        fx_event_object];

    for (callback of list)
    {
        if (callback.fx_event && (! callback.fx_event.includes(event))) continue;

        _event = callback.fx_pass_event_object ? fx_event_object : event;

        try
        {
            switch (typeof callback)
            {
            case "object":

                callback.callback.call(context, _event);
                break;

            case "function":

                callback.call(context, _event);
            };
        }
        catch (_e) {};

        data = fx_event[slot];
        if (data === null) break;

        [fx_document_width,  fx_document_height,
         fx_window_left,     fx_window_top,
         fx_window_width,    fx_window_height,
         fx_mouse_key,
         fx_mouse_x,         fx_mouse_y,
         fx_mouse_window_x,  fx_mouse_window_y,
         fx_touch1_x,        fx_touch1_y,
         fx_touch1_window_x, fx_touch1_window_y,
         fx_touch2_x,        fx_touch2_y,
         fx_touch2_window_x, fx_touch2_window_y,
         fx_keyboard_key,
         fx_event_object] = data;

        if (once && callback.fx_event) fx_event_remove(callback, event);
    };

    //restore ghost trap
    fx_ghost_buster_zuul = shadow_of_zuul;

    fx_event_consume();
};

function fx_event_consume()
{
    if (fx_event_slot >= 0) fx_event[fx_event_slot--] = null;
};

//==================================================================================================
//   FULL EVENT REGISTRATION
//==================================================================================================

function fx_register_callback(callback)
{
    if (! fx_callback.includes(callback))
        fx_callback.push(callback);
};

function fx_unregister_callback(callback)
{
    var index = fx_callback.indexOf(callback);
    if (index !== -1) fx_callback.splice(index, 1);
};

//==================================================================================================
//   SELECT EVENT REGISTRATION
//==================================================================================================

function fx_event_listen(object,
                         event,
                         _function = null,
                         passive   = true,
                         capture   = false)
{
    if (Array.isArray(object))
    {
        for (let _event of object)
            fx_event_listen(_event, event, _function, passive, capture);
        return;
    };

    if (Array.isArray(event))
    {
        for (let _event of event)
            fx_event_listen(object, _event, _function, passive, capture);
        return;
    };

    switch (typeof object)
    {
    case "object":

        var flag = true;

        if (object === window)
        {
            const map = {"load"         : "window_load",
                         "scroll"       : "window_scroll",
                         "resize"       : "window_resize",
                         "beforeunload" : "window_unload"};

            if (event in map) { event = map[event]; flag = false; };
        }
        else if (object === document)
        {
            const map = {"DOMContentLoaded" : "document_load",
                         "mousedown"        : "mousedown",
                         "mousemove"        : "mousemove",
                         "mouseup"          : "mouseup",
                         "mouseleave"       : "mouseleave",
                         "dblclick"         : "dblclick",
                         "touchstart"       : "mousedown",
                         "touchmove"        : "mousemove",
                         "touchend"         : "mouseup",
                         "touchcancel"      : "mouseleave",
                         "keydown"          : "keydown",
                         "keypress"         : "keypress",
                         "keyup"            : "keyup"};

            if (event in map) { event = map[event]; flag = false; };
        };

        if (flag)
        {
            object.addEventListener(event, (e) =>
            {
                _function.call(object, e); return;
            }, { capture : capture, passive : passive });
            return;
        };

        _function.fx_pass_event_object = true;
        break;

    case "string":

        _function = event;
        event     = object;
    };

    if (! _function.fx_event)                 _function.fx_event = [];
    if (! _function.fx_event.includes(event)) _function.fx_event.push(event);
    if (! fx_callback.includes(_function))    fx_callback.push(_function);
};

function fx_event_remove(object, event = "", _function = null)
{
    if (object.fx_event)
    {
        if (event !== "")
        {
            var index = object.fx_event.indexOf(event);
            if (index !== -1) object.fx_event.splice(index, 1);

            if (object.fx_event.length > 0) return;
        };

        fx_unregister_callback(object);
        return;
    };

    object.removeEventListener(event, _function);
};

//==================================================================================================
//   DEBOUNCING
//==================================================================================================

const fx_event_debounce_list = {};

function fx_event_debounce(event, e)
{
    if (fx_nodebounce_flag)
    {
        fx_event_raise(event, e);
        return;
    };

    if (! fx_event_debounce_list[event])
        fx_event_debounce_list[event] = { time: 0, flag: false };

    if (fx_event_debounce_list[event].flag) return;

    var fps      = 30;
    var interval = (1000 / fps) - (Date.now() - fx_event_debounce_list[event].time);

    if (interval <= 0) { _fx_event_debounce(event, e); return; };

    fx_event_debounce_list[event].flag = true;
    setTimeout(() => _fx_event_debounce(event, e), interval);
};

function _fx_event_debounce(event, e)
{
    fx_event_debounce_list[event] = { time: Date.now(), flag: false };
    fx_event_raise(event, e);
};

//==================================================================================================
//   GHOST EVENT BUSTING
//==================================================================================================

var fx_ghost_buster_control = null;
var fx_ghost_buster_zuul    = false;

function fx_ghost_buster()
{
    if (fx_ghost_buster_control)
        fx_ghost_buster_control.abort();

    fx_ghost_buster_control = new AbortController();

    var option   = {
        capture : true,
        passive : false,
        signal  : fx_ghost_buster_control.signal };

    var muon_trap = (e) =>
    {
        if (fx_ghost_buster_zuul)         return;
        if (e.detail === fx_touch_detail) return;

        e.preventDefault();
        e.stopImmediatePropagation();

        if (e.type === "click") fx_ghost_buster_control.abort();
    };

    ["mouseover", "mouseenter", "mousemove", "mousedown", "mouseup", "click"].forEach(
        type => { document.addEventListener(type, muon_trap, option); });
};

//==================================================================================================
//   EVENT INITIALISATION
//==================================================================================================

//window
window.addEventListener("load",
    (e) => fx_event_raise("window_load", e),
    { once: true });

window.addEventListener("resize",
    (e) => fx_event_debounce("window_resize", e),
    { passive: true });

window.addEventListener("beforeunload",
    (e) => fx_event_raise("window_unload", e),
    { passive: true });

//document
document.addEventListener("DOMContentLoaded", (e) =>
{
    fx_scroll_container =
        ["clip", "hidden"].includes(getComputedStyle(document.documentElement).overflow) ?
        document.body : window;

    fx_scroll_container.addEventListener("scroll",
        (e) => fx_event_debounce("window_scroll", e),
        { passive: true });

    fx_event_raise("document_load", e);
},
{ once: true });

//mouse
document.addEventListener("mousedown",
    (e) => { if ((fx_mouse_key > 0) && ((e.button + 1) !== fx_mouse_key))
             {
                     fx_event_raise("mouseleave", e);
                     return;
             };
             fx_event_raise("mousedown", e); },
    { passive: true });

document.addEventListener("mousemove",
    (e) => fx_event_debounce("mousemove", e),
    { passive: true });

document.addEventListener("mouseup",
    (e) => { fx_event_raise("mouseup", e);
             fx_mouse_key = false; },
    { passive: true });

document.addEventListener("mouseleave",
    (e) => { fx_event_raise("mouseleave", e);
             fx_mouse_key = false; },
    { passive: true });

document.addEventListener("dblclick",
    (e) => fx_event_raise("dblclick", e),
    { passive: true });

//touch
document.addEventListener("touchstart",
    (e) => { _fx_event_raise("mousedown", e);
             if (fx_noscroll_flag && e.cancelable) e.preventDefault();
             if (! fx_ghost_buster_zuul) fx_ghost_buster(); },
    { passive: false });

document.addEventListener("touchmove",
    (e) => fx_event_debounce("mousemove", e),
    { passive: true });

document.addEventListener("touchend",
    (e) => { fx_event_raise("mouseup", e);
             fx_mouse_key = false; },
    { passive: true });

document.addEventListener("touchcancel",
    (e) => { fx_event_raise("mouseleave", e);
             fx_mouse_key = false; },
    { passive: true });

//keyboard
document.addEventListener("keydown",
    (e) => fx_event_raise("keydown", e),
    { passive: true });

document.addEventListener("keypress",
    (e) => fx_event_raise("keypress", e),
    { passive: true });

document.addEventListener("keyup",
    (e) => { fx_event_raise("keyup", e);
             fx_keyboard_key = false; },
    { passive: true });