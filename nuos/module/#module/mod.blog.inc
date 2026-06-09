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

global $blog_message,
       $blog_index,
       $blog_date,
       $blog_meta,
       $blog_page,
       $blog_edit_message,
       $blog_edit_title,
       $blog_edit_meta,
       $blog_edit_status,
       $blog_edit_time_day,
       $blog_edit_time_month,
       $blog_edit_time_year,
       $blog_edit_time_hour,
       $blog_edit_time_minute,
       $blog_edit_sticky,
       $blog_edit_text,
       $blog_code_edit_message,
       $blog_code_edit_position,
       $blog_code_edit_text;

//load libraries
if (! (cms_load("blog") && cms_load("comment")))
{
    echo(CMS_MSG_UNAVAILABLE);
    return;
};

//template
$template = $GLOBALS["blog"]["template"] ?? NULL;
if ($template !== NULL)
    $template->query_data =
        array_merge($template->query_data,
                    ["blog_index" => $blog_index]);

//instance
if (isset($GLOBALS["blog"]["instance"]) && nstre($GLOBALS["blog"]["instance"]))
    $instance = $GLOBALS["blog"]["instance"];
else
    $instance = defined("CMS_CONTENT_INDEX") ? CMS_CONTENT_INDEX : "";

$blog = new blog($instance);
if (! $blog->enabled)
{
    echo(CMS_MSG_UNAVAILABLE);
    return;
};

//validate access permissions
if (! ($blog->reader || $blog->writer || $blog->operator)) return;

//current time
$time = time();

//--------------------------------------------------------------------------------------------------

//message

$error   = [];
$success = "";

switch ($blog_message)
{
case "_edit":
//..................................................................................................

    switch ($blog_edit_message)
    {
    case CMS_L_COMMAND_DELETE:
//..................................................................................................

        if (nstre($blog_index))
        {
            if ($blog->delete($blog_index))
            {
                //delete comments
                mysql_query("DELETE FROM " . CMS_DB_COMMENT . " " .
                            "WHERE " .       CMS_DB_COMMENT_INSTANCE .
                            "='" .           sqlesc("$instance.$blog_index") . "'");

                $blog_index = NULL;
                $success    = CMS_L_MOD_BLOG_006;
            }
            else
            {
                $error[]    = CMS_L_MOD_BLOG_012;
            };
        };

    case CMS_L_COMMAND_CANCEL:
//..................................................................................................

        $blog_message = NULL;
        break;

    case CMS_L_MOD_BLOG_005:
//..................................................................................................

        $blog_message = NULL;

    case CMS_L_COMMAND_SAVE:
//..................................................................................................

        if ((int)$blog_edit_status === 2)
        {
            $status = CMS_DB_BLOG_STATUS_ACTIVE;
            $_time  = mktime($blog_edit_time_hour,
                             $blog_edit_time_minute,
                             0,
                             $blog_edit_time_month,
                             $blog_edit_time_day,
                             $blog_edit_time_year);
        }
        else
        {
            $status = $blog_edit_status;
            $_time  = $time;
        };

        $sticky = $blog_edit_sticky ? CMS_DB_BLOG_STICKY_ON : CMS_DB_BLOG_STICKY_OFF;

        if (stre($blog_index))
        {
            $index = $blog->add($blog_edit_title,
                                $blog_edit_meta,
                                $blog_edit_text,
                                $status,
                                $_time,
                                $sticky);

            if ($index !== FALSE)
            {
                $blog_index = $index;
                $blog_date  = NULL;
                $blog_meta  = NULL;
                $blog_page  = NULL;
                $success    = CMS_L_MOD_BLOG_008;
            }
            else
            {
                $error[]    = CMS_L_MOD_BLOG_014;
            };
        }
        else
        {
            if ($blog->edit($blog_index,
                            $blog_edit_title,
                            $blog_edit_meta,
                            $blog_edit_text,
                            $status,
                            $_time,
                            $sticky))
                $success = CMS_L_MOD_BLOG_007;
            else
                $error[] = CMS_L_MOD_BLOG_013;
        };

        break;
    };

    break;

case "_code_edit":
//..................................................................................................

    switch ($blog_code_edit_message)
    {
    case CMS_L_COMMAND_CANCEL:

        break;

    case CMS_L_COMMAND_SAVE:

        if ($blog->code_set($blog_code_edit_position,
                            $blog_code_edit_text))
            $success = CMS_L_MOD_BLOG_017;
        else
            $error[] = CMS_L_MOD_BLOG_018;

        break;
    };

    $blog_message = NULL;
    break;
};

//--------------------------------------------------------------------------------------------------

echo("<section id=\"blog\" " .
              "class=\"blog\">" .
     "<div class=\"blog-content\">");

//display errors
if (count($error))
    echo("<div class=\"response-error\">" .
         implode("<br>", $error) .
         "</div>");

//display success
if (nstre($success))
    echo("<div class=\"response-success\">" .
         $success .
         "</div>");

//--------------------------------------------------------------------------------------------------

