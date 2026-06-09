<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   CATEGORY CLASS
//==================================================================================================

define("CMS_CATEGORY_CLASS_SPAM",          "#spam");
define("CMS_CATEGORY_LEARNING_THRESHOLD",  25);
define("CMS_CATEGORY_TOKEN_LIMIT",         15);
define("CMS_CATEGORY_DEVIATION_THRESHOLD", 10);
define("CMS_CATEGORY_TRAINING_THRESHOLD",  10);
define("CMS_CATEGORY_PROBABILITY_DEFAULT", 40);

//database
define("CMS_DB_CATEGORY_META",             CMS_DB_PREFIX . "category_meta");
define("CMS_DB_CATEGORY_META_INDEX",       "id");
define("CMS_DB_CATEGORY_META_CLASS",       "class");
define("CMS_DB_CATEGORY_META_COUNT_YES",   "count_yes");
define("CMS_DB_CATEGORY_META_COUNT_NO",    "count_no");

define("CMS_DB_CATEGORY",                  CMS_DB_PREFIX . "category");
define("CMS_DB_CATEGORY_INDEX",            "id");
define("CMS_DB_CATEGORY_TOKEN",            "token");
define("CMS_DB_CATEGORY_CLASS",            "class");
define("CMS_DB_CATEGORY_COUNT_YES",        "count_yes");
define("CMS_DB_CATEGORY_COUNT_NO",         "count_no");
define("CMS_DB_CATEGORY_PROBABILITY_YES",  "probability_yes");
define("CMS_DB_CATEGORY_PROBABILITY_NO",   "probability_no");
define("CMS_DB_CATEGORY_RATING",           "rating");

//related functions
//--------------------------------------------------------------------------------------------------

function category_tokenize_text($text)
{
    $text   = tokenize_text($text, TRUE);
    $text   = implode("", $text) . "\0\0\0";
    $result = [];
    $char1  = $char2 = $char3 = "";

    $c = strlen($text);

    for ($i = 0; $i < $c; )
    {
        $char4 = $text[$i++];
        $ord   = ord($char4);

        if ($ord === 0) break;

        if (($ord >= 128) && ($ord < 248))
        {
            if ($ord < 224)     $char4 .= $text[$i++];
            elseif ($ord < 240) $char4 .= $text[$i++] . $text[$i++];
            elseif ($ord < 248) $char4 .= $text[$i++] . $text[$i++] . $text[$i++];
        };

        $char1 = $char2;
        $char2 = $char3;
        $char3 = $char4;

        if ($char1 === "") continue;

        $result[$char1 . $char2 . $char3] = TRUE;
    };

    return array_keys($result);
};

class category
{

//properties
//--------------------------------------------------------------------------------------------------

    public $enabled = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct()
    {
        $mysql = new mysql();

        if (
             $mysql->verify_table(CMS_DB_CATEGORY_META,
                                  [CMS_DB_CATEGORY_META_INDEX      => "INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT",
                                   CMS_DB_CATEGORY_META_CLASS      => "VARCHAR(40) NOT NULL DEFAULT '' UNIQUE",
                                   CMS_DB_CATEGORY_META_COUNT_YES  => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_CATEGORY_META_COUNT_NO   => "INT UNSIGNED NOT NULL DEFAULT '0'"])
             &&
             $mysql->verify_table(CMS_DB_CATEGORY,
                                  [CMS_DB_CATEGORY_INDEX           => "INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT",
                                   CMS_DB_CATEGORY_TOKEN           => "CHAR(3) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL DEFAULT ''",
                                   CMS_DB_CATEGORY_CLASS           => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_CATEGORY_COUNT_YES       => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_CATEGORY_COUNT_NO        => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_CATEGORY_PROBABILITY_YES => "TINYINT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_CATEGORY_PROBABILITY_NO  => "TINYINT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_CATEGORY_RATING          => "TINYINT UNSIGNED NOT NULL DEFAULT '" .
                                                                      CMS_CATEGORY_PROBABILITY_DEFAULT . "'"],
                                  ["UNIQUE (" . CMS_DB_CATEGORY_CLASS . ", " .
                                                CMS_DB_CATEGORY_TOKEN . ")"])
           )
            $this->enabled = TRUE;
    }

//methods
//--------------------------------------------------------------------------------------------------

