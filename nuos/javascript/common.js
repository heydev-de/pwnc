/*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

//==================================================================================================
//   STRING
//==================================================================================================

function string_repeat(string,
                       count)
{
    return string.repeat(count);
};

function htmlspecialchars(string)
{
    const array = {
        "\"" : "&quot;",
        "&"  : "&amp;",
        "'"  : "&apos;",
        "<"  : "&lt;",
        ">"  : "&gt;"};

    return string.replace(/["&'<>]/g, (value) => array[value]);
};

function unique_id(count)
{
    var _string = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    var value   = "";

    for (let i = 0; i < count; i++)
        value += _string[Math.floor(Math.random() * 62)];

    return value;
};

function strabridge(string,
                    length  = 50,
                    cut_end = false)
{
    var _length = string.length;
    if (_length <= length) return string;

    if (cut_end) return string.slice(0, length - 2) + " …";

    length           -= 3;
    var length_start  = Math.round(length * 0.65);
    var length_end    = length - length_start;

    return string.slice(0, length_start) +
           " … " +
           string.slice(-length_end);
};

function addslashes(string)
{
    return JSON.stringify(string).slice(1, -1);
};

function strtocolor(string,
                    lightness = 75,
                    diff_min  = 0)
{
    let hue = djb2(string) % 360;

    if (strtocolor.hue !== undefined)
    {
        let diff = hue - strtocolor.hue;
        if (diff > 180)       diff -= 360;
        else if (diff < -180) diff += 360;

        let abs = Math.abs(diff);
        if (abs < diff_min)
        {
            let delta  = diff_min - abs;
            hue       += (diff >= 0) ? delta : -delta;
            hue        = (hue + 360) % 360;
        };
    };

    strtocolor.hue = hue;

    return "hsl(" + hue + ", " + lightness + "%, " + lightness + "%)";
};

//==================================================================================================
//   TEXTCONTROL
//==================================================================================================

function textcontrol(object,
                     image_path,
                     extension = "")
{
    const div      = document.createElement("div");
    div.className  = "textcontrol";
    object         = addslashes(object);
    image_path    += "textcontrol/";

    const array = [
        ["+",            "bold",        "bold"],
        ["/",            "italic",      "italic"],
        ["_",            "underline",   "underline"],
        ["<",            "big",         "big"],
        [">",            "small",       "small"],
        ["<-",           "left",        "align left"],
        ["<->",          "center",      "center"],
        ["->",           "right",       "align right"],
        /*
        ["*",            "title",       "title"],
        ["\"",           "quote",       "quote"],
        ["-",            "monospace",   "monospace"],
        */
        ["#image_left",  "image_left",  "left aligned image"],
        ["#image_right", "image_right", "right aligned image"],
        ["#link",        "link",        "link"],
        ["#email",       "email",       "email"]
        /*
        ["#table",       "table",       "table"],
        ["#tablerow",    "table_row",   "table separator"],
        ["#tablecell",   "table_cell",  "cell separator"]
        */
    ];

    const func = function(format, image, title)
    {
        const a   = document.createElement("a");
        a.href    = "javascript:textcontrol_set('" + object + "','" + format + "');";
        a.title   = title;

        const img = document.createElement("img");
        img.src   = image_path + "button_" + image + ".svg";
        img.alt   = "";

        a.appendChild(img);
        div.appendChild(a);
    };

    array.forEach(function(value) { func(value[0], value[1], value[2]); });

    if (extension !== "") div.insertAdjacentHTML("beforeend", extension);

    func("#remove", "remove", "remove formatting");

    const script = document.currentScript;
    if (script) script.parentNode.insertBefore(div, script);
};

