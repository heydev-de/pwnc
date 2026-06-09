<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   FORUM CLASS
//==================================================================================================

//permission
define("CMS_FORUM_PERMISSION_OPERATOR", "operator");
define("CMS_FORUM_PERMISSION_WRITER",   "writer");
define("CMS_FORUM_PERMISSION_READER",   "reader");

//database
define("CMS_DB_FORUM",                  CMS_DB_PREFIX . "forum");
define("CMS_DB_FORUM_INDEX",            "id");
define("CMS_DB_FORUM_CONTAINER",        "container");
define("CMS_DB_FORUM_USER",             "user");
define("CMS_DB_FORUM_TITLE",            "title");
define("CMS_DB_FORUM_TEXT",             "text");
define("CMS_DB_FORUM_EMAIL",            "email");
define("CMS_DB_FORUM_TIME",             "time");
define("CMS_DB_FORUM_ACCESS",           "access");

//related functions
//--------------------------------------------------------------------------------------------------

function forum_quote($text)
{
    $array  = preg_split("/\r?\n/", $text);
    $result = [];

    foreach ($array AS $value)
    {
        if (preg_match("/^>+ /", $value))
        {
            $result[] = ">$value";
            continue;
        };

        $_array = explode("\n", utf8_wordwrap($value));

        foreach ($_array AS $_value)
            $result[] = "> $_value";
    };

    return implode("\n", $result);
};

class forum
{

//properties
//--------------------------------------------------------------------------------------------------

    public $instance = NULL;
    public $table    = CMS_DB_FORUM;
    public $mysql    = NULL;
    public $operator = NULL;
    public $writer   = NULL;
    public $reader   = NULL;
    public $enabled  = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($instance = NULL)
    {
        $this->mysql    = new mysql();
        $this->instance = $instance;
        if (nstre($instance)) $this->table .= "_$instance";

        $ngram = (streq($this->mysql->software, "MySQL") &&
                 version_compare($this->mysql->version, "5.7.6", ">=")) ?
                 " WITH PARSER ngram" : "";

        if ($this->mysql->verify_table($this->table,
                                       [CMS_DB_FORUM_INDEX     => "INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT",
                                        CMS_DB_FORUM_CONTAINER => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                        CMS_DB_FORUM_USER      => "VARCHAR(40) NOT NULL DEFAULT ''",
                                        CMS_DB_FORUM_TITLE     => "VARCHAR(80) NOT NULL DEFAULT ''",
                                        CMS_DB_FORUM_TEXT      => "TEXT NOT NULL",
                                        CMS_DB_FORUM_EMAIL     => "TINYINT(1) NOT NULL DEFAULT '0'",
                                        CMS_DB_FORUM_TIME      => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                        CMS_DB_FORUM_ACCESS    => "INT UNSIGNED NOT NULL DEFAULT '0'"],
                                       ["FULLTEXT (" . CMS_DB_FORUM_TITLE . ")$ngram",
                                        "FULLTEXT (" . CMS_DB_FORUM_TEXT . ")$ngram"]))
        {
            $this->operator = cms_permission(CMS_FORUM_PERMISSION_OPERATOR . ".$instance");
            $this->writer   = cms_permission(CMS_FORUM_PERMISSION_WRITER . ".$instance");
            $this->reader   = cms_permission(CMS_FORUM_PERMISSION_READER . ".$instance");
            $this->enabled  = TRUE;
        };
    }

//methods
//--------------------------------------------------------------------------------------------------

