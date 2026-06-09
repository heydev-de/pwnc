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

//probably custom 404 page
if (isset($_SERVER["REDIRECT_STATUS"]) && ((string)$_SERVER["REDIRECT_STATUS"] === "404"))
{
    http_response_code(404);

    //skip error page for static content
    if (isset($_SERVER["REQUEST_URI"]))
    {
        $path = parse_url($_SERVER["REQUEST_URI"], PHP_URL_PATH);
        if ((string)$path !== "")
        {
            $extension = pathinfo($path, PATHINFO_EXTENSION);
            if ((string)$extension !== "")
            {
                $array = ["avif", "gif", "ico",  "jpeg", "jpg", "png", "svg", "webp",
                          "mp4",  "ogv", "webm",
                          "m4a",  "mp3", "ogg",  "wav",  "weba",
                          "eot",  "otf", "ttf",  "woff", "woff2",
                          "css",  "js"];
                if (in_array(strtolower($extension), $array)) exit();
            };
        };
    };
};

require(dirname(dirname(__FILE__)) . "/pwnc.inc");

(function() {

//memorize start time
$time_start = microtime(TRUE);

global $content_display,
       $content_index,
       $content_directory_index,
       $content_option,
       $content_user,
       $content_message,
       $range,
       $type,
       $value,
       $left,
       $top,
       $id;

if (isset($_GET["content_directory_index"])) unset($_GET["content_directory_index"]);
if (isset($_GET["content_display"]))         unset($_GET["content_display"]);

if (

//load libraries
     (! cms_load("content"))
     ||
     (! cms_load("template"))
     ||

//instantiate content object
     (! ($content = new content($content_user))->enabled)
   )
{
    http_response_code(500);
    exit();
};

//static editing parameters
if ($content->writer)
{
    $content_option = (int)$content_option;
    cms_param($content_option, "content_option");
    cms_param($content_user,   "content_user");
};

//get reference from directory
if (
     ($flag = stre($content_index))
     ||
     streq($content_display, "directory")
   )
{
    $data = new data("#system/directory");

    //select first entry if none is given
    if (stre($content_directory_index))
    {
        $content_directory_index = $data->move("first");
        $url = translate_url("directory://$content_directory_index");

        //language gate
        if (nstre(CMS_LANGUAGE_ENABLED))
        {
            header("Vary: Accept-Language");
            header("Location: $url", TRUE, 302);
            exit();
        };

        //homepage redirect
        header("Location: $url", TRUE, 301);
        exit();
    };

    //retrieve url
    $url = $data->get($content_directory_index, "url");

    while (nstre($url))
    {
        $_url = analyze_url($url);
        if ($_url === FALSE) { $flag = TRUE; break; };

        //directory dereferencing
        $list = [$content_directory_index => TRUE];
        while (streq($_url["scheme"], "directory"))
        {
            $content_directory_index = $_url["host"];

            //check for loop
            if (isset($list[$content_directory_index]))
            {
                http_response_code(508);
                exit();
            };
            $list[$content_directory_index] = TRUE;

            $url  = $data->get($content_directory_index, "url");
            $_url = analyze_url($url);
            if ($_url === FALSE) { $flag = TRUE; break 2; };
        };

        //internal reference
        if (streq($_url["scheme"], "content"))
        {
            $content_index = (int)$_url["host"];
            if ($flag = ($content_index === 0)) break;

            if (count($list) === 1) //no redirect
            {
                import_querystring("?" . $_url["query"]);
                break;
            };

            $url = translate_url($url);
        };

        //external reference
        $url = cms_url(absolute_path(CMS_ROOT_URL, $url));
        header("Location: $url", TRUE, 301);
        exit();
    };

    //missing or invalid reference
    if ($flag)
    {
        http_response_code(404);
        exit();
    };

    define("CMS_CONTENT_DIRECTORY_INDEX", $content_directory_index);
};

//copy partial content
if (streq($type, "#buffer"))
{
    /*
      this section is called by a javascript function
    */

    if (
         $content->test_update($content_index)
         &&
         ($result = mysql_query("SELECT " . CMS_DB_CONTENT_BUFFER_TEXT . ", " .
                                            CMS_DB_CONTENT_BUFFER_TEMPLATE . " " .
                                "FROM " .   CMS_DB_CONTENT . " " .
                                "WHERE " .  CMS_DB_CONTENT_INDEX .
                                "='" .      sqlesc($content_index) . "' " .
                                "LIMIT 1"))
         &&
         ($resultrow = mysql_fetch_assoc($result))
         &&
         cms_load("document")
       )
    {
        $document = new document($resultrow[CMS_DB_CONTENT_BUFFER_TEXT],
                                 $resultrow[CMS_DB_CONTENT_BUFFER_TEMPLATE]);
        $document = $document->extract($range);
        $buffer   = $document->export();

        //permanent caching
        cms_cache("content." . CMS_USER . ".buffer", $buffer, TRUE);
    };

    exit();
};

//verify read permission
if (! cms_permission(CMS_CONTENT_PERMISSION_READER . ".$content_index"))
{
    //no access
    if (headers_sent()) exit();

    if ($query_string = getenv("QUERY_STRING"))
        $query_string = "?$query_string";

    header("Location: " . CMS_MODULES_URL . "identification.php" .
           cms_url(["location" => CMS_ACTIVE_URL . $query_string]),
           TRUE, 302);
    exit();
};

//retrieve status
if (
     ! (
         ($result = mysql_query("SELECT " . CMS_DB_CONTENT_STATUS . " " .
                                "FROM " .   CMS_DB_CONTENT . " " .
                                "WHERE " .  CMS_DB_CONTENT_INDEX .
                                "='" .      sqlesc($content_index) . "' " .
                                "LIMIT 1"))
         &&
         ($resultrow = mysql_fetch_assoc($result))
       )
   )
{
    //not found
    http_response_code(404);
    exit();
};

//set global query values
if (str_replace("\\", "/", get_required_files()[0]) === CMS_MODULES_PATH . "content.php")
{
    cms_param($content_index,           "content_index");
    cms_param($content_directory_index, "content_directory_index");
};

//read mode
if (! $content->test_update($content_index))
{
    //not published
    if ((int)$resultrow[CMS_DB_CONTENT_STATUS] !== CMS_CONTENT_STATUS_PUBLICATION)
    {
        http_response_code(410); //assuming it was published before
        exit();
    };

    //log access
    $log = new log();
    $log->access("viewed", $content_index);

    //generate output
    $output = content_parse($content, $content_index, NULL, CMS_BOT_CHECK, $is_dynamic, $mod_time);

    //has no uncached parts
    if (! $is_dynamic)
    {
        $etag = "\"$mod_time\"";

        //check etag
        if (strpos($_SERVER["HTTP_IF_NONE_MATCH"] ?? "", $etag) !== FALSE)
        {
            http_response_code(304); //not modified
            exit(); //don't send body
        };

        //send modification time
        header("Last-Modified: " . gmdate("D, d M Y H:i:s", $mod_time) . " GMT");

        //send etag
        header("ETag: $etag");
    };

    echo($output);

    //memorize end time
    $time_end = microtime(TRUE);

    //display processing time
    echo("\n<!-- generated in " . (($time_end - $time_start) * 1000) . " ms -->");
    exit();
};

$interface_url  = CMS_MODULES_URL . "interface.php";
$desktop_url    = CMS_MODULES_URL . "desktop.php";
$javascript_url = x(CMS_JAVASCRIPT_URL);
$header         = jscript("document.documentElement.style.setProperty(\"opacity\", \"0\", \"important\");" .
                          "document.documentElement.style.setProperty(\"pointer-events\", \"none\", \"important\");" .
                          "var CMS_L_COMMAND_PASTE   = \"" . q(CMS_L_COMMAND_PASTE) . "\";" .
                          "var CMS_L_COMMAND_DELETE  = \"" . q(CMS_L_COMMAND_DELETE) . "\";" .
                          "var CMS_L_MOD_CONTENT_002 = \"" . q(CMS_L_MOD_CONTENT_002) . "\";" .
                          "var CMS_L_MOD_CONTENT_003 = \"" . q(CMS_L_MOD_CONTENT_003) . "\";" .
                          "var CMS_L_MOD_CONTENT_004 = \"" . q(CMS_L_MOD_CONTENT_004) . "\";" .
                          "var CMS_L_MOD_CONTENT_005 = \"" . q(CMS_L_MOD_CONTENT_005) . "\";" .
                          "var CMS_L_MOD_CONTENT_009 = \"" . q(CMS_L_MOD_CONTENT_009) . "\";" .
                          "var CMS_L_MOD_CONTENT_010 = \"" . q(CMS_L_MOD_CONTENT_010) . "\";" .
                          "var CMS_L_MOD_CONTENT_011 = \"" . q(CMS_L_MOD_CONTENT_011) . "\";" .
                          "var CMS_L_MOD_CONTENT_012 = \"" . q(CMS_L_MOD_CONTENT_012) . "\";" .
                          "var CMS_L_MOD_CONTENT_013 = \"" . q(CMS_L_MOD_CONTENT_013) . "\";") .
                  "\n<script src=\"" . $javascript_url . "asr.js\">" .
                  "</script>" .
                  "\n<script src=\"" . $javascript_url . "content.js\">" .
                  "</script>";

//process command
if (stre($range))
{
    $flag_redirect = TRUE;

    switch ($content_message)
    {
    case "apply":
//..................................................................................................

        $content->apply($content_index);
        break;

    case "revert":
//..................................................................................................

        $content->revert($content_index);
        break;

    case "undo":
//..................................................................................................

        $content->step_undo($content_index);
        break;

    case "redo":
//..................................................................................................

        $content->step_redo($content_index);
        break;

    default:
//..................................................................................................

        $flag_redirect = FALSE;
        break;
    };

    if ($flag_redirect)
    {
        header("Location: " . cms_url(["left" => $left, "top" => $top]), TRUE, 303);
        exit();
    };
}

//write changes
else
{
    content_set_range($content, $content_index, $range, $type, $value);
};

//set up option select function
$action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_SWITCH][CMS_TEMPLATE_CODE] =
    "tp_ctrl_opt_apply('" .
    q(cms_url(["content_option" => "\x1B%value%",
               "left"           => "\x1B%left%",
               "top"            => "\x1B%top%"])) . "');";

//initialize function list
$action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_COMMAND] = [];

