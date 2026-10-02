<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   INTERFACE
//==================================================================================================

//permission
ifc_permission(["" => t("access")]);

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "setup_password":
//..................................................................................................

    $data      = new data("#system/permission");
    $superuser = "user." . CMS_SUPERUSER;

    //password empty
    if (stre($ifc_param1))
    {
        $ifc_response = t("msg_error") . t("err_no_password");
        break;
    };

    //confirmation mismatch
    if (nstreq($ifc_param1, $ifc_param2))
    {
        $ifc_response = t("msg_error") . t("err_password_mismatch");
        break;
    };

    //encode and save
    $ifc_param1 = hash64($ifc_param1);
    $data->set($ifc_param1, $superuser, "password");

    //set credential
    $ifc_param1 = hash64(cms_salt("password") . $ifc_param1);
    cms_set_cookie(["cms_password" => $ifc_param1]);

    $ifc_response = $data->save() ? t("msg_done") : t("msg_error");
    break;

case "setup_database":
//..................................................................................................

    //set values
    $system = new system();
    $system->setval($ifc_param1, "mysql", "host");
    $system->setval($ifc_param2, "mysql", "database");
    $system->setval($ifc_param3, "mysql", "user");
    $system->setval($ifc_param4, "mysql", "password");

    $ifc_response = $system->save() ? t("msg_done") : t("msg_error");
    break;

case "mysql_restore":
//..................................................................................................

    if (! isset($ifc_param1)) break; //no selection

    //restore database
    if (streq($ifc_param1, "yes"))
    {
        $mysql = new mysql(); //establish database connection
        if (! $mysql->restore())
        {
            $ifc_response = t("msg_error") . t("err_initial_data_write");
            break;
        };
    };

    //remove database restore flag
    $system = new system();
    $system->setval(NULL, "mysql", "restore");
    $system->save();

    $ifc_response = t("msg_done");
    break;

case "setup_smtp":
//..................................................................................................

    //set values
    $system = new system();
    $system->setval($ifc_param1, "email", "address");
    $system->setval($ifc_param2, "email", "reply_to");
    $system->setval($ifc_param3, "email", "method");
    $system->setval($ifc_param4, "email", "smtp");
    $system->setval($ifc_param5, "email", "username");
    $system->setval($ifc_param6, "email", "password");

    $ifc_response = $system->save() ? t("msg_done") : t("msg_error");
    break;

case "do_not_show":
//..................................................................................................

    if (nstreq(CMS_SUPERUSER, "admin")) break; //administrator only

    //do not show setup after login anymore
    $system = new system();
    $system->setval(TRUE, "setup", "done");
    $system->setval(NULL, "mysql", "restore");

    $ifc_response = $system->save() ? t("msg_done") : t("msg_error");
    break;

case "update":
//..................................................................................................

    cms_daemon(
        "if (! cms_load(\"update\")) return;\n" .
        "if ((new update())->start(" . (isset($update_skip_backup) ? "TRUE" : "") . "))\n" .
        "    cms_cache_clean(CMS_DATA_PATH . \"#content/cache/\", TRUE);",
        "update",
        0,
        "System update");
    break;

case "update_status":
//..................................................................................................

    if (cms_load("update"))
    {
        header("Content-Type: application/json; charset=utf-8");

        $update = new update();
        $status = $update->get_status();
        $log    = ($status === CMS_UPDATE_STATUS_NONE) ?
                  t("waiting_for_execution") : $update->get_log();

        echo(json_encode([$status, $log]));
    };

    exit();

case "backup":
//..................................................................................................

    cms_daemon(
        "if (! cms_load(\"update\")) return;\n" .
        "(new update())->backup();",
        "backup",
        0,
        "System backup");
    break;

case "daemon":
//..................................................................................................

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        FALSE,
        NULL,
        NULL,
        t("daemon_monitor"));

    echo("<div id=\"setup-daemon-status\">" .
         "</div>");

?><style>

#setup-daemon-status
{
    MARGIN: var(--ifc-padding-half);
    WHITE-SPACE: pre-line;
}

#setup-daemon-status > DIV
{
    FONT-WEIGHT: bold;
}

</style><script>

function setup_update_status()
{
    asr_send("<?php echo(q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "_daemon"])));?>",
             (value) =>
    {
        value = htmlspecialchars(value);
        value = value.replace(/[^\n]+$/, "<div>$&</div>");
        document.getElementById("setup-daemon-status").innerHTML = value;
        setTimeout(setup_update_status, 5000);
    });
};

