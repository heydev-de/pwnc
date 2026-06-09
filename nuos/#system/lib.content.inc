<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   CONTENT CLASS
//==================================================================================================

//type
define("CMS_CONTENT_TYPE_ORIGINAL",                         1);
define("CMS_CONTENT_TYPE_DUPLICATE",                        2);
define("CMS_CONTENT_TYPE_COPY",                             3);

//status
define("CMS_CONTENT_STATUS_DRAFT",                          1);
define("CMS_CONTENT_STATUS_DOCUMENT",                       2);
define("CMS_CONTENT_STATUS_PUBLICATION",                    3);
define("CMS_CONTENT_STATUS_MAIL",                           4); //reserved
define("CMS_CONTENT_STATUS_POOL",                           5); //reserved

//flag
define("CMS_CONTENT_FLAG_NONE",                             0);
define("CMS_CONTENT_FLAG_SITEMAP_EXCLUDE",                  1);
define("CMS_CONTENT_FLAG_META_ROBOTS_NOINDEX",              2);
define("CMS_CONTENT_FLAG_META_ROBOTS_NOFOLLOW",             4);
define("CMS_CONTENT_FLAG_ALL",                     4294967295);

//action
define("CMS_CONTENT_ACTION_NONE",                           0);
define("CMS_CONTENT_ACTION_CREATE",                         1);
define("CMS_CONTENT_ACTION_UPDATE",                         2);
define("CMS_CONTENT_ACTION_AUTHORIZE",                      3);
define("CMS_CONTENT_ACTION_DERIVE_DRAFT",                   4);
define("CMS_CONTENT_ACTION_PUBLISH",                        5);
define("CMS_CONTENT_ACTION_WITHDRAW",                       6);
define("CMS_CONTENT_ACTION_DUPLICATE",                      7);
define("CMS_CONTENT_ACTION_COPY",                           8);
define("CMS_CONTENT_ACTION_DELETE",                         9);
define("CMS_CONTENT_ACTION_RECEIVE",                       10);
define("CMS_CONTENT_ACTION_CHANNEL",                       11);
define("CMS_CONTENT_ACTION_FLAG",                          12);
define("CMS_CONTENT_ACTION_EXTRA",                         13);

//role
define("CMS_CONTENT_ROLE_NONE",                             0);
define("CMS_CONTENT_ROLE_WRITER",                           1);
define("CMS_CONTENT_ROLE_EDITOR",                           2);
define("CMS_CONTENT_ROLE_PUBLISHER",                        4);
define("CMS_CONTENT_ROLE_ALL",                              7);
define("CMS_CONTENT_ROLE_WRITER_EDITOR",                    8);
define("CMS_CONTENT_ROLE_WRITER_PUBLISHER",                16);
define("CMS_CONTENT_ROLE_EDITOR_PUBLISHER",                32);
define("CMS_CONTENT_ROLE_WRITER_EDITOR_PUBLISHER",         64);

//schedule type
define("CMS_CONTENT_SCHEDULE_TYPE_APPLY",                   1);
define("CMS_CONTENT_SCHEDULE_TYPE_RETRIEVE",                2);
define("CMS_CONTENT_SCHEDULE_TYPE_PUBLISH",                 3);
define("CMS_CONTENT_SCHEDULE_TYPE_WITHDRAW",                4);

//permission
define("CMS_CONTENT_PERMISSION_READER",            "reader");
define("CMS_CONTENT_PERMISSION_WRITER",            "writer");
define("CMS_CONTENT_PERMISSION_EDITOR",            "editor");
define("CMS_CONTENT_PERMISSION_PUBLISHER",         "publisher");
define("CMS_CONTENT_PERMISSION_OPERATOR",          "operator");

//database
define("CMS_DB_CONTENT",                           CMS_DB_PREFIX . "content");
define("CMS_DB_CONTENT_INDEX",                     "id");
define("CMS_DB_CONTENT_OWNER",                     "owner");
define("CMS_DB_CONTENT_TYPE",                      "type");
define("CMS_DB_CONTENT_STATUS",                    "status");
define("CMS_DB_CONTENT_FLAG",                      "flag");
define("CMS_DB_CONTENT_CHANNEL",                   "channel");
define("CMS_DB_CONTENT_WRITER",                    "writer");
define("CMS_DB_CONTENT_WRITER_TIME",               "writer_time");
define("CMS_DB_CONTENT_WRITER_COMMENT",            "writer_comment");
define("CMS_DB_CONTENT_EDITOR",                    "editor");
define("CMS_DB_CONTENT_EDITOR_TIME",               "editor_time");
define("CMS_DB_CONTENT_EDITOR_COMMENT",            "editor_comment");
define("CMS_DB_CONTENT_PUBLISHER",                 "publisher");
define("CMS_DB_CONTENT_PUBLISHER_TIME",            "publisher_time");
define("CMS_DB_CONTENT_PUBLISHER_COMMENT",         "publisher_comment");
define("CMS_DB_CONTENT_TIME",                      "time");
define("CMS_DB_CONTENT_TITLE",                     "title");
define("CMS_DB_CONTENT_AUTHOR",                    "author");
define("CMS_DB_CONTENT_DESCRIPTION",               "description");
define("CMS_DB_CONTENT_KEYWORD",                   "keyword");
define("CMS_DB_CONTENT_IMAGE",                     "image");
define("CMS_DB_CONTENT_TEXT",                      "text");
define("CMS_DB_CONTENT_TEMPLATE",                  "template");
define("CMS_DB_CONTENT_BUFFER_TITLE",              "buffer_title");
define("CMS_DB_CONTENT_BUFFER_AUTHOR",             "buffer_author");
define("CMS_DB_CONTENT_BUFFER_DESCRIPTION",        "buffer_description");
define("CMS_DB_CONTENT_BUFFER_KEYWORD",            "buffer_keyword");
define("CMS_DB_CONTENT_BUFFER_IMAGE",              "buffer_image");
define("CMS_DB_CONTENT_BUFFER_TEXT",               "buffer_text");
define("CMS_DB_CONTENT_BUFFER_TEMPLATE",           "buffer_template");
define("CMS_DB_CONTENT_SENDER",                    "sender");
define("CMS_DB_CONTENT_SENDER_TIME",               "sender_time");
define("CMS_DB_CONTENT_SENDER_COMMENT",            "sender_comment");
define("CMS_DB_CONTENT_EXTRA_VALUE",               "extra_value");
define("CMS_DB_CONTENT_EXTRA_TYPE",                "extra_type");
define("CMS_DB_CONTENT_EXTRA_COLOR",               "extra_color");

define("CMS_DB_CONTENT_VERSION",                   CMS_DB_PREFIX . "content_version");
define("CMS_DB_CONTENT_VERSION_INDEX",             "id");
define("CMS_DB_CONTENT_VERSION_CONTENT",           "content");
define("CMS_DB_CONTENT_VERSION_TIME",              "time");
define("CMS_DB_CONTENT_VERSION_TITLE",             "title");
define("CMS_DB_CONTENT_VERSION_AUTHOR",            "author");
define("CMS_DB_CONTENT_VERSION_DESCRIPTION",       "description");
define("CMS_DB_CONTENT_VERSION_KEYWORD",           "keyword");
define("CMS_DB_CONTENT_VERSION_IMAGE",             "image");
define("CMS_DB_CONTENT_VERSION_TEXT",              "text");
define("CMS_DB_CONTENT_VERSION_TEMPLATE",          "template");
define("CMS_DB_CONTENT_VERSION_HASH",              "hash");

define("CMS_DB_CONTENT_SCHEDULE",                  CMS_DB_PREFIX . "content_schedule");
define("CMS_DB_CONTENT_SCHEDULE_TIME",             "time");
define("CMS_DB_CONTENT_SCHEDULE_TYPE",             "type");
define("CMS_DB_CONTENT_SCHEDULE_CONTENT",          "content");
define("CMS_DB_CONTENT_SCHEDULE_VALUE_1",          "value1");
define("CMS_DB_CONTENT_SCHEDULE_VALUE_2",          "value2");
define("CMS_DB_CONTENT_SCHEDULE_HASH",             "hash");

//related functions
//--------------------------------------------------------------------------------------------------

function content_get_range(&$content,
                           $index,
                           $range,
                           $type = NULL)
{
    if (! $content->enabled) return FALSE;

    //retrieve from temporary cache
    if (($document = cms_cache("content.$index")) !== NULL)
        return $document->get($range, $type);

    //retrieve from database
    if (
         cms_load("document")
         &&
         ($result = mysql_query("SELECT " . CMS_DB_CONTENT_BUFFER_TEXT . " " .
                                "FROM " .   CMS_DB_CONTENT . " " .
                                "WHERE " .  CMS_DB_CONTENT_INDEX . "='" . sqlesc($index) . "' " .
                                "LIMIT 1"))
         &&
         ($resultrow = mysql_fetch_assoc($result))
       )
    {
        $document = new document($resultrow[CMS_DB_CONTENT_BUFFER_TEXT]);
        cms_cache("content.$index", $document); //temporary caching
        return $document->get($range, $type);
    };

    return NULL;
};

//--------------------------------------------------------------------------------------------------

