/*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

function thumbnail_init()
{
    var list   = document.getElementsByTagName("a");
    var object = null;
    var c      = list.length;
    var func   = function() { thumbnail(this); return false; };

    for (var i = 0; i < c; i++)
    {
        object = list.item(i);
        if (object.className.match(/(^| )thumbnail($| )/) &&
            ((typeof photoswipe_active === "undefined") ||
             (! object.hasAttribute("data-size"))))
            object.onclick = func;
    };
};

function thumbnail(object)
{
    var body  = document.body;
    var image = new Image();
    var div1  = document.createElement("div");
    div1.setAttribute("class", "thumbnail-loading");
    body.appendChild(div1);

    image.onload = function()
    {
        var div2     = document.createElement("div");
        div2.setAttribute("class", "thumbnail");
        div2.onclick = function() { body.removeChild(div2); };

        var width    = this.width;
        var height   = this.height;
        var _width   = fx_window_width  - 50;
        var _height  = fx_window_height - 50;
        if (width  > _width)  { height *= _width  / width;  width  = _width;  };
        if (height > _height) { width  *= _height / height; height = _height; };

        var _image  = document.createElement("img");
        _image.setAttribute("src", this.src);
        _image.style.width      = width  + "px";
        _image.style.height     = height + "px";
        _image.style.marginLeft = "-" + (~~(width  / 2)) + "px";
        _image.style.marginTop  = "-" + (~~(height / 2)) + "px";

        body.removeChild(div1);

        div2.appendChild(_image);
        body.appendChild(div2);
    };

    image.onerror = function()
    {
        body.removeChild(div1);
    };

    image.src = object.getAttribute("href");
};