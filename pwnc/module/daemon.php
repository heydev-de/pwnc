<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   MODULE
//==================================================================================================

require("../pwnc.inc");

(function() {

//task path
$path = CMS_DATA_PATH . "#daemon/";
if (! mkpath($path)) exit();

//find empty process slot
for ($proc = 1; $proc <= CMS_DAEMON_PROC_LIMIT; $proc++)
{
    //try advisory lock
    $lock  = $path . "daemon.$proc.lock";
    $hfile = fopen($lock, "c");
    if ($hfile === FALSE) exit();

    //advisory lock
    if (flock($hfile, LOCK_EX | LOCK_NB)) break;
    fclose($hfile);
};

if ($proc > CMS_DAEMON_PROC_LIMIT) exit(); //no empty process slot

//remove task available flag
$flag = $path . "daemon.flag";
if (is_file($flag)) unlink($flag);

//retrieve tasks
$list = scandir($path, SCANDIR_SORT_NONE);
$list = array_diff($list, [".", "..", ".htaccess", "daemon.flag", "daemon.status"]);
$task = [];

foreach ($list AS $file)
{
    $_file = $path . $file;
    if (! is_file($_file))             continue;
    if (filesize($_file) === 0)        continue;
    if (substr($file, -5) === ".lock") continue;
    if (substr($file, -4) === ".tmp")  continue;
    $task[$file] = filemtime($_file);
};

if (blank($task)) exit(); //nothing to do

$lock_list = [];   //lock files and handles
$error     = [];   //per task error messages
$flag      = TRUE; //additional daemon

cms_daemon_status("Daemon $proc started.");

//order by modification time
asort($task, SORT_NUMERIC);

//execute tasks
foreach ($task AS $file => $time)
{
    $_file = $path . $file;

    register_shutdown_function(function() use (&$lock_list, $path, $file, $task, &$flag, &$error)
    {
        $_file = $path . $file;
        if (! is_file($_file)) return;

        //task lock
        $lock  = "$_file.lock";
        $hfile = fopen($lock, "c");
        if ($hfile === FALSE) return;
        if (! flock($hfile, LOCK_EX | LOCK_NB)) { fclose($hfile); return; };

        if (! is_file($_file) || filesize($_file) === 0) { fclose($hfile); return; }; //already completed
        $lock_list[$lock] = $hfile;

        //check next task
        if ($flag)
        {
            $flag = FALSE;
            foreach ($task AS $file => $_)
            {
                $next = $path . $file;
                if ($next === $_file) continue;
                if (! is_file($next) || filesize($next) === 0) continue;

                //try advisory lock
                $_hfile = fopen("$next.lock", "c");
                if ($_hfile === FALSE) continue;

                //advisory lock
                $_flag = flock($_hfile, LOCK_EX | LOCK_NB);

                //release lock
                if ($_flag) flock($_hfile, LOCK_UN);
                fclose($_hfile);

                //start additional daemon
                if ($_flag) { cms_daemon_run(TRUE); break; };
            };
        };

        //explicit invalidation required because past
        //modification time is used for priorization
        if (function_exists("opcache_invalidate"))
            opcache_invalidate($_file, true);

        set_time_limit(600);
        gc_collect_cycles();

        try
        {
            //load task script
            include($_file);
        }
        catch (\Throwable $exception)
        {
            $error[$_file] = $exception->getMessage();

            cms_error(
                E_USER_ERROR,
                $exception->getMessage(),
                $exception->getFile(),
                $exception->getLine());
        };
    });

    //clear task
    register_shutdown_function(function() use (&$lock_list, $path, $file, $time, &$error)
    {
        $_file = $path . $file;
        $lock  = "$_file.lock";
        if (! isset($lock_list[$lock])) return;

        if (! is_file($_file)) return;
        if ($time === 1)       unlink($_file);
        else                   file_put_contents($_file, "");

        //release task lock
        fclose($lock_list[$lock]);
        unset($lock_list[$lock]);
        if (is_file($lock)) unlink($lock);

        //error message
        if (isset($error[$_file]))
        {
            cms_daemon_status("Task $file failed: " . $error[$_file]);
            unset($error[$_file]);
            return;
        };

        cms_daemon_status("Task $file completed.");
    });
};

//release lock
register_shutdown_function(function() use ($lock, $hfile, $proc)
{
    touch($lock);
    flock($hfile, LOCK_UN);
    fclose($hfile);

    cms_daemon_status("Daemon $proc finished.");
});

exit();

})();