<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   INTERFACE
//==================================================================================================

//load libraries
if (! cms_load("image")) ifc_inactive($ifc_page);

//permission
ifc_permission([""                            => CMS_L_ACCESS,
                CMS_IMAGE_PERMISSION_OPERATOR => CMS_L_OPERATOR]);

cms_cache_init($object, "image." . CMS_USER . ".object");
init($language);

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "select":
//..................................................................................................

    $object = language_set($object, $ifc_param, $language);
    break;

case "select_language":
//..................................................................................................

    $language = $ifc_param;
    break;

case "display":
//..................................................................................................

    if (! $object) exit();

    cms_cache("image." . CMS_USER . ".object", $object, TRUE);

    $image  = translate_url($object);
    $size   = getimagesize($image, $info);
    $_image = image_process($image, 600, 600, NULL, FALSE, FALSE);

    preview("<a href=\"" . x($image) . "\" " .
               "onclick=\"window.open(this.href);return false;\">" .
            "<img src=\"" . x($_image) . "\" " .
                 "alt=\"" . x(file_name($image)) . "\">" .
            "</a>" .
            "<div class=\"image-display-info\">" .
            (($size !== FALSE) ? x(format_number($size[0])) . " x " . x(format_number($size[1])) . " px | " : "") .
            x(get_mime_type($image)) .
            "</div>",
            CMS_URL . "stylesheet/" . CMS_IFC_EDITION . ".css",
            "image-display");
    exit();

case "upload":
//..................................................................................................

    $image    = new image();
    $_object  = language_get($object, $language, TRUE);
    $category = $image->data->get($_object, "category");

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object"   => $object,
                    "language" => $language],
                   "_upload",
                   CMS_L_IFC_IMAGE_001);

    //name
    $ifc->set(CMS_L_NAME,                                          "text 40 40 bl");

    //file
    $ifc->set(CMS_L_IFC_IMAGE_002 . " (GIF, JPG, PNG, SVG, WEBP)", "file 40 b");

    //category
    echo("<hr>");
    $ifc->set(CMS_L_IFC_IMAGE_014,                                 "label");
    $ifc->set(image_get_select(),                                  "list 40 40 b", $category);

    //filename
    $ifc->set(CMS_L_IFC_IMAGE_015,                                 "title");
    $ifc->set(CMS_L_IFC_IMAGE_013,                                 "text 35 40");
    echo("<span class=\"element\">.ext</span>");

    //javascript

?><script>

ifc_object("ifc_file1").click();

</script><?php

    $ifc->close();

case "_upload":
//..................................................................................................

    if (isset($ifc_file1_name))
    {
        $image = new image();

        if ($_object = $image->add($ifc_file1,      //uploaded file
                                   $ifc_file1_name, //uploaded filename
                                   $ifc_param1,     //name
                                   $ifc_param2,     //category
                                   $ifc_param3))    //filename
        {
            $object       = language_set($object, $_object, $language);
            $ifc_response = CMS_MSG_DONE;
            break;
        };
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "upload_multi":
//..................................................................................................

    $image    = new image();
    $_object  = language_get($object, $language, TRUE);
    $category = $image->data->get($_object, "category");

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object"   => $object,
                    "language" => $language],
                   "_upload_multi",
                   CMS_L_IFC_IMAGE_016);

    //info
    $ifc->set(CMS_L_IFC_IMAGE_017,                                 "info b");

    //file
    $ifc->set(CMS_L_IFC_IMAGE_002 . " (GIF, JPG, PNG, SVG, WEBP)", "multifile 40 b");

    //category
    echo("<hr>");
    $ifc->set(CMS_L_IFC_IMAGE_014,                                 "label");
    $ifc->set(image_get_select(),                                  "list 40 40", $category);

//javascript

?><script>

ifc_object("ifc_file1[]").click();

</script><?php

    $ifc->close();

case "_upload_multi":
//..................................................................................................

    if (is_array($ifc_file1)) $array = array_combine($ifc_file1, $ifc_file1_name);
    else                      $array = [$ifc_file1 => $ifc_file1_name];

    $image   = new image();
    $_object = "";
    $error   = FALSE;

    foreach ($array AS $key => $value)
    {
        if ($__object = $image->add($key,         //uploaded file
                                    $value,       //uploaded filename
                                    NULL,         //name
                                    $ifc_param1)) //category
            $_object = $__object;
        else
            $error   = TRUE;
    };

    $object       = language_set($object, $_object, $language);
    $ifc_response = $error ? CMS_MSG_ERROR : CMS_MSG_DONE;
    break;

