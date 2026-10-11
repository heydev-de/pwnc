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

define("CMS_MAILBOX_PROPERTY_DATE",            1);
define("CMS_MAILBOX_PROPERTY_FROM",            2);
define("CMS_MAILBOX_PROPERTY_SUBJECT",         3);
define("CMS_MAILBOX_PROPERTY_STATUS",          4);
define("CMS_MAILBOX_PROPERTY_SPAM_INDICATOR",  5);
define("CMS_MAILBOX_PROPERTY_SIZE",            6);

define("CMS_MAILBOX_STATUS_NONE",              0);
define("CMS_MAILBOX_STATUS_IMPORTANCE_HIGH",   1);
define("CMS_MAILBOX_STATUS_READ",              2);
define("CMS_MAILBOX_STATUS_SENT",              4);
define("CMS_MAILBOX_STATUS_BAD",               8);
define("CMS_MAILBOX_STATUS_GOOD",             16);
define("CMS_MAILBOX_STATUS_DRAFT",            32);
define("CMS_MAILBOX_STATUS_ATTACHMENT",       64);

define("CMS_MAILBOX_DRAFT_TYPE_MESSAGE",       1);
define("CMS_MAILBOX_DRAFT_TYPE_REPLY",         2);
define("CMS_MAILBOX_DRAFT_TYPE_RELAY",         3);

define("CMS_MAILBOX_DRAFT_OPTION_TEXT",        1);
define("CMS_MAILBOX_DRAFT_OPTION_ATTACHMENT",  2);

//related functions
//--------------------------------------------------------------------------------------------------

function mailbox_text(&$data)
{
    $text = "";

    $data->move("first");
    while ($key = $data->move("next"))
    {
        //get content type
        $content_type = $data->get($key, "content-type") ?? "";
        $content_type = mime_extract_rfc2822_header($content_type);
        $content_type = nstre($content_type["#value"] ?? "") ?
                        strtolower($content_type["#value"]) :
                        "text/plain";

        //get subject
        $subject = $data->get($key, "subject") ?? "";
        $subject = mime_rfc2047_decode($subject);
        if (nstre($subject)) $text .= $subject . " ";

        //get body
        if (preg_match("/^text(?:\/|$)/", $content_type))
        {
            $body = $data->get($key, "#body");
            if (stre($body)) continue;

            //remove html markup
            if (streq($content_type, "text/html"))
                $body = htmltoplain($body);

            $text .= $body . " ";
        };
    };

    return substr($text, 0, -1);
};

//--------------------------------------------------------------------------------------------------

function mailbox_address($value)
{
    $array  = mime_extract_rfc2822_address($value);
    $return = [];

    foreach ($array AS $group => $address)
    {
        $_group = qx($group);
        $list   = [];

        foreach ($address AS $email => $name)
            $list[] = "<a href=\"javascript:a('$group','" . qx($name) . "','" . qx($email) . "');\" " .
                         "title=\"" . t("contacts") . "\">" .
                      image("desktop/icon_address") . " " .
                      x(stre($name) ? $email : "$name <$email>") .
                      "</a>";

        if ($list)
            $return[] = stre($group) ? implode(", ", $list) :
                        image("desktop/icon_group") . " " .
                        x($group) . ": " . implode(" ", $list);
    };

    return implode(" ", $return);
};

//--------------------------------------------------------------------------------------------------

