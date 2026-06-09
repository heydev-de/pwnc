<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   MYSQL CLASS
//==================================================================================================

class mysql
{

//properties
//--------------------------------------------------------------------------------------------------

    public $database  = NULL;
    public $host      = NULL;
    public $user      = NULL;
    public $password  = NULL;
    public $software  = NULL;
    public $version   = NULL;
    public $engine    = "InnoDB";
    public $charset   = "utf8mb4";
    public $collation = "utf8mb4_unicode_ci";

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct()
    {
        $system         = new system();
        $this->database = $system->getval("mysql", "database");
        $this->host     = $system->getval("mysql", "host");
        $this->user     = $system->getval("mysql", "user");
        $this->password = $system->getval("mysql", "password");

        $this->connection();
    }

//methods
//--------------------------------------------------------------------------------------------------

    function connection()
    {
        static $init = [];

        //retrieve global connection
        $connection = mysql_get_link_identifier();

        //establish new connection if required
        if ($connection === NULL)
        {
            if (stre($this->database)) return FALSE;
            if (stre($this->host))     return FALSE;

            $connection = mysql_connect($this->host, $this->user, $this->password);
            if ($connection === FALSE) return FALSE;
        };

        //check initialization
        $id = $connection->thread_id;
        if (isset($init[$id])) return $connection;

        //retrieve engine and version
        $result = mysql_query("SELECT VERSION()", $connection);
        if ($result === FALSE) return FALSE;

        $value = mysql_result($result, 0);
        if (! preg_match("/^([.0-9]+)(?:-([.0-9]+))?(-MariaDB(?:\W|$))?/", $value, $_value))
            die("Database " . x($value) . " is not supported.");

        $flag           = ($_value[3] ?? FALSE);
        $this->software = $flag ? "MariaDB" : "MySQL";
        $this->version  = ($flag && ($_value[2] ?? FALSE)) ? $_value[2] : $_value[1];

        //check engine and version
        switch ($this->software)
        {
        default:
        case "MySQL":

            if (version_compare($this->version, "5.6", "<"))
                die("MySQL >= 5.6 is required.");
            break;

        case "MariaDB":

            if (version_compare($this->version, "10", "<"))
                die("MariaDB >= 10 is required.");
        };

        //initialize connection
        mysql_query("SET SESSION sql_mode=''");
        mysql_query("SET NAMES " . $this->charset . " COLLATE " . $this->collation);
        mysql_query("SET time_zone='+00:00'");
        $init[$id] = TRUE;

        //select database
        if (! mysql_select_db($this->database)) return FALSE;

        return $connection;
    }

//--------------------------------------------------------------------------------------------------

    function get($index,
                 $column,
                 $table,
                 $index_key = "id")
    {
        if ($this->connection() === FALSE) return FALSE;

        $result = mysql_query(
            "SELECT `" . sqlesc($column,    TRUE) . "` " .
            "FROM `" .   sqlesc($table,     TRUE) . "` " .
            "WHERE `" .  sqlesc($index_key, TRUE) . "`" .
            "='" .       sqlesc($index) . "' " .
            "LIMIT 1");

        return ($result === FALSE) ? FALSE : mysql_result($result, 0);
    }

//--------------------------------------------------------------------------------------------------

    function set($index,
                 $column,
                 $table,
                 $value,
                 $index_key = "id")
    {
        if ($this->connection() === FALSE) return FALSE;

        return mysql_query(
            "UPDATE `" . sqlesc($table,     TRUE) . "` " .
            "SET `" .    sqlesc($column,    TRUE) . "`" .
            "='" .       sqlesc($value) . "' " .
            "WHERE `" .  sqlesc($index_key, TRUE) . "`" .
            "='" .       sqlesc($index) . "' " .
            "LIMIT 1");
    }

//--------------------------------------------------------------------------------------------------

    function delete($index,
                    $table,
                    $index_key  = "id",
                    $parent_key = "container")
    {
        if ($this->connection() === FALSE) return FALSE;

        $index      = sqlesc($index);
        $_table     = sqlesc($table, TRUE);
        $_index_key = sqlesc($index_key, TRUE);

        $result = mysql_query(
            "SELECT `$_index_key` " .
            "FROM `$_table` " .
            "WHERE `" . sqlesc($parent_key, TRUE) . "`='$index'");

        if ($result === FALSE) return FALSE;

        while ($resultrow = mysql_fetch_row($result))
        {
            if (! $this->delete($resultrow[0],
                                $table,
                                $index_key,
                                $parent_key))
                return FALSE;
        };

        return mysql_query(
            "DELETE FROM `$_table` " .
            "WHERE `$_index_key`='$index' " .
            "LIMIT 1");
    }

//--------------------------------------------------------------------------------------------------

