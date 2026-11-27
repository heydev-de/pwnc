<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   AGENT EVAL CLASS
//==================================================================================================

//database
define("CMS_DB_AGENT_EVAL",              CMS_DB_PREFIX . "agent_eval");
define("CMS_DB_AGENT_EVAL_AGENT",        "agent");
define("CMS_DB_AGENT_EVAL_CONTEXT",      "context");
define("CMS_DB_AGENT_EVAL_INDEX",        "id");
define("CMS_DB_AGENT_EVAL_TIME",         "time");
define("CMS_DB_AGENT_EVAL_LAST",         "last");
define("CMS_DB_AGENT_EVAL_ORIGIN",       "origin");
define("CMS_DB_AGENT_EVAL_STATUS",       "status");
define("CMS_DB_AGENT_EVAL_FAIL",         "fail");
define("CMS_DB_AGENT_EVAL_REQUEST",      "request");
define("CMS_DB_AGENT_EVAL_RETRY",        "retry");
define("CMS_DB_AGENT_EVAL_TOOL",         "tool");
define("CMS_DB_AGENT_EVAL_TOOL_FAIL",    "tool_fail");
define("CMS_DB_AGENT_EVAL_DURATION",     "duration");
define("CMS_DB_AGENT_EVAL_TOKEN_INPUT",  "token_input");
define("CMS_DB_AGENT_EVAL_TOKEN_CACHE",  "token_cache");
define("CMS_DB_AGENT_EVAL_TOKEN_OUTPUT", "token_output");
define("CMS_DB_AGENT_EVAL_SUMMARY",      "summary");
define("CMS_DB_AGENT_EVAL_RATING",       "rating");

class agent_eval
{

//properties
//--------------------------------------------------------------------------------------------------

    public $enabled = FALSE;
    public $limit   = 0; //retention in days

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct()
    {
        $mysql = new mysql();

        if ($mysql->verify_table(CMS_DB_AGENT_EVAL,
                                 [CMS_DB_AGENT_EVAL_AGENT        => "VARCHAR(40) NOT NULL DEFAULT ''",
                                  CMS_DB_AGENT_EVAL_CONTEXT      => "VARCHAR(40) NOT NULL DEFAULT ''",
                                  CMS_DB_AGENT_EVAL_INDEX        => "BIGINT UNSIGNED NOT NULL DEFAULT 0",
                                  CMS_DB_AGENT_EVAL_TIME         => "TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP",
                                  CMS_DB_AGENT_EVAL_LAST         => "BIGINT UNSIGNED NOT NULL DEFAULT 0",
                                  CMS_DB_AGENT_EVAL_ORIGIN       => "ENUM('chat', 'trigger', 'call', 'self') NOT NULL DEFAULT 'chat'",
                                  CMS_DB_AGENT_EVAL_STATUS       => "ENUM('open', 'completed', 'failed', 'interrupted') NOT NULL DEFAULT 'open'",
                                  CMS_DB_AGENT_EVAL_FAIL         => "INT UNSIGNED NOT NULL DEFAULT 0",
                                  CMS_DB_AGENT_EVAL_REQUEST      => "INT UNSIGNED NOT NULL DEFAULT 0",
                                  CMS_DB_AGENT_EVAL_RETRY        => "INT UNSIGNED NOT NULL DEFAULT 0",
                                  CMS_DB_AGENT_EVAL_TOOL         => "INT UNSIGNED NOT NULL DEFAULT 0",
                                  CMS_DB_AGENT_EVAL_TOOL_FAIL    => "INT UNSIGNED NOT NULL DEFAULT 0",
                                  CMS_DB_AGENT_EVAL_DURATION     => "INT UNSIGNED NOT NULL DEFAULT 0",
                                  CMS_DB_AGENT_EVAL_TOKEN_INPUT  => "BIGINT UNSIGNED NOT NULL DEFAULT 0",
                                  CMS_DB_AGENT_EVAL_TOKEN_CACHE  => "BIGINT UNSIGNED NOT NULL DEFAULT 0",
                                  CMS_DB_AGENT_EVAL_TOKEN_OUTPUT => "BIGINT UNSIGNED NOT NULL DEFAULT 0",
                                  CMS_DB_AGENT_EVAL_SUMMARY      => "VARCHAR(100) NOT NULL DEFAULT ''",
                                  CMS_DB_AGENT_EVAL_RATING       => "TINYINT DEFAULT NULL"],
                                 ["PRIMARY KEY (" . CMS_DB_AGENT_EVAL_AGENT . ", " .
                                                    CMS_DB_AGENT_EVAL_CONTEXT . ", " .
                                                    CMS_DB_AGENT_EVAL_INDEX . ")",
                                  "INDEX (" .       CMS_DB_AGENT_EVAL_TIME . ")"]))
        {
            $this->limit   = max(0, (int)(new system())->getval("agent_eval", "limit")) ?: 90;
            $this->enabled = TRUE;
        };
    }

//methods
//--------------------------------------------------------------------------------------------------

