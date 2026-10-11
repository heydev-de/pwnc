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
ifc_permission([""                              => t("access"),
                CMS_PROFILE_PERMISSION_OPERATOR => t("operator")]);

//instantiate profile object
$_profile = new profile();
if (! $_profile->enabled) ifc_inactive($ifc_page);

//initiate
init($object);

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
        $ifc_response = t("msg_done");
        break;
    };

    $ifc_response = t("msg_error");
    break;

case "delete":
//..................................................................................................

    if (blank($list)) break; //no selection

    $error = NULL;

    foreach ($list AS $value)
    {
        if (! $_profile->del($value)) $error = TRUE;
    };

    $ifc_response = $error ? t("msg_error") : t("msg_done");
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
        $ifc_response = $_profile->set($profile_data) ? t("msg_done") : t("msg_error");
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
            $array[$data->get($key, "name")] = permission_remove_prefix($key);
    };

    //order by name
    ksort($array, SORT_NATURAL | SORT_FLAG_CASE);

    $data = new data("#system/profile");

    //menu
    $menu = $_profile->operator ? [t("command_save") . "|profile/command_save" => "display_save"] : NULL;

    //display
    $ifc = new ifc(
          $ifc_response,
          $ifc_page,
          $menu,
          NULL,
          NULL,
          $profile_data->name);

    ifc_tab_open(t("basic_data"));

    //base data
    ifc_table_open();
    echo("<colgroup>" .
         "<col span=\"2\" style=\"WIDTH:50%\">" .
         "</colgroup>" .

         "<tr>" .

    //administration data
         "<th>" .
         t("user_credentials") .
         "</th>" .

    //personal data
         "<th>" .
         t("personal_data") .
         "</th>" .
         "</tr>" .

         "<tr>" .

    //administration data
         "<td class=\"varied\">");

    //code
    $ifc->set(t("identifier"), "text 35 40 w", $profile_data->code);

    echo("<table class=\"layout\">" .

         "<colgroup>" .
         "<col>" .
         "<col style=\"WIDTH:0\">" .
         "</colgroup>" .

         "<tr>" .

    //user
         "<td>");
    $ifc->set(t("username"), "text 30 40 w", $profile_data->user);
    echo("</td>" .

    //enabled flag
         "<td style=\"VERTICAL-ALIGN:middle\">");
    $ifc->set(t("active"), "checkbox", TRUE, $profile_data->enabled);
    echo("</td>" .
         "</tr>" .

         "<tr>" .

    //password
         "<td>");
    $ifc->set(t("password"), "text 30 40 w");
    echo("</td>" .

    //generate password button
         "<td style=\"VERTICAL-ALIGN:middle\">");
    $ifc->set(t("generate"), "button", "javascript:ifc_set('ifc_param4',unique_id(8));");
    echo("</td>" .
         "</tr>" .
         "</table>");

    //superuser
    $ifc->set(t("account_inheritance"), "label");
    $ifc->set($array,                   "select 35 w", $profile_data->superuser);

    //created / modified
    $ifc->set(t("created") . ": " . x(friendly_date($profile_data->time_created)) . ", " .
              t("updated") . ": " . x(friendly_date($profile_data->time_updated)),
              "info");
    echo("</td>" .

    //personal data
         "<td class=\"varied\">");

    //company
    $ifc->set(t("organization"), "text 35 255 w", $profile_data->company);

    //prename
    $ifc->set(t("forename"), "text 35 255 w", $profile_data->prename);

    //surname
    $ifc->set(t("surname"), "text 35 255 w", $profile_data->surname);

    //street
    $ifc->set(t("street"), "text 35 255 w", $profile_data->street);

    echo("<table class=\"layout\">" .
         "<tr>" .

    //zipcode
         "<td style=\"WIDTH:0\">");
    $ifc->set(t("zip"), "text 5 255 w", $profile_data->zipcode);
    echo("</td>" .

    //city
         "<td>");
    $ifc->set(t("city"), "text 25 255 w", $profile_data->city);
    echo("</td>" .
         "</tr>" .
         "</table>");

    //country
    $ifc->set(t("country"), "text 35 255 w", $profile_data->country);
    echo("</td>" .
         "</tr>" .
         "<tr>" .

    //extended data
         "<th>" .
         t("miscellaneous") .
         "</th>" .

    //contact data
         "<th>" .
         t("contact_information") .
         "</th>" .
         "</tr>" .
         "<tr>" .

    //extended data
         "<td class=\"varied\">");

    //comment
    $ifc->set(t("remark"), "textarea 35x15 65536 w", $profile_data->comment);
    echo("</td>" .

    //contact data
         "<td class=\"varied\">");

    //phone
    $ifc->set(t("phone"),   "text 35 255 w", $profile_data->phone1);
    $ifc->set(t("phone_2"), "text 35 255 w", $profile_data->phone2);

    //mobile phone
    $ifc->set(t("cell_phone"), "text 35 255 w", $profile_data->mobile);

    //fax
    $ifc->set(t("fax"), "text 35 255 w", $profile_data->fax);

    //email
    echo("<label class=\"element fullwidth\">" .
         "<a href=\"mailto:" . r($profile_data->email) . "\">" .
         t("email") .
         "</a><br>");
    $ifc->set(NULL, "text 35 255 w", $profile_data->email);
    echo("</label>" .

    //url
         "<label class=\"element fullwidth\">" .
         "<a href=\"" . x(cms_url($profile_data->url, NULL, TRUE)) . "\" " .
            "onclick=\"load_page(this.href);return false;\">" .
         t("url") .
         "</a><br>");
    $ifc->set(NULL, "text 35 255 w", $profile_data->url);
    echo("</label>" .
         "</td>" .
         "</tr>" .
         "<tr>" .

    //credit card data
         "<th>" .
         t("credit_card_data") .
         "</th>" .

    //bank account data
         "<th>" .
         t("banking_details") .
         "</th>" .
         "</tr>" .
         "<tr>" .

    //credit card data
         "<td class=\"varied\">");

    //credit card number
    $ifc->set(t("credit_card_number"), "text 35 255 w", $profile_data->credit_card_number);

    //credit institute
    $ifc->set(t("credit_institution"), "text 35 255 w", $profile_data->credit_institute);

    //credit card holder
    $ifc->set(t("credit_card_holder"), "text 35 255 w", $profile_data->credit_card_holder);

    //credit card validity
    $value = explode("/", $profile_data->credit_card_validity);

    //month
    echo("<label class=\"element\">" .
         "<span>" .
         t("expiry") .
         "</span><br>");
    $ifc->set(["01" => "01", "02" => "02", "03" => "03", "04" => "04",
               "05" => "05", "06" => "06", "07" => "07", "08" => "08",
               "09" => "09", "10" => "10", "11" => "11", "12" => "12"],
              "select",
              $value[0] ?: date("m"));
    echo("<span>/</span>");

    //year
    $ifc->set(NULL, "text 4 252", $value[1] ?? date("Y"));
    echo("</label>" .
         "</td>" .

    //bank account data
         "<td class=\"varied\">");

    //account number
    $ifc->set(t("account_number"), "text 35 255 w", $profile_data->account_number);

    //financial institution
    $ifc->set(t("bank"), "text 35 255 w", $profile_data->financial_institution);

    //bank code
    $ifc->set(t("bank_code"), "text 35 255 w", $profile_data->bankcode);

    //account holder
    $ifc->set(t("account_holder"), "text 35 255 w", $profile_data->account_holder);
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    //custom data
    ifc_tab_next(t("additional_information"));

    ifc_table_open();

    //custom data 1 to 10 (left)
    echo("<td class=\"varied\">");
    for ($i = 1; $i < 11; $i++)
    {
        if (! $title = l($data->get(CMS_DB_PROFILE_CUSTOM_FIELD . $i, "title")))
            $title = t("custom_field") . " $i";
        $value = $profile_data->{"field$i"};

        $ifc->set($title, "textarea 35x3 255 w", $value);
    }
    echo("</td>" .

    //custom data 11 to 20 (right)
         "<td class=\"varied\">");
    for ($i = 11; $i < 21; $i++)
    {
        if (! $title = l($data->get(CMS_DB_PROFILE_CUSTOM_FIELD . $i, "title")))
            $title = t("custom_field") . " $i";
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

    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        TRUE,
        NULL,
        "_config_base",
        t("basic_settings"));

    ifc_table_open("min");

    //administration data
    echo("<tr>" .
         "<th colspan=\"4\">" .
         t("user_credentials") .
         "</th>" .
         "</tr>" .

    //code
         "<tr>" .
         "<td class=\"varied\">" .
         t("identifier") .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CODE, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CODE, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CODE, "required"));
    echo("</td>" .
         "</tr>" .

    //user
         "<tr>" .
         "<td>" .
         t("username") .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_USER, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_USER, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_USER, "required"));
    echo("</td>" .
         "</tr>" .

    //password
         "<tr>" .
         "<td class=\"varied\">" .
         t("password") .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PASSWORD, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PASSWORD, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PASSWORD, "required"));
    echo("</td>" .
         "</tr>" .

    //superuser
         "<tr>" .
         "<td>" .
         t("account_inheritance") .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_SUPERUSER, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_SUPERUSER, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_SUPERUSER, "required"));
    echo("</td>" .
         "</tr>" .

    //personal data
         "<tr>" .
         "<th colspan=\"4\">" .
         t("personal_data") .
         "</th>" .
         "</tr>" .

    //company
         "<tr>" .
         "<td class=\"varied\">" .
         t("organization") .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_COMPANY, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_COMPANY, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_COMPANY, "required"));
    echo("</td>" .
         "</tr>" .

    //prename
         "<tr>" .
         "<td>" .
         t("forename") .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PRENAME, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PRENAME, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PRENAME, "required"));
    echo("</td>" .
         "</tr>" .

    //surname
         "<tr>" .
         "<td class=\"varied\">" .
         t("surname") .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_SURNAME, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_SURNAME, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_SURNAME, "required"));
    echo("</td>" .
         "</tr>" .

    //street
         "<tr>" .
         "<td>" .
         t("street") .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_STREET, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_STREET, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_STREET, "required"));
    echo("</td>" .
         "</tr>" .

    //zipcode
         "<tr>" .
         "<td class=\"varied\">" .
         t("zip") .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_ZIPCODE, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_ZIPCODE, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_ZIPCODE, "required"));
    echo("</td>" .
         "</tr>" .

    //city
         "<tr>" .
         "<td>" .
         t("city") .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CITY, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CITY, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CITY, "required"));
    echo("</td>" .
         "</tr>" .

    //country
         "<tr>" .
         "<td class=\"varied\">" .
         t("country") .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_COUNTRY, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_COUNTRY, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_COUNTRY, "required"));
    echo("</td>" .
         "</tr>" .

    //contact data
         "<tr>" .
         "<th colspan=\"4\">" .
         t("contact_information") .
         "</th>" .
         "</tr>" .

    //phone 1
         "<tr>" .
         "<td>" .
         t("phone") .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PHONE_1, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PHONE_1, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PHONE_1, "required"));
    echo("</td>" .
         "</tr>" .

    //phone 2
         "<tr>" .
         "<td class=\"varied\">" .
         t("phone_2") .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PHONE_2, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PHONE_2, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_PHONE_2, "required"));
    echo("</td>" .
         "</tr>" .

    //mobile
         "<tr>" .
         "<td>" .
         t("cell_phone") .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_MOBILE, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_MOBILE, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_MOBILE, "required"));
    echo("</td>" .
         "</tr>" .

    //fax
         "<tr>" .
         "<td class=\"varied\">" .
         t("fax") .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_FAX, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_FAX, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_FAX, "required"));
    echo("</td>" .
         "</tr>" .

    //email
         "<tr>" .
         "<td>" .
         t("email") .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_EMAIL, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_EMAIL, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_EMAIL, "required"));
    echo("</td>" .
         "</tr>" .

    //url
         "<tr>" .
         "<td class=\"varied\">" .
         t("url") .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_URL, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_URL, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_URL, "required"));
    echo("</td>" .
         "</tr>" .

    //bank account data
         "<tr>" .
         "<th colspan=\"4\">" .
         t("banking_details") .
         "</th>" .
         "</tr>" .

    //account number
         "<tr>" .
         "<td>" .
         t("account_number") .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_ACCOUNT_NUMBER, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_ACCOUNT_NUMBER, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_ACCOUNT_NUMBER, "required"));
    echo("</td>" .
         "</tr>" .

    //financial institution
         "<tr>" .
         "<td class=\"varied\">" .
         t("bank") .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_FINANCIAL_INSTITUTION, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_FINANCIAL_INSTITUTION, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_FINANCIAL_INSTITUTION, "required"));
    echo("</td>" .
         "</tr>" .

    //bankcode
         "<tr>" .
         "<td>" .
         t("bank_code") .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_BANKCODE, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_BANKCODE, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_BANKCODE, "required"));
    echo("</td>" .
         "</tr>" .

    //account holder
         "<tr>" .
         "<td class=\"varied\">" .
         t("account_holder") .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_ACCOUNT_HOLDER, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_ACCOUNT_HOLDER, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_ACCOUNT_HOLDER, "required"));
    echo("</td>" .
         "</tr>" .

    //credit card data
         "<tr>" .
         "<th colspan=\"4\">" .
         t("credit_card_data") .
         "</th>" .
         "</tr>" .

    //credit card number
         "<tr>" .
         "<td>" .
         t("credit_card_number") .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_CARD_NUMBER, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_CARD_NUMBER, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_CARD_NUMBER, "required"));
    echo("</td>" .
         "</tr>" .

    //credit institute
         "<tr>" .
         "<td class=\"varied\">" .
         t("credit_institution") .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_INSTITUTE, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_INSTITUTE, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_INSTITUTE, "required"));
    echo("</td>" .
         "</tr>" .

    //credit card holder
         "<tr>" .
         "<td>" .
         t("credit_card_holder") .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_CARD_HOLDER, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_CARD_HOLDER, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_CARD_HOLDER, "required"));
    echo("</td>" .
         "</tr>" .

    //credit card validity
         "<tr>" .
         "<td class=\"varied\">" .
         t("expiry") .
         "</td>" .
         "<td class=\"varied\">");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_CARD_VALIDITY, "visible"));
    echo("</td>" .
         "<td class=\"varied\">");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_CARD_VALIDITY, "editable"));
    echo("</td>" .
         "<td class=\"varied\">");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_CREDIT_CARD_VALIDITY, "required"));
    echo("</td>" .
         "</tr>" .

    //extended data
         "<tr>" .
         "<th colspan=\"4\">" .
         t("miscellaneous") .
         "</th>" .
         "</tr>" .

    //comment
         "<tr>" .
         "<td>" .
         t("remark") .
         "</td>" .
         "<td>");

    //visible
    $ifc->set(t("visible"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_COMMENT, "visible"));
    echo("</td>" .
         "<td>");

    //editable
    $ifc->set(t("editable"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_COMMENT, "editable"));
    echo("</td>" .
         "<td>");

    //required
    $ifc->set(t("required"), "checkbox", TRUE, $data->get(CMS_DB_PROFILE_COMMENT, "required"));
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

    $ifc_response = $data->save() ? t("msg_done") : t("msg_error");
    break;