if ($content->test_apply($content_index))
{
    //set up change apply function
    $action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_COMMAND][CMS_L_MOD_CONTENT_003] =
        "content_edit_apply('" .
        q(cms_url(["content_message" => "apply",
                   "left"            => "\x1B%left%",
                   "top"             => "\x1B%top%"])) . "');";

    //set up change revert function
    $action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_COMMAND][CMS_L_MOD_CONTENT_004] =
        "content_edit_revert('" .
        q(cms_url(["content_message" => "revert",
                   "left"            => "\x1B%left%",
                   "top"             => "\x1B%top%"])) . "');";
};

//set up action undo function
if ($content->test_step_undo($content_index))
{
    $name = CMS_L_MOD_CONTENT_016 . " (" . $content->step_undo_depth($content_index) . ")";
    $action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_COMMAND][$name] =
        "content_edit_command('" .
        q(cms_url(["content_message" => "undo",
                   "left"            => "\x1B%left%",
                   "top"             => "\x1B%top%"])) . "');";
};

//set up action redo function
if ($content->test_step_redo($content_index))
{
    $name = CMS_L_MOD_CONTENT_017 . " (" . $content->step_redo_depth($content_index) . ")";
    $action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_COMMAND][$name] =
        "content_edit_command('" .
        q(cms_url(["content_message" => "redo",
                   "left"            => "\x1B%left%",
                   "top"             => "\x1B%top%"])) . "');";
};

