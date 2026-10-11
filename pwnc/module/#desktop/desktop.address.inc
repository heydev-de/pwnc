<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   DESKTOP INTERFACE
//==================================================================================================

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "select":
//..................................................................................................

    $object  = $ifc_param;
    $initial = NULL;
    break;

case "initial":
//..................................................................................................

    $object  = NULL;
    $initial = $ifc_param;
    break;

case "address":
//..................................................................................................

    //find existing
    if ($object = $desktop->data->seek(["#type" => "address",
                                        "email" => $email]))
        break; //found

    $ifc_param = $group;
    if (stre($name)) $name = $email;

case "add":
//..................................................................................................

    if ($key = $desktop->data->seek(["#subtype" => "address"]))
    {
        $name ??= (nstre($ifc_param) || ($initial === "-")) ? NULL : $initial;
        $desktop->data->set_buffer(
            [["name"    => $name,
              "#type"   => "address",
              "company" => $ifc_param,
              "email"   => $email ?? NULL]]);

        if (
             ($_object = $desktop->data->insert($key))
             &&
             $desktop->save()
           )
        {
            $object       = $_object;
            $initial      = NULL;
            $ifc_response = t("msg_done");
            break;
        };
    };

    $ifc_response = t("msg_error");
    break;

case "save":
//..................................................................................................

    $desktop->object_set($object, "name",    $ifc_param1);
    $desktop->object_set($object, "company", $ifc_param2);
    $desktop->object_set($object, "phone",   $ifc_param3);
    $desktop->object_set($object, "fax",     $ifc_param4);
    $desktop->object_set($object, "email",   $ifc_param5);
    $desktop->object_set($object, "address", $ifc_param6);
    $desktop->object_set($object, "comment", $ifc_param7);

    if ($desktop->save())
    {
        $initial      = NULL;
        $ifc_response = t("msg_done");
        break;
    };

    $ifc_response = t("msg_error");
    break;

case "delete":
//..................................................................................................

    if (! blank($object) && $desktop->delete_object($object))
    {
        $object       = NULL;
        $ifc_response = t("msg_done");
        break;
    };

    $ifc_response = t("msg_error");
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

if (blank($initial))
{
    $initial = $desktop->object_get($object, "company") . $desktop->object_get($object, "name");
    $initial = nstre($initial) ? strtoupper($initial[0]) : "-";
    if (strpos("ABCDEFGHIJKLMNOPQRSTUVWXYZ", $initial) === FALSE) $initial = "-";
};

$array          = NULL;
$initial_count  = NULL;
$mailbox_user   = NULL;
$mailbox_object = NULL;

$desktop->data->move("first");
while($key = $desktop->data->move("next"))
{
    switch ($desktop->object_type($key))
    {
    case "address":
//..................................................................................................

        $name    = $desktop->object_get($key, "name")    ?? "";
        $company = $desktop->object_get($key, "company") ?? "";
        $char    = nstre($char = $company . $name) ? strtoupper($char[0]) : "-";
        if (strpos("ABCDEFGHIJKLMNOPQRSTUVWXYZ", $char) === FALSE) $char = "-";

        if (isset($initial_count[$char])) $initial_count[$char]     += 1;
        else                              $initial_count[$char]      = 1;

        if ($char === $initial)           $array[$company][$name][]  = $key;
        break;

    case "mailbox":
//..................................................................................................

        $mailbox_user   = DESKTOP_USER;
        $mailbox_object = $key;
    };
};

//menu
$menu = [t("command_add") . "|desktop/command_create" => "add"];
if ($object)
{
    //return value
    if (CMS_IFC_SELECT && $desktop->object_get($object, CMS_IFC_SELECT))
    {
        switch (CMS_IFC_SELECT)
        {
        case "email":

            $value = "\"" . $desktop->object_get($object, "name") . "\" " .
                     "<" . $desktop->object_get($object, "email") . ">";
            break;

        default:

            $value = $desktop->object_get($object, "name");
            break;
        };

        $menu[t("command_insert")] = "javascript:ifc_return('" . q($value) . "');";
    };

    $menu[t("command_save") .   "|desktop/command_save"]   = "save";
    $menu[t("command_delete") . "|desktop/command_delete"] = "#delete";
};

$ifc = new ifc(
    NULL,
    $ifc_page,
    $menu,
    array_merge(
        $param,
        ["object"  => $object,
         "initial" => $initial]),
    NULL,
    t("contact"));

