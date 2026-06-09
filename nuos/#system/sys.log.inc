<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   LOG REPORT OPTION CLASS
//==================================================================================================

//permission
define("CMS_LOG_PERMISSION_OPERATOR", "operator");

//constants
define("CMS_LOG_REPORT_OPTION_TYPE_NONE", 0);
define("CMS_LOG_REPORT_OPTION_TYPE_TEXT", 1);
define("CMS_LOG_REPORT_OPTION_TYPE_BEAM", 2);

class log_report_option
{

//properties
//--------------------------------------------------------------------------------------------------

    public $name           = NULL;
    public $type           = NULL;
    public $link           = NULL;
    public $link_encoding  = NULL;
    public $link_source    = NULL;
    public $value_function = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($name           = NULL,
                         $type           = NULL,
                         $link           = NULL,
                         $link_encoding  = NULL,
                         $link_source    = NULL,
                         $value_function = NULL)
    {
        $this->name           = $name;
        $this->type           = $type;
        $this->link           = $link;
        $this->link_encoding  = $link_encoding;
        $this->link_source    = $link_source;
        $this->value_function = $value_function;
    }
};

//related functions
//--------------------------------------------------------------------------------------------------

function log_report($sql,
                    $options = NULL)
{
    //query start time
    $time = microtime(TRUE);

    //error
    if (! $result = mysql_query($sql)) return FALSE;

    //compute query execution time
    $time = microtime(TRUE) - $time;

    //operation without selection
    if ($result === TRUE)
    {
        if ($options)
        {
            $rows = mysql_affected_rows();
            echo(sprintf(CMS_L_LOG_004, $rows, $time));
        };

        return TRUE;
    };

    //just return result
    if ($options === FALSE)
    {
        $array = [];
        $row   = 0;

        while ($resultrow = mysql_fetch_row($result))
            $array[$row++] = $resultrow;

        return $array;
    };

    //dimensions
    $rows   = mysql_num_rows($result);
    if ($rows === 0) return TRUE;
    $fields = mysql_num_fields($result);

    //initialize
    $list           = [];
    $visible        = [];
    $link_encoding  = [];
    $link_source    = [];
    $value_function = [];
    $max            = [];
    $sum            = [];
    $factor_max     = [];
    $factor_sum     = [];
    $flag           = [];
    $buffer         = "";

    //table header
    for ($i = 0; $i < $fields; $i++)
    {
        $flag[$i] = FALSE;

        //retrieve field name
        $_name = $name = mysql_field_name($result, $i);
        $_i    = 0;
        while (isset($list[$name])) $name = "$_name (" . ++$_i . ")";
        $_name = $name;

        //add field to list
        $list[$name] = $i;

        if (isset($options[$name])) //has option
        {
            //extended option
            if ($options[$name] instanceof log_report_option)
            {
                $option = $options[$name];

                if (! $option->type) continue; //type none

                //column name
                if (nstre($option->name))
                    $_name = $option->name;

                //type beam
                if ($option->type === CMS_LOG_REPORT_OPTION_TYPE_BEAM)
                {
                    $max[$i] = $sum[$i] = 0;
                }

                //link
                elseif (nstre($option->link))
                {
                    //link source field
                    $link_source[$i] =
                        nstre($option->link_source) ?
                        $option->link_source : $name;

                    //link encoding
                    $link_encoding[$i] =
                        ($option->link_encoding !== "") &&
                        is_callable($option->link_encoding) ?
                        $option->link_encoding : __NAMESPACE__ ."\\x";
                };

                //value function
                if (($option->value_function !== "") && is_callable($option->value_function))
                    $value_function[$i] = $option->value_function;

                $flag[$i] = TRUE;
            }

            //column name only
            elseif (nstre($options[$name]))
            {
                $_name = $options[$name];
            };
        };

        //output column name
        $buffer .= "<th>" . x($_name) . "</th>";

        //add field to visible list
        $visible[$name] = $i;
    };

    //display info
    if (! count($max)) //no beam
        echo("<span class=\"element\">" .
             sprintf(CMS_L_LOG_003, $rows, $fields, $time) .
             "</span>");

    //open table
    ifc_table_open("data varied");

    if (count($max)) //beam enabled
    {
        //set equal column width
        $columns = count($visible);
        $width   = round(100 / $columns, 3);

        echo("<colgroup>");
        for ($i = 0; $i < $columns; $i++)
            echo("<col style=\"WIDTH:$width%\">");
        echo("</colgroup>");

        //retrieve maximum and summarize values
        while ($resultrow = mysql_fetch_row($result))
        {
            foreach ($max as $key => $value)
            {
                $temp = $resultrow[$key];
                if (! is_numeric($temp)) continue;

                $max[$key]  = max($temp, $value);
                $sum[$key] += $temp;
            };
        };

        //compute factors
        foreach ($max as $key => $value)
        {
            $factor_max[$key] = ($value > 0) ? 100 / $value : 0;
            $value            = $sum[$key];
            $factor_sum[$key] = ($value > 0) ? 100 / $value : 0;
        };
    };

    echo("<tr>$buffer</tr>");

    //display data
    mysql_data_seek($result, 0); //reset
    while ($resultrow = mysql_fetch_row($result))
    {
        echo("<tr>");

        foreach ($visible as $name => $i)
        {
            $value = $resultrow[$i];

            if (! $flag[$i]) //no options
            {
                echo("<td>" . x($resultrow[$i]) . "</td>");
                continue;
            };

            //extended option
            $option = $options[$name];

            //display text
            if (($option->type === CMS_LOG_REPORT_OPTION_TYPE_TEXT) || (! is_numeric($value)))
            {
                //value function
                $value = isset($value_function[$i]) ?
                         ($value_function[$i])($value) :
                         x($value);

                //link
                if (isset($link_source[$i]))
                {
                    //link source field
                    $link = $resultrow[$list[$link_source[$i]]];

                    //link encoding
                    $link = ($link_encoding[$i])($link);

                    //generate link
                    $link = sprintf($option->link, $link);

                    //output value
                    echo("<td>" .
                         "<a href=\"" . x($link) ."\">" .
                         $value .
                         "</a>" .
                         "</td>");
                }

                //no link
                else
                {
                    //output value
                    echo("<td>$value</td>");
                };

                continue;
            }

            //display beam
            $_value = number_format($value, 0, NULL, CMS_L_THOUSAND_SEPARATOR);
            echo("<td>" .
                 "<div class=\"beam\" " .
                      "style=\"--width:" . (int)($value * $factor_max[$i]) . "\">" .
                 "</div> " .
                 "$_value (" .
                 number_format($value * $factor_sum[$i], 1,
                               CMS_L_DECIMAL_SEPARATOR, NULL) . "%)" .
                 "</td>");
        };

        echo("</tr>");
    };

    //table footer
    if (count($max)) //beam enabled
    {
        echo("<tr>");

        foreach ($visible as $i)
        {
            if (! isset($sum[$i]))
            {
                echo("<td></td>");
                continue;
            };

            $value = number_format($sum[$i], 0, NULL, CMS_L_THOUSAND_SEPARATOR);
            echo("<td>" .
                 sprintf(CMS_L_LOG_002, $value) .
                 "</td>");
        };

        echo("</tr>");
    };

    ifc_table_close();

    return $rows;
};