//content
if (is_file(CMS_MODULES_PATH . "#interface/ifc.content.inc"))
{
    //set up link edit function
    if ($content_option & CMS_TEMPLATE_OPTION_HREF)
        $action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_HREF] =
            [CMS_TEMPLATE_CODE  =>
                "content_edit_open('" .
                q(cms_url($interface_url,
                          ["ifc_page"          => "content",
                           "ifc_message"       => "edit_href",
                           "ifc_option"        => "external",
                           "ifc_select"        => "value",
                           "ifc_select_action" => "javascript:content_edit_href_save();",
                           "object"            => $content_index,
                           "range"             => "\x1B%index%",
                           "id"                => "\x1B%id%"],
                          TRUE)) . "');",
            CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_href.svg"];

    //set up plugin edit function
    if ($content_option & CMS_TEMPLATE_OPTION_PLUGIN)
        $action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_PLUGIN] =
            [CMS_TEMPLATE_CODE  =>
                "content_edit_open('" .
                q(cms_url($interface_url,
                          ["ifc_page"    => "content",
                           "ifc_message" => "edit_plugin",
                           "ifc_option"  => "external",
                           "object"      => $content_index,
                           "range"       => "\x1B%index%",
                           "id"          => "\x1B%id%"],
                          TRUE)) . "');",
            CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_plugin.svg"];

    //set up text edit function
    if ($content_option & CMS_TEMPLATE_OPTION_TEXT)
        $action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_TEXT] =
            [CMS_TEMPLATE_CODE  =>
                "content_edit_open('" .
                q(cms_url($interface_url,
                          ["ifc_page"    => "content",
                           "ifc_message" => "edit_range",
                           "ifc_option"  => "external",
                           "object"      => $content_index,
                           "range"       => "\x1B%index%",
                           "id"          => "\x1B%id%"],
                          TRUE)) . "');",
            CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_text.svg"];

    //set up value edit function
    if ($content_option & CMS_TEMPLATE_OPTION_VALUE)
        $action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_VALUE] =
            [CMS_TEMPLATE_CODE  =>
                "content_edit_open('" .
                q(cms_url($interface_url,
                          ["ifc_page"    => "content",
                           "ifc_message" => "edit_value",
                           "ifc_option"  => "external",
                           "object"      => $content_index,
                           "range"       => "\x1B%index%",
                           "id"          => "\x1B%id%"],
                          TRUE)) . "');",
            CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_value.svg"];

    //set up switch edit function
    if ($content_option & CMS_TEMPLATE_OPTION_SWITCH)
        $action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_SWITCH] =
            [CMS_TEMPLATE_CODE  =>
                "content_edit_switch('" .
                q(cms_url(["range" => "\x1B%index%",
                           "type"  => "value",
                           "value" => "\x1B%return%",
                           "left"  => "\x1B%left%",
                           "top"   => "\x1B%top%",
                           "id"    => "\x1B%id%"])) . "'," .
                "'%value%');",
            CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_switch.svg"];

    //set up meta data function
    $action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_COMMAND][CMS_L_MOD_CONTENT_018] =
        "load_page('" .
        q(cms_url($interface_url,
                  ["ifc_page"    => "content",
                   "ifc_message" => "meta",
                   "ifc_option"  => "external",
                   "ifc_param"   => $content_index],
                  TRUE)) . "');";

    //set up pool open function
    $action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_COMMAND][CMS_L_MOD_CONTENT_014] =
        "load_page('" .
        q(cms_url($interface_url,
                  ["ifc_page"    => "content",
                   "ifc_message" => "pool",
                   "ifc_option"  => "external"],
                  TRUE)) . "');";

    //set up text analysis function
    $action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_COMMAND][CMS_L_MOD_CONTENT_006] =
        "load_page('" .
        q(cms_url($interface_url,
                  ["ifc_page"    => "content",
                   "ifc_message" => "analyze",
                   "ifc_option"  => "external",
                   "object"      => $content_index],
                  TRUE)) . "');";

    //set up debug info function
    if (
         ($content_option & CMS_TEMPLATE_OPTION_DEBUG)
         &&
         cms_permission("interface.template.operator", NULL, NULL)
       )
        $action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_COMMAND][CMS_L_MOD_CONTENT_020] =
            "load_page('" .
            q(cms_url($interface_url,
                      ["ifc_page"    => "content",
                       "ifc_message" => "debug",
                       "ifc_option"  => "external",
                       "object"      => $content_index],
                      TRUE)) . "');";
};

