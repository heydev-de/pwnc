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
if (! cms_load("profile")) ifc_inactive($ifc_page);

//permission
ifc_permission([""                              => CMS_L_ACCESS,
                CMS_PROFILE_PERMISSION_OPERATOR => CMS_L_OPERATOR]);

//instantiate profile object
$_profile = new profile();
if (! $_profile->enabled) ifc_inactive($ifc_page);

//initiate primary interface values
if (isset($_sql_filter_field))  $sql_filter_field  = $_sql_filter_field;
if (isset($_sql_filter_option)) $sql_filter_option = $_sql_filter_option;
if (isset($_sql_filter_value))  $sql_filter_value  = $_sql_filter_value;
if (isset($_group))             $group             = $_group;

init($object);
init($sql_filter_field,  CMS_DB_PROFILE_SURNAME);
init($sql_filter_option, " LIKE '#value#%'");
init($sql_filter_value);
init($sql_order,         CMS_DB_PROFILE_SURNAME);
init($group,             CMS_DB_PROFILE_COMPANY);
init($page,               0);
init($limit,             25);

$page  = (int)$page;
$limit = (int)$limit;

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "add":
//..................................................................................................

    $profile_data = new profile_data();

    if ($_profile->add($profile_data))
    {
        $object       = $profile_data->id;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "delete":
//..................................................................................................

    if (empty($list)) break; //no selection

    $error = NULL;

    foreach ($list AS $value)
    {
        if (! $_profile->del($value)) $error = TRUE;
    };

    $ifc_response = $error ? CMS_MSG_ERROR : CMS_MSG_DONE;
    break;

case "display":
case "display_save":
//..................................................................................................

    switch (CMS_IFC_MESSAGE)
    {
    case "display_save":
//..................................................................................................

        //set base data
        $profile_data                        = $_profile->get($object);
        $profile_data->code                  = $ifc_param1;
        $profile_data->user                  = $ifc_param2;
        $profile_data->password              = $ifc_param4;
        $profile_data->superuser             = $ifc_param5;
        $profile_data->enabled               = isset($ifc_param3);
        $profile_data->company               = $ifc_param6;
        $profile_data->prename               = $ifc_param7;
        $profile_data->surname               = $ifc_param8;
        $profile_data->street                = $ifc_param9;
        $profile_data->zipcode               = $ifc_param10;
        $profile_data->city                  = $ifc_param11;
        $profile_data->country               = $ifc_param12;
        $profile_data->comment               = $ifc_param13;
        $profile_data->phone1                = $ifc_param14;
        $profile_data->phone2                = $ifc_param15;
        $profile_data->mobile                = $ifc_param16;
        $profile_data->fax                   = $ifc_param17;
        $profile_data->email                 = $ifc_param18;
        $profile_data->url                   = $ifc_param19;
        $profile_data->credit_card_number    = $ifc_param20;
        $profile_data->credit_institute      = $ifc_param21;
        $profile_data->credit_card_holder    = $ifc_param22;
        $profile_data->credit_card_validity  = "$ifc_param23/$ifc_param24";
        $profile_data->account_number        = $ifc_param25;
        $profile_data->financial_institution = $ifc_param26;
        $profile_data->bankcode              = $ifc_param27;
        $profile_data->account_holder        = $ifc_param28;

        //set custom data
        for ($i = 1; $i < 21; $i++)
            $profile_data->{"field$i"} = ${"ifc_param" . ($i + 28)};

        //save changes
        $ifc_response = $_profile->set($profile_data) ? CMS_MSG_DONE : CMS_MSG_ERROR;
        break;

    default:
//..................................................................................................

        //no selected or existing profile, close window
        if (blank($object) || (! $profile_data = $_profile->get($object))) ifc_close_external();
    };

    //user list
    $array = ["" => NULL];
    $data  = new data("#system/permission");

    $data->move("first");
    while ($key = $data->move("next"))
    {
        if (permission_is_user($key))
            $array[$data->get($key, "name")] = substr($key, 5);
    };

    //order by name
    ksort($array, SORT_NATURAL | SORT_FLAG_CASE);

    $data = new data("#system/profile");

    //menu
    $menu = $_profile->operator ? [CMS_L_COMMAND_SAVE . "|profile/command_save" => "display_save"] : NULL;

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   $menu,
                   NULL,
                   NULL,
                   $profile_data->name);

    ifc_tab_open(CMS_L_MOD_PROFILE_053);

    //base data
    ifc_table_open();
    echo("<colgroup>" .
         "<col span=\"2\" style=\"WIDTH:50%\">" .
         "</colgroup>" .

         "<tr>" .

    //administration data
         "<th>" .
         CMS_L_IFC_PROFILE_027 .
         "</th>" .

    //personal data
         "<th>" .
         CMS_L_IFC_PROFILE_028 .
         "</th>" .
         "</tr>" .

         "<tr>" .

    //administration data
         "<td class=\"varied\">");

    //code
    $ifc->set(CMS_L_IFC_PROFILE_002, "text 35 40 w", $profile_data->code);

    echo("<table class=\"layout\">" .

         "<colgroup>" .
         "<col>" .
         "<col style=\"WIDTH:0\">" .
         "</colgroup>" .

         "<tr>" .

    //user
         "<td>");
    $ifc->set(CMS_L_IFC_PROFILE_003, "text 30 40 w", $profile_data->user);
    echo("</td>" .

    //enabled flag
         "<td style=\"VERTICAL-ALIGN:middle\">");
    $ifc->set(CMS_L_IFC_PROFILE_041, "checkbox", TRUE, $profile_data->enabled);
    echo("</td>" .
         "</tr>" .

         "<tr>" .

    //password
         "<td>");
    $ifc->set(CMS_L_PASSWORD,        "text 30 40 w");
    echo("</td>" .

    //generate password button
         "<td style=\"VERTICAL-ALIGN:middle\">");
    $ifc->set(CMS_L_IFC_PROFILE_055, "button", "javascript:ifc_set('ifc_param4',unique_id(8));");
    echo("</td>" .
         "</tr>" .
         "</table>");

    //superuser
    $ifc->set(CMS_L_IFC_PROFILE_004, "label");
    $ifc->set($array,                "select 35 w", $profile_data->superuser);

    //created / modified
    $ifc->set(CMS_L_IFC_PROFILE_053 . ": " . x(friendly_date($profile_data->time_created)) . ", " .
              CMS_L_IFC_PROFILE_054 . ": " . x(friendly_date($profile_data->time_updated)),
              "info");
    echo("</td>" .

    //personal data
         "<td class=\"varied\">");

    //company
    $ifc->set(CMS_L_IFC_PROFILE_005, "text 35 255 w", $profile_data->company);

    //prename
    $ifc->set(CMS_L_IFC_PROFILE_006, "text 35 255 w", $profile_data->prename);

    //surname
    $ifc->set(CMS_L_IFC_PROFILE_007, "text 35 255 w", $profile_data->surname);

    //street
    $ifc->set(CMS_L_IFC_PROFILE_008, "text 35 255 w", $profile_data->street);

    echo("<table class=\"layout\">" .
         "<tr>" .

    //zipcode
         "<td style=\"WIDTH:0\">");
    $ifc->set(CMS_L_IFC_PROFILE_009, "text 5 255 w",  $profile_data->zipcode);
    echo("</td>" .

    //city
         "<td>");
    $ifc->set(CMS_L_IFC_PROFILE_010, "text 25 255 w", $profile_data->city);
    echo("</td>" .
         "</tr>" .
         "</table>");

    //country
    $ifc->set(CMS_L_IFC_PROFILE_011, "text 35 255 w", $profile_data->country);
    echo("</td>" .
         "</tr>" .
         "<tr>" .

    //extended data
         "<th>" .
         CMS_L_IFC_PROFILE_030 .
         "</th>" .

    //contact data
         "<th>" .
         CMS_L_IFC_PROFILE_029 .
         "</th>" .
         "</tr>" .
         "<tr>" .

    //extended data
         "<td class=\"varied\">");

    //comment
    $ifc->set(CMS_L_IFC_PROFILE_016, "textarea 35x15 65536 w", $profile_data->comment);
    echo("</td>" .

    //contact data
         "<td class=\"varied\">");

    //phone
    $ifc->set(CMS_L_IFC_PROFILE_012, "text 35 255 w", $profile_data->phone1);
    $ifc->set(CMS_L_IFC_PROFILE_013, "text 35 255 w", $profile_data->phone2);

    //mobile phone
    $ifc->set(CMS_L_IFC_PROFILE_014, "text 35 255 w", $profile_data->mobile);

    //fax
    $ifc->set(CMS_L_IFC_PROFILE_015, "text 35 255 w", $profile_data->fax);

    //email
    echo("<label class=\"element fullwidth\">" .
         "<a href=\"mailto:" . r($profile_data->email) . "\">" .
         CMS_L_EMAIL .
         "</a><br>");
    $ifc->set(NULL, "text 35 255 w", $profile_data->email);
    echo("</label>" .

    //url
         "<label class=\"element fullwidth\">" .
         "<a href=\"" . x(cms_url($profile_data->url, NULL, TRUE)) . "\" " .
            "onclick=\"load_page(this.href);return false;\">" .
         CMS_L_URL .
         "</a><br>");
    $ifc->set(NULL, "text 35 255 w", $profile_data->url);
    echo("</label>" .
         "</td>" .
         "</tr>" .
         "<tr>" .

    //credit card data
         "<th>" .
         CMS_L_IFC_PROFILE_042 .
         "</th>" .

    //bank account data
         "<th>" .
         CMS_L_IFC_PROFILE_043 .
         "</th>" .
         "</tr>" .
         "<tr>" .

    //credit card data
         "<td class=\"varied\">");

    //credit card number
    $ifc->set(CMS_L_IFC_PROFILE_044, "text 35 255 w", $profile_data->credit_card_number);

    //credit institute
    $ifc->set(CMS_L_IFC_PROFILE_045, "text 35 255 w", $profile_data->credit_institute);

    //credit card holder
    $ifc->set(CMS_L_IFC_PROFILE_046, "text 35 255 w", $profile_data->credit_card_holder);

    //credit card validity
    $value = explode("/", $profile_data->credit_card_validity);

    //month
    echo("<label class=\"element\">" .
         "<span>" .
         CMS_L_IFC_PROFILE_047 .
         "</span><br>");
    $ifc->set(["01" => "01", "02" => "02", "03" => "03", "04" => "04",
               "05" => "05", "06" => "06", "07" => "07", "08" => "08",
               "09" => "09", "10" => "10", "11" => "11", "12" => "12"],
              "select",
              $value[0] ?? date("m"));
    echo("<span>/</span>");

    //year
    $ifc->set(NULL, "text 4 252", $value[1] ?? date("Y"));
    echo("</label>" .
         "</td>" .

    //bank account data
         "<td class=\"varied\">");

    //account number
    $ifc->set(CMS_L_IFC_PROFILE_048, "text 35 255 w", $profile_data->account_number);

    //financial institution
    $ifc->set(CMS_L_IFC_PROFILE_049, "text 35 255 w", $profile_data->financial_institution);

    //bank code
    $ifc->set(CMS_L_IFC_PROFILE_050, "text 35 255 w", $profile_data->bankcode);

    //account holder
    $ifc->set(CMS_L_IFC_PROFILE_051, "text 35 255 w", $profile_data->account_holder);
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    //custom data
    ifc_tab_next(CMS_L_IFC_PROFILE_037);

    ifc_table_open();

    //custom data 1 to 10 (left)
    echo("<td class=\"varied\">");
    for ($i = 1; $i < 11; $i++)
    {
        if (! $title = l($data->get(CMS_DB_PROFILE_CUSTOM_FIELD . $i, "title")))
            $title = CMS_L_IFC_PROFILE_038 . " $i";
        $value = $profile_data->{"field$i"};

        $ifc->set($title, "textarea 35x3 255 w", $value);
    }
    echo("</td>" .

    //custom data 11 to 20 (right)
         "<td class=\"varied\">");
    for ($i = 11; $i < 21; $i++)
    {
        if (! $title = l($data->get(CMS_DB_PROFILE_CUSTOM_FIELD . $i, "title")))
            $title = CMS_L_IFC_PROFILE_038 . " $i";
        $value = $profile_data->{"field$i"};

        $ifc->set($title, "textarea 35x3 255 w", $value);
    };
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    ifc_tab_close();

    $ifc->close();