function textcontrol_set(object,
                         format,
                         data = "")
{
    var object_handle = document.querySelector(object);
    if (object_handle.disabled) return;

    var text         = object_handle.value;
    var _format      = "";
    var start        = 0;
    var end          = 0;
    var select_start = 0;
    var select_end   = 0;
    var select_text  = "";
    var flag         = object_handle instanceof HTMLTextAreaElement;

    if (flag)
    {
        select_start   = object_handle.selectionStart;
        select_end     = object_handle.selectionEnd;
        select_text    = text.substring(select_start, select_end);
        var scroll_top = object_handle.scrollTop;
    }
    else if (window.getSelection !== undefined)
    {
        var selection = window.getSelection();
        var range     = (selection.rangeCount === 0) ? document.createRange() : selection.getRangeAt(0);
        var _range    = document.createRange();
        _range.selectNodeContents(object_handle);

        if (range.compareBoundaryPoints(range.START_TO_START, _range) == 1)
            _range.setStart(range.startContainer, range.startOffset);

        if (range.compareBoundaryPoints(range.END_TO_END, _range) == -1)
            _range.setEnd(range.endContainer, range.endOffset);

        select_text  = _range.cloneContents().textContent;
        _range.setStart(object_handle, 0);
        select_end   = _range.cloneContents().textContent.length;
        select_start = select_end - select_text.length;
    };

    switch (format)
    {
    case "#image_left":

        if (data !== "")
        {
            _format = "[IMG " + data + "]" + select_text;
            start   = -select_text.length;
            end     = start;
        }
        else
        {
            _format = "[<-IMG https://URL * *]" + select_text;
            end     = -select_text.length - 5;
            start   = end - 3;
        };
        break;

    case "#image_right":

        if (data !== "")
        {
            _format = "[IMG-> " + data + "]" + select_text;
            start   = -select_text.length;
            end     = start;
        }
        else
        {
            _format = "[IMG-> https://URL * *]" + select_text;
            end     = -select_text.length - 5;
            start   = end - 3;
        };
        break;

    case "#link":

        if (data !== "")
        {
            if (select_text !== "")
            {
                let match = data.match(/^([^ \r\n]+)/);
                if (match) _format = "[" + match[1] + " " + select_text + "]";
            }
            else
            {
                _format = "[" + data + "]";
            };

            start = -1;
            end   = start;
        }
        else
        {
            _format = "[https://URL " + ((select_text !== "") ? select_text : "click") + "]";
            end     = -((select_text !== "") ? select_text.length : 5) - 2;
            start   = end - 3;
        };
        break;

    case "#email":

        if (data !== "")
        {
            if (select_text !== "")
            {
                let match = data.match(/^([^ \r\n]+)/);
                if (match) _format = "[mailto:" + match[1] + " " + select_text + "]";
            }
            else
            {
                _format = "[mailto:" + data + "]";
            };

            start = 0;
            end   = 0;
        }
        else
        {
            _format = "[mailto:account@domain " + ((select_text !== "") ? select_text : "click") + "]";
            end     = -((select_text !== "") ? select_text.length : 5) - 2;
            start   = end - 14;
        };
        break;

    case "#table":

        _format = "\n[# " + select_text + " ]\n";
        start   = -select_text.length - 3;
        end     = start;
        break;

    case "#tablerow":

        _format = " | \n | " + select_text;
        start   = -select_text.length;
        end     = start;
        break;

    case "#tablecell":

        _format = " | " + select_text;
        start   = 0;
        end     = 0;
        break;

    case "#token":

        if (data !== "")
        {
            if (! data.match(/^%%.*%%$/))
                data        = "%%" + data + "%%";

            if (data.match(/^[^ \r\n]+$/))
            {
                _format     = data + select_text;
                start       = data.length - _format.length;
                end         = start;
                break;
            };

            let match = null;

            if (select_text !== "")
            {
                select_text = select_text.replace(/,/g, "\\,").replace(/%%/g, "\\%%");
                data        = data.replace(/^([^ \r\n]+[ \r\n])[^,%]+/, "$1" + select_text);
                _format     = data;
                match       = data.match(/^(([^,]|\\,)+[^\\],[ \r\n]*|.+%%$)/);
            }
            else
            {
                _format     = data;
                match       = data.match(/^([^ \r\n]+[ \r\n]+)/);
            };

            start = match[1].length - _format.length;
            match = data.substring(match[1].length).match(/^(([^,]|\\,)*[^\\](,)|.*(%%)$|$)/);
            end   = start + match[1].length - (match[3]?.length || 0) - (match[4]?.length || 0);
        }
        else
        {
            _format = "%%" + select_text + "%%";
            end     = -2;
            start   = end - select_text.length;
        };
        break;

    case "#replace_all":

        select_start = 0;
        select_end   = text.length;

    case "#insert":

        /*
        _format = data + select_text;
        start   = -select_text.length;
        end     = start;
        break;
        */

    case "#replace":

        _format = data;
        start   = 0;
        end     = 0;
        break;

    case "#imbed":

        start = data.lastIndexOf("%text%");
        if (start === -1)
        {
            _format = data + select_text;
            start   = -select_text.length;
        }
        else
        {
            _format = data.replace(/%text%/, select_text);
            start   = -_format.length + start + select_text.length;
        };
        end = start;
        break;

    case "#remove":

        var temp     = textcontrol_remove(text, select_start, select_end);
        _format      = temp[0];
        select_start = temp[1];
        select_end   = temp[2];
        start        = 0;
        end          = -_format.length;
        break;

    default:

        _format = "[" + format + " " + select_text + "]";
        start   = -1;
        end     = -1;
        break;
    };

    if (flag || (window.getSelection !== undefined))
    {
        var temp  = select_start + _format.length;
        start    += temp;
        end      += temp;

        if (text.substring(select_start, select_end) !== _format)
        {
            object_handle.value = text.substring(0, select_start) +
                                  _format +
                                  text.substring(select_end);

            if (flag)
            {
                object_handle.scrollTop = scroll_top;
                object_handle.setSelectionRange(start, end);
            }
            else
            {
                selection.removeAllRanges();
                range.setStart(object_handle.firstChild, start);
                range.setEnd(object_handle.firstChild, end);
                selection.addRange(range);
            };
        };
    }
    else
    {
        object_handle.value = text + _format;
    };

    object_handle.focus();
};

