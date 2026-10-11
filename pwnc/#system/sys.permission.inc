<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   PERMISSION CLASS
//==================================================================================================

//related functions
//--------------------------------------------------------------------------------------------------

function permission_delete($user)
{
    //remove agent instruction file
    $file = encode_filename($user);
    $path = CMS_DATA_PATH . "#permission/instruction/$file";
    if (is_file($path)) unlink($path);

    //remove agent tokens
    $data = new data("#system/permission.token");
    while ($key = $data->seek(["user" => $user])) $data->del($key);
    $data->save();

    //remove ims data
    if (
         cms_load("core_resource")
         &&
         is_file(CMS_DATA_PATH . "#desktop/ims.core")
       )
    {
        $ims = new core_resource(
            CMS_DATA_PATH . "#desktop/ims.core",
            ["id"       => "string[8]",
             "thread"   => "string[8]",
             "time"     => "string[20]",
             "owner"    => "_string[40]",
             "receiver" => "_string[40]",
             "sender"   => "_string[40]",
             "status"   => "byte",
             "text"     => "_string[1500]",
             "hash"     => "string[32]"]);

        $ims->lock();
        while ($ims->seek(["owner" => $user])) $ims->del();
        $ims->unlock();

        unset($ims);
    };

    //remove desktop data
    if (cms_available("desktop"))
    {
        $path = CMS_DATA_PATH . "#desktop/$file/";

        if (
             is_dir($path)
             &&
             cms_load("filemanager")
           )
            filemanager_delete($path);
    };

    //change content owner to administrator
    if (cms_load("content"))
    {
        new mysql(); //establish database connection
        mysql_query("UPDATE " . CMS_DB_CONTENT . " " .
                    "SET " .    CMS_DB_CONTENT_OWNER . "='admin' " .
                    "WHERE " .  CMS_DB_CONTENT_OWNER . "='" . sqlesc($user) . "'");
    };
};

//--------------------------------------------------------------------------------------------------

function permission_match($access,
                          $permission,
                          $exclusion)
{
    //default permission
    if (strpos("|" . CMS_PERMISSION_ALWAYS . "|", "|$access|") !== FALSE) return TRUE; //always allow

    //no permissions
    if (stre($permission)) return NULL;

    //break up access string
    $array = explode(".", $access);

    //make exclusion array
    $exclusion = preg_split("/\s+/", $exclusion, -1, PREG_SPLIT_NO_EMPTY);
    $exclusion = array_flip($exclusion);

    //check global and specific exclusion
    if (($exclusion["*"] ?? $exclusion["operator"] ?? $exclusion[$access] ?? NULL) !== NULL) return FALSE;

    //make permission array
    $permission = preg_split("/\s+/", $permission, -1, PREG_SPLIT_NO_EMPTY);
    $permission = array_flip($permission);

    //check global and specific permission
    $permitted = $permission["*"] ?? $permission["operator"] ?? $permission[$access] ?? NULL;
    $permitted = isset($permitted) ? TRUE : NULL;

    //growing access string
    $string = "";

    foreach ($array AS $part)
    {
        //append access element
        $string .= $part;

        //check superordinate exclusion
        foreach ([".*", ".operator"] AS $value)
        {
            if (isset($exclusion[$string . $value])) return FALSE;
        };

        //check superordinate permission
        if ($permitted === NULL)
        {
            foreach ([".*", ".operator"] AS $value)
            {
                if (isset($permission[$string . $value]))
                {
                    $permitted = TRUE;
                    break;
                };
            };
        };

        $string .= ".";
    };

    return $permitted;
};

//--------------------------------------------------------------------------------------------------

function permission_merge($value1,
                          $value2)
{
    //merge and remove duplicates
    $array = preg_split("/\s+/", "$value1\n$value2", -1, PREG_SPLIT_NO_EMPTY);
    $array = array_flip($array);
    $array = array_keys($array);
    $array = implode("\n", $array);

    return $array;
};

//--------------------------------------------------------------------------------------------------

function permission_is_user($key)
{
    return substr($key, 0, 5) === "user.";
};

//--------------------------------------------------------------------------------------------------

function permission_is_group($key)
{
    return substr($key, 0, 6) === "group.";
};

