<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

function notify_admin($string)
{
    if (! cms_load("core_resource")) return FALSE;

    //create core object
    $ims    = new core_resource(
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
    $thread = unique_id(8);
    $time   = time();
    $string = CMS_L_COMMON_001 . ": $string";
    $hash   = hash32($thread . $time . "admin" . $string);

    //create message
    $ims->seek(["owner" => NULL], TRUE);
    $ims->set(
        ["id"       => unique_id(8),
         "thread"   => $thread,
         "time"     => $time,
         "owner"    => "admin",
         "receiver" => "admin",
         "sender"   => "admin",
         "status"   => 0,
         "text"     => $string,
         "hash"     => $hash]);

    //create notification file
    if ($hfile = fopen(CMS_DATA_PATH . "#desktop/admin/ims.flag", "wb"))
    {
        flock($hfile, LOCK_EX);
        fwrite($hfile, 1);
        fclose($hfile);
    };

    return TRUE;
};