function textcontrol_remove(text,
                            start,
                            end)
{
    var format      = {"+"     : true,
                       "/"     : true,
                       "_"     : true,
                       "<"     : true,
                       ">"     : true,
                       "<-"    : true,
                       "<->"   : true,
                       "->"    : true,
                       "*"     : true,
                       "\""    : true,
                       "-"     : true,
                       "IMG"   : true,
                       "<-IMG" : true,
                       "IMG->" : true,
                       "#"     : true};

    var depth       = 0;           //output buffer level
    var buffer      = [""];        //output buffer
    var mode        = 0;           //current parsing mode
    var stack       = [];          //format marker stack
    var _stack      = [];          //format offset stack
    var range       = [];          //format list
    var marker      = "";          //format marker
    var table_depth = 0;           //table nesting depth
    var table_char  = "";          //table character
    var char        = "";          //current character
    var _char       = "";          //previous character
    var __char      = "";          //antepenultimate character
    var whitespace  = "";          //whitespace buffer
    var i           = 0;           //text position
    var _i          = 0;
    var c           = text.length; //length of input text
    var flag        = false;
    var temp        = 0;

    while (true)
    {
        for (; i < c; i++)
        {
            char = text[i];
            flag = false;

            switch (mode)
            {
            case 1: //token
//..................................................................................................

                //append current character
                buffer[depth] += char;

                //unescaped token delimiter
                if ((char === "%") && (_char === "%") && (__char !== "\\"))
                    mode = 0; //reset mode
                break;

            case 2: //formatting
//..................................................................................................

                if (char === "\r") break;

                //unescaped space or linebreak
                if (
                     ((char === " ")                      && (_char  !== "\\"))
                     ||
                     ((char === "\n") && (_char !== "\r") && (_char  !== "\\"))
                     ||
                     ((char === "\n") && (_char === "\r") && (__char !== "\\"))
                   )
                {
                    //marker
                    if (format.hasOwnProperty(marker))
                    {
                        //memorize format marker
                        stack.push(marker);

                        //table
                        if (marker === "#")
                        {
                            buffer[depth] += "\n" + "\t".repeat(table_depth);
                            table_char     = ""; //reset table character
                            table_depth++;       //increase table nesting depth
                        };
                    }

                    //link, marker represents url
                    else
                    {
                        stack.push(""); //discard url
                    };

                    char                = ""; //consume current character
                    buffer[++depth]     = ""; //increase buffer level
                    mode                = 0;  //reset mode

                    //store range position
                    for (_i = depth, temp = 0; _i >= 0; _i--) temp += buffer[_i].length;
                    _stack[depth] = [temp, i - marker.length - 1];
                    break;
                };

                //append current character
                marker += char;
                break;

            default:
//..................................................................................................

                //collect adjacent whitespace
                if (
                     (char === " ")
                     ||
                     (char === "\r")
                     ||
                     (char === "\n")
                   )
                {
                    whitespace += char;
                    break;
                };

                //table
                if (table_depth > 0)
                {
                    if (char === "|") //table delimiter
                    {
                        if (table_char === "|") //table row delimiter
                        {
                            buffer[depth] += "\n" + "\t".repeat(table_depth - 1);

                            //consume memorized table delimiter
                            table_char     = "";
                        }
                        else
                        {
                            //memorize table delimiter
                            table_char     = char;
                        };

                        char       = ""; //consume current character
                        whitespace = ""; //discard whitespace
                    }
                    else
                    {
                        if (table_char === "|") //table cell delimiter
                        {
                            buffer[depth] += "\t";
                            whitespace     = ""; //discard whitespace
                        }
                        else if (table_char === "") //initial table cell
                        {
                            whitespace     = ""; //discard whitespace
                        };

                        //memorize table character
                        table_char = char;
                    };
                };

                switch (char)
                {
                case "%": //token

                    //unescaped token delimiter
                    if ((_char === "%") && (__char !== "\\"))
                    {
                        //append current character
                        buffer[depth] += whitespace + char;
                        whitespace     = ""; //consume whitespace
                        mode           = 1;  //switch to token mode
                        flag           = true;
                    };

                    break;

                case "[": //start formatting

                    if (_char === "\\") //escaped
                    {
                        _char = ""; //consume escape character
                        break;
                    };

                    //append whitespace
                    buffer[depth] += whitespace;
                    whitespace     = ""; //consume whitespace
                    marker         = ""; //reset marker
                    mode           = 2;  //switch to formatting mode
                    flag           = true;
                    break;

                case "]": //end formatting

                    if (_char === "\\") //escaped
                    {
                        _char = ""; //consume escape character
                        break;
                    };

                    //retrieve memorized marker
                    if ((marker = stack.pop()) !== undefined)
                    {
                        //format marker
                        if (format.hasOwnProperty(marker))
                        {
                            switch (marker)
                            {

                            //image
                            case "IMG":
                            case "<-IMG":
                            case "IMG->":

                                //discard url
                                buffer[depth] = "";
                                break;

                            //table
                            case "#":
                                whitespace = "\n"; //overwrite whitespace
                                table_depth--;     //decrease table nesting depth
                                break;
                            };

                            marker = ""; //clear marker
                        };

                        //append collected data
                        buffer[depth] += whitespace + marker;

                        //store range position
                        for (_i = depth, temp = 0; _i >= 0; _i--) temp += buffer[_i].length;
                        range.push([_stack[depth][0], _stack[depth][1], temp, i + 1]);

                        //decrease buffer level
                        buffer[--depth] += buffer.pop();
                    }

                    //no formatting initiated
                    else
                    {
                        //append current character
                        buffer[depth] += whitespace + char;
                    };

                    whitespace = ""; //consume whitespace
                    flag       = true;
                };

                if (flag) break;

                //append unconsumed backslash from previous pass
                if (_char === "\\")
                {
                    buffer[depth] += whitespace + _char;
                    whitespace     = ""; //consume whitespace
                };

                //collect backslash
                if (char === "\\")  break;

                //append current character
                buffer[depth] += whitespace + char;
                whitespace     = ""; //consume whitespace
            };

            //shift character
            __char = _char;
            _char  = char;
        };

        if (depth === 0) break; //end parsing

        switch (mode)
        {
        case 1: //token
            mode = 0; //reset mode
            break;

        case 2: //formatting
            text += " ]"; //complete formatting
            break;
        };

        //close open formattings
        text += "]".repeat(depth);
        c     = text.length;
    };

    //append unconsumed backslash
    if (_char === "\\")
        buffer[0] += whitespace + _char;

    //process format ranges
    var buffer_start = 0;
    var buffer_end   = 0;
    var text_start   = c;
    var text_end     = 0;
    var _range       = null;
    flag             = true;

    for (i = 0, c = range.length; i < c; i++)
    {
        var _range = range[i];
        var _start = _range[1];
        var _end   = _range[3];

        //range is outside selection
        if (_start >= end)   continue;
        if (_end   <= start) continue;

        //selection lies completely within range
        if (flag && (_start < start) && (_end > end))
        {
            text_start = _start; buffer_start = _range[0];
            text_end   = _end;   buffer_end   = _range[2];
            flag       = false;
            break;
        };

        //selection crosses with range
        if (((start >= _start) && (end >= _end)) ||
            ((start <= _start) && (end <= _end)) ||
            ((start <= _start) && (end >= _end)))
        {
            if (_start < text_start) { text_start = _start; buffer_start = _range[0]; };
            if (_end   > text_end)   { text_end   = _end;   buffer_end   = _range[2]; };
            flag = false;
        };
    };

    return flag ?
           [text.substring(start, end), start, end] :
           [buffer[0].substring(buffer_start, buffer_end), text_start, text_end];
};