switch ($blog_message)
{
case "edit":
case "_edit":
//..................................................................................................

    //retrieve article
    $result = mysql_query("SELECT * " .
                          "FROM " .  CMS_DB_BLOG . " " .
                          "WHERE " . CMS_DB_BLOG_INSTANCE .
                          "='" .     sqlesc($instance) . "' " .
                          "AND " .   CMS_DB_BLOG_INDEX .
                          "='" .     sqlesc($blog_index) . "' " .
                          "LIMIT 1");
    if ($result === FALSE) break; //error
    $resultrow = mysql_fetch_assoc($result);
    if (! $resultrow)      break; //article not found

    $status           = $resultrow[CMS_DB_BLOG_STATUS];
    $_time            = $resultrow[CMS_DB_BLOG_TIME];
    $blog_edit_sticky = $resultrow[CMS_DB_BLOG_STICKY];
    $blog_edit_title  = $resultrow[CMS_DB_BLOG_TITLE];
    $blog_edit_meta   = $resultrow[CMS_DB_BLOG_META];
    $blog_edit_text   = $resultrow[CMS_DB_BLOG_TEXT];

case "add":
//..................................................................................................

    //set meta data
    if ($template !== NULL)
        $template->title .= " – " . CMS_L_MOD_BLOG_002;

    //process status and time
    switch ($blog_message)
    {
    case "add":

        $blog_edit_status = CMS_DB_BLOG_STATUS_ACTIVE;
        $date             = getdate($time);
        break;

    case "edit":
    case "_edit":

        if (((int)$status === CMS_DB_BLOG_STATUS_ACTIVE))
        {
            $blog_edit_status = 2;
            $date             = getdate($_time);
            break;
        };

        $blog_edit_status = $status;
        $date             = getdate($time);
        break;
    };

    $blog_edit_time_day    = $date["mday"];
    $blog_edit_time_month  = $date["mon"];
    $blog_edit_time_year   = $date["year"];
    $blog_edit_time_hour   = $date["hours"];
    $blog_edit_time_minute = $date["minutes"];

    //time selection array
    for ($i = 1; $i <= 31; $i++) $array_day[sprintf("%02d", $i)] = $i;
    $array_month =
        [CMS_L_JANUARY   =>  1,
         CMS_L_FEBRUARY  =>  2,
         CMS_L_MARCH     =>  3,
         CMS_L_APRIL     =>  4,
         CMS_L_MAY       =>  5,
         CMS_L_JUNE      =>  6,
         CMS_L_JULY      =>  7,
         CMS_L_AUGUST    =>  8,
         CMS_L_SEPTEMBER =>  9,
         CMS_L_OCTOBER   => 10,
         CMS_L_NOVEMBER  => 11,
         CMS_L_DECEMBER  => 12];
    $i = $date["year"] - 10;
    $c = date("Y", $time) + 10;
    if ($i < 1970) $i = 1970;
    if ($c > 2037) $c = 2037;
    for (; $i <= $c; $i++)      $array_year[sprintf("%04d", $i)]   = $i;
    for ($i = 0; $i < 24; $i++) $array_hour[sprintf("%02d", $i)]   = $i;
    for ($i = 0; $i < 60; $i++) $array_minute[sprintf("%02d", $i)] = $i;

    //command
    echo("<div class=\"p\">" .

    //overview command
         "<a href=\"" . x(cms_url(["blog_date" => $blog_date,
                                   "blog_meta" => $blog_meta,
                                   "blog_page" => $blog_page])) .
                        "#blog\" " .
            "class=\"button\">" .
         CMS_L_MOD_BLOG_011 .
         "</a>");

    //delete command
    if ($blog->test_delete($blog_index) && nstreq($blog_message, "add"))
        echo("<a href=\"" . x(cms_url(["blog_message"      => "_edit",
                                       "blog_index"        => $blog_index,
                                       "blog_date"         => $blog_date,
                                       "blog_meta"         => $blog_meta,
                                       "blog_page"         => $blog_page,
                                       "blog_edit_message" => CMS_L_COMMAND_DELETE])) .
                            "#blog\" " .
                "class=\"button\" " .
                "onclick=\"return confirm('" . CMS_L_MOD_BLOG_029 . "');\">" .
             CMS_L_COMMAND_DELETE .
             "</a>");
    echo("</div>" . //p

    //display add form
         "<div class=\"blog-edit\">" .
         "<form name=\"blog_edit\" " .
               "action=\"" . x(cms_url(["blog_message" => "_edit",
                                        "blog_index"   => $blog_index,
                                        "blog_date"    => $blog_date,
                                        "blog_meta"    => $blog_meta,
                                        "blog_page"    => $blog_page])) .
                             "#blog\" " .
               "method=\"post\">" .

    //header
         "<h2>" .
         CMS_L_MOD_BLOG_002 .
         "</h2>" .

    //title
         "<div class=\"p\">" .
         "<label for=\"blog-edit-title\">" .
         CMS_L_MOD_BLOG_003 .
         "</label>" .
         "<input id=\"blog-edit-title\" " .
                "name=\"blog_edit_title\" " .
                "type=\"text\" " .
                "value=\"" . x($blog_edit_title) . "\">" .
         "</div>" . //p

    //tags
         "<div class=\"p\">" .
         "<label for=\"blog-edit-meta\">" .
         CMS_L_MOD_BLOG_004 .
         "</label>" .
         "<div class=\"blog-edit-meta\">" .
         "<input id=\"blog-edit-meta\" " .
                "name=\"blog_edit_meta\" " .
                "type=\"text\" " .
                "value=\"" . x($blog_edit_meta) . "\">");

    //retrieve tag suggestions
    $query = "SELECT " .    "c." . CMS_DB_BLOG_META_TERM_TEXT . " " .
             "FROM " .      CMS_DB_BLOG .           " AS a " .

             "LEFT JOIN " . CMS_DB_BLOG_META_LINK . " AS b " .
             "ON " .        "b." . CMS_DB_BLOG_META_LINK_ARTICLE .
             "=" .          "a." . CMS_DB_BLOG_INDEX . " " .

             "LEFT JOIN " . CMS_DB_BLOG_META_TERM . " AS c " .
             "ON " .        "c." . CMS_DB_BLOG_META_TERM_INDEX .
             "=" .          "b." . CMS_DB_BLOG_META_LINK_TERM . " " .

             "WHERE " .     "a." . CMS_DB_BLOG_INSTANCE .
             "='" .         sqlesc($instance) . "' " .
             "AND " .       "c." . CMS_DB_BLOG_META_TERM_TEXT . " " .
             "IS NOT NULL " .

             "GROUP BY " .  "b." . CMS_DB_BLOG_META_LINK_TERM . " " .
             "ORDER BY " .  "COUNT(b." . CMS_DB_BLOG_META_LINK_TERM . ") DESC";
    $result = mysql_query($query);

    //display tag suggestions
    if (($result !== FALSE) && (mysql_num_rows($result) > 0))
    {
        echo("<div class=\"blog-edit-meta-suggestion\">");

?><script>

function blog_tag(value)
{
    var object = document.getElementById("blog-edit-meta");
    if (object.value.match(/\S/)) object.value += ", ";
    object.value += value;
    return false;
};

</script><?php

        while ($resultrow = mysql_fetch_row($result))
            echo("<a href=\"#\" " .
                    "onclick=\"return blog_tag(this.textContent);\">" .
                 x($resultrow[0]) .
                 "</a> ");
        echo("</div>"); //blog-edit-meta-suggestion
    };

    echo("</div>" . //blog-edit-meta
         "</div>" . //p

    //publication options
         "<fieldset>" .
         "<legend>" .
         CMS_L_MOD_BLOG_022 .
         "</legend>" .

    //immediate publication
         "<label>" .
         "<input name=\"blog_edit_status\" " .
                "type=\"radio\" " .
                "value=\"" . CMS_DB_BLOG_STATUS_ACTIVE . "\"" .
                (((int)$blog_edit_status === CMS_DB_BLOG_STATUS_ACTIVE) ? " checked" : "") . ">" .
         CMS_L_MOD_BLOG_023 .
         "</label> " .

    //scheduled publication
         "<label>" .
         "<input name=\"blog_edit_status\" " .
                "type=\"radio\" " .
                "value=\"2\" " .
                (((int)$blog_edit_status === 2) ? " checked" : "") . ">" .
         CMS_L_MOD_BLOG_042 .
         "</label>" .

    //no publication
         "<label>" .
         "<input name=\"blog_edit_status\" " .
                "type=\"radio\" " .
                "value=\"" . x(CMS_DB_BLOG_STATUS_INACTIVE) . "\" " .
                (((int)$blog_edit_status === CMS_DB_BLOG_STATUS_INACTIVE) ? " checked" : "") . ">" .
         CMS_L_MOD_BLOG_024 .
         "</label> " .

    //sticky
         "<label>" .
         "<input name=\"blog_edit_sticky\" " .
                "type=\"checkbox\" " .
                "value=\"" . x(CMS_DB_BLOG_STICKY_ON) . "\" " .
                (((int)$blog_edit_sticky === CMS_DB_BLOG_STICKY_ON) ? " checked" : "") . ">" .
         CMS_L_MOD_BLOG_037 .
         "</label><br>" .

         "<label>" .
         CMS_L_MOD_BLOG_025 . "<br>");
    select($array_day, $blog_edit_time_day, "blog_edit_time_day", TRUE);
    echo("</label>" .
         "<label>" .
         CMS_L_MOD_BLOG_026 . "<br>");
    select($array_month, $blog_edit_time_month, "blog_edit_time_month", TRUE);
    echo("</label>" .
         "<label>" .
         CMS_L_MOD_BLOG_027 . "<br>");
    select($array_year, $blog_edit_time_year, "blog_edit_time_year", TRUE);
    echo("</label>" .
         "<label>" .
         CMS_L_MOD_BLOG_028 . "<br>");
    select($array_hour, $blog_edit_time_hour, "blog_edit_time_hour", TRUE);
    echo("</label>" .
         "<label>" .
         CMS_L_MOD_BLOG_039 . "<br>");
    select($array_minute, $blog_edit_time_minute, "blog_edit_time_minute", TRUE);
    echo("</label>");

?><script>

document.blog_edit.blog_edit_time_day.onchange    =
document.blog_edit.blog_edit_time_month.onchange  =
document.blog_edit.blog_edit_time_year.onchange   =
document.blog_edit.blog_edit_time_hour.onchange   =
document.blog_edit.blog_edit_time_minute.onchange =
() => { document.blog_edit.blog_edit_status[1].click(); };

</script><?php

    echo("</fieldset>" .

    //text
         "<div class=\"p\">" .
         "<label for=\"blog-edit-text\">" .
         CMS_L_MOD_BLOG_040 .
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
                                          "'[name=blog_edit_text]'," .
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
                                          "'[name=blog_edit_text]'," .
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
                                          "'[name=blog_edit_text]'," .
                                          "'#token'," .
                                          "'%return%');"],
                                     TRUE)) . "');";
            $extension .= "<a href=\"" . x($buffer) . "\" " .
                             "title=\"token\">" .
                          image("textcontrol/button_token_external") .
                          "</a>";
        };
    };

    echo(jscript("textcontrol(" .
                 "\"[name=blog_edit_text]\", " .
                 "\"" . q(CMS_IMAGES_URL) . "\"" .
                 (nstre($extension) ? ", \"" . q($extension) . "\"" : "") . ");") .
         "<textarea id=\"blog-edit-text\" " .
                   "name=\"blog_edit_text\" " .
                   "rows=\"20\">\n" .
         x($blog_edit_text) .
         "</textarea>" .
         "</div>" . //p

    //command
         "<div class=\"p\">" .

    //cancel button
         "<input name=\"blog_edit_message\" " .
                "type=\"submit\" " .
                "value=\"" . CMS_L_COMMAND_CANCEL . "\">" .

    //save button
         "<input name=\"blog_edit_message\" " .
                "type=\"submit\" " .
                "value=\"" . CMS_L_COMMAND_SAVE . "\">" .

    //save and preview button
         "<input name=\"blog_edit_message\" " .
                "type=\"submit\" " .
                "value=\"" . CMS_L_MOD_BLOG_005 . "\">" .
         "</div>" . //p

         "</form>" .
         "</div>" .     //blog-edit
         "</div>" .     //blog-content
         "</section>"); //blog

    return;

