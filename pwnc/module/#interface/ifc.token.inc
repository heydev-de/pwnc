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
if (! cms_load("token")) ifc_inactive($ifc_page);

//permission
ifc_permission([""                            => t("access"),
                CMS_TOKEN_PERMISSION_OPERATOR => t("operator")]);

$token = new token();
if (! $token->enabled) ifc_inactive($ifc_page);

if (blank($object)) $object = cms_cache("token." . CMS_USER . ".object");
else                $object = substr($object, 0, strcspn($object, " \r\n"));

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "select":
//..................................................................................................

    $object = token_get_index($token, $ifc_param);
    break;

case "display":
//..................................................................................................

    cms_cache("token." . CMS_USER . ".object", $object, TRUE);

    if (($resultrow = $token->get($object)) !== FALSE)
    {
        $value = stre($resultrow[CMS_DB_TOKEN_VALUE]) ?
                 "%%$object%%" :
                 "%%$object " . $resultrow[CMS_DB_TOKEN_VALUE] . "%%";
        preview($token->apply($value));
    };

    exit();

case "add":
case "edit":
//..................................................................................................

    if (streq(CMS_IFC_MESSAGE, "edit"))
    {
        if (blank($object)) break; //no selection

        if (($resultrow = $token->get($object)) === FALSE)
        {
            $ifc_response = t("msg_error");
            break;
        };

        $ifc_param1 = $resultrow[CMS_DB_TOKEN_TITLE];
        $ifc_param2 = $object;
        $ifc_param3 = $resultrow[CMS_DB_TOKEN_VALUE];
        $ifc_param4 = $resultrow[CMS_DB_TOKEN_TEXT];
    };

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object" => $object],
                   "_" . CMS_IFC_MESSAGE,
                   streq(CMS_IFC_MESSAGE, "add") ?
                   t("command_add") : t("command_edit"));

    //name
    $ifc->set(t("name"), "text 40 40 bl", $ifc_param1 ?? "");

    //index
    $ifc->set(t("placeholder"), "text 40 40 b", $ifc_param2 ?? "");

    //value
    $ifc->set(t("parameters_comma_separated"), "text 60 255 b", $ifc_param3 ?? "");

?><script>

function token_index()
{
    //standardize index
    var value = ifc_get("ifc_param2");
    if (value === "") value = ifc_get("l_ifc_param1");
    value     = value.replace(/[ \r\n]+/g, "_")
                     .replace(/_{2,}/g, "_");
    ifc_set("ifc_param2", value);
};

function token_value()
{
    //standardize value list
    var value  = ifc_get("ifc_param3")
                 .replace(/[^0-9a-zA-Z_,]+/g, "_")
                 .replace(/_{2,}/g, "_")
                 .replace(/,{2,}/g, ",")
                 .replace(/^,/, "")
                 .replace(/,$/, "");
    ifc_set("ifc_param3", value);

    var array  = value.split(",");
    var object = document.getElementById("placeholder");
    var node   = null;
    var text   = "";

    //clear placeholder list
    while (object.childNodes.length > 1)
        object.removeChild(object.lastChild);

    //create placeholder list
    for (var i in array)
    {
        value     = array[i];
        if (value === "") break;
        node      = document.createElement("option");
        node.text = value;
        object.appendChild(node);
    };
};

function token_insert(value)
{
    textcontrol_set("#l_ifc_param4", "#replace", "[ %" + value + "% ]");
    event.target.selectedIndex = 0; //reset
};

fx_event_listen(document.forms["ifc"].elements["ifc_param2"], "change", token_index);
fx_event_listen(document.forms["ifc"].elements["ifc_param3"], "change", token_value);

</script><?php

    //placeholders
    echo("<select id=\"placeholder\" " .
                 "onchange=\"token_insert(this.value);\">" .
         "<option value=\"\" hidden>" . t("insert_value") . "</option>");
    if (isset($ifc_param3) && nstre($ifc_param3))
    {
        $array = explode(",", $ifc_param3);
        foreach ($array AS $value)
            echo("<option>" .
                 x($value) .
                 "</option>");
    };
    echo("</select><br>");

    //text
    $ifc->set(t("replacement_code"), "code_html 60x20 65536 l", $ifc_param4 ?? "");

    //category
    echo("<hr>");
    $ifc->set(t("category"),            "label");
    $ifc->set(token_get_select($token), "list 40 40",              token_get_category($token, $object));

    $ifc->close();

