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

function mailbox_directory()
{
    global $object,
           $container_name;

    $array = [];
    $path  = DESKTOP_PATH . "$object/";

    if (! $hdir = opendir($path)) return $array;

    while ($file_object = readdir($hdir))
    {
        if (
             ($file_object !== ".") && ($file_object !== "..")
             &&
             is_dir($path . $file_object)
           )
        {
            $name         = isset($container_name[$file_object]) ?
                            $container_name[$file_object] :
                            x(rawurldecode($file_object));
            $array[$name] = $file_object;
        };
    };

    closedir($hdir);

    return $array;
};

//--------------------------------------------------------------------------------------------------

function mailbox_convert($value,
                         $charset = NULL)
{
    if (! utf8_detect($value)) //not utf-8
        $value = $charset ?
                 utf8_convert($value, $charset) : //convert charset to utf-8
                 utf8_convert($value);            //convert iso-8859-1 (default) to utf-8

    return utf8_normalize($value); //normalize
};

//--------------------------------------------------------------------------------------------------

function mailbox_text(&$data)
{
    $text = "";

    $data->move("first");
    while ($key = $data->move("next"))
    {
        //get content type
        $content_type       = $data->get($key, "content-type");
        $content_type       = mime_extract_rfc2822_header($content_type);
        $content_type_value = $content_type["#value"] ?
                              strtolower($content_type["#value"]) :
                              "text/plain";

        //get charset
        $charset = isset($content_type["charset"]) ?
                   $content_type["charset"] : NULL;

        //get subject
        $subject  = $data->get($key, "subject");
        $subject  = mailbox_convert($subject, $charset);
        $text    .= "$subject ";

        //get body
        if (preg_match("/^text(?:\/|$)/", $content_type_value))
        {
            $body = $data->get($key, "#body");
            $body = mailbox_convert($body, $charset);

            //remove html markup
            if (streq($content_type_value, "text/html"))
                $body = htmltoplain($body);

            $text .= "$body ";
        };
    };

    return $text;
};

//--------------------------------------------------------------------------------------------------

function mailbox_attachment(&$data)
{
    $data->move("first");
    while ($key = $data->move("next"))
    {
        //get content type
        $content_type       = $data->get($key, "content-type");
        $content_type       = mime_extract_rfc2822_header($content_type);
        $content_type_value = $content_type["#value"] ?
                              strtolower($content_type["#value"]) :
                              "text/plain";

        //embedded message
        if (streq($content_type_value, "message/rfc822")) return TRUE;

        //no multipart frame or text
        if (! preg_match("/^(?:multipart|text)(?:\/|$)/", $content_type_value)) return TRUE;
    };

    return FALSE;
};

