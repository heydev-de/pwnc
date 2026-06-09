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
ifc_permission([""                          => CMS_L_ACCESS,
                CMS_LOG_PERMISSION_OPERATOR => CMS_L_OPERATOR]);

$log = new log();
if (! $log->enabled) ifc_inactive($ifc_page);

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "config":
//..................................................................................................

    //must be operator
    if (! $log->operator) break;

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   NULL,
                   "_config",
                   CMS_L_IFC_LOG_044);

    //logging limit
    $ifc->set(CMS_L_IFC_LOG_001, "label");
    $ifc->set([CMS_L_IFC_LOG_002 => NULL,
               CMS_L_IFC_LOG_003 => 1095,
               CMS_L_IFC_LOG_004 =>  730,
               CMS_L_IFC_LOG_005 =>  365,
               CMS_L_IFC_LOG_006 =>  180,
               CMS_L_IFC_LOG_007 =>   90,
               CMS_L_IFC_LOG_008 =>   30,
               CMS_L_IFC_LOG_009 =>   14,
               CMS_L_IFC_LOG_010 =>    7,
               CMS_L_IFC_LOG_011 =>    3,
               CMS_L_IFC_LOG_012 =>    2,
               CMS_L_IFC_LOG_013 =>    1,
               CMS_L_IFC_LOG_014 =>   -1],
              "select 40 b",
              $log->limit);

    //privacy
    $ifc->set(CMS_L_IFC_LOG_088, "title");

    //ip anonymization
    $ifc->set(CMS_L_IFC_LOG_015, "checkbox", TRUE, $log->anonymize);

    //disable user data logging
    $ifc->set(CMS_L_IFC_LOG_016, "checkbox", TRUE, $log->privacy);

    //bot detection / handling
    $ifc->set(CMS_L_IFC_LOG_079, "title");

    //bot detection threshold
    $array = [];
    for ($i = 5; $i <= 15; $i++) $array[sprintf(CMS_L_IFC_LOG_089, $i)] = $i;
    $ifc->set(CMS_L_IFC_LOG_080, "label");
    $ifc->set($array, "select 40 b", $log->bot_limit);
    $ifc->set(CMS_L_IFC_LOG_081, "info b");

    //bad bot detection threshold
    $array = [];
    for ($i = 10; $i <= 20; $i++) $array[sprintf(CMS_L_IFC_LOG_089, $i)] = $i;
    $ifc->set(CMS_L_IFC_LOG_082, "label");
    $ifc->set($array, "select 40 b", $log->bad_bot_limit);
    $ifc->set(CMS_L_IFC_LOG_083, "info b");

    //bad bot countermeasure delay
    $array = [];
    for ($i = 1; $i <= 10; $i++) $array[sprintf(CMS_L_IFC_LOG_090, $i)] = $i;
    $ifc->set(CMS_L_IFC_LOG_084, "label");
    $ifc->set($array, "select 40 b", $log->bad_bot_delay);
    $ifc->set(CMS_L_IFC_LOG_085, "info b");

    //bad bot countermeasure
    $ifc->set(CMS_L_IFC_LOG_086, "checkbox b", TRUE, $log->bad_bot_block);
    $ifc->set(CMS_L_IFC_LOG_087, "info b");

    //bot reset interval
    $ifc->set(CMS_L_IFC_LOG_092, "label");
    $ifc->set([CMS_L_IFC_LOG_013 =>   1,
               CMS_L_IFC_LOG_012 =>   2,
               CMS_L_IFC_LOG_011 =>   3,
               CMS_L_IFC_LOG_010 =>   7,
               CMS_L_IFC_LOG_009 =>  14,
               CMS_L_IFC_LOG_008 =>  30,
               CMS_L_IFC_LOG_007 =>  90,
               CMS_L_IFC_LOG_006 => 180,
               CMS_L_IFC_LOG_005 => 365],
              "select 40 b",
              $log->bot_reset);
    $ifc->set(CMS_L_IFC_LOG_093, "info b");

    //bot logging limit
    $ifc->set(CMS_L_IFC_LOG_103, "label");
    $ifc->set([CMS_L_IFC_LOG_002 => NULL,
               CMS_L_IFC_LOG_003 => 1095,
               CMS_L_IFC_LOG_004 =>  730,
               CMS_L_IFC_LOG_005 =>  365,
               CMS_L_IFC_LOG_006 =>  180,
               CMS_L_IFC_LOG_007 =>   90,
               CMS_L_IFC_LOG_008 =>   30,
               CMS_L_IFC_LOG_009 =>   14,
               CMS_L_IFC_LOG_010 =>    7,
               CMS_L_IFC_LOG_011 =>    3,
               CMS_L_IFC_LOG_012 =>    2,
               CMS_L_IFC_LOG_013 =>    1,
               CMS_L_IFC_LOG_014 =>   -1],
              "select 40 b",
              $log->bot_retention);
    $ifc->set(CMS_L_IFC_LOG_104, "info");

    $ifc->close();

case "_config":
//..................................................................................................

    //must be operator
    if (! $log->operator) break;

    //set values
    $system = new system();
    $system->setval($ifc_param1,        "log", "limit");
    $system->setval(isset($ifc_param2), "log", "anonymize");
    $system->setval(isset($ifc_param3), "log", "privacy");
    $system->setval($ifc_param4,        "log", "bot_limit");
    $system->setval($ifc_param5,        "log", "bad_bot_limit");
    $system->setval($ifc_param6,        "log", "bad_bot_delay");
    $system->setval(isset($ifc_param7), "log", "bad_bot_block");
    $system->setval($ifc_param8,        "log", "bot_reset");
    $system->setval($ifc_param9,        "log", "bot_retention");

    //save
    $ifc_response = $system->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;

    //update
    if (! $log->enabled) ifc_inactive($ifc_page);

    //clear values
    for ($i = 1; $i <= 8; $i++) unset(${"ifc_param$i"});
    break;

case "save":
//..................................................................................................

    $ifc_response = $log->set_user($object,        //userid
                                   $ifc_param7,    //name
                                   $ifc_param8,    //email
                                   $ifc_param9,    //info
                                   $ifc_param10) ? //bot
                    CMS_MSG_DONE : CMS_MSG_ERROR;
};

//increase timeout but allow abort
ignore_user_abort(FALSE);
set_time_limit(600);

$resolution = [
    "h" => "YEAR(" . CMS_DB_LOG_ACCESS_TIME . ") * 10000 + (DAYOFYEAR(" . CMS_DB_LOG_ACCESS_TIME . ") - 1) * 24 + HOUR(" . CMS_DB_LOG_ACCESS_TIME . ")",
    "d" => "YEAR(" . CMS_DB_LOG_ACCESS_TIME . ") * 1000 + DAYOFYEAR(" . CMS_DB_LOG_ACCESS_TIME . ")",
    "w" => "YEARWEEK(" . CMS_DB_LOG_ACCESS_TIME . ", 3)",
    "m" => "YEAR(" . CMS_DB_LOG_ACCESS_TIME . ") * 100 + MONTH(" . CMS_DB_LOG_ACCESS_TIME . ")",
    "q" => "YEAR(" . CMS_DB_LOG_ACCESS_TIME . ") * 10 + QUARTER(" . CMS_DB_LOG_ACCESS_TIME . ")",
    "y" => "YEAR(" . CMS_DB_LOG_ACCESS_TIME . ")"];

$interval = [
    CMS_L_IFC_LOG_018 => ["now",              "h"],
    CMS_L_IFC_LOG_019 => ["-1 week +1 day",   "d"],
    CMS_L_IFC_LOG_020 => ["-1 month +1 day",  "d"],
    CMS_L_IFC_LOG_021 => ["-3 month +1 week", "w"],
    CMS_L_IFC_LOG_022 => ["-6 month +1 week", "w"],
    CMS_L_IFC_LOG_023 => ["-1 year +1 month", "m"],
    CMS_L_IFC_LOG_024 => ["-2 year +1 month", "m"],
    CMS_L_IFC_LOG_025 => ["-3 year +3 month", "q"],
    CMS_L_IFC_LOG_026 => ["-4 year +3 month", "q"],
    CMS_L_IFC_LOG_027 => ["-5 year +3 month", "q"],
    CMS_L_IFC_LOG_028 => ["-10 year +1 year", "y"]];

