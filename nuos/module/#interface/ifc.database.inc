<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   INTERFACE
//==================================================================================================

//permission
ifc_permission(["" => CMS_L_ACCESS]);

//establish database connection
$mysql = new mysql();

cms_cache_init($object, "database." . CMS_USER . ".object");
if (! isset($list)) $list = preg_match("/^(?:|ifc_)/", CMS_IFC_MESSAGE) ? [$object] : [];

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "select":
//..................................................................................................

    $object = $ifc_param;
    break;

case "sql_console":
//..................................................................................................

    //retrieve last command from permanent cache
    if (! isset($ifc_param1)) $ifc_param1 = cms_cache("database." . CMS_USER . ".code");

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        NULL,
        ["object" => $object],
        "sql_console",
        CMS_L_IFC_DATABASE_051);

    echo("<div>");

    //command
    $ifc->set(CMS_L_IFC_DATABASE_052,
              "textarea 0x4 w",
              $ifc_param1);

    //execute flag
    $ifc->param("execute", TRUE);

    //execute button
    echo("<div style=\"TEXT-ALIGN:right\">");
    $ifc->set(CMS_L_IFC_DATABASE_081,
              "button",
              "sql_console");
    echo("</div>" .
         "</div>");

    if (isset($execute))
    {
        //cache last command permanently
        cms_cache("database." . CMS_USER . ".code", $ifc_param1, TRUE);

        echo("<div>");
        if (! log_report($ifc_param1, "table"))
            $ifc->set(CMS_MSG_ERROR . x(mysql_error()), "info");
        echo("</div>");
    };

    $ifc->close();

//table editor
case "edit_table":
case "edit_table_add":
case "edit_table_update":
case "edit_table_delete":
//..................................................................................................

    //retrieve table structure
    $_object = sqlesc($object, TRUE);
    $result  = mysql_query("SHOW COLUMNS FROM `$_object`");
    if ($result === FALSE) ifc_close_external(); //invalid table

    $fieldname = [];
    $fieldtype = [];
    $index     = NULL;
    $_row      = NULL;

    while ($resultrow = mysql_fetch_assoc($result))
    {
        $value             = $resultrow["Field"];
        $fieldname[$value] = $value;
        $fieldtype[$value] = $resultrow["Type"];
        if (streq($resultrow["Key"], "PRI")) $index = $value;
    };

    //use primary key or row number as index
    $_index = ($primary = isset($index)) ?
              "`" . sqlesc($index, TRUE) . "`" :
              "(@id:=@id+1)";

    switch (CMS_IFC_MESSAGE)
    {
    case "edit_table_add":
//..................................................................................................

        //add new record
        $query = "INSERT `$_object` VALUES ()";

        if (mysql_query($query) !== FALSE)
        {
            $ifc_response = CMS_MSG_DONE;
            $row          = $primary ? mysql_insert_id() :
                            ((($result = mysql_query("SELECT COUNT(*) FROM `$_object`")) !== FALSE) ?
                            mysql_fetch_row($result)[0] : NULL);
            $field        = NULL;
            break;
        };

        $ifc_response = CMS_MSG_ERROR . x(mysql_error());
        break;

    case "edit_table_update":
//..................................................................................................

        init($ifc_param7); //value
        if (is_array($ifc_param7)) $ifc_param7 = implode(",", $ifc_param7);

        $_ifc_param7 = preg_match("/^bit\(([1-9][0-9]*)\)$/", $fieldtype[$field], $bracket) ?
                       "b'" . sqlesc(substr($ifc_param7, 0, $bracket[1])) . "'" :
                       "'" . sqlesc($ifc_param7) . "'";

        //update value
        if (! $primary) mysql_query("SET @id:=0");
        $query = "UPDATE `" . $_object . "` " .
                 "SET `" .    sqlesc($field, TRUE) . "`" .
                 "=" .        $_ifc_param7 . " " .
                 "WHERE " .   $_index .
                 "='" .       sqlesc($row) . "' " .
                 "LIMIT 1";

        if (mysql_query($query) !== FALSE)
        {
            if ($primary && streq($field, $index)) $row = $ifc_param7;
            $ifc_response = CMS_MSG_DONE;
            $field        = NULL;
            break;
        };

        $ifc_response = CMS_MSG_ERROR . x(mysql_error());
        break;

    case "edit_table_delete":
//..................................................................................................

        if (count($list) === 0) break; //no selection

        //delete selected records
        if (! $primary) mysql_query("SET @id:=0");
        $query = "DELETE FROM `" . $_object . "` " .
                 "WHERE " .        $_index . " " .
                 "IN ('" .         implode("', '", sqlesc($list)) . "')";

        if (mysql_query($query) !== FALSE)
        {
            $ifc_response = CMS_MSG_DONE;
            $row          = NULL;
            $field        = NULL;
            break;
        };

        $ifc_response = CMS_MSG_ERROR . x(mysql_error());
        break;
    };

    //selection
    init($row);
    init($field);

    //filtering
    init($ifc_param1); //field
    init($ifc_param2); //operator
    init($ifc_param3); //value

    $filter = stre($ifc_param3) ? "" :
              " WHERE `" . sqlesc($ifc_param1, TRUE) . "`$ifc_param2" .
              (preg_match("/^( LIKE | NOT LIKE )$/", $ifc_param2) ?
              "'%" . sqlesc($ifc_param3) . "%'" :
              "'" .  sqlesc($ifc_param3) . "'");

    //page offset, reset on filter change
    $_hash  = hash32($filter);
    $offset = streq($_hash, init($hash)) ? max((int)$offset, 0) : 0;

    //rows per page
    $ifc_param4 = max((int)init($ifc_param4, 25), 10);

    //number of rows passing filter
    if (! $primary) mysql_query("SET @id:=0");
    $query  = "SELECT 1 FROM `$_object`$filter";
    $result = mysql_query($query);
    $count  = ($result !== FALSE) ? mysql_num_rows($result) : 0;
    $_count = (int)ceil($count / $ifc_param4);
    $page   = (int)floor((int)$offset / $ifc_param4);
    if ($page >= $_count) $page = max($_count - 1, 0);
    $offset = $page * $ifc_param4;

    //characters per field
    $ifc_param5 = max((int)init($ifc_param5, 25), 10);

    //sorting
    $ifc_param6 ??= $index;
    if (nstre($ifc_param6))
        $filter .= " ORDER BY `" . sqlesc($ifc_param6, TRUE) . "`, `#id`";

    //build field list
    $array  = [];
    $_array = [];
    foreach ($fieldname AS $value)
    {
        $value    = sqlesc($value);
        $array[]  = $value;
        $_array[] = "LEFT(`$value`, $ifc_param5) AS `$value`";
    };

    //read data
    if (! $primary) mysql_query("SET @id:=0");
    $query  = "SELECT " . $_index . " AS `#id`, " .
              "`" .       implode("`, `", $array) . "` " .
              "FROM `" .  $_object . "`";
    $query  = "SELECT `#id`, " . implode(", ", $_array) . " " .
              "FROM ($query) AS `#t` " .
              "$filter " .
              "LIMIT " . sqlesc($offset) . ", " . sqlesc($ifc_param4);
    $result = mysql_query($query);

    if (($result === FALSE) && stre($ifc_response))
        $ifc_response = CMS_MSG_ERROR . x(mysql_error());

    //menu
    $menu = [CMS_L_IFC_DATABASE_045 . "|database/command_create_record"                      => "edit_table_add"];
    if ($result !== FALSE) $menu[CMS_L_COMMAND_DELETE_SELECTED . "|database/command_delete"] =  "#edit_table_delete";

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        $menu,
        ["object" => $object,
         "row"    => $row,
         "field"  => $field,
         "hash"   => $_hash,
         "offset" => $offset],
        "edit_table",
        CMS_L_IFC_DATABASE_017 . ": $object");

?><script>

function c()
{
    ifc_del("row");
    ifc_del("field");
    ifc_del("ifc_param7");
    ifc_submit();
};

function p(number)
{
    ifc_set("offset", number * <?php echo($ifc_param4);?>);
    ifc_submit();
};

function s(row,
           field)
{
    const fieldlist = ["#id", "<?php echo(implode("\", \"", $fieldname));?>"];
    const object    = ifc_object("list[" + row + "]");
    ifc_set("row",   object.value);
    ifc_set("field", fieldlist[field]);
    ifc_submit();
};

