/*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

//==================================================================================================
//   COMMAND
//==================================================================================================

function ifc_select_page(index,
                         popup = false)
{
    var url = ifc_page[0] +
              "?ifc_message=ifc_select_page" +
              "&ifc_param=" + encodeURIComponent(ifc_page[index]);

    if (popup)
    {
        load_page(url);
        return;
    };

    ifc_reset();
    load_page("this", url);
};

function ifc_select_menu(index)
{
    if ((ifc_menu[0][index] !== "") && (! confirm(ifc_menu[0][index] + "?"))) return;
    try { (new Function(ifc_menu[1][index]))(); } catch (e) {};
};

function ifc_submit()
{
    ifc_memorize_position();

    const form  = document.forms["ifc"];
    const event = new Event("submit", { bubbles: true, cancelable: true });
    form.dispatchEvent(event);

    if (! event.defaultPrevented) form.submit();
};

function ifc_post(message = "",
                  param   = "")
{
    if (message !== "") document.forms["ifc"].elements["ifc_message"].setAttribute("value", message);
    if (param   !== "") document.forms["ifc"].elements["ifc_param"].setAttribute("value", param);

    ifc_submit();
};

function ifc_cancel(offset = 0)
{
    ifc_reset(offset);
    ifc_post("ifc_cancel");
};

function ifc_autopost(object,
                      message = "")
{
    if (typeof object !== "object") object = ifc_object(object);

    fx_event_listen(object, "change", function() { ifc_post(message); });
};

function ifc_response(value)
{
    var object = ifc_object("ifc-response");
    if (object === null) return;

    object.className = "hidden";
    fx_animation_frame(() => { object.className = ""; });
    object.innerHTML = value;
};

//==================================================================================================
//   VALUE
//==================================================================================================

function ifc_get(object,
                 index = 0)
{
    if (typeof object !== "object") object = ifc_object(object, index);

    switch (object.type)
    {
    case "checkbox":
    case "radio":

        if (object.checked) return object.value;
        break;

    case "button":
    case "date":
    case "hidden":
    case "password":
    case "reset":
    case "select-one":
    case "select-multiple":
    case "submit":
    case "text":
    case "textarea":

        return object.value;

    case "file":

        return object.files ? object.files : object.value;

    default:

        var _object = null;
        var i       = 0;

        while ((_object = object[i++]) && (_object.type === "radio"))
        {
            if (_object.checked) return _object.value;
        };
    };

    return false;
};

function ifc_get_selection(object)
{
    return (object instanceof HTMLTextAreaElement) ?
           object.value.substring(object.selectionStart, object.selectionEnd) :
           window.getSelection().toString();
};

function ifc_title(object,
                   index = 0)
{
    if (typeof object !== "object") object = ifc_object(object, index);

    switch (object.type)
    {
    case "button":
    case "reset":
    case "submit":

        return object.textContent.trim();

    case "checkbox":
    case "file":
    case "hidden":
    case "password":
    case "radio":
    case "text":
    case "textarea":

        var label = object.closest("LABEL");
        if (! label) break;
        var _label = label.cloneNode(true);
        _label.querySelectorAll("*").forEach(item =>
        {
            if ((item.tagName !== "SPAN") || item.hasAttribute("class")) item.remove();
        });
        return _label.textContent.trim();

    case "select-one":
    case "select-multiple":

        return object.options[object.selectedIndex].text;
    };

    return "";
};

function ifc_reset(offset = 0)
{
    var object = null;

    while (object = document.forms["ifc"].elements[offset++]) ifc_del(object, 0, true);
};

function ifc_del(object,
                 index,
                 focus = true)
{
    if (typeof object !== "object") object = ifc_object(object, index);

    switch (object.type)
    {
    case "checkbox":
    case "radio":

        object.value   = "";
        object.checked = false;
        break;

    case "date":
    case "file":
    case "hidden":
    case "password":
    case "text":
    case "textarea":

        object.value = "";
        break;

    case "select-one":
    case "select-multiple":

        object.selectedIndex = -1;
        break;

    default:

        var _object = null;
        var i       = 0;

        while ((_object = object[i++]) && (_object.type == "radio"))
        {
            _object.value   = "";
            _object.checked = false;
        };
    };

    if (focus) ifc_focus(object);
};

function ifc_set(object,
                 value = "",
                 index = 0)
{
    if (typeof object !== "object") object = ifc_object(object, index);

    switch (object.type)
    {
    case "checkbox":
    case "date":
    case "hidden":
    case "password":
    case "radio":
    case "select-one":
    case "select-multiple":
    case "text":
    case "textarea":

        object.value = value;
        break;

    case "file": //can cause security exception

        break;

    default:

        var _object = null;
        var i       = 0;

        while ((_object = object[i++]) && (_object.type == "radio"))
            _object.checked = _object.value == value;
    };

    const target = object.getAttribute("data-l");
    if (target !== null) ifc_language_reload(object, target);
};

function ifc_copy(source,
                  target)
{
    ifc_set(target, ifc_get(source));
};

function ifc_select(object,
                    index = 0)
{
    if (typeof object != "object") object = ifc_object(object, index);

    switch (object.type)
    {
    case "checkbox":
    case "radio":

        object.click();
        break;

    case "text":
    case "textarea":

        ifc_focus(object);
        object.select();
        return;
    };

    ifc_focus(object);
};

function ifc_limit(object,
                   limit)
{
    var value = ifc_get(object);

    if (value.length > limit) ifc_set(object, value.substring(0, limit));
};

//==================================================================================================
//   LIST
//==================================================================================================

function ifc_list_activate(name = "list")
{
    var list = document.querySelectorAll("INPUT[name^=\"" + name + "[\"][name$=\"]\"");
    list.forEach(object => { if (! object.checked) object.click(); });
};

function ifc_list_invert(name = "list")
{
    var list = document.querySelectorAll("INPUT[name^=\"" + name + "[\"][name$=\"]\"");
    list.forEach(object => { object.click(); });
};

function ifc_list_deactivate(name = "list")
{
    var list = document.querySelectorAll("INPUT[name^=\"" + name + "[\"][name$=\"]\"");
    list.forEach(object => { if (object.checked) object.click(); });
};

//==================================================================================================
//   TEXTAREA
//==================================================================================================

function ifc_format(object,
                    index = 0)
{
    ifc_clean(object, index, true);
}

function ifc_clean(object,
                   index,
                   format = false)
{
    if (typeof object !== "object") object = ifc_object(object, index);

    var value       = ifc_get(object);
    var scroll_top  = object.scrollTop;
    var scroll_left = object.scrollLeft;

    //remove carriage return and zero-width characters
    value = value.replace(/[\r\u200C\u200D\uFEFF]/g, "");

    //replace hard spaces with normal space
    value = value.replace(/[\u00A0\u1680\u180E\u2000-\u200B\u202F\u205F\u3000]/g, " ");

    //replace line/paragraph separators with linebreak
    value = value.replace(/[\u2028\u2029]/g, "\n");

    //replace tab with 4 spaces
    value = value.replace(/\t/g, "    ");

    //remove whitepace before linebreak
    value = value.replace(/[^\S\n]+\n/g, "\n");

    if (format)
    {
        //replace multiple spaces or a single linebreak with a space
        value = value.replace(/(\S)( +|\n)(\S)/g, "$1 $3");

        //reduce 3+ linebreaks to 2
        value = value.replace(/\n{3,}/g, "\n\n");
    };

    //remove leading whitespace
    value = value.replace(/^(\s*\n)+/, "");

    //remove trailing whitespace
    value = value.replace(/\s+$/, "");

    ifc_set(object, value);
    _ifc_scroll(object, scroll_top, scroll_left);
};

function ifc_keydown(event)
{
    var key = event.key;

    switch (key)
    {
    case "Alt":
    case "Control":
    case "Meta":
    case "Shift":

        return true;
    };

    key = (event.altKey                     ? "[Alt]"     : "") +
          ((event.ctrlKey || event.metaKey) ? "[Control]" : "") +
          (event.shiftKey                   ? "[Shift]"   : "") +
          key;

    var object       = event.target;
    var text         = object.value;
    var select_start = 0;
    var select_end   = 0;
    var select_text  = "";
    var select_flag  = 0;
    var flag         = object instanceof HTMLTextAreaElement;
    var _return      = false;

    if (flag)
    {
        select_start   = object.selectionStart;
        select_end     = object.selectionEnd;
        select_text    = text.substring(select_start, select_end);
        var scroll_top = object.scrollTop;
    }
    else if (window.getSelection !== undefined)
    {
        var selection = window.getSelection();
        var range     = (selection.rangeCount === 0) ? document.createRange() : selection.getRangeAt(0);
        var _range    = document.createRange();
        _range.selectNodeContents(object);

        if (range.compareBoundaryPoints(range.START_TO_START, _range) === 1)
            _range.setStart(range.startContainer, range.startOffset);

        if (range.compareBoundaryPoints(range.END_TO_END, _range) === -1)
            _range.setEnd(range.endContainer, range.endOffset);

        select_text  = _range.cloneContents().textContent;
        _range.setStart(object, 0);
        select_end   = _range.cloneContents().textContent.length;
        select_start = select_end - select_text.length;
    }
    else
    {
        return true;
    };

    switch (key)
    {
    case "Backspace":

        if (select_text !== "") { _return = true; break; };
        if (select_start === 0) break;

        var space  = 0;            //current line indentation
        var _space = 0;            //initial line indentation
        var line   = 0;            //current line number
        var _flag  = true;         //break flag
        var __flag = false;        //non space character flag
        var i      = select_start; //current text position

        while (_flag && (i >= -3))
        {
            switch ((--i < 0) ? -1 : text.charCodeAt(i))
            {
            case -1: //outside beginning of text

                __flag = true;

            case 10: //line feed

                if (++line === 1) //initial line
                {
                    _space = space; //store
                }
                else if (_space === 0) //cursor at beginning of line
                {
                    if (line === 2) select_start -= __flag ? 1 : space + 1;

                    if (__flag)
                    {
                        if (line > 2) select_text = " ".repeat(space);
                        _flag = false;
                    };
                }
                else if (__flag && (space < _space))
                {
                    select_start -= _space - space;
                    _flag         = false;
                };

                space  = 0;
                __flag = false;
                break;

            case 13: //carriage return

                break;

            case 32: //space

                space++;
                break;

            default:

                if (line === 0) //cursor after indentation
                {
                    //select_start--;
                    _flag   = false;
                    _return = true;
                    break;
                };

                space  = 0;
                __flag = true;
                break;
            };
        };
        break;

    case "Tab":
    case "[Shift]Tab":

        select_flag   = select_text.indexOf("\n") !== -1; //multiline
        select_start -= text.substr(0, select_start).match(/(?:\n|^)([^\n]*)$/)[1].length;
        select_end    = select_flag ?
                        select_end + text.substr(select_end).match(/^([^\r\n]*)(?:\r?\n|$)/)[1].length :
                        select_start + text.substr(select_start).match(/^[^\S\r\n]*/)[0].length;
        select_text   = text.substring(select_start, select_end);
        select_text   = event.shiftKey ?
                        select_text.replace(select_flag ? /^ {1,4}/gm : /^ {1,4}/, "") :
                        select_text.replace(select_flag ? /^/gm       : /^/      , "    ");
        break;

    case "Enter":

        var space = "";

        //expand to start of line
        for (var i = select_start - 1; i >= 0; i--)
        {
            switch (text.charCodeAt(i))
            {
            case 10: i      = 0;   break;
            case 32: space += " "; break;
            default: space  = "";  break;
            };
        };

        select_text = "\n" + space;
        if (select_end === text.length) text += "\n";
        break;

    case "Home":

        select_text = text.substr(0, select_start).match(/( *)([^\n]*)$/);

        if (select_text[0].length === 0)
        {
            select_text   = text.substr(select_start).match(/^( *)/);
            select_start += select_text[1].length;
        }
        else
        {
            select_start -= (select_text[2] !== "") ?
                            select_text[2].length :
                            select_text[1].length;
        };

        select_end  = select_start;
        select_text = "";
        break;

    case "[Alt]f":

        ifc_format(object);
        return false;

    case "[Alt]w":

        ifc_clean(object);
        return false;

    case "[Control]y":
    case "[Control][Shift]Z":

        ifc_state_redo(object);
        return false;

    case "[Control]z":

        ifc_state_undo(object);
        return false;

    case "ArrowDown":
    case "ArrowLeft":
    case "ArrowRight":
    case "End":
    case "PageDown":
    case "PageUp":

        return true;

    default:

        _return = true;
    };

    ifc_state_save(object);

    if (_return) return true; //allow default action

    object.value = text.substring(0, select_start) +
                   select_text +
                   text.substring(select_end);
    select_end   = select_start + select_text.length;
    select_start = select_flag ? select_start : select_end;

    if (flag)
    {
        object.selectionStart = select_start;
        object.selectionEnd   = select_end;
        object.scrollTop      = scroll_top;
    }
    else
    {
        selection.removeAllRanges();
        select_start = ifc_get_position_in_context(object, select_start);
        select_end   = ifc_get_position_in_context(object, select_end);
        range.setStart(select_start.node, select_start.position);
        range.setEnd(select_end.node, select_end.position);
        selection.addRange(range);
    };

    delete object["ifc_highlight_value"];
    return false;
};