case "code_edit":
//..................................................................................................

    //set meta data
    if ($template !== NULL)
        $template->title .= " – " . CMS_L_MOD_BLOG_016;

    //display code form
    echo("<div class=\"blog-code-edit\">" .
         "<form name=\"blog_code_edit\" " .
               "action=\"" . x(cms_url(["blog_message" => "_code_edit",
                                        "blog_index"   => $blog_index,
                                        "blog_date"    => $blog_date,
                                        "blog_meta"    => $blog_meta,
                                        "blog_page"    => $blog_page])) .
                             "#blog\" " .
               "method=\"post\">" .

    //header
         "<h2>" .
         CMS_L_MOD_BLOG_016 .
         "</h2>" .

    //code
         "<div class=\"p\">" .
         "<label for=\"blog-code-edit-text\">" .
         CMS_L_MOD_BLOG_041 .
         "</label><br>" .
         "<input name=\"blog_code_edit_position\" " .
                "type=\"hidden\" " .
                "value=\"" . x($blog_code_edit_position) . "\">" .
         "<textarea id=\"blog-code-edit-text\" " .
                   "name=\"blog_code_edit_text\" " .
                   "rows=\"20\">\n" .
         x($blog->code_get($blog_code_edit_position)) .
         "</textarea><br>" .

    //placeholders
         CMS_L_MOD_BLOG_032 . ": ");
    $array = [CMS_L_URL          => "%url%",
              CMS_L_MOD_BLOG_003 => "%title%"];
    if (nstreq($blog_code_edit_position, CMS_DB_BLOG_CODE_POSITION_CONTROL))
    {
        $array[CMS_L_MOD_BLOG_034] = "%author%";
        $array[CMS_L_MOD_BLOG_035] = "%user%";
    };
    foreach ($array AS $key => $value)
        echo("<a href=\"#\" " .
                "onclick=\"textcontrol_set(" .
                "'[name=blog_code_edit_text]'," .
                "'#replace'," .
                "'" . qx($value) . "');" .
                "return false;\">" .
             x($key) .
             "</a> ");
    echo("</div>" . //p

    //command
         "<div class=\"p\">" .

    //cancel button
         "<input name=\"blog_code_edit_message\" " .
                "type=\"submit\" " .
                "value=\"" . CMS_L_COMMAND_CANCEL . "\">" .

    //save button
         "<input name=\"blog_code_edit_message\" " .
                "type=\"submit\" " .
                "value=\"" . CMS_L_COMMAND_SAVE . "\">" .
         "</div>" . //p

         "</form>" .
         "</div>" .     //blog-code-edit
         "</div>" .     //blog-content
         "</section>"); //blog

    return;

