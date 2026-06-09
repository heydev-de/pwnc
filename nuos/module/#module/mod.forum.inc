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
    echo(CMS_MSG_UNAVAILABLE);
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
    echo(CMS_MSG_UNAVAILABLE);
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
                                "WHERE " .  CMS_DB_FORUM_INDEX . "='" . sqlesc($forum_index) . "' " .
                                "LIMIT 1"))
         &&
         ($resultrow = mysql_fetch_assoc($result))
         &&
         $resultrow[CMS_DB_FORUM_CONTAINER]
       )
    {
        $forum_edit_title = $resultrow[CMS_DB_FORUM_TITLE];
        if (strpos($resultrow[CMS_DB_FORUM_TITLE], CMS_L_MOD_FORUM_002) !== 0)
            $forum_edit_title = CMS_L_MOD_FORUM_002 . $forum_edit_title;
        $forum_edit_text  = sprintf(CMS_L_MOD_FORUM_001,
                                    permission_get_name($resultrow[CMS_DB_FORUM_USER]),
                                    weekday(date("w", $resultrow[CMS_DB_FORUM_TIME])),
                                    date(CMS_L_DATE_FORMAT, $resultrow[CMS_DB_FORUM_TIME]),
                                    date(CMS_L_SHORT_TIME_FORMAT, $resultrow[CMS_DB_FORUM_TIME])) .
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
                                "WHERE " .  CMS_DB_FORUM_INDEX . "='" . sqlesc($forum_index) . "' " .
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
        $error = CMS_L_MOD_FORUM_018;
    };
    break;

