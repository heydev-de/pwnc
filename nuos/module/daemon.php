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

//try advisory lock
$lock  = $path . "daemon.lock";
$hfile = fopen($lock, "c");
if ($hfile === FALSE) exit();

//advisory lock
if (! flock($hfile, LOCK_EX | LOCK_NB))
{
    fclose($hfile);
    exit();
};

//remove task available flag
$flag = $path . "daemon.flag";
if (is_file($flag)) unlink($flag);

//retrieve tasks
$list = scandir($path);
$list = array_diff($list, [".", "..", ".htaccess", "daemon.flag", "daemon.lock", "daemon.status"]);
$task = [];

foreach ($list AS $file)
{
    $_file = $path . $file;
    if (! is_file($_file))            continue;
    if (filesize($_file) === 0)       continue;
    if (substr($file, -4) === ".tmp") continue;
    $task[$file] = filemtime($_file);
};

if (empty($task)) exit(); //nothing to do

cms_daemon_status("Daemon run started. Processing " . count($task) . " tasks.");

//order by modification time
asort($task, SORT_NUMERIC);

//execute tasks
foreach ($task AS $file => $time)
{
    $_file = $path . $file;

    register_shutdown_function(function() use ($_file)
    {
        if (! is_file($_file)) return;

        //explicit invalidation required because past
        //modification time is used for priorization
        if (function_exists("opcache_invalidate"))
            opcache_invalidate($_file, true);

        set_time_limit(600);
        gc_collect_cycles();

        try
        {
            //load task script
            require($_file);
        }
        catch (\Throwable $exception)
        {
            //forward to error handling
            throw $exception;
        };
    });

    //clear task
    register_shutdown_function(function() use ($_file, $time)
    {
        if (! is_file($_file)) return;

        if ($time === 1) unlink($_file);
        else             file_put_contents($_file, "");

        cms_daemon_status("Task completed.");
    });
};

//release lock
register_shutdown_function(function() use ($lock, $hfile)
{
    touch($lock);
    flock($hfile, LOCK_UN);
    fclose($hfile);

    cms_daemon_status("Daemon run completed.");
});

exit();

})();