case "add":
case "edit":
//..................................................................................................

    $image   = new image();
    $_object = language_get($object, $language, TRUE);
    $flag    = NULL;

    if (streq(CMS_IFC_MESSAGE, "edit"))
    {
        if (blank($_object)) break; //no selection
        $ifc_param1 = $image->data->get($_object, "name");
        $ifc_param2 = $image->data->get($_object, "url");
        $flag       = $image->internal($_object);
    };

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object"   => $object,
                    "language" => $language],
                   "_" . CMS_IFC_MESSAGE,
                   streq(CMS_IFC_MESSAGE, "add") ? CMS_L_IFC_IMAGE_009 : CMS_L_COMMAND_EDIT);

    //name
    $ifc->set(CMS_L_NAME,           "text 40 40 bl", isset($ifc_param1) ? $ifc_param1 : NULL);

    //url
    if ($flag) $ifc->dummy();
    else       $ifc->set(CMS_L_URL, "text 40 256 b", isset($ifc_param2) ? $ifc_param2 : "https://");

    //category
    echo("<hr>");
    $ifc->set(CMS_L_IFC_IMAGE_014,  "label");
    $ifc->set(image_get_select(),   "list 40 40 b",  $image->data->get($_object, "category"));

    if ($flag)
    {
        //extract extension and filename
        $filename  = $image->data->get($_object, "filename");
        if ($filename === NULL) $filename = substr($_object, 8);
        $extension = file_extension($filename);
        $filename  = file_name($filename);

        //filename
        $ifc->set(CMS_L_IFC_IMAGE_015, "title");
        $ifc->set(CMS_L_IFC_IMAGE_013, "text 35 40", $filename);
        echo("<span class=\"element\">" . x(".$extension") . "</span>");
    };

    $ifc->close();

case "_add":
//..................................................................................................

    $image = new image();

    if ($_object = $image->link($ifc_param2,  //url
                                $ifc_param1,  //name
                                $ifc_param3)) //category
    {
        $object       = language_set($object, $_object, $language);
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "_edit":
//..................................................................................................

    $image   = new image();
    $_object = language_get($object, $language, TRUE);
    $flag    = $image->internal($_object); //internal

    if ($_object = $image->set($_object,                           //index
                               $ifc_param1,                        //name
                               $ifc_param3,                        //category
                               $flag ? $ifc_param4 : $ifc_param2)) //filename or url
    {
        $object       = language_set($object, $_object, $language);
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "replace":
//..................................................................................................

    $_object = language_get($object, $language, TRUE);
    if (blank($_object)) break; //no selection

    $image = new image();

    //external
    if (! $image->internal($_object))
    {
        $ifc_response = CMS_L_IFC_IMAGE_012;
        break;
    };

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object"   => $object,
                    "language" => $language],
                   "_replace",
                   CMS_L_IFC_IMAGE_008);

    //file
    $ifc->set(CMS_L_IFC_IMAGE_002, "file 40");

//javascript

?><script>

ifc_autopost("ifc_file1", "_replace");
ifc_object("ifc_file1").click();

</script><?php

    $ifc->close();

case "_replace":
//..................................................................................................

    if (isset($ifc_file1_name))
    {
        $image        = new image();
        $_object      = language_get($object, $language, TRUE);
        $ifc_response = $image->replace($_object,          //index
                                        $ifc_file1,        //file
                                        $ifc_file1_name) ? //filename
                        CMS_MSG_DONE : CMS_MSG_ERROR;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "delete":
//..................................................................................................

    if (empty($_object)) break; //no selection

    $value      = reset($_object);
    $image      = new image();
    $category   = $image->data->get($value, "category");
    $flag_error = TRUE;

    //delete selected images
    foreach ($_object AS $value)
        $flag_error &= ! $image->unlink($value);

    //remove invalid image links from selection
    $array = explode(",", CMS_LANGUAGE_ENABLED);
    if (CMS_LANGUAGE_ENABLED) $array[] = FALSE;

    foreach ($array AS $value)
    {
        $_object = language_get($object, $value, TRUE);

        if (! $image->data->get($_object))
        {
            //active language
            if (streq($value, $language))
            {
                $image->data->move("first");
                $_object = $image->data->seek(["category" => $category]);
                $object  = language_set($object, $_object, $value);
            }

            //inactive language(s)
            else
            {
                $object  = language_set($object, NULL, $value);
            };
        };
    };

    $ifc_response = $flag_error ? CMS_MSG_ERROR : CMS_MSG_DONE;
    break;

case "category_rename":
//..................................................................................................

    $_object = language_get($object, $language, TRUE);
    if (blank($_object)) break; //no selection

    $image    = new image();
    $category = $image->data->get($_object, "category");

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object"   => $object,
                    "language" => $language],
                   "_category_rename",
                   CMS_L_IFC_IMAGE_010);

    //category
    $ifc->set(CMS_L_NAME, "text 40 40", $category);

    $ifc->close();