if (
     (! cms_load("pop"))
     ||
     (! cms_load("smtp"))
     ||
     (! cms_load("mime"))
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
    ["#inbox"    => CMS_L_DESKTOP_MAILBOX_001,
     "#draft"    => CMS_L_DESKTOP_MAILBOX_003,
     "#outbox"   => CMS_L_DESKTOP_MAILBOX_002,
     "#trashbin" => CMS_L_DESKTOP_MAILBOX_004,
     "#spam"     => CMS_L_DESKTOP_MAILBOX_005];

//verify predefined containers
$path = DESKTOP_PATH . "$object/";

foreach ($container_name AS $key => $_)
{
    if (! mkpath("$path$key/")) ifc_inactive($ifc_page);
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
        $ifc_response = CMS_MSG_ERROR . (nstre($pop->response) ? "<br>" . x($pop->response) : "");
        break;
    };

    //initial server response
    $response = $pop->response;

    //retrieve unique identifier list
    $list_uidl = $pop->get_unique_id_list();

    //message counter
    $count      = 0;
    $count_spam = 0;

    //scan for messages
    if (is_array($list_uidl))
    {
        //open message index
        $data_mailbox   = new data(DESKTOP_PATH . "$object/mailbox.dat");

        //instantiate spam filter
        $category       = cms_load("category") ? new category() : NULL;
        $spam_threshold = (int)$desktop->object_get($object, "email_spam_threshold");

        //delete message from server flag
        $flag_delete    = (bool)$desktop->object_get($object, "email_delete");

        //get time
        $time           = explode(" ", microtime());
        $time           = (float)$time[0] + (float)$time[1];

        foreach ($list_uidl AS $key => $value)
        {
            $id = hash32($value);

            //receive message
            if (
                 (! $data_mailbox->get($id))
                 &&
                 ($data_message = $pop->get_message($key))
               )
            {
                //delete message from server
                if ($flag_delete) $pop->delete($key);

                $key            = $data_message->move("first");
                $container      = "#inbox";
                $spam_indicator = NULL;
                $count++;

                //get content type
                $content_type   = $data_message->get($key, "content-type");
                $content_type   = mime_extract_rfc2822_header($content_type);

                //get charset
                $charset        = isset($content_type["charset"]) ?
                                  $content_type["charset"] : NULL;

                //meta data
                $from           = mailbox_convert($data_message->get($key, "from"),    $charset);
                $subject        = mailbox_convert($data_message->get($key, "subject"), $charset);
                $size           = $data_message->get($key, "#size");
                $status         = streq($data_message->get($key, "importance"), "high") ?
                                  CMS_MAILBOX_STATUS_IMPORTANCE_HIGH :
                                  CMS_MAILBOX_STATUS_NONE;
                if (mailbox_attachment($data_message)) $status |= CMS_MAILBOX_STATUS_ATTACHMENT;

                //apply spam filter
                if ($category && $category->enabled)
                {
                    $text           = mailbox_text($data_message);
                    $spam_indicator = $category->evaluate_spam($text);

                    //spam
                    if ($spam_threshold && ($spam_indicator >= $spam_threshold))
                    {
                        $container = "#spam";
                        $count_spam++;
                    }

                    //no spam
                    elseif ($category->train_nospam($text))
                    {
                        $status |= CMS_MAILBOX_STATUS_GOOD;
                    };
                };

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

                //save message
                $data_message->save(DESKTOP_PATH . "$object/$container/$id.dat");
            };
        };

        //save index
        $data_mailbox->save();

        //update spam filter
        if ($category) $category->update();
    };

    //terminate session
    $pop->quit();

    $container    = "#inbox";
    $ifc_response = CMS_MSG_DONE .
                    (nstre($response) ? x($response) . "<br>" : "") .
                    sprintf(CMS_L_DESKTOP_MAILBOX_006, $count, $count_spam);
    break;

case "display":
case "_display":
//..................................................................................................

    //open message
    $path         = DESKTOP_PATH . "$object/$container/$ifc_param.dat";
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
            [CMS_L_DESKTOP_MAILBOX_056  . "|desktop/command_reply"
             => "javascript:ifc_post('compose_reply','$_ifc_param');",
             CMS_L_DESKTOP_MAILBOX_057 .  "|desktop/command_relay"
             => "javascript:ifc_post('compose_relay','$_ifc_param');",
             CMS_L_COMMAND_CLOSE .        "|desktop/command_cancel"
             => NULL],
            array_merge(
                $param,
                ["container" => $container,
                 "order"     => $order]),
            NULL,
            CMS_L_DESKTOP_MAILBOX_030);

        ifc_table_open();

        $flag = TRUE; //display header flag

        $data_message->move("first");
        while ($key = $data_message->move("next"))
        {
            //get content type
            $content_type           = $data_message->get($key, "content-type");
            $content_type           = mime_extract_rfc2822_header($content_type);
            $content_type["#value"] = nstre($content_type["#value"]) ?
                                      strtolower($content_type["#value"]) :
                                      "text/plain";

            //get charset
            $charset = $content_type["charset"] ?? NULL;

            //split mime type
            preg_match("/^([^\/]+)(?:\/(.+))?$/",
                       $content_type["#value"],
                       $content_type_value);

            $content_type_subvalue = $content_type_value[2];
            $content_type_value    = $content_type_value[1];

            //display message header
            if ($flag)
            {
                //subject
                $subject = ($subject = $data_message->get($key, "subject")) ?
                           x(mailbox_convert($subject, $charset)) :
                           CMS_L_DESKTOP_MAILBOX_007 . " " . x($ifc_param);

                echo("<tr>" .
                     "<th colspan=\"2\" " .
                         "class=\"br\">" .
                     image("desktop/icon_mailbox") . " $subject" .
                     "</th>" .
                     "</tr>");

                //from
                if ($from = $data_message->get($key, "from"))
                    echo("<tr>" .
                         "<td class=\"varied\" " .
                             "style=\"WIDTH:0\">" .
                         CMS_L_DESKTOP_MAILBOX_008 . ":" .
                         "</td>" .
                         "<td class=\"br\">" .
                         x(mailbox_convert($from, $charset)) .
                         "</td>" .
                         "</tr>");

                //date
                if ($date = $data_message->get($key, "date"))
                {
                    if (($_date = strtotime($date)) > 0)
                        $date = friendly_date($_date);

                    echo("<tr>" .
                         "<td class=\"varied\" " .
                             "style=\"WIDTH:0\">" .
                         CMS_L_DESKTOP_MAILBOX_010 . ":" .
                         "</td>" .
                         "<td class=\"br\">" .
                         x($date) .
                         "</td>" .
                         "</tr>");
                };

                //to
                if ($to = $data_message->get($key, "to"))
                    echo("<tr>" .
                         "<td class=\"varied\" " .
                             "style=\"WIDTH:0\">" .
                         CMS_L_DESKTOP_MAILBOX_011 . ":" .
                         "</td>" .
                         "<td class=\"br\">" .
                         x(mailbox_convert($to, $charset)) .
                         "</td>" .
                         "</tr>");

                //cc
                if ($cc = $data_message->get($key, "cc"))
                    echo("<tr>" .
                         "<td class=\"varied\" " .
                             "style=\"WIDTH:0\">" .
                         CMS_L_DESKTOP_MAILBOX_012 . ":" .
                         "</td>" .
                         "<td class=\"br\">" .
                         x(mailbox_convert($cc, $charset)) .
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

                echo("<tr>" .
                     "<td colspan=\"2\">");
                ifc_table_open();
                continue 2;

            case "/container":
//..................................................................................................

                ifc_table_close();
                echo("</td>" .
                     "</tr>");
                continue 2;
            };

            //display message body
            echo("<tr class=\"varied\">" .
                 "<td colspan=\"2\">");

            //icon
            $image = image(get_mime_icon($content_type["#value"]));

            $_url = str_replace("%replace%", r($key), $url);

            //file download
            if (isset($content_type["name"]) && nstre($name = $content_type["name"]))
                echo("<a href=\"" . x($_url) . "\">" .
                     "<strong>" .
                     "$image " . x(mailbox_convert($name, $charset)) .
                     "</strong>" .
                     "</a> ");

            //open in browser
            else
                echo("<a href=\"" . x($_url) . "\" " .
                        "onclick=\"load_page('" . q($_url) . "');return false;\">" .
                     "<strong>" .
                     "$image " . CMS_L_COMMAND_SHOW .
                     "</strong>" .
                     "</a> ");

            //size
            echo(sprintf(CMS_L_DESKTOP_MAILBOX_013,
                         x($content_type["#value"]),
                         format_bytesize($data_message->get($key, "#size"))) .
                 "</td>" .
                 "</tr>");

            //preview text
            switch ($content_type_value)
            {
            case "text":
//..................................................................................................

                $body = $data_message->get($key, "#body");
                $body = mailbox_convert($body, $charset);

                //clean up text
                $body = streq($content_type_subvalue, "html") ?
                        htmltoplain($body, TRUE) :
                        stripspaces($body, TRUE);

                //display text
                if ($body)
                    echo("<tr>" .
                         "<td colspan=\"2\">" .
                         "<div class=\"text\">" .
                         x($body) .
                         "</div>" .
                         "</td>" .
                         "</tr>");
            };
        };

        ifc_table_close();

        //open message index
        $data_mailbox = new data(DESKTOP_PATH . "$object/mailbox.dat");

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
        $content_type            = $data_message->get($part, "content-type");
        $_content_type           = mime_extract_rfc2822_header($content_type);
        $_content_type["#value"] = $_content_type["#value"] ?
                                   strtolower($_content_type["#value"]) :
                                   "text/plain";

        //split mime type
        preg_match("/^([^\/]+)(?:\/(.+))?$/",
                   $_content_type["#value"],
                   $content_type_value);

        $content_type_subvalue = $content_type_value[2];
        $content_type_value    = $content_type_value[1];

        //get body
        $body = $data_message->get($part, "#body");

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
                               $data_message->get($key, "content-id"),
                               $array))
                    $translation["cid:" . $array[1]] = $_url;

                if ($content_location = $data_message->get($key, "content-location"))
                    $translation[$content_location]  = $_url;
            };

            //replace attachment placeholders with actual url
            $body = strtr($body, $translation);
        };

        //etag check
        $mod_time = filemtime($path);
        $filesize = strlen($body);
        $etag     = "\"" . $mod_time . "." . $filesize . "\"";

        if (strpos($_SERVER["HTTP_IF_NONE_MATCH"] ?? "", $etag) !== FALSE)
        {
            http_response_code(304); //not modified
            exit(); //don't send body
        };

        //http header
        header(nstre($name = ($_content_type["name"] ?? "")) ?
               "Content-Disposition: attachment; filename=\"" . addcslashes($name, "\"\\") . "\"" :
               "Content-Disposition: inline; filename=\"download\"");
        header("Content-Length: " . $filesize);
        header("Content-Type: $content_type");
        header("Last-Modified: " . gmdate("D, d M Y H:i:s", $mod_time) . " GMT");
        header("ETag: $etag");

        //transmit part
        echo($body);
        exit();
    };

