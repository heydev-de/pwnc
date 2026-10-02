<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   MEMORY CLASS
//==================================================================================================

//permission
define("CMS_MEMORY_PERMISSION_OPERATOR", "operator");
define("CMS_MEMORY_PERMISSION_WRITER",   "writer");
define("CMS_MEMORY_PERMISSION_READER",   "reader");

//database
define("CMS_DB_MEMORY",                 CMS_DB_PREFIX . "memory_");
define("CMS_DB_MEMORY_INDEX",           "id");
define("CMS_DB_MEMORY_MNEMONIC",        "mnemonic");
define("CMS_DB_MEMORY_DESCRIPTION",     "description");
define("CMS_DB_MEMORY_CONTENT",         "content");
define("CMS_DB_MEMORY_ATTACHMENT_TYPE", "attachment_type");
define("CMS_DB_MEMORY_OWNER",           "owner");
define("CMS_DB_MEMORY_CREATED",         "created");
define("CMS_DB_MEMORY_EDITOR",          "editor");
define("CMS_DB_MEMORY_EDITED",          "edited");
define("CMS_DB_MEMORY_ACCESS_COUNT",    "access_count");
define("CMS_DB_MEMORY_ACCESSED",        "accessed");
define("CMS_DB_MEMORY_FOLLOWUP",        "followup");
define("CMS_DB_MEMORY_READ_ONLY",       "read_only");
define("CMS_DB_MEMORY_SAFE",            "safe");
define("CMS_DB_MEMORY_RATING_VALUE",    "rating_value");
define("CMS_DB_MEMORY_RATING_SUM",      "rating_sum");
define("CMS_DB_MEMORY_RATING_COUNT",    "rating_count");

define("CMS_DB_MEMORY_MNEM",            CMS_DB_PREFIX . "memory_mnem_");
define("CMS_DB_MEMORY_MNEM_VALUE",      "value");
define("CMS_DB_MEMORY_MNEM_COUNT",      "count");

class memory
{

//properties
//--------------------------------------------------------------------------------------------------

    private $user     = NULL;
    private $operator = FALSE;
    private $mysql    = NULL;

    //relevance
    public $relevance_rating  = 1.0;
    public $relevance_recency = 2.0;
    public $relevance_limit   = 0.25;

    //cleanup
    public $cleanup_edit_expiry = 180;
    public $cleanup_read_expiry = 90;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($user = NULL)
    {
        $this->user     = $user ?? CMS_SUPERUSER;
        $this->operator = cms_permission("memory." . CMS_MEMORY_PERMISSION_OPERATOR, FALSE, FALSE);
        $this->mysql    = new mysql();
    }

//methods
//--------------------------------------------------------------------------------------------------

    function add($region,
                 $data,
                 $force = FALSE)
    {
        if (! $force && ! $this->is_writer($region)) return FALSE; //no permission
        if (! is_array($data))                       return FALSE; //invalid
        if (! $this->verify_table($region))          return FALSE; //error

        //prepare values
        $mnemonic    = $this->mnemonic_tokenize($data["mnemonic"] ?? "");
        $description = first_words(stripspaces($data["description"] ?? ""), 500);
        $attachment  = is_array($data["attachment"] ?? NULL) ? $data["attachment"] : NULL;
        $type        = $attachment ? (string)($attachment["type"] ?? "application/octet-stream") : "";
        $read_only   = empty($data["read_only"]) ? "0" : "1";
        $safe        = empty($data["safe"])      ? "0" : "1";
        $followup    = $data["followup"] ?? NULL;

        //convert follow-up date
        if (($followup !== NULL) && ($followup !== FALSE))
        {
            $followup = $this->parse_date($followup);
            if ($followup === NULL) return NULL; //invalid
        };

        //add record
        if (! mysql_query(
                "INSERT INTO `" . sqlesc($this->table_name($region), TRUE) . "` " .
                "(" . CMS_DB_MEMORY_MNEMONIC . ", " .
                      CMS_DB_MEMORY_DESCRIPTION . ", " .
                      CMS_DB_MEMORY_CONTENT . ", " .
                      CMS_DB_MEMORY_ATTACHMENT_TYPE . ", " .
                      CMS_DB_MEMORY_OWNER . ", " .
                      CMS_DB_MEMORY_EDITOR . ", " .
                      CMS_DB_MEMORY_FOLLOWUP . ", " .
                      CMS_DB_MEMORY_READ_ONLY . ", " .
                      CMS_DB_MEMORY_SAFE . ") " .
                "VALUES " .
                "('" . sqlesc(implode("\n", $mnemonic)) . "', " .
                 "'" . sqlesc($description) . "', " .
                 "'" . sqlesc((string)($data["content"] ?? "")) . "', " .
                 "'" . sqlesc($type) . "', " .
                 "'" . sqlesc($this->user) . "', " .
                 "'" . sqlesc($this->user) . "', " .
                 (isset($followup) ? "'" . sqlesc($followup) . "'" : "NULL") . ", " .
                 "'" . $read_only . "', " .
                 "'" . $safe . "')"))
            return FALSE;

        //new record id
        $id = mysql_insert_id();

        //index mnemonics
        $this->mnemonic_index($region, $mnemonic);

        //store attachment
        if ($attachment)
            write_file(
                $this->get_attachment_path($region, $id),
                base64_decode((string)($attachment["data"] ?? "")));

        return $id;
    }

//--------------------------------------------------------------------------------------------------

