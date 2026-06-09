<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   COMMENT CLASS
//==================================================================================================

//permission
define("CMS_COMMENT_PERMISSION_OPERATOR", "operator");
define("CMS_COMMENT_PERMISSION_WRITER",   "writer");
define("CMS_COMMENT_PERMISSION_READER",   "reader");

//status
define("CMS_DB_COMMENT_STATUS_INACTIVE",  0);
define("CMS_DB_COMMENT_STATUS_ACTIVE",    1);
define("CMS_DB_COMMENT_STATUS_HIDDEN",    2);

//database
define("CMS_DB_COMMENT",                  CMS_DB_PREFIX . "comment");
define("CMS_DB_COMMENT_INDEX",            "id");
define("CMS_DB_COMMENT_INSTANCE",         "instance");
define("CMS_DB_COMMENT_STATUS",           "status");
define("CMS_DB_COMMENT_TIME",             "time");
define("CMS_DB_COMMENT_NAME",             "name");
define("CMS_DB_COMMENT_EMAIL",            "email");
define("CMS_DB_COMMENT_URL",              "url");
define("CMS_DB_COMMENT_TEXT",             "text");
define("CMS_DB_COMMENT_HASH",             "hash");
define("CMS_DB_COMMENT_RATING_VALUE",     "rating_value");
define("CMS_DB_COMMENT_RATING_COUNT",     "rating_count");
define("CMS_DB_COMMENT_RATING_USERID",    "rating_userid");
define("CMS_DB_COMMENT_SPAM_PROBABILITY", "spam_probability");

class comment
{

//properties
//--------------------------------------------------------------------------------------------------

    public $instance       = "";
    public $enabled        = FALSE;
    public $operator       = FALSE;
    public $writer         = FALSE;
    public $reader         = FALSE;
    public $default_status = CMS_DB_COMMENT_STATUS_INACTIVE;
    public $spam_threshold = 95;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($instance = "")
    {
        $mysql          = new mysql();
        $this->instance = $instance;

        if ($mysql->verify_table(
            CMS_DB_COMMENT,
            [CMS_DB_COMMENT_INDEX            => "INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT",
             CMS_DB_COMMENT_INSTANCE         => "VARCHAR(40) NOT NULL DEFAULT ''",
             CMS_DB_COMMENT_STATUS           => "TINYINT UNSIGNED NOT NULL DEFAULT '" .
                                                sqlesc(CMS_DB_COMMENT_STATUS_INACTIVE) . "'",
             CMS_DB_COMMENT_TIME             => "INT UNSIGNED NOT NULL DEFAULT '0'",
             CMS_DB_COMMENT_NAME             => "VARCHAR(40) NOT NULL DEFAULT ''",
             CMS_DB_COMMENT_EMAIL            => "VARCHAR(40) NOT NULL DEFAULT ''",
             CMS_DB_COMMENT_URL              => "VARCHAR(40) NOT NULL DEFAULT ''",
             CMS_DB_COMMENT_TEXT             => "TEXT NOT NULL",
             CMS_DB_COMMENT_HASH             => "BINARY(16) NOT NULL DEFAULT _binary''",
             CMS_DB_COMMENT_RATING_VALUE     => "INT NOT NULL DEFAULT '0'",
             CMS_DB_COMMENT_RATING_COUNT     => "INT UNSIGNED NOT NULL DEFAULT '0'",
             CMS_DB_COMMENT_RATING_USERID    => "TEXT NOT NULL",
             CMS_DB_COMMENT_SPAM_PROBABILITY => "TINYINT UNSIGNED NOT NULL DEFAULT '0'"],
            ["INDEX (" . CMS_DB_COMMENT_HASH . ")"],
            ["md5"                           => CMS_DB_COMMENT_HASH]))
        {
            $this->operator = cms_permission(CMS_COMMENT_PERMISSION_OPERATOR . ".$instance");
            $this->writer   = cms_permission(CMS_COMMENT_PERMISSION_WRITER . ".$instance");
            $this->reader   = cms_permission(CMS_COMMENT_PERMISSION_READER . ".$instance");
            $this->enabled  = TRUE;
        };
    }

//methods
//--------------------------------------------------------------------------------------------------