//==================================================================================================
//   LOG CLASS
//==================================================================================================

//database
define("CMS_DB_LOG_ACCESS",          CMS_DB_PREFIX . "log_access");
define("CMS_DB_LOG_ACCESS_INDEX",    "id");
define("CMS_DB_LOG_ACCESS_USERID",   "userid");
define("CMS_DB_LOG_ACCESS_TIME",     "time");
define("CMS_DB_LOG_ACCESS_IP",       "ip");
define("CMS_DB_LOG_ACCESS_USER",     "user");
define("CMS_DB_LOG_ACCESS_ACTION",   "action");
define("CMS_DB_LOG_ACCESS_INFO",     "info");
define("CMS_DB_LOG_ACCESS_URL",      "url");
define("CMS_DB_LOG_ACCESS_URL_HASH", "url_hash");
define("CMS_DB_LOG_ACCESS_PATH",     "path");
define("CMS_DB_LOG_ACCESS_REFERRER", "referrer");
define("CMS_DB_LOG_ACCESS_DOMAIN",   "domain");
define("CMS_DB_LOG_ACCESS_AGENT",    "agent");
define("CMS_DB_LOG_ACCESS_BROWSER",  "browser");
define("CMS_DB_LOG_ACCESS_MOBILE",   "mobile");
define("CMS_DB_LOG_ACCESS_LANGUAGE", "language");
define("CMS_DB_LOG_ACCESS_REGION",   "region");
define("CMS_DB_LOG_ACCESS_BOT",      "bot");
define("CMS_DB_LOG_ACCESS_ERROR",    "error");

define("CMS_DB_LOG_USER",            CMS_DB_PREFIX . "log_user");
define("CMS_DB_LOG_USER_USERID",     "userid");
define("CMS_DB_LOG_USER_NAME",       "name");
define("CMS_DB_LOG_USER_EMAIL",      "email");
define("CMS_DB_LOG_USER_INFO",       "info");
define("CMS_DB_LOG_USER_BOT",        "bot");
define("CMS_DB_LOG_USER_BOT_TIME",   "bot_time");

define("CMS_DB_LOG_UA_LIST",         CMS_DB_PREFIX . "log_ua_list");
define("CMS_DB_LOG_UA_LIST_HASH",    "hash");

define("CMS_DB_LOG_UA_FREQ",         CMS_DB_PREFIX . "log_ua_freq");
define("CMS_DB_LOG_UA_FREQ_TOKEN",   "token");
define("CMS_DB_LOG_UA_FREQ_VALUE",   "value");

//constants
define("CMS_LOG_STATUS_USER_FIXED",         -1);
define("CMS_LOG_STATUS_USER_PROVISIONAL",    0);
define("CMS_LOG_STATUS_BOT_PROVISIONAL",     1);
define("CMS_LOG_STATUS_BOT_LIMIT_EXCEEDED",  2);
define("CMS_LOG_STATUS_BOT_FIXED",           3);
define("CMS_LOG_STATUS_BAD_BOT",             4);

