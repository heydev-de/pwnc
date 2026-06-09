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

require("../pwnc.inc");

(function() {

set_error_handler(function() { return TRUE; });

//establish database connection
$mysql = new mysql();
while ($mysql->connection())
{
    //get current bot status
    $query  = "SELECT " . CMS_DB_LOG_USER_BOT . " " .
              "FROM " .   CMS_DB_LOG_USER . " " .
              "WHERE " .  CMS_DB_LOG_USER_USERID .
              "='" .      sqlesc(CMS_IPHASH) . "' " .
              "LIMIT 1";
    $result = mysql_query($query);
    if (
         (! $result) //error
         ||
         (($resultrow = mysql_fetch_row($result)) === FALSE) //no entry
         ||
         ((int)$resultrow[0] !== CMS_LOG_STATUS_BOT_PROVISIONAL)
       )
        break; //no further processing

    $log = new log();
    $log->user(NULL, NULL, NULL, CMS_LOG_STATUS_USER_PROVISIONAL, NULL, FALSE);
    break;
};

//return black pixel
while (ob_get_level()) ob_end_clean();
header("Content-Type: image/gif");
header("X-Robots-Tag: noindex");
echo(base64_decode("R0lGODlhAQABAIAAAAUEBAAAACwAAAAAAQABAAACAkQBADs="));
exit();

})();