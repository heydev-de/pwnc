<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   IMAGE CLASS
//==================================================================================================

//permission
define("CMS_IMAGE_PERMISSION_OPERATOR", "operator");

//related functions
//--------------------------------------------------------------------------------------------------

function image_get_array()
{
    $data  = new data("#system/image");
    $array = ["" => []];

    $data->move("first");
    while ($key = $data->move("next"))
    {
        $category = (string)$data->get($key, "category");
        $name     = l($data->get($key, "name"));
        if (stre($name)) $name = $key;

        $_name = $name;
        $i     = 1;
        while (isset($array[$category][$_name]))
            $_name = $name . " (" . $i++ . ")";

        $array[$category][$_name] = $key;
    };

    ksort_recursive($array, SORT_NATURAL | SORT_FLAG_CASE);

    return $array;
};

//--------------------------------------------------------------------------------------------------

function image_get_select()
{
    $data  = new data("#system/image");
    $array = ["" => ""];

    $data->move("first");
    while ($key = $data->move("next"))
    {
        $category = (string)$data->get($key, "category");
        $array[$category] = $category;
    };

    ksort($array, SORT_NATURAL | SORT_FLAG_CASE);

    return $array;
};

class image
{

//properties
//--------------------------------------------------------------------------------------------------

    public $data     = NULL;
    public $operator = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct()
    {
        $this->data     = new data("#system/image");
        $this->operator = cms_permission(CMS_IMAGE_PERMISSION_OPERATOR);

        mkpath(CMS_DATA_PATH . "image");
    }

//methods
//--------------------------------------------------------------------------------------------------

