<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   INTERFACE
//==================================================================================================

//load libraries
if (! cms_load("filemanager")) ifc_inactive($ifc_page);
if (! cms_load("flexview"))    ifc_inactive($ifc_page);
if (! cms_load("plist"))       ifc_inactive($ifc_page);

//permission
ifc_permission(["" => CMS_L_ACCESS]);

//retrieve selected object from permanent cache
if (! isset($object)) $object = (string)cms_cache("filemanager." . CMS_USER . ".object");
if (stre($object))    $object = CMS_ROOT_PATH;

//verify and correct selected object
if (nstreq(substr($object, 0, strlen(CMS_ROOT_PATH)), CMS_ROOT_PATH)) $object = CMS_ROOT_PATH;
while (! file_exists($object))                                        $object = dirname($object) . "/";

if (! isset($list))   $list   = preg_match("/^(?:|ifc_)/", CMS_IFC_MESSAGE) ? [$object] : [];
if (! isset($target)) $target = NULL;
if (! isset($status)) $status = NULL;

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "select":
//..................................................................................................

    $object = stre($ifc_param) ? CMS_ROOT_PATH : $ifc_param;
    $list   = [$object];
    $status = NULL;
    break;

case "save":
//..................................................................................................

    $ifc_param1 = dirname($object) . "/" . basename($ifc_param1);
    if (is_dir($object)) $ifc_param1 .= "/";

    $access = (isset($ifc_param2) ? 256 : 0) + (isset($ifc_param5) ? 32 : 0) + (isset($ifc_param8)  ? 4 : 0) +
              (isset($ifc_param3) ? 128 : 0) + (isset($ifc_param6) ? 16 : 0) + (isset($ifc_param9)  ? 2 : 0) +
              (isset($ifc_param4) ?  64 : 0) + (isset($ifc_param7) ?  8 : 0) + (isset($ifc_param10) ? 1 : 0);
    chmod($object, $access);

    if (rename($object, $ifc_param1))
    {
        $object       = $ifc_param1;
        $list         = [$object];
        $status       = NULL;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "upload":
//..................................................................................................

    if (is_array($ifc_file1)) $array = array_combine($ifc_file1, $ifc_file1_name);
    else                      $array = [$ifc_file1 => $ifc_file1_name];

    $path  = is_dir($object) ? $object : dirname($object) . "/";
    $list  = [];
    $error = FALSE;

    filemanager_sort($array);

    foreach ($array AS $key => $value)
    {
        $value = $path . $value;

        //place upload
        if (move_uploaded_file($key, $value)) $list[] = $value;
        else                                  $error  = TRUE;
    };

    $object       = reset($list);
    $status       = NULL;
    $ifc_response = $error ? CMS_MSG_ERROR : CMS_MSG_DONE;
    break;

case "mkdir":
//..................................................................................................

    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object" => $object,
                    "list"   => $list],
                   "_mkdir",
                   CMS_L_IFC_FILEMANAGER_006);

    echo("<span class=\"element\">" .
         x(is_dir($object) ? $object : dirname($object) . "/") .
         "</span><br>");
    $ifc->set(CMS_L_NAME, "text 40 256");

    $ifc->close();

case "_mkdir":
//..................................................................................................

    $_object = (is_dir($object) ? $object : dirname($object) . "/") . "$ifc_param1/";

    if (mkpath($_object))
    {
        $object       = $_object;
        $list         = [$object];
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "mkfile":
//..................................................................................................

    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object" => $object,
                    "list"   => $list],
                   "_mkfile",
                   CMS_L_IFC_FILEMANAGER_007);

    echo("<span class=\"element\">" .
         x(is_dir($object) ? $object : dirname($object) . "/") .
         "</span><br>");
    $ifc->set(CMS_L_NAME,                "text 40 256 b");
    $ifc->set(CMS_L_IFC_FILEMANAGER_003, "textarea 80x30 n");

    $ifc->close();