function content_set_range(&$content,
                           $index,
                           $range,
                           $type,
                           $text)
{
    if (! $content->enabled) return FALSE;

    //retrieve data from database if not cached
    if (($document = cms_cache("content.$index")) === NULL)
    {
        if (
             cms_load("document")
             &&
             ($result = mysql_query("SELECT " . CMS_DB_CONTENT_BUFFER_TEXT . ", " .
                                                CMS_DB_CONTENT_BUFFER_TEMPLATE . " " .
                                    "FROM " .   CMS_DB_CONTENT . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX . "='" . sqlesc($index) . "' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
           )
            $document = new document($resultrow[CMS_DB_CONTENT_BUFFER_TEXT],
                                     $resultrow[CMS_DB_CONTENT_BUFFER_TEMPLATE]);
        else
            return FALSE;
    };

    switch ($type)
    {
    case "#paste":
//..................................................................................................

        $text      = cms_cache("content." . CMS_USER . ".buffer");
        $_document = new document($text);
        $document->inject($range, $_document);
        break;

    case "#swap":
    case "#dragdrop1":
//..................................................................................................

        $document->swap($range, $text);
        break;

    case "#copy":
    case "#dragdrop2":
//..................................................................................................

        $document->copy($range, $text);
        break;

    case "#kick1":
    case "#kick2":
//..................................................................................................

        $document->kick($range, $text);
        break;

    case "#drop1":
//..................................................................................................

        $document->drop($range, 1);
        break;

    case "#drop2":
//..................................................................................................

        $document->drop($range, -1);
        break;

    case "#clear":
//..................................................................................................

        $document->del($range);
        break;

    case "shift":
//..................................................................................................

        $document->shift($range, $text);
        $document->set($range, "shift", $text);
        break;

    default:
//..................................................................................................

        $document->set($range, $type, $text);
        break;
    };

    cms_cache("content.$index", $document); //temporary caching

    return $content->update(
        $index,                    //index
        NULL,                      //title
        NULL,                      //description
        NULL,                      //keyword
        NULL,                      //image
        $document->export(FALSE)); //text
};

//--------------------------------------------------------------------------------------------------

function content_get_directory_index($content_index)
{
    $data = new data("#system/directory");

    $data->move("first");
    while (($key = $data->move("next")) !== NULL)
    {
        if (nstreq($data->get($key, "#type"), "container")) continue;

        //check for linkage of specified content
        if (preg_match("/content:\/\/" . preg_quote($content_index, "/") . "(?:[#?]|$)/S",
                       $data->get($key, "url")))
            return $key;
    };

    return 0;
};

//--------------------------------------------------------------------------------------------------

function content_parse(&$content,
                       $index,
                       $action      = NULL,
                       $header      = NULL,
                       &$is_dynamic = FALSE,
                       &$mod_time   = 0)
{
    if (! $content->enabled)    return FALSE;
    if (! cms_load("document")) return FALSE;
    if (! cms_load("template")) return FALSE;

    if (! defined("CMS_CONTENT_INDEX")) define("CMS_CONTENT_INDEX", $index);

    //find first directory link
    if (! defined("CMS_CONTENT_DIRECTORY_INDEX"))
        define("CMS_CONTENT_DIRECTORY_INDEX", content_get_directory_index($index));

    $query = (isset($action[CMS_TEMPLATE_ACTION]) ?
             "SELECT " . CMS_DB_CONTENT_FLAG . ", " .
                         CMS_DB_CONTENT_WRITER_TIME .        " AS " . CMS_DB_CONTENT_TIME . ", " .
                         CMS_DB_CONTENT_BUFFER_TITLE .       " AS " . CMS_DB_CONTENT_TITLE . ", " .
                         CMS_DB_CONTENT_BUFFER_DESCRIPTION . " AS " . CMS_DB_CONTENT_DESCRIPTION . ", " .
                         CMS_DB_CONTENT_BUFFER_KEYWORD .     " AS " . CMS_DB_CONTENT_KEYWORD . ", " .
                         CMS_DB_CONTENT_BUFFER_TEXT .        " AS " . CMS_DB_CONTENT_TEXT . ", " .
                         CMS_DB_CONTENT_BUFFER_TEMPLATE .    " AS " . CMS_DB_CONTENT_TEMPLATE . " " :
             "SELECT " . CMS_DB_CONTENT_FLAG . ", " .
                         CMS_DB_CONTENT_TIME . ", " .
                         CMS_DB_CONTENT_TITLE . ", " .
                         CMS_DB_CONTENT_DESCRIPTION . ", " .
                         CMS_DB_CONTENT_KEYWORD . ", " .
                         CMS_DB_CONTENT_TEXT . ", " .
                         CMS_DB_CONTENT_TEMPLATE . " ") .
             "FROM " .   CMS_DB_CONTENT . " " .
             "WHERE " .  CMS_DB_CONTENT_INDEX . "='" . sqlesc($index) . "' " .
             "LIMIT 1";

    if (
         ($result = mysql_query($query))
         &&
         ($resultrow = mysql_fetch_assoc($result))
       )
    {
        //modification time
        $mod_time = $resultrow[CMS_DB_CONTENT_TIME];

        //set meta robots
        $flag = $resultrow[CMS_DB_CONTENT_FLAG];

        if (! defined("CMS_CONTENT_META_ROBOTS"))
        {
            $array = [($flag & CMS_CONTENT_FLAG_META_ROBOTS_NOINDEX)  ? "noindex"  : "index",
                      ($flag & CMS_CONTENT_FLAG_META_ROBOTS_NOFOLLOW) ? "nofollow" : "follow"];
            define("CMS_CONTENT_META_ROBOTS", implode(", ", $array));
        };

        //retrieve document
        if (($document = cms_cache("content.$index")) === NULL)
        {
            $document = new document($resultrow[CMS_DB_CONTENT_TEXT]);
            cms_cache("content.$index", $document); //temporary caching
        };

        //instantiate parser
        $template = new template();

        if (! isset($action[CMS_TEMPLATE_ACTION]))
        {
            //request variables
            $request = $_POST + $_GET;
            ksort_recursive($request);
            $request = serialize($request);

            //generate file path
            $code = hash32($index . ";" . $flag . ";" . CMS_USER . ";" . CMS_LANGUAGE . ";" . CMS_ACTIVE_URL . ";" . $request);
            $path = CMS_DATA_PATH . "#content/cache/" . substr($code, 0, 4) . "/";

            //permanent caching
            if (mkpath($path))
            {
                $path .= substr($code, 4);

                if (
                     is_file($path)
                     &&
                     ($hfile = fopen($path, "rb"))
                   )
                {
                    flock($hfile, LOCK_SH);

                    //check if cache is up to date
                    $_time = (int)trim(fgets($hfile, 20));

                    if ($_time == $mod_time)
                    {
                        $_time = filemtime($path);

                        if (
                             ($_time >= filemtime(CMS_DATA_PATH . "#system/directory.dat"))
                             &&
                             ($_time >= filemtime(CMS_DATA_PATH . "#system/template.dat"))
                           )
                        {
                            $text = fread($hfile, filesize($path));
                            fclose($hfile);

                            //postprocess
                            return $template->process_cache(
                                $document,
                                $text,
                                $resultrow[CMS_DB_CONTENT_TITLE],
                                $resultrow[CMS_DB_CONTENT_DESCRIPTION],
                                $resultrow[CMS_DB_CONTENT_KEYWORD],
                                $action,
                                $header,
                                $is_dynamic);
                        };
                    };

                    fclose($hfile);
                };

                //preprocess
                $text = $template->create_cache(
                    $document,
                    $resultrow[CMS_DB_CONTENT_TEMPLATE],
                    $resultrow[CMS_DB_CONTENT_TITLE],
                    $resultrow[CMS_DB_CONTENT_DESCRIPTION],
                    $resultrow[CMS_DB_CONTENT_KEYWORD],
                    $action,
                    $header,
                    $is_dynamic);

                if ($hfile = fopen($path, "wb")) //permanent caching
                {
                    flock($hfile, LOCK_EX);
                    fwrite($hfile, $resultrow[CMS_DB_CONTENT_TIME] . "\n" . $text["cache"]);
                    fclose($hfile);
                };

                //postprocess
                return $text["output"];
            };
        };

        //process completely
        $is_dynamic = TRUE;
        return $template->parse(
            $document,
            $resultrow[CMS_DB_CONTENT_TEMPLATE],
            $resultrow[CMS_DB_CONTENT_TITLE],
            $resultrow[CMS_DB_CONTENT_DESCRIPTION],
            $resultrow[CMS_DB_CONTENT_KEYWORD],
            $action,
            $header);
    };

    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function content_template_export(&$content,
                                 $index,
                                 $range = NULL) //do not change for basic template
{
    if (! $content->enabled) return FALSE;

    if (
         cms_load("document")
         &&
         cms_load("template")
         &&
         ($result = mysql_query("SELECT " . CMS_DB_CONTENT_BUFFER_TEXT . ", " .
                                            CMS_DB_CONTENT_BUFFER_TEMPLATE . " " .
                                "FROM " .   CMS_DB_CONTENT . " " .
                                "WHERE " .  CMS_DB_CONTENT_INDEX . "='" . sqlesc($index) . "' " .
                                "LIMIT 1"))
         &&
         ($resultrow = mysql_fetch_assoc($result))
       )
    {
        if (($document = cms_cache("content.$index")) === NULL)
        {
            $document = new document($resultrow[CMS_DB_CONTENT_BUFFER_TEXT]);
            cms_cache("content.$index", $document); //temporary caching
        };

        $template = new template();
        return $template->export(
            $document,
            isset($range) ? NULL : $resultrow[CMS_DB_CONTENT_BUFFER_TEMPLATE],
            $range,
            TRUE);
    };

    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function content_template_select()
{
    if (! cms_available("template")) return FALSE;

    $data  = new data("#system/template");
    $array = NULL;

    $data->move("first");
    while (($key = $data->move("next")) !== NULL)
    {
        if ($data->get($key, "page"))
        {
            $name          = l($data->get($key, "name"));
            if (($category = $data->get($key, "category")) != "")
                $name  = "$category/$name";
            $_name         = $name;
            $i             = 1;
            while (isset($array[$_name]))
                $_name = $name . " (" . $i++ . ")";
            $array[$_name] = $key;
        };
    };

    ksort($array, SORT_NATURAL | SORT_FLAG_CASE);

    return $array;
};

//--------------------------------------------------------------------------------------------------

function content_get_receiver(&$content,
                              $type,
                              $status)
{
    if (! $content->enabled) return FALSE;

    $array     = [CMS_CONTENT_TYPE_ORIGINAL  => NULL,
                  CMS_CONTENT_TYPE_DUPLICATE => NULL,
                  CMS_CONTENT_TYPE_COPY      => NULL];

    //test if object can be duplicated or copied
    $duplicate = $content->action($type, $status, CMS_CONTENT_ACTION_DUPLICATE);
    $copy      = $content->action($type, $status, CMS_CONTENT_ACTION_COPY);

    //backup permissions
    $writer    = $content->writer;
    $editor    = $content->editor;
    $publisher = $content->publisher;

    //go through user list
    $data      = new data("#system/permission");

    $data->move("first");
    while (($key = $data->move("next")) !== NULL)
    {
        if (! permission_is_user($key))  continue; //not a user
        $key = substr($key, 5);
        if (streq($key, $content->user)) continue; //user equals sender

        //store permissions
        $content->writer    = cms_permission("interface.content." . CMS_CONTENT_PERMISSION_WRITER,    FALSE, FALSE, $key);
        $content->editor    = cms_permission("interface.content." . CMS_CONTENT_PERMISSION_EDITOR,    FALSE, FALSE, $key);
        $content->publisher = cms_permission("interface.content." . CMS_CONTENT_PERMISSION_PUBLISHER, FALSE, FALSE, $key);

        //get permitted actions
        if ($content->action($type, $status, CMS_CONTENT_ACTION_RECEIVE))
            $array[CMS_CONTENT_TYPE_ORIGINAL][$key]  = TRUE;

        if ($duplicate && $content->action(CMS_CONTENT_TYPE_DUPLICATE, $status, CMS_CONTENT_ACTION_RECEIVE))
            $array[CMS_CONTENT_TYPE_DUPLICATE][$key] = TRUE;

        if ($copy && $content->action(CMS_CONTENT_TYPE_COPY, $status, CMS_CONTENT_ACTION_RECEIVE))
            $array[CMS_CONTENT_TYPE_COPY][$key]      = TRUE;
    };

    //restore permissions
    $content->writer    = $writer;
    $content->editor    = $editor;
    $content->publisher = $publisher;

    return $array;
};

class content
{

//properties
//--------------------------------------------------------------------------------------------------

    public $action    = NULL;
    public $user      = NULL;
    public $writer    = FALSE;
    public $editor    = FALSE;
    public $publisher = FALSE;
    public $operator  = FALSE;
    public $enabled   = FALSE;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($user = NULL)
    {
        //permission table
        $this->action = [CMS_CONTENT_TYPE_ORIGINAL       =>

                        [CMS_CONTENT_STATUS_DRAFT        =>

                        [CMS_CONTENT_ACTION_NONE         => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_CREATE       => CMS_CONTENT_ROLE_WRITER,
                         CMS_CONTENT_ACTION_UPDATE       => CMS_CONTENT_ROLE_WRITER,
                         CMS_CONTENT_ACTION_AUTHORIZE    => CMS_CONTENT_ROLE_EDITOR,
                         CMS_CONTENT_ACTION_DERIVE_DRAFT => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_PUBLISH      => CMS_CONTENT_ROLE_WRITER_EDITOR_PUBLISHER,
                         CMS_CONTENT_ACTION_WITHDRAW     => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_DUPLICATE    => CMS_CONTENT_ROLE_WRITER,
                         CMS_CONTENT_ACTION_COPY         => CMS_CONTENT_ROLE_ALL,
                         CMS_CONTENT_ACTION_DELETE       => CMS_CONTENT_ROLE_WRITER,
                         CMS_CONTENT_ACTION_RECEIVE      => CMS_CONTENT_ROLE_WRITER | CMS_CONTENT_ROLE_EDITOR,
                         CMS_CONTENT_ACTION_FLAG         => CMS_CONTENT_ROLE_PUBLISHER,
                         CMS_CONTENT_ACTION_CHANNEL      => CMS_CONTENT_ROLE_PUBLISHER],

                         CMS_CONTENT_STATUS_DOCUMENT     =>

                        [CMS_CONTENT_ACTION_NONE         => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_CREATE       => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_UPDATE       => CMS_CONTENT_ROLE_WRITER_EDITOR,
                         CMS_CONTENT_ACTION_AUTHORIZE    => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_DERIVE_DRAFT => CMS_CONTENT_ROLE_EDITOR,
                         CMS_CONTENT_ACTION_PUBLISH      => CMS_CONTENT_ROLE_PUBLISHER,
                         CMS_CONTENT_ACTION_WITHDRAW     => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_DUPLICATE    => CMS_CONTENT_ROLE_WRITER_EDITOR,
                         CMS_CONTENT_ACTION_COPY         => CMS_CONTENT_ROLE_ALL,
                         CMS_CONTENT_ACTION_DELETE       => CMS_CONTENT_ROLE_EDITOR,
                         CMS_CONTENT_ACTION_RECEIVE      => CMS_CONTENT_ROLE_EDITOR | CMS_CONTENT_ROLE_PUBLISHER,
                         CMS_CONTENT_ACTION_FLAG         => CMS_CONTENT_ROLE_PUBLISHER,
                         CMS_CONTENT_ACTION_CHANNEL      => CMS_CONTENT_ROLE_PUBLISHER],

                         CMS_CONTENT_STATUS_PUBLICATION  =>

                        [CMS_CONTENT_ACTION_NONE         => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_CREATE       => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_UPDATE       => CMS_CONTENT_ROLE_WRITER_EDITOR_PUBLISHER,
                         CMS_CONTENT_ACTION_AUTHORIZE    => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_DERIVE_DRAFT => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_PUBLISH      => CMS_CONTENT_ROLE_PUBLISHER,
                         CMS_CONTENT_ACTION_WITHDRAW     => CMS_CONTENT_ROLE_PUBLISHER,
                         CMS_CONTENT_ACTION_DUPLICATE    => CMS_CONTENT_ROLE_WRITER_EDITOR_PUBLISHER,
                         CMS_CONTENT_ACTION_COPY         => CMS_CONTENT_ROLE_ALL,
                         CMS_CONTENT_ACTION_DELETE       => CMS_CONTENT_ROLE_PUBLISHER,
                         CMS_CONTENT_ACTION_RECEIVE      => CMS_CONTENT_ROLE_PUBLISHER,
                         CMS_CONTENT_ACTION_FLAG         => CMS_CONTENT_ROLE_PUBLISHER,
                         CMS_CONTENT_ACTION_CHANNEL      => CMS_CONTENT_ROLE_PUBLISHER]],

                         CMS_CONTENT_TYPE_DUPLICATE      =>

                        [CMS_CONTENT_STATUS_DRAFT        =>

                        [CMS_CONTENT_ACTION_NONE         => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_CREATE       => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_UPDATE       => CMS_CONTENT_ROLE_WRITER,
                         CMS_CONTENT_ACTION_AUTHORIZE    => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_DERIVE_DRAFT => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_PUBLISH      => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_WITHDRAW     => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_DUPLICATE    => CMS_CONTENT_ROLE_WRITER,
                         CMS_CONTENT_ACTION_COPY         => CMS_CONTENT_ROLE_ALL,
                         CMS_CONTENT_ACTION_DELETE       => CMS_CONTENT_ROLE_ALL,
                         CMS_CONTENT_ACTION_RECEIVE      => CMS_CONTENT_ROLE_WRITER,
                         CMS_CONTENT_ACTION_FLAG         => CMS_CONTENT_ROLE_PUBLISHER,
                         CMS_CONTENT_ACTION_CHANNEL      => CMS_CONTENT_ROLE_PUBLISHER],

                         CMS_CONTENT_STATUS_DOCUMENT     =>

                        [CMS_CONTENT_ACTION_NONE         => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_CREATE       => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_UPDATE       => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_AUTHORIZE    => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_DERIVE_DRAFT => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_PUBLISH      => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_WITHDRAW     => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_DUPLICATE    => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_COPY         => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_DELETE       => CMS_CONTENT_ROLE_ALL,
                         CMS_CONTENT_ACTION_RECEIVE      => CMS_CONTENT_ROLE_WRITER,
                         CMS_CONTENT_ACTION_FLAG         => CMS_CONTENT_ROLE_PUBLISHER,
                         CMS_CONTENT_ACTION_CHANNEL      => CMS_CONTENT_ROLE_PUBLISHER],

                         CMS_CONTENT_STATUS_PUBLICATION  =>

                        [CMS_CONTENT_ACTION_NONE         => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_CREATE       => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_UPDATE       => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_AUTHORIZE    => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_DERIVE_DRAFT => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_PUBLISH      => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_WITHDRAW     => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_DUPLICATE    => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_COPY         => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_DELETE       => CMS_CONTENT_ROLE_ALL,
                         CMS_CONTENT_ACTION_RECEIVE      => CMS_CONTENT_ROLE_WRITER,
                         CMS_CONTENT_ACTION_FLAG         => CMS_CONTENT_ROLE_PUBLISHER,
                         CMS_CONTENT_ACTION_CHANNEL      => CMS_CONTENT_ROLE_PUBLISHER]],

                         CMS_CONTENT_TYPE_COPY           =>

                        [CMS_CONTENT_STATUS_DRAFT        =>

                        [CMS_CONTENT_ACTION_NONE         => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_CREATE       => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_UPDATE       => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_AUTHORIZE    => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_DERIVE_DRAFT => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_PUBLISH      => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_WITHDRAW     => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_DUPLICATE    => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_COPY         => CMS_CONTENT_ROLE_ALL,
                         CMS_CONTENT_ACTION_DELETE       => CMS_CONTENT_ROLE_ALL,
                         CMS_CONTENT_ACTION_RECEIVE      => CMS_CONTENT_ROLE_ALL,
                         CMS_CONTENT_ACTION_FLAG         => CMS_CONTENT_ROLE_PUBLISHER,
                         CMS_CONTENT_ACTION_CHANNEL      => CMS_CONTENT_ROLE_PUBLISHER],

                         CMS_CONTENT_STATUS_DOCUMENT     =>

                        [CMS_CONTENT_ACTION_NONE         => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_CREATE       => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_UPDATE       => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_AUTHORIZE    => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_DERIVE_DRAFT => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_PUBLISH      => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_WITHDRAW     => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_DUPLICATE    => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_COPY         => CMS_CONTENT_ROLE_ALL,
                         CMS_CONTENT_ACTION_DELETE       => CMS_CONTENT_ROLE_ALL,
                         CMS_CONTENT_ACTION_RECEIVE      => CMS_CONTENT_ROLE_ALL,
                         CMS_CONTENT_ACTION_FLAG         => CMS_CONTENT_ROLE_PUBLISHER,
                         CMS_CONTENT_ACTION_CHANNEL      => CMS_CONTENT_ROLE_PUBLISHER],

                         CMS_CONTENT_STATUS_PUBLICATION  =>

                        [CMS_CONTENT_ACTION_NONE         => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_CREATE       => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_UPDATE       => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_AUTHORIZE    => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_DERIVE_DRAFT => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_PUBLISH      => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_WITHDRAW     => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_DUPLICATE    => CMS_CONTENT_ROLE_NONE,
                         CMS_CONTENT_ACTION_COPY         => CMS_CONTENT_ROLE_ALL,
                         CMS_CONTENT_ACTION_DELETE       => CMS_CONTENT_ROLE_ALL,
                         CMS_CONTENT_ACTION_RECEIVE      => CMS_CONTENT_ROLE_ALL,
                         CMS_CONTENT_ACTION_FLAG         => CMS_CONTENT_ROLE_PUBLISHER,
                         CMS_CONTENT_ACTION_CHANNEL      => CMS_CONTENT_ROLE_PUBLISHER]]];

        //establish database connection
        $mysql = new mysql();

        if (
             $mysql->verify_table(CMS_DB_CONTENT,
                                  [CMS_DB_CONTENT_INDEX               => "INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT",

                                   CMS_DB_CONTENT_OWNER               => "VARCHAR(40) NOT NULL DEFAULT ''",
                                   CMS_DB_CONTENT_TYPE                => "CHAR(1) NOT NULL DEFAULT ''",
                                   CMS_DB_CONTENT_STATUS              => "CHAR(1) NOT NULL DEFAULT ''",
                                   CMS_DB_CONTENT_FLAG                => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_CONTENT_CHANNEL             => "TEXT NOT NULL",

                                   CMS_DB_CONTENT_WRITER              => "VARCHAR(40) NOT NULL DEFAULT ''",
                                   CMS_DB_CONTENT_WRITER_TIME         => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_CONTENT_WRITER_COMMENT      => "TEXT NOT NULL",

                                   CMS_DB_CONTENT_EDITOR              => "VARCHAR(40) NOT NULL DEFAULT ''",
                                   CMS_DB_CONTENT_EDITOR_TIME         => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_CONTENT_EDITOR_COMMENT      => "TEXT NOT NULL",

                                   CMS_DB_CONTENT_PUBLISHER           => "VARCHAR(40) NOT NULL DEFAULT ''",
                                   CMS_DB_CONTENT_PUBLISHER_TIME      => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_CONTENT_PUBLISHER_COMMENT   => "TEXT NOT NULL",

                                   CMS_DB_CONTENT_TIME                => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_CONTENT_TITLE               => "TEXT NOT NULL",
                                   CMS_DB_CONTENT_AUTHOR              => "VARCHAR(40) NOT NULL DEFAULT ''",
                                   CMS_DB_CONTENT_DESCRIPTION         => "TEXT NOT NULL",
                                   CMS_DB_CONTENT_KEYWORD             => "TEXT NOT NULL",
                                   CMS_DB_CONTENT_IMAGE               => "TEXT NOT NULL",
                                   CMS_DB_CONTENT_TEXT                => "MEDIUMTEXT",
                                   CMS_DB_CONTENT_TEMPLATE            => "VARCHAR(255) NOT NULL DEFAULT ''",

                                   CMS_DB_CONTENT_BUFFER_TITLE        => "TEXT NOT NULL",
                                   CMS_DB_CONTENT_BUFFER_AUTHOR       => "VARCHAR(40) NOT NULL DEFAULT ''",
                                   CMS_DB_CONTENT_BUFFER_DESCRIPTION  => "TEXT NOT NULL",
                                   CMS_DB_CONTENT_BUFFER_KEYWORD      => "TEXT NOT NULL",
                                   CMS_DB_CONTENT_BUFFER_IMAGE        => "TEXT NOT NULL",
                                   CMS_DB_CONTENT_BUFFER_TEXT         => "MEDIUMTEXT",
                                   CMS_DB_CONTENT_BUFFER_TEMPLATE     => "VARCHAR(255) NOT NULL DEFAULT ''",

                                   CMS_DB_CONTENT_SENDER              => "VARCHAR(40) NOT NULL DEFAULT ''",
                                   CMS_DB_CONTENT_SENDER_TIME         => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_CONTENT_SENDER_COMMENT      => "TEXT NOT NULL",

                                   CMS_DB_CONTENT_EXTRA_VALUE         => "TEXT NOT NULL",
                                   CMS_DB_CONTENT_EXTRA_TYPE          => "VARCHAR(10) NOT NULL DEFAULT ''",
                                   CMS_DB_CONTENT_EXTRA_COLOR         => "VARCHAR(40) NOT NULL DEFAULT ''"],
                                  ["INDEX (" . CMS_DB_CONTENT_OWNER . ", " .
                                               CMS_DB_CONTENT_TYPE . ", " .
                                               CMS_DB_CONTENT_STATUS . ")"])
             &&
             $mysql->verify_table(CMS_DB_CONTENT_VERSION,
                                  [CMS_DB_CONTENT_VERSION_INDEX       => "INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT",
                                   CMS_DB_CONTENT_VERSION_CONTENT     => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_CONTENT_VERSION_TIME        => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_CONTENT_VERSION_TITLE       => "TEXT NOT NULL",
                                   CMS_DB_CONTENT_VERSION_AUTHOR      => "VARCHAR(40) NOT NULL DEFAULT ''",
                                   CMS_DB_CONTENT_VERSION_DESCRIPTION => "TEXT NOT NULL",
                                   CMS_DB_CONTENT_VERSION_KEYWORD     => "TEXT NOT NULL",
                                   CMS_DB_CONTENT_VERSION_IMAGE       => "TEXT NOT NULL",
                                   CMS_DB_CONTENT_VERSION_TEXT        => "MEDIUMTEXT",
                                   CMS_DB_CONTENT_VERSION_TEMPLATE    => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_CONTENT_VERSION_HASH        => "CHAR(64) NOT NULL DEFAULT '' UNIQUE"],
                                  NULL,
                                  ["md5"                              => CMS_DB_CONTENT_VERSION_HASH])
             &&
             $mysql->verify_table(CMS_DB_CONTENT_SCHEDULE,
                                  [CMS_DB_CONTENT_SCHEDULE_TIME       => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_CONTENT_SCHEDULE_TYPE       => "CHAR(1) NOT NULL DEFAULT ''",
                                   CMS_DB_CONTENT_SCHEDULE_CONTENT    => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_CONTENT_SCHEDULE_VALUE_1    => "TEXT NOT NULL",
                                   CMS_DB_CONTENT_SCHEDULE_VALUE_2    => "TEXT NOT NULL",
                                   CMS_DB_CONTENT_SCHEDULE_HASH       => "CHAR(64) NOT NULL DEFAULT '' PRIMARY KEY"],
                                  NULL,
                                  ["md5"                              => CMS_DB_CONTENT_SCHEDULE_HASH])
           )
        {
            $this->enabled   = TRUE;

            //process schedule
            $this->writer    = TRUE;
            $this->editor    = TRUE;
            $this->publisher = TRUE;
            $this->operator  = TRUE;

            if ($result = mysql_query("SELECT * " .
                                      "FROM " .     CMS_DB_CONTENT_SCHEDULE . " " .
                                      "WHERE " .    CMS_DB_CONTENT_TIME . "<='" . time() . "' " .
                                      "ORDER BY " . CMS_DB_CONTENT_TIME))
            {
                while ($resultrow = mysql_fetch_assoc($result))
                {
                    $index = $resultrow[CMS_DB_CONTENT_SCHEDULE_CONTENT];

                    //retrieve current owner
                    if (
                         ($_result = mysql_query("SELECT " . CMS_DB_CONTENT_OWNER . " " .
                                                 "FROM " .   CMS_DB_CONTENT . " " .
                                                 "WHERE " .  CMS_DB_CONTENT_INDEX . "='" . sqlesc($index) . "' " .
                                                 "LIMIT 1"))
                         &&
                         ($this->user = mysql_result($_result, 0))
                       )
                    {
                        switch ($resultrow[CMS_DB_CONTENT_SCHEDULE_TYPE])
                        {
                        case CMS_CONTENT_SCHEDULE_TYPE_APPLY:

                            $this->apply($index);
                            break;

                        case CMS_CONTENT_SCHEDULE_TYPE_RETRIEVE:

                            $this->version_retrieve($resultrow[CMS_DB_CONTENT_SCHEDULE_VALUE_1], 0, TRUE);
                            break;

                        case CMS_CONTENT_SCHEDULE_TYPE_PUBLISH:

                            $this->publish(
                                $index,                                       //index
                                NULL,                                         //comment
                                0,                                            //time_publish
                                0,                                            //time_withdraw
                                $resultrow[CMS_DB_CONTENT_SCHEDULE_VALUE_1],  //directory_index
                                "replace",                                    //directory_action
                                $resultrow[CMS_DB_CONTENT_SCHEDULE_VALUE_2]); //directory_title
                            break;

                        case CMS_CONTENT_SCHEDULE_TYPE_WITHDRAW:

                            $this->withdraw(
                                $index,                                       //index
                                $resultrow[CMS_DB_CONTENT_SCHEDULE_VALUE_1]); //directory_index
                            break;
                        };
                    };

                    $this->_schedule_delete($resultrow[CMS_DB_CONTENT_SCHEDULE_HASH]);
                };
            };

            //permission
            if (stre($user)) $user = CMS_SUPERUSER;
            $this->user      = $user;
            $user            = streq($user, CMS_SUPERUSER) ? "" : "$user.";
            $this->writer    = cms_permission("interface.content.$user" . CMS_CONTENT_PERMISSION_WRITER,    FALSE, FALSE);
            $this->editor    = cms_permission("interface.content.$user" . CMS_CONTENT_PERMISSION_EDITOR,    FALSE, FALSE);
            $this->publisher = cms_permission("interface.content.$user" . CMS_CONTENT_PERMISSION_PUBLISHER, FALSE, FALSE);
            $this->operator  = cms_permission("interface.content." .      CMS_CONTENT_PERMISSION_OPERATOR,  FALSE, FALSE);
        };
    }

//methods
//--------------------------------------------------------------------------------------------------

    function test_create()
    {
        return $this->create("", NULL, NULL, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function create($title,
                    $template = NULL,
                    $comment  = NULL,
                    $test     = FALSE)
    {
        if (! $this->enabled) return FALSE;

        if ($this->action(CMS_CONTENT_TYPE_ORIGINAL,
                          CMS_CONTENT_STATUS_DRAFT,
                          CMS_CONTENT_ACTION_CREATE))
        {
            if ($test) return TRUE;

            if (language_get($title, FALSE) == "")
                $title = language_set($title, CMS_L_UNKNOWN, FALSE);

            $channel = cms_load("rss") ? rss_get_default() : "";
            $time    = time();
            $title   = sqlesc($title);
            $name    = sqlesc(CMS_NAME);

            if (mysql_query("INSERT " . CMS_DB_CONTENT . " " .
                            "(" .       CMS_DB_CONTENT_OWNER . ", " .
                                        CMS_DB_CONTENT_TYPE . ", " .
                                        CMS_DB_CONTENT_STATUS . ", " .
                                        CMS_DB_CONTENT_CHANNEL . ", " .
                                        CMS_DB_CONTENT_WRITER . ", " .
                                        CMS_DB_CONTENT_WRITER_TIME . ", " .
                            (($comment === NULL) ? "" :
                                        CMS_DB_CONTENT_WRITER_COMMENT . ", ") .
                                        CMS_DB_CONTENT_TIME . ", " .
                                        CMS_DB_CONTENT_TITLE . ", " .
                                        CMS_DB_CONTENT_AUTHOR . ", " .
                            (($template === NULL) ? "" :
                                        CMS_DB_CONTENT_TEMPLATE . ", ") .
                                        CMS_DB_CONTENT_BUFFER_TITLE . ", " .
                                        CMS_DB_CONTENT_BUFFER_AUTHOR .
                            (($template === NULL) ? ") " : ", " .
                                        CMS_DB_CONTENT_BUFFER_TEMPLATE . ") ") .
                            "VALUES (" .
                            "'" .       sqlesc($this->user) . "', " .
                            "'" .       CMS_CONTENT_TYPE_ORIGINAL . "', " .
                            "'" .       CMS_CONTENT_STATUS_DRAFT . "', " .
                            "'" .       sqlesc($channel) . "', " .
                            "'" .       sqlesc(CMS_SUPERUSER) . "', " .
                            "'" .       $time . "', " .
                            (($comment === NULL) ? "" :
                            "'" .       sqlesc($comment) . "', ") .
                            "'" .       $time . "', " .
                            "'" .       $title . "', " .
                            "'" .       $name . "', " .
                            (($template === NULL) ? "" :
                            "'" .       sqlesc($template) . "', ") .
                            "'" .       $title . "', " .
                            "'" .       $name . "'" .
                            (($template === NULL) ? ")" : ", " .
                            "'" .       $template . "')")))
            {
                $index = mysql_insert_id();

                //clear step buffer due to possible remains
                $this->step_clear($index);

                return mysql_insert_id();
            };
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_update($index)
    {
        return $this->update($index, NULL, NULL, NULL, NULL, NULL, NULL, NULL, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function update($index,
                    $title       = NULL,
                    $description = NULL,
                    $keyword     = NULL,
                    $image       = NULL,
                    $text        = NULL,
                    $comment     = NULL,
                    $template    = NULL,
                    $test        = FALSE)
    {
        if (! $this->enabled) return FALSE;

        $external = cms_permission("content." . CMS_CONTENT_PERMISSION_WRITER . ".$index", FALSE, FALSE);
        $_index   = sqlesc($index);

        if (
             ($result = mysql_query("SELECT " . CMS_DB_CONTENT_OWNER . ", " .
                                                CMS_DB_CONTENT_TYPE . ", " .
                                                CMS_DB_CONTENT_STATUS . ", " .
                                                CMS_DB_CONTENT_WRITER . " " .
                                    "FROM " .   CMS_DB_CONTENT . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX . "='$_index' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
             &&
             (
               (
                 streq($resultrow[CMS_DB_CONTENT_OWNER], $this->user) //user must be owner
                 &&
                 $this->action($resultrow[CMS_DB_CONTENT_TYPE],
                               $resultrow[CMS_DB_CONTENT_STATUS],
                               CMS_CONTENT_ACTION_UPDATE)
               )
               ||
               (
                 $external
                 &&
                 ($resultrow[CMS_DB_CONTENT_STATUS] == CMS_CONTENT_STATUS_PUBLICATION)
               )
             )
           )
        {
            if (($title !== NULL) && (language_get($title, FALSE) == ""))
                $title = language_set($title, CMS_L_UNKNOWN, FALSE);

            $status = ($resultrow[CMS_DB_CONTENT_STATUS] == CMS_CONTENT_STATUS_PUBLICATION) ?
                      CMS_CONTENT_STATUS_PUBLICATION : CMS_CONTENT_STATUS_DRAFT;
            $writer = $external ? $resultrow[CMS_DB_CONTENT_WRITER] : $this->user;

            if ($test) return TRUE;

            //store current step
            $this->step_store($index);

            if (mysql_query("UPDATE " . CMS_DB_CONTENT . " " .
                            "SET " .    CMS_DB_CONTENT_TYPE .               "='" . CMS_CONTENT_TYPE_ORIGINAL . "', " .
                                        CMS_DB_CONTENT_STATUS .             "='" . sqlesc($status) . "', " .
                                        CMS_DB_CONTENT_WRITER .             "='" . sqlesc($writer) . "', " .
                                        CMS_DB_CONTENT_WRITER_TIME .        "='" . time() . "', " .
                            (($comment === NULL) ? "" :
                                        CMS_DB_CONTENT_WRITER_COMMENT .     "='" . sqlesc($comment) . "', ") .
                            (($title === NULL) ? "" :
                                        CMS_DB_CONTENT_BUFFER_TITLE .       "='" . sqlesc($title) . "', ") .
                                        CMS_DB_CONTENT_BUFFER_AUTHOR .      "='" . sqlesc(CMS_NAME) . "'" .
                            (($description === NULL) ? "" : ", " .
                                        CMS_DB_CONTENT_BUFFER_DESCRIPTION . "='" . sqlesc($description) . "'") .
                            (($keyword === NULL) ? "" : ", " .
                                        CMS_DB_CONTENT_BUFFER_KEYWORD .     "='" . sqlesc($keyword) . "'") .
                            (($image === NULL) ? "" : ", " .
                                        CMS_DB_CONTENT_BUFFER_IMAGE .       "='" . sqlesc($image) . "'") .
                            (($text === NULL) ? "" : ", " .
                                        CMS_DB_CONTENT_BUFFER_TEXT .        "='" . sqlesc($text) . "'") .
                            (($template === NULL) ? " " : ", " .
                                        CMS_DB_CONTENT_BUFFER_TEMPLATE .    "='" . sqlesc($template) . "' ") .
                            "WHERE " .  CMS_DB_CONTENT_INDEX .              "='" . $_index . "' " .
                            "LIMIT 1"))
            {
                $this->refresh_extra($index);
                return TRUE;
            };
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_copy($index)
    {
        return $this->copy($index, FALSE, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function copy($index,
                  $duplicate = FALSE,
                  $test      = FALSE)
    {
        if (! $this->enabled) return FALSE;

        if ($duplicate)
        {
            $action = CMS_CONTENT_ACTION_DUPLICATE;
            $type   = CMS_CONTENT_TYPE_DUPLICATE;
        }
        else
        {
            $action = CMS_CONTENT_ACTION_COPY;
            $type   = CMS_CONTENT_TYPE_COPY;
        };

        $index = sqlesc($index);

        if (
             ($result = mysql_query("SELECT * " .
                                    "FROM " .   CMS_DB_CONTENT . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX . "='$index' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
             &&
             streq($resultrow[CMS_DB_CONTENT_OWNER], $this->user) //user must be owner
             &&
             $this->action($resultrow[CMS_DB_CONTENT_TYPE],
                           $resultrow[CMS_DB_CONTENT_STATUS],
                           $action)
           )
        {
            if ($test) return TRUE;

            $status = $resultrow[CMS_DB_CONTENT_STATUS];
            if ($action == CMS_CONTENT_ACTION_DUPLICATE)       $status = CMS_CONTENT_STATUS_DRAFT;
            elseif ($status == CMS_CONTENT_STATUS_PUBLICATION) $status = CMS_CONTENT_STATUS_DOCUMENT;

            if (mysql_query("INSERT " . CMS_DB_CONTENT . " " .
                            "(" .       CMS_DB_CONTENT_OWNER . ", " .
                                        CMS_DB_CONTENT_TYPE . ", " .
                                        CMS_DB_CONTENT_STATUS . ", " .
                                        CMS_DB_CONTENT_FLAG . ", " .
                                        CMS_DB_CONTENT_CHANNEL . ", " .
                                        CMS_DB_CONTENT_WRITER . ", " .
                                        CMS_DB_CONTENT_WRITER_TIME . ", " .
                                        CMS_DB_CONTENT_WRITER_COMMENT . ", " .
                                        CMS_DB_CONTENT_EDITOR . ", " .
                                        CMS_DB_CONTENT_EDITOR_TIME . ", " .
                                        CMS_DB_CONTENT_EDITOR_COMMENT . ", " .
                                        CMS_DB_CONTENT_PUBLISHER . ", " .
                                        CMS_DB_CONTENT_PUBLISHER_TIME . ", " .
                                        CMS_DB_CONTENT_TITLE . ", " .
                                        CMS_DB_CONTENT_AUTHOR . ", " .
                                        CMS_DB_CONTENT_DESCRIPTION . ", " .
                                        CMS_DB_CONTENT_KEYWORD . ", " .
                                        CMS_DB_CONTENT_IMAGE . ", " .
                                        CMS_DB_CONTENT_TIME . ", " .
                                        CMS_DB_CONTENT_TEXT . ", " .
                                        CMS_DB_CONTENT_TEMPLATE . ", " .
                                        CMS_DB_CONTENT_BUFFER_TITLE . ", " .
                                        CMS_DB_CONTENT_BUFFER_AUTHOR . ", " .
                                        CMS_DB_CONTENT_BUFFER_DESCRIPTION . ", " .
                                        CMS_DB_CONTENT_BUFFER_KEYWORD . ", " .
                                        CMS_DB_CONTENT_BUFFER_IMAGE . ", " .
                                        CMS_DB_CONTENT_BUFFER_TEXT . ", " .
                                        CMS_DB_CONTENT_BUFFER_TEMPLATE . ", " .
                                        CMS_DB_CONTENT_SENDER . ", " .
                                        CMS_DB_CONTENT_SENDER_TIME . ", " .
                                        CMS_DB_CONTENT_SENDER_COMMENT . ", " .
                                        CMS_DB_CONTENT_EXTRA_TYPE . ", " .
                                        CMS_DB_CONTENT_EXTRA_VALUE . ") " .
                            "VALUES (" .
                            "'" .       sqlesc($this->user) . "', " .
                            "'" .       sqlesc($type) . "', " .
                            "'" .       sqlesc($status) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_FLAG]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_CHANNEL]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_WRITER]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_WRITER_TIME]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_WRITER_COMMENT]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_EDITOR]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_EDITOR_TIME]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_EDITOR_COMMENT]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_PUBLISHER]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_PUBLISHER_TIME]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_TITLE]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_AUTHOR]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_DESCRIPTION]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_KEYWORD]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_IMAGE]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_TIME]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_TEXT]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_TEMPLATE]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_BUFFER_TITLE]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_BUFFER_AUTHOR]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_BUFFER_DESCRIPTION]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_BUFFER_KEYWORD]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_BUFFER_IMAGE]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_BUFFER_TEXT]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_BUFFER_TEMPLATE]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_SENDER]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_SENDER_TIME]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_SENDER_COMMENT]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_EXTRA_TYPE]) . "', " .
                            "'" .       sqlesc($resultrow[CMS_DB_CONTENT_EXTRA_VALUE]) . "')"))
            {
                $_index  = mysql_insert_id();
                $__index = sqlesc($_index);

                if ($duplicate)
                    mysql_query("INSERT " .  CMS_DB_CONTENT_VERSION . " " .
                                "(" .        CMS_DB_CONTENT_VERSION_CONTENT . ", " .
                                             CMS_DB_CONTENT_VERSION_TIME . ", " .
                                             CMS_DB_CONTENT_VERSION_TITLE . ", " .
                                             CMS_DB_CONTENT_VERSION_AUTHOR . ", " .
                                             CMS_DB_CONTENT_VERSION_DESCRIPTION . ", " .
                                             CMS_DB_CONTENT_VERSION_KEYWORD . ", " .
                                             CMS_DB_CONTENT_VERSION_IMAGE . ", " .
                                             CMS_DB_CONTENT_VERSION_TEXT . ", " .
                                             CMS_DB_CONTENT_VERSION_TEMPLATE . ", " .
                                             CMS_DB_CONTENT_VERSION_HASH . ") " .
                                "SELECT '" . $__index . "', " .
                                             CMS_DB_CONTENT_VERSION_TIME . ", " .
                                             CMS_DB_CONTENT_VERSION_TITLE . ", " .
                                             CMS_DB_CONTENT_VERSION_AUTHOR . ", " .
                                             CMS_DB_CONTENT_VERSION_DESCRIPTION . ", " .
                                             CMS_DB_CONTENT_VERSION_KEYWORD . ", " .
                                             CMS_DB_CONTENT_VERSION_IMAGE . ", " .
                                             CMS_DB_CONTENT_VERSION_TEXT . ", " .
                                             CMS_DB_CONTENT_VERSION_TEMPLATE . ", " .
                                             "SHA2(CONCAT_WS('|', " .
                                                             $__index . ", " .
                                                             CMS_DB_CONTENT_VERSION_TITLE . ", " .
                                                             CMS_DB_CONTENT_VERSION_DESCRIPTION . ", " .
                                                             CMS_DB_CONTENT_VERSION_KEYWORD . ", " .
                                                             CMS_DB_CONTENT_VERSION_IMAGE . ", " .
                                                             CMS_DB_CONTENT_VERSION_TEXT . ", " .
                                                             CMS_DB_CONTENT_VERSION_TEMPLATE . "), 256) " .
                                "FROM " .    CMS_DB_CONTENT_VERSION . " " .
                                "WHERE " .   CMS_DB_CONTENT_VERSION_CONTENT .
                                "='" .       $index . "'");

                //clear step buffer due to possible remains
                $this->step_clear($_index);

                return $_index;
            };
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_duplicate($index)
    {
        return $this->copy($index, TRUE, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function duplicate($index)
    {
        return $this->copy($index, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function test_authorize($index)
    {
        return $this->authorize($index, NULL, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function authorize($index,
                       $comment = NULL,
                       $test    = FALSE)
    {
        if (! $this->enabled) return FALSE;

        $_index = sqlesc($index);

        if (
             ($result = mysql_query("SELECT " . CMS_DB_CONTENT_OWNER . ", " .
                                                CMS_DB_CONTENT_TYPE . ", " .
                                                CMS_DB_CONTENT_STATUS . " " .
                                    "FROM " .   CMS_DB_CONTENT . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX . "='$_index' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
             &&
             streq($resultrow[CMS_DB_CONTENT_OWNER], $this->user) //user must be owner
             &&
             $this->action($resultrow[CMS_DB_CONTENT_TYPE],
                           $resultrow[CMS_DB_CONTENT_STATUS],
                           CMS_CONTENT_ACTION_AUTHORIZE)
             &&
             (
               $test
               ||
               mysql_query("UPDATE " . CMS_DB_CONTENT . " " .
                           "SET " .    CMS_DB_CONTENT_STATUS .         "='" . CMS_CONTENT_STATUS_DOCUMENT . "', " .
                                       CMS_DB_CONTENT_EDITOR .         "='" . sqlesc(CMS_NAME) . "', " .
                                       CMS_DB_CONTENT_EDITOR_TIME .    "='" . time() . "'" .
                           (($comment === NULL) ? " " : ", " .
                                       CMS_DB_CONTENT_EDITOR_COMMENT . "='" . sqlesc($comment) . "' ") .
                           "WHERE " .  CMS_DB_CONTENT_INDEX .          "='" . $_index . "' " .
                           "LIMIT 1")
             )
           )
        {
            //clear step buffer
            if (! $test) $this->step_clear($index);
            return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_derive_draft($index)
    {
        return $this->derive_draft($index, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function derive_draft($index,
                          $test = FALSE)
    {
        if (! $this->enabled) return FALSE;

        $_index = sqlesc($index);

        if (
             ($result = mysql_query("SELECT " . CMS_DB_CONTENT_OWNER . ", " .
                                                CMS_DB_CONTENT_TYPE . ", " .
                                                CMS_DB_CONTENT_STATUS . " " .
                                    "FROM " .   CMS_DB_CONTENT . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX . "='$_index' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
             &&
             streq($resultrow[CMS_DB_CONTENT_OWNER], $this->user) //user must be owner
             &&
             $this->action($resultrow[CMS_DB_CONTENT_TYPE],
                           $resultrow[CMS_DB_CONTENT_STATUS],
                           CMS_CONTENT_ACTION_DERIVE_DRAFT)
             &&
             (
               $test
               ||
               mysql_query("UPDATE " . CMS_DB_CONTENT . " " .
                           "SET " .    CMS_DB_CONTENT_STATUS . "='" . CMS_CONTENT_STATUS_DRAFT . "' " .
                           "WHERE " .  CMS_DB_CONTENT_INDEX .  "='" . $_index . "'")
             )
           )
        {
            //clear step buffer
            if (! $test) $this->step_clear($index);
            return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_publish($index)
    {
        return $this->publish($index, NULL, 0, 0, NULL, NULL, NULL, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function publish($index,
                     $comment          = NULL,
                     $time_publish     = 0,
                     $time_withdraw    = 0,
                     $directory_index  = NULL,
                     $directory_action = NULL, //replace, insert, append
                     $directory_title  = NULL,
                     $test             = FALSE)
    {
        if (! $this->enabled) return FALSE;

        $_index = sqlesc($index);

        if (
             ($result = mysql_query("SELECT " . CMS_DB_CONTENT_OWNER . ", " .
                                                CMS_DB_CONTENT_TYPE . ", " .
                                                CMS_DB_CONTENT_STATUS . ", " .
                                                CMS_DB_CONTENT_TITLE . ", " .
                                                CMS_DB_CONTENT_DESCRIPTION . " " .
                                    "FROM " .   CMS_DB_CONTENT . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX . "='$_index' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
             &&
             streq($resultrow[CMS_DB_CONTENT_OWNER], $this->user) //user must be owner
             &&
             $this->action($resultrow[CMS_DB_CONTENT_TYPE],
                           $resultrow[CMS_DB_CONTENT_STATUS],
                           CMS_CONTENT_ACTION_PUBLISH)
           )
        {
            if ($test) return TRUE;

            $time = time();

            //scheduled publication
            if ($time_publish > $time)
            {
                //directory
                if ($directory_action !== NULL)
                {
                    if (! cms_load("directory")) return FALSE;

                    $directory = new directory();

                    //create placeholder to be used on activation
                    switch ($directory_action)
                    {
                    case "insert":
//..................................................................................................

                        if ($directory_title === NULL) $directory_title = $resultrow[CMS_DB_CONTENT_TITLE];
                        $directory_index = $directory->insert($directory_index,                       //key
                                                              $directory_title,                       //name
                                                              $resultrow[CMS_DB_CONTENT_DESCRIPTION], //description
                                                              NULL,                                   //url
                                                              NULL,                                   //subtype
                                                              NULL,                                   //hidden
                                                              TRUE);                                  //placeholder
                        break;

                    case "append":
//..................................................................................................

                        if ($directory_title === NULL) $directory_title = $resultrow[CMS_DB_CONTENT_TITLE];
                        $directory_index = $directory->append($directory_index,                       //key
                                                              $directory_title,                       //name
                                                              $resultrow[CMS_DB_CONTENT_DESCRIPTION], //description
                                                              NULL,                                   //url
                                                              NULL,                                   //subtype
                                                              NULL,                                   //hidden
                                                              TRUE);                                  //placeholder
                        break;
                    };

                    if (! $directory->save()) return FALSE;

                    //schedule
                    if (
                         $this->schedule_add($time_publish,                     //time
                                             CMS_CONTENT_SCHEDULE_TYPE_PUBLISH, //type
                                             $index,                            //content_index
                                             $directory_index,                  //value1
                                             $directory_title)                  //value2
                         &&
                         (
                           ($time_withdraw <= $time_publish)
                           ||
                           $this->schedule_add($time_withdraw,                     //time
                                               CMS_CONTENT_SCHEDULE_TYPE_WITHDRAW, //type
                                               $index,                             //content_index
                                               $directory_index,                   //value1
                                               $directory_title)                   //value2
                         )
                       )
                        return $directory_index;

                    return FALSE;
                };

                //schedule
                if (
                     $this->schedule_add($time_publish,                     //time
                                         CMS_CONTENT_SCHEDULE_TYPE_PUBLISH, //type
                                         $index)                            //content_index
                     &&
                     (
                       ($time_withdraw <= $time_publish)
                       ||
                       $this->schedule_add($time_withdraw,                     //time
                                           CMS_CONTENT_SCHEDULE_TYPE_WITHDRAW, //type
                                           $index)                             //content_index
                     )
                   )
                    return TRUE;

                return FALSE;
            };

            //immediate publication
            if (mysql_query("UPDATE " . CMS_DB_CONTENT . " " .
                            "SET " .    CMS_DB_CONTENT_STATUS .            "='" . CMS_CONTENT_STATUS_PUBLICATION . "', " .
                                        CMS_DB_CONTENT_PUBLISHER .         "='" . sqlesc(CMS_NAME) . "', " .
                                        CMS_DB_CONTENT_PUBLISHER_TIME .    "='" . time() . "'" .
                            (($comment === NULL) ? " " : ", " .
                                        CMS_DB_CONTENT_PUBLISHER_COMMENT . "='" . sqlesc($comment) . "' ") .
                            "WHERE " .  CMS_DB_CONTENT_INDEX .             "='" . $_index . "' " .
                            "LIMIT 1"))
            {
                //clear step buffer
                $this->step_clear($index);

                //directory
                if ($directory_action !== NULL)
                {
                    if (! cms_load("directory")) return FALSE;

                    $directory = new directory();

                    //link to directory
                    switch ($directory_action)
                    {

                    //replace or activate scheduled publication
                    case "replace":
//..................................................................................................

                        $value                = $directory->data->get($directory_index);
                        if ($directory_title !== NULL)   $value["name"] = $directory_title;
                        elseif (! isset($value["name"])) $value["name"] = $resultrow[CMS_DB_CONTENT_TITLE];
                        $value["description"] = $resultrow[CMS_DB_CONTENT_DESCRIPTION];
                        $value["url"]         = "content://$index";
                        $directory->data->set($value, $directory_index);
                        break;

                    //create and connect placeholder
                    case "insert":
//..................................................................................................

                        if ($directory_title === NULL) $directory_title = $resultrow[CMS_DB_CONTENT_TITLE];
                        $directory_index = $directory->insert($directory_index,                       //key
                                                              $directory_title,                       //name
                                                              $resultrow[CMS_DB_CONTENT_DESCRIPTION], //description
                                                              "content://$index",                     //url
                                                              NULL,                                   //subtype
                                                              NULL,                                   //hidden
                                                              TRUE);                                  //placeholder
                        break;

                    case "append":
//..................................................................................................

                        if ($directory_title === NULL) $directory_title = $resultrow[CMS_DB_CONTENT_TITLE];
                        $directory_index = $directory->append($directory_index,                       //key
                                                              $directory_title,                       //name
                                                              $resultrow[CMS_DB_CONTENT_DESCRIPTION], //description
                                                              "content://$index",                     //url
                                                              NULL,                                   //subtype
                                                              NULL,                                   //hidden
                                                              TRUE);                                  //placeholder
                        break;
                    };

                    if (! $directory->save()) return FALSE;

                    //schedule
                    if (
                         ($time_withdraw <= $time)
                         ||
                         $this->schedule_add($time_withdraw,                     //time
                                             CMS_CONTENT_SCHEDULE_TYPE_WITHDRAW, //type
                                             $index,                             //content_index
                                             $directory_index,                   //value1
                                             $directory_title)                   //value2
                       )
                        return $directory_index;

                    return FALSE;
                };

                //schedule
                if (
                     ($time_withdraw <= $time)
                     ||
                     $this->schedule_add($time_withdraw,                     //time
                                         CMS_CONTENT_SCHEDULE_TYPE_WITHDRAW, //type
                                         $index)                             //content_index
                   )
                    return TRUE;
            };
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_apply($index)
    {
        return $this->apply($index, 0, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function apply($index,
                   $time = 0,
                   $test = FALSE)
    {
        if (! $this->test_update($index)) return FALSE;

        if ($test)
        {
            if (
                 ($result = mysql_query("SELECT " . CMS_DB_CONTENT_WRITER_TIME .
                                        "!=" .      CMS_DB_CONTENT_TIME . " " .
                                        "FROM " .   CMS_DB_CONTENT . " " .
                                        "WHERE " .  CMS_DB_CONTENT_INDEX .
                                        "='" .      sqlesc($index) . "' " .
                                        "LIMIT 1"))
                 &&
                 mysql_result($result, 0)
               )
                return TRUE;

            return FALSE;
        };

        //scheduled application
        if ($time > time())
            return $this->schedule_add(
                $time,
                CMS_CONTENT_SCHEDULE_TYPE_APPLY,
                $index);

        //immediate application
        if (

        //delete scheduled application
             $this->schedule_delete(
                 CMS_CONTENT_SCHEDULE_TYPE_APPLY,
                 $index)
             &&

        //automatically store version
             $this->_version_store($index)
             &&

        //apply
             mysql_query("UPDATE " . CMS_DB_CONTENT . " " .
                         "SET " .    CMS_DB_CONTENT_TIME .        "=" .  CMS_DB_CONTENT_WRITER_TIME . ", " .
                                     CMS_DB_CONTENT_TITLE .       "=" .  CMS_DB_CONTENT_BUFFER_TITLE . ", " .
                                     CMS_DB_CONTENT_AUTHOR .      "=" .  CMS_DB_CONTENT_BUFFER_AUTHOR . ", " .
                                     CMS_DB_CONTENT_DESCRIPTION . "=" .  CMS_DB_CONTENT_BUFFER_DESCRIPTION . ", " .
                                     CMS_DB_CONTENT_KEYWORD .     "=" .  CMS_DB_CONTENT_BUFFER_KEYWORD . ", " .
                                     CMS_DB_CONTENT_IMAGE .       "=" .  CMS_DB_CONTENT_BUFFER_IMAGE . ", " .
                                     CMS_DB_CONTENT_TEXT .        "=" .  CMS_DB_CONTENT_BUFFER_TEXT . ", " .
                                     CMS_DB_CONTENT_TEMPLATE .    "=" .  CMS_DB_CONTENT_BUFFER_TEMPLATE . " " .
                         "WHERE " .  CMS_DB_CONTENT_INDEX .       "='" . sqlesc($index) . "' " .
                         "LIMIT 1")
           )
        {
            //synchronize pool
            if (cms_load("content_pool"))
            {
                $pool = new content_pool();
                $pool->synchronize_content($index);
            };

            return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_revert($index)
    {
        return $this->revert($index, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function revert($index,
                    $test = FALSE)
    {
        if (! $this->test_update($index)) return FALSE;
        if ($test)                        return TRUE;

        //store current state
        $this->step_store($index);

        if (mysql_query("UPDATE " . CMS_DB_CONTENT . " " .
                        "SET " .    CMS_DB_CONTENT_WRITER_TIME .        "=" .  CMS_DB_CONTENT_TIME . ", " .
                                    CMS_DB_CONTENT_BUFFER_TITLE .       "=" .  CMS_DB_CONTENT_TITLE . ", " .
                                    CMS_DB_CONTENT_BUFFER_AUTHOR .      "=" .  CMS_DB_CONTENT_AUTHOR . ", " .
                                    CMS_DB_CONTENT_BUFFER_DESCRIPTION . "=" .  CMS_DB_CONTENT_DESCRIPTION . ", " .
                                    CMS_DB_CONTENT_BUFFER_KEYWORD .     "=" .  CMS_DB_CONTENT_KEYWORD . ", " .
                                    CMS_DB_CONTENT_BUFFER_IMAGE .       "=" .  CMS_DB_CONTENT_IMAGE . ", " .
                                    CMS_DB_CONTENT_BUFFER_TEXT .        "=" .  CMS_DB_CONTENT_TEXT . ", " .
                                    CMS_DB_CONTENT_BUFFER_TEMPLATE .    "=" .  CMS_DB_CONTENT_TEMPLATE . " " .
                        "WHERE " .  CMS_DB_CONTENT_INDEX .              "='" . sqlesc($index) . "' " .
                        "LIMIT 1"))
        {
            $this->refresh_extra($index);
            return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_version_store($index)
    {
        return $this->version_store($index, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function version_store($index,
                           $test = FALSE)
    {
        if (
             $this->test_update($index)
             &&
             (
               $test
               ||
               $this->_version_store($index)
             )
           )
            return TRUE;

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function _version_store($version_index)
    {
        return mysql_query("INSERT IGNORE INTO " . CMS_DB_CONTENT_VERSION . " " .
                           "(" .                   CMS_DB_CONTENT_VERSION_CONTENT . ", " .
                                                   CMS_DB_CONTENT_VERSION_TIME . ", " .
                                                   CMS_DB_CONTENT_VERSION_TITLE . ", " .
                                                   CMS_DB_CONTENT_VERSION_AUTHOR . ", " .
                                                   CMS_DB_CONTENT_VERSION_DESCRIPTION . ", " .
                                                   CMS_DB_CONTENT_VERSION_KEYWORD . ", " .
                                                   CMS_DB_CONTENT_VERSION_IMAGE . ", " .
                                                   CMS_DB_CONTENT_VERSION_TEXT . ", " .
                                                   CMS_DB_CONTENT_VERSION_TEMPLATE . ", " .
                                                   CMS_DB_CONTENT_VERSION_HASH . ") " .
                           "SELECT " .             CMS_DB_CONTENT_INDEX . ", " .
                                                   CMS_DB_CONTENT_WRITER_TIME . ", " .
                                                   CMS_DB_CONTENT_BUFFER_TITLE . ", " .
                                                   CMS_DB_CONTENT_BUFFER_AUTHOR . ", " .
                                                   CMS_DB_CONTENT_BUFFER_DESCRIPTION . ", " .
                                                   CMS_DB_CONTENT_BUFFER_KEYWORD . ", " .
                                                   CMS_DB_CONTENT_BUFFER_IMAGE . ", " .
                                                   CMS_DB_CONTENT_BUFFER_TEXT . ", " .
                                                   CMS_DB_CONTENT_BUFFER_TEMPLATE . ", " .
                                                   "SHA2(CONCAT_WS('|', " .
                                                                   CMS_DB_CONTENT_INDEX . ", " .
                                                                   CMS_DB_CONTENT_BUFFER_TITLE . ", " .
                                                                   CMS_DB_CONTENT_BUFFER_DESCRIPTION . ", " .
                                                                   CMS_DB_CONTENT_BUFFER_KEYWORD . ", " .
                                                                   CMS_DB_CONTENT_BUFFER_IMAGE . ", " .
                                                                   CMS_DB_CONTENT_BUFFER_TEXT . ", " .
                                                                   CMS_DB_CONTENT_BUFFER_TEMPLATE . "), 256) " .
                           "FROM " .               CMS_DB_CONTENT . " " .
                           "WHERE " .              CMS_DB_CONTENT_INDEX . "='" . sqlesc($version_index) . "' " .
                           "LIMIT 1") ?
            TRUE : FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_version_retrieve($version_index)
    {
        return $this->version_retrieve($version_index, 0, FALSE, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function version_retrieve($version_index,
                              $time  = 0,
                              $apply = FALSE,
                              $test  = FALSE)
    {
        if (! $this->enabled) return FALSE;

        if (
             ($result = mysql_query("SELECT " . CMS_DB_CONTENT_VERSION_CONTENT . ", " .
                                                CMS_DB_CONTENT_VERSION_TIME . ", " .
                                                CMS_DB_CONTENT_VERSION_TITLE . ", " .
                                                CMS_DB_CONTENT_VERSION_AUTHOR . ", " .
                                                CMS_DB_CONTENT_VERSION_DESCRIPTION . ", " .
                                                CMS_DB_CONTENT_VERSION_KEYWORD . ", " .
                                                CMS_DB_CONTENT_VERSION_IMAGE . ", " .
                                                CMS_DB_CONTENT_VERSION_TEXT . ", " .
                                                CMS_DB_CONTENT_VERSION_TEMPLATE . " " .
                                    "FROM " .   CMS_DB_CONTENT_VERSION . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX .
                                    "='" .      sqlesc($version_index) . "' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
             &&
             $this->test_update($resultrow[CMS_DB_CONTENT_VERSION_CONTENT])
           )
        {
            if ($test) return TRUE;

            $content_index = $resultrow[CMS_DB_CONTENT_VERSION_CONTENT];

            //scheduled retrieval
            if ($time > time())
                return $this->schedule_add($time,
                                           CMS_CONTENT_SCHEDULE_TYPE_RETRIEVE,
                                           $content_index,
                                           $version_index);

            //immediate retrieval

            //delete scheduled retrieval
            if ($this->schedule_delete(CMS_CONTENT_SCHEDULE_TYPE_RETRIEVE,
                                       $content_index,
                                       $version_index))
            {
                //store current state
                $this->step_store($content_index);

                //retrieve into active state
                if ($apply)
                    return mysql_query("UPDATE " . CMS_DB_CONTENT . " " .
                                       "SET " .    CMS_DB_CONTENT_TIME .
                                       "='" .      time() . "', " .
                                                   CMS_DB_CONTENT_TITLE .
                                       "='" .      sqlesc($resultrow[CMS_DB_CONTENT_VERSION_TITLE]) . "', " .
                                                   CMS_DB_CONTENT_AUTHOR .
                                       "='" .      sqlesc($resultrow[CMS_DB_CONTENT_VERSION_AUTHOR]) . "', " .
                                                   CMS_DB_CONTENT_DESCRIPTION .
                                       "='" .      sqlesc($resultrow[CMS_DB_CONTENT_VERSION_DESCRIPTION]) . "', " .
                                                   CMS_DB_CONTENT_KEYWORD .
                                       "='" .      sqlesc($resultrow[CMS_DB_CONTENT_VERSION_KEYWORD]) . "', " .
                                                   CMS_DB_CONTENT_IMAGE .
                                       "='" .      sqlesc($resultrow[CMS_DB_CONTENT_VERSION_IMAGE]) . "', " .
                                                   CMS_DB_CONTENT_TEXT .
                                       "='" .      sqlesc($resultrow[CMS_DB_CONTENT_VERSION_TEXT]) . "', " .
                                                   CMS_DB_CONTENT_TEMPLATE .
                                       "='" .      sqlesc($resultrow[CMS_DB_CONTENT_VERSION_TEMPLATE]) . "' " .
                                       "WHERE " .  CMS_DB_CONTENT_INDEX .
                                       "='" .      sqlesc($content_index) . "' " .
                                       "LIMIT 1") ?
                           TRUE : FALSE;

                //retrieve into buffered state
                if (mysql_query("UPDATE " . CMS_DB_CONTENT . " " .
                                "SET " .    CMS_DB_CONTENT_WRITER_TIME .
                                "='" .      sqlesc($resultrow[CMS_DB_CONTENT_VERSION_TIME]) . "', " .
                                            CMS_DB_CONTENT_BUFFER_TITLE .
                                "='" .      sqlesc($resultrow[CMS_DB_CONTENT_VERSION_TITLE]) . "', " .
                                            CMS_DB_CONTENT_BUFFER_AUTHOR .
                                "='" .      sqlesc($resultrow[CMS_DB_CONTENT_VERSION_AUTHOR]) . "', " .
                                            CMS_DB_CONTENT_BUFFER_DESCRIPTION .
                                "='" .      sqlesc($resultrow[CMS_DB_CONTENT_VERSION_DESCRIPTION]) . "', " .
                                            CMS_DB_CONTENT_BUFFER_KEYWORD .
                                "='" .      sqlesc($resultrow[CMS_DB_CONTENT_VERSION_KEYWORD]) . "', " .
                                            CMS_DB_CONTENT_BUFFER_IMAGE .
                                "='" .      sqlesc($resultrow[CMS_DB_CONTENT_VERSION_IMAGE]) . "', " .
                                            CMS_DB_CONTENT_BUFFER_TEXT .
                                "='" .      sqlesc($resultrow[CMS_DB_CONTENT_VERSION_TEXT]) . "', " .
                                            CMS_DB_CONTENT_BUFFER_TEMPLATE .
                                "='" .      sqlesc($resultrow[CMS_DB_CONTENT_VERSION_TEMPLATE]) . "' " .
                                "WHERE " .  CMS_DB_CONTENT_INDEX .
                                "='" .      sqlesc($content_index) . "' " .
                                "LIMIT 1"))
                {
                    $this->refresh_extra($content_index);
                    return TRUE;
                };
            };
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function schedule_add($time,
                          $type,
                          $content_index,
                          $value1 = NULL,
                          $value2 = NULL)
    {
        if (! $this->enabled) return FALSE;

        if (mysql_query("REPLACE " . CMS_DB_CONTENT_SCHEDULE . " " .
                        "(" .        CMS_DB_CONTENT_SCHEDULE_TIME . ", " .
                                     CMS_DB_CONTENT_SCHEDULE_TYPE . ", " .
                                     CMS_DB_CONTENT_SCHEDULE_CONTENT . ", " .
                                     CMS_DB_CONTENT_SCHEDULE_VALUE_1 . ", " .
                                     CMS_DB_CONTENT_SCHEDULE_VALUE_2 . ", " .
                                     CMS_DB_CONTENT_SCHEDULE_HASH . ") " .
                        "VALUES (" .
                        "'" .       sqlesc($time) . "', " .
                        "'" .       sqlesc($type) . "', " .
                        "'" .       sqlesc($content_index) . "', " .
                        "'" .       sqlesc($value1) . "', " .
                        "'" .       sqlesc($value2) . "', " .
                        "'" .       sqlesc(hash64("$type|$content_index|$value1|$value2")) . "')"))
            return TRUE;

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function schedule_delete($type,
                             $content_index,
                             $value1 = NULL,
                             $value2 = NULL)
    {
        return $this->_schedule_delete(hash64("$type|$content_index|$value1|$value2"));
    }

//--------------------------------------------------------------------------------------------------

    function _schedule_delete($hash)
    {
        if (! $this->enabled) return FALSE;

        if (mysql_query("DELETE FROM " . CMS_DB_CONTENT_SCHEDULE . " " .
                        "WHERE " .       CMS_DB_CONTENT_SCHEDULE_HASH .
                        "='" .           sqlesc($hash) . "' " .
                        "LIMIT 1"))
            return TRUE;

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_withdraw($index)
    {
        return $this->withdraw($index, NULL, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function withdraw($index,
                      $directory_index = NULL,
                      $test            = FALSE)
    {
        if (! $this->enabled) return FALSE;

        $_index = sqlesc($index);

        if (
             ($result = mysql_query("SELECT " . CMS_DB_CONTENT_OWNER . ", " .
                                                CMS_DB_CONTENT_TYPE . ", " .
                                                CMS_DB_CONTENT_STATUS . " " .
                                    "FROM " .   CMS_DB_CONTENT . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX . "='$_index' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
             &&
             streq($resultrow[CMS_DB_CONTENT_OWNER], $this->user) //user must be owner
             &&
             $this->action($resultrow[CMS_DB_CONTENT_TYPE],
                           $resultrow[CMS_DB_CONTENT_STATUS],
                           CMS_CONTENT_ACTION_WITHDRAW)
           )
        {
            if ($test) return TRUE;

            if (

            //withdraw only on no remaining linkage
                 $this->_withdraw($index, $directory_index)
                 &&

            //withdraw
                 mysql_query("UPDATE " . CMS_DB_CONTENT . " " .
                             "SET " .    CMS_DB_CONTENT_STATUS . "='" . CMS_CONTENT_STATUS_DOCUMENT . "' " .
                             "WHERE " .  CMS_DB_CONTENT_INDEX .  "='" . $_index . "' " .
                             "LIMIT 1")
               )
                return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function _withdraw($index,
                       $directory_index = NULL)
    {
        if (
             cms_load("directory")
             &&
             ($result = mysql_query("SELECT DISTINCT " . CMS_DB_CONTENT_SCHEDULE_VALUE_1 . " " .
                                    "FROM " .            CMS_DB_CONTENT_SCHEDULE . " " .
                                    "WHERE " .           CMS_DB_CONTENT_SCHEDULE_TYPE .
                                    "='" .               CMS_CONTENT_SCHEDULE_TYPE_PUBLISH . "'"))
           )
        {
            //generate scheduled usage list of directory indexes
            $array_schedule = NULL;
            while ($resultrow = mysql_fetch_row($result))
                $array_schedule[$resultrow[0]] = TRUE;

            //variable initialisation
            $directory = new directory();
            $array     = [];
            $length    = strlen($index) + 10;
            $depth     = 0;
            $limit     = 0;
            $count     = 0;

            //go through directory
            $directory->data->move("first");
            while (($key = $directory->data->move("next")) !== NULL)
            {
                switch ($directory->data->get($key, "#type"))
                {
                case "container":
//..................................................................................................

                    //check for linkage of specified content
                    $url = $directory->data->get($key, "url");
                    if (
                         streq(substr($url, 0, $length), "content://$index")
                         &&
                         (
                           stre($url = substr($url, $length, 1))
                           ||
                           ($url === "?")
                           ||
                           ($url === "#")
                         )
                       )
                    {
                        if (
                             ($directory_index === NULL)
                             ||
                             streq($key, $directory_index)
                           )

                            //release entry
                            $directory->data->del($key, "url");

                        else

                            //increase remainder count
                            $count++;
                    };

                    //check if entry may be removed
                    $array[++$depth] = $key;
                    if (
                         (! $directory->data->get($key, "dynamic"))
                         ||
                         ($directory->data->get($key, "url") != "")
                         ||
                         isset($array_schedule[$key])
                       )

                        //set cut off limit
                        $limit = $depth;

                    break;

                case "/container":
//..................................................................................................

                    if ($depth === 0)      break 2;
                    if ($depth > $limit)   $directory->data->del($array[$depth]);
                    if (--$depth < $limit) $limit = $depth;
                };
            };

            return $directory->save() && ($count === 0);
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_send($index)
    {
        return $this->send($index, $this->user, NULL, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function send($index,
                  $receiver,
                  $comment = NULL,
                  $test    = FALSE)
    {
        if (! $this->enabled) return FALSE;

        $index = sqlesc($index);

        if (
             ($result = mysql_query("SELECT " . CMS_DB_CONTENT_OWNER . ", " .
                                                CMS_DB_CONTENT_TYPE . ", " .
                                                CMS_DB_CONTENT_STATUS . ", " .
                                                CMS_DB_CONTENT_WRITER . " " .
                                    "FROM " .   CMS_DB_CONTENT . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX . "='$index' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
             &&
             streq($resultrow[CMS_DB_CONTENT_OWNER], $this->user) //user must be owner
           )
        {
            $writer          = $this->writer;
            $editor          = $this->editor;
            $publisher       = $this->publisher;
            $this->writer    = cms_permission("interface.content." . CMS_CONTENT_PERMISSION_WRITER,    FALSE, FALSE, $receiver);
            $this->editor    = cms_permission("interface.content." . CMS_CONTENT_PERMISSION_EDITOR,    FALSE, FALSE, $receiver);
            $this->publisher = cms_permission("interface.content." . CMS_CONTENT_PERMISSION_PUBLISHER, FALSE, FALSE, $receiver);

            $flag            = $this->action($resultrow[CMS_DB_CONTENT_TYPE],
                                             $resultrow[CMS_DB_CONTENT_STATUS],
                                             CMS_CONTENT_ACTION_RECEIVE);

            $this->writer    = $writer;
            $this->editor    = $editor;
            $this->publisher = $publisher;

            if (
                 $flag
                 &&
                 (
                   $test
                   ||
                   mysql_query("UPDATE " . CMS_DB_CONTENT . " " .
                               "SET " .    CMS_DB_CONTENT_OWNER .          "='" . sqlesc($receiver) . "', " .
                                           CMS_DB_CONTENT_SENDER .         "='" . sqlesc(CMS_NAME) . "', " .
                                           CMS_DB_CONTENT_SENDER_TIME .    "='" . time() . "'" .
                               (($comment === NULL) ? " " : ", " .
                                           CMS_DB_CONTENT_SENDER_COMMENT . "='" . sqlesc($comment) . "' ") .
                               "WHERE " .  CMS_DB_CONTENT_INDEX .          "='" . $index . "' " .
                               "LIMIT 1")
                 )
               )
            {
                //clear step buffer
                if (! $test) $this->step_clear($index);
                return TRUE;
            };
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_delete($index)
    {
        return $this->delete($index, FALSE, FALSE, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function delete($index,
                    $ignore_directory = FALSE,
                    $override_owner   = FALSE,
                    $test             = FALSE)
    {
        if (! $this->enabled) return FALSE;

        $_index = sqlesc($index);

        if (
             ($result = mysql_query("SELECT " . CMS_DB_CONTENT_OWNER . ", " .
                                                CMS_DB_CONTENT_TYPE . ", " .
                                                CMS_DB_CONTENT_STATUS . " " .
                                    "FROM " .   CMS_DB_CONTENT . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX . "='$_index' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
             &&
             ($override_owner || streq($resultrow[CMS_DB_CONTENT_OWNER], $this->user)) //user must be owner
             &&
             $this->action($resultrow[CMS_DB_CONTENT_TYPE],
                           $resultrow[CMS_DB_CONTENT_STATUS],
                           CMS_CONTENT_ACTION_DELETE)
           )
        {
            if ($test)               return TRUE;

            if (! $ignore_directory) $this->_withdraw($index);

            if (

            //delete content
                 mysql_query("DELETE FROM " . CMS_DB_CONTENT . " " .
                             "WHERE " .       CMS_DB_CONTENT_INDEX . "='$_index' " .
                             "LIMIT 1")
                 &&

            //delete version
                 mysql_query("DELETE FROM " . CMS_DB_CONTENT_VERSION . " " .
                             "WHERE " .       CMS_DB_CONTENT_VERSION_CONTENT . "='$_index'")
                 &&

            //delete schedule
                 mysql_query("DELETE FROM " . CMS_DB_CONTENT_SCHEDULE . " " .
                             "WHERE " .       CMS_DB_CONTENT_SCHEDULE_CONTENT . "='$_index'")
               )
            {
                //clear step buffer
                $this->step_clear($index);
                return TRUE;
            };
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_flag_set($index)
    {
        return $this->flag_set($index, CMS_CONTENT_FLAG_NONE, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function flag_set($index,
                      $flag = CMS_CONTENT_FLAG_NONE,
                      $test = FALSE)
    {
        if (! $this->enabled) return FALSE;

        $index = sqlesc($index);

        if (
             ($result = mysql_query("SELECT " . CMS_DB_CONTENT_OWNER . ", " .
                                                CMS_DB_CONTENT_TYPE . ", " .
                                                CMS_DB_CONTENT_STATUS . " " .
                                    "FROM " .   CMS_DB_CONTENT . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX . "='$index' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
             &&
             streq($resultrow[CMS_DB_CONTENT_OWNER], $this->user) //user must be owner
             &&
             $this->action($resultrow[CMS_DB_CONTENT_TYPE],
                           $resultrow[CMS_DB_CONTENT_STATUS],
                           CMS_CONTENT_ACTION_FLAG)
           )
        {
            if ($test) return TRUE;

            if (mysql_query("UPDATE " . CMS_DB_CONTENT . " " .
                            "SET " .    CMS_DB_CONTENT_FLAG .  "='" . sqlesc($flag) . "' " .
                            "WHERE " .  CMS_DB_CONTENT_INDEX . "='" . $index . "' " .
                            "LIMIT 1"))
                     return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_channel_set($index)
    {
        return $this->channel_set($index, "", TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function channel_set($index,
                         $channel = "",
                         $test    = FALSE)
    {
        if (! $this->enabled) return FALSE;

        $index = sqlesc($index);

        if (
             ($result = mysql_query("SELECT " . CMS_DB_CONTENT_OWNER . ", " .
                                                CMS_DB_CONTENT_TYPE . ", " .
                                                CMS_DB_CONTENT_STATUS . " " .
                                    "FROM " .   CMS_DB_CONTENT . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX . "='$index' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_assoc($result))
             &&
             streq($resultrow[CMS_DB_CONTENT_OWNER], $this->user) //user must be owner
             &&
             $this->action($resultrow[CMS_DB_CONTENT_TYPE],
                           $resultrow[CMS_DB_CONTENT_STATUS],
                           CMS_CONTENT_ACTION_CHANNEL)
           )
        {
            if ($test) return TRUE;

            if (mysql_query("UPDATE " . CMS_DB_CONTENT . " " .
                            "SET " .    CMS_DB_CONTENT_CHANNEL . "='" . sqlesc($channel) . "' " .
                            "WHERE " .  CMS_DB_CONTENT_INDEX .   "='" . $index . "' " .
                            "LIMIT 1"))
                     return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function step_store($index)
    {
        if (! $this->enabled) return FALSE;

        //retrieve settings
        $array    = explode(";", cms_cache("content.$index.step.meta"));
        $offset   = isset($array[0]) ? (int)$array[0] : 0;
        $count    = isset($array[1]) ? (int)$array[1] : 0;
        $position = isset($array[2]) ? (int)$array[2] : 0;

        if (
             ($result = mysql_query("SELECT " . CMS_DB_CONTENT_BUFFER_TEXT . " " .
                                    "FROM " .   CMS_DB_CONTENT . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX .
                                    "='" .      sqlesc($index) . "' " .
                                    "LIMIT 1"))
             &&
             (mysql_num_rows($result) > 0)
           )
        {
            //store current state
            $_position  = $offset + $position;
            $_position %= 10;

            cms_cache("content.$index.step.$_position", mysql_result($result, 0),  TRUE);

            //increase storage position and save settings
            if ($position >= 9) $offset = ++$offset % 10;
            else                $count  = ++$position;

            cms_cache("content.$index.step.meta",       "$offset;$count;$position", TRUE);
            return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_step_undo($index)
    {
        return $this->step_undo($index, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function step_undo($index,
                       $test = FALSE)
    {
        if (! $this->enabled) return FALSE;

        //retrieve settings
        $array    = explode(";", cms_cache("content.$index.step.meta"));
        $offset   = isset($array[0]) ? (int)$array[0] : 0;
        $count    = isset($array[1]) ? (int)$array[1] : 0;
        $position = isset($array[2]) ? (int)$array[2] : 0;

        //validate
        if ($position < 1)                return FALSE;
        if (! $this->test_update($index)) return FALSE;
        if ($test)                        return TRUE;

        //store current state
        $_position  = $offset + $position;
        $_position %= 10;
        $_index     = sqlesc($index);

        if (
             ($result = mysql_query("SELECT " . CMS_DB_CONTENT_BUFFER_TEXT . " " .
                                    "FROM " .   CMS_DB_CONTENT . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX . "='$_index' " .
                                    "LIMIT 1"))
             &&
             (mysql_num_rows($result) > 0)
           )
            cms_cache("content.$index.step.$_position", mysql_result($result, 0),  TRUE);

        //decrease storage position and restore state
        $position--;
        $_position  = $offset + $position;
        $_position %= 10;

        if (mysql_query("UPDATE " . CMS_DB_CONTENT . " " .
                        "SET " .    CMS_DB_CONTENT_WRITER_TIME .
                        "='" .      sqlesc(time()) . "', " .
                                    CMS_DB_CONTENT_BUFFER_TEXT .
                        "='" .      sqlesc(cms_cache("content.$index.step.$_position")) . "' " .
                        "WHERE " .  CMS_DB_CONTENT_INDEX .
                        "='" .      $_index . "' " .
                        "LIMIT 1"))
        {
            //save settings
            cms_cache("content.$index.step.meta", "$offset;$count;$position", TRUE);
            $this->refresh_extra($index);
            return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function step_undo_depth($index)
    {
        if (! $this->enabled) return FALSE;

        $array = explode(";", cms_cache("content.$index.step.meta"));
        return isset($array[2]) ? (int)$array[2] : 0;
    }

//--------------------------------------------------------------------------------------------------

    function test_step_redo($index)
    {
        return $this->step_redo($index, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function step_redo($index,
                       $test = FALSE)
    {
        if (! $this->enabled) return FALSE;

        //retrieve settings
        $array    = explode(";", cms_cache("content.$index.step.meta"));
        $offset   = isset($array[0]) ? (int)$array[0] : 0;
        $count    = isset($array[1]) ? (int)$array[1] : 0;
        $position = isset($array[2]) ? (int)$array[2] : 0;

        //validate
        if ($position >= $count)          return FALSE;
        if (! $this->test_update($index)) return FALSE;
        if ($test)                        return TRUE;

        //increase storage position and restore state
        $position++;
        $_position  = $offset + $position;
        $_position %= 10;

        if (mysql_query("UPDATE " . CMS_DB_CONTENT . " " .
                        "SET " .    CMS_DB_CONTENT_WRITER_TIME .
                        "='" .      sqlesc(time()) . "', " .
                                    CMS_DB_CONTENT_BUFFER_TEXT .
                        "='" .      sqlesc(cms_cache("content.$index.step.$_position")) . "' " .
                        "WHERE " .  CMS_DB_CONTENT_INDEX .
                        "='" .      sqlesc($index) . "' " .
                        "LIMIT 1"))
        {
            //save settings
            cms_cache("content.$index.step.meta", "$offset;$count;$position", TRUE);
            $this->refresh_extra($index);
            return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function step_redo_depth($index)
    {
        if (! $this->enabled) return FALSE;

        $array    = explode(";", cms_cache("content.$index.step.meta"));
        $count    = isset($array[1]) ? (int)$array[1] : 0;
        $position = isset($array[2]) ? (int)$array[2] : 0;
        return $count - $position;
    }

//--------------------------------------------------------------------------------------------------

    function step_clear($index)
    {
        if (! $this->enabled) return FALSE;

        cms_cache("content.$index.step.meta", "", TRUE);
        for ($i = 0; $i < 10; $i++) cms_cache("content.$index.step.$i", "", TRUE);
        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function action($type,
                    $status,
                    $action)
    {
        $action = $this->action[$type][$status][$action];

        if ($this->writer)
        {
            if ($action & CMS_CONTENT_ROLE_WRITER)                                 return TRUE;

            if ($this->editor)
            {
                if ($action & CMS_CONTENT_ROLE_WRITER_EDITOR)                                 return TRUE;
                if ($this->publisher && ($action & CMS_CONTENT_ROLE_WRITER_EDITOR_PUBLISHER)) return TRUE;
            };

            if ($this->publisher && ($action & CMS_CONTENT_ROLE_WRITER_PUBLISHER)) return TRUE;
        };

        if ($this->editor)
        {
            if ($action & CMS_CONTENT_ROLE_EDITOR)                                 return TRUE;
            if ($this->publisher && ($action & CMS_CONTENT_ROLE_EDITOR_PUBLISHER)) return TRUE;
        };

        if ($this->publisher && ($action & CMS_CONTENT_ROLE_PUBLISHER)) return TRUE;

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function refresh_extra($index)
    {
        if (! $this->enabled) return FALSE;

        $system = new system();
        $id     = $system->getval("content", "extra_id");
        if (stre($id)) return NULL; //not enabled

        $_index = sqlesc($index);

        if (
             cms_load("document")
             &&
             ($result = mysql_query("SELECT " . CMS_DB_CONTENT_BUFFER_TEXT . " " .
                                    "FROM " .   CMS_DB_CONTENT . " " .
                                    "WHERE " .  CMS_DB_CONTENT_INDEX . "='$_index' " .
                                    "LIMIT 1"))
             &&
             (mysql_num_rows($result) > 0)
           )
        {
            //extract extra value from content
            $text     = mysql_result($result, 0);
            $document = new document($text);
            $type     = "";
            $value    = "";

            if (isset($document->data[$id]))
            {
                $type  = sqlesc($document->data[$id][CMS_DOCUMENT_TYPE]);
                $value = sqlesc($document->data[$id][CMS_DOCUMENT_VALUE]);
            };

            //save extra value to database
            if (mysql_query("UPDATE " . CMS_DB_CONTENT . " " .
                            "SET " .    CMS_DB_CONTENT_EXTRA_TYPE .  "='$type', " .
                                        CMS_DB_CONTENT_EXTRA_VALUE . "='$value' " .
                            "WHERE " .  CMS_DB_CONTENT_INDEX .       "='$_index' " .
                            "LIMIT 1"))
                return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function test_set_extra($index)
    {
        return $this->set_extra($index, NULL, "value", NULL, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function set_extra($index,
                       $value = NULL,
                       $type  = "value",
                       $color = NULL,
                       $test  = FALSE)
    {
        if (! $this->test_update($index)) return FALSE;
        if ($test)                        return TRUE;

        $_index = sqlesc($index);

        //save extra value to database
        if (mysql_query("UPDATE " . CMS_DB_CONTENT . " " .
                        "SET " .    CMS_DB_CONTENT_EXTRA_TYPE .  "='" . sqlesc($type) . "', " .
                                    CMS_DB_CONTENT_EXTRA_VALUE . "='" . sqlesc($value) . "', " .
                                    CMS_DB_CONTENT_EXTRA_COLOR . "='" . sqlesc($color) . "' " .
                        "WHERE " .  CMS_DB_CONTENT_INDEX .       "='$_index' " .
                        "LIMIT 1"))
            return TRUE;

        return FALSE;
    }
};