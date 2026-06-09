<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   MODULE
//==================================================================================================

global $profile_message,
       $profile_param,
       $profile_user,
       $profile_email,
       $profile_captcha_key,
       $profile_captcha_code,
       $profile_code;

//load library
if (! cms_load("profile"))
{
    echo(CMS_MSG_UNAVAILABLE);
    return;
};

//instantiate
$profile = new profile();
if (! $profile->enabled)
{
    echo(CMS_MSG_UNAVAILABLE);
    return;
};

//operator status required for some operations
$profile->operator = TRUE;

//activation
if (streq($profile_message, "activate"))
{
    //seek registration data
    $data = new data("#system/profile.registration");
    if (stre($profile_code) || ($data->get($profile_code) === NULL))
    {
        //not found, invalid code
        echo("<div class=\"response-error\">" .
             CMS_L_MOD_PROFILE_047 .
             "</div>");
        return;
    };

    //set profile data
    $profile_data = new profile_data();
    $array        = get_object_vars($profile_data);

    foreach ($array AS $key => $_)
        $profile_data->$key = $data->get($profile_code, $key);

    $profile_data->superuser = "profile";
    $profile_data->superuser = "profile";
    $profile_data->enabled   = TRUE;

    //create profile
    if ($profile->add($profile_data))
    {
        //log
        $log = new log();
        $log->access("registered", $profile_data->user);
        $log->user($profile_data->user, $profile_data->email);

        //remove registration data
        $data->del($profile_code);
        $data->save();

        if (headers_sent())
        {
            //success
            echo("<div class=\"response-success\">" .
                 CMS_L_MOD_PROFILE_048 .
                 "</div>");
            return;
        };

        //encrypt password
        $password = hash64(cms_cache("cms.salt_password") . hash64($profile_data->password));

        //set credentials
        cms_set_cookie(["cms_user"     => $profile_data->user,
                        "cms_password" => $ifc_param3]);

        //redirect
        header("Location: " . cms_url(["profile_message" => "_activate"]), TRUE, 303);
        exit();
    };

    //error
    echo("<div class=\"response-error\">" .
         CMS_MSG_ERROR .
         "</div>");
    return;
}

//retrieve profile
elseif (($profile_data = $profile->get(CMS_USER, CMS_DB_PROFILE_USER)) !== FALSE)
{
    if (stre($profile_message)) $profile_message = "edit";
}

//no profile
else
{
    //create new profile
    $profile_data = new profile_data();
    if (stre($profile_message)) $profile_message = "register";
};

//build field configuration
$data = new data("#system/profile");

//categories
$title = [1 => CMS_L_MOD_PROFILE_002,
          2 => CMS_L_MOD_PROFILE_003,
          3 => CMS_L_MOD_PROFILE_004,
          4 => CMS_L_MOD_PROFILE_005,
          5 => CMS_L_MOD_PROFILE_006,
          6 => CMS_L_MOD_PROFILE_007,
          7 => CMS_L_MOD_PROFILE_011];

//default fields
$field = [1 => [CMS_DB_PROFILE_CODE                  => CMS_L_MOD_PROFILE_008,
                CMS_DB_PROFILE_USER                  => CMS_L_MOD_PROFILE_009,
                CMS_DB_PROFILE_PASSWORD              => CMS_L_PASSWORD],
          2 => [CMS_DB_PROFILE_COMPANY               => CMS_L_MOD_PROFILE_012,
                CMS_DB_PROFILE_PRENAME               => CMS_L_MOD_PROFILE_013,
                CMS_DB_PROFILE_SURNAME               => CMS_L_MOD_PROFILE_014,
                CMS_DB_PROFILE_STREET                => CMS_L_MOD_PROFILE_015,
                CMS_DB_PROFILE_ZIPCODE               => CMS_L_MOD_PROFILE_016,
                CMS_DB_PROFILE_CITY                  => CMS_L_MOD_PROFILE_017,
                CMS_DB_PROFILE_COUNTRY               => CMS_L_MOD_PROFILE_018],
          3 => [CMS_DB_PROFILE_PHONE_1               => CMS_L_MOD_PROFILE_019,
                CMS_DB_PROFILE_PHONE_2               => CMS_L_MOD_PROFILE_020,
                CMS_DB_PROFILE_MOBILE                => CMS_L_MOD_PROFILE_021,
                CMS_DB_PROFILE_FAX                   => CMS_L_MOD_PROFILE_022,
                CMS_DB_PROFILE_EMAIL                 => CMS_L_EMAIL,
                CMS_DB_PROFILE_URL                   => CMS_L_URL],
          4 => [CMS_DB_PROFILE_ACCOUNT_NUMBER        => CMS_L_MOD_PROFILE_023,
                CMS_DB_PROFILE_FINANCIAL_INSTITUTION => CMS_L_MOD_PROFILE_024,
                CMS_DB_PROFILE_BANKCODE              => CMS_L_MOD_PROFILE_025,
                CMS_DB_PROFILE_ACCOUNT_HOLDER        => CMS_L_MOD_PROFILE_026],
          5 => [CMS_DB_PROFILE_CREDIT_CARD_NUMBER    => CMS_L_MOD_PROFILE_027,
                CMS_DB_PROFILE_CREDIT_INSTITUTE      => CMS_L_MOD_PROFILE_028,
                CMS_DB_PROFILE_CREDIT_CARD_HOLDER    => CMS_L_MOD_PROFILE_029,
                CMS_DB_PROFILE_CREDIT_CARD_VALIDITY  => CMS_L_MOD_PROFILE_030],
          6 => [CMS_DB_PROFILE_COMMENT               => CMS_L_MOD_PROFILE_031]];

