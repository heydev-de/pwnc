<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   MODULE
//==================================================================================================

require("../pwnc.inc");

init($desktop_display);
if (in_array($desktop_display, ["interface", "link"])) $ifc_option = "external";

//load libraries
cms_load("ifc",     TRUE);
cms_load("desktop", TRUE);

//load settings
cms_cache_sync($user, "desktop." . CMS_USER . ".user", TRUE);

//set user
if (
     stre($user)
     ||
     (
       nstreq($user, CMS_SUPERUSER)
       &&
       (! cms_permission(CMS_APPLICATION . ".$user", NULL, NULL))
     )
   )
    $user = CMS_SUPERUSER;

define("DESKTOP_USER", $user);
define("DESKTOP_PATH", CMS_DATA_PATH . "#desktop/" . safe_filename(DESKTOP_USER) . "/");

//permission
$data = new data("#system/permission"); //used again later
ifc_permission([""           => CMS_L_ACCESS,
                DESKTOP_USER => $data->get("user." . DESKTOP_USER, "name")]);

switch ($desktop_display)
{
default:
//..................................................................................................

    $desktop = new desktop(DESKTOP_USER);

    if (! isset($object)) $object = (string)cms_cache("desktop." . DESKTOP_USER . ".object");
    if (! isset($parent)) $parent = (string)cms_cache("desktop." . DESKTOP_USER . ".parent");
    if (! isset($load))   $load   = FALSE;

    //message handling
    switch (CMS_IFC_MESSAGE)
    {
    case "activate":
//..................................................................................................

        if (
             (! $ifc_param)
             ||
             streq($desktop->object_type($ifc_param), "container")
           )
        {
            $parent = $ifc_param;
        }
        else
        {
            $object = $ifc_param;
            $load   = TRUE;
        };

        break;

    case "select":
//..................................................................................................

        $object = $ifc_param;
        break;

    case "create":
//..................................................................................................

        $ifc_param = explode(";", $ifc_param);

        if ($_object = $desktop->create_object($parent, $ifc_param[0], $ifc_param[1]))
        {
            $object = $_object;
            $load   = nstreq($ifc_param[0], "container");
            break;
        };

        $ifc_response = CMS_MSG_ERROR;
        break;

    case "quickaccess":
//..................................................................................................

        $desktop->object_set($object,
                             "quickaccess",
                             ! $desktop->object_get($object, "quickaccess"));

        if (! $desktop->save()) $ifc_response = CMS_MSG_ERROR;
        break;

    case "rename":
//..................................................................................................

        if ($ifc_param)
        {
            $desktop->object_set($object, "name", $ifc_param);
            if ($desktop->save()) break;
        };

        $ifc_response = CMS_MSG_ERROR;
        break;

    case "drop":
//..................................................................................................

        $ifc_param = explode(",", $ifc_param);
        $desktop->object_set($ifc_param[0], "x", $ifc_param[1]);
        $desktop->object_set($ifc_param[0], "y", $ifc_param[2]);

        if ($desktop->save())
        {
            $object = $ifc_param[0];
            break;
        };

        $ifc_response = CMS_MSG_ERROR;
        break;

    case "move":
//..................................................................................................

        if ($ifc_param)
        {
            $ifc_param = explode(",", $ifc_param);
            $object    = $ifc_param[0];
            $parent    = $ifc_param[1];
        };

        if ($_object = $desktop->move_object($object, $parent))
        {
            $desktop->object_set($_object, "x", NULL);
            $desktop->object_set($_object, "y", NULL);
            $object = $_object;
            break;
        };

        $ifc_response = CMS_MSG_ERROR;
        break;

    case "delete":
//..................................................................................................

        if ($desktop->delete_object($ifc_param))
        {
            if (streq($ifc_param, $object)) $object = NULL;
            break;
        };

        $ifc_response = CMS_MSG_ERROR;
        break;

    case "send":
//..................................................................................................

        //generate receiver list
        $object = $ifc_param;
        $array  = NULL;

        $data->move("first");
        while ($key = $data->move("next"))
        {
            if (! permission_is_user($key)) continue;
            $_key = substr($key, 5);

            if (
                 nstreq($_key, DESKTOP_USER)
                 &&
                 cms_permission(CMS_APPLICATION, NULL, NULL, $_key)
               )
                $array[$data->get($key, "name")] = $_key;
        };

        if (! $array)
        {
            $ifc_response = CMS_MSG_ERROR . CMS_L_MOD_DESKTOP_010;
            break;
        };

        ksort($array, SORT_NATURAL | SORT_FLAG_CASE);

        //display
        $ifc = new ifc(NULL,
                       NULL,
                       TRUE,
                       ["user"   => DESKTOP_USER,
                        "object" => $object,
                        "parent" => $parent],
                       "_send",
                       CMS_L_MOD_DESKTOP_011);

        $ifc->set(CMS_L_MOD_DESKTOP_012, "label");
        $ifc->set($array,                "multiselect 30x15");

        $ifc->close();

    case "_send":
//..................................................................................................

        if (blank($ifc_param1)) break;

        $desktop->data->copy($object);
        $flag = TRUE;

        foreach ($ifc_param1 AS $key => $value)
        {
            $_desktop = new desktop($value);

            $_desktop->data->set_buffer(array_merge(
                [["name"  => date(CMS_L_SHORT_DATETIME_FORMAT) . " " . CMS_NAME,
                  "#type" => "container"]],
                $desktop->data->buffer,
                [["#type" => "/container"]]));
            $key = $_desktop->data->seek(["#subtype" => "mailbox"]);
            $_desktop->data->insert($key);

            $flag &= $_desktop->save();
        };

        $ifc_response = $flag ? CMS_MSG_DONE : CMS_MSG_ERROR;
        break;

    case "__config":
//..................................................................................................

        $ifc_response = CMS_MSG_DONE;
        break;

    case "_config":
//..................................................................................................

        $superuser    = "user." . CMS_SUPERUSER;
        $ifc_response = NULL;

        //password
        while (nstre($ifc_param3))
        {
            //check current password
            $password = $data->get("user." . CMS_SUPERUSER, "password");
            $flag     = isset($ifc_param2);
            if (stre($password)) $password = $flag ? md5("") : hash64("");
            if (nstreq($flag ? md5($ifc_param1) : hash64($ifc_param1), $password))
            {
                $ifc_response = CMS_MSG_ERROR . CMS_L_MOD_DESKTOP_031;
                break;
            };

            //check new password
            if (nstreq($ifc_param3, $ifc_param4))
            {
                $ifc_response = CMS_MSG_ERROR . CMS_L_MOD_DESKTOP_032;
                break;
            };

            //encode and save
            $ifc_param3 = hash64($ifc_param3);
            $data->set($ifc_param3, $superuser, "password");

            //set credential
            $ifc_param3 = hash64(cms_cache("cms.salt_password") . $ifc_param3);
            cms_set_cookie(["cms_password" => $ifc_param3]);
            break;
        };

        //timezone
        $data->set($ifc_param5, $superuser, "timezone");

        //background image
        if (isset($ifc_param6) || ($flag = (isset($ifc_file1_name) && is_file($ifc_file1))))
        {
            $array = ["jpg", "png", "webp"];

            //delete existing
            foreach ($array AS $extension)
            {
                $file = DESKTOP_PATH . "background." . $extension;
                if (is_file($file)) unlink($file);
            };

            if ($flag)
            {
                $extension = file_extension($ifc_file1_name);
                if (streq($extension, "jpeg")) $extension = "jpg";

                if (in_array($extension, $array))
                    move_uploaded_file($ifc_file1, DESKTOP_PATH . "background." . $extension);
            };
        };

        if ($ifc_response === NULL)
        {
            if ($data->save())
            {
                header("Location: " . cms_url(["ifc_message" => "__config",
                                               "user"        => DESKTOP_USER]),
                       TRUE, 303);
                exit();
            };

            $ifc_response = CMS_MSG_ERROR;
        };

    case "config":
//..................................................................................................

        //display
        $ifc = new ifc($ifc_response,
                       NULL,
                       TRUE,
                       ["user"   => DESKTOP_USER,
                        "object" => $object,
                        "parent" => $parent],
                       "_config",
                       CMS_L_MOD_DESKTOP_026);

        $ifc->set(CMS_L_MOD_DESKTOP_027, "title");
        $ifc->set(CMS_L_MOD_DESKTOP_014, "password 30 40");
        $ifc->set(CMS_L_MOD_DESKTOP_013, "checkbox b", TRUE, isset($ifc_param2));
        $ifc->set(CMS_L_MOD_DESKTOP_015, "password 40 40 b");
        $ifc->set(CMS_L_MOD_DESKTOP_016, "password 40 40 b");

        //timezone
        $array = timezone_identifiers_list();
        $array = array_combine($array, $array);
        $ifc->set(CMS_L_MOD_DESKTOP_028, "title");
        $ifc->set(CMS_L_MOD_DESKTOP_025, "label");
        $ifc->set($array,                "select 40 b", $ifc_param5 ?? CMS_TIMEZONE);

        //background image
        $ifc->set(CMS_L_MOD_DESKTOP_017 . " (JPG, PNG, WEBP)", "file 30");
        $ifc->set(CMS_L_COMMAND_DELETE ,                       "checkbox", TRUE);

        $ifc->close();
    };

    //cache selected object and parent permanently
    if (CMS_IFC_MESSAGE !== "")
    {
        cms_cache("desktop." . DESKTOP_USER . ".object", $object, TRUE);
        cms_cache("desktop." . DESKTOP_USER . ".parent", $parent, TRUE);
    };

    //icon
    $desktop_icon =
        ["link"        => "desktop/icon_webpage",
         "note"        => "desktop/icon_note",
         "appointment" => "desktop/icon_appointment",
         "address"     => "desktop/icon_address",
         "container"   => "desktop/icon_container",
         "+container"  => "desktop/icon_containeropen",
         "mailbox"     => "desktop/icon_mailbox"];

    //type
    $desktop_type =
        ["link"        => CMS_DESKTOP_TYPE_LINK,
         "note"        => CMS_DESKTOP_TYPE_NOTE,
         "appointment" => CMS_DESKTOP_TYPE_APPOINTMENT,
         "address"     => CMS_DESKTOP_TYPE_ADDRESS,
         "container"   => CMS_DESKTOP_TYPE_CONTAINER,
         "mailbox"     => CMS_DESKTOP_TYPE_MAILBOX];

    //accept
    $desktop_accept =
        ["link"        => CMS_DESKTOP_TYPE_NONE,
         "note"        => CMS_DESKTOP_TYPE_NONE,
         "appointment" => CMS_DESKTOP_TYPE_NONE,
         "address"     => CMS_DESKTOP_TYPE_NONE,
         "container"   => CMS_DESKTOP_TYPE_ALL,
         "mailbox"     => CMS_DESKTOP_TYPE_NONE];

    //popup preopening name
    $_popup = unique_id();

    //menu
    $menu = NULL;
    if ($object) $menu[CMS_L_MOD_DESKTOP_009 . "|desktop/command_rename"] = "javascript:desktop_rename();";
    $menu[CMS_L_COMMAND_REFRESH . "|desktop/command_refresh"]             = TRUE;
    $menu[CMS_L_MOD_DESKTOP_026 . "|desktop/command_configuration"]       = "config";

    //display
    $ifc = new ifc($ifc_response,
                   NULL,
                   $menu,
                   ["user"  => DESKTOP_USER,
                    "popup" => $_popup],
                   NULL,
                   NULL,
                   "desktop-content");

    //background-image
    $array = ["jpg", "png", "webp"];
    $flag  = FALSE;

    foreach ($array AS $extension)
    {
        $file = DESKTOP_PATH . "background." . $extension;
        if (is_file($file)) { $flag = TRUE; break; };
    };

    if ($flag)
    {
        $url = cms_url(["desktop_display" => "background",
                        "user"            => DESKTOP_USER,
                        "extension"       => $extension,
                        "time"            => filemtime($file)]); //cache buster
        echo("<div id=\"desktop-background\" " .
                  "style=\"--image:url('" . x($url) . "');\">" .
             "</div>");
    };

    //rename object
    if ($object)
    {

?><script>

function desktop_rename()
{
    var value = prompt("<?php echo(q(CMS_L_MOD_DESKTOP_009));?>",
                       "<?php echo(q($desktop->object_get($object, "name")));?>");
    if (value) ifc_post("rename", value);
};

</script><?php

    };

    //desktop control
    echo("<div id=\"desktop-control\">" .

         "<a id=\"desktop-logo\" " .
            "href=\"" . x(CMS_HOMEPAGE) . "\" " .
            "title=\"" . x(CMS) . "\" " .
            "onclick=\"window.open(this.href);return false;\">" .
         image(CMS_IFC_EDITION . "/logo") .
         image(CMS_IFC_EDITION . "/icon") .
         "</a>" .

    //desktop control head
         "<div id=\"desktop-control-head\">");

    //interface
    if (cms_permission("interface", NULL, NULL))
        echo("<a href=\"" . x(cms_url(CMS_MODULES_URL . "interface.php")) . "\">" .
             image(CMS_IFC_EDITION . "/icon_interface") . " " . CMS_L_MOD_DESKTOP_029 .
             "</a>");

    echo("<a href=\"" . x(cms_url(CMS_ROOT_URL)) . "\">" .
         image(CMS_IFC_EDITION . "/icon_website") . " " . CMS_L_MOD_DESKTOP_030 .
         "</a>" .
         "</div>" .

    //desktop control body
         "<div id=\"desktop-control-body\">" .

    //user selection
         "<div id=\"desktop-user\">");

    $array = NULL;

    $data->move("first");
    while ($key = $data->move("next"))
    {
        if (! permission_is_user($key)) continue;
        $_key = substr($key, 5);

        if (
             nstreq($_key, CMS_SUPERUSER)
             &&
             cms_permission(CMS_APPLICATION, NULL, NULL, $_key)
             &&
             cms_permission($_key)
           )
            $array[$data->get($key, "name")] = $_key;
    };

    if ($array)
    {

?><script>

function desktop_user_select(value)
{
    ifc_set("user", value);
    ifc_post();
};

</script><?php

        echo("<select onchange=\"desktop_user_select(this.value);\">" .
             "<option value=\"" . x(CMS_SUPERUSER) . "\"" .
                      (streq(DESKTOP_USER, CMS_SUPERUSER)  ? " selected>" : ">") .
             x(CMS_NAME) .
             "</option>");

        ksort($array, SORT_NATURAL | SORT_FLAG_CASE);
        foreach ($array AS $key => $value)
            echo("<option value=\"" . x($value) . "\"" .
                          (streq(DESKTOP_USER, $value) ? " selected>" : ">") .
                 x($key) .
                 "</option>");

        echo("</select>");
    }
    else
    {
        echo(x(CMS_NAME));
    };

    echo("</div>" .

    //user favourites
         "<div id=\"desktop-link-title\">" .
         ($object ? "<a href=\"javascript:ifc_post('quickaccess');\">" : "") .
         CMS_L_MOD_DESKTOP_018 .
         ($object ? " +/-</a>"                                         : "") .
         "</div>" .

         "<div id=\"desktop-link\">");

    $buffer      = NULL;
    $_buffer     = NULL;
    $offset_x    = 10;
    $offset_y    = 42;
    $mod_x       = $offset_x % 105;
    $mod_y       = $offset_y %  60;
    $depth       = $parent ? 0                                  : 1;
    $raster      = $parent ? [$offset_x => [$offset_y => TRUE]] : NULL;
    $path        = NULL;
    $_path       = [];
    $appointment = [];
    $time        = time();
    $time_start  = $time - 3600;
    $time_end    = $time + 3600;

    $desktop->data->move("first");
    while ($key = $desktop->data->move("next"))
    {
        $name = x($desktop->object_get($key, "name"));
        if (streq($key, $object))
            $name = sprintf("<strong>%s</strong>", $name);

        $type = $desktop->object_type($key);

        if ($desktop->object_get($key, "quickaccess"))
            echo("<a href=\"javascript:desktop_activate(" .
                           "'" . qx($key) . "'," .
                           qx($desktop_type[$type]) . ");\">" .
                 image($desktop_icon[$type]) . " $name" .
                 "</a>");

        switch ($type)
        {
        default:
//..................................................................................................

            if ($depth == 1)
            {
                $x  = (int)$desktop->object_get($key, "x") - 250;
                if ($x < $offset_x) $x = $offset_x;
                elseif ($x > 1680)  $x = 1680;
                $x -= $x % 105 - $mod_x;

                $y  = (int)$desktop->object_get($key, "y") - 92;
                if ($y < $offset_y) $y = $offset_y;
                elseif ($y > 900)  $y = 900;
                $y -= $y % 60  - $mod_y;

                if (isset($raster[$x][$y]))
                {
                    for (; $x < 1680; $x += 105)
                    {
                        for (; $y < 960; $y += 60)
                        {
                            if (! isset($raster[$x][$y])) break 2;
                        };
                    };
                };

                $raster[$x][$y]  = TRUE;

                $color           = $desktop->object_get($key, "color");
                $buffer         .= "<div id=\"" . x("dd-$key") . "\" " .
                                        "class=\"icon\" " .
                                        "style=\"" . (stre($color) ? "" :
                                                "--color:" . $color . ";") .
                                                "--left:" .  $x . "px;" .
                                                "--top:" .   $y . "px\">" .
                                   image($desktop_icon[$type]) . "<br>$name" .
                                   "</div>";
                $_buffer        .= "dd_register(\"dd-" . q($key) . "\", " .
                                                $desktop_type[$type] . ", " .
                                                $desktop_accept[$type] . ");";
            };

            if (streq($type, "container"))
            {
                if (nstreq($path, $parent))
                {
                    $path = $key;
                    array_push($_path, $path);
                };

                if (
                     ($depth > 0)
                     ||
                     streq($key, $parent)
                   )
                    $depth++;
            };

            break;

        case "/container":
//..................................................................................................

            if (nstreq($path, $parent)) array_pop($_path);
            if ($depth)                 $depth--;
            break;
        };

        if (
             streq($type, "appointment")
             &&
             (($_time = $desktop->object_get($key, "time")) >= $time_start)
           )
            $appointment[$_time] = $key;
    };
    echo("</div>" .

    //create
         "<div id=\"desktop-create-title\">" .
         CMS_L_MOD_DESKTOP_001 .
         "</div>" .

         "<div id=\"desktop-create\">");

?><script>

function desktop_create(type)
{
    var value;

    switch (type)
    {
    case "link":

        value = "<?php echo(q(CMS_L_MOD_DESKTOP_003));?>";
        break;

    case "note":

        value = "<?php echo(q(CMS_L_MOD_DESKTOP_004));?>";
        break;

    case "appointment":

        value = "<?php echo(q(CMS_L_MOD_DESKTOP_005));?>";
        break;

    case "address":

        value = "<?php echo(q(CMS_L_MOD_DESKTOP_006));?>";
        break;

    case "container":

        value = "<?php echo(q(CMS_L_MOD_DESKTOP_007));?>";
        break;

    case "mailbox":

        value = "<?php echo(q(CMS_L_MOD_DESKTOP_008));?>";
        break;
    };

    value = prompt("<?php echo(q(CMS_L_MOD_DESKTOP_023));?>".replace(/%s/, value), "");

    if (value)
    {
        if (type !== "container")
            load_page("<?php echo(q($_popup));?>",
                      "<?php echo(q(CMS_URL . "blank.htm"));?>");
        ifc_post("create", type + ";" + value);
    };
};

</script><?php

    //link
    echo("<a href=\"javascript:desktop_create('link');\">" .
         image($desktop_icon["link"]) . " " . CMS_L_MOD_DESKTOP_003 .
         "</a>" .

    //note
         "<a href=\"javascript:desktop_create('note');\">" .
         image($desktop_icon["note"]) . " " . CMS_L_MOD_DESKTOP_004 .
         "</a>" .

    //appointment
         "<a href=\"javascript:desktop_create('appointment');\">" .
         image($desktop_icon["appointment"]) . " " . CMS_L_MOD_DESKTOP_005 .
         "</a>" .

    //address
         "<a href=\"javascript:desktop_create('address');\">" .
         image($desktop_icon["address"]) . " " . CMS_L_MOD_DESKTOP_006 .
         "</a>" .

    //container
         "<a href=\"javascript:desktop_create('container');\">" .
         image($desktop_icon["container"]) . " " . CMS_L_MOD_DESKTOP_007 .
         "</a>" .

    //mailbox
         "<a href=\"javascript:desktop_create('mailbox');\">" .
         image($desktop_icon["mailbox"]) . " " . CMS_L_MOD_DESKTOP_008 .
         "</a>" .

         "</div>");

    //show current appointments
    if (! empty($appointment))
    {
        echo("<div id=\"desktop-appointment-title\">" .
             CMS_L_MOD_DESKTOP_002 .
             "</div>" .

             "<div id=\"desktop-appointment\">");

        $i = 0;
        ksort($appointment, SORT_NUMERIC);

        foreach ($appointment AS $key => $value)
        {
            if (($key < $time_start) || (++$i > 3)) break;

            $name = x($desktop->object_get($value, "name"));

            //lapsed
            if ($key < $time)
                $name = "<span style=\"FONT-STYLE:italic\">" .
                        $name .
                        "</span>";

            //imminent
            elseif ($key <= $time_end)
                $name = "<strong>" .
                        $name .
                        "</strong>";

            echo("<a href=\"javascript:desktop_activate(" .
                           "'" . qx($value) . "'," .
                           qx(CMS_DESKTOP_TYPE_APPOINTMENT) . ");\" " .
                    "title=\"" . x(friendly_date($key)) . "\">" .
                 image("desktop/icon_appointment") . " $name" .
                 "</a>");
        };

        echo("</div>");
    };

    //receiver
    echo("<div id=\"dd-receiver\" " .
              "title=\"" . CMS_L_MOD_DESKTOP_011 . "\">" .
         image("desktop/icon_receiver") .
         "</div>" .

    //trashbin
         "<div id=\"dd-trashbin\" " .
              "title=\"" . CMS_L_COMMAND_DELETE . "\">" .
         image("desktop/icon_trashbin") .
         "</div>" .

         "</div>" .

    //desktop control foot
         "<div id=\"desktop-control-foot\">" .

    //logout
         "<a href=\"" . x(CMS_MODULES_URL) . "identification.php?cms_logout=1\">" .
         image(CMS_IFC_EDITION . "/icon_logout") . " " . CMS_L_IFC_007 .
         "</a>" .
         "</div>" .

    //date / time
         "<div id=\"desktop-date\">" .
         date(CMS_L_DATETIME_FORMAT) .
         "</div>" .

         "</div>" .

    //shortlinks
         "<div id=\"desktop-link-quick\">" .

    //instant messenger
         "<a href=\"javascript:load_page('','" .
             qr(cms_url(["desktop_display"   => "interface",
                         "desktop_interface" => "ims",
                         "user"              => DESKTOP_USER])) . "');\" " .
            "title=\"" . CMS_L_MOD_DESKTOP_020 . "\">" .
         (is_file(DESKTOP_PATH . "ims.flag") ?
         "<span class=\"blink\">" .
         image("desktop/icon_ims") .
         "</span>" .
         "</a> " .
         "<audio autoplay>" .
         "<source src=\"" . CMS_SOUNDS_URL . "desktop/ims_alert.mp3\" " .
                 "type=\"audio/mpeg\">" .
         "</audio>" :
         image("desktop/icon_ims") .
         "</a>") .

    //calendar
         "<a href=\"javascript:load_page('','" .
             qr(cms_url(["desktop_display"   => "interface",
                         "desktop_interface" => "appointment",
                         "user"              => DESKTOP_USER])) . "');\" " .
            "title=\"" . CMS_L_MOD_DESKTOP_022 . "\">" .
         image("desktop/icon_appointment") .
         "</a>" .

    //address book
         "<a href=\"javascript:load_page('','" .
             qr(cms_url(["desktop_display"   => "interface",
                         "desktop_interface" => "address",
                         "user"              => DESKTOP_USER])) . "');\" " .
            "title=\"" . CMS_L_MOD_DESKTOP_021 . "\">" .
         image("desktop/icon_address") .
         "</a>" .
         "</div>");

    //path
    if (count($_path))
    {
        echo("<div id=\"desktop-path\">" .
             "<a href=\"javascript:ifc_post('activate');\">" .
             CMS_L_MOD_DESKTOP_024 .
             "</a>");

        foreach ($_path AS $key => $value)
        {
            echo(" / <a href=\"javascript:desktop_activate('" . qx($value) . "');\">" .
                 x($desktop->object_get($value, "name")) .
                 "</a>");
        };

        echo("</div>");
    };

    if ($parent)
    {
        $_parent = $desktop->data->move("parent", $parent);

        echo("<div id=\"dd-" . x($_parent) . "\" " .
                  "class=\"icon\" " .
                  "style=\"--left:" . $offset_x . "px;" .
                          "--top:" . $offset_y . "px\">" .
             image("desktop/icon_containeropen") . "<br>" .
             ($_parent ? x($desktop->object_get($_parent, "name")) : CMS_L_MOD_DESKTOP_024) .
             "</div>");
    };

    echo($buffer);

?><script>

function desktop_event(event,
                       source,
                       target)
{
    source_id = (source == null) ? "" : source.id.substring(3);
    target_id = (target == null) ? "" : target.id.substring(3);

    switch (event)
    {
    case "activate":

        if (source_id == "trashbin") break;
        if (source_id == "receiver") break;

        desktop_activate(source_id, source.dd_type);
        break;

    case "select":

        if (source_id == "")         break;
        if (source_id == "trashbin") break;
        if (source_id == "receiver") break;
        ifc_post("select", source_id);
        break;

    case "dropon":

        if (target_id == "trashbin")      ifc_post("delete", source_id);
        else if (target_id == "receiver") ifc_post("send", source_id);
        else                              ifc_post("move", source_id + "," + target_id);
        break;

    case "drop":

        ifc_post("drop", source_id + "," + fx_mouse_x + "," + fx_mouse_y);
        break;
    };
};

function desktop_activate(id,
                          type = 0)
{
    //preopen popup
    switch (type)
    {
    case "<?php echo(q(CMS_DESKTOP_TYPE_LINK));?>":
    case "<?php echo(q(CMS_DESKTOP_TYPE_NOTE));?>":
    case "<?php echo(q(CMS_DESKTOP_TYPE_APPOINTMENT));?>":
    case "<?php echo(q(CMS_DESKTOP_TYPE_ADDRESS));?>":
    case "<?php echo(q(CMS_DESKTOP_TYPE_MAILBOX));?>":
        load_page("<?php echo(q($_popup));?>",
                  "<?php echo(q(CMS_URL . "blank.htm"));?>");
    };

    ifc_post("activate", id);
};

dd_set_callback(desktop_event);
dd_register("dd-receiver", 0, 255, true);
dd_register("dd-trashbin", 0, 255, true);

<?php

    if ($parent)
        echo("dd_register(\"dd-" . q($_parent) . "\", " .
                          $desktop_type["container"] . ", " .
                          $desktop_accept["container"] . ", " .
                          "true);");
    echo($_buffer);

?>

</script><?php

    //open popup
    if ($load)
        echo(jscript("load_page('" . q($popup) . "','" .
                     q(cms_url(["desktop_display" => "interface",
                                "user"            => DESKTOP_USER,
                                "object"          => $object])) . "');"));

    $ifc->close();

case "interface":
//..................................................................................................

    $desktop = new desktop(DESKTOP_USER);

    if (empty($object))
    {
        $object = 0;
        if (blank($desktop_interface)) ifc_close_external();
    }
    else
    {
        $desktop_interface = $desktop->object_type($object);
    };

    $file = CMS_DESKTOP_PATH . "desktop.$desktop_interface.inc";
    if (! is_file($file)) ifc_close_external();

    $param = ["desktop_interface" => $desktop_interface,
              "user"              => DESKTOP_USER,
              "object"            => $object];

    require($file);
    exit();

case "background":
//..................................................................................................

    while (preg_match("/^(?:jpg|png|webp)$/", $extension ?? ""))
    {
        //check file
        $file = DESKTOP_PATH . "background." . $extension;
        if (! is_file($file)) break;

        //send headers
        header("Cache-Control: private, max-age=86400");
        header("Content-Type: " . get_mime_type($file));

        //send file
        readfile($file);
        exit();
    };

    http_response_code(404); //not found
    exit();
};