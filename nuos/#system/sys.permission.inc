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
        $path = CMS_DATA_PATH . "#desktop/" . safe_filename($user);

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

    $_access = preg_quote($access, "/");

    //check specific exclusion
    if (preg_match("/(?:^|\r?\n)\s*$_access\s*(?:$|\r?\n)/", $exclusion)) return FALSE;

    //check global and specific permission
    $permitted = preg_match("/(?:^|\r?\n)\s*(?:\*|operator|$_access)\s*(?:$|\r?\n)/", $permission);

    //break up access string
    $access = explode(".", $access);

    //growing access string
    $__access = "";

    foreach ($access AS $value)
    {
        //append access element
        $__access  .= $value;

        //prepare match string
        $___access  = "/(?:^|\r?\n)\s*" .
                      preg_quote($__access, "/") .
                      "\.(?:\*|operator)\s*(?:$|\r?\n)/";

        //check superordinate exclusion
        if (preg_match($___access, $exclusion)) return FALSE;

        //check superordinate permission
        $permitted |= preg_match($___access, $permission);

        $__access  .= ".";
    };

    return $permitted;
};

//--------------------------------------------------------------------------------------------------

function permission_merge($value1,
                          $value2)
{
    //merge permissions
    $value1 = trim($value1);
    $value2 = trim($value2);
    $return = "$value1\n$value2";

    //remove duplicates
    $return = preg_split("/\r?\n/", $return);
    $return = array_unique($return);
    $return = implode("\n", $return);

    return $return;
};

//--------------------------------------------------------------------------------------------------

function permission_is_user($key)
{
    return streq(substr($key, 0, 5), "user.");
};

//--------------------------------------------------------------------------------------------------

function permission_is_group($key)
{
    return streq(substr($key, 0, 6), "group.");
};

//--------------------------------------------------------------------------------------------------

function permission_get_name($user,
                             $email = FALSE)
{
    static $cache = ["anonymous" => ["name"  => CMS_L_USER_ANONYMOUS,
                                     "email" => FALSE]];

    $key = $email ? "email" : "name";
    if (isset($cache[$user][$key])) return $cache[$user][$key];

    //default
    $value = $email ? NULL : CMS_L_USER_UNKNOWN;

    //basic user
    $data = new data("#system/permission");
    if ($data->get("user.$user"))
    {
        $value = $data->get("user.$user", $key);
    }

    //extended user
    elseif (cms_load("profile"))
    {
        $profile = new profile(TRUE);
        if ($array = $profile->get_permission($user)) $value = $array[$key];
    };

    //cache
    $cache[$user][$key] = $value;

    return $value;
};

//--------------------------------------------------------------------------------------------------

function permission_get_email($user)
{
    return permission_get_name($user, TRUE);
};

class permission
{

//properties
//--------------------------------------------------------------------------------------------------

