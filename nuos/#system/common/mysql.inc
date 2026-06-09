<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//global mysql connection
$cms_mysql_connection = NULL;

//result type constants
define("MYSQL_ASSOC", MYSQLI_ASSOC);
define("MYSQL_NUM",   MYSQLI_NUM);
define("MYSQL_BOTH",  MYSQLI_BOTH);

//disable mysql error reporting as of php 8.1.0
mysqli_report(MYSQLI_REPORT_OFF);

function mysql_get_link_identifier($link_identifier = NULL)
{
    global $cms_mysql_connection;

    if ($link_identifier instanceof \mysqli)
        return $link_identifier;

    if ($cms_mysql_connection instanceof \mysqli)
        return $cms_mysql_connection;

    return NULL;
};

//--------------------------------------------------------------------------------------------------

function mysql_affected_rows($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return mysqli_affected_rows($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_client_encoding($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return mysqli_character_set_name($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_close($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return mysqli_close($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_connect($server      = NULL,
                       $username    = NULL,
                       $password    = NULL,
                       $new_link    = NULL,
                       $client_flag = NULL,
                       $persistent  = FALSE)
{
    global $cms_mysql_connection;

    $port   = NULL;
    $socket = NULL;

    //port or socket
    if (
         (($pos = strrpos($server, ":")) !== FALSE)
         &&
         (strpos($server, "]", $pos) === FALSE)
       )
    {
        $temp   = substr($server, $pos + 1);
        $server = substr($server, 0, $pos);

        if (ctype_digit($temp)) $port   = (int)$temp;
        else                    $socket = $temp;
    };

    if ($persistent)
        $server = (strpos($server, "[") === 0) ?
                  "[p:" . substr($server, 1) :
                  "p:$server";

    $cms_mysql_connection = mysqli_connect($server, $username, $password, NULL, $port, $socket);
    return $cms_mysql_connection;
};

//--------------------------------------------------------------------------------------------------

function mysql_data_seek($result,
                         $row_number)
{
    return mysqli_data_seek($result, $row_number);
};

//--------------------------------------------------------------------------------------------------

function mysql_db_name($result,
                       $row,
                       $field = NULL)
{
    if (! mysqli_data_seek($result, $row)) return FALSE;
    $resultrow = mysqli_fetch_array($result);
    return ($field === NULL) ? $resultrow[0] : $resultrow[$field];
};

//--------------------------------------------------------------------------------------------------

function mysql_errno($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return mysqli_errno($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_error($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return mysqli_error($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_fetch_array($result,
                           $result_type = MYSQL_BOTH)
{
    return mysqli_fetch_array($result, $result_type) ?: FALSE;
};

//--------------------------------------------------------------------------------------------------

function mysql_fetch_assoc($result)
{
    return mysqli_fetch_assoc($result) ?: FALSE;
};

//--------------------------------------------------------------------------------------------------

function mysql_fetch_field($result,
                           $field_offset = NULL)
{
    if ($field_offset !== NULL) mysqli_field_seek($result, $field_offset);
    return mysqli_fetch_field($result) ?: FALSE;
};

//--------------------------------------------------------------------------------------------------

function mysql_fetch_lengths($result)
{
    return mysqli_fetch_lengths($result) ?: FALSE;
};

//--------------------------------------------------------------------------------------------------

function mysql_fetch_object($result,
                            $class_name = NULL,
                            $params     = NULL)
{
    return mysqli_fetch_object($result, $class_name ?? "stdClass", $params) ?: FALSE;
};

//--------------------------------------------------------------------------------------------------

function mysql_fetch_row($result)
{
    return mysqli_fetch_row($result) ?: FALSE;
};

//--------------------------------------------------------------------------------------------------

function mysql_field_flags($result,
                           $field_offset)
{
    if (! mysqli_field_seek($result, $field_offset)) return FALSE;
    $field = mysqli_fetch_field($result);
    return $field ? $field->flags : FALSE; //omitting string conversion
};

//--------------------------------------------------------------------------------------------------

function mysql_field_len($result,
                         $field_offset)
{
    if (! mysqli_field_seek($result, $field_offset)) return FALSE;
    $field = mysqli_fetch_field($result);
    return $field ? $field->length : FALSE;
};

//--------------------------------------------------------------------------------------------------

function mysql_field_name($result,
                          $field_offset)
{
    if (! mysqli_field_seek($result, $field_offset)) return FALSE;
    $field = mysqli_fetch_field($result);
    return $field ? $field->name : FALSE;
};

//--------------------------------------------------------------------------------------------------

function mysql_field_seek($result,
                          $field_offset)
{
    return mysqli_field_seek($result, $field_offset);
};

//--------------------------------------------------------------------------------------------------

function mysql_field_table($result,
                           $field_offset)
{
    if (! mysqli_field_seek($result, $field_offset)) return FALSE;
    $field = mysqli_fetch_field($result);
    return $field ? $field->table : FALSE;
};

//--------------------------------------------------------------------------------------------------

function mysql_field_type($result,
                          $field_offset)
{
    if (! mysqli_field_seek($result, $field_offset)) return FALSE;
    $field = mysqli_fetch_field($result);
    return $field ? $field->type : FALSE; //omitting string conversion
};

//--------------------------------------------------------------------------------------------------

function mysql_free_result($result)
{
    return mysqli_free_result($result);
};

//--------------------------------------------------------------------------------------------------

function mysql_get_client_info()
{
    return mysqli_get_client_info();
};

//--------------------------------------------------------------------------------------------------

function mysql_get_host_info($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return mysqli_get_host_info($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_get_proto_info($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return mysqli_get_proto_info($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_get_server_info($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return mysqli_get_server_info($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_info($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return mysqli_info($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_insert_id($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return mysqli_insert_id($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_list_dbs($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return mysqli_query($link_identifier, "SHOW DATABASES");
};

//--------------------------------------------------------------------------------------------------

function mysql_list_processes($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return mysqli_query($link_identifier, "SHOW PROCESSLIST");
};

//--------------------------------------------------------------------------------------------------

function mysql_num_fields($result)
{
    return mysqli_num_fields($result);
};

//--------------------------------------------------------------------------------------------------

function mysql_num_rows($result)
{
    return mysqli_num_rows($result);
};

//--------------------------------------------------------------------------------------------------

function mysql_pconnect($server      = NULL,
                        $username    = NULL,
                        $password    = NULL,
                        $client_flag = NULL)
{
    return mysql_connect($server, $username, $password, NULL, TRUE);
};

//--------------------------------------------------------------------------------------------------

function mysql_ping($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return (bool)mysqli_query($link_identifier, "SELECT 1");
};

//--------------------------------------------------------------------------------------------------

function mysql_query($query,
                     $link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    $result = mysqli_query($link_identifier, $query);

    if ($result === FALSE) trigger_error(mysqli_error($link_identifier) . "; " . $query, E_USER_ERROR);
    return $result;
};

//--------------------------------------------------------------------------------------------------

function mysql_real_escape_string($unescaped_string,
                                  $link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return mysqli_real_escape_string($link_identifier, $unescaped_string);
};

//--------------------------------------------------------------------------------------------------

function mysql_result($result,
                      $row,
                      $field = NULL)
{
    if (! mysqli_data_seek($result, $row)) return FALSE;
    $resultrow = mysqli_fetch_array($result);
    if ($resultrow === NULL) return FALSE;
    return ($field === NULL) ? $resultrow[0] : ($resultrow[$field] ?? FALSE);
};

//--------------------------------------------------------------------------------------------------

function mysql_select_db($database_name,
                         $link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return mysqli_select_db($link_identifier, $database_name);
};

//--------------------------------------------------------------------------------------------------

function mysql_stat($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return mysqli_stat($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_thread_id($link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);
    return mysqli_thread_id($link_identifier);
};

//--------------------------------------------------------------------------------------------------

function mysql_unbuffered_query($query,
                                $link_identifier = NULL)
{
    $link_identifier = mysql_get_link_identifier($link_identifier);

    if (mysqli_real_query($link_identifier, $query))
        return mysqli_field_count($link_identifier) ?
               mysqli_use_result($link_identifier) : TRUE;

    trigger_error(mysqli_error($link_identifier) . "; " . $query, E_USER_ERROR);
    return FALSE;
};