case "config_base":
//..................................................................................................

    if (! $_profile->operator) break; //must be operator

    $data = new data("#system/profile");

    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["sql_filter_field"  => $sql_filter_field,
                    "sql_filter_option" => $sql_filter_option,
                    "sql_filter_value"  => $sql_filter_value,
                    "sql_order"         => $sql_order,
                    "group"             => $group,
                    "page"              => $page,
                    "limit"             => $limit],
                   "_config_base",
                   CMS_L_IFC_PROFILE_034);

    ifc_table_open("min");

    //administration data
    echo("<tr>" .
         "<th colspan=\"4\">" .
         CMS_L_IFC_PROFILE_027 .
         "</th>" .
         "</tr>" .

    //code
         "<tr>" .
         "<td class=\"varied\">" .
         CMS_L_IFC_PROFILE_002 .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CODE, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CODE, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CODE, "required"));
    echo("</td>" .
         "</tr>" .

    //user
         "<tr>" .
         "<td>" .
         CMS_L_IFC_PROFILE_003 .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_USER, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_USER, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_USER, "required"));
    echo("</td>" .
         "</tr>" .

    //password
         "<tr>" .
         "<td class=\"varied\">" .
         CMS_L_PASSWORD .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PASSWORD, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PASSWORD, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PASSWORD, "required"));
    echo("</td>" .
         "</tr>" .

    //superuser
         "<tr>" .
         "<td>" .
         CMS_L_IFC_PROFILE_004 .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_SUPERUSER, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_SUPERUSER, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_SUPERUSER, "required"));
    echo("</td>" .
         "</tr>" .

    //personal data
         "<tr>" .
         "<th colspan=\"4\">" .
         CMS_L_IFC_PROFILE_028 .
         "</th>" .
         "</tr>" .

    //company
         "<tr>" .
         "<td class=\"varied\">" .
         CMS_L_IFC_PROFILE_005 .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_COMPANY, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_COMPANY, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_COMPANY, "required"));
    echo("</td>" .
         "</tr>" .

    //prename
         "<tr>" .
         "<td>" .
         CMS_L_IFC_PROFILE_006 .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PRENAME, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PRENAME, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PRENAME, "required"));
    echo("</td>" .
         "</tr>" .

    //surname
         "<tr>" .
         "<td class=\"varied\">" .
         CMS_L_IFC_PROFILE_007 .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_SURNAME, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_SURNAME, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_SURNAME, "required"));
    echo("</td>" .
         "</tr>" .

    //street
         "<tr>" .
         "<td>" .
         CMS_L_IFC_PROFILE_008 .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_STREET, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_STREET, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_STREET, "required"));
    echo("</td>" .
         "</tr>" .

    //zipcode
         "<tr>" .
         "<td class=\"varied\">" .
         CMS_L_IFC_PROFILE_009 .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_ZIPCODE, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_ZIPCODE, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_ZIPCODE, "required"));
    echo("</td>" .
         "</tr>" .

    //city
         "<tr>" .
         "<td>" .
         CMS_L_IFC_PROFILE_010 .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CITY, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CITY, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CITY, "required"));
    echo("</td>" .
         "</tr>" .

    //country
         "<tr>" .
         "<td class=\"varied\">" .
         CMS_L_IFC_PROFILE_011 .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_COUNTRY, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_COUNTRY, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_COUNTRY, "required"));
    echo("</td>" .
         "</tr>" .

    //contact data
         "<tr>" .
         "<th colspan=\"4\">" .
         CMS_L_IFC_PROFILE_029 .
         "</th>" .
         "</tr>" .

    //phone 1
         "<tr>" .
         "<td>" .
         CMS_L_IFC_PROFILE_012 .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PHONE_1, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PHONE_1, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PHONE_1, "required"));
    echo("</td>" .
         "</tr>" .

    //phone 2
         "<tr>" .
         "<td class=\"varied\">" .
         CMS_L_IFC_PROFILE_013 .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PHONE_2, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PHONE_2, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PHONE_2, "required"));
    echo("</td>" .
         "</tr>" .

    //mobile
         "<tr>" .
         "<td>" .
         CMS_L_IFC_PROFILE_014 .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_MOBILE, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_MOBILE, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_MOBILE, "required"));
    echo("</td>" .
         "</tr>" .

    //fax
         "<tr>" .
         "<td class=\"varied\">" .
         CMS_L_IFC_PROFILE_015 .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_FAX, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_FAX, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_FAX, "required"));
    echo("</td>" .
         "</tr>" .

    //email
         "<tr>" .
         "<td>" .
         CMS_L_EMAIL .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_EMAIL, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_EMAIL, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_EMAIL, "required"));
    echo("</td>" .
         "</tr>" .

    //url
         "<tr>" .
         "<td class=\"varied\">" .
         CMS_L_URL .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_URL, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_URL, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_URL, "required"));
    echo("</td>" .
         "</tr>" .

    //bank account data
         "<tr>" .
         "<th colspan=\"4\">" .
         CMS_L_IFC_PROFILE_043 .
         "</th>" .
         "</tr>" .

    //account number
         "<tr>" .
         "<td>" .
         CMS_L_IFC_PROFILE_048 .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_ACCOUNT_NUMBER, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_ACCOUNT_NUMBER, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_ACCOUNT_NUMBER, "required"));
    echo("</td>" .
         "</tr>" .

    //financial institution
         "<tr>" .
         "<td class=\"varied\">" .
         CMS_L_IFC_PROFILE_049 .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_FINANCIAL_INSTITUTION, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_FINANCIAL_INSTITUTION, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_FINANCIAL_INSTITUTION, "required"));
    echo("</td>" .
         "</tr>" .

    //bankcode
         "<tr>" .
         "<td>" .
         CMS_L_IFC_PROFILE_050 .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_BANKCODE, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_BANKCODE, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_BANKCODE, "required"));
    echo("</td>" .
         "</tr>" .

    //account holder
         "<tr>" .
         "<td class=\"varied\">" .
         CMS_L_IFC_PROFILE_051 .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_ACCOUNT_HOLDER, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_ACCOUNT_HOLDER, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_ACCOUNT_HOLDER, "required"));
    echo("</td>" .
         "</tr>" .

    //credit card data
         "<tr>" .
         "<th colspan=\"4\">" .
         CMS_L_IFC_PROFILE_042 .
         "</th>" .
         "</tr>" .

    //credit card number
         "<tr>" .
         "<td>" .
         CMS_L_IFC_PROFILE_044 .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_CARD_NUMBER, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_CARD_NUMBER, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_CARD_NUMBER, "required"));
    echo("</td>" .
         "</tr>" .

    //credit institute
         "<tr>" .
         "<td class=\"varied\">" .
         CMS_L_IFC_PROFILE_045 .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_INSTITUTE, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_INSTITUTE, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_INSTITUTE, "required"));
    echo("</td>" .
         "</tr>" .

    //credit card holder
         "<tr>" .
         "<td>" .
         CMS_L_IFC_PROFILE_046 .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_CARD_HOLDER, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_CARD_HOLDER, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_CARD_HOLDER, "required"));
    echo("</td>" .
         "</tr>" .

    //credit card validity
         "<tr>" .
         "<td class=\"varied\">" .
         CMS_L_IFC_PROFILE_047 .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_CARD_VALIDITY, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_CARD_VALIDITY, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_CARD_VALIDITY, "required"));
    echo("</td>" .
         "</tr>" .

    //extended data
         "<tr>" .
         "<th colspan=\"4\">" .
         CMS_L_IFC_PROFILE_030 .
         "</th>" .
         "</tr>" .

    //comment
         "<tr>" .
         "<td>" .
         CMS_L_IFC_PROFILE_016 .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_COMMENT, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_COMMENT, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get(CMS_DB_PROFILE_COMMENT, "required"));
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    $ifc->close();

