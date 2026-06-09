<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   CORE_CONTROL FUNCTIONS
//==================================================================================================

function core_control_profile(&$core,
                              $guid,
                              $channel = NULL,
                              $url     = NULL)
{
    $status = $core->get_status($guid, $channel);

    //skip invisible
    if ((! $core->operator) && nstreq($guid, $core->guid) && flag($status, CMS_CORE_STATUS_INVISIBLE)) return;

    $profile = $core->get_profile($guid);

    //class / icon / status
    $class   = "";
    $icon    = "core/icon_user";
    $_status = "";

    if (
         (substr($guid, 0, 1) === "!")
         &&
         cms_permission(CMS_CORE_PERMISSION_OPERATOR, TRUE, TRUE, substr($guid, 1))
       )
    {
        $_status  = CMS_L_CORE_CONTROL_047;
        $icon    .= "_ow";
        $class    = "core-control-profile-owner";
    }
    elseif (flag($status, CMS_CORE_STATUS_OWNER))
    {
        $_status  = CMS_L_CORE_CONTROL_004;
        $icon    .= "_ow";
        $class    = "core-control-profile-owner";
    }
    elseif (flag($status, CMS_CORE_STATUS_OPERATOR))
    {
        $_status  = CMS_L_CORE_CONTROL_005;
        $icon    .= "_op";
        $class    = "core-control-profile-operator";
    }
    elseif (substr($guid, 0, 1) === "!")
    {
        $_status  = CMS_L_CORE_CONTROL_006;
        $class    = "core-control-profile-member";
    }
    else
    {
        $_status  = CMS_L_CORE_CONTROL_007;
        $class    = "core-control-profile-guest";
    };

    //special status
    if ($core->operator)
    {
        if (flag($status, CMS_CORE_STATUS_SPY))
        {
            $_status .= " (" . CMS_L_CORE_CONTROL_028 . ")";
            $class   .= " core-control-profile-spy";
        };

        if (flag($status, CMS_CORE_STATUS_INVISIBLE))
        {
            $_status .= " (" . CMS_L_CORE_CONTROL_029 . ")";
            $icon    .= "_i";
            $class   .= " core-control-profile-invisible";
        };
    };

    //additional status
    if (flag($status, CMS_CORE_STATUS_ABSENT))
    {
        $_status .= " (" . CMS_L_CORE_CONTROL_030 . ")";
        $icon    .= "_a";
        $class   .= " core-control-profile-absent";
    };

    if (flag($status, CMS_CORE_STATUS_MUTE))
    {
        $_status .= " (" . CMS_L_CORE_CONTROL_031 . ")";
        $icon    .= "_m";
        $class   .= " core-control-profile-mute";
    };

    if (flag($status, CMS_CORE_STATUS_BANNED))
    {
        $_status .= " (" . CMS_L_CORE_CONTROL_032 . ")";
        $class   .= " core-control-profile-banned";
    };

    //color
    $color = $profile["color"];
    if (nstre($color = $profile["color"]))
    {
        //resonably working text color determination
        $_color = (hexdec(substr($color, 1, 2)) * 3 +
                   hexdec(substr($color, 3, 2)) * 6 +
                   hexdec(substr($color, 5, 2))) < 1500 ?
                  "#ffffff" : "#000000";
        $color  = " style=\"BACKGROUND-COLOR:$color;COLOR:$_color\"";
    };

    //url
    echo(($flag = stre($url)) ?
         "<div class=\"core-control-profile $class\"$color>" :
         "<a href=\"" . x($url) . "\" " .
            "class=\"core-control-profile $class\"$color>");

    //image
    if (nstre($profile["image"]))
    {
        $image = image_process(CMS_DATA_URL . "core/" . r($profile["image"]), 35);
        echo("<img src=\"" . x($image) . "\" " .
                  "alt=\"\" " .
                  "class=\"core-control-profile-image\">");
    };

    //name
    echo("<div class=\"core-control-profile-name\">" .
         image($icon) . " " . x($profile["name"]) .
         "</div>"); //core-control-profile-name

    //description
    if (nstre($profile["text"]))
        echo("<div>" .
             x($profile["text"]) .
             "</div>");

    //primary status
    echo("<div class=\"core-control-profile-status\">" .
         $_status .
         "</div>" .                    //core-control-profile-status
         ($flag ? "</div>" : "</a>")); //core-control-profile
};

//--------------------------------------------------------------------------------------------------

