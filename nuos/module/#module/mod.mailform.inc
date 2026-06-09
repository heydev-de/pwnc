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

global $mailform_form,
       $mailform_form_active,
       $mailform_page,
       $mailform_message,
       $mailform_captcha_code,
       $mailform_captcha_key;

if (! cms_load("smtp"))
{
    echo(CMS_MSG_UNAVAILABLE);
    return;
};

$data = new data("#system/mailform");

//display form selection
if (stre($mailform_form))
{
    $buffer = "";
    $count  = 0;

    //list all valid forms
    $data->move("first");
    while (($key = $data->move("next")) !== NULL)
    {
        if (
             $data->is_container($key)
             &&
             (
               nstre(l($data->get($key, "email")))
               ||
               nstre(l($data->get($key, "url")))
             )
           )
        {
            $mailform_form  = $key;
            $count         += 1;
            $name           = l($data->get($key, "name"));
            $description    = l($data->get($key, "description"));

            $buffer        .= "<li>" .
                              "<a href=\"" . x(cms_url(["mailform_form" => $key])) . "\">" .
                              "<strong>" . x($name) . "</strong>" .
                              "</a>" .
                              ($description ? "<div>" . parse_text($description) . "</div>" : "") .
                              "</li>";
        };
    };

    //no or more than one form
    if ($count <> 1)
    {
        echo("<section id=\"mailform\" " .
                      "class=\"mailform-overview\">");

        //more than one form => display list and exit
        if ($count > 1)
        {
            insert("top");

            echo("<h2>" .
                 CMS_L_MOD_MAILFORM_011 .
                 "</h2>" .

                 "<ul>" .
                 $buffer .
                 "</ul>");

            insert("bottom");
            permission(["" => CMS_L_ACCESS]);
        }

        //no form => display message and exit
        else
        {
            echo(CMS_MSG_UNAVAILABLE);
        };

        echo("</section>"); //mailform
        return;
    };

    //otherwise display the one existing form
};

//check if current form has been submitted
$flag_active = streq($mailform_form, $mailform_form_active);

if ($flag_active)
{
    $_mailform_page = (int)$mailform_page;

    //process message
    switch ($mailform_message)
    {
    case CMS_L_COMMAND_PREVIOUS: //go to previous page

        if (--$_mailform_page < 0) $_mailform_page = 0;
        break;

    default:
    case CMS_L_COMMAND_NEXT:     //go to next page or submit form

        $_mailform_page++;
        break;
    };
}

//otherwise go to first page
else
{
    $_mailform_page = 0;
};

//process data
$array          = [];
$form_name      = l($data->get($mailform_form, "name"));
$_form_name     = x($form_name);
$confirm        = NULL;
$page           = 0;
$flag_mismatch  = FALSE;
$flag_pagebreak = FALSE;
$flag_captcha   = FALSE;
$i              = 1;

//seek current form and run through elements
$data->move("to", $mailform_form);
while (($key = $data->move("next")) !== NULL)
{
    //exit loop at end of current form
    $type = $data->get($key, "#type");
    if (streq($type, "/container")) break;

    //check for page break
    if (streq($type, "pagebreak"))
    {
        //overwrite element data
        $array[$key] = TRUE;

        //exit loop at end of current page
        if (++$page > $_mailform_page)
        {
            $flag_pagebreak = TRUE;
            break;
        };

        continue;
    };

    //set element id
    $id = $data->get($key, "id");
    if (stre($id)) $id = "mailform_value$i";
    $i++;

    //get submitted data
    $_id   = preg_replace("/[\\x20\\x2E\\x5B\\x80-\\x9F]/", "_", $id);
    $value = isset($GLOBALS[$_id]) ? $GLOBALS[$_id] : NULL;
    if (is_array($value)) $value = implode("\n", $value);

    //add data to element list
    $array[$key] = ["name"  => l($data->get($key, "name")),
                    "id"    => $id,
                    "value" => $value];

    if ($flag_active) //form has been submitted
    {
        if ($flag_mismatch || ($page < $_mailform_page)) //invalid input or final submission
        {
            $_flag_mismatch = FALSE;

            if (nstre($value)) //value is set
            {
                //check input against regular expression
                if (
                     nstre($match = $data->get($key, "match"))
                     &&
                     (preg_match($match, $value) !== 1)
                   )
                    $_flag_mismatch = TRUE; //failed

                //add input to confirmation receiver list
                elseif (
                         ($data->get($key, "confirm") !== NULL)
                         &&
                         verify_email($value)
                       )
                    $confirm[] = $value;
            }

            //check if input is required
            elseif ($data->get($key, "required") !== NULL)
            {
                $_flag_mismatch = TRUE;
            };

            //set display form flag on mismatch
            if ($_flag_mismatch)
            {
                $array[$key]["mismatch"] = TRUE;
                $flag_mismatch           = TRUE;
                $_mailform_page          = $page;
            };
        };
    };
};