class log
{

//properties
//--------------------------------------------------------------------------------------------------

    public $limit         = NULL;
    public $anonymize     = NULL;
    public $privacy       = NULL;
    public $bot_limit     = 5;
    public $bad_bot_limit = 10;
    public $bad_bot_delay = 5;
    public $bad_bot_block = FALSE;
    public $bot_reset     = 14;
    public $bot_retention = 30;
    public $operator      = FALSE;
    public $enabled       = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct()
    {
        //establish database connection
        $mysql = new mysql();

        if (
             $mysql->verify_table(CMS_DB_LOG_ACCESS,
                                  [CMS_DB_LOG_ACCESS_INDEX    => "INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT",
                                   CMS_DB_LOG_ACCESS_USERID   => "CHAR(32) NOT NULL DEFAULT ''",
                                   CMS_DB_LOG_ACCESS_TIME     => "TIMESTAMP NOT NULL DEFAULT '0000-00-00 00:00:00'",
                                   CMS_DB_LOG_ACCESS_IP       => "VARCHAR(91) NOT NULL DEFAULT ''",
                                   CMS_DB_LOG_ACCESS_USER     => "VARCHAR(40) NOT NULL DEFAULT ''",
                                   CMS_DB_LOG_ACCESS_ACTION   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_LOG_ACCESS_INFO     => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_LOG_ACCESS_URL      => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_LOG_ACCESS_URL_HASH => "BINARY(16)",
                                   CMS_DB_LOG_ACCESS_PATH     => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_LOG_ACCESS_REFERRER => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_LOG_ACCESS_DOMAIN   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_LOG_ACCESS_AGENT    => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_LOG_ACCESS_BROWSER  => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_LOG_ACCESS_MOBILE   => "TINYINT(1) UNSIGNED NOT NULL DEFAULT 0",
                                   CMS_DB_LOG_ACCESS_LANGUAGE => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_LOG_ACCESS_REGION   => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_LOG_ACCESS_BOT      => "TINYINT(1) UNSIGNED NOT NULL DEFAULT 1",
                                   CMS_DB_LOG_ACCESS_ERROR    => "TINYINT(1) UNSIGNED NOT NULL DEFAULT 0"],
                                  ["INDEX (" . CMS_DB_LOG_ACCESS_USERID . ")",
                                   "INDEX (" . CMS_DB_LOG_ACCESS_TIME . ", " .
                                               CMS_DB_LOG_ACCESS_USERID . ", " .
                                               CMS_DB_LOG_ACCESS_ERROR . ")",
                                   "INDEX (" . CMS_DB_LOG_ACCESS_TIME . ", " .
                                               CMS_DB_LOG_ACCESS_BOT . ", " .
                                               CMS_DB_LOG_ACCESS_USERID . ", " .
                                               CMS_DB_LOG_ACCESS_MOBILE . ")",
                                   "INDEX (" . CMS_DB_LOG_ACCESS_PATH . "(40))",
                                   "INDEX (" . CMS_DB_LOG_ACCESS_DOMAIN . "(8))",
                                   "INDEX (" . CMS_DB_LOG_ACCESS_BROWSER  . "(4))",
                                   "INDEX (" . CMS_DB_LOG_ACCESS_LANGUAGE . "(10))",
                                   "INDEX (" . CMS_DB_LOG_ACCESS_REGION . "(2))",
                                   "INDEX (" . CMS_DB_LOG_ACCESS_ACTION . "(20))"],
                                  ["client"                   => CMS_DB_LOG_ACCESS_AGENT,
                                   "date"                     => CMS_DB_LOG_ACCESS_TIME,
                                   "datetime"                 => CMS_DB_LOG_ACCESS_TIME,
                                   "referer"                  => CMS_DB_LOG_ACCESS_REFERRER])
             &&
             $mysql->verify_table(CMS_DB_LOG_USER,
                                  [CMS_DB_LOG_USER_USERID     => "CHAR(32) NOT NULL PRIMARY KEY",
                                   CMS_DB_LOG_USER_NAME       => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_LOG_USER_EMAIL      => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_LOG_USER_INFO       => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_LOG_USER_BOT        => "TINYINT(1) NOT NULL " .
                                                                 "DEFAULT " . CMS_LOG_STATUS_BOT_PROVISIONAL,
                                   CMS_DB_LOG_USER_BOT_TIME   => "TIMESTAMP NOT NULL DEFAULT '0000-00-00 00:00:00'"],
                                  ["INDEX (" . CMS_DB_LOG_USER_BOT . ", " .
                                               CMS_DB_LOG_USER_BOT_TIME . ")"],
                                  ["comment"                  => CMS_DB_LOG_USER_INFO])
             &&
             $mysql->verify_table(CMS_DB_LOG_UA_LIST,
                                  [CMS_DB_LOG_UA_LIST_HASH    => "BINARY(16) NOT NULL PRIMARY KEY"])
             &&
             $mysql->verify_table(CMS_DB_LOG_UA_FREQ,
                                  [CMS_DB_LOG_UA_FREQ_TOKEN   => "VARCHAR(255) NOT NULL PRIMARY KEY",
                                   CMS_DB_LOG_UA_FREQ_VALUE   => "BIGINT UNSIGNED NOT NULL DEFAULT 0"])
           )
        {
            $system              = new system();
            $this->limit         = $system->getval("log", "limit");
            $this->anonymize     = $system->getval("log", "anonymize");
            $this->privacy       = $system->getval("log", "privacy");
            $this->bot_limit     = $system->getval("log", "bot_limit")     ?? 5;
            $this->bad_bot_limit = $system->getval("log", "bad_bot_limit") ?? 10;
            $this->bad_bot_delay = $system->getval("log", "bad_bot_delay") ?? 5;
            $this->bad_bot_block = $system->getval("log", "bad_bot_block") ?? FALSE;
            $this->bot_reset     = $system->getval("log", "bot_reset")     ?? 14;
            $this->bot_retention = $system->getval("log", "bot_retention") ?? 30;
            $this->operator      = defined("CMS_USER") ? cms_permission(CMS_LOG_PERMISSION_OPERATOR) : FALSE;
            $this->enabled       = TRUE;
        };
    }

//methods
//--------------------------------------------------------------------------------------------------

