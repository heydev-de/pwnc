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

global $comment_message,
       $comment_index,
       $comment_page,
       $comment_add_name,
       $comment_add_email,
       $comment_add_url,
       $comment_add_text,
       $comment_add_message,
       $comment_add_captcha_key,
       $comment_add_captcha_code,
       $comment_edit_name,
       $comment_edit_email,
       $comment_edit_url,
       $comment_edit_text,
       $comment_edit_message;

if (! cms_load("comment"))
{
    echo(t("msg_unavailable"));
    return;
};

//instance
if (isset($GLOBALS["comment"]["instance"]) && nstre($GLOBALS["comment"]["instance"]))
    $instance = $GLOBALS["comment"]["instance"];
else
    $instance = defined("CMS_CONTENT_INDEX") ? CMS_CONTENT_INDEX : "";

$comment = new comment($instance);
if (! $comment->enabled)
{
    echo(t("msg_unavailable"));
    return;
};

//validate access permissions
if (! ($comment->reader || $comment->writer || $comment->operator)) return;

//default status
$default_status = empty($GLOBALS["comment"]["default_status"]) ?
                  CMS_DB_COMMENT_STATUS_INACTIVE :
                  CMS_DB_COMMENT_STATUS_ACTIVE;

//notification e-mail
$notification_email = (empty($GLOBALS["comment"]["notification_email"]) ||
                      ! verify_email($GLOBALS["comment"]["notification_email"])) ?
                      "" : $GLOBALS["comment"]["notification_email"];

//display order
$display_order = ! empty($GLOBALS["comment"]["display_order"]);

//spam threshold
$spam_threshold = (isset($GLOBALS["comment"]["spam_threshold"]) &&
                   nstre($GLOBALS["comment"]["spam_threshold"])) ?
                  (int)$GLOBALS["comment"]["spam_threshold"] : 95;
$spam_threshold = max(0, min($spam_threshold, 100));

//use captcha
$use_captcha = ! empty($GLOBALS["comment"]["use_captcha"]);

//configuration
$comment->default_status = $default_status;
$comment->spam_threshold = $spam_threshold;

//process messages
$error   = [];
$success = "";

