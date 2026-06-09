<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   FILEMANAGER FUNCTIONS
//==================================================================================================

function filemanager_flexview_compare($value1,
                                      $value2)
{
    $dir1 = $value1[-1] === "/";
    $dir2 = $value2[-1] === "/";
    if ($dir1 !== $dir2) return $dir1 ? -1 : 1;

    $ext1 = (string)strrchr($value1, ".");
    $ext2 = (string)strrchr($value2, ".");
    if ($ext1 !== $ext2) return $ext1 <=> $ext2;

    return utf8_strcasecmp($value1, $value2);
};

//--------------------------------------------------------------------------------------------------

function filemanager_sort(&$array)
{
    uasort($array, __NAMESPACE__ . "\\filemanager_flexview_compare");
};

//--------------------------------------------------------------------------------------------------

function filemanager_flexview($root = NULL,
                              $path = NULL)
{
    if ($root == NULL)                   $root = CMS_ROOT_PATH;
    if ($path == NULL)                   $path = $root;
    if (! $hdir[$root] = opendir($root)) return FALSE;
    $_path    = $root;

    $flexview = new flexview();
    while (TRUE)
    {
        while (
                ($file_object = readdir($hdir[$root]))
                &&
                (
                  ($file_object === ".") || ($file_object === "..")
                )
              );

        if (! $file_object)
        {
            closedir($hdir[$root]);

            uksort($flexview->object[$parent], __NAMESPACE__ . "\\filemanager_flexview_compare");

            if (streq($root, $_path)) break;
            $root = dirname($root) . "/";

            continue;
        };

        if (streq($root, $_path)) $parent = "";
        else                      $parent = $root;

        $_root = "$root$file_object";

        if (is_dir($_root))
        {
            $_root .= "/";

            if ($hdir[$_root] = opendir($_root))
            {
                $flexview->set(
                    $_root,
                    ["name"  => $file_object,
                     "#type" => "container"],
                    $parent);

                if (streq(substr($path, 0, strlen($_root)), $_root))
                    $root .= "$file_object/";
            };
        }
        else
        {
            $extension = file_extension($file_object);
            $flexview->set(
                $root . $file_object,
                ["name"     => $file_object,
                 "#type"    => "file",
                 "#subtype" => $extension],
                $parent);
        };
    };

    return $flexview;
};

//--------------------------------------------------------------------------------------------------

function filemanager_flexview_directory($root = NULL,
                                        $path = NULL)
{
    if (! $root)                         $root = CMS_ROOT_PATH;
    if (! $path)                         $path = $root;
    if (! $hdir[$root] = opendir($root)) return FALSE;
    $_path = $root;

    $flexview = new flexview();
    while (TRUE)
    {
        while (
                ($file_object = readdir($hdir[$root]))
                &&
                (
                  ($file_object === ".") || ($file_object === "..")
                  ||
                  (! is_dir($root . $file_object))
                )
              );

        if (! $file_object)
        {
            closedir($hdir[$root]);

            uksort($flexview->object[$parent], __NAMESPACE__ . "\\filemanager_flexview_compare");

            if (streq($root, $_path)) break;
            $root = dirname($root) . "/";

            continue;
        };

        if (streq($root, $_path)) $parent = "";
        else                      $parent = $root;

        $_root = "$root$file_object/";

        if ($hdir[$_root] = opendir($_root))
        {
            $flexview->set(
                $_root,
                ["name"  => $file_object,
                 "#type" => "container"],
                $parent);

            if (streq(substr($path, 0, strlen($_root)), $_root))
                $root .= "$file_object/";
        };
    };

    return $flexview;
};

//--------------------------------------------------------------------------------------------------

function filemanager_get_select($root = NULL)
{
    if (! $root)                         $root = CMS_ROOT_PATH;
    if (! $hdir[$root] = opendir($root)) return FALSE;

    $i                                = 0;
    $c                                = strlen(CMS_ROOT_PATH);
    $path                             = $root;
    $array[(string)substr($path, $c)] = $root;

    while (TRUE)
    {
        while (
                ($file_object = readdir($hdir[$path]))
                &&
                (
                  ($file_object === ".") || ($file_object === "..")
                  ||
                  (! is_dir($path . $file_object))
                )
              );

        if (! $file_object)
        {
            closedir($hdir[$path]);
            if (streq($path, $root)) break;
            $path  = dirname($path) . "/";
            $i    -= 2;

            continue;
        };

        $path = "$path$file_object/";

        if ($hdir[$path] = opendir($path))
        {
            $file_object  = str_repeat("\xE2\x80\x83", $i) . $file_object;
            $_file_object = $file_object;
            $_i           = 1;

            while (isset($array[$_file_object]))
                $_file_object = "$file_object (" . $_i++ . ")";

            $array[$_file_object]  = $path;
            $i                    += 2;
        };
    };

    return $array;
};

