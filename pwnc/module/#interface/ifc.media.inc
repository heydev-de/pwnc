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
if (! cms_load("media"))      ifc_inactive($ifc_page);
if (! cms_load("media_type")) ifc_inactive($ifc_page);

//permission
ifc_permission([""                            => t("access"),
                CMS_MEDIA_PERMISSION_OPERATOR => t("operator")]);

//retrieve selected object from permanent cache
cms_cache_init($object, "media." . CMS_USER . ".object");

//initialize variables
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

    cms_cache("media." . CMS_USER . ".object", $object, TRUE);

    $media = new media();
    $name  = l($media->data->get($object, "name"));
    $url   = translate_url($object);
    $size  = file_size($url);

    preview($media->parse($object, //address
                          NULL,    //id
                          "600",   //width
                          "400",   //height
                          $name,   //alt
                          $name) . //title
            "<div class=\"media-display-info\">" .
            (($size !== FALSE) ? format_bytesize($size) . " | " : "") .
            x(get_mime_type($url)) .
            "</div>",
            CMS_URL . "stylesheet/" . CMS_IFC_EDITION . ".css",
            "media-display");

    exit();

case "upload":
//..................................................................................................

    $media    = new media();
    $_object  = language_get($object, $language, TRUE);
    $category = $media->data->get($_object, "category") ?? "";

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object"   => $object,
                    "language" => $language],
                   "_upload",
                   t("upload_media"));

    //name
    $ifc->set(t("name"), "text 40 40 bl");

    //file
    $ifc->set(t("media_file"), "file 40 b");

    //type
    $ifc->set(t("media_format"),       "label");
    $ifc->set(media_type_get_select(), "select 40 b");

    //category
    echo("<hr>");
    $ifc->set(t("category"),      "label");
    $ifc->set(media_get_select(), "list 40 40 b", $category);

    //filename
    $ifc->set(t("additional"), "title");
    $ifc->set(t("file_name"),  "text 35 40");
    echo("<span class=\"element\">.ext</span>");

?><script>

ifc_object("ifc_file1").click();

</script><?php

    $ifc->close();

case "_upload":
//..................................................................................................

    if (isset($ifc_file1_name))
    {
        $media = new media();

        if ($_object = $media->add($ifc_file1,      //uploaded file
                                   $ifc_file1_name, //uploaded filename
                                   $ifc_param1,     //name
                                   $ifc_param2,     //type
                                   $ifc_param3,     //category
                                   $ifc_param4))    //filename
        {
            $object       = language_set($object, $_object, $language);
            $ifc_response = t("msg_done");
            break;
        };
    };

    $ifc_response = t("msg_error");
    break;

case "upload_multi":
//..................................................................................................

    $media    = new media();
    $_object  = language_get($object, $language, TRUE);
    $category = $media->data->get($_object, "category") ?? "";

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object"   => $object,
                    "language" => $language],
                   "_upload_multi",
                   t("bulk_upload"));

    //info
    $ifc->set(t("hint_bulk_upload"), "info b");

    //file
    $ifc->set(t("media_file"), "multifile 40 b");

    //type
    $ifc->set(t("media_format"),       "label");
    $ifc->set(media_type_get_select(), "select 40 b");

    //category
    echo("<hr>");
    $ifc->set(t("category"),      "label");
    $ifc->set(media_get_select(), "list 40 40", $category);

//javascript

?><script>

ifc_object("ifc_file1[]").click();

</script><?php

    $ifc->close();

case "_upload_multi":
//..................................................................................................

    if (is_array($ifc_file1)) $array = array_combine($ifc_file1, $ifc_file1_name);
    else                      $array = [$ifc_file1 => $ifc_file1_name];

    $media   = new media();
    $_object = "";
    $error   = FALSE;

    foreach ($array AS $key => $value)
    {
        if ($__object = $media->add($key,         //uploaded file
                                    $value,       //uploaded filename
                                    NULL,         //name
                                    $ifc_param1,  //type
                                    $ifc_param2)) //category
            $_object = $__object;
        else
            $error   = TRUE;
    };

    $object       = language_set($object, $_object, $language);
    $ifc_response = $error ? t("msg_error") : t("msg_done");
    break;