//--------------------------------------------------------------------------------------------------

function permission_is_agent($user)
{
    return nstre((new data("#system/permission"))->get("user.$user", "api"));
};

//--------------------------------------------------------------------------------------------------

function permission_is_active($user,
                              $set = FALSE)
{
    $window = 900;
    return $set ? cms_cache("permission.active.$user", time(), TRUE, $window) :
           ((time() - cms_cache("permission.active.$user", NULL, FALSE, FALSE)) <= $window);
};

//--------------------------------------------------------------------------------------------------

function permission_remove_prefix($key)
{
    return (($pos = strpos($key, ".")) === FALSE) ? $key : substr($key, $pos + 1);
};

//--------------------------------------------------------------------------------------------------

function permission_get_name($user,
                             $fallback = NULL,
                             $email    = FALSE)
{
    static $cache = NULL;

    $cache    ??= ["anonymous" => ["name"  => t("user_anonymous"),
                                   "email" => FALSE]];
    $fallback ??= t("user_unknown");

    $key = $email ? "email" : "name";
    if (isset($cache[$user][$key])) return $cache[$user][$key];

    //default
    $value = $email ? NULL : $fallback;

    //basic user
    $data = new data("#system/permission");
    if (($array = $data->get("user.$user")) !== NULL)
    {
        $value = $array[$key] ?? ($email ? NULL : $user);
    }

    //extended user
    elseif (cms_load("profile"))
    {
        if ($array = (new profile(TRUE))->get_permission($user)) $value = $array[$key];
    };

    //cache and return
    return $cache[$user][$key] = $value;
};

//--------------------------------------------------------------------------------------------------

function permission_get_email($user)
{
    return permission_get_name($user, t("user_unknown"), TRUE);
};

//--------------------------------------------------------------------------------------------------

function permission_get_language($user)
{
    $data = new data("#system/permission");
    return $data->get("user.$user", "language")     ??
           $data->get("user.anonymous", "language") ??
           CMS_LANGUAGE;
};

//--------------------------------------------------------------------------------------------------

function permission_get_group($user,
                              $data = NULL)
{
    if (! ($data instanceof data)) $data = new data("#system/permission");

    $array = [];
    foreach (preg_split("/\n/", $data->get("user.$user", "group") ?? "", -1, PREG_SPLIT_NO_EMPTY) AS $key)
        $array[$key] = $data->get($key, "name") ?? "";

    return $array;
};

//--------------------------------------------------------------------------------------------------

function permission_get_member($group,
                               $data = NULL)
{
    if (! ($data instanceof data)) $data = new data("#system/permission");

    $array = [];
    $group = "group.$group";

    $data->move("first");
    while ($key = $data->move("next"))
    {
        if (
             permission_is_user($key)
             &&
             (strpos("\n" . ($data->get($key, "group") ?? "") . "\n", "\n$group\n") !== FALSE)
           )
            $array[$key] = $data->get($key, "name") ?? "";
    };

    return $array;
};

class permission
{

//properties
//--------------------------------------------------------------------------------------------------

    public $data = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct()
    {
        $this->data = new data("#system/permission");

        //administrator
        $this->data->del("user.admin", "disabled");

        if (stre($this->data->get("user.admin", "name")))
            $this->data->set(t("administrator"), "user.admin", "name");

        if (stre($this->data->get("user.admin", "password")))
            $this->data->set("8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918", "user.admin", "password");

        $this->data->del("user.admin", "group");
        $this->data->set("*", "user.admin", "permission");
        $this->data->del("user.admin", "exclusion");

        if (stre($this->data->get("user.admin", "comment")))
            $this->data->set(t("main_administrative_account"), "user.admin", "comment");

        //anonymous user
        if (stre($this->data->get("user.anonymous", "name")))
            $this->data->set(t("user_anonymous"), "user.anonymous", "name");

        $this->data->del("user.anonymous", "password");

        if (stre($this->data->get("user.anonymous", "comment")))
            $this->data->set(t("comment_user_anonymous"), "user.anonymous", "comment");

        //profile user
        if (stre($this->data->get("user.profile", "name")))
            $this->data->set(t("user_registration"), "user.profile", "name");

        if (stre($this->data->get("user.profile", "password")))
            $this->data->set(bin2hex(random_bytes(32)), "user.profile", "password");

        if (stre($this->data->get("user.profile", "comment")))
            $this->data->set(t("comment_user_profile"), "user.profile", "comment");

        //daemon user
        if (stre($this->data->get("user.daemon", "name")))
            $this->data->set(t("daemon"), "user.daemon", "name");

        if (stre($this->data->get("user.daemon", "password")))
            $this->data->set(bin2hex(random_bytes(32)), "user.daemon", "password");

        if (stre($this->data->get("user.daemon", "comment")))
            $this->data->set(t("comment_user_daemon"), "user.daemon", "comment");

        $this->permit("user.daemon", "daemon");

        //trigger user
        if (stre($this->data->get("user.trigger", "name")))
            $this->data->set(t("event"), "user.trigger", "name");

        if (stre($this->data->get("user.trigger", "password")))
            $this->data->set(bin2hex(random_bytes(32)), "user.trigger", "password");

        if (stre($this->data->get("user.trigger", "comment")))
            $this->data->set(t("comment_user_trigger"), "user.trigger", "comment");
    }

//methods
//--------------------------------------------------------------------------------------------------

