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
if (! cms_load("download")) ifc_inactive($ifc_page);

//permission
ifc_permission([""                               => CMS_L_ACCESS,
                CMS_DOWNLOAD_PERMISSION_OPERATOR => CMS_L_OPERATOR]);

cms_cache_init($object, "download." . CMS_USER . ".object");
init($language);

//backwards compatibility fallback
if (nstre($object) && (strncmp($object, "download://", 11) !== 0))
    $object = "download://$object";

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

    //unset permission
    ifc_permission(FALSE);

    $ifc = new ifc(NULL, NULL, NULL);
    if (stre($object)) $ifc->close();

    cms_cache("download." . CMS_USER . ".object", $object, TRUE);

    $download = new download();
    $filename = $download->data->get($object, "filename") ??
                $download->data->remove_prefix($object); //compatibility fallback
    $name     = $download->data->get($object, "name");

    if (
         ($name !== NULL)
         &&
         is_file($path = CMS_DATA_PATH . "#download/$filename")
       )
    {
        ifc_table_open();
        echo("<colgroup>" .
             "<col style=\"WIDTH:0\">" .
             "<col>" .
             "</colgroup>" .

        //info
             "<tr>" .
             "<th colspan=\"2\">" .
             CMS_L_IFC_DOWNLOAD_005 .
             "</th>" .
             "</tr>" .

        //name
             "<tr>" .
             "<td" . ($varied = ifc_varied()) . ">" .
             CMS_L_NAME . ":" .
             "</td>" .
             "<td$varied>" .
             "<strong>" .
             x(l($name)) .
             "</strong>" .
             "</td>" .
             "</tr>");

        //format
        if ($buffer = file_extension($filename))
            echo("<tr>" .
                 "<td" . ($varied = ifc_varied()) . ">" .
                 CMS_L_IFC_DOWNLOAD_011 . ":" .
                 "</td>" .
                 "<td$varied>" .
                 x(strtoupper($buffer)) .
                 " (" . x(get_mime_type($buffer)) . ")" .
                 "</td>" .
                 "</tr>");

        //size
        if ($buffer = filesize($path))
            echo("<tr>" .
                 "<td" . ($varied = ifc_varied()) . ">" .
                 CMS_L_IFC_DOWNLOAD_012 . ":" .
                 "</td>" .
                 "<td$varied>" .
                 format_bytesize($buffer) .
                 "</td>" .
                 "</tr>");

        //description
        if ($buffer = l($download->data->get($object, "description")))
            echo("<tr>" .
                 "<td" . ($varied = ifc_varied()) . ">" .
                 CMS_L_DESCRIPTION . ":" .
                 "</td>" .
                 "<td$varied>" .
                 "<div class=\"br\">" .
                 parse_text($buffer) .
                 "</div>" .
                 "</td>" .
                 "</tr>");

        //download
        if (cms_permission("download", NULL, NULL))
        {
            //spacer
            echo("<tr>" .
                 "<td" . ($varied = ifc_varied()) . ">" .
                 "</td>" .

            //download button
                 "<td$varied " .
                     "style=\"FONT-WEIGHT:bold\">");
            $ifc->set(CMS_L_IFC_DOWNLOAD_013,
                      "button",
                      "javascript:ifc_download('" .
                      q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                 "ifc_message" => "download",
                                 "object"      => $object])) . "');");
            echo("</td>" .
                 "</tr>");
        };

        ifc_table_close();
    };

    $ifc->close();

case "download":
//..................................................................................................

    $download = new download();
    $filename = $download->data->get($object, "filename") ??
                $download->data->remove_prefix($object); //compatibility fallback
    $name     = $download->data->get($object, "name");

    if ($name !== NULL) download(CMS_DATA_PATH . "#download/" . $filename, $name);
    exit();

case "upload":
//..................................................................................................

    $download = new download();
    $_object  = language_get($object, $language, TRUE);
    $category = $download->data->get($_object, "category");

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object"   => $object,
                    "language" => $language],
                   "_upload",
                   CMS_L_IFC_DOWNLOAD_001);

    //name
    $ifc->set(CMS_L_NAME,             "text 40 40 bl");

    //description
    $ifc->set(CMS_L_DESCRIPTION,      "texteditor 50x8 1024 bilrt");

    //file
    $ifc->set(CMS_L_IFC_DOWNLOAD_002, "file 40 b");