//--------------------------------------------------------------------------------------------------

function filemanager_copy($source_path,
                          $target_path)
{
    //prevents infinite recursion when copying source into itself

    static $flag = TRUE;

    if ($flag)
    {
        ice_check_path($source_path);
        ice_check_path($target_path);
    };

    if (is_dir($source_path))
    {
        $source_path = rtrim($source_path, "/") . "/";
        $target_path = rtrim($target_path, "/") . "/";

        if (! $hdir = opendir($source_path)) return FALSE;

        $stack = [];

        while (($file_object = readdir($hdir)) !== FALSE)
        {
            if (($file_object === ".") || ($file_object === "..")) continue;
            array_push($stack, $file_object);
        };

        closedir($hdir);

        if (! is_dir($target_path))
        {
            $mode = fileperms($source_path) & 0777;
            if (! \mkdir($target_path, $mode)) return FALSE;
        };

        $success = TRUE;
        $flag    = FALSE;

        while (($file_object = array_pop($stack)) !== NULL)
            $success &= filemanager_copy($source_path . $file_object,
                                         $target_path . $file_object);

        $flag = TRUE;
        return $success;
    };

    if (is_file($target_path)) return FALSE;

    $success = FALSE;

    if ($hfile_source = fopen($source_path, "rb"))
    {
        if ($hfile_target = fopen($target_path, "wb"))
        {
            flock($hfile_source, LOCK_SH);
            flock($hfile_target, LOCK_EX);

            $success = stream_copy_to_stream($hfile_source, $hfile_target) !== FALSE;
            fclose($hfile_target);

            if ($success) chmod($target_path, fileperms($source_path) & 0777);
            else          unlink($target_path);
        };

        fclose($hfile_source);
    };

    return $success;
};

//--------------------------------------------------------------------------------------------------

function filemanager_delete($path)
{
    if (is_dir($path))
    {
        $path = rtrim($path, "/") . "/";
        if (! $hdir = opendir($path)) return FALSE;

        $success = TRUE;

        while (($file_object = readdir($hdir)) !== FALSE)
        {
            if (($file_object === ".") || ($file_object === "..")) continue;
            $success &= filemanager_delete($path . $file_object);
        };

        closedir($hdir);

        if (! rmdir($path)) return FALSE;
        return $success;
    };

    return unlink($path);
};

//--------------------------------------------------------------------------------------------------

function filemanager_gzcompress($source,
                                $target = NULL)
{
    $flag = FALSE;

    if (
         is_file($source)
         &&
         ($hfile_source = fopen($source, "rb"))
       )
    {
        if (! $target)                         $target  = $source;
        if (! preg_match("/\.gz$/i", $target)) $target .= ".gz";

        if ($hfile_target = gzopen($target, "wb9"))
        {
            flock($hfile_source, LOCK_SH);
            flock($hfile_target, LOCK_EX);

            while ($buffer = fread($hfile_source, 65536))
                gzwrite($hfile_target, $buffer);
            gzclose($hfile_target);

            $flag = TRUE;
        };

        fclose($hfile_source);
    };

    return $flag ? $target : FALSE;
};

//--------------------------------------------------------------------------------------------------

function filemanager_gzdecompress($source,
                                  $target = NULL)
{
    $flag = FALSE;

    if (
         is_file($source)
         &&
         preg_match("/\.gz$/i", $source)
         &&
         ($hfile_source = gzopen($source, "rb"))
       )
    {
        if (! $target) $target = substr($source, 0, -3);

        if ($hfile_target = fopen($target, "wb"))
        {
            flock($hfile_source, LOCK_SH);
            flock($hfile_target, LOCK_EX);

            while ($buffer = gzread($hfile_source, 65536))
                fwrite($hfile_target, $buffer);
            fclose($hfile_target);

            $flag = TRUE;
        };

        gzclose($hfile_source);
    };

    return $flag ? $target : FALSE;
};