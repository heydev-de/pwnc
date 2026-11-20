<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   CLASSIFIER CLASS
//==================================================================================================

//database
define("CMS_DB_CLASSIFIER_TOKEN",        CMS_DB_PREFIX . "classifier_token");
define("CMS_DB_CLASSIFIER_TOKEN_INDEX",  "id");
define("CMS_DB_CLASSIFIER_TOKEN_VALUE",  "token");

define("CMS_DB_CLASSIFIER_CLASS_INDEX",  "id");
define("CMS_DB_CLASSIFIER_CLASS_NAME",   "class");
define("CMS_DB_CLASSIFIER_CLASS_COUNT",  "count");
define("CMS_DB_CLASSIFIER_CLASS_WEIGHT", "weight");

define("CMS_DB_CLASSIFIER_WEIGHT_TOKEN", "token");
define("CMS_DB_CLASSIFIER_WEIGHT_CLASS", "class");
define("CMS_DB_CLASSIFIER_WEIGHT_VALUE", "weight");

//related functions
//--------------------------------------------------------------------------------------------------

function classifier_tokenize_text($text)
{
    $text   = implode("", tokenize_text(utf8_strtolower($text), TRUE)) . "\0\0\0";
    $result = [];
    $char1  = $char2 = $char3 = "";
    $count  = 0;

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
            else                $char4 .= $text[$i++] . $text[$i++] . $text[$i++];
        };

        $char1 = $char2;
        $char2 = $char3;
        $char3 = $char4;

        if ($char1 === "") continue;

        //count occurrences
        $key          = $char1 . $char2 . $char3;
        $result[$key] = ($result[$key] ?? 0) + 1;
        $count++;
    };

    //compute weight
    $count = sqrt($count);
    foreach ($result AS $key => $value)
        $result[$key] = sqrt($value) / $count;

    return $result;
};

class classifier
{

//properties
//--------------------------------------------------------------------------------------------------

    public $enabled = NULL;

    private $table_class;
    private $table_weight;
    private $lock;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($instance = "")
    {
        if (nstre($instance)) $instance = "_$instance";
        $instance           = CMS_DB_PREFIX . "classifier" . $instance . "_";
        $this->table_class  = $instance . "class";
        $this->table_weight = $instance . "weight";

        $mysql = new mysql();

        if (
             $mysql->verify_table(CMS_DB_CLASSIFIER_TOKEN,
                                  [CMS_DB_CLASSIFIER_TOKEN_INDEX  => "INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT",
                                   CMS_DB_CLASSIFIER_TOKEN_VALUE  => "CHAR(3) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL DEFAULT '' UNIQUE"])
             &&
             $mysql->verify_table($this->table_class,
                                  [CMS_DB_CLASSIFIER_CLASS_INDEX  => "INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT",
                                   CMS_DB_CLASSIFIER_CLASS_NAME   => "VARCHAR(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_bin NOT NULL DEFAULT '' UNIQUE",
                                   CMS_DB_CLASSIFIER_CLASS_COUNT  => "INT NOT NULL DEFAULT '0'",
                                   CMS_DB_CLASSIFIER_CLASS_WEIGHT => "FLOAT NOT NULL DEFAULT '0'"])
             &&
             $mysql->verify_table($this->table_weight,
                                  [CMS_DB_CLASSIFIER_WEIGHT_TOKEN => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_CLASSIFIER_WEIGHT_CLASS => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_CLASSIFIER_WEIGHT_VALUE => "FLOAT NOT NULL DEFAULT '0'"],
                                  ["PRIMARY KEY (" . CMS_DB_CLASSIFIER_WEIGHT_TOKEN . ", " .
                                                     CMS_DB_CLASSIFIER_WEIGHT_CLASS . ")"])
           )
        {
            $this->lock    = "pwnc." . hash32($mysql->database . ".classifier");
            $this->enabled = TRUE;
        };
    }

//methods
//--------------------------------------------------------------------------------------------------

    function train($text,
                   $class)
    {
        return $this->retrain("", [], $text, $class);
    }

//--------------------------------------------------------------------------------------------------

    function untrain($text,
                     $class)
    {
        return $this->retrain($text, $class, "", []);
    }

//--------------------------------------------------------------------------------------------------

