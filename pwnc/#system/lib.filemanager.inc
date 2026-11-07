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

                if (str_starts_with($path, $_root))
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

            if (str_starts_with($path, $_root))
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
        if (! str_starts_with($path, $base))          continue; //not within base path
        if (pta_check($path))                         continue; //path traversal
        if (! file_exists($path) && ! is_link($path)) continue; //no file object

        switch (filetype($path))
        {
        case "link":
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

                ($element->isDir() && ! $element->isLink()) ?
                $list[$_path . "/"] = TRUE : $list[$_path] = FALSE;
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

    //not within root path
    if (! str_starts_with($target, CMS_ROOT_PATH)) return FALSE;

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
            if (str_starts_with($path_target, $from))
            {
                if ($to === FALSE) continue 2; //skip

                //replace
                $path_target = $to . substr($path_target, strlen($from));
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

            //create parent directories and directory with original permission
            $mode = fileperms($path_source) & 0777;
            if (mkpath(dirname($_path_target)) && \mkdir($_path_target, $mode))

                //memorize new directory
                $copied[] = $_path_target;

            else //error

                //skip directory if not created
                $replace[$path_target] = FALSE;

            continue;
        };

        //linked directory or broken link
        if (! is_file($path_source)) continue;

        //do not overwrite
        $path_target = unique_filename($path_target);

        //create parent directories
        if (! mkpath(dirname($path_target))) continue;

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

function filemanager_base_path($list)
{
    $list = array_unique($list);

    sort($list); //directories come before contained files

    $result = [];
    $prev   = NULL;

    foreach ($list AS $path)
    {
        if (
             ($prev !== NULL)
             &&
             str_starts_with($path, $prev)
           )
            continue; //within previous directory

        if ($path[-1] === "/") $prev = $path;
        $result[] = $path;
    };

    return $result;
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
    $moved    = [];

    //not within root path
    if (! str_starts_with($target, CMS_ROOT_PATH)) return FALSE;

    //not within base path
    $source = array_filter($source, function($path) use ($base) { return str_starts_with($path, $base); });

    foreach (filemanager_base_path($source) AS $path_source)
    {
        $path_target = $target . substr($path_source, $len_base);
        $path_target = unique_filename($path_target);

        //create parent directories
        if (! mkpath(dirname($path_target))) continue;

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
    $gone    = function($path) { return ! file_exists($path) && ! is_link($path); };
    $deleted = array_values(array_filter((array)$list, $gone));
    $list    = array_reverse(filemanager_collect_recursive($list), TRUE);

    foreach ($list AS $path => $is_dir)
    {
        $path   = CMS_ROOT_PATH . $path;
        $remove = $is_dir || (DIRECTORY_SEPARATOR === "\\" && is_link($path) && is_dir($path));

        if ($remove ? rmdir($path) : unlink($path))

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

        //not a linked directory or broken link
        if (is_file($base . $path)) $zip->addFile($base . $path, $path);
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
    $zip = new \ZipArchive();
    if ($zip->open($source) !== TRUE) return FALSE;

    $target ??= unique_filename(dirname($source) . "/" . file_name($source)) . "/";
    if (! mkpath($target)) return FALSE;

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
        $is_dir      = str_ends_with($_path_source, "/");
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

class filemanager_filter
{

//properties
//--------------------------------------------------------------------------------------------------

    const FILE_LIMIT      = 16777216;
    const SNIPPET_LIMIT   = 3;
    const SNIPPET_CONTEXT = 50;

//methods
//--------------------------------------------------------------------------------------------------

    static function exclude($text = NULL)
    {
        $text ??= (new system())->getval("filemanager", "filter_exclude");
        $list   = [];

        foreach (preg_split("/\s*\r?\n\s*/", trim((string)$text)) AS $line)
        {
            $line = trim(real_path($line), "/");
            if ($line !== "") $list[] = CMS_ROOT_PATH . "$line/";
        };

        return $list;
    }

//--------------------------------------------------------------------------------------------------

    static function search($filter,
                           $regex,
                           $base    = NULL,
                           $exclude = [],
                           $replace = NULL,
                           $limit   = 10000)
    {
        $result = ["match" => [], "error" => 0, "regex" => "", "limit" => FALSE];

        //check regex
        $error = "";
        if (stre($filter) || ($regex && ! verify_regex($filter, $error)))
        {
            $result["regex"] = $error;
            return $result;
        };

        //convert literal search to regex
        if (! $regex)
        {
            $filter = self::pattern($filter);
            if ($replace !== NULL) $replace = addcslashes($replace, "\\$");
        };

        //base path
        $base   = $base ?? CMS_ROOT_PATH;
        $file   = is_file($base) ? basename($base) : NULL;
        $base   = ($file === NULL) ? rtrim($base, "/") . "/" : dirname($base) . "/"; pta_block($base);
        $length = strlen($base);

        //not within root path
        if (! str_starts_with($base, CMS_ROOT_PATH))
        {
            $result["error"]++;
            return $result;
        };

        //cache
        $cache_key  = "filemanager." . CMS_USER . (($replace === NULL) ? ".search" : ".replace");
        $cache      = cms_cache($cache_key);
        $hash       = hash32($filter . "\0" . $base . $file . "\0" . $replace);
        $valid      = is_array($cache) && (($cache["hash"] ?? NULL) === $hash);
        $cache_time = $valid ? $cache["time"] : 0;
        $_list      = $valid ? $cache["list"] : [];

        //exluded directories
        $_exclude = [];
        foreach ($exclude AS $path)
            if (! str_starts_with($base, $path)) $_exclude[$path] = TRUE;

        //get next file object
        $readdir = function($hdir)
        {
            while (($file_object = readdir($hdir)) !== FALSE) yield $file_object;
        };

        //search
        $stack = [$base];
        $list  = [];
        $time  = time();
        $found = 0;
        $read  = 0;

        while ($path = array_pop($stack))
        {
            set_time_limit(30); //per directory

            //not readable
            if (! is_readable($path) || (($hdir = opendir($path)) === FALSE))
            {
                $result["error"]++;
                continue;
            };

            foreach (($file === NULL) ? $readdir($hdir) : [$file] AS $file_object)
            {
                if (($file_object === ".") || ($file_object === "..")) continue;

                //check file object type
                $_path = $path . $file_object;
                $stat  = lstat($_path);
                if ($stat === FALSE) continue;

                switch ($type = $stat["mode"] & 0170000)
                {
                case 0040000: //directory

                    $_path .= "/";
                    if (isset($_exclude[$_path])) continue 2;

                    $stack[] = $_path;
                    break;

                case 0120000: //link

                    if (! is_file($_path)) continue 2;
                    $stat = stat($_path);
                    break;

                case 0100000: //file

                    break;

                default: //other

                    continue 2;
                };

                //name
                if (($match = self::match($file_object, $filter, NULL, 0)) === FALSE)
                {
                    $count = 0;
                    $result["error"]++;
                }
                else
                {
                    $count = $match["count"];
                };

                $snippet = [];

                if (

                //not a directory
                     ($type !== 0040000)
                     &&

                //not empty
                     (($size = $stat["size"]) > 0)
                     &&

                //less that maximum size
                     ($size <= self::FILE_LIMIT)
                   )
                {
                    $name  = substr($_path, $length);
                    $mtime = $stat["mtime"];
                    $cache = $_list[$name] ?? NULL;

                    //not modified
                    if (

                    //cache exists
                         isset($cache[3])
                         &&

                    //modification time equal
                         ($cache[0] === $mtime)
                         &&

                    //file size equal
                         ($cache[1] === $size)
                         &&

                    //modified before last full second of cache creation
                         ($mtime + 1 < $cache_time)
                       )
                    {
                        $list[$name]  = $cache;
                        $count       += $cache[2];
                        $snippet      = $cache[3];
                    }

                    //read from file
                    elseif (($text = read_text_file($_path)) !== FALSE)
                    {
                        $read++;
                        $match = ($text === NULL) ?
                                 ["count" => 0, "list" => []] :
                                 self::match($text, $filter, $replace);

                        if ($match !== FALSE)
                        {
                            $count       += $match["count"];
                            $snippet      = $match["list"] ? self::snippet($text, $match["list"]) : [];
                            $list[$name]  = [$mtime, $size, $match["count"], $snippet];
                        }
                        else
                        {
                            $result["error"]++;
                        };
                    }

                    //reading failed
                    else
                    {
                        $result["error"]++;
                    };
                };

                if ($count === 0) continue; //not found

                //file search limit exceeded
                if ($found >= $limit)
                {
                    $result["limit"] = TRUE;
                    closedir($hdir);
                    break 2;
                };

                //add result
                $result["match"][] =
                    ["path"    => rtrim($_path, "/"),
                     "count"   => $count,
                     "snippet" => $snippet];
                $found++;
            };

            closedir($hdir);
        };

        //write cache if modified
        if (($read > 0) || (count($list) !== count($_list)))
            cms_cache($cache_key, ["hash" => $hash, "time" => $time, "list" => $list], TRUE);

        return $result;
    }

//--------------------------------------------------------------------------------------------------

    static function replace($list,
                            $filter,
                            $regex,
                            $replace,
                            $base = NULL)
    {
        if (($filter === "") || ($regex && ! verify_regex($filter))) return FALSE;

        $base   = rtrim($base ?? CMS_ROOT_PATH, "/") . "/"; pta_block($base);
        $result = ["file" => 0, "count" => 0, "error" => 0];

        //convert literal search to regex
        if (! $regex)
        {
            $filter  = self::pattern($filter);
            $replace = addcslashes($replace, "\\$");
        };

        //not within root path
        if (! str_starts_with($base, CMS_ROOT_PATH)) return $result;

        foreach (array_unique($list) AS $path)
        {
            set_time_limit(30); //per file

            if (! str_starts_with($path, $base)) continue; //not within base path
            if (! is_file($path))                continue; //no file

            //skip links
            if (is_link($path))
                { $result["error"]++; continue; };

            //file too large
            if (filesize($path) > self::FILE_LIMIT)
                { $result["error"]++; continue; };

            //get content
            if (($text = read_text_file($path)) === FALSE)
                { $result["error"]++; continue; };
            if ($text === NULL) continue; //binary

            //replace
            $count = 0;
            $text  = preg_replace($filter, $replace, $text, -1, $count);

            //error
            if ($text === NULL)
                { $result["error"]++; continue; };

            //not found
            if ($count === 0) continue;

            //store
            if (! write_file($path, $text))
                { $result["error"]++; continue; };

            $result["file"]  += 1;
            $result["count"] += $count;
        };

        return $result;
    }

//--------------------------------------------------------------------------------------------------

    private static function pattern($filter)
    {
        //generalize line breaks
        return "/" . preg_replace("/\r\n?|\n/", "(?>\r\n?|\n)", preg_quote($filter, "/")) . "/";
    }

//--------------------------------------------------------------------------------------------------

    private static function match($text,
                                  $pattern,
                                  $replace       = NULL,
                                  $snippet_limit = self::SNIPPET_LIMIT)
    {
        $result = ["count" => 0, "list" => []];

        //count all matches
        $count = preg_match_all($pattern, $text);
        if ($count === FALSE) return FALSE;

        $result["count"] = $count;

        //extract snippets
        $found = preg_replace_callback(
            $pattern,
            function($match) use (&$result)
            {
                $result["list"][] = [$match[0][1], strlen($match[0][0])];
                return "";
            },
            $text, $snippet_limit, $_, PREG_OFFSET_CAPTURE);

        if ($found === NULL)   return FALSE;   //no match
        if ($replace === NULL) return $result; //no replace

        //replacement
        $last = $text;

        foreach ($result["list"] AS $i => [$pos, $length])
        {
            $next    = preg_replace($pattern, $replace, $text, $i + 1);
            $_length = strlen($last);
            $_pos    = $pos + $_length - strlen($text);
            $_length = strlen($next) - $_length + $length;
            $last    = $next;
            $result["list"][$i][2] = substr($last, $_pos, $_length);
        };

        return $result;
    }

//--------------------------------------------------------------------------------------------------

    private static function snippet($text,
                                    $list,
                                    $context = self::SNIPPET_CONTEXT)
    {
        $result = [];
        $size   = strlen($text);
        $break  = fn($part) => x(preg_replace("/\r\n?|\n/", "↵", $part));

        foreach ($list AS $item)
        {
            [$pos, $length] = $item;
            $replace        = $item[2] ?? NULL;

            //prevent splitting crlf line breaks
            if (($pos > 0) && (substr($text, $pos - 1, 2) === "\r\n"))
            {
                $pos--;
                $length++;
                if ($replace !== NULL) $replace = "\r$replace";
            }
            elseif (substr($text, $pos + $length - 1, 2) === "\r\n")
            {
                $length++;
                if ($replace !== NULL) $replace .= "\n";
            };

            $byte   = 4 * $context;
            $start  = max(0, $pos - 4 * $context);
            $end    = $pos + $length;
            $before = utf8_substr(substr($text, $start, $pos - $start),       -$context);
            $match  = utf8_substr(substr($text, $pos,   min($length, $byte)), 0, $context);
            $after  = utf8_substr(substr($text, $end,   $byte),               0, $context);

            //match snippet
            if ($replace === NULL)
            {
                $match = "<mark>" . $break($match) . (strlen($match) < $length ? " …" : "") . "</mark>";
            }

            //replace preview
            else
            {
                $_replace = utf8_substr($replace, 0, $context);
                $match    = "<del>" . $break($match) .    (strlen($match)    < $length          ? " …" : "") . "</del>" .
                            "<ins>" . $break($_replace) . (strlen($_replace) < strlen($replace) ? " …" : "") . "</ins>";
            };

            $result[] =
                ["line" => substr_count($text, "\n", 0, $pos) + 1,
                 "html" => (strlen($before) < $pos ? "… " : "") . $break($before) .
                           $match .
                           $break($after) . ($end + strlen($after) < $size ? " …" : "")];
        };

        return $result;
    }
};