//==================================================================================================
//   LOCATION
//==================================================================================================

function load_page(url,
                   target)
{
    [url, target] = (target === undefined) ? [url, "_blank"] : [target, url];

    var external = false;

    try
    {
        var _url = new URL(url, location.href);
        if (_url.host !== location.host) external = true;
    }
    catch (e) {};

    var width  = 900;
    var height = 600;

    if (width  > screen.availWidth)  width  = screen.availWidth;
    if (height > screen.availHeight) height = screen.availHeight;
    var left   = parseInt((screen.availWidth  - width)  / 2);
    var top    = parseInt((screen.availHeight - height) / 2);
    var object = window.open(
        url,
        target,
        "width=" + width + "," +
        "height=" + height + "," +
        "left=" + left + "," +
        "top=" + top + "," +
        "location=no," +
        "menubar=no," +
        "resizable=yes," +
        "scrollbars=yes," +
        "status=no," +
        "toolbar=no" + (external ? "," +
        "noopener," +
        "noreferrer" : ""));

    if (! object) return; //important

    object.first_load = true;

    fx_event_listen(object, "load", function()
    {
        if (object.first_load) fx_adjust_window(object);
    });
};

//==================================================================================================
//   FORM
//==================================================================================================

function limit(object,
               limit)
{
    if (object.value.length <= limit) return;

    var start    = object.selectionStart;
    var end      = object.selectionEnd;
    object.value = object.value.slice(0, limit);
    object.setSelectionRange(start, end);
};