if (! $flag_pagebreak) //last page
{
    $use_captcha = $data->get($mailform_form, "captcha");

    if (

    //captcha enabled
         $use_captcha
         &&

    //form has been submitted
         $flag_active
         &&

    //invalid input or final submission
         ($flag_mismatch || ($page < $_mailform_page))
         &&

    //load library
         cms_load("captcha")
         &&

    //verify captcha
         (! (new captcha())->verify($mailform_captcha_key, $mailform_captcha_code))
       )
    {
        //captcha mismatch
        $flag_captcha   = TRUE;
        $flag_mismatch  = TRUE;
        $_mailform_page = $page;
    };

    $page++;
};

//display form
if ($page > $_mailform_page)
{
    echo("<section id=\"mailform-" . x($mailform_form) . "\" " .
                  "class=\"mailform\">");

    insert("form_top");

    echo("<form action=\"" . x(cms_url()) . "#mailform-" . rx($mailform_form) . "\" " .
               "method=\"post\">" .

    //title
         "<h2>" .
         $_form_name .
         "</h2>" .

    //hidden data fields
         "<input name=\"mailform_form\" " .
                "type=\"hidden\" " .
                "value=\"" . x($mailform_form) . "\">" .
         "<input name=\"mailform_form_active\" " .
                "type=\"hidden\" " .
                "value=\"" . x($mailform_form) . "\">" .
         "<input name=\"mailform_page\" " .
                "type=\"hidden\" " .
                "value=\"$_mailform_page\">");

    //display mismatch message
    if ($flag_mismatch)
        echo("<div class=\"response-error\">" .
             CMS_L_MOD_MAILFORM_008 .
             "</div>");

    $_page = 0;

    foreach ($array AS $key => $value)
    {
        //skip page break
        if ($value === TRUE)
        {
            $_page++;
            continue;
        };

        //field name
        $name     = x($value["name"]);
        if ($data->get($key, "required") !== NULL) $name = "* $name";

        //field id
        $id       = x($value["id"]);
        $_id      = x("mailform-$mailform_form-$key");

        //mismatch flag
        $mismatch = isset($value["mismatch"]);

        //field value
        $value    = $value["value"];

        //hide input of precedent pages
        if ($_page < $_mailform_page)
        {
            echo("<input name=\"$id\" " .
                        "type=\"hidden\" " .
                        "value=\"" . x($value) . "\">");
            continue;
        };

        //display element
        $type = $data->get($key, "#type");

        switch ($type)
        {
        case "checkbox":
        case "radio":
//..................................................................................................

            //option values
            $option  = preg_split("/\r?\n/", l($data->get($key, "option")),
                                  -1, PREG_SPLIT_NO_EMPTY);
            if (count($option) === 0) break;

            //selected values
            $novalue = stre($value);
            $value   = explode("\n", $value);

            //formatting
            $column  = (int)$data->get($key, "column");
            if ($column < 1)          $column = 1;
            $width   = 100 / $column; //100% width divided by number of columns

            //start output
            echo("<fieldset class=\"mailform-$type" . ($mismatch ? " mailform-mismatch" : "") . "\">" .

                 "<legend" . ($mismatch ? " style=\"COLOR:var(--cms-error-color)\"" : "") . ">" .
                 $name .
                 "</legend>");

            //description
            $description = l($data->get($key, "description"));
            if (nstre($description))
                echo("<div class=\"p\">" .
                     parse_text($description) .
                     "</div>");

            echo("<div class=\"mailform-multicolumn\">");

            foreach ($option AS $_key => $_value)
            {
                //preselection
                $selected  = FALSE;
                if (streq(substr($_value, 0, 1), "+"))
                {
                    $selected = $novalue && ($page > $_mailform_page);
                    $_value   = substr($_value, 1);
                };
                $selected |= in_array($_value, $value);

                //output option
                echo("<div style=\"WIDTH:$width%\">" .

                     "<label>" .
                     "<input id=\"$_id-$_key\" " .
                            "name=\"$id" . (streq($type, "checkbox") ? "[$_key]" : "") . "\" " .
                            "type=\"$type\" " .
                            "value=\"" . x($_value) . "\"" .
                            ($selected ? " checked" : "") . ">" .
                     x($_value) .
                     "</label>" .

                     "</div>");
            };

            echo("</div>" .      //mailform-multicolumn
                 "</fieldset>"); //mailform-$type

            break;

        case "select":
//..................................................................................................

            //option values
            $option  = preg_split("/\r?\n/", l($data->get($key, "option")),
                                  -1, PREG_SPLIT_NO_EMPTY);
            if (count($option) === 0) break;

            $novalue = stre($value);

            //start output
            echo("<div class=\"mailform-select p" . ($mismatch ? " mailform-mismatch" : "") . "\">" .

                 "<label for=\"$_id\"" . ($mismatch ? " style=\"COLOR:var(--cms-error-color)\"" : "") . ">" .
                 $name .
                 "</label><br>");

            //description
            $description = l($data->get($key, "description"));
            if (nstre($description))
                echo("<div class=\"p\">" .
                     parse_text($description) .
                     "</div>");

            echo("<select id=\"$_id\" " .
                         "name=\"$id\">");

            foreach ($option AS $_key => $_value)
            {
                //preselection
                $selected  = FALSE;
                if (streq(substr($_value, 0, 1), "+"))
                {
                    $selected = stre($value) && ($page > $_mailform_page);
                    $_value   = substr($_value, 1);
                };
                $selected |= streq($_value, $value);

                //output option
                $_value    = x($_value);
                echo("<option value=\"$_value\"" . ($selected ? " selected" : "") . ">" .
                     $_value .
                     "</option>");
            };

            echo("</select>" .

                 "</div>"); //mailform-select

            break;

        case "text":
//..................................................................................................

            //start output
            echo("<div class=\"mailform-text p" . ($mismatch ? " mailform-mismatch" : "") . "\">" .

                 "<label for=\"$_id\">" .
                 $name .
                 "</label><br>");

            //description
            $description = l($data->get($key, "description"));
            if (nstre($description))
                echo("<div class=\"p\">" .
                     parse_text($description) .
                     "</div>");

            //preset value
            if (stre($value) && ($page > $_mailform_page))
                $value = l($data->get($key, "default"));

            //parameters
            $width  = (int)$data->get($key, "width");
            $length = (int)$data->get($key, "length");

            echo("<input id=\"$_id\" " .
                        "name=\"$id\" " .
                        "type=\"text\" " .
                        "value=\"" . x($value) . "\"" .
                        (($width  > 0) ? " size=\"" . x($width) . "\""          : "") .
                        (($length > 0) ? " maxlength=\"" . x($length) . "\""    : "") .
                        (($width  > 0) ? " style=\"WIDTH:" . x($width) . "ch\"" : "") . ">" .

                 "</div>"); //mailform-text

            break;

        case "textarea":
//..................................................................................................

            //start output
            echo("<div class=\"mailform-textarea p" . ($mismatch ? " mailform-mismatch" : "") . "\">" .

                 "<label for=\"$_id\">" .
                 $name .
                 "</label><br>");

            //description
            $description = l($data->get($key, "description"));
            if (nstre($description))
                echo("<div class=\"p\">" .
                     parse_text($description) .
                     "</div>");

            //preset value
            if (stre($value) && ($page > $_mailform_page))
                $value = l($data->get($key, "default"));

            //parameters
            $width  = (int)$data->get($key, "width");
            $height = (int)$data->get($key, "height");
            $style  = "";
            if ($width  > 0) $style .= "WIDTH:" .  $width .  "ch;";
            if ($height > 0) $style .= "HEIGHT:" . $height . "em;";

            echo("<textarea id=\"$_id\" " .
                           "name=\"$id\" " .
                           (($width  > 0) ? " cols=\"" . x($width) . "\""                 : "") .
                           (($height > 0) ? " rows=\"" . x($height) . "\""                : "") .
                           (nstre($style) ? " style=\"" . x(substr($style, 0, -1)) . "\"" : "") . ">" .
                 x($value) .
                 "</textarea>" .

                 "</div>"); //mailform-textarea

            break;

        case "code":
//..................................................................................................

            //start output
            if ($flag_codeonly = $data->get($key, "codeonly"))
            {
                echo("<div class=\"mailform-code p" . ($mismatch ? " mailform-mismatch" : "") . "\">");
            }
            else
            {
                echo("<fieldset class=\"mailform-code" . ($mismatch ? " mailform-mismatch" : "") . "\">" .

                     "<legend>" .
                     $name .
                     "</legend>");

                //description
                $description = l($data->get($key, "description"));
                if (nstre($description))
                        echo("<div class=\"p\">" .
                             parse_text($description) .
                             "</div>");
            };

            //preset value
            if (stre($value) && ($page > $_mailform_page))
                $value = l($data->get($key, "default"));

            //code
            $code = l($data->get($key, "code"));

            //replace placeholder
            $placeholder = ["label" => $name,
                            "id"    => $_id,
                            "name"  => $id,
                            "value" => x($value)];

            foreach ($placeholder AS $_key => $_value)
                $code = stre($_value) ?
                    preg_replace("/(?:\\[[^\\[\\]]*)?%$_key%(?:[^\\[\\]]*\\])?/",
                                 "",
                                 $code) :
                    preg_replace("/(\\[([^\\[\\]]*))?%$_key%(([^\\[\\]]*)\\])?/",
                                 "\${2}$_value\\4",
                                 $code);

            echo($code .

                 ($flag_codeonly ? "</div>" : "</fieldset>")); //mailform-code

            break;

        case "hidden":
//..................................................................................................

            //preset value
            if ((! $value) && ($page > $_mailform_page))
                $value = l($data->get($key, "default"));

            echo("<input name=\"$id\" " .
                        "type=\"hidden\" " .
                        "value=\"" . x($value) . "\">");

            break;
        };
    };

    if (
         (! $flag_pagebreak) //last page
         &&
         $use_captcha        //use captcha
         &&
         cms_load("captcha") //load library
       )
    {
        //create captcha
        $captcha = (new captcha())->create();

        //start output
        if ($captcha !== FALSE)
            echo("<div class=\"mailform-captcha p" . ($flag_captcha ? " mailform-mismatch" : "") . "\">" .

                 "<label for=\"mailform-captcha\">" .
                 "* " . CMS_L_MOD_MAILFORM_009 .
                 "</label><br>" .

            //captcha input
                 "<input id=\"mailform-captcha\" " .
                        "name=\"mailform_captcha_key\" " .
                        "type=\"text\" " .
                        "value=\"\"><br>" .

                 "<small>" .
                 CMS_L_MOD_MAILFORM_010 .
                 "</small><br>" .

            //verification code
                 "<input name=\"mailform_captcha_code\" " .
                        "type=\"hidden\" " .
                        "value=\"" . x($captcha["code"]) . "\">" .
                 "<img src=\"" . x($captcha["image"]) . "\" " .
                      "width=\"" . x($captcha["width"]) . "\" " .
                      "height=\"" . x($captcha["height"]) . "\" " .
                      "alt=\"" . CMS_L_MOD_MAILFORM_009 . "\">" .

                 "</div>"); //mailform-captcha
    };

    //form commands
    echo("<div class=\"p\">");

    //button back
    if ($_mailform_page)
        echo("<input name=\"mailform_message\" " .
                    "type=\"submit\" " .
                    "value=\"" . CMS_L_COMMAND_PREVIOUS . "\">");

    //button next/submit
    if ($flag_pagebreak)
        $value = CMS_L_COMMAND_NEXT;
    elseif (stre($value = l($data->get($mailform_form, "submit"))))
        $value = CMS_L_MOD_MAILFORM_003;
    else
        $value = x($value);

    echo("<button name=\"mailform_message\" " .
                 "type=\"submit\" " .
                 "value=\"" . CMS_L_COMMAND_NEXT . "\">" .
         $value .
         "</button>" .
         "</div>" . //p

         "</form>");

    insert("form_bottom");
    permission(["" => CMS_L_ACCESS]);

    echo("</section>"); //mailform
    return;
};

