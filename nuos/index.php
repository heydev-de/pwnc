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

require("pwnc.inc");

(function() {

    header("Location: " . cms_url(CMS_MODULES_URL . "desktop.php"), TRUE, 303);
    exit();

})();