//download
if (
     ($content_option & CMS_TEMPLATE_OPTION_DOWNLOAD)
     &&
     cms_available("download")
     &&
     is_file(CMS_MODULES_PATH . "#interface/ifc.download.inc")
     &&
     cms_permission("interface.download", NULL, NULL)
   )
{
    //set up download edit function
    $buffer = "'+encodeURIComponent('" .
              q(cms_url(["range" => "\x1B%index%",
                         "type"  => "download",
                         "value" => "\x1B%return%",
                         "left"  => "\x1B%left%",
                         "top"   => "\x1B%top%",
                         "id"    => "\x1B%id%"])) .
              "')+'";

    $action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_DOWNLOAD] =
        [CMS_TEMPLATE_CODE  =>
            str_replace(
                "%replace%",
                $buffer,
                "content_edit_open('" .
                q(cms_url($interface_url,
                          ["ifc_page"          => "download",
                           "ifc_option"        => "external",
                           "object"            => "\x1B%value%",
                           "ifc_select"        => "object",
                           "ifc_select_action" => "\x1B%replace%"],
                          TRUE)) . "');"),
        CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_download.svg"];
};

//image
if (
     cms_available("image")
     &&
     is_file(CMS_MODULES_PATH . "#interface/ifc.image.inc")
     &&
     cms_permission("interface.image", NULL, NULL)
   )
{
    //set up image edit function
    $buffer = "'+encodeURIComponent('" .
              q(cms_url(["range" => "\x1B%index%",
                         "type"  => "image",
                         "value" => "\x1B%return%",
                         "left"  => "\x1B%left%",
                         "top"   => "\x1B%top%",
                         "id"    => "\x1B%id%"])) .
              "')+'";

    $buffer = str_replace(
        "%replace%",
        $buffer,
        "content_edit_open('" .
        q(cms_url($interface_url,
                  ["ifc_page"          => "image",
                   "ifc_option"        => "external",
                   "object"            => "\x1B%value%",
                   "ifc_select"        => "object",
                   "ifc_select_action" => "\x1B%replace%"],
                  TRUE)) . "');");

    //set up image edit function
    if ($content_option & CMS_TEMPLATE_OPTION_IMAGE)
        $action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_IMAGE] =
            [CMS_TEMPLATE_CODE  => $buffer,
             CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_image.svg"];

    //set up thumbnail edit function
    if ($content_option & CMS_TEMPLATE_OPTION_THUMBNAIL)
        $action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_THUMBNAIL] =
            [CMS_TEMPLATE_CODE  => $buffer,
             CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_thumbnail.svg"];
};