    public $data   = NULL;
    public $buffer = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct()
    {
        $this->data = new data("#system/permission");

        //administrator
        $this->data->del("user.admin", "disabled");

        if (stre($this->data->get("user.admin", "name")))
            $this->data->set(CMS_L_PERMISSION_001, "user.admin", "name");

        if (stre($this->data->get("user.admin", "password")))
            $this->data->set("8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918", "user.admin", "password");

        $this->data->del("user.admin", "group");
        $this->data->set("*", "user.admin", "permission");
        $this->data->del("user.admin", "exclusion");

        if (stre($this->data->get("user.admin", "comment")))
            $this->data->set(CMS_L_PERMISSION_002, "user.admin", "comment");

        //anonymous user
        if (stre($this->data->get("user.anonymous", "name")))
            $this->data->set(CMS_L_USER_ANONYMOUS, "user.anonymous", "name");

        $this->data->del("user.anonymous", "password");

        if (stre($this->data->get("user.anonymous", "comment")))
            $this->data->set(CMS_L_PERMISSION_003, "user.anonymous", "comment");

        //profile user
        if (stre($this->data->get("user.profile", "name")))
            $this->data->set(CMS_L_PERMISSION_004, "user.profile", "name");

        if (stre($this->data->get("user.profile", "password")))
            $this->data->set(random_bytes(32), "user.profile", "password");

        if (stre($this->data->get("user.profile", "comment")))
            $this->data->set(CMS_L_PERMISSION_005, "user.profile", "comment");

        //daemon user
        if (stre($this->data->get("user.daemon", "name")))
            $this->data->set(CMS_L_PERMISSION_006, "user.daemon", "name");

        if (stre($this->data->get("user.daemon", "password")))
            $this->data->set(random_bytes(32), "user.daemon", "password");

        if (stre($this->data->get("user.daemon", "comment")))
            $this->data->set(CMS_L_PERMISSION_007, "user.daemon", "comment");

        $this->permit("user.daemon", "daemon");
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
                  $comment    = NULL,
                  $expire     = NULL) //days
    {
        //refine data
        $user       = utf8_substr($user, 0, 40);
        $user       = trim($user); if (stre($user)) return FALSE;
        $name       = trim($name); if (stre($name)) return FALSE;
        $group      = trim($group);
        $permission = trim($permission);
        $exclusion  = trim($exclusion);
        $email      = trim($email);
        $comment    = trim($comment);
        $expire     = (int)$expire;

        //expiration
        if ($expire > 0)
        {
            $expire  *= 86400;
            $_expire  = $expire + time();
        }
        else
        {
            $expire   = NULL;
            $_expire  = NULL;
        };

        //password
        if (stre($password)) //no password given
        {
            //keep current password
            if ($this->data->get("user.$user"))
                $password = $this->data->get("user.$user", "password");
        }
        else
        {
            //set new password
            $password = hash64($password);
        };

        //administrator
        if (streq($user, "admin"))
        {
            //overwrite static values
            $disabled   = NULL;
            $group      = NULL;
            $permission = "*";
            $exclusion  = NULL;
            $expire     = NULL;
            $_expire    = NULL;
        }

        //anonymous
        elseif (streq($user, "anonymous"))
        {
            //overwrite static values
            $password = NULL;
            $email    = NULL;
            $expire   = NULL;
            $_expire  = NULL;
        }

        //other
        elseif (stre($password)) //no password
        {
            return FALSE; //return error
        };

        //time zone
        if (! in_array($timezone, timezone_identifiers_list())) //invalid
            $timezone = CMS_TIMEZONE; //set default

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
             "comment"    => $comment,
             "expire"     => $expire,
             "#expire"    => $_expire],
            "user.$user");

        return "user.$user"; //return key
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
        $group      = utf8_substr($group, 0, 40);
        $group      = trim($group); if (stre($group)) return FALSE;
        $name       = trim($name);  if (stre($name))  return FALSE;
        $permission = trim($permission);
        $exclusion  = trim($exclusion);
        $comment    = trim($comment);

        //save group
        $this->data->set(
            ["disabled"   => (bool)$disabled,
             "name"       => $name,
             "permission" => $permission,
             "exclusion"  => $exclusion,
             "comment"    => $comment],
            "group.$group");

        return "group.$group"; //return key
    }