    function is_child($index,
                      $parent,
                      $table,
                      $index_key  = "id",
                      $parent_key = "container")
    {
        if ($this->connection() === FALSE) return FALSE;

        do
        {
            $result = mysql_query(
                "SELECT `" . sqlesc($parent_key, TRUE) . "` " .
                "FROM `" .   sqlesc($table,      TRUE) . "` " .
                "WHERE `" .  sqlesc($index_key,  TRUE) . "`" .
                "='" .       sqlesc($index) . "' " .
                "LIMIT 1");
            if ($result === FALSE) return FALSE;

            $index = mysql_result($result, 0, $parent_key);
            if (streq($index, $parent)) return TRUE;
        }
        while ($index !== FALSE);

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function verify_table($table,
                          $column,
                          $index   = NULL,
                          $mapping = NULL)
    {
        static $table_list = NULL; //list of all tables

        //return if not connected
        if ($this->connection() === FALSE) return FALSE;

        //retrieve table list
        if ($table_list === NULL)
        {
            if (($result = mysql_query("SHOW TABLES")) === FALSE) return FALSE;
            while ($resultrow = mysql_fetch_row($result))
                $table_list[$resultrow[0]] = TRUE;
        };

        if (! is_array($index))   $index   = [];
        if (! is_array($mapping)) $mapping = [];

        //compute schema hash
        $schema_hash = hash32(serialize([$column, $index, $mapping, $this->engine, $this->collation]));

        //escape table name
        $table_src = sqlesc($table, TRUE);

        //create table if it does not exist
        if (! isset($table_list[$table]))
        {
            //build and execute create table statement
            $buffer = [];
            foreach ($column AS $key => $value)
                $buffer[] = "`" . sqlesc($key, TRUE) . "` $value";
            $buffer = array_merge($buffer, $index);
            if (! mysql_query("CREATE TABLE `$table_src` (" . implode(", ", $buffer) . ") " .
                              "ENGINE=" . $this->engine . " " .
                              "DEFAULT CHARACTER SET " . $this->charset . " COLLATE " . $this->collation))
                return FALSE;

            //add created table to list
            $table_list[$table] = TRUE;

            //store schema hash
            cms_cache("mysql.schema_hash.$table", $schema_hash, TRUE);
            return TRUE; //done
        };

        //skip processing if target schema has not changed
        $_schema_hash = cms_cache("mysql.schema_hash.$table");
        if ($schema_hash === $_schema_hash) return TRUE;

        set_time_limit(3600); //1 hour

        //retrieve source table information
        if (($result = mysql_query("SHOW TABLE STATUS LIKE '" . sqlesc($table) . "'")) === FALSE) return FALSE;
        $resultrow = mysql_fetch_assoc($result);

        $buffer = [];

        //change engine
        if ($resultrow["Engine"] !== $this->engine)
            $buffer[] = "ENGINE=" . $this->engine;

        //change character set
        if ($resultrow["Collation"] !== $this->collation)
            $buffer[] = "CONVERT TO CHARACTER SET " . $this->charset . " " .
                        "COLLATE " . $this->collation;

        //apply changes if applicable
        if ($buffer && (! mysql_query("ALTER TABLE `$table_src` " . implode(", ", $buffer))))
            return FALSE;

        //create target table to simplify structural comparison
        $table_tgt  = "temp_" . unique_id();
        $_table_tgt = sqlesc($table_tgt, TRUE);
        $buffer     = [];
        foreach ($column AS $key => $value)
            $buffer[] = "`" . sqlesc($key, TRUE) . "` $value";
        $buffer     = array_merge($buffer, $index);
        if (! mysql_query("CREATE TABLE `$_table_tgt` (" . implode(", ", $buffer) . ") " .
                          "ENGINE=" . $this->engine . " " .
                          "DEFAULT CHARACTER SET " . $this->charset . " COLLATE " . $this->collation))
            return FALSE;

        //delete target table on script termination
        register_shutdown_function([$this, "drop_table"], $table_tgt);

        //negative regex lookahead for protected areas
        $regex_protected =
            "(?!" .
            "'(?:[^']|\\')*'|" . //strings
            "`(?:[^`]|``)*`|" .  //denominators
            "\/\*.*?\*\/|" .     //block comments
            "--[^\n]*)";         //single line comments

        //regular expressions
        $regex_auto_increment    = "/$regex_protected(?:^|\s)auto_increment(?:,|$)/s";
        $regex_unique_primary    = "/$regex_protected\s+(?:UNIQUE|PRIMARY(?:\s+KEY)?)(?=\s|$)/is";
        $regex_ai_unique_primary = "/$regex_protected\s+(?:AUTO_INCREMENT|UNIQUE|PRIMARY(?:\s+KEY)?)(?=\s|$)/is";

        //retrieve column settings of source table
        if (! $result = mysql_query("SHOW COLUMNS FROM `$table_src`")) return FALSE;

        $column_src    = [];   //source column settings
        $ai_column_src = NULL; //auto increment source column

        while ($resultrow = mysql_fetch_assoc($result))
        {
            $key = array_shift($resultrow); //column name
            $column_src[$key] = $resultrow;

            //store auto increment source column
            if (preg_match($regex_auto_increment, $resultrow["Extra"]))
                $ai_column_src = $key;
        };

        //retrieve index definitions of target and source table
        $index_keys = []; //index keys by column
        $index_cols = []; //columns by index key

        for ($i = 0; $i <= 1; $i++) //two runs: target and source table
        {
            //collect indices
            if (! $result = mysql_query("SHOW INDEX FROM `" . (($i === 0) ? $_table_tgt : $table_src) ."`")) return FALSE;

            $index_src = []; //indices

            while ($resultrow = mysql_fetch_assoc($result))
            {
                $key         = $resultrow["Key_name"];
                $column_name = $resultrow["Column_name"];
                $sub_part    = nstre($resultrow["Sub_part"]) ? "(" . $resultrow["Sub_part"] . ")" : "";
                $index_src[$key]["Column_name"][$resultrow["Seq_in_index"]] = "`" . sqlesc($column_name, TRUE) . "`$sub_part";
                $index_src[$key]["Non_unique"]    = $resultrow["Non_unique"];
                $index_src[$key]["Index_type"]    = $resultrow["Index_type"];
                $index_src[$key]["Index_comment"] = $resultrow["Index_comment"];
                if ($i === 1)
                {
                    $index_keys[$column_name][] = $key;
                    $index_cols[$key][]         = $column_name;
                };
            };

            //process indices
            foreach ($index_src AS $key => $value)
            {
                //sort columns and convert to string
                ksort($index_src[$key]["Column_name"]);
                $index_src[$key]["Column_name"] = implode(", ", $index_src[$key]["Column_name"]);
            };

            //save target indices
            if ($i === 0) $index_tgt = $index_src;
        };

        //default values used to replace null values
        /*
        $replace_null = [];
        */

        //statement buffers
        $buffer_del_ai      = [];
        $buffer_del_index   = [];
        $buffer_mod_column  = [];
        $buffer_add_column  = [];
        $buffer_add_index   = [];
        $buffer_add_ai      = [];

        //auto increment
        $ai_column = NULL;  //target column
        $ai_legacy = NULL;  //legacy column
        $ai_remove = FALSE; //remove flag

        //retrieve column definitions of target table and compare to source
        if (! $result = mysql_query("SHOW COLUMNS FROM `$_table_tgt`")) return FALSE;

        while ($resultrow = mysql_fetch_assoc($result))
        {
            $key = $_key = array_shift($resultrow); //column name

            //store target auto increment column
            if (
                 ($ai_column === NULL) //not yet found
                 &&
                 preg_match($regex_auto_increment, $resultrow["Extra"])
               )
            {
                $ai_column = $key; //store column

                //set auto increment to be removed from source column
                $ai_remove = ($ai_column_src !== NULL) && ($ai_column_src !== $key);
            };

            //target column does not already exist
            $temp = NULL;

            if (! $flag = isset($column_src[$key]))
            {
                //find first existing mapped source column
                foreach (array_keys($mapping, $_key) AS $temp)
                {
                    if (isset($column_src[$temp])) break; //found
                };
            };

            //rename mapped source column if applicable
            if ($temp !== NULL)
            {
                //rename and modify source column
                $key  = $temp;
                $temp = preg_replace($regex_ai_unique_primary, "", $column[$_key]);
                $buffer_mod_column[] = "CHANGE COLUMN `" . sqlesc($key, TRUE) . "` `" . sqlesc($_key, TRUE) . "` $temp";

                //renamed column is defined and not a mapping target
                if (isset($column[$key]) && (! in_array($key, $mapping)))
                {
                    //add column to source table
                    $temp = preg_replace($regex_ai_unique_primary, "", $column[$key]);
                    $buffer_add_column[] = "ADD COLUMN `" . sqlesc($key, TRUE) ."` $temp";
                };
            }

            //column does not exist in source table and is no mapping target
            elseif ((! $flag) && (! in_array($key, $mapping)))
            {
                //add column to source table
                $temp = preg_replace($regex_ai_unique_primary, "", $column[$key]);
                $buffer_add_column[] = "ADD COLUMN `" . sqlesc($key, TRUE) ."` $temp";
                continue;
            }

            //column exists in source table and definition differs
            elseif ($flag && ($resultrow["Type"] !== $column_src[$key]))
            {
                //modify source column
                $temp = preg_replace($regex_ai_unique_primary, "", $column[$key]);
                $buffer_mod_column[] = "MODIFY COLUMN `" . sqlesc($key, TRUE) . "` $temp";
            }

            //no change
            else
            {
                continue; //skippediskip
            };

            //collect source columns requiring null value replacement
            /*
            if (

            //target column does not allow null value
                 ($resultrow["Null"] === "NO")
                 &&

            //source column allows null value
                 ($column_src[$key]["Null"] === "YES")
               )
                //store data type of source column
                $replace_null[$key] = strtok($column_src[$key]["Type"], " (");
            */

            //remove source indices to avoid modification conflicts
            if (isset($index_keys[$key]))
            {
                //traverse indices of source column
                foreach ($index_keys[$key] AS $value)
                {
                    if (! isset($index_src[$value])) continue; //index already removed

                    //remove conflicting auto increment
                    if ((! $ai_remove) && ($ai_column_src !== NULL))
                    {
                        //traverse columns of index
                        foreach ($index_cols[$value] AS $_value)
                        {
                            //test for auto increment
                            if ($_value !== $ai_column_src) continue;

                            //set auto increment to be removed from source column
                            $ai_remove = TRUE;
                            break;
                        };
                    };

                    //remove source index
                    $buffer_del_index[] = "DROP INDEX `" . sqlesc($value, TRUE) . "`";

                    //remove from source index list
                    //to prevent re-removal and trigger
                    //re-creation if defined
                    unset($index_src[$value]);
                };
            };
        };

        //compare source with target indices
        foreach ($index_tgt AS $key => $value)
        {
            //target index also exists in source table
            if (isset($index_src[$key]))
            {
                //skip if indices are identical
                if ($value === $index_src[$key]) continue;

                //remove conflicting auto increment
                if ((! $ai_remove) && ($ai_column_src !== NULL))
                {
                    //traverse columns of index
                    foreach ($index_cols[$key] AS $_value)
                    {
                        //test for auto increment
                        if ($_value !== $ai_column_src) continue;

                        //set auto increment to be removed from source column
                        $ai_remove = TRUE;
                        break;
                    };
                };

                //remove differing source index
                $buffer_del_index[] = "DROP INDEX `" . sqlesc($key, TRUE) . "`";
            };

            //add target index to source table
            if ($key === "PRIMARY")
            {
                $temp = "PRIMARY KEY";
            }
            else
            {
                switch ($value["Index_type"])
                {
                case "FULLTEXT":
                case "SPATIAL":

                    $temp = $value["Index_type"] . " INDEX";
                    break;

                default:

                    $temp = (($value["Non_unique"] === "0") ? "UNIQUE" : "INDEX");
                };

                $temp .= " `" . sqlesc($key, TRUE) . "`";
            };

            $buffer_add_index[] = "ADD $temp (" . $value["Column_name"] . ") " .
                                  "COMMENT '" . sqlesc($value["Index_comment"]) . "'";
        };

        //remove auto increment
        if ($ai_remove)
        {
            //extract column definitions of source table
            if (! $result = mysql_query("SHOW CREATE TABLE `$table_src`")) return FALSE;

            $array = mysql_result($result, 0, "Create Table");
            $array = explode("\n", $array);
            $array = array_slice($array, 1, -1);

            //find auto increment column definition
            $_ai_column_src = preg_quote(sqlesc($ai_column_src, TRUE), "/");

            foreach ($array AS $value)
            {
                //skip columns without auto increment
                if (! preg_match("/^\s*(`$_ai_column_src`\s.*?),?$/", $value, $value)) continue;

                //remove auto increment
                $temp = preg_replace($regex_ai_unique_primary, "", $value[1]);
                $buffer_del_ai[] = "MODIFY COLUMN $temp";

                //store legacy auto increment column definition
                if (isset($column[$ai_column_src])) $ai_legacy = $value[1];
                break;
            };
        };

        //set target auto increment in source table if applicable
        if ($ai_column !== NULL)
        {
            $temp = preg_replace($regex_unique_primary, "", $column[$ai_column]);
            $buffer_add_ai[] = "MODIFY COLUMN `" . sqlesc($ai_column, TRUE) . "` $temp";
        }

        //otherwise restore legacy auto increment
        elseif ($ai_legacy !== NULL)
        {
            $temp = preg_replace($regex_unique_primary, "", $ai_legacy);
            $buffer_add_index[] = "ADD UNIQUE `index_" . sqlesc(unique_id(), TRUE) . "` " .
                                  "(`" . sqlesc($ai_column_src, TRUE) . "`)";
            $buffer_add_ai[]    = "MODIFY COLUMN $temp";
        };

        //replace null values
        /*
        foreach ($replace_null AS $key => $value)
        {
            switch ($value)
            {
            default:

                $value = "''";
                break;

            case "bit":
            case "tinyint":
            case "smallint":
            case "mediumint":
            case "int":
            case "bigint":
            case "float":
            case "double":
            case "decimal":

                $value = "0";
                break;

            case "date":

                $value = "'0000-00-00'";
                break;

            case "datetime":
            case "timestamp":

                $value = "'0000-00-00 00:00:00'";
                break;

            case "time":

                $value = "'00:00:00'";
                break;

            case "year":

                $value = "'0000'";
                break;
            };

            $key = sqlesc($key, TRUE);
            if (! mysql_query("UPDATE `$table_src` " .
                              "SET `$key`=$value " .
                              "WHERE `$key` IS NULL"))
                return FALSE;
        };
        */

        //set statement order
        $buffer = array_merge($buffer_del_ai,
                              $buffer_del_index,
                              $buffer_mod_column,
                              $buffer_add_column,
                              $buffer_add_index,
                              $buffer_add_ai);

        //execute statements
        foreach ($buffer AS $value)
            if (! mysql_query("ALTER TABLE `$table_src` $value")) return FALSE;

        //store schema hash
        cms_cache("mysql.schema_hash.$table", $schema_hash, TRUE);
        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function export_sql($table,
                        $file = NULL)
    {
        if ($this->connection() === FALSE) return FALSE;

        if ($file === NULL) $file = CMS_DATA_PATH . "#database/$table";

        $path = dirname($file);

        $result = mysql_query("SHOW CREATE TABLE `" . sqlesc($table, TRUE) . "`");
        if ($result === FALSE) return FALSE;

        if (! mkpath($path))                               return FALSE;
        if (($hfile = fopen("$file.sql", "wb")) === FALSE) return FALSE;

        flock($hfile, LOCK_EX);
        fwrite($hfile, mysql_result($result, 0, 1));
        fclose($hfile);

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function export_csv($table,
                        $separator  = ",",
                        $delimiter  = "\"",
                        $file       = NULL,
                        $set_fields = FALSE)
    {
        if ($this->connection() === FALSE) return FALSE;

        if ($file === NULL) $file = CMS_DATA_PATH . "#database/$table";

        set_time_limit(3600); //1 hour

        $path       = dirname($file);
        $table      = sqlesc($table, TRUE);
        $_delimiter = str_repeat($delimiter, 2);

        $result = mysql_query("SHOW COLUMNS FROM `$table`");
        if ($result === FALSE) return FALSE;

        $array = [];
        while ($resultrow = mysql_fetch_assoc($result))
            $array[]  = $resultrow["Field"];

        $_separator   = sqlesc($separator);
        $__delimiter  = sqlesc($delimiter);
        $___delimiter = sqlesc($_delimiter);

        $result = mysql_unbuffered_query(
            "SELECT CONCAT(" .
            "CONCAT_WS('$_separator', " .
            "CONCAT('$__delimiter', " .
            "REPLACE(" .
            "IFNULL(" .
            "CAST(`" .
            implode("` AS CHAR CHARACTER SET utf8mb4) COLLATE utf8mb4_unicode_ci, " .
                    "''), " .
                    "'$__delimiter', '$___delimiter'), '$__delimiter'), " .
                    "CONCAT('$__delimiter', " .
                    "REPLACE(" .
                    "IFNULL(" .
                    "CAST(`",
                    sqlesc($array, TRUE)) .
            "` AS CHAR CHARACTER SET utf8mb4) COLLATE utf8mb4_unicode_ci, " .
            "''), " .
            "'$__delimiter', '$___delimiter'), '$__delimiter')), " .
            "'\n') " .
            "FROM `$table`");
        if ($result === FALSE)                             return FALSE;
        if (! mkpath($path))                               return FALSE;
        if (($hfile = fopen("$file.csv", "wb")) === FALSE) return FALSE;

        flock($hfile, LOCK_EX);

        if ($set_fields)
        {
            $array = str_replace($delimiter, $_delimiter, $array);
            fwrite($hfile, $delimiter . implode($delimiter . $separator . $delimiter, $array) . $delimiter . "\n");
        };

        while ($resultrow = mysql_fetch_row($result))
            fwrite($hfile, $resultrow[0]);

        fclose($hfile);

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function export_excel($table,
                          $file       = NULL,
                          $set_fields = FALSE)
    {
        if ($this->connection() === FALSE)  return FALSE;
        if (! extension_loaded("mbstring")) return FALSE;

        if ($file === NULL) $file = CMS_DATA_PATH . "#database/$table";

        if (! $this->export_csv($table, "\t", "\"", "$file.temp", $set_fields)) return FALSE;

        //convert to utf-16le
        $success = FALSE;
        if ($hfile_source = fopen("$file.temp.csv", "rb"))
        {
            if ($hfile_target = fopen("$file.xls", "wb"))
            {
                flock($hfile_source, LOCK_SH);
                flock($hfile_target, LOCK_EX);

                fwrite($hfile_target, "\xFF\xFE"); //utf-16le byte order mark

                //convert using stream filter
                stream_filter_append($hfile_source,
                                     //"convert.mbstring.encoding.UTF-8:UTF-16LE"
                                     "convert.iconv.UTF-8/UTF-16LE");
                $success = stream_copy_to_stream($hfile_source, $hfile_target) !== FALSE;

                fclose($hfile_target);
            };

            fclose($hfile_source);
        };

        unlink("$file.temp.csv");

        return $success;
    }

//--------------------------------------------------------------------------------------------------

    function export_html($table,
                         $file       = NULL,
                         $set_fields = FALSE)
    {
        if ($this->connection() === FALSE) return FALSE;

        if ($file === NULL) $file = CMS_DATA_PATH . "#database/$table";

        $path  = dirname($file);
        $table = sqlesc($table, TRUE);

        $result = mysql_query("SHOW COLUMNS FROM `$table`");
        if ($result === FALSE) return FALSE;

        $array = [];
        while ($resultrow = mysql_fetch_assoc($result))
            $array[] = $resultrow["Field"];

        $result = mysql_unbuffered_query(
            "SELECT CAST(`" .
            implode("` AS CHAR CHARACTER SET utf8mb4) COLLATE utf8mb4_unicode_ci, " .
                    "CAST(`",
                    sqlesc($array, TRUE)) .
            "` AS CHAR CHARACTER SET utf8mb4) COLLATE utf8mb4_unicode_ci " .
            "FROM `$table`");
        if ($result === FALSE)                             return FALSE;
        if (! mkpath($path))                               return FALSE;
        if (($hfile = fopen("$file.htm", "wb")) === FALSE) return FALSE;

        flock($hfile, LOCK_EX);
        fwrite($hfile,
               "<!DOCTYPE HTML>" .
               "<html>" .
               "<head>" .
               "<meta charset=\"utf-8\">" .
               "<title>" . x($table) . "</title>" .
               "<meta name=\"generator\" " .
                     "content=\"" . x(CMS) . "\">" .
               "</head>" .
               "<body>" .
               "<table>");

        if ($set_fields)
        {
            $array = array_map(__NAMESPACE__ . "\\x",     $array);
            $array = array_map(__NAMESPACE__ . "\\nl2br", $array);
            fwrite($hfile,
                   "<thead>" .
                   "<tr>" .
                   "<th>" .
                   implode("</th><th>", $array) .
                   "</th>" .
                   "</tr>" .
                   "</thead>");
        };

        fwrite($hfile,
               "<tbody>");

        while ($resultrow = mysql_fetch_row($result))
        {
            $resultrow = array_map(__NAMESPACE__ . "\\x",     $resultrow);
            $resultrow = array_map(__NAMESPACE__ . "\\nl2br", $resultrow);
            fwrite($hfile,
                   "<tr>" .
                   "<td>" .
                   implode("</td><td>", $resultrow) .
                   "</td>" .
                   "</tr>");
        };

        fwrite($hfile,
               "</tbody>" .
               "</table>" .
               "</body>" .
               "</html>");

        fclose($hfile);

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function import_csv($file,
                        $separator        = ",",
                        $delimiter        = "\"",
                        $table            = NULL,
                        $get_fields       = FALSE,
                        $ignore_first_row = FALSE,
                        $ignore_existing  = TRUE,
                        $mapping          = NULL)
    {
        if ($this->connection() === FALSE) return FALSE;

        //build statement
        $table     = sqlesc($table, TRUE);
        $statement = $ignore_existing ?
                     "INSERT IGNORE INTO `$table` " :
                     "REPLACE `$table` ";

        //process mapping
        if ((! $get_fields) && is_array($mapping))
        {
            $_mapping = [];

            foreach ($mapping AS $key => $value)
            {
                if (! is_int($key))      continue; //no column number
                if (! is_string($value)) continue; //no field name
                if ($value === "")       continue; //no field assigned

                $_mapping[$key] = sqlesc($value, TRUE);
            };

            if (count($_mapping) === 0) return TRUE; //no assignments

            $statement .= "(`" . implode("`, `", $_mapping) . "`) ";
        };

        $statement .= "VALUES ";

        //open source file
        if (($hfile = fopen($file, "rb")) === FALSE) return FALSE;
        flock($hfile, LOCK_SH);

        //parse csv

        /*
            parsing bufferend chunks with a 2 byte window should be faster than fgetcsv
            especially for larger files because we avoid the function overhead
        */

        $chunk_size = 65536;
        $buffer     = fread($hfile, $chunk_size); //read first chunk

        if (stre($buffer)) //empty file
        {
            fclose($hfile);
            return $get_fields ? [] : TRUE;
        };

        $value   = [];
        $_value  = [];
        $return  = FALSE;
        $_buffer = [];
        $char    = $buffer[0];
        $c       = strlen($buffer);
        $_c      = $c - 2;
        $i       = 0;
        $_i      = 0;
        $__i     = 0;
        $flag    = FALSE;

        while (TRUE)
        {
            set_time_limit(600); //10 minutes

            if (++$i >= $_c) //2 bytes or less left
            {
                //keep 2 bytes and append next chunk
                $buffer = (($i   < $c) ? $buffer[$i] : "") .
                          ((++$i < $c) ? $buffer[$i] : "") .
                          fread($hfile, $chunk_size);
                $c      = strlen($buffer);
                $_c     = $c - 2;
                $i      = 0;
            };

            //lookahead
            $_char = ($i < $c) ? $buffer[$i] : FALSE;

            //within delimiter
            if ($flag)
            {
                //delimiter
                if ($char === $delimiter)
                {
                    //escaped delimiter
                    if ($_char === $delimiter)
                    {
                        $value[] = $char;

                        //overwrite lookahead
                        $_char = (++$i < $c) ? $buffer[$i] : FALSE;
                    }

                    //closing delimiter
                    else
                    {
                        $flag = FALSE;
                    };
                }

                //content
                else
                {
                    $value[] = $char;
                };
            }

            //opening delimiter
            elseif ($char === $delimiter)
            {
                $flag = TRUE;
            }

            //separator
            elseif ($char === $separator)
            {
                //set and advance field in record
                $_value[$_i++] = $get_fields ?
                                 implode("", $value) :
                                 sqlesc(implode("", $value));
                $value         = [];
            }

            //end of record
            elseif (($char === "\n") || ($char === FALSE))
            {
                //only return fields of first row
                if ($get_fields)
                {
                    $_value[$_i] = implode("", $value);
                    fclose($hfile);
                    return $_value;
                };

                $_value[$_i] = sqlesc(implode("", $value));

                //skip first row
                if ($ignore_first_row)
                {
                    $ignore_first_row = FALSE;
                }

                //insert record by mapping
                elseif (isset($_mapping))
                {
                    $query = "";
                    foreach ($_mapping AS $key => $_)
                        $query .= isset($_value[$key]) ?
                                  "_binary'" . $_value[$key] . "', " :
                                  "'', ";
                    $_buffer[] = "(" . substr($query, 0, -2) . "), ";
                    $__i++;
                }

                //insert record sequentially
                else
                {
                    $_buffer[] = "(_binary'" . implode("', _binary'", $_value) . "'), ";
                    $__i++;
                };

                //execute buffered query
                if (($__i === 100) || (($_char === FALSE) && ($__i > 0)))
                {
                    $return  |= ! mysql_query($statement . substr(implode("", $_buffer), 0, -2));
                    $_buffer  = [];
                    $__i      = 0;
                };

                //end of file => exit loop
                if ($_char === FALSE) break;

                //reset field in row
                $value  = [];
                $_value = [];
                $_i     = 0;
            }

            //content
            elseif (($char !== "\r") || ($_char !== "\n"))
            {
                $value[] = $char;
            };

            //shift lookahead
            $char = $_char;

            //end of file
            if ($char === FALSE) $flag = FALSE;
        };

        fclose($hfile);
        return ! $return;
    }

//--------------------------------------------------------------------------------------------------

    function backup()
    {
        if ($this->connection() === FALSE) return FALSE;

        //get tables
        $result = mysql_query("SHOW TABLES");
        if ($result === FALSE) return FALSE;

        $path   = CMS_DATA_PATH . "#database/backup/";
        $return = FALSE;

        while ($resultrow = mysql_fetch_row($result))
        {
            $table = $resultrow[0];

            //export definition and content of table that is no backup
            if (nstreq(substr($table, 0, 8), "#backup_"))
            {
                $return |= ! $this->export_sql($table, $path . $table);
                $return |= ! $this->export_csv($table, ",", "\"", $path . $table, TRUE);
            };
        };

        return ! $return;
    }

//--------------------------------------------------------------------------------------------------

    function restore()
    {
        if ($this->connection() === FALSE) return FALSE;

        $path = CMS_DATA_PATH . "#database/backup";
        if (! is_dir($path))                    return FALSE;
        if (($hdir = opendir($path)) === FALSE) return FALSE;

        $return = FALSE;

        while ($file_object = readdir($hdir))
        {
            //get definition file
            if (! is_file("$path/$file_object"))                             continue;
            if (! preg_match("/^(.*)\.sql$/i", $file_object, $_file_object)) continue;

            $query = read_file("$path/$file_object");
            if ($query !== FALSE)
            {
                $table  = $_file_object[1]; //assuming filename is not changed
                $_table = sqlesc($table, TRUE);

                //create backup of existing table
                if ($flag = (bool)mysql_num_rows(mysql_query("SHOW TABLES LIKE '" . sqlesc($table) . "'")))
                {
                    $backup = sqlesc("#backup_" . $table . "_" . date("Y-m-d_H-i-s") . "_" . unique_id(), TRUE);
                    mysql_query("RENAME TABLE `$_table` TO `$backup`");
                };

                //execute, create table
                if (mysql_query($query))
                {
                    $_file_object = "$path/$table.csv";

                    //restore data
                    if (is_file($_file_object))
                        $return |= ! $this->import_csv(
                            $_file_object, //file
                            ",",           //separator
                            "\"",          //delimiter
                            $table,        //table
                            FALSE,         //get fields
                            TRUE,          //ignore first row
                            FALSE);        //ignore existing
                }

                //failed, restore backup
                elseif ($flag)
                {
                    mysql_query("RENAME TABLE `$backup` TO `$_table`");
                    $return = TRUE;
                };
            }

            //no file contents
            else
            {
                $return = TRUE;
            };
        };

        closedir($hdir);

        return ! $return;
    }

//--------------------------------------------------------------------------------------------------

    function drop_table($table)
    {
        return mysql_query("DROP TABLE `" . sqlesc($table, TRUE) . "`");
    }
};