//media
if (
     ($content_option & CMS_TEMPLATE_OPTION_MEDIA)
     &&
     cms_available("media")
     &&
     cms_available("media_type")
     &&
     is_file(CMS_MODULES_PATH . "#interface/ifc.media.inc")
     &&
     cms_permission("interface.media", NULL, NULL)
   )
{
    //set up media edit function
    $buffer = "'+encodeURIComponent('" .
              q(cms_url(["range" => "\x1B%index%",
                         "type"  => "media",
                         "value" => "\x1B%return%",
                         "left"  => "\x1B%left%",
                         "top"   => "\x1B%top%",
                         "id"    => "\x1B%id%"])) .
              "')+'";

    $action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_MEDIA] =
        [CMS_TEMPLATE_CODE  =>
            str_replace(
                "%replace%",
                $buffer,
                "content_edit_open('" .
                q(cms_url($interface_url,
                          ["ifc_page"          => "media",
                           "ifc_option"        => "external",
                           "object"            => "\x1B%value%",
                           "ifc_select"        => "object",
                           "ifc_select_action" => "\x1B%replace%"],
                          TRUE)) . "');"),
        CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_media.svg"];
};

//template
if (
     is_file(CMS_MODULES_PATH . "#interface/ifc.template.inc")
     &&
     cms_permission("interface.template", NULL, NULL)
   )
{
    //set up template edit function
    if ($content_option & CMS_TEMPLATE_OPTION_TEMPLATE)
    {
        $buffer = "'+encodeURIComponent('" .
                  q(cms_url(["range" => "\x1B%index%",
                             "type"  => "template",
                             "value" => "\x1B%return%",
                             "left"  => "\x1B%left%",
                             "top"   => "\x1B%top%",
                             "id"    => "\x1B%id%"])) .
                  "')+'";

        $action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_TEMPLATE] =
            [CMS_TEMPLATE_CODE  =>
                str_replace(
                    "%replace%",
                    $buffer,
                    "content_edit_open('" .
                    q(cms_url($interface_url,
                              ["ifc_page"          => "template",
                               "ifc_option"        => "external",
                               "object"            => "\x1B%value%",
                               "ifc_select"        => "object",
                               "ifc_select_action" => "\x1B%replace%"],
                              TRUE)) . "');"),
            CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_template.svg"];
    };

    if (cms_permission("interface.template.operator", NULL, NULL))
    {
        //set up template export function
        $buffer = "'+encodeURIComponent('" .
                  q(cms_url(["id"    => "\x1B%id%",
                             "left"  => "\x1B%left%",
                             "top"   => "\x1B%top%",
                             "range" => "\x1B%index%",
                             "type"  => "template",
                             "value" => "\x1B%return%"])) .
                  "')+'";

        $action[CMS_TEMPLATE_COMMAND][CMS_TEMPLATE_COMMAND_EXPORT] =
            str_replace(
                "%replace%",
                $buffer,
                "content_edit_open('" .
                q(cms_url($interface_url,
                          ["content_index"     => $content_index,
                           "content_range"     => "\x1B%index%",
                           "ifc_message"       => "export",
                           "ifc_option"        => "external",
                           "ifc_page"          => "template",
                           "ifc_select"        => "object",
                           "ifc_select_action" => "\x1B%replace%"],
                          TRUE)) . "');");

        //set up page template export function
        $action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_COMMAND][CMS_L_MOD_CONTENT_021] =
            "load_page('" .
            q(cms_url($interface_url,
                      ["content_index" => $content_index,
                       "ifc_message"   => "export",
                       "ifc_option"    => "external",
                       "ifc_page"      => "template"],
                      TRUE)) . "');";
    };
};