    function retrain($text_old,
                     $class_old,
                     $text_new,
                     $class_new)
    {
        if (! $this->enabled) return FALSE;

        //process input
        $class_old = $this->normalize($class_old);
        $text_old  = $class_old ? classifier_tokenize_text($text_old) : [];
        if (! $text_old) $class_old = [];

        $class_new = $this->normalize($class_new);
        $text_new  = $class_new ? classifier_tokenize_text($text_new) : [];
        if (! $text_new) $class_new = [];

        if (! $class_old && ! $class_new) return TRUE;

        //claim lock
        if (! $this->lock()) return FALSE; //error

        try
        {
            //add new classes
            if (
                 $class_new
                 &&
                 ! mysql_query("INSERT IGNORE INTO `" . $this->table_class . "` " .
                               "(" .                    CMS_DB_CLASSIFIER_CLASS_NAME . ") " .
                               "VALUES ('" .            implode("'), ('", sqlesc($class_new)) . "')")
               )
                return FALSE; //error

            //retrieve class indices
            if (! $result = mysql_query("SELECT " . CMS_DB_CLASSIFIER_CLASS_INDEX . ", " .
                                                    CMS_DB_CLASSIFIER_CLASS_NAME . " " .
                                        "FROM `" .  $this->table_class . "` " .
                                        "WHERE " .  CMS_DB_CLASSIFIER_CLASS_NAME . " " .
                                        "IN ('" .   implode("', '", sqlesc(array_merge($class_old, $class_new))) . "')"))
                return FALSE; //error

            $index = [];
            while ($resultrow = mysql_fetch_row($result))
                $index[$resultrow[1]] = (int)$resultrow[0];

            //collect class indices
            $old = $new = [];
            foreach ($class_old AS $value) { if (isset($index[$value])) $old[$index[$value]] = TRUE; };
            foreach ($class_new AS $value) { if (isset($index[$value])) $new[$index[$value]] = TRUE; };
            $class = array_keys($old + $new);
            if (! $class) return TRUE;

            //add new tokens
            if (
                 $text_new
                 &&
                 ! mysql_query("INSERT IGNORE INTO `" . CMS_DB_CLASSIFIER_TOKEN . "` " .
                               "(" .                    CMS_DB_CLASSIFIER_TOKEN_VALUE . ") " .
                               "VALUES ('" .            implode("'), ('", sqlesc(array_keys($text_new))) . "')")
               )
                return FALSE; //error

            //retrieve token indices and weights
            $result = mysql_query(
                "SELECT `#t`." . CMS_DB_CLASSIFIER_TOKEN_INDEX . ", " .
                "`#t`." .        CMS_DB_CLASSIFIER_TOKEN_VALUE . ", " .
                "`#w`." .        CMS_DB_CLASSIFIER_WEIGHT_CLASS . ", " .
                "`#w`." .        CMS_DB_CLASSIFIER_WEIGHT_VALUE . " " .
                "FROM `" .       CMS_DB_CLASSIFIER_TOKEN . "` AS `#t` " .
                "LEFT JOIN `" .  $this->table_weight . "` AS `#w` " .
                "ON `#w`." .     CMS_DB_CLASSIFIER_WEIGHT_TOKEN .
                "=`#t`." .       CMS_DB_CLASSIFIER_TOKEN_INDEX . " " .
                "AND `#w`." .    CMS_DB_CLASSIFIER_WEIGHT_CLASS . " " .
                "IN (" .         implode(", ", $class) . ") " .
                "WHERE `#t`." .  CMS_DB_CLASSIFIER_TOKEN_VALUE . " " .
                "IN ('" .        implode("', '", sqlesc(array_keys($text_old + $text_new))) . "')");
            if (! $result) return FALSE; //error

            $text = $weight = [];
            while ($resultrow = mysql_fetch_row($result))
            {
                $text[$resultrow[0]] = $resultrow[1];
                if ($resultrow[2] !== NULL)
                    $weight[$resultrow[0]][$resultrow[2]] = $resultrow[3];
            };

            //compute weights
            $list   = $_weight = [];
            $_count = array_fill_keys(array_keys(array_diff_key($new, $old)), 1); //new classes

            foreach ($text AS $key => $value)
            {
                foreach ($class AS $_value)
                {
                    $delta = (isset($new[$_value]) ? $text_new[$value] ?? 0 : 0) -
                             (isset($old[$_value]) ? $text_old[$value] ?? 0 : 0);
                    if (! $delta) continue; //no change

                    $weight_old = $weight[$key][$_value] ?? 0;
                    if (! $weight_old && ($delta < 0)) continue; //no subtraction effect

                    $weight_new = max($weight_old + $delta, 0);
                    $list[]     = "($key, $_value, $weight_new)";

                    //adjust class weight
                    $_weight[$_value] =
                        ($_weight[$_value] ?? 0) +
                        $weight_new * $weight_new -
                        $weight_old * $weight_old;

                    //remove text from class
                    if (($delta < 0) && ! isset($new[$_value])) $_count[$_value] = -1;
                };
            };

            //add or update weights
            if (
                 $list
                 &&
                 ! mysql_query("INSERT INTO `" .            $this->table_weight . "` " .
                               "(" .                        CMS_DB_CLASSIFIER_WEIGHT_TOKEN . ", " .
                                                            CMS_DB_CLASSIFIER_WEIGHT_CLASS . ", " .
                                                            CMS_DB_CLASSIFIER_WEIGHT_VALUE . ") " .
                               "VALUES " .                  implode(", ", $list) . " " .
                               "ON DUPLICATE KEY UPDATE " . CMS_DB_CLASSIFIER_WEIGHT_VALUE .
                               "=VALUES(" .                 CMS_DB_CLASSIFIER_WEIGHT_VALUE . ")")
               )
                return FALSE; //error

            //adjust class weights and coun
            if ($class = array_keys($_weight + $_count))
            {
                $weight = $count = "";
                foreach ($class AS $key)
                {
                    $weight .= " WHEN $key THEN " . ($_weight[$key] ?? 0);
                    $count  .= " WHEN $key THEN " . ($_count[$key]  ?? 0);
                };

                if (! mysql_query("UPDATE `" .   $this->table_class . "` " .
                                  "SET " .       CMS_DB_CLASSIFIER_CLASS_WEIGHT .
                                  "=GREATEST(" . CMS_DB_CLASSIFIER_CLASS_WEIGHT .
                                  "+CASE " .     CMS_DB_CLASSIFIER_CLASS_INDEX .
                                                 $weight . " END, 0), " .
                                                 CMS_DB_CLASSIFIER_CLASS_COUNT .
                                  "=GREATEST(" . CMS_DB_CLASSIFIER_CLASS_COUNT .
                                  "+CASE " .     CMS_DB_CLASSIFIER_CLASS_INDEX .
                                                 $count . " END, 0) " .
                                  "WHERE " .     CMS_DB_CLASSIFIER_CLASS_INDEX . " " .
                                  "IN (" .       implode(", ", $class) . ")"))
                    return FALSE; //error
            };

            return TRUE;
        }
        finally
        {
            //release lock
            $this->lock(TRUE);
        };
    }

//--------------------------------------------------------------------------------------------------