case "_config_base":
//..................................................................................................

    if (! $_profile->operator) break; //must be operator

    $data = new data("#system/profile");

    $data->set(isset($ifc_param1),  CMS_DB_PROFILE_CODE,                  "visible");
    $data->set(isset($ifc_param2),  CMS_DB_PROFILE_CODE,                  "editable");
    $data->set(isset($ifc_param3),  CMS_DB_PROFILE_CODE,                  "required");
    $data->set(isset($ifc_param4),  CMS_DB_PROFILE_USER,                  "visible");
    $data->set(isset($ifc_param5),  CMS_DB_PROFILE_USER,                  "editable");
    $data->set(isset($ifc_param6),  CMS_DB_PROFILE_USER,                  "required");
    $data->set(isset($ifc_param7),  CMS_DB_PROFILE_PASSWORD,              "visible");
    $data->set(isset($ifc_param8),  CMS_DB_PROFILE_PASSWORD,              "editable");
    $data->set(isset($ifc_param9),  CMS_DB_PROFILE_PASSWORD,              "required");
    $data->set(isset($ifc_param10), CMS_DB_PROFILE_SUPERUSER,             "visible");
    $data->set(isset($ifc_param11), CMS_DB_PROFILE_SUPERUSER,             "editable");
    $data->set(isset($ifc_param12), CMS_DB_PROFILE_SUPERUSER,             "required");
    $data->set(isset($ifc_param13), CMS_DB_PROFILE_COMPANY,               "visible");
    $data->set(isset($ifc_param14), CMS_DB_PROFILE_COMPANY,               "editable");
    $data->set(isset($ifc_param15), CMS_DB_PROFILE_COMPANY,               "required");
    $data->set(isset($ifc_param16), CMS_DB_PROFILE_PRENAME,               "visible");
    $data->set(isset($ifc_param17), CMS_DB_PROFILE_PRENAME,               "editable");
    $data->set(isset($ifc_param18), CMS_DB_PROFILE_PRENAME,               "required");
    $data->set(isset($ifc_param19), CMS_DB_PROFILE_SURNAME,               "visible");
    $data->set(isset($ifc_param20), CMS_DB_PROFILE_SURNAME,               "editable");
    $data->set(isset($ifc_param21), CMS_DB_PROFILE_SURNAME,               "required");
    $data->set(isset($ifc_param22), CMS_DB_PROFILE_STREET,                "visible");
    $data->set(isset($ifc_param23), CMS_DB_PROFILE_STREET,                "editable");
    $data->set(isset($ifc_param24), CMS_DB_PROFILE_STREET,                "required");
    $data->set(isset($ifc_param25), CMS_DB_PROFILE_ZIPCODE,               "visible");
    $data->set(isset($ifc_param26), CMS_DB_PROFILE_ZIPCODE,               "editable");
    $data->set(isset($ifc_param27), CMS_DB_PROFILE_ZIPCODE,               "required");
    $data->set(isset($ifc_param28), CMS_DB_PROFILE_CITY,                  "visible");
    $data->set(isset($ifc_param29), CMS_DB_PROFILE_CITY,                  "editable");
    $data->set(isset($ifc_param30), CMS_DB_PROFILE_CITY,                  "required");
    $data->set(isset($ifc_param31), CMS_DB_PROFILE_COUNTRY,               "visible");
    $data->set(isset($ifc_param32), CMS_DB_PROFILE_COUNTRY,               "editable");
    $data->set(isset($ifc_param33), CMS_DB_PROFILE_COUNTRY,               "required");
    $data->set(isset($ifc_param34), CMS_DB_PROFILE_PHONE_1,               "visible");
    $data->set(isset($ifc_param35), CMS_DB_PROFILE_PHONE_1,               "editable");
    $data->set(isset($ifc_param36), CMS_DB_PROFILE_PHONE_1,               "required");
    $data->set(isset($ifc_param37), CMS_DB_PROFILE_PHONE_2,               "visible");
    $data->set(isset($ifc_param38), CMS_DB_PROFILE_PHONE_2,               "editable");
    $data->set(isset($ifc_param39), CMS_DB_PROFILE_PHONE_2,               "required");
    $data->set(isset($ifc_param40), CMS_DB_PROFILE_MOBILE,                "visible");
    $data->set(isset($ifc_param41), CMS_DB_PROFILE_MOBILE,                "editable");
    $data->set(isset($ifc_param42), CMS_DB_PROFILE_MOBILE,                "required");
    $data->set(isset($ifc_param43), CMS_DB_PROFILE_FAX,                   "visible");
    $data->set(isset($ifc_param44), CMS_DB_PROFILE_FAX,                   "editable");
    $data->set(isset($ifc_param45), CMS_DB_PROFILE_FAX,                   "required");
    $data->set(isset($ifc_param46), CMS_DB_PROFILE_EMAIL,                 "visible");
    $data->set(isset($ifc_param47), CMS_DB_PROFILE_EMAIL,                 "editable");
    $data->set(isset($ifc_param48), CMS_DB_PROFILE_EMAIL,                 "required");
    $data->set(isset($ifc_param49), CMS_DB_PROFILE_URL,                   "visible");
    $data->set(isset($ifc_param50), CMS_DB_PROFILE_URL,                   "editable");
    $data->set(isset($ifc_param51), CMS_DB_PROFILE_URL,                   "required");
    $data->set(isset($ifc_param52), CMS_DB_PROFILE_ACCOUNT_NUMBER,        "visible");
    $data->set(isset($ifc_param53), CMS_DB_PROFILE_ACCOUNT_NUMBER,        "editable");
    $data->set(isset($ifc_param54), CMS_DB_PROFILE_ACCOUNT_NUMBER,        "required");
    $data->set(isset($ifc_param55), CMS_DB_PROFILE_FINANCIAL_INSTITUTION, "visible");
    $data->set(isset($ifc_param56), CMS_DB_PROFILE_FINANCIAL_INSTITUTION, "editable");
    $data->set(isset($ifc_param57), CMS_DB_PROFILE_FINANCIAL_INSTITUTION, "required");
    $data->set(isset($ifc_param58), CMS_DB_PROFILE_BANKCODE,              "visible");
    $data->set(isset($ifc_param59), CMS_DB_PROFILE_BANKCODE,              "editable");
    $data->set(isset($ifc_param60), CMS_DB_PROFILE_BANKCODE,              "required");
    $data->set(isset($ifc_param61), CMS_DB_PROFILE_ACCOUNT_HOLDER,        "visible");
    $data->set(isset($ifc_param62), CMS_DB_PROFILE_ACCOUNT_HOLDER,        "editable");
    $data->set(isset($ifc_param63), CMS_DB_PROFILE_ACCOUNT_HOLDER,        "required");
    $data->set(isset($ifc_param64), CMS_DB_PROFILE_CREDIT_CARD_NUMBER,    "visible");
    $data->set(isset($ifc_param65), CMS_DB_PROFILE_CREDIT_CARD_NUMBER,    "editable");
    $data->set(isset($ifc_param66), CMS_DB_PROFILE_CREDIT_CARD_NUMBER,    "required");
    $data->set(isset($ifc_param67), CMS_DB_PROFILE_CREDIT_INSTITUTE,      "visible");
    $data->set(isset($ifc_param68), CMS_DB_PROFILE_CREDIT_INSTITUTE,      "editable");
    $data->set(isset($ifc_param69), CMS_DB_PROFILE_CREDIT_INSTITUTE,      "required");
    $data->set(isset($ifc_param70), CMS_DB_PROFILE_CREDIT_CARD_HOLDER,    "visible");
    $data->set(isset($ifc_param71), CMS_DB_PROFILE_CREDIT_CARD_HOLDER,    "editable");
    $data->set(isset($ifc_param72), CMS_DB_PROFILE_CREDIT_CARD_HOLDER,    "required");
    $data->set(isset($ifc_param73), CMS_DB_PROFILE_CREDIT_CARD_VALIDITY,  "visible");
    $data->set(isset($ifc_param74), CMS_DB_PROFILE_CREDIT_CARD_VALIDITY,  "editable");
    $data->set(isset($ifc_param75), CMS_DB_PROFILE_CREDIT_CARD_VALIDITY,  "required");
    $data->set(isset($ifc_param76), CMS_DB_PROFILE_COMMENT,               "visible");
    $data->set(isset($ifc_param77), CMS_DB_PROFILE_COMMENT,               "editable");
    $data->set(isset($ifc_param78), CMS_DB_PROFILE_COMMENT,               "required");

    $ifc_response = $data->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "config_custom":
