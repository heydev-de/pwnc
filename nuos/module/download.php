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

global $download_index,
       $download_start;

$flag = FALSE;

//verify index
if (nstre($download_index) && cms_load("download"))
{
    $download = new download();
    $filename = $download->data->get($download_index, "filename") ??
                $download->data->remove_prefix($download_index); //compatibility fallback

    if (
         (($name = $download->data->get($download_index, "name")) !== NULL)
         &&
         is_file($path = CMS_DATA_PATH . "#download/$filename")
       )
    {
        //collect data
        $name        = l($name);
        $extension   = file_extension($filename);
        $size        = sprintf("%u", filesize($path));
        $description = l($download->data->get($download_index, "description"));
        $target      = cms_url(["download_index" => $download_index,
                                "download_start" => 1]);
        $flag        = TRUE;

    };
};

if (empty($download_start))
{
    echo(CMS_DOCTYPE_HTML .
         "<html>" .
         "<head>" .
         CMS_HTML_HEADER . CMS_JAVASCRIPT . CMS_STYLESHEET .
         "</head>" .
         "<body class=\"" . x(CMS_CLASS) . "\"?>");

    //start download in 2.5 seconds
    if ($flag)
    {

?><script>

setTimeout(function() { location.replace("<?php echo(q($target));?>"); }, 2500);

</script><?php

    };

    insert("top");

    //display data
    if ($flag)
    {
        //title
        echo("<h1>" .
             CMS_L_MOD_DOWNLOAD_001 .
             "</h1>" .

        //name
             "<div class=\"p\">" .
             "<strong>" .
             x($name) .
             "</strong> " .
             "(");

        //extension
        if ($extension)
            echo(x(strtoupper($extension)) . ", ");

        //filesize
        echo(format_bytesize($size) .
             ")");

        //description
        if ($description)
            echo("<br>" . parse_text($description));

        echo("</div>" .

             "<div class=\"p\">" .
             "<strong>" .
             sprintf(CMS_L_MOD_DOWNLOAD_002, x($target)) .
             "</strong><br>" .
             CMS_L_MOD_DOWNLOAD_003 .
             "</div>");
    }

    //download not available
    else
    {
        echo(alert(CMS_L_MOD_DOWNLOAD_007));
    };

    insert("bottom");

    permission(["" => CMS_L_ACCESS]);

    echo("</body>" .
         "</html>");
}
elseif ($flag)
{
    if (download($path))
    {
        //log download action
        $log = new log();
        $log->access("downloaded", $name);
    };

    exit();
};

})();