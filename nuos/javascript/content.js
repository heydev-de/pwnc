/*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

if (! this.name) this.name = "content_edit_" + parseInt(Math.random() * 99999999);
var content_buffer = null;

function content_edit_open(url)
{
    load_page(url);
};

function content_edit_command(url,
                              text = "")
{
    if ((text !== "") && (! confirm(text + "?"))) return;
    url = url.replace(/%left%/, fx_position_left());
    url = url.replace(/%top%/,  fx_position_top());
    location.replace(url);
};

function content_edit_copy(url,
                           range)
{
    asr_send(url);
    content_buffer = range;
};

function content_edit_paste(url)
{
    content_edit_command(url, CMS_L_COMMAND_PASTE);
};

function content_edit_swap(url)
{
    if (content_buffer) content_edit_command(url, CMS_L_MOD_CONTENT_005);
    else                alert(CMS_L_MOD_CONTENT_002);
};

function content_edit_kick1(url)
{
    var value = prompt(CMS_L_MOD_CONTENT_011, 1);
    if (value === null) return;

    if (! /^[0-9]+$/.test(value))
    {
        alert(CMS_L_MOD_CONTENT_013);
        return;
    };

    url = url.replace(/%return%/, value);
    content_edit_command(url);
};

function content_edit_kick2(url)
{
    var value = prompt(CMS_L_MOD_CONTENT_012, 1);
    if (value === null) return;

    if (! /^[0-9]+$/.test(value))
    {
        alert(CMS_L_MOD_CONTENT_013);
        return;
    };

    url = url.replace(/%return%/, -value);
    content_edit_command(url);
};

function content_edit_clear(url)
{
    content_edit_command(url, CMS_L_COMMAND_DELETE);
};

function content_edit_repeat(url,
                             value)
{
    value = prompt(CMS_L_MOD_CONTENT_009, value);
    if (value === null) return;

    if (! /^[0-9]+$/.test(value))
    {
        alert(CMS_L_MOD_CONTENT_013);
        return;
    };

    url = url.replace(/%return%/, value);
    content_edit_command(url);
};

function content_edit_shift(url,
                            value)
{
    value = prompt(CMS_L_MOD_CONTENT_010, value);
    if (value === null) return;

    if (! /^-?[0-9]+$/.test(value))
    {
        alert(CMS_L_MOD_CONTENT_013);
        return;
    };

    url = url.replace(/%return%/, value);
    content_edit_command(url);
};

function content_edit_switch(url,
                             value)
{
    value = (value === "") ? "1" : "";
    url   = url.replace(/%return%/, value);
    content_edit_command(url);
};

function content_edit_apply(url)
{
    content_edit_command(url, CMS_L_MOD_CONTENT_003);
};

function content_edit_revert(url)
{
    content_edit_command(url, CMS_L_MOD_CONTENT_004);
};

function content_load(url)
{
    parent.location.replace(url);
};