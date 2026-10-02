<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

function pta_block($path)
{
    if (preg_match("/(?:^|[\/\\\])\.{1,2}(?:[\/\\\]|$)/", $path)) die("Invalid path.");
};

//--------------------------------------------------------------------------------------------------

function chgrp($filename,
               ...$args)
{
    pta_block($filename);

    if ($result = \chgrp($filename, ...$args))
        clearstatcache(TRUE, $filename);

    return $result;
};

//--------------------------------------------------------------------------------------------------

function chmod($filename,
               ...$args)
{
    pta_block($filename);

    if ($result = \chmod($filename, ...$args))
        clearstatcache(TRUE, $filename);

    return $result;
};

//--------------------------------------------------------------------------------------------------

function chown($filename,
               ...$args)
{
    pta_block($filename);

    if ($result = \chown($filename, ...$args))
        clearstatcache(TRUE, $filename);

    return $result;
};

//--------------------------------------------------------------------------------------------------

function copy($from,
              $to,
              ...$args)
{
    pta_block($from);
    pta_block($to);

    //save deferred data before copying
    if ($flag = class_exists(__NAMESPACE__ . "\\data", FALSE))
        data::defer_apply($from);

    if ($result = \copy($from, $to, ...$args))
    {
        clearstatcache(TRUE, $to);

        //discard deferred data of replaced file
        if ($flag) data::defer_discard($to);

        if (
             \is_file($to)
             &&
             \chmod($to, \is_file($from) ?
                         (\fileperms($from) & 0777) :
                         (CMS_APACHE ? 0666 : 0644))
           )
            clearstatcache(TRUE, $to);
    };

    return $result;
};

//--------------------------------------------------------------------------------------------------

function rename($from,
                $to,
                ...$args)
{
    pta_block($from);
    pta_block($to);

    //save deferred data before moving
    if ($flag = class_exists(__NAMESPACE__ . "\\data", FALSE))
        data::defer_apply(\is_dir($from) ? NULL : $from);

    if (DIRECTORY_SEPARATOR === "\\") //windows
    {
        for ($i = 0; $i < 10; $i++) //10 retries
        {
            if ($result = @\rename($from, $to, ...$args)) break; //silent, may fail
            usleep(20000); //wait 20 ms
        };
    };

    if (empty($result) && (! $result = \rename($from, $to, ...$args))) return $result;

    clearstatcache(TRUE, $from);
    clearstatcache(TRUE, $to);

    //relocate deferred data cache
    if ($flag) data::defer_relocate($from, $to);

    return $result;
};

//--------------------------------------------------------------------------------------------------

function move_uploaded_file($from,
                            $to,
                            ...$args)
{
    pta_block($from);
    pta_block($to);

    if (CMS_MCP_ACTIVE && isset(mcp::$post_temp[$from]))
    {
        if ($result = \rename($from, $to)) unset(mcp::$post_temp[$from]);
    }
    else
    {
        $result = \move_uploaded_file($from, $to, ...$args);
    };

    if ($result)
    {
        \chmod($to, CMS_APACHE ? 0666 : 0644);
        clearstatcache(TRUE, $to);
    };

    return $result;
};

//--------------------------------------------------------------------------------------------------

function file($filename,
              ...$args)
{
    pta_block($filename);

    return \file($filename, ...$args);
};

//--------------------------------------------------------------------------------------------------

function readfile($filename,
                  ...$args)
{
    pta_block($filename);

    return \readfile($filename, ...$args);
};

//--------------------------------------------------------------------------------------------------

function file_get_contents($filename,
                           ...$args)
{
    pta_block($filename);

    return \file_get_contents($filename, ...$args);
};

//--------------------------------------------------------------------------------------------------

function file_put_contents($filename,
                           ...$args)
{
    pta_block($filename);

    $flag   = ! \is_file($filename);
    $result = \file_put_contents($filename, ...$args);

    if ($result !== FALSE)
    {
        clearstatcache(TRUE, $filename);

        if ($flag && \is_file($filename) && \chmod($filename, CMS_APACHE ? 0666 : 0644))
            clearstatcache(TRUE, $filename);
    };

    return $result;
};

//--------------------------------------------------------------------------------------------------

function opendir($directory,
                 ...$args)
{
    pta_block($directory);

    return \opendir($directory, ...$args);
};

//--------------------------------------------------------------------------------------------------

function mkdir($directory,
               ...$args)
{
    pta_block($directory);

    $basename   = basename($directory);
    $char       = substr($basename, 0, 1);
    $executable = $char === "!";
    $protected  = $executable || ($char === "#");
    $mode       =  CMS_APACHE ? 0777 :        //apache module
                  ($protected ? 0700 : 0755); //cgi
    $args[0]    = $mode;

    //suppress warning about existing directory
    $prev = set_error_handler(function(...$error) use (&$prev, $directory)
    {
        return is_dir($directory) || (($prev !== NULL) && $prev(...$error));
    });

    //create
    $result = \mkdir($directory, ...$args);

    restore_error_handler();

    if ($result)
    {
        clearstatcache(TRUE, $directory);

        if ($protected)
            write_file(
                "$directory/.htaccess",
                "<IfModule mod_authz_core.c>\n" .
                "Require all denied\n" .
                "</IfModule>\n" .
                "<IfModule !mod_authz_core.c>\n" .
                "Order Allow,Deny\n" .
                "Deny from all\n" .
                "</IfModule>" .
                ($executable ? "\nOptions +ExecCGI" : ""));
    };

    return $result;
};

//--------------------------------------------------------------------------------------------------

function rmdir($directory,
               ...$args)
{
    pta_block($directory);

    if ($result = \rmdir($directory, ...$args))
        clearstatcache(TRUE, $directory);

    return $result;
};

//--------------------------------------------------------------------------------------------------

function fopen($filename,
               ...$args)
{
    pta_block($filename);

    $flag   = ! \is_file($filename);
    $result = \fopen($filename, ...$args);

    if ($result !== FALSE)
    {
        clearstatcache(TRUE, $filename);

        if ($flag && \is_file($filename) && \chmod($filename, CMS_APACHE ? 0666 : 0644))
            clearstatcache(TRUE, $filename);
    };

    return $result;
};

//--------------------------------------------------------------------------------------------------

function fclose($stream)
{
    $meta   = stream_get_meta_data($stream);
    $result = \fclose($stream);

    if ($result)
    {
        $uri = $meta["uri"] ?? "";
        if (nstre($uri) && (parse_url($uri, PHP_URL_SCHEME) === NULL))
            clearstatcache(TRUE, $uri);
    };

    return $result;
};

//--------------------------------------------------------------------------------------------------

function touch($filename,
               ...$args)
{
    pta_block($filename);

    if ($result = \touch($filename, ...$args))
        clearstatcache(TRUE, $filename);

    return $result;
};

//--------------------------------------------------------------------------------------------------

function unlink($filename,
                ...$args)
{
    pta_block($filename);

    if ($result = \unlink($filename, ...$args))
    {
        clearstatcache(TRUE, $filename);

        //discard deferred data
        if (class_exists(__NAMESPACE__ . "\\data", FALSE))
            data::defer_discard($filename);
    };

    return $result;
};