switch ($comment_message)
{
case "add":
//..................................................................................................

    if (! $comment->writer) break;

    if (nstreq(CMS_USER, "anonymous"))
    {
        $comment_add_name = CMS_NAME;
        if (verify_email(CMS_EMAIL)) $comment_add_email = CMS_EMAIL;
    };

    //validate input data
    $comment_add_name = stripspaces($comment_add_name);
    if (stre($comment_add_name))
        $error[] = t("please_specify_your_name");

    if (! verify_email($comment_add_email))
        $error[] = t("err_email_invalid");

    if (
         nstre($comment_add_url)
         &&
         ! preg_match("/^https?:\/\//", $comment_add_url)
       )
        $comment_add_url = "https://$comment_add_url";

    $comment_add_text = stripspaces($comment_add_text);
    if (stre($comment_add_text))
        $error[] = t("err_comment_empty");

    //verify captcha
    if (
         $use_captcha
         &&
         cms_load("captcha")
         &&
         ! (new captcha())->verify($comment_add_captcha_key, $comment_add_captcha_code)
       )
        $error[] = t("err_captcha_invalid"); //captcha mismatch

    if (count($error) > 0) break; //abort on error

    //add comment
    $index = $comment->add(
        $comment_add_name,
        $comment_add_email,
        $comment_add_url,
        $comment_add_text);

    //error handling
    switch ($index)
    {
    case -1: //spam

        $error[] = t("err_comment_spam");
        break 2;

    case -2: //double post

        $error[] = t("err_comment_duplicate");
        break 2;

    case FALSE: //error

        $error[] = t("err_comment_not_saved");
        break 2;
    };

    //success
    $success = ($comment->default_status === CMS_DB_COMMENT_STATUS_INACTIVE) ?
               t("msg_comment_saved_moderated") : t("msg_comment_saved");

    //e-mail notification
    if (nstre($notification_email) && cms_load("smtp"))
    {
        $url  = u() . "#comment";
        $_url = CMS_MODULES_URL . "identification.php" .
            cms_param(["location" => u(["comment_message" => "\x1B%s",
                                        "comment_index"   => $index]) .
                                     "#comment"]);

        //requires approval
        if ($comment->default_status === CMS_DB_COMMENT_STATUS_INACTIVE)
            smtp_send($notification_email,
                      t("new_comment_approval_required"),
                      sprintf(t("mail_moderation_request"),
                              $url,
                              $comment_add_text,
                              str_replace("%25s", "enable", $_url),
                              str_replace("%25s", "delete", $_url),
                              str_replace("%25s", "spam",   $_url)));

        //instantly visible
        else
            smtp_send($notification_email,
                      t("new_comment"),
                      sprintf(t("mail_moderation_request"),
                              $url,
                              $comment_add_text,
                              str_replace("%25s", "disable", $_url),
                              str_replace("%25s", "delete",  $_url),
                              str_replace("%25s", "spam",    $_url)));
    };

    //reset input data
    $comment_add_name  = "";
    $comment_add_email = "";
    $comment_add_url   = "";
    $comment_add_text  = "";

    break;

case "rate_good":
//..................................................................................................

    if (! $comment->rate_good($comment_index))
        $error[] = t("err_command_failed");
    break;

case "rate_bad":
//..................................................................................................

    if (! $comment->rate_bad($comment_index))
        $error[] = t("err_command_failed");
    break;

case "_edit":
//..................................................................................................

    if (! $comment->operator) break;

    //editing cancelled
    if (streq($comment_edit_message, "cancel"))
    {
        $comment_message = "";
        break;
    };

    //validate input data
    $comment_edit_name = stripspaces($comment_edit_name);
    if (stre($comment_edit_name))
        $error[] = t("please_specify_a_name");

    if (! verify_email($comment_edit_email))
        $error[] = t("err_email_invalid");

    if (
         nstre($comment_edit_url)
         &&
         ! preg_match("/^https?:\/\//", $comment_edit_url)
       )
        $comment_edit_url = "https://$comment_edit_url";

    $comment_edit_text = stripspaces($comment_edit_text);
    if (stre($comment_edit_text))
        $error[] = t("err_edit_empty");

    if (count($error) === 0) //no error
    {
        //edit comment
        if ($comment->edit($comment_index,
                           $comment_edit_name,
                           $comment_edit_email,
                           $comment_edit_url,
                           $comment_edit_text))
        {
            $comment_message = "";
            $success         = t("msg_changes_saved");
            break;
        };

        $error[] = t("err_edit_not_saved");
    };

    break;

case "disable":
//..................................................................................................

    if (! $comment->operator) break;
    if (! $comment->status($comment_index, CMS_DB_COMMENT_STATUS_HIDDEN))
        $error[] = t("err_command_failed");
    break;

case "enable":
//..................................................................................................

    if (! $comment->operator) break;

    if (
         ($result = mysql_query("SELECT " . CMS_DB_COMMENT_STATUS . ", " .
                                            CMS_DB_COMMENT_EMAIL . " " .
                                "FROM " .   CMS_DB_COMMENT . " " .
                                "WHERE " .  CMS_DB_COMMENT_INDEX .
                                "='" .      sqlesc($comment_index) . "' " .
                                "LIMIT 1"))
         &&
         ($resultrow = mysql_fetch_assoc($result))
         &&
         $comment->status($comment_index, CMS_DB_COMMENT_STATUS_ACTIVE)
       )
    {
        //send approval notification on initial activation
        if (
             ($resultrow[CMS_DB_COMMENT_STATUS] === CMS_DB_COMMENT_STATUS_INACTIVE)
             &&
             verify_email($resultrow[CMS_DB_COMMENT_EMAIL])
             &&
             cms_load("smtp")
           )
            smtp_send($resultrow[CMS_DB_COMMENT_EMAIL],
                      t("approval_of_your_comment",
                        $language = permission_get_language("anonymous")),
                      sprintf(t("mail_comment_approved", $language),
                              u() . "#comment"));
        break;
    };

    $error[] = t("err_command_failed");
    break;

case "delete":
//..................................................................................................

    if (! $comment->operator) break;
    if (! $comment->delete($comment_index))
        $error[] = t("err_command_failed");
    break;

case "spam":
//..................................................................................................

    if (! $comment->operator) break;
    if (! $comment->delete($comment_index, TRUE))
        $error[] = t("err_command_failed");
    break;
};

