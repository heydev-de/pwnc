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

if (empty($_COOKIE["cms_check_cookie"]))
{
    //check for cookie availability
    if (
         (! isset($_GET["cms_check_cookie"]))
         &&
         cms_set_cookie(["cms_check_cookie" => 1])
       )
    {
        global $location;

        $location = cms_url(["cms_check_cookie" => 1,
                             "location"         => $location]);

        //refresh has to be used instead of location
        //in order to redirect or cookie might not be set
        header("Refresh: 0; url=$location");

        echo(CMS_DOCTYPE_HTML .
             "<html>" .
             "<head>" .
             CMS_HTML_HEADER . CMS_STYLESHEET .
             "</head>" .

             "<body class=\"" . x(CMS_CLASS) . "\">" .
             "<div>" .
             sprintf(CMS_L_MOD_IDENTIFICATION_007, x($location)) .
             "</div>" .
             "</body>" .
             "</html>");
    }

    //cookies not available
    else
    {
        echo(CMS_DOCTYPE_HTML .
             "<html>" .
             "<head>" .
             CMS_HTML_HEADER . CMS_STYLESHEET .
             "</head>" .

             "<body class=\"" . x(CMS_CLASS) . "\">" .
             "<div>" .
             CMS_L_NOCOOKIE .
             "</div>" .
             "</body>" .
             "</html>");
    };
}

//cookies available, show identification prompt
else
{
    echo(CMS_DOCTYPE_HTML .
         "<html>" .
         "<head>" .
         CMS_HTML_HEADER . CMS_JAVASCRIPT . CMS_STYLESHEET .
         "</head>" .

         "<body class=\"" . x(CMS_CLASS) . "\">");

    //identification prompt
    (function() { require(CMS_MODULES_PATH . "#module/mod.identification.inc"); })();

    echo("</body>" .
         "</html>");
};

})();