$filter_field = [
    CMS_L_IFC_LOG_029 => CMS_DB_LOG_ACCESS_USERID,
    CMS_L_IFC_LOG_030 => CMS_DB_LOG_ACCESS_USER,
    CMS_L_IFC_LOG_031 => CMS_DB_LOG_ACCESS_IP,
    CMS_L_IFC_LOG_032 => CMS_DB_LOG_ACCESS_ACTION,
    CMS_L_IFC_LOG_034 => CMS_DB_LOG_ACCESS_URL,
    CMS_L_IFC_LOG_033 => CMS_DB_LOG_ACCESS_PATH,
    CMS_L_IFC_LOG_036 => CMS_DB_LOG_ACCESS_REFERRER,
    CMS_L_IFC_LOG_035 => CMS_DB_LOG_ACCESS_DOMAIN,
    CMS_L_IFC_LOG_038 => CMS_DB_LOG_ACCESS_AGENT,
    CMS_L_IFC_LOG_037 => CMS_DB_LOG_ACCESS_BROWSER,
    CMS_L_IFC_LOG_071 => CMS_DB_LOG_ACCESS_MOBILE,
    CMS_L_IFC_LOG_039 => CMS_DB_LOG_ACCESS_LANGUAGE,
    CMS_L_IFC_LOG_040 => CMS_DB_LOG_ACCESS_REGION,
    CMS_L_IFC_LOG_091 => CMS_DB_LOG_ACCESS_ERROR];

$filter_operation = [
    CMS_L_IFC_LOG_041 => " LIKE '%%%s%%'",
    CMS_L_IFC_LOG_042 => " NOT LIKE '%%%s%%'",
    CMS_L_IFC_LOG_043 => "='%s'"];

//timezone offset
if (($tz_offset = (int)date("Z")) !== 0)
{
    $temp = set_error_handler(function() { return TRUE; });
    $flag = mysql_query("SET time_zone='" . sqlesc(date_default_timezone_get()) . "'");
    set_error_handler($temp);

    if (! $flag)
    {
        $hour   = floor(abs($tz_offset) / 3600);
        $minute = floor((abs($tz_offset) / 60) % 60);
        $temp   = sprintf("%s%02d:%02d", ($tz_offset >= 0) ? "+" : "-", $hour, $minute);

        mysql_query("SET time_zone='" . sqlesc($temp) . "'");
    };
};

//change timezone
date_default_timezone_set("UTC");

if (! in_array(CMS_IFC_MESSAGE,
               ["load_raw",      "load_origin",     "load_content",  "load_activity", "load_region",
                "load_language", "load_technology", "load_identity", "load_time"]))
{
    //default
    if (isset($ifc_param1))      $ifc_param1 = strtotime($ifc_param1);
    if (stre($ifc_param1 ?? "")) $ifc_param1 = time() + $tz_offset;
    if (! isset($ifc_param2))    $ifc_param2 = 2;
    if (! isset($ifc_param3))    $ifc_param3 = 0;
    if (! isset($ifc_param4))    $ifc_param4 = 0;
    if (! isset($ifc_param5))    $ifc_param5 = NULL;
    if (! isset($ifc_param6))    $ifc_param6 = 0;

    //interval
    $temp        = array_values($interval)[$ifc_param2];
    $_interval   = $temp[0];
    $_resolution = $temp[1];

    //time interval
    $ts_start = strtotime($_interval, $ifc_param1);
    $ts_end   = $ifc_param1;
    $_ts_end  = $ifc_param1; //for view box width

    //quantize start and end dates to resolution
    switch($_resolution)
    {
    case "h":

        $ts_start = strtotime(date("Y-m-d 00:00:00", $ts_start));
        $ts_end   = strtotime(date("Y-m-d 23:59:59", $ts_end));
        $_ts_end  = strtotime(date("Y-m-d 23:00:00", $_ts_end));
        $count    = ($_ts_end - $ts_start) / 3600 + 1;
        break;

    case "d":

        $ts_start = strtotime(date("Y-m-d 00:00:00", $ts_start));
        $ts_end   = strtotime(date("Y-m-d 23:59:59", $ts_end));
        $_ts_end  = strtotime(date("Y-m-d 00:00:00", $_ts_end));
        $count    = ($_ts_end - $ts_start) / 86400 + 1;
        break;

    case "w":

        $ts_start   = strtotime("monday this week 00:00:00", $ts_start);
        $ts_end     = strtotime("sunday this week 23:59:59", $ts_end);
        $_ts_end    = strtotime("monday this week 00:00:00", $_ts_end);
        $count      = ($_ts_end - $ts_start) / (7 * 86400) + 1;
        break;

    case "m":

        $ts_start   = strtotime(date("Y-m-01 00:00:00", $ts_start));
        $ts_end     = strtotime(date("Y-m-t 23:59:59",  $ts_end));
        $_ts_end    = strtotime(date("Y-m-01 00:00:00", $_ts_end));
        $count      = (date("Y", $_ts_end) - date("Y", $ts_start)) * 12 +
                      (date("m", $_ts_end) - date("m", $ts_start)) + 1;
        break;

    case "q":

        $temp       = ceil(date("n", $ts_start) / 3) * 3 - 2;
        $ts_start   = strtotime(date("Y-$temp-01 00:00:00", $ts_start));
        $temp       = ceil(date("n", $ts_end) / 3) * 3;
        $ts_end     = strtotime(date("Y-$temp-t 23:59:59",  $ts_end));
        $temp       = ceil(date("n", $_ts_end) / 3) * 3 - 2;
        $_ts_end    = strtotime(date("Y-$temp-01 00:00:00", $_ts_end));
        $count      = (date("Y", $_ts_end) - date("Y", $ts_start)) * 4 +
                      ceil(date("n", $_ts_end) / 3) - ceil(date("n", $ts_start) / 3) + 1;
        break;

    case "y":

        $ts_start   = strtotime(date("Y-01-01 00:00:00", $ts_start));
        $ts_end     = strtotime(date("Y-12-31 23:59:59", $ts_end));
        $_ts_end    = strtotime(date("Y-01-01 00:00:00", $_ts_end));
        $count      = date("Y", $_ts_end) - date("Y", $ts_start) + 1;
        break;
    };

    $bot = $ifc_param6;
    if (nstre($ifc_param5))
    {
        $_filter_field     = $ifc_param3;
        $_filter_operation = $ifc_param4;
        $filter_value      = $ifc_param5;
    };
};

//date filter
$filter = "WHERE " .    CMS_DB_LOG_ACCESS_TIME . " " .
          "BETWEEN '" . sqlesc(date("Y-m-d H:i:s", $ts_start)) . "' " .
          "AND '" .     sqlesc(date("Y-m-d H:i:s", $ts_end)) . "' ";

//bot filter
if ($bot < 2)
    $filter .= "AND " . CMS_DB_LOG_ACCESS_BOT .
               "=" .    (int)$bot . " ";

//data filter
if (isset($filter_value))
    $filter .= "AND " . array_values($filter_field)[$_filter_field] .
               sprintf(array_values($filter_operation)[$_filter_operation],
                       sqlesc($filter_value)) . " ";

