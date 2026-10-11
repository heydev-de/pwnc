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

global $forum_message,
       $forum_index,
       $forum_buffer,
       $forum_search,
       $forum_user,
       $forum_page,
       $forum_edit_message,
       $forum_edit_title,
       $forum_edit_text,
       $forum_edit_email;

//load libraries
if (! cms_load("forum"))
{
    echo(t("msg_unavailable"));
    return;
};

//template
$template = $GLOBALS["forum"]["template"] ?? NULL;
if ($template !== NULL)
    $template->query_data =
        array_merge($template->query_data,
                    ["forum_index" => $forum_index]);

//instance
if (isset($GLOBALS["forum"]["instance"]) && nstre($GLOBALS["forum"]["instance"]))
    $instance = $GLOBALS["forum"]["instance"];
else
    $instance = defined("CMS_CONTENT_INDEX") ? CMS_CONTENT_INDEX : "";

$forum = new forum($instance);
if (! $forum->enabled)
{
    echo(t("msg_unavailable"));
    return;
};

//validate access permissions
if (! ($forum->reader || $forum->writer || $forum->operator)) return;

//basic variable initiation
if (! $forum_index) $forum_index = "0";
$table   = sqlesc($forum->table, TRUE);
$success = "";
$error   = "";

//--------------------------------------------------------------------------------------------------

//message handling

//process edit message
switch ($forum_message)
{
case "add":
//..................................................................................................

    if (! $forum->test_add($forum_index)) break;

    //quote post if replying
    if (
         $forum_index
         &&
         ($result = mysql_query("SELECT " . CMS_DB_FORUM_CONTAINER . ", " .
                                            CMS_DB_FORUM_USER . ", " .
                                            CMS_DB_FORUM_TITLE . ", " .
                                            CMS_DB_FORUM_TEXT . ", " .
                                            CMS_DB_FORUM_TIME . " " .
                                "FROM `" .  $table . "` " .
                                "WHERE " .  CMS_DB_FORUM_INDEX .
                                "='" .      sqlesc($forum_index) . "' " .
                                "LIMIT 1"))
         &&
         ($resultrow = mysql_fetch_assoc($result))
         &&
         $resultrow[CMS_DB_FORUM_CONTAINER]
       )
    {
        $forum_edit_title = $resultrow[CMS_DB_FORUM_TITLE];
        if (strpos($resultrow[CMS_DB_FORUM_TITLE], t("re")) !== 0)
            $forum_edit_title = t("re") . $forum_edit_title;
        $forum_edit_text  = sprintf(t("post_quote_header"),
                                    permission_get_name($resultrow[CMS_DB_FORUM_USER]),
                                    weekday(date("w", $resultrow[CMS_DB_FORUM_TIME])),
                                    date(t("date_format"), $resultrow[CMS_DB_FORUM_TIME]),
                                    date(t("short_time_format"), $resultrow[CMS_DB_FORUM_TIME])) .
                            ":\n\n" .
                            forum_quote(remove_format($resultrow[CMS_DB_FORUM_TEXT]));
        $forum_edit_text  = str_replace("[", "\[", $forum_edit_text);
        $forum_edit_text  = str_replace("]", "\]", $forum_edit_text);
    };

    $forum_message = "_add"; //go to editing
    break;

case "edit":
//..................................................................................................

    if (
         $forum->test_edit($forum_index)
         &&
         ($result = mysql_query("SELECT " . CMS_DB_FORUM_TITLE . ", " .
                                            CMS_DB_FORUM_TEXT . ", " .
                                            CMS_DB_FORUM_EMAIL . " " .
                                "FROM `" .  $table . "` " .
                                "WHERE " .  CMS_DB_FORUM_INDEX .
                                "='" .      sqlesc($forum_index) . "' " .
                                "LIMIT 1"))
         &&
         ($resultrow = mysql_fetch_assoc($result))
       )
    {
        $forum_edit_title = $resultrow[CMS_DB_FORUM_TITLE];
        $forum_edit_text  = $resultrow[CMS_DB_FORUM_TEXT];
        $forum_edit_email = $resultrow[CMS_DB_FORUM_EMAIL];
        $forum_message    = "_edit"; //go to editing
    }
    else
    {
        $error = t("err_edit_failed");
    };
    break;

case "_add":
case "_edit":
//..................................................................................................

    switch ($forum_edit_message)
    {
    case t("command_cancel"):

        $forum_message = ""; //back to main forum
        break;

    case t("command_show"):

        //trim whitespace
        $forum_edit_title = stripspaces($forum_edit_title);
        $forum_edit_text  = utf8_trim($forum_edit_text);

        //validate input
        if (stre($forum_edit_title) || stre($forum_edit_text))
        {
            //keep editing
            $error = t("err_title_content_required");
            break;
        };

        $forum_message = "_$forum_message"; //go to preview
    };
    break;

case "__add":
case "__edit":
//..................................................................................................

    switch ($forum_edit_message)
    {
    case t("command_cancel"):

        $forum_message = ""; //back to main forum
        break;

    case t("command_edit"):

        $forum_message = substr($forum_message, 1); //back to editing
        break;

    case t("command_save"):

        //trim whitespace
        $forum_edit_title = stripspaces($forum_edit_title);
        $forum_edit_text  = utf8_trim($forum_edit_text);

        //validate input
        if (stre($forum_edit_title) || stre($forum_edit_text))
        {
            //back to editing
            $forum_message = substr($forum_message, 1);
            $error         = t("err_title_content_required");
            break;
        };

        $forum_message = "_$forum_message"; //go to saving
    };
    break;

};