//transmit form
$subject      = isset($GLOBALS["mailform_subject"]) ? $GLOBALS["mailform_subject"] : $form_name;
$confirmation = l($data->get($mailform_form, "confirmation"));
$confirmation = stre($confirmation) ? CMS_L_MOD_MAILFORM_002 : parse_text($confirmation);
$flag_success = FALSE;

if (($email = l($data->get($mailform_form, "email"))) || $confirm)
{
    //stylesheet
    $buffer   = read_file(CMS_PATH . "stylesheet/mail.css");
    $buffer   = stripspaces($buffer);
    $buffer   = str_replace("%", "%%", $buffer);

    //html
    $buffer   = CMS_DOCTYPE_HTML .
                "<html>" .
                "<head>" .
                "<meta charset=\"utf-8\">" .

    //title
                "<title>" .
                "%s" .
                "</title>" .
                "<meta name=\"generator\" " .
                      "content=\"" . x(CMS) . "\">" .
                "<style>" .
                $buffer .
                "</style>" .
                "</head>" .
                "<body>" .

    //confirmation
                "%s<br>" .
                "<br>" .
                "<div style=\"TEXT-ALIGN:right\">" .
                date(CMS_L_SHORT_DATETIME_FORMAT) .
                "</div>" .
                "<table>" .
                "<tr>" .
                "<th>" .
                CMS_L_MOD_MAILFORM_005 .
                "</th>" .
                "<th>" .
                CMS_L_MOD_MAILFORM_006 .
                "</th>" .
                "</tr>" .

    //content
                "%s" .
                "</table>" .
                "</body>" .
                "</html>";

    //content
    $content  = NULL;
    $_content = NULL;

    foreach ($array AS $key => $value)
    {
        if ($value === TRUE) continue;

        $_buffer   = "<tr>" .
                     "<td>" .
                     x($value["name"]) .
                     "</td>" .
                     "<td>" .
                     nl2br(x($value["value"])) .
                     "</td>" .
                     "</tr>";
        $content  .= $_buffer;
        if ($data->get($key, "secret")) continue;
        $_content .= $_buffer;

    };

    //send confirmation
    if ($confirm)
    {
        $_buffer = sprintf(
            $buffer,
            CMS_L_MOD_MAILFORM_001 . ": $_form_name",
            $confirmation,
            $_content);

        foreach ($confirm AS $value)
            smtp_send(
                $value,                                //to
                CMS_L_MOD_MAILFORM_001 . ": $subject", //subject
                $_buffer,                              //body
                TRUE);                                 //html
    };

    //send form
    if ($email)
    {
        $_buffer      = sprintf($buffer, $_form_name, "", $content);
        $flag_success = smtp_send(
            $email,                                     //to
            $subject,                                   //subject
            $_buffer,                                   //body
            TRUE,                                       //html
            $confirm ? implode(", ", $confirm) : NULL); //reply_to
    };
};

//http post
if (nstre($url = l($data->get($mailform_form, "url"))))
{
    $_array  = ["mailform_form"    => $mailform_form,
                "mailform_subject" => $subject];

    foreach ($array AS $value)
        $_array[$value["id"]] = $value["value"];

    $__array = preg_split("/[\p{C}\p{Z}]+/u", $url);

    foreach ($__array AS $value)
        $flag_success |= http_post(translate_url($value), $_array) !== FALSE;
};

//display confirmation
if ($flag_success)
{
    $log = new log();
    $log->access("sent", $form_name);

    echo("<section id=\"mailform-" . x($mailform_form) . "\" " .
                  "class=\"mailform-success\">");

    insert("confirmation_top");

    echo("<div class=\"response-success\">" .
         $confirmation .
         "</div>"); //mailform

    insert("confirmation_bottom");

    echo("</section>");
}

//fail
else
{
    echo("<section id=\"mailform-" . x($mailform_form) . "\" " .
                  "class=\"mailform-error\">" .

         "<div class=\"response-error\">" .
         CMS_L_MOD_MAILFORM_012 .
         "</div>" .

         "</section>"); //mailform
};