case "add":
case "edit":
//..................................................................................................

    $media   = new media();
    $_object = language_get($object, $language, TRUE);
    $flag    = NULL;

    if (streq(CMS_IFC_MESSAGE, "edit"))
    {
        if (blank($_object)) break; //no selection
        $ifc_param1 = $media->data->get($_object, "name");
        $ifc_param2 = $media->data->get($_object, "url");
        $ifc_param3 = $media->data->get($_object, "type");
        $flag       = $media->internal($_object);
    };

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object"   => $object,
                    "language" => $language],
                   "_" . CMS_IFC_MESSAGE,
                   streq(CMS_IFC_MESSAGE, "add") ? t("add_url") : t("command_edit"));

    //name
    $ifc->set(t("name"), "text 40 40 bl", $ifc_param1 ?? NULL);

    //url
    if ($flag) $ifc->dummy();
    else       $ifc->set(t("url"), "text 40 256 b$flag", $ifc_param2 ?? CMS_PROTOCOL . "://");

    //type
    $ifc->set(t("media_format"),       "label");
    $ifc->set(media_type_get_select(), "select 40 b",        $ifc_param3 ?? NULL);

    //category
    echo("<hr>");
    $ifc->set(t("category"),      "label");
    $ifc->set(media_get_select(), "list 40 40 b",       $media->data->get($_object, "category"));

    if ($flag)
    {
        //extract extension and filename
        $filename  = $media->data->get($_object, "filename");
        if ($filename === NULL) $filename = $_object;
        $extension = file_extension($filename);
        $filename  = file_name($filename);

        //filename
        $ifc->set(t("additional"), "title");
        $ifc->set(t("file_name"),  "text 35 40", $filename);
        echo("<span class=\"element\">" . x(".$extension") . "</span>");
    };

    $ifc->close();

case "_add":
//..................................................................................................

    $media = new media();

    if ($_object = $media->link($ifc_param2,  //url
                                $ifc_param1,  //name
                                $ifc_param3,  //type
                                $ifc_param4)) //category
    {
        $object       = language_set($object, $_object, $language);
        $ifc_response = t("msg_done");
        break;
    };

    $ifc_response = t("msg_error");
    break;

case "_edit":
//..................................................................................................

    $media   = new media();
    $_object = language_get($object, $language, TRUE);
    $flag    = $media->internal($_object); //internal

    if ($_object = $media->set($_object,                           //index
                               $ifc_param1,                        //name
                               $ifc_param3,                        //type
                               $ifc_param4,                        //category
                               $flag ? $ifc_param5 : $ifc_param2)) //filename or url
    {
        $object       = language_set($object, $_object, $language);
        $ifc_response = t("msg_done");
        break;
    };

    $ifc_response = t("msg_error");
    break;

case "replace":
//..................................................................................................

    $_object = language_get($object, $language, TRUE);
    if (blank($object)) break; //no selection

    $media = new media();

    //external
    if (! $media->internal($_object))
    {
        $ifc_response = t("err_external_source_replace");
        break;
    };

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object"   => $object,
                    "language" => $language],
                   "_replace",
                   t("replace_media"));

    //file
    $ifc->set(t("media_file"), "file 40");

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
        $media        = new media();
        $_object      = language_get($object, $language, TRUE);
        $ifc_response = $media->replace($_object,          //index
                                        $ifc_file1,        //file
                                        $ifc_file1_name) ? //filename
                        t("msg_done") : t("msg_error");
        break;
    };

    $ifc_response = t("msg_error");
    break;

case "delete":
//..................................................................................................

    if (blank($_object)) break; //no selection

    $value      = reset($_object);
    $media      = new media();
    $category   = $media->data->get($value, "category") ?? "";
    $flag_error = TRUE;

    //delete selected media
    foreach ($_object AS $value)
        $flag_error &= ! $media->unlink($value);

    //remove invalid media links from selection
    $array = explode(",", CMS_LANGUAGE_ENABLED);
    if (CMS_LANGUAGE_ENABLED) $array[] = FALSE;

    foreach ($array AS $value)
    {
        if (($_object = language_get($object, $value, TRUE)) === NULL) continue;
        if ($media->data->get($_object) === NULL)                      continue;

        //active language
        if (streq($value, $language))
        {
            $media->data->move("first");
            $_object = $media->data->seek(["category" => $category]);
            $object  = language_set($object, $_object, $value);
        }

        //inactive language(s)
        else
        {
            $object  = language_set($object, NULL, $value);
        };
    };

    $ifc_response = $flag_error ? t("msg_error") : t("msg_done");
    break;

case "category_rename":
//..................................................................................................

    $_object = language_get($object, $language, TRUE);
    if (blank($_object)) break; //no selection

    $media    = new media();
    $category = $media->data->get($_object, "category") ?? "";

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object"   => $object,
                    "language" => $language],
                   "_category_rename",
                   t("rename_category"));

    //category
    $ifc->set(t("name"), "text 40 40 b", $category);

    $ifc->close();

case "_category_rename":
//..................................................................................................

    $media    = new media();
    $_object  = language_get($object, $language, TRUE);
    $category = $media->data->get($_object, "category") ?? "";

    $media->data->move("first");
    while ($key = $media->data->move("next"))
    {
        $_category = $media->data->get($key, "category") ?? "";
        if (streq($_category, $category))
            $media->data->set($ifc_param1, $key, "category");
    };

    $ifc_response = $media->data->save() ? t("msg_done") : t("msg_error");
    break;

