<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   DESKTOP INTERFACE
//==================================================================================================

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "reload":
//..................................................................................................

    $ifc_param1 = NULL;
    break;

case "save":
//..................................................................................................

    $desktop->object_set($object, "url", $ifc_param1);
    $ifc_response = $desktop->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

$param = ["user"   => DESKTOP_USER,
          "object" => $object];

//determine url
if (isset($ifc_param1) && nstre($ifc_param1))              $url = $ifc_param1;
elseif (stre($url = $desktop->object_get($object, "url"))) $url = CMS_URL . "blank.htm";

$url = cms_url($url, NULL, TRUE);

//display
$ifc = new ifc(NULL,
               $ifc_page,
               [CMS_L_DESKTOP_LINK_003 . "|desktop/command_refresh"  => "reload",
                CMS_L_COMMAND_SAVE .     "|desktop/command_save"     => "save",
                CMS_L_DESKTOP_LINK_001 . "|desktop/command_breakout" => "javascript:desktop_link_breakout();"],
               $param,
               NULL,
               CMS_L_DESKTOP_LINK_002);

//url input
$ifc->set(CMS_L_URL, "text 60 256 :", $url);
$ifc->set("Laden",   "button b",      "javascript:desktop_link_load()");

//menu open button
echo("<a id=\"ifc-desktop-link-menu\" " .
        "href=\"javascript:;\">" .
     "…" .
     "</a>" .

     //content frame
     "<iframe id=\"desktop-link-output\" " .
                  "src=\"" . x(cms_url($url)) . "\" " .
                  "allow=\"camera *; microphone *; geolocation *; " .
                          "fullscreen *; autoplay *; payment *; " .
                          "display-capture *; clipboard-write *; clipboard-read *; " .
                          "gamepad *; midi *; accelerometer *; " .
                          "gyroscope *; magnetometer *; xr-spatial-tracking *; " .
                          "picture-in-picture *; web-share *\">" .
     "</iframe>");

?><script>

var iframe     = ifc_object("desktop-link-output");
var ifc_param1 = ifc_object("ifc_param1");

function desktop_link_breakout()
{
    window.location.replace("<?php echo(q($url));?>");
};

function desktop_link_load()
{
    iframe.src = ifc_get(ifc_param1);
};

fx_event_listen(ifc_param1, "keydown", function(event)
{
    if (event.key === "Enter")
    {
        event.preventDefault();
        desktop_link_load();
    };
}, false);

fx_event_listen(iframe, "load", function()
{
    try { ifc_set(ifc_param1, this.contentWindow.location.href); }
    catch {};
});

fx_event_listen("window_load", function()
{
    ifc_param1.blur();
});

</script><?php

$ifc->close();