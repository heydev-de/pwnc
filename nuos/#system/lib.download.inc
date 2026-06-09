<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   DOWNLOAD CLASS
//==================================================================================================

//permission
define("CMS_DOWNLOAD_PERMISSION_OPERATOR", "operator");

//related functions
//--------------------------------------------------------------------------------------------------

function download_get_array()
{
    $data  = new data("#system/download");
    $array = ["" => []];

    $data->move("first");
    while (($key = $data->move("next")) !== NULL)
    {
        $category = (string)$data->get($key, "category");
        $name     = l($data->get($key, "name"));
        if (stre($name)) $name = $key;

        $_name    = $name;
        $i        = 1;
        while (isset($array[$category][$_name]))
            $_name = $name . " (" . $i++ . ")";

        $array[$category][$_name] = $key;
    };

    ksort_recursive($array, SORT_NATURAL | SORT_FLAG_CASE);

    return $array;
};

//--------------------------------------------------------------------------------------------------

function download_get_select()
{
    $data  = new data("#system/download");
    $array = ["" => ""];

    $data->move("first");
    while (($key = $data->move("next")) !== NULL)
    {
        $category = (string)$data->get($key, "category");
        $array[$category] = $category;
    };

    ksort($array, SORT_NATURAL | SORT_FLAG_CASE);

    return $array;
};

class download
{

//properties
//--------------------------------------------------------------------------------------------------

    public $data     = NULL;
    public $operator = FALSE;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct()
    {
        $this->data     = new data("#system/download");
        $this->operator = cms_permission(CMS_DOWNLOAD_PERMISSION_OPERATOR);

        mkpath(CMS_DATA_PATH . "#download");
    }

//methods
//--------------------------------------------------------------------------------------------------

    function add($uploaded_file,
                 $uploaded_filename,
                 $name        = "",
                 $description = "",
                 $category    = "")
    {
        if (! $this->operator)         return FALSE; //no permission
        if (! is_file($uploaded_file)) return FALSE; //no file

        //extract upload extension
        $extension = file_extension($uploaded_filename);

        //default name
        $name_default = language_get($name, FALSE);

        //no default name
        if (stre($name_default))
        {
            //use uploaded filename as default name
            $name_default = file_name($uploaded_filename);

            //set default name
            $name = language_set($name, $name_default, FALSE);
        };

        //generate unique filename
        $filename = unique_id() . ".$extension";

        //move uploaded file
        if (move_uploaded_file($uploaded_file, CMS_DATA_PATH . "#download/$filename"))
        {
            //generate index
            $index = "download://" . unique_id();

            //add dataset
            $this->data->set(
                ["name"        => $name,
                 "description" => $description,
                 "category"    => $category,
                 "filename"    => $filename],
                $index);

            //save
            return $this->data->save() ? $index : FALSE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function set($index,
                 $name,
                 $description,
                 $category)
    {
        if (! $this->operator) return FALSE; //no permission
        if (! $index)          return FALSE; //security check

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

        //update dataset
        $this->data->set($name,        $index, "name");
        $this->data->set($description, $index, "description");
        $this->data->set($category,    $index, "category");

        //save
        return $this->data->save() ? $index : FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function replace($index,
                     $uploaded_file,
                     $uploaded_filename)
    {
        if (! $this->operator)         return FALSE; //no permission
        if (! is_file($uploaded_file)) return FALSE; //no file

        //extract upload extension
        $extension_new = file_extension($uploaded_filename);

        //retrieve filename
        $filename = $this->data->get($index, "filename") ??
                    $this->data->remove_prefix($index); //compatibility fallback

        //extract old extension
        $extension_old = file_extension($filename);

        //format matches
        if (streq($extension_new, $extension_old))

            //move uploaded file and exit
            return move_uploaded_file($uploaded_file, CMS_DATA_PATH . "#download/$filename");

        //delete old file
        if (is_file(CMS_DATA_PATH . "#download/$filename"))
            unlink(CMS_DATA_PATH . "#download/$filename");

        //extract filename
        $filename = file_name($filename) . ".$extension_new";

        //move uploaded file
        if (move_uploaded_file($uploaded_file, CMS_DATA_PATH . "#download/$filename"))
        {
            //update dataset
            $this->data->set($filename, $index, "filename");

            //save
            if ($this->data->save()) return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function unlink($index)
    {
        if (! $this->operator) return FALSE; //no permission
        if (! $index)          return FALSE; //security check

        //delete file
        $filename = $this->data->get($index, "filename") ??
                    $this->data->remove_prefix($index); //compatibility fallback

        if (is_file(CMS_DATA_PATH . "#download/$filename"))
            unlink(CMS_DATA_PATH . "#download/$filename");

        //delete dataset
        $this->data->del($index);

        //save
        return $this->data->save();
    }
};