</script><?php

    //control
    ifc_tab_open(CMS_L_IFC_DATABASE_023);

    //field
    $ifc->set($fieldname, "select", $ifc_param1);

    //operation
    $ifc->set([CMS_L_IFC_DATABASE_024 => " LIKE ",
               CMS_L_IFC_DATABASE_080 => " NOT LIKE ",
               "<"                    => "<",
               "<="                   => "<=",
               "="                    => "=",
               ">="                   => ">=",
               ">"                    => ">",
               "!="                   => "!="],
              "select",
              $ifc_param2);

    //value
    $ifc->set(NULL, "text 20 80 :", $ifc_param3);

    //apply button
    $ifc->set(CMS_L_COMMAND_CONFIRM, "button");

    //filter
    ifc_tab_next(CMS_L_IFC_DATABASE_018);

    $array = [ 10 =>  "10",
               25 =>  "25",
               50 =>  "50",
              100 => "100",
              250 => "250",
              500 => "500"];

    //rows per page
    $ifc->set(CMS_L_IFC_DATABASE_021, "label");
    $ifc->set($array, "select :", $ifc_param4);

    //characters per field
    $ifc->set(CMS_L_IFC_DATABASE_022, "label");
    $ifc->set($array, "select :", $ifc_param5);

    //order
    $ifc->set(CMS_L_IFC_DATABASE_025, "label");
    $ifc->set(array_merge(["" => NULL], $fieldname), "select :", $ifc_param6);

    //apply button
    $ifc->set(CMS_L_COMMAND_CONFIRM, "button");

    ifc_tab_close();

    if ($result === FALSE) $ifc->close(); //no results

    //output
    echo("<span class=\"element\">" .
         sprintf(CMS_L_IFC_DATABASE_026,
                 number_format($count, 0, NULL, CMS_L_THOUSAND_SEPARATOR)) .
         "</span>");

    //pagination
    pagination(
        "javascript:p(%page%);",
        $page,
        $_count,
        CMS_L_COMMAND_NEXT,
        "pagination");

    $column_count = count($fieldname);
    $colspan      = $column_count + 1;

    ifc_table_open();
    echo("<colgroup>" .
         "<col style=\"WIDTH:0\">" .
         "<col span=\"$column_count\">" .
         "</colgroup>" .

    //header
         "<tr>" .

    //selection
         "<td class=\"select\">" .
         "</td>");

    //field name
    foreach ($fieldname AS $_field => $_)
        echo("<th>" .
             x($_field) .
             "</th>");
    echo("</tr>");

    $i = 0;

    while ($resultrow = mysql_fetch_row($result))
    {
        $_row         = $resultrow[0];
        $_i           = 1;
        $row_selected = streq($_row, $row);
        $varied       = ifc_varied();

        echo("<tr>" .

        //selection
             "<td class=\"select\">");
        $ifc->set(NULL, "checkbox", $_row, $row_selected, "list[$i]");
        echo("</td>");

        foreach ($fieldname AS $_field => $_)
        {
            echo("<td$varied>");

            //edit
            if (
                 $row_selected
                 &&
                 streq($_field, $field)
               )
            {
                $title   = $fieldtype[$_field];
                $type    = preg_match("/^([^\( ]*)/", $title, $type)  ? $type[1]    : NULL;
                $bracket = preg_match("/\((.*)\)/", $title, $bracket) ? $bracket[1] : NULL;

                //retrieve full value
                if (! $primary) mysql_query("SET @id:=0");
                $query = "SELECT `" . sqlesc($field, TRUE) . "` " .
                         "FROM `" .   $_object . "` " .
                         "WHERE " .   $_index .
                         "='" .       sqlesc($row) . "' " .
                         "LIMIT 1";
                $value = (($_result = mysql_query($query)) !== FALSE) ?
                         mysql_result($_result, 0, 0) : NULL;

                //line input
                switch ($type)
                {
                case "bit":
//..................................................................................................

                    $ifc->set($title, "text 30 w", bitstring($value, $bracket));
                    break;

                case "tinyint":
                case "smallint":
                case "mediumint":
                case "int":
                case "bigint":
                case "double":
                case "float":
                case "decimal":
                case "date":
                case "time":
                case "datetime":
                case "timestamp":
                case "year":
//..................................................................................................

                    $ifc->set($title, "text 30 w", $value);
                    break;

                case "enum":
                case "set":
//..................................................................................................

                    //selectable values
                    $array  = streq($type, "enum") ? [" " => NULL] : NULL;
                    $_array = preg_split("/','/", substr($bracket, 1, -1));

                    foreach ($_array AS $_value)
                    {
                        $_value = str_replace("''", "'", $_value);
                        $array[$_value] = $_value;
                    };

                    $ifc->set($type, "label");

                    //single selection
                    if (streq($type, "enum"))
                        $ifc->set($array, "select 30 w", $value);

                    //multiple selection
                    else
                        $ifc->set($array, "multiselect 30x6 w", explode(",", $value));

                    break;

                default:
//..................................................................................................

                    $ifc->set($title, "textarea 30x10 w", $value);
                    break;
                };

                echo("<div style=\"TEXT-ALIGN:right\">");

                //apply button
                $ifc->set(CMS_L_COMMAND_CONFIRM, "button", "edit_table_update");

                //cancel button
                $ifc->set(CMS_L_COMMAND_CANCEL, "button b", "javascript:c();");

                echo("</div>");
            }

            //display
            else
            {
                $value = $resultrow[$_i];

                //value
                if ($value)
                {
                    if (preg_match("/^bit\(([1-9][0-9]*)\)$/", $fieldtype[$_field], $bracket))
                        $value = bitstring($value, $bracket[1]);

                    $numeric = preg_match("/^[0-9,\.:-]+$/", $value);

                    if ($numeric) echo("<div style=\"TEXT-ALIGN:right\">");
                    echo("<a href=\"javascript:s($i,$_i);\" " .
                            "class=\"br\">" .
                         x($value) .
                         "</a>");
                    if ($numeric) echo("</div>");
                }

                //empty
                else
                {
                       echo("<div style=\"TEXT-ALIGN:center\">" .
                            "<a href=\"javascript:s($i,$_i);\">" .
                            "<small>" .
                            CMS_L_IFC_DATABASE_079 .
                            "</small>" .
                            "</a>" .
                            "</div>");
                };
            };

            echo("</td>");
            $_i++;
        };

        echo("</tr>");
        $i++;
    };

    //selection control
    echo("<tr>" .
         "<td colspan=\"$colspan\" " .
             "class=\"select\" " .
             "style=\"TEXT-ALIGN:right\">");
    $ifc->set(CMS_L_ALL,    "button", "javascript:ifc_list_activate();");
    $ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert();");
    $ifc->set(CMS_L_NONE,   "button", "javascript:ifc_list_deactivate();");
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    $ifc->close();

case "alter_table":
case "create_table":
case "_create_table":
//..................................................................................................

    //get available collations / character sets
    $collation = [CMS_L_IFC_DATABASE_090 => "."];

    if ($result = mysql_query("SHOW COLLATION"))
    {
        while ($resultrow = mysql_fetch_assoc($result))
        {
            $_collation = $resultrow["Collation"];
            $charset    = $resultrow["Charset"];
            $collation[$_collation] = "$charset.$_collation";
        };

        //sort by charset / collation
        asort($collation, SORT_STRING);
    };

    //get available engines
    $engine = [];

    if ($result = mysql_query("SHOW ENGINES"))
    {
        while ($resultrow = mysql_fetch_assoc($result))
        {
            if (streq($resultrow["Support"], "NO")) continue; //not supported

            $_engine  = $resultrow["Engine"];
            $__engine = strtolower($_engine);

            switch ($resultrow["Support"])
            {
            case "DEFAULT":

                init($ifc_param2, $__engine);

            case "YES":

                $engine[$_engine] = $__engine;
            };
        };

        //sort by engine
        asort($engine, SORT_STRING);
    };

    switch (CMS_IFC_MESSAGE)
    {
    case "alter_table":
//..................................................................................................

        //apply
        if (isset($ifc_param1))
        {
            list($charset, $_collation) = explode(".", $ifc_param3);
            $query  = "ALTER TABLE `" .   sqlesc($object, TRUE) . "` "; //command
            if ($ifc_param1 = utf8_trim($ifc_param1)) $query .=         //name
                      "RENAME TO `" .     sqlesc($ifc_param1, TRUE) . "`, ";
            if ($ifc_param2)                          $query .=         //engine
                      "ENGINE='" .        sqlesc($ifc_param2) . "', ";
            if ($charset)                             $query .=         //charset
                      "CHARACTER SET '" . sqlesc($charset) . "', ";
            if ($_collation)                          $query .=         //collation
                      "COLLATE '" .       sqlesc($_collation) . "', ";
            $query .= "COMMENT='" .       sqlesc($ifc_param4) . "'";    //comment

            //execute
            if (mysql_query($query))
            {
                $object       = $ifc_param1;
                $ifc_response = CMS_MSG_DONE;
                break 2;
            };

            $ifc_response = CMS_MSG_ERROR;
        }

        //analyze
        else
        {
            $ifc_param1 = $object;
            $result     = mysql_query("SHOW TABLE STATUS");

            //seek selected table
            while (
                    ($resultrow = mysql_fetch_assoc($result))
                    &&
                    nstreq($resultrow["Name"], $object)
                  );

            $ifc_param1 = $resultrow["Name"];               //name
            $ifc_param2 = strtolower($resultrow["Engine"]); //engine
            if (isset($resultrow["Collation"]) &&           //collation
                isset($collation[$resultrow["Collation"]]))
                $ifc_param3 = $collation[$resultrow["Collation"]];
            $ifc_param4 = $resultrow["Comment"];            //comment
        };

        break;

    case "_create_table":
//..................................................................................................

        $ifc_param1 = utf8_trim($ifc_param1);

        if (! $ifc_param1) //no name
        {
            $ifc_response = CMS_MSG_ERROR;
            break;
        };

        list($charset, $_collation) = explode(".", $ifc_param3);

        //no definition file
        if (empty($ifc_param5))
        {
            $query  = "CREATE TABLE `" .  sqlesc($ifc_param1, TRUE) . "` " .       //command
                      "(`id` INT UNSIGNED PRIMARY KEY AUTO_INCREMENT NOT NULL) " . //type
                      "ENGINE='" .        sqlesc($ifc_param2) . "' ";              //engine
            if ($charset)    $query .=                                             //charset
                      "CHARACTER SET '" . sqlesc($charset) . "' ";
            if ($_collation) $query .=                                             //collation
                      "COLLATE '" .       sqlesc($_collation) . "' ";
            $query .= "COMMENT='" .       sqlesc($ifc_param4) . "'";               //comment

            //execute
            if (mysql_query($query))
            {
                $object       = $ifc_param1;
                $ifc_response = CMS_MSG_DONE;
                break 2;
            };
        }

        //use definition file
        elseif ($query = read_file($ifc_param5))
        {
            $query = preg_replace("/^(CREATE\s+(?:TEMPORARY\s+)?" . //name
                                  "TABLE\s+(?:IF NOT EXISTS\s+)?)" .
                                  "(?:[^\(]+)/i",
                                  "\\1`$ifc_param1`",
                                  $query);

            //execute
            if (mysql_query($query))
            {
                $object       = $ifc_param1;
                $ifc_message  = "alter_table"; //table created, change mode

                //apply settings subsequently
                $query        = "ALTER TABLE `" .   sqlesc($object, TRUE) . "` "; //command
                if ($ifc_param2) $query .=                                        //engine
                                "ENGINE='" .        sqlesc($ifc_param2) . "', ";
                if ($charset)    $query .=                                        //charset
                                "CHARACTER SET '" . sqlesc($charset) . "', ";
                if ($_collation) $query .=                                        //collation
                                "COLLATE '" .       sqlesc($_collation) . "', ";
                $query       .= "COMMENT='" .       sqlesc($ifc_param4) . "'";    //comment

                //execute
                if (mysql_query($query))
                {
                    $ifc_response = CMS_MSG_DONE;
                    break 2;
                };
            };
        };

        $ifc_response = CMS_MSG_ERROR;
        break;
    };

