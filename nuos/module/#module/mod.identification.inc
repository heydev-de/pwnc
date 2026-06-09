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

global $location,
       $identification_message,
       $identification_user,
       $identification_email,
       $identification_captcha_key,
       $identification_captcha_code,
       $identification_code;

$recover_message = "<div class=\"p\">" .
                   CMS_L_MOD_IDENTIFICATION_010 .
                   "</div>";

switch ($identification_message)
{
case "__recover":
//..................................................................................................

    if (stre($identification_code)) //no code
    {
        //fail
        echo("<div class=\"response-error\">" .
             CMS_L_MOD_IDENTIFICATION_017 .
             "</div>");
        return; //exit
    };

    //retrieve recovery data
    $data = new data("#system/identification.recover");
    $user = $data->get($identification_code, "user");
    $type = $data->get($identification_code, "type");

    //erase recovery data
    $data->del($identification_code);
    $data->save();

    $password = unique_id();
    $success  = FALSE;

    switch ($type)
    {
    case "permission":
//..................................................................................................

        //verify user
        $data  = new data("#system/permission");
        $email = $data->get("user.$user", "email");
        if ($email === NULL) break; //fail

        //set new password
        $data->set(hash64($password), "user.$user", "password");
        if (! $data->save()) break; //fail

        $success = TRUE;
        break;

    case "profile":
//..................................................................................................

        //verify user
        if (! cms_load("profile"))   break;
        $profile      = new profile(TRUE);
        $profile_data = $profile->get($user, CMS_DB_PROFILE_USER);
        if ($profile_data === FALSE) break; //fail
        $email        = $profile_data->email;

        //set new password
        $profile_data->password = $password;
        if (! $profile->set($profile_data)) break; //fail

        $success = TRUE;
        break;
    };

    if ($success) //successfully wrote new password
    {
        //success
        echo("<div class=\"response-success\">" .
             CMS_L_MOD_IDENTIFICATION_018 .
             "</div>");

        //send password mail
        if (
             cms_load("smtp")
             &&
             smtp_send($email,
                       CMS_L_MOD_IDENTIFICATION_022,
                       sprintf(CMS_L_MOD_IDENTIFICATION_021,
                               $user,
                               CMS_ROOT_URL,
                               $password))
           )
            //success
            echo("<div class=\"response-success\">" .
                 CMS_L_MOD_IDENTIFICATION_019 .
                 "</div>");
        else
            //fail
            echo("<div class=\"response-error\">" .
                 CMS_L_MOD_IDENTIFICATION_020 .
                 "</div>");
    }
    else
    {
        //fail
        echo("<div class=\"response-error\">" .
             CMS_L_MOD_IDENTIFICATION_017 .
             "</div>");
    };

    return; //exit

case "_recover":
//..................................................................................................

    //verify captcha
    if (
         cms_load("captcha")
         &&
         (! (new captcha())->verify($identification_captcha_key, $identification_captcha_code))
       )
    {
        //captcha mismatch
        $recover_message = "<div class=\"response-error\">" .
                           CMS_L_MOD_IDENTIFICATION_013 .
                           "</div>";
        break;
    };

    //fail as default
    $recover_message = "<div class=\"response-error\">" .
                       CMS_L_MOD_IDENTIFICATION_011 .
                       "</div>";

    if (stre($identification_user)) break; //missing user id, fail

    //seek user in permissions
    $data  = new data("#system/permission");
    $email = $data->get("user.$identification_user", "email");
    $type  = "permission";

    if (($email === NULL) && cms_load("profile")) //fail
    {
        //alternatively seek user in profiles
        $profile      = new profile(TRUE);
        $profile_data = $profile->get($identification_user, CMS_DB_PROFILE_USER);

        if (($profile_data !== FALSE) && nstre($profile_data->email))
        {
            $email = $profile_data->email;
            $type  = "profile";
        };
    };

    //user has valid email address
    if (verify_email($email))
    {
        //store recovery data
        $data = new data("#system/identification.recover");
        $code = unique_id(16);

        $data->set(["user"    => $identification_user,
                    "type"    => $type,
                    "#expire" => time() + 3600], //1 hour
                   $code);

        if (

        //save recovery data
             $data->save()
             &&

        //send recovery mail
             cms_load("smtp")
             &&
             smtp_send($email,
                       CMS_L_MOD_IDENTIFICATION_009,
                       sprintf(CMS_L_MOD_IDENTIFICATION_016,
                               $identification_user,
                               CMS_ROOT_URL,
                               cms_url(["identification_message" => "__recover",
                                        "identification_code"    => $code])))
           )
        {
            //success
            echo("<div class=\"response-success\">" .
                 CMS_L_MOD_IDENTIFICATION_012 .
                 "</div>");
            return; //exit
        };

    };
};

