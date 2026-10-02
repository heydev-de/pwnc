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
                   t("please_enter_your_username") .
                   "</div>";

switch ($identification_message)
{
case "__recover":
//..................................................................................................

    if (stre($identification_code)) //no code
    {
        //fail
        echo("<div class=\"response-error\">" .
             t("err_operation_invalid") .
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
             t("msg_password_set") .
             "</div>");

        //send password mail
        if (
             cms_load("smtp")
             &&
             smtp_send($email,
                       t("your_new_password"),
                       sprintf(t("mail_password_set"),
                               $user,
                               CMS_ROOT_URL,
                               $password))
           )
            //success
            echo("<div class=\"response-success\">" .
                 t("msg_password_sent") .
                 "</div>");
        else
            //fail
            echo("<div class=\"response-error\">" .
                 t("err_password_send_failed") .
                 "</div>");
    }
    else
    {
        //fail
        echo("<div class=\"response-error\">" .
             t("err_operation_invalid") .
             "</div>");
    };

    return; //exit

case "_recover":
//..................................................................................................

    //verify captcha
    if (
         cms_load("captcha")
         &&
         ! (new captcha())->verify($identification_captcha_key, $identification_captcha_code)
       )
    {
        //captcha mismatch
        $recover_message = "<div class=\"response-error\">" .
                           t("err_captcha_incorrect") .
                           "</div>";
        break;
    };

    //fail as default
    $recover_message = "<div class=\"response-error\">" .
                       t("err_user_unknown") .
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
                       t("set_new_password"),
                       sprintf(t("mail_password_reset_link"),
                               $identification_user,
                               CMS_ROOT_URL,
                               cms_url(["identification_message" => "__recover",
                                        "identification_code"    => $code])))
           )
        {
            //success
            echo("<div class=\"response-success\">" .
                 t("msg_reset_mail_sent") .
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
         t("set_new_password") .
         "</h2>" .

    //instruction
         $recover_message .

    //username
         "<div class=\"p\">" .

         "<label for=\"identification-user\">" .
         t("username") .
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
                 t("captcha") .
                 "</label><br>" .

            //captcha input
                 "<input id=\"identification-captcha\" " .
                        "name=\"identification_captcha_key\" " .
                        "type=\"text\"><br>" .

                 "<small>" .
                 t("hint_captcha") .
                 "</small><br>" .

            //verification code
                 "<input name=\"identification_captcha_code\" " .
                        "type=\"hidden\" " .
                        "value=\"" . x($captcha["code"]) . "\">" .
                 "<img src=\"" . x($captcha["image"]) . "\" " .
                      "width=\"" . x($captcha["width"]) . "\" " .
                      "height=\"" . x($captcha["height"]) . "\" " .
                      "alt=\"" . t("captcha") . "\">" .

                 "</div>"); //p
    };

    //submit button
    echo("<div class=\"p\">" .

         "<button name=\"identification_message\" " .
                 "type=\"submit\" " .
                 "value=\"_recover\">" .
         t("command_confirm") .
         "</button>" .

         "</div>" . //p

         "</form>");

    insert("recover_bottom");

    echo("</section>"); //identification

    return; //exit
};

//verify location
while (nstre($location))
{
    if (

    //url is valid
         ($url = parse_url($location))
         &&

    //protocol empty or http(s)
         (
           ! isset($url["scheme"])
           ||
           preg_match("/^https?$/i", $url["scheme"])
         )
       )
    {
        //host empty
        if (! isset($url["host"])) break;

        //verify against canonical or current host
        $host = is_file($path = CMS_DATA_PATH . "#system/host.txt") ?
                file($path, FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES) :
                FALSE;
        if (! $host) $host = [CMS_DOMAIN];

        //match
        if (in_array(strtolower($url["host"]), $host)) break;
    };

    //invalidate
    $location = NULL;
    break;
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
    var salt     = "<?php echo(qb(cms_salt("password")));?>";
    var object   = document.getElementById("identification-password");
    var value    = str2utf8bin(object.value);
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
             sprintf(t("err_login_locked"),
                     ceil((CMS_LOGIN_BLOCK_TIME - time() + $attempt_time) / 60)) .
             "</section>");
        return; //exit
    };

    //remaining attempts reminder
    $response = sprintf(t("login_attempts_left"),
                        CMS_LOGIN_ATTEMPT_MAX - $attempt_count);
};

echo("<section id=\"identification\" " .
              "class=\"identification\">");

insert("top");

echo("<form id=\"identification-form\" " .
           "action=\"$location\" " .
           "method=\"post\" " .
           "onsubmit=\"event.preventDefault();return identification_submit();\">" .

     "<noscript>" .
     "<input name=\"cms_login_noscript\" " .
            "type=\"hidden\" " .
            "value=\"1\">" .
     "</noscript>" .

//title
     "<h2>" .
     t("login") .
     "</h2>");

if ($response !== NULL)
    echo("<div class=\"response\">" .
         $response .
         "</div>");

//display current user
if (nstreq(CMS_USER, "anonymous"))
{
    echo("<div class=\"p\">" .
         sprintf(t("logged_in_as"), x(CMS_NAME), x(CMS_USER)) .
         "</div>" .

         "<div class=\"p\">");
    if ($flag_continue)
        echo("<a href=\"$location\" " .
                "class=\"button\">" .
             t("proceed_as_current_user") .
             "</a>");
    echo("<a href=\"" . x(cms_url(CMS_MODULES_URL . "identification.php", ["cms_logout" => 1])). "\" " .
            "class=\"button\">" .
         t("log_out") .
         "</a>" .
         "</div>"); //p
};

//username
echo("<div class=\"p\">" .

     "<label for=\"identification-user\">" .
     t("username") .
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
     t("password") .
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
     t("legacy") .
     "</label> | " .

//recover password
     "<a href=\"" . x(cms_url(["identification_message" => "recover"])) . "#identification\">" .
     t("forgot_password") .
     "</a>" .
     "</div>" . //p

//submit button
     "<div class=\"p\">" .

     "<button type=\"submit\">" .
     t("log_in") .
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