    function user($user,
                  $disabled   = NULL,
                  $name       = NULL,
                  $password   = NULL,
                  $group      = NULL,
                  $permission = NULL,
                  $exclusion  = NULL,
                  $email      = NULL,
                  $timezone   = NULL,
                  $language   = NULL,
                  $comment    = NULL,
                  $expire     = NULL)
    {
        //refine data
        $user       = trim(utf8_substr($user, 0, 40)); if (stre($user)) return FALSE;
        $name       = trim($name    ?? "");            if (stre($name)) return FALSE;
        $password   = stre($password) ? $this->data->get("user.$user", "password") : hash64($password);
        $group      = trim($group   ?? "");
        $permission = $this->optimize($permission ?? "");
        $exclusion  = $this->optimize($exclusion  ?? "");
        $email      = trim($email   ?? "");
        $comment    = trim($comment ?? "");

        //expiry date
        if ($expire !== NULL)
            $expire = is_string($expire) ? strtotime($expire) : (int)$expire;

        //administrator
        if ($user === "admin")
        {
            //overwrite static values
            $disabled   = NULL;
            $group      = NULL;
            $permission = "*";
            $exclusion  = NULL;
            $expire     = NULL;
        }

        //anonymous
        elseif ($user === "anonymous")
        {
            //overwrite static values
            $password = NULL;
            $email    = NULL;
            $expire   = NULL;
        }

        //other
        elseif (stre($password)) //no password
        {
            return FALSE; //return error
        };

        //time zone
        if (! in_array($timezone, timezone_identifiers_list())) //invalid
            $timezone = CMS_TIMEZONE; //set default

        //language
        if (! isset(language_ui_available()[$language ?? ""])) //invalid
            $language = NULL; //auto

        //save user
        $this->data->set(
            ["disabled"   => (bool)$disabled,
             "name"       => $name,
             "password"   => $password,
             "group"      => $group,
             "permission" => $permission,
             "exclusion"  => $exclusion,
             "email"      => $email,
             "timezone"   => $timezone,
             "language"   => $language,
             "comment"    => $comment,
             "expire"     => $expire],
            "user.$user");

        //return key
        return $this->data->save() ? "user.$user" : FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function group($group,
                   $disabled   = NULL,
                   $name       = NULL,
                   $permission = NULL,
                   $exclusion  = NULL,
                   $comment    = NULL)
    {
        //refine data
        $group      = trim(utf8_substr($group, 0, 40)); if (stre($group)) return FALSE;
        $name       = trim($name    ?? "");             if (stre($name))  return FALSE;
        $permission = $this->optimize($permission ?? "");
        $exclusion  = $this->optimize($exclusion  ?? "");
        $comment    = trim($comment ?? "");

        //save group
        $this->data->set(
            ["disabled"   => (bool)$disabled,
             "name"       => $name,
             "permission" => $permission,
             "exclusion"  => $exclusion,
             "comment"    => $comment],
            "group.$group");

        //return key
        return $this->data->save() ? "group.$group" : FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function permit($key,
                    $access,
                    $explicit = TRUE,
                    $invert   = FALSE)
    {
        $_access = preg_quote($access, "/");

        $param = $invert ? "permission" : "exclusion";
        $value = $this->data->get($key, $param) ?? "";
        $value = preg_replace("/(?:^|\s+)$_access\s*(?:\s+|$)/", "\n", $value);
        $value = $this->optimize($value);
        $this->data->set($value, $key, $param);

        if ($explicit)
        {
            $param = $invert ? "exclusion" : "permission";
            $value = $this->data->get($key, $param) ?? "";
            if (! preg_match("/(?:^|\s+)$_access\s*(?:\s+|$)/", $value)) $value .= "\n$access";
            $value = $this->optimize($value);
            $this->data->set($value, $key, $param);
        };

        return $this->data->save();
    }

//--------------------------------------------------------------------------------------------------

    function exclude($key,
                     $access,
                     $explicit = TRUE)
    {
        return $this->permit($key, $access, $explicit, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function optimize($value)
    {
        $array = preg_split("/\s+/", $value, -1, PREG_SPLIT_NO_EMPTY);
        if (array_intersect(["*", "operator"], $array)) return "*";

        natsort($array);

        $array  = array_flip($array);
        $return = [];

        //iterate access strings
        foreach ($array AS $line => $_)
        {
            $part      = explode(".", $line); //break up access string
            $string    = "";                  //growing access string
            $redundant = FALSE;               //redundancy flag

            //iterate access elements
            foreach ($part AS $value)
            {
                //append access element
                $string .= "$value.";

                //check for inclusion
                $v1 = $string . "*";
                $v2 = $string . "operator";

                //.* found and not current line
                //or .operator found and not current line
                if (isset($array[$v1]) ?  ($line !== $v1) :
                    isset($array[$v2]) && ($line !== $v2))
                {
                    $redundant = TRUE;
                    break;
                };
            };

            if (! $redundant) $return[] = $line; //add access string to new access set
        };

        return implode("\n", $return);
    }

//--------------------------------------------------------------------------------------------------

    function add_group($user,
                       $group)
    {
        //add group
        $group = ($this->data->get("user.$user", "group") ?? "") . "\n$group";

        //remove duplicates
        $group = preg_split("/\n/", $group, -1, PREG_SPLIT_NO_EMPTY);
        $group = array_flip($group);
        $group = array_keys($group);
        $group = implode("\n", $group);

        //store
        $this->data->set($group, "user.$user", "group");
        return $this->data->save();
    }

//--------------------------------------------------------------------------------------------------

    function del_group($user,
                       $group)
    {
        //remove groups
        $group  = preg_split("/\n/", $group, -1, PREG_SPLIT_NO_EMPTY);
        $_group = $this->data->get("user.$user", "group") ?? "";
        $_group = preg_split("/\n/", $_group, -1, PREG_SPLIT_NO_EMPTY);
        $_group = array_diff($_group, $group);
        $_group = implode("\n", $_group);

        //store
        $this->data->set($_group, "user.$user", "group");
        return $this->data->save();
    }

//--------------------------------------------------------------------------------------------------

    function delete($key)
    {
        //empty key may delete all data
        if (stre($key)) return FALSE; //error

        //remove and save
        $this->data->del($key);
        if (! $this->data->save()) return FALSE; //error

        //clean user data
        if (permission_is_user($key))
        {
            permission_delete(permission_remove_prefix($key));
        }

        //remove group from users
        elseif (permission_is_group($key))
        {
            $this->data->move("first");
            while ($_key = $this->data->move("next"))
            {
                if (permission_is_user($_key))
                    $this->del_group(permission_remove_prefix($_key), $key);
            };
        };

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function verify_user($user,
                         $password)
    {
        //basic user
        if ($array = $this->data->get("user.$user"))
        {
            if ($array["disabled"] ?? FALSE) return FALSE; //disabled

            $user_password = $array["password"] ?? NULL;
            $superuser     = $user;
            $name          = $array["name"]     ?? NULL;
            $profile       = NULL;

            //anonymous user
            if ($user === "anonymous")
                return ["superuser" => $superuser,
                        "name"      => $name,
                        "profile"   => $profile];
        }

        //extended user
        elseif (
                 cms_load("profile")
                 &&
                 ($array = (new profile(TRUE))->get_permission($user))
               )
        {
            $user_password = $array["password"];
            $superuser     = $array["superuser"];
            $name          = $array["name"];
            $profile       = $array["index"];

            //superuser
            $array = $this->data->get("user.$superuser");
            if ($array === FALSE)           return FALSE; //invalid
            if ($array["disabled"] ?? TRUE) return FALSE; //disabled
        }

        //user not found
        else
        {
            return FALSE;
        };

        //check user expiry -> disable
        $time   = time();
        $expire = (int)($array["expire"] ?? $time);
        if ($expire < $time)
        {
            $this->data->set(TRUE, "user.$superuser", "disabled");
            $this->data->save();
            return FALSE;
        };

        //verify password
        $salt = cms_salt("password");

        if (
             ($password === hash64($salt . $user_password))
             ||
             ($password === hash64($salt . $this->data->get("user.admin", "password")))
           )

            //return user data
            return ["superuser" => $superuser,
                    "name"      => $name ? $name : $user,
                    "profile"   => $profile];

        return FALSE; //invalid password
    }

//--------------------------------------------------------------------------------------------------

    function get_user_permission($user,
                                 $invert = FALSE)
    {
        if ($this->data->get("user.$user", "disabled")) return $invert ? "*" : ""; //disabled

        //return user settings
        $param = $invert ? "exclusion" : "permission";
        $value = $this->data->get("user.$user", $param);
        return isset($value) ? $this->optimize($value) : "";
    }

//--------------------------------------------------------------------------------------------------

    function get_user_exclusion($user)
    {
        return $this->get_user_permission($user, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function get_group_permission($user,
                                  $invert = FALSE)
    {
        if ($this->data->get("user.$user", "disabled")) return $invert ? "*" : ""; //disabled

        //get groups from user
        $group = $this->data->get("user.$user", "group") ?? "";
        $group = preg_split("/\n/", $group, -1, PREG_SPLIT_NO_EMPTY);
        $group = array_flip($group);
        $param = $invert ? "exclusion" : "permission";
        $value = "";

        //iterate groups
        foreach ($group AS $key => $_)
        {
            if ($this->data->get($key, "disabled")) continue;        //skip disabled group
            $value .= ($this->data->get($key, $param) ?? "") . "\n"; //add group settings
        };

        return $this->optimize($value);
    }

//--------------------------------------------------------------------------------------------------

    function get_group_exclusion($user)
    {
        return $this->get_group_permission($user, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function given($access   = NULL,
                   $user     = NULL,
                   $password = NULL,
                   $test     = NULL)
    {
        //default access request
        if (stre($access))
        {
            $access = CMS_APPLICATION;
            if (nstre(CMS_INSTANCE)) $access .= "." . CMS_INSTANCE;
        };

        //default permission
        if (strpos("|" . CMS_PERMISSION_ALWAYS . "|", "|$access|") !== FALSE) return TRUE; //always allow

        //default user data
        if (stre($user))     $user     = CMS_USER;
        if (stre($password)) $password = CMS_PASSWORD;

//..................................................................................................

        if (! $this->data->get("user.anonymous", "disabled"))
        {
            //anonymous permissions apply to anyone
            if (permission_match($access,
                                 $this->get_user_permission("anonymous"),
                                 $this->get_user_exclusion("anonymous")))
                return TRUE;

            if (permission_match($access,
                                 $this->get_group_permission("anonymous"),
                                 $this->get_group_exclusion("anonymous")))
                return TRUE;
        };

        //anonymous check ends here
        if ($user === "anonymous") return FALSE;

//..................................................................................................

        //basic user
        if ($array = $this->data->get("user.$user"))
        {
            if ($array["disabled"] ?? FALSE) return FALSE; //user disabled
            $user_password = $array["password"] ?? NULL;
        }

        //extended user
        elseif (
                 cms_load("profile")
                 &&
                 (($array = (new profile(TRUE))->get_permission($user)) !== FALSE)
               )
        {
            $user          = $array["superuser"];
            $user_password = $array["password"];
        }

        //user not found
        else
        {
            return FALSE;
        };

        if (stre($user_password)) return FALSE; //no password -> invalid

//..................................................................................................

        //verify password
        $salt           = cms_salt("password");
        $password_match = $test //override
                          ||

                          //verify user password
                          ($password === hash64($salt . $user_password))
                          ||

                          //verify admin password
                          //always works for any user for administrative reasons
                          ($password === hash64($salt . $this->data->get("user.admin", "password")));

        if (! $password_match) return FALSE; //invalid password

//..................................................................................................

        //check specific user permissions
        $flag = permission_match(
            $access,
            $this->get_user_permission($user),
            $this->get_user_exclusion($user));
        if ($flag !== NULL) return $flag;

        //check specific group permissions
        $flag = permission_match(
            $access,
            $this->get_group_permission($user),
            $this->get_group_exclusion($user));
        return (bool)$flag;
    }

//--------------------------------------------------------------------------------------------------

    function test($access,
                  $user)
    {
        return $this->given($access, $user, NULL, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function agent($user,
                   $instruction       = NULL,
                   $reminder_interval = NULL,
                   $token_expire      = NULL,
                   $api               = NULL,
                   $endpoint          = NULL,
                   $model             = NULL,
                   $api_key           = NULL,
                   $option            = NULL)
    {
        //retrieve user data
        $array = $this->data->get("user.$user");
        if (blank($array)) return FALSE;

        $error = FALSE;
        $save  = FALSE;

        //set instruction text
        if ($instruction !== NULL)
            $error |= ! $this->instruction($user, $instruction);

        //instruction reminder interval
        if ($reminder_interval !== NULL)
        {
            $array["instruction_interval"] = (int)$reminder_interval;
            $save = TRUE;
        };

        //token expiry date
        if ($token_expire !== NULL)
        {
            $array["token_expire"] = is_string($token_expire) ?
                                     strtotime($token_expire) :
                                     (int)$token_expire;
            $save = TRUE;
        };

        //provider configuration, blank clears the field
        foreach (["api"      => $api,
                  "endpoint" => $endpoint,
                  "model"    => $model,
                  "api_key"  => $api_key,
                  "option"   => $option]
                 AS $key => $value)
        {
            if ($value === NULL) continue;
            if (nstre($value))  $array[$key] = $value;
            else                unset($array[$key]);
            $save = TRUE;
        };

        //save
        if ($save)
        {
            $this->data->set($array, "user.$user");
            $error |= ! $this->data->save();
        };

        return ! $error;
    }

//--------------------------------------------------------------------------------------------------

    static function instruction($user,
                                $set = NULL)
    {
        $path = CMS_DATA_PATH . "#permission/instruction/" . encode_filename($user);

        if ($set !== NULL)
        {
            if (nstre($set))        return write_file($path, $set);
            elseif (is_file($path)) return unlink($path);
            return TRUE;
        };

        return read_file($path);
    }

//--------------------------------------------------------------------------------------------------

    function get_user($token)
    {
        //retrieve user
        $user = (new data("#system/permission.token"))->get(hash64($token), "user");
        if ($user === NULL) return NULL; //token not found

        //retrieve user data
        $array = $this->data->get("user.$user");
        if (blank($array)) return NULL;

        //user disabled
        if ($array["disabled"] ?? FALSE) return NULL;

        //check token expiry
        $time   = time();
        $expire = (int)($array["token_expire"] ?? $time);
        if ($expire < $time)
        {
            //expired
            $this->del_token($user);
            return NULL;
        };

        return $user;
    }

//--------------------------------------------------------------------------------------------------

    function rotate_token($user)
    {
        //retrieve user
        $array = $this->data->get("user.$user");
        if (blank($array)) return FALSE;

        //delete token
        $data = new data("#system/permission.token");
        while ($key = $data->seek(["user" => $user, "owner" => ""])) $data->del($key);

        //store new token
        $token = bin2hex(random_bytes(32));
        $data->set(["user" => $user], hash64($token));

        return $data->save() ? $token : FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function del_token($user)
    {
        //delete tokens
        $data = new data("#system/permission.token");
        while ($key = $data->seek(["user" => $user])) $data->del($key);

        if ($data->save())
        {
            //delete expiry date
            $this->data->del("user.$user", "token_expire");
            if ($this->data->save()) return TRUE;
        };

        return FALSE;
    }
};