    function add($name,
                 $email,
                 $url,
                 $text)
    {
        if (! $this->enabled) return FALSE;
        if (! $this->writer)  return FALSE;

        //determine spam probability
        $spam_probability = 0;
        if ($flag = cms_load("category"))
        {
            $category         = new category();
            $spam_probability = $category->evaluate_spam($text);
            if ($spam_probability > $this->spam_threshold) return -1; //spam
        };

        $time = time();
        $hash = hash32($text, TRUE);

        //check for previous comment with same content
        if (! $result = mysql_query("SELECT " .   CMS_DB_COMMENT_INDEX . " " .
                                    "FROM " .     CMS_DB_COMMENT . " " .
                                    "WHERE " .    CMS_DB_COMMENT_INSTANCE .
                                    "='" .        sqlesc($this->instance) . "' " .
                                    "AND " .      CMS_DB_COMMENT_TIME .
                                    ">='" .       sqlesc($time - 3600) . "' " . //1 hour
                                    "AND " .      CMS_DB_COMMENT_HASH .
                                    "=_binary'" . sqlesc($hash) . "' " .
                                    "LIMIT 1"))
            return FALSE;

        if (mysql_num_rows($result) > 0) return -2;

        //create comment
        if (mysql_query("INSERT " .  CMS_DB_COMMENT . " " .
                        "(" .        CMS_DB_COMMENT_INSTANCE . ", " .
                                     CMS_DB_COMMENT_STATUS . ", " .
                                     CMS_DB_COMMENT_TIME . ", " .
                                     CMS_DB_COMMENT_NAME . ", " .
                                     CMS_DB_COMMENT_EMAIL . ", " .
                                     CMS_DB_COMMENT_URL . ", " .
                                     CMS_DB_COMMENT_TEXT . ", " .
                                     CMS_DB_COMMENT_HASH . ", " .
                                     CMS_DB_COMMENT_SPAM_PROBABILITY . ") " .
                        "VALUES (" .
                        "'" .        sqlesc($this->instance) . "', " .
                        "'" .        sqlesc($this->default_status) . "', " .
                        "'" .        sqlesc($time) . "', " .
                        "'" .        sqlesc($name) . "', " .
                        "'" .        sqlesc($email) . "', " .
                        "'" .        sqlesc($url) . "', " .
                        "'" .        sqlesc($text) . "', " .
                        "_binary'" . sqlesc($hash) . "', " .
                        "'" .        sqlesc($spam_probability) . "')"))
        {
            $index = mysql_insert_id();

            //automatically rate positively if active by default
            if ($flag && ($this->default_status == CMS_DB_COMMENT_STATUS_ACTIVE))
            {
                $category->train_nospam($text);
                $category->update();
            };

            //log
            $log = new log();
            $log->access("created", $index);
            $log->user($name, $email);

            return $index;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function edit($index,
                  $name,
                  $email,
                  $url,
                  $text)
    {
        if (! $this->enabled)  return FALSE;
        if (! $this->operator) return FALSE;

        $index = sqlesc($index);

        if (

        //retrieve status and text
             $result = mysql_query("SELECT " . CMS_DB_COMMENT_STATUS . ", " .
                                               CMS_DB_COMMENT_TEXT . " " .
                                   "FROM " .   CMS_DB_COMMENT . " " .
                                   "WHERE " .  CMS_DB_COMMENT_INDEX .
                                   "='" .      $index . "' " .
                                   "LIMIT 1")
             &&
             $resultrow = mysql_fetch_assoc($result)
             &&

        //update comment
             mysql_query("UPDATE " .   CMS_DB_COMMENT . " " .
                         "SET " .      CMS_DB_COMMENT_NAME .
                         "='" .        sqlesc($name) . "', " .
                                       CMS_DB_COMMENT_EMAIL .
                         "='" .        sqlesc($email) . "', " .
                                       CMS_DB_COMMENT_URL .
                         "='" .        sqlesc($url) . "', " .
                                       CMS_DB_COMMENT_TEXT .
                         "='" .        sqlesc($text) . "', " .
                                       CMS_DB_COMMENT_HASH .
                         "=_binary'" . sqlesc(hash32($text, TRUE)) . "' " .
                         "WHERE " .    CMS_DB_COMMENT_INDEX .
                         "='" .        $index . "' " .
                         "LIMIT 1")
           )
        {
            //correct spam training
            if (
                 ($resultrow[CMS_DB_COMMENT_STATUS] == CMS_DB_COMMENT_STATUS_ACTIVE)
                 &&
                 cms_load("cms_category")
               )
            {
                $category = new category();
                $category->undo_nospam($resultrow[CMS_DB_COMMENT_TEXT]);
                $category->train_nospam($text);
                $category->update();
            };

            return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function status($index,
                    $status)
    {
        if (! $this->enabled)  return FALSE;
        if (! $this->operator) return FALSE;

        if ($status == CMS_DB_COMMENT_STATUS_INACTIVE) return FALSE; //invalid value

        //check if current status equals designated status
        if (
             ($result = mysql_query("SELECT " . CMS_DB_COMMENT_STATUS . ", " .
                                                CMS_DB_COMMENT_TEXT . " " .
                                    "FROM " .   CMS_DB_COMMENT . " " .
                                    "WHERE " .  CMS_DB_COMMENT_INDEX .
                                    "='" .      sqlesc($index) . "' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
           )
        {
            if ($resultrow[CMS_DB_COMMENT_STATUS] == $status) return TRUE; //no change

            switch ($status)
            {
            case CMS_DB_COMMENT_STATUS_HIDDEN:

                if ($resultrow[CMS_DB_COMMENT_STATUS] != CMS_DB_COMMENT_STATUS_ACTIVE) break;

                //undo previous training
                if (! cms_load("category")) break;
                $category = new category();
                $category->undo_nospam($resultrow[CMS_DB_COMMENT_TEXT]);
                $category->update();
                break;

            case CMS_DB_COMMENT_STATUS_ACTIVE:

                //train as good
                if (! cms_load("category")) break;
                $category = new category();
                $category->train_nospam($resultrow[CMS_DB_COMMENT_TEXT]);
                $category->update();
                break;
            };

            if (mysql_query("UPDATE " . CMS_DB_COMMENT . " " .
                            "SET " .    CMS_DB_COMMENT_STATUS .
                            "='" .      sqlesc($status) . "' " .
                            "WHERE " .  CMS_DB_COMMENT_INDEX  .
                            "='" .      sqlesc($index) . "' " .
                            "LIMIT 1"))
                return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function rate_good($index,
                       $invert = FALSE)
    {
        if (! $this->enabled) return FALSE;
        if (! $this->reader)  return FALSE;

        $userid = sqlesc("/" . CMS_IPHASH . "/");
        return mysql_query("UPDATE " .     CMS_DB_COMMENT . " " .
                           "SET " .        CMS_DB_COMMENT_RATING_VALUE .
                           "=" .           CMS_DB_COMMENT_RATING_VALUE .
                                           ($invert ? "-" : "+") . "1, " .
                                           CMS_DB_COMMENT_RATING_COUNT .
                           "=" .           CMS_DB_COMMENT_RATING_COUNT . "+1, " .
                                           CMS_DB_COMMENT_RATING_USERID .
                           "=CONCAT('" .   $userid . "', " . CMS_DB_COMMENT_RATING_USERID . "), " .
                           "WHERE " .      CMS_DB_COMMENT_INDEX  .
                           "='" .          sqlesc($index) . "' " .
                           "AND " .        CMS_DB_COMMENT_RATING_USERID . " " .
                           "NOT LIKE '%" . $userid . "%' " .
                           "LIMIT 1") ?
               TRUE : FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function rate_bad($index)
    {
        return $this->rate_good($index, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function delete($index,
                    $spam = FALSE)
    {
        if (! $this->enabled)  return FALSE;
        if (! $this->operator) return FALSE;

        //train spam filter
        if (
             $spam
             &&
             cms_load("category")
             &&
             ($result = mysql_query("SELECT " . CMS_DB_COMMENT_STATUS . ", " .
                                                CMS_DB_COMMENT_TEXT . " " .
                                    "FROM " .   CMS_DB_COMMENT . " " .
                                    "WHERE " .  CMS_DB_COMMENT_INDEX .
                                    "='" .      sqlesc($index) . "' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
           )
        {
            $category = new category();

            //undo previous spam filter training
            if ($resultrow[CMS_DB_COMMENT_STATUS] == CMS_DB_COMMENT_STATUS_ACTIVE)
                $category->undo_nospam($resultrow[CMS_DB_COMMENT_TEXT]);

            $category->train_spam($resultrow[CMS_DB_COMMENT_TEXT]);
            $category->update();
        };

        return mysql_query("DELETE FROM " . CMS_DB_COMMENT . " " .
                           "WHERE " .       CMS_DB_COMMENT_INDEX .
                           "='" .           sqlesc($index) . "' " .
                           "LIMIT 1") ?
               TRUE : FALSE;
    }
};