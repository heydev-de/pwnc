/*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

function asr_send(url,
                  callback = null)
{
    var random  = "_";
    var _string = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (var i = 0; i < 8; i++)
        random += _string.charAt(Math.floor(Math.random() * 62));

    [_url, hash] = url.split("#");
    url          = _url + (_url.includes("?") ? "&" : "?") + random + "=1" + (hash ? "#" + hash : "");

    fetch(
        url,
        {
            cache: "no-store"
        })
        .then(response =>
        {
            if (! response.ok) return false;
            return response.text();
        })
        .then(text =>
        {
            if (callback) callback(text);
        })
        .catch(error =>
        {
            if (callback) callback(false);
        });
};

function asr_form_bind(object,
                       callback = null)
{
    object.asr_submit_function1 = function(event)
    {
        event.preventDefault();
        asr_form_post(object, callback);
    };
    object.addEventListener("submit", object.asr_submit_function1);

    object.asr_submit_function2 = object.submit;
    object.submit               = function() { object.dispatchEvent(new Event('submit')); };
};

function asr_form_unbind(object)
{
    object.removeEventListener("submit", object.asr_submit_function1);
    delete object.asr_submit_function1;

    object.submit = object.asr_submit_function2;
    delete object.asr_submit_function2;
};

function asr_form_post(object,
                       callback = "")
{
    var data = new FormData(object);
    var url  = object.action;

    fetch(url, { method: "POST", body: data })
        .then(response =>
        {
            if (! response.ok) return false;
            return response.text();
        })
        .then(text =>
        {
            if (callback) callback(text);
        });
};