function mailbox_attachment(&$data)
{
    $data->move("first");
    while ($key = $data->move("next"))
    {
        //get content type
        $content_type = $data->get($key, "content-type") ?? "";
        $content_type = mime_extract_rfc2822_header($content_type);
        $content_type = nstre($content_type["#value"] ?? "") ?
                        strtolower($content_type["#value"]) :
                        "text/plain";

        //embedded message
        if (streq($content_type, "message/rfc822")) return TRUE;

        //no multipart frame or text
        if (! preg_match("/^(?:multipart|text)(?:\/|$)/", $content_type)) return TRUE;
    };

    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function mailbox_path($container = NULL,
                      $filename  = NULL)
{
    global $object,
           $container_name;
    static $cache = [];

    $mailbox = ($cache[$object] ??= encode_filename($object)) . "/";

    if (nstre($container))
        $container = isset($container_name[$container]) ? "$container/" :
                     ($cache[$container] ??= encode_filename($container)) . "/";

    if (nstre($filename))
        $filename = $cache[$filename] ??= encode_filename($filename);

    return DESKTOP_PATH . $mailbox . $container . $filename;
};

if (
     ! cms_load("pop")
     ||
     ! cms_load("smtp")
     ||
     ! cms_load("mime")
   )
    ifc_inactive($ifc_page);

//set predefined containers
$container_icon =
    ["#inbox"    => "desktop/icon_inbox",
     "#draft"    => "desktop/icon_draft",
     "#outbox"   => "desktop/icon_outbox",
     "#trashbin" => "desktop/icon_trashbin",
     "#spam"     => "desktop/icon_spam"];
$container_name =
    ["#inbox"    => t("in_box"),
     "#draft"    => t("drafts"),
     "#outbox"   => t("out_box"),
     "#trashbin" => t("trash"),
     "#spam"     => t("spam")];

//verify predefined containers
foreach ($container_name AS $key => $_)
{
    if (! mkpath(mailbox_path($key))) ifc_inactive($ifc_page);
};

init($container, "#inbox");
init($order,     CMS_MAILBOX_PROPERTY_DATE);

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "retrieve":
//..................................................................................................

    //establish pop3 connection
    $pop = new pop(
        $desktop->object_get($object, "email_pop_host"),
        $desktop->object_get($object, "email_username"),
        $desktop->object_get($object, "email_password"));

    if (! $pop->enabled)
    {
        $ifc_response = t("msg_error") . (nstre($pop->response) ? "<br>" . x($pop->response) : "");
        break;
    };

    //initial server response
    $response = $pop->response;

    //retrieve unique identifier list
    $list_uidl = $pop->get_unique_id_list();

    //message counter
    $count      = 0;
    $count_spam = 0;

    //error flag
    $error = FALSE;

    //scan for messages
    if (is_array($list_uidl))
    {
        //open message index
        $data_mailbox = new data(mailbox_path(NULL, "mailbox.dat"));

        //instantiate spam filter
        $category       = cms_load("category") ? new category() : NULL;
        $spam_threshold = (int)$desktop->object_get($object, "email_spam_threshold");

        //delete message from server flag
        $flag_delete = (bool)$desktop->object_get($object, "email_delete");

        //get time
        $time = explode(" ", microtime());
        $time = (float)$time[0] + (float)$time[1];

        //new message directories
        $path_inbox = mailbox_path("#inbox");
        $path_spam  = mailbox_path("#spam");

        foreach ($list_uidl AS $key => $value)
        {
            $id = hash32($value);

            //skip stored message
            if ($data_mailbox->get($id)) continue;

            //receive message
            $data_message = $pop->get_message($key);
            if ($error = ($data_message === FALSE)) break;

            $key            = $data_message->move("first");
            $path           = $path_inbox;
            $spam_indicator = NULL;
            $count++;

            //get content type
            $content_type = $data_message->get($key, "content-type") ?? "";
            $content_type = mime_extract_rfc2822_header($content_type);

            //meta data
            $from    = $data_message->get($key, "from");
            $subject = mime_rfc2047_decode($data_message->get($key, "subject") ?? $value);
            $size    = (int)$data_message->get($key, "#size");
            $status  = streq(strtolower($data_message->get($key, "importance") ?? ""), "high") ?
                       CMS_MAILBOX_STATUS_IMPORTANCE_HIGH : CMS_MAILBOX_STATUS_NONE;
            if (mailbox_attachment($data_message)) $status |= CMS_MAILBOX_STATUS_ATTACHMENT;

            //apply spam filter
            if ($category && $category->enabled)
            {
                $text           = mailbox_text($data_message);
                $spam_indicator = $category->evaluate_spam($text);

                //spam
                if ($spam_threshold && ($spam_indicator >= $spam_threshold))
                {
                    $path = $path_spam;
                    $count_spam++;
                }

                //no spam
                elseif ($category->train_nospam($text))
                {
                    $status |= CMS_MAILBOX_STATUS_GOOD;
                };
            };

            //force save message
            if (! $data_message->save($path . encode_filename("$id.dat"), TRUE))
            {
                $pop->response = sprintf(t("err_message_save"), x($value));
                $error = TRUE;
                break;
            };

            //delete message from server
            if ($flag_delete) $pop->delete($key);

            //set message index
            $data_mailbox->set(
                [CMS_MAILBOX_PROPERTY_DATE           => $time,
                 CMS_MAILBOX_PROPERTY_FROM           => $from,
                 CMS_MAILBOX_PROPERTY_SUBJECT        => $subject,
                 CMS_MAILBOX_PROPERTY_STATUS         => $status,
                 CMS_MAILBOX_PROPERTY_SPAM_INDICATOR => $spam_indicator,
                 CMS_MAILBOX_PROPERTY_SIZE           => $size],
                $id);

            //advance time to preserve sorting order
            $time += 0.01;
        };

        //save index
        $data_mailbox->save();

        //update spam filter
        if ($category) $category->update();
    };

    //terminate session
    $pop->quit();

    $container    = "#inbox";
    $ifc_response = ($error ?
                    t("msg_error") . (nstre($pop->response) ? x($pop->response) . "<br>" : "") :
                    t("msg_done") .  (nstre($response)      ? x($response)      . "<br>" : "")) .
                    sprintf(t("new_messages_spam"), $count, $count_spam);
    break;

case "display":
case "_display":
//..................................................................................................

    //open message
    $path         = mailbox_path($container, "$ifc_param.dat"); //used later again
    $data_message = new data($path);

    //extended display url
    $url = cms_url(
        ["desktop_display" => "interface",
         "ifc_message"     => "_display",
         "ifc_param"       => $ifc_param,
         "user"            => DESKTOP_USER,
         "object"          => $object,
         "container"       => $container,
         "part"            => "\x1B%replace%"]);

    switch (CMS_IFC_MESSAGE)
    {
    case "display":
//..................................................................................................

        $_ifc_param = q($ifc_param);

        //display
        $ifc = new ifc(
            NULL,
            $ifc_page,
            [t("reply") . "|desktop/command_reply"
             => "javascript:ifc_post('compose_reply','$_ifc_param');",
             t("forward") . "|desktop/command_relay"
             => "javascript:ifc_post('compose_relay','$_ifc_param');",
             t("command_close") . "|desktop/command_cancel"
             => NULL],
            array_merge(
                $param,
                ["container" => $container,
                 "order"     => $order]),
            NULL,
            t("read_message"));

?><script>

//address
function a(group,
           name,
           email)
{
    load_page("<?php
        echo(str_replace(
            ["%group%",
             "%name%",
             "%email%"],
            ["\"+encodeURIComponent(group)+\"",
             "\"+encodeURIComponent(name)+\"",
             "\"+encodeURIComponent(email)+\""],
            q(cms_url(array_merge(
                $param,
                ["desktop_display"   => "interface",
                 "desktop_interface" => "address",
                 "object"            => NULL,
                 "ifc_message"       => "address",
                 "group"             => "\x1B%group%",
                 "name"              => "\x1B%name%",
                 "email"             => "\x1B%email%"])))));?>");
};

</script><?php

        ifc_table_open();
        echo("<colgroup>" .
             "<col style=\"WIDTH:0\">" .
             "<col>" .
             "</colgroup>");

        $flag = TRUE; //display header flag

        $data_message->move("first");
        while ($key = $data_message->move("next"))
        {
            //get content type
            $content_type           = $data_message->get($key, "content-type") ?? "";
            $content_type           = mime_extract_rfc2822_header($content_type);
            $content_type["#value"] = nstre($content_type["#value"]) ?
                                      strtolower($content_type["#value"]) :
                                      "text/plain";

            //split mime type
            preg_match(
                "/^([^\/]+)(?:\/(.+))?$/",
                $content_type["#value"],
                $content_type_value);

            $content_type_subvalue = $content_type_value[2] ?? "";
            $content_type_value    = $content_type_value[1] ?? "";

            //display message header
            if ($flag)
            {
                //subject
                $subject = $data_message->get($key, "subject") ?? "";
                $subject = nstre($subject) ?
                           x(mime_rfc2047_decode($subject)) :
                           t("no_subject");

                echo("<tr>" .
                     "<th colspan=\"2\" " .
                         "class=\"br\">" .
                     image("desktop/icon_mailbox") . " " . $subject .
                     "</th>" .
                     "</tr>");

                //from
                if (nstre($from = $data_message->get($key, "from")))
                    echo("<tr>" .
                         "<td class=\"varied\">" .
                         t("sender") . ":" .
                         "</td>" .
                         "<td class=\"br\">" .
                         mailbox_address($from) .
                         "</td>" .
                         "</tr>");

                //date
                if (nstre($date = $data_message->get($key, "date")))
                {
                    if (($_date = strtotime($date)) > 0)
                        $date = friendly_date($_date);

                    echo("<tr>" .
                         "<td class=\"varied\">" .
                         t("send_date") . ":" .
                         "</td>" .
                         "<td class=\"br\">" .
                         x($date) .
                         "</td>" .
                         "</tr>");
                };

                //to
                if (nstre($to = $data_message->get($key, "to")))
                    echo("<tr>" .
                         "<td class=\"varied\">" .
                         t("recipient") . ":" .
                         "</td>" .
                         "<td class=\"br\">" .
                         mailbox_address($to) .
                         "</td>" .
                         "</tr>");

                //cc
                if (nstre($cc = $data_message->get($key, "cc")))
                    echo("<tr>" .
                         "<td class=\"varied\">" .
                         t("secondary_recipient") . ":" .
                         "</td>" .
                         "<td class=\"br\">" .
                         mailbox_address($cc) .
                         "</td>" .
                         "</tr>");

                $flag = NULL;
            };

            //process nesting
            switch ($data_message->get($key, "#type"))
            {
            case "container":
//..................................................................................................

                if (streq($content_type_value, "message")) $flag = TRUE;
                continue 2;

            case "/container":
//..................................................................................................

                continue 2;
            };

            //display message body
            echo("<tr class=\"varied\">" .
                 "<td colspan=\"2\">");

            //icon
            $image = image(get_mime_icon($content_type["#value"]));

            $_url = str_replace("%replace%", r($key), $url);

            //file download
            if (nstre($name = ($content_type["name"] ?? "")))
                echo("<a href=\"" . x($_url) . "\">" .
                     "<strong>" .
                     "$image " . x($name) .
                     "</strong>" .
                     "</a> ");

            //open in browser
            else
                echo("<a href=\"" . x($_url) . "\" " .
                        "onclick=\"load_page('" . q($_url) . "');return false;\">" .
                     "<strong>" .
                     "$image " . t("command_show") .
                     "</strong>" .
                     "</a> ");

            //size
            echo(sprintf(t("attachment_format_size"),
                         x($content_type["#value"]),
                         format_bytesize($data_message->get($key, "#size"))) .
                 "</td>" .
                 "</tr>");

            //preview text
            switch ($content_type_value)
            {
            case "text":
//..................................................................................................

                $body = $data_message->get($key, "#body") ?? "";
                if (stre($body)) break; //no content

                //clean up text
                $body = streq($content_type_subvalue, "html") ?
                        htmltoplain($body, TRUE) :
                        stripspaces($body, TRUE);

                //display text
                echo("<tr>" .
                     "<td colspan=\"2\" " .
                         "class=\"text\">" .
                     x($body) .
                     "</td>" .
                     "</tr>");
            };
        };

        ifc_table_close();

        //open message index
        $data_mailbox = new data(mailbox_path(NULL, "mailbox.dat"));

        //get status
        $status = $data_mailbox->get($ifc_param, CMS_MAILBOX_PROPERTY_STATUS);

        //set read status
        if (! flag($status, CMS_MAILBOX_STATUS_READ))
        {
            $data_mailbox->set($status | CMS_MAILBOX_STATUS_READ, $ifc_param, CMS_MAILBOX_PROPERTY_STATUS);
            $data_mailbox->save();
        };

        $ifc->close();

    case "_display":
//..................................................................................................

        //get content type
        $content_type            = $data_message->get($part, "content-type") ?? "";
        $_content_type           = mime_extract_rfc2822_header($content_type);
        $_content_type["#value"] = nstre($_content_type["#value"]) ?
                                   strtolower($_content_type["#value"]) :
                                   "text/plain";

        //split mime type
        preg_match(
            "/^([^\/]+)(?:\/(.+))?$/",
            $_content_type["#value"],
            $content_type_value);

        $content_type_subvalue = $content_type_value[2] ?? "";
        $content_type_value    = $content_type_value[1] ?? "";

        //get body
        $body = $data_message->get($part, "#body") ?? "";

        //embedded content
        if (streq($content_type_value, "text"))
        {
            $translation = [];

            $data_message->move("first");
            while ($key = $data_message->move("next"))
            {
                //attachment url
                $_url = str_replace("%replace%", r($key), $url);

                if (preg_match("/<?((?(?<=<)[^>]+|.+))/",
                               $data_message->get($key, "content-id") ?? "",
                               $array))
                    $translation["cid:" . $array[1]] = $_url;

                if (nstre($content_location = $data_message->get($key, "content-location")))
                    $translation[$content_location]  = $_url;
            };

            //replace attachment placeholders with actual url
            $body = strtr($body, $translation);
        };

        //etag check
        $mod_time = filemtime($path);
        $filesize = strlen($body);
        $etag     = "\"$mod_time.$filesize\"";

        if (strpos($_SERVER["HTTP_IF_NONE_MATCH"] ?? "", $etag) !== FALSE)
        {
            http_response_code(304); //not modified
            exit(); //don't send body
        };

        //http header
        header(nstre($name = ($_content_type["name"] ?? "")) ?
               "Content-Disposition: attachment; filename=" . quote_http_param($name) :
               "Content-Disposition: inline; filename=download");
        header("Content-Length: " . $filesize);
        header("Content-Type: $content_type");
        header("Last-Modified: " . gmdate("D, d M Y H:i:s", (int)$mod_time) . " GMT");
        header("ETag: $etag");

        //transmit part
        echo($body);
        exit();
    };

case "move":
//..................................................................................................

    if (empty($list))               break; //no selection
    if (streq($container, $target)) break; //same directory

    $error = FALSE;

    foreach ($list AS $value)
        $error |= ! rename(mailbox_path($container, "$value.dat"),
                           mailbox_path($target,    "$value.dat"));

    $ifc_response = $error ? t("msg_error") : t("msg_done");
    break;

case "mail":
case "compose":
case "compose_edit":
case "compose_reply":
case "compose_relay":
case "_compose":
case "compose_send":
case "compose_save":
//..................................................................................................

    switch (CMS_IFC_MESSAGE)
    {
    case "mail":
    case "compose":
//..................................................................................................

        //set draft type
        $draft_type = CMS_MAILBOX_DRAFT_TYPE_MESSAGE;

        //initialize input fields
        init($to);
        init($cc);
        init($bcc);
        init($ifc_param1);                                                              //subject
        init($ifc_param2, "\n\n\n" . $desktop->object_get($object, "email_signature")); //body

        //create draft
        $message = unique_id();
        $mime    = new mime(mailbox_path("#draft", "$message.dat"));

        //create multipart frame
        $key   = $mime->add_multipart();
        $email = $desktop->object_get($object, "email_address") ?? "";
        $name  = $desktop->object_get($object, "email_name")    ?? CMS_NAME;
        $from  = mime_build_rfc2822_address(["" => [$email => $name]]);

        $mime->data->set($from,                    $key, "from");
        $mime->data->set($draft_type,              $key, "#draft_type");
        $mime->data->set(CMS_MAILBOX_STATUS_DRAFT, $key, "#status");

        //create text body
        $key = $mime->add_text(
            NULL,        //subject
            $ifc_param2, //body
            "plain",     //subtype
            $key);       //parent_key
        $mime->data->set(CMS_MAILBOX_DRAFT_OPTION_TEXT, $key, "#draft_option");

        //save draft
        $mime->data->save();

        //set container
        $container = "#draft";
        break;

    case "compose_edit":
//..................................................................................................

        //open draft
        $message = $ifc_param;
        $mime    = new mime(mailbox_path($container, "$message.dat"));

        //get status
        $key    = $mime->data->move("first");
        $status = $mime->data->get($key, "#status");
        if ($status != CMS_MAILBOX_STATUS_DRAFT) break 2;

        //get draft type
        $draft_type = $mime->data->get($key, "#draft_type");

        //initialize input fields
        $to         = $mime->data->get($key, "to");
        $cc         = $mime->data->get($key, "cc");
        $bcc        = $mime->data->get($key, "bcc");
        $ifc_param1 = $mime->data->get($key, "subject"); //subject
        $key        = $mime->data->seek(["#draft_option" => CMS_MAILBOX_DRAFT_OPTION_TEXT]);
        $ifc_param2 = $mime->data->get($key, "#body");   //body
        break;

    case "compose_reply":
//..................................................................................................

        //set draft type
        $draft_type = CMS_MAILBOX_DRAFT_TYPE_REPLY;

        //initialize input fields
        init($to);
        init($cc);
        init($bcc);
        init($ifc_param1); //subject
        init($ifc_param2); //body

        //open reply message
        $data_message = new data(mailbox_path($container, "$ifc_param.dat"));

        //get reply address
        $key = $data_message->move("first");
        $to  = $data_message->get($key, "reply-to");
        if (stre($to)) $to = $data_message->get($key, "from");
        if (nstre($to))
        {
            $to = mime_extract_rfc2822_address($to);
            $to = key(reset($to));
        };

        //get reply text body
        $depth = 0;

        while ($key = $data_message->move("next"))
        {
            //get content type
            $content_type           = $data_message->get($key, "content-type") ?? "";
            $content_type           = mime_extract_rfc2822_header($content_type);
            $content_type["#value"] = nstre($content_type["#value"]) ?
                                      strtolower($content_type["#value"]) :
                                      "text/plain";

            //get reply subject
            if (stre($ifc_param1))
            {
                $ifc_param1 = mime_rfc2047_decode($data_message->get($key, "subject") ?? "");
                $ifc_param1 = t("re") . ": $ifc_param1";
            };

            //split mime type
            preg_match(
                "/^([^\\/]+)(?:\\/(.+))?$/",
                $content_type["#value"],
                $content_type_value);

            $content_type_subvalue = $content_type_value[2] ?? "plain";
            $content_type_value    = $content_type_value[1] ?? "text";

            //process nesting
            switch ($data_message->get($key, "#type"))
            {
            case "container":

                if ($depth || streq($content_type_value, "message")) $depth++;
                break;

            case "/container":

                if ($depth) $depth--;
                break;
            };

            //find first text body that is not an attachment
            //or located within an encapsulated message
            if (! $depth && streq($content_type_value, "text"))
            {
                $content_disposition = $data_message->get($key, "content-disposition") ?? "";
                $content_disposition = mime_extract_rfc2822_header($content_disposition);

                if (strcasecmp($content_disposition["#value"] ?? "", "attachment"))
                {
                    $body       = $data_message->get($key, "#body");
                    $ifc_param2 = streq($content_type_subvalue, "html") ?
                                  htmltoplain($body, TRUE) :
                                  stripspaces($body, TRUE);
                    break;
                };
            };
        };

        //message text
        $ifc_param2 = "\n\n\n" . $desktop->object_get($object, "email_signature") .
                      "\n\n" . t("original_message") .
                      "\n\n$ifc_param2";

        //create draft
        $message = unique_id();
        $mime    = new mime(DESKTOP_PATH . $object . "/#draft/" . $message . ".dat");

        //create multipart frame
        $key  = $mime->add_multipart();
        $from = $desktop->object_get($object, "email_address");
        $name = $desktop->object_get($object, "email_name");
        if (stre($name)) $name = CMS_NAME;
        $name = addcslashes($name, "\"\\");
        $mime->data->set("\"$name\" <$from>",      $key, "from");
        $mime->data->set($draft_type,              $key, "#draft_type");
        $mime->data->set(CMS_MAILBOX_STATUS_DRAFT, $key, "#status");

        //create text body
        $key = $mime->add_text(
            NULL,        //subject
            $ifc_param2, //body
            "plain",     //subtype
            $key);       //parent_key
        $mime->data->set(CMS_MAILBOX_DRAFT_OPTION_TEXT, $key, "#draft_option");

        //save draft
        $mime->data->save();

        //set container
        $container = "#draft";
        break;

    case "compose_relay":
//..................................................................................................

        //set draft type
        $draft_type = CMS_MAILBOX_DRAFT_TYPE_RELAY;

        //initialize input fields
        init($to);
        init($cc);
        init($bcc);
        init($ifc_param1);                                                              //subject
        init($ifc_param2, "\n\n\n" . $desktop->object_get($object, "email_signature")); //body

        //open relay message
        $data_message = new data(mailbox_path($container, "$ifc_param.dat"));
        $key = $data_message->move("first");

        //get content type
        $content_type = $data_message->get($key, "content-type") ?? "";
        $content_type = mime_extract_rfc2822_header($content_type);

        //get charset
        $charset = $content_type["charset"] ?? NULL;

        //get reply subject
        if (stre($ifc_param1))
        {
            $ifc_param1 = mime_rfc2047_decode($data_message->get($key, "subject") ?? "");
            $ifc_param1 = t("fw") . ": $ifc_param1";
        };

        //create draft
        $message = unique_id();
        $mime    = new mime(DESKTOP_PATH . $object . "/#draft/" . $message . ".dat");

        //create multipart frame
        $key  = $mime->add_multipart();
        $from = $desktop->object_get($object, "email_address");
        $name = $desktop->object_get($object, "email_name");
        if (stre($name)) $name = CMS_NAME;
        $name = addcslashes($name, "\"\\");
        $mime->data->set("\"$name\" <$from>",      $key, "from");
        $mime->data->set($draft_type,              $key, "#draft_type");
        $mime->data->set(CMS_MAILBOX_STATUS_DRAFT, $key, "#status");

        //attach relay message
        $mime->add_message("rfc822", $data_message, $key);

        //create text body
        $key = $mime->add_text(
            NULL,        //subject
            $ifc_param2, //body
            "plain",     //subtype
            $key);       //parent_key
        $mime->data->set(CMS_MAILBOX_DRAFT_OPTION_TEXT, $key, "#draft_option");

        //save draft
        $mime->data->save();

        //set container
        $container = "#draft";
        break;

    case "_compose":
    case "compose_send":
    case "compose_save":
//..................................................................................................

        //open draft
        $mime = new mime(mailbox_path($container, "$message.dat"));

        //get draft type
        $key        = $mime->data->move("first");
        $draft_type = (int)$mime->data->get($key, "#draft_type");

        //update header
        $from = $desktop->object_get($object, "email_address");
        $name = $desktop->object_get($object, "email_name");
        if (stre($name)) $name = CMS_NAME;
        $name = addcslashes($name, "\"\\");
        $mime->data->set("\"$name\" <$from>", $key, "from");
        $mime->data->set($to,                 $key, "to");
        $mime->data->set($cc,                 $key, "cc");
        $mime->data->set($bcc,                $key, "bcc");
        $mime->data->set($ifc_param1,         $key, "subject");

        if ($draft_type !== CMS_MAILBOX_DRAFT_TYPE_RELAY)
        {
            //remove attachment
            if (isset($remove))
            {
                foreach ($remove AS $_key => $_)
                    $mime->data->del($_key);
            };

            //add attachment
            if ($ifc_file1)
            {
                $_key = $mime->add_file($ifc_file1, $ifc_file1_name, TRUE, $key);
                $mime->data->set(CMS_MAILBOX_DRAFT_OPTION_ATTACHMENT, $_key, "#draft_option");
            };
        };

        //update text body
        $_key = $mime->data->seek(["#draft_option" => CMS_MAILBOX_DRAFT_OPTION_TEXT]);
        $mime->data->set($ifc_param2, $_key, "#body");

        //compute size in octets
        $size = 0;

        $mime->data->move("first");
        while ($_key = $mime->data->move("next"))
        {
            $value  = strlen($mime->data->get($_key, "#body") ?? "");
            $size  += $value;
            $mime->data->set($value, $_key, "#size");
        };

        $mime->data->set($size, $key, "#size");

        //save draft, forced so transport cannot strip the stored state
        $mime->data->save(NULL, TRUE);

        //open message index
        $data_mailbox = new data(mailbox_path(NULL, "mailbox.dat"));

        //delete message from index to force update
        $data_mailbox->del($message);

        //save message index
        $data_mailbox->save();

        switch (CMS_IFC_MESSAGE)
        {
        case "compose_send":
//..................................................................................................

            $host     = $desktop->object_get($object, "email_smtp_host");
            $username = $desktop->object_get($object, "email_username");
            $password = $desktop->object_get($object, "email_password");

            $smtp = new smtp($host, $username, $password);

            if (! $smtp->enabled)
            {
                $ifc_response = t("msg_error") . (nstre($smtp->response) ? "<br>" . x($smtp->response) : "");
                break;
            };

            if (! $smtp->send($mime))
            {
                $ifc_response = t("msg_error") . (nstre($smtp->response) ? "<br>" . x($smtp->response) : "");
                $smtp->quit();
                break;
            };

            //close smtp connection
            $smtp->quit();

            //reopen message, transport removes headers from the sent object
            $mime = new mime(mailbox_path($container, "$message.dat"));

            //set message status
            $key = $mime->data->move("first");
            $mime->data->set(CMS_MAILBOX_STATUS_SENT, $key, "#status");

            //remove draft settings
            $mime->data->del($key, "#draft_type");
            while ($key = $mime->data->move("next"))
            {
                $mime->data->del($key, "#draft_type");
                $mime->data->del($key, "#draft_option");
            };

            //write message to outbox
            $mime->data->save(mailbox_path("#outbox", "$message.dat"), TRUE);
            unlink(mailbox_path($container, "$message.dat"));

            //set container
            $container = "#outbox";

            $ifc_response = t("msg_done");
            break 3;

        case "compose_save":
//..................................................................................................

            break 3;
        };
    };

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        [t("send") .           "|desktop/command_apply"  => "compose_send",
         t("command_save") .   "|desktop/command_save"   => "compose_save",
         t("command_cancel") . "|desktop/command_cancel" => FALSE],
        array_merge(
            $param,
            ["container" => $container,
             "message"   => $message,
             "order"     => $order]),
        "_compose",
        t("compose_message"));

    //generate address list
    $list_address = [];

    $desktop->data->move("first");
    while($key = $desktop->data->move("next"))
    {
        if (
             streq($desktop->object_type($key), "address")
             &&
             nstre($email = $desktop->object_get($key, "email"))
           )
        {
            $name    = $desktop->object_get($key, "name");
            $company = $desktop->object_get($key, "company");
            $key     = (nstre($company) ? $company : "-") . $name . count($list_address);

            if (stre($name)) $label = nstre($company)          ?    $company   : $email;
            else             $label = $name . (nstre($company) ? " ($company)" : "");

            $list_address[$key] = [$email, $label];
        };
    };

    ksort($list_address, SORT_NATURAL | SORT_FLAG_CASE);

    //display address list
    echo("<datalist id=\"contact\">");
    foreach ($list_address AS [$email, $label])
        echo("<option value=\"" . x($email) . "\">" .
             x($label) .
             "</option>");
    echo("</datalist>");

    //input fields

    //to
    ifc_tab_open(t("recipient"));
    echo("<div class=\"fullwidth\">" .
         "<input name=\"to\" " .
                "type=\"email\" " .
                "value=\"" . x($to) . "\" " .
                "list=\"contact\" " .
                "multiple>" .
         "</div>");

    //cc
    ifc_tab_next(t("cc_copy"));
    echo("<div class=\"fullwidth\">" .
         "<input name=\"cc\" " .
                "type=\"email\" " .
                "value=\"" . x($cc) . "\" " .
                "list=\"contact\" " .
                "multiple>" .
         "</div>");

    //bcc
    ifc_tab_next(t("bcc_blind_copy"));
    echo("<div class=\"fullwidth\">" .
         "<input name=\"bcc\" " .
                "type=\"email\" " .
                "value=\"" . x($bcc) . "\" " .
                "list=\"contact\" " .
                "multiple>" .
         "</div>");
    ifc_tab_close();

    //subject
    $ifc->set(t("subject"), "text 0 w", $ifc_param1);

    //body
    $ifc->set(t("message"), "textarea 0x20 w", $ifc_param2);

    //attachments
    if ($draft_type === CMS_MAILBOX_DRAFT_TYPE_RELAY)
    {
        $ifc->set(t("msg_forward_attached"), "info");
    }
    else
    {
        ifc_table_open();
        echo("<colgroup>" .
             "<col>" .
             "<col style=\"WIDTH:0\">" .
             "</colgroup>" .

             "<tr>" .
             "<th colspan=\"2\">" .
             t("attachment") .
             "</th>" .
             "</tr>");

        //extended display url
        $url = cms_url(
            ["desktop_display" => "interface",
             "ifc_message"     => "_display",
             "ifc_param"       => $message,
             "user"            => DESKTOP_USER,
             "object"          => $object,
             "container"       => $container,
             "part"            => "\x1B%replace%"]);

        $mime->data->move("first");
        while ($key = $mime->data->move("next"))
        {
            if ($mime->data->get($key, "#draft_option") == CMS_MAILBOX_DRAFT_OPTION_ATTACHMENT)
            {
                //get content type
                $content_type           = $mime->data->get($key, "content-type") ?? "";
                $content_type           = mime_extract_rfc2822_header($content_type);
                $content_type["#value"] = nstre($content_type["#value"] ?? "") ?
                                          strtolower($content_type["#value"]) :
                                          "text/plain";

                //get charset
                $charset = $content_type["charset"] ?? NULL;

                //icon
                $image = image(get_mime_icon($content_type["#value"]));

                //display attachment
                echo("<tr>" .
                     "<td>");

                //attachment url
                $_url = str_replace("%replace%", r($key), $url);

                //file download
                if (nstre($name = $content_type["name"] ?? ""))
                    echo("<a href=\"" . x($_url) . "\">" .
                         "<strong>" .
                         "$image " . x($name) .
                         "</strong>" .
                         "</a> ");

                //open in browser
                else
                    echo("<a href=\"" . x($_url) . "\" " .
                            "onclick=\"load_page('" . q($_url) . "');return false;\">" .
                         "<strong>" .
                         "$image [" . t("command_show") . "]" .
                         "</strong>" .
                         "</a> ");

                echo("</td>" .
                     "<td>");
                $ifc->set(t("command_delete"), "checkbox b", TRUE, NULL, "remove[" . x($key) . "]");
                echo("</td>" .
                     "</tr>");
            };
        };

        //add attachment
        echo("<tr class=\"varied\">" .
             "<td colspan=\"2\">");
        $ifc->set(NULL,                 "file 35 :");
        $ifc->set(t("command_confirm"), "button");
        echo("</td>" .
             "</tr>");
        ifc_table_close();
    };

    echo("</td>" .
         "</tr>" .
         "</table>");

    $ifc->close();

case "create_container":
//..................................................................................................

    if (mkpath(mailbox_path($ifc_param)))
    {
        $container    = $ifc_param;
        $ifc_response = t("msg_done");
        break;
    };

    $ifc_response = t("msg_error");
    break;

case "rename_container":
//..................................................................................................

    if (rename(mailbox_path($container), mailbox_path($ifc_param)))
    {
        $container    = $ifc_param;
        $ifc_response = t("msg_done");
        break;
    };

    $ifc_response = t("msg_error");
    break;

case "delete_container":
//..................................................................................................

    $path  = mailbox_path($container);
    $_path = mailbox_path("#trashbin");

    if (
         cms_load("filemanager")
         &&
         (($hdir = opendir($path)) !== FALSE)
       )
    {
        while (($file_object = readdir($hdir)) !== FALSE)
        {
            if (($file_object === ".") || ($file_object === "..")) continue;

            //move data file to trashbin
            if (
                 is_file($path . $file_object)
                 &&
                 (substr($file_object, -4) === ".dat")
               )
                rename($path . $file_object, $_path . $file_object);
        };

        closedir($hdir);

        //delete anything left
        if (filemanager_delete($path))
        {
            $container    = "#trashbin";
            $ifc_response = t("msg_done");
            break;
        };
    };

    $ifc_response = t("msg_error");
    break;

case "train_bad":
case "train_good":
//..................................................................................................

    if (cms_load("category"))
    {
        $category = new category();

        if ($category->enabled)
        {
            $data_mailbox = new data(mailbox_path(NULL, "mailbox.dat"));
            $data_message = new data(mailbox_path($container, "$ifc_param.dat"));
            $status       = (int)$data_mailbox->get($ifc_param, CMS_MAILBOX_PROPERTY_STATUS);
            $text         = mailbox_text($data_message);

            $array = (CMS_IFC_MESSAGE === "train_bad") ?
                     [CMS_MAILBOX_STATUS_GOOD, "undo_nospam", "#spam",  CMS_MAILBOX_STATUS_BAD,  "train_spam"] :
                     [CMS_MAILBOX_STATUS_BAD,  "undo_spam",   "#inbox", CMS_MAILBOX_STATUS_GOOD, "train_nospam"];

            //undo previous training
            if (flag($status, $array[0]))
            {
                $category->{$array[1]}($text);
                $status &= ~$array[0];
            };

            //move message to target container
            if (nstreq($container, $array[2]))
                rename(mailbox_path($container, "$ifc_param.dat"),
                       mailbox_path($array[2],  "$ifc_param.dat"));

            //set status
            $status |= $array[3];
            $data_mailbox->set($status, $ifc_param, CMS_MAILBOX_PROPERTY_STATUS);
            $data_mailbox->save();

            //feed spam filter with text
            if ($category->{$array[4]}($text))
            {
                $category->update();
                $ifc_response = t("msg_done");
                break;
            };
        };
    };

    $ifc_response = t("msg_error");
    break;

case "empty_trashbin":
//..................................................................................................

    //generate trashbin object list
    $list_trashbin = [];
    $path          = mailbox_path("#trashbin");

    if (($hdir = opendir($path)) !== FALSE)
    {
        while (($file_object = readdir($hdir)) !== FALSE)
        {
            if (
                 ($file_object !== ".")  && ($file_object !== "..")
                 &&
                 is_file($path . $file_object)
                 &&
                 preg_match("/^(.+)\.dat$/", $file_object, $file_object)
               )
                $list_trashbin[$file_object[1]] = TRUE;
        };

        closedir($hdir);
    };

    if (! $list_trashbin) break; //trashbin empty

    //message counter
    $count        = 0;
    $count_remote = 0;

    //establish pop3 connection
    $pop = new pop(
        $desktop->object_get($object, "email_pop_host"),
        $desktop->object_get($object, "email_username"),
        $desktop->object_get($object, "email_password"));

    if ($pop->enabled)
    {
        //initial server response
        $response = $pop->response;

        //retrieve unique identifier list
        $list_uidl = $pop->get_unique_id_list();

        //scan for messages
        if (is_array($list_uidl))
        {
            //remove remote messages
            foreach ($list_uidl AS $key => $value)
            {
                $id = hash32($value);

                if (isset($list_trashbin[$id]))
                {
                    if ($pop->delete($key)) $count_remote++;
                    else                    unset($list_trashbin[$id]);
                };
            };
        };

        //terminate session
        $pop->quit();

        $ifc_response = t("msg_done") . x($response);
    }

    //connection failure
    else
    {
        $ifc_response = t("msg_error") . (nstre($pop->response) ? "<br>" . x($pop->response) : "");
    };

    //open message index
    $data_mailbox = new data(mailbox_path(NULL, "mailbox.dat"));

    //remove local messages
    foreach ($list_trashbin AS $key => $_)
    {
        if (unlink("$path$key.dat"))
        {
            $data_mailbox->del($key);
            $count++;
        };
    };

    //save index
    $data_mailbox->save();

    $ifc_response .= "<br>" . sprintf(t("deleted_messages_remote"), $count, $count_remote);
    break;

case "configure":
//..................................................................................................

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        TRUE,
        array_merge(
            $param,
            ["container" => $container,
             "order"     => $order]),
        "_configure",
        t("settings"));

    $ifc->set(t("name"),                      "text 40 b",       $desktop->object_get($object, "email_name"));
    $ifc->set(t("email"),                     "text 40 b",       $desktop->object_get($object, "email_address"));
    $ifc->set(t("account_name"),              "text 40 b",       $desktop->object_get($object, "email_username"));
    $ifc->set(t("password"),                  "password 40 b",   $desktop->object_get($object, "email_password"));
    $ifc->set(t("incoming_mail_server_pop3"), "text 40 b",       $desktop->object_get($object, "email_pop_host"));
    $ifc->set(t("outgoing_mail_server_smtp"), "text 40 b",       $desktop->object_get($object, "email_smtp_host"));
    $ifc->set(t("signature"),                 "textarea 40x8 b", $desktop->object_get($object, "email_signature"));
    $ifc->set(t("spam_filter_threshold"),     "label");
    $ifc->set(["95%"         => 95,
               "90%"         => 90,
               "85%"         => 85,
               "80%"         => 80,
               t("disabled") =>  0],
              "select b",
              (int)$desktop->object_get($object, "email_spam_threshold"));
    $ifc->set(t("delete_after_receipt"), "checkbox", TRUE, $desktop->object_get($object, "email_delete"));

    $ifc->close();

