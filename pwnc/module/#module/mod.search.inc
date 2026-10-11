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

global $search_message,
       $search_term,
       $search_language,
       $search_page,
       $search_submit_url,
       $search_submit_message;

if (
     ! cms_load("search")
     ||
     ! ($search = new search())->enabled
   )
{
    echo(t("msg_unavailable"));
    return;
};

switch ($search_message)
{
case "submit":
//..................................................................................................

    if (! cms_permission(CMS_SEARCH_PERMISSION_SUBMIT))     break;
    if (streq($search_submit_message, t("command_cancel"))) break;

    echo("<section id=\"search\" " .
                  "class=\"search-submit\">");

    insert("submit_top");

    echo("<form action=\"" . x(cms_url(["search_message" => "submit"])) . "#search\" " .
               "method=\"post\">" .

         "<h2>" .
         t("submit_page") .
         "</h2>");

    if ($search_submit_message !== "")
        $search->queue_add($search_submit_url, CMS_SEARCH_QUEUE_TYPE_SUBMISSION);

    echo("<div class=\"p\">" .
         "<label for=\"search-submit-url\">" .
         t("url") .
         "</label><br>" .
         "<input id=\"search-submit-url\" " .
                "name=\"search_submit_url\" " .
                "type=\"text\" " .
                "value=\"https://\">" .
         "</div>" . //p

         "<div class=\"p\">" .
         "<input name=\"search_submit_message\" " .
                "type=\"submit\" " .
                "value=\"" . t("command_cancel") . "\">" .
         "<input name=\"search_submit_message\" " .
                "type=\"submit\" " .
                "value=\"" . t("submit") . "\">" .
         "</div>" . //p

         "</form>");

    insert("submit_bottom");
    permission([""                           => t("read"),
                CMS_SEARCH_PERMISSION_SUBMIT => t("write")]);

    echo("</section>"); //search
    return;
};

echo("<section id=\"search\" " .
              "class=\"search\">");

insert("top");

echo("<form name=\"search_input\" " .
           "action=\"" . x(cms_url()) . "#search\" " .
           "method=\"post\">");

//reset command
if (
     ($result = mysql_query("SELECT COUNT(*) FROM " . CMS_DB_SEARCH_ENTRY . " " .
                            "WHERE " . CMS_DB_SEARCH_ENTRY_ERROR . "='0'"))
     &&
     ($resultrow = mysql_fetch_row($result))
   )
    echo("<h2>" .
         sprintf(t("search_in_pages"), $resultrow[0]) .
         "</h2>");

echo("<div class=\"p\">");

//language
if (stre($search_language)) $search_language = CMS_LANGUAGE;
if (stre($search_language)) $search_language = "#any";
$_search_language = streq($search_language, "#any") ? NULL : $search_language;

$data  = new data("#system/language");
$array = [t("all_languages") => "#any"];

$data->move("first");
while ($key = $data->move("next"))
    $array[$data->get($key, "name")] = $key;

asort($array, SORT_NATURAL | SORT_FLAG_CASE);

select($array, $search_language, "search_language", TRUE);

//search term input
echo("<input name=\"search_term\" " .
            "type=\"text\" " .
            "value=\"" . x($search_term) . "\" " .
            "placeholder=\"" . t("search_term") . "\">");

//submit command
echo("<input type=\"submit\" " .
            "value=\"" . t("search") . "\">" .
     "</div>"); //p

if (nstre($search_term))
{
    //ensure selected page is integer >= 0
    $search_page = max((int)$search_page, 0);

    //execute search query
    $result = $search->find($search_term, $search_page, $_search_language);

    echo("<div class=\"search-result p\">");

    //no results
    if ($result === FALSE)
    {
        echo(($search_page > 0) ? t("no_more_results") : t("no_results"));
    }
    else
    {
        //display results
        echo("<ol start=\"" . ($search_page * $search->find_results_per_page + 1) . "\">");

        foreach ($result AS $value)
        {
            extract($value);

            echo("<li>" .

            //title
                 "<a href=\"" . x(cms_url($address)) . "\">" .
                 "<strong>" .
                 x($title) .
                 "</strong>" .
                 "</a><br>" .

            //text
                 $text . "<br>" .

            //address, date
                 "<small>" .
                 sprintf(t("result_info"),
                         x($address),
                         x(friendly_date($time)),
                         number_format($score, 2,
                                       t("decimal_separator"),
                                       t("thousand_separator"))));

            //number of similar pages
            if ($supplemental > 0)
                echo(sprintf(t("similar_pages"), $supplemental));
            echo("</small>" .
                 "</li>");
        };

        echo("</ol>");
    };

    echo("</div>"); //search-result

    //result page navigation
    $flag = FALSE;
    if (($search_page > 0) || ($result !== FALSE))
        echo("<nav class=\"search-pagination p\">");

    //previous pages
    for ($i = 0; $i < $search_page; $i++)
    {
        $offset = $i * $search->find_results_per_page;
        $end    = $offset + $search->find_results_per_page;
        $offset++;

        if ($flag) echo(" | ");
        else       $flag = TRUE;

        echo("<a href=\"" . x(cms_url(["search_term"     => $search_term,
                                       "search_language" => $search_language,
                                       "search_page"     => $i])) .
                            "#search\">" .
             "$offset - $end" .
             "</a>");
    };

    //further results
    if ($result !== FALSE)
        echo(($flag ? " | " : "") .
             "<a href=\"" . x(cms_url(["search_term"     => $search_term,
                                       "search_language" => $search_language,
                                       "search_page"     => $search_page + 1])) .
                            "#search\">" .
             t("more_results") .
             "</a>");

    if (($search_page > 0) || ($result !== FALSE))
        echo("</nav>"); //search-pagination
};

//display tag cloud of weightiest words
if (stre($search_term))
{
    $tag = $search->tag(NULL, 100, $_search_language);

    if (is_array($tag) && (count($tag) > 0))
    {
        echo("<div class=\"search-cloud p\">");

        $size_max   = 40;
        $size_min   = 15;
        $score_max  = max($tag);
        $score_min  = min($tag);
        $score_diff = $score_max - $score_min;
        $factor     = ($size_max - $size_min) / $score_diff;
        $array      = array_keys($tag);

        //order by tag
        sort($array, SORT_NATURAL | SORT_FLAG_CASE);

        foreach ($array AS $value)
        {
            $score   = $tag[$value] - $score_min;
            $size    = $size_min + round($factor * $score);
            $opacity = round(0.35 + 0.65 * $score / $score_max, 2);

            echo("<a href=\"" . x(cms_url(["search_term"     => $value,
                                           "search_language" => $search_language])) .
                                "#search\" " .
                "style=\"FONT-SIZE:" . x($size / 20) . "em;" .
                    "LINE-HEIGHT:100%;" .
                    "OPACITY:$opacity\">" .
            x($value) .
            "</a> ");
        };

        echo("</div>"); //search-cloud
    };
};

//submit page command
if (cms_permission(CMS_SEARCH_PERMISSION_SUBMIT))
    echo("<div class=\"p\">" .
         "<a href=\"" . x(cms_url(["search_message" => "submit"])) . "#search\" " .
        "class=\"button\">" .
    t("submit_page") .
    "</a>" .
    "</div>");

echo("</form>");

insert("bottom");
permission([""                           => t("read"),
            CMS_SEARCH_PERMISSION_SUBMIT => t("write")]);

echo(jscript("document.search_input.search_term.focus();") .
     "</section>"); //search