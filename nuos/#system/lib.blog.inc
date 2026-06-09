<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   BLOG CLASS
//==================================================================================================

//permission
define("CMS_BLOG_PERMISSION_OPERATOR",      "operator");
define("CMS_BLOG_PERMISSION_WRITER",        "writer");
define("CMS_BLOG_PERMISSION_READER",        "reader");

//status
define("CMS_DB_BLOG_STATUS_INACTIVE",       0);
define("CMS_DB_BLOG_STATUS_ACTIVE",         1);

//sticky
define("CMS_DB_BLOG_STICKY_OFF",            0);
define("CMS_DB_BLOG_STICKY_ON",             1);

//code position
define("CMS_DB_BLOG_CODE_POSITION_CONTROL", 0);
define("CMS_DB_BLOG_CODE_POSITION_TEASER",  1);
define("CMS_DB_BLOG_CODE_POSITION_BEFORE",  2);
define("CMS_DB_BLOG_CODE_POSITION_AFTER",   3);

//database
define("CMS_DB_BLOG",                       CMS_DB_PREFIX . "blog");
define("CMS_DB_BLOG_INDEX",                 "id");
define("CMS_DB_BLOG_INSTANCE",              "instance");
define("CMS_DB_BLOG_STATUS",                "status");
define("CMS_DB_BLOG_TIME",                  "time");
define("CMS_DB_BLOG_STICKY",                "sticky");
define("CMS_DB_BLOG_OWNER",                 "owner");
define("CMS_DB_BLOG_TITLE",                 "title");
define("CMS_DB_BLOG_META",                  "meta");
define("CMS_DB_BLOG_TEXT",                  "text");

define("CMS_DB_BLOG_META_TERM",             CMS_DB_PREFIX . "blog_meta_term");
define("CMS_DB_BLOG_META_TERM_INDEX",       "id");
define("CMS_DB_BLOG_META_TERM_TEXT",        "text");
define("CMS_DB_BLOG_META_TERM_LANGUAGE",    "language");

define("CMS_DB_BLOG_META_LINK",             CMS_DB_PREFIX . "blog_meta_link");
define("CMS_DB_BLOG_META_LINK_ARTICLE",     "article");
define("CMS_DB_BLOG_META_LINK_TERM",        "term");

define("CMS_DB_BLOG_CODE",                  CMS_DB_PREFIX . "blog_code");
define("CMS_DB_BLOG_CODE_INSTANCE",         "instance");
define("CMS_DB_BLOG_CODE_POSITION",         "position");
define("CMS_DB_BLOG_CODE_TEXT",             "text");

class blog
{

//properties
//--------------------------------------------------------------------------------------------------