    function get_task($agent,
                      $context,
                      $index = PHP_INT_MAX)
    {
        if (! $this->enabled) return FALSE;

        $query =
            "SELECT * " .
            "FROM " .     CMS_DB_AGENT_EVAL . " " .
            "WHERE " .    CMS_DB_AGENT_EVAL_AGENT .
            "='" .        sqlesc($agent) . "' " .
            "AND " .      CMS_DB_AGENT_EVAL_CONTEXT .
            "='" .        sqlesc($context) . "' " .
            "AND " .      CMS_DB_AGENT_EVAL_INDEX .
            "<=" .        (int)$index . " " .
            "ORDER BY " . CMS_DB_AGENT_EVAL_INDEX . " DESC " .
            "LIMIT 1";

        $result = mysql_query($query);
        if (! $result) return FALSE;

        return mysql_fetch_assoc($result) ?: NULL;
    }

//--------------------------------------------------------------------------------------------------

    function set_task($array)
    {
        //truncate summary
        $array["summary"] = utf8_substr($array["summary"], 0, 100);

        //prepare data
        $_array  = sqlesc($array);
        $update  = [CMS_DB_AGENT_EVAL_TIME . "=NOW()"];
        $__array = array_diff_key(
            $_array,
            array_flip(
                [CMS_DB_AGENT_EVAL_AGENT,
                 CMS_DB_AGENT_EVAL_CONTEXT,
                 CMS_DB_AGENT_EVAL_INDEX,
                 CMS_DB_AGENT_EVAL_ORIGIN]));
        foreach ($__array AS $key => $value)
            $update[] = "$key='$value'";

        //store task
        $query =
            "INSERT " .   CMS_DB_AGENT_EVAL . " " .
            "(" .         implode(", ", array_keys($_array)) . ") " .
            "VALUES ('" . implode("','", $_array) . "') " .
            "ON DUPLICATE KEY UPDATE " . implode(", ", $update);
        return mysql_query($query);
    }

//--------------------------------------------------------------------------------------------------

    function rate($agent,
                  $context,
                  $index,
                  $value)
    {
        //equal rating unsets
        $value = ($value = max(-1, min(1, (int)$value))) ?
                 "IF(" . CMS_DB_AGENT_EVAL_RATING . "<=>" . $value . ",NULL," . $value . ")" :
                 "NULL";

        $query =
            "UPDATE " . CMS_DB_AGENT_EVAL . " " .
            "SET " .    CMS_DB_AGENT_EVAL_RATING .  "=" .  $value . " " .
            "WHERE " .  CMS_DB_AGENT_EVAL_AGENT .   "='" . sqlesc($agent) . "' " .
            "AND " .    CMS_DB_AGENT_EVAL_CONTEXT . "='" . sqlesc($context) . "' " .
            "AND " .    CMS_DB_AGENT_EVAL_INDEX .   "=" .  (int)$index . " " .
            "AND " .    CMS_DB_AGENT_EVAL_STATUS .  "='completed' " .
            "LIMIT 1";
        return mysql_query($query);
    }

//--------------------------------------------------------------------------------------------------