case "_mkfile":
//..................................................................................................

    $_object = (is_dir($object) ? $object : dirname($object) . "/") . $ifc_param1;

    if (is_file($_object) || (! $hfile = fopen($_object, "wb")))
    {
        $ifc_response = CMS_MSG_ERROR;
        break;
    };

    flock($hfile, LOCK_EX);
    fwrite($hfile, $ifc_param2);
    fclose($hfile);

    $object       = $_object;
    $list         = [$object];
    $ifc_response = CMS_MSG_DONE;
    break;

case "_edit":
//..................................................................................................

    if ($hfile = fopen($object, "wb"))
    {
        flock($hfile, LOCK_EX);
        fwrite($hfile, $ifc_param1);
        fclose($hfile);

        $ifc_response = CMS_MSG_DONE;
    }
    else
    {
        $ifc_response = CMS_MSG_ERROR;
    };

    echo($ifc_response);
    exit();

case "edit":
//..................................................................................................

    if (! is_file($object)) ifc_close_external();

    if (streq(CMS_IFC_MESSAGE, "edit"))
    {
        $top = $left = 0;

        //add access
        $plist = new plist("#system/filemanager.recent");
        $plist->add(substr($object, strlen(CMS_ROOT_PATH)), TRUE);
        $plist->remove(10, 0);
    };

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   [CMS_L_COMMAND_SAVE . "|filemanager/command_save" =>
                    "javascript:asr_form_post(document.forms['ifc'],ifc_response);"],
                   ["object" => $object],
                   "_edit",
                   CMS_L_COMMAND_EDIT);

    switch (get_mime_type($object))
    {
    case "text/html":
    case "image/svg+xml":

        $type = "code_html";
        break;

    case "application/x-php":
    case "application/x-pwnc-include":

        $type = "code_php";
        break;

    case "text/css":

        $type = "code_style";
        break;

    case "application/javascript":

        $type = "code_script";
        break;

    default:

        $type = "textarea";
        break;
    };

    $ifc->set($object, "$type 80x20 fn", file_get_contents($object));
    $ifc->close();