    public $instance = "";
    public $enabled  = FALSE;
    public $operator = FALSE;
    public $writer   = FALSE;
    public $reader   = FALSE;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($instance = "")
    {
        $mysql          = new mysql();
        $this->instance = $instance;

        if (
             $mysql->verify_table(CMS_DB_BLOG,
                                  [CMS_DB_BLOG_INDEX              => "INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT",
                                   CMS_DB_BLOG_INSTANCE           => "VARCHAR(40) NOT NULL DEFAULT ''",
                                   CMS_DB_BLOG_STATUS             => "CHAR(1) NOT NULL DEFAULT '" .
                                                                     sqlesc(CMS_DB_BLOG_STATUS_INACTIVE) . "'",
                                   CMS_DB_BLOG_TIME               => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_BLOG_STICKY             => "CHAR(1) NOT NULL DEFAULT '" .
                                                                     sqlesc(CMS_DB_BLOG_STICKY_OFF) . "'",
                                   CMS_DB_BLOG_OWNER              => "VARCHAR(40) NOT NULL DEFAULT ''",
                                   CMS_DB_BLOG_TITLE              => "TEXT NOT NULL",
                                   CMS_DB_BLOG_META               => "TEXT NOT NULL",
                                   CMS_DB_BLOG_TEXT               => "TEXT NOT NULL"])
             &&
             $mysql->verify_table(CMS_DB_BLOG_META_TERM,
                                  [CMS_DB_BLOG_META_TERM_INDEX    => "INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT",
                                   CMS_DB_BLOG_META_TERM_TEXT     => "VARCHAR(40) NOT NULL DEFAULT '' UNIQUE",
                                   CMS_DB_BLOG_META_TERM_LANGUAGE => "VARCHAR(40) NOT NULL DEFAULT ''"])
             &&
             $mysql->verify_table(CMS_DB_BLOG_META_LINK,
                                  [CMS_DB_BLOG_META_LINK_ARTICLE  => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_BLOG_META_LINK_TERM     => "INT UNSIGNED NOT NULL DEFAULT '0'"],
                                  ["UNIQUE (" . CMS_DB_BLOG_META_LINK_ARTICLE . ", " .
                                                CMS_DB_BLOG_META_LINK_TERM . ")"])
             &&
             $mysql->verify_table(CMS_DB_BLOG_CODE,
                                  [CMS_DB_BLOG_CODE_INSTANCE      => "VARCHAR(40) NOT NULL DEFAULT ''",
                                   CMS_DB_BLOG_CODE_POSITION      => "CHAR(1) NOT NULL DEFAULT '" .
                                                                     sqlesc(CMS_DB_BLOG_CODE_POSITION_CONTROL) . "'",
                                   CMS_DB_BLOG_CODE_TEXT          => "TEXT NOT NULL"],
                                  ["UNIQUE (" . CMS_DB_BLOG_CODE_INSTANCE . ", " .
                                                CMS_DB_BLOG_CODE_POSITION . ")"])
           )
        {
            $this->operator = cms_permission(CMS_BLOG_PERMISSION_OPERATOR . ".$instance");
            $this->writer   = cms_permission(CMS_BLOG_PERMISSION_WRITER . ".$instance");
            $this->reader   = cms_permission(CMS_BLOG_PERMISSION_READER . ".$instance");
            $this->enabled  = TRUE;
        };
    }

//methods
//--------------------------------------------------------------------------------------------------