//..................................................................................................

    if (! $_profile->operator) break; //must be operator

    $data = new data("#system/profile");

    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["sql_filter_field"  => $sql_filter_field,
                    "sql_filter_option" => $sql_filter_option,
                    "sql_filter_value"  => $sql_filter_value,
                    "sql_order"         => $sql_order,
                    "group"             => $group,
                    "page"              => $page,
                    "limit"             => $limit],
                   "_config_custom",
                   CMS_L_IFC_PROFILE_036);

    ifc_table_open("min");
    echo("<tr>" .
         "<th colspan=\"4\">" .
         CMS_L_IFC_PROFILE_037 .
         "</th>" .
         "</tr>");
    for ($i = 1; $i < 21; $i++)
    {
        $name   = CMS_DB_PROFILE_CUSTOM_FIELD . $i;
        $varied = ifc_varied();

        echo("<tr>" .
             "<td$varied>");

        //name
        $ifc->set(NULL,                  "text 40 40 l:",  $data->get($name, "title"));
        echo("</td>" .
             "<td$varied>");

        //visible
        $ifc->set(CMS_L_IFC_PROFILE_032, "checkbox", TRUE, $data->get($name, "visible"));
        echo("</td>" .
             "<td$varied>");

        //editable
        $ifc->set(CMS_L_IFC_PROFILE_033, "checkbox", TRUE, $data->get($name, "editable"));
        echo("</td>" .
             "<td$varied>");

        //required
        $ifc->set(CMS_L_IFC_PROFILE_035, "checkbox", TRUE, $data->get($name, "required"));
        echo("</td>" .
             "</tr>");
    };
    ifc_table_close();

    $ifc->close();

