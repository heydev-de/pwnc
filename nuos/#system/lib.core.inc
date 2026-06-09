<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   COMMUNICATION RELAY (CORE) CLASS
//==================================================================================================

//permission
define("CMS_CORE_PERMISSION_CONTROL",  "control");
define("CMS_CORE_PERMISSION_OPERATOR", "operator");

//various
define("CMS_CORE_PORTAL",              "Lobby");

//status
define("CMS_CORE_STATUS_NONE",        0);
define("CMS_CORE_STATUS_OWNER",       1);
define("CMS_CORE_STATUS_OPERATOR",    2);
define("CMS_CORE_STATUS_SPY",         4);
define("CMS_CORE_STATUS_INVISIBLE",   8);
define("CMS_CORE_STATUS_ABSENT",     16);
define("CMS_CORE_STATUS_MUTE",       32);
define("CMS_CORE_STATUS_BANNED",     64);

//data types
define("CMS_CORE_DATA_DEFAULT",       0);
define("CMS_CORE_DATA_SYSTEM",        1);
define("CMS_CORE_DATA_PRIVATE",       2);
define("CMS_CORE_DATA_PRIVATE_META",  4);
define("CMS_CORE_DATA_PRIVATE_DATA",  8);
define("CMS_CORE_DATA_RESPONSE",     16);
define("CMS_CORE_DATA_SPY_META",     32);
define("CMS_CORE_DATA_SPY_DATA",     64);

class core
{

//properties
//--------------------------------------------------------------------------------------------------

    public $guid     = NULL;  //generic userid

    public $timeout  = 15;    //connection timeout
    public $enabled  = FALSE; //core activity status of executing user
    public $operator = FALSE; //main operator status

    public $index    = NULL;  //index resource object
    public $profile  = NULL;  //profile resource object
    public $channel  = NULL;  //channel resource object
    public $status   = NULL;  //status resource object
    public $data     = NULL;  //data resource object

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($timeout  = 15,
                         $datapath = "#core")
    {
        //load data resource library
        if (! cms_load("core_resource")) return;

        //generate userid
        $this->guid = streq(CMS_USER, "anonymous") ? CMS_USERID : "!" . CMS_USER;

        //userstatus related to temporary and permanent channels
        $this->status = new core_resource(
            CMS_DATA_PATH . "$datapath/status.core",
            ["guid"    => "string[41]",
             "channel" => "_string[20]",
             "status"  => "byte"]);

        //check status
        $status = $this->get_status();

        if (
             (! $this->operator = cms_permission(CMS_CORE_PERMISSION_OPERATOR))
             &&
             flag($status, CMS_CORE_STATUS_BANNED)
             &&
             (! flag($status, CMS_CORE_STATUS_OWNER))
             &&
             (! flag($status, CMS_CORE_STATUS_OPERATOR))
           )
        {
            $this->status = NULL;
            return;
        };

        $this->enabled = TRUE;
        $this->timeout = $timeout;

        //connected users
        $this->index   = new core_resource(
            CMS_DATA_PATH . "$datapath/index.core",
            ["guid"     => "_string[41]",
             "channel"  => "_string[20]",
             "time"     => "string[20]"]);

        //temporary and permanent userprofiles
        $this->profile = new core_resource(
            CMS_DATA_PATH . "$datapath/profile.core",
            ["guid"     => "_string[41]",
             "name"     => "_string[40]",
             "color"    => "string[7]",
             "text"     => "_string[1024]",
             "reserved" => "_string[40]",
             "image"    => "string[256]"]);

        //permanent channels
        $this->channel = new core_resource(
            CMS_DATA_PATH . "$datapath/channel.core",
            ["channel"  => "_string[20]",
             "text"     => "_string[1024]",
             "password" => "_string[20]"]);

        //data exchange buffer
        $this->data    = new core_resource(
            CMS_DATA_PATH . "$datapath/data.core",
            ["guid"     => "_string[41]",
             "receiver" => "_string[41]",
             "status"   => "byte",
             "data"     => "_string[1024]"]);

        //clean up
        $time = time();
        if ((int)cms_cache("core.$datapath.time") < $time)
        {
            cms_cache("core.$datapath.time", $time + 900, TRUE); //15 minutes
            $this->clean();
        };
    }

//methods
//--------------------------------------------------------------------------------------------------

    function unique_name($name)
    {
        if (! $this->enabled) return FALSE;

        $result = TRUE;

        //backup resource position
        $profile_offset = $this->profile->offset;

        //set resource lock
        $this->profile->lock();

        //clean up name
        $name = utf8_substr(stripspaces($name), 0, 40);

        //pass through profiles having equal name
        $this->profile->reset();
        while ($this->profile->next(["name" => $name]))
        {
            //ignore self
            $guid = $this->profile->get("guid");
            if (streq($guid, $this->guid)) continue; //skip

            $result = FALSE;
            break;
        };

        //remove resource lock
        $this->profile->unlock();

        //restore resource position
        $this->profile->offset = $profile_offset;

        return $result;
    }

//--------------------------------------------------------------------------------------------------

