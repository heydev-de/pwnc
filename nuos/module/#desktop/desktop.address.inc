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

case "add":
//..................................................................................................

    if ($key = $desktop->data->seek(["#subtype" => "address"]))
    {
        $name = ($ifc_param || ($initial === "-")) ? NULL : $initial;
        $desktop->data->set_buffer(
            [["name"    => $name,
              "#type"   => "address",
              "company" => $ifc_param]]);

        if (
             ($_object = $desktop->data->insert($key))
             &&
             $desktop->save()
           )
        {
            $object       = $_object;
            $initial      = NULL;
            $ifc_response = CMS_MSG_DONE;
            break;
        };
    };

    $ifc_response = CMS_MSG_ERROR;
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
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "delete":
//..................................................................................................

    if ($desktop->delete_object($object))
    {
        $object       = NULL;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

if (blank($initial))
{
    $initial = $desktop->object_get($object, "company") . $desktop->object_get($object, "name");
    $initial = $initial ? strtoupper($initial[0]) : "-";
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

        $name    = (string)$desktop->object_get($key, "name");
        $company = (string)$desktop->object_get($key, "company");
        $char    = ($char = $company . $name) ? strtoupper($char[0]) : "-";
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
$menu = [CMS_L_COMMAND_ADD . "|desktop/command_create" => "add"];
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

        $menu[CMS_L_COMMAND_INSERT] = "javascript:ifc_return('" . q($value) . "');";
    };

    $menu[CMS_L_COMMAND_SAVE .   "|desktop/command_save"]   = "save";
    $menu[CMS_L_COMMAND_DELETE . "|desktop/command_delete"] = "#delete";
};

$ifc = new ifc(NULL,
               $ifc_page,
               $menu,
               array_merge(
                   $param,
                   ["object"  => $object,
                    "initial" => $initial]),
               NULL,
               CMS_L_DESKTOP_ADDRESS_006);

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
     CMS_L_NAME .
     "</th>" .
     "<th>" .
     CMS_L_DESKTOP_ADDRESS_004 .
     "</th>" .
     "<th>" .
     CMS_L_EMAIL .
     "</th>" .
     "</tr>" .

     "<tr>" .
     "<td colspan=\"3\" " .
         "class=\"highlight\">" .
     "<a href=\"javascript:ifc_post('add');\">" .
     image("desktop/command_create") . " " . CMS_L_COMMAND_ADD .
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
                         image("desktop/command_create") . " " . CMS_L_COMMAND_ADD .
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
                if ($email = $desktop->object_get($__value, "email"))
                {
                    $_email = x($email);
                    $_param = array_merge(
                        $param,
                        ["desktop_display" => "interface",
                         "user"            => $mailbox_user,
                         "object"          => $mailbox_object,
                         "ifc_message"     => "mail",
                         "to"              => $email]);

                    echo($mailbox_user ?
                         "<a href=\"" . x(cms_url($_param)) . "\">" .
                         image("desktop/icon_mailbox") . " $_email" .
                         "</a>" :
                         $_email);
                };
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
    $ifc->set(CMS_L_NAME,
              "text 40 40",
              $desktop->object_get($object, "name"));
    echo("</th>" .
         "</tr>" .
         "<tr>" .
         "<td class=\"varied\">");
    $ifc->set(CMS_L_DESKTOP_ADDRESS_003,
              "text 40 80 b",
              $desktop->object_get($object, "company"));
    $ifc->set(CMS_L_DESKTOP_ADDRESS_004,
              "text 40 40 b",
              $desktop->object_get($object, "phone"));
    $ifc->set(CMS_L_DESKTOP_ADDRESS_005,
              "text 40 80 b",
              $desktop->object_get($object, "fax"));
    $ifc->set(CMS_L_EMAIL,
              "text 40 80 b",
              $desktop->object_get($object, "email"));
    $ifc->set(CMS_L_DESKTOP_ADDRESS_001,
              "textarea 40x4 256 b",
              $desktop->object_get($object, "address"));
    $ifc->set(CMS_L_DESKTOP_ADDRESS_002,
              "textarea 40x4 256",
              $desktop->object_get($object, "comment"));
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    echo("</td>");
};

echo("</tr>" .
     "</table>");

$ifc->close();