case "_category_rename":
//..................................................................................................

    $image = new image();
    if (! $image->operator) break;

    $_object  = language_get($object, $language, TRUE);
    $category = $image->data->get($_object, "category");

    $image->data->move("first");
    while ($key = $image->data->move("next"))
    {
        $_category = $image->data->get($key, "category");
        if (streq($_category, $category))
            $image->data->set($ifc_param1, $key, "category");
    };

    $ifc_response = $image->data->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "clear_cache":
//..................................................................................................

    $image = new image();

    set_time_limit(600); //10 minutes

    if (

    //verify permission
         $image->operator
         &&

    //delete cache
         cms_load("filemanager")
         &&
         (
           (! is_dir(CMS_DATA_PATH . "image/cache"))
           ||
           filemanager_delete(CMS_DATA_PATH . "image/cache")
         )
         &&
         (
           (! is_dir(CMS_DATA_PATH . "#content/cache"))
           ||
           filemanager_delete(CMS_DATA_PATH . "#content/cache")
         )
       )
    {
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "config":
//..................................................................................................

    //get values
    $system     = new system();
    $ifc_param1 = $system->getval("image", "preference");
    $ifc_param2 = $system->getval("image", "resolution");
    $ifc_param3 = $system->getval("image", "daemon");

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   NULL,
                   "_config",
                   CMS_L_IFC_IMAGE_003);

    //preferred format
    $ifc->set(CMS_L_IFC_IMAGE_006, "label");
    $ifc->set(["WebP" => "webp",
               "JPEG" => "jpg"],
              "select 40 b",
              $ifc_param1);

    //maximum resolution
    $ifc->set(CMS_L_IFC_IMAGE_018, "label");
    $ifc->set(["UHD-II (7680 x 4320 px)" => "7680x4320",
               "UHD-I (3840 x 2160 px)"  => "3840x2160",
               "FHD (1920 x 1080 px)"    => "1920x1080",
               "HD (1080 x 720 px)"      => "1080x720"],
              "select 40 b",
              $ifc_param2);

    //daemon processing
    $ifc->set(CMS_L_IFC_IMAGE_005, "checkbox", TRUE, $ifc_param3);

    $ifc->close();

case "_config":
//..................................................................................................

    //check permission
    if (! cms_permission(CMS_IMAGE_PERMISSION_OPERATOR)) break;

    //set values
    $system = new system();
    $system->setval($ifc_param1,          "image", "preference");
    $system->setval($ifc_param2,          "image", "resolution");
    $system->setval($ifc_param3 ?? FALSE, "image", "daemon");

    $ifc_response = $system->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

$image   = new image();
$_object = language_get($object, $language, TRUE);
$array   = image_get_array();

//get category from object
if (nstre($_object) && in_array_recursive($_object, $array))
{
    $category = $image->data->get($_object, "category");
    if ($category === NULL) $category = "";
    cms_cache("image." . CMS_USER . ".category", $category, TRUE);
}

//get object from category
else
{
    $category = cms_cache("image." . CMS_USER . ".category");
    if ((! $category) || (! isset($array[$category]))) $category = "";

    if (is_array($array[$category]))
    {
        $_object = current($array[$category]);
        $object  = language_set($object, $_object, $language);
    };
};

//menu
$menu = NULL;
if (CMS_IFC_SELECT && $_object)
    $menu[CMS_L_COMMAND_INSERT . "|image/command_apply"]         = "javascript:ifc_return();";