case "create_table":
//..................................................................................................

    //get available definition files
    $definition = [" " => NULL];
    $path       = CMS_DATA_PATH . "#database";

    if (
         nstreq($ifc_message, "alter_table")
         &&
         is_dir($path)
         &&
         ($hdir = opendir($path))
       )
    {
        while ($file_object = readdir($hdir))
        {
            if (
                 is_file("$path/$file_object")
                 &&
                 preg_match("/\.sql$/i", $file_object)
               )
                $definition[$file_object] = "$path/$file_object";
        };

        closedir($hdir);
    };

    //add mysql error message if applicable
    if (streq($ifc_response, CMS_MSG_ERROR) && nstre($error = mysql_error()))
        $ifc_response .= x($error);

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        TRUE,
        ["object" => $object],
        streq($ifc_message,    "alter_table") ?
        "alter_table"          : "_create_table",
        streq(CMS_IFC_MESSAGE, "alter_table") ?
        CMS_L_IFC_DATABASE_014 : CMS_L_IFC_DATABASE_015);

    //name
    $ifc->set(CMS_L_NAME,
              "text 40 256 b",
              isset($ifc_param1) ? $ifc_param1 : NULL);

    //engine
    $ifc->set(CMS_L_IFC_DATABASE_013, "label");
    $ifc->set($engine,
              "select 40 b",
              isset($ifc_param2) ? $ifc_param2 : $mysql->engine);

    //collation
    $ifc->set(CMS_L_IFC_DATABASE_088, "label");
    $ifc->set($collation,
              "select 40 b",
              isset($ifc_param3) ? $ifc_param3 : ".");

    //comment
    $ifc->set(CMS_L_IFC_DATABASE_016,
              "textarea 40x4 60 b",
              isset($ifc_param4) ? $ifc_param4 : NULL);

    //definition file
    if (count($definition) > 1)
    {
        asort($definition, SORT_STRING);
        $ifc->set(CMS_L_IFC_DATABASE_074, "label");
        $ifc->set($definition,            "select 40");
    };

    $ifc->close();

case "add_field":
case "_add_field":
case "change_field":
//..................................................................................................

    //get available collations / character sets
    $collation = [CMS_L_IFC_DATABASE_090 => "."];

    if ($result = mysql_query("SHOW COLLATION"))
    {
        while ($resultrow = mysql_fetch_assoc($result))
        {
            $_collation = $resultrow["Collation"];
            $charset    = $resultrow["Charset"];
            $collation[$_collation] = "$charset.$_collation";
        };

        //sort by charset / collation
        asort($collation, SORT_STRING);
    };

    //use existing data
    if (nstreq(CMS_IFC_MESSAGE, "add_field"))
    {
        //analyze field
        if (empty($ifc_param1) && streq(CMS_IFC_MESSAGE, "change_field"))
        {
            preg_match("/^([^\.]+)\.(.+)$/", $object, $_object);

            $query = "SHOW FULL COLUMNS FROM `" . sqlesc($_object[1], TRUE) . "` " .
                     "LIKE '" .                   sqlesc($_object[2]) . "'";

            //execute
            if (! $result = mysql_query($query))
            {
                $ifc_response = CMS_MSG_ERROR;
                break;
            };

            $array       = mysql_fetch_assoc($result);
            $ifc_param1  = preg_match("/^([^\( ]*)/", $array["Type"], $ifc_param1) ? //type
                           $ifc_param1[1] : NULL;
            $brackets    = preg_match("/\((.*)\)/",   $array["Type"], $brackets)   ? //length
                           $brackets[1]   : NULL;
            $ifc_param2  = $array["Field"];                                          //name
            if (preg_match("/^(NO)?$/", $array["Null"])) $ifc_param3 = TRUE;         //null
            $ifc_param4  = isset($array["Default"]) ? $array["Default"] : NULL;      //default
            $ifc_param19 = isset($array["Comment"]) ? $array["Comment"] : NULL;      //comment

            switch ($ifc_param1)
            {
            case "bit":
//..................................................................................................

                $ifc_param5 = $brackets;                             //length
                break;

            case "char":
            case "varchar":
//..................................................................................................

                $ifc_param6 = $brackets;                             //length
                if (isset($array["Collation"]) &&                    //collation
                    isset($collation[$array["Collation"]]))
                    $ifc_param7 = $collation[$array["Collation"]];
                break;

            case "tinyint":
            case "smallint":
            case "mediumint":
            case "int":
            case "bigint":
//..................................................................................................

                $ifc_param8 = $brackets;                             //length
                if (stristr($array["Type"], "ZEROFILL"))             //attributes
                    $ifc_param9  = "UNSIGNED ZEROFILL";
                elseif (stristr($array["Type"], "UNSIGNED"))
                    $ifc_param9  = "UNSIGNED";
                if (stristr($array["Extra"], "AUTO_INCREMENT"))      //auto increment
                    $ifc_param10 = TRUE;
                break;

            case "tinytext":
            case "text":
            case "mediumtext":
            case "longtext":
//..................................................................................................

                if (isset($array["Collation"]) &&                    //collation
                    isset($collation[$array["Collation"]]))
                    $ifc_param11 = $collation[$array["Collation"]];
                break;

            case "double":
            case "float":
            case "decimal":
//..................................................................................................

                $ifc_param12 = explode(",", $brackets, 2);
                if (isset($ifc_param12[1]))                          //decimals
                    $ifc_param13 = $ifc_param12[1];
                $ifc_param12 = $ifc_param12[0];                      //length
                if (stristr($array["Type"], "ZEROFILL"))             //attributes
                    $ifc_param14 = "UNSIGNED ZEROFILL";
                elseif (stristr($array["Type"], "UNSIGNED"))
                    $ifc_param14 = "UNSIGNED";
                break;

            case "binary":
            case "varbinary":
//..................................................................................................

                $ifc_param15 = $brackets;                            //length
                break;

            case "year":
//..................................................................................................

                $ifc_param16 = $brackets;                            //length
                break;

            case "enum":
            case "set":
//..................................................................................................

                $ifc_param17 = substr($brackets, 1, -1);             //values
                $ifc_param17 = preg_split("/','/", $ifc_param17);
                $ifc_param17 = implode("\n", $ifc_param17);

                $ifc_param17 = str_replace("''", "'", $ifc_param17);
                if (isset($array["Collation"]) &&                    //collation
                    isset($collation[$array["Collation"]]))
                    $ifc_param18 = $collation[$array["Collation"]];
                break;
            };
        }

        //apply changes
        else
        {
            //change field
            if (streq(CMS_IFC_MESSAGE, "change_field"))
            {
                preg_match("/^([^\.]+)\.(.+)$/", $object, $_object);
                $query = "ALTER TABLE `" . sqlesc($_object[1], TRUE) . "` " . //command
                         "CHANGE `" .      sqlesc($_object[2], TRUE) . "` " . //old name
                         "`" .             sqlesc($ifc_param2, TRUE) . "` " . //new name
                                           $ifc_param1;                       //type
            }

            //add field
            else
            {
                preg_match("/^([^\.]+)/", $object, $_object);
                $query = "ALTER TABLE `" . sqlesc($_object[1], TRUE) . "` " . //command
                         "ADD `" .         sqlesc($ifc_param2, TRUE) . "` " . //name
                                           $ifc_param1;                       //type
            };

            switch ($ifc_param1)
            {
            case "bit":
//..................................................................................................

                $query .= ($ifc_param5         ? "(" .                (int)$ifc_param5 . ")"    : NULL) . //length
                          (isset($ifc_param3)  ? " NOT NULL"                                    : NULL) . //null
                          ($ifc_param4         ? " DEFAULT b'" .      sqlesc($ifc_param4) . "'" : NULL);  //default
                break;

            case "tinyint":
            case "smallint":
            case "mediumint":
            case "int":
            case "bigint":
//..................................................................................................

                if (isset($ifc_param10)) $ifc_param4 = NULL; //no default on auto increment
                $query .= ($ifc_param8         ? "(" .                (int)$ifc_param8 . ")"    : NULL) . //length
                          ($ifc_param9         ? " " .                $ifc_param9               : NULL) . //attributes
                          (isset($ifc_param3)  ? " NOT NULL"                                    : NULL) . //null
                          ($ifc_param4         ? " DEFAULT '" .       sqlesc($ifc_param4) . "'" : NULL) . //default
                          (isset($ifc_param10) ? " AUTO_INCREMENT"                              : NULL);  //auto increment
                break;

            case "double":
            case "float":
            case "decimal":
//..................................................................................................

                $query .= ($ifc_param12        ? "(" .                (int)$ifc_param12 .                 //length
                          ($ifc_param13        ? ", " .               (int)$ifc_param13         : NULL) . //decimals
                                                 ")"                                            : NULL) .
                          ($ifc_param14        ? " " .                $ifc_param14              : NULL) . //attributes
                          (isset($ifc_param3)  ? " NOT NULL"                                    : NULL) . //null
                          ($ifc_param4         ? " DEFAULT '" .       sqlesc($ifc_param4) . "'" : NULL);  //default
                break;

            case "date":
            case "time":
            case "datetime":
            case "timestamp":
//..................................................................................................

                $query .= (isset($ifc_param3)  ? " NOT NULL"                                    : NULL) . //null
                          ($ifc_param4         ? " DEFAULT '" .       sqlesc($ifc_param4) . "'" : NULL);  //default
                break;

            case "year":
//..................................................................................................

                $query .= ($ifc_param16        ? "(" .                (int)$ifc_param16 . ")"   : NULL) . //length
                          (isset($ifc_param3)  ? " NOT NULL"                                    : NULL) . //null
                          ($ifc_param4         ? " DEFAULT '" .       sqlesc($ifc_param4) . "'" : NULL);  //default
                break;

            case "char":
            case "varchar":
//..................................................................................................

                list($charset, $_collation) = explode(".", $ifc_param7);
                $query .= ($ifc_param6         ? "(" .                (int)$ifc_param6 . ")"    : NULL) . //length
                          ($charset            ? " CHARACTER SET '" . sqlesc($charset) . "'"    : NULL) . //charset
                          ($_collation         ? " COLLATE '" .       sqlesc($_collation) . "'" : NULL) . //collation
                          (isset($ifc_param3)  ? " NOT NULL"                                    : NULL) . //null
                          ($ifc_param4         ? " DEFAULT '" .       sqlesc($ifc_param4) . "'" : NULL);  //default
                break;

            case "tinytext":
            case "text":
            case "mediumtext":
            case "longtext":
//..................................................................................................

                list($charset, $_collation) = explode(".", $ifc_param11);
                $query .= ($charset            ? " CHARACTER SET '" . sqlesc($charset) . "'"    : NULL) . //charset
                          ($_collation         ? " COLLATE '" .       sqlesc($_collation) . "'" : NULL) . //collation
                          (isset($ifc_param3)  ? " NOT NULL"                                    : NULL);  //null
                break;

            case "binary":
            case "varbinary":
//..................................................................................................

                $query .= ($ifc_param15        ? "(" .                (int)$ifc_param15 . ")"   : NULL) . //length
                          (isset($ifc_param3)  ? " NOT NULL"                                    : NULL) . //null
                          ($ifc_param4         ? " DEFAULT '" .       sqlesc($ifc_param4) . "'" : NULL);  //default
                break;

            case "tinyblob":
            case "blob":
            case "mediumblob":
            case "longblob":
//..................................................................................................

                $query .= (isset($ifc_param3)  ? " NOT NULL"                                    : NULL);  //null
                break;

            case "enum":
            case "set":
//..................................................................................................

                list($charset, $_collation) = explode(".", $ifc_param18);
                $query .= "('" . str_replace("\\n", "', '",           sqlesc($ifc_param17)) . "')" .      //values
                          ($charset            ? " CHARACTER SET '" . sqlesc($charset) . "'"    : NULL) . //charset
                          ($_collation         ? " COLLATE '" .       sqlesc($_collation) . "'" : NULL) . //collation
                          (isset($ifc_param3)  ? " NOT NULL"                                    : NULL) . //null
                          ($ifc_param4         ? " DEFAULT '" .       sqlesc($ifc_param4) . "'" : NULL);  //default
                break;
            };

            $query .= " COMMENT '" . sqlesc($ifc_param19) . "'"; //comment

            //execute
            if (mysql_query($query))
            {
                $object       = $_object[1] . ".$ifc_param2";
                $ifc_response = CMS_MSG_DONE;
                break;
            };

            $ifc_response = CMS_MSG_ERROR;
        };
    };