switch (CMS_IFC_MESSAGE)
{
case "load_raw":
//..................................................................................................

    $_bot = function($value)
    {
        switch ($value)
        {
        case CMS_LOG_STATUS_USER_FIXED:

            return "<span style=\"COLOR:var(--ifc-color-success)\">" .
                   CMS_L_IFC_LOG_073 .
                   "</span>";

        case CMS_LOG_STATUS_USER_PROVISIONAL:

            return "<span style=\"COLOR:var(--ifc-color-success)\">" .
                   CMS_L_IFC_LOG_074 .
                   "</span>";

        case CMS_LOG_STATUS_BOT_PROVISIONAL:

            return CMS_L_IFC_LOG_075;

        case CMS_LOG_STATUS_BOT_LIMIT_EXCEEDED:

            return CMS_L_IFC_LOG_076;

        case CMS_LOG_STATUS_BOT_FIXED:

            return CMS_L_IFC_LOG_077;

        case CMS_LOG_STATUS_BAD_BOT:

            return "<span style=\"COLOR:var(--ifc-color-error)\">" .
                   CMS_L_IFC_LOG_078 .
                   "</span>";
        };

        return "";
    };

    //retrieve data
    $query = "SELECT " .   CMS_DB_LOG_ACCESS_TIME . " AS `#datetime`, " .
                           CMS_DB_LOG_ACCESS_IP . ", " .
                           CMS_DB_LOG_ACCESS_USERID . ", " .
                           CMS_DB_LOG_ACCESS_USER . ", " .
                           CMS_DB_LOG_ACCESS_URL . ", " .
                           CMS_DB_LOG_ACCESS_ERROR . ", " .
                           CMS_DB_LOG_ACCESS_ACTION . ", " .
                           CMS_DB_LOG_ACCESS_INFO . ", " .
                           CMS_DB_LOG_ACCESS_REFERRER . ", " .
                           CMS_DB_LOG_ACCESS_BROWSER . ", " .
                           CMS_DB_LOG_ACCESS_AGENT . ", " .
                           CMS_DB_LOG_ACCESS_MOBILE . ", " .
                           CMS_DB_LOG_ACCESS_LANGUAGE . ", " .
                           CMS_DB_LOG_ACCESS_REGION . " " .
             "FROM " .     CMS_DB_LOG_ACCESS . " " .
             $filter .
             "ORDER BY " . CMS_DB_LOG_ACCESS_INDEX . " DESC " .
             "LIMIT " .    (int)$offset . ", 100";

    $query = "SELECT DATE(`#t1`.`#datetime`) AS `#date`, " .
                    "TIME_FORMAT(`#t1`.`#datetime`, '%H:%i:%s') AS `#time`, " .
                    "`#t1`." . CMS_DB_LOG_ACCESS_IP . ", " .
                    "`#t1`." . CMS_DB_LOG_ACCESS_USERID . ", " .
                    "`#t2`." . CMS_DB_LOG_USER_NAME . ", " .
                    "`#t1`." . CMS_DB_LOG_ACCESS_USER . ", " .
                    "`#t2`." . CMS_DB_LOG_USER_BOT . ", " .
                    "`#t1`." . CMS_DB_LOG_ACCESS_URL . ", " .
                    "`#t1`." . CMS_DB_LOG_ACCESS_ERROR . ", " .
                    "`#t1`." . CMS_DB_LOG_ACCESS_ACTION . ", " .
                    "`#t1`." . CMS_DB_LOG_ACCESS_INFO . ", " .
                    "`#t1`." . CMS_DB_LOG_ACCESS_REFERRER . ", " .
                    "`#t1`." . CMS_DB_LOG_ACCESS_BROWSER . ", " .
                    "`#t1`." . CMS_DB_LOG_ACCESS_AGENT . ", " .
                    "`#t1`." . CMS_DB_LOG_ACCESS_MOBILE . ", " .
                    "`#t1`." . CMS_DB_LOG_ACCESS_LANGUAGE . ", " .
                    "`#t1`." . CMS_DB_LOG_ACCESS_REGION . " " .
             "FROM ($query) AS `#t1` " .
             "LEFT JOIN " .    CMS_DB_LOG_USER . " AS `#t2` " .
             "ON `#t1`." .     CMS_DB_LOG_ACCESS_USERID .
             "=`#t2`." .       CMS_DB_LOG_USER_USERID;

    $rows = log_report($query,
                       ["#date"                    => new log_report_option(CMS_L_IFC_LOG_067,
                                                                            CMS_LOG_REPORT_OPTION_TYPE_TEXT,
                                                                            "javascript:d('%s');",
                                                                            __NAMESPACE__ ."\\q"),
                        "#time"                    => CMS_L_IFC_LOG_068,
                        CMS_DB_LOG_ACCESS_IP       => new log_report_option(CMS_L_IFC_LOG_031,
                                                                            CMS_LOG_REPORT_OPTION_TYPE_TEXT,
                                                                            "javascript:f(2,2,'%s');",
                                                                            __NAMESPACE__ ."\\q"),
                        CMS_DB_LOG_ACCESS_USERID   => new log_report_option(NULL,
                                                                            CMS_LOG_REPORT_OPTION_TYPE_NONE),
                        CMS_DB_LOG_USER_NAME       => new log_report_option(CMS_L_IFC_LOG_063,
                                                                            CMS_LOG_REPORT_OPTION_TYPE_TEXT,
                                                                            "javascript:f(0,2,'%s');",
                                                                            __NAMESPACE__ ."\\q",
                                                                            CMS_DB_LOG_ACCESS_USERID,
                                                                            __NAMESPACE__ ."\\strtocolor"),
                        CMS_DB_LOG_ACCESS_USER     => new log_report_option(CMS_L_IFC_LOG_030,
                                                                            CMS_LOG_REPORT_OPTION_TYPE_TEXT,
                                                                            "javascript:f(1,2,'%s');",
                                                                            __NAMESPACE__ ."\\q"),
                        CMS_DB_LOG_USER_BOT        => new log_report_option(CMS_L_IFC_LOG_065,
                                                                            CMS_LOG_REPORT_OPTION_TYPE_TEXT,
                                                                            "javascript:b(%d);",
                                                                            __NAMESPACE__ ."\\q",
                                                                            NULL,
                                                                            $_bot),
                        CMS_DB_LOG_ACCESS_URL      => new log_report_option(CMS_L_IFC_LOG_034,
                                                                            CMS_LOG_REPORT_OPTION_TYPE_TEXT,
                                                                            "javascript:l('%s');",
                                                                            __NAMESPACE__ ."\\q"),
                        CMS_DB_LOG_ACCESS_ERROR    => new log_report_option(CMS_L_IFC_LOG_091,
                                                                            CMS_LOG_REPORT_OPTION_TYPE_TEXT,
                                                                            "javascript:f(13,2,'%s');",
                                                                            __NAMESPACE__ ."\\q",
                                                                            NULL,
                                                                            __NAMESPACE__ ."\\yesno"),
                        CMS_DB_LOG_ACCESS_ACTION   => new log_report_option(CMS_L_IFC_LOG_032,
                                                                            CMS_LOG_REPORT_OPTION_TYPE_TEXT,
                                                                            "javascript:f(3,2,'%s');",
                                                                            __NAMESPACE__ ."\\q"),
                        CMS_DB_LOG_ACCESS_INFO     => CMS_L_IFC_LOG_064,
                        CMS_DB_LOG_ACCESS_REFERRER => new log_report_option(CMS_L_IFC_LOG_069,
                                                                            CMS_LOG_REPORT_OPTION_TYPE_TEXT,
                                                                            "javascript:l('%s');",
                                                                            __NAMESPACE__ ."\\q"),
                        CMS_DB_LOG_ACCESS_BROWSER  => new log_report_option(CMS_L_IFC_LOG_037,
                                                                            CMS_LOG_REPORT_OPTION_TYPE_TEXT,
                                                                            "javascript:f(9,2,'%s');",
                                                                            __NAMESPACE__ ."\\q"),
                        CMS_DB_LOG_ACCESS_AGENT    => new log_report_option(CMS_L_IFC_LOG_038,
                                                                            CMS_LOG_REPORT_OPTION_TYPE_TEXT,
                                                                            "javascript:f(8,2,'%s');",
                                                                            __NAMESPACE__ ."\\q"),
                        CMS_DB_LOG_ACCESS_MOBILE   => new log_report_option(CMS_L_IFC_LOG_071,
                                                                            CMS_LOG_REPORT_OPTION_TYPE_TEXT,
                                                                            "javascript:f(10,2,'%s');",
                                                                            __NAMESPACE__ ."\\q",
                                                                            NULL,
                                                                            __NAMESPACE__ ."\\yesno"),
                        CMS_DB_LOG_ACCESS_LANGUAGE => new log_report_option(CMS_L_IFC_LOG_039,
                                                                            CMS_LOG_REPORT_OPTION_TYPE_TEXT,
                                                                            "javascript:f(11,2,'%s');",
                                                                            __NAMESPACE__ ."\\q"),
                        CMS_DB_LOG_ACCESS_REGION   => new log_report_option(CMS_L_IFC_LOG_040,
                                                                            CMS_LOG_REPORT_OPTION_TYPE_TEXT,
                                                                            "javascript:f(12,2,'%s');",
                                                                            __NAMESPACE__ ."\\q")]);

    //more button
    if ($rows === 100)
        echo("<div style=\"TEXT-ALIGN:center\">" .
             "<button class=\"fullwidth\" " .
                     "onclick=\"log_load_raw(" . ($offset + 100) . ");" .
                               "this.parentElement.remove();\">" .
             CMS_L_IFC_LOG_017 .
             "</button>" .
             "</div>");

    exit();

case "load_origin":
//..................................................................................................

    //referrer domain
    $query = "SELECT " . CMS_DB_LOG_ACCESS_DOMAIN . " AS `#value`, " .
                         "COUNT(*) AS `#count` " .
             "FROM " .   CMS_DB_LOG_ACCESS . " " .
             $filter .
             "GROUP BY `#value` " .
             "ORDER BY NULL";

    $query = "SELECT `#value`, " .
                    "`#count` " .
             "FROM ($query) AS `#t1` " .
             "ORDER BY `#count` DESC " .
             "LIMIT 100";

    log_report($query,
               ["#value" => new log_report_option(CMS_L_IFC_LOG_055,
                                                  CMS_LOG_REPORT_OPTION_TYPE_TEXT,
                                                  "javascript:f(7,2,'%s');",
                                                  __NAMESPACE__ ."\\q"),
                "#count" => new log_report_option(CMS_L_IFC_LOG_070,
                                                  CMS_LOG_REPORT_OPTION_TYPE_BEAM)]);

    exit();

case "load_content":
//..................................................................................................

    //access path
    $query = "SELECT " . CMS_DB_LOG_ACCESS_PATH . " AS `#value`, " .
                         "COUNT(*) AS `#count` " .
             "FROM " .   CMS_DB_LOG_ACCESS . " " .
             $filter .
             "GROUP BY `#value` " .
             "ORDER BY NULL";

    $query = "SELECT `#value`, " .
                    "`#count` " .
             "FROM ($query) AS `#t1` " .
             "ORDER BY `#count` DESC " .
             "LIMIT 100";

    log_report($query,
               ["#value" => new log_report_option(CMS_L_IFC_LOG_056,
                                                  CMS_LOG_REPORT_OPTION_TYPE_TEXT,
                                                  "javascript:f(5,2,'%s');",
                                                  __NAMESPACE__ ."\\q"),
                "#count" => new log_report_option(CMS_L_IFC_LOG_070,
                                                  CMS_LOG_REPORT_OPTION_TYPE_BEAM)]);

    exit();

case "load_activity":
//..................................................................................................

    //action
    $query = "SELECT " . CMS_DB_LOG_ACCESS_ACTION . " AS `#value`, " .
                         "COUNT(*) AS `#count` " .
             "FROM " .   CMS_DB_LOG_ACCESS . " " .
             $filter .
             "GROUP BY `#value` " .
             "ORDER BY NULL";

    $query = "SELECT `#value`, " .
                    "`#count` " .
             "FROM ($query) AS `#t1` " .
             "ORDER BY `#count` DESC " .
             "LIMIT 100";

    log_report($query,
               ["#value" => new log_report_option(CMS_L_IFC_LOG_057,
                                                  CMS_LOG_REPORT_OPTION_TYPE_TEXT,
                                                  "javascript:f(3,2,'%s');",
                                                  __NAMESPACE__ ."\\q"),
                "#count" => new log_report_option(CMS_L_IFC_LOG_070,
                                                  CMS_LOG_REPORT_OPTION_TYPE_BEAM)]);

    exit();

case "load_region":
//..................................................................................................

    //region
    $query = "SELECT " . CMS_DB_LOG_ACCESS_REGION . " AS `#value`, " .
                         "COUNT(*) AS `#count` " .
             "FROM " .   CMS_DB_LOG_ACCESS . " " .
             $filter .
             "GROUP BY `#value` " .
             "ORDER BY NULL";

    $query = "SELECT `#value`, " .
                    "`#count` " .
             "FROM ($query) AS `#t1` " .
             "ORDER BY `#count` DESC " .
             "LIMIT 100";

    log_report($query,
               ["#value" => new log_report_option(CMS_L_IFC_LOG_060,
                                                  CMS_LOG_REPORT_OPTION_TYPE_TEXT,
                                                  "javascript:f(12,2,'%s');",
                                                  __NAMESPACE__ ."\\q"),
                "#count" => new log_report_option(CMS_L_IFC_LOG_070,
                                                  CMS_LOG_REPORT_OPTION_TYPE_BEAM)]);

    exit();

case "load_language":
//..................................................................................................

    //language
    $query = "SELECT " . CMS_DB_LOG_ACCESS_LANGUAGE . " AS `#value`, " .
                         "COUNT(*) AS `#count` " .
             "FROM " .   CMS_DB_LOG_ACCESS . " " .
             $filter .
             "GROUP BY `#value` " .
             "ORDER BY NULL";

    $query = "SELECT `#value`, " .
                    "`#count` " .
             "FROM ($query) AS `#t1` " .
             "ORDER BY `#count` DESC " .
             "LIMIT 100";

    log_report($query,
               ["#value" => new log_report_option(CMS_L_IFC_LOG_059,
                                                  CMS_LOG_REPORT_OPTION_TYPE_TEXT,
                                                  "javascript:f(11,2,'%s');",
                                                  __NAMESPACE__ ."\\q"),
                "#count" => new log_report_option(CMS_L_IFC_LOG_070,
                                                  CMS_LOG_REPORT_OPTION_TYPE_BEAM)]);

    exit();

case "load_technology":
//..................................................................................................

    //browser
    $query = "SELECT " . CMS_DB_LOG_ACCESS_BROWSER . " AS `#value`, " .
                         "COUNT(*) AS `#count` " .
             "FROM " .   CMS_DB_LOG_ACCESS . " " .
             $filter .
             "GROUP BY `#value` " .
             "ORDER BY NULL";

    $query = "SELECT `#value`, " .
                    "`#count` " .
             "FROM ($query) AS `#t1` " .
             "ORDER BY `#count` DESC " .
             "LIMIT 100";

    log_report($query,
               ["#value" => new log_report_option(CMS_L_IFC_LOG_058,
                                                  CMS_LOG_REPORT_OPTION_TYPE_TEXT,
                                                  "javascript:f(9,2,'%s');",
                                                  __NAMESPACE__ ."\\q"),
                "#count" => new log_report_option(CMS_L_IFC_LOG_070,
                                                  CMS_LOG_REPORT_OPTION_TYPE_BEAM)]);

    exit();

case "load_identity":
//..................................................................................................

    //top users
    $query = "SELECT " . CMS_DB_LOG_ACCESS_USERID . " AS `#id`, " .
                         "COUNT(*) AS `#count` " .
             "FROM " .   CMS_DB_LOG_ACCESS . " " .
             $filter .
             "GROUP BY `#id` " .
             "ORDER BY NULL";

    $query = "SELECT `#id`, " .
                    "`#count` " .
             "FROM ($query) AS `#t1` " .
             "ORDER BY `#count` DESC " .
             "LIMIT 100";

    $query = "SELECT `#t3`." . CMS_DB_LOG_USER_NAME . " AS `#value`, " .
                    "`#t2`.`#count`, " .
                    "`#t2`.`#id` " .
             "FROM ($query) AS `#t2` " .
             "LEFT JOIN " . CMS_DB_LOG_USER . " AS `#t3` " .
             "ON `#t2`.`#id`=`#t3`." . CMS_DB_LOG_USER_USERID;

    log_report($query,
               ["#value" => new log_report_option(CMS_L_IFC_LOG_061,
                                                  CMS_LOG_REPORT_OPTION_TYPE_TEXT,
                                                  "javascript:f(0,2,'%s');",
                                                  __NAMESPACE__ ."\\q",
                                                  "#id",
                                                  __NAMESPACE__ ."\\strtocolor"),
                "#count" => new log_report_option(CMS_L_IFC_LOG_070,
                                                  CMS_LOG_REPORT_OPTION_TYPE_BEAM),
                "#id"    => new log_report_option(NULL,
                                                  CMS_LOG_REPORT_OPTION_TYPE_NONE)]);

    exit();

case "load_time":
//..................................................................................................

    //daily activity
    $func  = function($value) { return x(sprintf("%02d:00 – %02d:00", $value, $value + 1)); };

    $query = "SELECT HOUR(" . CMS_DB_LOG_ACCESS_TIME . ") AS `#value`, " .
                    "COUNT(*) AS `#count` " .
             "FROM " .   CMS_DB_LOG_ACCESS . " " .
             $filter .
             "GROUP BY `#value` " .
             "ORDER BY NULL";

    $query = "SELECT `#value`, " .
                    "`#count` " .
             "FROM ($query) AS `#t1` " .
             "ORDER BY `#value` " .
             "LIMIT 24";

    log_report($query,
               ["#value" => new log_report_option(CMS_L_IFC_LOG_062,
                                                  CMS_LOG_REPORT_OPTION_TYPE_TEXT,
                                                  NULL, NULL, NULL, $func),
                "#count" => new log_report_option(CMS_L_IFC_LOG_070,
                                                  CMS_LOG_REPORT_OPTION_TYPE_BEAM)]);

    exit();
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

//selected user id
$object = (streq($ifc_param3, "0") && streq($ifc_param4, "2") && nstre($ifc_param5)) ? $ifc_param5 : NULL;

$menu = $log->operator ? [CMS_L_IFC_LOG_044 . "|log/command_configuration" => "config"] : NULL;

//display
$ifc = new ifc($ifc_response,
               $ifc_page,
               $menu,
               ["object" => $object]);

//date
$ifc->set(CMS_L_IFC_LOG_045, "date", date("Y-m-d", $ifc_param1));

//interval
$ifc->set(CMS_L_IFC_LOG_046, "label");
$ifc->set(array_flip(array_keys($interval)), "select", $ifc_param2);

//filter
echo("<div class=\"element\">" .
     "<span>" . CMS_L_IFC_LOG_047 . "</span><br>");
$ifc->set(array_flip(array_keys($filter_field)), "select", $ifc_param3);
$ifc->set(array_flip(array_keys($filter_operation)), "select", $ifc_param4);
$ifc->set(NULL, "text 20", $ifc_param5);
echo("</div>");

//bots
$ifc->set(CMS_L_IFC_LOG_048, "label");
$ifc->set([CMS_L_IFC_LOG_049 => 0,  //off
           CMS_L_IFC_LOG_050 => 1,  //only
           CMS_L_IFC_LOG_051 => 2], //all
          "select", $ifc_param6);

//apply button
$ifc->set(CMS_L_COMMAND_CONFIRM, "button");

//reset button
$ifc->set(CMS_L_IFC_LOG_052, "button b", "javascript:ifc_reset(1);ifc_submit();");

//retrieve data
$query = "SELECT " . $resolution[$_resolution] . " AS `#index`, " .
                     CMS_DB_LOG_ACCESS_USERID . " AS `#userid`, " .
                    "COUNT(*) AS `#count`, " .
                    "SUM(" . CMS_DB_LOG_ACCESS_MOBILE . ") AS `#mobile` " .
         "FROM " .   CMS_DB_LOG_ACCESS . " " .
         $filter .
         "GROUP BY `#index`, `#userid` " .
         "ORDER BY NULL";

$query = "SELECT `#index`, " .
                "`#userid`, " .
                "`#count`, " .
                "`#mobile` " .
         "FROM (" . $query . ") AS `#t1` " .
         "ORDER BY `#index`, `#userid` ";

$result = mysql_query($query);
$array  = [];
$index  = -1;

while ($resultrow = mysql_fetch_row($result))
{
    $_index = (int)$resultrow[0];

    if ($_index !== $index)
    {
        $array[$_index] = [(int)$resultrow[2], 1, (int)$resultrow[3]];
        $index          = $_index;
        continue;
    };

    $array[$_index][0] += (int)$resultrow[2];
    $array[$_index][1]++;
    $array[$_index][2] += (int)$resultrow[3];
};

$access_sum = 0;
$access_max = 0;
$unique_sum = 0;
$mobile_sum = 0;
$flag       = FALSE;

//export data to javascript
echo("<script>" .
     "const log = {");

foreach ($array AS $key => $value)
{
    list($access, $unique, $mobile) = $value;
    $access_sum += $access;
    if ($access > $access_max) $access_max = $access;
    $unique_sum += $unique;
    $mobile_sum += $mobile;

    if ($flag) echo(", ");
    else       $flag = TRUE;
    echo("$key: [$access, $unique, $mobile]");
};

$width    = 500; //view box width
$height   = 100; //view box height
$_width   = $_ts_end - $ts_start;
$factor_x = $width / $_width;
$factor_y = ($access_max > 0) ? $height / $access_max : 0;
$border   = 7.5;
$_border  = $border * 2;
$__width  = $width + $border;

echo("};" .
     "const width    = $width;" .
     "const height   = $height;" .
     "const factor_x = $factor_x;" .
     "const factor_y = $factor_y;" .
     "const border   = $border;" .
     "</script>");

//graph
ifc_tab_open(CMS_L_IFC_LOG_054);

$date_start = date(CMS_L_DATE_FORMAT, $ts_start);
$date_end   = date(CMS_L_DATE_FORMAT, $ts_end);

ifc_table_open();
echo("<colgroup>");
for ($i = 0; $i < 4; $i++)
    echo("<col style=\"WIDTH:0\">" .
         "<col style=\"WIDTH:25%\">");
echo("</colgroup>" .

//interval
     "<tr>" .
     "<th colspan=\"8\">" .
     x(streq($date_start, $date_end) ? $date_start : "$date_start – $date_end") .
     "</th>" .
     "</tr>" .

//accesses
     "<tr>" .
     "<td class=\"varied\">" .
     "<span style=\"COLOR:var(--ifc-color-highlight2)\">&#x2B24;</span> " .
     CMS_L_IFC_LOG_053 . ":" .
     "</td>" .
     "<td>" .
     number_format($access_sum, 0,
                   CMS_L_DECIMAL_SEPARATOR,
                   CMS_L_THOUSAND_SEPARATOR) .
     " (&#8709; " . number_format(($count > 0) ? $access_sum / $count : 0, 1,
                                  CMS_L_DECIMAL_SEPARATOR,
                                  CMS_L_THOUSAND_SEPARATOR) . ")" .
     "</td>" .

//unique users
     "<td class=\"varied\">" .
     "<span style=\"COLOR:var(--ifc-color-error)\">&#x2B24;</span> " .
     CMS_L_IFC_LOG_030 . ":" .
     "</td>" .
     "<td>" .
     number_format($unique_sum, 0,
                   CMS_L_DECIMAL_SEPARATOR,
                   CMS_L_THOUSAND_SEPARATOR) .
     " (&#8709; " . number_format(($count > 0) ? $unique_sum / $count : 0, 1,
                                  CMS_L_DECIMAL_SEPARATOR,
                                  CMS_L_THOUSAND_SEPARATOR) . ")" .
     "</td>" .

//accesses per user
     "<td class=\"varied\">" . CMS_L_IFC_LOG_053 . " / " . CMS_L_IFC_LOG_030 . ":</td>" .
     "<td>" .
     number_format(($unique_sum > 0) ? $access_sum / $unique_sum : 0, 1,
                   CMS_L_DECIMAL_SEPARATOR,
                   CMS_L_THOUSAND_SEPARATOR) .
     "</td>" .

//mobile
     "<td class=\"varied\">" .
     "<span style=\"COLOR:var(--ifc-color-success)\">&#x2B24;</span> " .
     CMS_L_IFC_LOG_071 . ":" .
     "</td>" .
     "<td>" .
     number_format(($access_sum > 0) ? 100 / $access_sum * $mobile_sum : 0, 1,
                   CMS_L_DECIMAL_SEPARATOR,
                   CMS_L_THOUSAND_SEPARATOR) . "%" .
     "</td>" .
     "</tr>");
ifc_table_close();

//svg
echo("<svg xmlns=\"http://www.w3.org/2000/svg\" " .
          "id=\"log-graph\" " .
          "viewBox=\"-$border -" . ($border + 22.5) . " " . ($width + $_border) . " " . ($height + $border * 3 + 22.5) . "\" " .
          "style=\"BACKGROUND:var(--cms-color-bg-input);MARGIN:5px;USER-SELECT:none\">" .

     "<style type=\"text/css\">" .
     "#log-hline line { STROKE: color-mix(in srgb, var(--ifc-color), transparent 75%); STROKE-WIDTH: 0.5; STROKE-DASHARRAY: 1.5,1; }" .
     "#log-hline text { FILL: var(--ifc-color); FONT-SIZE: 5px; }" .
     "#log-vline line { STROKE: color-mix(in srgb, var(--ifc-color), transparent 75%); STROKE-WIDTH: 0.5; STROKE-DASHARRAY: 1.5,1; }" .
     "#log-access-path { FILL: color-mix(in srgb, var(--ifc-color-highlight2), transparent 75%); STROKE: var(--ifc-color-highlight2); STROKE-WIDTH: 0.5; }" .
     "#log-mobile-path { FILL: color-mix(in srgb, var(--ifc-color-success), transparent 75%); STROKE: var(--ifc-color-success); STROKE-WIDTH: 0.5; }" .
     "#log-unique-path { FILL: color-mix(in srgb, var(--ifc-color-error), transparent 75%); STROKE: var(--ifc-color-error); STROKE-WIDTH: 0.5; }" .
     ".log-button path { FILL: none; STROKE: var(--ifc-color); STROKE-WIDTH: 1.65; }" .
     ".log-button line { STROKE: var(--ifc-color); STROKE-WIDTH: 1.65; }" .
     ".log-button rect { CURSOR: pointer; FILL: transparent; STROKE: var(--ifc-color); STROKE-WIDTH: 0.35; }" .
     "#log-label text { FILL: var(--ifc-color); FONT-SIZE: 5px; }" .
     "#log-mobile circle { FILL: var(--ifc-color-success); }" .
     "#log-unique circle { FILL: var(--ifc-color-error); }" .
     "#log-access circle { CURSOR: pointer; FILL: var(--ifc-color-highlight2); STROKE: var(--ifc-color); STROKE-WIDTH: 0.5; }" .
     "</style>" .
     "<defs>" .

     "<clipPath id=\"clip\">" .
     "<rect x=\"0\" y=\"-0.25\" width=\"$width\" height=\"" . ($height + 0.5) . "\"/>" .
     "</clipPath>" .
     "</defs>" .

     "<g clip-path=\"url(#clip)\">" .
     "<g id=\"log-hline\">" .
     "<line x1=\"-$border\" y1=\"" . ($temp = round($height * 0.75, 1)) . "\" x2=\"$__width\" y2=\"$temp\"/>" .
     "<text x=\"0\" y=\"$temp\" dy=\"1.35em\">" . round($access_max * 0.25, 1) . "</text>" .
     "<line x1=\"-$border\" y1=\"" . ($temp = round($height * 0.50, 2)) . "\" x2=\"$__width\" y2=\"$temp\"/>" .
     "<text x=\"0\" y=\"$temp\" dy=\"1.35em\">" . round($access_max * 0.5, 1) . "</text>" .
     "<line x1=\"-$border\" y1=\"" . ($temp = round($height * 0.25, 2)) . "\" x2=\"$__width\" y2=\"$temp\"/>" .
     "<text x=\"0\" y=\"$temp\" dy=\"1.35em\">" . round($access_max * 0.75, 1) . "</text>" .
     "<line x1=\"-$border\" y1=\"0\" x2=\"$__width\" y2=\"0\"/>" .
     "<text x=\"0\" y=\"0\" dy=\"1.35em\">" .
     "<tspan class=\"access\">$access_max</tspan>" .
     "</text>" .
     "</g>" .

     "<g id=\"log-vline\"/>" .

     "<path id=\"log-access-path\"/>" .
     "<path id=\"log-mobile-path\"/>" .
     "<path id=\"log-unique-path\"/>" .
     "</g>" .

     "<g id=\"log-prev\" class=\"log-button\">" .
     "<path d=\"M9.5,-19 L5.5,-15 L9.5,-11\"/>" .
     "<rect x=\"0\" y=\"-22.5\" width=\"15\" height=\"15\"/>" .
     "</g>" .

     "<g id=\"log-zoom-out\" class=\"log-button\">" .
     "<line x1=\"" . ($width / 2 - 3.25) . "\" y1=\"-15\" x2=\"" . ($width / 2 + 3.25) . "\" y2=\"-15\"/>" .
     "<rect x=\"" . ($width / 2 - 7.5) . "\" y=\"-22.5\" width=\"15\" height=\"15\"/>" .
     "</g>" .

     "<g id=\"log-next\" class=\"log-button\">" .
     "<path d=\"M" . ($width - 9.5) . ",-19 L" . ($width - 5.5) . ",-15 L" . ($width - 9.5) . ",-11\"/>" .
     "<rect x=\"" . ($width - 15) . "\" y=\"-22.5\" width=\"15\" height=\"15\"/>" .
     "</g>" .

     "<g id=\"log-label\"/>" .
     "<g id=\"log-mobile\"/>" .
     "<g id=\"log-unique\"/>" .
     "<g id=\"log-access\"/>" .

     "</svg><br>");

?><script>

Date.prototype.getISOYear = function()
{
    let date = new Date(this);
    date.setUTCDate(date.getUTCDate() + 3 - (date.getUTCDay() + 6) % 7);
    return date.getUTCFullYear();
};

Date.prototype.getISOWeek = function()
{
    let date = new Date(this);
    date.setUTCHours(0, 0, 0, 0);
    date.setUTCDate(date.getUTCDate() + 3 - (date.getUTCDay() + 6) % 7);
    let jan4 = new Date(Date.UTC(date.getUTCFullYear(), 0, 4));
    let diff = (date - jan4) / 86400000;
    return 1 + Math.round((diff - 3 + (jan4.getUTCDay() + 6) % 7) / 7);
};

Date.prototype.getTimeFromISOWeek = function(week)
{
    let year   = this.getUTCFullYear();
    let jan4   = new Date(Date.UTC(year, 0, 4));
    let day    = jan4.getUTCDay() || 7;
    let diff   = 4 - day + 1;
    let result = new Date(Date.UTC(year, 0, diff + (week - 1) * 7));
    result.setUTCHours(0, 0, 0, 0);
    return result.getTime();
};

Date.prototype.getWeekdayAbbr = function()
{
    let list = [
        "<?php echo(CMS_L_SUNDAY_ABBR);?>",
        "<?php echo(CMS_L_MONDAY_ABBR);?>",
        "<?php echo(CMS_L_TUESDAY_ABBR);?>",
        "<?php echo(CMS_L_WEDNESDAY_ABBR);?>",
        "<?php echo(CMS_L_THURSDAY_ABBR);?>",
        "<?php echo(CMS_L_FRIDAY_ABBR);?>",
        "<?php echo(CMS_L_SATURDAY_ABBR);?>"];
    return list[this.getUTCDay()];
};

</script><?php

$array = [
    "h" => 10000,
    "d" => 1000,
    "w" => 100,
    "m" => 100,
    "q" => 10,
    "y" => 1];

?><script>

const factor = <?php echo($array[$_resolution]);?>;

</script><?php

$array = [
    "h" => "date.getUTCHours() === 0",
    "d" => "date.getUTCDay() === 0",
    "w" => "date.getUTCDate() <= 7",
    "m" => "date.getUTCMonth() === 0",
    "q" => "date.getUTCMonth() === 0",
    "y" => "false"];

?><script>

function log_vline(date)
{
    return <?php echo($array[$_resolution]);?>;
};

</script><?php

$array = [
    "h" => "(value % 24).toString().padStart(2, \"0\") + \":00\"",
    "d" => "new Date(Date.UTC(year, 0, value)).getWeekdayAbbr() + \" \" + " .
           "new Date(Date.UTC(year, 0, value)).toISOString().slice(0, 10)",
    "w" => "year.toString().padStart(4, \"0\") + \"-W\" + value.toString().padStart(2, \"0\")",
    "m" => "year.toString().padStart(4, \"0\") + \"-\" + value.toString().padStart(2, \"0\")",
    "q" => "year.toString().padStart(4, \"0\") + \"-Q\" + value",
    "y" => "year.toString()"];

?><script>

function log_label(value)
{
    let year  = Math.floor(value / factor);
    value    %= factor;
    return <?php echo($array[$_resolution]);?>;
};

</script><?php

$array = [
    "h" => "year * 10000 + Math.floor((date - Date.UTC(year, 0, 1)) / 3600000)",
    "d" => "year * 1000 + Math.floor((date - new Date(Date.UTC(year, 0, 0))) / 86400000)",
    "w" => "date.getISOYear() * 100 + date.getISOWeek()",
    "m" => "year * 100 + date.getUTCMonth() + 1",
    "q" => "year * 10 + Math.floor(date.getUTCMonth() / 3) + 1",
    "y" => "year"];

?><script>

function log_index(date)
{
    let year = date.getUTCFullYear();
    return <?php echo($array[$_resolution]);?>;
};

</script><?php

$array = [
    "h" => "Date.UTC(year, 0, 1) + (value + 1) * 3600000",
    "d" => "Date.UTC(year, 0, value + 1)",
    "w" => "new Date(Date.UTC(year, 0, 4)).getTimeFromISOWeek(value + 1)",
    "m" => "Date.UTC(year, value, 1)",
    "q" => "Date.UTC(year, value * 3, 1)",
    "y" => "Date.UTC(year + 1, 0, 1)"];

?><script>

function log_timestamp(value)
{
    let year  = Math.floor(value / factor);
    value    %= factor;
    return <?php echo($array[$_resolution]);?> - 1;
};

</script><?php

$array = [
    "h" => "date.setUTCHours(date.getUTCHours() %s 1)",
    "d" => "date.setUTCDate(date.getUTCDate() %s 1)",
    "w" => "date.setUTCDate(date.getUTCDate() %s 7)",
    "m" => "date.setUTCMonth(date.getUTCMonth() %s 1)",
    "q" => "date.setUTCMonth(date.getUTCMonth() %s 3)",
    "y" => "date.setUTCFullYear(date.getUTCFullYear() %s 1)"];

$_array = [
    "h" => 0,
    "d" => 0,
    "w" => 1,
    "m" => 2,
    "q" => 3,
    "y" => 5];

?><script>

const vline_container  = document.getElementById("log-vline");
const label_container  = document.getElementById("log-label");
const access_container = document.getElementById("log-access");
const unique_container = document.getElementById("log-unique");
const mobile_container = document.getElementById("log-mobile");
const ts_start         = <?php echo($ts_start * 1000);?>;
const ts_end           = <?php echo($_ts_end * 1000);?>;
const date             = new Date(ts_start);
const date_end         = new Date(ts_end);
let access_path        = "M-" + border + "," + (height + border);
let unique_path        = access_path;
let mobile_path        = access_path;
let text_width         = 0;

while (date <= date_end)
{
    let index  = log_index(date);
    let access = 0;
    let unique = 0;
    let mobile = 0;

    if (log[index])
    {
        access = log[index][0];
        unique = log[index][1];
        mobile = log[index][2];
    };

    let x  = ((date.getTime() - ts_start) / 1000 * factor_x).toFixed(2);
    let y1 = height - (access * factor_y).toFixed(2);
    let y2 = height - (unique * factor_y).toFixed(2);
    let y3 = height - (mobile * factor_y).toFixed(2);

    //vertical line
    if (log_vline(date))
    {
        let line = document.createElementNS("http://www.w3.org/2000/svg", "line");
        let _x   = x;

<?php if ($_resolution === "w") { ?>
        _x = new Date(date.getUTCFullYear(), date.getUTCMonth(), 1).getTime();
        _x = ((_x - ts_start) / 1000 * factor_x).toFixed(2);
<?php }; ?>

        line.setAttribute("x1", _x);
        line.setAttribute("y1", 0);
        line.setAttribute("x2", _x);
        line.setAttribute("y2", 100);
        vline_container.appendChild(line);
    };

    //label
    let text = document.createElementNS("http://www.w3.org/2000/svg", "text");
    text.setAttribute("x", x);
    text.setAttribute("y", height);
    text.setAttribute("dy", "0.35em");
    text.setAttribute("transform", "rotate(-90," + x + "," + height + ")");
    text.textContent = log_label(index);
    label_container.appendChild(text);
    let _text_width = text.getBBox().width;
    text_width = Math.max(text_width, _text_width);
    text.setAttribute("dx", -_text_width - border);

    //access
    let circle = document.createElementNS("http://www.w3.org/2000/svg", "circle");
    circle.setAttribute("cx", x);
    circle.setAttribute("cy", y1);
    circle.setAttribute("r", 2.35);
    circle.setAttribute("data-index", index);
    access_container.appendChild(circle);

    let title = document.createElementNS("http://www.w3.org/2000/svg", "title");
    title.textContent = "<?php echo(q(CMS_L_IFC_LOG_053));?>: " + access;
    circle.appendChild(title);

    //click event
    fx_event_listen(circle, "click", (event) =>
    {
        let index = event.target.getAttribute("data-index");
        let time  = log_timestamp(index);
        let date  = new Date(time);

        ifc_set("ifc_param1", date.toISOString().slice(0, 10));
        ifc_set("ifc_param2", <?php echo($_array[$_resolution]);?>);
        ifc_submit();
    });

    //unique
    circle = document.createElementNS("http://www.w3.org/2000/svg", "circle");
    circle.setAttribute("cx", x);
    circle.setAttribute("cy", y2);
    circle.setAttribute("r", 2);
    unique_container.appendChild(circle);

    title = document.createElementNS("http://www.w3.org/2000/svg", "title");
    title.textContent = "<?php echo(q(CMS_L_IFC_LOG_030));?>: " + unique;
    circle.appendChild(title);

    //mobile
    circle = document.createElementNS("http://www.w3.org/2000/svg", "circle");
    circle.setAttribute("cx", x);
    circle.setAttribute("cy", y3);
    circle.setAttribute("r", 1.65);
    mobile_container.appendChild(circle);

    title = document.createElementNS("http://www.w3.org/2000/svg", "title");
    title.textContent = "<?php echo(q(CMS_L_IFC_LOG_071));?>: " + mobile;
    circle.appendChild(title);

    //draw paths
    access_path += " L" + x + "," + y1;
    unique_path += " L" + x + "," + y2;
    mobile_path += " L" + x + "," + y3;

    <?php echo(sprintf($array[$_resolution], "+"));?>;
};

//re-set view box
const graph  = document.getElementById("log-graph");
let value    = graph.getAttribute("viewBox").split(" ").map(Number);
value[3]    += Math.round(text_width);
graph.setAttribute("viewBox", value.join(" "));

//add paths
value = " L" + (width + border) + "," + (height + border);
document.getElementById("log-access-path").setAttribute("d", access_path + value);
document.getElementById("log-unique-path").setAttribute("d", unique_path + value);
document.getElementById("log-mobile-path").setAttribute("d", mobile_path + value);

//previous click event
fx_event_listen(document.getElementById("log-prev"), "click", (event) =>
{
    let value = ifc_get("ifc_param1");
    let date  = new Date(value);
    <?php echo(sprintf($array[$_resolution], "-"));?>;
    ifc_set("ifc_param1", date.toISOString().slice(0, 10));
    ifc_submit();
});

//zoom out click event
fx_event_listen(document.getElementById("log-zoom-out"), "click", (event) =>
{
    let value = parseInt(ifc_get("ifc_param2"));
    if (value === 10) return;
    ifc_set("ifc_param2", value + 1);
    ifc_submit();
});

//next click event
fx_event_listen(document.getElementById("log-next"), "click", (event) =>
{
    let value = ifc_get("ifc_param1");
    let date  = new Date(value);
    <?php echo(streq($_resolution, "h") ?
               sprintf($array["d"], "+") :
               sprintf($array[$_resolution], "+"));?>;
    ifc_set("ifc_param1", date.toISOString().slice(0, 10));
    ifc_submit();
});

function d(value)
{
    ifc_set("ifc_param1", value);
    ifc_submit();
};

function f(field,
           operator,
           value)
{
    ifc_set("ifc_param3", field);
    ifc_set("ifc_param4", operator);
    ifc_set("ifc_param5", value);
    ifc_submit();
};

function l(value)
{
    try
    {
        let url   = new URL(value);
        let token = (url.host === location.host) ?
                    "?<?php echo(qr(CMS_TOKEN));?>" : "";
        load_page(url.origin + url.pathname + token);
    } catch (e) {};
};

function b(value)
{
    ifc_set("ifc_param6", Number(value > 0));
    ifc_submit();
};

</script><?php

//raw data
ifc_tab_next(CMS_L_IFC_LOG_066);

echo("<div id=\"log-raw\">" .
     "<span class=\"element\">" .
     CMS_L_IFC_LOG_095 .
     "</span>" .
     "</div>");

?><script>

function log_load_raw(offset = 0,
                      append = true)
{
    let url = "<?php echo(q(cms_url(["ifc_page"          => CMS_IFC_PAGE,
                                     "ifc_message"       => "load_raw",
                                     "ts_start"          => $ts_start,
                                     "ts_end"            => $ts_end,
                                     "_filter_field"     => $ifc_param3,
                                     "_filter_operation" => $ifc_param4,
                                     "filter_value"      => $ifc_param5,
                                     "bot"               => $ifc_param6,
                                     "offset"            => "\x1B%offset%"])));?>";
    asr_send(url.replace("%offset%", offset), (value) =>
    {
        if (append) document.getElementById("log-raw").innerHTML += value;
        else        document.getElementById("log-raw").innerHTML  = value;
    });
};

