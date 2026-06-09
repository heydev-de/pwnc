<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   UPDATE CLASS
//==================================================================================================

//constants
define("CMS_UPDATE_PATH",            CMS_ROOT_PATH . "#update/");
define("CMS_UPDATE_URL_VERSION",     "https://raw.githubusercontent.com/heydev-de/pwnc/refs/heads/main/nuos/version.txt");
define("CMS_UPDATE_URL_ARCHIVE",     "https://github.com/heydev-de/pwnc/archive/main.zip");
define("CMS_UPDATE_TOKEN",           "");
define("CMS_UPDATE_CHECK_INTERVAL",  3600); //hourly

define("CMS_UPDATE_STATUS_ERROR",    -1);
define("CMS_UPDATE_STATUS_NONE",      0);
define("CMS_UPDATE_STATUS_DONE",      1);
define("CMS_UPDATE_STATUS_BACKUP",    2);
define("CMS_UPDATE_STATUS_DOWNLOAD",  3);
define("CMS_UPDATE_STATUS_INSTALL",   4);
define("CMS_UPDATE_STATUS_CLEANUP",   5);

class update
{

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct()
    {
        mkpath(CMS_UPDATE_PATH);
    }

//methods
//--------------------------------------------------------------------------------------------------

    function log($text,
                 $reset   = FALSE,
                 $newline = TRUE)
    {
        return file_put_contents(
            CMS_UPDATE_PATH . "update.log",
            (($newline && (! $reset)) ? "\n" : "") . $text,
            ($reset ? 0 : FILE_APPEND) | LOCK_EX) !== FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function get_log()
    {
        $path = CMS_UPDATE_PATH . "update.log";
        return is_file($path) ? file_get_contents($path) : "";
    }

//--------------------------------------------------------------------------------------------------

    function progress()
    {
        return $this->log("…", FALSE, FALSE);
    }

//--------------------------------------------------------------------------------------------------

    function status($value = CMS_UPDATE_STATUS_NONE,
                    $text  = "")
    {
        $reset = ($value !== CMS_UPDATE_STATUS_NONE) &&
                 ($this->get_status() === CMS_UPDATE_STATUS_NONE);
        $flag  = nstre($text) ? $this->log($text, $reset) : TRUE;

        if (file_put_contents(CMS_UPDATE_PATH . "update.status",
                              $value,
                              LOCK_EX) === FALSE)
            return FALSE;

        return $flag;
    }

//--------------------------------------------------------------------------------------------------

    function get_status()
    {
        $path = CMS_UPDATE_PATH . "update.status";
        if (! is_file($path)) return CMS_UPDATE_STATUS_NONE;

        $hfile = fopen($path, "rb");
        if ($hfile === FALSE) return CMS_UPDATE_STATUS_NONE;

        if (! flock($hfile, LOCK_SH))
        {
            fclose($hfile);
            return CMS_UPDATE_STATUS_NONE;
        };

        $status = (int)stream_get_contents($hfile);
        flock($hfile, LOCK_UN);
        fclose($hfile);
        return $status;
    }

//--------------------------------------------------------------------------------------------------

    function error($text)
    {
        return $this->status(CMS_UPDATE_STATUS_ERROR, $text);
    }

//--------------------------------------------------------------------------------------------------

    function available($enforce = FALSE)
    {
        $path    = CMS_UPDATE_PATH . "update.check";
        $time    = 0;
        $_time   = time() - CMS_UPDATE_CHECK_INTERVAL;
        $version = "";

        if (is_file($path))
        {
            $time    = filemtime($path);
            $version = file_get_contents($path);
        };

        if ($enforce || ($time < $_time))
        {
            $option = [
                "http" => [
                    "header" => "User-Agent: " . CMS_USER_AGENT . "\r\n" .
                                (nstre(CMS_UPDATE_TOKEN) ?
                                "Authorization: Bearer " . CMS_UPDATE_TOKEN . "\r\n" : ""),
                    "method" => "GET"
                ]
            ];

            $context = stream_context_create($option);
            $version = file_get_contents(CMS_UPDATE_URL_VERSION, FALSE, $context);

            file_put_contents($path, $version, LOCK_EX);
        };

        if ($version === FALSE) return FALSE;
        return version_compare(CMS_VERSION, $version, "<") ? $version : NULL;
    }

//--------------------------------------------------------------------------------------------------

    function start($skip_backup = FALSE)
    {
        if ($this->get_status() !== CMS_UPDATE_STATUS_NONE) return FALSE;

        if ($skip_backup) $this->status(CMS_UPDATE_STATUS_DONE, CMS_L_UPDATE_026);
        $flag = ($skip_backup || $this->backup()) && $this->download() && $this->install() && $this->cleanup();
        $this->status(CMS_UPDATE_STATUS_DONE, CMS_L_UPDATE_025);

        return $flag;
    }

//--------------------------------------------------------------------------------------------------

    function backup()
    {
        if (! in_array($this->get_status(), [CMS_UPDATE_STATUS_NONE, CMS_UPDATE_STATUS_DONE])) return FALSE;

        $this->status(CMS_UPDATE_STATUS_BACKUP, CMS_L_UPDATE_027);

        $mysql = new mysql();
        if (! $mysql->backup())
        {
            $this->error(CMS_L_UPDATE_028);
            return FALSE;
        };

        $this->log(CMS_L_UPDATE_029 . "\n" . CMS_L_UPDATE_009);

        $path = CMS_UPDATE_PATH . "backup/";
        if (! mkpath($path))
        {
            $this->error(sprintf(CMS_L_UPDATE_001, $path));
            return FALSE;
        };

        $include     = [CMS_ROOT_PATH];
        $exclude     = [CMS_UPDATE_PATH,
                        CMS_ROOT_PATH . ".git/",
                        CMS_ROOT_PATH . ".github/",
                        CMS_DATA_PATH . "#cache/",
                        CMS_DATA_PATH . "#content/cache/",
                        CMS_DATA_PATH . "#daemon/",
                        CMS_DATA_PATH . "captcha/",
                        CMS_DATA_PATH . "image/cache/"];
        $exclude     = array_flip($exclude);
        $path_temp   = $path . "backup.temp";
        $path_target = $path . "backup.zip";
        $path_backup = $path . "_backup.zip";
        $length      = strlen(CMS_ROOT_PATH);
        $error       = FALSE;
        $i           = 0;

        $zip = new \ZipArchive();
        if ($zip->open($path_temp, \ZipArchive::CREATE | \ZipArchive::OVERWRITE) !== TRUE)
        {
            $this->error(sprintf(CMS_L_UPDATE_002, $path_temp));
            return FALSE;
        };

        foreach ($include AS $path)
        {
            if (($hdir = opendir($path)) === FALSE)
            {
                $this->error(sprintf(CMS_L_UPDATE_003, $path));
                $error = TRUE;
                continue;
            };

            $_hdir = [];

            while (TRUE)
            {
                while (
                        (($file_object = readdir($hdir)) !== FALSE)
                        &&
                        (($file_object === ".") || ($file_object === ".."))
                      );

                if ($file_object === FALSE)
                {
                    closedir($hdir);

                    $hdir = array_pop($_hdir);
                    if ($hdir === NULL) break;

                    $path = dirname($path) . "/";
                    continue;
                };

                $_path = $path . $file_object;
                if (is_dir($_path))
                {
                    $_path .= "/";
                    if (isset($exclude[$_path])) continue;
                    if (($__hdir = opendir($_path)) === FALSE)
                    {
                        $this->error(sprintf(CMS_L_UPDATE_003, $_path));
                        $error = TRUE;
                        break;
                    };

                    $_hdir[] = $hdir;
                    $hdir    = $__hdir;
                    $path    = $_path;

                    if (! $zip->addEmptyDir(substr($path, $length)))
                    {
                        $this->error(sprintf(CMS_L_UPDATE_004, $path, $path_temp));
                        $error = TRUE;
                        break;
                    };

                    continue;
                };

                if (isset($exclude[$_path])) continue;
                if (! $zip->addFile($_path, substr($_path, $length)))
                {
                    $this->error(sprintf(CMS_L_UPDATE_005, $_path, $path_temp));
                    $error = TRUE;
                    break;
                };

                if (++$i === 100)
                {
                    set_time_limit(600);
                    $this->progress();
                    $i = 0;
                };
            };
        };

        if (! $zip->close())
        {
            $this->error(sprintf(CMS_L_UPDATE_006, $path_temp));
            return FALSE;
        };

        if (is_file($path_target))
        {
            if (is_file($path_backup))
            {
                if (! unlink($path_backup))
                {
                    $this->error(sprintf(CMS_L_UPDATE_007, $path_backup));
                    return FALSE;
                };
            };

            if (! rename($path_target, $path_backup))
            {
                $this->error(sprintf(CMS_L_UPDATE_008, $path_target, $path_backup));
                return FALSE;
            };
        };

        if (! rename($path_temp, $path_target))
        {
            $this->error(sprintf(CMS_L_UPDATE_008, $path_temp, $path_target));
            return FALSE;
        };

        $error ? $this->error(CMS_L_UPDATE_011) : $this->status(CMS_UPDATE_STATUS_DONE, CMS_L_UPDATE_010);
        return ! $error;
    }

//--------------------------------------------------------------------------------------------------

    function download()
    {
        if (! in_array($this->get_status(), [CMS_UPDATE_STATUS_NONE, CMS_UPDATE_STATUS_DONE])) return FALSE;

        $this->status(CMS_UPDATE_STATUS_DOWNLOAD, CMS_L_UPDATE_012);

        if (! mkpath(CMS_UPDATE_PATH))
        {
            $this->error(sprintf(CMS_L_UPDATE_001, CMS_UPDATE_PATH));
            return FALSE;
        };

        $option = [
            "http" => [
                "header"        => "User-Agent: " . CMS_USER_AGENT . "\r\n" .
                                   (nstre(CMS_UPDATE_TOKEN) ?
                                   "Authorization: Bearer " . CMS_UPDATE_TOKEN . "\r\n" : ""),
                "method"        => "GET",
                "ignore_errors" => TRUE,
                "timeout"       => 60
            ]
        ];

        $context = stream_context_create($option);
        $hfile   = fopen(CMS_UPDATE_URL_ARCHIVE, "rb", FALSE, $context);
        if ($hfile === FALSE)
        {
            $this->error(CMS_L_UPDATE_013);
            return FALSE;
        };

        $path   = CMS_UPDATE_PATH . "update.zip";
        $_hfile = fopen($path, "wb");
        if ($_hfile === FALSE)
        {
            fclose($hfile);
            $this->error(sprintf(CMS_L_UPDATE_002, $path));
            return FALSE;
        };

        if (! flock($_hfile, LOCK_EX))
        {
            fclose($hfile);
            fclose($_hfile);
            $this->error(sprintf(CMS_L_UPDATE_014, $path));
            return FALSE;
        };

        $size  = 8192;
        $error = FALSE;
        $i     = 0;

        while (! feof($hfile))
        {
            if (
                 (($buffer = fread($hfile, $size)) === FALSE)
                 ||
                 (fwrite($_hfile, $buffer) === FALSE)
               )
            {
                $error = TRUE;
                break;
            };

            if (++$i === 500)
            {
                set_time_limit(600);
                $this->progress();
                $i = 0;
            };
        };

        fclose($hfile);
        fflush($_hfile);
        flock($_hfile, LOCK_UN);
        fclose($_hfile);

        if ($error)
        {
            unlink($path);
            $this->error(CMS_L_UPDATE_015);
            return FALSE;
        };

        $this->status(CMS_UPDATE_STATUS_DONE, CMS_L_UPDATE_016);
        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function install()
    {
        if (! in_array($this->get_status(), [CMS_UPDATE_STATUS_NONE, CMS_UPDATE_STATUS_DONE])) return FALSE;

        $this->status(CMS_UPDATE_STATUS_INSTALL, CMS_L_UPDATE_019);

        $path_source = CMS_UPDATE_PATH . "update.zip";
        $path_target = CMS_UPDATE_PATH . "new/";
        $base        = "";
        $path_backup = CMS_UPDATE_PATH . "old/" . date("Y-m-d_H-i-s") . "_" . unique_id() . "/";

        //create extraction directory
        if (! mkpath($path_target))
        {
            $this->error(sprintf(CMS_L_UPDATE_001, $path_target));
            return FALSE;
        };

        //create rollback directory
        if (! mkpath($path_backup))
        {
            $this->error(sprintf(CMS_L_UPDATE_001, $path_backup));
            return FALSE;
        };

        //open update archive
        $zip = new \ZipArchive();
        if ($zip->open($path_source, \ZipArchive::CHECKCONS) !== TRUE)
        {
            $this->error(sprintf(CMS_L_UPDATE_017, $path_source));
            return FALSE;
        };

        //get update base directory
        if (($base = $zip->getNameIndex(0)) === FALSE)
        {
            $zip->close();
            $this->error(sprintf(CMS_L_UPDATE_022, $path_source));
            return FALSE;
        };

        //check update base directory
        if (substr($base, -1) !== "/")
        {
            $this->error(CMS_L_UPDATE_033);
            return FALSE;
        };

        //iterate specified update directories
        $replace = ["pwnc" => NULL, "nuos" => NULL];
        $error   = FALSE;
        $c       = $zip->numFiles;

        foreach ($replace AS $path => $_)
        {
            $_path  = $base . $path . "/";
            $length = strlen($_path);
            $flag   = FALSE;

            //extract files from specified update directories
            for ($i = 1; $i < $c; $i++)
            {
                $file_object = $zip->getNameIndex($i);

                if (strncmp($file_object, $_path, $length) !== 0) continue;

                if (! $zip->extractTo($path_target, $file_object))
                {
                    $this->error(sprintf(CMS_L_UPDATE_018, $file_object, $path_source));
                    $error = TRUE;
                    break 2;
                };

                $flag = TRUE; //found
            };

            $replace[$path] = $flag;

            //move original to rollback directory
            if (
                 file_exists(CMS_ROOT_PATH . $path)
                 &&
                 (! rename(CMS_ROOT_PATH . $path, $path_backup . $path))
               )
            {
                $this->error(sprintf(CMS_L_UPDATE_008, CMS_ROOT_PATH . $path, $path_backup . $path));
                $error = TRUE;
                break;
            };

            //install new version
            if (
                 file_exists($path_target . $_path)
                 &&
                 (! rename($path_target . $_path, CMS_ROOT_PATH . $path))
               )
            {
                $this->error(sprintf(CMS_L_UPDATE_008, $path_target . $_path, CMS_ROOT_PATH . $path));
                $error = TRUE;
                break;
            };
        };

        $zip->close();

        //rollback on error
        if ($error)
        {
            $this->error(CMS_L_UPDATE_032);
            $_error = FALSE;

            //no access to rollback directory
            if (($hdir = opendir($path_backup)) === FALSE)
            {
                $this->error(sprintf(CMS_L_UPDATE_003, $path_backup));
                $_error = TRUE;
            }

            //restore original directories
            else
            {
                while (($file_object = readdir($hdir)) !== FALSE)
                {
                    if (($file_object === ".") || ($file_object === "..")) continue;
                    if (! rename($path_backup . $file_object, CMS_ROOT_PATH . $file_object))
                    {
                        $this->error(sprintf(CMS_L_UPDATE_031, CMS_ROOT_PATH . $file_object));
                        $_error = TRUE;
                    };
                };

                closedir($hdir);
            };

            if ($_error) $this->error(CMS_L_UPDATE_030);
        }

        //temporary migration phase handling
        elseif (! $replace["pwnc"])
        {
            //move /nuos/ to /pwnc/
            file_exists(CMS_ROOT_PATH . "nuos/") &&
            rename(CMS_ROOT_PATH . "nuos/", CMS_ROOT_PATH . "pwnc/") &&

            //move /pwnc/nuos/ to /nuos/ - redirects to /pwnc/
            file_exists(CMS_ROOT_PATH . "pwnc/nuos/") &&
            rename(CMS_ROOT_PATH . "pwnc/nuos/", CMS_ROOT_PATH . "nuos/");
        };

        $error ? $this->error(CMS_L_UPDATE_020) : $this->status(CMS_UPDATE_STATUS_DONE, CMS_L_UPDATE_021);
        return ! $error;
    }

//--------------------------------------------------------------------------------------------------

    function cleanup()
    {
        if (! in_array($this->get_status(), [CMS_UPDATE_STATUS_NONE, CMS_UPDATE_STATUS_DONE])) return FALSE;

        $this->status(CMS_UPDATE_STATUS_CLEANUP, CMS_L_UPDATE_023);

        $path_source = CMS_UPDATE_PATH . "update.zip";
        $path_target = CMS_UPDATE_PATH . "new/";
        $path_backup = CMS_UPDATE_PATH . "old/";

        if (is_file($path_source)) unlink($path_source);

        if (cms_load("filemanager"))
        {
            if (is_dir($path_target)) filemanager_delete($path_target);
            if (is_dir($path_backup)) filemanager_delete($path_backup);
        };

        $this->status(CMS_UPDATE_STATUS_DONE, CMS_L_UPDATE_024);
        return TRUE;
    }
};