case "add_field":
//..................................................................................................

    //add mysql error message if applicable
    if (streq($ifc_response, CMS_MSG_ERROR) && nstre($error = mysql_error()))
        $ifc_response .= x($error);

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        TRUE,
        ["object" => $object],
        streq(CMS_IFC_MESSAGE, "change_field") ?
        "change_field"         : "_add_field",
        streq(CMS_IFC_MESSAGE, "change_field") ?
        CMS_L_IFC_DATABASE_001 : CMS_L_IFC_DATABASE_002);

    //type
    $ifc->set(CMS_L_IFC_DATABASE_003, "label");
    $ifc->set(["BIT"        => "bit",
               "TINYINT"    => "tinyint",
               "SMALLINT"   => "smallint",
               "MEDIUMINT"  => "mediumint",
               "INT"        => "int",
               "BIGINT"     => "bigint",
               "DOUBLE"     => "double",
               "FLOAT"      => "float",
               "DECIMAL"    => "decimal",
               "DATE"       => "date",
               "TIME"       => "time",
               "DATETIME"   => "datetime",
               "TIMESTAMP"  => "timestamp",
               "YEAR"       => "year",
               "CHAR"       => "char",
               "VARCHAR"    => "varchar",
               "TINYTEXT"   => "tinytext",
               "TEXT"       => "text",
               "MEDIUMTEXT" => "mediumtext",
               "LONGTEXT"   => "longtext",
               "BINARY"     => "binary",
               "VARBINARY"  => "varbinary",
               "TINYBLOB"   => "tinyblob",
               "BLOB"       => "blob",
               "MEDIUMBLOB" => "mediumblob",
               "LONGBLOB"   => "longblob",
               "ENUM"       => "enum",
               "SET"        => "set"],
              "select",
              isset($ifc_param1) ? $ifc_param1 : "char");

    //name
    $ifc->set(CMS_L_NAME,
              "text 30 256",
              isset($ifc_param2) ? $ifc_param2 : NULL);

    //null
    $ifc->set("NOT NULL",
              "checkbox",
              TRUE,
              isset($ifc_param3));

    ifc_table_open("min");
    echo("<tr>" .
         "<th colspan=\"4\">" .
         CMS_L_IFC_DATABASE_004 .
         "</th>" .
         "</tr>" .
         "<tr>" .

    //common
         "<td colspan=\"4\">");
    $ifc->set(CMS_L_IFC_DATABASE_005,
              "text 60 w",
              isset($ifc_param4) ? $ifc_param4 : NULL);
    echo("</td>" .
         "</tr>" .
         "<tr>" .

    //bit
         "<td>" .
         "BIT" .
         "</td>" .
         "<td>");

    //length
    $ifc->set(CMS_L_IFC_DATABASE_006,
              "text 20",
              isset($ifc_param5) ? $ifc_param5 : NULL);
    echo("</td>" .

    //char
         "<td>" .
         "CHAR<br>VARCHAR" .
         "</td>" .
         "<td>");

    //length
    $ifc->set(CMS_L_IFC_DATABASE_010,
              "text 20 b",
              isset($ifc_param6) ? $ifc_param6 : NULL);

    //collation
    $ifc->set(CMS_L_IFC_DATABASE_088, "label");
    $ifc->set($collation,
              "select 20",
              isset($ifc_param7) ? $ifc_param7 : ".");
    echo("</td>" .
         "</tr>" .
         "<tr>" .

    //integer
         "<td>" .
         "TINYINT<br>SMALLINT<br>MEDIUMINT<br>INT<br>BIGINT" .
         "</td>" .
         "<td>");

    //length
    $ifc->set(CMS_L_IFC_DATABASE_006,
              "text 20 b",
              isset($ifc_param8) ? $ifc_param8 : NULL);

    //attributes
    $ifc->set(CMS_L_IFC_DATABASE_007, "label");
    $ifc->set([NULL                => NULL,
               "UNSIGNED"          => "UNSIGNED",
               "UNSIGNED ZEROFILL" => "UNSIGNED ZEROFILL"],
              "select 20 b",
              isset($ifc_param9) ? $ifc_param9 : NULL);

    //auto increment
    $ifc->set("AUTO_INCREMENT",
              "checkbox",
              TRUE,
              isset($ifc_param10));
    echo("</td>" .

    //text
         "<td>" .
         "TINYTEXT<br>TEXT<br>MEDIUMTEXT<br>LONGTEXT" .
         "</td>" .
         "<td>");

    //collation
    $ifc->set(CMS_L_IFC_DATABASE_088, "label");
    $ifc->set($collation,
              "select 20",
              isset($ifc_param11) ? $ifc_param11 : ".");
    echo("</td>" .
         "</tr>" .
         "<tr>" .

    //decimal
         "<td>" .
         "DOUBLE<br>FLOAT<br>DECIMAL" .
         "</td>" .
         "<td>");

    //length
    $ifc->set(CMS_L_IFC_DATABASE_006,
              "text 20 b",
              isset($ifc_param12) ? $ifc_param12 : NULL);

    //decimals
    $ifc->set(CMS_L_IFC_DATABASE_008,
              "text 20 b",
              isset($ifc_param13) ? $ifc_param13 : NULL);

    //attributes
    $ifc->set(CMS_L_IFC_DATABASE_007, "label");
    $ifc->set([NULL                => NULL,
               "UNSIGNED"          => "UNSIGNED",
               "UNSIGNED ZEROFILL" => "UNSIGNED ZEROFILL"],
              "select 20",
              isset($ifc_param14) ? $ifc_param14 : NULL);
    echo("</td>" .

    //binary
         "<td>" .
         "BINARY<br>VARBINARY" .
         "</td>" .
         "<td>");

    //length
    $ifc->set(CMS_L_IFC_DATABASE_010,
              "text 20",
              isset($ifc_param15) ? $ifc_param15 : NULL);
    echo("</td>" .
         "</tr>" .
         "<tr>" .

    //year
         "<td>" .
         "YEAR" .
         "</td>" .
         "<td>");

    //length
    $ifc->set(CMS_L_IFC_DATABASE_009, "label");
    $ifc->set(["YYYY" => 4,
               "YY"   => 2],
              "select 20",
              isset($ifc_param16) ? $ifc_param16 : 4);
    echo("</td>" .

    //selection
         "<td>" .
         "ENUM<br>SET" .
         "</td>" .
         "<td>");

    //values
    $ifc->set(CMS_L_IFC_DATABASE_011,
              "textarea 20x4 b",
              isset($ifc_param17) ? $ifc_param17 : NULL);

    //collation
    $ifc->set(CMS_L_IFC_DATABASE_088, "label");
    $ifc->set($collation,
              "select 20",
              isset($ifc_param18) ? $ifc_param18 : ".");
    echo("</td>" .
         "</tr>" .
         "<tr>");
    ifc_table_close();

    //comment
    $ifc->set(CMS_L_IFC_DATABASE_016,
              "textarea 60x4 255",
              isset($ifc_param19) ? $ifc_param19 : NULL);

    $ifc->close();