    function get($region,
                 $id)
    {
        if (! $this->is_reader($region)) return FALSE; //no permission

        $table = $this->table_name($region);
        $array = $this->get_record($table, $id);
        if ($array === FALSE) return FALSE; //error or not found

        //update access and remove follow-up date
        mysql_query(
            "UPDATE `" . sqlesc($table, TRUE) . "` " .
            "SET " .     CMS_DB_MEMORY_ACCESS_COUNT .
            "=" .        CMS_DB_MEMORY_ACCESS_COUNT . "+1, " .
                         CMS_DB_MEMORY_ACCESSED . "=NOW(), " .
                         CMS_DB_MEMORY_FOLLOWUP . "=NULL " .
            "WHERE " .   CMS_DB_MEMORY_INDEX .
            "='" .       sqlesc($id) . "' " .
            "LIMIT 1");
        $array[CMS_DB_MEMORY_FOLLOWUP] = NULL;

        //attachment path
        if (nstre($array[CMS_DB_MEMORY_ATTACHMENT_TYPE]))
            $array["attachment_path"] = $this->get_attachment_path($region, $id);

        return $array;
    }

//--------------------------------------------------------------------------------------------------

    function get_attachment($region,
                            $id)
    {
        if (! $this->is_reader($region))                     return FALSE; //no permission
        if (($table = $this->table_name($region)) === FALSE) return FALSE; //invalid

        //retrieve attachment type
        $type = $this->mysql->get($id, CMS_DB_MEMORY_ATTACHMENT_TYPE, $table, CMS_DB_MEMORY_INDEX);
        if (stre($type)) return FALSE; //no attachment

        //get path
        $path = $this->get_attachment_path($region, $id);
        if (($path === FALSE) || ! is_file($path)) return FALSE; //no file

        return ["type" => $type,
                "path" => $path];
    }

//--------------------------------------------------------------------------------------------------