    function connect($name = "")
    {
        if (! $this->enabled) return FALSE;

        //backup resource position
        $index_offset = $this->index->offset;

        //exit if user is already connected (id is in use)
        if ($this->index->seek(["guid" => $this->guid]))
        {
            //restore resource position
            $this->index->offset = $index_offset;

            return FALSE;
        };

        //backup resource position
        $profile_offset = $this->profile->offset;

        //set resource lock
        $this->index->lock();
        $this->profile->lock();
        $this->status->lock();

        //recurring user
        if ($this->profile->seek(["guid" => $this->guid]))
        {
            $name = $this->profile->get("name");
        }

        //guest
        elseif (streq(CMS_USER, "anonymous"))
        {
            //clean up name
            $name = utf8_substr(stripspaces($name), 0, 40);

            //name is available
            if ($this->unique_name($name))
            {
                //generate random color
                $color = sprintf("#%02x%02x%02x", rand(0, 255), rand(0, 255), rand(0, 255));

                //set profile
                $this->profile->next(["guid" => NULL], TRUE);
                $this->profile->set(
                    ["guid"  => $this->guid,
                     "name"  => $name,
                     "color" => $color]);
            }

            //name already used
            else
            {
                //remove resource lock
                $this->index->unlock();
                $this->profile->unlock();

                //restore resource position
                $this->index->offset   = $index_offset;
                $this->profile->offset = $profile_offset;

                return FALSE;
            };
        }

        //new user
        else
        {
            //clean up name
            $_name = utf8_substr(stripspaces(CMS_NAME), 0, 40);

            //make name unique
            $name = $_name;
            $i    = 0;

            while (! $this->unique_name($name))
                    $name = utf8_substr($_name, 0, 39 - strlen(++$i)) . "_$i";

            //set profile
            $this->profile->next(["guid" => NULL], TRUE);
            $this->profile->set(
                ["guid" => $this->guid,
                 "name" => $name]);
        };

        //find first free index space and insert user
        $this->index->next(["guid" => NULL], TRUE);
        $this->index->set(
            ["guid"  => $this->guid,
             "time"  => time()]);

        //set status
        if ($this->operator)
        {
                $this->status(CMS_CORE_STATUS_OWNER,    NULL, NULL, TRUE);
                $this->status(CMS_CORE_STATUS_OPERATOR, NULL, NULL, TRUE);
        };

        //inform channel
        $this->send(sprintf(CMS_L_CORE_001, $name), NULL, CMS_CORE_DATA_SYSTEM);

        //remove resource lock
        $this->index->unlock();
        $this->profile->unlock();
        $this->status->unlock();

        //restore resource position
        $this->index->offset   = $index_offset;
        $this->profile->offset = $profile_offset;

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function disconnect($clean = TRUE)
    {
        if (! $this->enabled) return FALSE;

        $this->index->lock();
        if (stre($guid = $this->index->get("guid")))
        {
            $this->index->unlock();
            return FALSE; //no user
        };

        //backup resource position
        $index_offset   = $this->index->offset;
        $profile_offset = $this->profile->offset;
        $status_offset  = $this->status->offset;
        $data_offset    = $this->data->offset;

        //set resource lock
        $this->index->lock();
        $this->profile->lock();
        $this->status->lock();
        $this->data->lock();

        $channel = $this->index->get("channel");
        $name    = FALSE;

        //inform channel
        if ($this->profile->seek(["guid" => $guid]))
        {
            //temporarily use guid of disconnecting user
            $this->switch_guid($guid);

            //send disconnect message
            $name = $this->profile->get("name");
            $this->send(sprintf(CMS_L_CORE_002, $name), NULL, CMS_CORE_DATA_SYSTEM);

            $this->reset_guid(); //restore guid
        };

        //delete user
        $this->index->del();;

        if ($name !== FALSE)
        {
            //check if user is banned
            $banned = FALSE;

            $this->status->reset();
            while ($this->status->next(["guid" => $guid]))
            {
                if (flag($this->status->get("status"), CMS_CORE_STATUS_BANNED))
                {
                    $banned = TRUE;
                    break;
                };
            };

            //delete profile if user is neither banned nor permanent
            if ((! $banned) && (substr($guid, 0, 1) !== "!") && $clean)
            {
                $this->delete_profile($guid, TRUE);
                //$this->delete_status($guid);
            };
        };

        //delete messages
        while ($this->data->seek(["receiver" => $guid])) $this->data->del();

        //remove resource lock
        $this->index->unlock();
        $this->profile->unlock();
        $this->status->unlock();
        $this->data->unlock();

        //restore resource position
        $this->index->offset   = $index_offset;
        $this->profile->offset = $profile_offset;
        $this->status->offset  = $status_offset;
        $this->data->offset    = $data_offset;

        //clean up status
        $this->update_status($channel);

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function get_status($guid    = NULL,
                        $channel = NULL)
    {
        if ($this->status === NULL) return FALSE;

        if ($guid === NULL) $guid = $this->guid;

        //backup resource position
        $status_offset = $this->status->offset;

        //set resource lock
        $this->status->lock();

        //initialize status
        $status = CMS_CORE_STATUS_NONE;

        //get global status
        if ($this->status->seek(["guid"    => $guid,
                                 "channel" => NULL]))
            $status = $this->status->get("status");

        //add specific channel status
        if (
             nstre($channel)
             &&
             $this->status->seek(["guid"    => $guid,
                                  "channel" => $channel],
                                 TRUE)
           )
            $status |= $this->status->get("status");

        //remove resource lock
        $this->status->unlock();

        //restore resource position
        $this->status->offset = $status_offset;

        return $status;
    }

//--------------------------------------------------------------------------------------------------

    function set_status($status  = CMS_CORE_STATUS_NONE,
                        $guid    = NULL,
                        $channel = NULL)
    {
        if (! $this->enabled) return FALSE;

        if ($guid === NULL) $guid = $this->guid;

        //backup resource position
        $status_offset = $this->status->offset;

        //set resource lock
        $this->status->lock();

        //seek resource position
        $this->status->next(
            ["guid"    => $guid,
             "channel" => $channel],
            TRUE);

        //set status
        $this->status->set(
            ["guid"     => $guid,
             "channel"  => $channel,
             "status"   => $status]);

        //remove resource lock
        $this->status->unlock();

        //restore resource position
        $this->status->offset = $status_offset;

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function delete_status($guid = NULL)
    {
        if (! $this->enabled) return FALSE;

        if ($guid === NULL) $guid = $this->guid;

        //backup resource position
        $status_offset = $this->status->offset;

        //set resource lock
        $this->status->lock();

        //remove all status information related to guid
        while ($this->status->seek(["guid" => $guid]))
            $this->status->del();

        //remove resource lock
        $this->status->unlock();

        //restore resource position
        $this->status->offset = $status_offset;

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function update_status($channel)
    {
        if (! $this->enabled) return FALSE;

        if (stre($channel)) return FALSE; //no channel

        //backup resource position
        $index_offset   = $this->index->offset;
        $channel_offset = $this->channel->offset;
        $status_offset  = $this->status->offset;

        //set resource lock
        $this->index->lock();
        $this->channel->lock();
        $this->status->lock();

        if (

        //no temporary channel
             (! $this->index->seek(["channel" => $channel]))
             &&

        //no permanent channel
             (! $this->channel->seek(["channel" => $channel]))
           )
        {
            //remove channel status
            while ($this->status->seek(["channel" => $channel])) $this->status->del();

            $result = TRUE;
        };

        //remove resource lock
        $this->index->unlock();
        $this->channel->unlock();
        $this->status->unlock();

        //restore resource position
        $this->index->offset   = $index_offset;
        $this->channel->offset = $channel_offset;
        $this->status->offset  = $status_offset;

        return isset($result);
    }

//--------------------------------------------------------------------------------------------------

    function update_index($query_connection = FALSE)
    {
        if (! $this->enabled)          return FALSE;
        if ((int)$this->timeout === 0) return FALSE; //no timeout

        //backup resource position
        $index_offset = $this->index->offset;

        //set resource lock
        $this->index->lock();

        $time    = time();
        $timeout = $time - $this->timeout;

        $this->index->reset();
        while ($this->index->next(["guid" => TRUE]))
        {
            //update own time
            if ((! $query_connection) && streq($this->index->get("guid"), $this->guid))
                $this->index->set(["time" => $time]);

            //disconnect on timeout
            elseif ($this->index->get("time") < $timeout)
                $this->disconnect(FALSE);
        };

        //remove resource lock
        $this->index->unlock();

        //restore resource position
        $this->index->offset = $index_offset;

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function connect_channel($channel,
                             $password = "")
    {
        if (! $this->enabled) return FALSE;

        //backup resource position
        $channel_offset = $this->channel->offset;

        //set resource lock
        $this->channel->lock();

        do
        {
            //administrators are always permitted
            if ($this->operator) break;

            $status = $this->get_status(NULL, $channel);

            //owners are always permitted
            if (flag($status, CMS_CORE_STATUS_OWNER)) break;

            //operators are always permitted
            if (flag($status, CMS_CORE_STATUS_OPERATOR)) break;

            //banned
            if (! flag($status, CMS_CORE_STATUS_BANNED))
            {
                //no matching permanent channel
                if (! $exists = $this->channel->seek(["channel" => $channel])) break;

                //no password
                if (stre($_password = $this->channel->get("password"))) break;

                //correct password given
                if (streq($password, $_password)) break;
            };

            //release resource lock
            $this->channel->unlock();

            //restore resource position
            $this->channel->offset = $channel_offset;

            return FALSE;
        }
        while (FALSE);

        //backup resource position
        $index_offset   = $this->index->offset;

        //set resource lock
        $this->index->lock();

        //get current channel
        $this->index->seek(["guid" => $this->guid]);
        $_channel = $this->index->get("channel");

        //check if target and current channel are the same
        if (streq($channel, $_channel))
        {
            //release resource lock
            $this->index->unlock();
            $this->channel->unlock();

            //restore resource position
            $this->index->offset   = $index_offset;
            $this->channel->offset = $channel_offset;

            return FALSE; //skip
        };

        //notify current channel
        $profile = $this->get_profile();
        $this->send(sprintf(CMS_L_CORE_004,
                            $profile["name"],
                            stre($_channel) ? CMS_CORE_PORTAL : $_channel),
                    NULL,
                    CMS_CORE_DATA_SYSTEM);

        //assume new temporary channel if not existing
        do
        {
            //permanent channel already exists
            if (! empty($exists))                              break;
            if ($this->channel->seek(["channel" => $channel])) break;

            //temporary channel already exists
            if ($this->index->seek(["channel" => $channel]))   break;

            //set resource lock
            $this->status->lock();

            //grant superior status
            $this->status(CMS_CORE_STATUS_OWNER,    NULL, $channel, TRUE);
            $this->status(CMS_CORE_STATUS_OPERATOR, NULL, $channel, TRUE);

            //release resource lock
            $this->status->unlock();
        }
        while (FALSE);

        //set channel
        $this->index->seek(["guid"   => $this->guid]);
        $this->index->set(["channel" => $channel]);

        //notify target channel
        $this->send(sprintf(CMS_L_CORE_003,
                            $profile["name"],
                            stre($channel) ? CMS_CORE_PORTAL : $channel),
                    NULL,
                    CMS_CORE_DATA_SYSTEM);

        //release resource lock
        $this->index->unlock();
        $this->channel->unlock();

        //restore resource position
        $this->index->offset   = $index_offset;
        $this->channel->offset = $channel_offset;

        //clean up previous channel status
        $this->update_status($_channel);

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function create_channel($channel,
                            $text     = "",
                            $password = "")
    {
        if (! $this->enabled) return FALSE;

        if (stre($channel = utf8_trim($channel))) return FALSE; //no channel name
        if (streq($channel, CMS_CORE_PORTAL))     return FALSE; //channel name not allowed

        //backup resource position
        $channel_offset = $this->channel->offset;

        //set resource lock
        $this->channel->lock();

        if (

        //must be administrator
             $this->operator
             &&

        //permanent channel may not exist yet
             (! $this->channel->next(["channel" => $channel], TRUE))
           )
        {
            //set channel information
            $this->channel->set(
                ["channel"  => $channel,
                 "text"     => $text,
                 "password" => $password]);

            //grant superior status
            $this->status->lock();
            $this->status(CMS_CORE_STATUS_OWNER,    NULL, $channel, TRUE);
            $this->status(CMS_CORE_STATUS_OPERATOR, NULL, $channel, TRUE);
            $this->status->unlock();

            //inform user
            $this->send(sprintf(CMS_L_CORE_005, $channel),
                        $this->guid,
                        CMS_CORE_DATA_SYSTEM);

            $return = TRUE;
        };

        //release resource lock
        $this->channel->unlock();

        //restore resource position
        $this->channel->offset = $channel_offset;

        return isset($return);
    }

//--------------------------------------------------------------------------------------------------

    function set_channel($channel,
                         $text     = "",
                         $password = "")
    {
        if (! $this->enabled) return FALSE;

        //backup resource position
        $channel_offset = $this->channel->offset;

        //set resource lock
        $this->channel->lock();

        if (

        //must be permanent channel
             $this->channel->seek(["channel" => $channel])
             &&

        //must be administrator or owner
             ($this->operator || flag($this->get_status(NULL, $channel), CMS_CORE_STATUS_OWNER))
           )
        {
            //set channel information
            $this->channel->set(
                 ["text"     => $text,
                  "password" => $password]);

            //inform user
            $this->send(sprintf(CMS_L_CORE_006, $channel),
                        $this->guid,
                        CMS_CORE_DATA_SYSTEM);

            $return = TRUE;
        };

        //remove resource lock
        $this->channel->unlock();

        //restore resource position
        $this->channel->offset = $channel_offset;

        return isset($return);
    }

//--------------------------------------------------------------------------------------------------

    function delete_channel($channel)
    {
        if (! $this->enabled) return FALSE;

        //backup resource position
        $channel_offset = $this->channel->offset;

        //set resource lock
        $this->channel->lock();

        //find channel
        $this->channel->reset();
        while ($this->channel->next(["channel" => $channel]))
        {
            //must be administrator or owner
            if (
                 $this->operator
                 ||
                 flag($this->get_status(NULL, $channel), CMS_CORE_STATUS_OWNER)
               )
            {
                //remove channel
                $this->channel->del();
                $return = TRUE;
            };
        };

        //remove resource lock
        $this->channel->unlock();

        //restore resource position
        $this->channel->offset = $channel_offset;

        if (isset($return))
        {
            //inform user
            $this->send(sprintf(CMS_L_CORE_007, $channel),
                        $this->guid,
                        CMS_CORE_DATA_SYSTEM);

            //clean up channel status
            $this->update_status($channel);

            return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function send($data,
                  $receiver = NULL,
                  $status   = CMS_CORE_DATA_DEFAULT)
    {
        if (! $this->enabled) return FALSE;

        //backup resource position
        $index_offset = $this->index->offset;

        //set resource lock
        $this->index->lock();

        //check if sender is connected
        if (! $this->index->seek(["guid" => $this->guid]))
        {
            //release resource lock
            $this->index->unlock();

            //restore resource position
            $this->index->offset = $index_offset;

            return FALSE;
        };

        //get status
        $channel         = $this->index->get("channel");
        $_status         = $this->get_status(NULL, $channel);
        $status_override = $this->operator ||
                           flag($status,  CMS_CORE_DATA_SYSTEM) ||
                           flag($_status, CMS_CORE_STATUS_OWNER) ||
                           flag($_status, CMS_CORE_STATUS_OPERATOR);

        //check if user is muted
        if ((! $status_override) && flag($_status, CMS_CORE_STATUS_MUTE))
        {
            //release resource lock
            $this->index->unlock();

            //restore resource position
            $this->index->offset = $index_offset;

            return FALSE;
        };

        //receiver given, private message
        if ($receiver !== NULL) $status |= CMS_CORE_DATA_PRIVATE;

        $status_private   = flag($status, CMS_CORE_DATA_PRIVATE);
        $status_invisible = flag($_status, CMS_CORE_STATUS_INVISIBLE);

        //revoke absent status
        if ($status === CMS_CORE_DATA_DEFAULT) $this->status(-CMS_CORE_STATUS_ABSENT);

        //backup resource position
        $data_offset = $this->data->offset;

        //set resource lock
        $this->data->lock();

        //pass through channel participants
        $this->data->reset();
        $this->index->reset();
        while ($this->index->next(["channel" => $channel]))
        {
            $_receiver = $this->index->get("guid");
            $sender    = FALSE;
            $spy       = FALSE;

            //private message
            do
            {
                //not a private message
                if (! $status_private) break;

                //check if user is in receiver list
                $match = is_array($receiver) ?
                         in_array($_receiver, $receiver) :
                         streq($_receiver, $receiver);
                if ($match) break;

                //sender
                if ($sender = streq($_receiver, $this->guid)) break;

                //skip system messages
                if (flag($status, CMS_CORE_DATA_SYSTEM)) continue 2;

                //spy
                $__status = $this->get_status($_receiver, $channel);
                if ($spy = flag($__status, CMS_CORE_STATUS_SPY)) break;

                continue 2; //skip
            }
            while (FALSE);

            //invisible sender
            do
            {
                //not invisible
                if (! $status_invisible) break;

                //sender
                if ($sender) break;

                //receiver is invisible, too
                $__status = $this->get_status($_receiver, $channel);
                if (flag($__status, CMS_CORE_STATUS_INVISIBLE)) break;

                continue 2; //skip
            }
            while (FALSE);

            //seek empty data slot
            $this->data->next(["receiver" => NULL]);

            //spy
            if ($sender || $spy)
            {
                //put receiver
                $this->data->set(
                    ["guid"     => $this->guid,
                     "receiver" => $_receiver,
                     "status"   => $status |
                                   ($spy ?
                                   CMS_CORE_DATA_SPY_META :
                                   CMS_CORE_DATA_PRIVATE_META),
                     "data"     => is_array($receiver) ?
                                   implode("\n", $receiver) :
                                   $receiver]);

                //seek empty data slot
                $this->data->next(["receiver" => NULL]);

                //put message
                $this->data->set(
                    ["guid"     => $this->guid,
                     "receiver" => $_receiver,
                     "status"   => $status |
                                   ($spy ?
                                   CMS_CORE_DATA_SPY_DATA :
                                   CMS_CORE_DATA_PRIVATE_DATA),
                     "data"     => $data]);
                continue;
            };

            //put message
            $this->data->set(
                ["guid"     => $this->guid,
                 "receiver" => $_receiver,
                 "status"   => $status |
                               (streq($_receiver, $this->guid) ?
                               CMS_CORE_DATA_RESPONSE :
                               CMS_CORE_DATA_DEFAULT),
                 "data"     => $data]);
        };

        //release resource lock
        $this->index->unlock();
        $this->data->unlock();

        //restore resource position
        $this->index->offset = $index_offset;
        $this->data->offset  = $data_offset;

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function receive($delete = TRUE)
    {
        if (! $this->enabled) return FALSE;

        $data = [];

        //backup resource position
        $data_offset = $this->data->offset;

        //set resource lock
        $this->data->lock();

        //collect all messages for current user
        $this->data->reset();
        while ($this->data->next(["receiver" => $this->guid]))
        {
            $data[] = $this->data->get();
            if ($delete) $this->data->del(); //remove message
        };

        //release resource lock
        $this->data->unlock();

        //restore resource position
        $this->data->offset = $data_offset;

        return $data;
    }

//--------------------------------------------------------------------------------------------------

    function get_profile($guid = NULL)
    {
        if (! $this->enabled) return FALSE;

        if ($guid === NULL) $guid = $this->guid;

        //backup resource position
        $profile_offset = $this->profile->offset;

        //set resource lock
        $this->profile->lock();

        //retrieve profile information
        $return = $this->profile->seek(["guid" => $guid]) ?
                  $this->profile->get() : FALSE;

        //remove resource lock
        $this->profile->unlock();

        //restore resource position
        $this->profile->offset = $profile_offset;

        return $return;
    }

//--------------------------------------------------------------------------------------------------

    function set_profile($guid,
                         $name,
                         $color = "",
                         $text  = "",
                         $image = "")
    {
        if (! $this->enabled) return FALSE;

        if ($guid === NULL) $guid = $this->guid;

        //check permission
        do
        {
            //self always permitted
            if (streq($guid, $this->guid)) break;

            //administrators only
            if (! $this->operator) return FALSE;

            //guests always permitted
            if (substr($guid, 0, 1) !== "!") break;

            //administrators are untouchable
            if (cms_permission(CMS_CORE_PERMISSION_OPERATOR,
                               TRUE,
                               TRUE,
                               substr($guid, 1)))
                return FALSE;
        }
        while (FALSE);

        //backup resource position
        $profile_offset = $this->profile->offset;

        //set resource lock
        $this->profile->lock();

        //preserve current settings
        $this->profile->next(["guid" => $guid], TRUE);
        $_image = $this->profile->get("image");
        $_name  = $this->profile->get("name");

        //clean up name
        $name = utf8_substr(stripspaces($name), 0, 40);

        //verify name
        if (stre($name) || (! $this->unique_name($name))) $name = $_name;

        //set new image
        if (nstre($image) && is_file(CMS_DATA_PATH . "core/$image"))
        {
            //remove old image
            if (nstre($_image) && is_file(CMS_DATA_PATH . "core/$_image"))
                unlink(CMS_DATA_PATH . "core/$_image");
        }

        //keep current image
        else
        {
            $image = $_image;
        };

        //set profile
        $this->profile->set(
            ["guid"  => $guid,
             "name"  => $name,
             "color" => $color,
             "text"  => $text,
             "image" => $image]);

        //release resource lock
        $this->profile->unlock();

        //restore resource position
        $this->profile->offset = $profile_offset;

        //inform user
        $this->send(CMS_L_CORE_008, $this->guid, CMS_CORE_DATA_SYSTEM);

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function delete_profile($guid   = NULL,
                            $system = FALSE)
    {
        if (! $this->enabled) return FALSE;

        if ($guid === NULL) $guid = $this->guid;

        //check permission
        do
        {
            //system override
            if ($system) break;

            //administrators only
            if (! $this->operator) return FALSE;

            //self always permitted
            if (streq($guid, $this->guid)) break;

            //guests always permitted
            if (substr($guid, 0, 1) !== "!") break;

            //administrators are untouchable
            if (cms_permission(CMS_CORE_PERMISSION_OPERATOR,
                               TRUE,
                               TRUE,
                               substr($guid, 1)))
                return FALSE;
        }
        while (FALSE);

        //backup resource position
        $profile_offset = $this->profile->offset;

        //set resource lock
        $this->profile->lock();

        //find profile
        while ($this->profile->seek(["guid" => $guid]))
        {
            //remove image
            if (
                 nstre($image = $this->profile->get("image"))
                 &&
                 is_file(CMS_DATA_PATH . "core/$image")
               )
                unlink(CMS_DATA_PATH . "core/$image");

            //remove profile
            $this->profile->del();
        };

        //release resource lock
        $this->profile->unlock();

        //restore resource position
        $this->profile->offset = $profile_offset;

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function status($value   = CMS_CORE_STATUS_NONE,
                    $guid    = NULL,
                    $channel = NULL,
                    $system  = FALSE,
                    $test    = FALSE)
    {
        if (! $this->enabled) return FALSE;

        if ($guid === NULL) $guid = $this->guid;

        $set    = $value > 0;                //status flag
        $value  = abs($value);               //status value
        $self   = streq($guid, $this->guid); //self flag
        $result = TRUE;

        //backup resource position
        $status_offset = $this->status->offset;

        //set resource lock
        $this->status->lock();

        //check if status change is possible
        $status = CMS_CORE_STATUS_NONE;

        if ($this->status->seek(["guid"    => $guid,
                                 "channel" => $channel]))
        {
            //get target channel status
            $status = $this->status->get("status");

            if ($set === flag($status, $value)) $result = FALSE;
        };

        //release resource lock
        $this->status->unlock();

        //restore resource position
        $this->status->offset = $status_offset;

        if (! $result) return FALSE;

        //check permission
        if (! $system) //no system override
        {
            //general check
            do
            {
                //self always permitted
                if ($self) break;

                //guests always permitted
                if (substr($guid, 0, 1) !== "!") break;

                //administrators are untouchable
                if (cms_permission(CMS_CORE_PERMISSION_OPERATOR,
                                   TRUE,
                                   TRUE,
                                   substr($guid, 1)))
                    return FALSE;
            }
            while (FALSE);

            //specific check
            switch ($value)
            {
            case CMS_CORE_STATUS_OWNER:
            case CMS_CORE_STATUS_OPERATOR:
//..................................................................................................

                //may only be applied to others
                if ($self) return FALSE;

                $_status = $this->get_status($guid, $channel);

                //may not be applied to muted users
                if (flag($_status, CMS_CORE_STATUS_MUTE)) return FALSE;

                //may not be applied to banned users
                if (flag($_status, CMS_CORE_STATUS_BANNED)) return FALSE;

                //may be applied by administrators
                if ($this->operator) break;

                //may be applied by channel owners
                $_status = $this->get_status(NULL, $channel);
                if (flag($_status, CMS_CORE_STATUS_OWNER)) break;

                return FALSE;

            case CMS_CORE_STATUS_SPY:
            case CMS_CORE_STATUS_INVISIBLE:
//..................................................................................................

                //may only be applied to self
                if (! $self) return FALSE;

                //may be applied by administrators
                if ($this->operator) break;

                return FALSE;

            case CMS_CORE_STATUS_ABSENT:
//..................................................................................................

                //may only be applied to self
                if (! $self) return FALSE;

                $channel = NULL; //always global
                break;

            case CMS_CORE_STATUS_MUTE:
            case CMS_CORE_STATUS_BANNED:
//..................................................................................................

                //may only be applied to others
                if ($self) return FALSE;

                $_status = $this->get_status($guid, $channel);

                //may not be applied to owners
                if (flag($_status, CMS_CORE_STATUS_OWNER)) return FALSE;

                //may not be applied to operators
                if (flag($_status, CMS_CORE_STATUS_OPERATOR)) return FALSE;

                //may be applied by administrators
                if ($this->operator) break;

                $_status = $this->get_status(NULL, $channel);

                //may be applied by owners
                if (flag($_status, CMS_CORE_STATUS_OWNER)) break;

                //may be applied by operators
                if (flag($_status, CMS_CORE_STATUS_OPERATOR)) break;

                return FALSE;

            default:
//..................................................................................................

                //invalid status
                return FALSE;
            };
        };

        //just testing
        if ($test) return TRUE;

        //backup resource position
        $index_offset  = $this->index->offset;
        $status_offset = $this->status->offset;

        //set resource lock
        $this->index->lock();
        $this->status->lock();

        //change status
        $_status = $set ? $status | $value : $status & ~$value;

        //check if target user channel is influenced by status change
        $_channel = NULL;
        $effect   = FALSE;

        //there are three combinations that allow effect
        //1. target and current channel are both portal
        //2. only target channel is portal
        //3. target and current channel are not portal
        //there is one other combination that never allows effect
        //4. only current channel is portal

        //get channel of target user
        if ($this->index->seek(["guid" => $guid]))
        {
            $_channel = $this->index->get("channel");
            $__status = CMS_CORE_STATUS_NONE;

            //target channel is portal
            if (stre($channel))
            {
                //current channel is portal, too
                if (stre($_channel))
                {
                    $effect = TRUE; //always
                }

                //get status of current channel
                else
                {
                    if ($this->status->seek(["guid"    => $guid,
                                             "channel" => $_channel]))
                        $__status = $this->status->get("status");

                    $effect = ($status | $__status) !== ($_status | $__status);
                };
            }

            //target channel and current channel are equal and not portal
            elseif (streq($channel, $_channel))
            {
                //get status of portal
                if ($this->status->seek(["guid"    => $guid,
                                         "channel" => NULL]))
                    $__status = $this->status->get("status");

                $effect = ($__status | $status) !== ($__status | $_status);
            };
        };

        //response
        $receiver = NULL;

        switch ($value)
        {
        case CMS_CORE_STATUS_OWNER:
//..................................................................................................

            $response = CMS_L_CORE_010;
            break;

        case CMS_CORE_STATUS_OPERATOR:
//..................................................................................................

            $response = CMS_L_CORE_011;
            break;

        case CMS_CORE_STATUS_SPY:
//..................................................................................................

            $response = CMS_L_CORE_015;
            $receiver = $guid;
            break;

        case CMS_CORE_STATUS_INVISIBLE:
//..................................................................................................

            //send false disconnect message
            if ($set && $effect)
            {
                $profile = $this->get_profile($guid);
                $this->send(stre($channel) ?
                            sprintf(CMS_L_CORE_002, $profile["name"]) :
                            sprintf(CMS_L_CORE_004, $profile["name"], $_channel),
                            NULL,
                            CMS_CORE_DATA_SYSTEM);
            };

            $response = CMS_L_CORE_017;
            $receiver = $guid;
            break;

        case CMS_CORE_STATUS_ABSENT:
//..................................................................................................

            $response = CMS_L_CORE_012;
            break;

        case CMS_CORE_STATUS_MUTE:
//..................................................................................................

            $response = CMS_L_CORE_013;
            break;

        case CMS_CORE_STATUS_BANNED:
//..................................................................................................

            $response = CMS_L_CORE_014;
            break;
        };

        //inform about status change
        if (nstre($response))
        {
            //compose message
            $profile = $this->get_profile($guid);
            $message = sprintf($set           ? CMS_L_CORE_009  : CMS_L_CORE_016,
                               $profile["name"],
                               stre($channel) ? CMS_CORE_PORTAL : $channel,
                               $response);

            //temporarily use guid of target user
            $this->switch_guid($guid);

            //inform channel
            if ($effect)
                $this->send($message, $receiver, CMS_CORE_DATA_SYSTEM);

            //inform target user only
            else
                $this->send($message, $guid, CMS_CORE_DATA_SYSTEM);

            //ban user
            if (($value === CMS_CORE_STATUS_BANNED) && $set && $effect)
            {
                //kick out of channel
                if (nstre($channel))
                    $this->connect_channel(NULL);

                //disconnect
                elseif ($this->index->seek(["guid" => $guid]))
                    $this->disconnect(FALSE);
            };

            $this->reset_guid(); //restore guid

            //inform initiator
            do
            {
                //target user already received message
                if ($self) break;

                //impacted channel already received message
                if (
                     $effect
                     &&
                     $this->index->seek(["guid" => $this->guid])
                     &&
                     streq($this->index->get("channel"), $_channel)
                   )
                    break;

                //send message
                $this->send($message, $this->guid, CMS_CORE_DATA_SYSTEM);
            }
            while (FALSE);
        };

        //save status
        $this->set_status($_status, $guid, $channel);

        //send false connect message
        if (($value === CMS_CORE_STATUS_INVISIBLE) && (! $set) && $effect)
        {
            $profile = $this->get_profile($guid);
            $this->send(stre($channel) ?
                        sprintf(CMS_L_CORE_001, $profile["name"]) :
                        sprintf(CMS_L_CORE_003, $profile["name"], $_channel),
                        NULL,
                        CMS_CORE_DATA_SYSTEM);
        };

        //release resource lock
        $this->index->unlock();
        $this->status->unlock();

        //restore resource position
        $this->index->offset  = $index_offset;
        $this->status->offset = $status_offset;

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function clean()
    {
        if (! $this->enabled) return FALSE;

        //backup resource position
        $index_offset   = $this->index->offset;
        $profile_offset = $this->profile->offset;
        $status_offset  = $this->status->offset;
        $data_offset    = $this->data->offset;

        //set resource lock
        $this->index->lock();
        $this->profile->lock();
        $this->status->lock();
        $this->data->lock();

        $list = []; //account list

        //retrieve list of basic user accounts
        $permission = new permission();

        foreach ($permission->data->data AS $key => $_)
        {
            if (nstreq(substr($key, 0, 5), "user.")) continue; //no user

            //strip prefix and add to list
            $key        = substr($key, 5);
            $list[$key] = TRUE;
        };

        //retrieve list of extended user accounts
        new mysql(); //establish database connection

        if (
             cms_load("profile")
             &&
             ($result = mysql_query("SELECT " . CMS_DB_PROFILE_USER . " " .
                                    "FROM " .   CMS_DB_PROFILE))
           )
        {
            while ($resultrow = mysql_fetch_row($result))
                $list[$resultrow[0]] = TRUE;
        };

        //clean profiles
        $this->profile->reset();
        while ($this->profile->next(["guid" => TRUE]))
        {
            //get guid
            $guid = $this->profile->get("guid");

            //permanent user
            if (substr($guid, 0, 1) === "!")
            {
                //check if user account still exists
                $user = substr($guid, 1);
                if (isset($list[$user])) continue; //skip
            };

            //check if user is currently connected
            if ($this->index->seek(["guid" => $guid])) continue; //skip

            //remove image
            if (
                 nstre($image = $this->profile->get("image"))
                 &&
                 is_file(CMS_DATA_PATH . "core/$image")
               )
                unlink(CMS_DATA_PATH . "core/$image");

            //remove profile
            $this->profile->del();
        };

        //clean status
        $this->status->reset();
        while ($this->status->next(["guid" => TRUE]))
        {
            //get guid
            $guid = $this->status->get("guid");

            //permanent user
            if (substr($guid, 0, 1) === "!")
            {
                //check if user account still exists
                $user = substr($guid, 1);
                if (isset($list[$user])) continue; //skip
            };

            //check if user is currently connected
            if ($this->index->seek(["guid" => $guid])) continue; //skip

            //remove status
            $this->status->del();
        };

        //clean data
        $this->data->reset();
        while ($this->data->next(["receiver" => TRUE]))
        {
            //get guid
            $guid = $this->data->get("receiver");

            //check if user is currently connected
            if ($this->index->seek(["guid" => $guid])) continue; //skip

            //remove data
            $this->data->del();
        };

        //remove resource lock
        $this->index->unlock();
        $this->profile->unlock();
        $this->status->unlock();
        $this->data->unlock();

        //restore resource position
        $this->index->offset   = $index_offset;
        $this->profile->offset = $profile_offset;
        $this->status->offset  = $status_offset;
        $this->data->offset    = $data_offset;

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function switch_guid($guid,
                         $reset = FALSE)
    {
        static $stack = [];

        //restore previous settings
        if ($reset)
        {
            $array = array_pop($stack);
            if ($array !== NULL) list($this->guid, $this->operator) = $array;
            return;
        };

        //backup current settings
        $stack[] = [$this->guid, $this->operator];

        //set new information
        $this->guid     = $guid;
        $this->operator = (substr($guid, 0, 1) === "!") &&
                          cms_permission(CMS_CORE_PERMISSION_OPERATOR,
                                         TRUE,
                                         TRUE,
                                         substr($guid, 1));
    }

//--------------------------------------------------------------------------------------------------

    function reset_guid()
    {
        $this->switch_guid(NULL, TRUE);
    }
};