case "move":
//..................................................................................................

    if (empty($list)) break; //no selection

    if (nstreq($container, $target))
    {
        $path = DESKTOP_PATH . "$object/";

        foreach ($list AS $value)
            rename("$path$container/$value.dat", "$path$target/$value.dat");
    };

    $ifc_response = CMS_MSG_DONE;
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
        $mime    = new mime(DESKTOP_PATH . "$object/#draft/$message.dat");

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
            NULL,                   //subject
            $ifc_param2,            //body
            "plain",                //subtype
            CMS_MIME_CHARSET_UTF_8, //charset
            $key);                  //parent_key
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
        $mime    = new mime(DESKTOP_PATH . "$object/$container/$message.dat");

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
        $data_message = new data(DESKTOP_PATH . "$object/$container/$ifc_param.dat");

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
            $content_type           = $data_message->get($key, "content-type");
            $content_type           = mime_extract_rfc2822_header($content_type);
            $content_type["#value"] = nstre($content_type["#value"]) ?
                                      strtolower($content_type["#value"]) :
                                      "text/plain";

            //get charset
            $charset = $content_type["charset"] ?? NULL;

            //get reply subject
            if (stre($ifc_param1))
            {
                $ifc_param1 = $data_message->get($key, "subject");
                $ifc_param1 = mailbox_convert($ifc_param1, $charset);
                $ifc_param1 = CMS_L_DESKTOP_MAILBOX_058 . ": $ifc_param1";
            };

            //split mime type
            preg_match("/^([^\\/]+)(?:\\/(.+))?$/",
                       $content_type["#value"],
                       $content_type_value);

            $content_type_subvalue = $content_type_value[2];
            $content_type_value    = $content_type_value[1];

            //process nesting
            switch ($data_message->get($key, "#type"))
            {
            case "container":

                if ($depth || streq($content_type_value, "message")) $depth++;
                break;

            case "/container":

                if ($depth) $depth--;
            };

            //find first text body that is not an attachment
            //or located within an encapsulated message
            if ((! $depth) && streq($content_type_value, "text"))
            {
                $content_disposition = $data_message->get($key, "content-disposition");
                $content_disposition = mime_extract_rfc2822_header($content_disposition);

                if (strcasecmp($content_disposition["#value"], "attachment"))
                {
                    $body = $data_message->get($key, "#body");
                    $body = mailbox_convert($body, $charset);

                    switch ($content_type_subvalue)
                    {
                    default:
                    case "plain":

                        $ifc_param2 = stripspaces($body, TRUE);
                        break 2;

                    case "html":

                        $ifc_param2 = htmltoplain($body, TRUE);
                        break 2;
                    };
                };
            };
        };

        //message text
        $ifc_param2 = "\n\n\n" . $desktop->object_get($object, "email_signature") .
                      "\n\n" . CMS_L_DESKTOP_MAILBOX_054 .
                      "\n\n$ifc_param2";

        //create draft
        $message = unique_id();
        $mime    = new mime(DESKTOP_PATH . "$object/#draft/$message.dat");

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
            NULL,                   //subject
            $ifc_param2,            //body
            "plain",                //subtype
            CMS_MIME_CHARSET_UTF_8, //charset
            $key);                  //parent_key
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
        $data_message = new data(DESKTOP_PATH . "$object/$container/$ifc_param.dat");
        $key          = $data_message->move("first");

        //get content type
        $content_type = $data_message->get($key, "content-type");
        $content_type = mime_extract_rfc2822_header($content_type);

        //get charset
        $charset = isset($content_type["charset"]) ?? NULL;

        //get reply subject
        if (stre($ifc_param1))
        {
            $ifc_param1 = $data_message->get($key, "subject");
            $ifc_param1 = mailbox_convert($ifc_param1, $charset);
            $ifc_param1 = CMS_L_DESKTOP_MAILBOX_059 . ": $ifc_param1";
        };

        //create draft
        $message = unique_id();
        $mime    = new mime(DESKTOP_PATH . "$object/#draft/$message.dat");

        //create multipart frame
        $key  = $mime->add_multipart();
        $from = $desktop->object_get($object, "email_address");
        $name = $desktop->object_get($object, "email_name");
        if (! $name) $name = CMS_NAME;
        $name = addcslashes($name, "\"\\");
        $mime->data->set("\"$name\" <$from>",      $key, "from");
        $mime->data->set($draft_type,              $key, "#draft_type");
        $mime->data->set(CMS_MAILBOX_STATUS_DRAFT, $key, "#status");

        //attach relay message
        $mime->add_message("rfc822", $data_message, $key);

        //create text body
        $key = $mime->add_text(
            NULL,                   //subject
            $ifc_param2,            //body
            "plain",                //subtype
            CMS_MIME_CHARSET_UTF_8, //charset
            $key);                  //parent_key
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
        $mime = new mime(DESKTOP_PATH . "$object/$container/$message.dat");

        //get draft type
        $key        = $mime->data->move("first");
        $draft_type = $mime->data->get($key, "#draft_type");

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

        if ($draft_type != CMS_MAILBOX_DRAFT_TYPE_RELAY)
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
            $value  = strlen($mime->data->get($_key, "#body"));
            $size  += $value;
            $mime->data->set($value, $_key, "#size");
        };

        $mime->data->set($size, $key, "#size");

        //save draft
        $mime->data->save();

        //open message index
        $data_mailbox = new data(DESKTOP_PATH . "$object/mailbox.dat");

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
                $ifc_response = CMS_MSG_ERROR . (nstre($smtp->response) ? "<br>" . x($smtp->response) : "");
                break;
            };

            if (! $smtp->send($mime))
            {
                $ifc_response = CMS_MSG_ERROR . (nstre($smtp->response) ? "<br>" . x($smtp->response) : "");
                $smtp->quit();
                break;
            };

            //close smtp connection
            $smtp->quit();

            //set message status
            $key = $mime->data->move("first");
            $mime->data->set(CMS_MAILBOX_STATUS_SENT, $key, "#status");

            //remove draft settings
            while ($key = $mime->data->move("next"))
            {
                $mime->data->del($key, "#draft_type");
                $mime->data->del($key, "#draft_option");
            };

            //save message
            $mime->save();

            //move message to outbox
            $path = DESKTOP_PATH . "$object/";
            rename("$path$container/$message.dat", "$path#outbox/$message.dat");

            //set container
            $container = "#outbox";

            $ifc_response = CMS_MSG_DONE;
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
        [CMS_L_DESKTOP_MAILBOX_025 . "|desktop/command_apply"  => "compose_send",
         CMS_L_COMMAND_SAVE .        "|desktop/command_save"   => "compose_save",
         CMS_L_COMMAND_CANCEL .      "|desktop/command_cancel" => FALSE],
        array_merge(
            $param,
            ["container" => $container,
             "message"   => $message,
             "order"     => $order]),
        "_compose",
        CMS_L_DESKTOP_MAILBOX_014);

    //generate address list
    $list_address = [];

    $desktop->data->move("first");
    while($key = $desktop->data->move("next"))
    {
        if (
             streq($desktop->object_type($key), "address")
             &&
             ($email = (string)$desktop->object_get($key, "email"))
           )
        {
            $name    = (string)$desktop->object_get($key, "name");
            $company = (string)$desktop->object_get($key, "company");
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
    ifc_tab_open(CMS_L_DESKTOP_MAILBOX_011);
    echo("<div class=\"fullwidth\">" .
         "<input name=\"to\" " .
                "type=\"email\" " .
                "value=\"" . x($to) . "\" " .
                "list=\"contact\" " .
                "multiple>" .
         "</div>");

    //cc
    ifc_tab_next(CMS_L_DESKTOP_MAILBOX_015);
    echo("<div class=\"fullwidth\">" .
         "<input name=\"cc\" " .
                "type=\"email\" " .
                "value=\"" . x($cc) . "\" " .
                "list=\"contact\" " .
                "multiple>" .
         "</div>");

    //bcc
    ifc_tab_next(CMS_L_DESKTOP_MAILBOX_016);
    echo("<div class=\"fullwidth\">" .
         "<input name=\"bcc\" " .
                "type=\"email\" " .
                "value=\"" . x($bcc) . "\" " .
                "list=\"contact\" " .
                "multiple>" .
         "</div>");
    ifc_tab_close();

    //subject
    $ifc->set(CMS_L_DESKTOP_MAILBOX_017, "text 0 w",        $ifc_param1);

    //body
    $ifc->set(CMS_L_DESKTOP_MAILBOX_007, "textarea 0x20 w", $ifc_param2);

    //attachments
    if ($draft_type == CMS_MAILBOX_DRAFT_TYPE_RELAY)
    {
        $ifc->set(CMS_L_DESKTOP_MAILBOX_055, "info");
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
             CMS_L_DESKTOP_MAILBOX_053 .
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
                $content_type           = $mime->data->get($key, "content-type");
                $content_type           = mime_extract_rfc2822_header($content_type);
                $content_type["#value"] = $content_type["#value"] ?
                                          strtolower($content_type["#value"]) :
                                          "text/plain";

                //get charset
                $charset = isset($content_type["charset"]) ?
                           $content_type["charset"] : NULL;

                //icon
                $image = image(get_mime_icon($content_type["#value"]));

                //display attachment
                echo("<tr>" .
                     "<td>");

                //attachment url
                $_url = str_replace("%replace%", r($key), $url);

                //file download
                if (isset($content_type["name"]) && ($name = $content_type["name"]))
                    echo("<a href=\"" . x($_url) . "\">" .
                         "<strong>" .
                         "$image " . x(mailbox_convert($name, $charset)) .
                         "</strong>" .
                         "</a> ");

                //open in browser
                else
                    echo("<a href=\"" . x($_url) . "\" " .
                            "onclick=\"load_page('" . q($_url) . "');return false;\">" .
                         "<strong>" .
                         "$image [" . CMS_L_COMMAND_SHOW . "]" .
                         "</strong>" .
                         "</a> ");

                echo("</td>" .
                     "<td>");
                $ifc->set(CMS_L_COMMAND_DELETE, "checkbox b", TRUE, NULL, "remove[" . x($key) . "]");
                echo("</td>" .
                     "</tr>");
            };
        };

        //add attachment
        echo("<tr class=\"varied\">" .
             "<td colspan=\"2\">");
        $ifc->set(NULL,                  "file 35 :");
        $ifc->set(CMS_L_COMMAND_CONFIRM, "button");
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

    $ifc_param = r($ifc_param);
    $path      = DESKTOP_PATH . "$object/$ifc_param/";

    if (mkpath($path))
    {
        $container    = $ifc_param;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "rename_container":
//..................................................................................................

    $ifc_param = r($ifc_param);
    $path      = DESKTOP_PATH . "$object/";

    if (rename($path . $container, $path . $ifc_param))
    {
        $container    = $ifc_param;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "delete_container":
//..................................................................................................

    $path  = DESKTOP_PATH . "$object/$container/";
    $_path = DESKTOP_PATH . "$object/#trashbin/";

    if (
         cms_load("filemanager")
         &&
         ($hdir = opendir($path))
       )
    {
        while ($file_object = readdir($hdir))
        {
            if (($file_object === ".") || ($file_object === "..")) continue;

            //move data file to trashbin
            if (
                 is_file($path . $file_object)
                 &&
                 preg_match("/\.dat$/", $file_object)
               )
                rename($path . $file_object, $_path . $file_object);

            //or delete file object
            else
                filemanager_delete($path . $file_object);
        };

        closedir($hdir);

        if (rmdir($path))
        {
            $container    = "#trashbin";
            $ifc_response = CMS_MSG_DONE;
            break;
        };
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "train_bad":
//..................................................................................................

    if (cms_load("category"))
    {
        $category = new category();

        if ($category->enabled)
        {
            $path         = DESKTOP_PATH . "$object/";
            $data_mailbox = new data($path . "mailbox.dat");
            $data_message = new data("$path$container/$ifc_param.dat");
            $status       = $data_mailbox->get($ifc_param, CMS_MAILBOX_PROPERTY_STATUS);
            $text         = mailbox_text($data_message);

            //undo previous training
            if (flag($status, CMS_MAILBOX_STATUS_GOOD))
            {
                $category->undo_nospam($text);
                $status &= ~CMS_MAILBOX_STATUS_GOOD;
            };

            //move message to spam container
            if (nstreq($container, "#spam"))
                rename("$path$container/$ifc_param.dat", "$path#spam/$ifc_param.dat");

            //set status
            $status |= CMS_MAILBOX_STATUS_BAD;
            $data_mailbox->set($status, $ifc_param, CMS_MAILBOX_PROPERTY_STATUS);
            $data_mailbox->save();

            //feed spam filter with bad text
            if ($category->train_spam($text))
            {
                $category->update();
                $ifc_response = CMS_MSG_DONE;
                break;
            };
        };
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "train_good":
//..................................................................................................

    if (cms_load("category"))
    {
        $category = new category();

        if ($category->enabled)
        {
            $path         = DESKTOP_PATH . "$object/";
            $data_mailbox = new data($path . "mailbox.dat");
            $data_message = new data("$path$container/$ifc_param.dat");
            $status       = $data_mailbox->get($ifc_param, CMS_MAILBOX_PROPERTY_STATUS);
            $text         = mailbox_text($data_message);

            //undo previous training
            if (flag($status, CMS_MAILBOX_STATUS_BAD))
            {
                $category->undo_spam($text);
                $status &= ~CMS_MAILBOX_STATUS_BAD;
            };

            //move message from spam container
            if (streq($container, "#spam"))
                rename("$path$container/$ifc_param.dat", "$path#inbox/$ifc_param.dat");

            //set status
            $status |= CMS_MAILBOX_STATUS_GOOD;
            $data_mailbox->set($status, $ifc_param, CMS_MAILBOX_PROPERTY_STATUS);
            $data_mailbox->save();

            //feed spam filter with good text
            if ($category->train_nospam($text))
            {
                $category->update();
                $ifc_response = CMS_MSG_DONE;
                break;
            };
        };
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "empty_trashbin":
//..................................................................................................

    //generate trashbin object list
    $list_trashbin = [];
    $path          = DESKTOP_PATH . "$object/#trashbin/";

    if ($hdir = opendir($path))
    {
        while ($file_object = readdir($hdir))
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

    if (! count($list_trashbin)) break; //trashbin empty

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
        $response  = $pop->response;

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

        $ifc_response = CMS_MSG_DONE . x($response);
    }

    //connection failure
    else
    {
        $ifc_response = CMS_MSG_ERROR . (nstre($pop->response) ? "<br>" . x($pop->response) : "");
    };

    //open message index
    $data_mailbox = new data(DESKTOP_PATH . "$object/mailbox.dat");

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

    $ifc_response .= "<br>" . sprintf(CMS_L_DESKTOP_MAILBOX_060, $count, $count_remote);
    break;

case "configure":
//..................................................................................................

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   array_merge(
                       $param,
                       ["container" => $container,
                        "order"     => $order]),
                   "_configure",
                   CMS_L_DESKTOP_MAILBOX_029);

    $ifc->set(CMS_L_NAME,                "text 40 b",       $desktop->object_get($object, "email_name"));
    $ifc->set(CMS_L_EMAIL,               "text 40 b",       $desktop->object_get($object, "email_address"));
    $ifc->set(CMS_L_DESKTOP_MAILBOX_018, "text 40 b",       $desktop->object_get($object, "email_username"));
    $ifc->set(CMS_L_PASSWORD,            "password 40 b",   $desktop->object_get($object, "email_password"));
    $ifc->set(CMS_L_DESKTOP_MAILBOX_019, "text 40 b",       $desktop->object_get($object, "email_pop_host"));
    $ifc->set(CMS_L_DESKTOP_MAILBOX_020, "text 40 b",       $desktop->object_get($object, "email_smtp_host"));
    $ifc->set(CMS_L_DESKTOP_MAILBOX_021, "textarea 40x8 b", $desktop->object_get($object, "email_signature"));
    $ifc->set(CMS_L_DESKTOP_MAILBOX_022, "label");
    $ifc->set(["100%"                    => 100,
               "99%"                     =>  99,
               "98%"                     =>  98,
               "97%"                     =>  97,
               "96%"                     =>  96,
               "95%"                     =>  95,
               "94%"                     =>  94,
               "93%"                     =>  93,
               "92%"                     =>  92,
               "91%"                     =>  91,
               "90%"                     =>  90,
               "85%"                     =>  85,
               "80%"                     =>  80,
               "70%"                     =>  70,
               "60%"                     =>  60,
               "50%"                     =>  50,
               "40%"                     =>  40,
               "30%"                     =>  30,
               "20%"                     =>  20,
               "10%"                     =>  10,
               CMS_L_DESKTOP_MAILBOX_023 =>   0],
              "select b",
              (int)$desktop->object_get($object, "email_spam_threshold"));
    $ifc->set(CMS_L_DESKTOP_MAILBOX_051, "checkbox", TRUE, $desktop->object_get($object, "email_delete"));

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

    $ifc_response = $desktop->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

//menu
$menu = [CMS_L_DESKTOP_MAILBOX_024 . "|desktop/command_refresh"        => "retrieve",
         CMS_L_DESKTOP_MAILBOX_014 . "|desktop/command_create"         => "compose",
         CMS_L_DESKTOP_MAILBOX_026 . "|desktop/command_create_folder"  => "javascript:desktop_mailbox_create_container();"];
if (! isset($container_name[$container]))
{
    $menu[CMS_L_DESKTOP_MAILBOX_027 . "|desktop/command_rename"]        = "javascript:desktop_mailbox_rename_container();";
    $menu[CMS_L_DESKTOP_MAILBOX_028 . "|desktop/command_delete_folder"] = "#delete_container";
};
$menu[CMS_L_DESKTOP_MAILBOX_050 . "|desktop/command_clean"]             = "#empty_trashbin";
$menu[CMS_L_DESKTOP_MAILBOX_029 . "|desktop/command_configuration"]     = "configure";

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
    CMS_L_DESKTOP_MAILBOX_031);

?><script>

function desktop_mailbox_create_container()
{
    var value = prompt("<?php echo(CMS_L_DESKTOP_MAILBOX_026);?>", "");
    if (value) ifc_post("create_container", value);
};

function desktop_mailbox_rename_container()
{
    var value = prompt("<?php echo(CMS_L_DESKTOP_MAILBOX_027);?>",
                       "<?php echo(q(rawurldecode($container)));?>");
    if (value) ifc_post("rename_container", value);
};

function o(value)
{
    ifc_set("order", value);
    ifc_submit();
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
$path  = DESKTOP_PATH . "$object/";
$array = [];

if ($hdir = opendir($path))
{
    while ($file_object = readdir($hdir))
    {
        if (
             ($file_object !== ".") && ($file_object !== "..")
             &&
             is_dir($path . $file_object)
           )
            $array[] = $file_object;
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
    $name = $container_name[$value] ?? x(rawurldecode($value));
    $_array["$name|$icon"] = $value;
};

$ifc->set($_array, "select", $container, NULL, "container");

?><script>

fx_event_listen(ifc_object("container", 1), "change", () => ifc_submit());

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
     $f(CMS_L_DESKTOP_MAILBOX_037, CMS_L_DESKTOP_MAILBOX_038, CMS_MAILBOX_PROPERTY_DATE, "↘") .
     " / " .
     $f(CMS_L_DESKTOP_MAILBOX_043, CMS_L_DESKTOP_MAILBOX_044, CMS_MAILBOX_PROPERTY_SIZE, "↘") .
     "</th>" .

//from / subject
     "<th>" .
     $f(CMS_L_DESKTOP_MAILBOX_039, CMS_L_DESKTOP_MAILBOX_040, CMS_MAILBOX_PROPERTY_FROM, "↗") .
     " / " .
     $f(CMS_L_DESKTOP_MAILBOX_045, CMS_L_DESKTOP_MAILBOX_046, CMS_MAILBOX_PROPERTY_SUBJECT, "↗") .
     "</th>" .

//spam indicator
     "<th>" .
     $f(CMS_L_DESKTOP_MAILBOX_041, CMS_L_DESKTOP_MAILBOX_042, CMS_MAILBOX_PROPERTY_SPAM_INDICATOR, "↘") .
     "</th>" .
     "</tr>");

//seek messages
$path = DESKTOP_PATH . "$object/$container/";

if ($hdir = opendir($path));
{
    $data_mailbox = new data(DESKTOP_PATH . "$object/mailbox.dat");
    $array        = [];
    $flag         = NULL;

    while ($file_object = readdir($hdir))
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
            if (! $data_mailbox->get($_file_object))
            {
                $_path        = DESKTOP_PATH . "$object/$container/";
                $data_message = new data("$_path$_file_object.dat");
                $key          = $data_message->move("first");

                //get content type
                $content_type = $data_message->get($key, "content-type");
                $content_type = mime_extract_rfc2822_header($content_type);

                //get charset
                $charset      = isset($content_type["charset"]) ?
                                $content_type["charset"] : NULL;

                //get meta data
                $from         = mailbox_convert($data_message->get($key, "from"),    $charset);
                $subject      = mailbox_convert($data_message->get($key, "subject"), $charset);
                $size         = $data_message->get($key, "#size");
                $status       = $data_message->get($key, "#status");
                if (streq($data_message->get($key, "importance"), "high")) $status |= CMS_MAILBOX_STATUS_IMPORTANCE_HIGH;
                if (mailbox_attachment($data_message))                     $status |= CMS_MAILBOX_STATUS_ATTACHMENT;

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
        $date           = $data_mailbox->get($key, CMS_MAILBOX_PROPERTY_DATE);
        $from           = $data_mailbox->get($key, CMS_MAILBOX_PROPERTY_FROM);
        $subject        = ($subject = $data_mailbox->get($key, CMS_MAILBOX_PROPERTY_SUBJECT)) ?
                          x(strabridge($subject, 60)) : CMS_L_DESKTOP_MAILBOX_007 . " " . x($key);
        $status         = $data_mailbox->get($key, CMS_MAILBOX_PROPERTY_STATUS);
        $spam_indicator = (int)$data_mailbox->get($key, CMS_MAILBOX_PROPERTY_SPAM_INDICATOR);
        $size           = $data_mailbox->get($key, CMS_MAILBOX_PROPERTY_SIZE);
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
            echo(" <span title=\"" . CMS_L_DESKTOP_MAILBOX_061 . "\">" .
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
                        "title=\"" . CMS_L_DESKTOP_MAILBOX_048 . "\">" .
                     image("desktop/icon_spam") .
                     "</a>");

            if (! flag($status, CMS_MAILBOX_STATUS_GOOD))
                echo("<a href=\"javascript:g('$_key');\" " .
                        "title=\"" . CMS_L_DESKTOP_MAILBOX_049 . "\">" .
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
$ifc->set(CMS_L_ALL,                       "button", "javascript:ifc_list_activate();");
$ifc->set(CMS_L_INVERT,                    "button", "javascript:ifc_list_invert();");
$ifc->set(CMS_L_NONE,                      "button", "javascript:ifc_list_deactivate();");
$ifc->set(CMS_L_DESKTOP_MAILBOX_036 . ":", "button", "move");
$ifc->set(mailbox_directory(),             "select", "#trashbin", NULL, "target");
echo("</td>" .
     "</tr>");
ifc_table_close();

$ifc->close();