//process message
switch ($forum_message)
{
case "_add":
case "_edit":
//..................................................................................................

    //set meta data
    if ($template !== NULL)
        $template->title .= " – " . t("write_post");

    echo("<section id=\"forum\" " .
                  "class=\"forum-edit\">");

    insert("top_edit");

    echo("<form name=\"forum_edit\" " .
               "action=\"" . x(cms_url(["forum_index"   => $forum_index,
                                        "forum_message" => $forum_message])) .
                             "#forum\" " .
               "method=\"post\">");

    //display error
    if (nstre($error))
        echo("<div class=\"response-error\">" .
             $error .
             "</div>");

    //header
    echo("<h2>" .
         t("write_post") .
         "</h2>" .

    //title
         "<div class=\"p\">" .
         "<label for=\"forum-edit-title\">" .
         t("title") .
         "</label><br>" .
         "<input id=\"forum-edit-title\" " .
                "name=\"forum_edit_title\" " .
                "type=\"text\" " .
                "value=\"" . x($forum_edit_title) . "\" " .
                "maxlength=\"80\">" .
         "</div>" . //p

    //text
         "<div class=\"p\">" .
         "<label for=\"forum-edit-text\">" .
         t("content") .
         "</label><br>");

    //extended editing buttons
    $extension = "";

    if (cms_load("ifc"))
    {
        //images
        if (cms_available("image") && ifc_available("image") && cms_permission("interface.image", NULL, NULL))
        {
            $buffer     = "javascript:load_page('" .
                          qx(cms_url(CMS_MODULES_URL . "interface.php",
                                     ["ifc_page"          => "image",
                                      "ifc_option"        => "external",
                                      "ifc_select"        => "_object",
                                      "ifc_select_action" =>
                                          "javascript:this.opener.textcontrol_set(" .
                                          "'[name=forum_edit_text]'," .
                                          "'#image_left'," .
                                          "'%return%');"],
                                     TRUE)) . "');";
            $extension .= "<a href=\"" . x($buffer) . "\" " .
                             "title=\"image\">" .
                          image("textcontrol/button_image_external") .
                          "</a>";
        };

        //links
        if (cms_available("content") && ifc_available("content") && cms_permission("interface.content", NULL, NULL))
        {
            $buffer     = "javascript:load_page('" .
                          qx(cms_url(CMS_MODULES_URL . "interface.php",
                                     ["ifc_page"          => "content",
                                      "ifc_message"       => "edit_href",
                                      "ifc_option"        => "external",
                                      "ifc_select"        => "_value",
                                      "ifc_select_action" =>
                                          "javascript:this.opener.textcontrol_set(" .
                                          "'[name=forum_edit_text]'," .
                                          "'#link'," .
                                          "'%return%');"],
                                     TRUE)) . "');";
            $extension .= "<a href=\"" . x($buffer) . "\" " .
                             "title=\"link\">" .
                          image("textcontrol/button_link_external") .
                          "</a>";
        };

        //tokens
        if (cms_available("token") && ifc_available("token") && cms_permission("interface.token", NULL, NULL))
        {
            $buffer     = "javascript:load_page('" .
                          qx(cms_url(CMS_MODULES_URL . "interface.php",
                                     ["ifc_page"          => "token",
                                      "ifc_option"        => "external",
                                      "ifc_select"        => "token",
                                      "ifc_select_action" =>
                                          "javascript:this.opener.textcontrol_set(" .
                                          "'[name=forum_edit_text]'," .
                                          "'#token'," .
                                          "'%return%');"],
                                     TRUE)) . "');";
            $extension .= "<a href=\"" . x($buffer) . "\" " .
                             "title=\"token\">" .
                          image("textcontrol/button_token_external") .
                          "</a>";
        };
    };

    //editing buttons
    echo(jscript("textcontrol(" .
                 "\"[name=forum_edit_text]\", " .
                 "\"" . q(CMS_IMAGES_URL) . "\"" .
                 (nstre($extension) ? ", \"" . q($extension) . "\"" : "") . ");") .

    //text
         "<textarea id=\"forum-edit-text\" " .
                   "name=\"forum_edit_text\" " .
                   "rows=\"20\">\n" .
         x($forum_edit_text) .
         "</textarea>" .
         "</div>"); //p

    //notification
    if (nstreq(CMS_USER, "anonymous"))
        echo("<div class=\"p\">" .
             "<label>" .
             "<input name=\"forum_edit_email\" " .
                    "type=\"checkbox\" " .
                    "value=\"1\"" .
                    ($forum_edit_email ? " checked" : "") . ">" .
             t("notify_on_reply") .
             "</label>" .
             "</div>"); //p

    //command
    echo("<div class=\"p\">" .

    //cancel button
         "<input name=\"forum_edit_message\" " .
                "type=\"submit\" " .
                "value=\"" . t("command_cancel") . "\">" .

    //preview button
         "<input name=\"forum_edit_message\" " .
                "type=\"submit\" " .
                "value=\"" . t("command_show") . "\">" .
         "</div>" . //p

         "</form>");

    insert("bottom_edit");
    permission([CMS_FORUM_PERMISSION_READER . ".$instance"   => t("read"),
                CMS_FORUM_PERMISSION_WRITER . ".$instance"   => t("write"),
                CMS_FORUM_PERMISSION_OPERATOR . ".$instance" => t("operator")]);

    echo("</section>"); //forum-edit
    return;

case "__add":
case "__edit":
//..................................................................................................

    //set meta data
    if ($template !== NULL)
        $template->title .= " – " . t("write_post");

    echo("<section id=\"forum\" " .
                  "class=\"forum-preview\">");

    insert("top_preview");

    echo("<form name=\"forum_edit\" " .
               "action=\"" . x(cms_url(["forum_index"   => $forum_index,
                                        "forum_message" => $forum_message])) .
                             "#forum\" " .
               "method=\"post\">" .

    //carry over input data
         "<input name=\"forum_edit_title\" " .
                "type=\"hidden\" " .
                "value=\"" . x($forum_edit_title) . "\">" .
         "<input name=\"forum_edit_text\" " .
                "type=\"hidden\" " .
                "value=\"" . x($forum_edit_text) . "\">" .
         "<input name=\"forum_edit_email\" " .
                "type=\"hidden\" " .
                "value=\"" . x($forum_edit_email) . "\">" .

    //title
         "<h2 class=\"forum-title\">" .
         x($forum_edit_title) .
         "</h2>" .

    //text
         "<div class=\"forum-text p\">" .
         parse_text($forum_edit_text) .
         "</div>" .

    //command
         "<div class=\"p\">" .

    //cancel command
         "<input name=\"forum_edit_message\" " .
                "type=\"submit\" " .
                "value=\"" . t("command_cancel") . "\">" .

    //edit command
         "<input name=\"forum_edit_message\" " .
                "type=\"submit\" " .
                "value=\"" . t("command_edit") . "\">" .

    //save command
         "<input name=\"forum_edit_message\" " .
                "type=\"submit\" " .
                "value=\"" . t("command_save") . "\">" .
         "</div>" . //p

         "</form>");

    insert("bottom_preview");
    permission([CMS_FORUM_PERMISSION_READER . ".$instance"   => t("read"),
                CMS_FORUM_PERMISSION_WRITER . ".$instance"   => t("write"),
                CMS_FORUM_PERMISSION_OPERATOR . ".$instance" => t("operator")]);

    echo("</section>"); //forum-preview
    return;

case "___add":
//..................................................................................................

    $index = $forum->add(
        $forum_index,
        $forum_edit_title,
        $forum_edit_text,
        $forum_edit_email);

    if ($index)
    {
        //redirect to prevent reposting
        header("Location: " .
               cms_url(["forum_index"   => $index,
                        "forum_message" => "____add"]) .
               "#forum",
               TRUE, 303);
        exit();
    };

    $error = t("err_create_failed");
    break;

case "____add":
//..................................................................................................

    $success = t("msg_created");
    break;

case "___edit":
//..................................................................................................

    if ($forum->edit($forum_index,
                     $forum_edit_title,
                     $forum_edit_text,
                     $forum_edit_email))
    {
        $success = t("msg_updated");
        break;
    };

    $error = t("err_update_failed");
    break;

case "insert":
//..................................................................................................

    if ($forum->move($forum_buffer, $forum_index))
    {
        $forum_buffer = NULL;
        $success      = t("msg_moved");
        break;
    };

    $error = t("err_move_failed");
    break;

case "delete":
//..................................................................................................

    if (
         (($index = $forum->mysql->get($forum_index, CMS_DB_FORUM_CONTAINER, $forum->table)) !== FALSE)
         &&
         $forum->delete($forum_index)
       )
    {
        $forum_index = $index; //parent of deleted post
        $success     = t("msg_deleted");
        break;
    };

    $error = t("err_delete_failed");
    break;
};