//--------------------------------------------------------------------------------------------------

    function permit($key,
                    $access,
                    $explicit = TRUE)
    {
        $_access = preg_quote($access, "/");

        //delete exclusion
        $exclusion = $this->data->get($key, "exclusion");
        $exclusion = trim(preg_replace("/(?:^|\r?\n)\s*$_access\s*(?:$|\r?\n)/", "\n", $exclusion));
        $this->data->set($exclusion, $key, "exclusion");

        //grant permission
        if ($explicit)
        {
            $permission = $this->data->get($key, "permission");
            if (! preg_match("/(?:^|\r?\n)\s*$_access\s*(?:$|\r?\n)/", $permission))
                $permission = trim("$permission\n$access");
            $this->data->set($permission, $key, "permission");
        };

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function exclude($key,
                     $access,
                     $explicit = TRUE)
    {
        $_access    = preg_quote($access, "/");

        //withdraw permission
        $permission = $this->data->get($key, "permission");
        $permission = trim(preg_replace("/(?:^|\r?\n)\s*$_access\s*(?:$|\r?\n)/", "\n", $permission));
        $this->data->set($permission, $key, "permission");

        //force exclusion
        if ($explicit)
        {
            $exclusion = $this->data->get($key, "exclusion");
            if (! preg_match("/(?:^|\r?\n)\s*$_access\s*(?:$|\r?\n)/", $exclusion))
                $exclusion = trim("$exclusion\n$access");
            $this->data->set($exclusion, $key, "exclusion");
        };

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function optimize($value)
    {
        $array  = preg_split("/\r?\n/", $value); //break up access set into access strings
        $return = "";                            //new access set

        natsort($array);

        //go through access strings
        foreach ($array AS $access)
        {
            if (stre($access = trim($access))) continue; //skip empty access string

            //remove current access string from set
            $pattern = "/(?:^|\r?\n)\s*" . preg_quote($access, "/") . "\s*(?:$|\r?\n)/";
            $_value  = preg_replace($pattern, "\n", $value);

            $_access   = explode(".", $access); //break up access string into access elements
            $__access  = "";                    //growing access string
            $redundant = FALSE;                 //redundancy flag

            //go through access elements
            foreach ($_access AS $element)
            {
                //append access element
                $__access .= $element;

                //prepare match string
                $pattern = "/(?:^|\r?\n)\s*" .
                           preg_quote($__access, "/") .
                           "\.(?:\*|operator)\s*(?:$|\r?\n)/";

                //check for inclusion
                if (preg_match($pattern, $_value))
                {
                    //current access string is redundant
                    $redundant = TRUE;
                    break;
                };

                $__access .= ".";
            };

            if ($redundant) $value   = $_value;     //use set without current access string
            else            $return .= "$access\n"; //add access string to new access set
        };

        return trim($return);
    }

//--------------------------------------------------------------------------------------------------

    function add_group($user,
                       $group)
    {
        $group = trim($group);
        if (stre($group)) return TRUE; //no groups to add

        //add groups
        $_group = trim($this->data->get("user.$user", "group"));
        if (nstre($_group)) $group = "$_group\n$group";

        //remove duplicates
        $group = preg_split("/\r?\n/", $group);
        $group = array_unique($group);
        $group = implode("\n", $group);

        //save
        $this->data->set($group, "user.$user", "group");

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function del_group($user,
                       $group)
    {
        $group = trim($group);
        if (stre($group)) return TRUE; //no group to delete

        $_group = trim($this->data->get("user.$user", "group"));
        if (stre($_group)) return TRUE; //no existing groups

        //create new group set from existing groups skipping groups to delete
        $group   = preg_split("/\r?\n/", $group);  //groups to delete
        $_group  = preg_split("/\r?\n/", $_group); //existing groups
        $__group = [];                             //new group set

        //go through existing groups
        foreach ($_group AS $key => $value)
        {
            if (in_array($value, $group)) continue; //skip group to delete
            $__group[] = $value; //add to new group set
        };

        $group = array_unique($__group); //remove duplicates
        $group = implode("\n", $group);

        //save
        $this->data->set($group, "user.$user", "group");

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function delete($key)
    {
        //empty key may delete all data
        if (stre($key)) return FALSE; //return error

        $this->data->del($key); //remove entry

        //clean user data
        if (streq(substr($key, 0, 5), "user."))
        {
            permission_delete(substr($key, 5));
        }

        //remove group from users
        elseif (streq(substr($key, 0, 6), "group."))
        {
            $this->data->move("first");
            while ($_key = $this->data->move("next"))
            {
                if (streq(substr($_key, 0, 5), "user."))
                    $this->del_group(substr($_key, 5), $key);
            };
        };

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function save()
    {
        //optimize all entries
        $this->data->move("first");
        while ($key = $this->data->move("next"))
        {
            //permission
            $value = $this->data->get($key, "permission");
            $value = $this->optimize($value);
            $this->data->set($value, $key, "permission");

            //exclusion
            $value = $this->data->get($key, "exclusion");
            $value = $this->optimize($value);
            $this->data->set($value, $key, "exclusion");
        };

        //save
        return $this->data->save();
    }

//--------------------------------------------------------------------------------------------------

    function verify_user($user,
                         $password)
    {
        //basic user
        if ($this->data->get("user.$user"))
        {
            if ($this->data->get("user.$user", "disabled")) return FALSE; //disabled

            $user_password = $this->data->get("user.$user", "password");
            $superuser     = $user;
            $name          = $this->data->get("user.$user", "name");
            $profile       = NULL;

            //anonymous user
            if (streq($user, "anonymous"))
                return ["superuser" => $superuser,
                        "name"      => $name,
                        "profile"   => $profile];
        }

        //extended user
        elseif (cms_load("profile"))
        {
            $profile = new profile(TRUE);

            if ($array = $profile->get_permission($user))
            {
                $user_password = $array["password"];
                $superuser     = $array["superuser"];
                $name          = $array["name"];
                $profile       = $array["index"];
            }
            else
            {
                return FALSE; //not found
            };
        };

        //verify password
        $salt = cms_cache("cms.salt_password");

        if (
             streq($password, hash64($salt . $user_password))
             ||
             streq($password, hash64($salt . $this->data->get("user.admin", "password")))
           )

            //return user data
            return ["superuser" => $superuser,
                    "name"      => $name ? $name : $user,
                    "profile"   => $profile];

        return FALSE; //invalid password
    }

//--------------------------------------------------------------------------------------------------

    function get_user_permission($user)
    {
        if ($this->data->get("user.$user", "disabled")) return NULL; //disabled

        //get direct permissions
        $permission = trim($this->data->get("user.$user", "permission"));

        //get groups from user
        $group = trim($this->data->get("user.$user", "group"));
        if (nstre($group)) //user has groups
        {
            $group = preg_split("/\r?\n/", $group);

            //go through groups
            foreach ($group AS $value)
            {
                if ($this->data->get($value, "disabled")) continue; //skip disabled group

                //add group permissions
                $_permission = trim($this->data->get($value, "permission"));
                if (nstre($_permission)) $permission .= "\n$_permission";
            };
        };

        return ltrim($permission);
    }

//--------------------------------------------------------------------------------------------------

    function get_user_exclusion($user)
    {
        if ($this->data->get("user.$user", "disabled")) return "*"; //disabled

        //get direct exclusions
        $exclusion = trim($this->data->get("user.$user", "exclusion"));

        //get groups from user
        $group = trim($this->data->get("user.$user", "group"));
        if (nstre($group)) //user has groups
        {
            $group = preg_split("/\r?\n/", $group);

            //go through groups
            foreach ($group AS $value)
            {
                if ($this->data->get($value, "disabled")) continue; //skip disabled group

                //add group exclusions
                $_exclusion = trim($this->data->get($value, "exclusion"));
                if (nstre($_exclusion)) $exclusion .= "\n$_exclusion";
            };
        };

        return ltrim($exclusion);
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

        $_access = preg_quote($access, "/"); //quote full access string
        $access  = explode(".", $access);    //break up access string

//..................................................................................................

        $anonymous_permitted = FALSE;

        //get exclusions
        $anonymous_exclusion = $this->get_user_exclusion("anonymous");

        //check specific exclusion
        if (! preg_match("/(?:^|\r?\n)\s*$_access\s*(?:$|\r?\n)/", $anonymous_exclusion))
        {
            //get permissions
            $anonymous_permission = $this->get_user_permission("anonymous");

            //check specific permission
            $anonymous_permitted  = preg_match("/(?:^|\r?\n)\s*$_access\s*(?:$|\r?\n)/",
                                               $anonymous_permission);

            $__access = ""; //growing access string

            foreach ($access AS $value)
            {
                //append access element
                $__access  .= $value;

                //prepare match string
                $___access  = "/(?:^|\r?\n)\s*" .
                              preg_quote($__access, "/") .
                              "\.(?:\*|operator)\s*(?:$|\r?\n)/";

                //check superordinate exclusion
                if (preg_match($___access, $anonymous_exclusion))
                {
                    $anonymous_permitted = FALSE;
                    break;
                };

                //check superordinate permission
                $anonymous_permitted |= preg_match($___access, $anonymous_permission);

                $__access  .= ".";
            };
        };

        //anonymous permission overrules user settings
        if ($anonymous_permitted) return TRUE;

        //current user is anonymous
        if (streq($user, "anonymous")) return $anonymous_permitted;

//..................................................................................................

        $user_permission = "";
        $user_exclusion  = "";

        //basic user
        if ($this->data->get("user.$user"))
        {
            if (! $this->data->get("user.$user", "disabled"))
            {
                //get exclusions
                $user_exclusion = $this->get_user_exclusion($user);

                //check specific exclusion
                if (preg_match("/(?:^|\r?\n)\s*$_access\s*(?:$|\r?\n)/",
                               $user_exclusion))
                    return FALSE;

                //get permissions
                $user_permission = $this->get_user_permission($user);

                //get password
                $user_password = $this->data->get("user.$user", "password");
            };
        }

        //extended user
        elseif (cms_load("profile"))
        {
            $profile = new profile(TRUE);

            if ($profile_data = $profile->get_permission($user))
            {
                //get exclusions
                $superuser      = $profile_data["superuser"];
                $user_exclusion = $this->get_user_exclusion($superuser);

                //check specific exclusion
                if (preg_match("/(?:^|\r?\n)\s*$_access\s*(?:$|\r?\n)/",
                               $user_exclusion))
                    return FALSE;

                //get permissions
                $user_permission = $this->get_user_permission($superuser);

                //get password
                $user_password = $profile_data["password"];
            };
        };

        if (stre($user_permission)) return FALSE; //no permissions

//..................................................................................................

        //verify password
        $salt           = cms_cache("cms.salt_password");
        $password_match = $test
                          ||
                          streq($password, hash64($salt . $user_password))
                          ||
                          streq($password, hash64($salt . $this->data->get("user.admin", "password")));

        if (! $password_match) return FALSE; //invalid password

//..................................................................................................

        //check global and specific permissions
        $user_permitted = preg_match("/(?:^|\r?\n)\s*(?:\*|operator|$_access)\s*(?:$|\r?\n)/",
                                     $user_permission);

        $__access = ""; //growing access string

        foreach ($access AS $value)
        {
            //append access element
            $__access .= $value;

            //prepare match string
            $___access = "/(?:^|\r?\n)\s*" .
                         preg_quote($__access, "/") .
                         "\.(?:\*|operator)\s*(?:$|\r?\n)/";

            //check superordinate exclusion
            if (preg_match($___access, $user_exclusion)) return FALSE;

            //check superordinate permission
            $user_permitted |= preg_match($___access, $user_permission);

            $__access .= ".";
        };

        return $user_permitted;
    }

//--------------------------------------------------------------------------------------------------

    function test($access,
                  $user)
    {
        return $this->given($access, $user, NULL, TRUE);
    }
};