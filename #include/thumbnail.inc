<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

?><script>

load_css("<?php echo(x(CMS_ROOT_URL));?>3rdparty/thumbnail/thumbnail.css");
load_script("<?php echo(x(CMS_ROOT_URL));?>3rdparty/thumbnail/thumbnail.js", function() { thumbnail_init(); });

</script>