switch ($identification_message)
{
case "_recover":
case "recover":
//..................................................................................................

    echo("<section id=\"identification\" " .
                  "class=\"identification\">");

    insert("recover_top");

    echo("<form action=\"" . x(cms_url()) . "#identification\" " .
               "method=\"post\">" .

    //title
         "<h2>" .
         CMS_L_MOD_IDENTIFICATION_009 .
         "</h2>" .

    //instruction
         $recover_message .

    //username
         "<div class=\"p\">" .

         "<label for=\"identification-user\">" .
         CMS_L_MOD_IDENTIFICATION_002 .
         "</label><br>" .

         "<input id=\"identification-user\" " .
                "name=\"identification_user\" " .
                "type=\"text\" " .
                "value=\"" . x($identification_user) . "\" " .
                "autocomplete=\"username\" " .
                "required>" .

         "</div>"); //p

    //captcha
    if (cms_load("captcha"))
    {
        //create captcha
        $captcha = (new captcha())->create();

        //start output
        if ($captcha !== FALSE)
            echo("<div class=\"p\">" .

                 "<label for=\"identification-captcha\">" .
                 CMS_L_MOD_IDENTIFICATION_014 .
                 "</label><br>" .

            //captcha input
                 "<input id=\"identification-captcha\" " .
                        "name=\"identification_captcha_key\" " .
                        "type=\"text\"><br>" .

                 "<small>" .
                 CMS_L_MOD_IDENTIFICATION_015 .
                 "</small><br>" .

            //verification code
                 "<input name=\"identification_captcha_code\" " .
                        "type=\"hidden\" " .
                        "value=\"" . x($captcha["code"]) . "\">" .
                 "<img src=\"" . x($captcha["image"]) . "\" " .
                      "width=\"" . x($captcha["width"]) . "\" " .
                      "height=\"" . x($captcha["height"]) . "\" " .
                      "alt=\"" . CMS_L_MOD_IDENTIFICATION_014 . "\">" .

                 "</div>"); //p
    };

    //submit button
    echo("<div class=\"p\">" .

         "<button name=\"identification_message\" " .
                 "type=\"submit\" " .
                 "value=\"_recover\">" .
         CMS_L_COMMAND_CONFIRM .
         "</button>" .

         "</div>" . //p

         "</form>");

    insert("recover_bottom");

    echo("</section>"); //identification

    return; //exit
};

$flag_continue = FALSE;
if (stre($location)) $location      = cms_url();
else                 $flag_continue = TRUE;
$location      = x($location);

?><script src="<?php echo(x(CMS_JAVASCRIPT_URL));?>sha256.js"></script><script>

function identification_submit()
{
    if (document.getElementById("identification-legacy").checked)
        load_script("<?php echo(q(CMS_JAVASCRIPT_URL));?>md5.js",
                    _identification_submit);
    else
        _identification_submit(false);
    return false;
};

function _identification_submit(legacy = true)
{
    var salt     = "<?php echo(qb(cms_cache("cms.salt_password")));?>";
    var object   = document.getElementById("identification-password");
    var value    = object.value;
    value        = sha256(salt + (legacy ? md5(value) : sha256(value)));
    object.value = value;
    document.getElementById("identification-form").submit();
};

</script><?php

