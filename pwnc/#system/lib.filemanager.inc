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
    if ($ext1 !== $ext2) return strnatcasecmp($ext1, $ext2);

    return strnatcasecmp($value1, $value2);
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
    $root        ??= CMS_ROOT_PATH;
    $hdir[$root]   = opendir($root);
    if ($hdir[$root] === FALSE) return FALSE;

    $path  ??= $root;
    $_path   = $root;

    $flexview = new flexview();
    while (TRUE)
    {
        while (
                (($file_object = readdir($hdir[$root])) !== FALSE)
                &&
                (
                  ($file_object === ".") || ($file_object === "..")
                )
              );

        if ($file_object === FALSE)
        {
            closedir($hdir[$root]);

            if (! isset($parent)) break;
            uksort($flexview->object[$parent], __NAMESPACE__ . "\\filemanager_flexview_compare");

            if (streq($root, $_path)) break;
            $root = dirname($root) . "/";

            continue;
        };

        if (streq($root, $_path)) $parent = "";
        else                      $parent = $root;

        $_root = $root . $file_object;

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
    $root        ??= CMS_ROOT_PATH;
    $hdir[$root]   = opendir($root);
    if ($hdir[$root] === FALSE) return FALSE;

    $path  ??= $root;
    $_path   = $root;

    $flexview = new flexview();
    while (TRUE)
    {
        while (
                (($file_object = readdir($hdir[$root])) !== FALSE)
                &&
                (
                  ($file_object === ".") || ($file_object === "..")
                  ||
                  ! is_dir($root . $file_object)
                )
              );

        if ($file_object === FALSE)
        {
            closedir($hdir[$root]);

            if (! isset($parent)) break;
            uksort($flexview->object[$parent], __NAMESPACE__ . "\\filemanager_flexview_compare");

            if (streq($root, $_path)) break;
            $root = dirname($root) . "/";

            continue;
        };

        if (streq($root, $_path)) $parent = "";
        else                      $parent = $root;

        $_root = "$root$file_object/";

        if (($hdir[$_root] = opendir($_root)) !== FALSE)
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
    $root        ??= CMS_ROOT_PATH;
    $hdir[$root]   = opendir($root);
    if ($hdir[$root] === FALSE) return FALSE;

    $i                                = 0;
    $c                                = strlen(CMS_ROOT_PATH);
    $path                             = $root;
    $array[(string)substr($path, $c)] = $root;

    while (TRUE)
    {
        while (
                (($file_object = readdir($hdir[$path])) !== FALSE)
                &&
                (
                  ($file_object === ".") || ($file_object === "..")
                  ||
                  ! is_dir($path . $file_object)
                )
              );

        if ($file_object === FALSE)
        {
            closedir($hdir[$path]);
            if (streq($path, $root)) break;
            $path  = dirname($path) . "/";
            $i    -= 2;
            continue;
        };

        $path = "$path$file_object/";

        if (($hdir[$path] = opendir($path)) !== FALSE)
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

function filemanager_collect_recursive($source,
                                       $base = NULL)
{
    if (! is_array($source)) $source = [$source];
    $base   = rtrim($base ?? CMS_ROOT_PATH, "/") . "/";
    $length = strlen($base);

    //collect all unique paths
    $list = [];

    foreach ($source AS $path)
    {
        if (strncmp($path, $base, $length) !== 0) continue; //not within base path
        if (! file_exists($path))                 continue; //no file object

        switch (filetype($path))
        {
        case "file":

            $list[substr($path, $length)] = FALSE;
            break;

        case "dir":

            $path = rtrim($path, "/") . "/";
            $list[substr($path, $length)] = TRUE;

            $iterator = new \RecursiveIteratorIterator(
                new \RecursiveDirectoryIterator(
                    $path, \RecursiveDirectoryIterator::SKIP_DOTS),
                \RecursiveIteratorIterator::SELF_FIRST,
                \RecursiveIteratorIterator::CATCH_GET_CHILD);

            foreach ($iterator AS $element)
            {
                $_path = str_replace("\\", "/", $element->getPathname());
                $_path = substr($_path, $length); //relative to base

                $element->isDir() ? $list[$_path . "/"] = TRUE : $list[$_path] = FALSE;
            };
        };
    };

    //order by key length so directories always come before their content
    uksort($list, function($a, $b) { return strlen($a) - strlen($b); });

    return $list;
};

//--------------------------------------------------------------------------------------------------

function filemanager_copy($source,
                          $target,
                          $base = NULL)
{
    $target = rtrim($target, "/") . "/";
    $base   = rtrim($base ?? CMS_ROOT_PATH, "/") . "/";

    //collect all unique paths
    $list = filemanager_collect_recursive($source, $base);

    //process
    $replace = [];
    $copied  = [];

    foreach ($list AS $path => $is_dir)
    {
        $path_source = $base . $path;
        $path_target = $target . $path;

        //apply replacement
        foreach ($replace AS $from => $to)
        {
            $length = strlen($from);
            if (strncmp($path_target, $from, $length) === 0)
            {
                if ($to === FALSE) continue 2; //skip

                //replace
                $path_target = $to . substr($path_target, $length);
                break;
            };
        };

        //directory
        if ($is_dir)
        {
            //find a useable name
            $_path_target = unique_filename($path_target);
            if (nstreq($path_target, $_path_target))

                //use new name and memorize replacement
                $replace[$path_target] = $_path_target;

            //create directory with original permission
            $mode = fileperms($path_source) & 0777;
            if (\mkdir($_path_target, $mode))

                //memorize new directory
                $copied[] = $_path_target;

            else //error

                //skip directory if not created
                $replace[$path_target] = FALSE;

            continue;
        };

        //do not overwrite
        $path_target = unique_filename($path_target);

        if ($hfile_source = fopen($path_source, "rb"))
        {
            if ($hfile_target = fopen($path_target, "wb"))
            {
                flock($hfile_source, LOCK_SH);
                flock($hfile_target, LOCK_EX);

                $success = stream_copy_to_stream($hfile_source, $hfile_target) !== FALSE;
                fclose($hfile_target);

                if ($success)
                {
                    chmod($path_target, fileperms($path_source) & 0777);
                    $copied[] = $path_target; //memorize new file
                }
                else //error
                {
                    unlink($path_target);
                };
            };

            fclose($hfile_source);
        };
    };

    return (count($copied) === 0) ? FALSE : $copied;
};

//--------------------------------------------------------------------------------------------------

function filemanager_move($source,
                          $target,
                          $base = NULL)
{
    if (! is_array($source)) $source = [$source];
    $target   = rtrim($target, "/") . "/";
    $base     = rtrim($base ?? CMS_ROOT_PATH, "/") . "/";
    $len_base = strlen($base);

    //cluster paths and sort by length
    //directories will come before contained files
    sort($source);

    $path_prev = NULL;
    $len_prev  = 0;
    $moved     = [];

    foreach ($source AS $path_source)
    {
        if (strncmp($path_source, $base, $len_base) !== 0) continue; //not within base path

        if (
             ($path_prev !== NULL)
             &&
             (strncmp($path_source, $path_prev, $len_prev) === 0)
           )
            continue; //within previous path

        //memorize
        if (substr($path_source, -1) === "/")
        {
            $path_prev = $path_source;
            $len_prev  = strlen($path_prev);
        };

        $path_target = $target . substr($path_source, $len_base);
        $path_target = unique_filename($path_target);

        //move
        if (rename($path_source, $path_target))

            //memorize moved file object
            $moved[] = $path_target;
    };

    return (count($moved) === 0) ? FALSE : $moved;
};

//--------------------------------------------------------------------------------------------------

function filemanager_delete($list)
{
    $list = filemanager_collect_recursive($list);
    if (empty($list)) return TRUE; //nothing to delete

    $list    = array_reverse($list, TRUE);
    $deleted = [];

    foreach ($list AS $path => $is_dir)
    {
        $path = CMS_ROOT_PATH . $path;
        if ($is_dir ? rmdir($path) : unlink($path))

            //memorize deleted file object
            $deleted[] = $path;
    };

    return (count($deleted) === 0) ? FALSE : $deleted;
};

//--------------------------------------------------------------------------------------------------

function filemanager_zip($source,
                         $target,
                         $base = NULL)
{
    $base = rtrim($base ?? CMS_ROOT_PATH, "/") . "/";

    //collect all unique paths
    $list = filemanager_collect_recursive($source, $base);

    //target file
    $temp = $target . "." . unique_id();

    //create archive
    $zip = new \ZipArchive();
    if ($zip->open($temp, \ZipArchive::CREATE | \ZipArchive::OVERWRITE) !== TRUE) return FALSE;

    foreach ($list AS $path => $is_dir)
    {
        if ($is_dir)
        {
            $path = rtrim($path, "/") . "/";
            $zip->addEmptyDir($path);
            continue;
        };

        $zip->addFile($base . $path, $path);
    };

    //rename temporary file
    if ($zip->close())
    {
        $target = unique_filename($target);
        if (rename($temp, $target)) return $target;
    };

    if (file_exists($temp)) unlink($temp);
    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function filemanager_unzip($source,
                           $target = NULL)
{
    $target ??= unique_filename(dirname($source) . "/" . file_name($source)) . "/";
    if (! mkpath($target)) return FALSE;

    $zip = new \ZipArchive();
    if ($zip->open($source) !== TRUE) return FALSE;

    //invalid path components
    $regex = ["^[A-Za-z]:",           //win drive
              "^//",                  //net drive
              "^[+\-.0-9A-Za-z]+://", //protocol wrapper
              "(?:^|/)\.\.(?:/|$)"];  //relative path
    $regex = "~(?:" . implode("|", $regex) . ")~";

    //process
    $error = FALSE;

    for ($i = 0, $c = $zip->numFiles; $i < $c; $i++)
    {
        //read path
        $path_source = $zip->getNameIndex($i);
        if ($path_source === FALSE) { $error = TRUE; continue; };

        //skip invalid paths
        $_path_source = str_replace("\\", "/", $path_source);
        if (preg_match($regex, $_path_source)) { $error = TRUE; continue; };

        //create directory
        $is_dir      = substr($_path_source, -1) === "/";
        $path_target = $target . $_path_source;
        $path        = $is_dir ? $path_target : dirname($path_target) . "/";

        if (! mkpath($path)) { $error = TRUE; continue; };

        if ($is_dir) continue; //no file to extract

        //extract file
        //using stream, much slower but error tolerant
        $hfile_source = $zip->getStream($path_source);
        if ($flag = ($hfile_source !== FALSE))
        {
            $hfile_target = fopen($path_target, "wb");
            if ($flag = ($hfile_target !== FALSE))
            {
                stream_copy_to_stream($hfile_source, $hfile_target, 1048576);
                fclose($hfile_target);
            };

            fclose($hfile_source);
        };

        if (! $flag) $error = TRUE;
    };

    $zip->close();

    return $error ? FALSE : $target;
};