//==================================================================================================
//   LANGUAGE
//==================================================================================================

var ifc_language_separator = String.fromCharCode(31);
var ifc_language_flag      = false;

function ifc_language_select_all(language)
{
    ifc_language_flag = true;

    var object    = null;
    var _id       = "";
    var _language = "";
    var i         = 0;
    var _i        = 0;

    while (object = document.links[i++])
    {
        if (object.className.substr(0, 9) != "language-") continue;
        _id       = object.id;
        _i        = _id.indexOf(":")
        if (_i === -1)                                    continue;
        _language = _id.substr(_i + 1);
        if (_language === language)                       object.onclick();
    };

    ifc_language_flag = false;
    return false;
};

function ifc_language_select(source,
                             target,
                             language)
{
    if (typeof target !== "object") target = ifc_object(target);

    if (target.language === language)
    {
        if (ifc_language_flag) return false;
        return ifc_language_select_all(language);
    };

    ifc_language_highlight((typeof source === "object") ? source.name : source, language);
    ifc_language_load(source, target, language, true);
    return false;
};

function ifc_language_highlight(id,
                                language)
{
    var object    = null;
    var _id       = "";
    var _language = "";
    var length    = id.length + 1;
    var i         = 0;

    while (object = document.links[i++])
    {
        _id              = object.id;
        if (_id.substr(0, length) != id + ":") continue;
        _language        = _id.substr(length);
        object.className = "language-" + ((_language === language) ? "on" : "off");
    };
};