case "rss":
//..................................................................................................

    //close output buffer
    ob_end_clean();

    //send rss content type header
    header("Content-Type: application/rss+xml; charset=utf-8");

    //caching
    $cache_key  = "blog.rss.$instance." . CMS_LANGUAGE;
    $cache_time = cms_cache_time($cache_key);

    if (($cache_time !== FALSE) && ($cache_time > ($time - 60))) //60 seconds delay
    {
        echo(cms_cache_notouch($cache_key));
        exit();
    };

    //start output
    $buffer  = "<?xml version=\"1.0\" " .
                     "encoding=\"utf-8\"?>" .
               "<rss version=\"2.0\">" .
               "<channel>";

    //title
    $value   = isset($GLOBALS["blog"]["rss_title"]) ?
               $GLOBALS["blog"]["rss_title"] : CMS_L_MOD_BLOG_031;
    $buffer .= "<title>" . x($value) . "</title>";

    //link
    $buffer .= "<link>" . x(u()) . "</link>";

    //description
    $value   = isset($GLOBALS["blog"]["rss_description"]) ?
               $GLOBALS["blog"]["rss_description"] : "";
    $buffer .= "<description>" . x($value) . "</description>";

    //language
    $buffer .= "<language>" . x(CMS_LANGUAGE) . "</language>";

    //retrieve articles
    $query  = "SELECT * " .
              "FROM " .     CMS_DB_BLOG . " " .
              "WHERE " .    CMS_DB_BLOG_INSTANCE .
              "='" .        sqlesc($instance) . "' " .
              "AND " .      CMS_DB_BLOG_STATUS .
              "='" .        CMS_DB_BLOG_STATUS_ACTIVE . "' " .
              "AND " .      CMS_DB_BLOG_TIME .
              "<='" .       sqlesc($time) . "' " .
              "ORDER BY " . CMS_DB_BLOG_TIME . " DESC " .
              "LIMIT 50";
    $result = mysql_query($query);

    //publication date
    if ($resultrow = mysql_fetch_assoc($result))
    {
        $value   = date("r", $resultrow[CMS_DB_BLOG_TIME]);
        $buffer .= "<pubDate>" . x($value) . "</pubDate>";
    };

    //last build date
    $value   = date("r");
    $buffer .= "<lastBuildDate>" . x($value) . "</lastBuildDate>";

    //category
    if (
         isset($GLOBALS["blog"]["rss_category"])
         &&
         nstre($value = $GLOBALS["blog"]["rss_category"])
       )
        $buffer .= "<category>" . x($value) . "</category>";

    //generator
    $buffer .= "<generator>" . x(CMS_IDENTIFIER) . "</generator>";

    //list articles
    $_time = $time - 259200; //3 days

    while ($resultrow)
    {
        $buffer .= "<item>";

        //title
        $value   = $resultrow[CMS_DB_BLOG_TITLE];
        if ($resultrow[CMS_DB_BLOG_TIME] > $_time)
            $value .= " (" . friendly_date($resultrow[CMS_DB_BLOG_TIME]) . ")";
        $buffer .= "<title>" . x($value) . "</title>";

        //link
        $value   = u(["blog_index" => $resultrow[CMS_DB_BLOG_INDEX]]);
        $buffer .= "<link>" . x($value) . "</link>";

        //description
        $value   = first_paragraph(stripspaces(remove_format($resultrow[CMS_DB_BLOG_TEXT], TRUE, TRUE), TRUE));
        $buffer .= "<description>" . x($value) . "</description>";

        //enclosure
        $value   = get_first_image($resultrow[CMS_DB_BLOG_TEXT]);
        if ($value !== NULL)
        {
            $url = translate_url($value);
            $url = image_process($url, 500, 500);

            //local
            if (($path = image_path($url)) !== FALSE)
            {
                $length = filesize($path);
                $type   = get_mime_type($path);
            }

            //remote
            elseif ($value = get_headers($url))
            {
                $length = isset($value["Content-Length"]) ? $value["Content-Length"] : FALSE;
                $type   = isset($value["Content-Length"]) ? $value["Content-Type"]   : FALSE;
            };

            if (($length !== FALSE) && ($type !== FALSE))
                $buffer .= "<enclosure url=\"" .    x($url) .    "\" " .
                                      "length=\"" . x($length) . "\" " .
                                      "type=\"" .   x($type) .   "\"/>";
        };

        //guid
        $value   = $resultrow[CMS_DB_BLOG_INDEX];
        $buffer .= "<guid isPermaLink=\"false\">" . x($value) . "</guid>";

        //publication date
        $value   = date("r", $resultrow[CMS_DB_BLOG_TIME]);
        $buffer .= "<pubDate>" . x($value) . "</pubDate>";

        $buffer .= "</item>";

        //fetch next article
        $resultrow = mysql_fetch_assoc($result);
    };

    $buffer .= "</channel>" .
               "</rss>";

    //cache permanently
    cms_cache($cache_key, $buffer, TRUE);

    echo($buffer);
    exit();
};

//--------------------------------------------------------------------------------------------------

//article

