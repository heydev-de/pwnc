/*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

function parallax_init()
{
    if (! window.WebGLRenderingContext) return;

    document.querySelectorAll("[data-parallax-distance]").forEach(object =>
    {
        let distance = parseInt(object.getAttribute("data-parallax-distance"));
        if (! isNaN(distance)) parallax_add(object, distance);
    });

    parallax_initialize();
};

const parallax_list = [];

function parallax_add(object,
                      distance)
{
    object.style.willChange = "clip-path, transform";

    var item = {
        object,
        distance: distance / 100,
        x:        0,
        y:        0,
        size_x:   0,
        size_y:   0,
        scale:    1,
        flag:     true
    };

    new ResizeObserver(() => { item.flag = true; }).observe(object);

    parallax_list.push(item);
};

function parallax_process(item)
{
    var object   = item.object;
    var distance = item.distance;

    var x      = fx_left(object);
    var y      = fx_top(object);
    var size_x = fx_width(object, true);
    var size_y = fx_height(object, true);

    //compute the scaling factor for x and y to cover the gaps created by the parallax translation
    //ensures that after the element is translated, it still fully covers its original area
    var scale_x = (fx_window_width  - size_x) * distance / size_x;
    var scale_y = (fx_window_height - size_y) * distance / size_y;
    var scale   = 1 + Math.max(scale_x, scale_y);

    //compute how much the element grows due to scaling
    var scale_excess     = scale - 1; //excess size compared to the original element
    var scale_correction = 2 * scale; //divide by 2 and adjust for scale to get pre-transform half

    //compute half of the difference between transformed and original size
    var half_scale_delta_x = (size_x * scale_excess) / scale_correction; // half width increase
    var half_scale_delta_y = (size_y * scale_excess) / scale_correction; // half height increase

    Object.assign(item,
    {
        x,
        y,
        size_x,
        size_y,
        scale,
        flag: false,
        half_scale_delta_x,
        half_scale_delta_y
    });
};

function parallax_event()
{
    var list     = [];
    var item     = null;
    var screen_x = fx_window_left;
    var screen_y = fx_window_top;
    var end_x    = screen_x + fx_window_width;
    var end_y    = screen_y + fx_window_height;
    var center_x = screen_x + fx_window_width  / 2;
    var center_y = screen_y + fx_window_height / 2;

    //reset
    for (item of parallax_list)
    {
        if (item.flag) item.object.style.transform = "none";
    };

    for (item of parallax_list)
    {
        if (item.flag) parallax_process(item);

        let { x, y, size_x, size_y } = item;

        if ((x + size_x <= screen_x) || (x >= end_x) ||
            (y + size_y <= screen_y) || (y >= end_y))
            continue;

        let { distance, scale, half_scale_delta_x, half_scale_delta_y } = item;

        //compute the translation of the element relative to the viewport center
        let distance_x = (center_x - x - size_x / 2) * distance;
        let distance_y = (center_y - y - size_y / 2) * distance;

        //compute the translation offset in pre-transform coordinates
        let offset_x = distance_x / scale; //x-offset adjusted for scale
        let offset_y = distance_y / scale; //y-offset adjusted for scale

        //compute the final clip insets
        let clip_left   = half_scale_delta_x - offset_x;
        let clip_right  = half_scale_delta_x + offset_x;
        let clip_top    = half_scale_delta_y - offset_y;
        let clip_bottom = half_scale_delta_y + offset_y;

        list.push(
        {
            object:
                item.object,
            style:
            {
                transform: "translate3d(" + distance_x + "px, " +
                                            distance_y + "px, " +
                                           "0) " +
                           "scale(" + scale + ")",
                clipPath:  "inset(" + Math.max(0, clip_top)    + "px " +
                                      Math.max(0, clip_right)  + "px " +
                                      Math.max(0, clip_bottom) + "px " +
                                      Math.max(0, clip_left)   + "px)"
            }
        });
    };

    for (item of list) Object.assign(item.object.style, item.style);
};

function parallax_initialize()
{
    if (parallax_list.length === 0) return;

    fx_event_listen(["window_load", "window_scroll", "window_resize"], parallax_event);
};