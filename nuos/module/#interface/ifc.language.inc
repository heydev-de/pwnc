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

//permission
ifc_permission(["" => CMS_L_ACCESS]);

cms_cache_init($object, "language." . CMS_USER . ".object");
init($list, []);

$system = new system();
$data   = new data("#system/language");
$map    = new map("#system/language.image");

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "select":
//..................................................................................................

    $object = $ifc_param;
    break;

case "add":
//..................................................................................................

    if (! $data->get("x-undefined"))
        $data->set(["name" => CMS_L_IFC_LANGUAGE_010], "x-undefined");
    $object       = "x-undefined";
    $ifc_response = $data->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "set":
//..................................................................................................

    mkpath(CMS_DATA_PATH . "language");

    //tag
    $ifc_param1 = preg_replace("/[^-0-9A-Za-z]+/", "", $ifc_param1);
    $ifc_param1 = strtolower($ifc_param1);

    //no name given, keep current name
    if (stre($ifc_param1))
    {
        $ifc_param1 = $object;
    }

    //name changed
    elseif (nstreq($object, $ifc_param1))
    {
        //delete target icon if already existing
        if (($image = $map->get_value($ifc_param1)) !== NULL)
        {
            $map->del_key($ifc_param1);
            if (is_file(CMS_DATA_PATH . "language/$image"))
                unlink(CMS_DATA_PATH . "language/$image");
        };

        //rename source icon to target icon
        if (($image = $map->get_value($object)) !== NULL)
        {
            $map->del_key($object);
            if (is_file(CMS_DATA_PATH . "language/$image"))
            {
                $extension = file_extension($image);
                $map->set($ifc_param1, "$ifc_param1.$extension");
                rename(CMS_DATA_PATH . "language/$image",
                       CMS_DATA_PATH . "language/$ifc_param1.$extension");
            };
        };

        //remove target language if already existing
        $data->del($object);
        $object = $ifc_param1;
    };

    //name
    if (! $ifc_param2) $ifc_param2 = $object;
    $data->set($ifc_param2, $object, "name");

    //default language
    if (isset($ifc_param3))
    {
        $language = $system->getval("language", "default");

        if (stre($language))
        {
            $language = $object;
        }
        else
        {
            $array    = explode(",", $language);
            array_unshift($array, $object);
            $array    = array_unique($array);
            $language = implode(",", $array);
        };

        $system->setval($language, "language", "default");
        $system->save();
    };

    //delete icon if new icon is uploaded or delete option selected
    if (
         (nstre($ifc_file1) || isset($ifc_param4))
         &&
         (($image = $map->get_value($object)) !== NULL)
       )
    {
        $map->del_key($object);
        if (is_file(CMS_DATA_PATH . "language/$image"))
            unlink(CMS_DATA_PATH . "language/$image");
    };

    //process upload of icon
    if (
         nstre($ifc_file1)
         &&
         ($extension = file_extension($ifc_file1_name))
         &&
         preg_match("/^(?:gif|jpe?g|png|svg|webp)$/", $extension)
       )
    {
        $ifc_file1_name = CMS_DATA_PATH . "language/$object.$extension";
        $map->set($ifc_param1, "$object.$extension");
        move_uploaded_file($ifc_file1, $ifc_file1_name);
    };

    //stopwords
    $data->set($ifc_param5, $object, "stopword");

    $ifc_response = ($data->save() & $map->save()) ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "del":
//..................................................................................................

    if (empty($list)) break; //no selection

    foreach ($list AS $value)
    {
        //delete icon
        if (($image = $map->get_value($value)) !== NULL)
        {
            $map->del_key($value);
            if (is_file(CMS_DATA_PATH . "language/$image"))
                unlink(CMS_DATA_PATH . "language/$image");
        };

        //delete language
        $data->del($value);
    };

    $object       = NULL;
    $ifc_response = ($data->save() & $map->save()) ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "enable":
//..................................................................................................

    if (count($list))
    {
        $array   = explode(",", $system->getval("language", "default"));
        $primary = reset($array);
        if (in_array($primary, $list)) array_unshift($list, $primary);
        $list    = array_unique($list);
        $list    = implode(",", $list);
    }
    else
    {
        $list = NULL;
    };

    $system->setval($list, "language", "default");

    $ifc_response = ($system->save() && cms_load("directory") && directory_create_filesystem($list)) ?
                    CMS_MSG_DONE : CMS_MSG_ERROR;
    break;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

//cache selected object permanently
if (CMS_IFC_MESSAGE !== "") cms_cache("language." . CMS_USER . ".object", $object, TRUE);

$array   = array_flip(explode(",", $system->getval("language", "default")));
reset($array);
$primary = key($array);

//menu
$menu = [CMS_L_COMMAND_ADD .             "|language/command_add"    => "add",
         CMS_L_COMMAND_DELETE_SELECTED . "|language/command_delete" => "#del",
         CMS_L_IFC_LANGUAGE_009 .        "|language/command_enable" => "#enable"];

