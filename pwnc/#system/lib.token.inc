<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   TOKEN CLASS
//==================================================================================================

//permission
define("CMS_TOKEN_PERMISSION_OPERATOR", "operator");

//database
define("CMS_DB_TOKEN",                  CMS_DB_PREFIX . "token");
define("CMS_DB_TOKEN_INDEX",            "id");
define("CMS_DB_TOKEN_VALUE",            "value");
define("CMS_DB_TOKEN_CATEGORY",         "category");
define("CMS_DB_TOKEN_TITLE",            "title");
define("CMS_DB_TOKEN_TEXT",             "text");

//related functions
//--------------------------------------------------------------------------------------------------

function token_get_index(&$token,
                         $category)
{
    if (
         $token->enabled
         &&
         ($result = mysql_query("SELECT " .   CMS_DB_TOKEN_INDEX . " " .
                                "FROM " .     CMS_DB_TOKEN . " " .
                                "WHERE " .    CMS_DB_TOKEN_CATEGORY .
                                "='" .        sqlesc($category) . "' " .
                                "ORDER BY " . CMS_DB_TOKEN_INDEX . " " .
                                "LIMIT 1"))
         &&
         mysql_num_rows($result)
       )
        return mysql_result($result, 0, CMS_DB_TOKEN_INDEX);

    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function token_get_category(&$token,
                            $index)
{
    if (
         nstre($index)
         &&
         $token->enabled
         &&
         ($result = mysql_query("SELECT " . CMS_DB_TOKEN_CATEGORY . " " .
                                "FROM " .   CMS_DB_TOKEN . " " .
                                "WHERE " .  CMS_DB_TOKEN_INDEX .
                                "='" .      sqlesc($index) . "' " .
                                "LIMIT 1"))
         &&
         mysql_num_rows($result)
       )
        return mysql_result($result, 0, CMS_DB_TOKEN_CATEGORY);

    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function token_get_select(&$token)
{
    if (
         $token->enabled
         &&
         ($result = mysql_query("SELECT DISTINCT " . CMS_DB_TOKEN_CATEGORY . " " .
                                "FROM " .            CMS_DB_TOKEN . " " .
                                "WHERE " .           CMS_DB_TOKEN_CATEGORY . "!='' " .
                                "ORDER BY " .        CMS_DB_TOKEN_CATEGORY))
       )
    {
        $array = ["" => ""];

        while ($resultrow = mysql_fetch_assoc($result))
        {
            $category                          = $resultrow[CMS_DB_TOKEN_CATEGORY];
            $array[$category ? $category : ""] = $category;
        };

        return $array;
    };

    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function token_override($index,
                        $text = NULL)
{
    if ($text === NULL) unset(token::$override[$index]);
    else                token::$override[$index] = $text;
};

class token
{

//properties
//--------------------------------------------------------------------------------------------------

    public $operator        = NULL;
    public $enabled         = NULL;
    public static $override = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct()
    {
        $mysql = new mysql();

        if ($mysql->verify_table(CMS_DB_TOKEN,
                                 [CMS_DB_TOKEN_INDEX    => "VARCHAR(255) NOT NULL DEFAULT '' PRIMARY KEY",
                                  CMS_DB_TOKEN_VALUE    => "VARCHAR(255) NOT NULL DEFAULT ''",
                                  CMS_DB_TOKEN_CATEGORY => "VARCHAR(40) NOT NULL DEFAULT ''",
                                  CMS_DB_TOKEN_TITLE    => "TEXT NOT NULL",
                                  CMS_DB_TOKEN_TEXT     => "TEXT NOT NULL"]))
        {
            $this->operator = cms_permission(CMS_TOKEN_PERMISSION_OPERATOR);
            $this->enabled  = TRUE;
        };
    }

//methods
//--------------------------------------------------------------------------------------------------

    function add($index,
                 $value,
                 $category,
                 $title,
                 $text)
    {
        if (! $this->enabled)                  return FALSE;
        if (! $this->operator)                 return FALSE;
        if (stre($index))                      $index = language_get($title, FALSE);
        if (stre($index))                      return FALSE;
        if (stre(language_get($title, FALSE))) $title = language_set($title, $index, FALSE);
        if (stre($title))                      $title = $index;

        $index = preg_replace(["/[ \r\n]+/", "/_{2,}/"], "_", $index);
        $value = preg_replace(["/[^0-9a-zA-Z_,]+/", "/_{2,}/", "/,{2,}/", "/^,/", "/,$/"],
                              ["_",                 "_",       ","      , "",     ""],
                              $value);

        if (mysql_query("INSERT " . CMS_DB_TOKEN .
                             " (" . CMS_DB_TOKEN_INDEX . ", " .
                                    CMS_DB_TOKEN_VALUE . ", " .
                                    CMS_DB_TOKEN_CATEGORY . ", " .
                                    CMS_DB_TOKEN_TITLE . ", " .
                                    CMS_DB_TOKEN_TEXT . ") " .
                        "VALUES (" .
                              "'" . sqlesc($index) . "', " .
                              "'" . sqlesc($value) . "', " .
                              "'" . sqlesc($category) . "', " .
                              "'" . sqlesc($title) . "', " .
                              "'" . sqlesc($text) . "')"))
        {
            $this->cache_del($index);
            return $index;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function get($index)
    {
        if (
             $this->enabled
             &&
             ($result = mysql_query("SELECT " . CMS_DB_TOKEN_INDEX . ", " .
                                                CMS_DB_TOKEN_VALUE . ", " .
                                                CMS_DB_TOKEN_CATEGORY . ", " .
                                                CMS_DB_TOKEN_TITLE . ", " .
                                                CMS_DB_TOKEN_TEXT . " " .
                                    "FROM " .   CMS_DB_TOKEN . " " .
                                    "WHERE " .  CMS_DB_TOKEN_INDEX .
                                    "='" .      sqlesc($index) . "' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
           )
            return $resultrow;

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function update($index,
                    $_index,
                    $value,
                    $category,
                    $title,
                    $text)
    {
        if (! $this->enabled)                  return FALSE;
        if (! $this->operator)                 return FALSE;
        if (stre($index))                      return FALSE;
        if (stre($_index))                     $_index = language_get($title, FALSE);
        if (stre($_index))                     $_index = $index;
        if (stre(language_get($title, FALSE))) $title  = language_set($title, $index, FALSE);

        $index = preg_replace(["/[ \r\n]+/", "/_{2,}/"], "_", $index);
        $value = preg_replace(["/[^0-9a-zA-Z_,]+/", "/_{2,}/", "/,{2,}/", "/^,/", "/,$/"],
                              ["_",                 "_",       ","      , "",     ""],
                              $value);

        if (mysql_query("UPDATE " . CMS_DB_TOKEN . " " .
                        "SET " .    CMS_DB_TOKEN_INDEX .    "='" . sqlesc($_index) . "', " .
                                    CMS_DB_TOKEN_VALUE .    "='" . sqlesc($value) . "', " .
                                    CMS_DB_TOKEN_CATEGORY . "='" . sqlesc($category) . "', " .
                                    CMS_DB_TOKEN_TITLE .    "='" . sqlesc($title) . "', " .
                                    CMS_DB_TOKEN_TEXT .     "='" . sqlesc($text) . "' " .
                        "WHERE " .  CMS_DB_TOKEN_INDEX .    "='" . sqlesc($index) . "' " .
                        "LIMIT 1"))
        {
            $this->cache_del($index);
            $this->cache_del($_index);
            return $_index;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function delete($index)
    {
        if (! $this->enabled)  return FALSE;
        if (! $this->operator) return FALSE;

        if (mysql_query("DELETE FROM " . CMS_DB_TOKEN . " " .
                        "WHERE " .       CMS_DB_TOKEN_INDEX .
                        "='" .           sqlesc($index) . "' " .
                        "LIMIT 1"))
        {
            $this->cache_del($index);
            return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function apply($text)
    {
        if (! $this->enabled) return FALSE;

        $list   = [];
        $pos    = [];
        $offset = 0;
        $length = strlen($text);

        //seek start tag
        while (
                ($offset < $length)
                &&
                (($offset = strpos($text, "%%", $offset)) !== FALSE)
              )
        {
            //scan for escape character
            if (($offset >= 1) && ($text[$offset - 1] === "\\"))
            {
                $text = substr_replace($text, "", $offset - 1, 1);
                $length--;
                continue;
            };

            $offset += 2; //proceed after delimiter

            //seek end tag
            if (($_offset = strpos($text, "%%", $offset)) === FALSE) break;

            //extract token index
            $string = substr($text, $offset, $_offset - $offset);
            $i      = strcspn($string, " \r\n");
            $index  = substr($string, 0, $i);

            //store index and occurrence data
            if ($index !== "")
            {
                $list[$index] = TRUE;
                $pos[]        = [$index, $offset, $i, $_offset];
            };

            $offset = $_offset + 2; //proceed after delimiter
        };

        if (! $list) return $text; //no token found

//..................................................................................................

        //retrieve from cache
        $_list = [];
        foreach (array_keys($list) AS $index)
        {
            $cache = $this->cache_get($index);

            //not cached
            if ($cache === FALSE) { $_list[$index] = TRUE; continue; };

            //token does not exist
            if ($cache === [NULL, NULL]) continue;

            $_text        = isset(self::$override[$index]) ?
                            self::$override[$index] :
                            $cache[1];
            $list[$index] = [0 => explode(",", $cache[0]),
                             1 => l($_text)];
        };

        //retrieve from database
        if (
             $_list
             &&
             ($result = mysql_query("SELECT " . CMS_DB_TOKEN_INDEX . ", " .
                                                CMS_DB_TOKEN_VALUE . ", " .
                                                CMS_DB_TOKEN_TEXT . " " .
                                    "FROM " .   CMS_DB_TOKEN . " " .
                                    "WHERE " .  CMS_DB_TOKEN_INDEX . " " .
                                    "IN ('" .   implode("', '", sqlesc(array_keys($_list))) . "')"))
           )
        {
            while ($resultrow = mysql_fetch_row($result))
            {
                $index        = $resultrow[0];
                $value        = $resultrow[1];
                $_text        = isset(self::$override[$index]) ?
                                self::$override[$index] :
                                $resultrow[2];
                $list[$index] = [0 => explode(",", $value),
                                 1 => l($_text)];

                //cache
                $this->cache_set($index, $value, $resultrow[2]);
            };
        };

//..................................................................................................

        $buffer = "";
        $offset = 0;

        foreach ($pos AS $value)
        {
            $index = $value[0];
            $start = $value[1] - 2;
            $end   = $value[3] + 2;

            //text
            $buffer .= substr($text, $offset, $start - $offset);

            //token not found
            if ($list[$index] === TRUE)
            {

                $buffer .= substr($text, $start, $end - $start);

                //cache
                if (isset($_list[$index])) $this->cache_set($index, NULL, NULL);
            }

            //replace token
            else
            {
                //get parameter values
                $offset = $value[1] + $value[2];
                $length = $value[3] - $offset;
                $string = trim(substr($text, $offset, $length), " \r\n");
                $array  = preg_split("/[ \r\n]*(?<!\\\),[ \r\n]*/", $string);

                //assign parameter name -> value
                $replacement = [];
                foreach ($list[$index][0] AS $key => $_value)
                    $replacement[$_value] =
                        isset($array[$key]) ? str_replace("\\,", ",", $array[$key]) : "";

                //apply parameter values
                $buffer .= replace_placeholder($list[$index][1], $replacement);
            };

            $offset = $end;
        };

        return $buffer . substr($text, $offset);
    }

//--------------------------------------------------------------------------------------------------

    function cache_set($index,
                       $value,
                       $text)
    {
        $code = hash32($index);
        $path = CMS_DATA_PATH . "#token/cache/" . substr($code, 0, 4) . "/" . substr($code, 4);

        return write_file($path, serialize([$value, $text]));
    }

//--------------------------------------------------------------------------------------------------

    function cache_get($index)
    {
        $code = hash32($index);
        $path = CMS_DATA_PATH . "#token/cache/" . substr($code, 0, 4) . "/" . substr($code, 4);
        return unserialize(read_file($path));
    }

//--------------------------------------------------------------------------------------------------

    function cache_del($index)
    {
        $code = hash32($index);
        $path = CMS_DATA_PATH . "#token/cache/" . substr($code, 0, 4) . "/" . substr($code, 4);
        if (is_file($path)) return unlink($path);
    }

//--------------------------------------------------------------------------------------------------

    function cache_clean()
    {
        if (! cms_load("filemanager")) return FALSE;
        return filemanager_delete(CMS_DATA_PATH . "#token/cache/");
    }
};