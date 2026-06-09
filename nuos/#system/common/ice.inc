<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

function ice_check_path($path)
{
    if (preg_match("/(?:^|[\/\\\])\.{1,2}(?:[\/\\\]|$)/", $path)) exit();
};

//--------------------------------------------------------------------------------------------------

function chgrp($filename,
               $group,
               ...$args)
{
    ice_check_path($filename);

    return \chgrp($filename, $group, ...$args);
};

//--------------------------------------------------------------------------------------------------

function chmod($filename,
               $mode,
               ...$args)
{
    ice_check_path($filename);

    return \chmod($filename, $mode, ...$args);
};

//--------------------------------------------------------------------------------------------------

function chown($filename,
               $user,
               ...$args)
{
    ice_check_path($filename);

    return \chown($filename, $user, ...$args);
};

//--------------------------------------------------------------------------------------------------

function copy($source,
              $dest,
              ...$args)
{
    ice_check_path($source);
    ice_check_path($dest);

    if (($return = \copy($source, $dest, ...$args)) && \is_file($dest))
        \chmod($dest, \is_file($source) ? (\fileperms($source) & 0777) : (CMS_APACHE ? 0666 : 0644));

    return $return;
};

//--------------------------------------------------------------------------------------------------

function file_get_contents($filename,
                           ...$args)
{
    ice_check_path($filename);

    return \file_get_contents($filename, ...$args);
};

//--------------------------------------------------------------------------------------------------

function file_put_contents($filename,
                           $data,
                           ...$args)
{
    ice_check_path($filename);

    $flag   = ! \is_file($filename);
    $return = \file_put_contents($filename, $data, ...$args);

    if (($return !== FALSE) && $flag && \is_file($filename))
        \chmod($filename, CMS_APACHE ? 0666 : 0644);

    return $return;
};

//--------------------------------------------------------------------------------------------------

function file($filename,
              ...$args)
{
    ice_check_path($filename);

    return \file($filename, ...$args);
};

//--------------------------------------------------------------------------------------------------

function fopen($filename,
               $mode,
               ...$args)
{
    ice_check_path($filename);

    $flag   = ! \is_file($filename);
    $return = \fopen($filename, $mode, ...$args);

    if (($return !== FALSE) && $flag && \is_file($filename))
        \chmod($filename, CMS_APACHE ? 0666 : 0644);

    return $return;
};

//--------------------------------------------------------------------------------------------------

function mkdir($pathname,
               ...$args)
{
    ice_check_path($pathname);

    $basename   = basename($pathname);
    $char       = substr($basename, 0, 1);
    $executable = $char === "!";
    $protected  = $executable || ($char === "#");
    $mode       =  CMS_APACHE ? 0777 :        //apache module
                  ($protected ? 0700 : 0755); //cgi
    $args[0]    = $mode;

    if (
         ($return = \mkdir($pathname, ...$args))
         &&
         $protected
         &&
         ($hfile = \fopen("$pathname/.htaccess", "wb"))
       )
    {
        flock($hfile, LOCK_EX);
        fwrite($hfile,
               "Order Allow,Deny\n" .
               "Deny from all");
        if ($executable)
            fwrite($hfile, "\nOptions +ExecCGI");
        fclose($hfile);
    };

    return $return;
};

//--------------------------------------------------------------------------------------------------

function move_uploaded_file($filename,
                            $destination,
                            ...$args)
{
    ice_check_path($filename);
    ice_check_path($destination);

    if ($return = \move_uploaded_file($filename, $destination, ...$args))
        \chmod($destination, CMS_APACHE ? 0666 : 0644);

    return $return;
};

//--------------------------------------------------------------------------------------------------

function opendir($path,
                 ...$args)
{
    ice_check_path($path);

    return \opendir($path, ...$args);
};

//--------------------------------------------------------------------------------------------------

function readfile($filename,
                  ...$args)
{
    ice_check_path($filename);

    return \readfile($filename, ...$args);
};

//--------------------------------------------------------------------------------------------------

function rename($oldname,
                $newname,
                ...$args)
{
    ice_check_path($oldname);
    ice_check_path($newname);

    return \rename($oldname, $newname, ...$args);
};

//--------------------------------------------------------------------------------------------------

function rmdir($dirname,
               ...$args)
{
    ice_check_path($dirname);

    return \rmdir($dirname, ...$args);
};

//--------------------------------------------------------------------------------------------------

function unlink($filename,
                ...$args)
{
    ice_check_path($filename);

    return \unlink($filename, ...$args);
};