    function add($title,
                 $meta,
                 $text,
                 $status = CMS_DB_BLOG_STATUS_ACTIVE,
                 $time   = NULL,
                 $sticky = CMS_DB_BLOG_STICKY_OFF,
                 $test   = FALSE)
    {
        if (! $this->enabled) return FALSE;
        if (! $this->writer)  return FALSE;

        if ($test) return TRUE; //permission test

        if ($time === NULL) $time = time();

        //create article
        if (mysql_query("INSERT " . CMS_DB_BLOG . " " .
                        "(" .       CMS_DB_BLOG_INSTANCE . ", " .
                                    CMS_DB_BLOG_STATUS . ", " .
                                    CMS_DB_BLOG_TIME . ", " .
                                    CMS_DB_BLOG_STICKY . ", " .
                                    CMS_DB_BLOG_OWNER . ", " .
                                    CMS_DB_BLOG_TITLE . ", " .
                                    CMS_DB_BLOG_META . ", " .
                                    CMS_DB_BLOG_TEXT . ") " .
                        "VALUES (" .
                        "'" .       sqlesc($this->instance) . "', " .
                        "'" .       sqlesc($status) . "', " .
                        "'" .       sqlesc($time) . "', " .
                        "'" .       sqlesc($sticky) . "', " .
                        "'" .       sqlesc(CMS_USER) . "', " .
                        "'" .       sqlesc($title) . "', " .
                        "'" .       sqlesc($meta) . "', " .
                        "'" .       sqlesc($text) . "')"))
        {
            $index = mysql_insert_id();
            $this->meta_link($index, $meta); //link with meta data
            return $index;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_add()
    {
        return $this->add(NULL, NULL, NULL, NULL, NULL, NULL, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function edit($index,
                  $title,
                  $meta,
                  $text,
                  $status = NULL,
                  $time   = NULL,
                  $sticky = NULL,
                  $test   = FALSE)
    {
        if (! $this->enabled) return FALSE;
        if (! $this->writer)  return FALSE;

        if (! $this->operator)
        {
            //verify ownership
            $result = mysql_query("SELECT " . CMS_DB_BLOG_OWNER . " " .
                                  "FROM " .   CMS_DB_BLOG . " " .
                                  "WHERE " .  CMS_DB_BLOG_INDEX .
                                  "='" .      sqlesc($index) . "' " .
                                  "LIMIT 1");
            if ($result === FALSE)                          return FALSE; //error
            if (nstreq(mysql_result($result, 0), CMS_USER)) return FALSE; //not owner
        };

        if ($test) return TRUE; //permission test

        //edit article
        if (mysql_query("UPDATE " . CMS_DB_BLOG . " " .
                        "SET " .    (($status === NULL) ? "" :
                                    CMS_DB_BLOG_STATUS . "='" . sqlesc($status) . "', ") .
                                    (($time === NULL)   ? "" :
                                    CMS_DB_BLOG_TIME .   "='" . sqlesc($time) . "', ") .
                                    (($sticky === NULL) ? "" :
                                    CMS_DB_BLOG_STICKY . "='" . sqlesc($sticky) . "', ") .
                                    CMS_DB_BLOG_TITLE .  "='" . sqlesc($title) . "', " .
                                    CMS_DB_BLOG_META .   "='" . sqlesc($meta) . "', " .
                                    CMS_DB_BLOG_TEXT .   "='" . sqlesc($text) . "' " .
                        "WHERE " .  CMS_DB_BLOG_INDEX .  "='" . sqlesc($index) . "' " .
                        "LIMIT 1") !== FALSE)
        {
            $this->meta_link($index, $meta); //(re)link with meta data
            return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_edit($index)
    {
        return $this->edit($index, NULL, NULL, NULL, NULL, NULL, NULL, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function delete($index,
                    $test = FALSE)
    {
        if (! $this->enabled) return FALSE;
        if (! $this->writer)  return FALSE;

        $index = sqlesc($index);

        if (! $this->operator)
        {
            //verify ownership
            $result = mysql_query("SELECT " . CMS_DB_BLOG_OWNER . " " .
                                  "FROM " .   CMS_DB_BLOG . " " .
                                  "WHERE " .  CMS_DB_BLOG_INDEX .
                                  "='" .      $index . "' " .
                                  "LIMIT 1");
            if ($result === FALSE)                          return FALSE; //error
            if (nstreq(mysql_result($result, 0), CMS_USER)) return FALSE; //not owner
        };

        if ($test) return TRUE; //permission test

        //delete article
        if (mysql_query("DELETE FROM " . CMS_DB_BLOG . " " .
                        "WHERE " .       CMS_DB_BLOG_INDEX .
                        "='" .           $index . "' " .
                        "LIMIT 1") !== FALSE)
        {
            $this->meta_clean(); //remove orphaned meta data
            return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_delete($index)
    {
        return $this->delete($index, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function meta_link($index,
                       $meta)
    {
        if (! $this->enabled) return FALSE;
        if (! $this->writer)  return FALSE;

        $index = sqlesc($index);

        //clean up
        mysql_query("DELETE FROM " . CMS_DB_BLOG_META_LINK . " " .
                    "WHERE " .       CMS_DB_BLOG_META_LINK_ARTICLE .
                    "='" .           $index . "'");

        //normalize
        $meta = explode(",", $meta);
        $meta = array_map(__NAMESPACE__ . "\\stripspaces", $meta);
        $meta = array_filter($meta, __NAMESPACE__ . "\\nstre");
        $meta = array_map(__NAMESPACE__ . "\\utf8_strtolower", $meta);
        $meta = array_unique($meta);
        $meta = array_map(__NAMESPACE__ . "\\sqlesc", $meta);

        if (count($meta) === 0) return TRUE; //no terms

        //insert terms into list
        if (mysql_query("INSERT IGNORE " . CMS_DB_BLOG_META_TERM . " " .
                        "(" .              CMS_DB_BLOG_META_TERM_TEXT . ") " .
                        "VALUES ('" .      implode("'), ('", $meta) . "')") === FALSE)
            return FALSE; //error

        //retrieve indexes for terms
        $result = mysql_query("SELECT " . CMS_DB_BLOG_META_TERM_INDEX . " " .
                              "FROM " .   CMS_DB_BLOG_META_TERM . " " .
                              "WHERE " .  CMS_DB_BLOG_META_TERM_TEXT . " " .
                              "IN ('" .   implode("', '", $meta) . "')");
        if ($result === FALSE) return FALSE; //error

        $list = [];
        while ($resultrow = mysql_fetch_row($result))
            $list[] = $resultrow[0];

        if (count($list) === 0) return TRUE; //no terms

        //create linkage between article and meta terms
        if (mysql_query("INSERT IGNORE " . CMS_DB_BLOG_META_LINK . " " .
                        "(" .              CMS_DB_BLOG_META_LINK_ARTICLE . ", " .
                                           CMS_DB_BLOG_META_LINK_TERM . ") " .
                        "VALUES ('" .      $index . "', '" . implode("'), ('$index', '", $list) . "')") === FALSE)
            return FALSE; //error

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function meta_clean()
    {
        if (! $this->enabled) return FALSE;
        if (! $this->writer)  return FALSE;

        //clean up meta links
        mysql_query("DELETE " .    "a " .
                    "FROM " .             CMS_DB_BLOG_META_LINK . " a " .
                    "LEFT JOIN " .        CMS_DB_BLOG .           " b " .
                    "ON " .        "a." . CMS_DB_BLOG_META_LINK_ARTICLE .
                    "=" .          "b." . CMS_DB_BLOG_INDEX . " " .
                    "WHERE " .     "b." . CMS_DB_BLOG_INDEX . " IS NULL");

        //clean up meta terms
        mysql_query("DELETE " .    "a " .
                    "FROM " .             CMS_DB_BLOG_META_TERM . " a " .
                    "LEFT JOIN " .        CMS_DB_BLOG_META_LINK . " b " .
                    "ON " .        "a." . CMS_DB_BLOG_META_TERM_INDEX .
                    "=" .          "b." . CMS_DB_BLOG_META_LINK_TERM . " " .
                    "WHERE " .     "b." . CMS_DB_BLOG_META_LINK_TERM . " IS NULL");

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function code_set($position,
                      $text,
                      $test = FALSE)
    {
        if (! $this->enabled)  return FALSE;
        if (! $this->operator) return FALSE;

        if ($test) return TRUE; //permission test

        //retrieve code
        return (bool)mysql_query("REPLACE " . CMS_DB_BLOG_CODE . " " .
                                 "(" .        CMS_DB_BLOG_CODE_INSTANCE . ", " .
                                              CMS_DB_BLOG_CODE_POSITION . ", " .
                                              CMS_DB_BLOG_CODE_TEXT . ") " .
                                 "VALUES (" .
                                 "'" .        sqlesc($this->instance) . "', " .
                                 "'" .        sqlesc($position) . "', " .
                                 "'" .        sqlesc($text) . "')");
    }

//--------------------------------------------------------------------------------------------------

    function test_code_set()
    {
        return $this->code_set(NULL, NULL, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function code_get($position)
    {
        if (! $this->enabled) return FALSE;

        //retrieve code
        $result = mysql_query("SELECT " . CMS_DB_BLOG_CODE_TEXT . " " .
                              "FROM " .   CMS_DB_BLOG_CODE . " " .
                              "WHERE " .  CMS_DB_BLOG_CODE_INSTANCE .
                              "='" .      sqlesc($this->instance) . "' " .
                              "AND " .    CMS_DB_BLOG_CODE_POSITION .
                              "='" .      sqlesc($position) . "' " .
                              "LIMIT 1");
        if ($result === FALSE) return FALSE; //error

        return mysql_result($result, 0);
    }

//--------------------------------------------------------------------------------------------------

    function code_parse($position,
                        $replacement = NULL)
    {
        if (! $this->enabled) return FALSE;

        $code = l($this->code_get($position));
        if (stre($code)) return ""; //no code

        //replace placeholder
        if (is_array($replacement))
        {
            foreach ($replacement AS $key => $value)
            {
                $key  = preg_quote($key, "/");
                $code = $value ?
                    preg_replace("/(\\[([^\\[\\]]*))?%$key%(([^\\[\\]]*)\\])?/",
                                 "\${2}" . x($value) . "\\4",
                                 $code) :
                    preg_replace("/(?:\\[[^\\[\\]]*)?%$key%(?:[^\\[\\]]*\\])?/",
                                 "",
                                 $code);
            };
        };

        return $code;
    }
};