    function clear_agent($agent)
    {
        if (! $this->enabled) return FALSE;

        return mysql_query(
            "DELETE FROM " . CMS_DB_AGENT_EVAL . " " .
            "WHERE " .       CMS_DB_AGENT_EVAL_AGENT . "='" . sqlesc($agent) . "'");
    }

//--------------------------------------------------------------------------------------------------

    function cleanup()
    {
        if (! $this->enabled) return;

        mysql_query(
            "DELETE FROM " . CMS_DB_AGENT_EVAL . " " .
            "WHERE " .       CMS_DB_AGENT_EVAL_TIME . "<=(NOW() - INTERVAL " . $this->limit . " DAY) " .
            "LIMIT 10000");
    }

//--------------------------------------------------------------------------------------------------

    function add_summaries(&$array)
    {
        if (! $this->enabled || blank($array)) return; //disabled or no context

        $_array = array_map(
            fn($value) => "('" . sqlesc($value[2]) . "','" . sqlesc($value[1]) . "')",
            $array);

        //retrieve most recent summary of each context
        $query =
            "SELECT " .
                CMS_DB_AGENT_EVAL_AGENT . ", " .
                CMS_DB_AGENT_EVAL_CONTEXT . ", " .
                "SUBSTRING(" .
                    "MAX(CONCAT(" .
                        "LPAD(" . CMS_DB_AGENT_EVAL_INDEX . ", 20, '0')," .
                        CMS_DB_AGENT_EVAL_SUMMARY . ")), " .
                    "21) " .
            "FROM " .
                CMS_DB_AGENT_EVAL . " " .
            "WHERE (" .
                CMS_DB_AGENT_EVAL_AGENT . ", " .
                CMS_DB_AGENT_EVAL_CONTEXT . ") " .
            "IN (" .
                implode(", ", $_array) . ") " .
            "AND " .
                CMS_DB_AGENT_EVAL_SUMMARY . "!='' " .
            "GROUP BY " .
                CMS_DB_AGENT_EVAL_AGENT . ", " .
                CMS_DB_AGENT_EVAL_CONTEXT;

        $result = mysql_query($query);
        if (! $result) return;

        while ($resultrow = mysql_fetch_row($result))
        {
            foreach ($array AS &$value)
            {
                if ($value[1] !== $resultrow[1]) continue;
                if ($value[2] !== $resultrow[0]) continue;
                $value[3] = $resultrow[2];
            };
        };
    }

//--------------------------------------------------------------------------------------------------