case "_configure":
//..................................................................................................

    $desktop->object_set($object, "email_name",           $ifc_param1);
    $desktop->object_set($object, "email_address",        $ifc_param2);
    $desktop->object_set($object, "email_username",       $ifc_param3);
    $desktop->object_set($object, "email_password",       $ifc_param4);
    $desktop->object_set($object, "email_pop_host",       $ifc_param5);
    $desktop->object_set($object, "email_smtp_host",      $ifc_param6);
    $desktop->object_set($object, "email_signature",      $ifc_param7);
    $desktop->object_set($object, "email_spam_threshold", (int)$ifc_param8);
    $desktop->object_set($object, "email_delete",         isset($ifc_param9));

    $ifc_response = $desktop->save() ? t("msg_done") : t("msg_error");
    break;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

//menu
$menu = [t("receive") . "|desktop/command_refresh"             => "retrieve",
         t("compose_message") . "|desktop/command_create"      => "compose",
         t("create_folder") . "|desktop/command_create_folder" => "javascript:desktop_mailbox_create_container();"];
if (! isset($container_name[$container]))
{
    $menu[t("rename_folder") . "|desktop/command_rename"]        = "javascript:desktop_mailbox_rename_container();";
    $menu[t("delete_folder") . "|desktop/command_delete_folder"] = "#delete_container";
};
$menu[t("empty_trash") . "|desktop/command_clean"]      = "#empty_trashbin";
$menu[t("settings") . "|desktop/command_configuration"] = "configure";