while (nstre($blog_index))
{
    echo("<div class=\"p\">" .

    //overview command
         "<a href=\"" . x(cms_url(["blog_date" => $blog_date,
                                   "blog_meta" => $blog_meta,
                                   "blog_page" => $blog_page])) .
                        "#blog\" " .
        "class=\"button\">" .
    CMS_L_MOD_BLOG_011 .
    "</a>");

    $buffer = "";

    //filter active
    if (! ($blog->writer || $blog->operator))
        $buffer = "AND " . CMS_DB_BLOG_STATUS .
                  "='" .   CMS_DB_BLOG_STATUS_ACTIVE . "' " .
                  "AND " . CMS_DB_BLOG_TIME .
                  "<='" .  sqlesc($time) . "' ";

    //retrieve article
    $result = mysql_query("SELECT * " .
                          "FROM " .  CMS_DB_BLOG . " " .
                          "WHERE " . CMS_DB_BLOG_INSTANCE .
                          "='" .     sqlesc($instance) . "' " .
                          "AND " .   CMS_DB_BLOG_INDEX .
                          "='" .     sqlesc($blog_index) . "' " .
                          $buffer .
                          "LIMIT 1");

    //error
    if ($result === FALSE)
    {
        //set meta data
        if ($template !== NULL)
        {
            $template->title       .= " – " . CMS_L_MOD_BLOG_044;
            $template->description  = CMS_L_MOD_BLOG_045;
        };

        echo("</div>" . //p

             "<div class=\"response-error\">" .
             CMS_L_MOD_BLOG_045 .
             "</div>" . //response-error
             "</div>"); //blog-content
        break;
    };

    //article not found
    if (! $resultrow = mysql_fetch_assoc($result))
    {
        //set gone status
        http_response_code(410);

        //set meta data
        if ($template !== NULL)
        {
            $template->title       .= " – " . CMS_L_MOD_BLOG_043;
            $template->description  = CMS_L_MOD_BLOG_036;
            unset($template->query_data["blog_index"]);
        };

        echo("</div>" . //p

             "<article class=\"blog-article\">" .
             "<h2>" .
             CMS_L_MOD_BLOG_043 .
             "</h2>" .
             "<div class=\"p\">" .
             CMS_L_MOD_BLOG_036 .
             "</div>" .     //p
             "</article>" . //blog-article
             "</div>");     //blog-content
        break;
    };

    //delete command
    if ($blog->test_delete($blog_index))
        echo("<a href=\"" . x(cms_url(["blog_message"      => "_edit",
                                       "blog_index"        => $blog_index,
                                       "blog_date"         => $blog_date,
                                       "blog_meta"         => $blog_meta,
                                       "blog_edit_message" => CMS_L_COMMAND_DELETE])) .
                            "#blog\" " .
                "class=\"button\" " .
                "onclick=\"return confirm('" . CMS_L_MOD_BLOG_029 . "');\">" .
             CMS_L_COMMAND_DELETE .
             "</a>");

    //edit command
    if ($blog->test_edit($blog_index))
        echo("<a href=\"" . x(cms_url(["blog_message" => "edit",
                                       "blog_index"   => $blog_index,
                                       "blog_date"    => $blog_date,
                                       "blog_meta"    => $blog_meta])) .
                            "#blog\" " .
                "class=\"button\">" .
             CMS_L_MOD_BLOG_009 .
             "</a>");

    echo("</div>" . //p

         "<article class=\"blog-article\">");

    //set meta data
    if ($template !== NULL)
    {
        $template->title       .= " – " . $resultrow[CMS_DB_BLOG_TITLE];
        $template->description  = first_words(stripspaces(remove_format($resultrow[CMS_DB_BLOG_TEXT], TRUE, TRUE)));
        $template->keyword      = $resultrow[CMS_DB_BLOG_META];
    };

    insert("top_article");

    //placeholders
    $replacement = ["url"    => u(["blog_index" => $blog_index]),
                    "title"  => $resultrow[CMS_DB_BLOG_TITLE],
                    "author" => permission_get_name($resultrow[CMS_DB_BLOG_OWNER]),
                    "user"   => $resultrow[CMS_DB_BLOG_OWNER]];

    //code
    echo("<aside class=\"blog-code p\">");

    //edit code command
    if ($blog->test_code_set())
        echo("<div class=\"p\">" .
             "<a href=\"" . x(cms_url(["blog_message"            => "code_edit",
                                       "blog_index"              => $blog_index,
                                       "blog_date"               => $blog_date,
                                       "blog_meta"               => $blog_meta,
                                       "blog_page"               => $blog_page,
                                       "blog_code_edit_position" => CMS_DB_BLOG_CODE_POSITION_BEFORE])) .
                            "#blog\" " .
                "class=\"button\">" .
             CMS_L_MOD_BLOG_016 .
             "</a>" .
             "</div>"); //p

    echo($blog->code_parse(CMS_DB_BLOG_CODE_POSITION_BEFORE, $replacement) .
         "</aside>" . //blog-code

         "<header>" .

    //title
         "<h2 class=\"blog-article-title\">" .
         x($resultrow[CMS_DB_BLOG_TITLE]) .
         "</h2>" .

    //info
         "<div class=\"blog-article-info\">" .

    //time
         "<time datetime=\"" . x(date("c", $resultrow[CMS_DB_BLOG_TIME])) . "\" " .
               "class=\"blog-article-time\">" .
         x(friendly_date($resultrow[CMS_DB_BLOG_TIME])) .
         "</time> " .

    //author
         "<span class=\"blog-article-author\">" .
         sprintf(CMS_L_MOD_BLOG_033, x($replacement["author"])) .
         "</span>" .

         "</div>" . //blog-article-info

         "</header>" .

    //text
         "<div class=\"blog-article-text p\">" .
         parse_text($resultrow[CMS_DB_BLOG_TEXT]) .
         "</div>" .

    //code
         "<aside class=\"blog-code p\">");

    //edit code command
    if ($blog->test_code_set())
        echo("<div class=\"p\">" .
             "<a href=\"" . x(cms_url(["blog_message"            => "code_edit",
                                       "blog_index"              => $blog_index,
                                       "blog_date"               => $blog_date,
                                       "blog_meta"               => $blog_meta,
                                       "blog_page"               => $blog_page,
                                       "blog_code_edit_position" => CMS_DB_BLOG_CODE_POSITION_AFTER])) .
                            "#blog\" " .
                "class=\"button\">" .
             CMS_L_MOD_BLOG_016 .
             "</a>" .
             "</div>"); //p

    echo($blog->code_parse(CMS_DB_BLOG_CODE_POSITION_AFTER, $replacement) .
         "</aside>"); //blog-code

    insert("bottom_article");

    echo("</article>");

    //comments
    $GLOBALS["comment"]["instance"] = "$instance.$blog_index";

    echo("<div class=\"blog-comment\">");
    cms_param($blog_index, "blog_index");
    cms_param($blog_date,  "blog_date");
    cms_param($blog_meta,  "blog_meta");
    cms_param($blog_page,  "blog_page");
    cms_application("comment");
    cms_param("", "blog_index");
    cms_param("", "blog_date");
    cms_param("", "blog_meta");
    cms_param("", "blog_page");
    echo("</div>" . //blog-comment
         "</div>"); //blog-content
    break;
};

//--------------------------------------------------------------------------------------------------

//command

if (stre($blog_index))
{
    //write article
    if ($blog->test_add())
        echo("<div class=\"p\">" .
             "<a href=\"" . x(cms_url(["blog_message" => "add",
                                       "blog_date"    => $blog_date,
                                       "blog_meta"    => $blog_meta,
                                       "blog_page"    => $blog_page])) .
                            "#blog\" " .
                "class=\"button\">" .
             CMS_L_MOD_BLOG_002 .
             "</a>" .
             "</div>"); //p
};

//--------------------------------------------------------------------------------------------------

//overview