//custom fields
for ($i = 1; $i <= 20; $i++)
{
    if (! $_title = $data->get(CMS_DB_PROFILE_CUSTOM_FIELD . $i, "title"))
        $_title = CMS_L_MOD_PROFILE_010 . " $i";

    $field[7][CMS_DB_PROFILE_CUSTOM_FIELD . $i] = $_title;
};

//read configuration
$visible  = [];
$editable = [];
$required = [];
$mismatch = [];

//registration
if ($flag = in_array($profile_message, ["register", "_register"]))
{
    //remove fields used in registration
    unset($field[1]);
    unset($field[3][CMS_DB_PROFILE_EMAIL]);

    //collect field settings
    foreach ($field AS $key => $value)
    {
        foreach ($value AS $_key => $_)
        {
            //always visible and editable when required during registration
            if ($_flag = $data->get($_key, "required"))
                $required[$key][$_key] = TRUE;
            if ($_flag || $data->get($_key, "visible"))
            {
                $visible[$key][$_key]  = TRUE;
                if ($_flag || $data->get($_key, "editable"))
                    $editable[$key][$_key] = TRUE;
            };
        };
    };
}

//editing
else
{
    //collect field settings
    foreach ($field AS $key => $value)
    {
        foreach ($value AS $_key => $_)
        {
            if ($data->get($_key, "visible"))
            {
                $visible[$key][$_key] = TRUE;
                if ($data->get($_key, "editable"))
                {
                    $editable[$key][$_key] = TRUE;
                    if ($data->get($_key, "required"))
                        $required[$key][$_key] = TRUE;
                };
            };
        };
    };
};

$user_equals_email = ! (bool)$data->get(CMS_DB_PROFILE_USER, "required");