setup_update_status();

</script><?php

    $ifc->close();

case "_daemon":
//..................................................................................................

    echo(cms_daemon_status());
    exit();
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

$system     = new system();
$setup_done = $system->getval("setup", "done");

//change administrator passwort
if (streq(CMS_SUPERUSER, "admin")) //user is administrator
{
    $data = new data("#system/permission");

    if (

    //password is empty
         stre($password = $data->get("user." . CMS_SUPERUSER, "password"))
         ||

    //password matches default
         streq($password, "8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918") //hash64("admin");
       )
    {
        //display
        $ifc = new ifc(
            $ifc_response,
            $ifc_page,
            NULL);

        ifc_table_open("min");

        //title
        echo("<tr>" .
             "<th>" .
             t("administrator_password") .
             "</th>" .
             "</tr>" .

        //description
             "<tr>" .
             "<td class=\"info\">" .
             t("hint_default_admin_password") .
             "</td>" .
             "</tr>" .

        //values
             "<tr>" .
             "<td>");
        $ifc->set(t("new_password"),          "password 40 40 b");
        $ifc->set(t("password_confirmation"), "password 40 40");
        echo("</td>" .
             "</tr>" .

        //submit button
             "<tr>" .
             "<td class=\"varied\" " .
                 "style=\"FONT-WEIGHT:bold;TEXT-ALIGN:right\">");
        $ifc->set(t("command_next"), "button", "setup_password");
        echo("</td>" .
             "</tr>");
        ifc_table_close();

        //do not show setup after login anymore
        if (! $setup_done) $ifc->set(t("do_not_show_setup_again"), "button b", "do_not_show");

        $ifc->close();
    };
};

//..................................................................................................

//set up database connection
cms_error_silent();
new mysql(); //establish database connection
cms_error_silent(FALSE);

if (mysql_get_link_identifier() === NULL) //no connection
{
    //display error
    if (streq(CMS_IFC_MESSAGE, "setup_database"))
    {
        $ifc_response = t("msg_error") . t("err_db_connection");
    }

    //get values
    else
    {
        $ifc_param1 = $system->getval("mysql", "host");
        $ifc_param2 = $system->getval("mysql", "database");
        $ifc_param3 = $system->getval("mysql", "user");
        $ifc_param4 = $system->getval("mysql", "password");
    };

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        NULL);

    ifc_table_open("min");

    //title
    echo("<tr>" .
         "<th>" .
         t("database_connection") .
         "</th>" .
         "</tr>" .

    //description
         "<tr>" .
         "<td class=\"info\">" .
         t("hint_db_credentials") .
         "</td>" .
         "</tr>" .

    //values
         "<tr>" .
         "<td>");
    $ifc->set(t("mysql_server"),   "text 40 b",   $ifc_param1);
    $ifc->set(t("mysql_database"), "text 40 b",   $ifc_param2);
    $ifc->set(t("user"),           "text 40 b",   $ifc_param3);
    $ifc->set(t("password"),       "password 40", $ifc_param4);
    echo("</td>" .
         "</tr>" .

    //submit button
         "<tr>" .
         "<td class=\"varied\" " .
             "style=\"FONT-WEIGHT:bold;TEXT-ALIGN:right\">");
    $ifc->set(t("command_next"), "button", "setup_database");
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    //do not show setup after login anymore
    if (! $setup_done) $ifc->set(t("do_not_show_setup_again"), "button b", "do_not_show");

    $ifc->close();
};

//..................................................................................................

//restore database
if ($system->getval("mysql", "restore"))
{
    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        NULL);

    ifc_table_open("min");

    //title
    echo("<tr>" .
         "<th>" .
         t("database_initialization") .
         "</th>" .
         "</tr>" .

    //description
         "<tr>" .
         "<td class=\"info\">" .
         t("hint_write_initial_data") .
         "</td>" .
         "</tr>" .

    //values
         "<tr>" .
         "<td>");
    $ifc->set(t("yes_write_data"), "radio", "yes", TRUE);
    $ifc->set(t("no_skip_step"),   "radio", "no");
    echo("</td>" .
         "</tr>" .

    //submit button
         "<tr>" .
         "<td class=\"varied\" " .
             "style=\"FONT-WEIGHT:bold;TEXT-ALIGN:right\">");
    $ifc->set(t("command_next"), "button", "mysql_restore");
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    //do not show setup after login anymore
    if (! $setup_done) $ifc->set(t("do_not_show_setup_again"), "button b", "do_not_show");

    $ifc->close();
};