case "_config_custom":
//..................................................................................................

    if (! $_profile->operator) break; //must be operator

    $data = new data("#system/profile");

    for ($i = 1, $_i = 0; $i < 21; $i++)
    {
        $name     = CMS_DB_PROFILE_CUSTOM_FIELD . $i;
        $title    = ${"ifc_param" . ++$_i};
        $visible  = isset(${"ifc_param" . ++$_i});
        $editable = isset(${"ifc_param" . ++$_i});
        $required = isset(${"ifc_param" . ++$_i});

        $data->set($title,    $name, "title");
        $data->set($visible,  $name, "visible");
        $data->set($editable, $name, "editable");
        $data->set($required, $name, "required");
    };

    $ifc_response = $data->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "sql_order":
//..................................................................................................

    $sql_order = streq($sql_order, "$ifc_param DESC") ? $ifc_param : "$ifc_param DESC";
    break;

case "page":
//..................................................................................................

    $page = $ifc_param;
    break;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

//menu
$menu = NULL;
if ($_profile->operator)
{
    $menu[CMS_L_COMMAND_ADD .             "|profile/command_add"]              = "add";
    $menu[CMS_L_COMMAND_DELETE_SELECTED . "|profile/command_delete"]           = "#delete";
    $menu[CMS_L_IFC_PROFILE_034 .         "|profile/command_configuration"]    = "config_base";
    $menu[CMS_L_IFC_PROFILE_036 .         "|profile/command_configuration_ex"] = "config_custom";
};

