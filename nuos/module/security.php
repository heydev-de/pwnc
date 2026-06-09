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

global $event,
       $location;

echo(CMS_DOCTYPE_HTML .
     "<html>" .
     "<head>" .
     CMS_HTML_HEADER . CMS_STYLESHEET .
     "</head>" .
     "<body class=\"" . x(CMS_CLASS) . "\">" .
     "<section>");

switch ($event)
{
default:
case CMS_SECURITY_EVENT_CSRF:

    echo("<div>" .
         "<h1>" .
         CMS_L_MOD_SECURITY_001 .
         "</h1>" .
         "<p>" .
         sprintf(CMS_L_MOD_SECURITY_002,
                 stre($location) ? CMS_L_MOD_SECURITY_003 : x($location)) .
         "</p>");
    if (nstre($location))
        echo("<a href=\"" . x(cms_url($location)) . "\">" .
             CMS_L_MOD_SECURITY_005 .
             "</a> | ");
    echo("<a href=\"" . x(cms_url(CMS_ROOT_URL)) . "\">" .
         CMS_L_MOD_SECURITY_004 .
         "</a>" .
         "</div>");
};

echo("<section>" .
     "</body>" .
     "</html>");

exit();

})();