    function add($index,
                 $title,
                 $text,
                 $email,
                 $test = FALSE)
    {
        $table = sqlesc($this->table, TRUE);
        $index = sqlesc($index);

        if (
             $this->enabled
             &&
             $this->writer
             &&
             ($index || $this->operator)
           )
        {
            if ($test) return TRUE;

            //create post
            if (mysql_query("INSERT `" . $table . "` " .
                                   "(" . CMS_DB_FORUM_CONTAINER . ", " .
                                         CMS_DB_FORUM_USER . ", " .
                                         CMS_DB_FORUM_TITLE . ", " .
                                         CMS_DB_FORUM_TEXT . ", " .
                                         CMS_DB_FORUM_EMAIL . ", " .
                                         CMS_DB_FORUM_TIME . ") " .
                            "VALUES " .
                                  "('" . $index . "', " .
                                   "'" . sqlesc(CMS_USER) . "', " .
                                   "'" . sqlesc($title) . "', " .
                                   "'" . sqlesc($text) . "', " .
                                   "'" . sqlesc($email) . "', " .
                                   "'" . sqlesc(time()) . "')"))
            {
                $_index = mysql_insert_id();

                //log
                $log = new log();
                $log->access("created", $_index);

                //notify by e-mail
                if (
                     $index
                     &&
                     ($result = mysql_query("SELECT " . CMS_DB_FORUM_USER . ", " .
                                                        CMS_DB_FORUM_TITLE . ", " .
                                                        CMS_DB_FORUM_TIME . " " .
                                            "FROM `" .  $table . "` " .
                                            "WHERE " .  CMS_DB_FORUM_INDEX . "='$index' " .
                                            "AND " .    CMS_DB_FORUM_EMAIL . ">0 " .
                                            "LIMIT 1"))
                    &&
                    ($resultrow = mysql_fetch_assoc($result))
                    &&
                    nstre($email = permission_get_email($resultrow[CMS_DB_FORUM_USER]))
                    &&
                    cms_load("smtp")
                  )
                    smtp_send($email,
                              CMS_L_FORUM_001,
                              sprintf(CMS_L_FORUM_002,
                                      $resultrow[CMS_DB_FORUM_TITLE],
                                      u(["forum_index" => $_index])));

                return $_index;
            };
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_add($index)
    {
        return $this->add($index, NULL, NULL, NULL, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function edit($index,
                  $title,
                  $text,
                  $email,
                  $test = FALSE)
    {
        $table = sqlesc($this->table, TRUE);
        $index = sqlesc($index);

        if (
             $this->enabled
             &&
             $this->writer
             &&
             $index
             &&
             (
               $this->operator
               ||
               (
                 nstreq(CMS_USER, "anonymous")
                 &&
                 ($result = mysql_query("SELECT 1 " .
                                        "FROM `" .  $table . "` " .
                                        "WHERE " .  CMS_DB_FORUM_INDEX . "='" . $index . "' " .
                                        "AND " .    CMS_DB_FORUM_USER .  "='" . sqlesc(CMS_USER) . "' " .
                                        "LIMIT 1"))
                 &&
                 mysql_num_rows($result)
               )
             )
             &&
             (
               $test
               ||
               mysql_query("UPDATE `" . $table . "` " .
                           "SET " .     CMS_DB_FORUM_TITLE . "='" . sqlesc($title) . "', " .
                                        CMS_DB_FORUM_TEXT .  "='" . sqlesc($text) . "', " .
                                        CMS_DB_FORUM_EMAIL . "='" . sqlesc($email) . "' " .
                           "WHERE " .   CMS_DB_FORUM_INDEX . "='" . $index . "' " .
                           "LIMIT 1")
             )
           )
            return TRUE;

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_edit($index)
    {
        return $this->edit($index, NULL, NULL, NULL, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function move($index,
                  $parent,
                  $test = FALSE)
    {
        if (
             $this->enabled
             &&
             $this->operator
             &&
             $index
             &&
             ($index !== $parent)
             &&
             ($parent !== $this->mysql->get($index, CMS_DB_FORUM_CONTAINER, $this->table))
             &&
             (! $this->mysql->is_child($parent, $index, $this->table))
             &&
             (
               $test
               ||
               mysql_query("UPDATE `" . sqlesc($this->table, TRUE) . "` " .
                           "SET " .     CMS_DB_FORUM_CONTAINER . "='" . sqlesc($parent) . "' " .
                           "WHERE " .   CMS_DB_FORUM_INDEX .     "='" . sqlesc($index) . "' " .
                           "LIMIT 1")
             )
           )
            return TRUE;

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_move($index,
                       $parent)
    {
        return $this->move($index, $parent, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function delete($index,
                    $test = FALSE)
    {
        if (
             $this->enabled
             &&
             $this->operator
             &&
             $index
             &&
             (
               $test
               ||
               $this->mysql->delete($index, $this->table)
             )
           )
            return TRUE;

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_delete($index)
    {
        return $this->delete($index, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function search($value)
    {
        if (! $this->enabled) return FALSE;

        //log
        $log = new log();
        $log->access("search", $value);

        //input sanitation for boolean mode
        $_value = preg_replace("/(?:[\"\(\)\*\+\-<>@\[\]~]|" . CMS_REGEX_BORDER . ")+/u", " ", $value);
        $_value = preg_replace("/" . CMS_REGEX_WORD . "/u", "\\0*", $_value);

        $query = "SELECT *, " .
                 "MATCH(" .     CMS_DB_FORUM_TITLE . ") " .
                 "AGAINST ('" . sqlesc($value) .    "' IN NATURAL LANGUAGE MODE) * 2 + " .
                 "MATCH(" .     CMS_DB_FORUM_TEXT .  ") " .
                 "AGAINST ('" . sqlesc($value) .    "' IN NATURAL LANGUAGE MODE) + " .
                 "MATCH(" .     CMS_DB_FORUM_TITLE . ") " .
                 "AGAINST ('" . sqlesc($_value) .   "' IN BOOLEAN MODE) * 2 + " .
                 "MATCH(" .     CMS_DB_FORUM_TEXT . ") " .
                 "AGAINST ('" . sqlesc($_value) .   "' IN BOOLEAN MODE) AS `relevance` " .
                 "FROM `" .     sqlesc($this->table, TRUE) . "` " .
                 "WHERE " .     CMS_DB_FORUM_CONTAINER . ">0 " .
                 "HAVING `relevance`>0 " .
                 "ORDER BY `relevance` DESC " .
                 "LIMIT 100";

        return mysql_query($query);
    }

//--------------------------------------------------------------------------------------------------

    function log_access($index)
    {
        if (! $this->enabled) return FALSE;

        mysql_query("UPDATE `" . sqlesc($this->table, TRUE) . "` " .
                    "SET " .     CMS_DB_FORUM_ACCESS . "=" . CMS_DB_FORUM_ACCESS . "+1 " .
                    "WHERE " .   CMS_DB_FORUM_INDEX .  "='" . sqlesc($index) . "' " .
                    "LIMIT 1");
    }
};