//display
$ifc = new ifc($ifc_response,
               $ifc_page,
               $menu,
               ["sql_filter_field"  => $sql_filter_field,
                "sql_filter_option" => $sql_filter_option,
                "sql_filter_value"  => $sql_filter_value,
                "sql_order"         => $sql_order,
                "group"             => $group,
                "page"              => $page,
                "limit"             => $limit]);

//search filter
echo("<div class=\"element\">" .
     "<span>" . CMS_L_IFC_PROFILE_039 . "</span><br>");

//field
$ifc->set([CMS_L_IFC_PROFILE_001 => CMS_DB_PROFILE_INDEX,
           CMS_L_IFC_PROFILE_002 => CMS_DB_PROFILE_CODE,
           CMS_L_IFC_PROFILE_003 => CMS_DB_PROFILE_USER,
           CMS_L_IFC_PROFILE_004 => CMS_DB_PROFILE_SUPERUSER,
           CMS_L_IFC_PROFILE_005 => CMS_DB_PROFILE_COMPANY,
           CMS_L_IFC_PROFILE_006 => CMS_DB_PROFILE_PRENAME,
           CMS_L_IFC_PROFILE_007 => CMS_DB_PROFILE_SURNAME,
           CMS_L_IFC_PROFILE_008 => CMS_DB_PROFILE_STREET,
           CMS_L_IFC_PROFILE_009 => CMS_DB_PROFILE_ZIPCODE,
           CMS_L_IFC_PROFILE_010 => CMS_DB_PROFILE_CITY,
           CMS_L_IFC_PROFILE_011 => CMS_DB_PROFILE_COUNTRY,
           CMS_L_IFC_PROFILE_012 => CMS_DB_PROFILE_PHONE_1,
           CMS_L_IFC_PROFILE_013 => CMS_DB_PROFILE_PHONE_2,
           CMS_L_IFC_PROFILE_014 => CMS_DB_PROFILE_MOBILE,
           CMS_L_IFC_PROFILE_015 => CMS_DB_PROFILE_FAX,
           CMS_L_EMAIL           => CMS_DB_PROFILE_EMAIL,
           CMS_L_URL             => CMS_DB_PROFILE_URL,
           CMS_L_IFC_PROFILE_048 => CMS_DB_PROFILE_ACCOUNT_NUMBER,
           CMS_L_IFC_PROFILE_049 => CMS_DB_PROFILE_FINANCIAL_INSTITUTION,
           CMS_L_IFC_PROFILE_050 => CMS_DB_PROFILE_BANKCODE,
           CMS_L_IFC_PROFILE_051 => CMS_DB_PROFILE_ACCOUNT_HOLDER,
           CMS_L_IFC_PROFILE_044 => CMS_DB_PROFILE_CREDIT_CARD_NUMBER,
           CMS_L_IFC_PROFILE_045 => CMS_DB_PROFILE_CREDIT_INSTITUTE,
           CMS_L_IFC_PROFILE_046 => CMS_DB_PROFILE_CREDIT_CARD_HOLDER,
           CMS_L_IFC_PROFILE_047 => CMS_DB_PROFILE_CREDIT_CARD_VALIDITY,
           CMS_L_IFC_PROFILE_016 => CMS_DB_PROFILE_COMMENT],
          "select",
          $sql_filter_field,
          NULL,
          "_sql_filter_field");