?><script>

ifc_object("ifc_file1").click();

</script><?php

    //category
    echo("<hr>");
    $ifc->set(CMS_L_IFC_DOWNLOAD_003, "label");
    $ifc->set(download_get_select(),  "list 40 40", $category);

    $ifc->close();

case "_upload":
//..................................................................................................

    if (isset($ifc_file1_name))
    {
        $download = new download();

        if ($_object = $download->add($ifc_file1,      //file
                                      $ifc_file1_name, //filename
                                      $ifc_param1,     //name
                                      $ifc_param2,     //description
                                      $ifc_param3))    //category
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

    $download = new download();
    $_object  = language_get($object, $language, TRUE);
    $category = $download->data->get($_object, "category");

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object"   => $object,
                    "language" => $language],
                   "_upload_multi",
                   CMS_L_IFC_DOWNLOAD_014);

    //info
    $ifc->set(CMS_L_IFC_DOWNLOAD_015, "info b");

    //file
    $ifc->set(CMS_L_IFC_DOWNLOAD_002, "multifile 40 b");

    //category
    echo("<hr>");
    $ifc->set(CMS_L_IFC_DOWNLOAD_003, "label");
    $ifc->set(download_get_select(),  "list 40 40", $category);

//javascript

?><script>

ifc_object("ifc_file1[]").click();

</script><?php

    $ifc->close();

case "_upload_multi":
//..................................................................................................

    if (is_array($ifc_file1)) $array = array_combine($ifc_file1, $ifc_file1_name);
    else                      $array = [$ifc_file1 => $ifc_file1_name];

    $download = new download();
    $_object  = "";
    $error    = FALSE;

    foreach ($array AS $key => $value)
    {
        if ($__object = $download->add($key,         //file
                                       $value,       //filename
                                       NULL,         //name
                                       NULL,         //description
                                       $ifc_param1)) //category
            $_object = $__object;
        else
            $error   = TRUE;
    };

    $object       = language_set($object, $_object, $language);
    $ifc_response = $error ? CMS_MSG_ERROR : CMS_MSG_DONE;
    break;

case "edit":
//..................................................................................................

    $_object = language_get($object, $language, TRUE);
    if (blank($_object)) break; //no selection

    $download = new download();

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object"   => $object,
                    "language" => $language],
                   "_edit",
                   CMS_L_COMMAND_EDIT);

    //name
    $ifc->set(CMS_L_NAME,
              "text 40 40 bl",
              $download->data->get($_object, "name"));

    //description
    $ifc->set(CMS_L_DESCRIPTION,
              "texteditor 50x8 1024 bilrt",
              $download->data->get($_object, "description"));

    //category
    echo("<hr>");
    $ifc->set(CMS_L_IFC_DOWNLOAD_003, "label");
    $ifc->set(download_get_select(),
              "list 40 40",
              $download->data->get($_object, "category"));

    $ifc->close();