case "_add":
//..................................................................................................

    $_object = $token->add(
        $ifc_param2,  //index
        $ifc_param3,  //value
        $ifc_param5,  //category
        $ifc_param1,  //title
        $ifc_param4); //text

    if ($_object !== FALSE)
    {
        $object       = $_object;
        $ifc_response = t("msg_done");
        break;
    };

    $ifc_response = t("msg_error");
    break;

case "_edit":
//..................................................................................................

    $_object = $token->update(
        $object,      //index
        $ifc_param2,  //_index
        $ifc_param3,  //value
        $ifc_param5,  //category
        $ifc_param1,  //title
        $ifc_param4); //text

    if ($_object !== FALSE)
    {
        $object       = $ifc_param2;
        $ifc_response = t("msg_done");
        break;
    };

    $ifc_response = t("msg_error");
    break;

case "delete":
//..................................................................................................

    if (blank($_object)) break; //no selection

    $category   = token_get_category($token, $object);
    $flag_error = TRUE;

    //delete selected tokens
    foreach ($_object AS $value)
    {
        $value       = substr($value, 0, strcspn($value, " \r\n"));
        $flag_error &= ! $token->delete($value);
    };

    $object       = token_get_index($token, $category);
    $ifc_response = $flag_error ? t("msg_error") : t("msg_done");
    break;

case "category_rename":
//..................................................................................................

    if (blank($object)) break; //no selection

    $category = token_get_category($token, $object);

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object" => $object],
                   "_category_rename",
                   t("rename_category"));

    //category
    $ifc->set(t("name"), "text 40 40", $category);

    $ifc->close();

case "_category_rename":
//..................................................................................................

    $category     = token_get_category($token, $object);
    $ifc_response = mysql_query("UPDATE " . CMS_DB_TOKEN . " " .
                                "SET " .    CMS_DB_TOKEN_CATEGORY . "='" . sqlesc($ifc_param1) . "' " .
                                "WHERE " .  CMS_DB_TOKEN_CATEGORY . "='" . sqlesc($category) . "'") ?
                    t("msg_done") : t("msg_error");
    break;

case "clear_cache":
//..................................................................................................

    set_time_limit(600); //10 minutes

    if (

    //verify permission
         $token->operator
         &&

    //delete cache
         cms_load("filemanager")
         &&
         (
           ! is_dir(CMS_DATA_PATH . "#token/cache/")
           ||
           filemanager_delete(CMS_DATA_PATH . "#token/cache/")
         )
         &&
         (
           ! is_dir(CMS_DATA_PATH . "#content/cache/")
           ||
           filemanager_delete(CMS_DATA_PATH . "#content/cache/")
         )
       )
    {
        $ifc_response = t("msg_done");
        break;
    };

    $ifc_response = t("msg_error");
    break;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

//get category from object
if (nstre($object))
{
    $category = token_get_category($token, $object);
    if ($category === FALSE) $category = "";
    cms_cache("token." . CMS_USER . ".category", $category, TRUE);
}

//get object from category
else
{
    $category = (string)cms_cache("token." . CMS_USER . ".category");
    $object   = token_get_index($token, $category);

    if ($object === FALSE)
    {
        $category = "";
        $object   = token_get_index($token, $category);
    };
};

//menu
$menu = NULL;
if (CMS_IFC_SELECT && $object)
    $menu[t("command_insert") . "|token/command_apply"] = "javascript:ifc_return();";
if ($token->operator)
{
    $menu[t("command_add") . "|token/command_add"] = "add";
    if ($object)
    {
        $menu[t("command_edit") .    "|token/command_edit"]   = "edit";
        $menu[t("command_delete") .  "|token/command_delete"] = "#delete";
        $menu[t("rename_category") . "|token/command_rename"] = "category_rename";
    };
    $menu[t("clear_placeholder_cache") . "|token/command_clear"] = "#clear_cache";
};