//login attempt limit
$attempt_key   = "identification.attempt" .
                 (isset($_SERVER["REMOTE_ADDR"]) ? "." . $_SERVER["REMOTE_ADDR"] : "");
$attempt_count = (int)cms_cache("$attempt_key.count");
$response      = NULL;
if ($attempt_count) //previously failed attempt
{
    if ($attempt_count >= CMS_LOGIN_ATTEMPT_MAX) //blocked
    {
        $attempt_time = cms_cache("$attempt_key.time");
        echo("<section id=\"identification\" " .
                      "class=\"identification\">" .
             sprintf(CMS_L_MOD_IDENTIFICATION_024,
                     ceil((CMS_LOGIN_BLOCK_TIME - time() + $attempt_time) / 60)) .
             "</section>");
        return; //exit
    };

    //remaining attempts reminder
    $response = sprintf(CMS_L_MOD_IDENTIFICATION_025,
                        CMS_LOGIN_ATTEMPT_MAX - $attempt_count);
};

echo("<section id=\"identification\" " .
              "class=\"identification\">");

insert("top");

echo("<form id=\"identification-form\" " .
           "action=\"$location\" " .
           "method=\"post\" " .
           "onsubmit=\"return identification_submit();\">" .

     "<noscript>" .
     "<input name=\"cms_login_noscript\" " .
            "type=\"hidden\" " .
            "value=\"1\">" .
     "</noscript>" .

//title
     "<h2>" .
     CMS_L_MOD_IDENTIFICATION_004 .
     "</h2>");

if ($response !== NULL)
    echo("<div class=\"response\">" .
         $response .
         "</div>");

//display current user
if (nstreq(CMS_USER, "anonymous"))
{
    echo("<div class=\"p\">" .
         sprintf(CMS_L_MOD_IDENTIFICATION_001, x(CMS_NAME), x(CMS_USER)) .
         "</div>" .

         "<div class=\"p\">");
    if ($flag_continue)
        echo("<a href=\"$location\" " .
                "class=\"button\">" .
             CMS_L_MOD_IDENTIFICATION_008 .
             "</a>");
    echo("<a href=\"" . x(cms_url(CMS_MODULES_URL . "identification.php", ["cms_logout" => 1])). "\" " .
            "class=\"button\">" .
         CMS_L_MOD_IDENTIFICATION_003 .
         "</a>" .
         "</div>"); //p
};

//username
echo("<div class=\"p\">" .

     "<label for=\"identification-user\">" .
     CMS_L_MOD_IDENTIFICATION_002 .
     "</label><br>" .

     "<input id=\"identification-user\" " .
            "name=\"cms_login_user\" " .
            "type=\"text\" " .
            "value=\"" . (isset($_POST["cms_login_user"]) ?
                         x($_POST["cms_login_user"]) :
                         (nstreq(CMS_USER, "anonymous") ? x(CMS_USER) : "")) . "\" " .
            "autocomplete=\"username\" " .
            "required>" .

     "</div>" . //p

//password
     "<div class=\"p\">" .

     "<label for=\"identification-password\">" .
     CMS_L_PASSWORD .
     "</label><br>" .

     "<input id=\"identification-password\" " .
            "name=\"cms_login_password\" " .
            "type=\"password\" " .
            "autocomplete=\"current-password\" " .
            "required><br>" .

//legacy password
     "<label>" .
     "<input id=\"identification-legacy\" " .
            "type=\"checkbox\" " .
            "value=\"1\">" .
     CMS_L_MOD_IDENTIFICATION_023 .
     "</label> | " .

//recover password
     "<a href=\"" . x(cms_url(["identification_message" => "recover"])) . "#identification\">" .
     CMS_L_MOD_IDENTIFICATION_006 .
     "</a>" .
     "</div>" . //p

//submit button
     "<div class=\"p\">" .

     "<button type=\"submit\">" .
     CMS_L_MOD_IDENTIFICATION_005 .
     "</button>" .

     "</div>" . //p
     "</form>");

insert("bottom");

echo("</section>"); //identification

?><script>

var object = document.getElementById("identification-user");
object.select();
object.focus();

</script>