case "compress":
//..................................................................................................

    if (count($list) === 0) break; //no selection

    $_list = [];
    $error = FALSE;

    foreach ($list AS $value)
    {
        if ($value = filemanager_gzcompress($value)) $_list[]  = $value;
        else                                         $error   |= TRUE;
    };

    if (! $error)
    {
        $list         = $_list;
        $status       = NULL;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "decompress":
//..................................................................................................

    if (count($list) === 0) break; //no selection

    $_list = [];
    $error = FALSE;

    foreach ($list AS $value)
    {
        if ($value = filemanager_gzdecompress($value)) $_list[]  = $value;
        else                                           $error   |= TRUE;
    };

    if (! $error)
    {
        $list         = $_list;
        $status       = NULL;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "copy":
case "cut":
//..................................................................................................

    $target = $object;
    $status = CMS_IFC_MESSAGE;
    break;

case "target":
//..................................................................................................

    $target = $ifc_param;
    break;

case "copy_insert":
//..................................................................................................

    if (count($list) === 0) break; //no selection

    if (stre($ifc_param)) $ifc_param = CMS_ROOT_PATH;

    $_list = [];
    $error = FALSE;

    foreach ($list AS $value)
    {
        $_value = basename($value);

        if (is_dir($value))
        {
            $_value  .= "/";
            $__value  = $ifc_param . $_value;
            $i        = 0;

            while (is_dir($__value))
                $__value = $ifc_param . str_repeat("_", ++$i) . $_value;
        }
        else
        {
            $__value  = $ifc_param . $_value;
            $i        = 0;

            while (is_file($__value))
                $__value = $ifc_param . str_repeat("_", ++$i) . $_value;
        };

        if (filemanager_copy($value, $__value)) $_list[] = $__value;
        else                                    $error   = TRUE;
    };

    if (! $error)
    {
        $object       = $ifc_param;
        $list         = $_list;
        $status       = NULL;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "cut_insert":
//..................................................................................................

    if (count($list) === 0) break; //no selection

    if (stre($ifc_param)) $ifc_param = CMS_ROOT_PATH;

    $_list = [];
    $error = FALSE;

    foreach ($list AS $value)
    {
        $_value = basename($value);

        if (is_dir($value))
        {
            $_value  .= "/";
            $__value  = $ifc_param . $_value;
            $i        = 0;

            while (is_dir($__value))
                $__value = $ifc_param . str_repeat("_", ++$i) . $_value;
        }
        else
        {
            $__value  = $ifc_param . $_value;
            $i        = 0;

            while (is_file($__value))
                $__value = $ifc_param . str_repeat("_", ++$i) . $_value;
        };

        if (rename($value, $__value)) $_list[] = $__value;
        else                          $error   = TRUE;
    };

    if (! $error)
    {
        $object        = $ifc_param;
        $list          = $_list;
        $status        = NULL;
        $ifc_response  = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "download":
//..................................................................................................

    download($object);
    exit();

case "delete":
//..................................................................................................

    if (count($list) === 0) break; //no selection

    $error = FALSE;

    foreach ($list AS $value)
        $error |= ! filemanager_delete($value);

    if (! $error)
    {
        $list         = [$object];
        $status       = NULL;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "transfer":
//..................................................................................................

    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object" => $object,
                    "list"   => $list],
                   "_transfer",
                   CMS_L_IFC_FILEMANAGER_033);

    $ifc->set(CMS_L_URL,  "text 50 b", "https://");
    echo("<span class=\"element\">" .
         x(is_dir($object) ? $object : dirname($object) . "/") .
         "</span><br>");
    $ifc->set(CMS_L_NAME, "text 50 256");

    $ifc->close();

case "_transfer":
//..................................................................................................

    $ifc_param1 = utf8_trim($ifc_param1);
    $ifc_param2 = utf8_trim($ifc_param2);
    if (stre($ifc_param2)) $ifc_param2 = basename($ifc_param1);

    $_object = ((! is_dir($object)) ? dirname($object) . "/" : $object) . $ifc_param2;

    if (retrieve_file($ifc_param1, $_object))
    {
        $object       = $_object;
        $list         = [$object];
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

if (CMS_IFC_MESSAGE !== "")
{
    //verify and correct selected object
    if (nstreq(substr($object, 0, strlen(CMS_ROOT_PATH)), CMS_ROOT_PATH)) $object = CMS_ROOT_PATH;
    while (! file_exists($object))                                        $object = dirname($object) . "/";

    //cache selected object permanently
    cms_cache("filemanager." . CMS_USER . ".object", $object, TRUE);
};

$flexview  = filemanager_flexview(NULL, $object);
$mime_list = get_mime_list();
$filename  = basename($object);
$url       = substr($object, strlen(CMS_ROOT_PATH));
//                                      -./ 0 - 9  A - Z      _    a - z
$url       = preg_replace_callback("/[^\\x2D-\\x39\\x41-\\x5A\\x5F\\x61-\\x7A]/",
                                   function($value) { return "%" . bin2hex($value[0]); },
                                   $url);
$url       = CMS_ROOT_URL . $url;

//menu
$menu = [CMS_L_IFC_FILEMANAGER_006 . "|filemanager/command_create_directory" => "mkdir",
         CMS_L_IFC_FILEMANAGER_007 . "|filemanager/command_create_file"      => "mkfile"];
if (is_file($object))
{
    if (
         preg_match("/\.dat$/", $object)
         &&
         ifc_available("system")
         &&
         cms_permission("system", TRUE, NULL)
       )
        $menu[CMS_L_COMMAND_EDIT . "|filemanager/command_edit"] =
            "javascript:load_page('" .
            q(cms_url(["ifc_page"    => "system",
                       "ifc_message" => "filemanager",
                       "ifc_option"  => "external",
                       "file"        => $object])) . "');";
    else
        $menu[CMS_L_COMMAND_EDIT . "|filemanager/command_edit"] =
            "javascript:load_page('" .
            q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                       "ifc_message" => "edit",
                       "ifc_option"  => "external",
                       "object"      => $object])) . "');";
};
$menu[CMS_L_COMMAND_COPY .            "|filemanager/command_copy"]       = "copy";
$menu[CMS_L_COMMAND_CUT .             "|filemanager/command_cut"]        = "cut";
$menu[CMS_L_COMMAND_DELETE_SELECTED . "|filemanager/command_delete"]     = "#delete";
$menu[CMS_L_IFC_FILEMANAGER_036 .     "|filemanager/command_compress"]   = "compress";
$menu[CMS_L_IFC_FILEMANAGER_037 .     "|filemanager/command_decompress"] = "decompress";
$menu[CMS_L_IFC_FILEMANAGER_033 .     "|filemanager/command_transfer"]   = "transfer";
if (
     cms_available("directory")
     &&
     cms_permission("directory", TRUE, NULL)
   )
    $menu[CMS_L_IFC_FILEMANAGER_008 . "|filemanager/command_directory"] =
        "javascript:load_page('" .
        q(cms_url(["ifc_page"    => "directory",
                   "ifc_message" => "add_target",
                   "ifc_option"  => "external",
                   "ifc_param1"  => $filename,
                   "ifc_param5"  => "\x1B",
                   "ifc_param6"  => $url,
                   "ifc_param7"  => "\x1B"])) . "');";

//display
$ifc = new ifc($ifc_response,
               $ifc_page,
               $menu,
               ["object" => $object,
                "status" => $status]);

echo("<table class=\"layout\">" .

     "<colgroup>" .
     "<col>" .
     ($status ? "<col style=\"WIDTH:200px\">" : "") .
     "<col style=\"WIDTH:0\">" .
     "</colgroup>" .

     "<tr>" .

//directory
     "<td>");

ifc_table_open();

//path
echo("<tr>" .
     "<th>" .
     x($object) .
     "</th>" .
     "</tr>" .

     "<tr>" .
     "<td>");

//address
$ifc->set(CMS_L_URL, "description :");
echo("<input name=\"address\" " .
            "type=\"text\" " .
            "value=\"" . x($url) . "\" " .
            "onfocus=\"this.select();\" " .
            "readonly " .
            "class=\"select\">");

//display button
$ifc->set(CMS_L_COMMAND_SHOW,
          "button",
          "javascript:load_page('" . q(cms_url($url, NULL, TRUE)) . "');");

//download button
if (is_file($object))
    $ifc->set(CMS_L_IFC_FILEMANAGER_029,
              "button",
              "javascript:ifc_download('" .
              q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                         "ifc_message" => "download",
                         "object"      => $object])) . "');");