case "_create_index":
//..................................................................................................

    preg_match("/^([^\.]+)/", $object, $_object);

    $query  = "ALTER TABLE `" . sqlesc($_object[1], TRUE) . "` " . //command
              "ADD " .          $ifc_param1;                       //type
    if (nstre($ifc_param2))              $query .=                 //name
              " `" .            sqlesc($ifc_param2, TRUE) . "`";
    if (nstreq($ifc_param1, "FULLTEXT")) $query .=                 //format
              " USING " .       $ifc_param3;

    //index fields
    if (count($list) > 0)
    {
        $query .= " (";

        foreach ($list AS $key => $value)
        {
            $query .= "`" . sqlesc($value, TRUE) . "`";
            if (isset($length[$key]) && (($_value = (int)$length[$key]) > 0))
                $query .= "($_value)";
            $query .= ", ";
        };

        $query  = substr($query, 0, -2) . ")";
    };

    //execute
    if (mysql_query($query))
    {
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;

case "create_index":
//..................................................................................................

    preg_match("/^([^\.]+)(?:\.(.+))?$/", $object, $_object);
    if (! isset($_object[2])) $_object[2] = NULL;

    $query = "SHOW COLUMNS FROM `" . sqlesc($_object[1], TRUE) . "`";

    //execute
    if ((! $result = mysql_query($query)) && stre($ifc_response))
        $ifc_response = CMS_MSG_ERROR;

    //add mysql error message if applicable
    if (streq($ifc_response, CMS_MSG_ERROR) && nstre($error = mysql_error()))
        $ifc_response .= x($error);

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        TRUE,
        ["object" => $object],
        "_create_index",
        CMS_L_IFC_DATABASE_012);

    //type
    $ifc->set(CMS_L_IFC_DATABASE_091, "label");
    $ifc->set(["INDEX"       => "INDEX",
               "UNIQUE"      => "UNIQUE",
               "PRIMARY KEY" => "PRIMARY KEY",
               "FULLTEXT"    => "FULLTEXT"],
              "select",
              isset($ifc_param1) ? $ifc_param1 : NULL);

    //name
    $ifc->set(CMS_L_NAME,
              "text 30 256",
              isset($ifc_param2) ? $ifc_param2 : $_object[2]);

    //format
    $ifc->set(CMS_L_IFC_DATABASE_084, "label");
    $ifc->set(["BTREE" => "BTREE",
               "HASH"  => "HASH"],
              "select b",
              isset($ifc_param3) ? $ifc_param3 : NULL);

    //javascript

?><script>

function s(value)
{
    ifc_select("list[" + value + "]");
};

</script><?php

    //display fields
    $ifc->set(CMS_L_IFC_DATABASE_092, "description b");

    ifc_table_open("min");
    echo("<colgroup>" .
         "<col style=\"WIDTH:0\">" .
         "<col span=\"3\">" .
         "</colgroup>" .

    //header
         "<tr>" .

    //selection
         "<td class=\"select\">" .
         "</td>" .

    //name
         "<th>" .
         CMS_L_NAME .
         "</th>" .

    //type
         "<th>" .
         CMS_L_IFC_DATABASE_003 .
         "</th>" .

    //size
         "<th>" .
         CMS_L_IFC_DATABASE_010 .
         "</th>" .
         "</tr>");

    $i = 0;

    while ($resultrow = mysql_fetch_assoc($result))
    {
        $name   = $resultrow["Field"];
        $type   = $resultrow["Type"];
        $type   = preg_replace("/^(enum|set)\(.*\)/", "\\1(…)", $type);
        $varied = ifc_varied();

        echo("<tr>" .

        //selection
             "<td class=\"select\">");
        $ifc->set(NULL,
                  "checkbox",
                  $name,
                  isset($field[$i]) || streq($name, $_object[2]),
                  "list[$i]");
        echo("</td>" .

        //name
             "<td$varied>" .
             "<a href=\"javascript:s($i);\">" .
             image("database/icon_field") . " " . x($name) .
             "</a>" .
             "</td>" .

        //type
             "<td$varied>" .
             x($type) .
             "</td>" .

        //length
             "<td$varied>");
        if (preg_match("/^(char|varchar|binary|varbinary|" .
                       "tinytext|text|mediumtext|longtext|" .
                       "tinyblob|blob|mediumblob|longblob)(?: |\(|$)/",
                       $type))
            $ifc->set(NULL,
                      "text 10 :",
                      isset($length[$i]) ? $length[$i] : 25,
                      NULL,
                      "length[$i]");
        echo("</td>" .
             "</tr>");

        $i++;
    };

    //selection control
    echo("<tr>" .
         "<td colspan=\"4\" " .
             "class=\"select\" " .
             "style=\"TEXT-ALIGN:right\">");
    $ifc->set(CMS_L_ALL,    "button", "javascript:ifc_list_activate();");
    $ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert();");
    $ifc->set(CMS_L_NONE,   "button", "javascript:ifc_list_deactivate();");
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    $ifc->close();

case "export_definition":
//..................................................................................................

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        TRUE,
        ["object" => $object],
        "_export_definition",
        CMS_L_IFC_DATABASE_075);

    //file
    $ifc->set(CMS_L_NAME, "text 40 80 b", $object);

    $ifc->close();

case "_export_definition":
//..................................................................................................

    if (stre($ifc_param1)) $ifc_param1 = $object;
    $file = CMS_DATA_PATH . "#database/$ifc_param1";

    if ($mysql->export_sql($object, $file))
    {
        //download
        $link         = cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                 "ifc_message" => "download",
                                 "file"        => "$ifc_param1.sql"]);
        $ifc_response = CMS_MSG_DONE .
                        "<button type=\"button\" " .
                                "onclick=\"ifc_download('" . qx($link) . "');\">" .
                        CMS_L_IFC_DATABASE_047 .
                        "</button>";
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "export_table":
//..................................................................................................

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        TRUE,
        ["object" => $object],
        "_export_table",
        CMS_L_IFC_DATABASE_037);

    //file name
    $ifc->set(CMS_L_IFC_DATABASE_027,
              "text 40 80 b",
              $object . "_" . date("Y-m-d"));

    //type
    $ifc->set("HTML", "radio",   0);
    $ifc->set("XLS",  "radio",   1);
    $ifc->set("CSV",  "radio b", 2, TRUE);

    //set fields
    $ifc->set(CMS_L_IFC_DATABASE_077, "checkbox", TRUE, TRUE);

    //csv settings
    $ifc->set(CMS_L_IFC_DATABASE_048, "title");

    //separator
    $ifc->set(CMS_L_IFC_DATABASE_028, "label");
    $ifc->set([CMS_L_IFC_DATABASE_029 => ",",
               CMS_L_IFC_DATABASE_030 => ";",
               CMS_L_IFC_DATABASE_031 => "\t",
               CMS_L_IFC_DATABASE_032 => " "],
              "select 40 b",
              ",");

    //delimiter
    $ifc->set(CMS_L_IFC_DATABASE_033, "label");
    $ifc->set([CMS_L_IFC_DATABASE_034 => "\"",
               CMS_L_IFC_DATABASE_035 => "'",
               CMS_L_IFC_DATABASE_036 => ""],
              "select 40 b",
              "\"");

    $ifc->close();