    function access($action = NULL,
                    $info   = NULL)
    {
        if ((! $this->enabled) || ($this->limit < 0)) return; //skip disabled

        //reload flag
        $reload = FALSE;

        //user
        $user = "";
        if (defined("CMS_USER"))
        {
            //if (streq(CMS_USER, "admin")) return; //skip admin
            $user = streq(CMS_USER, "anonymous") ? "" : CMS_USER;
        };

        //user id
        $userid = sqlesc(CMS_IPHASH);

        //url
        $url  = CMS_HOST . $_SERVER["REQUEST_URI"] ?? "";
        $hash = hash32($url, TRUE);
        $path = (string)parse_url($url, PHP_URL_PATH);

        //error
        $error = isset($_SERVER["REDIRECT_STATUS"]) &&
                 streq($_SERVER["REDIRECT_STATUS"], "404");

        //retrieve current bot status
        $query  = "SELECT " . CMS_DB_LOG_USER_BOT . ", " .
                  "(" .       CMS_DB_LOG_USER_BOT_TIME .
                  "<=NOW() - INTERVAL " . (int)$this->bad_bot_delay . " SECOND) " .
                  "FROM " .   CMS_DB_LOG_USER . " " .
                  "WHERE " .  CMS_DB_LOG_USER_USERID .
                  "='" .      $userid . "' " .
                  "LIMIT 1";
        $result = mysql_query($query);

        //known user
        if ($result && ($resultrow = mysql_fetch_row($result)))
        {
            $bot = $_bot = (int)$resultrow[0];
            $bot_time    = (bool)$resultrow[1];

            //check access rate within last 10 seconds
            $query  = "SELECT " .   CMS_DB_LOG_ACCESS_URL_HASH . ", " .
                                    CMS_DB_LOG_ACCESS_ACTION . ", " .
                                    CMS_DB_LOG_ACCESS_INFO . " " .
                      "FROM " .     CMS_DB_LOG_ACCESS . " " .
                      "WHERE " .    CMS_DB_LOG_ACCESS_TIME .
                      ">NOW() - INTERVAL 10 SECOND " .
                      "AND " .      CMS_DB_LOG_ACCESS_USERID .
                      "='" .        $userid . "' " .
                      "ORDER BY " . CMS_DB_LOG_ACCESS_INDEX . " DESC " .
                      "LIMIT " .    (int)$this->bot_limit;
            $result = mysql_query($query);
            while ($result && ($resultrow = mysql_fetch_row($result)))
            {
                //repeated access
                if (
                     streq($resultrow[0], $hash)
                     &&
                     streq($resultrow[1], $action)
                     &&
                     streq($resultrow[2], $info)
                   )
                {
                    $reload = TRUE;
                    break;
                };

                if (

                //still provisional
                     in_array($_bot, [CMS_LOG_STATUS_USER_PROVISIONAL,
                                      CMS_LOG_STATUS_BOT_PROVISIONAL])
                     &&

                //access rate exceeds bot limit
                     (mysql_num_rows($result) >= $this->bot_limit)
                   )
                    $_bot = CMS_LOG_STATUS_BOT_LIMIT_EXCEEDED;

                //check invalid url access rate within last 10 seconds
                if ($error && ($_bot === CMS_LOG_STATUS_BOT_LIMIT_EXCEEDED))
                {
                    $query  = "SELECT 1 " .
                              "FROM " .     CMS_DB_LOG_ACCESS . " " .
                              "WHERE " .    CMS_DB_LOG_ACCESS_TIME .
                              ">NOW() - INTERVAL 10 SECOND " .
                              "AND " .      CMS_DB_LOG_ACCESS_USERID .
                              "='" .        $userid . "' " .
                              "AND " .      CMS_DB_LOG_ACCESS_ERROR . "=1 " .
                              "ORDER BY " . CMS_DB_LOG_ACCESS_INDEX . " DESC " .
                              "LIMIT " .    (int)$this->bad_bot_limit;
                    $result = mysql_query($query);
                    if ($result && (mysql_num_rows($result) >= $this->bad_bot_limit))
                        $_bot = CMS_LOG_STATUS_BAD_BOT;
                };

                break;
            };
        }

        //new user
        else
        {
            $bot      = NULL;
            $_bot     = stre($user) ?
                        CMS_LOG_STATUS_BOT_PROVISIONAL :
                        CMS_LOG_STATUS_USER_PROVISIONAL;
            $bot_time = FALSE;
        };

        if (! $reload)
        {
            //create user or set changed bot status
            if ($bot !== $_bot) $this->user(NULL, NULL, NULL, $_bot, NULL, FALSE);

            //action
            $action = cms_application() . (stre($action) ? "" : ":$action");

            //implementation of abnf from rfc 3986
            $dec_octet   = "(?:\d|[1-9]\d|1\d{2}|2[0-4]\d|25[0-5])";
            $ipv4address = "(?:$dec_octet\.){3}$dec_octet";
            $h16         = "[0-9a-f]{1,4}";
            $ls32        = "(?:$h16:$h16|$ipv4address)";
            $ipv6address = "(?:" .
                               "(?:" .
                                   "(?:$h16:){6}" .
                                   "|" .
                                   "::(?:$h16:){5}" .
                                   "|" .
                                   "(?:$h16)?::(?:$h16:){4}" .
                                   "|" .
                                   "(?:(?:$h16:){0,1}$h16)?::(?:$h16:){3}" .
                                   "|" .
                                   "(?:(?:$h16:){0,2}$h16)?::(?:$h16:){2}" .
                                   "|" .
                                   "(?:(?:$h16:){0,3}$h16)?::$h16:" .
                                   "|" .
                                   "(?:(?:$h16:){0,4}$h16)?::" .
                               ")" .
                               "$ls32" .
                               "|" .
                               "(?:" .
                                    "(?:(?:$h16:){0,5}$h16)?::$h16" .
                                    "|" .
                                    "(?:($h16:){0,6}$h16)?::" .
                               ")" .
                               "|" .
                               "$ipv4address" . //additional ipv4 support
                           ")";

            //ip address
            if (
                 nstre($ip = $_SERVER["HTTP_X_FORWARDED_FOR"] ?? "")
                 &&
                 preg_match("/$ipv6address/i", $ip, $_ip)
               )
            {
                $ip  = $_ip[0];
                if ($this->anonymize) $ip = anonymize_ip($ip);
                $ip .= "|";
            };

            $_ip  = $_SERVER["REMOTE_ADDR"] ?? "";
            if ($this->anonymize) $_ip = anonymize_ip($_ip);
            $ip  .= $_ip;

            //referrer
            $referrer = $_SERVER["HTTP_REFERER"] ?? "";
            $domain   = (string)parse_url($referrer, PHP_URL_HOST);

            //agent / browser / mobile
            $agent   = $_SERVER["HTTP_USER_AGENT"] ?? "";
            $browser = $this->browser($agent);
            $mobile  = preg_match("/(?:^|-)Mobile$/", $browser);

            //language
            $language = $_SERVER["HTTP_ACCEPT_LANGUAGE"] ?? "";
            $language = preg_replace("/\s+/", "", $language);
            $list     = [];
            $region   = "";
            $q        = 0;

            preg_match_all(
                "/(?:^|,)" .                                   //entry start
                "([a-z]{2,3})" .                               //language [1]
                "(?:-[a-z]{4})?" .                             //script
                "(?:-([a-z]{2}|[0-9]{3}))?" .                  //region [2]
                "(?:-[a-z0-9]{1,8})*?" .                       //other tags
                "(;q=)?(0(?:\.[0-9]{1,3})?|1(?:\.0{1,3})?)?" . //priority [3][4]
                "(?=,|$)/i",                                   //entry end
                $language,
                $language,
                PREG_SET_ORDER | PREG_UNMATCHED_AS_NULL);

            foreach ($language as $value)
            {
                //priority
                $_q = 1.0;
                if (($value[3] !== NULL) && ($value[4] !== NULL))
                {
                    $_q = (float)$value[4];
                    if ($_q === 0.0) continue;
                };

                //code
                $code = strtolower($value[1]);
                if ((! isset($list[$code])) || ($_q > $list[$code]))
                    $list[$code] = $_q;

                //region
                if ($_q > $q)
                {
                    $region = ($value[2] !== NULL) ? strtoupper($value[2]) : $code;
                    $q      = $_q;
                };
            };

            arsort($list, SORT_NUMERIC);
            $language = implode(", ", array_keys($list));

            //write log record
            $query = "INSERT " .  CMS_DB_LOG_ACCESS .
                     " (" .       CMS_DB_LOG_ACCESS_USERID . ", " .
                                  CMS_DB_LOG_ACCESS_TIME . ", " .
                                  CMS_DB_LOG_ACCESS_IP . ", " .
                                  CMS_DB_LOG_ACCESS_USER . ", " .
                                  CMS_DB_LOG_ACCESS_ACTION . ", " .
                                  CMS_DB_LOG_ACCESS_INFO . ", " .
                                  CMS_DB_LOG_ACCESS_URL . ", " .
                                  CMS_DB_LOG_ACCESS_URL_HASH . ", " .
                                  CMS_DB_LOG_ACCESS_PATH . ", " .
                                  CMS_DB_LOG_ACCESS_REFERRER . ", " .
                                  CMS_DB_LOG_ACCESS_DOMAIN . ", " .
                                  CMS_DB_LOG_ACCESS_AGENT . ", " .
                                  CMS_DB_LOG_ACCESS_BROWSER . ", " .
                                  CMS_DB_LOG_ACCESS_MOBILE . ", " .
                                  CMS_DB_LOG_ACCESS_LANGUAGE . ", " .
                                  CMS_DB_LOG_ACCESS_REGION . ", " .
                                  CMS_DB_LOG_ACCESS_BOT . ", " .
                                  CMS_DB_LOG_ACCESS_ERROR . ") " .
                     "VALUES (" .
                          "'" .   $userid .            "', " .
                                 "NOW(), " .
                          "'" .   sqlesc($ip) .        "', " .
                          "'" .   sqlesc($user) .      "', " .
                     "LEFT('" .   sqlesc($action) .    "', 255), " .
                     "LEFT('" .   sqlesc($info) .      "', 255), " .
                     "LEFT('" .   sqlesc($url) .       "', 255), " .
                     "_binary'" . sqlesc($hash) .      "', " .
                     "LEFT('" .   sqlesc($path) .      "', 255), " .
                     "LEFT('" .   sqlesc($referrer) .  "', 255), " .
                     "LEFT('" .   sqlesc($domain) .    "', 255), " .
                     "LEFT('" .   sqlesc($agent) .     "', 255), " .
                     "LEFT('" .   sqlesc($browser) .   "', 255), " .
                     "LEFT('" .   sqlesc($mobile) .    "', 255), " .
                     "LEFT('" .   sqlesc($language) .  "', 255), " .
                     "LEFT('" .   sqlesc($region) .    "', 255), " .
                     (($_bot > 0) ? 1 : 0) . ", " .
                     ($error      ? 1 : 0) . ")";
            mysql_query($query);
        };

        //block bad bots
        if (
             $this->bad_bot_block
             &&
             ($_bot === CMS_LOG_STATUS_BAD_BOT)
             &&
             $bot_time
             &&
             nstreq($user, "admin")
           )
        {
            //clear and quit
            header_remove();
            while (ob_get_level()) ob_end_clean();
            cms_error_silent();
            exit();
        };
    }

//--------------------------------------------------------------------------------------------------