log_load_raw(0, false);

</script><?php

//source
ifc_tab_next(CMS_L_IFC_LOG_094);

echo("<div id=\"log-origin\">" .
     "<span class=\"element\">" .
     CMS_L_IFC_LOG_095 .
     "</span>" .
     "</div>");

?><script>

function log_load_stats(object,
                        target,
                        message)
{
    const url = "<?php echo(q(cms_url(["ifc_page"          => CMS_IFC_PAGE,
                                       "ifc_message"       => "\x1B%message%",
                                       "ts_start"          => $ts_start,
                                       "ts_end"            => $ts_end,
                                       "_filter_field"     => $ifc_param3,
                                       "_filter_operation" => $ifc_param4,
                                       "filter_value"      => $ifc_param5,
                                       "bot"               => $ifc_param6])));?>"
        .replace("%message%", message);

    const func = () =>
    {
        asr_send(url, (value) => { target.innerHTML = value; });
        fx_event_remove(object, "change", func);
    };

    if (object.checked) func();
    else                fx_event_listen(object, "change", func);
};

log_load_stats(document.querySelector("input[name=\"ifc_tab_1\"][value=\"3\"]"),
               document.getElementById("log-origin"),
               "load_origin");

</script><?php

//content
ifc_tab_next(CMS_L_IFC_LOG_100);