case "config_custom":
//..................................................................................................

    if (! $_profile->operator) break; //must be operator

    $data = new data("#system/profile");

    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        TRUE,
        NULL,
        "_config_custom",
        t("additional_settings"));

    ifc_table_open("min");
    echo("<tr>" .
         "<th colspan=\"4\">" .
         t("additional_information") .
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
        $ifc->set(t("visible"), "checkbox", TRUE, $data->get($name, "visible"));
        echo("</td>" .
             "<td$varied>");

        //editable
        $ifc->set(t("editable"), "checkbox", TRUE, $data->get($name, "editable"));
        echo("</td>" .
             "<td$varied>");

        //required
        $ifc->set(t("required"), "checkbox", TRUE, $data->get($name, "required"));
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

    $ifc_response = $data->save() ? t("msg_done") : t("msg_error");
    break;

case "reset":
//..................................................................................................

    $filter_field  = NULL;
    $filter_option = NULL;
    $filter_value  = NULL;
    $group         = NULL;
    $page          = NULL;
    $limit         = NULL;
    $order         = NULL;

    cms_cache_delete(
        ["profile." . CMS_USER . ".filter_field",
         "profile." . CMS_USER . ".filter_option",
         "profile." . CMS_USER . ".filter_value",
         "profile." . CMS_USER . ".group",
         "profile." . CMS_USER . ".page",
         "profile." . CMS_USER . ".limit",
         "profile." . CMS_USER . ".order"]);
    break;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

//filter option list
$filter_option_list =
    [0 => " LIKE '#value#%'",
     1 => " LIKE '%#value#%'",
     2 => " LIKE '%#value#'",
     3 => ">'#value#'",
     4 => "='#value#'",
     5 => "<'#value#'"];

//order list
$order_list =
    [0 => CMS_DB_PROFILE_INDEX,   1 => CMS_DB_PROFILE_INDEX .   " DESC",
     2 => CMS_DB_PROFILE_CODE,    3 => CMS_DB_PROFILE_CODE .    " DESC",
     4 => CMS_DB_PROFILE_SURNAME, 5 => CMS_DB_PROFILE_SURNAME . " DESC"];

//sync settings
cms_cache_sync($filter_field,  "profile." . CMS_USER . ".filter_field",  CMS_DB_PROFILE_SURNAME);
cms_cache_sync($filter_option, "profile." . CMS_USER . ".filter_option",  0);
cms_cache_sync($filter_value,  "profile." . CMS_USER . ".filter_value",  "");
cms_cache_sync($group,         "profile." . CMS_USER . ".group",         CMS_DB_PROFILE_COMPANY);
cms_cache_sync($page,          "profile." . CMS_USER . ".page",           0);
cms_cache_sync($limit,         "profile." . CMS_USER . ".limit",         25);
cms_cache_sync($order,         "profile." . CMS_USER . ".order",          4);

$filter_option = (int)$filter_option;
$page          = (int)$page;
$limit         = (int)$limit;
$order         = (int)$order;

//menu
$menu =
    $_profile->operator ?
    [t("command_add") .             "|profile/command_add"              => "add",
     t("command_delete_selected") . "|profile/command_delete"           => "#delete",
     t("basic_settings") .          "|profile/command_configuration"    => "config_base",
     t("additional_settings") .     "|profile/command_configuration_ex" => "config_custom"] :
    NULL;

//display
$ifc = new ifc(
    $ifc_response,
    $ifc_page,
    $menu,
    ["order" => $order,
     "page"  => $page]);

//search filter
echo("<div class=\"element\">" .
     "<span>" . t("filter") . "</span><br>");

//field
$ifc->set([t("id")                  => CMS_DB_PROFILE_INDEX,
           t("identifier")          => CMS_DB_PROFILE_CODE,
           t("username")            => CMS_DB_PROFILE_USER,
           t("account_inheritance") => CMS_DB_PROFILE_SUPERUSER,
           t("organization")        => CMS_DB_PROFILE_COMPANY,
           t("forename")            => CMS_DB_PROFILE_PRENAME,
           t("surname")             => CMS_DB_PROFILE_SURNAME,
           t("street")              => CMS_DB_PROFILE_STREET,
           t("zip")                 => CMS_DB_PROFILE_ZIPCODE,
           t("city")                => CMS_DB_PROFILE_CITY,
           t("country")             => CMS_DB_PROFILE_COUNTRY,
           t("phone")               => CMS_DB_PROFILE_PHONE_1,
           t("phone_2")             => CMS_DB_PROFILE_PHONE_2,
           t("cell_phone")          => CMS_DB_PROFILE_MOBILE,
           t("fax")                 => CMS_DB_PROFILE_FAX,
           t("email")               => CMS_DB_PROFILE_EMAIL,
           t("url")                 => CMS_DB_PROFILE_URL,
           t("account_number")      => CMS_DB_PROFILE_ACCOUNT_NUMBER,
           t("bank")                => CMS_DB_PROFILE_FINANCIAL_INSTITUTION,
           t("bank_code")           => CMS_DB_PROFILE_BANKCODE,
           t("account_holder")      => CMS_DB_PROFILE_ACCOUNT_HOLDER,
           t("credit_card_number")  => CMS_DB_PROFILE_CREDIT_CARD_NUMBER,
           t("credit_institution")  => CMS_DB_PROFILE_CREDIT_INSTITUTE,
           t("credit_card_holder")  => CMS_DB_PROFILE_CREDIT_CARD_HOLDER,
           t("expiry")              => CMS_DB_PROFILE_CREDIT_CARD_VALIDITY,
           t("remark")              => CMS_DB_PROFILE_COMMENT],
          "select",
          $filter_field,
          NULL,
          "filter_field");

//condition
$ifc->set([t("starts_with")  => 0,
           t("contains")     => 1,
           t("ends_with")    => 2,
           t("greater_than") => 3,
           t("equal_to")     => 4,
           t("less_than")    => 5],
          "select",
          $filter_option,
          NULL,
          "filter_option");

//value
$ifc->set(NULL, "text 20 40 :", $filter_value, NULL, "filter_value");
echo("</div>");

//group by
$ifc->set(t("grouping"), "label");
$ifc->set([""                       => " ",
           t("account_inheritance") => CMS_DB_PROFILE_SUPERUSER,
           t("organization")        => CMS_DB_PROFILE_COMPANY,
           t("surname")             => CMS_DB_PROFILE_SURNAME,
           t("street")              => CMS_DB_PROFILE_STREET,
           t("zip")                 => CMS_DB_PROFILE_ZIPCODE,
           t("city")                => CMS_DB_PROFILE_CITY,
           t("country")             => CMS_DB_PROFILE_COUNTRY,
           t("bank")                => CMS_DB_PROFILE_FINANCIAL_INSTITUTION,
           t("bank_code")           => CMS_DB_PROFILE_BANKCODE,
           t("credit_institution")  => CMS_DB_PROFILE_CREDIT_INSTITUTE],
          "select",
          $group,
          NULL,
          "group");

//row display limit per page
$ifc->set(t("rows"), "label");
$ifc->set([ 10 => "10",
            25 => "25",
            50 => "50",
           100 => "100",
           250 => "250",
           500 => "500"],
          "select",
          $limit,
          NULL,
          "limit");

//apply button
$ifc->set(t("command_confirm"), "button");

//reset button
$ifc->set(t("reset"), "button b", "reset");

//retrieve list
$array = [CMS_DB_PROFILE_INDEX,
          CMS_DB_PROFILE_CODE,
          CMS_DB_PROFILE_PRENAME,
          CMS_DB_PROFILE_SURNAME,
          CMS_DB_PROFILE_PHONE_1,
          CMS_DB_PROFILE_FAX,
          CMS_DB_PROFILE_EMAIL];

if (CMS_IFC_SELECT && ! in_array(CMS_IFC_SELECT, $array))        $array[] = sqlesc(CMS_IFC_SELECT);
if (($flag = nstreq($group, " ")) && ! in_array($group, $array)) $array[] = sqlesc($group);

$query   =  "SELECT " .   implode(", ", $array) . " " .
            "FROM " .     CMS_DB_PROFILE;
if (nstre($filter_value)) $query .=
           " WHERE " .    sqlesc($filter_field) .
                          str_replace("#value#", sqlesc($filter_value),
                                      $filter_option_list[$filter_option] ?? $filter_option_list[0]);
$query  .= " GROUP BY " . CMS_DB_PROFILE_INDEX .
           " ORDER BY ";
$_order  = $order_list[$order] ?? CMS_DB_PROFILE_INDEX;
preg_match("/^(?:[^ ]+)((?: DESC)?)$/", $_order, $__order);
if ($flag && nstreq($group, $__order[0])) $query .=
                          sqlesc($group) . $__order[1] . ", ";
$query  .=                sqlesc($_order);

//empty result
if (
     (! $result = mysql_query($query))
     ||
     ! mysql_num_rows($result)
   )
{
    $ifc->set(t("empty"), "info");
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

function o(value)
{
    ifc_set("order", value);
    ifc_post();
};

function p(number)
{
    ifc_set("page", number);
    ifc_post();
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
        t("command_next"),
        "pagination");

    mysql_data_seek($result, $page * $limit);
};

//helper functions
$f1 = function($order, $asc, $dsc)
{
    switch ((int)$order)
    {
    case $asc: return " ↗";
    case $dsc: return " ↘";
    };

    return "";
};

$f2 = function($order, $asc, $dsc)
{
    switch ((int)$order)
    {
    default:
    case $dsc: return "javascript:o($asc);";
    case $asc: return "javascript:o($dsc);";
    };
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
$ifc->set(t("id") . $f1($order, 0, 1), "button", $f2($order, 0, 1));
echo("</th>" .

//code
     "<th>");
$ifc->set(t("identifier") . $f1($order, 2, 3), "button", $f2($order, 2, 3));
echo("</th>");

//select
if (CMS_IFC_SELECT)
    echo("<th>" .
         "</th>");

//name
echo("<th>");
$ifc->set(t("name") . $f1($order, 4, 5), "button", $f2($order, 4, 5));
echo("</th>" .

//phone 1
     "<th>" .
     t("phone") .
     "</th>" .

//email
     "<th>" .
     t("email") .
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
    if (nstreq($group, " "))
    {
        $value = nstre($resultrow[$group]) ? $resultrow[$group] : t("no_value");

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

        if (! $value) $value = t("no_value");
    };

    //select
    if (CMS_IFC_SELECT)
        echo("<td$varied>" .
             "<a href=\"javascript:r('" . qx($resultrow[CMS_IFC_SELECT]) . "');\" " .
                "title=\"" . t("command_insert") . "\">" .
             image("profile/button_return") .
             "</a>" .
             "</td>");

    //name
    echo("<td$varied>" .
         "<a href=\"javascript:d('" . qx($index) . "');\" " .
            "title=\"" . t("command_show") . "\">" .
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
$ifc->set(t("all"),    "button", "javascript:ifc_list_activate();");
$ifc->set(t("invert"), "button", "javascript:ifc_list_invert();");
$ifc->set(t("none"),   "button", "javascript:ifc_list_deactivate();");
echo("</td>" .
     "</tr>");
ifc_table_close();

//display newly created profile
if (
     streq(CMS_IFC_MESSAGE, "add")
     &&
     streq($ifc_response, t("msg_done"))
   )
    echo(jscript("d(\"" . rq($object) . "\");"));

$ifc->close();