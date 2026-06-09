<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//isolate
(function() {

//overload constant functions to automatically include namespaces
function constant($name)
{
    return \constant(__NAMESPACE__ . "\\$name");
};

//--------------------------------------------------------------------------------------------------

function define($name,
                $value,
                $case_insensitive = FALSE)
{
    return \define(__NAMESPACE__ . "\\$name", $value, $case_insensitive);
};

//--------------------------------------------------------------------------------------------------

function defined($name)
{
    return \defined(__NAMESPACE__ . "\\$name");
};

//==================================================================================================
//   SOFTWARE INFORMATION
//==================================================================================================

define("CMS_SOFTWARE",   "PWNC");
define("CMS_VERSION",    is_file(dirname(__FILE__) . "/version.txt") ?
                         file_get_contents(dirname(__FILE__) . "/version.txt") : "?");
define("CMS_COPYRIGHT",  "© 2026 Patrick Heyer");
define("CMS_HOMEPAGE",   "https://pwnc.it");
define("CMS_IDENTIFIER", CMS_SOFTWARE . "/1.0 (+" . CMS_HOMEPAGE . ")");
define("CMS",            CMS_SOFTWARE . " " . CMS_VERSION . " " . CMS_COPYRIGHT . ", " . CMS_HOMEPAGE);

//==================================================================================================
//   REQUIREMENTS / RESOURCES
//==================================================================================================

if (version_compare(PHP_VERSION, "7.4.0", "<"))
    die("PHP7 >= 7.4.0 is required.");

if (! extension_loaded("mysqli"))
    die("MySQLi support is required.");

if (! extension_loaded("pcre"))
    die("PCRE support is required.");

if (! preg_match("/\pL/u", "a"))
    die("PCRE UTF-8 support is required.");

if (! extension_loaded("sodium"))
    die("Sodium support is required.");

//set minimum resource limits
if (function_exists("ini_set"))
{
    function cms_ini_set_minimum($key, $value)
    {
        $array = ["" => 1, "K" => 1024, "M" => 1048576, "G" => 1073741824];

        if (! preg_match("/^(-?\d+)([kmg]?)$/i", $value, $value_new)) return FALSE;
        $value_new = $value_new[1] * $array[strtoupper($value_new[2])];
        if ($value_new < 0) return ini_set($key, $value); //unlimited

        if (($_value = ini_get($key)) === FALSE) return FALSE;

        if (! preg_match("/^(-?\d+)([kmg]?)$/i", $_value, $value_old)) return FALSE;
        $value_old = $value_old[1] * $array[strtoupper($value_old[2])];
        if ($value_old < 0) return $_value;

        if ($value_new > $value_old) return ini_set($key, $value);
        return $_value;
    };

    cms_ini_set_minimum("max_execution_time",  "600");
    cms_ini_set_minimum("max_file_uploads",    "500");
    cms_ini_set_minimum("memory_limit",        "256M");
    cms_ini_set_minimum("post_max_size",       "505M");
    cms_ini_set_minimum("upload_max_filesize", "500M");
};

//==================================================================================================
//   INITIAL SETTINGS
//==================================================================================================

//user abort
ignore_user_abort(TRUE);

//output buffering
ob_implicit_flush(FALSE);
ob_start();

//timezone
define("CMS_TIMEZONE_DEFAULT", "UTC");
date_default_timezone_set(CMS_TIMEZONE_DEFAULT);

//file permissions
umask(0002);

//==================================================================================================
//   ERROR HANDLING
//==================================================================================================

error_reporting(E_ALL);

//error handler
function cms_error($code,
                   $message,
                   $path    = NULL,
                   $line    = NULL,
                   $display = NULL)
{
    /*
      buffer, output and log errors
    */

    //silent mode
    if (cms_cache("cms.error.silent")) return TRUE;

    //ignore disabled errors
    if (($code >= 0) && ! (error_reporting() & $code)) return FALSE;

    static $array = []; //error buffer

    //error count
    $count = count($array) + 1;

    //output error buffer
    if ($code === -1)
    {
        if ($count === 1) return TRUE; //no errors

        //output
        echo("<script>\n" .
             "document.title = \"⚠️ \" + document.title;\n");
        foreach ($array AS $value) echo($value . "\n");
        echo("</script>");

        $array = []; //reset

        return TRUE;
    };

    //ignore deprecated and strict info
    if (($code & (E_ALL & ~E_DEPRECATED & ~E_STRICT)) === 0) return;

    //error types
    $_array = [E_ERROR             => ["Error",             "#FF0000", "error"],
               E_WARNING           => ["Warning",           "#FF7F00", "warn"],
               E_PARSE             => ["Parse error",       "#FF0000", "error"],
               E_NOTICE            => ["Notice",            "#007FFF", "info"],
               E_CORE_ERROR        => ["Core error",        "#FF0000", "error"],
               E_CORE_WARNING      => ["Core warning",      "#FF7F00", "warn"],
               E_COMPILE_ERROR     => ["Compile error",     "#FF0000", "error"],
               E_COMPILE_WARNING   => ["Compile warning",   "#FF7F00", "warn"],
               E_USER_ERROR        => ["User error",        "#FF0000", "error"],
               E_USER_WARNING      => ["User warning",      "#FF7F00", "warn"],
               E_USER_NOTICE       => ["User notice",       "#007FFF", "info"],
               E_STRICT            => ["Strict",            "#777777", "info"],
               E_RECOVERABLE_ERROR => ["Recoverable Error", "#FF0000", "error"],
               E_DEPRECATED        => ["Deprecated",        "#777777", "info"],
               E_USER_DEPRECATED   => ["User deprecated",   "#777777", "info"],
               100                 => ["Unknown error",     "#777777", "log"]];

    if (! isset($_array[$code])) $code = 100;

    //compose error message
    $buffer = rtrim($message, "\r\n") .
              ($path ? " in $path"      : "") .
              ($line ? " on line $line" : "") . "\n";

    if (

         //explicit
         ($display === TRUE)
         ||

         //local development environment
         (isset($_SERVER["REMOTE_ADDR"]) && in_array($_SERVER["REMOTE_ADDR"], ["127.0.0.1", "::1"]))
         ||

         //admin
         (defined("CMS_SUPERUSER") && (constant("CMS_SUPERUSER") === "admin"))
       )
    {
        //display details
        $__array = debug_backtrace();
        array_shift($__array); //skip current function

        foreach ($__array AS $key => $value)
        {
            //output stack level
            if (($file     = ($value["file"]     ?? "")) === "") $file     = "unknown";
            if (($line     = ($value["line"]     ?? "")) === "") $line     = "unknown";
            if (($function = ($value["function"] ?? "")) === "") $function = "unknown";
            $class = $value["class"] ?? "";
            $type  = $value["type"]  ?? "";

            $buffer .= "\n" .
                       "$key: file $file, line $line\n" .
                       "  $class$type$function\n" .
                       "    (\n";

            //arguments
            if (isset($value["args"]) && is_array($value["args"]))
            {
                foreach ($value["args"] AS $_key => $_value)
                {
                    $buffer .= "      $_key: ";
                    $type = gettype($_value);

                    switch ($type)
                    {
                    case "boolean":

                        $buffer .= "(boolean)" . ($_value ? "TRUE" : "FALSE");
                        break;

                    case "integer":

                        $buffer .= "(integer)$_value";
                        break;

                    case "double":

                        $buffer .= "(float)$_value";
                        break;

                    case "string":

                        $buffer .= "(string[" . strlen($_value) . "])";
                        if (preg_match("/^(.{50}).{4,}(.{50})$/su", $_value, $__value))
                            $_value = $__value[1] . " … " . $__value[2];
                        $_value  = strtr($_value,
                                         ["\0" => "\\0",
                                          "\n" => "\\n", "\r" => "\\r", "\t" => "\\t",
                                          "\v" => "\\v", "\e" => "\\e", "\f" => "\\f"]);
                        $_value  = preg_replace_callback("/[\p{C}]/u",
                                                         function ($match) { return sprintf("\\x%02X", ord($match[0])); },
                                                         $_value);
                        $buffer  .= "\"$_value\"";
                        break;

                    case "array":

                        $buffer .= "(array[" . count($_value) . "])";
                        break;

                    case "object":

                        $buffer .= "(object)" . get_class($_value);
                        break;

                    case "resource":

                        $buffer .= "(resource)" . get_resource_type($_value);
                        break;

                    case "NULL":

                        $buffer .= "NULL";
                        break;

                    default:

                        $buffer .= "($type)";
                        break;
                    };

                    $buffer .= "\n";
                };
            };

            $buffer .= "    )\n";
        };
    };

    //store error message for later output
    $array[] = "console." . $_array[$code][2] . "(" .
               json_encode("%c " . CMS_SOFTWARE . " %c\n\n" .
                           "%c" . $_array[$code][0] . "%c ($count): " .
                           $buffer) . "," .
               "\"BACKGROUND:#ffffff;COLOR:#000000;FONT-SIZE:larger\",\"\"," .
               "\"COLOR:" . $_array[$code][1] . ";FONT-WEIGHT:bold\",\"\");";

    //prepend to log
    if (
         defined("CMS_DATA_PATH")
         &&
         function_exists(__NAMESPACE__ . "\\mkpath")
         &&
         mkpath(CMS_DATA_PATH . "#log")
         &&
         (($hfile = fopen(CMS_DATA_PATH . "#log/error.lock", "c")) !== FALSE)
       )
    {
        //wait for exclusive lock
        flock($hfile, LOCK_EX);

        $temp = uniqid() . ".tmp";

        //create temporary file
        if (($_hfile = fopen(CMS_DATA_PATH . "#log/$temp", "wb")) !== FALSE)
        {
            flock($_hfile, LOCK_EX);

            //write new message
            fwrite($_hfile,
                   "==== " . gmdate("Y-m-d H:i:s \U\T\C") . " " . str_repeat("=", 73) . "\n\n" .
                   $_array[$code][0] . " ($count): " .
                   "$buffer\n");

            //append 500 kb lines from existing log
            if (
                 is_file(CMS_DATA_PATH . "#log/error.txt")
                 &&
                 (($__hfile = fopen(CMS_DATA_PATH . "#log/error.txt", "rb")) !== FALSE)
               )
            {
                stream_copy_to_stream($__hfile, $_hfile, 512000);
                fclose($__hfile);
            };

            fclose($_hfile);

            //swap
            if (! rename(CMS_DATA_PATH . "#log/$temp", CMS_DATA_PATH . "#log/error.txt"))
                unlink(CMS_DATA_PATH . "#log/$temp");
        };

        //release lock
        flock($hfile, LOCK_UN);
        fclose($hfile);
    };

    return TRUE;
};

//--------------------------------------------------------------------------------------------------

function cms_error_silent($flag = TRUE)
{
    return cms_cache("cms.error.silent", (bool)$flag);
};

//--------------------------------------------------------------------------------------------------

function cms_shutdown()
{
    /*
      capture fatal errors and output error buffer on termination
    */

    $error = error_get_last();
    if (($error !== NULL) && ($error["type"] & (E_ERROR | E_PARSE | E_CORE_ERROR | E_COMPILE_ERROR)))
        cms_error($error["type"], $error["message"], $error["file"], $error["line"]);
    cms_error(-1, "");
};

set_error_handler(__NAMESPACE__ . "\\cms_error");
register_shutdown_function(__NAMESPACE__ . "\\cms_shutdown");

//==================================================================================================
//   INPUT PREPROCESSING
//==================================================================================================

function cms_initialize_globals()
{
    /*
      sanitize and load superglobal data into global scope
    */

    if (defined("CMS_INITIALIZE_GLOBALS_EXECUTED")) exit();
    define("CMS_INITIALIZE_GLOBALS_EXECUTED", TRUE);

    foreach ($_COOKIE AS $key => $value)
        $GLOBALS[$key] = cms_utf8_normalize($value);

    foreach ($_GET AS $key => $value)
        $GLOBALS[$key] = cms_utf8_normalize($value);

    foreach ($_POST AS $key => $value)
        $GLOBALS[$key] = cms_utf8_normalize($value);

    foreach ($_FILES AS $key => $value)
    {
        foreach ($value AS $_key => $_value)
        {
            $__key           = $key . (($_key !== "tmp_name") ? "_$_key" : NULL);
            $GLOBALS[$__key] = cms_utf8_normalize($_value);
        };
    };

    //session
    /*
    if (isset($GLOBALS[session_name()]))
        session_id($GLOBALS[session_name()]);
    */
};

//--------------------------------------------------------------------------------------------------

function cms_utf8_normalize($value)
{
    /*
      normalize line breaks and repair invalid utf-8
    */

    return is_array($value) ?
           array_map(__NAMESPACE__ . "\\cms_utf8_normalize", $value) :
           utf8_normalize(preg_replace("/\r\n?/", "\n", $value));
};

//==================================================================================================
//   IDENTIFICATION
//==================================================================================================

function cms_identification()
{
    /*
      handle user authentication, logout, account expiration, csrf and brute-force attack protection
    */

    //security check
    if (defined("CMS_USER"))      exit();
    if (defined("CMS_PASSWORD"))  exit();
    if (defined("CMS_TOKEN"))     exit();
    if (defined("CMS_SUPERUSER")) exit();
    if (defined("CMS_NAME"))      exit();
    if (defined("CMS_EMAIL"))     exit();
    if (defined("CMS_TIMEZONE"))  exit();
    if (defined("CMS_PROFILE"))   exit();

    global $cms_user,
           $cms_password,
           $cms_logout;

    //initialize csrf protection variables
    $security_prefix = "identification.security.";
    $security_key    = (string)($_COOKIE["cms_security_key"] ?? "");
    $security_token  = NULL;

    //logout
    if ($cms_logout)
    {
        //logout is always allowed, even without security token
        //this may be a csrf target, but without prize
        //users should always be able to log out

        //delete cookies
        cms_set_cookie(["cms_user"         => NULL,
                        "cms_password"     => NULL,
                        "cms_security_key" => NULL]);

        //delete security token
        if ($security_key !== "")
            cms_cache($security_prefix . $security_key, "", TRUE);

        header("Location: " . CMS_ROOT_URL, TRUE, 303);
        exit();
    };

    //random string used for password obfuscation
    if (($salt = cms_cache("cms.salt_password")) === NULL)
    {
        $salt = random_bytes(32);
        cms_cache("cms.salt_password", $salt, TRUE);
    };

    $permission = new permission();
    $login_try  = FALSE;

    //login attempt limit
    $attempt_key   = "identification.attempt." . ($_SERVER["REMOTE_ADDR"] ?? "0.0.0.0");
    $attempt_count = (int)cms_cache("$attempt_key.count");

    //attempt present
    if ($attempt_count)
    {
        $attempt_time = cms_cache("$attempt_key.time");

        //attempt time expired
        if ((time() - $attempt_time) > CMS_LOGIN_BLOCK_TIME)
        {
            //delete limit data
            cms_cache("$attempt_key.count", "", TRUE);
            cms_cache("$attempt_key.time", "", TRUE);

            $attempt_count = 0;
        };
    };

    //daemon
    if (CMS_APPLICATION === "daemon")
    {
        $cms_user     = "daemon";
        $cms_password = hash64($salt . $permission->data->get("user.daemon", "password"));
    }

    //login
    elseif (($_SERVER["REQUEST_METHOD"] === "POST") && isset($_POST["cms_login_user"]))
    {
        //blocked due to too many failed attempts
        if ($attempt_count >= CMS_LOGIN_ATTEMPT_MAX)
        {
            //reset login data
            $cms_login_user     = "anonymous";
            $cms_login_password = "";
        }

        //allowed to log in
        else
        {
            //get login credentials
            $cms_login_user     = (string)$_POST["cms_login_user"];
            $cms_login_password = (string)($_POST["cms_login_password"] ?? "");
            $cms_login_noscript = (bool)($_POST["cms_login_noscript"] ?? FALSE);

            //hash password
            if (($cms_login_password !== "") && $cms_login_noscript)
                $cms_login_password = hash64($salt . hash64($cms_login_password));
        };

        //set credentials
        $cms_user     = $cms_login_user;
        $cms_password = $cms_login_password;
        $login_try    = (($cms_login_user !== "anonymous") && ($cms_login_password !== ""));
    }

    //default user
    elseif ((string)$cms_user === "")
    {
        $cms_user     = "anonymous";
        $cms_password = "";
    };

//..................................................................................................

    //verify user
    if (($array = $permission->verify_user($cms_user, $cms_password)) !== FALSE)
    {
        //cross-site request forgery (csrf) protection
        $security_token = ($security_key !== "") ?
                          cms_cache($security_prefix . $security_key) :
                          NULL;

        if (
             ($cms_user !== "anonymous")
             &&
             ($cms_user !== "daemon")
             &&
             (strpos("|" . CMS_PERMISSION_ALWAYS . "|", "|" . CMS_APPLICATION . "|") === FALSE)
           )
        {
            //generate new security token
            if ($login_try)
            {
                $security_key   = unique_id();
                $security_token = unique_id(4);
                cms_cache($security_prefix . $security_key, $security_token, TRUE);
            }

            //verify security token
            elseif (($security_token === NULL) || (! isset($_GET[$security_token])))
            {
                //log access without security token
                (new log())->access("token_mismatch", $login_try);

                //redirect to security message
                $query = (string)getenv("QUERY_STRING");
                $url   = CMS_ACTIVE_URL . (($query !== "") ? "?". $query : "");
                header("Location: " . CMS_MODULES_URL . "security.php" .
                       cms_param(["event"    => CMS_SECURITY_EVENT_CSRF,
                                  "location" => $url],
                                 NULL, TRUE),
                       TRUE, 303);
                exit();
            };
        };

        //set verified user data
        $superuser = $array["superuser"];
        $name      = $array["name"];
        $email     = $permission->data->get("user.$superuser", "email");
        $timezone  = $permission->data->get("user.$superuser", "timezone");
        $profile   = $array["profile"];
    }

    //unverified user
    else
    {
        //failed login attempt
        if ($login_try)
        {
            //set attempt data
            $attempt_count = min($attempt_count, CMS_LOGIN_ATTEMPT_MAX) + 1;
            cms_cache("$attempt_key.count", $attempt_count, TRUE);
            cms_cache("$attempt_key.time", time(), TRUE);

            //log suspension of user
            if ($attempt_count === CMS_LOGIN_ATTEMPT_MAX)
                (new log())->access("suspended", $login_try);
        };

        //set basic user data
        $cms_user     = "anonymous";
        $cms_password = "";
        $superuser    = $cms_user;
        $name         = $permission->data->get("user.$cms_user", "name");
        $email        = $permission->data->get("user.$cms_user", "email");
        $timezone     = $permission->data->get("user.$cms_user", "timezone");
        $profile      = "";
    };

    //set timezone
    if (! in_array($timezone, timezone_identifiers_list()))
        $timezone = CMS_TIMEZONE_DEFAULT;
    date_default_timezone_set($timezone);

    //set constants
    define("CMS_USER",      $cms_user);
    define("CMS_PASSWORD",  $cms_password);
    define("CMS_TOKEN",     $security_token);
    define("CMS_SUPERUSER", $superuser);
    define("CMS_NAME",      $name);
    define("CMS_EMAIL",     $email);
    define("CMS_TIMEZONE",  $timezone);
    define("CMS_PROFILE",   $profile);

    //verify permission
    if (! $permission->given())
    {
        //log failed access attempt
        if ($login_try) (new log)->access("unauthorized", CMS_USER);

        //redirect to identification prompt
        $query = (string)getenv("QUERY_STRING");
        $url   = CMS_ACTIVE_URL . (($query !== "") ? "?". $query : "");
        header("Location: " . CMS_MODULES_URL . "identification.php" .
               cms_param(["location" => $url]),
               TRUE, 302);
        exit();
    };

//..................................................................................................

    //at this point we are sure the user is valid
    //and permitted to access the current application

    //set credentials
    if (CMS_USER !== "anonymous")
        cms_set_cookie(["cms_user"         => CMS_USER,
                        "cms_password"     => CMS_PASSWORD,
                        "cms_security_key" => $security_key]);

    //on initial login
    if ($login_try)
    {
        //clear interface page
        cms_set_cookie(["cms_ifc_page" => NULL]);

        //log successful identification
        (new log)->access("identified", CMS_USER);

        //redirect administrator to setup
        if (CMS_USER === "admin")
        {
            $system = new system();
            if ($system->getval("setup", "done") === NULL)
            {
                header("Location: " . CMS_MODULES_URL . "interface.php" .
                       cms_param(["ifc_page" => "setup"]),
                       TRUE, 302);
                exit();
            };
        }

        //set account expiration if applicable
        elseif (
                 (CMS_USER !== "anonymous")
                 &&
                 (($expire = (int)$permission->data->get("user." . CMS_USER, "expire")) > 0)
               )
        {
            $permission->data->set(time() + $expire, "user." . CMS_USER, "#expire");
            $permission->save();
        };
    };
};

//--------------------------------------------------------------------------------------------------

function cms_generate_id()
{
    /*
      generate anonymous fingerprint and ip hashes of current client
    */

    if (defined("CMS_USERID")) exit();
    if (defined("CMS_IPHASH")) exit();

    //generate random string used for user id obfuscation
    if (cms_cache("cms.salt_id") === NULL)
        cms_cache("cms.salt_id", uniqid(), TRUE);

    $salt = cms_cache("cms.salt_id");
    $id   = $salt .
            ($_SERVER["HTTP_ACCEPT_CHARSET"]  ?? "") .
            ($_SERVER["HTTP_ACCEPT_ENCODING"] ?? "") .
            ($_SERVER["HTTP_ACCEPT_LANGUAGE"] ?? "") .
            ($_SERVER["HTTP_USER_AGENT"]      ?? "") .
            ($_SERVER["HTTP_X_FORWARDED_FOR"] ?? "") .
            ($_SERVER["REMOTE_ADDR"]          ?? "") .
            ($_SERVER["REMOTE_IDENT"]         ?? "") .
            ($_SERVER["REMOTE_USER"]          ?? "");
    define("CMS_USERID", hash("ripemd128", $id));

    $id   = $salt . ($_SERVER["REMOTE_ADDR"]  ?? "");
    define("CMS_IPHASH", hash("ripemd128", $id));
};

//==================================================================================================
//   LANGUAGE
//==================================================================================================

function cms_language()
{
    /*
      inilialize language settings
    */

    if (defined("CMS_LANGUAGE_ENABLED")) exit();
    if (defined("CMS_LANGUAGE_DEFAULT")) exit();
    if (defined("CMS_LANGUAGE"))         exit();

    global $cms_select_language,
           $cms_language;

    $system   = new system();
    $language = strtolower($system->getval("language", "default"));

    define("CMS_LANGUAGE_ENABLED", $language);
    define("CMS_LANGUAGE_DEFAULT", substr(CMS_LANGUAGE_ENABLED, 0, strcspn(CMS_LANGUAGE_ENABLED, ",")));

    //retrieve selected language
    if ((string)$cms_language === "") $cms_language = cms_cache("language." . CMS_USERID);

    //test validity
    if (strpos(CMS_LANGUAGE_ENABLED . ",", "$cms_select_language,") === FALSE) $cms_select_language = "";
    if (strpos(CMS_LANGUAGE_ENABLED . ",", "$cms_language,")        === FALSE) $cms_language        = "";

    //new language requested by user
    if ((string)$cms_select_language !== "")
    {
        $cms_language = $cms_select_language;
    }

    //no language set
    elseif ((string)$cms_language === "")
    {
        $cms_language = cms_language_extract(getenv("HTTP_ACCEPT_LANGUAGE"), CMS_LANGUAGE_ENABLED);
        if ($cms_language === NULL) $cms_language = CMS_LANGUAGE_DEFAULT;
    };

    define("CMS_LANGUAGE", $cms_language);

    //store selected language
    cms_cache("language." . CMS_USERID, CMS_LANGUAGE, TRUE);

    if (CMS_LANGUAGE !== "") header("Content-Language: " . CMS_LANGUAGE);

    require(((CMS_LANGUAGE !== "") && is_file(CMS_PATH . "#language/" . CMS_LANGUAGE . ".language.inc")) ?
            CMS_PATH . "#language/" . CMS_LANGUAGE . ".language.inc" :
            CMS_PATH . "#language/language.inc");
};

//--------------------------------------------------------------------------------------------------

function cms_language_extract($requested,
                              $supported)
{
    /*
      find best match of RFC 9110 / BCP 47 language string in supported languages
    */

    $requested = cms_language_parse($requested);
    $supported = array_map("trim", explode(",", strtolower($supported)));
    $result    = NULL;
    $priority  = 0;
    $specifity = 0;
    $c         = 0;

    foreach ($requested AS $key => $value)
    {
        $_requested = explode("-", $key);
        $_priority  = $value["q"];
        $_c         = count($_requested);

        foreach ($supported AS $value)
        {
            $_supported = explode("-", $value);
            $__c        = count($_supported);

            if (

            //equal length (exact match possible)
                 ($_c === $__c)
                 &&

            //higher specifity
                 ($_c > $c)
                 &&

            //exact match
                 ($_requested === $_supported)
               )
            {
                $result    = $value;
                $priority  = $_priority;
                $specifity = $c = $_c;
                break;
            };

            for ($i = min($_c, $__c); $i > $specifity; $i--) //specifity must be higher
            {
                if (array_slice($_requested, 0, $i) === array_slice($_supported, 0, $i))
                {
                    $result    = $value;
                    $priority  = $_priority;
                    $specifity = $i;
                    break;
                };
            };
        };
    };

    return $result;
};

//--------------------------------------------------------------------------------------------------

function cms_language_parse($string)
{
    /*
      f($string = (string))
      => (array) parse RFC 9110 / BCP 47 language string into components

      --- COMPONENT MAPPING ---
      code    => ISO 639 language code (2-3 chars)
      script  => ISO 15924 script subtag (4 chars)
      region  => ISO 3166-1 alpha-2 or UN M.49 code (2 chars / 3 digits)
      variant => registered language variants
      q       => priority value (0.0 to 1.0)
    */

    $string = strtolower(preg_replace("/\s+/", "", $string));
    $list   = [];

    preg_match_all(
        "/(?:^|,)" .                                   //entry start
        "(" .                                          //full [1]
        "([a-z]{2,3})" .                               //code [2]
        "(?:-([a-z]{4}))?" .                           //script [3]
        "(?:-([a-z]{2}|[0-9]{3}))?" .                  //region [4]
        "(?:-([a-z0-9]{1,8}(?:-[a-z0-9]{1,8})*))?)" .  //variant [5]
        "(;q=)?(0(?:\.[0-9]{1,3})?|1(?:\.0{1,3})?)?" . //priority [6][7]
        "(?=,|$)/i",                                   //entry end
        $string,
        $string,
        PREG_SET_ORDER | PREG_UNMATCHED_AS_NULL);

    foreach ($string as $value)
    {
        $q = 1.0;
        if (($value[6] !== NULL) && ($value[7] !== NULL))
        {
            $q = (float)$value[7];
            if ($q === 0.0) continue;
        };

        $key = $value[1];
        if (isset($list[$key]) && ($q <= $list[$key]["q"])) continue;

        $list[$key] = [
            "code"    => $value[2],
            "script"  => $value[3] ?? "",
            "region"  => $value[4] ?? "",
            "variant" => $value[5] ?? "",
            "q"       => $q];
    };

    uasort($list, function($a, $b) { return $b["q"] <=> $a["q"]; });
    return $list;
};

//==================================================================================================
//   LIBRARIES
//==================================================================================================

function cms_load($library,
                  $exit_on_error = FALSE,
                  $test          = FALSE)
{
    /*
      --- LIBRARY LOADING ---
      f($library = (string))
      => (bool) load library

      f($library = (string), $exit_on_error = TRUE)
      => (bool) load library, terminate with error message on fail

      --- LIBRARY AVAILABILITY ---
      f($library = (string), $exit_on_error = FALSE, $test = TRUE)
      => (bool) test for availability of library
    */

    static $loaded = [];

    if (! isset($loaded[$library]))
    {
        $_library = CMS_SYSTEM_PATH . "lib.$library.inc";

        if (! is_file($_library))
        {
            if ($exit_on_error)

                //failed loading
                die("Library '$_library' cannot be opened.");

            //store fail state
            $loaded[$library] = FALSE;
        }
        else
        {
            if (! $test)
            {
                //load library
                (function($file) { require($file); })($_library);

                //store load stated
                $loaded[$library] = TRUE;
            };

            //library is available
            return TRUE;
        };
    };

    return $loaded[$library];
};

//--------------------------------------------------------------------------------------------------

function cms_available($library)
{
    /*
      => (bool) check if library is available
    */

    return cms_load($library, FALSE, TRUE);
};

//--------------------------------------------------------------------------------------------------

function cms_load_system()
{
    /*
      load system libraries
    */

    if (defined("CMS_LOAD_SYSTEM_EXECUTED")) exit();
    define("CMS_LOAD_SYSTEM_EXECUTED", TRUE);

    if (($hdir = opendir(CMS_SYSTEM_PATH)) === FALSE)
        die("Path '" . CMS_SYSTEM_PATH . "' cannot be opened.");

    while ($file_object = readdir($hdir))
    {
        if (
             is_file(CMS_SYSTEM_PATH . $file_object)
             &&
             preg_match("/^sys\.([^\.]+)\.inc$/", $file_object)
           )
            (function($file) { require($file); })(CMS_SYSTEM_PATH . $file_object);
    };

    closedir($hdir);
};

//==================================================================================================
//   APPLICATIONS
//==================================================================================================

function cms_application($application = NULL,
                         $instance    = NULL,
                         $permission  = NULL,
                         $user        = NULL)
{
    /*
       --- APPLICATION LOADING ---
       f($application = (string))
       => load application

       f($application = (string), $instance = (string))
       => load application using instance

       --- GENERAL PERMISSION CHECKS ---
       f($application = NULL, $instance = NULL, $permission = (string))
       => (bool) test permission for current user

       f($application = NULL, $instance = NULL, $permission = (string), $user = (string))
       => (bool) test permission for specified user

       --- APPLICATION AND INSTANCE SPECIFIC PERMISSION CHECKS ---
       f($application = TRUE, $instance = TRUE, $permission = (string))
       => (bool) test specified permission for current application.instance for current user

       f($application = TRUE, $instance = TRUE, $permission = (string), $user = (string))
       => (bool) test specified permission for current application.instance for specified user

       f($application = TRUE, $instance = NULL, $permission = (string))
       => (bool) test specified permission for application.permission for current user

       f($application = TRUE, $instance = NULL, $permission = (string), $user = (string))
       => (bool) test specified permission for application.permission for specified user

       f($application = NULL, $instance = TRUE, $permission = (string))
       => (bool) test permission for instance.permission for current user

       f($application = NULL, $instance = TRUE, $permission = (string), $user = (string))
       => (bool) test permission for instance.permission for specified user

       --- CONTEXT INFORMATION ---
       f()
       => (string) return current application.instance

       f($application = TRUE)
       => (string) return current application

       f($application = NULL, $instance = TRUE)
       => (string) return current instance
    */

    static $_application         = CMS_APPLICATION,
           $_instance            = CMS_INSTANCE,
           $application_instance = "",
           $_permission          = NULL,
           $initialize           = TRUE;

    if ($initialize)
    {
        $application_instance = CMS_APPLICATION . ((CMS_INSTANCE !== "") ? "." . CMS_INSTANCE : "");
        $_permission          = new permission();
        $initialize           = FALSE;
    };

    if ($application === TRUE)
    {
        if ($instance === TRUE)
        {
            //test permission
            if ((string)$permission !== "")
            {
                if ((string)$user !== "") return $_permission->test("$application_instance.$permission", $user);
                return $_permission->given("$application_instance.$permission");
            };

            //return information
            return $application_instance;
        };

        if ((string)$instance === "")
        {
            if ((string)$permission !== "")
            {
                //test permission
                if ((string)$user !== "") return $_permission->test("$_application.$permission", $user);
                return $_permission->given("$_application.$permission");
            };

            //return information
            return $_application;
        };
    }
    elseif ((string)$application === "")
    {
        if ($instance === TRUE)
        {
            if ((string)$permission !== "")
            {
                //test permission
                if ((string)$user !== "") return $_permission->test("$_instance.$permission", $user);
                return $_permission->given("$_instance.$permission");
            };

            //return information
            return $_instance;
        }
        elseif ((string)$instance === "")
        {
            if ((string)$permission !== "")
            {
                //test permission
                if ((string)$user !== "") return $_permission->test($permission, $user);
                return $_permission->given($permission);
            };

            //return information
            return $application_instance;
        };
    };

    //load module
    $file = CMS_MODULES_PATH . "#module/mod.$application.inc";
    if (is_file($file))
    {
        //save settings
        $__application         = $_application;
        $__instance            = $_instance;
        $_application_instance = $application_instance;

        //add application layer
        $_application         .= ".$application";
        $_instance             = preg_replace("/[^_a-z0-9]+/", "-", strtolower($instance));
        $application_instance  = $_application . (($_instance !== "") ? ".$_instance" : "");

        //load application
        if ($_permission->given($application_instance))
            (function($file) { require($file); })($file);

        //restore settings
        $_application          = $__application;
        $_instance             = $__instance;
        $application_instance  = $_application_instance;
    };
};

//--------------------------------------------------------------------------------------------------

function cms_instance()
{
    /*
      => (string) return current instance
    */

    return cms_application(NULL, TRUE);
};

//--------------------------------------------------------------------------------------------------

function cms_permission($permission,
                        $application = TRUE,
                        $instance    = TRUE,
                        $user        = NULL)
{
    /*
      --- APPLICATION INSTANCE PERMISSION CHECKS ---
      f($permission = (string))
      => (bool) test application.instance.permission for current user

      f($permission = (string), $application = TRUE, $instance = TRUE, $user = (string))
      => (bool) test application.instance.permission for specified user

      --- APPLICATION PERMISSION CHECKS ---
      f($permission = (string), $application = TRUE)
      => (bool) test application.permission for current user

      f($permission = (string), $application = TRUE, $instance = NULL, $user = (string))
      => (bool) test application.permission for specified user

      --- INSTANCE PERMISSION CHECKS ---
      f($permission = (string), $application = NULL, $instance = TRUE)
      => (bool) test instance.permission for current user

      f($permission = (string), $application = NULL, $instance = TRUE, $user = (string))
      => (bool) test instance.permission for specified user

      --- PERMISSION CHECKS ---
      f($permission = (string), $application = NULL, $instance = NULL)
      => (bool) test permission for current user

      f($permission = (string), $application = NULL, $instance = NULL, $user = (string))
      => (bool) test permission for specified user
    */

    return cms_application((bool)$application, (bool)$instance, $permission, $user);
};

//==================================================================================================
//   URL FUNCTIONS
//==================================================================================================

function cms_url($address    = NULL,
                 $param      = NULL,
                 $omit_param = FALSE)
{
    /*
       --- URL GENERATION ---
       f()
       => (string) generate url for current address with stored values

       f($address = (array))
       => (string) generate url for current address adding/overwriting values

       f($address = (string))
       => (string) generate url for specified address with stored values

       f($address = (string), $param = (array))
       => (string) generate url for specified address adding/overwriting values

       f($address = (mixed), $param = (mixed), $omit_param)
       => (string) generate url omitting stored values
    */

    $external = NULL;

    if (is_array($address))
    {
        $param    = $address;
        $address  = CMS_ACTIVE_URL;
        $external = FALSE;
    }
    else
    {
        if (! is_array($param))          $param    = [];
        if (is_object($address))         $address  = (string)$address;
        elseif (! is_string($address)) { $address  = CMS_ACTIVE_URL;
                                         $external = FALSE; };
    };

    $address = parse_url($address);
    if ($address === FALSE) return FALSE;

    $external ??= isset($address["host"]) ?
                  (strcasecmp($address["host"], CMS_DOMAIN) !== 0) :
                  FALSE;

    //merge query of specified url with param array
    if (! $external)
    {
        //break down querystring
        if (nstre($address["query"] ?? ""))
            parse_str($address["query"], $_param);
        else
            $_param = [];

        //extend query parameters
        $param = is_array($param) ? array_replace_recursive($_param, $param) : $_param;
    };

    //build querystring
    $address["query"] = substr(cms_param($param, ($omit_param || $external) ?: NULL, $external), 1);

    return cms_build_url($address);
};

//--------------------------------------------------------------------------------------------------

function cms_param($value      = NULL,
                   $key        = NULL,
                   $omit_token = FALSE)
{
    /*
       --- QUERYSTRING GENERATION ---
       f()
       => (string) generate querystring with current values

       f($value = (array))
       => (string) generate querystring temporarily adding/overwriting values

       f($value = (array), $key = TRUE)
       => (string) generate querystring omitting stored values

       f($value = (array), $key = (mixed), $omit_token = TRUE)
       => (string) generate querystring without security token

       --- VALUE MANAGEMENT ---
       f($value = (mixed), $key = (string))
       => (bool) set value of key (accepts multi-dimensional array)

       f($value = FALSE, $key = (string))
       => (mixed) delete specified key and return its last value

       f($value = FALSE)
       => (bool) delete all stored values

       --- VALUE RETRIEVAL ---
       f($value = (mixed))
       => (mixed) return value of specified key ($value)

       f($value = TRUE)
       => (array) return all stored values
    */

    static $param = []; //stored values

    //key is set
    if (($key !== NULL) && ($key !== TRUE))
    {
        $key = (string)$key;

        //erase value
        if ($value === FALSE)
        {
            $value = $param[$key] ?? NULL;
            unset($param[$key]);
            return $value;
        };

        //store value
        $param[$key] = $value;
        return TRUE;
    };

    //return all stored values
    if ($value === TRUE) return $param;

    //erase all stored values
    if ($value === FALSE)
    {
        $param = [];
        return TRUE;
    };

    //return specified stored value
    if (($value !== NULL) && (! is_array($value)))
        return $param[(string)$value] ?? NULL;

    $value  = is_array($value) ? $value : [];
    if ($key !== TRUE) $value = array_replace_recursive($param, $value);
    $return = [];

    //recursively retrieve field names and values
    $array = [];
    $func  = function($value, $key = "") use (&$func, &$array)
    {
        $key = (string)$key;

        if (is_array($value))
        {
            ksort($value, SORT_NATURAL | SORT_FLAG_CASE);
            foreach ($value AS $_key => $_value)
                $func($_value, ($key === "") ? $_key : $key . "[$_key]");
            return;
        };

        $value = (string)$value;
        if ($value !== "") $array[$key] = $value;
    };

    $func($value);

    //construct querystring
    foreach ($array AS $key => $value)
    {
        $value    = (substr($value, 0, 1) === "\x1B") ? //esc
                    substr($value, 1) : rawurlencode($value);
        $return[] = rawurlencode($key) . "=" . $value;
    };

    //append session id
    /*
    if (session_status() === PHP_SESSION_ACTIVE)
    {
        $key = session_name();
        if (! isset($param[$key]))
        {
            $key     = rawurlencode($key);
            $value   = rawurlencode(session_id());
            $return .= (($return === "") ? "?" : "&") . "$key=$value";
        };
    };
    */

    //prepend csrf protection token
    if ((! $omit_token) && defined("CMS_TOKEN") && (CMS_TOKEN !== NULL))
        return "?" . rawurlencode(CMS_TOKEN) .
               (empty($return) ? "" : "&" . implode("&", $return));

    return empty($return) ? "" : "?" . implode("&", $return);
};

//--------------------------------------------------------------------------------------------------

function cms_build_url($parts)
{
    /*
      => (string) build a valid url from parts
    */

    $array = [];

    if (nstre($parts["host"] ?? ""))
    {
        if (nstre($parts["scheme"] ?? ""))
            $array[] = strtolower($parts["scheme"]) . ":";
        $array[]     = "//";
        if (nstre($parts["user"] ?? ""))
            $array[] = stre($parts["pass"] ?? "") ?
                       rawurlencode($parts["user"]) . "@" :
                       rawurlencode($parts["user"]) . ":" .
                       rawurlencode($parts["pass"]) . "@";
        $array[]     = strtolower($parts["host"]);
        if (nstre($parts["port"] ?? ""))
            $array[] = ":" . (int)$parts["port"];
        if (substr($parts["path"] ?? "", 0, 1) !== "/")
            $array[] = "/";
    };
    if (nstre($parts["path"]     ?? "")) $array[] = $parts["path"];
    if (nstre($parts["query"]    ?? "")) $array[] = "?" . $parts["query"];
    if (nstre($parts["fragment"] ?? "")) $array[] = "#" . rawurlencode($parts["fragment"]);

    return implode("", $array);
};

//==================================================================================================
//   CACHE DATA STORAGE
//==================================================================================================

function cms_cache($key       = NULL,
                   $value     = NULL,
                   $permanent = FALSE,
                   $notouch   = FALSE)
{
    /*
      --- VALUE MANAGEMENT ---
      f($key = (string), $value = (mixed))
      => (bool) store temporary value of key

      f($key = (string), $value = "")
      => (bool) delete temporary value of key

      f($key = (string), $value = "", $permanent = TRUE)
      => (bool) delete temporary and permanent value of key

      --- VALUE RETRIEVAL ---
      f($key = (string))
      => (mixed) return value (updating time)

      f($key = (string), $value = NULL, $permanent = FALSE, $notouch = TRUE)
      => (mixed) return value without updating time

      f()
      => (array) return all stored and previously retrieved values
    */

    static $cache = [];

    if ($key === NULL) return $cache;

    if (($value !== NULL) || ($permanent !== FALSE))
    {
        //delete
        if ($value === "")
        {
            if ($permanent)
            {
                $code = hash("ripemd128", $key);
                $path = CMS_DATA_PATH . "#cache/" . substr($code, 0, 4) . "/" . substr($code, 4);

                if (is_file("$path.ser")) unlink("$path.ser");
                if (is_file($path))       unlink($path);
            };

            unset($cache[$key]);
            return TRUE;
        };

        //store
        $cache[$key] = $value;
        $return      = TRUE;

        if ($permanent)
        {
            $code = hash("ripemd128", $key);
            $path = CMS_DATA_PATH . "#cache/" . substr($code, 0, 4) . "/";

            if (! mkpath($path)) return FALSE;

            $temp    = $path . uniqid() . ".tmp";
            $path   .= substr($code, 4);
            $flag    = is_string($value);
            $return  = FALSE;

            //serialize if not string
            if (! $flag) $value = serialize($value);

            if (file_put_contents($temp, $value, LOCK_EX) !== FALSE)
            {
                $return = TRUE;

                if (! rename($temp, $flag ? $path : "$path.ser"))
                {
                    //error, remove file
                    unlink($temp);
                    $return = FALSE;
                };
            };

            //remove other file
            if ($flag) $path = "$path.ser";
            if (is_file($path)) unlink($path);
        };

        return $return;
    };

    //retrieve
    if (array_key_exists($key, $cache)) return $cache[$key];

    $code = hash("ripemd128", $key);
    $path = CMS_DATA_PATH . "#cache/" . substr($code, 0, 4) . "/" . substr($code, 4);

    //check for serialized value first
    if (($value = read_file("$path.ser")) !== FALSE)
    {
        if ($notouch === FALSE) touch("$path.ser");

        $value = unserialize($value);
    }

    //check for string value
    elseif (($value = read_file($path)) !== FALSE)
    {
        if ($notouch === FALSE) touch($path);
    }

    //not found
    else
    {
        return NULL;
    };

    $cache[$key] = $value;
    return $value;
};

//--------------------------------------------------------------------------------------------------

function cms_cache_delete($key,
                          $permanent = TRUE)
{
    /*
      delete value of key from cache, accepts array of keys
    */

    if (! is_array($key)) return cms_cache($key, "", $permanent);

    $flag = TRUE;

    foreach ($key AS $value)
        $flag &= cms_cache($value, "", $permanent);

    return (bool)$flag;
};

//--------------------------------------------------------------------------------------------------

function cms_cache_sync(&$variable,
                        $key,
                        $default       = NULL,
                        $load_on_empty = FALSE)
{
    /*
      initialize undefined/empty variable with cache value or update cache
    */

    $flag = FALSE;

    if (! isset($variable))
    {
        $flag = TRUE;
    }
    elseif ($load_on_empty)
    {
        if (in_array(gettype($variable), ["boolean", "integer", "double", "string"]))
            $flag = (string)$variable === "";

        else
            $flag = empty($variable);
    };

    if ($flag)
    {
        $variable = cms_cache($key);
        if ($variable !== NULL) return;
    };

    if ($variable === NULL) $variable = $default;
    if ($variable !== NULL) cms_cache($key, $variable, TRUE);
};

//--------------------------------------------------------------------------------------------------

function cms_cache_init(&$variable,
                        $key,
                        $load_on_empty = FALSE)
{
    /*
      initialize undefined/empty variable with cache value
    */

    cms_cache_sync($variable, $key, NULL, $load_on_empty);
}

//--------------------------------------------------------------------------------------------------

function cms_cache_notouch($key)
{
    /*
      => (mixed) return value without updating time
    */

    return cms_cache($key, NULL, FALSE, TRUE);
};

//--------------------------------------------------------------------------------------------------

function cms_cache_time($key)
{
    /*
      return last time of key
    */

    $code = hash("ripemd128", $key);
    $path = CMS_DATA_PATH . "#cache/" . substr($code, 0, 4) . "/" . substr($code, 4);

    if (is_file("$path.ser")) return filemtime("$path.ser");
    if (is_file($path))       return filemtime($path);
    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function cms_cache_touch($key)
{
    /*
      update time of key
    */

    $code = hash("ripemd128", $key);
    $path = CMS_DATA_PATH . "#cache/" . substr($code, 0, 4) . "/" . substr($code, 4);

    if (is_file("$path.ser")) return touch("$path.ser");
    if (is_file($path))       return touch($path);
    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function cms_cache_clean($path,
                         $force = FALSE)
{
    /*
      remove expired cache files
    */

    if (! is_dir($path)) return TRUE; //nothing to remove

    $depth        = 0;
    $hdir[$depth] = opendir($path);
    if ($hdir[$depth] === FALSE) return FALSE;
    $path         = [$depth => $path];
    $flag[$depth] = FALSE;
    $time         = time();

    do
    {
        while (($file_object = readdir($hdir[$depth])) !== FALSE)
        {
            if (in_array($file_object, [".", "..", ".htaccess"], TRUE)) continue;

            //directory
            if (is_dir($path[$depth] . $file_object))
            {
                $depth++;
                $path[$depth] = $path[$depth - 1] . "$file_object/";
                $hdir[$depth] = opendir($path[$depth]);
                if ($hdir[$depth] === FALSE) $depth--;
                else                         $flag[$depth] = TRUE;
                continue;
            };

            //file
            $_flag = $force || (($time - fileatime($path[$depth] . $file_object)) > CMS_CACHE_TTL);
            if ($_flag) unlink($path[$depth] . $file_object);
            else        $flag[$depth] = FALSE;
        };

        if ($hdir[$depth] !== FALSE) closedir($hdir[$depth]);
        if ($flag[$depth])           rmdir($path[$depth]);
        if ($depth > 1)              $flag[$depth - 1] &= $flag[$depth];
        $depth--;
    }
    while ($depth >= 0);

    return TRUE;
};

//==================================================================================================
//   DAEMON
//==================================================================================================

function cms_daemon($code,
                    $id       = NULL,
                    $interval = 0,
                    $status   = "")
{
    /*
      f($code = (string), $id = (string), $interval = (int))
      => (bool) add background task

      - provided id prevents duplicate queueing
      - interval prevents requeueing after last execution
    */

    $path = CMS_DATA_PATH . "#daemon/";
    if (! mkpath($path)) return FALSE;

    $temp  = $path . uniqid() . ".tmp";
    $hash  = hash("ripemd128", ((string)$id === "") ? $code : $id);
    $_path = $path . $hash;

    if (is_file($_path))
    {
        //check if queued
        if (filesize($_path) > 0) return FALSE;

        //check interval
        if (
             ($interval > 0)
             &&
             ((time() - filemtime($_path)) < $interval)
           )
            return FALSE;
    };

    //write task code
    if ((string)$status === "")
        $status = ((string)$id === "") ? $hash : $id;

    $code = "<?php\n" .
            "namespace cms;\n" .
            "cms_daemon_status(\"Task started: " . addslashes($status) . " …\");\n" .
            $code;

    if (file_put_contents($temp, $code, LOCK_EX) !== FALSE)
    {
        if (! rename($temp, $_path))
        {
            //error, remove file
            unlink($temp);
            return FALSE;
        };
    };

    if (! $interval) touch($_path, 1);

    //create task available flag
    file_put_contents($path . "daemon.flag", "");

    return TRUE;
};

//--------------------------------------------------------------------------------------------------

function cms_daemon_status($value = NULL)
{
    /*
      sets or returns background worker status
    */

    $path = CMS_DATA_PATH . "#daemon/";
    if (! mkpath($path)) return FALSE;

    $temp   = $path . uniqid() . ".tmp";
    $path  .= "daemon.status";
    $count  = 25;

    //return current status
    if ((string)$value === "") return is_file($path) ? file_get_contents($path) : "";

    $array = [];

    if (is_file($path) && ($hfile = fopen($path, "rb")))
    {
        $buffer = "";
        $length = 4096;
        $_count = 0;

        fseek($hfile, 0, SEEK_END);
        $offset = ftell($hfile);

        while (($offset > 0) && ($_count < $count))
        {
            $_length  = min($offset, $length);
            $offset  -= $_length;

            fseek($hfile, $offset);
            $_buffer = fread($hfile, $_length);

            $_count += substr_count($_buffer, "\n");
            $buffer  = $_buffer . $buffer;
        };

        fclose($hfile);
        $array = explode("\n", $buffer);
    };

    $array[] = gmdate("Y-m-d H:i:s \U\T\C") . " — " . preg_replace("/[\r\n?]+/", " — ", $value);
    $buffer  = implode("\n", array_slice($array, -$count));

    if (file_put_contents($temp, $buffer, LOCK_EX) !== FALSE)
    {
        if (! rename($temp, $path))
        {
            //error, remove file
            unlink($temp);
            return FALSE;
        };
    };

    return TRUE;
};

//--------------------------------------------------------------------------------------------------

function cms_daemon_exists($id)
{
    /*
      => (bool) check if task id is queued
    */

    $path = CMS_DATA_PATH . "#daemon/" . hash("ripemd128", $id);
    return is_file($path) && (filesize($path) > 0);
};

//--------------------------------------------------------------------------------------------------

function cms_daemon_run()
{
    /*
      start discrete asynchronous background worker
    */

    $path = CMS_DATA_PATH . "#daemon/";

    //check task available flag
    $_path = $path . "daemon.flag";
    if (! is_file($_path)) return FALSE; //nothing to do

    //try advisory lock
    $_path = $path . "daemon.lock";
    $hfile = fopen($_path, "c");
    if ($hfile === FALSE) return FALSE;

    if (! flock($hfile, LOCK_EX | LOCK_NB))
    {
        fclose($hfile);
        return FALSE; //daemon already running
    };

    //release lock
    flock($hfile, LOCK_UN);
    fclose($hfile);

    //configure http call
    $context = stream_context_create(
        ["http" => ["timeout"     => 1,
                    "header"      => "User-Agent: " . CMS_USER_AGENT . "\r\n" .
                                     "Connection: close\r\n"],
         "ssl"  => ["verify_peer" => FALSE]]);

    //temporarily override error handling
    $error = set_error_handler(function() { return TRUE; });

    //invoke external daemon worker
    \file_get_contents(CMS_MODULES_URL . "daemon.php", FALSE, $context);

    //restore error handling
    set_error_handler($error);

    return TRUE;
};

//==================================================================================================
//   FLAG
//==================================================================================================

function cms_flag_set($key,
                      $mode = 0)
{
    static $hfile = [];

    $path = CMS_DATA_PATH . "#flag/";
    if (! mkpath($path)) return FALSE;

    $hash  = hash("ripemd128", $key);
    $path .= $hash;

    //set
    if ($mode === 0) return file_put_contents($path, "", LOCK_EX) !== FALSE;

    $_hfile = $hfile[$hash] ?? NULL;
    if (! isset($_hfile))
    {
        if (! is_file($path))                        return FALSE;
        if (($_hfile = fopen($path, "r")) === FALSE) return FALSE;
        if (! flock($_hfile, LOCK_EX | LOCK_NB))     { fclose($_hfile); return FALSE; };

        clearstatcache(TRUE, $path);
        if (! is_file($path)) { fclose($_hfile); return FALSE; };

        $hfile[$hash] = $_hfile;
    };

    //get
    if ($mode === 1) return TRUE;

    //del
    if ($mode === 2)
    {
        unlink($path);
        fclose($_hfile);
        unset($hfile[$hash]);
        return TRUE;
    };

    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function cms_flag_get($key)
{
    return cms_flag_set($key, 1);
};

//--------------------------------------------------------------------------------------------------

function cms_flag_del($key)
{
    return cms_flag_set($key, 2);
};

//==================================================================================================
//   MISC
//==================================================================================================

function cms_set_cookie($array)
{
    /*
      store or delete session cookies
    */

    $time   = time() + 3600;
    $option = ["httponly" => TRUE,
               "path"     => CMS_RELATIVE_URL,
               "samesite" => "Strict",
               "secure"   => CMS_PROTOCOL === "https"];

    foreach ($array AS $key => $value)
    {
        $value = (string)$value;
        $option["expires"] = ($value === "") ? 1 : $time;
        if (! setcookie($key, $value, $option)) return FALSE;
    };

    return TRUE;
};

//--------------------------------------------------------------------------------------------------

function cms_email_agent()
{
    /*
      initialize system email
    */

    if (defined("CMS_EMAIL_AGENT")) exit();

    $system = new system();
    if ((string)($address = $system->getval("email", "address")) === "")
        $address = "mailagent@" . preg_replace("/^www\./i", "", CMS_DOMAIN);
    define("CMS_EMAIL_AGENT", $address);
};

//--------------------------------------------------------------------------------------------------

function cms_path_urlencode($string)
{
    /*
      => (string) encode non-alphanumerical characters except $-_.+!*'(), according to RFC 1738

      - excludes path separator /
      - makes file paths safe
    */

    return preg_replace_callback(
        "/[^0-9A-Za-z\\x21\\x24\\x27-\\x2F\\x5F]/",
        function($value) { return "%" . bin2hex($value[0]); },
        $string);
};

$application = get_required_files();
if (count($application) <= 1)
    die("pwnc.inc may not be executed directly.");

$php_self     = $_SERVER["SCRIPT_NAME"];
$http_host    = getenv("HTTP_HOST");

//base url
$base_url     = reset($application);
$base_url     = substr($base_url, 0, -strlen($php_self));
$base_url     = dirname(substr(__FILE__, strlen($base_url)));
$base_url     = str_replace("\\", "/", $base_url) . "/";
$base_url     = cms_path_urlencode($base_url);

$relative_url = dirname($base_url) . "/";
$relative_url = str_replace("\\", "/", $relative_url);
$relative_url = str_replace("//", "/", $relative_url);
$php_self     = cms_path_urlencode($php_self);

//application
end($application);
$application  = prev($application);
$application  = strtolower(pathinfo($application, PATHINFO_FILENAME));

//instance
$instance     = "";

//interface
if ($application === "interface")
{
    if (
         isset($_REQUEST["ifc_message"])
         &&
         ($_REQUEST["ifc_message"] === "ifc_select_page")
         &&
         isset($_REQUEST["ifc_param"])
       )
        $instance = (string)$_REQUEST["ifc_param"];

    elseif (isset($_REQUEST["ifc_page"]))
        $instance = (string)$_REQUEST["ifc_page"];

    else
        $instance = $_COOKIE["cms_ifc_page"] ?? "";
};

//instance
if ($instance !== "") $instance = preg_replace("/[^_a-z0-9]+/", "-", strtolower($instance));

//==================================================================================================
//   SYSTEM CONSTANTS
//==================================================================================================

// --- SYSTEM ---
define("CMS_APPLICATION",         $application);
define("CMS_INSTANCE",            $instance);
define("CMS_DB_PREFIX",           "cms_");
define("CMS_IFC_EDITION",         "ifc");

// --- PATHS ---
define("CMS_PATH",                str_replace("\\", "/", dirname(__FILE__)) . "/");
define("CMS_SYSTEM_PATH",         CMS_PATH . "#system/");
define("CMS_MODULES_PATH",        CMS_PATH . "module/");
define("CMS_INTERFACE_PATH",      CMS_MODULES_PATH  . "#interface/");
define("CMS_DESKTOP_PATH",        CMS_MODULES_PATH . "#desktop/");
define("CMS_IMAGES_PATH",         CMS_PATH . "image/");
define("CMS_ROOT_PATH",           dirname(CMS_PATH) . "/");
define("CMS_DATA_PATH",           CMS_ROOT_PATH . "data/");

// --- URLS ---
define("CMS_PROTOCOL",            ((! preg_match("/^(?:|off)$/i", getenv("HTTPS"))) ||
                                  (strcasecmp(getenv("HTTP_X_FORWARDED_PROTO"), "https") === 0)) ?
                                  "https" : "http");
define("CMS_HOST",                CMS_PROTOCOL . "://$http_host");
define("CMS_DOMAIN",              explode(":", $http_host, 2)[0]);
define("CMS_ACTIVE_URL",          CMS_HOST . $php_self);
define("CMS_URL",                 CMS_HOST . $base_url);
define("CMS_MODULES_URL",         CMS_URL . "module/");
define("CMS_IMAGES_URL",          CMS_URL . "image/");
define("CMS_JAVA_URL",            CMS_URL . "java/");
define("CMS_JAVASCRIPT_URL",      CMS_URL . "javascript/");
define("CMS_SOUNDS_URL",          CMS_URL . "sound/");
define("CMS_ROOT_URL",            dirname(CMS_URL) . "/");
define("CMS_RELATIVE_URL",        $relative_url);
define("CMS_DATA_URL",            CMS_ROOT_URL . "data/");

// --- CONFIGURAION ---
define("CMS_APACHE",              function_exists("apache_get_version"));
define("CMS_CACHE_TTL",           2592000); //30 days
define("CMS_USER_AGENT",          "Mozilla/5.0 (compatible) " .
                                  CMS_SOFTWARE . "/1.0 (+" . CMS_HOST . ")");
define("CMS_LOGIN_ATTEMPT_MAX",   5);
define("CMS_LOGIN_BLOCK_TIME",    1800); //30 minutes
define("CMS_PERMISSION_ALWAYS",   "identification|index|check|security");

// --- REGEX PATTERNS ---
//                                     |                                       #    %    '    /    \
define("CMS_REGEX_MATTER",        "(?![\\x7C])[\p{Cf}\p{L}\p{M}\p{N}\p{Pc}\p{S}\\x23\\x25\\x27\\x2F\\x5C]");
//                                     "    #    %    '    /    ;    \    ¡        ¶        ·        ¿
define("CMS_REGEX_JOINT",         "(?![\\x22\\x23\\x25\\x27\\x2F\\x3B\\x5C\\x{00A1}\\x{00B6}\\x{00B7}\\x{00BF}])[\p{Pd}\p{Po}]");
//                                                                      "    ;    |    ¡        ¶        ·        ¿
define("CMS_REGEX_SEPARATOR",     "[\s\p{Pe}\p{Pf}\p{Pi}\p{Ps}\p{Z}\\x0B\\x22\\x3B\\x7C\\x{00A1}\\x{00B6}\\x{00B7}\\x{00BF}]");
define("CMS_REGEX_WORD",          "(?:" . CMS_REGEX_MATTER .    "|(?:" . CMS_REGEX_JOINT . ")++(?=" . CMS_REGEX_MATTER . "))++");
define("CMS_REGEX_BORDER",        "(?:" . CMS_REGEX_SEPARATOR . "|(?:" . CMS_REGEX_JOINT . ")++(?!" . CMS_REGEX_MATTER . "))++");

// --- SECURITY EVENT TYPES ---
define("CMS_SECURITY_EVENT_CSRF", 1);

//==================================================================================================
//   INITIALIZATION
//==================================================================================================

//load system libraries
cms_load_system();

//generate user id
cms_generate_id();

//register global variables
//normalize line breaks
//repair invalid utf-8
cms_initialize_globals();

//initialize language
cms_language();

//initialize email agent
cms_email_agent();

//user identification
cms_identification();

//add instance to default parameters
cms_param(CMS_INSTANCE, "cms_instance");

//==================================================================================================
//   HTML CONSTANTS
//==================================================================================================

define("CMS_DOCTYPE_HTML", "<!DOCTYPE HTML>");
define("CMS_BOT_CHECK",    (CMS_USER === "anonymous") ?
                           "<link rel=\"preload\" " .
                                 "href=\"" . htmlspecialchars(CMS_MODULES_URL) . "check.php\" " .
                                 "as=\"image\">" :
                           "");
define("CMS_HTML_HEADER",  "<meta charset=\"utf-8\">" .
                           "<title>" .
                           htmlspecialchars(CMS_DOMAIN . ": " . ucfirst(CMS_APPLICATION)) .
                           "</title>" .
                           "<meta name=\"generator\" " .
                                 "content=\"" . htmlspecialchars(CMS_IDENTIFIER) . "\">" .
                           "<meta name=\"robots\" " .
                                 "content=\"noindex, nofollow\">" .
                           "<base href=\"" . htmlspecialchars(CMS_ROOT_URL) . "\">" .
                           CMS_BOT_CHECK);
define("CMS_JAVASCRIPT",   "<script src=\"" . htmlspecialchars(CMS_JAVASCRIPT_URL) . "asr.js\">" .
                           "</script>" .
                           "<script src=\"" . htmlspecialchars(CMS_JAVASCRIPT_URL) . "common.js\">" .
                           "</script>" .
                           "<script src=\"" . htmlspecialchars(CMS_JAVASCRIPT_URL) . "fx.js\">" .
                           "</script>" .
                           "<script src=\"" . htmlspecialchars(CMS_JAVASCRIPT_URL) . "dragdrop.js\">" .
                           "</script>");
define("CMS_STYLESHEET",   "<link rel=\"stylesheet\" " .
                                 "href=\"" . htmlspecialchars(CMS_DATA_URL) . "stylesheet.css\">");
define("CMS_CLASS",        "cms-" . str_replace(".", "-", CMS_APPLICATION));

//==================================================================================================
//   HTTP HEADERS
//==================================================================================================

$date = gmdate("D, d M Y H:i:s");
header((CMS_USER === "anonymous") ?
       "Cache-Control: no-cache, must-revalidate" :
       "Cache-Control: no-store, no-cache, must-revalidate");
header("Content-Type: text/html; charset=utf-8");
header("Date: $date GMT");
header("Last-Modified: $date GMT");
header("X-Generator: " . CMS_IDENTIFIER, TRUE);

//==================================================================================================
//   MIGRATION
//==================================================================================================

if (
     (substr(CMS_PATH, -6) === "/nuos/")
     &&
     isset($_SERVER["REMOTE_ADDR"])
     &&
     (! in_array($_SERVER["REMOTE_ADDR"], ["127.0.0.1", "::1"]))
     &&
     cms_flag_set("cms.migrate")
   )
{
    //migrate nuos to pwnc
    register_shutdown_function(function()
    {
        if (! cms_flag_get("cms.migrate")) return;
        if (rename(CMS_PATH, CMS_ROOT_PATH . "pwnc/"))
        {
            rename(CMS_ROOT_PATH . "pwnc/nuos/", CMS_PATH);
            cms_flag_set("cms.directory.regenerate");
        };
        cms_flag_del("cms.migrate");
    });
}
elseif (cms_flag_get("cms.directory.regenerate"))
{
    //regenerate directory
    if (cms_load("directory")) directory_create_filesystem();
    cms_flag_del("cms.directory.regenerate");
};

//==================================================================================================
//   DAEMON
//==================================================================================================

//cache cleaning
cms_daemon(
    "cms_cache_clean(CMS_DATA_PATH . \"#cache/\");" .
    "cms_cache_clean(CMS_DATA_PATH . \"#content/cache/\");",
    "cache.daemon",
    3600, //1 hour
    "Cache cleanup");

//log cleaning
cms_daemon(
    "(new log())->cleanup();",
    "log.daemon",
    300, //5 minutes
    "Log maintenance");

//search update processing
cms_daemon(
    "if (! cms_load(\"search\")) return;\n" .
    "\$search = new search();\n" .
    "\$search->queue_add_update();\n" .
    "\$search->daemon();",
    "search.daemon.update",
    300, //5 minutes
    "Search update processing");

//search score processing
cms_daemon(
    "if (! cms_load(\"search\")) return;\n" .
    "(new search())->score_compute();",
    "search.daemon.score",
    3600, //1 hour
    "Search score processing");

//summon daemon
cms_daemon_run();

})();

//==================================================================================================
//   CLEAN UP
//==================================================================================================

unset($cms_user);
unset($cms_password);
unset($cms_login_user);
unset($cms_login_password);