function core_control(&$core,
                      $core_control_object  = NULL,
                      $core_control_command = NULL,
                      $core_control_value   = NULL)
{

    $max_image_size = 5120000;

    //object type and id
    if (($i = strpos($core_control_object, ":")) !== FALSE)
    {
        $type   = substr($core_control_object, 0, $i);
        $object = substr($core_control_object, $i + 1);
    }
    else
    {
        $type   = $core_control_object;
        $object = "";
    };

    //enable / disable external refresh
    echo(jscript("parent.postMessage(\"chat_control_refresh_" .
                 (in_array($core_control_object,
                           ["", "channel", "channel_index"]) ?
                  "enable" : "disable") . "\", " .
                 "\"*\");"));

    //command
    if (nstre($core_control_command))
    {
        switch ($type)
        {
        case "channel":
//..................................................................................................

            switch ($core_control_command)
            {
            case "connect":

                $core->connect_channel($object,
                                       isset($core_control_value["password"]) ?
                                       $core_control_value["password"] : "");
            };

            break;

        case "channel_create":
//..................................................................................................

            switch ($core_control_command)
            {
            case "create":

                $type   = "channel";
                $object = $core_control_value["channel"];

                if (isset($core_control_value["permanent"]))
                    $core->create_channel($object,
                                          $core_control_value["text"],
                                          $core_control_value["password"]);
                $core->connect_channel($object);
            };

            break;

        case "channel_edit":
//..................................................................................................

            switch ($core_control_command)
            {
            case "edit":

                $type = "channel";
                $core->set_channel($object,
                                   $core_control_value["text"],
                                   $core_control_value["password"]);
            };

            break;

        case "channel_delete":
//..................................................................................................

            switch ($core_control_command)
            {
            case "delete":

                $type = "channel";
                $core->delete_channel($object);
            };

            break;

        case "user":
//..................................................................................................

            switch ($core_control_command)
            {
            case "message":

                $core->send($core_control_value["data"],
                            $object);
                echo(jscript("parent.postMessage(\"chat_control_receive\", \"*\");"));
                break;
            };

            break;

        case "profile":
//..................................................................................................

            switch ($core_control_command)
            {
            case "edit":

                if (nstreq($object, $core->guid))
                {
                    if (! $core->operator) break;

                    if (
                         (substr($object, 0, 1) === "!")
                         &&
                         cms_permission(CMS_CORE_PERMISSION_OPERATOR, TRUE, TRUE, substr($object, 1))
                       )
                        break;
                };

                global $core_control_value_image,
                       $core_control_value_image_name,
                       $core_control_value_image_size,
                       $core_control_value_image_type;

                if (
                     nstre($core_control_value_image)
                     &&
                     is_file($core_control_value_image)
                     &&
                     preg_match("/\.(gif|jpe?g|png|webp)$/i", $core_control_value_image_name)
                     &&
                     ($core_control_value_image_size <= $max_image_size)
                     &&
                     preg_match("/^image\//i", $core_control_value_image_type)
                   )
                {
                    $path      = CMS_DATA_PATH . "core/";
                    $extension = file_extension($core_control_value_image_name);
                    if (streq($extension, "jpeg")) $extension = "jpg";
                    $filename  = unique_id() . ".$extension";

                    mkpath($path);
                    move_uploaded_file($core_control_value_image,
                                       $path . $filename);
                }
                else
                {
                    $filename = "";
                };

                $core->set_profile($object,
                                   $core_control_value["name"],
                                   $core_control_value["color"],
                                   $core_control_value["text"],
                                   $filename);

                $log = new log();
                $log->user($core_control_value["name"]);
            };

            break;

        case "status":
        case "status_channel":
        case "status_user":
        case "status_all":
//..................................................................................................

            switch ($core_control_command)
            {
            case "apply":

                foreach ($core_control_value AS $key => $value)
                {
                    foreach ($value AS $_key => $_value)
                    {
                        foreach ($_value AS $__key => $__value)
                            $core->status($__value ? $__key : -$__key, $_key, trim($key));
                    };
                };
            };

            break;
        };
    };

    //active channel
    $channel = $core->index->seek(["guid" => $core->guid]) ?
               $core->index->get("channel") : "";

    //set default
    if (stre($type))
    {
        $type   = "channel";
        $object = $channel;
    };

    echo("<form id=\"core-control-form\" " .
               "method=\"post\" " .
               "action=\"" . x(cms_url(["core_control_object" => $core_control_object])) . "\" " .
               "enctype=\"multipart/form-data\" " .
               "onsubmit=\"parent.postMessage('chat_control_submit','*');\">" .

    //menu
         "<nav class=\"p\">" .

    //users in active channel
         "<a href=\"" . x(cms_url()) . "\">" .
         CMS_L_CORE_CONTROL_002 .
         "</a> " .
         "| " .

    //channels
         "<a href=\"" . x(cms_url(["core_control_object" => "channel_index"])) . "\">" .
         CMS_L_CORE_CONTROL_001 .
         "</a> " .
         "| " .

    //profile
         "<a href=\"" . x(cms_url(["core_control_object" => "profile:" . $core->guid])) . "\">" .
         CMS_L_CORE_CONTROL_041 .
         "</a> " .
         "| " .

    //status
         "<a href=\"" . x(cms_url(["core_control_object" => "status"])) . "\">" .
         CMS_L_CORE_CONTROL_025 .
         "</a>" .
         "</nav>");

    switch ($type)
    {
    case "channel":
//..................................................................................................

        $flag = FALSE;

        //channel
        if (
             nstre($object)
             &&
             (
               ($flag = $core->channel->seek(["channel" => $channel]))
               ||
               $core->index->seek(["channel" => $channel])
             )
           )
        {
            //title
            echo("<h2>" .
                 image("core/icon_channel") . " " . x($object) .
                 "</h2>");

            //description
            if ($flag)
            {
                $description = $core->channel->get("text");
                if (nstre($description))
                    echo("<div class=\"p\">" .
                         x($description) .
                         "</div>");
            };
        }

        //portal
        else
        {
            //title
            echo("<h2>" .
                 image("core/icon_channel") . " " . CMS_CORE_PORTAL .
                 "</h2>");
        };

        //menu
        $status = $core->get_status(NULL, $object);
        $flag   = TRUE;

        //owner
        if (
             nstre($object)
             &&
             flag($status, CMS_CORE_STATUS_OWNER)
             &&
             $core->channel->seek(["channel" => $object])
           )
        {
            echo("<div class=\"p\">" .

            //edit command
                 "<a href=\"" . x(cms_url(["core_control_object" => "channel_edit:$object"])) . "\" " .
                    "class=\"button\">" .
                 CMS_L_CORE_CONTROL_015 .
                 "</a>" .

            //delete command
                 "<a href=\"" . x(cms_url(["core_control_object" => "channel_delete:$object"])) . "\" " .
                    "class=\"button\">" .
                 CMS_L_CORE_CONTROL_016 .
                 "</a>");

            $flag = FALSE;
        };

        //owner / moderator
        if (flag($status, CMS_CORE_STATUS_OWNER | CMS_CORE_STATUS_OPERATOR))
        {
            if ($flag)
            {
                echo("<div class=\"p\">");
                $flag = FALSE;
            };

            //moderate command
            echo("<a href=\"" . x(cms_url(["core_control_object" => "status_channel:$object"])) . "\" " .
                    "class=\"button\">" .
                 CMS_L_CORE_CONTROL_051 .
                 "</a>");
        }

        if (! $flag)
            echo("</div>"); //p

        //show connect dialog
        if (nstreq($object, $channel))
        {
            $status_override = flag($status, CMS_CORE_STATUS_OWNER) || flag($status, CMS_CORE_STATUS_OPERATOR);

            //banned
            if ((! $status_override) && flag($status, CMS_CORE_STATUS_BANNED))
            {
                echo("<div class=\"response-error\">" .
                     sprintf(CMS_L_CORE_CONTROL_014, x($object)) .
                     "</div>");
                break;
            };

            //data
            echo("<input name=\"core_control_command\" " .
                        "type=\"hidden\" " .
                        "value=\"connect\">");

            //password
            if (
                 (! $status_override)
                 &&
                 $core->channel->seek(["channel"  => $object,
                                       "password" => TRUE])
               )
                echo("<fieldset>" .
                     "<legend>" .
                     CMS_L_CORE_CONTROL_013 .
                     "</legend>" .
                     "<input name=\"core_control_value[password]\" " .
                            "type=\"password\" " .
                            "maxlength=\"20\">" .
                     "</fieldset>");

            //connect button
            echo("<fieldset>" .
                 "<input type=\"submit\" " .
                        "value=\"" . CMS_L_CORE_CONTROL_012 . "\">" .
                 "</fieldset>");
            break;
        };

        //show users in active channel
        echo("<h3>" .
             CMS_L_CORE_CONTROL_002 .
             "</h3>" .

             "<div class=\"core-control-list\">");

        $core->index->reset();
        while ($core->index->next(["guid"    => TRUE,
                                   "channel" => $object]))
        {
            $guid = $core->index->get("guid");
            $url  = cms_url(["core_control_object" =>
                             streq($guid, $core->guid) ?
                             "profile:$guid" :
                             "user:$guid"]);
            core_control_profile($core, $guid, $object, $url);
        };

        echo("</div>"); //core-control-list
        break;

    case "channel_create":
//..................................................................................................

        //title
        echo("<h2>" .
             CMS_L_CORE_CONTROL_017 .
             "</h2>" .

        //data
             "<input name=\"core_control_command\" " .
                    "type=\"hidden\" " .
                    "value=\"create\">" .

        //info
             "<div class=\"p\">" .
             CMS_L_CORE_CONTROL_048 .
             "</div>" .

        //name
             "<fieldset>" .
             "<legend>" .
             CMS_L_NAME .
             "</legend>" .
             "<input name=\"core_control_value[channel]\" " .
                    "type=\"text\" " .
                    "maxlength=\"20\">" .
             "</fieldset>");

        //extended
        if ($core->operator)
        {
            //permanent
            echo("<fieldset>" .
                 "<label>" .
                 "<input name=\"core_control_value[permanent]\" " .
                       "type=\"checkbox\" " .
                       "value=\"1\" " .
                       "onchange=\"core_control_channel_switch(this);\">" .
                 CMS_L_CORE_CONTROL_018 .
                 "</label>" .
                 "</fieldset>" .

            //description
                 "<fieldset>" .
                 "<legend>" .
                 CMS_L_DESCRIPTION .
                 "</legend>" .
                 "<textarea name=\"core_control_value[text]\" " .
                           "rows=\"5\" " .
                           "onkeyup=\"limit(this,1024);\">" .
                 "</textarea>" .
                 "</fieldset>" .

            //password
                 "<fieldset>" .
                 "<legend>" .
                 CMS_L_PASSWORD .
                 "</legend>" .
                 "<input name=\"core_control_value[password]\" " .
                        "type=\"password\" " .
                        "maxlength=\"20\">" .
                 "</fieldset>");

?><script>

function core_control_channel_switch(input)
{
    var display = input.checked ? false : "none";
    var object  = input.parentElement.parentElement.nextElementSibling;
    fx_style(object, "display", display);
    fx_style(object.nextElementSibling, "display", display);
};
core_control_channel_switch(document.getElementsByName("core_control_value[permanent]")[0]);

</script><?php

        };

        //create button
        echo("<fieldset>" .
             "<input type=\"submit\" " .
                    "value=\"" . CMS_L_CORE_CONTROL_027 . "\">" .
             "</fieldset>");
        break;

    case "channel_edit":
//..................................................................................................

        if (stre($object)) break;

        //title
        echo("<h2>" .
             CMS_L_CORE_CONTROL_015 .
             "</h2>");

        //not found
        if (! $core->channel->seek(["channel" => $object]))
        {
            echo("<div class=\"response-error\">" .
                 CMS_L_CORE_CONTROL_019 .
                 "</div>");
            break;
        };

        //data
        echo("<input name=\"core_control_command\" " .
                    "type=\"hidden\" " .
                    "value=\"edit\">" .

        //description
             "<fieldset>" .
             "<legend>" .
             CMS_L_DESCRIPTION .
             "</legend>" .
             "<textarea name=\"core_control_value[text]\" " .
                       "rows=\"5\" " .
                       "onkeyup=\"limit(this,1024);\">" .
             x($core->channel->get("text")) .
             "</textarea>" .
             "</fieldset>" .

        //password
             "<fieldset>" .
             "<legend>" .
             CMS_L_PASSWORD .
             "</legend>" .
             "<input name=\"core_control_value[password]\" " .
                    "type=\"password\" " .
                    "value=\"" . x($core->channel->get("password")) . "\" " .
                    "maxlength=\"20\">" .
             "</fieldset> " .

        //save button
             "<fieldset>" .
             "<input type=\"submit\" " .
                    "value=\"" . CMS_L_COMMAND_SAVE . "\">" .
             "</fieldset>");
        break;

    case "channel_delete":
//..................................................................................................

        if (stre($object)) break;

        //title
        echo("<h2>" .
             CMS_L_CORE_CONTROL_016 .
             "</h2>");

        //not found
        if (! $core->channel->seek(["channel" => $object]))
        {
            echo("<div class=\"response-error\">" .
                 CMS_L_CORE_CONTROL_019 .
                 "</div>");
            break;
        };

        //data
        echo("<input name=\"core_control_command\" " .
                    "type=\"hidden\" " .
                    "value=\"delete\">" .

        //info
             "<div class=\"p\">" .
             sprintf(CMS_L_CORE_CONTROL_020, x($object)) .
             "</div>" .

        //delete button
             "<fieldset>" .
             "<input type=\"submit\" " .
                    "value=\"" . CMS_L_COMMAND_CLOSE . "\">" .
             "</fieldset>");
        break;

    case "channel_index":
//..................................................................................................

        //title
        echo("<h2>" .
             CMS_L_CORE_CONTROL_001 .
             "</h2>" .

        //create command
             "<div class=\"p\">" .
             "<a href=\"" . x(cms_url(["core_control_object" => "channel_create"])) . "\" " .
                "class=\"button\">" .
             CMS_L_CORE_CONTROL_017 .
             "</a>" .
             "</div>"); //p

        //collect permanent channels
        $list = [];

        $core->channel->reset();
        while ($core->channel->next(["channel" => TRUE]))
        {
            $_channel = $core->channel->get("channel");
            $list[$_channel] = ["text"     => $core->channel->get("text"),
                                "password" => (bool)$core->channel->get("password"),
                                "user"     => 0];
        };

        //collect temporary channels
        $_list = ["" => 0];

        $core->index->reset();
        while ($core->index->next(["guid" => TRUE]))
        {
            $guid     = $core->index->get("guid");
            $_channel = $core->index->get("channel");

            //invisible
            if (
                 (! $core->operator)
                 &&
                 nstreq($guid, $core->guid)
                 &&
                 flag($core->get_status($guid, $_channel), CMS_CORE_STATUS_INVISIBLE)
               )
                continue;

            if (isset($list[$_channel]))      $list[$_channel]["user"]++;
            elseif (isset($_list[$_channel])) $_list[$_channel]++;
            else                              $_list[$_channel] = 1;
        };

        //display permanent channels
        echo("<div class=\"core-control-list\">" .

        //display portal
             "<a href=\"" . x(cms_url(["core_control_object" => "channel"])) . "\" " .
                "class=\"core-control-channel\">" .

        //name
             "<div class=\"core-control-channel-name\">" .
             image("core/icon_channel") . " " . x(CMS_CORE_PORTAL) .
             "</div>" . //core-control-channel-name

        //status
             "<div class=\"core-control-channel-status\">" .
             sprintf(CMS_L_CORE_CONTROL_010, $_list[""]) .
             "</div>" . //core-control-channel-status
             "</a>");   //core-control-channel

        foreach ($list AS $key => $_)
        {
            echo("<a href=\"" . x(cms_url(["core_control_object" => "channel:$key"])) . "\" " .
                    "class=\"core-control-channel\">" .

            //name
                 "<div class=\"core-control-channel-name\">" .
                 image("core/icon_channel") . " " . x($key) .
                 "</div>" . //core-control-channel-name

            //description
                 "<div>" .
                 x($list[$key]["text"]) .
                 "</div>" .

            //status
                 "<div class=\"core-control-channel-status\">" .
                 sprintf(CMS_L_CORE_CONTROL_010, $list[$key]["user"]) .
                 (stre($list[$key]["password"]) ? ""  : "<br>" . CMS_L_CORE_CONTROL_011) .
                 "</div>" . //core-control-channel-status
                 "</a>");   //core-control-channel
        };

        echo("</div>"); //core-control-list

        unset($_list[""]);

        //display temporary channels
        if (count($_list) > 0)
        {
            echo("<h3>" .
                 CMS_L_CORE_CONTROL_009 .
                 "</h3>");

            foreach ($_list AS $key => $value)
            {
                echo("<a href=\"" . x(cms_url(["core_control_object" => "channel:$key"])) . "\" " .
                        "class=\"core-control-channel\">" .

                //name
                     "<div class=\"core-control-channel-name\">" .
                     image("core/icon_channel") . " " . x($key) .
                     "</div>" . //core-control-channel-name

                //status
                     "<div class=\"core-control-channel-status\">" .
                     sprintf(CMS_L_CORE_CONTROL_010, $value) .
                     "</div>" . //core-control-channel-status
                     "</a>");   //core-control-channel
            };
        };

        break;

    case "user":
//..................................................................................................

        echo("<h2>" .
             CMS_L_CORE_CONTROL_035 .
             "</h2>");

        //not found
        if (($profile = $core->get_profile($object)) === FALSE)
        {
            echo("<div class=\"response-error\">" .
                 CMS_L_CORE_CONTROL_046 .
                 "</div>");
            break;
        };

        //menu
        if ($core->operator)
            echo("<div class=\"p\">" .

            //edit profile command
                 "<a href=\"" . x(cms_url(["core_control_object" => "profile:$object"])) . "\" " .
                    "class=\"button\">" .
                 CMS_L_CORE_CONTROL_008 .
                 "</a>" .

            //moderate command
                 "<a href=\"" . x(cms_url(["core_control_object" => "status_user:$object"])) . "\" " .
                    "class=\"button\">" .
                 CMS_L_CORE_CONTROL_052 .
                 "</a>" .
                 "</div>"); //p

        //recipient
        echo("<fieldset>" .
             "<legend>" .
             CMS_L_CORE_CONTROL_036 .
             "</legend>");

        core_control_profile($core, $object);

        //data
        echo("<input name=\"core_control_command\" " .
                    "type=\"hidden\" " .
                    "value=\"message\">" .
             "</fieldset>" .

        //message
             "<fieldset>" .
             "<legend>" .
             CMS_L_CORE_CONTROL_037 .
             "</legend>" .
             "<textarea name=\"core_control_value[data]\" " .
                       "rows=\"5\" " .
                       "onkeyup=\"limit(this,1024);\">\n" .
             "</textarea>" .
             "</fieldset>" .

        //send button
             "<fieldset>" .
             "<input type=\"submit\" " .
                    "value=\"" . CMS_L_CORE_CONTROL_038 . "\">" .
             "</fieldset>");
        break;

    case "profile":
//..................................................................................................

        //third-party profile
        if (nstreq($object, $core->guid))
        {
            //admin only
            if (! $core->operator) break;

            //exclude admins
            if (
                 (substr($object, 0, 1) === "!")
                 &&
                 cms_permission(CMS_CORE_PERMISSION_OPERATOR, TRUE, TRUE, substr($object, 1))
               )
                break;
        };

        echo("<h2>" .
             CMS_L_CORE_CONTROL_041 .
             "</h2>");

        //not found
        if (($profile = $core->get_profile($object)) === FALSE)
        {
            echo("<div class=\"response-error\">" .
                 CMS_L_CORE_CONTROL_046 .
                 "</div>");
            break;
        };

        //menu
        if ($core->operator)
            echo("<div class=\"p\">" .

            //profile index command
                 "<a href=\"" . x(cms_url(["core_control_object" => "profile_index"])) . "\" " .
                    "class=\"button\">" .
                 CMS_L_CORE_CONTROL_045 .
                 "</a>" .

            //moderate command
                 "<a href=\"" . x(cms_url(["core_control_object" => "status_user:$object"])) . "\" " .
                    "class=\"button\">" .
                 CMS_L_CORE_CONTROL_052 .
                 "</a>" .
                 "</div>"); //p

        //data
        echo("<input name=\"core_control_command\" " .
                    "type=\"hidden\" " .
                    "value=\"edit\">" .

        //name
             "<fieldset>" .
             "<legend>" .
             CMS_L_NAME .
             "</legend>" .
             "<input name=\"core_control_value[name]\" " .
                    "type=\"text\" " .
                    "value=\"" . x($profile["name"]) . "\" " .
                    "maxlength=\"40\">" .
             "</fieldset> " .

        //color
             "<fieldset>" .
             "<legend>" .
             CMS_L_COLOR .
             "</legend>" .
             "<input name=\"core_control_value[color]\" " .
                    "type=\"color\" " .
                    "value=\"" . x($profile["color"]) . "\">" .
             "</fieldset>" .

        //description
             "<fieldset>" .
             "<legend>" .
             CMS_L_CORE_CONTROL_042 .
             "</legend>" .
             "<textarea name=\"core_control_value[text]\" " .
                       "rows=\"5\" " .
                       "onkeyup=\"limit(this,1024);\">\n" .
             x($profile["text"]) .
             "</textarea>" .
             "</fieldset>");

        //image
        $image = $profile["image"];
        $image = stre($image) ? "no_image" : CMS_DATA_URL . "core/$image";

        echo("<fieldset>" .
             "<legend>" .
             CMS_L_CORE_CONTROL_043 .
             "</legend>" .
             "<a href=\"#\" " .
                "onclick=\"return core_control_profile_image_select(this);\">" .
             image($image, 200, NULL, CMS_L_CORE_CONTROL_021) .
             "</a>" .
             "<input name=\"MAX_FILE_SIZE\" " .
                    "type=\"hidden\" " .
                    "value=\"5120000\">" .
             "<input name=\"core_control_value_image\" " .
                    "type=\"file\" " .
                    "accept=\"image/gif,image/jpeg,image/png,image/webp\" " .
                    "style=\"DISPLAY:none\" " .
                    "onchange=\"return core_control_profile_image_add(this);\">" .
             "</fieldset>" .
             "<div class=\"p\">" .
             sprintf(CMS_L_CORE_CONTROL_044, x(format_bytesize($max_image_size))) .
             "</div>");

?><script>

function core_control_profile_image_select(link)
{
    var input = link.nextElementSibling.nextElementSibling;
    input.click();
    return false;
};

//--------------------------------------------------------------------------------------------------

function core_control_profile_image_add(input)
{
    if (! (input.files && input.files[0])) return;

    if (input.files[0].size > <?php echo($max_image_size);?>)
    {
        alert("<?php echo(q(CMS_L_CORE_CONTROL_049));?>");
        var link = input.nextElementSibling;
        return;
    };

    var image1    = new Image();
    var reader    = new FileReader();
    reader.onload = function(event)
    {
        image1.src    = event.target.result;
        image1.onload = function ()
        {
                    var image2 = input.parentElement.children[1].children[0];
                    image2.removeAttribute("srcset");
                    image2.removeAttribute("sizes");
                    image2.src = image1.src;
        };
    };

    reader.readAsDataURL(input.files[0]);
};

</script><?php

        //save button
        echo("<fieldset>" .
             "<input type=\"submit\" " .
                    "value=\"" . CMS_L_COMMAND_SAVE . "\">" .
             "</fieldset>");
        break;

    case "profile_index":
//..................................................................................................

        if (! $core->operator) break; //admin only

        echo("<h2>" .
             CMS_L_CORE_CONTROL_045 .
             "</h2>" .

        //profile list
             "<div class=\"core-control-list\">");

        $core->profile->reset();
        while ($core->profile->next(["guid" => TRUE]))
        {
            $guid = $core->profile->get("guid");
            $url  = (
                      streq($guid, $core->guid) //self
                      ||
                      (substr($guid, 0, 1) !== "!") //member
                      ||
                      (! cms_permission(CMS_CORE_PERMISSION_OPERATOR, TRUE, TRUE, substr($guid, 1))) //not admin
                    ) ?
                    CMS_ACTIVE_URL . cms_param(["core_control_object" => "profile:$guid"]) :
                    NULL;
            core_control_profile($core, $guid, NULL, $url);
        };

        echo("</div>"); //core-control-list
        break;

    case "status":
    case "status_channel":
    case "status_user":
    case "status_all":
//..................................................................................................

        $guid    = NULL;
        $channel = NULL;

        switch ($type)
        {
        case "status":

            $guid    = $core->guid;
            $channel = $core->index->seek(["guid" => $core->guid]) ?
                       $core->index->get("channel") : "";
            $object  = $type;
            $title   = CMS_L_CORE_CONTROL_025;
            break;

        case "status_channel":

            $channel = $object;
            $object  = "$type:$object";
            $title   = CMS_L_CORE_CONTROL_051;
            break;

        case "status_user":

            $guid    = $object;
            $object  = "$type:$object";
            $title   = CMS_L_CORE_CONTROL_052;
            break;

        case "status_all":

            if (! $core->operator) break 2; //admin only
            $guid    = NULL;
            $channel = NULL;
            $object  = $type;
            $title   = CMS_L_CORE_CONTROL_040;
        };

        //data
        echo("<input name=\"core_control_command\" " .
                    "type=\"hidden\" " .
                    "value=\"apply\">" .

        //title
             "<h2>" .
             $title .
             "</h2>");

        //administration command
        if ($core->operator && nstreq($type, "status_all"))
            echo("<div class=\"p\">" .
                 "<a href=\"" . x(CMS_ACTIVE_URL . cms_param(["core_control_object" => "status_all"])) . "\" " .
                    "class=\"button\">" .
                 CMS_L_CORE_CONTROL_040 .
                 "</a>" .
                 "</div>"); //p

        //target channel
        if ($channel !== NULL)
        {
            //not found
            if (
                 nstre($channel)
                 &&
                 (! $core->channel->seek(["channel" => $channel]))
                 &&
                 (! $core->index->seek(["channel" => $channel]))
               )
            {
                echo("<div class=\"response-error\">" .
                     CMS_L_CORE_CONTROL_019 .
                     "</div>");
                break;
            };

            $list = stre($channel) ?
                [CMS_CORE_PORTAL => ""] :
                [$channel        => $channel];
        }

        //all channels
        else
        {
            $list = [CMS_CORE_PORTAL => ""];

            //permanent channels
            $core->channel->reset();
            while ($core->channel->next(["channel" => TRUE]))
            {
                $name        = $core->channel->get("channel");
                $list[$name] = $name;
            };

            //temporary channels
            $core->index->reset();
            while ($core->index->next(["channel" => TRUE]))
            {
                $name        = $core->index->get("channel");
                $list[$name] = $name;
            };

            ksort($list, SORT_NATURAL | SORT_FLAG_CASE);
        };

        //target user
        if ($guid !== NULL)
        {
            //not found
            if (($profile = $core->get_profile($guid)) === FALSE)
            {
                echo("<div class=\"response-error\">" .
                     CMS_L_CORE_CONTROL_046 .
                     "</div>");
                break;
            };

            $_list = [$guid => $profile["name"]];
        }

        //all users
        else
        {
            $_list = [];

            $core->profile->reset();
            while ($core->profile->next(["guid" => TRUE]))
            {
                $guid         = $core->profile->get("guid");
                $name         = $core->profile->get("name");
                $_list[$guid] = $name;
            };

            asort($_list, SORT_NATURAL | SORT_FLAG_CASE);
        };

        //status
        $__list = [];

        if ($core->operator)
        {
            $__list[CMS_L_CORE_CONTROL_004] = CMS_CORE_STATUS_OWNER;
            $__list[CMS_L_CORE_CONTROL_028] = CMS_CORE_STATUS_SPY;
            $__list[CMS_L_CORE_CONTROL_029] = CMS_CORE_STATUS_INVISIBLE;
        };

        $__list[CMS_L_CORE_CONTROL_005] = CMS_CORE_STATUS_OPERATOR;
        $__list[CMS_L_CORE_CONTROL_031] = CMS_CORE_STATUS_MUTE;
        $__list[CMS_L_CORE_CONTROL_032] = CMS_CORE_STATUS_BANNED;

        $buffer = "";
        $flag   = TRUE;

        foreach ($list AS $key => $value) //channel
        {
            $_flag = TRUE;

            foreach ($_list AS $_key => $_value) //user
            {
                $status = $core->get_status($_key, $value);
                $__flag = TRUE;

                foreach ($__list AS $__key => $__value) //status
                {
                    $___flag = flag($status, $__value);
                    if (! $core->status($___flag ? -$__value : $__value, $_key, $value, FALSE, TRUE)) continue;

                    //channel
                    if ($_flag)
                    {
                        echo("<h3>" .
                             image("core/icon_channel") . " " . x($key) .
                             "</h3>" .
                             "<div class=\"core-control-list\">");
                        $flag = $_flag = FALSE;
                    };

                    //user
                    if ($__flag)
                    {
                        echo("<div class=\"core-control-profile\">" .
                             image("core/icon_user") . " " . x($_value) . "<br>");
                        $__flag = FALSE;
                    };

                    $buffer   = x(stre($value) ? "  " : $value); //channel
                    $_buffer  = x($_key);                        //user
                    $__buffer = x($__value);                     //status

                    echo("<label>" .
                         "<input name=\"core_control_value[$buffer][$_buffer][$__buffer]\" " .
                                "type=\"hidden\" " .
                                "value=\"\">" .
                         "<input name=\"core_control_value[$buffer][$_buffer][$__buffer]\" " .
                                "type=\"checkbox\" " .
                                "value=\"1\"" .
                                ($___flag ? " checked" : "") . ">" .
                         x($__key) .
                         "</label> ");
                }; //status

                if (! $__flag) echo("</div>");
            }; //user

            if (! $_flag) echo("</div>");
        }; //channel

        echo($flag ?

        //no actions available
             "<div class=\"p\">" .
             CMS_L_CORE_CONTROL_003 .
             "</div>" : //p

        //apply command
             "<fieldset>" .
             "<input type=\"submit\" " .
                    "value=\"" . CMS_L_CORE_CONTROL_026 . "\">" .
             "</fieldset>");
    };

    echo("</form>");
};