case "type":
case "type_select":
case "type_add":
case "type_set":
case "type_delete":
//..................................................................................................

    $media_type = new media_type();

    switch (CMS_IFC_MESSAGE)
    {
    case "type_select":
//..................................................................................................

        $type_object = $ifc_param;
        break;

    case "type_add":
//..................................................................................................

        if ($_type_object = $media_type->add(t("new_media_format")))
        {
            $type_object  = $_type_object;
            $ifc_response = t("msg_done");
            break;
        };

        $ifc_response = t("msg_error");
        break;

    case "type_set":
//..................................................................................................

        $ifc_response = $media_type->set($type_object, $ifc_param1, $ifc_param2, $ifc_param3) ?
                        t("msg_done") : t("msg_error");
        break;

    case "type_delete":
//..................................................................................................

        if (blank($list)) break; //no selection

        //deselect object if deleted
        if (in_array($type_object, $list)) $type_object = "";

        $ifc_response = $media_type->delete($list) ?
                        t("msg_done") : t("msg_error");
        break;
    };

    if (blank($type_object))
    {
        //select assigned type
        if (nstre($object))
        {
            $media = new media();

            if (
                 stre($type_object = $media->data->get($object, "type"))
                 &&
                 (
                   nstre($filename = $media->data->get($object, "filename"))
                   ||
                   nstre($filename = $media->data->get($object, "url"))
                 )
               )
            {
                $extension   = file_extension($filename);
                $type_object = $media_type->get_index($extension);
            };
        };

        //select first type
        if (blank($type_object)) $type_object = $media_type->data->move("first");
    };

    //menu
    $menu = [t("command_add") .             "|media/command_add"    => "type_add",
             t("command_delete_selected") . "|media/command_delete" => "#type_delete",
             t("command_close") .           "|media/command_cancel" => FALSE];

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   $menu,
                   ["object"      => $object,
                    "language"    => $language,
                    "type_object" => $type_object],
                   "type",
                   t("media_formats"));

    echo("<table class=\"layout\">" .

         "<colgroup>" .
         "<col>" .
         (nstre($type_object) ? "<col style=\"WIDTH:0\">" : "") .
         "</colgroup>" .

         "<tr>" .
         "<td>");

    ifc_table_open();
    echo("<colgroup>" .
         "<col style=\"WIDTH:0\">" .
         "<col>" .
         "</colgroup>" .

    //header
         "<tr>" .

    //selection
         "<td class=\"select\">" .
         "</td>" .

    //name
         "<th>" .
         t("media_formats") .
         "</th>" .
         "</tr>");

    data_sort($media_type->data, "name");

    //list
    $media_type->data->move("first");
    while ($key = $media_type->data->move("next"))
    {
        $name   = x($media_type->data->get($key, "name"));
        if (streq($key, $type_object)) $name = "<strong>$name</strong>";
        $varied = ifc_varied();

        echo("<tr>" .

        //selection
             "<td class=\"select\">");
        $ifc->set(NULL, "checkbox", $key, streq($key, $type_object), "list[]");
        echo("</td>" .

        //name
             "<td$varied>" .
             "<a href=\"javascript:ifc_post('type_select','" . qx($key) . "');\">" .
             image("media/icon_type") . " $name" .
             "</a>" .
             "</td>" .
             "</tr>");
    };

    //selection control
    echo("<tr>" .
         "<td colspan=\"2\" " .
             "class=\"select\" " .
             "style=\"TEXT-ALIGN:right\">");
    $ifc->set(t("all"),    "button", "javascript:ifc_list_activate();");
    $ifc->set(t("invert"), "button", "javascript:ifc_list_invert();");
    $ifc->set(t("none"),   "button", "javascript:ifc_list_deactivate();");
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    echo("</td>");

    if (nstre($type_object))
    {
        //data
        echo("<td>");

        ifc_table_open();

        //header
        echo("<tr>" .
             "<th>" .
             t("data") .
             "</th>" .
             "</tr>" .

        //settings
             "<tr>" .
             "<td>");

        //name
        $ifc->set(t("name"), "text 40 40", $media_type->data->get($type_object, "name"));

        //save button
        $ifc->set(t("command_save"), "button b", "type_set");

        //file extensions
        $ifc->set(t("assigned_file_extensions"), "text 60 256 b", $media_type->data->get($type_object, "type"));

        //javascript

?><script>

function media_type_insert(value)
{
    textcontrol_set("#l_ifc_param3", "#replace", "[ " + value + "=\"%" + value + "%\" ]");
    event.target.selectedIndex = 0; //reset
};

</script><?php

        //placeholders
        echo("<select onchange=\"media_type_insert(this.value);\">" .
             "<option value=\"\" hidden> ".   t("insert_value") . "</option>" .
             "<option>id</option>" .
             "<option>src</option>" .
             "<option>type</option>" .
             "<option>width</option>" .
             "<option>height</option>" .
             "<option>alt</option>" .
             "<option>title</option>" .
             "<option>class</option>" .
             "<option>style</option>" .
             "</select><br>");

        //code
        $ifc->set(t("embed_code"), "code_html 60x25 ln", $media_type->get_code($type_object));

        echo("</td>" .
             "</tr>");
        ifc_table_close();

        echo("</td>");
    };
    echo("</tr>" .
         "</table>");

    $ifc->close();
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