if (stre($blog_index))
{
    //set meta data
    if ($template !== NULL)
        $template->title .= " – " . CMS_L_MOD_BLOG_011;

    $buffer = "";

    //filter active
    if (! ($blog->writer || $blog->operator))
        $buffer = "AND " . "a." . CMS_DB_BLOG_STATUS .
                  "='" .   CMS_DB_BLOG_STATUS_ACTIVE . "' " .
                  "AND " . "a." . CMS_DB_BLOG_TIME .
                  "<='" .  sqlesc($time) . "' ";

    //archive selection
    if (nstre($blog_date))
    {
        $year  = (int)substr($blog_date, 0, 4);
        $month = (int)substr($blog_date, 4, 2);
        $day   = (int)substr($blog_date, 6, 2);

        if ($year !== 0)
            $buffer .= "AND YEAR(FROM_UNIXTIME(" .  "a." . CMS_DB_BLOG_TIME . "))" .
                       "='" .                       sqlesc($year) . "' ";
        if ($month !== 0)
            $buffer .= "AND MONTH(FROM_UNIXTIME(" . "a." . CMS_DB_BLOG_TIME . "))" .
                       "='" .                       sqlesc($month) . "' ";
        if ($day !== 0)
            $buffer .= "AND DAY(FROM_UNIXTIME(" .   "a." . CMS_DB_BLOG_TIME . "))" .
                       "='" .                       sqlesc($day) . "' ";
    };

    //meta selection
    if (nstre($blog_meta))
        $buffer .= "AND " . "b." . CMS_DB_BLOG_META_LINK_TERM .
                   "='" .   sqlesc($blog_meta) . "' ";

    //get number of articles
    $query = "SELECT COUNT(DISTINCT a." . CMS_DB_BLOG_INDEX . ") " .
             "FROM " .      CMS_DB_BLOG .           " AS a " .
             "LEFT JOIN " . CMS_DB_BLOG_META_LINK . " AS b " .
             "ON " .        "b." . CMS_DB_BLOG_META_LINK_ARTICLE .
             "=" .          "a." . CMS_DB_BLOG_INDEX . " " .
             "WHERE " .     "a." . CMS_DB_BLOG_INSTANCE .
             "='" .         sqlesc($instance) . "' " .
             $buffer;

    $result = mysql_query($query);
    $count  = ($result === FALSE) ? 0 : mysql_result($result, 0);
    $_count = ceil($count / 10) - 1;
    if ($_count < 0)         $_count = 0;
    $page   = (int)$blog_page;
    if ($page < 0)           $page   = 0;
    elseif ($page > $_count) $page   = $_count;

    $comment = new comment($instance);

    while ($count > 0)
    {
        $query = "SELECT " .     "a.* " .
                 ($comment->reader ?
                              ", COUNT(DISTINCT c." . CMS_DB_COMMENT_INDEX . ") AS `#count` " : "") .
                 "FROM " .       CMS_DB_BLOG .           " AS a " .
                 "LEFT JOIN " .  CMS_DB_BLOG_META_LINK . " AS b " .
                 "ON " .         "b." . CMS_DB_BLOG_META_LINK_ARTICLE .
                 "=" .           "a." . CMS_DB_BLOG_INDEX . " " .
                 ($comment->reader ?
                 "LEFT JOIN " .  CMS_DB_COMMENT .        " AS c " .
                 "ON " .         "c." . CMS_DB_COMMENT_STATUS .
                 "='" .          sqlesc(CMS_DB_COMMENT_STATUS_ACTIVE) . "' " .
                 "AND " .        "c." . CMS_DB_COMMENT_INSTANCE .
                 "=CONCAT_WS(" . "'.', " .
                                 "'" . sqlesc($instance) . "', " .
                                 "a." . CMS_DB_BLOG_INDEX . ") " .
                 "WHERE " .      "a." . CMS_DB_BLOG_INSTANCE .
                 "='" .          sqlesc($instance) . "' " : "WHERE 1 ") .
                 $buffer .
                 "GROUP BY " .   "a." . CMS_DB_BLOG_INDEX . " " .
                 "ORDER BY " .   "a." . CMS_DB_BLOG_STICKY . " DESC, " .
                                 "a." . CMS_DB_BLOG_TIME .   " DESC " .
                 "LIMIT " .      sqlesc($page * 10) . ", 10";

        $result = mysql_query($query);
        if ($result === FALSE) break; //error

        echo("<div class=\"blog-overview\">");

        insert("top_overview");

        while ($resultrow = mysql_fetch_assoc($result))
        {
            $url = x(cms_url(["blog_index" => $resultrow[CMS_DB_BLOG_INDEX],
                              "blog_date"  => $blog_date,
                              "blog_meta"  => $blog_meta,
                              "blog_page"  => ($page > 0) ? $page : NULL]));

            if ((int)$resultrow[CMS_DB_BLOG_STATUS] === CMS_DB_BLOG_STATUS_ACTIVE)
                $class  = ($resultrow[CMS_DB_BLOG_TIME] > $time) ?
                          "blog-overview-article-scheduled" :
                          "blog-overview-article";
            else
                $class  = "blog-overview-article-disabled";

            if ((int)$resultrow[CMS_DB_BLOG_STICKY] === CMS_DB_BLOG_STICKY_ON)
                $class .= "-sticky";

            echo("<article class=\"$class\">");

            //image
            $image = get_first_image($resultrow[CMS_DB_BLOG_TEXT]);
            if ($image !== NULL)
                echo("<div class=\"blog-overview-image\">" .
                     "<a href=\"$url#blog\">" .
                     image(translate_url($image),         //url
                           NULL,                          //width
                           NULL,                          //height
                           $resultrow[CMS_DB_BLOG_TITLE], //title
                           NULL,                          //style
                           TRUE) .                        //defer
                     "</a>" .
                     "</div>"); //blog-overview-image

            //title
            echo("<h2 class=\"blog-overview-title\">" .
                 "<a href=\"$url#blog\">" .
                 x($resultrow[CMS_DB_BLOG_TITLE]) .
                 "</a>" .
                 "</h2>" .

            //info
                 "<div class=\"blog-overview-info\">" .

            //time
                 "<time datetime=\"" . x(date("c", $resultrow[CMS_DB_BLOG_TIME])) . "\" " .
                       "class=\"blog-overview-time\">" .
                 x(friendly_date($resultrow[CMS_DB_BLOG_TIME])) .
                 "</time> " .

            //author
                 "<span class=\"blog-overview-author\">" .
                 sprintf(CMS_L_MOD_BLOG_033, x(permission_get_name($resultrow[CMS_DB_BLOG_OWNER]))) .
                 "</span>");

            if ($comment->reader && ($resultrow["#count"] > 0))
                 echo(" | " .
                      "<a href=\"$url#comment\">" .
                      sprintf(CMS_L_MOD_BLOG_038, $resultrow["#count"]) .
                      "</a>");

            echO("</div>" . //blog-overview-info

            //teaser
                 "<div class=\"blog-overview-text p\">" .
                 x(first_paragraph(stripspaces(remove_format($resultrow[CMS_DB_BLOG_TEXT], TRUE, TRUE), TRUE))) . " " .
                 "<a href=\"$url#blog\">" .
                 CMS_L_MOD_BLOG_010 .
                 "</a>" .
                 "</div>"); //blog-overview-text

            //command
            $buffer = "";

            //delete command
            if ($blog->test_delete($blog_index))
                $buffer .= "<a href=\"" . x(cms_url(["blog_message"      => "_edit",
                                                     "blog_index"        => $resultrow[CMS_DB_BLOG_INDEX],
                                                     "blog_date"         => $blog_date,
                                                     "blog_meta"         => $blog_meta,
                                                     "blog_page"         => $blog_page,
                                                     "blog_edit_message" => CMS_L_COMMAND_DELETE])) .
                                          "#blog\" " .
                              "class=\"button\" " .
                              "onclick=\"return confirm('" . CMS_L_MOD_BLOG_029 . "');\">" .
                           CMS_L_COMMAND_DELETE .
                           "</a>";

            //edit command
            if ($blog->test_edit($blog_index))
                $buffer .= "<a href=\"" . x(cms_url(["blog_message" => "edit",
                                                     "blog_index"   => $resultrow[CMS_DB_BLOG_INDEX],
                                                     "blog_date"    => $blog_date,
                                                     "blog_meta"    => $blog_meta,
                                                     "blog_page"    => $blog_page])) .
                                          "#blog\" " .
                              "class=\"button\">" .
                           CMS_L_MOD_BLOG_009 .
                           "</a>";

            if (nstre($buffer))
                echo("<div class=\"p\">" .
                     ltrim($buffer) .
                     "</div>"); //p

            //code
            echo("<aside class=\"blog-code p\">");

            //edit code command
            if ($blog->test_code_set())
                echo("<div class=\"p\">" .
                     "<a href=\"" . x(cms_url(["blog_message"            => "code_edit",
                                               "blog_date"               => $blog_date,
                                               "blog_meta"               => $blog_meta,
                                               "blog_page"               => $blog_page,
                                               "blog_code_edit_position" => CMS_DB_BLOG_CODE_POSITION_TEASER])) .
                                    "#blog\" " .
                        "class=\"button\">" .
                     CMS_L_MOD_BLOG_016 .
                     "</a>" .
                     "</div>"); //p

            echo($blog->code_parse(CMS_DB_BLOG_CODE_POSITION_TEASER,
                                   ["url"    => u(["blog_index" => $resultrow[CMS_DB_BLOG_INDEX]]),
                                    "title"  => $resultrow[CMS_DB_BLOG_TITLE],
                                    "author" => permission_get_name($resultrow[CMS_DB_BLOG_OWNER]),
                                    "user"   => $resultrow[CMS_DB_BLOG_OWNER]]) .
                 "</aside>" . //blog-code
                 "</article>");
        };

        //pagination
        pagination(
            cms_url(["blog_date" => $blog_date,
                     "blog_meta" => $blog_meta,
                     "blog_page" => "\x1B%page%"]) .
            "#blog",
            $page,
            $_count,
            CMS_L_MOD_BLOG_001,
            "blog-overview-pagination");

        insert("bottom_overview");

        echo("</div>"); //blog-overview
        break;
    };

    echo("</div>"); //blog-content
};