echo("<div id=\"log-content\">" .
     "<span class=\"element\">" .
     CMS_L_IFC_LOG_095 .
     "</span>" .
     "</div>");

?><script>

log_load_stats(document.querySelector("input[name=\"ifc_tab_1\"][value=\"4\"]"),
               document.getElementById("log-content"),
               "load_content");

</script><?php

//activity
ifc_tab_next(CMS_L_IFC_LOG_096);

echo("<div id=\"log-activity\">" .
     "<span class=\"element\">" .
     CMS_L_IFC_LOG_095 .
     "</span>" .
     "</div>");

?><script>

log_load_stats(document.querySelector("input[name=\"ifc_tab_1\"][value=\"5\"]"),
               document.getElementById("log-activity"),
               "load_activity");

</script><?php

//region
ifc_tab_next(CMS_L_IFC_LOG_097);

echo("<div id=\"log-region\">" .
     "<span class=\"element\">" .
     CMS_L_IFC_LOG_095 .
     "</span>" .
     "</div>");

?><script>

log_load_stats(document.querySelector("input[name=\"ifc_tab_1\"][value=\"6\"]"),
               document.getElementById("log-region"),
               "load_region");

</script><?php

//language
ifc_tab_next(CMS_L_IFC_LOG_102);

echo("<div id=\"log-language\">" .
     "<span class=\"element\">" .
     CMS_L_IFC_LOG_095 .
     "</span>" .
     "</div>");