//--------------------------------------------------------------------------------------------------

//command

$command = function($index) use ($forum, $forum_index, $forum_buffer)
{
    echo("<div class=\"p\">");

    //cut
    if ($forum->operator && $index && ($index !== $forum_buffer))
        echo("<a href=\"" . x(cms_url(["forum_index"  => $forum_index,
                                       "forum_buffer" => $index])) .
                            "#forum\" " .
                "class=\"button\">" .
             t("command_cut") .
             "</a>");

    //insert
    if ($forum->test_move($forum_buffer, $index))
        echo("<a href=\"" . x(cms_url(["forum_index"   => $index,
                                       "forum_buffer"  => $forum_buffer,
                                       "forum_message" => "insert"])) .
                            "#forum\" " .
                "class=\"button\">" .
             t("command_paste") .
             "</a>");

    //delete
    if ($forum->test_delete($index))
        echo("<a href=\"" . x(cms_url(["forum_index"   => $index,
                                       "forum_message" => "delete"])) .
                            "#forum\" " .
                "class=\"button\" " .
                "onclick=\"return confirm('" . q(t("confirm_delete")) . "');\">" .
             t("command_delete") .
             "</a>");

    //edit
    if ($forum->test_edit($index))
        echo("<a href=\"" . x(cms_url(["forum_index"   => $index,
                                       "forum_message" => "edit"])) .
                            "#forum\" " .
                "class=\"button\">" .
             t("command_edit") .
             "</a>");

    //add
    if ($forum->test_add($index))
    {
        echo("<a href=\"" . x(cms_url(["forum_index"   => $index,
                                       "forum_message" => "add"])) .
                            "#forum\" " .
                "class=\"button\">");
        if (! $index)
            echo(t("create_topic"));
        elseif (! $forum->mysql->get($index, CMS_DB_FORUM_CONTAINER, $forum->table))
            echo(t("write_post"));
        else
            echo(t("reply"));
        echo("</a>");
    };

    echo("</div>"); //p
};