echo("</td>" .
     "</tr>" .

//shortcuts
     "<tr>" .
     "<td class=\"highlight\">" .
     "<div class=\"br\">");
$array = [CMS_L_IFC_FILEMANAGER_024 => "design",
          CMS_L_IFC_FILEMANAGER_025 => "data",
          CMS_L_IFC_FILEMANAGER_026 => "data/#database",
          CMS_L_IFC_FILEMANAGER_028 => "data/#log",
          CMS_L_IFC_FILEMANAGER_031 => "#include",
          CMS_L_IFC_FILEMANAGER_032 => "3rdparty",
          CMS_L_IFC_FILEMANAGER_034 => "pwnc/#language",
          CMS_L_IFC_FILEMANAGER_035 => "#update/backup",
          CMS_SOFTWARE              => "pwnc"];
foreach ($array AS $key => $value)
    echo("<a href=\"javascript:ifc_post('select','" . qx(CMS_ROOT_PATH . "$value/") . "');\">" .
         image("filemanager/icon_folder") . " $key" .
         "</a> ");
echo("</div>" .
     "</td>" .
     "</tr>");

//recently edited
$plist = new plist("#system/filemanager.recent");
$array = $plist->get(0, 7);

if (count($array))
{
    echo("<tr>" .
         "<td class=\"varied\">" .
         "<div class=\"br\">");
    foreach ($array AS $value)
    {
        $extension = file_extension($value);
        $image     = isset($mime_list[$extension]) ? $mime_list[$extension] : "filemanager/icon_file";
        $dirname   = streq(($dirname = basename(dirname($value))), ".") ? "" : "$dirname/";
        $basename  = basename($value);

        echo("<a href=\"javascript:ifc_post('select','" . qx(CMS_ROOT_PATH . $value) . "');\" " .
            "title=\"" . x("/$value") . "\">" .
        image($image) . " " . x($dirname . $basename) .
        "</a> ");
    };
    echo("</div>" .
         "</td>" .
         "</tr>");
};