//process form data
while (in_array($profile_message, ["_register", "_edit"]))
{
    //set field data
    if (count($editable) > 0)
    {
        foreach ($editable AS $key => $value)
        {
            foreach ($value AS $_key => $_value)
            {
                if (! isset($profile_param[$_key])) continue;

                switch ($_key)
                {
                default:
//..................................................................................................

                    $_value = $profile_param[$_key];
                    break;

                case CMS_DB_PROFILE_EMAIL:
//..................................................................................................

                    $_value = $profile_param[$_key];
                    if (stre($_value)) break;

                    //verify email
                    if (! verify_email($_value))
                    {
                        $mismatch[$key][$_key] = CMS_L_MOD_PROFILE_044;
                        continue 2;
                    };

                    break;

                case CMS_DB_PROFILE_PASSWORD:
//..................................................................................................

                    //keep password
                    if (stre($profile_param[$_key][0])) continue 2;

                    //password and confirmation do not match
                    if (nstreq($profile_param[$_key][0],
                               $profile_param[$_key][1]))
                    {
                        $mismatch[$key][$_key] = CMS_L_MOD_PROFILE_032;
                        continue 2;
                    };

                    //password is too short
                    if (strlen($profile_param[$_key][0]) < 4)
                    {
                        $mismatch[$key][$_key] = CMS_L_MOD_PROFILE_033;
                        continue 2;
                    };

                    $_value = $profile_param[$_key][0];
                    break;

                case CMS_DB_PROFILE_CREDIT_CARD_VALIDITY:
//..................................................................................................

                    $_value = implode("/", $profile_param[$_key]);
                    break;
                };

                //check if required data is given
                if (isset($required[$key][$_key]) && stre(utf8_trim($_value)))
                {
                    $mismatch[$key][$_key] = CMS_L_MOD_PROFILE_051;
                    continue;
                };

                $profile_data->$_key = $_value;
            };
        };

        //save
        if (
             (count($mismatch) === 0)
             &&
             streq($profile_message, "_edit")
           )
        {
            if ($profile->set($profile_data))
                echo("<div class=\"response-success\">" .
                     CMS_L_MOD_PROFILE_034 .
                     "</div>");
            break;
        };
    };

    //registration

    //verify user
    if (! $user_equals_email)
    {
        $profile_user = stripspaces($profile_user);
        $profile_user = utf8_substr($profile_user, 0, 40);
        $profile_user = utf8_trim($profile_user);
        if (stre($profile_user))
        {
            $mismatch[0]["user"] = CMS_L_MOD_PROFILE_051;
        }
        else
        {
            $permission   = new data("#system/permission");
            $registration = new data("#system/profile.registration");

            if (
                 ($profile->get($profile_user, CMS_DB_PROFILE_USER) !== FALSE)
                 ||
                 ($permission->get("user.$profile_user") !== NULL)
                 ||
                 ($registration->seek(["user" => $profile_user]) !== FALSE)
               )
                $mismatch[0]["user"] = CMS_L_MOD_PROFILE_045;
        };
    };

    //verify email
    if (stre($profile_email))
        $mismatch[0]["email"] = CMS_L_MOD_PROFILE_051;

    elseif (! verify_email($profile_email))
        $mismatch[0]["email"] = CMS_L_MOD_PROFILE_044;

    elseif ($user_equals_email)
    {
        $profile_user = $profile_email;
        $permission   = new data("#system/permission");
        $registration = new data("#system/profile.registration");

        if (
             ($profile->get($profile_user, CMS_DB_PROFILE_USER) !== FALSE)
             ||
             ($permission->get("user.$profile_user") !== NULL)
             ||
             ($registration->seek(["user" => $profile_user]) !== FALSE)
           )
            $mismatch[0]["email"] = CMS_L_MOD_PROFILE_052;
    };

    //verify captcha
    if (
         cms_load("captcha")
         &&
         (! (new captcha())->verify($profile_captcha_key, $profile_captcha_code))
       )
        $mismatch[0]["captcha"] = CMS_L_MOD_PROFILE_046; //captcha mismatch

    //mismatch found
    if (count($mismatch)) break;

    //set registration data
    $code     = unique_id();
    $password = unique_id();
    $registration->set(
        ["user"     => $profile_user,
         "email"    => $profile_email,
         "password" => $password,
         "#expire"  => time() + 86400] +
        get_object_vars($profile_data),
        $code);

    if (

    //save registration data
         $registration->save()
         &&

    //send confirmation mail
         cms_load("smtp")
         &&
         smtp_send($profile_email,
                   CMS_L_MOD_PROFILE_041,
                   sprintf(CMS_L_MOD_PROFILE_042,
                           CMS_ROOT_URL,
                           $profile_user,
                           $password,
                           cms_url(["profile_message" => "activate",
                                    "profile_code"    => $code])))
       )
    {
        //success
        echo("<div class=\"response-success\">" .
             CMS_L_MOD_PROFILE_043 .
             "<div>");
        return;
    };

    //error
    echo("<div class=\"response-error\">" .
         CMS_MSG_ERROR .
         "</div>");

    break;
};

//display
echo("<section id=\"profile\" " .
              "class=\"profile\">");

insert("top");

echo("<form action=\"" . x(cms_url()) . "#profile\" " .
           "method=\"post\">");

//display activation message
if (streq($profile_message, "_activate"))
    echo("<div class=\"response-success\">" .
         CMS_L_MOD_PROFILE_048 .
         "</div>");