function ifc_language_load(source,
                           target,
                           language,
                           select = true)
{
    if (typeof source !== "object") source = ifc_object(source);
    if (typeof target !== "object") target = ifc_object(target);
    var scroll_top    = target.scrollTop;
    var scroll_left   = target.scrollLeft;
    ifc_state_purge(target);
    target["ifc_state_nosave"] = true;
    target.value      = ifc_language_get(source.value, language);
    delete target["ifc_state_nosave"];
    target.language   = language;
    if (ifc_language_flag) return;

    if (select)
    {
        if (target.type.match(/^(text|textarea)$/) && (target.value.length <= 20))
            target.select();
        target.focus();
        ifc_scroll(target, scroll_top, scroll_left);
    };
};

function ifc_language_reload(source,
                             target)
{
    var language = "";
    if (typeof source != "object") source   = ifc_object(source);
    if (typeof target != "object") target   = ifc_object(target);
    if (target.language != null)   language = target.language;
    ifc_language_load(source, target, language);
};

function ifc_language_save(source,
                           target)
{
    var language = "";
    if (typeof source != "object") source   = ifc_object(source);
    if (typeof target != "object") target   = ifc_object(target);
    if (source.language != null)   language = source.language;
    target.value = ifc_language_set(target.value, source.value, language);
};

function ifc_language_get(text,
                          language)
{
    var i  = 0;
    var _i = 0;
    if (language === "")
    {
        if ((i = text.indexOf(ifc_language_separator)) === -1) i = text.length;
        return text.substring(0, i);
    };
    if ((i = text.indexOf(ifc_language_separator + language + ":", i)) === -1) return "";
    var c  = language.length + 2;
    if ((_i = text.indexOf(ifc_language_separator, i + c)) === -1)             _i = text.length;
    return text.substring(i + c, _i);
};

function ifc_language_set(text,
                          value,
                          language)
{
    var i  = 0;
    var _i = 0;
    if (language === "")
    {
        if ((i = text.indexOf(ifc_language_separator)) === -1) i = text.length;
        return value + text.substr(i);
    };
    if (value != "")
        value = ifc_language_separator + language + ":" + value;
    if ((i = text.indexOf(ifc_language_separator + language + ":", i)) === -1)
        return text + value;
    var c  = language.length + 2;
    if ((_i = text.indexOf(ifc_language_separator, i + c)) === -1)
        _i = text.length
    return text.substring(0, i) + value + text.substr(_i);
};

//==================================================================================================
//   COMMON
//==================================================================================================

function ifc_object(name,
                    index  = 0,
                    window = this)
{
    if (index >= 0)
    {
        var list = window.document.getElementsByName(name);
        if (list[index]) return list[index];
    };

    var object = window.document.getElementById(name);
    return object ? object : null;
};

function ifc_focus(object,
                   index = 0)
{
    if (typeof object !== "object") object = ifc_object(object, index);

    if (object.disabled)          return;
    if (object.type === "hidden") return;

    object.focus();
};