//--------------------------------------------------------------------------------------------------

//generate path

$path = [];
$id   = $forum_index;

while ($id > 0)
{
    $query     = "SELECT " . CMS_DB_FORUM_CONTAINER . " " .
                 "FROM `" .  $table . "` " .
                 "WHERE " .  CMS_DB_FORUM_INDEX .
                 "='" .      sqlesc($id) . "' " .
                 "LIMIT 1";
    $result    = mysql_query($query);
    $resultrow = mysql_fetch_row($result);
    if ($resultrow === FALSE) break;
    $path[$id] = $resultrow[0];
    $id        = $resultrow[0];
};

$path       = array_reverse($path, TRUE);
$path_depth = count($path);

//--------------------------------------------------------------------------------------------------

//main forum

echo("<section id=\"forum\" " .
              "class=\"forum\">");

insert("top");

echo("<form action=\"" . x(cms_url()) . "#forum\" " .
           "method=\"post\">");

//display post being moved
if ($forum_buffer && (($buffer = $forum->mysql->get($forum_buffer, CMS_DB_FORUM_TITLE, $forum->table)) !== FALSE))
    echo("<div class=\"response\">" .
         sprintf(t("msg_moving_post"), x($buffer)) .
         "</div>");

//display error
if (nstre($error))
    echo("<div class=\"response-error\">" .
         $error .
         "</div>");

//display success
if (nstre($success))
    echo("<div class=\"response-success\">" .
         $success .
         "</div>");

//--------------------------------------------------------------------------------------------------

//navigation

echo("<div class=\"p\">");

//back
if ($path_depth > 1)
    echo("<a href=\"" . x(cms_url(["forum_index"  => end($path),
                                   "forum_buffer" => $forum_buffer])) .
                        "#forum\" " .
            "class=\"button\">" .
         t("command_previous") .
         "</a>");

//overview
if ($forum_index || nstre($forum_search) || nstre($forum_user))
    echo("<a href=\"" . x(cms_url(["forum_buffer" => $forum_buffer])) . "#forum\" " .
            "class=\"button\">" .
         t("overview") .
         "</a>");

//search
if ($forum_buffer)
    echo("<input name=\"forum_buffer\" " .
                "type=\"hidden\" " .
                "value=\"" . x($forum_buffer) . "\">");
echo("<input name=\"forum_search\" " .
            "type=\"text\" " .
            "value=\"" . x($forum_search) . "\" " .
            "placeholder=\"" . t("search") . "\">" .

     "</div>"); //p

//current path
if ($path_depth > 1)
{
    echo("<nav class=\"forum-path p\">" .

         "<a href=\"" . x(cms_url(["forum_buffer" => $forum_buffer])) . "#forum\">" .
         t("overview") .
         "</a>");

    $query  = "SELECT " . CMS_DB_FORUM_INDEX . ", " .
                          CMS_DB_FORUM_TITLE . " " .
              "FROM `" .  $table . "` " .
              "WHERE " .  CMS_DB_FORUM_INDEX . " " .
              "IN ('" .   implode("', '", $path) . "')";
    $result = mysql_query($query);

    while ($resultrow = mysql_fetch_assoc($result))
        echo("&nbsp;› " .
             "<a href=\"" . x(cms_url(["forum_index"  => $resultrow[CMS_DB_FORUM_INDEX],
                                       "forum_buffer" => $forum_buffer])) .
                            "#forum\">" .
             x($resultrow[CMS_DB_FORUM_TITLE]) .
             "</a>");

    echo("</nav>"); //forum-path
};

//--------------------------------------------------------------------------------------------------

//search results