//display
$ifc = new ifc(
    $ifc_response,
    $ifc_page,
    $menu,
    array_merge(
        $param,
        ["container" => $container,
         "order"     => $order]),
    NULL,
    t("mailbox"));

?><script>

function desktop_mailbox_create_container()
{
    var value = prompt("<?php echo(t("create_folder"));?>", "");
    if (value) ifc_post("create_container", value);
};

function desktop_mailbox_rename_container()
{
    var value = prompt("<?php echo(t("rename_folder"));?>",
                       "<?php echo(q(decode_filename($container)));?>");
    if (value) ifc_post("rename_container", value);
};

function o(value)
{
    ifc_set("order", value);
    ifc_post();
};

function c(index)
{
    ifc_post("compose_edit", index);
};

function d(index)
{
    ifc_post("display", index);
};

function b(index)
{
    ifc_post("train_bad", index);
};

function g(index)
{
    ifc_post("train_good", index);
};

</script><?php

//create mailbox directory list
$path  = mailbox_path();
$array = [];

if (($hdir = opendir($path)) !== FALSE)
{
    while (($file_object = readdir($hdir)) !== FALSE)
    {
        if (
             ($file_object !== ".") && ($file_object !== "..")
             &&
             is_dir($path . $file_object)
           )
            $array[] = isset($container_name[$file_object]) ?
                       $file_object : decode_filename($file_object);
    };

    closedir($hdir);
};