//group
if ($content_option & CMS_TEMPLATE_OPTION_GROUP)
{
    //set up template edit function
    $action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_GROUP] =
        [CMS_TEMPLATE_CODE  => ";",
         CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_group.svg"];
};

//repeat
if ($content_option & CMS_TEMPLATE_OPTION_REPEAT)
{
    //set up repeat edit function
    $action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_REPEAT] =
        [CMS_TEMPLATE_CODE  =>
            "content_edit_repeat('" .
            q(cms_url(["range" => "\x1B%index%",
                       "type"  => "repeat",
                       "value" => "\x1B%return%",
                       "left"  => "\x1B%left%",
                       "top"   => "\x1B%top%",
                       "id"    => "\x1B%id%"])) . "'," .
            "'%value%');",
        CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_repeat.svg"];
};

//shift
if ($content_option & CMS_TEMPLATE_OPTION_SHIFT)
{
    //set up shift edit function
    $action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_SHIFT] =
        [CMS_TEMPLATE_CODE  =>
            "content_edit_shift('" .
            q(cms_url(["range" => "\x1B%index%",
                       "type"  => "shift",
                       "value" => "\x1B%return%",
                       "left"  => "\x1B%left%",
                       "top"   => "\x1B%top%",
                       "id"    => "\x1B%id%"])) . "'," .
            "'%value%');",
        CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_shift.svg"];
};