    function classify($text,
                      $limit     = 0,
                      $threshold = 0.25)
    {
        if (! $this->enabled) return FALSE;

        //compute token weights
        $token = classifier_tokenize_text($text);
        if (! $token) return []; //no tokens

        //search tokens
        $array = [];
        foreach ($token AS $key => $value)
            $array[] = "SELECT '" . sqlesc($key) . "' COLLATE utf8mb4_bin AS `#token`, $value AS `#value`";

        //sum score per class
        $query =
            "SELECT STRAIGHT_JOIN " .
                "`#w`." . CMS_DB_CLASSIFIER_WEIGHT_CLASS . " AS `#class`, " .
                "SUM(`#q`.`#value` * `#w`." . CMS_DB_CLASSIFIER_WEIGHT_VALUE . ") AS `#score` " .

            "FROM (" . implode(" UNION ALL ", $array) . ") AS `#q` " .

            //tokens
            "JOIN `" . CMS_DB_CLASSIFIER_TOKEN . "` AS `#t` " .
            "ON `#t`." . CMS_DB_CLASSIFIER_TOKEN_VALUE . "=`#q`.`#token` " .

            //weights
            "JOIN `" . $this->table_weight . "` AS `#w` " .
            "ON `#w`." . CMS_DB_CLASSIFIER_WEIGHT_TOKEN .
                "=`#t`." . CMS_DB_CLASSIFIER_TOKEN_INDEX . " " .

            "GROUP BY `#w`." . CMS_DB_CLASSIFIER_WEIGHT_CLASS;

        //resolve classes and normalize score
        $query =
            "SELECT " .
                "`#c`." . CMS_DB_CLASSIFIER_CLASS_NAME . " AS `#class`, " .
                "`#s`.`#score`/SQRT(`#c`." . CMS_DB_CLASSIFIER_CLASS_WEIGHT . ") AS `#score` " .

            "FROM ($query) AS `#s` " .

            //classes
            "JOIN `" . $this->table_class . "` AS `#c` " .
            "ON `#c`." . CMS_DB_CLASSIFIER_CLASS_INDEX . "=`#s`.`#class` " .
            "AND `#c`." . CMS_DB_CLASSIFIER_CLASS_COUNT . ">0 " .
            "AND `#c`." . CMS_DB_CLASSIFIER_CLASS_WEIGHT . ">0 " .

            //threshold
            "HAVING `#score`>=" . (float)$threshold;

        if (! $result = mysql_query($query)) return FALSE; //error

        //collect results
        $array = [];
        while ($resultrow = mysql_fetch_assoc($result))
            $array[$resultrow["#class"]] = $resultrow["#score"];

        //sort
        arsort($array);

        //apply limit
        return $limit ? array_slice($array, 0, $limit, TRUE) : $array;
    }

//--------------------------------------------------------------------------------------------------

    function count()
    {
        if (! $this->enabled) return FALSE;

        $result = mysql_query(
            "SELECT COUNT(*) " .
            "FROM `" . $this->table_class . "` " .
            "WHERE " . CMS_DB_CLASSIFIER_CLASS_COUNT . ">0");
        return $result ? (int)mysql_result($result, 0) : FALSE;
    }

//--------------------------------------------------------------------------------------------------

    private function normalize($class)
    {
        return array_unique(array_map(
            fn($value) => utf8_substr((string)$value, 0, 255),
            (array)$class));
    }

//--------------------------------------------------------------------------------------------------

    private function lock($remove = FALSE)
    {
        if ($remove) return (bool)mysql_query("SELECT RELEASE_LOCK('" . $this->lock . "')");
        $result = mysql_query("SELECT GET_LOCK('" . $this->lock . "', 10)");
        return $result && mysql_result($result, 0);
    }
};