    function evaluate($agent_list = NULL,
                      &$error     = NULL)
    {
        if (! $this->enabled) return FALSE;

        //task without complications
        $task =
            "(" .    CMS_DB_AGENT_EVAL_REQUEST . ">0 " .
            "AND " . CMS_DB_AGENT_EVAL_RETRY .   "=0 " .
            "AND " . CMS_DB_AGENT_EVAL_FAIL .    "=0 " .
            "AND " . CMS_DB_AGENT_EVAL_STATUS .  " IN ('open', 'completed'))";

        //generate daily sums
        $query =
            "SELECT " .

        //agent
                CMS_DB_AGENT_EVAL_AGENT . ", " .

        //days past
                "DATEDIFF(CURDATE(), " . CMS_DB_AGENT_EVAL_TIME . ") AS `#day`, " .

        //reliability
                "SUM(IF(" .
                    CMS_DB_AGENT_EVAL_REQUEST .         "=0 " .
                    "OR " . CMS_DB_AGENT_EVAL_STATUS .  "='failed' " .
                    "OR (" . CMS_DB_AGENT_EVAL_STATUS . "='open' " .
                    "AND " . CMS_DB_AGENT_EVAL_ORIGIN . "!='chat'), " .
                    "0, " .
                    "1-(" . CMS_DB_AGENT_EVAL_RETRY .
                    "+" .   CMS_DB_AGENT_EVAL_FAIL .    ")" .
                    "/" .   CMS_DB_AGENT_EVAL_REQUEST . ")) " .
                "AS `#reliability`, " .
                "SUM(" . CMS_DB_AGENT_EVAL_REQUEST . ">0) " .
                "AS `#reliability_count`, " .

        //proficiency
                "SUM(" . CMS_DB_AGENT_EVAL_TOOL .      ") AS `#tool`, " .
                "SUM(" . CMS_DB_AGENT_EVAL_TOOL_FAIL . ") AS `#tool_fail`, " .

        //rating
                "SUM(" . CMS_DB_AGENT_EVAL_RATING .   ") AS `#rating`, " .
                "COUNT(" . CMS_DB_AGENT_EVAL_RATING . ") AS `#rating_count`, " .

        //speed and efficiency
                "SUM(IF($task, " . CMS_DB_AGENT_EVAL_REQUEST .      ", 0)) AS `#request`, " .
                "SUM(IF($task, " . CMS_DB_AGENT_EVAL_DURATION .     ", 0)) AS `#duration`, " .
                "SUM(IF($task, " . CMS_DB_AGENT_EVAL_TOKEN_OUTPUT . ", 0)) AS `#token`, " .

        //count
                "COUNT(*) AS `#count` " .

        //agent evaluation table
            "FROM " .
                CMS_DB_AGENT_EVAL . " " .

        //retention period
            "WHERE " .
                CMS_DB_AGENT_EVAL_TIME . ">=CURDATE()-INTERVAL " . ($this->limit - 1) . " DAY " .

        //filter agents
            (($agent_list === NULL) ? "" :
            "AND " .
                CMS_DB_AGENT_EVAL_AGENT . " IN ('" . implode("', '", sqlesc($agent_list)) . "') ") .

        //agent and day grouping
            "GROUP BY " .
                CMS_DB_AGENT_EVAL_AGENT . ", `#day`";

        //query in local time
        $offset = mysql_local_time_zone();
        $result = mysql_query($query);
        $error  = mysql_error();

        //reset time zone
        if ($offset) mysql_query("SET time_zone='+00:00'");

        //error
        if ($result === FALSE) return FALSE;

        //measures from sums
        $calc = fn($sum) =>
            ["count"        => $sum["#count"],
             "reliability"  => $sum["#reliability_count"] ? 100 * $sum["#reliability"] / $sum["#reliability_count"] : NULL,
             "proficiency"  => $sum["#tool"] ? 100 * (1 - $sum["#tool_fail"] / $sum["#tool"]) : NULL,
             "rating"       => $sum["#rating_count"] ? 50 * ($sum["#rating"] / $sum["#rating_count"] + 1) : NULL,
             "rating_count" => $sum["#rating_count"],
             "speed"        => $sum["#request"] ? $sum["#duration"] / $sum["#request"] : NULL,
             "efficiency"   => $sum["#request"] ? $sum["#token"] / $sum["#request"] : NULL];

        //collect sums
        $list_sum = []; //sums per agent
        $list_day = []; //sums per agent and day

        while ($resultrow = mysql_fetch_assoc($result))
        {
            //agent and day
            $agent = $resultrow[CMS_DB_AGENT_EVAL_AGENT];
            $day   = (int)$resultrow["#day"];
            unset($resultrow[CMS_DB_AGENT_EVAL_AGENT], $resultrow["#day"]);

            //summarize days
            $list_day[$agent][$day] = $resultrow;
            foreach ($resultrow AS $key => $value)
                $list_sum[$agent][$key] = ($list_sum[$agent][$key] ?? 0) + $value;
        };

        return [array_map($calc, $list_sum),                                   //measures per agent
                array_map(fn($array) => array_map($calc, $array), $list_day)]; //measures per agent and day
    }
};