    function set($region,
                 $id,
                 $data)
    {
        if (! $this->is_writer($region)) return FALSE; //no permission
        if (! is_array($data))           return FALSE; //invalid

        $table = $this->table_name($region);
        $array = $this->get_record($table, $id);
        if ($array === FALSE) return FALSE; //error or not found

        //check permission
        if (

        //locked
             $array[CMS_DB_MEMORY_READ_ONLY]
             &&

        //not owner
             nstreq($array[CMS_DB_MEMORY_OWNER], $this->user)
             &&

        //not operator
             ! $this->is_operator($region)
           )
            return FALSE; //no permission

        //editing
        $set = [CMS_DB_MEMORY_EDITOR . "='" . sqlesc($this->user) . "'",
                CMS_DB_MEMORY_EDITED . "=NOW()"];

        //follow-up
        if (isset($data["followup"]))
        {
            if (
                 (($value = $data["followup"]) !== FALSE)
                 &&
                 (($value = $this->parse_date($value)) === NULL)
               )
                return NULL; //invalid

            $set[] = CMS_DB_MEMORY_FOLLOWUP . "=" . ($value ? "'" . sqlesc($value) . "'" : "NULL");
        };

        //normalize mnemonic
        if (isset($data["mnemonic"]))
        {
            $mnemonic         = $this->mnemonic_tokenize($data["mnemonic"]);
            $data["mnemonic"] = implode("\n", $mnemonic);
        };

        //normalize description
        if (nstre($data["description"] ?? ""))
            $data["description"] = first_words(stripspaces($data["description"]), 500);

        //text
        foreach (["mnemonic"    => CMS_DB_MEMORY_MNEMONIC,
                  "description" => CMS_DB_MEMORY_DESCRIPTION,
                  "content"     => CMS_DB_MEMORY_CONTENT]
                 AS $key => $value)
        {
            if (! isset($data[$key])) continue;
            $_value = ($data[$key] === FALSE) ? "" : (string)$data[$key];
            $set[]  = "`" . sqlesc($value, TRUE) . "`='" . sqlesc($_value) . "'";
        };

        //attachment
        if (
             (($value = ($data["attachment"] ?? NULL)) !== NULL)
             &&
             (($path = $this->get_attachment_path($region, $id)) !== FALSE)
           )
        {
            if ($value === FALSE)
            {
                if (is_file($path)) unlink($path);
                $set[] = CMS_DB_MEMORY_ATTACHMENT_TYPE . "=''";
            }
            elseif (is_array($value))
            {
                write_file($path, base64_decode((string)($value["data"] ?? "")));
                $set[] = CMS_DB_MEMORY_ATTACHMENT_TYPE . "='" .
                         sqlesc((string)($value["type"] ?? "application/octet-stream")) . "'";
            };
        };

        //flag
        foreach (["read_only" => CMS_DB_MEMORY_READ_ONLY,
                  "safe"      => CMS_DB_MEMORY_SAFE]
                 AS $key => $value)
        {
            if (! isset($data[$key])) continue;
            $set[] = "`" . sqlesc($value, TRUE) . "`='" . ($data[$key] ? "1" : "0") . "'";
        };

        //update record
        if (! mysql_query("UPDATE `" . sqlesc($table, TRUE) . "` " .
                          "SET " .     implode(", ", $set) . " " .
                          "WHERE " .   CMS_DB_MEMORY_INDEX .
                          "='" .       sqlesc($id) . "' " .
                          "LIMIT 1"))
            return FALSE; //error

        //update mnemonic index
        if (isset($mnemonic))
        {
            $_mnemonic = $this->mnemonic_tokenize($array[CMS_DB_MEMORY_MNEMONIC]);
            if ($data["mnemonic"] !== implode("\n", $_mnemonic))
                $this->mnemonic_index($region, $mnemonic, $_mnemonic);
        };

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function rate($region,
                  $id,
                  $value)
    {
        if (! $this->is_writer($region)) return FALSE; //no permission

        $table = $this->table_name($region);
        $value = (int)$value;

        //delete
        if ($value === 0)
        {
            if (

            //record exists
                 (($array = $this->get_record($table, $id)) !== FALSE)
                 &&

            //not locked
                 ! $array[CMS_DB_MEMORY_SAFE]
                 &&

            //owner or operator
                 (
                   streq($array[CMS_DB_MEMORY_OWNER], $this->user)
                   ||
                   $this->is_operator($region)
                 )
                 &&

            //delete successful
                mysql_query("DELETE FROM `" . sqlesc($table, TRUE) . "` " .
                            "WHERE " .        CMS_DB_MEMORY_INDEX .
                            "='" .            sqlesc($id) . "' " .
                            "LIMIT 1")
               )
            {
                //delete attachment
                $path = $this->get_attachment_path($region, $id);
                if (($path !== FALSE) && is_file($path)) unlink($path);

                //delete mnemonics
                if ($mnemonic = $this->mnemonic_tokenize($array[CMS_DB_MEMORY_MNEMONIC]))
                    $this->mnemonic_index($region, [], $mnemonic);

                return TRUE;
            };

            return FALSE; //error or no permission
        };

        if (($value < 1) || ($value > 5)) return FALSE; //invalid

        return mysql_query(
            "UPDATE `" . sqlesc($table, TRUE) . "` " .

        //increase sum
            "SET " .     CMS_DB_MEMORY_RATING_SUM .
            "=" .        CMS_DB_MEMORY_RATING_SUM . "+$value, " .

        //increase count
                         CMS_DB_MEMORY_RATING_COUNT .
            "=" .        CMS_DB_MEMORY_RATING_COUNT . "+1, " .

        //compute average value
                         CMS_DB_MEMORY_RATING_VALUE .
            "=" .        CMS_DB_MEMORY_RATING_SUM . "/" . CMS_DB_MEMORY_RATING_COUNT . " " .
            "WHERE " .   CMS_DB_MEMORY_INDEX .
            "='" .       sqlesc($id) . "' " .
            "LIMIT 1")
            &&

        //record affected
            mysql_affected_rows();
    }

//--------------------------------------------------------------------------------------------------

    function get_region_list()
    {
        //retrieve memory tables
        $result = mysql_query(
            "SELECT TABLE_NAME " .
            "FROM INFORMATION_SCHEMA.TABLES " .
            "WHERE TABLE_SCHEMA=DATABASE() " .
            "AND (TABLE_NAME LIKE '" . CMS_DB_MEMORY . "scope_%' " .
            "OR TABLE_NAME LIKE '" .   CMS_DB_MEMORY . "user_%')");
        if ($result === FALSE) return FALSE; //error

        //collect accessible regions
        $array = [];
        while ($resultrow = mysql_fetch_row($result))
        {
            $region = $this->table_to_region($resultrow[0]);
            if (($region === FALSE) || ! $this->is_reader($region)) continue; //skip

            //add region
            $array[$region] = [
                "mnemonic" => $this->mnemonic_list($region, 10),
                "reader"   => TRUE,
                "writer"   => $this->is_writer($region),
                "operator" => $this->is_operator($region)];
        };

        return $array;
    }

//--------------------------------------------------------------------------------------------------

    function get_region_overview($region,
                                 $limit = 20)
    {
        if (! $this->is_reader($region)) return FALSE; //no permission

        //rating factor
        $order  = sqlesc($this->relevance_rating / 5) .
                  "*" . CMS_DB_MEMORY_RATING_VALUE;

        //recency factor
        $order .= "+" . sqlesc($this->relevance_recency) .
                  "/(1+DATEDIFF(NOW(), COALESCE(" .
                  CMS_DB_MEMORY_EDITED . ", " . CMS_DB_MEMORY_CREATED . ")))";

        //retrieve relevant records
        $result = mysql_query(
            "SELECT * " .
            "FROM `" .     sqlesc($this->table_name($region), TRUE) . "` " .
            "ORDER BY (" . $order . ") DESC " .
            "LIMIT " .     (int)$limit);
        if ($result === FALSE) return FALSE; //error

        //collect data
        $array = [];
        while ($resultrow = mysql_fetch_assoc($result))
            $array[] = $resultrow;

        return $array;
    }

//--------------------------------------------------------------------------------------------------

    function followup_list()
    {
        //retrieve memory tables
        $result = mysql_query(
            "SELECT TABLE_NAME " .
            "FROM INFORMATION_SCHEMA.TABLES " .
            "WHERE TABLE_SCHEMA=DATABASE() " .
            "AND (TABLE_NAME LIKE '" . CMS_DB_MEMORY . "scope_%' " .
            "OR TABLE_NAME LIKE '" .   CMS_DB_MEMORY . "user_%')");
        if ($result === FALSE) return FALSE; //error

        //collect follow-ups
        $array = [];
        while ($resultrow = mysql_fetch_row($result))
        {
            $region = $this->table_to_region($resultrow[0]);
            if ($region === FALSE)           continue; //invalid
            if (! $this->is_reader($region)) continue; //no permission

            //retrieve
            $_result = mysql_query(
                "SELECT " . CMS_DB_MEMORY_INDEX . " " .
                "FROM `" .  sqlesc($resultrow[0], TRUE) . "` " .
                "WHERE " .  CMS_DB_MEMORY_FOLLOWUP . "<=NOW()");
            if ($_result === FALSE) continue; //error

            //collect
            while ($_resultrow = mysql_fetch_row($_result))
                $array[] = ["region" => $region,
                            "id"     => (int)$_resultrow[0]];
        };

        return $array;
    }

//--------------------------------------------------------------------------------------------------

    function cleanup()
    {
        if (! cms_permission("daemon", FALSE, FALSE)) return FALSE; //no permission

        //retrieve tables
        $result = mysql_query(
            "SELECT TABLE_NAME " .
            "FROM INFORMATION_SCHEMA.TABLES " .
            "WHERE TABLE_SCHEMA=DATABASE() " .
            "AND (TABLE_NAME LIKE '" . CMS_DB_MEMORY . "scope_%' " .
            "OR TABLE_NAME LIKE '" .   CMS_DB_MEMORY . "user_%')");
        if ($result === FALSE) return FALSE; //error

        //prepare statement
        $query =
            "SELECT " .       CMS_DB_MEMORY_INDEX . ", " .
                              CMS_DB_MEMORY_MNEMONIC . " " .
            "FROM `%s` " .

        //not locked
            "WHERE " .        CMS_DB_MEMORY_SAFE . "=0 " .

        //last write date exceeds age limit multiplied by rating factor
            "AND COALESCE(" . CMS_DB_MEMORY_EDITED . ", " .
                              CMS_DB_MEMORY_CREATED . ")" .
            "<DATE_SUB(NOW(), " .
            "INTERVAL (" .    ($this->cleanup_edit_expiry / 5) .
            "*" .             CMS_DB_MEMORY_RATING_VALUE . ") DAY) " .

        //last read date exceeds age limit multiplied by rating factor
            "AND COALESCE(" . CMS_DB_MEMORY_ACCESSED . ", " .
                              CMS_DB_MEMORY_CREATED . ")" .
            "<DATE_SUB(NOW(), " .
            "INTERVAL (" .    ($this->cleanup_read_expiry / 5) .
            "*" .             CMS_DB_MEMORY_RATING_VALUE . ") DAY)";

        //process tables
        while ($resultrow = mysql_fetch_row($result))
        {
            if (($region = $this->table_to_region($table = $resultrow[0])) === FALSE) continue; //invalid
            $table  = sqlesc($table, TRUE);
            $_table = NULL;

            //execute query
            $_result = mysql_query(sprintf($query, $table));
            if ($_result === FALSE) continue; //error

            //collect records
            $id = $mnemonic = [];
            while ($resultrow = mysql_fetch_row($_result))
            {
                $id[] = $resultrow[0];

                //delete file
                $path = $this->get_attachment_path($region, $resultrow[0]);
                if (($path !== FALSE) && is_file($path)) unlink($path);

                //collect mnemonics
                foreach ($this->mnemonic_tokenize($resultrow[1]) AS $value)
                    $mnemonic[$value] = ($mnemonic[$value] ?? 0) + 1;
            };

            if ($id)
            {
                //delete records
                if (! mysql_query("DELETE FROM `$table` " .
                                  "WHERE " . CMS_DB_MEMORY_INDEX . " " .
                                  "IN ('" .  implode("', '", sqlesc($id)) . "')"))
                    continue; //error

                //update mnemonic index
                if ($mnemonic)
                {
                    //mnemonic table
                    $_table = sqlesc($this->table_name($region, TRUE), TRUE);

                    //build subtraction list
                    $array = [];
                    foreach ($mnemonic AS $key => $value)
                        $array[] = "('" . sqlesc($key) . "', $value)";

                    //decrease mnemonic count
                    mysql_query(
                        "INSERT INTO `$_table` (" .  CMS_DB_MEMORY_MNEM_VALUE . ", " .
                                                     CMS_DB_MEMORY_MNEM_COUNT . ") " .
                        "VALUES " .                  implode(", ", $array) . " " .
                        "ON DUPLICATE KEY UPDATE " . CMS_DB_MEMORY_MNEM_COUNT .
                        "=" .                        CMS_DB_MEMORY_MNEM_COUNT .
                        "-VALUES(" .                 CMS_DB_MEMORY_MNEM_COUNT . ")");

                    //delete unused mnemonics
                    mysql_query(
                        "DELETE FROM `$_table` " .
                        "WHERE " . CMS_DB_MEMORY_MNEM_COUNT . "<=0");
                };
            };

            //delete empty tables
            $_result = mysql_query("SELECT 1 FROM `$table` LIMIT 1");
            if (($_result === FALSE) || mysql_num_rows($_result)) continue; //error or not empty

            mysql_query("DROP TABLE `$table`");
            mysql_query("DROP TABLE `" . ($_table ?? sqlesc($this->table_name($region, TRUE), TRUE)) . "`");
        };

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------
//   SEARCH
//--------------------------------------------------------------------------------------------------

    function search($region,
                    $value,
                    &$mnemonic = NULL)
    {
        if (! $this->is_reader($region)) return FALSE; //no permission

        $result = $this->search_table($this->table_name($region), $value);
        if ($result === FALSE) return FALSE; //error

        //collect mnemonics
        $array = [];
        foreach ($result AS $_value)
            $array[] = $this->mnemonic_tokenize($_value[CMS_DB_MEMORY_MNEMONIC]);

        //retrieve mnemonic count from region
        $mnemonic = $this->mnemonic_count($region, array_unique(array_merge(...$array)));

        return $result;
    }

//--------------------------------------------------------------------------------------------------

    function search_all($value,
                        &$mnemonic = NULL)
    {
        if (($list = $this->get_region_list()) === FALSE) return FALSE; //error

        //extract words
        preg_match_all("/" . CMS_REGEX_WORD . "/u", (string)$value, $match);
        $array = array_unique(array_map(__NAMESPACE__ . "\\utf8_strtolower", $match[0]));
        if (! $array) return []; //empty

        //prepare statement
        $_array = [];
        foreach ($array AS $_value)
        {
            $_value   = addcslashes($_value, "%_");
            $_array[] = CMS_DB_MEMORY_MNEM_VALUE . " LIKE '%" . sqlesc($_value) . "%'";
        };

        $query = "SELECT 1 " .
                 "FROM `%s` " .
                 "WHERE " . str_replace("%", "%%", implode(" OR ", $_array)) . " " .
                 "LIMIT 1";

        $return = [];
        foreach (array_keys($list) AS $region)
        {
            //query mnemonic index for matches
            $result = mysql_query(sprintf($query, sqlesc($this->table_name($region, TRUE), TRUE)));
            if (($result === FALSE) || ! mysql_num_rows($result)) continue; //no match

            //search region
            $result = $this->search_table($this->table_name($region), $value, 20);
            if (! $result) continue; //no match

            //normalize relevance
            $max = $result[0]["relevance"];
            foreach ($result AS $_value)
            {
                $_value["relevance"] = $_value["relevance"] / $max;
                $_value["region"]    = $region;
                $return[]            = $_value;
            };
        };

        if (! $return) return []; //empty

        //order by relevance
        usort($return, function($a, $b) { return $b["relevance"] <=> $a["relevance"]; });

        //filter relevant results
        $limit = $return[0]["relevance"] * $this->relevance_limit;
        for ($i = 1, $c = count($return); $i < $c; $i++)
            if ($return[$i]["relevance"] < $limit) break;

        $return = array_slice($return, 0, $i);

        //collect mnemonics
        $array = [];
        foreach ($return AS $value)
            $array[] = $this->mnemonic_tokenize($value[CMS_DB_MEMORY_MNEMONIC]);

        $array = array_unique(array_merge(...$array));

        //retrieve mnemonic count from accessible regions
        $mnemonic = [];
        foreach ($list AS $region => $_)
        {
            $count = $this->mnemonic_count($region, $array);
            if (! $count) continue;

            //summarize
            foreach ($count AS $key => $value)
                $mnemonic[$key] = ($mnemonic[$key] ?? 0) + $value;
        };

        //order the summarized counts
        $mnemonic = $this->mnemonic_sort($mnemonic);

        return $return;
    }

//--------------------------------------------------------------------------------------------------

    private function search_table($table,
                                  $value,
                                  $limit = 200)
    {
        //input sanitation for boolean mode
        $_value = preg_replace("/(?:[\"\(\)\*\+\-<>@\[\]~]|" . CMS_REGEX_BORDER . ")+/u", " ", $value);
        $_value = preg_replace("/" . CMS_REGEX_WORD . "/u", "\\0*", $_value);

        $value  = sqlesc($value);
        $_value = sqlesc($_value);

        //retrieve matching records ordered by relevance
        $result = mysql_query(
            "SELECT *, " .
            "MATCH(" . CMS_DB_MEMORY_MNEMONIC .    ") AGAINST ('$value' IN NATURAL LANGUAGE MODE) * 4 + " .
            "MATCH(" . CMS_DB_MEMORY_DESCRIPTION . ") AGAINST ('$value' IN NATURAL LANGUAGE MODE) * 2 + " .
            "MATCH(" . CMS_DB_MEMORY_CONTENT .     ") AGAINST ('$value' IN NATURAL LANGUAGE MODE) + " .
            "MATCH(" . CMS_DB_MEMORY_MNEMONIC .    ") AGAINST ('$_value' IN BOOLEAN MODE) * 4 + " .
            "MATCH(" . CMS_DB_MEMORY_DESCRIPTION . ") AGAINST ('$_value' IN BOOLEAN MODE) * 2 + " .
            "MATCH(" . CMS_DB_MEMORY_CONTENT .     ") AGAINST ('$_value' IN BOOLEAN MODE) AS `relevance` " .
            "FROM `" . sqlesc($table, TRUE) . "` " .
            "WHERE MATCH(" . CMS_DB_MEMORY_MNEMONIC . ", " .
                             CMS_DB_MEMORY_DESCRIPTION . ", " .
                             CMS_DB_MEMORY_CONTENT . ") " .
            "AGAINST ('$_value' IN BOOLEAN MODE) " .
            "HAVING `relevance`>0 " .
            "ORDER BY `relevance` DESC " .
            "LIMIT " . (int)$limit);
        if ($result === FALSE) return FALSE; //error

        //filter relevant results
        $array = [];
        $limit = NULL;

        while ($resultrow = mysql_fetch_assoc($result))
        {
            $relevance   = $resultrow["relevance"];
            $limit     ??= $relevance * $this->relevance_limit;
            if ($relevance < $limit) break;

            $array[] = $resultrow;
        };

        return $array;
    }

//--------------------------------------------------------------------------------------------------
//   MNEMONIC
//--------------------------------------------------------------------------------------------------

    function mnemonic_list($region,
                           $limit = 100)
    {
        if (! $this->is_reader($region))                           return FALSE; //no permission
        if (($table = $this->table_name($region, TRUE)) === FALSE) return FALSE; //invalid

        //retrieve mnemonics from region
        $result = mysql_query(
            "SELECT " .   CMS_DB_MEMORY_MNEM_VALUE . ", " .
                          CMS_DB_MEMORY_MNEM_COUNT . " " .
            "FROM `" .    sqlesc($table, TRUE) . "` " .
            "ORDER BY " . CMS_DB_MEMORY_MNEM_COUNT . " DESC, " .
                          CMS_DB_MEMORY_MNEM_VALUE . " ASC " .
            "LIMIT " .    (int)$limit);
        if ($result === FALSE) return FALSE; //error

        //collect data
        $array = [];
        while ($resultrow = mysql_fetch_row($result))
            $array[$resultrow[0]] = (int)$resultrow[1];

        return $this->mnemonic_sort($array);
    }

//--------------------------------------------------------------------------------------------------

    function mnemonic_count($region,
                            $list)
    {
        if (! $this->is_reader($region))                           return FALSE; //no permission
        if (($table = $this->table_name($region, TRUE)) === FALSE) return FALSE; //invalid

        if (! is_array($list)) $list = $this->mnemonic_tokenize($list);
        if (empty($list))      return []; //empty

        //retrieve count of specified mnemonics
        $_list  = implode("', '", sqlesc($list));
        $result = mysql_query(
            "SELECT " .   CMS_DB_MEMORY_MNEM_VALUE . ", " .
                          CMS_DB_MEMORY_MNEM_COUNT . " " .
            "FROM `" .    sqlesc($table, TRUE) . "` " .
            "WHERE " .    CMS_DB_MEMORY_MNEM_VALUE . " " .
            "IN ('" .     $_list . "')");
        if ($result === FALSE) return FALSE; //error

        //collect data
        $array = [];
        while ($resultrow = mysql_fetch_row($result))
            $array[$resultrow[0]] = (int)$resultrow[1];

        return $this->mnemonic_sort($array);
    }

//--------------------------------------------------------------------------------------------------

    private function mnemonic_sort($array)
    {
        //sort by descending key first, natural ascending value second
        uksort($array, function($a, $b) use ($array)
        {
            return ($array[$b] <=> $array[$a]) ?: strnatcmp($a, $b);
        });

        return $array;
    }

//--------------------------------------------------------------------------------------------------

    function mnemonic_tokenize($value)
    {
        //remove unnecessary space
        $value = stripspaces($value, TRUE, FALSE);
        if ($value === "") return [];

        //replace delimiters with break
        $value = preg_replace("/(?:(?:^|\s*)(?:,|\r?\n)(?:\s*|$))+/u", "\n", $value);
        if ($value === "\n") return [];

        //limit to 500 chars total
        $value = limitstr($value, 500, "\n");

        //lowercase
        $value = utf8_strtolower($value);

        //array
        $value = explode("\n", $value);

        //limit single mnemonic to 100 chars
        $value = array_map(function($v) { return first_words($v, 100); }, $value);

        //remove duplicates
        $value = array_unique($value);

        //sort
        natsort($value);

        return $value;
    }

//--------------------------------------------------------------------------------------------------

    private function mnemonic_index($region,
                                    $in,
                                    $out = [])
    {
        if (($table = $this->table_name($region, TRUE)) === FALSE) return FALSE; //invalid

        $table = sqlesc($table, TRUE);
        $add   = array_diff($in, $out);
        $del   = array_diff($out, $in);

        //increase mnemonic count
        if ($add)
            mysql_query(
                "INSERT INTO `$table` (" .   CMS_DB_MEMORY_MNEM_VALUE . ", " .
                                             CMS_DB_MEMORY_MNEM_COUNT . ") " .
                "VALUES ('" .                implode("', 1), ('", sqlesc($add)) . "', 1) " .
                "ON DUPLICATE KEY UPDATE " . CMS_DB_MEMORY_MNEM_COUNT .
                "=" .                        CMS_DB_MEMORY_MNEM_COUNT . "+1");

        //decrease mnemonic count
        if ($del)
            mysql_query(
                "UPDATE `$table` " .
                "SET " .   CMS_DB_MEMORY_MNEM_COUNT .
                "=" .      CMS_DB_MEMORY_MNEM_COUNT . "-1 " .
                "WHERE " . CMS_DB_MEMORY_MNEM_VALUE . " " .
                "IN ('" .  implode("', '", sqlesc($del)) . "') " .
                "AND " .   CMS_DB_MEMORY_MNEM_COUNT . ">0");

        //delete unused mnemonics
        if ($del)
            mysql_query(
                "DELETE FROM `$table` " .
                "WHERE " . CMS_DB_MEMORY_MNEM_COUNT . "<=0");

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function mnemonic_rebuild($region)
    {
        if (! $this->operator)                                      return FALSE; //no permission
        if (($table = $this->table_name($region)) === FALSE)        return FALSE; //invalid
        if (($_table = $this->table_name($region, TRUE)) === FALSE) return FALSE; //invalid

        //erase mnemonic index
        if (! mysql_query("TRUNCATE TABLE `" . sqlesc($_table, TRUE) . "`")) return FALSE;

        //retrieve region mnemonics
        $result = mysql_query(
            "SELECT " . CMS_DB_MEMORY_MNEMONIC . " " .
            "FROM `" .  sqlesc($table, TRUE) . "`");
        if ($result === FALSE) return FALSE; //error

        //collect mnemonics
        $array = [];
        while ($resultrow = mysql_fetch_row($result))
        {
            foreach ($this->mnemonic_tokenize($resultrow[0]) AS $value)
                $array[$value] = ($array[$value] ?? 0) + 1;
        };

        if ($array)
        {
            //prepare values
            $_array = [];
            foreach ($array AS $key => $value)
                $_array[] = "('" . sqlesc($key) . "', $value)";

            //write mnemonic index
            mysql_query(
                "INSERT INTO `" . sqlesc($_table, TRUE) . "` " .
                "(" .             CMS_DB_MEMORY_MNEM_VALUE . ", " .
                                  CMS_DB_MEMORY_MNEM_COUNT . ") " .
                "VALUES " .       implode(", ", $_array));
        };

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------
//   PERMISSION
//--------------------------------------------------------------------------------------------------

    private function has_permission($region,
                                    $permission)
    {
        if (($value = $this->parse_region($region)) === FALSE) return FALSE; //invalid
        return $this->operator || cms_permission(
            ($region === "user." . $this->user) ? "memory" : //user memory
            "memory." . $value["type"] . "." . $value["identifier"] . ".$permission",
            FALSE, FALSE, $this->user);
    }

//--------------------------------------------------------------------------------------------------

    function is_reader($region)
    {
        return $this->has_permission($region, CMS_MEMORY_PERMISSION_READER);
    }

//--------------------------------------------------------------------------------------------------

    function is_writer($region)
    {
        return $this->has_permission($region, CMS_MEMORY_PERMISSION_WRITER);
    }

//--------------------------------------------------------------------------------------------------

    function is_operator($region)
    {
        return $this->has_permission($region, CMS_MEMORY_PERMISSION_OPERATOR);
    }

//--------------------------------------------------------------------------------------------------
//   HELPER
//--------------------------------------------------------------------------------------------------

    private function parse_region($value)
    {
        if (! preg_match("/^(scope|user)\.(.+)$/s", (string)$value, $match)) return FALSE; //invalid
        if (! $this->verify_identifier($match[2]))                           return FALSE; //invalid

        return ["type" => $match[1], "identifier" => $match[2]];
    }

//--------------------------------------------------------------------------------------------------

    private function verify_identifier($value)
    {
        return preg_match("/^[-0-9a-z_]{1,40}$/", (string)$value);
    }

//--------------------------------------------------------------------------------------------------

    private function table_name($value,
                                $mnemonic = FALSE)
    {
        return ($value = $this->parse_region($value)) ?
               ($mnemonic ? CMS_DB_MEMORY_MNEM : CMS_DB_MEMORY) .
               $value["type"] . "_" . $value["identifier"] :
               FALSE; //invalid
    }

//--------------------------------------------------------------------------------------------------

    private function table_to_region($value)
    {
        if (! preg_match("/^" . CMS_DB_MEMORY . "(scope|user)_(.+)$/", $value, $match)) return FALSE; //invalid
        return $this->verify_identifier($match[2]) ? $match[1] . "." . $match[2] : FALSE;
    }

//--------------------------------------------------------------------------------------------------

    private function get_attachment_path($region,
                                         $id)
    {
        if (($region = $this->parse_region($region)) === FALSE) return FALSE; //invalid

        $id = (int)$id;
        return CMS_DATA_PATH . "#memory/" . $region["type"] . "/" . $region["identifier"] . "/" .
               intdiv($id, 1000000) . "/" . intdiv($id, 1000) . "/$id";
    }

//--------------------------------------------------------------------------------------------------

    private function verify_table($region)
    {
        $ngram = (streq($this->mysql->software, "MySQL") &&
                 version_compare($this->mysql->version, "5.7.6", ">=")) ?
                 " WITH PARSER ngram" : "";

        return $this->mysql->verify_table(
                   $this->table_name($region),
                   [CMS_DB_MEMORY_INDEX           => "INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT",
                    CMS_DB_MEMORY_MNEMONIC        => "VARCHAR(500) NOT NULL",
                    CMS_DB_MEMORY_DESCRIPTION     => "VARCHAR(500) NOT NULL",
                    CMS_DB_MEMORY_CONTENT         => "TEXT NOT NULL",
                    CMS_DB_MEMORY_ATTACHMENT_TYPE => "VARCHAR(100) NOT NULL DEFAULT ''",
                    CMS_DB_MEMORY_OWNER           => "VARCHAR(40) NOT NULL DEFAULT ''",
                    CMS_DB_MEMORY_CREATED         => "DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP",
                    CMS_DB_MEMORY_EDITOR          => "VARCHAR(40) NOT NULL DEFAULT ''",
                    CMS_DB_MEMORY_EDITED          => "DATETIME DEFAULT NULL",
                    CMS_DB_MEMORY_ACCESS_COUNT    => "INT UNSIGNED NOT NULL DEFAULT '0'",
                    CMS_DB_MEMORY_ACCESSED        => "DATETIME DEFAULT NULL",
                    CMS_DB_MEMORY_FOLLOWUP        => "DATETIME DEFAULT NULL",
                    CMS_DB_MEMORY_READ_ONLY       => "TINYINT(1) NOT NULL DEFAULT '0'",
                    CMS_DB_MEMORY_SAFE            => "TINYINT(1) NOT NULL DEFAULT '0'",
                    CMS_DB_MEMORY_RATING_VALUE    => "DECIMAL(3,2) UNSIGNED NOT NULL DEFAULT '3.00'",
                    CMS_DB_MEMORY_RATING_SUM      => "INT UNSIGNED NOT NULL DEFAULT '0'",
                    CMS_DB_MEMORY_RATING_COUNT    => "INT UNSIGNED NOT NULL DEFAULT '0'"],
                   ["INDEX (" . CMS_DB_MEMORY_OWNER . ")",
                    "FULLTEXT (" . CMS_DB_MEMORY_MNEMONIC . ")$ngram",
                    "FULLTEXT (" . CMS_DB_MEMORY_DESCRIPTION . ")$ngram",
                    "FULLTEXT (" . CMS_DB_MEMORY_CONTENT . ")$ngram",
                    "FULLTEXT (" . CMS_DB_MEMORY_MNEMONIC . ", " .
                                   CMS_DB_MEMORY_DESCRIPTION . ", " .
                                   CMS_DB_MEMORY_CONTENT . ")$ngram"])
               &&
               $this->mysql->verify_table(
                   $this->table_name($region, TRUE),
                   [CMS_DB_MEMORY_MNEM_VALUE => "VARCHAR(100) NOT NULL PRIMARY KEY",
                    CMS_DB_MEMORY_MNEM_COUNT => "INT UNSIGNED NOT NULL DEFAULT '0'"],
                   ["INDEX (" . CMS_DB_MEMORY_MNEM_COUNT . ")"]);
    }

//--------------------------------------------------------------------------------------------------

    private function get_record($table,
                                $id)
    {
        if ($table === FALSE) return FALSE; //invalid

        $result = mysql_query(
            "SELECT * " .
            "FROM `" . sqlesc($table, TRUE) . "` " .
            "WHERE " . CMS_DB_MEMORY_INDEX .
            "='" .     sqlesc($id) . "' " .
            "LIMIT 1");
        if ($result === FALSE) return FALSE; //error

        return mysql_fetch_assoc($result);
    }

//--------------------------------------------------------------------------------------------------

    function parse_date($value)
    {
        switch (gettype($value))
        {
        case "integer":

            if ($value < 0) return NULL; //invalid
            break;

        case "string":

            if (($value = strtotime($value)) === FALSE) return NULL; //invalid
            break;

        default:

            return NULL; //invalid
        };

        return gmdate("Y-m-d H:i:s", $value);
    }
};