    function user($name   = NULL,
                  $email  = NULL,
                  $info   = NULL,
                  $bot    = NULL,
                  $userid = NULL,
                  $append = TRUE)
    {
        if (! $this->enabled) return;

        $name  = trim($name);
        $email = trim($email);
        $info  = trim($info);
        $_name = $_email = $_info = $_bot = $time = NULL;

        //retrieve user from database
        $userid  = nstre($userid) ? $userid : CMS_IPHASH;
        $_userid = sqlesc($userid);
        $result  = mysql_query("SELECT " . CMS_DB_LOG_USER_NAME . ", " .
                                           CMS_DB_LOG_USER_EMAIL . ", " .
                                           CMS_DB_LOG_USER_INFO . ", " .
                                           CMS_DB_LOG_USER_BOT . ", " .
                                           CMS_DB_LOG_USER_BOT_TIME . " " .
                               "FROM " .   CMS_DB_LOG_USER . " " .
                               "WHERE " .  CMS_DB_LOG_USER_USERID . "='$_userid' " .
                               "LIMIT 1");
        if ($result && ($resultrow = mysql_fetch_row($result))) //user exists
        {
            if ("$name$email$info$bot" === "") return; //no new data

            $_name  = $resultrow[0];
            $_email = $resultrow[1];
            $_info  = $resultrow[2];
            $_bot   = (int)$resultrow[3];
            $time   = $resultrow[4];
        };

        //name
        if (nstre($name))
        {
            if ($append)
            {
                //keep existing name or generate pseudonym
                if ($this->privacy)
                {
                    $name = nstre($_name) ? $_name : generate_pseudonym($userid);
                }

                //use new name and append existing name
                elseif (nstre($_name))
                {
                    $array = array_map("trim", explode(",", $_name));
                    $array = array_filter($array, function($value) use ($name) { return $value !== $name; });
                    if (array_unshift($array, $name) > 3) $array = array_slice($array, 0, 3);
                    $name  = implode(", ", $array);
                };
            };
        }
        elseif (nstre($_name))
        {
            $name = $_name;
        }
        else
        {
            $name = generate_pseudonym($userid);
        };

        //email
        if (nstre($email))
        {
            if ($append)
            {
                //keep existing email or set empty
                if ($this->privacy)
                {
                    $email = nstre($_email) ? $_email : NULL;
                }

                //use new email and append existing email
                else
                {
                    $array = array_map("trim", explode(",", $_email));
                    $array = array_filter($array, function($value) use ($email) { return $value !== $email; });
                    if (array_unshift($array, $email) > 3) $array = array_slice($array, 0, 3);
                    $email = implode(", ", $array);
                };
            };
        }
        elseif (nstre($_email))
        {
            $email = $_email;
        };

        //info
        if (stre($info)) $info = $_info;

        //bot
        if ($bot === NULL) $bot = ($_bot === NULL) ? CMS_LOG_STATUS_BOT_PROVISIONAL : $_bot;
        else               $bot = (int)$bot;

        //old status is provisional
        $array = [CMS_LOG_STATUS_USER_PROVISIONAL, CMS_LOG_STATUS_BOT_PROVISIONAL];
        $flag  = in_array($_bot, $array);

        //status changes and either old or new status is not provisional
        $_flag = ($bot !== $_bot) && (! $flag || ! in_array($bot, $array));

        //bot time
        $bot_time = $_flag ? "NOW()" : CMS_DB_LOG_USER_BOT_TIME;

        //add or update user
        $query  = "INSERT " . CMS_DB_LOG_USER .
                       " (" . CMS_DB_LOG_USER_USERID . ", " .
                              CMS_DB_LOG_USER_NAME . ", " .
                              CMS_DB_LOG_USER_EMAIL . ", " .
                              CMS_DB_LOG_USER_INFO . ", " .
                              CMS_DB_LOG_USER_BOT . ", " .
                              CMS_DB_LOG_USER_BOT_TIME . ") " .
                  "VALUES (" .
                            "'$_userid', " .
                  "LEFT('" .  sqlesc($name) .  "', 255), " .
                  "LEFT('" .  sqlesc($email) . "', 255), " .
                  "LEFT('" .  sqlesc($info) .  "', 255), " .
                              $bot . ", " .
                              $bot_time . ") " .
                  "ON DUPLICATE KEY UPDATE " .
                  CMS_DB_LOG_USER_NAME .     "=VALUES(" . CMS_DB_LOG_USER_NAME . "), " .
                  CMS_DB_LOG_USER_EMAIL .    "=VALUES(" . CMS_DB_LOG_USER_EMAIL . "), " .
                  CMS_DB_LOG_USER_INFO .     "=VALUES(" . CMS_DB_LOG_USER_INFO . "), " .
                  CMS_DB_LOG_USER_BOT .      "=VALUES(" . CMS_DB_LOG_USER_BOT . "), " .
                  CMS_DB_LOG_USER_BOT_TIME . "=VALUES(" . CMS_DB_LOG_USER_BOT_TIME . ")";
        $result = (bool)mysql_query($query);

        //old status is provisional and user/bot status changes
        if ($flag && (($bot > 0) !== ($_bot > 0)))
        {
            //retrospectively set bot flag in access table
            $bot   = ($bot > 0) ? 1 : 0;
            $query = "UPDATE " . CMS_DB_LOG_ACCESS . " " .
                     "SET " .    CMS_DB_LOG_ACCESS_BOT .    "=$bot " .
                     "WHERE " .  CMS_DB_LOG_ACCESS_USERID . "='$_userid'";
            if ($time !== NULL) $query .=
                " AND " .    CMS_DB_LOG_ACCESS_TIME .
                 ">'" .      sqlesc($time) . "'";
            $result |= (bool)mysql_query($query);
        };

        return $result;
    }

//--------------------------------------------------------------------------------------------------