//..................................................................................................

//ask user to set up smtp connection
while (TRUE)
{
    //get values
    if ($flag = nstreq(CMS_IFC_MESSAGE, "setup_smtp"))
    {
        $ifc_param1 = $system->getval("email", "address");
        $ifc_param2 = $system->getval("email", "reply_to");
        $ifc_param3 = $system->getval("email", "method");
        $ifc_param4 = $system->getval("email", "smtp");
        $ifc_param5 = $system->getval("email", "username");
        $ifc_param6 = $system->getval("email", "password");
    };

    //set default email
    if (stre($ifc_param1)) $ifc_param1 = CMS_EMAIL_AGENT;

    if (

    //method is not mail function
         nstreq($ifc_param3, "mail")
         &&

    //and host is not set
         stre($ifc_param4)
       )
    {
        //show error on incomplete input
        if (! $flag) $ifc_response = t("msg_error");
    }

    //test mail delivery
    else
    {
        if (! cms_load("smtp")) ifc_inactive($ifc_page);

        $response = NULL; //smtp response

        //try to connect
        cms_error_silent();
        $smtp = new smtp();
        cms_error_silent(FALSE);

        //success
        if ($smtp->enabled)
        {
            if (! cms_load("mime")) ifc_inactive($ifc_page);

            //generate test message
            $mime = new mime();
            $mime->add_text(t("test_mail_subject"), //subject
                            t("test_mail_body"));   //body

            //get receiver
            $to = nstre($ifc_param2) ? $ifc_param2 : $ifc_param1;

            //send test message
            cms_error_silent();
            if (! $smtp->send($mime,        //mime
                              $ifc_param1,  //from
                              $to,          //to
                              NULL,         //cc
                              NULL,         //bcc
                              $ifc_param2)) //reply_to
                $response = init($smtp->response, t("unknown_error"));

            $smtp->quit();
            cms_error_silent(FALSE);
        }

        //error
        else
        {
            $response = init($smtp->response, t("unknown_error"));
        };

        if (stre($response)) break; //no error

        //show error on invalid input
        if (! $flag) $ifc_response = t("msg_error") . x($response);
    };

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        NULL);

    ifc_table_open("min");

    //title
    echo("<tr>" .
         "<th>" .
         t("email_delivery") .
         "</th>" .
         "</tr>" .

    //description
         "<tr>" .
         "<td class=\"info\">" .
         t("hint_email_delivery") .
         "</td>" .
         "</tr>" .

    //values
         "<tr>" .
         "<td>");
    $ifc->set(t("email_from"),        "text 40 b", $ifc_param1);
    $ifc->set(t("email_reply_to"),    "text 40 b", $ifc_param2);
    $ifc->set(t("use_mail_function"), "radio b",   "mail", streq($ifc_param3, "mail"));
    $ifc->set(t("smtp_settings"),     "title");
    $ifc->set(t("use_smtp_server"),   "radio b",     NULL, stre($ifc_param3));
    $ifc->set(t("smtp_server"),       "text 40 b",   $ifc_param4);
    $ifc->set(t("user"),              "text 40 b",   $ifc_param5);
    $ifc->set(t("password"),          "password 40", $ifc_param6);
    echo("</td>" .
         "</tr>" .

    //submit button
         "<tr>" .
         "<td class=\"varied\" " .
             "style=\"FONT-WEIGHT:bold;TEXT-ALIGN:right\">");
    $ifc->set(t("command_next"), "button", "setup_smtp");
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    //do not show setup after login anymore
    if (! $setup_done) $ifc->set(t("do_not_show_setup_again"), "button b", "do_not_show");

    $ifc->close();
};

//..................................................................................................

//display
$ifc = new ifc(
    $ifc_response,
    $ifc_page,
    [t("daemon_monitor") . "|setup/command_daemon" => "daemon"]);

//do not show setup after login anymore
if (! $setup_done)
{
    $system->setval(TRUE, "setup", "done");
    $system->setval(NULL, "mysql", "restore");
    $system->save();
};

//update
if (! cms_load("update")) ifc_inactive($ifc_page);
$update = new update();