//condition
$ifc->set([CMS_L_IFC_PROFILE_017 => " LIKE '#value#%'",
           CMS_L_IFC_PROFILE_018 => " LIKE '%#value#%'",
           CMS_L_IFC_PROFILE_019 => " LIKE '%#value#'",
           CMS_L_IFC_PROFILE_020 => ">'#value#'",
           CMS_L_IFC_PROFILE_021 => "='#value#'",
           CMS_L_IFC_PROFILE_022 => "<'#value#'"],
          "select",
          $sql_filter_option,
          NULL,
          "_sql_filter_option");

//value
$ifc->set(NULL, "text 20 40 :", $sql_filter_value, NULL, "sql_filter_value");
echo("</div>");

//group by
$ifc->set(CMS_L_IFC_PROFILE_023, "label");
$ifc->set([" "                   => NULL,
           CMS_L_IFC_PROFILE_004 => CMS_DB_PROFILE_SUPERUSER,
           CMS_L_IFC_PROFILE_005 => CMS_DB_PROFILE_COMPANY,
           CMS_L_IFC_PROFILE_007 => CMS_DB_PROFILE_SURNAME,
           CMS_L_IFC_PROFILE_008 => CMS_DB_PROFILE_STREET,
           CMS_L_IFC_PROFILE_009 => CMS_DB_PROFILE_ZIPCODE,
           CMS_L_IFC_PROFILE_010 => CMS_DB_PROFILE_CITY,
           CMS_L_IFC_PROFILE_011 => CMS_DB_PROFILE_COUNTRY,
           CMS_L_IFC_PROFILE_049 => CMS_DB_PROFILE_FINANCIAL_INSTITUTION,
           CMS_L_IFC_PROFILE_050 => CMS_DB_PROFILE_BANKCODE,
           CMS_L_IFC_PROFILE_045 => CMS_DB_PROFILE_CREDIT_INSTITUTE],
          "select",
          $group,
          NULL,
          "_group");

//row display limit per page
$ifc->set(CMS_L_IFC_PROFILE_024, "label");
$ifc->set([ 10 => "10",
            25 => "25",
            50 => "50",
           100 => "100",
           250 => "250",
           500 => "500"],
          "select",
          $limit,
          NULL,
          "_limit");

//apply button
$ifc->set(CMS_L_COMMAND_CONFIRM, "button");

//reset button
$ifc->set(CMS_L_IFC_PROFILE_025, "button b", "javascript:ifc_reset(1);ifc_submit();");

//retrieve list
$array = [CMS_DB_PROFILE_INDEX,
          CMS_DB_PROFILE_CODE,
          CMS_DB_PROFILE_PRENAME,
          CMS_DB_PROFILE_SURNAME,
          CMS_DB_PROFILE_PHONE_1,
          CMS_DB_PROFILE_FAX,
          CMS_DB_PROFILE_EMAIL];

if (CMS_IFC_SELECT && (! in_array(CMS_IFC_SELECT, $array))) $array[] = sqlesc(CMS_IFC_SELECT);
if ($group && (! in_array($group, $array)))                 $array[] = sqlesc($group);

$query  =  "SELECT " .   implode(", ", $array) . " " .
           "FROM " .     CMS_DB_PROFILE;
if ($sql_filter_value)                        $query .=
          " WHERE " .    sqlesc($sql_filter_field) .
                         str_replace("#value#", sqlesc($sql_filter_value), $sql_filter_option);
$query .= " GROUP BY " . CMS_DB_PROFILE_INDEX .
          " ORDER BY ";
preg_match("/^(?:[^ ]+)((?: DESC)?)$/", $sql_order, $_sql_order);
if ($group && nstreq($group, $_sql_order[0])) $query .=
                         sqlesc($group) . $_sql_order[1] . ", ";
$query .=                sqlesc($sql_order);

//empty result
if (
     (! $result = mysql_query($query))
     ||
     (! mysql_num_rows($result))
   )
{
    $ifc->set(CMS_L_IFC_PROFILE_026, "info");
    $ifc->close();
};

?><script>

function r(value)
{
    ifc_return(value);
};

function d(index)
{
    load_page("<?php
        echo(str_replace("%replace%",
                         "\"+encodeURIComponent(index)+\"",
                         q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "display",
                                    "ifc_option"  => "external",
                                    "object"      => "\x1B%replace%"]))));?>");

    var object = null;
    var i      = 0;

    while (object = ifc_object("profile[" + i++ + "]"))
    {
        if ((object.value == index) && (! object.checked))
        {
            object.click();
            return;
        };
    };
};