//display
$ifc = new ifc($ifc_response,
               $ifc_page,
               $menu,
               ["object" => $object]);

?><script>

function s(index) //select
{
    ifc_post("select", index);
};

function language_select_active()
{
    var list = document.querySelectorAll("INPUT[name=\"list[]\"]");

    list.forEach(object =>
    {
        switch (object.value)
        {
<?php

foreach ($array AS $key => $_)
    echo("case \"" . q($key) . "\":\n");

?>
                        if (! object.checked) object.click();
                        break;

                default:

                        if (object.checked)   object.click();
                };
    });
};

</script><?php

echo("<table class=\"layout\">" .

     "<colgroup>" .
     "<col>" .
     ($object ? "<col style=\"WIDTH:0\">" : "") .
     "</colgroup>" .

     "<tr>" .
     "<td>");

ifc_table_open();
echo("<colgroup>" .
     "<col style=\"WIDTH:0\">" .
     "<col span=\"2\">" .
     "</colgroup>" .

//header
     "<tr>" .

//selection
     "<td class=\"select\">" .
     "</td>" .

//name
     "<th>" .
     CMS_L_IFC_LANGUAGE_005 .
     "</th>" .

//tag
     "<th>" .
     CMS_L_IFC_LANGUAGE_004 .
     "</th>" .
     "</tr>");

if (is_array($data->data)) ksort($data->data, SORT_NATURAL | SORT_FLAG_CASE);

$data->move("first");
while ($key = $data->move("next"))
{
    $name   = x($data->get($key, "name"));
    if (streq($object, $key)) $name = "<strong>$name</strong>";
    $varied = ifc_varied();

    echo("<tr>" .

    //selection
         "<td class=\"" . (isset($array[$key]) ? "enabled" : "select") . "\">");
    $ifc->set(NULL, "checkbox", $key, streq($key, $object), "list[]");
    echo("</td>" .

    //name
         "<td$varied>" .
         "<a href=\"javascript:s('" . qx($key) . "');\">" .
         image(((($image = $map->get_value($key)) !== NULL) &&
               is_file(CMS_DATA_PATH . "language/$image")) ?
               CMS_DATA_URL . "language/$image" :
               "language/default",
               16, 12) . " " .
         $name .
         "</a>" .
         "</td>" .

    //tag
         "<td$varied>" .
         x($key) .
         "</td>" .
         "</tr>");
};

//selection control
echo("<tr>" .
     "<td colspan=\"3\" " .
         "class=\"select\" " .
         "style=\"TEXT-ALIGN:right\">");
$ifc->set(CMS_L_ALL,              "button", "javascript:ifc_list_activate();");
$ifc->set(CMS_L_INVERT,           "button", "javascript:ifc_list_invert();");
$ifc->set(CMS_L_NONE,             "button", "javascript:ifc_list_deactivate();");
$ifc->set(CMS_L_IFC_LANGUAGE_011, "button", "javascript:language_select_active();");
echo("</td>" .
     "</tr>");
ifc_table_close();

echo("</td>");

if ($object)
{
    //data
    echo("<td>");

    ifc_table_open();

    //header
    echo("<tr>" .
         "<th>" .
         CMS_L_IFC_LANGUAGE_006 .
         "</th>" .
         "</tr>" .

    //settings
         "<tr>" .
         "<td class=\"varied\">");

    //tag
    $ifc->set(CMS_L_IFC_LANGUAGE_004, "text 30 40",   $object);

    //save button
    $ifc->set(CMS_L_COMMAND_SAVE,     "button b",     "set");

    //name
    $ifc->set(CMS_L_IFC_LANGUAGE_005, "text 40 40 b", $data->get($object, "name"));

    //primary language
    $ifc->set(CMS_L_IFC_LANGUAGE_013, "checkbox b",   TRUE, streq($object, $primary));

    //icon
    $flag  = (($image = $map->get_value($object)) !== NULL) &&
             is_file(CMS_DATA_PATH . "language/$image");
    $image = $flag ?
             CMS_DATA_URL . "language/$image" :
             "language/default";

    echo("<table class=\"layout\">" .
         "<tr>" .

    //icon display
         "<td style=\"VERTICAL-ALIGN:middle\">" .
         image($image, 48, 48) .
         "</td>" .

         "<td>");

    //icon file upload
    $ifc->set(CMS_L_IFC_LANGUAGE_007, "file 34 b");

    //icon delete option
    if ($flag) $ifc->set(CMS_L_COMMAND_DELETE, "checkbox");
    else       $ifc->dummy();
    echo("</td>" .
         "</tr>" .
         "</table>");

    //stopwords
    $ifc->set(CMS_L_IFC_LANGUAGE_012, "textarea 40x20", $data->get($object, "stopword"));

    ifc_table_close();

    echo("</td>");
};
echo("</tr>" .
     "</table>");

$ifc->close();