//--------------------------------------------------------------------------------------------------

echo("<div class=\"blog-control\">");

insert("top_control");

//archive

echo("<nav class=\"blog-archive p\">" .

     "<h2>" .
     CMS_L_MOD_BLOG_019 .
     "</h2>");

if (stre($blog_date))
{
    $year  = 0;
    $month = 0;
    $day   = 0;
}
else
{
    echo("<a href=\"" . x(cms_url(["blog_meta" => $blog_meta])) . "#blog\">" .
         CMS_L_MOD_BLOG_021 .
         "</a><br>");

    $year  = (int)substr($blog_date, 0, 4);
    $month = (int)substr($blog_date, 4, 2);
    $day   = (int)substr($blog_date, 6, 2);
};

//filter active
$buffer = "";
if (! ($blog->writer || $blog->operator))
    $buffer = "AND " . CMS_DB_BLOG_STATUS .
              "='" .   CMS_DB_BLOG_STATUS_ACTIVE . "' " .
              "AND " . CMS_DB_BLOG_TIME .
              "<='" .  sqlesc($time) . "' ";

$_time  = mktime(0, 0, 0, $month, 1, $year);

$result = mysql_query("SELECT YEAR(FROM_UNIXTIME(" . CMS_DB_BLOG_TIME . ")) AS `#year`, " .
                             "COUNT(*) AS `#count` " .
                      "FROM " .  CMS_DB_BLOG . " " .
                      "WHERE " . CMS_DB_BLOG_INSTANCE .
                      "='" .     sqlesc($instance) . "' " .
                      $buffer .
                      "GROUP BY `#year` " .
                      "ORDER BY `#year` DESC");

while ($resultrow = mysql_fetch_assoc($result))
{
    $flag = (int)$resultrow["#year"] === $year;
    $date = sprintf("%04d", $resultrow["#year"]);
    echo("<a href=\"" . x(cms_url(["blog_date" => $date,
                                   "blog_meta" => $blog_meta])) .
                        "#blog\" " .
            "title=\"" . sprintf(CMS_L_MOD_BLOG_015, $resultrow["#count"]) . "\" " .
            "class=\"" . ($flag ? "blog-archive-year-active" : "blog-archive-year") . "\">" .
         x($resultrow["#year"]) .
         "</a> ");

    //selected year
    if ($flag)
    {
        $_result = mysql_query("SELECT MONTH(FROM_UNIXTIME(" . CMS_DB_BLOG_TIME . ")) AS `#month`, " .
                                      "COUNT(*) AS `#count` " .
                               "FROM " .  CMS_DB_BLOG . " " .
                               "WHERE " . CMS_DB_BLOG_INSTANCE .
                               "='" .     sqlesc($instance) . "' " .
                               "AND YEAR(FROM_UNIXTIME(" . CMS_DB_BLOG_TIME . "))" .
                               "='" .     sqlesc($year) . "' " .
                               "GROUP BY `#month` " .
                               "ORDER BY `#month`");

        while ($_resultrow = mysql_fetch_assoc($_result))
        {
            $_flag = (int)$_resultrow["#month"] === $month;
            $date  = sprintf("%04d%02d",
                             $resultrow["#year"],
                             $_resultrow["#month"]);
            echo("<a href=\"" . x(cms_url(["blog_date" => $date,
                                           "blog_meta" => $blog_meta])) .
                                "#blog\" " .
                    "title=\"" . sprintf(CMS_L_MOD_BLOG_015, $_resultrow["#count"]) . "\" " .
                    "class=\"" . ($_flag ? "blog-archive-month-active" : "blog-archive-month") . "\">" .
                 month($_resultrow["#month"]) .
                 "</a> ");

            //selected month
            if ($_flag)
            {
                $__result = mysql_query("SELECT DAY(FROM_UNIXTIME(" . CMS_DB_BLOG_TIME . ")) AS `#day`, " .
                                               "COUNT(*) AS `#count` " .
                                        "FROM " .  CMS_DB_BLOG . " " .
                                        "WHERE " . CMS_DB_BLOG_INSTANCE .
                                        "='" .     sqlesc($instance) . "' " .
                                        "AND YEAR(FROM_UNIXTIME(" . CMS_DB_BLOG_TIME . "))" .
                                        "='" .     sqlesc($year) . "' " .
                                        "AND MONTH(FROM_UNIXTIME(" . CMS_DB_BLOG_TIME . "))" .
                                        "='" .     sqlesc($month) . "' " .
                                        "GROUP BY `#day` " .
                                        "ORDER BY `#day`");

                while ($__resultrow = mysql_fetch_assoc($__result))
                    $array[$__resultrow["#day"]] = $__resultrow["#count"];

                //weekdays
                echo("<table>" .
                     "<tr>");
                for ($i = 0; $i < 7; $i++)
                    echo("<th>" .
                         utf8_substr(weekday($i), 0, 2) .
                         "</th>");
                echo("</tr>");

                //days
                $wday = (int)date("w", $_time);
                $mday = (int)date("t", $_time);
                $i    = 1;

                while (TRUE)
                {
                    if (($i % 7) === 1) echo("<tr>");

                    echo("<td>");
                    if (($i > $wday) && (($_i = $i - $wday) <= $mday)) //within month range
                    {
                        if (isset($array[$_i]))
                        {
                            $__flag = $_i === $day;
                            $date   = sprintf("%04d%02d%02d",
                                              $resultrow["#year"],
                                              $_resultrow["#month"],
                                              $_i);
                            echo("<a href=\"" . x(cms_url(["blog_date" => $date,
                                                           "blog_meta" => $blog_meta])) .
                                                "#blog\" " .
                                    "title=\"" . sprintf(CMS_L_MOD_BLOG_015, $array[$_i]) . "\" " .
                                    "class=\"" . ($__flag ? "blog-archive-day-active" : "blog-archive-day") . "\">" .
                                 $_i .
                                 "</a>");
                        }
                        else
                        {
                            echo($_i);
                        };
                    };
                    echo("</td>");

                    if (($i % 7) === 0)
                    {
                        echo("</tr>");
                        if ($i > $mday) break;
                    };

                    $i++;
                };

                echo("</table>");
            };
        };
    };
};