function p(number)
{
    ifc_post("page", number);
};

</script><?php

//pagination
$count = mysql_num_rows($result);

if ($count > $limit)
{
    $count = ceil($count / $limit);
    $page  = min((int)$page, $count - 1);

    pagination(
        "javascript:p(%page%);",
        $page,
        $count,
        CMS_L_COMMAND_NEXT,
        "pagination");

    mysql_data_seek($result, $page * $limit);
};

ifc_table_open();
echo("<colgroup>" .
     "<col style=\"WIDTH:0\">" .
     (CMS_IFC_SELECT ?
     "<col>" .
     "<col style=\"WIDTH:0\">" .
     "<col span=\"3\">" :
     "<col span=\"4\">") .
     "</colgroup>" .

     "<tr>" .

//selection
     "<td class=\"select\">" .
     "</td>" .

//index
     "<th>");
$ifc->set(CMS_L_IFC_PROFILE_001,
          "button",
          "javascript:ifc_post('sql_order','" . q(CMS_DB_PROFILE_INDEX) . "');");
echo("</th>" .

//code
     "<th>");
$ifc->set(CMS_L_IFC_PROFILE_002,
          "button",
          "javascript:ifc_post('sql_order','" . q(CMS_DB_PROFILE_CODE) . "');");
echo("</th>");

//select
if (CMS_IFC_SELECT)
    echo("<th>" .
         "</th>");

//name
echo("<th>");
$ifc->set(CMS_L_NAME,
          "button",
          "javascript:ifc_post('sql_order','" . q(CMS_DB_PROFILE_SURNAME) . "');");
echo("</th>" .

//phone 1
     "<th>" .
     CMS_L_IFC_PROFILE_012 .
     "</th>" .

//email
     "<th>" .
     CMS_L_EMAIL .
     "</th>" .
     "</tr>");

$_group = NULL;
$i      = 0;

while (
        ($i++ < $limit)
        &&
        ($resultrow = mysql_fetch_assoc($result))
      )
{
    //group
    if ($group)
    {
        $value = $resultrow[$group] ? $resultrow[$group] : CMS_L_IFC_PROFILE_031;

        if (nstreq($_group, $value))
        {
            $_group = $value;

            echo("<tr>" .

            //spacer
                 "<td colspan=\"" . (CMS_IFC_SELECT ? "4" : "3") . "\" " .
                     "class=\"highlight\">" .
                 "</td>" .

            //group
                 "<td colspan=\"3\" " .
                     "class=\"highlight\">" .
                 x($value) .
                 "</td>" .
                 "</tr>");
        };
    };

    $index  = $resultrow[CMS_DB_PROFILE_INDEX];
    $varied = ifc_varied();

    echo("<tr>" .

    //selection
         "<td class=\"select\">");
    $ifc->set(NULL, "checkbox", $index, FALSE, "list[]");
    echo("</td>" .

    //index
         "<td$varied>" .
         x($index) .
         "</td>" .

    //code
         "<td$varied>" .
         x($resultrow[CMS_DB_PROFILE_CODE]) .
         "</td>");

    //name
    if (
         $resultrow[CMS_DB_PROFILE_PRENAME]
         &&
         $resultrow[CMS_DB_PROFILE_SURNAME]
       )
    {
        $value = $resultrow[CMS_DB_PROFILE_SURNAME] . ", " .
                 $resultrow[CMS_DB_PROFILE_PRENAME];
    }
    else
    {
        $value = $resultrow[CMS_DB_PROFILE_PRENAME] .
                 $resultrow[CMS_DB_PROFILE_SURNAME];

        if (! $value) $value = CMS_L_IFC_PROFILE_031;
    };

    //select
    if (CMS_IFC_SELECT)
        echo("<td$varied>" .
             "<a href=\"javascript:r('" . qx($resultrow[CMS_IFC_SELECT]) . "');\" " .
                "title=\"" . CMS_L_COMMAND_INSERT . "\">" .
             image("profile/button_return") .
             "</a>" .
             "</td>");

    //name
    echo("<td$varied>" .
         "<a href=\"javascript:d('" . qx($index) . "');\" " .
            "title=\"" . CMS_L_COMMAND_SHOW . "\">" .
         image("profile/icon_entry") . " " . x($value) .
         "</a>" .
         "</td>");

    //phone 1
    echo("<td$varied>" .
         x($resultrow[CMS_DB_PROFILE_PHONE_1]) .
         "</td>" .

    //email
         "<td$varied>");
    if ($resultrow[CMS_DB_PROFILE_EMAIL])
    {
        $value = $resultrow[CMS_DB_PROFILE_EMAIL];

        echo("<a href=\"mailto:" . r($value) . "\">" .
             x($value) .
             "</a>");
    };
    echo("</td>" .
         "</tr>");
};

//selection control
echo("<tr>" .
     "<td colspan=\"" . (CMS_IFC_SELECT ? "7" : "6") . "\" " .
         "class=\"select\" " .
         "style=\"TEXT-ALIGN:right\">");
$ifc->set(CMS_L_ALL,    "button", "javascript:ifc_list_activate();");
$ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert();");
$ifc->set(CMS_L_NONE,   "button", "javascript:ifc_list_deactivate();");
echo("</td>" .
     "</tr>");
ifc_table_close();

//display newly created profile
if (
     streq(CMS_IFC_MESSAGE, "add")
     &&
     streq($ifc_response, CMS_MSG_DONE)
   )
    echo(jscript("d(\"" . rq($object) . "\");"));

$ifc->close();