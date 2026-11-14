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
         t("security_validation") .
         "</h1>" .
         "<p>" .
         sprintf(t("err_csrf_validation"),
                 stre($location) ? t("unknown_address") : x($location)) .
         "</p>");
    if (nstre($location))
        echo("<a href=\"" . x(cms_url($location)) . "\">" .
             t("ignore_and_continue") .
             "</a> | ");
    echo("<a href=\"" . x(cms_url(CMS_ROOT_URL . "index.php")) . "\">" .
         t("go_to_home_page") .
         "</a>" .
         "</div>");
};

echo("<section>" .
     "</body>" .
     "</html>");

exit();

})();