case "_edit":
//..................................................................................................

    $download = new download();
    $_object  = language_get($object, $language, TRUE);

    if ($download->set($_object,     //index
                       $ifc_param1,  //name
                       $ifc_param2,  //description
                       $ifc_param3)) //category
    {
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "replace":
//..................................................................................................

    $_object = language_get($object, $language, TRUE);
    if (blank($_object)) break; //no selection

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object"   => $object,
                    "language" => $language],
                   "_replace",
                   CMS_L_IFC_DOWNLOAD_008);

    //file
    $ifc->set(CMS_L_IFC_DOWNLOAD_002, "file 40 b");

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
        $download     = new download();
        $_object      = language_get($object, $language, TRUE);
        $ifc_response = $download->replace($_object,          //index
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
    $download   = new download();
    $category   = $download->data->get($value, "category");
    $flag_error = TRUE;

    //delete selected images
    foreach ($_object AS $value)
        $flag_error &= ! $download->unlink($value);

    //remove invalid download links from selection
    $array = explode(",", CMS_LANGUAGE_ENABLED);
    if (CMS_LANGUAGE_ENABLED) $array[] = FALSE;

    foreach ($array AS $value)
    {
        $_object = language_get($object, $value, TRUE);

        if (! $download->data->get($_object))
        {
            //active language
            if (streq($value, $language))
            {
                $download->data->move("first");
                $_object = $download->data->seek(["category" => $category]);
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
    if (blank($object)) break; //no selection

    $download = new download();
    $category = $download->data->get($_object, "category");

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object"   => $object,
                    "language" => $language],
                   "_category_rename",
                   CMS_L_IFC_DOWNLOAD_006);

    //category
    $ifc->set(CMS_L_NAME, "text 40 40 b", $category);

    $ifc->close();

case "_category_rename":
//..................................................................................................

    $download = new download();
    $_object  = language_get($object, $language, TRUE);
    $category = $download->data->get($_object, "category");

    $download->data->move("first");
    while ($key = $download->data->move("next"))
    {
        $_category = $download->data->get($key, "category");
        if (streq($_category, $category))
            $download->data->set($ifc_param1, $key, "category");
    };

    $ifc_response = $download->data->save() ? CMS_MSG_DONE : CMS_MSG_DONE;
    break;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

$download = new download();
$_object  = language_get($object, $language, TRUE);
$array    = download_get_array();

//get category from object
if ($_object && in_array_recursive($_object, $array))
{
    $category = $download->data->get($_object, "category");
    if (! $category) $category = "";
    cms_cache("download." . CMS_USER . ".category", $category, TRUE);
}

//get object from category
else
{
    $category = (string)cms_cache("download." . CMS_USER . ".category");
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
    $menu[CMS_L_COMMAND_INSERT .   "|download/command_apply"]         = "javascript:ifc_return();";
if ($download->operator)
{
    $menu[CMS_L_IFC_DOWNLOAD_001 . "|download/command_upload"]        = "upload";
    $menu[CMS_L_IFC_DOWNLOAD_014 . "|download/command_upload_multi"]  = "upload_multi";
    if ($_object)
    {
        $menu[CMS_L_COMMAND_EDIT .     "|download/command_edit"]      = "edit";
        $menu[CMS_L_IFC_DOWNLOAD_008 . "|download/command_overwrite"] = "replace";
        $menu[CMS_L_COMMAND_DELETE .   "|download/command_delete"]    = "#delete";
        $menu[CMS_L_IFC_DOWNLOAD_006 . "|download/command_rename"]    = "category_rename";
    };
};

//display
$ifc = new ifc($ifc_response,
               $ifc_page,
               $menu,
               ["object"   => $object,
                "language" => $language]);

//javascript

?><script>

function download_select(value)
{
    var language = "<?php echo(q($language));?>";
    if (ifc_language_get(ifc_get("object"), language) === value) return;
    ifc_set("object", ifc_language_set(ifc_get("object"), value, language));
    document.getElementById("download-display").src = "<?php
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
$ifc->set(CMS_L_IFC_DOWNLOAD_003, "description b");
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

    $ifc->set(CMS_L_IFC_DOWNLOAD_007, "description b");
    echo("<div id=\"_object\" " .
              "data-size=\"14\" " .
              "class=\"select list\" " .
              "data-onchange=\"download_select(this.value);\" " .
              "ondblclick=\"ifc_return();\">");
    foreach ($array[$category] AS $key => $value)
    {
        $type = file_extension($value) .
            " (" . get_mime_type($value) . ")";
        echo("<label title=\"" . x($type) . "\">" .
             "<input name=\"_object[]\" " .
                    "type=\"checkbox\" " .
                    "value=\"" . x($value) . "\"" .
                    (streq($value, $_object) ? " checked>" : ">") .
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

echo("</td>");

//display download
if ($_object)
    $_object = cms_url(["ifc_page"    => CMS_IFC_PAGE,
                        "ifc_message" => "display",
                        "object"      => $_object]);
else
    $_object = CMS_URL . "blank.htm";

echo("<td class=\"iframe\">" .
     "<iframe id=\"download-display\" " .
             "src=\"" . x($_object) . "\" " .
             "sandbox=\"allow-downloads allow-same-origin allow-scripts\">" .
     CMS_L_IFC_DOWNLOAD_004 .
     "</iframe>" .
     "</td>" .
     "</tr>");
$ifc->close();