    function train($text,
                   $class = "",
                   $valid = TRUE,
                   $undo  = FALSE)
    {
        if (! $this->enabled) return FALSE;

        //tokenize text
        $token = category_tokenize_text($text);
        $count = count($token);
        if ($count === 0) return TRUE; //no tokens
        $token = sqlesc($token);

        $field_meta = $valid ? CMS_DB_CATEGORY_META_COUNT_YES : CMS_DB_CATEGORY_META_COUNT_NO;
        $field      = $valid ? CMS_DB_CATEGORY_COUNT_YES      : CMS_DB_CATEGORY_COUNT_NO;
        $class      = sqlesc($class);

        //add meta class
        if (! mysql_query("INSERT IGNORE INTO " . CMS_DB_CATEGORY_META . " " .
                          "(" .                   CMS_DB_CATEGORY_CLASS . ")" .
                          "VALUES ('" .           $class . "')"))
            return FALSE;

        //get class index
        if (
             (! $class_index = (int)mysql_insert_id())
             &&
             ($result = mysql_query("SELECT " . CMS_DB_CATEGORY_META_INDEX . " " .
                                    "FROM " .   CMS_DB_CATEGORY_META . " " .
                                    "WHERE " .  CMS_DB_CATEGORY_META_CLASS .
                                    "='" .      $class . "' " .
                                    "LIMIT 1"))
             &&
             mysql_num_rows($result)
           )
            $class_index = (int)mysql_result($result, 0, 0);

        if (
             $class_index
             &&

        //adjust meta count
             mysql_query("UPDATE " . CMS_DB_CATEGORY_META . " " .

        //increase/decrease counter
                         "SET " .    $field_meta .
                         "=" .       $field_meta . ($undo ? "-" : "+") . "1 " .

        //restrict to class
                         "WHERE " .  CMS_DB_CATEGORY_META_INDEX .
                         "='" .      $class_index . "' " .

        //only subtract if counter is positive
                         ($undo ? "AND $field_meta>0 " : "") .

        //limit to one operation
                         "LIMIT 1")
             &&

        //add token/class combinations
             mysql_query("INSERT IGNORE INTO " . CMS_DB_CATEGORY . " " .
                         "(" .                   CMS_DB_CATEGORY_TOKEN . ", " .
                                                 CMS_DB_CATEGORY_CLASS . ") " .
                         "VALUES (" .
                                           "'" . implode("', '" . $class_index . "'), ('", $token) . "', " .
                                           "'" . $class_index . "')")
             &&

        //adjust count
             mysql_query("UPDATE " . CMS_DB_CATEGORY . " " .

        //increase/decrease counter
                         "SET " .    $field .
                         "=" .       $field . ($undo ? "-" : "+") . "1 " .

        //restrict to tokens
                         "WHERE " .  CMS_DB_CATEGORY_TOKEN . " " .
                         "IN ('" .   implode("', '", $token) . "') " .

        //restrict to class
                         "AND " .    CMS_DB_CATEGORY_CLASS .
                         "='" .      $class_index . "' " .

        //only subtract if counter is positive
                         ($undo ? "AND $field>0 " : "") .

        //limit to number of tokens
                         "LIMIT " .  $count)
           )
            return TRUE;

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function update()
    {
        if (! $this->enabled) return FALSE;

        //update probability
        return mysql_query("UPDATE " .              CMS_DB_CATEGORY .      " AS a " .
                           "INNER JOIN " .          CMS_DB_CATEGORY_META . " AS b " .
                           "ON " .           "a." . CMS_DB_CATEGORY_CLASS .
                           "=" .             "b." . CMS_DB_CATEGORY_META_INDEX . " " .

        //compute relative positive rating
                           "SET " .          "a." . CMS_DB_CATEGORY_PROBABILITY_YES .
                           "=ROUND(100.0*" . "a." . CMS_DB_CATEGORY_COUNT_YES .
                           "/GREATEST(" .    "b." . CMS_DB_CATEGORY_META_COUNT_YES . ", 1)), " .

        //compute relative negative rating
                                             "a." . CMS_DB_CATEGORY_PROBABILITY_NO .
                           "=ROUND(100.0*" . "a." . CMS_DB_CATEGORY_COUNT_NO .
                           "/GREATEST(" .    "b." . CMS_DB_CATEGORY_META_COUNT_NO . ", 1))")
               &&

        //apply bayes formula
               mysql_query("UPDATE " .       CMS_DB_CATEGORY . " " .
                           "SET " .          CMS_DB_CATEGORY_RATING .
                           "=ROUND(100.0*" . CMS_DB_CATEGORY_PROBABILITY_YES .
                           "/GREATEST(" .    CMS_DB_CATEGORY_PROBABILITY_YES .
                           "+" .             CMS_DB_CATEGORY_PROBABILITY_NO . ", 1))")
               &&

        //clean up noise
               mysql_query("DELETE FROM " . CMS_DB_CATEGORY . " " .

        //restrict to sufficiently trained tokens
                           "WHERE (" .      CMS_DB_CATEGORY_COUNT_YES .
                           "+" .            CMS_DB_CATEGORY_COUNT_NO . ")" .
                           ">" .            CMS_CATEGORY_TRAINING_THRESHOLD . " " .

        //restrict to insignificant ratings
                           "AND " .         CMS_DB_CATEGORY_RATING . " " .
                           "BETWEEN " .     (50 - CMS_CATEGORY_DEVIATION_THRESHOLD) . " " .
                           "AND " .         (50 + CMS_CATEGORY_DEVIATION_THRESHOLD));
    }

//--------------------------------------------------------------------------------------------------

    function evaluate($text,
                      $class = "")
    {
        if (! $this->enabled) return FALSE;

        //tokenize text
        $token = category_tokenize_text($text);
        $count = count($token);
        if ($count === 0) return CMS_CATEGORY_PROBABILITY_DEFAULT; //no tokens
        $token = sqlesc($token);

        //check for minimum required number of positive and negative ratings
        if (! $result = mysql_query("SELECT " . CMS_DB_CATEGORY_META_INDEX . ", " .
                                                CMS_DB_CATEGORY_META_COUNT_YES . ", " .
                                                CMS_DB_CATEGORY_META_COUNT_NO . " " .
                                    "FROM " .   CMS_DB_CATEGORY_META . " " .
                                    "WHERE " .  CMS_DB_CATEGORY_META_CLASS .
                                    "='" .      sqlesc($class) . "' " .
                                    "AND " .    CMS_DB_CATEGORY_META_COUNT_YES .
                                    ">" .       CMS_CATEGORY_LEARNING_THRESHOLD . " " .
                                    "AND " .    CMS_DB_CATEGORY_META_COUNT_NO .
                                    ">" .       CMS_CATEGORY_LEARNING_THRESHOLD . " " .
                                    "LIMIT 1"))
            return FALSE; //error

        if (! $resultrow = mysql_fetch_row($result))
            return CMS_CATEGORY_PROBABILITY_DEFAULT; //learning mode

        list($class_index, $count_yes, $count_no) = $resultrow;

        //limit number of tokens to be used
        $count = min($count, CMS_CATEGORY_TOKEN_LIMIT);

        //retrieve most important matching tokens
        if ($result = mysql_query("SELECT " .      CMS_DB_CATEGORY_RATING . ", " .

        //compute deviation to indicate significance
                                  "ABS(50-CAST(" . CMS_DB_CATEGORY_RATING . " AS SIGNED)) " .
                                  "AS `" .        "#relevance` " .

                                  "FROM " .        CMS_DB_CATEGORY . " " .

        //restrict to class
                                  "WHERE " .       CMS_DB_CATEGORY_CLASS .
                                  "='" .           (int)$class_index . "' " .

        //restrict to tokens
                                  "AND " .         CMS_DB_CATEGORY_TOKEN . " " .
                                  "IN ('" .        implode("', '", $token) . "') " .

        //restrict to sufficiently trained tokens
                                  "AND (" .        CMS_DB_CATEGORY_COUNT_YES .
                                  "+" .            CMS_DB_CATEGORY_COUNT_NO . ")" .
                                  ">" .            CMS_CATEGORY_TRAINING_THRESHOLD . " " .

        //use significant ratings only
                                  "HAVING " .      "`#relevance`" .
                                  ">=" .           CMS_CATEGORY_DEVIATION_THRESHOLD . " " .

        //order by significance
                                  "ORDER BY " .    "`#relevance` DESC " .

        //limit to first n most significant ratings
                                  "LIMIT " .       $count))
        {
            $_count = mysql_num_rows($result);
            if ($_count === 0) return CMS_CATEGORY_PROBABILITY_DEFAULT; //no match

            //add up ratings retrieved from matching tokens
            $rating_avg = 0;
            while ($resultrow = mysql_fetch_row($result))
                $rating_avg += $resultrow[0];

            //add default rating for remaining tokens
            $rating_avg += ($count - $_count) * CMS_CATEGORY_PROBABILITY_DEFAULT;

            //compute average rating
            $rating_avg /= $count;

            return round($rating_avg);
        };

        return FALSE; //error
    }

//--------------------------------------------------------------------------------------------------

    function train_valid($text,
                         $class = "")
    {
        return $this->train($text, $class);
    }

//--------------------------------------------------------------------------------------------------

    function undo_valid($text,
                        $class = "")
    {
        return $this->train($text, $class, TRUE, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function train_invalid($text,
                           $class = "")
    {
        return $this->train($text, $class, FALSE);
    }

//--------------------------------------------------------------------------------------------------

    function undo_invalid($text,
                          $class = "")
    {
        return $this->train($text, $class, FALSE, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function train_spam($text)
    {
        return $this->train($text, CMS_CATEGORY_CLASS_SPAM);
    }

//--------------------------------------------------------------------------------------------------

    function undo_spam($text)
    {
        return $this->train($text, CMS_CATEGORY_CLASS_SPAM, TRUE, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function train_nospam($text)
    {
        return $this->train($text, CMS_CATEGORY_CLASS_SPAM, FALSE);
    }

//--------------------------------------------------------------------------------------------------

    function undo_nospam($text)
    {
        return $this->train($text, CMS_CATEGORY_CLASS_SPAM, FALSE, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function evaluate_spam($text)
    {
        return $this->evaluate($text, CMS_CATEGORY_CLASS_SPAM);
    }
};