    function set_user($userid,
                      $name,
                      $email,
                      $info,
                      $bot)
    {
        return $this->user($name, $email, $info, $bot, $userid, FALSE);
    }

//--------------------------------------------------------------------------------------------------

    function cleanup()
    {
        if (! $this->enabled) return;

        //remove bot access entries exceeding limit
        if ($this->bot_retention > 0)
            mysql_query(
                "DELETE FROM " . CMS_DB_LOG_ACCESS . " " .
                "WHERE " .       CMS_DB_LOG_ACCESS_TIME .
                "<=" .          "(NOW() - INTERVAL " . (int)$this->bot_retention . " DAY) " .
                "AND " .         CMS_DB_LOG_ACCESS_BOT .
                ">0 " .
                "LIMIT 10000");

        //remove access entries exceeding limit
        if ($this->limit > 0)
            mysql_query(
                "DELETE FROM " . CMS_DB_LOG_ACCESS . " " .
                "WHERE " .       CMS_DB_LOG_ACCESS_TIME .
                "<=" .          "(NOW() - INTERVAL " . (int)$this->limit . " DAY) " .
                "LIMIT 10000");

        //remove orphaned users
        mysql_query(
            "DELETE `#t1`.* " .
            "FROM " .        CMS_DB_LOG_USER .   " AS `#t1` " .
            "LEFT JOIN " .   CMS_DB_LOG_ACCESS . " AS `#t2` " .
            "ON `#t1`." .    CMS_DB_LOG_USER_USERID .
            "=`#t2`." .      CMS_DB_LOG_ACCESS_USERID . " " .
            "WHERE `#t2`." . CMS_DB_LOG_ACCESS_USERID . " " .
            "IS NULL " .
            "AND `#t1`." .   CMS_DB_LOG_USER_BOT . " " .
            "IN (" .         CMS_LOG_STATUS_USER_PROVISIONAL . ", " .
                             CMS_LOG_STATUS_BOT_PROVISIONAL . ", " .
                             CMS_LOG_STATUS_BOT_LIMIT_EXCEEDED . ", " .
                             CMS_LOG_STATUS_BAD_BOT . ")");

        //reset to provisional bot status
        if ($this->bot_reset > 0)
        {
            $query = "SELECT `#t2`." .   CMS_DB_LOG_ACCESS_TIME . " " .
                     "FROM " .           CMS_DB_LOG_ACCESS . " AS `#t2` " .
                     "WHERE `#t2`." .    CMS_DB_LOG_ACCESS_USERID .
                     "=`#t1`." .         CMS_DB_LOG_USER_USERID . " " .
                     "ORDER BY `#t2`." . CMS_DB_LOG_ACCESS_INDEX . " DESC " .
                     "LIMIT 1";

            $query = "UPDATE " .      CMS_DB_LOG_USER . " AS `#t1` " .
                     "SET `#t1`." .   CMS_DB_LOG_USER_BOT .
                     "=" .            CMS_LOG_STATUS_BOT_PROVISIONAL . ", " .
                     "`#t1`." .       CMS_DB_LOG_USER_BOT_TIME .
                     "=NOW() " .
                     "WHERE `#t1`." . CMS_DB_LOG_USER_BOT . " " .
                     "IN (" .         CMS_LOG_STATUS_USER_PROVISIONAL . ", " .
                                      CMS_LOG_STATUS_BOT_LIMIT_EXCEEDED . ", " .
                                      CMS_LOG_STATUS_BAD_BOT . ") " .
                     "AND (" .        $query . ")" .
                     "<=(NOW() - INTERVAL " . (int)$this->bot_reset . " DAY) " .
                     "LIMIT 1000";

            mysql_query($query);
        };
    }

//--------------------------------------------------------------------------------------------------