if ($content_option)
{
    $unique_id = "_" . unique_id(); //used to prevent caching

    //set up copy function
    $action[CMS_TEMPLATE_COMMAND][CMS_TEMPLATE_COMMAND_BUFFER] =
        "content_edit_copy('" .
        q(cms_url(["range"    => "\x1B%path%",
                   "type"     => "#buffer",
                   "value"    => "none",
                   $unique_id => TRUE])) .
        "','%index%');";

    //set up paste function
    $action[CMS_TEMPLATE_COMMAND][CMS_TEMPLATE_COMMAND_PASTE] =
        "content_edit_paste('" .
        q(cms_url(["range"    => "\x1B%path%",
                   "type"     => "#paste",
                   "value"    => "none",
                   "left"     => "\x1B%left%",
                   "top"      => "\x1B%top%",
                   "id"       => "\x1B%id%",
                   $unique_id => TRUE])) . "');";

    //set up swap function
    $action[CMS_TEMPLATE_COMMAND][CMS_TEMPLATE_COMMAND_SWAP] =
        "content_edit_swap('" .
        str_replace("%replace%",
                    "'+encodeURIComponent(content_buffer)+'",
                    q(cms_url(["range"    => "\x1B%path%",
                               "type"     => "#swap",
                               "value"    => "\x1B%replace%",
                               "left"     => "\x1B%left%",
                               "top"      => "\x1B%top%",
                               "id"       => "\x1B%id%",
                               $unique_id => TRUE]))) . "');";

    //set up kick down function
    $action[CMS_TEMPLATE_COMMAND][CMS_TEMPLATE_COMMAND_KICK1] =
        "content_edit_kick1('" .
        q(cms_url(["range"    => "\x1B%path%",
                   "type"     => "#kick1",
                   "value"    => "\x1B%return%",
                   "left"     => "\x1B%left%",
                   "top"      => "\x1B%top%",
                   "id"       => "\x1B%id%",
                   $unique_id => TRUE])) . "');";

    //set up kick up function
    $action[CMS_TEMPLATE_COMMAND][CMS_TEMPLATE_COMMAND_KICK2] =
        "content_edit_kick2('" .
        q(cms_url(["range"    => "\x1B%path%",
                   "type"     => "#kick2",
                   "value"    => "\x1B%return%",
                   "left"     => "\x1B%left%",
                   "top"      => "\x1B%top%",
                   "id"       => "\x1B%id%",
                   $unique_id => TRUE])) . "');";

    //set up drop up function
    $action[CMS_TEMPLATE_COMMAND][CMS_TEMPLATE_COMMAND_DROP1] =
        "content_edit_command('" .
        q(cms_url(["range"    => "\x1B%path%",
                   "type"     => "#drop1",
                   "value"    => "1",
                   "left"     => "\x1B%left%",
                   "top"      => "\x1B%top%",
                   "id"       => "\x1B%id%",
                   $unique_id => TRUE])) . "');";

    //set up drop down function
    $action[CMS_TEMPLATE_COMMAND][CMS_TEMPLATE_COMMAND_DROP2] =
        "content_edit_command('" .
        q(cms_url(["range"    => "\x1B%path%",
                   "type"     => "#drop2",
                   "value"    => "-1",
                   "left"     => "\x1B%left%",
                   "top"      => "\x1B%top%",
                   "id"       => "\x1B%id%",
                   $unique_id => TRUE])) . "');";

    if (is_file(CMS_MODULES_PATH . "#interface/ifc.content.inc"))
    {
        //set up release pool function
        if (cms_permission("interface.content.pool.operator", NULL, NULL))
        {
            $action[CMS_TEMPLATE_COMMAND][CMS_TEMPLATE_COMMAND_RELEASE] =
                "content_edit_open('" .
                q(cms_url($interface_url,
                          ["ifc_page"      => "content",
                           "ifc_message"   => "pool_add",
                           "ifc_option"    => "external",
                           "pool_type"     => "\x1B%type%",
                           "content_index" => $content_index,
                           "content_range" => "\x1B%path%"],
                          TRUE)) . "');";
        };

        //set up reference pool function
        $buffer = "'+encodeURIComponent('" .
                  q(cms_url(["range" => "\x1B%index%",
                             "type"  => "#reference",
                             "value" => "\x1B%return%",
                             "left"  => "\x1B%left%",
                             "top"   => "\x1B%top%",
                             "id"    => "\x1B%id%"])) .
                  "')+'";

        $action[CMS_TEMPLATE_COMMAND][CMS_TEMPLATE_COMMAND_REFERENCE] =
            str_replace(
                "%replace%",
                $buffer,
                "content_edit_open('" .
                q(cms_url($interface_url,
                          ["ifc_page"          => "content",
                           "ifc_message"       => "pool",
                           "ifc_option"        => "external",
                           "pool_object"       => "\x1B%reference%",
                           "pool_type"         => "\x1B%type%",
                           "ifc_select"        => "pool_object",
                           "ifc_select_action" => "\x1B%replace%"],
                          TRUE)) . "');");
    };

    //set up clear function
    $action[CMS_TEMPLATE_COMMAND][CMS_TEMPLATE_COMMAND_CLEAR] =
        "content_edit_clear('" .
        q(cms_url(["range"    => "\x1B%path%",
                   "type"     => "#clear",
                   "value"    => "none",
                   "left"     => "\x1B%left%",
                   "top"      => "\x1B%top%",
                   "id"       => "\x1B%id%",
                   $unique_id => TRUE])) . "');";

    //set up dragdrop swap function
    $action[CMS_TEMPLATE_COMMAND][CMS_TEMPLATE_COMMAND_DRAGDROP1] =
        "content_edit_command('" .
        q(cms_url(["range"    => "\x1B%path%",
                   "type"     => "#dragdrop1",
                   "value"    => "\x1B%value%",
                   "left"     => "\x1B%left%",
                   "top"      => "\x1B%top%",
                   "id"       => "\x1B%id%",
                   $unique_id => TRUE])) . "');";

    //set up dragdrop copy function
    $action[CMS_TEMPLATE_COMMAND][CMS_TEMPLATE_COMMAND_DRAGDROP2] =
        "content_edit_command('" .
        q(cms_url(["range"    => "\x1B%path%",
                   "type"     => "#dragdrop2",
                   "value"    => "\x1B%value%",
                   "left"     => "\x1B%left%",
                   "top"      => "\x1B%top%",
                   "id"       => "\x1B%id%",
                   $unique_id => TRUE])) . "');";

    //set conditional block flag
    if ($content_option & CMS_TEMPLATE_OPTION_CBLOCK)
        $action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_CBLOCK] = TRUE;

    //set alternative block flag
    if ($content_option & CMS_TEMPLATE_OPTION_CALT)
        $action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_CALT]   = TRUE;

    //set debug flag
    if ($content_option & CMS_TEMPLATE_OPTION_DEBUG)
        $action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_DEBUG]  = TRUE;
};