?><script>

log_load_stats(document.querySelector("input[name=\"ifc_tab_1\"][value=\"7\"]"),
               document.getElementById("log-language"),
               "load_language");

</script><?php

//technology
ifc_tab_next(CMS_L_IFC_LOG_098);

echo("<div id=\"log-technology\">" .
     "<span class=\"element\">" .
     CMS_L_IFC_LOG_095 .
     "</span>" .
     "</div>");

?><script>

log_load_stats(document.querySelector("input[name=\"ifc_tab_1\"][value=\"8\"]"),
               document.getElementById("log-technology"),
               "load_technology");

</script><?php

//identity
ifc_tab_next(CMS_L_IFC_LOG_099);

echo("<div id=\"log-identity\">" .
     "<span class=\"element\">" .
     CMS_L_IFC_LOG_095 .
     "</span>" .
     "</div>");

?><script>

log_load_stats(document.querySelector("input[name=\"ifc_tab_1\"][value=\"9\"]"),
               document.getElementById("log-identity"),
               "load_identity");

</script><?php

//time
ifc_tab_next(CMS_L_IFC_LOG_101);

echo("<div id=\"log-time\">" .
     "<span class=\"element\">" .
     CMS_L_IFC_LOG_095 .
     "</span>" .
     "</div>");