//get status
$_update   = cms_daemon_exists("update");
$scheduled = $_update || cms_daemon_exists("backup");
$running   = $scheduled && (cms_daemon_running("update") || cms_daemon_running("backup"));

//reset status
if (! $running) $update->status(CMS_UPDATE_STATUS_NONE);

//not running
if (! $scheduled)
{
    //get available version
    cms_error_silent();
    $version = $update->available(TRUE);
    cms_error_silent(FALSE);

    //no connection
    if ($version === FALSE)
    {
        $ifc->set(t("err_update_version_unknown"),
                  "info b");
        $ifc->set(t("try_again") . "|setup/command_refresh",
                  "button b");
        $ifc->close();
    };

    //no new version
    if ($version === NULL)
    {
        $ifc->set(t("system_up_to_date"), "info");
    }

    //new version available
    else
    {
        $ifc->set(sprintf(t("newer_version_available"),
                          x($version),
                          x(CMS_VERSION)),
                  "info b");
        $ifc->set(t("skip_backup_not_recommended"),
                  "checkbox b",
                  TRUE,
                  FALSE,
                  "update_skip_backup");
        $ifc->set(t("install_update_now") . "|setup/command_update",
                  "button b",
                  "update");
    };

    //create backup
    $ifc->set(t("backup"),
              "title");
    $ifc->set(t("create_new_backup") . "|setup/command_backup",
              "button b",
              "backup");

    //download backup
    $path  = CMS_UPDATE_PATH . "backup/";
    $list  = ["backup.zip", "_backup.zip"];
    $flag  = [];
    $_flag = streq(CMS_IFC_MESSAGE, "download");

    foreach ($list AS $key => $value)
    {
        if (! is_file($path . $value)) continue;

        $time = filemtime($path . $value);
        if ($time === FALSE) continue;
        $flag[$key] = $time;

        if ($_flag && streq($object, $key))
        {
            download($path . $value);
            exit();
        };
    };

    if (count($flag))
    {
        $ifc->set(t("download_backup"), "title");

        foreach ($flag AS $key => $value)
            $ifc->set(date(t("datetime_format"), $value) .
                      "|setup/command_download",
                      "button",
                      "javascript:ifc_download('" .
                      q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                 "ifc_message" => "download",
                                 "object"      => $key])) . "');");
    }

    $ifc->close();
};

//..................................................................................................

//update or backup currently running
if ($_update)
{
    $label = t("update_installation");
    $max   = 4;
}
else
{
    $label = t("backup_operation");
    $max   = 2;
};

$ifc->set($label, "label");

//progress bar
echo("<label>" .
     $label . "<br>" .
     "<progress id=\"setup_update_progress\" " .
               "max=\"$max\" " .
               "value=\"0\">" .
     "</progress>" .
     "</label><br>" .

//status report
     "<div id=\"setup_update_response\">" .
     "</div>");

?><style>

LABEL:has(#setup_update_progress)
{
    MARGIN: var(--ifc-padding-half);
}

#setup_update_progress
{
    MARGIN-TOP: var(--ifc-padding-half);
}

#setup_update_response
{
    MARGIN: var(--ifc-padding-half);
    WHITE-SPACE: pre-line;
}

#setup_update_response > DIV
{
    FONT-WEIGHT: bold;
}

</style><script>

function setup_update_response(value)
{
    value = htmlspecialchars(value);
    value = value.replace(/[^\n]+$/, "<div>$&</div>");
    document.getElementById("setup_update_response").innerHTML = value;
};

function setup_update_status()
{
    asr_send("<?php echo(q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "update_status"])));?>",
             (value) =>
    {
        if (! value)
        {
            setup_update_response("<?php echo(q(t("err_no_server_response")));?>");
            return;
        };

        var [status, log] = JSON.parse(value);
        var progress      = document.getElementById("setup_update_progress");

        switch (status)
        {
        case -1: value = 0; break;
        case  0: value = 0; break;
        case  1: value = 4; break;
        case  2: value = 1; break;
        case  3: value = 2; break;
        case  4: value = 3; break;
        case  5: value = 4; break;
        };

        progress.value = Math.min(value, progress.max);

        setup_update_response(log);

        if ((status >= 0) && (status !== 1))
            setTimeout(setup_update_status, (status === 0) ? 5000 : 1000);
    });
};

setup_update_status();

</script><?php

$ifc->close();