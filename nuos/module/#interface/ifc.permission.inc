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

init($object);
init($type, "user");

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "select":
//..................................................................................................

    $object = $ifc_param;
    break;

case "select_user":
//..................................................................................................

    //clear selected object when changing type
    if (nstreq($type, "user")) $object = NULL;

    $type = "user";
    break;

case "select_group":
//..................................................................................................

    //clear selected object when changing type
    if (nstreq($type, "group")) $object = NULL;

    $type = "group";
    break;

case "add_user":
//..................................................................................................

    //display
    $ifc = new ifc(NULL,
                   $ifc_page,
                   TRUE,
                   ["object" => $object,
                    "type"   => $type],
                   "_add_user",
                   CMS_L_IFC_PERMISSION_001);

    //name
    $ifc->set(CMS_L_NAME,               "text 40 40 b");

    //user id
    $ifc->set(CMS_L_IFC_PERMISSION_002, "text 40 40 b");

    //password
    $ifc->set(CMS_L_PASSWORD,           "password 40 40 b");
    $ifc->set(CMS_L_IFC_PERMISSION_003, "password 40 40");

    $ifc->close();

case "_add_user":
//..................................................................................................

    //password matches repetition
    if (streq($ifc_param3, $ifc_param4))
    {
        $permission = new permission();

        //user does not exist
        if (! $permission->data->get("user.$ifc_param2"))
        {
            //create user
            if ($_object = $permission->user($ifc_param2,  //user
                                             TRUE,         //disabled
                                             $ifc_param1,  //name
                                             $ifc_param3)) //password
            {
                //save changes
                if ($permission->save())
                {
                    $object       = $_object;
                    $type         = "user";
                    $ifc_response = CMS_MSG_DONE;
                    break;
                };
            };
        };
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "add_group":
//..................................................................................................

    //display
    $ifc = new ifc(NULL,
                   $ifc_page,
                   TRUE,
                   ["object" => $object],
                   "_add_group",
                   CMS_L_IFC_PERMISSION_004);

    //name
    $ifc->set(CMS_L_NAME,               "text 40 40 b");

    //group id
    $ifc->set(CMS_L_IFC_PERMISSION_005, "text 40 40");

    $ifc->close();

case "_add_group":
//..................................................................................................

    $permission = new permission();

    //group does no exist
    if (! $permission->data->get("group.$ifc_param2"))
    {
        //create group
        if ($_object = $permission->group($ifc_param2,  //group
                                          TRUE,         //disabled
                                          $ifc_param1)) //name
        {
            //save changes
            if ($permission->save())
            {
                $object       = $_object;
                $type         = "group";
                $ifc_response = CMS_MSG_DONE;
                break;
            };
        };
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "save_user":
//..................................................................................................

    $permission = new permission();

    //no password given or password matches repetition
    if (nstre($ifc_param3) && nstreq($ifc_param3, $ifc_param4))
    {
        $ifc_param3   = NULL;
        $ifc_response = CMS_L_IFC_PERMISSION_033;
    };

    $ifc_param5  = blank($ifc_param5)  ? NULL : implode("\n", $ifc_param5); //group
    $ifc_param11 = blank($ifc_param11) ? NULL : $ifc_param11;               //expire

    //apply changes
    if ($permission->user(substr($object, 5), //user
                          isset($ifc_param2), //disabled
                          $ifc_param1,        //name
                          $ifc_param3,        //password
                          $ifc_param5,        //group
                          $ifc_param6,        //permission
                          $ifc_param7,        //exclusion
                          $ifc_param8,        //email
                          $ifc_param9,        //timezone
                          $ifc_param10,       //comment
                          $ifc_param11))      //expire
    {
        //save changes
        if ($permission->save())
        {
            $ifc_response = CMS_MSG_DONE . $ifc_response;
            break;
        };
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "save_group":
//..................................................................................................

    $permission = new permission();

    //apply changes
    if ($permission->group(substr($object, 6), //group
                           isset($ifc_param2), //disabled
                           $ifc_param1,        //name
                           $ifc_param4,        //permission
                           $ifc_param5,        //exclusion
                           $ifc_param6))       //comment
    {
        init($ifc_param3, []);

        //apply group assignments to users
        $permission->data->move("first");
        while ($key = $permission->data->move("next"))
        {
            if (! permission_is_user($key))  continue; //skip groups
            $_key = substr($key, 5);

            if (in_array($key, $ifc_param3)) $permission->add_group($_key, $object);
            else                             $permission->del_group($_key, $object);
        };

        //save changes
        if ($permission->save())
        {
            $ifc_response = CMS_MSG_DONE;
            break;
        };
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "activate":
//..................................................................................................

    if (empty($list)) break; //no selection

    $permission = new permission();

    //delete disabled flags of selected objects
    foreach ($list AS $value)
        $permission->data->del($value, "disabled");

    //save changes
    $ifc_response = $permission->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "deactivate":
//..................................................................................................

    if (empty($list)) break; //no selection

    $permission = new permission();

    //set disabled flags of selected objects
    foreach ($list AS $value)
        $permission->data->set(TRUE, $value, "disabled");

    //save changes
    $ifc_response = $permission->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "delete":
//..................................................................................................

    if (empty($list)) break; //no selection

    $permission = new permission();

    //delete selected objects
    //deleted groups are automatically removed from users
    foreach ($list AS $value)
        $permission->delete($value);

    //deselect object if deleted
    if (in_array($object, $list)) $object = NULL;

    //save changes
    $ifc_response = $permission->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "set":
//..................................................................................................

    $ifc_param1 = blank($access) ? cms_cache("permission." . CMS_USER . ".access") : $access;

case "_set":
case "set_add":
case "set_add_ex":
case "set_del":
case "set_del_ex":
//..................................................................................................

    $permission = new permission();

    //permit or exclude access for selected users / groups
    if (
         ($ifc_param1 = utf8_trim($ifc_param1)) //access
         &&
         ((! empty($ifc_param2)) || (! empty($ifc_param3)))
         &&
         preg_match("/^set_(?:add|del)(?:_ex)?$/", CMS_IFC_MESSAGE)
       )
    {
        switch (CMS_IFC_MESSAGE)
        {
        case "set_add_ex":
//..................................................................................................

            $explicit = TRUE;

        case "set_add":
//..................................................................................................

            $explicit = isset($explicit);

            //give permission to selected users / groups
            if (! empty($ifc_param3))
            {
                foreach ($ifc_param3 AS $value)
                    $permission->permit($value, $ifc_param1, $explicit);
            };

            if (! empty($ifc_param2))
            {
                foreach ($ifc_param2 AS $value)
                    $permission->permit($value, $ifc_param1, $explicit);
            };

            break;

        case "set_del_ex":
//..................................................................................................

            $explicit = TRUE;

        case "set_del":
//..................................................................................................

            $explicit = isset($explicit);

            //deny permission from selected users / groups
            if (! empty($ifc_param2))
            {
                foreach ($ifc_param2 AS $value)
                    $permission->exclude($value, $ifc_param1, $explicit);
            };

            if (! empty($ifc_param3))
            {
                foreach ($ifc_param3 AS $value)
                    $permission->exclude($value, $ifc_param1, $explicit);
            };

            break;
        };

        //save changes
        $ifc_response = $permission->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    };

    //cache last access permanently
    cms_cache("permission." . CMS_USER . ".access", $ifc_param1, TRUE);

    $data      = &$permission->data;
    $array     = [];
    $permitted = [];
    $excluded  = [];
    $pattern   = "/(?:^|\r?\n)\s*" . preg_quote($ifc_param1, "/") . "\s*(?:$|\r?\n)/";

    data_sort($data, "name");

    $data->move("first");
    while ($key = $data->move("next"))
    {
        if ($data->get($key, "disabled")) continue; //skip disabled users / groups

        $name = $data->get($key, "name");

        //group
        if (permission_is_group($key))
        {
            $name             = "[" . utf8_strtoupper($name) . "]";
            $group_permission = $data->get($key, "permission");
            $group_exclusion  = $data->get($key, "exclusion");
            $flag_permitted   = FALSE;
            $flag_excluded    = FALSE;

            //group permission given
            if (permission_match($ifc_param1, $group_permission, $group_exclusion))
            {
                $_name                    = preg_match($pattern, $group_permission) ? " ⁺" : "";
                $permitted["$name$_name"] = $key;
                $flag_permitted           = TRUE;
            }

            //group permission not given
            else
            {
                $_name                   = preg_match($pattern, $group_exclusion) ? " ⁻" : "";
                $excluded["$name$_name"] = $key;
                $flag_excluded           = TRUE;
            };

            //check users in current group
            $data->move("first");
            while ($_key = $data->move("next"))
            {
                if ($data->get($_key, "disabled")) continue; //skip disabled
                if (! permission_is_user($_key))   continue; //skip anything but users

                //skip users not in current group
                $group = $data->get($_key, "group");
                if (! preg_match("/(?:^|\r?\n)" . preg_quote($key, "/") . "(?:$|\r?\n)/", $group)) continue;

                $_name = "  " . $data->get($_key, "name");
                $i     = 1;

                //get user permission / exclusion
                $user  = substr($_key, 5);
                if (isset($array[$user])) //buffered
                {
                    $user_permission = $array[$user][0];
                    $user_exclusion  = $array[$user][1];
                }
                else
                {
                    $user_permission = $permission->get_user_permission($user);
                    $user_exclusion  = $permission->get_user_exclusion($user);
                    $array[$user][0] = $user_permission;
                    $array[$user][1] = $user_exclusion;
                };

                $_user_permission = $data->get($_key, "permission");

                //user permission given
                if (permission_match($ifc_param1, $user_permission, $user_exclusion))
                {
                    //add group to permission list if needed
                    if (! $flag_permitted)
                    {
                        $permitted[$name] = $key;
                        $flag_permitted   = TRUE;
                    };

                    //add user to permission list
                    $_name  = "\xE2\x80\x83" . //em space
                              $_name .
                              (preg_match($pattern, $_user_permission) ? " ⁺" : "");
                    $__name = $_name;
                    while (isset($permitted[$__name])) $__name = $_name . " (" . $i++ . ")";

                    $permitted[$__name] = $_key;
                }

                //user permission not given
                else
                {
                    //add group to exclusion list if needed
                    if (! $flag_excluded)
                    {
                        $excluded[$name] = $key;
                        $flag_excluded   = TRUE;
                    };

                    //add user to exclusion list
                    $_user_exclusion = $data->get($_key, "exclusion");
                    $_name           = "\xE2\x80\x83" . //em space
                                       $_name .
                                       (preg_match($pattern, $_user_exclusion)  ? " ⁻" :
                                       (preg_match($pattern, $_user_permission) ? " ⁺" : ""));
                    $__name          = $_name;
                    while (isset($excluded[$__name])) $__name = $_name . " (" . $i++ . ")";

                    $excluded[$__name] = $_key;
                };
            };

            //continue after group position
            $data->move("to", $key);
        }

        //user
        else
        {
            //get user permission / exclusion
            $user = substr($key, 5);
            if (isset($array[$user])) //buffered
            {
                $user_permission = $array[$user][0];
                $user_exclusion  = $array[$user][1];
            }
            else
            {
                $user_permission = $permission->get_user_permission($user);
                $user_exclusion  = $permission->get_user_exclusion($user);
                $array[$user][0] = $user_permission;
                $array[$user][1] = $user_exclusion;
            };

            $_user_permission = $data->get($key, "permission");

            //user permission given
            if (permission_match($ifc_param1, $user_permission, $user_exclusion))
            {
                //add user to permission list
                $name  .= preg_match($pattern, $_user_permission) ? " ⁺" : "";
                $_name  = $name;
                while (isset($permitted[$_name])) $_name = $name . " (" . $i++ . ")";

                $permitted[$_name] = $key;
            }

            //user permission not given
            else
            {
                //add user to exclusion list
                $_user_exclusion  = $data->get($key, "exclusion");
                $name            .= (preg_match($pattern, $_user_exclusion)  ? " ⁻" :
                                    (preg_match($pattern, $_user_permission) ? " ⁺" : ""));
                $_name            = $name;
                while (isset($excluded[$_name])) $_name = $name . " (" . $i++ . ")";

                $excluded[$_name] = $key;
            };
        };
    };

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   [CMS_L_COMMAND_CLOSE . "|permission/command_cancel" => FALSE],
                   ["type" => $type],
                   "_set",
                   CMS_L_IFC_PERMISSION_014);

    ifc_table_open("min");
    echo("<tr>" .
         "<td colspan=\"3\">");

    //access
    $ifc->set(CMS_L_IFC_PERMISSION_032, "text 40",  $ifc_param1);

    //refresh button
    $ifc->set(CMS_L_COMMAND_REFRESH,    "button",   "_set");
    echo("</td>" .
         "</tr>" .
         "<tr>" .

    //permitted
         "<th>" .
         image("permission/icon_access") . " " . CMS_L_IFC_PERMISSION_015 .
         "</th>" .

    //space
         "<td class=\"select\">" .
         "</td>" .

    //excluded
         "<th>" .
         image("permission/icon_denial") . " " . CMS_L_IFC_PERMISSION_016 .
         "</th>" .
         "</tr>" .
         "<tr>" .

    //permitted
         "<td class=\"varied\">");

    //permitted users / groups list
    $ifc->set($permitted,               "multiselect 30x15 b");
    $ifc->set(CMS_L_IFC_PERMISSION_030, "info");
    echo("</td>" .

    //explicit control
         "<td class=\"select\" " .
             "style=\"TEXT-ALIGN:center;VERTICAL-ALIGN:middle\">");

    //explicit permit button
    $ifc->set("‹",                      "button b", "set_add_ex");

    //explicit exclude button
    $ifc->set("›",                      "button",   "set_del_ex");
    echo("</td>" .

    //excluded
         "<td class=\"varied\">");

    //excluded users / group list
    $ifc->set($excluded,                "multiselect 30x15 b");
    $ifc->set(CMS_L_IFC_PERMISSION_031, "info");
    echo("</td>" .
         "</tr>" .

    //control
         "<tr>" .
         "<td colspan=\"3\" " .
             "class=\"select\" " .
             "style=\"TEXT-ALIGN:center\">");

    //delete permission button
    $ifc->set(CMS_L_IFC_PERMISSION_028, "button",   "set_del");

    //delete exclusion button
    $ifc->set(CMS_L_IFC_PERMISSION_029, "button",   "set_add");
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    $ifc->set(CMS_L_IFC_PERMISSION_013, "info");

    $ifc->close();
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

$count = strlen($type);

//set type to match selected object
if ($object && strncmp($object, $type, $count))
    $type = substr($object, 0, stripos($object, "."));

//menu
$menu = [CMS_L_IFC_PERMISSION_001 .      "|permission/command_create_user"  => "add_user",
         CMS_L_IFC_PERMISSION_004 .      "|permission/command_create_group" => "add_group",
         CMS_L_IFC_PERMISSION_026 .      "|permission/command_enable"       => "activate",
         CMS_L_IFC_PERMISSION_027 .      "|permission/command_disable"      => "deactivate",
         CMS_L_COMMAND_DELETE_SELECTED . "|permission/command_delete"       => "#delete",
         CMS_L_IFC_PERMISSION_014 .      "|permission/command_permission"   => "set"];

//display
$ifc = new ifc($ifc_response,
               $ifc_page,
               $menu,
               ["object" => $object,
                "type"   => $type]);

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

//type selection
     "<tr>" .
     "<td colspan=\"3\">");
$ifc->set(CMS_L_IFC_PERMISSION_021, "button", "select_user");
$ifc->set(CMS_L_IFC_PERMISSION_022, "button", "select_group");
echo("</td>" .
     "</tr>" .

//header
     "<tr>" .

//selection
     "<td class=\"select\">" .
     "</td>" .

//name
     "<th>" .
     CMS_L_NAME .
     "</th>" .

//description
     "<th>" .
     CMS_L_IFC_PERMISSION_006 .
     "</th>" .
     "</tr>");

$permission = new permission();
$data       = &$permission->data;
data_sort($data, "name");

$initial = NULL;
$_list   = NULL;
$__list  = NULL;

$data->move("first");
while ($key = $data->move("next"))
{
    $name = $data->get($key, "name");

    if (strncmp($key, $type, $count)) //not selected type
    {
        //get associations
        if ($object)
        {
            $_list[$name] = $key;

            if (
                 streq($type, "group")
                 &&
                 preg_match("/\n\s*" . preg_quote($object, "/") . "\s*\n/",
                            "\n" . $data->get($key, "group") . "\n")
               )
                $__list[] = $key;
        };

        continue; //skip display
    };

    $varied   = ifc_varied();
    $_initial = utf8_strtoupper(utf8_substr($name, 0, 1));

    //initial separator
    if (nstreq($initial, $_initial))
    {
        $initial = $_initial;
        echo("<tr>" .

        //spacer
             "<td class=\"highlight\">" .
             "</td>" .

        //initial
             "<td colspan=\"2\" " .
                 "class=\"highlight\">" .
             x($initial) .
             "</td>" .
             "</tr>");
    };

    echo("<tr>" .

    //selection
         "<td class=\"select\">");
    $ifc->set(NULL, "checkbox", $key, streq($key, $object), "list[]");
    echo("</td>" .

    //name
         "<td$varied>" .

    //icon
         "<a href=\"javascript:ifc_post('select','" . qx($key) . "');\">" .
         image($data->get($key, "disabled") ?
               "permission/icon_" . $type . "_disabled" :
               "permission/icon_" . $type));

    //name
    $name = x($name);
    if (streq($key, $object))
        $name = "<strong>$name</strong>";

    echo(" $name" .
         "</a>" .
         "</td>" .

    //comment
         "<td$varied>" .
         "<span class=\"br\">" .
         x($data->get($key, "comment")) .
         "</span>" .
         "</td>" .
         "</tr>");
};

//selection control
echo("<tr>" .
     "<td colspan=\"3\" " .
         "class=\"select\" " .
         "style=\"TEXT-ALIGN:right\">");
$ifc->set(CMS_L_ALL,    "button", "javascript:ifc_list_activate();");
$ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert();");
$ifc->set(CMS_L_NONE,   "button", "javascript:ifc_list_deactivate();");
echo("</td>" .
     "</tr>");
ifc_table_close();

echo("</td>");

//properties
if ($object)
{
    echo("<td>");

    ifc_table_open();

    switch ($type)
    {
    case "group":
//..................................................................................................

        //group id
        echo("<tr>" .
             "<th>" .
             x(substr($object, 6)) .
             "</th>" .
             "</tr>" .

             "<tr>" .
             "<td class=\"varied\">");

        //name
        $ifc->set(CMS_L_NAME,               "text 30 40",          $data->get($object, "name"));

        //save button
        $ifc->set(CMS_L_COMMAND_SAVE,       "button b",            "save_group");

        //disabled flag
        $ifc->set(CMS_L_IFC_PERMISSION_008, "checkbox b",          TRUE, $data->get($object, "disabled"));

        //users
        $ifc->set(CMS_L_IFC_PERMISSION_012, "label");
        $ifc->set($_list,                   "multiselect 40x8 b",  $__list);

        //access
        $ifc->set(CMS_L_IFC_PERMISSION_019, "title");

        //permissions
        $ifc->set(CMS_L_IFC_PERMISSION_009, "textarea 40x8 b",     $data->get($object, "permission"));

        //exclusions
        $ifc->set(CMS_L_IFC_PERMISSION_018, "textarea 40x8 b",     $data->get($object, "exclusion"));
        $ifc->set(CMS_L_IFC_PERMISSION_013, "info");

        //extended
        $ifc->set(CMS_L_IFC_PERMISSION_020, "title");

        //comment
        $ifc->set(CMS_L_IFC_PERMISSION_006, "textarea 40x4 256 b", $data->get($object, "comment"));

        break;

    case "user":
//..................................................................................................

        //user id
        echo("<tr>" .
             "<th>" .
             x(substr($object, 5)) .
             "</th>" .
             "</tr>" .

             "<tr>" .
             "<td class=\"varied\">");

        //name
        $ifc->set(CMS_L_NAME,               "text 30 40",          $data->get($object, "name"));

        //save button
        $ifc->set(CMS_L_COMMAND_SAVE,       "button b",            "save_user");

        //disabled flag
        $ifc->set(CMS_L_IFC_PERMISSION_008, "checkbox b",          TRUE, $data->get($object, "disabled"));

        //password
        $ifc->set(CMS_L_PASSWORD,           "password 40 40 b",    NULL);
        $ifc->set(CMS_L_IFC_PERMISSION_003, "password 40 40 b",    NULL);

        //groups
        $ifc->set(CMS_L_IFC_PERMISSION_007, "label");
        $ifc->set($_list,                   "multiselect 40x8 b",  preg_split("/\r?\n/", $data->get($object, "group")));

        //access
        $ifc->set(CMS_L_IFC_PERMISSION_019, "title");

        //permissions
        $ifc->set(CMS_L_IFC_PERMISSION_009, "textarea 40x4 b",     $data->get($object, "permission"));

        //exclusions
        $ifc->set(CMS_L_IFC_PERMISSION_018, "textarea 40x4 b",     $data->get($object, "exclusion"));
        $ifc->set(CMS_L_IFC_PERMISSION_013, "info");

        //extended
        $ifc->set(CMS_L_IFC_PERMISSION_020, "title");

        //email
        $ifc->set(CMS_L_EMAIL,              "text 40 40 b",        $data->get($object, "email"));

        //timezone
        $array = timezone_identifiers_list();
        $array = array_combine($array, $array);
        $value = $data->get($object, "timezone");
        if (! isset($array[$value]))
            $value = CMS_TIMEZONE_DEFAULT;
        $ifc->set(CMS_L_IFC_PERMISSION_017, "label");
        $ifc->set($array,                   "select 40 b",         $value);

        //comment
        $ifc->set(CMS_L_IFC_PERMISSION_006, "textarea 40x4 256 b", $data->get($object, "comment"));

        //expiration in days
        if (! preg_match("/^user\.(admin|anonymous|profile)$/", $object))
            $ifc->set(CMS_L_IFC_PERMISSION_011, "text 40 40", $data->get($object, "expire") / 86400);

        break;
    };

    echo("</td>" .
         "</tr>");
    ifc_table_close();

    echo("</td>");
};

echo("</tr>" .
     "</table>");

$ifc->close();