case "_add":
case "_edit":
//..................................................................................................

    switch ($forum_edit_message)
    {
    case CMS_L_COMMAND_CANCEL:

        $forum_message = ""; //back to main forum
        break;

    case CMS_L_COMMAND_SHOW:

        //trim whitespace
        $forum_edit_title = stripspaces($forum_edit_title);
        $forum_edit_text  = utf8_trim($forum_edit_text);

        //validate input
        if (stre($forum_edit_title) || stre($forum_edit_text))
        {
            //keep editing
            $error = CMS_L_MOD_FORUM_017;
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
    case CMS_L_COMMAND_CANCEL:

        $forum_message = ""; //back to main forum
        break;

    case CMS_L_COMMAND_EDIT:

        $forum_message = substr($forum_message, 1); //back to editing
        break;

    case CMS_L_COMMAND_SAVE:

        //trim whitespace
        $forum_edit_title = stripspaces($forum_edit_title);
        $forum_edit_text  = utf8_trim($forum_edit_text);

        //validate input
        if (stre($forum_edit_title) || stre($forum_edit_text))
        {
            //back to editing
            $forum_message = substr($forum_message, 1);
            $error         = CMS_L_MOD_FORUM_017;
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
        $template->title .= " – " . CMS_L_MOD_FORUM_010;

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
         CMS_L_MOD_FORUM_010 .
         "</h2>" .

    //title
         "<div class=\"p\">" .
         "<label for=\"forum-edit-title\">" .
         CMS_L_MOD_FORUM_014 .
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
         CMS_L_MOD_FORUM_028 .
         "</label><br>");

    //extended editing buttons
    $extension = "";

    if (cms_load("ifc"))
    {
        //images
        if (cms_available("image") && ifc_available("image") && cms_permission("interface.image", NULL, NULL))
        {
            $buffer     = "javascript:load_page('" .
                          qr(cms_url(CMS_MODULES_URL . "interface.php",
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
                          qr(cms_url(CMS_MODULES_URL . "interface.php",
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
                          qr(cms_url(CMS_MODULES_URL . "interface.php",
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
             CMS_L_MOD_FORUM_009 .
             "</label>" .
             "</div>"); //p

    //command
    echo("<div class=\"p\">" .

    //cancel button
         "<input name=\"forum_edit_message\" " .
                "type=\"submit\" " .
                "value=\"" . CMS_L_COMMAND_CANCEL . "\">" .

    //preview button
         "<input name=\"forum_edit_message\" " .
                "type=\"submit\" " .
                "value=\"" . CMS_L_COMMAND_SHOW . "\">" .
         "</div>" . //p

         "</form>");

    insert("bottom_edit");
    permission([CMS_FORUM_PERMISSION_READER . ".$instance"   => CMS_L_READ,
                CMS_FORUM_PERMISSION_WRITER . ".$instance"   => CMS_L_WRITE,
                CMS_FORUM_PERMISSION_OPERATOR . ".$instance" => CMS_L_OPERATOR]);

    echo("</section>"); //forum-edit
    return;

case "__add":
case "__edit":
//..................................................................................................

    //set meta data
    if ($template !== NULL)
        $template->title .= " – " . CMS_L_MOD_FORUM_010;

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
                "value=\"" . CMS_L_COMMAND_CANCEL . "\">" .

    //edit command
         "<input name=\"forum_edit_message\" " .
                "type=\"submit\" " .
                "value=\"" . CMS_L_COMMAND_EDIT . "\">" .

    //save command
         "<input name=\"forum_edit_message\" " .
                "type=\"submit\" " .
                "value=\"" . CMS_L_COMMAND_SAVE . "\">" .
         "</div>" . //p

         "</form>");

    insert("bottom_preview");
    permission([CMS_FORUM_PERMISSION_READER . ".$instance"   => CMS_L_READ,
                CMS_FORUM_PERMISSION_WRITER . ".$instance"   => CMS_L_WRITE,
                CMS_FORUM_PERMISSION_OPERATOR . ".$instance" => CMS_L_OPERATOR]);

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

    $error = CMS_L_MOD_FORUM_023;
    break;

case "____add":
//..................................................................................................

    $success = CMS_L_MOD_FORUM_022;
    break;

case "___edit":
//..................................................................................................

    if ($forum->edit($forum_index,
                     $forum_edit_title,
                     $forum_edit_text,
                     $forum_edit_email))
    {
        $success = CMS_L_MOD_FORUM_024;
        break;
    };

    $error = CMS_L_MOD_FORUM_025;
    break;

case "insert":
//..................................................................................................

    if ($forum->move($forum_buffer, $forum_index))
    {
        $forum_buffer = NULL;
        $success      = CMS_L_MOD_FORUM_026;
        break;
    };

    $error = CMS_L_MOD_FORUM_027;
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
        $success     = CMS_L_MOD_FORUM_019;
        break;
    };

    $error = CMS_L_MOD_FORUM_013;
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
             CMS_L_COMMAND_CUT .
             "</a>");

    //insert
    if ($forum->test_move($forum_buffer, $index))
        echo("<a href=\"" . x(cms_url(["forum_index"   => $index,
                                       "forum_buffer"  => $forum_buffer,
                                       "forum_message" => "insert"])) .
                            "#forum\" " .
                "class=\"button\">" .
             CMS_L_COMMAND_PASTE .
             "</a>");

    //delete
    if ($forum->test_delete($index))
        echo("<a href=\"" . x(cms_url(["forum_index"   => $index,
                                       "forum_message" => "delete"])) .
                            "#forum\" " .
                "class=\"button\" " .
                "onclick=\"return confirm('" . q(CMS_L_MOD_FORUM_011) . "');\">" .
             CMS_L_COMMAND_DELETE .
             "</a>");

    //edit
    if ($forum->test_edit($index))
        echo("<a href=\"" . x(cms_url(["forum_index"   => $index,
                                       "forum_message" => "edit"])) .
                            "#forum\" " .
                "class=\"button\">" .
             CMS_L_COMMAND_EDIT .
             "</a>");

    //add
    if ($forum->test_add($index))
    {
        echo("<a href=\"" . x(cms_url(["forum_index"   => $index,
                                       "forum_message" => "add"])) .
                            "#forum\" " .
                "class=\"button\">");
        if (! $index)
            echo(CMS_L_MOD_FORUM_003);
        elseif (! $forum->mysql->get($index, CMS_DB_FORUM_CONTAINER, $forum->table))
            echo(CMS_L_MOD_FORUM_010);
        else
            echo(CMS_L_MOD_FORUM_006);
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
         sprintf(CMS_L_MOD_FORUM_021, x($buffer)) .
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
         CMS_L_COMMAND_PREVIOUS .
         "</a>");

//overview
if ($forum_index || nstre($forum_search) || nstre($forum_user))
    echo("<a href=\"" . x(cms_url(["forum_buffer" => $forum_buffer])) . "#forum\" " .
            "class=\"button\">" .
         CMS_L_MOD_FORUM_012 .
         "</a>");

//search
if ($forum_buffer)
    echo("<input name=\"forum_buffer\" " .
                "type=\"hidden\" " .
                "value=\"" . x($forum_buffer) . "\">");
echo("<input name=\"forum_search\" " .
            "type=\"text\" " .
            "value=\"" . x($forum_search) . "\" " .
            "placeholder=\"" . CMS_L_MOD_FORUM_004 . "\">" .

     "</div>"); //p

//current path
if ($path_depth > 1)
{
    echo("<nav class=\"forum-path p\">" .

         "<a href=\"" . x(cms_url(["forum_buffer" => $forum_buffer])) . "#forum\">" .
         CMS_L_MOD_FORUM_012 .
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
        $template->title .= " – " . CMS_L_MOD_FORUM_004;

    //title
    echo("<h2>" .
         CMS_L_MOD_FORUM_005 .
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

        //seek beginning of page
        mysql_data_seek($result, $page * 10);
        $i = 0;

        //display search results
        while (($resultrow = mysql_fetch_assoc($result)) && ($i++ < 10))
        {
            //search result item
            echo("<div class=\"forum-search-item\">" .

            //title
                 "<h3 class=\"forum-title\">" .
                 "<a href=\"" . x(cms_url(["forum_index"  => $resultrow[CMS_DB_FORUM_INDEX],
                                           "forum_buffer" => $forum_buffer])) .
                                "#forum\">" .
                 x($resultrow[CMS_DB_FORUM_TITLE]) .
                 "</a>" .
                 "</h3>" .

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
                 sprintf(CMS_L_MOD_FORUM_015,
                         "<a href=\"" . x(cms_url(["forum_user"   => $user,
                                                   "forum_buffer" => $forum_buffer])) .
                                        "#forum\">" .
                         x($name) .
                         "</a>") .
                 "</span>" . //forum-name

            //count
                 " | " .
                 "<span class=\"forum-count\">" .
                 sprintf(CMS_L_MOD_FORUM_016, $resultrow["count"]) .
                 "</span> " .

                 "</div>"); //forum-info

            //text
            $text  = stripspaces(remove_format($resultrow[CMS_DB_FORUM_TEXT], TRUE, TRUE));
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
            CMS_L_MOD_FORUM_007,
            "forum_page");
    }

    //not found
    else
    {
        echo(CMS_L_MOD_FORUM_008);
    };

    echo("</div>"); //forum-search-result
}

//--------------------------------------------------------------------------------------------------

//user

elseif (nstre($forum_user))
{
    //set meta data
    if ($template !== NULL)
        $template->title .= " – " . CMS_L_MOD_FORUM_012;

    //title
    echo("<h2>" .
         x(permission_get_name($forum_user)) .
         "</h2>" .

         "<div class=\"forum-user\">");

    //retrieve user posts
    $query  = "SELECT * " .
              "FROM `" .         $table . "` AS `t1` " .
              "LEFT JOIN (" .

              "SELECT " .        CMS_DB_FORUM_CONTAINER . ", " .
                                 "COUNT(*) AS `count` " .
              "FROM `" .         $table . "` " .
              "GROUP BY " .      CMS_DB_FORUM_CONTAINER . ") AS `t2` " .

              "ON `t1`." .       CMS_DB_FORUM_INDEX .
              "=`t2`." .         CMS_DB_FORUM_CONTAINER . " " .
              "WHERE `t1`." .    CMS_DB_FORUM_CONTAINER .
              ">'" .            "0' " .
              "AND `t1`." .      CMS_DB_FORUM_USER .
              "='" .             sqlesc($forum_user) . "' " .
              "ORDER BY `t1`." . CMS_DB_FORUM_TIME . " DESC";
    $result = mysql_query($query);

    if (($result !== FALSE) && ($count = mysql_num_rows($result)))
    {
        //pagination values
        $_count = ceil($count / 10) - 1;
        if ($_count < 0)         $_count = 0;
        $page   = (int)$forum_page;
        if ($page < 0)           $page   = 0;
        elseif ($page > $_count) $page   = $_count;

        //seek beginning of page
        mysql_data_seek($result, $page * 10);
        $i = 0;

        //display user posts
        while (($resultrow = mysql_fetch_assoc($result)) && ($i++ < 10))
        {
            //user post item
            echo("<div class=\"forum-user-item\">" .

            //title
                 "<h3 class=\"forum-title\">" .
                 "<a href=\"" . x(cms_url(["forum_index"  => $resultrow[CMS_DB_FORUM_INDEX],
                                           "forum_buffer" => $forum_buffer])) .
                                "#forum\">" .
                 x($resultrow[CMS_DB_FORUM_TITLE]) .
                 "</a>" .
                 "</h3>" . //forum-title

            //info
                 "<div class=\"forum-info p\">" .

            //date
                 "<span class=\"forum-date\">" .
                 x(friendly_date($resultrow[CMS_DB_FORUM_TIME])) .
                 "</span>" .

            //count
                 " | " .
                 "<span class=\"forum-count\">" .
                 sprintf(CMS_L_MOD_FORUM_016, $resultrow["count"]) .
                 "</span> " .

                 "</div>" . //forum-info

            //text
                 "<div class=\"forum-text p\">" .
                 x(first_words(stripspaces(remove_format($resultrow[CMS_DB_FORUM_TEXT], TRUE, TRUE)))) .
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
            CMS_L_MOD_FORUM_007,
            "forum-pagination");
    }

    //not found
    else
    {
        echo(CMS_L_MOD_FORUM_020);
    };

    echo("</div>"); //forum-user
}

//--------------------------------------------------------------------------------------------------

//overview

elseif (! $forum_index)
{
    //set meta data
    if ($template !== NULL)
        $template->title .= " – " . CMS_L_MOD_FORUM_012;

    //display overview
    $query  = "SELECT * " .
              "FROM `" .         $table . "` AS `t1` " .
              "LEFT JOIN (" .

              "SELECT " .        CMS_DB_FORUM_CONTAINER . ", " .
                                 "COUNT(*) AS `count` " .
              "FROM `" .         $table . "` " .
              "GROUP BY " .      CMS_DB_FORUM_CONTAINER . ") AS `t2` " .

              "ON `t1`." .       CMS_DB_FORUM_INDEX .
              "=`t2`." .         CMS_DB_FORUM_CONTAINER . " " .
              "WHERE `t1`." .    CMS_DB_FORUM_CONTAINER . "='0' " .
              "ORDER BY `t1`." . CMS_DB_FORUM_TITLE;
    $result = mysql_query($query);

    echo("<div class=\"forum-overview\">");

    while ($resultrow = mysql_fetch_assoc($result))
    {
        $url = x(cms_url(["forum_index"  => $resultrow[CMS_DB_FORUM_INDEX],
                          "forum_buffer" => $forum_buffer])) .
               "#forum";

        //overview item
        echo("<div class=\"forum-item\">" .

        //topic
             "<div class=\"forum-topic\">");

        //image
        $image = get_first_image($resultrow[CMS_DB_FORUM_TEXT]);
        if ($image !== NULL)
            echo("<div class=\"forum-image\">" .
                 "<a href=\"$url\">" .
                 image(translate_url($image),          //url
                       NULL,                           //width
                       NULL,                           //height
                       $resultrow[CMS_DB_FORUM_TITLE], //alt
                       NULL,                           //style
                       TRUE) .                         //defer
                 "</a>" .
                 "</div>"); //forum-image

        //title
        echo("<h2 class=\"forum-title\">" .
             "<a href=\"$url\">" .
             x($resultrow[CMS_DB_FORUM_TITLE]) .
             "</a>" .
             "</h2>" . //forum-title

        //teaser
             "<div class=\"forum-text p\">" .
             x(first_paragraph(stripspaces(remove_format($resultrow[CMS_DB_FORUM_TEXT], TRUE, TRUE), TRUE))) . " " .
             "</div>" .

             "</div>"); //forum-topic

        //latest posts
        if ($resultrow["count"])
        {
            $query   = "SELECT * " .
                       "FROM `" .         $table . "` AS `t1` " .
                       "LEFT JOIN (" .

                       "SELECT " .        CMS_DB_FORUM_CONTAINER . ", " .
                                          "COUNT(*) AS `count` " .
                       "FROM `" .         $table . "` " .
                       "GROUP BY " .      CMS_DB_FORUM_CONTAINER . ") AS `t2` " .

                       "ON `t1`." .       CMS_DB_FORUM_INDEX .
                       "=`t2`." .         CMS_DB_FORUM_CONTAINER . " " .
                       "WHERE `t1`." .    CMS_DB_FORUM_CONTAINER .
                       "='" .             sqlesc($resultrow[CMS_DB_FORUM_INDEX]) . "' " .
                       "ORDER BY `t1`." . CMS_DB_FORUM_TIME . " DESC " .
                       "LIMIT 3";
            $_result = mysql_query($query);

            while ($_resultrow = mysql_fetch_assoc($_result))
            {
                //post summary
                echo("<div class=\"forum-post\">" .

                //title
                     "<div class=\"forum-title\">" .
                     "<a href=\"" . x(cms_url(["forum_index"  => $_resultrow[CMS_DB_FORUM_INDEX],
                                               "forum_buffer" => $forum_buffer])) .
                                    "#forum\">" .
                     x($_resultrow[CMS_DB_FORUM_TITLE]) .
                     "</a>" .
                     "</div>" . //forum-title

                //info
                     "<div class=\"forum-info p\">" .

                //date
                     "<span class=\"forum-date\">" .
                     x(friendly_date($_resultrow[CMS_DB_FORUM_TIME])) .
                     "</span> ");

                //name
                $user = $_resultrow[CMS_DB_FORUM_USER];
                $name = permission_get_name($user);

                echo("<span class=\"forum-name\">" .
                     sprintf(CMS_L_MOD_FORUM_015,
                             "<a href=\"" . x(cms_url(["forum_user"   => $user,
                                                      "forum_buffer" => $forum_buffer])) .
                                            "#forum\">" .
                             x($name) .
                             "</a>") .
                     "</span>" . //forum-name

                //count
                     " | " .
                     "<span class=\"forum-count\">" .
                     sprintf(CMS_L_MOD_FORUM_016, $_resultrow["count"]) .
                     "</span> " .

                     "</div>" . //forum-info
                     "</div>"); //forum-post
            };
        };

        //command
        $command($resultrow[CMS_DB_FORUM_INDEX]);

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
        $template->title       .= " – " . CMS_L_MOD_FORUM_030;
        $template->description  = CMS_L_MOD_FORUM_029;
        unset($template->query_data["forum_index"]);
    };

    //post item
    echo("<div class=\"forum-item\">" .

    //post
         "<div class=\"forum-post\">" .

    //title
         "<h2 class=\"forum-title\">" .
         CMS_L_MOD_FORUM_030 .
         "</h2>" .

    //text
         "<div class=\"forum-text p\">" .
         CMS_L_MOD_FORUM_029 .
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
                 "FROM `" .      $table . "` AS `t1` " .
                 "LEFT JOIN (" .

                 "SELECT " .     CMS_DB_FORUM_CONTAINER . ", " .
                                 "COUNT(*) AS `count` " .
                 "FROM `" .      $table . "` " .
                 "GROUP BY " .   CMS_DB_FORUM_CONTAINER . ") AS `t2` " .

                 "ON `t1`." .    CMS_DB_FORUM_INDEX .
                 "=`t2`." .      CMS_DB_FORUM_CONTAINER . " " .
                 "WHERE `t1`." . CMS_DB_FORUM_INDEX .
                 "='" .          sqlesc($forum_index) . "' " .
                 "LIMIT 1";
    $result    = mysql_query($query);
    $resultrow = mysql_fetch_assoc($result);

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
    if ($resultrow["count"])
    {
        $query   = "SELECT * " .
                   "FROM `" .         $table . "` AS `t1` " .
                   "LEFT JOIN (" .

                   "SELECT " .        CMS_DB_FORUM_CONTAINER . ", " .
                                      "COUNT(*) AS `count` " .
                   "FROM `" .         $table . "` " .
                   "GROUP BY " .      CMS_DB_FORUM_CONTAINER . ") AS `t2` " .

                   "ON `t1`." .       CMS_DB_FORUM_INDEX .
                   "=`t2`." .         CMS_DB_FORUM_CONTAINER . " " .
                   "WHERE `t1`." .    CMS_DB_FORUM_CONTAINER .
                   "='" .             sqlesc($forum_index) . "' " .
                   "ORDER BY `t1`." . CMS_DB_FORUM_TIME . " DESC";
        $_result = mysql_query($query);
        $count   = mysql_num_rows($_result);

        //pagination values
        $_count = ceil($count / 10) - 1;
        if ($_count < 0)         $_count = 0;
        $page   = (int)$forum_page;
        if ($page < 0)           $page   = 0;
        elseif ($page > $_count) $page   = $_count;

        //seek beginning of page
        mysql_data_seek($_result, $page * 10);
        $i = 0;

        //display replies
        while (($_resultrow = mysql_fetch_assoc($_result)) && ($i++ < 10))
        {
            //post
            echo("<div class=\"forum-post\">" .

            //title
                 "<h3 class=\"forum-title\">" .
                 "<a href=\"" . x(cms_url(["forum_index"  => $_resultrow[CMS_DB_FORUM_INDEX],
                                           "forum_buffer" => $forum_buffer])) .
                                "#forum\">" .
                 x($_resultrow[CMS_DB_FORUM_TITLE]) .
                 "</a>" .
                 "</h3>" . //forum-title

            //info
                 "<div class=\"forum-info p\">" .

            //date
                 "<span class=\"forum-date\">" .
                 x(friendly_date($_resultrow[CMS_DB_FORUM_TIME])) .
                 "</span> ");

            //name
            $user = $_resultrow[CMS_DB_FORUM_USER];
            $name = permission_get_name($user);

            echo("<span class=\"forum-name\">" .
                 sprintf(CMS_L_MOD_FORUM_015,
                         "<a href=\"" . x(cms_url(["forum_user"   => $user,
                                                   "forum_buffer" => $forum_buffer])) .
                                        "#forum\">" .
                         x($name) .
                         "</a>") .
                 "</span>" . //forum-name

            //count
                 " | " .
                 "<span class=\"forum-count\">" .
                 sprintf(CMS_L_MOD_FORUM_016, $_resultrow["count"]) .
                 "</span> " .

                 "</div>" . //forum-post

            //teaser
                 "<div class=\"forum-text p\">" .
                 x(first_words(stripspaces(remove_format($resultrow[CMS_DB_FORUM_TEXT], TRUE, TRUE)))) . " " .
                 "</div>");

            $command($_resultrow[CMS_DB_FORUM_INDEX]);

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
            CMS_L_MOD_FORUM_007,
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
                 "FROM `" .      $table . "` AS `t1` " .
                 "LEFT JOIN (" .

                 "SELECT " .     CMS_DB_FORUM_CONTAINER . ", " .
                                 "COUNT(*) AS `count` " .
                 "FROM `" .      $table . "` " .
                 "GROUP BY " .   CMS_DB_FORUM_CONTAINER . ") AS `t2` " .

                 "ON `t1`." .    CMS_DB_FORUM_INDEX .
                 "=`t2`." .      CMS_DB_FORUM_CONTAINER . " " .
                 "WHERE `t1`." . CMS_DB_FORUM_INDEX .
                 "='" .          sqlesc($forum_index) . "' " .
                 "LIMIT 1";
    $result    = mysql_query($query);
    $resultrow = mysql_fetch_assoc($result);

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
         sprintf(CMS_L_MOD_FORUM_015,
                 "<a href=\"" . x(cms_url(["forum_user"   => $user,
                                           "forum_buffer" => $forum_buffer])) .
                                "#forum\">" .
                 x($name) .
                 "</a>") .
         "</span>" . //forum-name

    //count
         " | " .
         "<span class=\"forum-count\">" .
         sprintf(CMS_L_MOD_FORUM_016, $resultrow["count"]) .
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
    if ($resultrow["count"])
    {
        $query   = "SELECT * " .
                   "FROM `" .         $table . "` AS `t1` " .
                   "LEFT JOIN (" .

                   "SELECT " .        CMS_DB_FORUM_CONTAINER . ", " .
                                      "COUNT(*) AS `count` " .
                   "FROM `" .         $table . "` " .
                   "GROUP BY " .      CMS_DB_FORUM_CONTAINER . ") AS `t2` " .

                   "ON `t1`." .       CMS_DB_FORUM_INDEX .
                   "=`t2`." .         CMS_DB_FORUM_CONTAINER . " " .
                   "WHERE `t1`." .    CMS_DB_FORUM_CONTAINER .
                   "='" .             sqlesc($forum_index) . "' " .
                   "ORDER BY `t1`." . CMS_DB_FORUM_TIME;
        $_result = mysql_query($query);
        $count   = mysql_num_rows($_result);

        //pagination values
        $_count = ceil($count / 10) - 1;
        if ($_count < 0)         $_count = 0;
        $page   = (int)$forum_page;
        if ($page < 0)           $page   = 0;
        elseif ($page > $_count) $page   = $_count;

        //seek beginning of page
        mysql_data_seek($_result, $page * 10);
        $i = 0;

        //display replies
        while (($_resultrow = mysql_fetch_assoc($_result)) && ($i++ < 10))
        {
            //reply
            echo("<div class=\"forum-reply\">" .

            //title
                 "<h3 class=\"forum-title\">" .
                 "<a href=\"" . x(cms_url(["forum_index"  => $_resultrow[CMS_DB_FORUM_INDEX],
                                           "forum_buffer" => $forum_buffer])) .
                                "#forum\">" .
                 x($_resultrow[CMS_DB_FORUM_TITLE]) .
                 "</a>" .
                 "</h3>" . //forum-title

            //info
                 "<div class=\"forum-info p\">" .

            //date
                 "<span class=\"forum-date\">" .
                 x(friendly_date($_resultrow[CMS_DB_FORUM_TIME])) .
                 "</span> ");

            //name
            $user = $_resultrow[CMS_DB_FORUM_USER];
            $name = permission_get_name($user);

            echo("<span class=\"forum-name\">" .
                 sprintf(CMS_L_MOD_FORUM_015,
                         "<a href=\"" . x(cms_url(["forum_user"   => $user,
                                                   "forum_buffer" => $forum_buffer])) .
                                        "#forum\">" .
                         x($name) .
                         "</a>") .
                 "</span>" . //forum-name

            //count
                 " | " .
                 "<span class=\"forum-count\">" .
                 sprintf(CMS_L_MOD_FORUM_016, $_resultrow["count"]) .
                 "</span> " .

                 "</div>" . //forum-info

            //text
                 "<div class=\"forum-text p\">" .
                 parse_text($_resultrow[CMS_DB_FORUM_TEXT]) .
                 "</div>");

            $command($_resultrow[CMS_DB_FORUM_INDEX]);

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
            CMS_L_MOD_FORUM_007,
            "forum-pagination");
    };

    echo("</div>"); //forum-item
};

echo("</form>");

insert("bottom");
permission([CMS_FORUM_PERMISSION_READER . ".$instance"   => CMS_L_READ,
            CMS_FORUM_PERMISSION_WRITER . ".$instance"   => CMS_L_WRITE,
            CMS_FORUM_PERMISSION_OPERATOR . ".$instance" => CMS_L_OPERATOR]);

echo("</section>"); //forum