insert("top");

//get number of comments
if (! $result = mysql_query("SELECT COUNT(" . CMS_DB_COMMENT_INDEX . ") " .
                            "FROM " .         CMS_DB_COMMENT . " " .
                            "WHERE " .        CMS_DB_COMMENT_INSTANCE .
                            "='" .            sqlesc($comment->instance) . "'" .
                            ($comment->operator ? "" :
                           " AND " .          CMS_DB_COMMENT_STATUS .
                            "='" .            sqlesc(CMS_DB_COMMENT_STATUS_ACTIVE) . "'")))
    return;

$count  = mysql_result($result, 0);
$_count = ceil($count / 10) - 1;
if ($_count < 0)         $_count = 0;
$page   = (int)$comment_page;
if ($page < 0)           $page   = 0;
elseif ($page > $_count) $page   = $_count;

//title
echo("<section id=\"comment\" " .
              "class=\"comment\">" .
     "<h2>" .
     sprintf(t("comment_count"), $count) .
     "</h2>");

//display comments
if ($count > 0)
{
    echo(jscript("
function comment_rate_bad(index)
{
    var text = \"" . q(t("confirm_rate_negative")) . "\";
    var url  = \"" . q(cms_url(["comment_message" => "rate_bad",
                                "comment_index"   => "\x1B%s",
                                "comment_page"    => $page])) .
                     "#comment_%s\".replace(/%s/g, index);
    if (confirm(text)) window.location.href = url;
};

function comment_rate_good(index)
{
    var text = \"" . q(t("confirm_rate_positive")) . "\";
    var url  = \"" . q(cms_url(["comment_message" => "rate_good",
                                "comment_index"   => "\x1B%s",
                                "comment_page"    => $page])) .
                     "#comment_%s\".replace(/%s/g, index);
    if (confirm(text)) window.location.href = url;
};

function comment_delete(index)
{
    var text = \"" . q(t("confirm_delete")) . "\";
    var url  = \"" . q(cms_url(["comment_message" => "delete",
                                "comment_index"   => "\x1B%s",
                                "comment_page"    => $page])) .
                     "#comment\".replace(/%s/, index);
    if (confirm(text)) window.location.href = url;
};

function comment_spam(index)
{
    var text = \"" . q(t("confirm_delete_spam")) . "\";
    var url  = \"" . q(cms_url(["comment_message" => "spam",
                                "comment_index"   => "\x1B%s",
                                "comment_page"    => $page])) .
                     "#comment\".replace(/%s/, index);
    if (confirm(text)) window.location.href = url;
};"));

    if (
         ($result = mysql_query("SELECT * " .
                                "FROM " .     CMS_DB_COMMENT . " " .
                                "WHERE " .    CMS_DB_COMMENT_INSTANCE .
                                "='" .        sqlesc($comment->instance) . "' " .
                                ($comment->operator ? "" :
                                "AND " .      CMS_DB_COMMENT_STATUS .
                                "='" .        sqlesc(CMS_DB_COMMENT_STATUS_ACTIVE) . "' ") .
                                "ORDER BY " . CMS_DB_COMMENT_TIME . " " .
                                              ($display_order ? "" : "DESC ") .
                                "LIMIT " .    sqlesc($page * 10) . ", 10"))
         &&
         (mysql_num_rows($result) > 0)
       )
    {
        echo("<ol start=\"" . ($page * 10 + 1) . "\" " .
                 "class=\"comment-display\">");

        while ($resultrow = mysql_fetch_assoc($result))
        {
            $index  = $resultrow[CMS_DB_COMMENT_INDEX];
            $_index = x($index);
            $name   = x($resultrow[CMS_DB_COMMENT_NAME]);
            $url    = $resultrow[CMS_DB_COMMENT_URL];

            //rating
            $rating_value     = (int)$resultrow[CMS_DB_COMMENT_RATING_VALUE];
            $rating_count     = (int)$resultrow[CMS_DB_COMMENT_RATING_COUNT];
            $rating_negative  = ($rating_count - $rating_value) / 2;
            $rating_positive  = ($rating_count + $rating_value) / 2;
            $factor           = 50 / max($rating_negative, $rating_positive, 50);
            $_rating_negative = $factor * $rating_negative;
            $_rating_positive = $factor * $rating_positive;

            echo("<li itemscope " .
                     "itemtype=\"https://schema.org/Comment\">" .

            //head
                 "<div class=\"comment-display-head\">" .

            //name
                 "<a id=\"comment_$_index\" " .
                    (preg_match("/^https?:\/\//", $url) ?
                    "href=\"" . x($url) . "\"" .
                    "title=\"" . t("homepage") . "\"" .
                    "rel=\"nofollow ugc\" " : "") .
                    "itemprop=\"author\">" .
                 $name .
                 "</a> " .

            //timestamp
                 "<time datetime=\"" . x(date("c", $resultrow[CMS_DB_COMMENT_TIME])) . "\" " .
                       "itemprop=\"datePublished\">" .
                 "<a href=\"" . x(u($param = ["comment_page" => $page])) .
                                "#comment_$_index\" " .
                    "title=\"" . sprintf(t("comment_id"), $_index) . "\">" .
                 "(" . x(friendly_date($resultrow[CMS_DB_COMMENT_TIME])) . ")" .
                 "</a>" .
                 "</time>" .
                 "</div>" .

            //body
                 "<div class=\"comment-display-body\" " .
                      (($resultrow[CMS_DB_COMMENT_STATUS] != CMS_DB_COMMENT_STATUS_ACTIVE) ?
                      "style=\"OPACITY:0.5\" " : "") .
                      "itemprop=\"text\">" .
                 x($resultrow[CMS_DB_COMMENT_TEXT]) .
                 "</div>" .

            //foot
                 "<div class=\"comment-display-foot\">");

            //edit rating
            if (strpos($resultrow[CMS_DB_COMMENT_RATING_USERID], "/" . CMS_IPHASH . "/") === FALSE)

                //rate bad
                echo("<div class=\"comment-display-rate\">" .

                     "<button class=\"comment-display-rate-bad\" " .
                             "onclick=\"comment_rate_bad('$_index');\">" .
                     t("rate_negatively") .
                     "</button>" .

                //rate good
                     "<button class=\"comment-display-rate-good\" " .
                             "onclick=\"comment_rate_good('$_index');\">" .
                     t("rate_positively") .
                     "</button>" .

                     "</div>"); //comment-display-rate

            //display rating
            echo("<div class=\"comment-display-rating\">" .

                 "<div class=\"comment-display-rating-negative-bar\" " .
                      "style=\"WIDTH:$_rating_negative%\">" .
                 "</div>" .

                 "<div class=\"comment-display-rating-positive-bar\" " .
                      "style=\"WIDTH:$_rating_positive%\">" .
                 "</div>" .

                 "<div class=\"comment-display-rating-negative-value\">" .
                 $rating_negative .
                 "</div>" .

                 "<div class=\"comment-display-rating-positive-value\" " .
                      "itemprop=\"upvoteCount\">" .
                 $rating_positive .
                 "</div>" .

                 "</div>" . //comment-display-rating
                 "</div>"); //comment-display-foot

            //command
            if ($comment->operator)
            {
                echo("<div class=\"p\">" .

                //edit command
                     "<a href=\"" . x(cms_url(["comment_message" => "edit",
                                               "comment_index"   => $index,
                                               "comment_page"    => $page])) .
                                    "#comment-form\" " .
                        "class=\"button\">" .
                     t("command_edit") .
                     "</a>");

                //disable command
                if ($resultrow[CMS_DB_COMMENT_STATUS] == CMS_DB_COMMENT_STATUS_ACTIVE)
                    echo("<a href=\"" . x(cms_url(["comment_message" => "disable",
                                                   "comment_index"   => $index,
                                                   "comment_page"    => $page])) .
                                        "#comment_$_index\" " .
                            "class=\"button\">" .
                         t("disable") .
                         "</a>");

                //enable command
                else
                    echo("<a href=\"" . x(cms_url(["comment_message" => "enable",
                                                   "comment_index"   => $index,
                                                   "comment_page"    => $page])) .
                                        "#comment_$_index\" " .
                            "class=\"button\">" .
                         t("enable") .
                         "</a>");

                //delete command
                echo("<a href=\"#\" " .
                        "onclick=\"comment_delete('$_index');return false;\" " .
                        "class=\"button\">" .
                     t("command_delete") .
                     "</a>" .

                //spam command
                     "<a href=\"#\" " .
                        "onclick=\"comment_spam('$_index');return false;\" " .
                        "class=\"button\">" .
                     t("spam") .
                     " <span title=\"" . t("spam_probability") . "\">" .
                     "(" . x($resultrow[CMS_DB_COMMENT_SPAM_PROBABILITY]) . "%)" .
                     "</span>" .
                     "</a>" .
                     "</div>"); //p
            };

            echo("</li>");
        };

        echo("</ol>");
    };

    //pagination
    pagination(
        cms_url(["comment_page" => "\x1B%page%"]) .
        "#comment",
        $page,
        $_count,
        t("more_comments"),
        "comment-pagination");

};

echo("<div id=\"comment-form\">");

//display errors
if (count($error) > 0)
    echo("<div class=\"response-error\">" .
         implode("<br>", $error) .
         "</div>");

//display success
if (nstre($success))
    echo("<div class=\"response-success\">" .
         $success .
         "</div>");

//process messages
switch ($comment_message)
{
case "edit":
//..................................................................................................

    if (! $comment->operator) break;

    //get comment data
    if (
         ($result = mysql_query("SELECT * " .
                                "FROM " .  CMS_DB_COMMENT . " " .
                                "WHERE " . CMS_DB_COMMENT_INDEX .
                                "='" .     sqlesc($comment_index) . "' " .
                                "LIMIT 1"))
         &&
         ($resultrow = mysql_fetch_assoc($result))
       )
    {
        $comment_edit_name  = $resultrow[CMS_DB_COMMENT_NAME];
        $comment_edit_email = $resultrow[CMS_DB_COMMENT_EMAIL];
        $comment_edit_url   = $resultrow[CMS_DB_COMMENT_URL];
        $comment_edit_text  = $resultrow[CMS_DB_COMMENT_TEXT];
    };

case "_edit":
//..................................................................................................

    if (! $comment->operator) break;

    //display edit form
    echo("<form name=\"comment_edit\" " .
               "action=\"" . x(cms_url(["comment_message" => "_edit",
                                        "comment_index"   => $comment_index,
                                        "comment_page"    => $page])) .
                             "#comment-form\" " .
               "method=\"post\" " .
               "class=\"comment-edit\">" .

    //title
         "<h3>" .
         t("edit_comment") .
         "</h3>" .

    //name
         "<div class=\"p\">" .
         "<label for=\"comment-edit-name\">" .
         "*&nbsp;" . t("name") .
         "</label><br>" .
         "<input id=\"comment-edit-name\" " .
                "name=\"comment_edit_name\" " .
                "type=\"text\" " .
                "value=\"" . x($comment_edit_name) . "\">" .
         "</div>" . //p

    //e-mail
         "<div class=\"p\">" .
         "<label for=\"comment-edit-email\">" .
         "*&nbsp;" . t("email") .
         "</label><br>" .
         "<input id=\"comment-edit-email\" " .
                "name=\"comment_edit_email\" " .
                "type=\"text\" " .
                "value=\"" . x($comment_edit_email) . "\"><br>" .
         "<small>" .
         t("will_not_be_published") .
         "</small>" .
         "</div>" . //p

    //homepage url
         "<div class=\"p\">" .
         "<label for=\"comment-edit-url\">" .
         t("homepage_url") .
         "</label><br>" .
         "<input id=\"comment-edit-url\" " .
                "name=\"comment_edit_url\" " .
                "type=\"text\" " .
                "value=\"" . x($comment_edit_url) . "\">" .
         "</div>" . //p

    //text
         "<div class=\"p\">" .
         "<label for=\"comment-edit-text\">" .
         "*&nbsp;" . t("comment") .
         "</label><br>" .
         "<textarea id=\"comment-edit-text\" " .
                   "name=\"comment_edit_text\">\n" .
         x($comment_edit_text) .
         "</textarea>" .
         "</div>" . //p

    //buttons
         "<div class=\"p\">" .

    //cancel button
         "<button name=\"comment_edit_message\" " .
                 "type=\"submit\" " .
                 "value=\"cancel\">" .
         t("command_cancel") .
         "</button>" .

    //save button
         "<button name=\"comment_edit_message\" " .
                 "type=\"submit\" " .
                 "value=\"save\">" .
         t("command_save") .
         "</button>" .
         "</div>"); //p

    break;

default:
//..................................................................................................

    if (! $comment->writer) break;
    if (nstre($success))    break;

    //display add form
    echo("<form name=\"comment_add\" " .
               "action=\"" . x(cms_url(["comment_message" => "add",
                                        "comment_page"    => $page])) .
                             "#comment-form\" " .
               "method=\"post\" " .
               "class=\"comment-add\">" .

    //title
         "<h3>" .
         t("write_comment") .
         "</h3>");

    if ($flag = streq(CMS_USER, "anonymous"))

        //name
        echo("<div class=\"p\">" .
             "<label for=\"comment-add-name\">" .
             "*&nbsp;" . t("name") .
             "</label><br>" .
             "<input id=\"comment-add-name\" " .
                    "name=\"comment_add_name\" " .
                    "type=\"text\" " .
                    "value=\"" . x($comment_add_name) . "\">" .
             "</div>"); //p

    if ($flag || ! verify_email(CMS_EMAIL))
    {
        if (stre($comment_add_email)) $comment_add_email = CMS_EMAIL;

        //e-mail
        echo("<div class=\"p\">" .
             "<label for=\"comment-add-email\">" .
             "*&nbsp;" . t("email") .
             "</label><br>" .
             "<input id=\"comment-add-email\" " .
                    "name=\"comment_add_email\" " .
                    "type=\"text\" " .
                    "value=\"" . x($comment_add_email) . "\"><br>" .
             "<small>" .
             t("will_not_be_published") .
             "</small>" .
             "</div>"); //p
    };

    //homepage url
    echo("<div class=\"p\">" .
         "<label for=\"comment-add-url\">" .
         t("homepage_url") .
         "</label><br>" .
         "<input id=\"comment-add-url\" " .
                "name=\"comment_add_url\" " .
                "type=\"text\" " .
                "value=\"" . x($comment_add_url) . "\">" .
         "</div>" . //p

    //text
         "<div class=\"p\">" .
         "<label for=\"comment-add-text\">" .
         "*&nbsp;" . t("comment") .
         "</label><br>" .
         "<textarea id=\"comment-add-text\" " .
                   "name=\"comment_add_text\">\n" .
         x($comment_add_text) .
         "</textarea>" .
         "</div>"); //p

    //captcha
    if ($use_captcha && cms_load("captcha"))
    {
        $captcha = (new captcha())->create();

        if ($captcha !== FALSE)
            echo("<div class=\"p\">" .

                 "<label for=\"comment-add-captcha-key\">" .
                 t("captcha") .
                 "</label><br>" .

            //captcha input
                 "<input id=\"comment-add-captcha-key\" " .
                        "name=\"comment_add_captcha_key\" " .
                        "type=\"text\" " .
                        "value=\"\"><br>" .
                 "<small>" .
                 t("hint_captcha") .
                 "</small><br>" .

            //verification code
                 "<input name=\"comment_add_captcha_code\" " .
                        "type=\"hidden\" " .
                        "value=\"" . x($captcha["code"]) . "\">" .
                 "<img src=\"" . x($captcha["image"]) . "\" " .
                      "width=\"" . x($captcha["width"]) . "\" " .
                      "height=\"" . x($captcha["height"]) . "\" " .
                      "alt=\"" . t("captcha") . "\">" .

                 "</div>"); //p
    };

    //buttons
    echo("<div class=\"p\">" .

    //save button
         "<button name=\"comment_add_message\" " .
                 "type=\"submit\" " .
                 "value=\"save\"> " .
         t("command_save") .
         "</button>" .
         "</div>"); //p
};

echo("</form>" .
     "</div>"); //comment-form

insert("bottom");
permission([CMS_COMMENT_PERMISSION_READER . ".$instance"   => t("read"),
            CMS_COMMENT_PERMISSION_WRITER . ".$instance"   => t("write"),
            CMS_COMMENT_PERMISSION_OPERATOR . ".$instance" => t("operator")]);

echo("</section>"); //comment