$media   = new media();
$_object = language_get($object, $language, TRUE);
$array   = media_get_array();

//get category from object
if ($_object && in_array_recursive($_object, $array))
{
    $category = $media->data->get($_object, "category") ?? "";
    cms_cache("media." . CMS_USER . ".category", $category, TRUE);
}

//get object from category
else
{
    $category = cms_cache("media." . CMS_USER . ".category") ?? "";
    if (! isset($array[$category])) $category = "";

    if (is_array($array[$category]))
    {
        $_object = current($array[$category]);
        $object  = language_set($object, $_object, $language);
    };
};

//menu
$menu = NULL;
if (CMS_IFC_SELECT && $_object)
    $menu[t("command_insert") . "|media/command_apply"] = "javascript:ifc_return();";
if ($media->operator)
{
    $menu[t("upload_media") . "|media/command_upload"]       = "upload";
    $menu[t("bulk_upload") .  "|media/command_upload_multi"] = "upload_multi";
    $menu[t("add_url") .      "|media/command_add"]          = "add";
    if ($_object)
    {
        $menu[t("command_edit") .    "|media/command_edit"]      = "edit";
        $menu[t("replace_media") .   "|media/command_overwrite"] = "replace";
        $menu[t("command_delete") .  "|media/command_delete"]    = "#delete";
        $menu[t("rename_category") . "|media/command_rename"]    = "category_rename";
    };
    $menu[t("media_formats") . "|media/command_code"] = "type";
};

//display
$ifc = new ifc(
    $ifc_response,
    $ifc_page,
    $menu,
    ["object"   => $object,
     "language" => $language]);

//javascript

?><script>

function media_select(value)
{
    var language = "<?php echo(q($language));?>";
    if (ifc_language_get(ifc_get("object"), language) === value) return;
    ifc_set("object", ifc_language_set(ifc_get("object"), value, language));
    document.getElementById("media-display").src = "<?php
        echo(str_replace("%replace%",
                         "\"+encodeURIComponent(value)+\"",
                         q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "display",
                                    "object"      => "\x1B%replace%"]))));?>";
};

</script><?php

ifc_table_open("fullsize");
echo("<colgroup>" .
     "<col style=\"WIDTH:0\">" .
     "<col>" .
     "</colgroup>" .

     "<tr>" .
     "<td>");

//category
$ifc->set(t("category"), "description b");
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
if ($flag = is_array($array[$category]))
{
    $media_type = new media_type();

    echo("<br>");

    $ifc->set(t("media"), "description b");
    echo("<div id=\"_object\" " .
              "data-size=\"14\" " .
              "class=\"select list\" " .
              "data-onchange=\"media_select(this.value);\" " .
              "ondblclick=\"ifc_return();\">");
    foreach ($array[$category] AS $key => $value)
    {
        if (
             (! $type   = $media->data->get($value, "type"))
             &&
             ($filename = $media->data->get($value, "filename") ?:
                          $media->data->get($value, "url"))
           )
        {
            $extension = file_extension($filename);
            $type      = $media_type->get_index($extension);
        };
        $type = $media_type->data->get($type, "name");
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
    $ifc->set(t("all"),    "button", "javascript:ifc_list_activate(\"_object\");");
    $ifc->set(t("invert"), "button", "javascript:ifc_list_invert(\"_object\");");
    $ifc->set(t("none"),   "button", "javascript:ifc_list_deactivate(\"_object\");");
    echo("</div>" .
         jscript("ifc_custom_select(\"_object\");"));
};

//language
if (CMS_IFC_SELECT && CMS_LANGUAGE_ENABLED)
    language_selector($language, "javascript:ifc_post('select_language','%language%');");

echo("</td>" .

//display media
     "<td class=\"iframe\">" .
     "<iframe id=\"media-display\" " .
             "src=\"" . x(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                   "ifc_message" => "display",
                                   "object"      => $_object])) . "\" " .
             "sandbox=\"allow-same-origin allow-scripts\">" .
     t("preview_unavailable") .
     "</iframe>" .
     "</td>" .
     "</tr>");
ifc_table_close();

$ifc->close();