case "_export_table":
//..................................................................................................

    if (stre($ifc_param1)) $ifc_param1 = $object . "_" . date("Y-m-d");
    $file    = CMS_DATA_PATH . "#database/$ifc_param1";
    $success = FALSE;

    switch ($ifc_param2)
    {
    case 0: //HTML

        $success   = $mysql->export_html($object,             //table
                                         $file,               //file
                                         isset($ifc_param3)); //set fields
        $extension = "htm";
        break;

    case 1: //XLS

        $success   = $mysql->export_excel($object,             //table
                                          $file,               //file
                                          isset($ifc_param3)); //set fields
        $extension = "xls";
        break;

    case 2: //CSV

        $success   = $mysql->export_csv($object,             //table
                                        $ifc_param4,         //separator
                                        $ifc_param5,         //delimiter
                                        $file,               //file
                                        isset($ifc_param3)); //set fields
        $extension = "csv";
        break;
    };

    //download
    if ($success)
    {
        $link         = cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                 "ifc_message" => "download",
                                 "file"        => "$ifc_param1.$extension"]);
        $ifc_response = CMS_MSG_DONE .
                        "<button type=\"button\" " .
                                "onclick=\"ifc_download('" . qx($link) . "');\">" .
                        CMS_L_IFC_DATABASE_047 .
                        "</button>";
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "download":
//..................................................................................................

    download(CMS_DATA_PATH . "#database/" . $file);
    exit();

case "import_table":
//..................................................................................................

    //get available data files
    $path = CMS_DATA_PATH . "#database";

    if ((! is_dir($path)) || (! $hdir = opendir($path)))
    {
        $ifc_response = CMS_MSG_ERROR;
        break;
    };

    $array = NULL;
    while ($file_object = readdir($hdir))
    {
        if (
             (! is_file("$path/$file_object"))
             ||
             (! preg_match("/\.(csv|txt)$/i", $file_object))
           )
            continue;

        $array[$file_object] = "$path/$file_object";
    };

    closedir($hdir);

    if (! $array) //no files
    {
        $ifc_response = CMS_MSG_ERROR;
        break;
    };

    //order by filename
    ksort($array, SORT_NATURAL | SORT_FLAG_CASE);

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        TRUE,
        ["object" => $object],
        $object ? "_import_table" : "___import_table",
        CMS_L_IFC_DATABASE_038);

    //file
    $ifc->set(CMS_L_IFC_DATABASE_039, "label");
    $ifc->set($array,
              "select 40 b",
              isset($file) ? $file : NULL);

    //separator
    $ifc->set(CMS_L_IFC_DATABASE_028, "label");
    $ifc->set([CMS_L_IFC_DATABASE_029 => ",",
               CMS_L_IFC_DATABASE_030 => ";",
               CMS_L_IFC_DATABASE_031 => "\t",
               CMS_L_IFC_DATABASE_032 => " "],
              "select 40 b",
              isset($separator) ? $separator : ",");

    //delimiter
    $ifc->set(CMS_L_IFC_DATABASE_033, "label");
    $ifc->set([CMS_L_IFC_DATABASE_034 => "\"",
               CMS_L_IFC_DATABASE_035 => "'",
               CMS_L_IFC_DATABASE_036 => ""],
              "select 40 b",
              isset($delimiter) ? $delimiter : "\"");

    //get fields
    $ifc->set(CMS_L_IFC_DATABASE_077,
              "checkbox b",
              TRUE,
              isset($ignore_first_row) ? $ignore_first_row : TRUE);

    //ignore existing
    $ifc->set(CMS_L_IFC_DATABASE_046,
              "checkbox",
              TRUE,
              isset($ignore_existing) ? $ignore_existing : TRUE);

    $ifc->close();

case "_import_table":
//..................................................................................................

    //get available fields from table
    $fields = [" " => NULL];

    if ($result = mysql_query("SHOW COLUMNS FROM `" . sqlesc($object) . "`"))
    {
        $i = 0;

        while ($resultrow = mysql_fetch_assoc($result))
        {
            //name
            $name  = $resultrow["Field"];
            $_name = strabridge($name, 20);

            //type
            $type = $resultrow["Type"];
            $type = strabridge($type, 17);

            $fields["`$_name` $type"] = $name;
            $_fields[$i++]            = $name;
        };
    };

    //get first row from file
    $contents = $mysql->import_csv(
        $ifc_param1, //file
        $ifc_param2, //separator
        $ifc_param3, //delimiter
        NULL,        //table
        TRUE);       //get fields

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        [CMS_L_COMMAND_CONFIRM .  "|database/command_apply"    => "__import_table",
         CMS_L_COMMAND_PREVIOUS . "|database/command_previous" => "import_table",
         CMS_L_COMMAND_CANCEL .   "|database/command_cancel"   => FALSE],
        ["object" => $object],
        "__import_table",
        CMS_L_IFC_DATABASE_038);

    //settings from previous screen
    $ifc->param(
        ["file"             => $ifc_param1,
         "separator"        => $ifc_param2,
         "delimiter"        => $ifc_param3,
         "ignore_first_row" => isset($ifc_param4),
         "ignore_existing"  => isset($ifc_param5)]);

    $ifc->set(CMS_L_IFC_DATABASE_095, "description b");

    ifc_table_open("min");
    echo("<tr>" .

    //column number
         "<th>" .
         "</th>" .

    //field
         "<th>" .
         CMS_L_IFC_DATABASE_093 .
         "</th>" .

    //content
         "<th>" .
         CMS_L_IFC_DATABASE_094 .
         "</th>" .
         "</tr>");

    foreach ($contents AS $key => $value)
    {
        $varied = ifc_varied();

        echo("<tr>" .

        //column number
             "<td$varied style=\"TEXT-ALIGN:right\">" .
             x($key + 1) .
             "</td>" .

        //field
             "<td$varied>");
        $ifc->set($fields,
                  "select 40",
                  isset($_fields[$key]) ? $_fields[$key] : NULL,
                  NULL,
                  "mapping[$key]");
        echo("</td>" .

        //content
             "<td class=\"info\">" .
             x(strabridge($value, 100)) .
             "</td>" .
             "</tr>");
    };
    ifc_table_close();

    $ifc->close();