if (nstre($forum_search))
{
    //set meta data
    if ($template !== NULL)
        $template->title .= " – " . t("search");

    //title
    echo("<h2>" .
         t("search_results") .
         "</h2>" .

         "<div class=\"forum-search-result\">");

    //execute search
    $result = $forum->search($forum_search);

    if (($result !== FALSE) && ($count = mysql_num_rows($result)))
    {
        //pagination values
        $_count = ceil($count / 10) - 1;
        if ($_count < 0)         $_count = 0;
        $page   = (int)$forum_page;
        if ($page < 0)           $page   = 0;
        elseif ($page > $_count) $page   = $_count;

        //buffer displayed page
        mysql_data_seek($result, $page * 10);
        $array = $count = [];
        for ($i = 0; ($i < 10) && ($resultrow = mysql_fetch_assoc($result)); $i++)
        {
            $array[] = $resultrow;
            $count[] = $resultrow[CMS_DB_FORUM_INDEX];
        };

        //fetch reply count
        $count = $forum->reply_count($count);

        //display search results
        foreach ($array AS $value)
        {
            $index = $value[CMS_DB_FORUM_INDEX];

            //search result item
            echo("<div class=\"forum-search-item\">" .

            //title
                 "<h3 class=\"forum-title\">" .
                 "<a href=\"" . x(cms_url(["forum_index"  => $index,
                                           "forum_buffer" => $forum_buffer])) .
                                "#forum\">" .
                 x($value[CMS_DB_FORUM_TITLE]) .
                 "</a>" .
                 "</h3>" .

            //info
                 "<div class=\"forum-info p\">" .

            //date
                 "<span class=\"forum-date\">" .
                 x(friendly_date($value[CMS_DB_FORUM_TIME])) .
                 "</span> ");

            //name
            $user = $value[CMS_DB_FORUM_USER];
            $name = permission_get_name($user);

            echo("<span class=\"forum-name\">" .
                 sprintf(t("posted_by"),
                         "<a href=\"" . x(cms_url(["forum_user"   => $user,
                                                   "forum_buffer" => $forum_buffer])) .
                                        "#forum\">" .
                         x($name) .
                         "</a>") .
                 "</span>" . //forum-name

            //count
                 " | " .
                 "<span class=\"forum-count\">" .
                 sprintf(t("reply_count"), $count[$index] ?? 0) .
                 "</span> " .

                 "</div>"); //forum-info

            //text
            $text  = stripspaces(remove_format($value[CMS_DB_FORUM_TEXT], TRUE, TRUE));
            $_text = quote_text($text, $forum_search, 1);

            //quote not found, display beginning of text instead
            if (stre($_text)) $_text = first_words($text);

            echo("<div class=\"forum-text p\">" .
                 $_text .
                 "</div>" .

                 "</div>"); //forum-search-item
        };

        //pagination
        pagination(
            cms_url(["forum_search" => $forum_search,
                     "forum_buffer" => $forum_buffer,
                     "forum_page"   => "\x1B%page%"]) .
            "#forum",
            $page,
            $_count,
            t("more_posts"),
            "forum_page");
    }

    //not found
    else
    {
        echo(t("search_no_results"));
    };

    echo("</div>"); //forum-search-result
}

//--------------------------------------------------------------------------------------------------

//user

elseif (nstre($forum_user))
{
    //set meta data
    if ($template !== NULL)
        $template->title .= " – " . t("overview");

    //title
    echo("<h2>" .
         x(permission_get_name($forum_user)) .
         "</h2>" .

         "<div class=\"forum-user\">");

    //retrieve user posts
    $query  = "SELECT * " .
              "FROM `" .    $table . "` " .
              "WHERE " .    CMS_DB_FORUM_CONTAINER . ">0 " .
              "AND " .      CMS_DB_FORUM_USER .
              "='" .        sqlesc($forum_user) . "' " .
              "ORDER BY " . CMS_DB_FORUM_TIME . " DESC";
    $result = mysql_query($query);

    if (($result !== FALSE) && ($count = mysql_num_rows($result)))
    {
        //pagination values
        $_count = ceil($count / 10) - 1;
        if ($_count < 0)         $_count = 0;
        $page   = (int)$forum_page;
        if ($page < 0)           $page   = 0;
        elseif ($page > $_count) $page   = $_count;

        //buffer displayed page
        mysql_data_seek($result, $page * 10);
        $array = $count = [];
        for ($i = 0; ($i < 10) && ($resultrow = mysql_fetch_assoc($result)); $i++)
        {
            $array[] = $resultrow;
            $count[] = $resultrow[CMS_DB_FORUM_INDEX];
        };

        //fetch reply count
        $count = $forum->reply_count($count);

        //display user posts
        foreach ($array AS $value)
        {
            $index = $value[CMS_DB_FORUM_INDEX];

            //user post item
            echo("<div class=\"forum-user-item\">" .

            //title
                 "<h3 class=\"forum-title\">" .
                 "<a href=\"" . x(cms_url(["forum_index"  => $index,
                                           "forum_buffer" => $forum_buffer])) .
                                "#forum\">" .
                 x($value[CMS_DB_FORUM_TITLE]) .
                 "</a>" .
                 "</h3>" . //forum-title

            //info
                 "<div class=\"forum-info p\">" .

            //date
                 "<span class=\"forum-date\">" .
                 x(friendly_date($value[CMS_DB_FORUM_TIME])) .
                 "</span>" .

            //count
                 " | " .
                 "<span class=\"forum-count\">" .
                 sprintf(t("reply_count"), $count[$index] ?? 0) .
                 "</span> " .

                 "</div>" . //forum-info

            //text
                 "<div class=\"forum-text p\">" .
                 x(first_words(stripspaces(remove_format($value[CMS_DB_FORUM_TEXT], TRUE, TRUE)))) .
                 "</div>" .

                 "</div>"); //forum-user-item
        };

        //pagination
        pagination(
            cms_url(["forum_user"   => $forum_user,
                     "forum_buffer" => $forum_buffer,
                     "forum_page"   => "\x1B%page%"]) .
            "#forum",
            $page,
            $_count,
            t("more_posts"),
            "forum-pagination");
    }

    //not found
    else
    {
        echo(t("user_no_posts"));
    };

    echo("</div>"); //forum-user
}

//--------------------------------------------------------------------------------------------------

//overview