//directory
echo("<tr>" .
     "<td>" .
     "<div class=\"br\">");
$icon = array_merge(
    $mime_list,
    ["file"       => "filemanager/icon_file",
     "container"  => "filemanager/icon_folder",
     "+container" => "filemanager/icon_folderopen"]);
$flexview->set_encoding_function(__NAMESPACE__ . "\\qr");
$flexview->set_checkbox_identifier("list");
$flexview->set_checkbox_list($list);
$flexview->show_column(
    $object,                                    //index
    "javascript:ifc_post('select','%index%');", //action
    "name",                                     //name key
    NULL,                                       //mark
    $icon);                                     //icon
echo("</div>" .
     "</td>" .
     "</tr>" .

//selection control
     "<tr>" .
     "<td class=\"select\" " .
         "style=\"TEXT-ALIGN:right\">");
$ifc->set(CMS_L_ALL,    "button", "javascript:ifc_list_activate();");
$ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert();");
$ifc->set(CMS_L_NONE,   "button", "javascript:ifc_list_deactivate();");
echo("</td>" .
     "</tr>");
ifc_table_close();

echo("</td>");

if ($status !== NULL)
{
    //target
    echo("<td>");

    ifc_table_open();
    echo("<tr>" .
         "<th>" .
         (streq($status, "copy") ? CMS_L_IFC_FILEMANAGER_005 : CMS_L_IFC_FILEMANAGER_030) .
         "</th>" .
         "</tr>" .
         "<tr>" .
         "<td class=\"target\">");
    $flexview = filemanager_flexview_directory(NULL, $target);
    $flexview->set_encoding_function(__NAMESPACE__ . "\\qr");
    $flexview->show_target($target,
                           "javascript:ifc_post('target','%index%');",
                           "javascript:ifc_post('" . qr($status) . "_insert','%index%');");
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    echo("</td>");
};

//data
echo("<td>");

ifc_table_open();
echo("<tr>" .
     "<th>" .
     CMS_L_IFC_FILEMANAGER_009 .
     "</th>" .
     "</tr>" .
     "<tr>" .
     "<td>");
if (nstreq($object, CMS_ROOT_PATH))
{
    $ifc->set(CMS_L_IFC_FILEMANAGER_004, "text 30",  $filename);
    $ifc->set(CMS_L_COMMAND_SAVE,        "button b", "save");
};

$ifc->set(CMS_L_IFC_FILEMANAGER_010, "title");

//access permission
$access = fileperms($object);

echo("<table class=\"layout\">" .
     "<tr>" .

//owner
     "<td>" .
     "<span class=\"element\">" .
     CMS_L_IFC_FILEMANAGER_011 . " (". x(fileowner($object)) . ")" .
     "</span>" .
     "</td>" .

//group
     "<td>" .
     "<span class=\"element\">" .
     CMS_L_IFC_FILEMANAGER_012 . " (" . x(filegroup($object)) . ")" .
     "</span>" .
     "</td>" .

//all
     "<td>" .
     "<span class=\"element\">" .
     CMS_L_IFC_FILEMANAGER_013 .
     "</span>" .
     "</td>" .
     "</tr>" .
     "<tr>" .

//owner
     "<td>");
$ifc->set(CMS_L_IFC_FILEMANAGER_014,        "checkbox w", TRUE, $access & 0400); //read
$ifc->set(CMS_L_IFC_FILEMANAGER_015 . " *", "checkbox w", TRUE, $access & 0200); //write
$ifc->set(CMS_L_IFC_FILEMANAGER_016,        "checkbox w", TRUE, $access & 0100); //execute
echo("</td>" .

     "<td>");