if (isset($mailbox_user))
{

?><script>

function m(value) //mail
{
    load_page("<?php
        echo(str_replace(
            "%replace%",
            "\"+encodeURIComponent(value)+\"",
            q(cms_url(array_merge(
                $param,
                ["desktop_display" => "interface",
                 "user"            => $mailbox_user,
                 "object"          => $mailbox_object,
                 "ifc_message"     => "mail",
                 "to"              => "\x1B%replace%"])))));?>");
};

</script><?php

};

echo("<table class=\"layout\">" .
     "<tr>" .
     "<td style=\"WIDTH:0\">");

//initial list
ifc_table_open();
for ($i = 65; $i <= 90; $i++)
{
    $char = chr($i);

    echo("<tr>" .
         "<td" . (($char === $initial) ? " class=\"enabled\"" : (isset($initial_count[$char]) ? " class=\"highlight\"" : "")) . ">" .
         "<a href=\"javascript:ifc_post('initial','$char');\">" .
         (($char === $initial)         ? sprintf("<strong>%s</strong>", $char)                 : $char) .
         (isset($initial_count[$char]) ? sprintf(" (%d)",               $initial_count[$char]) : NULL) .
         "</a>" .
         "</td>" .
         "</tr>");
};
echo("<tr>" .
     "<td" . (($initial === "-") ? " class=\"enabled\"" : (isset($initial_count["-"]) ? " class=\"highlight\"" : "")) . ">" .
     "<a href=\"javascript:ifc_post('initial','-');\">" .
     (($initial === "-")         ? "<strong>…</strong>"                  : "…") .
     (isset($initial_count["-"]) ? sprintf(" (%d)", $initial_count["-"]) : NULL) .
     "</a>" .
     "</td>" .
     "</tr>");
ifc_table_close();

echo("</td>" .

//address list
     "<td>");

ifc_table_open();
echo("<tr>" .
     "<th>" .
     t("name") .
     "</th>" .
     "<th>" .
     t("phone") .
     "</th>" .
     "<th>" .
     t("email") .
     "</th>" .
     "</tr>" .

     "<tr>" .
     "<td colspan=\"3\" " .
         "class=\"highlight\">" .
     "<a href=\"javascript:ifc_post('add');\">" .
     image("desktop/command_create") . " " . t("command_add") .
     "</a>" .
     "</td>" .
     "</tr>");

$company = NULL;

if ($array)
{
    ksort($array, SORT_NATURAL | SORT_FLAG_CASE);

    foreach ($array AS $key => $value)
    {
        ksort($value, SORT_NATURAL | SORT_FLAG_CASE);

        foreach ($value AS $_key => $_value)
        {
            foreach ($_value AS $__key => $__value)
            {
                $_key   = x($_key);
                $varied = ifc_varied();

                if (nstreq($key, $company))
                {
                    echo("<tr>" .
                         "<th colspan=\"3\">" .
                         x($key) .
                         "</th>" .
                         "</tr>" .

                         "<tr>" .
                         "<td colspan=\"3\" " .
                             "class=\"highlight\">" .
                         "<a href=\"javascript:ifc_post('add','" . qx($key) . "');\">" .
                         image("desktop/command_create") . " " . t("command_add") .
                         "</a>" .
                         "</td>" .
                         "</tr>");

                    $company = $key;
                };

                echo("<tr>" .
                     "<td$varied>" .
                     "<a href=\"javascript:ifc_post('select','" . qx($__value) . "');\">" .
                     image("desktop/icon_address") . " " .
                     (streq($__value, $object) ? sprintf("<strong>%s</strong>", $_key) : $_key) .
                     "</a>" .
                     "</td>" .
                     "<td$varied>" .
                     x($desktop->object_get($__value, "phone")) .
                     "</td>" .
                     "<td$varied>");
                if (nstre($email = $desktop->object_get($__value, "email")))
                    echo(isset($mailbox_user) ?
                         "<a href=\"javascript:m('" . qx($email) . "');\">" .
                         image("desktop/icon_mailbox") . " " . x($email) .
                         "</a>" :
                         x($email));
                echo("</td>" .
                     "</tr>");
            };
        };
    };
};
ifc_table_close();

echo("</td>");

if ($object)
{
    //data
    echo("<td style=\"WIDTH:0\">");

    ifc_table_open();
    echo("<tr>" .
         "<th>");
    $ifc->set(t("name"), "text 40", $desktop->object_get($object, "name"));
    echo("</th>" .
         "</tr>" .
         "<tr>" .
         "<td class=\"varied\">");
    $ifc->set(t("organization"), "text 40 b",       $desktop->object_get($object, "company"));
    $ifc->set(t("phone"),        "text 40 b",       $desktop->object_get($object, "phone"));
    $ifc->set(t("fax"),          "text 40 b",       $desktop->object_get($object, "fax"));
    $ifc->set(t("email"),        "text 40 b",       $desktop->object_get($object, "email"));
    $ifc->set(t("contact"),      "textarea 40x4 b", $desktop->object_get($object, "address"));
    $ifc->set(t("memo"),         "textarea 40x4",   $desktop->object_get($object, "comment"));
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    echo("</td>");
};

echo("</tr>" .
     "</table>");

$ifc->close();