elseif (! $forum_index)
{
    //set meta data
    if ($template !== NULL)
        $template->title .= " – " . t("overview");

    $topic     = []; //topic data
    $post_list = []; //post index list
    $post      = []; //post data
    $count     = []; //post reply count

    //retrieve 3 latest posts per topic
    $query = CMS_DB_FORUM_INDEX . " ORDER BY " . CMS_DB_FORUM_TIME . " DESC";
    $query = "SELECT GROUP_CONCAT($query) " .
             "FROM `" . $table . "` " .
             "WHERE " . CMS_DB_FORUM_CONTAINER .
             "=`t1`." . CMS_DB_FORUM_INDEX . " " .
             "LIMIT 3";

    //retrieve topics
    $query = "SELECT " .   CMS_DB_FORUM_INDEX . ", " .
                           CMS_DB_FORUM_TITLE . ", " .
                           CMS_DB_FORUM_TEXT . ", " .
             "(" .         $query . ") AS `post_list` " .
             "FROM `" .    $table . "` AS `t1` " .
             "WHERE " .    CMS_DB_FORUM_CONTAINER . "='0' " .
             "ORDER BY " . CMS_DB_FORUM_TITLE;

    //collect topic data and post index list
    $result = mysql_query($query);
    while ($resultrow = mysql_fetch_assoc($result))
    {
        $topic[] = $resultrow;
        $value   = $resultrow["post_list"];
        if (nstre($value)) $post_list = array_merge($post_list, explode(",", $value));
    };

    //collect post data and reply count
    if ($post_list)
    {
        //retrieve post data
        $query = "SELECT " . CMS_DB_FORUM_INDEX . ", " .
                             CMS_DB_FORUM_USER . ", " .
                             CMS_DB_FORUM_TITLE . ", " .
                             CMS_DB_FORUM_TIME . " " .
                 "FROM `" .  $table . "` " .
                 "WHERE " .  CMS_DB_FORUM_INDEX . " " .
                 "IN ('" .   implode("', '" , $post_list) . "')";

        $result = mysql_query($query);
        while ($resultrow = mysql_fetch_assoc($result))
            $post[$resultrow[CMS_DB_FORUM_INDEX]] = $resultrow;

        //retrieve post reply count
        $count = $forum->reply_count($post_list);
    };

    echo("<div class=\"forum-overview\">");

    foreach ($topic AS $value)
    {
        $index = $value[CMS_DB_FORUM_INDEX];
        $url   = x(cms_url(["forum_index"  => $index,
                            "forum_buffer" => $forum_buffer])) .
                 "#forum";

        //overview item
        echo("<div class=\"forum-item\">" .

        //topic
             "<div class=\"forum-topic\">");

        //image
        $image = get_first_image($value[CMS_DB_FORUM_TEXT]);
        if ($image !== NULL)
            echo("<div class=\"forum-image\">" .
                 "<a href=\"$url\">" .
                 image(translate_url($image),      //url
                       NULL,                       //width
                       NULL,                       //height
                       $value[CMS_DB_FORUM_TITLE], //alt
                       NULL,                       //style
                       TRUE) .                     //defer
                 "</a>" .
                 "</div>"); //forum-image

        //title
        echo("<h2 class=\"forum-title\">" .
             "<a href=\"$url\">" .
             x($value[CMS_DB_FORUM_TITLE]) .
             "</a>" .
             "</h2>" . //forum-title

        //teaser
             "<div class=\"forum-text p\">" .
             x(first_paragraph(stripspaces(remove_format($value[CMS_DB_FORUM_TEXT], TRUE, TRUE), TRUE))) . " " .
             "</div>" .

             "</div>"); //forum-topic

        //latest posts
        $post_list = nstre($post_list = $value["post_list"]) ? explode(",", $post_list) : [];
        foreach ($post_list AS $_index)
        {
            $value = $post[$_index];

            //post summary
            echo("<div class=\"forum-post\">" .

            //title
                    "<div class=\"forum-title\">" .
                    "<a href=\"" . x(cms_url(["forum_index"  => $_index,
                                              "forum_buffer" => $forum_buffer])) .
                                   "#forum\">" .
                    x($value[CMS_DB_FORUM_TITLE]) .
                    "</a>" .
                    "</div>" . //forum-title

            //info
                    "<div class=\"forum-info p\">" .

            //date
                    "<span class=\"forum-date\">" .
                    x(friendly_date($value[CMS_DB_FORUM_TIME])) .
                    "</span> ");

            //name
            $name = permission_get_name($value[CMS_DB_FORUM_USER]);

            echo("<span class=\"forum-name\">" .
                    sprintf(t("posted_by"),
                            "<a href=\"" . x(cms_url(["forum_user"   => $value[CMS_DB_FORUM_USER],
                                                      "forum_buffer" => $forum_buffer])) .
                                           "#forum\">" .
                            x($name) .
                            "</a>") .
                    "</span>" . //forum-name

            //count
                    " | " .
                    "<span class=\"forum-count\">" .
                    sprintf(t("reply_count"), $count[$_index] ?? 0) .
                    "</span> " .

                    "</div>" . //forum-info
                    "</div>"); //forum-post
        };

        //command
        $command($index);

        echo("</div>"); //forum-item
    };

    //command
    $command($forum_index);

    echo("</div>"); //forum-overview
}

//--------------------------------------------------------------------------------------------------

//not found