$_order = array_keys($container_name);

usort($array, function($a, $b) use ($_order)
{
    $pos_a = array_search($a, $_order);
    $pos_b = array_search($b, $_order);

    if (($pos_a !== FALSE) && ($pos_b !== FALSE)) return $pos_a <=> $pos_b;

    if ($pos_a !== FALSE) return -1;
    if ($pos_b !== FALSE) return  1;

    return strnatcasecmp($a, $b);
});

$_array = [];

foreach ($array AS $value)
{
    $icon = $container_icon[$value] ?? "desktop/icon_container";
    $name = $container_name[$value] ?? x($value);
    $_array["$name|$icon"] = $value;
};

$ifc->set($_array, "select", $container, NULL, "container");

?><script>

fx_event_listen(ifc_object("container", 1), "change", () => ifc_post());

</script><?php

//helper function
$f = function($label, $title, $property, $icon) use ($order)
{
    if (streq($property, $order)) $label .= " $icon";
    return "<a href=\"javascript:o($property);\" " .
              "title=\"$title\">" .
           $label .
           "</a>";
};

//message list
ifc_table_open();
echo("<colgroup>" .
     "<col style=\"WIDTH:0\">" .
     "<col style=\"WIDTH:100px\">" .
     "<col>" .
     "<col style=\"WIDTH:0\">" .
     "</colgroup>" .

//selection
     "<tr>" .
     "<td class=\"select\">" .
     "</td>" .

//date / size
     "<th>" .
     $f(t("date"), t("sort_by_date"), CMS_MAILBOX_PROPERTY_DATE, "↘") .
     " / " .
     $f(t("size"), t("sort_by_size"), CMS_MAILBOX_PROPERTY_SIZE, "↘") .
     "</th>" .

//from / subject
     "<th>" .
     $f(t("sender"), t("sort_by_sender"), CMS_MAILBOX_PROPERTY_FROM, "↗") .
     " / " .
     $f(t("subject"), t("sort_by_subject"), CMS_MAILBOX_PROPERTY_SUBJECT, "↗") .
     "</th>" .

//spam indicator
     "<th>" .
     $f(t("spam"), t("sort_by_spam_probability"), CMS_MAILBOX_PROPERTY_SPAM_INDICATOR, "↘") .
     "</th>" .
     "</tr>");