$ifc->set(CMS_L_IFC_FILEMANAGER_014,        "checkbox w", TRUE, $access & 040); //read
$ifc->set(CMS_L_IFC_FILEMANAGER_015,        "checkbox w", TRUE, $access & 020); //write
$ifc->set(CMS_L_IFC_FILEMANAGER_016,        "checkbox w", TRUE, $access & 010); //execute
echo("</td>" .

//all
     "<td>");
$ifc->set(CMS_L_IFC_FILEMANAGER_014,        "checkbox w", TRUE, $access & 04); //read
$ifc->set(CMS_L_IFC_FILEMANAGER_015,        "checkbox w", TRUE, $access & 02); //write
$ifc->set(CMS_L_IFC_FILEMANAGER_016,        "checkbox w", TRUE, $access & 01); //execute
echo("</td>" .
     "</tr>" .
     "<tr>" .
     "</table>" .

     "<span class=\"element\">" .
     "<small>" .
     "* " . CMS_L_IFC_FILEMANAGER_017 .
     "</small>" .
     "</span><br>");

//information
$filesize         = sprintf("%u", filesize($object));
$transtime        = round($filesize / 1250000);
$disk_total_space = disk_total_space(CMS_ROOT_PATH);
$disk_free_space  = disk_free_space(CMS_ROOT_PATH);
$percentage_used  = 100 / $disk_total_space * $disk_free_space;

$ifc->set(CMS_L_IFC_FILEMANAGER_040, "title");

ifc_table_open("nomargin");

//type
echo("<tr>" .
     "<td>" .
     CMS_L_IFC_FILEMANAGER_018 . ":" .
     "</td>" .
     "<td>" .
     filetype($object) .
     "</td>" .
     "</tr>" .

//mime type
     "<tr>" .
     "<td>" .
     CMS_L_IFC_FILEMANAGER_039 . ":" .
     "</td>" .
     "<td>" .
     x(get_mime_type($object)) .
     "</td>" .
     "</tr>" .

//size
     "<tr>" .
     "<td>" .
     CMS_L_IFC_FILEMANAGER_019 . ":" .
     "</td>" .
     "<td>" .
     format_bytesize($filesize) .
     "</td>" .
     "</tr>" .

//transmission time
     "<tr>" .
     "<td>" .
     CMS_L_IFC_FILEMANAGER_023 . ":" .
     "</td>" .
     "<td>" .
     gmdate(CMS_L_TIME_FORMAT, $transtime) .
     "</td>" .
     "</tr>" .

//modified time
     "<tr>" .
     "<td>" .
     CMS_L_IFC_FILEMANAGER_020 . ":" .
     "</td>" .
     "<td>" .
     x(friendly_date(filemtime($object))) .
     "</td>" .
     "</tr>" .

//accessed time
     "<tr>" .
     "<td>" .
     CMS_L_IFC_FILEMANAGER_021 . ":" .
     "</td>" .
     "<td>" .
     x(friendly_date(fileatime($object))) .
     "</td>" .
     "</tr>" .

//total space
     "<tr>" .
     "<td>" .
     CMS_L_IFC_FILEMANAGER_038 . ":" .
     "</td>" .
     "<td>" .
     format_bytesize($disk_total_space) .
     "</td>" .
     "</tr>" .

//free space
     "<tr>" .
     "<td>" .
     CMS_L_IFC_FILEMANAGER_022 . ":" .
     "</td>" .
     "<td>" .
     format_bytesize($disk_free_space) .
     " (" . number_format($percentage_used, 1, CMS_L_DECIMAL_SEPARATOR, NULL) . "%)" .
     "</td>" .
     "</tr>");
ifc_table_close();

//file upload
echo("<hr>");
$ifc->set(CMS_L_IFC_FILEMANAGER_002, "multifile 30");
$ifc->set(CMS_L_IFC_FILEMANAGER_001, "button b", "upload");
$ifc->set(CMS_L_IFC_FILEMANAGER_027, "info");
echo(jscript("ifc_autopost(\"ifc_file1[]\", \"upload\");") .

     "</td>" .
     "</tr>" .
     "</table>");

$ifc->close();