?><script>

log_load_stats(document.querySelector("input[name=\"ifc_tab_1\"][value=\"10\"]"),
               document.getElementById("log-time"),
               "load_time");

</script><?php

//user data
if ($object)
{
    ifc_tab_next(CMS_L_IFC_LOG_030);

    $query  = "SELECT * " .
              "FROM " .  CMS_DB_LOG_USER . " " .
              "WHERE " . CMS_DB_LOG_USER_USERID .
              "='" .     sqlesc($object) . "' " .
              "LIMIT 1";

    $result = mysql_query($query);
    if ($result && ($resultrow = mysql_fetch_assoc($result)))
    {
        $ifc->set(CMS_L_IFC_LOG_063,  "text 30 255",         $resultrow[CMS_DB_LOG_USER_NAME]);
        $ifc->set(CMS_L_COMMAND_SAVE, "button b",            "save");
        $ifc->set(CMS_L_EMAIL,        "text 40 255 b",       $resultrow[CMS_DB_LOG_USER_EMAIL]);
        $ifc->set(CMS_L_IFC_LOG_064,  "textarea 40x4 255 b", $resultrow[CMS_DB_LOG_USER_INFO]);
        $ifc->set(CMS_L_IFC_LOG_072,  "label");
        $ifc->set([CMS_L_IFC_LOG_073 => CMS_LOG_STATUS_USER_FIXED,
                   CMS_L_IFC_LOG_074 => CMS_LOG_STATUS_USER_PROVISIONAL,
                   CMS_L_IFC_LOG_075 => CMS_LOG_STATUS_BOT_PROVISIONAL,
                   CMS_L_IFC_LOG_076 => CMS_LOG_STATUS_BOT_LIMIT_EXCEEDED,
                   CMS_L_IFC_LOG_077 => CMS_LOG_STATUS_BOT_FIXED,
                   CMS_L_IFC_LOG_078 => CMS_LOG_STATUS_BAD_BOT],
                  "select",
                  $resultrow[CMS_DB_LOG_USER_BOT]);
    };
};

ifc_tab_close();

$ifc->close();