if ($flag)
{
    //title
    echo("<h2>" .
         CMS_L_MOD_PROFILE_039 .
         "</h2>");

    //display mismatch message
    if (count($mismatch) > 0)
        echo("<div class=\"response-error\">" .
             CMS_L_MOD_PROFILE_036 .
             "</div>");

    //user
    if (! $user_equals_email)
    {
        $_mismatch = isset($mismatch[0]["user"]);
        echo("<div class=\"p" . ($_mismatch ? " profile-mismatch" : "") . "\">" .

             "<label for=\"profile-user\">" .
             "* " . CMS_L_MOD_PROFILE_009 .
             "</label><br>");

        if ($_mismatch)
            echo("<div>" .
                 x($mismatch[0]["user"]) .
                 "</div>");

        echo("<input id=\"profile-user\" " .
                    "name=\"profile_user\" " .
                    "type=\"text\" " .
                    "value=\"" . x($profile_user) . "\">" .

             "</div>"); //p
    };

    //email
    $_mismatch = isset($mismatch[0]["email"]);
    echo("<div class=\"p" . ($_mismatch ? " profile-mismatch" : "") . "\">" .

         "<label for=\"profile-email\">" .
         "* " . CMS_L_EMAIL .
         "</label><br>");

    if ($_mismatch)
        echo("<div>" .
             x($mismatch[0]["email"]) .
             "</div>");

    echo("<input id=\"profile-email\" " .
                "name=\"profile_email\" " .
                "type=\"text\" " .
                "value=\"" . x($profile_email) . "\">" .

         "</div>"); //p

    //captcha
    if (cms_load("captcha"))
    {
        //create captcha
        $captcha = (new captcha())->create();

        //start output
        if ($captcha !== FALSE)
        {
            $_mismatch = isset($mismatch[0]["captcha"]);
            echo("<div class=\"p" . ($_mismatch ? " profile-mismatch" : "") . "\">" .

                 "<label for=\"profile-captcha\">" .
                 "* " . CMS_L_MOD_PROFILE_037 .
                 "</label><br>");

            if ($_mismatch)
                echo("<div>" .
                     x($mismatch[0]["captcha"]) .
                     "</div>");

            //captcha input
            echo("<input id=\"profile-captcha\" " .
                        "name=\"profile_captcha_key\" " .
                        "type=\"text\" " .
                        "value=\"\"><br>" .

                 "<small>" .
                 CMS_L_MOD_PROFILE_038 .
                 "</small><br>" .

            //verification code
                 "<input name=\"profile_captcha_code\" " .
                        "type=\"hidden\" " .
                        "value=\"" . x($captcha["code"]) . "\">" .
                 "<img src=\"" . x($captcha["image"]) . "\" " .
                      "width=\"" . x($captcha["width"]) . "\" " .
                      "height=\"" . x($captcha["height"]) . "\" " .
                      "alt=\"" . CMS_L_MOD_PROFILE_037 . "\">" .

                 "</div>"); //p
        };
    };
};