function ifc_autofocus()
{
    var list   = document.querySelectorAll(
        "CODE[contenteditable], " +
        "INPUT[type=file], " +
        "INPUT[type=text], " +
        "TEXTAREA");
    var object = null;
    var rect   = null;

    for (var i = 0; i < list.length; i++)
    {
        object = list[i];
        rect   = object.getBoundingClientRect();
        if (rect.bottom < 0)               continue;
        if (rect.right < 0)                continue;
        if (rect.top > window.innerHeight) continue;
        if (rect.left > window.innerWidth) continue;

        switch (object.type)
        {
        case "text":

            if (object.value.length <= 20) object.select();

        default:

            object.focus({ preventScroll: true });
        };

        return;
    };
};

function ifc_scroll(object,
                    top,
                    left)
{
    if (typeof object !== "object") object = ifc_object(object);
    if (object.type !== "textarea") return;

    setTimeout(function() { _ifc_scroll(object, top, left); }, 10);
};

function _ifc_scroll(object,
                     top,
                     left)
{
    object.scrollTop  = top;
    object.scrollLeft = left;
};

function ifc_memorize_position()
{
    ifc_set("ifc_left", Math.round(fx_scroll_container.scrollLeft));
    ifc_set("ifc_top",  Math.round(fx_scroll_container.scrollTop));
};

//==================================================================================================
//   LOADING ANIMATION
//==================================================================================================

function ifc_loading_event(event)
{
    if (event !== "window_unload")              return;
    if (document.getElementById("ifc-loading")) return;

    var div = document.createElement("div");
    div.setAttribute("id", "ifc-loading");

    document.body.appendChild(div);
};

function _ifc_loading_event()
{
    fx_visible("ifc-loading", false);
};

//==================================================================================================
//   DOWNLOAD
//==================================================================================================

function ifc_download(url)
{
    const object         = document.createElement("a");
    object.href          = url;
    object.style.display = "none";

    document.body.appendChild(object);
    object.click();
    setTimeout(() => {
        document.body.removeChild(object);
        _ifc_loading_event();
    }, 1000);
};

//==================================================================================================
//   CUSTOM SELECT
//==================================================================================================

function ifc_custom_select(object)
{
    if (typeof object !== "object") object = ifc_object(object, -1);

    Object.defineProperties(object,
    {
        type:
        {
            get()
            {
                return object.querySelector("INPUT[type=checkbox]") ?
                       "select-multiple" : "select-one";
            }
        },

        value:
        {
            get()
            {
                var item = object.querySelector("INPUT:checked");
                return item ? item.value : "";
            },
            set(value)
            {
                var list = object.querySelectorAll("INPUT");
                list.forEach(item => { item.checked = item.value === value; });
                change();
            }
        },

        selectedIndex:
        {
            get()
            {
                var list = object.querySelectorAll("INPUT");
                return Array.from(list).findIndex(item => item.checked);
            },
            set(index)
            {
                var list = object.querySelectorAll("INPUT");
                list.forEach((item, i) => { item.checked = (i === index); });
                change();
            }
        },

        options:
        {
            get()
            {
                var list = object.querySelectorAll("INPUT");
                return Array.from(list).map(function(item, index)
                {
                    var label = item.closest("label");
                    return {index:    index,
                        value:    item.value,
                        selected: item.checked,
                        text:     label ? label.textContent.trim() : ""};
                });
            }
        }
    });

    object._value = object.value;

    function change()
    {
        var __value = object.value;
        if (__value === object._value) return;
        object._value = __value;
        object.dispatchEvent(new Event("change"));

        var code = object.getAttribute("data-onchange");
        if (code) new Function(code).call(object);
    };

    object.querySelectorAll("INPUT").forEach(item =>
    {
        fx_event_listen(item, "change", change);

        if (item.type !== "checkbox") return;

        var label = item.closest("LABEL");
        if (! label) return;
        fx_event_listen(label, "click", event =>
        {
            if (event.target === item) return;
            event.preventDefault();
            if (event.detail !== 1) return;
            var list = object.querySelectorAll("INPUT");
            list.forEach(_item => { _item.checked = _item === item; });
            change();
        }, false);
    });

    var size = object.getAttribute("data-size");
    if (size !== null) object.style.setProperty("--size", size);

    var item = object.querySelector("INPUT:checked");
    if (item) (item.closest("label") || item).scrollIntoView({ block: "start" });
};

//==================================================================================================
//   SYNTAX HIGHLIGHTING
//==================================================================================================

var ifc_highlight_php    = [ /<\?(?:php\s|=)/i, 30, "php", 1 ];
var ifc_highlight_token  = [ /(?:^|[^\\])(%%(?:%(?!%)|[^%\s])*)/, 50, "token", 2 ];
var ifc_highlight_detect = [];

