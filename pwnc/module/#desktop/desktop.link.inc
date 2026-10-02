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
case "display":
//..................................................................................................

    //check cache for csp setting
    $cache_id = "desktop.link_csp.$ifc_param";
    $flag     = (cms_cache_time($cache_id) >= (time() - 3600)) ? cms_cache($cache_id) : NULL;
    $found    = FALSE;

    //no cache and headers retrievable
    if (($flag === NULL) && cms_load("http") && (($header = http_header($ifc_param)) !== FALSE))
    {
        $flag = TRUE; //default to allowed

        //csp header given
        if (isset($header["content-security-policy"]))
        {
            $value = strtolower($header["content-security-policy"]);
            $array = preg_split("/\s*[,;]\s*/", $value, -1, PREG_SPLIT_NO_EMPTY);

            //traverse policies
            foreach ($array AS $value)
            {
                $_array = preg_split("/\s+/", $value, -1, PREG_SPLIT_NO_EMPTY);
                if (nstreq($_array[0] ?? "", "frame-ancestors")) continue; //ignore

                if (! $found) //first frame-ancestors
                {
                    $found = TRUE;
                    $url   = CMS_PROTOCOL . "://" . CMS_DOMAIN . ":" . CMS_PORT;
                };

                $flag = FALSE; //default to forbidden

                //traverse policy tokens
                for ($i = 1, $c = count($_array); $i < $c; $i++)
                {
                    $value = $_array[$i];

                    switch ($value)
                    {
                    case "'none'":

                        $flag = FALSE;
                        break 3;

                    case "'self'":

                        $_url = parse_url($ifc_param);
                        if ($_url === FALSE) break;

                        $_url = ($_url["scheme"] ?? CMS_PROTOCOL) . "://" .
                                ($_url["host"]   ?? CMS_DOMAIN)   . ":" .
                                ($_url["port"]   ?? CMS_PORT);
                        if (streq($url, $_url)) $flag = TRUE;
                        break;

                    case "*":

                        $flag = TRUE;
                        break;

                    default:

                        if (! preg_match("/^([^:]+:\/\/)?(.+?)(:[^:]+)?$/", $value, $value)) break; //invalid

                        if (stre($value[1] ?? "")) $value[1] = CMS_PROTOCOL . "://";
                        if (stre($value[3] ?? "")) $value[3] = ":" . CMS_PORT;
                        $value = $value[1] . $value[2] . $value[3];

                        $replace =
                            ["/(?<=^|\\\\\:\\\\\/\\\\\/)\\\\\*\\\\\./"       => "(?:.+\.)?",
                             "/(?<=^|\\\\\:\\\\\/\\\\\/)\\\\\*(?=\\\\\:|$)/" => ".+",
                             "/\\\\\:\\\\\*$/"                               => "(?:.+$)?"];

                        $regex = preg_quote($value, "/");
                        $regex = preg_replace(array_keys($replace), $replace, $regex, 1);

                        if (preg_match("/^" . $regex . "$/", $url)) $flag = TRUE; //match
                    };
                };

                if (! $flag) break;
            };
        };

        //alternatively xfo header given
        if (! $found && isset($header["x-frame-options"]))
        {
            $value = strtolower(trim($header["x-frame-options"]));

            switch ($value)
            {
            case "deny":

                $flag = FALSE;
                break;

            case "sameorigin":

                $domain = parse_url($ifc_param, PHP_URL_HOST);
                if (stre($domain) || nstreq($domain, CMS_DOMAIN)) $flag = FALSE;
            };
        };

        cms_cache($cache_id, $flag, TRUE);
    };

    if ($flag ?? TRUE)
    {
        header("Location: $ifc_param", TRUE, 301);
        exit();
    };

    $ifc = new ifc(NULL, $ifc_page, NULL);

?><script>

function desktop_link_load()
{
    window.top.location.replace("<?php echo(q($ifc_param));?>");
};

</script><?php

    echo("<div style=\"ALIGN-CONTENT:center;" .
                      "HEIGHT:calc(100cqh - 2 * var(--ifc-padding));" .
                      "TEXT-ALIGN:center\">");
    $ifc->set(t("hint_link_blocks_desktop"), "info b");
    $ifc->set(t("load_without_menu"),        "button", "javascript:desktop_link_load();");
    echo("</div>");

    $ifc->close();

case "reload":
//..................................................................................................

    $ifc_param1 = NULL;
    break;

case "save":
//..................................................................................................

    $desktop->object_set($object, "url", $ifc_param1);
    $ifc_response = $desktop->save() ? t("msg_done") : t("msg_error");
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
$ifc = new ifc(
    NULL,
    $ifc_page,
    [t("reload") .       "|desktop/command_refresh"  => "reload",
     t("command_save") . "|desktop/command_save"     => "save",
     t("remove_menu") .  "|desktop/command_breakout" => "javascript:desktop_link_breakout();"],
    $param,
    NULL,
    t("link"));

//url input
$ifc->set(t("url"), "text 60 :", $url);
$ifc->set("Laden",  "button b",  "javascript:desktop_link_load()");

//menu open button
echo("<a id=\"ifc-desktop-link-menu\" " .
        "href=\"javascript:;\">" .
     "…" .
     "</a>" .

     //content frame
     "<iframe id=\"desktop-link-output\" " .
                  "src=\"" . x(cms_url(["desktop_display" => "interface",
                                        "ifc_message"     => "display",
                                        "ifc_param"       => $url,
                                        "user"            => DESKTOP_USER,
                                        "object"          => $object])) . "\" " .
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