//seek messages
$path = mailbox_path($container);

if (($hdir = opendir($path)) !== FALSE)
{
    $data_mailbox = new data(mailbox_path(NULL, "mailbox.dat"));
    $array        = [];
    $flag         = NULL;

    while (($file_object = readdir($hdir)) !== FALSE)
    {
        if (
             ($file_object !== ".") && ($file_object !== "..")
             &&
             is_file($path . $file_object)
             &&
             (substr($file_object, -4) === ".dat")
           )
        {
            $_file_object = substr($file_object, 0, -4);

            //update index if message not listed
            if (($data_mailbox->get($_file_object)) === NULL)
            {
                $data_message = new data(mailbox_path($container, "$_file_object.dat"));
                $key          = $data_message->move("first");

                //get content type
                $content_type = $data_message->get($key, "content-type") ?? "";
                $content_type = mime_extract_rfc2822_header($content_type);

                //get charset
                $charset = $content_type["charset"] ?? NULL;

                //get meta data
                $from    = $data_message->get($key, "from");
                $subject = mime_rfc2047_decode($data_message->get($key, "subject") ?? "");
                $size    = (int)$data_message->get($key, "#size");
                $status  = (int)$data_message->get($key, "#status");
                if (streq(strtolower($data_message->get($key, "importance") ?? ""), "high"))
                    $status |= CMS_MAILBOX_STATUS_IMPORTANCE_HIGH;
                if (mailbox_attachment($data_message))
                    $status |= CMS_MAILBOX_STATUS_ATTACHMENT;

                //set message index
                $data_mailbox->set(
                    [CMS_MAILBOX_PROPERTY_DATE    => filemtime($path . $file_object),
                     CMS_MAILBOX_PROPERTY_FROM    => $from,
                     CMS_MAILBOX_PROPERTY_SUBJECT => $subject,
                     CMS_MAILBOX_PROPERTY_STATUS  => $status,
                     CMS_MAILBOX_PROPERTY_SIZE    => $size],
                    $_file_object);
                $flag = TRUE;
            };

            $array[$_file_object] = $data_mailbox->get($_file_object, $order);
        };
    };

    closedir($hdir);

    //save index if updated
    if ($flag) $data_mailbox->save();

    //sort
    switch ($order)
    {
    case CMS_MAILBOX_PROPERTY_DATE:
    case CMS_MAILBOX_PROPERTY_SIZE:
    case CMS_MAILBOX_PROPERTY_SPAM_INDICATOR:

        arsort($array, SORT_NUMERIC);
        break;

    default:

        asort($array, SORT_NATURAL | SORT_FLAG_CASE);
    };

    //list messages
    foreach ($array AS $key => $_)
    {
        $date           = (int)$data_mailbox->get($key, CMS_MAILBOX_PROPERTY_DATE);
        $from           = $data_mailbox->get($key, CMS_MAILBOX_PROPERTY_FROM) ?? "";
        $subject        = stre($subject = $data_mailbox->get($key, CMS_MAILBOX_PROPERTY_SUBJECT)) ?
                          t("message") . " " . x($key) : x(strabridge($subject, 60));
        $status         = (int)$data_mailbox->get($key, CMS_MAILBOX_PROPERTY_STATUS);
        $spam_indicator = (int)$data_mailbox->get($key, CMS_MAILBOX_PROPERTY_SPAM_INDICATOR);
        $size           = (int)$data_mailbox->get($key, CMS_MAILBOX_PROPERTY_SIZE);
        $_key           = qx($key);
        $varied         = ifc_varied();

        //selection
        echo("<tr>" .
             "<td rowspan=\"2\" " .
                 "class=\"select\">");
        $ifc->set(NULL, "checkbox", $key, FALSE, "list[]");
        echo("</td>" .

        //date
             "<td$varied>" .
             x(friendly_date($date)) .
             "</td>" .

        //from
             "<td$varied>");

        $from = mime_extract_rfc2822_address($from);

        if (
             ($from = reset($from))
             &&
             (($address = key($from)) !== NULL)
           )
            echo(nstre($name = current($from)) ?
                 "<span title=\"" . x($address) . "\">" .
                 x($name) .
                 "</span>" :
                 x($address));

        echo("</td>" .

        //spam indicator
             "<td$varied>");
        if ($flag = ! flag($status, CMS_MAILBOX_STATUS_DRAFT | CMS_MAILBOX_STATUS_SENT))
             echo((($spam_indicator < 0) ? 0 : $spam_indicator) . "%");
        echo("</td>" .
             "</tr>" .

        //size
             "<tr>" .
             "<td$varied>" .
             format_bytesize($size) .
             "</td>" .

        //subject
             "<td$varied>");

        if (flag($status, CMS_MAILBOX_STATUS_SENT))
        {
            echo("<a href=\"javascript:d('$_key');\" " .
                    "class=\"br\">" .
            image("desktop/icon_message_sent"));
        }
        elseif (flag($status, CMS_MAILBOX_STATUS_DRAFT))
        {
            echo("<a href=\"javascript:c('$_key');\" " .
                    "class=\"br\">" .
            image("desktop/icon_message_draft"));
        }
        else
        {
            echo("<a href=\"javascript:d('$_key');\" " .
                    "class=\"br\">");
            if (flag($status, CMS_MAILBOX_STATUS_BAD))
                echo(image("desktop/icon_message_spam"));
            elseif (flag($status, CMS_MAILBOX_STATUS_IMPORTANCE_HIGH))
                echo(image("icon_alert"));
            else
                echo(image("desktop/icon_message"));

            if (! flag($status, CMS_MAILBOX_STATUS_READ))
                $subject = "<strong>" .
                           $subject .
                           "</strong>";
        };

        echo(" $subject");

        //display attachment
        if (flag($status, CMS_MAILBOX_STATUS_ATTACHMENT))
            echo(" <span title=\"" . t("contains_attachment") . "\">" .
                 image("desktop/icon_attachment") .
                 "</span>");

        echo("</a>" .
             "</td>" .

        //spam filter training
             "<td$varied>");
        if ($flag)
        {
            if (! flag($status, CMS_MAILBOX_STATUS_BAD))
                echo("<a href=\"javascript:b('$_key');\" " .
                        "title=\"" . t("train_as_spam") . "\">" .
                     image("desktop/icon_spam") .
                     "</a>");

            if (! flag($status, CMS_MAILBOX_STATUS_GOOD))
                echo("<a href=\"javascript:g('$_key');\" " .
                        "title=\"" . t("train_as_normal") . "\">" .
                     image("desktop/icon_accept") .
                     "</a>");
        };

        echo("</td>" .
             "</tr>");
    };
};

//selection control
echo("<tr>" .
     "<td colspan=\"7\" " .
         "class=\"select\" " .
         "style=\"TEXT-ALIGN:right\">");
$ifc->set(t("all"),           "button", "javascript:ifc_list_activate();");
$ifc->set(t("invert"),        "button", "javascript:ifc_list_invert();");
$ifc->set(t("none"),          "button", "javascript:ifc_list_deactivate();");
$ifc->set(t("move_to") . ":", "button", "move");
$ifc->set($_array,            "select", "#trashbin", NULL, "target");
echo("</td>" .
     "</tr>");
ifc_table_close();

$ifc->close();