case "__import_table":
//..................................................................................................

    //import data into existing table
    if ($mysql->import_csv($file,             //file
                           $separator,        //separator
                           $delimiter,        //delimiter
                           $object,           //table
                           NULL,              //get fields
                           $ignore_first_row, //ignore first row
                           $ignore_existing,  //ignore_existing
                           $mapping))         //mapping
    {
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "___import_table":
//..................................................................................................

    if (

    //get first row from file
         (! $field = $mysql->import_csv($ifc_param1, //file
                                        $ifc_param2, //separator
                                        $ifc_param3, //delimiter
                                        NULL,        //table
                                        TRUE))       //get fields
         ||

    //get number of fields from file
         (! $count = count($field))
       )
    {
        $ifc_response = CMS_MSG_ERROR;
        break;
    };

    //table name
    $_ifc_param1 = strtoalphanum(file_name($ifc_param1), "_");
    $table       = sqlesc($_ifc_param1, TRUE);

    //create backup if table exists
    $backup = sqlesc("#backup_" . $_ifc_param1 . "_" . date("Ymd_His") . "_" . unique_id(), TRUE);
    mysql_query("RENAME TABLE `$table` TO `$backup`");

    //create table
    $query = "CREATE TABLE `$table` (";

    //first row contains field names
    if (isset($ifc_param4))
        for ($i = 0; $i < $count; $i++) $query .= "`" . sqlesc($field[$i], TRUE) . "` BLOB NOT NULL, ";

    //no field names, enumerate
    else
        for ($i = 0; $i < $count; $i++) $query .= "`column_$i` BLOB NOT NULL, ";

    $query = substr($query, 0, -2) . ") " .
             "DEFAULT CHARACTER SET " . $mysql->charset . " COLLATE " . $mysql->collation;

    //execute
    if (! mysql_query($query))
    {
        //failed, restore backup
        mysql_query("RENAME TABLE `$backup` TO `$table`");
    }
    elseif ($mysql->import_csv($ifc_param1,  //file
                               $ifc_param2,  //separator
                               $ifc_param3,  //delimiter
                               $_ifc_param1, //table
                               NULL,         //get fields
                               TRUE))        //ignore first row
    {
        $object       = $_ifc_param1;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "delete":
//..................................................................................................

    if (count($list) === 0) break; //no selection

    foreach ($list AS $value)
    {
        //split object
        $value = explode(".", $value);
        $table = $value[0] ?? NULL;
        $field = $value[1] ?? NULL;
        $index = $value[2] ?? NULL;

        //delete index
        if ($index)
            $query = "ALTER TABLE `" . sqlesc($table, TRUE) . "` " .
                     (streq($index, "PRIMARY") ?
                     "DROP PRIMARY KEY" :
                     "DROP INDEX `" . sqlesc($index, TRUE) . "`");

        //delete field
        elseif ($field)
            $query = "ALTER TABLE `" . sqlesc($table, TRUE) . "` " .
                     "DROP COLUMN `" . sqlesc($field, TRUE) . "`";

        //delete table
        elseif ($table)
            $query = "DROP TABLE IF EXISTS `" . sqlesc($table, TRUE) . "`";

        //no object
        else
            continue;

        //execute
        if (! mysql_query($query))
        {
            if (stre($ifc_response))           $ifc_response  = CMS_MSG_ERROR;
            if (nstre($error = mysql_error())) $ifc_response .= x($error);
        };
    };

    //deselect object if deleted
    while (in_array($object, $list))
        $object = preg_replace("/\.?[^\.]+$/", "", $object);

    if (stre($ifc_response)) $ifc_response = CMS_MSG_DONE;
    break;

case "backup":
//..................................................................................................

    $ifc_response = $mysql->backup() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "restore":
//..................................................................................................

    $ifc_response = $mysql->restore() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "maintain":
//..................................................................................................

    //get tables
    if (! $result = mysql_query("SHOW TABLES"))
    {
        $ifc_response = CMS_MSG_ERROR;
        break;
    };

    if (! mysql_num_rows($result)) break; //no tables

    //repair tables
    while ($resultrow = mysql_fetch_row($result))
    {
        $query = "REPAIR TABLE `" . sqlesc($resultrow[0], TRUE) . "`";

        //execute
        if (! mysql_query($query))
        {
            if (stre($ifc_response))           $ifc_response  = CMS_MSG_ERROR;
            if (nstre($error = mysql_error())) $ifc_response .= x($error);
        };
    };

    //reset record pointer
    mysql_data_seek($result, 0);

    //optimize tables
    while ($resultrow = mysql_fetch_row($result))
    {
        $query = "OPTIMIZE TABLE `" . sqlesc($resultrow[0], TRUE) . "`";

        //execute
        if (! mysql_query($query))
        {
            if (stre($ifc_response))           $ifc_response  = CMS_MSG_ERROR;
            if (nstre($error = mysql_error())) $ifc_response .= x($error);
        };
    };

    if (stre($ifc_response)) $ifc_response = CMS_MSG_DONE;
    break;

case "config":
//..................................................................................................

    //get values
    $system     = new system();
    $ifc_param1 = $system->getval("mysql", "host");
    $ifc_param2 = $system->getval("mysql", "database");
    $ifc_param3 = $system->getval("mysql", "user");
    $ifc_param4 = $system->getval("mysql", "password");

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        TRUE,
        NULL,
        "_config",
        CMS_L_IFC_DATABASE_050);

    $ifc->set(CMS_L_IFC_DATABASE_041, "text 40 b",   $ifc_param1);
    $ifc->set(CMS_L_IFC_DATABASE_042, "text 40 b",   $ifc_param2);
    $ifc->set(CMS_L_IFC_DATABASE_043, "text 40 b",   $ifc_param3);
    $ifc->set(CMS_L_IFC_DATABASE_044, "password 40", $ifc_param4);

    $ifc->close();

case "_config":
//..................................................................................................

    //set values
    $system = new system();
    $system->setval($ifc_param1, "mysql", "host");
    $system->setval($ifc_param2, "mysql", "database");
    $system->setval($ifc_param3, "mysql", "user");
    $system->setval($ifc_param4, "mysql", "password");

    if ($system->save())
    {
        $mysql        = new mysql();
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

//cache selected object permanently
cms_cache("database." . CMS_USER . ".object", $object, TRUE);

//no connection
if (! $mysql->connection())
{
    $ifc = new ifc(
        CMS_L_IFC_DATABASE_040,
        $ifc_page,
        NULL);

    $ifc->set(CMS_L_IFC_DATABASE_050, "button", "config");

    $ifc->close();
};

//split object
$_object = explode(".", $object);
$table   = $_object[0] ?? NULL;
$field   = $_object[1] ?? NULL;
$index   = $_object[2] ?? NULL;

//determine selected object type
if ($index)     $type = "index";
elseif ($field) $type = "field";
elseif ($table) $type = "table";
else            $type = NULL;

//menu
$menu = [CMS_L_IFC_DATABASE_051 . "|database/command_code"
         => "javascript:load_page('" .
            q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                       "ifc_message" => "sql_console",
                       "ifc_option"  => "external"])) . "');"];
if ($object)
{
    $menu[CMS_L_IFC_DATABASE_017 . "|database/command_editor"] =
        "javascript:load_page('" .
        q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                   "ifc_message" => "edit_table",
                   "ifc_option"  => "external",
                   "object"      => $table])) . "');";

    switch ($type)
    {
    case "table":
//..................................................................................................

        $menu[CMS_L_IFC_DATABASE_014 . "|database/command_edit"]             = "alter_table";
        $menu[CMS_L_IFC_DATABASE_002 . "|database/command_create_field"]     = "add_field";
        $menu[CMS_L_IFC_DATABASE_012 . "|database/command_create_index"]     = "create_index";
        $menu[CMS_L_IFC_DATABASE_075 . "|database/command_export_schema"]    = "export_definition";
        $menu[CMS_L_IFC_DATABASE_037 . "|database/command_export_data"]      = "export_table";
        $menu[CMS_L_IFC_DATABASE_038 . "|database/command_import_data"]      = "import_table";
        break;

    case "field":
//..................................................................................................

        $menu[CMS_L_IFC_DATABASE_002 . "|database/command_create_field"]     = "add_field";
        $menu[CMS_L_IFC_DATABASE_001 . "|database/command_edit"]             = "change_field";
        $menu[CMS_L_IFC_DATABASE_012 . "|database/command_create_index"]     = "create_index";
        break;

    case "index":
//..................................................................................................

        $menu[CMS_L_IFC_DATABASE_002 . "|database/command_create_field"]     = "add_field";
        break;
    };

    $menu[CMS_L_COMMAND_DELETE_SELECTED . "|database/command_delete"]        = "#delete";
}
else
{
    $menu[CMS_L_IFC_DATABASE_015 .        "|database/command_create_table"]  = "create_table";
    $menu[CMS_L_IFC_DATABASE_038 .        "|database/command_import_data"]   = "import_table";
    $menu[CMS_L_COMMAND_DELETE_SELECTED . "|database/command_delete"]        = "#delete";
    $menu[CMS_L_IFC_DATABASE_082 .        "|database/command_backup"]        = "#backup";
    $menu[CMS_L_IFC_DATABASE_083 .        "|database/command_restore"]       = "#restore";
    $menu[CMS_L_IFC_DATABASE_053 .        "|database/command_repair"]        = "#maintain";
    $menu[CMS_L_IFC_DATABASE_050 .        "|database/command_configuration"] = "config";
};

//add mysql error message if applicable
if (streq($ifc_response, CMS_MSG_ERROR) && nstre($error = mysql_error()))
    $ifc_response .= x($error);

//display
$ifc = new ifc(
    $ifc_response,
    $ifc_page,
    $menu,
    ["object" => $object]);

echo("<table class=\"layout\">" .

     "<colgroup>" .
     "<col>" .
     ($object ? "<col style=\"WIDTH:0\">" : "") .
     "</colgroup>" .

     "<tr>" .
     "<td>");

ifc_table_open();
echo("<colgroup>" .
     "<col style=\"WIDTH:0\">" .
     "<col span=\"3\">" .
     "</colgroup>");