//establish edit mode
if (! isset($action[CMS_TEMPLATE_ACTION]))
    $action[CMS_TEMPLATE_ACTION] = TRUE;

//set up interface open function
if (cms_permission("interface", FALSE, FALSE))
    $action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_COMMAND][CMS_L_MOD_CONTENT_007] =
        "content_load('" . q(cms_url($interface_url, TRUE)) . "');";

//set up desktop open function
if (cms_permission("desktop", FALSE, FALSE))
    $action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_COMMAND][CMS_L_MOD_CONTENT_008] =
        "content_load('" . q(cms_url($desktop_url, TRUE)) . "');";

//set up logout function
    $action[CMS_TEMPLATE_CONTROL][CMS_TEMPLATE_COMMAND][CMS_L_MOD_CONTENT_015] =
        "content_load('" .
        q(cms_url(CMS_MODULES_URL . "identification.php", ["cms_logout" => 1], TRUE)) . "');";

//generate output
echo(content_parse($content, $content_index, $action, $header));

?><script>

(function()
{
    tp_flp_restore("<?php echo(q($content_index));?>");
    fx_event_listen("window_load", tp_event);

    var func = function(object)
    {
        var type   = object.getAttribute("data-tp-dd-type");
        var accept = object.getAttribute("data-tp-dd-accept");
        dd_register(object.id, type, accept);
    };

    Array.prototype.forEach.call(document.getElementsByClassName("tp-dd"),    func);
    Array.prototype.forEach.call(document.getElementsByClassName("tp-dd100"), func);

    func = function()
    {
        fx_style(document.documentElement, "opacity", false);
        fx_style(document.documentElement, "pointer-events", false);
        var list = document.querySelectorAll("<?php
        if ($flag = in_array($type, ["#swap", "#dragdrop1", "#dragdrop2"]))
            echo("DIV[data-tp-path=\\\"" . q($value) . "\\\"]");
        if (nstreq($type, "#dragdrop2"))
            echo(($flag ? "," : "") . "DIV[data-tp-path=\\\"" . q($range) . "\\\"]");
        ?>");
        for (var i = 0; i < list.length; i++) list[i].className += " tp-edited";
    };

    if (document.URL.indexOf("#") !== -1)
    {
        fx_event_listen(window, "pageshow", func);
        return;
    };

    var scroll_behavior = fx_style(document.documentElement, "scroll-behavior");
    fx_style(document.documentElement, "scroll-behavior", "auto");
    fx_event_listen(window, "pageshow", function()
    {
        fx_update_window_size();
        fx_scroll_container.scrollTo(
            fx_document_width  / 100 * <?php echo((float)$left); ?>,
            fx_document_height / 100 * <?php echo((float)$top); ?>);
        fx_style(document.documentElement, "scroll-behavior", scroll_behavior);
        func();
    });
})();

</script><?php

//display processing time
$time_end = microtime(TRUE);
echo("\n<!-- generated in " . (($time_end - $time_start) * 1000) . " ms -->");

})();