elseif ($path_depth === 0)
{
    //set gone status
    http_response_code(410);

    //set meta data
    if ($template !== NULL)
    {
        $template->title       .= " – " . t("post_not_found");
        $template->description  = t("err_post_unavailable");
        unset($template->query_data["forum_index"]);
    };

    //post item
    echo("<div class=\"forum-item\">" .

    //post
         "<div class=\"forum-post\">" .

    //title
         "<h2 class=\"forum-title\">" .
         t("post_not_found") .
         "</h2>" .

    //text
         "<div class=\"forum-text p\">" .
         t("err_post_unavailable") .
         "</div>" . //forum-text
         "</div>" . //forum-post
         "</div>"); //forum-item
}

//--------------------------------------------------------------------------------------------------

//topic

elseif ($path_depth === 1)
{
    //log access
    $forum->log_access($forum_index);

    //display topic
    $query     = "SELECT * " .
                 "FROM `" . $table . "` " .
                 "WHERE " . CMS_DB_FORUM_INDEX .
                 "='" .     sqlesc($forum_index) . "' " .
                 "LIMIT 1";
    $result    = mysql_query($query);
    $resultrow = mysql_fetch_assoc($result);

    //fetch reply count
    $count = $forum->reply_count($forum_index);
    $count = $count[$forum_index] ?? 0;

    //set meta data
    if ($template !== NULL)
    {
        $template->title       .= " – " . $resultrow[CMS_DB_FORUM_TITLE];
        $template->description  = first_words(stripspaces(remove_format($resultrow[CMS_DB_FORUM_TEXT], TRUE, TRUE)));
    };

    //topic item
    echo("<div class=\"forum-item\">" .

    //topic
         "<div class=\"forum-topic\">" .

    //title
         "<h2 class=\"forum-title\">" .
         x($resultrow[CMS_DB_FORUM_TITLE]) .
         "</h2>" .

    //text
         "<div class=\"forum-text p\">" .
         parse_text($resultrow[CMS_DB_FORUM_TEXT]) .
         "</div>");

    //command
    $command($forum_index);

    echo("</div>"); //forum-topic

    //posts
    if ($count)
    {
        $query  = "SELECT * " .
                  "FROM `" .    $table . "` " .
                  "WHERE " .    CMS_DB_FORUM_CONTAINER .
                  "='" .        sqlesc($forum_index) . "' " .
                  "ORDER BY " . CMS_DB_FORUM_TIME . " DESC";
        $result = mysql_query($query);
        $count  = mysql_num_rows($result);

        //pagination values
        $_count = ceil($count / 10) - 1;
        if ($_count < 0)         $_count = 0;
        $page   = (int)$forum_page;
        if ($page < 0)           $page   = 0;
        elseif ($page > $_count) $page   = $_count;

        //buffer displayed page
        mysql_data_seek($result, $page * 10);
        $array = $count = [];
        for ($i = 0; ($i < 10) && ($resultrow = mysql_fetch_assoc($result)); $i++)
        {
            $array[] = $resultrow;
            $count[] = $resultrow[CMS_DB_FORUM_INDEX];
        };

        //fetch reply count
        $count = $forum->reply_count($count);

        //display replies
        foreach ($array AS $value)
        {
            $index = $value[CMS_DB_FORUM_INDEX];

            //post
            echo("<div class=\"forum-post\">" .

            //title
                 "<h3 class=\"forum-title\">" .
                 "<a href=\"" . x(cms_url(["forum_index"  => $index,
                                           "forum_buffer" => $forum_buffer])) .
                                "#forum\">" .
                 x($value[CMS_DB_FORUM_TITLE]) .
                 "</a>" .
                 "</h3>" . //forum-title

            //info
                 "<div class=\"forum-info p\">" .

            //date
                 "<span class=\"forum-date\">" .
                 x(friendly_date($value[CMS_DB_FORUM_TIME])) .
                 "</span> ");

            //name
            $user = $value[CMS_DB_FORUM_USER];
            $name = permission_get_name($user);

            echo("<span class=\"forum-name\">" .
                 sprintf(t("posted_by"),
                         "<a href=\"" . x(cms_url(["forum_user"   => $user,
                                                   "forum_buffer" => $forum_buffer])) .
                                        "#forum\">" .
                         x($name) .
                         "</a>") .
                 "</span>" . //forum-name

            //count
                 " | " .
                 "<span class=\"forum-count\">" .
                 sprintf(t("reply_count"), $count[$index] ?? 0) .
                 "</span> " .

                 "</div>" . //forum-post

            //teaser
                 "<div class=\"forum-text p\">" .
                 x(first_words(stripspaces(remove_format($value[CMS_DB_FORUM_TEXT], TRUE, TRUE)))) . " " .
                 "</div>");

            $command($index);

            echo("</div>"); //forum-info
        };

        //pagination
        pagination(
            cms_url(["forum_index"  => $forum_index,
                     "forum_buffer" => $forum_buffer,
                     "forum_page"   => "\x1B%page%"]) .
            "#forum",
            $page,
            $_count,
            t("more_posts"),
            "forum-pagination");
    };

    echo("</div>"); //forum-item
}

//--------------------------------------------------------------------------------------------------

//post

