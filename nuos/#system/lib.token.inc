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
                                "WHERE " .    CMS_DB_TOKEN_CATEGORY . "='" . sqlesc($category) . "' " .
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
                                "WHERE " .  CMS_DB_TOKEN_INDEX . "='" . sqlesc($index) . "' " .
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
            return $index;

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
                                    "WHERE " .  CMS_DB_TOKEN_INDEX . "='" . sqlesc($index) . "' " .
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
            return $_index;

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function delete($index)
    {
        if (! $this->enabled)  return FALSE;
        if (! $this->operator) return FALSE;

        return mysql_query("DELETE FROM " . CMS_DB_TOKEN . " " .
                           "WHERE " .       CMS_DB_TOKEN_INDEX . "='" . sqlesc($index) . "' " .
                           "LIMIT 1");
    }

//--------------------------------------------------------------------------------------------------

    function apply($text)
    {
        if (! $this->enabled) return FALSE;

        $array  = [];
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
            if (($offset > 0) && ($text[$offset - 1] === "\\"))
            {
                $offset++;
                continue;
            };

            $offset += 2; //adjust text position

            //seek end tag
            if ($offset === $length)                                 break;
            if (($_offset = strpos($text, "%%", $offset)) === FALSE) break;

            //extract token index
            $string = substr($text, $offset, $_offset - $offset);
            $i      = strcspn($string, " \r\n");
            $string = substr($string, 0, $i);

            if (! isset($array[$string])) $array[$string] = NULL;

            $offset = $_offset + 2; //adjust text position
        };

        //retrieve token values from database
        $_array = array_keys($array);

        if ($result = mysql_query("SELECT " . CMS_DB_TOKEN_INDEX . ", " .
                                              CMS_DB_TOKEN_VALUE . ", " .
                                              CMS_DB_TOKEN_TEXT . " " .
                                  "FROM " .   CMS_DB_TOKEN . " " .
                                  "WHERE " .  CMS_DB_TOKEN_INDEX . " " .
                                  "IN ('" .   implode("', '", sqlesc($_array)) . "')"))
        {
            while ($resultrow = mysql_fetch_row($result))
            {
                $index         = $resultrow[0];
                $value         = $resultrow[1];
                $_text         = isset(self::$override[$index]) ?
                                 self::$override[$index] :
                                 $resultrow[2];
                $array[$index] = [0 => explode(",", $value),
                                  1 => l($_text)];
            };
        };

        $offset = 0;

        //seek start tag
        while (
                ($offset < $length)
                &&
                (($offset = strpos($text, "%%", $offset)) !== FALSE)
              )
        {
            //scan for escape character
            if ($offset > 0)
            {
                $_offset = $offset - 1;
                if ($text[$_offset] === "\\")
                {
                    //remove escape character
                    $text = substr_replace($text, "", $_offset, 1);
                    $length--; //adjust text length
                    continue;
                };
            };

            $_offset = $offset + 2;

            //seek end tag
            if ($_offset === $length)                                  break;
            if (($__offset = strpos($text, "%%", $_offset)) === FALSE) break;

            $_length = $__offset - $_offset;
            $string  = substr($text, $_offset, $_length);

            //extract token index
            $i       = strcspn($string, " \r\n");
            $index   = substr($string, 0, $i);

            if ($array[$index] !== NULL) //token is valid
            {
                $string  = trim(substr($string, $i), " \r\n"); //user values
                $_array  = preg_split("/[ \r\n]*(?<!\\\),[ \r\n]*/", $string);
                $_text   = $array[$index][1];                  //replacement text

                //go through value list
                foreach ($array[$index][0] AS $key => $value)
                {
                    //get user value
                    $_value = isset($_array[$key]) ?
                              str_replace("\\,", ",", $_array[$key]) : "";

                    //replace placeholder with user value
                    $_text  = stre($_value) ?
                              preg_replace("/(?:\\[[^\\[\\]]*)?%$value%(?:[^\\[\\]]*\\])?/",
                                           "",
                                           $_text) :
                              preg_replace("/(\\[([^\\[\\]]*))?%$value%(([^\\[\\]]*)\\])?/",
                                           "\${2}" . x($_value) . "\\4",
                                           $_text);
                };

                //replace token in source text
                $text    = substr_replace($text, $_text, $offset, $_length + 4);

                $offset += strlen($_text); //adjust text position
                $length  = strlen($text);  //adjust text length
                continue;
            };

            $offset = $__offset + 2; //adjust text position
        };

        return $text;
    }
};