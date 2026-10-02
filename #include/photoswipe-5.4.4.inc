<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

?><script type="module">

var photoswipe_active = true;
load_css("<?php echo(x(CMS_ROOT_URL));?>3rdparty/photoswipe-5.4.4/photoswipe.css");
import { PhotoSwipe, photoswipe_init } from "<?php echo(x(CMS_ROOT_URL));?>3rdparty/photoswipe-5.4.4/photoswipe.esm.min.js";
photoswipe_init();

</script>