//==================================================================================================
//   COOKIE
//==================================================================================================

function getcookie(name)
{
    var match = document.cookie.match(new RegExp("(?:^|; *)" + name + "=([^;]*)"));
    return match ? decodeURIComponent(match[1]) : "";
};

function setcookie(name,
                   value,
                   expires = null)
{
    var cookie = name + "=" + encodeURIComponent(value) + "; path=/; samesite=Strict";
    if (location.protocol === "https:") cookie += "; secure";
    if (expires instanceof Date) cookie += "; expires=" + expires.toUTCString();
    else if (expires)            cookie += "; expires=" + expires;

    document.cookie = cookie;
};

function delcookie(name)
{
    document.cookie = name + "=; path=/; samesite=Strict; expires=Thu, 01 Jan 1970 00:00:00 GMT";
};

//==================================================================================================
//   VARIOUS
//==================================================================================================

function djb2(string)
{
    string   = (new TextEncoder()).encode(string);
    let hash = 5381;

    for (let i = 0; i < string.length; i++)
        hash = ((hash << 5) + hash + string[i]) & 0xFFFFFFFF;

    return hash;
};

function crc32(string)
{
    string  = (new TextEncoder()).encode(string);
    let crc = 0xFFFFFFFF;

    for (let i = 0; i < string.length; i++)
    {
        const byte = string[i];
        crc        = crc ^ byte;

        for (let _i = 0; _i < 8; _i++)
            crc = (crc >>> 1) ^ ((crc & 1) ? 0xEDB88320 : 0);
    };

    return (crc ^ 0xFFFFFFFF) >>> 0;
};

function load_script(url,
                     onload)
{
    if (document.querySelector("script[src=\"" + url + "\"]")) return;

    var link = document.createElement("script");

    if (typeof onload === "function")
        fx_event_listen(link, "load", onload);

    link.src   = url;
    link.async = true;

    document.head.appendChild(link);
};

function load_css(url)
{
    if (document.querySelector("link[href=\"" + url + "\"]")) return;

    var link = document.createElement("link");

    link.rel    = "preload";
    link.href   = url;
    link.as     = "style";
    link.onload = function() { this.onload = null; this.rel = "stylesheet"; };

    document.head.appendChild(link);
};

function document_write(output)
{
    const script = document.currentScript;
    if (script) script.insertAdjacentHTML("beforebegin", output);
};