//generate input form
foreach ($visible AS $key => $value)
{
    //title
    echo("<h2>" .
         $title[$key] .
         "</h2>");

    //display mismatch message
    if ((! $flag) && (count($mismatch) > 0))
    {
        echo("<div class=\"response-error\">" .
             CMS_L_MOD_PROFILE_036 .
             "</div>");
        $flag = TRUE;
    };

    //content
    foreach ($value AS $_key => $_)
    {
        $_value    = $profile_data->$_key;
        $id        = "profile-" . preg_replace("/[^0-9a-zA-Z]+/", "-", $_key);
        $name      = isset($editable[$key][$_key]) ? "profile_param[" . x($_key) . "]" : "";
        $_mismatch = isset($mismatch[$key][$_key]);
        $required  = isset($required[$key][$_key]);
        $_flag     = TRUE;

        switch ($_key)
        {
        default:
//..................................................................................................

            echo("<div class=\"p" . ($_mismatch ? " profile-mismatch" : "") . "\">" .
                 "<label for=\"$id\">" .
                 ($required ? "* " : "") . x($field[$key][$_key]) .
                 "</label><br>");
            break;

        case CMS_DB_PROFILE_PASSWORD:
        case CMS_DB_PROFILE_CREDIT_CARD_VALIDITY:
//..................................................................................................

            echo("<fieldset" . ($_mismatch ? " class=\"profile-mismatch\"" : "") . ">" .
                 "<legend>" .
                 ($required ? "* " : "") . x($field[$key][$_key]) .
                 "</legend>");
            $_flag = FALSE;
            break;
        };

        if ($_mismatch)
            echo("<div>" .
                 x($mismatch[$key][$_key]) .
                 "</div>");

        switch ($_key)
        {
        case CMS_DB_PROFILE_CODE:
        case CMS_DB_PROFILE_USER:
        case CMS_DB_PROFILE_COMPANY:
        case CMS_DB_PROFILE_PRENAME:
        case CMS_DB_PROFILE_SURNAME:
        case CMS_DB_PROFILE_STREET:
        case CMS_DB_PROFILE_ZIPCODE:
        case CMS_DB_PROFILE_CITY:
        case CMS_DB_PROFILE_COUNTRY:
        case CMS_DB_PROFILE_PHONE_1:
        case CMS_DB_PROFILE_PHONE_2:
        case CMS_DB_PROFILE_MOBILE:
        case CMS_DB_PROFILE_FAX:
        case CMS_DB_PROFILE_EMAIL:
        case CMS_DB_PROFILE_URL:
        case CMS_DB_PROFILE_ACCOUNT_NUMBER:
        case CMS_DB_PROFILE_FINANCIAL_INSTITUTION:
        case CMS_DB_PROFILE_BANKCODE:
        case CMS_DB_PROFILE_ACCOUNT_HOLDER:
        case CMS_DB_PROFILE_CREDIT_INSTITUTE:
        case CMS_DB_PROFILE_CREDIT_CARD_HOLDER:
        default:
//..................................................................................................

            //fixed
            if (stre($name))
            {
                echo(stre($_value) ? CMS_L_MOD_PROFILE_001 : x($_value));
                break;
            };

            //editable
            echo("<input id=\"$id\" " .
                        "name=\"$name\" " .
                        "type=\"text\" " .
                        "value=\"" . x($_value) . "\">");
            break;

        case CMS_DB_PROFILE_PASSWORD:
//..................................................................................................

            //fixed
            if (stre($name))
            {
                echo("****");
                break;
            };

            //editable
            if (! $mismatch)
                echo("<div class=\"p\">" .
                     CMS_L_MOD_PROFILE_049 .
                     "</div>");

            echo("<div class=\"p\">" .
                 "<label for=\"$id\">" .
                 x($field[$key][$_key]) .
                 "</label><br>" .
                 "<input id=\"$id\" " .
                        "name=\"" . $name . "[0]\" " .
                        "type=\"password\"><br>" .
                 "</div>" . //p

                 "<div class=\"p\">" .
                 "<label for=\"$id-confirmation\">" .
                 "* " . CMS_L_MOD_PROFILE_035 .
                 "</label><br>" .
                 "<input id=\"$id-confirmation\" " .
                        "name=\"" . $name . "[1]\" " .
                        "type=\"password\">" .
                 "</div>"); //p
            break;

        case CMS_DB_PROFILE_CREDIT_CARD_VALIDITY:
//..................................................................................................

            //fixed
            if (stre($name))
            {
                echo(stre($_value) ? CMS_L_MOD_PROFILE_001 : x($_value));
                break;
            };

            //editable
            $_value = explode("/", $_value);

            select(["01" => "01", "02" => "02", "03" => "03", "04" => "04",
                    "05" => "05", "06" => "06", "07" => "07", "08" => "08",
                    "09" => "09", "10" => "10", "11" => "11", "12" => "12"],
                   $_value[0] ?? date("m"),
                   $name . "[0]");
            echo(" / " .
                 "<input id=\"$id\" " .
                        "name=\"" . $name . "[1]\" " .
                        "type=\"text\" " .
                        "value=\"" . ($_value[1] ?? date("Y")) . "\" " .
                        "size=\"4\">");
            break;

        case CMS_DB_PROFILE_COMMENT:
//..................................................................................................

            //fixed
            if (stre($name))
            {
                echo(stre($_value) ? CMS_L_MOD_PROFILE_001 : x($_value));
                break;
            };

            //editable
            echo("<textarea id=\"$id\" " .
                           "name=\"$name\">\n" .
                 x($_value) .
                 "</textarea>");
            break;
        };

        echo($_flag ? "</div>" : "</fieldset>");
    };
};

switch ($profile_message)
{
case "register":
case "_register":
//..................................................................................................

    //submit button
    echo("<div class=\"p\">" .
         "<button name=\"profile_message\" " .
                 "type=\"submit\" " .
                 "value=\"_register\">" .
         CMS_L_MOD_PROFILE_040 .
         "</button>" .
         "</div>"); //p

    break;

default:
case "_activate":
case "edit":
case "_edit":
//..................................................................................................

    //save button
    if (count($editable) > 0)
        echo("<div class=\"p\">" .
             "<button name=\"profile_message\" " .
                     "type=\"submit\" " .
                     "value=\"_edit\">" .
             CMS_L_COMMAND_SAVE .
             "</button>" .
             "</div>"); //p

    break;
};

echo("</form>");

insert("bottom");
permission(["" => CMS_L_ACCESS]);

echo("</section>"); //profile