    function browser($string,
                     $count = 1)
    {
        $string = strtolower($string);

        //tokenize user agent string
        preg_match_all(
            "/(?:^|\s)" .                           //entry start
            //"([^(\/\s]+)" .                         //name
            "([a-z](?:[0-9a-z]|-(?!-))+[0-9a-z])" . //name
            "(?:\/\S*)?" .                          //version
            "(?:\s*\([^)]+(?:\)|$))?/i",            //parentheses
            $string,
            $token,
            PREG_SET_ORDER);
        $token = array_unique(array_column($token, 1));
        $token = array_diff($token, ["compatible", "like", "mobile", "version"]);
        sort($token, SORT_NATURAL);

        //get token frequencies
        $freq  = array_fill_keys($token, 0);
        $list  = array_map(__NAMESPACE__ . "\\sqlesc", $token);
        $query = "SELECT " . CMS_DB_LOG_UA_FREQ_TOKEN . ", " .
                             CMS_DB_LOG_UA_FREQ_VALUE . " " .
                 "FROM " .   CMS_DB_LOG_UA_FREQ . " " .
                 "WHERE " .  CMS_DB_LOG_UA_FREQ_TOKEN . " " .
                 "IN ('" .   implode("', '", $list) . "')";
        if ($result = mysql_query($query))
        {
            while ($resultrow = mysql_fetch_row($result))
                $freq[$resultrow[0]] = (int)$resultrow[1];
            arsort($freq, SORT_NUMERIC);
        };

        //increase token frequencies
        $hash  = sqlesc(hash32(implode(" ", $token), TRUE));
        $query = "INSERT IGNORE INTO " . CMS_DB_LOG_UA_LIST . " " .
                 "(" .                   CMS_DB_LOG_UA_LIST_HASH . ") " .
                 "VALUES (_binary'" .    $hash . "')";
        if (mysql_query($query) && (mysql_affected_rows() > 0))
        {
            $query = "INSERT INTO " .             CMS_DB_LOG_UA_FREQ . " " .
                     "(" .                        CMS_DB_LOG_UA_FREQ_TOKEN . ", " .
                                                  CMS_DB_LOG_UA_FREQ_VALUE . ") " .
                     "VALUES ('" .                implode("', 1), ('", $list) . "', 1) " .
                     "ON DUPLICATE KEY UPDATE " . CMS_DB_LOG_UA_FREQ_VALUE .
                     "=" .                        CMS_DB_LOG_UA_FREQ_VALUE . "+1";
            mysql_query($query);
        };

        //lowest frequency token combination
        $freq = array_keys(array_slice($freq, -$count, $count, TRUE));
        $freq = array_map("ucfirst", $freq);

        //add mobile token
        if (strpos($string, "mobile") !== FALSE) $freq[] = "Mobile";

        return implode("-", $freq);
    }
};