//display
$ifc = new ifc(
    $ifc_response,
    $ifc_page,
    $menu,
    ["object" => $object]);

?><script>

function token_select(value)
{
    ifc_set("object", value);
    ifc_set("token", "%%" + value + "%%");
    document.getElementById("token-display").src = "<?php
        echo(str_replace("%replace%",
                         "\"+encodeURIComponent(value)+\"",
                         q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "display",
                                    "object"      => "\x1B%replace%"]))));?>";
};

</script><?php

//category
ifc_table_open();
echo("<colgroup>" .
     "<col style=\"WIDTH:0\">" .
     "<col>" .
     "</colgroup>" .

     "<tr>" .
     "<td>");

//category
$ifc->set(t("category"), "description b");
echo("<select class=\"list\" " .
             "onchange=\"ifc_post('select',this.value);\">" .
     "<option value=\"\"" .
              (nstre($category) ? " selected" : "") . ">" .
     "</option>");

$result = mysql_query("SELECT DISTINCT " . CMS_DB_TOKEN_CATEGORY . " " .
                      "FROM " .            CMS_DB_TOKEN . " " .
                      "WHERE " .           CMS_DB_TOKEN_CATEGORY . "!='' " .
                      "ORDER BY " .        CMS_DB_TOKEN_CATEGORY);

while ($resultrow = mysql_fetch_assoc($result))
{
    $value = $resultrow[CMS_DB_TOKEN_CATEGORY];
    $name  = strabridge($value, 40);
    echo("<option value=\"" . x($value) . "\"" .
                  (streq($resultrow[CMS_DB_TOKEN_CATEGORY], $category) ? " selected>" : ">") .
         x($name) .
         "</option>");
};
echo("</select>");

//list
if (
     ($result = mysql_query("SELECT " .   CMS_DB_TOKEN_INDEX . ", " .
                                          CMS_DB_TOKEN_VALUE . ", " .
                                          CMS_DB_TOKEN_TITLE . " " .
                            "FROM " .     CMS_DB_TOKEN . " " .
                            "WHERE " .    CMS_DB_TOKEN_CATEGORY . "='" . sqlesc($category) . "'"))
     &&
     mysql_num_rows($result)
   )
{
    $array   = [];
    $_object = "";

    while ($resultrow = mysql_fetch_assoc($result))
    {
        $value  = $resultrow[CMS_DB_TOKEN_INDEX];
        $_value = $resultrow[CMS_DB_TOKEN_VALUE];
        $_value = stre($_value) ? $value : "$value " . str_replace(",", ", ", $_value);
        $name   = l($resultrow[CMS_DB_TOKEN_TITLE]);

        $_name = $name;
        $i     = 1;
        while (isset($array[$_name]))
            $_name = $name . " (" . $i++ . ")";

        $array[$_name] = $_value;

        if (stre($object))          $object  = $value;
        if (streq($value, $object)) $_object = $_value;
    };

    ksort($array, SORT_NATURAL | SORT_FLAG_CASE);

    echo("<br>");

    $ifc->set(t("placeholders"), "description b");
    echo("<div id=\"_object\" " .
              "data-size=\"14\" " .
              "class=\"select list\" " .
              "data-onchange=\"token_select(this.value);\" " .
              "ondblclick=\"ifc_return();\">");
    foreach ($array AS $key => $value)
    {
        echo("<label>" .
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

    //token
    $ifc->set(t("code"), "description b");
    echo("<input name=\"token\" " .
                "type=\"text\" " .
                (stre($_object) ? "" : "value=\"%%" . x($_object) . "%%\" ") .
                "onfocus=\"this.select();\" " .
                "readonly " .
                "class=\"select\">");
};
echo("</td>" .

//display token
     "<td class=\"iframe\">" .
     "<iframe id=\"token-display\" " .
             "src=\"" . x(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                   "ifc_message" => "display",
                                   "object"      => $object])) . "\" " .
             "sandbox=\"allow-same-origin\">" .
     t("preview_unavailable") .
     "</iframe>" .
     "</td>" .
     "</tr>");
ifc_table_close();

$ifc->close();