//html
ifc_highlight_detect[0] =
[
    ifc_highlight_php,
    ifc_highlight_token,
    [ /<!--/, 4, "html-comment" ],
    [ /<script(?:\s[^>]*|)>/i, 20, "script" ],
    [ /<style(?:\s[^>]*|)>/i, 10, "style" ],
    [ /<\/?CMS:[^\s="'\/>]*/, 1, "html-cms", 4 ],
    [ /<[^\s="'<>]+/, 1, "html-tag" ]
];

//html tag
ifc_highlight_detect[1] =
[
    ifc_highlight_php,
    ifc_highlight_token,
    [ /[^\s="'\/>]+/, 0, "html-attribute" ],
    [ /"/, 2, "html-string" ],
    [ /'/, 3, "html-string" ],
    [ />/, -1 ]
];

//html string (double quotes)
ifc_highlight_detect[2] =
[
    ifc_highlight_php,
    ifc_highlight_token,
    [ /"/, -1 ]
];

//html string (single quotes)
ifc_highlight_detect[3] =
[
    ifc_highlight_php,
    ifc_highlight_token,
    [ /'/, -1 ]
];

//html comment
ifc_highlight_detect[4] =
[
    ifc_highlight_php,
    ifc_highlight_token,
    [ /-->/, -1 ]
];

//style
ifc_highlight_detect[10] =
[
    ifc_highlight_php,
    ifc_highlight_token,
    [ /"/, 11, "style-string" ],
    [ /'/, 12, "style-string" ],
    [ /\/\*/, 13, "style-comment" ],
    [ /[\(\)\[\]\{\}]/, 0, "style-bracket" ],
    [ /(#[\da-f]{3}(?:[\da-f]{3})?\b|\d*\.?\d+)/i, 0, "style-number" ],
    [ /(?:\d*\.?\d+)([%a-z]+)/i, 0, "style-unit" ],
    [ /@-?[_a-z]+[-\w]*(?=[^;\}]*(?:;|\{))/i, 0, "style-atkeyword" ],
    [ /#-?[_a-z]+[-\w]*(?=[^;\}]*\{)/i, 0, "style-identifier" ],
    [ /\.-?[_a-z]+[-\w]*(?=[^;\}]*\{)/i, 0, "style-class" ],
    [ /::?-?[_a-z]+[-\w]*(?=[^;\}]*\{)/i, 0, "style-selector" ],
    [ /(?:\*\/|;|\{)\s*(-?[_a-z]+[-\w]*)(?=\s*:)(?![^;\}]*\{)/i, 0, "style-property" ],
    [ /(?:var\(|\*\/|;|\{)\s*(--[_a-z]+[-\w]*)/i, 0, "style-variable" ],
    [ /!-?[_a-z]+[-\w]*/i, 0, "style-important" ],
    [ /(?:^|[^-\w])(-?[_a-z]+[-\w]*)/i, 0, "style-name" ],
    [ /<\/style(?:\s[^>]*|)>/i, -1, "", 8 ]
];

//style string (double quotes)
ifc_highlight_detect[11] =
[
    ifc_highlight_php,
    ifc_highlight_token,
    [ /\\./, 0, "style-escape" ],
    [ /"/, -1 ]
];

//style string (single quotes)
ifc_highlight_detect[12] =
[
    ifc_highlight_php,
    ifc_highlight_token,
    [ /\\./, 0, "style-escape" ],
    [ /'/, -1 ]
];

//style comment (multiline)
ifc_highlight_detect[13] =
[
    ifc_highlight_php,
    ifc_highlight_token,
    [ /\*\//, -1 ]
];

//script
ifc_highlight_detect[20] =
[
    ifc_highlight_php,
    ifc_highlight_token,
    [ /"/, 21, "script-string" ],
    [ /'/, 22, "script-string" ],
    [ /\/\*/, 23, "script-comment" ],
    [ /\/\//, 24, "script-comment" ],
    [ /[\(\)\[\]\{\}]/, 0, "script-bracket" ],
    [ /(?:0x[\da-f]+|0b[01]+|\d*\.?\d+(?:e[+-]?\d+)?)/i, 0, "script-number" ],
    [ /[$_a-z][$\w]*/i, 0, "script-name" ],
    [ /(?:^|[^"$')<\]\s\w]|\b(?:return|yield))\s*(\/)(?![\*\/])/, 25, "script-regex" ],
    [ /<\/script(?:\s[^>]*|)>/i, -1, "", 8 ]
];

//script string (double quotes)
ifc_highlight_detect[21] =
[
    ifc_highlight_php,
    ifc_highlight_token,
    [ /\\./, 0, "script-escape" ],
    [ /"/, -1 ]
];

//script string (single quotes)
ifc_highlight_detect[22] =
[
    ifc_highlight_php,
    ifc_highlight_token,
    [ /\\./, 0, "script-escape" ],
    [ /'/, -1 ]
];

//script comment (multiline)
ifc_highlight_detect[23] =
[
    ifc_highlight_php,
    ifc_highlight_token,
    [ /\*\//, -1 ]
];

//script comment (single line)
ifc_highlight_detect[24] =
[
    ifc_highlight_php,
    ifc_highlight_token,
    [ /\n/, -1 ]
];

//script regex
ifc_highlight_detect[25] =
[
    ifc_highlight_php,
    ifc_highlight_token,
    [ /\\./, 0, "script-escape" ],
    [ /\/[a-z]*/i, -1 ]
];

//php
ifc_highlight_detect[30] =
[
    ifc_highlight_token,
    [ /"/, 31, "php-string" ],
    [ /'/, 32, "php-string" ],
    [ /\/\*/, 33, "php-comment" ],
    [ /\/\//, 34, "php-comment" ],
    [ /[\(\)\[\]\{\}]/, 0, "php-bracket" ],
    [ /(?:0x[\da-f]+|0b[01]+|\d*\.?\d+(?:e[+-]?\d+)?)/i, 0, "php-number" ],
    [ /\$\w+/, 0, "php-variable" ],
    [ /[_a-z]\w*/i, 0, "php-name" ],
    [ /\?>/, -1, "", 8 ]
];

//php string (double quotes)
ifc_highlight_detect[31] =
[
    ifc_highlight_token,
    [ /(?:[^\\]?)(\$\w+)/, 0, "php-variable" ],
    [ /\\./, 0, "php-escape" ],
    [ /"/, -1 ]
];

//php string (single quotes)
ifc_highlight_detect[32] =
[
    ifc_highlight_token,
    [ /(?:[^\\]?)(\$\w+)/, 0, "php-variable" ],
    [ /\\./, 0, "php-escape" ],
    [ /'/, -1 ]
];

//php comment (multiline)
ifc_highlight_detect[33] =
[
    ifc_highlight_token,
    [ /\*\//, -1 ]
];

//php comment (single line)
ifc_highlight_detect[34] =
[
    ifc_highlight_token,
    [ /\n/, -1 ]
];

//format
ifc_highlight_detect[40] =
[
    ifc_highlight_token,
    [ /(?:^|[^\\])(\[)/, 41, "format" ],
    [ /(?:\\%|%(?!%)|\\\[|\\]|[^%\[\]])+/, 0, "format-content", 8 ],
    [ /(?:^|[^\\])(])/, -2, "", 8 ]
];

//format-type
ifc_highlight_detect[41] =
[
    ifc_highlight_token,
    [ /\+\s/, 40, "format-type-bold" ],
    [ /\/\s/, 40, "format-type-italic" ],
    [ /_\s/, 40, "format-type-underline" ],
    [ /<\s/, 40, "format-type-big" ],
    [ />\s/, 40, "format-type-small" ],
    [ /<-\s/, 40, "format-type-left" ],
    [ /<->\s/, 40, "format-type-center" ],
    [ /->\s/, 40, "format-type-right" ],
    [ /\*\s/, 40, "format-type-title" ],
    [ /"\s/, 40, "format-type-quote" ],
    [ /-\s/, 40, "format-type-monospace" ],
    [ /(?:\\%|%(?!%)|\\\[|\\]|[^\%\[\]\s])+/, 40, "format-type" ],
    [ /\s/g , 40, "format-type" ],
    [ /(?:^|[^\\])(])/, -1 ]
];

//token
ifc_highlight_detect[50] =
[
    [ /(?:\\%|%(?!%)|\\,|[^%,])+/, 0, "token-value" ],
    [ /(?:^|[^\\])(%%)/, -1 ]
];

function ifc_highlight_init()
{
    var map   = new Map();
    var array = [];
    var value = "";
    var key   = "";
    var i = 0, _i = 0, __i = 0;

    for (i in ifc_highlight_detect)
    {
        array = ifc_highlight_detect[i];

        for (_i in array)
        {
            value = array[_i][0].toString();
            key   = map.get(value);
            if (key === undefined) { key = __i++; map.set(value, key); };
            array[_i][4] = key;
        };
    };
};

ifc_highlight_init();

function ifc_highlight(object)
{
    //check for change
    var text = null;
    if (object.hasOwnProperty("ifc_highlight_value"))
    {
        text      = object.value;
        var _text = object["ifc_highlight_value"];
        if ((text.length === _text.length) && (text === _text))
        {
            delete object["ifc_highlight_time"];
            return;
        };
    };

    var mode      = (arguments.length > 1) ? arguments[1] : 0;
    var no_insert = (arguments.length > 2) ? arguments[2] : false;
    var bounce    = arguments.length > 3;
    var time      = Date.now();
    var delay     = 250;

    //debouncing
    if (! bounce)
    {
        if (! object.hasOwnProperty("ifc_highlight_time"))
            setTimeout(function() { ifc_highlight(object, mode, no_insert, true); }, delay);
        object["ifc_highlight_time"] = time;
        return;
    };

    if ((time - object["ifc_highlight_time"]) < delay)
    {
        setTimeout(function() { ifc_highlight(object, mode, no_insert, true); }, delay);
        return;
    };

    delete object["ifc_highlight_time"];

    if (text === null) text = object.value;
    object["ifc_highlight_value"] = text; //store for comparison

    var classname      = "";
    var buffer         = [];
    var _buffer        = "";
    var state_stack    = [mode];
    var state          = mode;
    var state_temp     = 0;
    var bracket_stack  = [];
    bracket_stack[10]  = [];
    bracket_stack[20]  = [];
    bracket_stack[30]  = [];
    var bracket_match  = { "(" : ")", "[" : "]", "{" : "}" };
    var bracket_id     = "";
    var position       = 0;
    var _position      = -1;
    var index          = 0;
    var count          = 0;
    var match          = null;
    var match_index    = -1;
    var match_position = -1;
    var match_length   = -1;
    var type           = 0;
    var temp           = "";
    var temp_position  = 0;
    var temp_length    = 0;
    var cache          = [];
    var flag           = true;

    //save cursor state
    var restore = ifc_save_selection(object);

    do
    {
        match_index    = -1;
        match_position = -1;
        match_length   = -1;

        //find best match in state
        for (index = 0, count = ifc_highlight_detect[state].length; index < count; index++)
        {
            type = ifc_highlight_detect[state][index][3];

            switch (type)
            {
            case 1: //skip php
            case 2: //skip token
            case 4: //skip cms

                if ((no_insert === true) || (no_insert & type)) continue;
                break;

            case 8: //skip at root

                if ((mode === state) && (state_stack.length === 1)) continue;
                break;
            };

            temp = ifc_highlight_detect[state][index][4];
            flag = true;

            //retrieve search from cache
            if (cache[temp])
            {
                temp_position = cache[temp][0];
                if (temp_position === -1) continue;
                if (temp_position >= position)
                {
                    temp_length = cache[temp][1];
                    flag        = false;
                };
            };

            //execute new search
            if (flag)
            {
                if (_buffer === "") _buffer = text.slice(position);
                match = ifc_highlight_detect[state][index][0].exec(_buffer);

                if (match === null) { cache[temp] = [-1]; continue; };

                temp_length   = match[match.length - 1].length;
                temp_position = position + match.index + (match[0].length - temp_length);
                cache[temp]   = [temp_position, temp_length];
            };

            //earlier is considered better
            if ((match_position > -1) && (temp_position >= match_position)) continue;

            //store current match
            match_index    = index;
            match_position = temp_position;
            match_length   = temp_length;

            //no better match possible
            if (temp_position === position) break;
        };

        //process best match
        if (match_index > -1)
        {
            buffer.push(htmlspecialchars(text.substring(position, match_position)));
            state_temp = ifc_highlight_detect[state][match_index][1];

            //enter state
            if (state_temp > 0)
            {
                classname = ifc_highlight_detect[state][match_index][2];
                buffer.push("<span class=\"hl-" + classname + "\">" +
                            htmlspecialchars(text.substr(match_position, match_length)));
                state     = state_temp;
                state_stack.push(state);
            }

            //exit state
            else if (state_temp < 0)
            {
                buffer.push("</span>".repeat(Math.abs(state_temp) - 1) +
                            htmlspecialchars(text.substr(match_position, match_length)) +
                            "</span>");
                for (var i = 0; i > state_temp; i--) state_stack.pop();
                state = state_stack[state_stack.length - 1];
            }

            //keep state
            else
            {
                classname  = ifc_highlight_detect[state][match_index][2];
                _buffer    = text.substr(match_position, match_length);
                bracket_id = "";

                //find and mark bracket pairs
                switch (state)
                {
                case 10:
                case 20:
                case 30:

                    if (! classname.endsWith("-bracket")) break;

                    switch (_buffer)
                    {
                    case "(":
                    case "[":
                    case "{":

                        temp       = bracket_match[_buffer];
                        bracket_id = " id=\"hl-bracket-open-" + match_position + "\"";
                        bracket_stack[state].push([temp, match_position]);
                        break;

                    case ")":
                    case "]":
                    case "}":

                        temp = bracket_stack[state].pop();
                        if (temp && (temp[0] === _buffer))
                            bracket_id = " id=\"hl-bracket-close-" + temp[1] + "\"";
                    };
                };

                buffer.push("<span" + bracket_id + " class=\"hl-" + classname + "\">" +
                            htmlspecialchars(_buffer) +
                            "</span>");
            };

            position = match_position + match_length;
            _buffer  = "";
        };
    }
    while (match_index > -1);

    buffer.push(htmlspecialchars(text.substr(position)));
    buffer = buffer.join("");

    //line numbers
    flag   = fx_style(object, "white-space") === "pre";
    buffer = flag ?
             buffer.replace(/^/gm, "<span class=\"hl-line\"></span>") :
             buffer.replace(/^(?!$)/gm, "<span class=\"hl-line\"></span>");

    temp           = object.nextElementSibling.style;
    temp.display   = "none";
    temp.minHeight = (buffer.match(/^/gm).length * parseInt(fx_style(object, "line-height"))) + "px";
    temp.display   = "";

    //update contenteditable
    object.style.display = "none";
    object.innerHTML     = buffer;

    if (! flag) object.style.display = ""; //flexible height

    //restore cursor state
    restore();

    if (flag) object.style.display = ""; //fixed height

    console.log("Highlighting took " + (Date.now() - time) + " ms.");
};

function ifc_highlight_bracket(object)
{
    object.querySelectorAll("SPAN[id^=\"hl-bracket-\"].active").forEach(
        node => node.classList.remove("active"));

    var selection = window.getSelection();
    if (selection.rangeCount === 0) return;

    var node = selection.focusNode;
    if (! object.contains(node)) return;

    if (node.nodeType === Node.TEXT_NODE)
    {
        let _node = node.parentElement;

        if (_node.id && _node.id.startsWith("hl-bracket-"))
        {
            if (selection.focusOffset > 0)
            {
                let __node = _node.nextElementSibling;
                node = (__node.id && __node.id.startsWith("hl-bracket-")) ? _node : __node;
            }
            else
            {
                node = _node.previousElementSibling;
            };
        };
    };

    var match = null;
    var depth = 0;

    while (node && (node !== object))
    {
        if (node.id && (match = node.id.match(/^hl-bracket-(open|close)-(\d+)$/)))
        {
            depth += (match[1] === "open") ? -1 : 1;

            if (depth < 0)
            {
                node.classList.add("active");
                node = document.getElementById("hl-bracket-close-" + match[2]);
                if (node) node.classList.add("active");
                break;
            };
        };

        node = node.previousElementSibling ?? node.parentElement;
    };
};

function ifc_save_selection(context)
{
    var selection = window.getSelection();
    if (selection.rangeCount === 0)               return function() {};
    if (! context.contains(selection.anchorNode)) return function() {};
    if (! context.contains(selection.focusNode))  return function() {};
    var direction = selection.anchorNode.compareDocumentPosition(selection.focusNode);
    direction     = (((direction === 0) && (selection.anchorOffset > selection.focusOffset)) ||
                    (direction === Node.DOCUMENT_POSITION_PRECEDING));
    var range     = selection.getRangeAt(0);
    var _range    = range.cloneRange();
    var length    = _range.cloneContents().textContent.length;
    _range.setStart(context, 0);
    var end       = _range.cloneContents().textContent.length;
    var start     = end - length;

    return function()
    {
        var focus  = document.activeElement;
        var range  = new Range();
        var _start = ifc_get_position_in_context(context, start);
        var _end   = ifc_get_position_in_context(context, end);

        if (_start.node === null)
        {
            if (_end.node === null) return;
            _start = _end;
        }
        else if (_end.node === null)
        {
            _end = _start;
        };

        selection.removeAllRanges();
        range.setStart(_start.node, _start.position);
        range.setEnd(_end.node, _end.position);
        selection.addRange(range);

        if (direction) _start = [ _end, _end = _start ][0];
        selection.collapse(_start.node, _start.position);
        selection.extend(_end.node, _end.position);
        focus.focus();
    };
};

function ifc_get_position_in_context(context,
                                     position)
{
    var node = null;

    var treeWalker = document.createTreeWalker(context, NodeFilter.SHOW_TEXT, function(object)
    {
        if (position >= object.textContent.length + 1)
        {
            position -= object.textContent.length;
            return NodeFilter.FILTER_REJECT
        };

        return NodeFilter.FILTER_ACCEPT;
    });

    node = treeWalker.nextNode();

    return { "node" : node, "position" : position };
};

function ifc_contenteditable_init(id)
{
    var object            = document.getElementById(id);
    var placeholder       = document.createElement("div");
    placeholder.className = "placeholder";
    object.insertAdjacentElement("afterend", placeholder);

    Object.defineProperty(object, "type",
    {
        value: "textarea"
    });

    Object.defineProperty(object, "value",
    {
        get: function()
        {
            return this.textContent;
        },

        set: function(value)
        {
            var _value       = this.textContent;
            if ((value.length === _value.length) && (value === _value)) return;
            ifc_state_save(this);
            this.textContent = value;
            var event        = document.createEvent("HTMLEvents");
            event.initEvent("change", false, true);
            this.dispatchEvent(event);
        }
    });

    Object.defineProperty(object, "select",
    {
        value: function ()
        {
            var selection = window.getSelection();
            var range     = document.createRange();

            range.selectNodeContents(this);
            selection.removeAllRanges();
            selection.addRange(range);
        },
    });

    fx_event_listen(document, "selectionchange", () => ifc_highlight_bracket(object));
};

//==================================================================================================
//   UNDO / REDO
//==================================================================================================

function ifc_state_save(object)
{
    if (object.hasOwnProperty("ifc_state_nosave")) return;

    var value = object.value;

    if (! object.hasOwnProperty("ifc_state_undo"))
    {
        object["ifc_state_undo"]     = [];
        object["ifc_state_redo"]     = [];
        object["ifc_state_position"] = -2;
    };

    var start = object["ifc_state_position"];
    var range = ifc_state_get_range(object);
    object["ifc_state_position"] = range[0];

    if (object["ifc_state_undo"].length > 0)
    {
        if (Math.abs(range[0] - start) <= 1) return;
        var _value = object["ifc_state_undo"][object["ifc_state_undo"].length - 1][0];
        if ((value.length === _value.length) && (value === _value)) return;
    };

    object["ifc_state_undo"].push([object.value, range]);
    object["ifc_state_undo"] = object["ifc_state_undo"].slice(-100);
    object["ifc_state_redo"] = [];
};

function ifc_state_undo(object,
                        redo = false)
{
    var get = "ifc_state_" + (redo ? "redo" : "undo");
    var set = "ifc_state_" + (redo ? "undo" : "redo");

    if (! object.hasOwnProperty(get)) return;
    if (object[get].length === 0)     return;

    object[set].push([object.value, ifc_state_get_range(object)]);
    object[set] = object[set].slice(-100);

    var state    = object[get].pop();
    object["ifc_state_nosave"] = true;
    object.value = state[0];
    delete object["ifc_state_nosave"];
    delete object["ifc_highlight_value"]
    ifc_state_set_range(object, state[1][0], state[1][1]);
};

function ifc_state_redo(object)
{
    ifc_state_undo(object, true);
};

function ifc_state_get_range(object)
{
    var start = 0;
    var end   = 0;

    if (object instanceof HTMLTextAreaElement)
    {
        start = object.selectionStart;
        end   = object.selectionEnd;
    }
    else if (window.getSelection !== undefined)
    {
        var selection = window.getSelection();
        var range     = (selection.rangeCount === 0) ? document.createRange() : selection.getRangeAt(0);
        var _range    = document.createRange();
        _range.selectNodeContents(object);

        if (range.compareBoundaryPoints(range.START_TO_START, _range) === 1)
            _range.setStart(range.startContainer, range.startOffset);

        if (range.compareBoundaryPoints(range.END_TO_END, _range) === -1)
            _range.setEnd(range.endContainer, range.endOffset);

        var text  = _range.cloneContents().textContent;
        _range.setStart(object, 0);
        end       = _range.cloneContents().textContent.length;
        start     = end - text.length;
    };

    return [start, end];
};

function ifc_state_set_range(object,
                             start,
                             end)
{
    if (object instanceof HTMLTextAreaElement)
    {
        object.selectionStart = start;
        object.selectionEnd   = end;
    }
    else
    {
        var selection = window.getSelection();
        var range     = document.createRange();
        var start     = ifc_get_position_in_context(object, start);
        var end       = ifc_get_position_in_context(object, end);

        selection.removeAllRanges();
        range.setStart(start.node, start.position);
        range.setEnd(end.node, end.position);
        selection.addRange(range);
    };
};

function ifc_state_purge(object)
{
    delete object["ifc_state_undo"];
    delete object["ifc_state_redo"];
    delete object["ifc_state_position"];
    delete object["ifc_state_nosave"];
};

//==================================================================================================
//   FILE UPLOAD PROGRESS
//==================================================================================================

function ifc_show_upload_progress(form)
{
    //check for file inputs
    const list = form.querySelectorAll("INPUT[type=\"file\"]");
    if (! list.length) return; //none found

    fx_event_listen(form, "submit", (e) =>
    {
        //prevent multiple submits
        if (form.uploading)
        {
            e.preventDefault();
            return;
        };

        //retrieve active file inputs
        const _list = [];

        list.forEach(file =>
        {
            if (! file.files.length) return;

            //create progress element
            const object = document.createElement("progress");
            object.max   = 100;
            object.value = 0;

            file.before(object);
            _list.push(object);
        });

        if (! _list.length) return; //none found

        form.uploading = true; //mark active

        //override default form submission
        const request = new XMLHttpRequest();
        let time      = Date.now();
        let timeout   = false;

        e.preventDefault();

        //display alternative animation if not progressing
        const timeout_check = () =>
        {
            if (timeout) return;

            //progress update timeout
            if ((Date.now() - time) > 250)
            {
                form.classList.add("timeout");
                timeout = true;
                return;
            };

            setTimeout(timeout_check, 100);
        };

        //display progress
        request.upload.onprogress = (e) =>
        {
            if (! e.lengthComputable) return;

            //transmission completed
            if (e.loaded === e.total)
            {
                timeout = true;
                _list.forEach(object => { if (object) object.value = 100; });
                return;
            };

            //update progress value
            const value = Math.floor(e.loaded * 100 / e.total);
            _list.forEach(object => { if (object) object.value = value; });

            time = Date.now(); //timestamp

            //restart timeout check
            if (timeout)
            {
                timeout = false;
                form.classList.remove("timeout");
                timeout_check();
            };
        };

        //display result page
        request.onload = () =>
        {
            //we use this method instead of document.write
            //to prevent javascript redeclaration conflicts
            const blob    = new Blob([request.responseText], { type: "text/html" });
            const url     = URL.createObjectURL(blob);
            location.href = url;
        };

        //upload failed, clean up
        request.onerror = () =>
        {
            if (timeout) form.classList.remove("timeout");
            else         timeout = true;

            _list.forEach(object => { if (object) object.remove(); });

            form.uploading = false;
        };

        //send form
        request.open(form.method || "post", form.action || location.href);
        request.send(new FormData(form));

        //trigger global loading animation
        ifc_loading_event("window_unload");

        //run timeout check
        timeout_check();
    },
    false);
};