if ($image->operator)
{
    $menu[CMS_L_IFC_IMAGE_001 .  "|image/command_upload"]        = "upload";
    $menu[CMS_L_IFC_IMAGE_016 .  "|image/command_upload_multi"]  = "upload_multi";
    $menu[CMS_L_IFC_IMAGE_009 .  "|image/command_add"]           = "add";
    if ($_object)
    {
        $menu[CMS_L_COMMAND_EDIT .   "|image/command_edit"]      = "edit";
        $menu[CMS_L_IFC_IMAGE_008 .  "|image/command_overwrite"] = "replace";
        $menu[CMS_L_COMMAND_DELETE . "|image/command_delete"]    = "#delete";
        $menu[CMS_L_IFC_IMAGE_010 .  "|image/command_rename"]    = "category_rename";
    };
    $menu[CMS_L_IFC_IMAGE_011 . "|image/command_clear"]         = "#clear_cache";
    $menu[CMS_L_IFC_IMAGE_003 . "|image/command_configuration"] = "config";
};

//display
$ifc = new ifc($ifc_response,
               $ifc_page,
               $menu,
               ["object"   => $object,
                "language" => $language]);

//javascript

?><script>

function image_select(value)
{
    var language = "<?php echo(q($language));?>";
    if (ifc_language_get(ifc_get("object"), language) === value) return;
    ifc_set("object", ifc_language_set(ifc_get("object"), value, language));
    document.getElementById("image-display").src = "<?php
        echo(str_replace("%replace%",
                         "\"+encodeURIComponent(value)+\"",
                         q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "display",
                                    "object"      => "\x1B%replace%"]))));?>";
};

</script><?php

ifc_table_open();
echo("<colgroup>" .
     "<col style=\"WIDTH:0\">" .
     "<col>" .
     "</colgroup>" .

     "<tr>" .
     "<td>");

//category
$ifc->set(CMS_L_IFC_IMAGE_014, "description b");
echo("<select class=\"list\" " .
             "onchange=\"ifc_post('select',this.value);\">");
foreach ($array AS $key => $value)
{
    $name  = strabridge($key, 40);
    $value = is_array($value) ? current($value) : NULL;
    echo("<option value=\"" . x($value) . "\"" .
                  (streq($key, $category) ? " selected>" : ">") .
         x($name) .
         "</option>");
};
echo("</select>");

//list
if (is_array($array[$category]))
{
    echo("<br>");

    $ifc->set(CMS_L_IFC_IMAGE_007, "description b");
    echo("<div id=\"_object\" " .
              "data-size=\"14\" " .
              "class=\"select image-list\" " .
              "data-onchange=\"image_select(this.value);\" " .
              "ondblclick=\"ifc_return();\">");
    foreach ($array[$category] AS $key => $value)
    {
        $_image = image_process(translate_url($value), 100, 100);
        echo("<label>" .
             "<input name=\"_object[]\" " .
                    "type=\"checkbox\" " .
                    "value=\"" . x($value) . "\"" .
                    (streq($value, $_object) ? " checked>" : ">") .
             "<image src=\"" . x($_image) . "\" " .
                    "alt=\"\">" .
             x($key) .
             "</label>");
    };
    echo("</div>" .
         "<div style=\"TEXT-ALIGN:right\">");
    $ifc->set(CMS_L_ALL,    "button", "javascript:ifc_list_activate(\"_object\");");
    $ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert(\"_object\");");
    $ifc->set(CMS_L_NONE,   "button", "javascript:ifc_list_deactivate(\"_object\");");
    echo("</div>" .
         jscript("ifc_custom_select(\"_object\");"));
};

//language
if (CMS_IFC_SELECT && CMS_LANGUAGE_ENABLED)
    language_selector($language, "javascript:ifc_post('select_language','%language%');");

echo("</td>" .

//display image
     "<td class=\"iframe\">" .
     "<iframe id=\"image-display\" " .
             "src=\"" . x(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                   "ifc_message" => "display",
                                   "object"      => $_object])) . "\" " .
             "sandbox=\"allow-popups allow-same-origin allow-scripts\">" .
     CMS_L_IFC_IMAGE_004 .
     "</iframe>" .
     "</td>" .
     "</tr>");
ifc_table_close();

$ifc->close();