else
{
    //log access
    $forum->log_access($forum_index);

    //display post
    $query     = "SELECT * " .
                 "FROM `" . $table . "` " .
                 "WHERE " . CMS_DB_FORUM_INDEX .
                 "='" .     sqlesc($forum_index) . "' " .
                 "LIMIT 1";
    $result    = mysql_query($query);
    $resultrow = mysql_fetch_assoc($result);

    //fetch reply count
    $count = $forum->reply_count($forum_index);
    $count = $count[$forum_index] ?? 0;

    //set meta data
    if ($template !== NULL)
    {
        $template->title       .= " – " . $resultrow[CMS_DB_FORUM_TITLE];
        $template->description  = first_words(stripspaces(remove_format($resultrow[CMS_DB_FORUM_TEXT], TRUE, TRUE)));
    };

    //post item
    echo("<div class=\"forum-item\">" .

    //post
         "<div class=\"forum-post\">" .

    //title
         "<h2 class=\"forum-title\">" .
         x($resultrow[CMS_DB_FORUM_TITLE]) .
         "</h2>" .

    //info
         "<div class=\"forum-info p\">" .

    //date
         "<span class=\"forum-date\">" .
         x(friendly_date($resultrow[CMS_DB_FORUM_TIME])) .
         "</span> ");

    //name
    $user = $resultrow[CMS_DB_FORUM_USER];
    $name = permission_get_name($user);

    echo("<span class=\"forum-name\">" .
         sprintf(t("posted_by"),
                 "<a href=\"" . x(cms_url(["forum_user"   => $user,
                                           "forum_buffer" => $forum_buffer])) .
                                "#forum\">" .
                 x($name) .
                 "</a>") .
         "</span>" . //forum-name

    //count
         " | " .
         "<span class=\"forum-count\">" .
         sprintf(t("reply_count"), $count) .
         "</span> " .

         "</div>" . //forum-info

    //text
         "<div class=\"forum-text p\">" .
         parse_text($resultrow[CMS_DB_FORUM_TEXT]) .
         "</div>");

    //command
    $command($forum_index);

    echo("</div>"); //forum-post

    //replies
    if ($count)
    {
        $query  = "SELECT * " .
                  "FROM `" .    $table . "` " .
                  "WHERE " .    CMS_DB_FORUM_CONTAINER .
                  "='" .        sqlesc($forum_index) . "' " .
                  "ORDER BY " . CMS_DB_FORUM_TIME;
        $result = mysql_query($query);
        $count  = mysql_num_rows($result);

        //pagination values
        $_count = ceil($count / 10) - 1;
        if ($_count < 0)         $_count = 0;
        $page   = (int)$forum_page;
        if ($page < 0)           $page   = 0;
        elseif ($page > $_count) $page   = $_count;

        //buffer displayed page
        mysql_data_seek($result, $page * 10);
        $array = $count = [];
        for ($i = 0; ($i < 10) && ($resultrow = mysql_fetch_assoc($result)); $i++)
        {
            $array[] = $resultrow;
            $count[] = $resultrow[CMS_DB_FORUM_INDEX];
        };

        //fetch reply count
        $count = $forum->reply_count($count);

        //display replies
        foreach ($array AS $value)
        {
            $index = $value[CMS_DB_FORUM_INDEX];

            //reply
            echo("<div class=\"forum-reply\">" .

            //title
                 "<h3 class=\"forum-title\">" .
                 "<a href=\"" . x(cms_url(["forum_index"  => $index,
                                           "forum_buffer" => $forum_buffer])) .
                                "#forum\">" .
                 x($value[CMS_DB_FORUM_TITLE]) .
                 "</a>" .
                 "</h3>" . //forum-title

            //info
                 "<div class=\"forum-info p\">" .

            //date
                 "<span class=\"forum-date\">" .
                 x(friendly_date($value[CMS_DB_FORUM_TIME])) .
                 "</span> ");

            //name
            $user = $value[CMS_DB_FORUM_USER];
            $name = permission_get_name($user);

            echo("<span class=\"forum-name\">" .
                 sprintf(t("posted_by"),
                         "<a href=\"" . x(cms_url(["forum_user"   => $user,
                                                   "forum_buffer" => $forum_buffer])) .
                                        "#forum\">" .
                         x($name) .
                         "</a>") .
                 "</span>" . //forum-name

            //count
                 " | " .
                 "<span class=\"forum-count\">" .
                 sprintf(t("reply_count"), $count[$index] ?? 0) .
                 "</span> " .

                 "</div>" . //forum-info

            //text
                 "<div class=\"forum-text p\">" .
                 parse_text($value[CMS_DB_FORUM_TEXT]) .
                 "</div>");

            $command($index);

            echo("</div>"); //forum-reply
        };

        //pagination
        pagination(
            cms_url(["forum_index"  => $forum_index,
                     "forum_buffer" => $forum_buffer,
                     "forum_page"   => "\x1B%page%"]) .
            "#forum",
            $page,
            $_count,
            t("more_posts"),
            "forum-pagination");
    };

    echo("</div>"); //forum-item
};

echo("</form>");

insert("bottom");
permission([CMS_FORUM_PERMISSION_READER . ".$instance"   => t("read"),
            CMS_FORUM_PERMISSION_WRITER . ".$instance"   => t("write"),
            CMS_FORUM_PERMISSION_OPERATOR . ".$instance" => t("operator")]);

echo("</section>"); //forum