echo("</nav>"); //blog-archive

//--------------------------------------------------------------------------------------------------

//meta

//filter active
$buffer = "";
if (! ($blog->writer || $blog->operator))
    $buffer = "AND " . "a." . CMS_DB_BLOG_STATUS .
              "='" .   CMS_DB_BLOG_STATUS_ACTIVE . "' " .
              "AND " . "a." . CMS_DB_BLOG_TIME .
              "<='" .  sqlesc($time) . "' ";

$query = "SELECT " .    "b." . CMS_DB_BLOG_META_LINK_TERM . ", " .
                        "c." . CMS_DB_BLOG_META_TERM_TEXT . ", " .
                        "COUNT(*) " .
         "FROM " .      CMS_DB_BLOG .           " AS a " .

         "LEFT JOIN " . CMS_DB_BLOG_META_LINK . " AS b " .
         "ON " .        "b." . CMS_DB_BLOG_META_LINK_ARTICLE .
         "=" .          "a." . CMS_DB_BLOG_INDEX . " " .

         "LEFT JOIN " . CMS_DB_BLOG_META_TERM . " AS c " .
         "ON " .        "c." . CMS_DB_BLOG_META_TERM_INDEX .
         "=" .          "b." . CMS_DB_BLOG_META_LINK_TERM . " " .

         "WHERE " .     "a." . CMS_DB_BLOG_INSTANCE .
         "='" .         sqlesc($instance) . "' " .
         $buffer .
         "AND " .       "c." . CMS_DB_BLOG_META_TERM_TEXT . " " .
         "IS NOT NULL " .

         "GROUP BY " .  "b." . CMS_DB_BLOG_META_LINK_TERM . " " .
         "ORDER BY " .  "c." . CMS_DB_BLOG_META_TERM_TEXT;

$result = mysql_query($query);
if ($result !== FALSE)
{
    echo("<nav class=\"blog-meta p\">" .

         "<h2>" .
         CMS_L_MOD_BLOG_020 .
         "</h2>");

    if (nstre($blog_meta))
        echo("<a href=\"" . x(cms_url(["blog_date" => $blog_date])) . "#blog\">" .
             CMS_L_MOD_BLOG_021 .
             "</a><br>");

    $list = [];
    $min  = PHP_INT_MAX;
    $max  = 0;

    while ($resultrow = mysql_fetch_row($result))
    {
        $list[$resultrow[0]] = [0 => $resultrow[1], $resultrow[2]];
        if ($resultrow[2] < $min) $min = $resultrow[2];
        if ($resultrow[2] > $max) $max = $resultrow[2];
    };

    $diff   = $max - $min;
    $factor = ($diff === 0) ? 0.25 : 0.5 / $diff;

    foreach ($list AS $key => $value)
    {
        $size = 0.75 + $factor * ($value[1] - $min);

        echo("<a href=\"" . x(cms_url(["blog_date" => $blog_date,
                                       "blog_meta" => $key])) .
                            "#blog\" " .
                "title=\"" . sprintf(CMS_L_MOD_BLOG_015, $value[1]) . "\" " .
                (streq($blog_meta, $key) ? "class=\"blog-meta-active\" " : "") .
                "style=\"FONT-SIZE:" . x($size) . "em\">" .
             x(utf8_ucwords($value[0])) .
             "</a> ");
    };

    echo("</nav>"); //blog-meta
};

//--------------------------------------------------------------------------------------------------

//code

echo("<aside class=\"blog-code p\">");

//edit code command
if ($blog->test_code_set())
    echo("<div class=\"p\">" .
         "<a href=\"" . x(cms_url(["blog_message"            => "code_edit",
                                   "blog_code_edit_position" => CMS_DB_BLOG_CODE_POSITION_CONTROL,
                                   "blog_index"              => $blog_index,
                                   "blog_date"               => $blog_date,
                                   "blog_meta"               => $blog_meta,
                                   "blog_page"               => $blog_page])) .
                        "#blog\" " .
            "class=\"button\">" .
         CMS_L_MOD_BLOG_016 .
         "</a>" .
         "</div>"); //p

echo($blog->code_parse(CMS_DB_BLOG_CODE_POSITION_CONTROL,
                       ["url"    => u(),
                        "title"  => isset($GLOBALS["blog"]["rss_title"]) ?
                                    $GLOBALS["blog"]["rss_title"] : CMS_L_MOD_BLOG_031]) .
     "</aside>"); //blog-code

//--------------------------------------------------------------------------------------------------

//feed
echo("<div class=\"p\">" .
     "<a href=\"" . x(u(["blog_message" => "rss"])) . "\">" .
     CMS_L_MOD_BLOG_030 .
     "</a>" .
     "</div>"); //p

insert("bottom_control");

echo("</div>"); //blog-control

permission([CMS_BLOG_PERMISSION_READER . ".$instance"   => CMS_L_READ,
            CMS_BLOG_PERMISSION_WRITER . ".$instance"   => CMS_L_WRITE,
            CMS_BLOG_PERMISSION_OPERATOR . ".$instance" => CMS_L_OPERATOR]);

echo("</section>"); //blog