//display structure of table
if ($table)
{
    $_table = $table;
    $value  = $_table;
    $name   = x($_table);
    if (streq($value, $object)) $name = "<strong>$name</strong>";

    echo("<tr>" .

    //selection
         "<td rowspan=\"2\" " .
             "class=\"select\">" .
         "</td>" .

    //name
         "<td colspan=\"2\" " .
             "class=\"highlight\">" .
         "<a href=\"javascript:ifc_post('select','" . qx($value) . "');\">" .
         image("database/icon_table") . " $name" .
         "</a>" .
         "</td>" .

    //up
         "<td style=\"TEXT-ALIGN:center\">" .
         "<a href=\"javascript:ifc_post('select');\">" .
         image("database/icon_up") . " " . CMS_L_COMMAND_CLOSE .
         "</a>" .
         "</td>" .
         "</tr>" .
         "<tr>" .

    //name
         "<th>" .
         CMS_L_NAME .
         "</th>" .

    //type
         "<th>" .
         CMS_L_IFC_DATABASE_003 .
         "</th>" .

    //index
         "<th>" .
         CMS_L_IFC_DATABASE_068 .
         "</th>" .
         "</tr>");

    //field
    $_result = mysql_query("SHOW COLUMNS FROM `" . sqlesc($_table, TRUE) . "`");

    while ($_resultrow = mysql_fetch_assoc($_result))
    {
        $_field = $_resultrow["Field"];
        $_value = "$value.$_field";
        $name   = x($_field);
        if ($flag = streq($_value, $object)) $name = "<strong>$name</strong>";
        $varied = ifc_varied();

        echo("<tr>" .

        //selection
             "<td class=\"select\">");
        $ifc->set(NULL, "checkbox", $_value, streq($_value, $object), "list[]");
        echo("</td>" .

        //name
             "<td$varied>" .
             "<a href=\"javascript:ifc_post('select','" . qx($_value) . "');\">" .
             image("database/icon_field") . " $name" .
             "</a>" .
             "</td>" .

        //type
             "<td$varied>" .
             x(preg_replace("/^(enum|set)\(.*\)/", "\\1(…)", $_resultrow["Type"])) .
             "</td>" .

        //index
             "<td$varied>" .
             x($_resultrow["Key"]) .
             "</td>" .
             "</tr>");

        //index
        if (streq($_field, $field))
        {
            $__result = mysql_query("SHOW INDEX FROM `" . sqlesc($table, TRUE) . "`");
            $i        = 0;

            while ($__resultrow = mysql_fetch_assoc($__result))
            {
                if (nstreq($__resultrow["Column_name"], $_field)) continue;

                $_index  = $__resultrow["Key_name"];
                $__value = "$_value.$_index";
                $name    = x($_index);
                if (streq($__value, $object)) $name = "<strong>$name</strong>";

                $varied = ifc_varied();

                echo("<tr>" .

                //selection
                     "<td class=\"select\">");
                $ifc->set(NULL, "checkbox", $__value, streq($__value, $object), "list[]");
                echo("</td>" .

                //name
                     "<td$varied colspan=\"3\">" .
                     "<a href=\"javascript:ifc_post('select','" . qx($__value) . "');\">" .
                     image("database/icon_index") . " $name" .
                     "</a>" .
                     "</td>" .
                     "</tr>");
            };
        };
    };
}

//display table list
else
{
    //header
    echo("<tr>" .

    //selection
         "<td class=\"select\">" .
         "</td>" .

    //name
         "<th colspan=\"3\">" .
         CMS_L_IFC_DATABASE_054 .
         "</th>" .
         "</tr>");

    $result = mysql_query("SHOW TABLES");

    while ($resultrow = mysql_fetch_row($result))
    {
        $_table = $resultrow[0];
        $name   = x($_table);
        $value  = $_table;
        $varied = ifc_varied();

        echo("<tr>" .

        //selection
             "<td class=\"select\">");
        $ifc->set(NULL, "checkbox", $_table, NULL, "list[]");
        echo("</td>" .

        //name
             "<td colspan=\"3\"$varied>" .
             "<a href=\"javascript:ifc_post('select','" . qx($value) . "');\">" .
             image("database/icon_table") . " $name" .
             "</a>" .
             "</td>" .
             "</tr>");
    };
};

//selection control
echo("<tr>" .
     "<td colspan=\"4\" " .
         "class=\"select\" " .
         "style=\"TEXT-ALIGN:right\">");
$ifc->set(CMS_L_ALL,    "button", "javascript:ifc_list_activate();");
$ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert();");
$ifc->set(CMS_L_NONE,   "button", "javascript:ifc_list_deactivate();");
echo("</td>" .
     "</tr>");
ifc_table_close();

echo("</td>");

//no information
if (! $object)
{
    echo("</tr>" .
         "</table>");

    $ifc->close();
};

//data
echo("<td>");

switch ($type)
{
case "table":
//..................................................................................................

    $result    = mysql_query("SHOW TABLE STATUS " .
                             "LIKE '" . sqlesc($table) . "'");
    $resultrow = mysql_fetch_assoc($result);

    ifc_table_open();
    echo("<tr>" .
         "<th colspan=\"2\">" .
         x($resultrow["Name"]) .
         "</th>" .
         "</tr>" .

         (isset($resultrow["Engine"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_013 . ":" .
         "</td>" .
         "<td>" .
         x($resultrow["Engine"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Version"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_086 . ":" .
         "</td>" .
         "<td>" .
         x($resultrow["Version"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Row_format"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_056 . ":" .
         "</td>" .
         "<td>" .
         x($resultrow["Row_format"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Rows"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_057 . ":" .
         "</td>" .
         "<td>" .
         x($resultrow["Rows"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Avg_row_length"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_058 . ":" .
         "</td>" .
         "<td>" .
         format_bytesize($resultrow["Avg_row_length"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Data_length"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_059 . ":" .
         "</td>" .
         "<td>" .
         format_bytesize($resultrow["Data_length"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Max_data_length"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_060 . ":" .
         "</td>" .
         "<td>" .
         format_bytesize($resultrow["Max_data_length"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Index_length"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_061 . ":" .
         "</td>" .
         "<td>" .
         format_bytesize($resultrow["Index_length"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Data_free"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_062 . ":" .
         "</td>" .
         "<td>" .
         format_bytesize($resultrow["Data_free"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Auto_increment"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_063 . ":" .
         "</td>" .
         "<td>" .
         x($resultrow["Auto_increment"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Create_time"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_064 . ":" .
         "</td>" .
         "<td>" .
         x($resultrow["Create_time"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Update_time"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_065 . ":" .
         "</td>" .
         "<td>" .
         x($resultrow["Update_time"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Check_time"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_066 . ":" .
         "</td>" .
         "<td>" .
         x($resultrow["Check_time"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Collation"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_088 . ":" .
         "</td>" .
         "<td>" .
         x($resultrow["Collation"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Checksum"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_087 . ":" .
         "</td>" .
         "<td>" .
         x($resultrow["Checksum"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Create_options"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_069 . ":" .
         "</td>" .
         "<td>" .
         x($resultrow["Create_options"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Comment"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_016 . ":" .
         "</td>" .
         "<td class=\"br\">" .
         x($resultrow["Comment"]) .
         "</td>" .
         "</tr>" :
         NULL));
    ifc_table_close();
    break;

case "field":
//..................................................................................................

    $result    = mysql_query("SHOW FULL COLUMNS FROM `" . sqlesc($table, TRUE) . "` " .
                             "LIKE '" .                   sqlesc($field) . "'");
    $resultrow = mysql_fetch_assoc($result);

    ifc_table_open();
    echo("<tr>" .
         "<th colspan=\"2\">" .
         x($resultrow["Field"]) .
         "</th>" .
         "</tr>" .

         (isset($resultrow["Type"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_003 . ":" .
         "</td>" .
         "<td>" .
         x(preg_replace("/^(enum|set)\(.*\)/", "\\1(…)", $resultrow["Type"])) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Collation"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_088 . ":" .
         "</td>" .
         "<td>" .
         x($resultrow["Collation"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Null"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_067 . ":" .
         "</td>" .
         "<td>" .
         yesno(! preg_match("/^(NO)?$/", $resultrow["Null"])) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Key"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_068 . ":" .
         "</td>" .
         "<td>" .
         x($resultrow["Key"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Default"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_005 . ":" .
         "</td>" .
         "<td>" .
         x($resultrow["Default"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Extra"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_069 . ":" .
         "</td>" .
         "<td>" .
         x($resultrow["Extra"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Privileges"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_089 . ":" .
         "</td>" .
         "<td>" .
         str_replace(",", "<br>", x($resultrow["Privileges"])) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Comment"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_016 . ":" .
         "</td>" .
         "<td class=\"br\">" .
         x($resultrow["Comment"]) .
         "</td>" .
         "</tr>" :
         NULL));
    ifc_table_close();
    break;

case "index":
//..................................................................................................

    $result    = mysql_query("SHOW INDEX FROM `" .  sqlesc($table, TRUE) . "` " .
                             "WHERE `Key_name`='" . sqlesc($index) . "' " .
                             "AND `Column_name`='" . sqlesc($field) . "'");
    $resultrow = mysql_fetch_assoc($result);

    ifc_table_open();
    echo("<tr>" .
         "<th colspan=\"2\">" .
         x($resultrow["Key_name"]) .
         "</th>" .
         "</tr>" .

         (isset($resultrow["Non_unique"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_070 . ":" .
         "</td>" .
         "<td>" .
         yesno(! $resultrow["Non_unique"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Seq_in_index"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_071 . ":" .
         "</td>" .
         "<td>" .
         x($resultrow["Seq_in_index"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Collation"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_025 . ":" .
         "</td>" .
         "<td>" .
         yesno($resultrow["Collation"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Cardinality"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_073 . ":" .
         "</td>" .
         "<td>" .
         x($resultrow["Cardinality"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Sub_part"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_010 . ":" .
         "</td>" .
         "<td>" .
         x($resultrow["Sub_part"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Packed"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_085 . ":" .
         "</td>" .
         "<td>" .
         x($resultrow["Packed"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Null"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_067 . ":" .
         "</td>" .
         "<td>" .
         yesno(! preg_match("/^(NO)?$/", $resultrow["Null"])) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Index_type"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_084 . ":" .
         "</td>" .
         "<td>" .
         x($resultrow["Index_type"]) .
         "</td>" .
         "</tr>" :
         NULL) .

         (isset($resultrow["Index_comment"]) ?
         "<tr>" .
         "<td>" .
         CMS_L_IFC_DATABASE_016 . ":" .
         "</td>" .
         "<td class=\"br\">" .
         x($resultrow["Index_comment"]) .
         "</td>" .
         "</tr>" :
         NULL));
    ifc_table_close();
    break;
};

echo("</td>" .
     "</tr>" .
     "</table>");

$ifc->close();