    function add($uploaded_file,
                 $uploaded_filename,
                 $name     = NULL,
                 $category = NULL,
                 $filename = NULL)
    {
        if (! $this->operator)         return FALSE; //no permission
        if (! is_file($uploaded_file)) return FALSE; //no file

        //extract upload extension and filename
        $extension         = file_extension($uploaded_filename);
        $uploaded_filename = file_name($uploaded_filename);
        if (streq($extension, "jpeg")) $extension = "jpg";

        if (! preg_match("/^(?:gif|jpg|png|svg|webp)$/", $extension)) return FALSE; //invalid format

        //default name
        $name_default = language_get($name, FALSE);

        //no default name
        if (stre($name_default))
        {
            //use filename as default name
            $name_default = $filename;

            //use uploaded filename as default name
            if (stre($name_default)) $name_default = $uploaded_filename;

            //set default name
            $name = language_set($name, $name_default, FALSE);
        };

        //no filename
        if (stre($filename))
        {
            //use default name as filename
            $filename = $name_default;

            //use uploaded filename as filename
            if (stre($filename)) $filename = $uploaded_filename;

            //generate random filename
            if (stre($filename)) $filename = unique_id();
        };

        //clean up filename
        $filename = stringtofilename($filename);

        //generate unique filename
        $_filename = "$filename.$extension";
        $i         = 0;

        while (is_file(CMS_DATA_PATH . "image/$_filename"))
            $_filename = "$filename-" . ++$i . ".$extension";

        //move uploaded file
        if (move_uploaded_file($uploaded_file, CMS_DATA_PATH . "image/$_filename"))
        {
            //generate index
            $index = "image://" . unique_id();

            //add dataset
            $this->data->set(
                ["name"     => $name,
                 "category" => $category,
                 "filename" => $_filename],
                $index);

            //save
            return $this->data->save() ? $index : FALSE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function set($index,
                 $name,
                 $category,
                 $filename_or_url)
    {
        if (! $this->operator) return FALSE; //no permission
        if (! $index)          return FALSE; //security check

        //internal
        if ($this->internal($index))
        {
            $filename = $filename_or_url;

            //default name
            $name_default = language_get($name, FALSE);

            //no default name
            if (stre($name_default))
            {
                //use old default name as default name
                $name_old     = $this->data->get($index, "name");
                $name_default = language_get($name_old, FALSE);

                //set default name
                $name = language_set($name, $name_default, FALSE);
            };

            //retrieve old filename
            $filename_old = $this->data->get($index, "filename") ??
                            $this->data->remove_prefix($index); //compatibility fallback

            //no filename
            if (stre($filename))
            {
                //keep old filename
                $_filename = $filename_old;
            }

            //new filename
            else
            {
                //clean up filename
                $filename  = stringtofilename($filename);

                //extract extension
                $extension = file_extension($filename_old);

                //generate unique filename
                $_filename = "$filename.$extension";
                $i         = 0;

                while (
                        nstreq($_filename, $filename_old)
                        &&
                        is_file(CMS_DATA_PATH . "image/$_filename")
                      )
                    $_filename = "$filename-" . ++$i . ".$extension";

                //change filename
                if (nstreq($_filename, $filename_old))
                    rename(CMS_DATA_PATH . "image/$filename_old",
                           CMS_DATA_PATH . "image/$_filename");
            };

            //update dataset
            $this->data->set(
                ["name"     => $name,
                 "category" => $category,
                 "filename" => $_filename],
                $index);

            //save
            return $this->data->save() ? $index : FALSE;
        };

        //external
        $url = $filename_or_url;

        //default name
        $name_default = language_get($name, FALSE);

        //no default name
        if (stre($name_default))
        {
            //use old default name as default name
            $name_old     = $this->data->get($index, "name");
            $name_default = language_get($name_old, FALSE);

            //set default name
            $name         = language_set($name, $name_default, FALSE);
        };

        //update dataset
        $this->data->set(
            ["name"     => $name,
             "category" => $category,
             "url"      => $url],
            $index);

        //save
        return $this->data->save() ? $index : FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function replace($index,
                     $uploaded_file,
                     $uploaded_filename)
    {
        if (! $this->operator)         return FALSE; //no permission
        if (! $this->internal($index)) return FALSE; //not internal
        if (! is_file($uploaded_file)) return FALSE; //no file

        //extract upload extension
        $extension_new = file_extension($uploaded_filename);
        if (streq($extension_new, "jpeg")) $extension_new = "jpg";

        if (! preg_match("/^(?:gif|jpg|png|svg|webp)$/", $extension_new)) return FALSE; //invalid format

        //retrieve filename
        $filename = $this->data->get($index, "filename") ??
                    $this->data->remove_prefix($index); //compatibility fallback

        //extract old extension
        $extension_old = file_extension($filename);

        //format matches
        if (streq($extension_new, $extension_old))

            //move uploaded file and exit
            return move_uploaded_file($uploaded_file, CMS_DATA_PATH . "image/$filename");

        //delete old file
        if (is_file(CMS_DATA_PATH . "image/$filename"))
            unlink(CMS_DATA_PATH . "image/$filename");

        //extract filename
        $filename = file_name($filename);

        //generate unique filename
        $_filename = "$filename.$extension_new";
        $i         = 0;

        while (is_file(CMS_DATA_PATH . "image/$_filename"))
            $_filename = "$filename-" . ++$i . ".$extension_new";

        //move uploaded file
        if (move_uploaded_file($uploaded_file, CMS_DATA_PATH . "image/$_filename"))
        {
            //update dataset
            $this->data->set($_filename, $index, "filename");

            //save
            if ($this->data->save()) return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function link($url,
                  $name     = NULL,
                  $category = NULL)
    {
        if (! $this->operator) return FALSE; //no permission

        //default name
        $name_default = language_get($name, FALSE);

        if (stre($name_default)) //no default name

            //use filename as default name
            $name = language_set($name, file_name($url), FALSE);

        //generate index
        $index = "image://" . unique_id();

        //add dataset
        $this->data->set(
            ["name"     => $name,
             "category" => $category,
             "url"      => $url],
            $index);

        //save
        return $this->data->save() ? $index : FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function unlink($index)
    {
        if (! $this->operator) return FALSE; //no permission
        if (! $index)          return FALSE; //security check

        if ($this->internal($index)) //internal
        {
            //retrieve filename
            $filename = $this->data->get($index, "filename") ??
                        $this->data->remove_prefix($index); //compatibility fallback

            //delete file
            if (is_file(CMS_DATA_PATH . "image/$filename"))
                unlink(CMS_DATA_PATH . "image/$filename");
        };

        //delete dataset
        $this->data->del($index);

        //save
        return $this->data->save();
    }

//--------------------------------------------------------------------------------------------------

    function internal($index)
    {
        if ($this->data->get($index, "url") !== NULL)   return FALSE;
        $filename = $this->data->get($index, "filename");
                    $this->data->remove_prefix($index); //compatibility fallback

        if (is_file(CMS_DATA_PATH . "image/$filename")) return TRUE;
        return FALSE;
    }
};