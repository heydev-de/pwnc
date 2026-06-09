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
if (! cms_load("content"))      ifc_inactive($ifc_page);
if (! cms_load("content_pool")) ifc_inactive($ifc_page);
if (! cms_load("directory"))    ifc_inactive($ifc_page);
if (! cms_load("document"))     ifc_inactive($ifc_page);
if (! cms_load("flexview"))     ifc_inactive($ifc_page);
if (! cms_load("template"))     ifc_inactive($ifc_page);

//initialize main variables
cms_cache_sync($user, "content." . CMS_USER . ".user", CMS_SUPERUSER);
init($object);
init($directory_object);

//check user
if (

//not superuser
     nstreq($user, CMS_SUPERUSER)
     &&
     (

//target user can not access content module
       (! cms_permission(cms_application(), NULL, NULL, $user))
       ||

//current user can not access user's content
       (! cms_permission($user))
     )
   )

    //fallback
    $user = CMS_SUPERUSER;

//TODO: check if active object may be accessed by current user
//by testing if current user can access owner's content

//instantiate content class
$content = new content($user);
if (! $content->enabled) ifc_inactive($ifc_page);

//permission
$data = new data("#system/permission");
$name = $data->get("user.$user", "name");

ifc_permission(
    [""                                          => CMS_L_ACCESS,
     CMS_CONTENT_PERMISSION_WRITER               => CMS_L_IFC_CONTENT_009,
     CMS_CONTENT_PERMISSION_EDITOR               => CMS_L_IFC_CONTENT_015,
     CMS_CONTENT_PERMISSION_PUBLISHER            => CMS_L_IFC_CONTENT_019,
     $user                                       => "$name " . CMS_L_ACCESS,
     "$user." . CMS_CONTENT_PERMISSION_WRITER    => "$name " . CMS_L_IFC_CONTENT_009,
     "$user." . CMS_CONTENT_PERMISSION_EDITOR    => "$name " . CMS_L_IFC_CONTENT_015,
     "$user." . CMS_CONTENT_PERMISSION_PUBLISHER => "$name " . CMS_L_IFC_CONTENT_019,
     CMS_CONTENT_POOL_PERMISSION_OPERATOR        => CMS_L_IFC_CONTENT_131 . " " . CMS_L_OPERATOR,
     CMS_CONTENT_PERMISSION_OPERATOR             => CMS_L_OPERATOR]);

//object icons and status/type descriptions
//..................................................................................................

$icon   = [CMS_CONTENT_STATUS_DRAFT       => [CMS_CONTENT_TYPE_ORIGINAL  => "content/icon_draft",
                                              CMS_CONTENT_TYPE_DUPLICATE => "content/icon_draft_duplicate",
                                              CMS_CONTENT_TYPE_COPY      => "content/icon_draft_copy"],
           CMS_CONTENT_STATUS_DOCUMENT    => [CMS_CONTENT_TYPE_ORIGINAL  => "content/icon_document",
                                              CMS_CONTENT_TYPE_DUPLICATE => "content/icon_document_duplicate",
                                              CMS_CONTENT_TYPE_COPY      => "content/icon_document_copy"],
           CMS_CONTENT_STATUS_PUBLICATION => [CMS_CONTENT_TYPE_ORIGINAL  => "content/icon_publication"]];
$status = [CMS_CONTENT_STATUS_DRAFT       => CMS_L_IFC_CONTENT_031,
           CMS_CONTENT_STATUS_DOCUMENT    => CMS_L_IFC_CONTENT_032,
           CMS_CONTENT_STATUS_PUBLICATION => CMS_L_IFC_CONTENT_033];
$type   = [CMS_CONTENT_TYPE_ORIGINAL      => CMS_L_IFC_CONTENT_034,
           CMS_CONTENT_TYPE_DUPLICATE     => CMS_L_IFC_CONTENT_035,
           CMS_CONTENT_TYPE_COPY          => CMS_L_IFC_CONTENT_036];

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "select":
//..................................................................................................

    $object = $ifc_param;
    $page   = 0;

    //seek active object in directory
    $data   = new data("#system/directory");
    $url    = "content://$object";
    $length = strlen($url);

    //iterate directory
    $data->move("first");
    while (($key = $data->move("next")) !== NULL)
    {
        //check for linkage
        $_url = $data->get($key, "url");

        if (
             (strncmp($url, $_url, $length) === 0)
             &&
             (strcspn($_url, "?#", $length) === 0)
           )
        {
            //found topmost linkage
            $directory_object = $key;
            break 2;
        };
    };

    //not found
    $directory_object = NULL;
    cms_cache_delete("directory." . CMS_USER . ".object");
    break;

case "directory_select":
//..................................................................................................

    $directory_object = $ifc_param;
    $page             = 0;

    //retrieve active object from directory
    $data = new data("#system/directory");
    $url  = $data->get($directory_object, "url");

    //directory dereferencing
    $list = [$directory_object => TRUE];
    while (strncmp($url, "directory://", 12) === 0)
    {
        //get directory index
        $key = substr($url, 12, strcspn($url, "?#", 12));

        //check for loop
        if (isset($list[$key])) break;
        $list[$key] = TRUE;

        $url = $data->get($key, "url");
    };

    //linked entry
    if (strncmp($url, "content://", 10) === 0)
    {
        //get content index
        $object = substr($url, 10);
        $object = substr($object, 0, strspn($object, "1234567890"));

        //verify object
        $query  = "SELECT " . CMS_DB_CONTENT_OWNER . " " .
                  "FROM " .   CMS_DB_CONTENT . " " .
                  "WHERE " .  CMS_DB_CONTENT_INDEX .
                  "='" .      sqlesc($object) . "' " .
                  "LIMIT 1";
        $result = mysql_query($query);

        //switch to owner if applicable
        if ($resultrow = mysql_fetch_assoc($result))
        {
            $_user = $resultrow[CMS_DB_CONTENT_OWNER];

            if (

            //current user is owner
                 streq($user, $_user)
                 ||
                 (

            //or owner has content access
                   cms_permission(cms_application(), NULL, NULL, $_user)
                   &&

            //and current user can access owner content
                   cms_permission($_user)
                 )
               )
            {
                //switch to owner
                $user = $_user;
                break;
            };
        };
    };

    //not found or no access
    $object = NULL;
    cms_cache_delete("content." . CMS_USER . ".object");
    break;

case "info":
//..................................................................................................

    $result = mysql_query(
        "SELECT * " .
        "FROM " .  CMS_DB_CONTENT . " " .
        "WHERE " . CMS_DB_CONTENT_INDEX .
        "='" .     sqlesc($object) . "' " .
        "LIMIT 1");

    if (! ($result && ($object_data = mysql_fetch_assoc($result)))) ifc_close_external();

    //display
    $ifc = new ifc(NULL, NULL, NULL);

    $title = "<tr><th colspan=\"2\">%s</th></tr>";
    $row   = "<tr><td>%s:</td><td class=\"br\">%s</td></tr>";

    ifc_table_open("varied");

    printf($title, CMS_L_IFC_CONTENT_138);

    //status
    printf($row, CMS_L_IFC_CONTENT_006,
           $status[$object_data[CMS_DB_CONTENT_STATUS]] .
           (($object_data[CMS_DB_CONTENT_WRITER_TIME] !== $object_data[CMS_DB_CONTENT_TIME]) ?
           " " . CMS_L_IFC_CONTENT_084 : ""));

    //type
    printf($row, CMS_L_IFC_CONTENT_007, $type[$object_data[CMS_DB_CONTENT_TYPE]]);

    //title
    printf($row, CMS_L_IFC_CONTENT_027, x(l($object_data[CMS_DB_CONTENT_TITLE])));

    //description
    if (nstre($temp = l($object_data[CMS_DB_CONTENT_DESCRIPTION])))
        printf($row, CMS_L_IFC_CONTENT_008, parse_text($temp));

    //keyword
    if (nstre($temp = l($object_data[CMS_DB_CONTENT_KEYWORD])))
        printf($row, CMS_L_IFC_CONTENT_012, x($temp));

    //image
    if (nstre($image = l($object_data[CMS_DB_CONTENT_IMAGE])))
        printf($row, CMS_L_IMAGE, image(translate_url($image), 75, 75));

    //writer
    $name = $data->get("user." . $object_data[CMS_DB_CONTENT_WRITER], "name");
    printf($row, CMS_L_IFC_CONTENT_009, x(nstre($name) ? $name : $object_data[CMS_DB_CONTENT_WRITER]));

    //writer time
    printf($row, CMS_L_IFC_CONTENT_010, x(friendly_date($object_data[CMS_DB_CONTENT_WRITER_TIME])));

    //comment
    if (nstre($temp = $object_data[CMS_DB_CONTENT_WRITER_COMMENT]))
        printf($row, CMS_L_IFC_CONTENT_017, x($temp));

    //sender
    if ($temp = $object_data[CMS_DB_CONTENT_SENDER_TIME])
    {
        printf($title, CMS_L_IFC_CONTENT_002);

        //sender
        printf($row, CMS_L_IFC_CONTENT_003, x($object_data[CMS_DB_CONTENT_SENDER]));

        //sender time
        printf($row, CMS_L_IFC_CONTENT_004, x(friendly_date($temp)));

        //sender comment
        if (nstre($temp = $object_data[CMS_DB_CONTENT_SENDER_COMMENT]))
            printf($row, CMS_L_IFC_CONTENT_017, x($temp));
    };

    //authorization
    if ($temp = $object_data[CMS_DB_CONTENT_EDITOR_TIME])
    {
        printf($title, CMS_L_IFC_CONTENT_014);

        //editor
        printf($row, CMS_L_IFC_CONTENT_015, x($object_data[CMS_DB_CONTENT_EDITOR]));

        //editor time
        printf($row, CMS_L_IFC_CONTENT_016, x(friendly_date($temp)));

        //editor comment
        if (nstre($temp = $object_data[CMS_DB_CONTENT_EDITOR_COMMENT]))
            printf($row, CMS_L_IFC_CONTENT_017, x($temp));
    };

    //publication
    if ($temp = $object_data[CMS_DB_CONTENT_PUBLISHER_TIME])
    {
        printf($title, CMS_L_IFC_CONTENT_018);

        //publisher
        printf($row, CMS_L_IFC_CONTENT_019, x($object_data[CMS_DB_CONTENT_PUBLISHER]));

        //publisher time
        printf($row, CMS_L_IFC_CONTENT_020, x(friendly_date($temp)));

        //publisher comment
        if (nstre($temp = $object_data[CMS_DB_CONTENT_PUBLISHER_COMMENT]))
            printf($row, CMS_L_IFC_CONTENT_017, x($temp));
    };

    ifc_table_close();

    $ifc->close();

case "_meta":
//..................................................................................................

    //retrieve text of active object
    $result = mysql_query(
        "SELECT " . CMS_DB_CONTENT_BUFFER_TEXT . " " .
        "FROM " .   CMS_DB_CONTENT . " " .
        "WHERE " .  CMS_DB_CONTENT_INDEX .
        "='" .      sqlesc($object) . "' " .
        "LIMIT 1");

    if (
         $result
         &&
         ($resultrow = mysql_fetch_assoc($result))
         &&

    //update active object
         $content->update(
             $object,                                //index
             $ifc_param1,                            //title
             $ifc_param2,                            //description
             $ifc_param3,                            //keyword
             $ifc_param4,                            //image
             $resultrow[CMS_DB_CONTENT_BUFFER_TEXT], //text
             $ifc_param5,                            //comment
             $ifc_param6)                            //template
       )
    {
        if (streq(CMS_IFC_OPTION, "external")) ifc_close_external();

        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_param    = $object;
    $ifc_response = CMS_MSG_ERROR;

case "meta":
//..................................................................................................

    $object = $ifc_param;

    //retrieve meta data of active object
    $result = mysql_query(
        "SELECT " . CMS_DB_CONTENT_WRITER_COMMENT . ", " .
                    CMS_DB_CONTENT_BUFFER_TITLE . ", " .
                    CMS_DB_CONTENT_BUFFER_DESCRIPTION . ", " .
                    CMS_DB_CONTENT_BUFFER_KEYWORD . ", " .
                    CMS_DB_CONTENT_BUFFER_IMAGE . ", " .
                    CMS_DB_CONTENT_BUFFER_TEMPLATE . " " .
        "FROM " .   CMS_DB_CONTENT . " " .
        "WHERE " .  CMS_DB_CONTENT_INDEX .
        "='" .      sqlesc($object) . "' " .
        "LIMIT 1");

    if ($result && ($resultrow = mysql_fetch_assoc($result)))
    {
        //menu
        $menu = streq($ifc_option, "external") ?
            [CMS_L_COMMAND_SAVE . "|content/command_save" => "_meta"] :
            TRUE;

        //display
        $ifc = new ifc(
            $ifc_response,
            $ifc_page,
            $menu,
            ["user"             => $user,
             "object"           => $object,
             "directory_object" => $directory_object],
            "_meta",
            CMS_L_IFC_CONTENT_077);

?><script>

function content_template_preview()
{
    const value = ifc_get("ifc_param6");
    if (value) load_page("<?php
        echo(str_replace("%replace%",
                         "\"+encodeURIComponent(value)+\"",
                         q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "template_preview",
                                    "object"      => "\x1B%replace%"]))));?>");
};

</script><?php

        $buffer = "javascript:load_page('" .
                  str_replace("%replace%",
                              "'+encodeURIComponent(ifc_get('ifc_param4'))+'",
                              q(cms_url(["ifc_page"          => "image",
                                         "ifc_option"        => "external",
                                         "object"            => "\x1B%replace%",
                                         "ifc_select"        => "object",
                                         "ifc_select_action" => "\x1Bjavascript:this.opener.ifc_set('ifc_param4','%return%');"]))) . "');";

        $ifc->set(CMS_L_IFC_CONTENT_027,     "text 50 bl",            $resultrow[CMS_DB_CONTENT_BUFFER_TITLE]);
        $ifc->set(CMS_L_IFC_CONTENT_008,     "texteditor 50x8 bilrt", $resultrow[CMS_DB_CONTENT_BUFFER_DESCRIPTION]);
        $ifc->set(CMS_L_IFC_CONTENT_012,     "text 50 bl",            $resultrow[CMS_DB_CONTENT_BUFFER_KEYWORD]);
        $ifc->set(CMS_L_IFC_CONTENT_142,     "title");
        $ifc->set(CMS_L_IMAGE,               "text 40 l",             $resultrow[CMS_DB_CONTENT_BUFFER_IMAGE]);
        $ifc->set("…",                       "button b",              $buffer);
        $ifc->set(CMS_L_IFC_CONTENT_017,     "textarea 50x8 b",       $resultrow[CMS_DB_CONTENT_WRITER_COMMENT]);
        $ifc->set(CMS_L_IFC_CONTENT_013,     "label");
        $ifc->set(content_template_select(), "select 40",             $resultrow[CMS_DB_CONTENT_BUFFER_TEMPLATE]);
        $ifc->set(CMS_L_COMMAND_SHOW,        "button",                "javascript:content_template_preview();");

        $ifc->close();
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "template_preview":
//..................................................................................................

    template_preview($object);
    exit();

case "edit_range":
//..................................................................................................

    //menu
    $menu = [CMS_L_COMMAND_INSERT . "|content/command_apply" => "javascript:content_edit_range_save();"];

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        $menu,
        ["object" => $object,
         "range"  => $range,
         "type"   => "text",
         "left"   => 0,
         "top"    => 0,
         "id"     => $id],
        NULL,
        CMS_L_COMMAND_EDIT);

    $ifc->set(
        CMS_L_IFC_CONTENT_028,
        "texteditor filrt",
        content_get_range($content, $object, $range, "text"),
        NULL,
        "value");

?><script>

function content_edit_range_save()
{
    if (! this.opener.closed)
    {
        document.forms["ifc"].target = this.opener.name;
        document.forms["ifc"].action = this.opener.document.URL;
        ifc_set("left", this.opener.fx_position_left());
        ifc_set("top",  this.opener.fx_position_top());
        ifc_submit();
    };

    window.close();
};

</script><?php

    $ifc->close();

case "edit_value":
//..................................................................................................

    //menu
    $menu = [CMS_L_COMMAND_INSERT . "|content/command_apply" => "javascript:content_edit_value_save();"];

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        $menu,
        ["object" => $object,
         "range"  => $range,
         "type"   => "value",
         "left"   => 0,
         "top"    => 0,
         "id"     => $id],
        NULL,
        CMS_L_COMMAND_EDIT);

    $ifc->set(
        CMS_L_IFC_CONTENT_136,
        "textarea fl",
        content_get_range($content, $object, $range, "value"),
        NULL,
        "value");

?><script>

function content_edit_value_save()
{
    if (! this.opener.closed)
    {
        document.forms["ifc"].target = this.opener.name;
        document.forms["ifc"].action = this.opener.document.URL;
        ifc_set("left", this.opener.fx_position_left());
        ifc_set("top",  this.opener.fx_position_top());
        ifc_submit();
    };

    window.close();
};

</script><?php

    $ifc->close();

case "edit_plugin":
//..................................................................................................

    //menu
    $menu = [CMS_L_COMMAND_INSERT . "|content/command_apply" => "javascript:content_edit_plugin_save();"];

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        $menu,
        ["object" => $object,
         "range"  => $range,
         "type"   => "plugin",
         "left"   => 0,
         "top"    => 0,
         "id"     => $id],
        NULL,
        CMS_L_COMMAND_EDIT);

    $value = content_get_range($content, $object, $range, "plugin");

    $ifc->set(
        CMS_L_URL,
        "text f",
        stre($value) ? CMS_PROTOCOL . "://" : $value,
        NULL,
        "value");

?><script>

function content_edit_plugin_save()
{
    if (! this.opener.closed)
    {
        document.forms["ifc"].target = this.opener.name;
        document.forms["ifc"].action = this.opener.document.URL;
        ifc_set("left", this.opener.fx_position_left());
        ifc_set("top",  this.opener.fx_position_top());
        if (ifc_get("value") === "<?php echo(CMS_PROTOCOL);?>://") ifc_del("value");
        ifc_submit();
    };

    window.close();
};

</script><?php

    $ifc->close();

case "edit_href":
case "edit_href_select_language":
//..................................................................................................

    init($range);
    init($anchor);
    init($value);   //multi-language url
    init($_value);  //link code
    init($__value); //url
    init($id);

    switch (CMS_IFC_MESSAGE)
    {
    case "edit_href_select_language":

        $language = $ifc_param;
        break;

    default:

        if (nstre($range)) $value = content_get_range($content, $object, $range, "href");
        $language = NULL;
    };

    //menu
    $menu = [CMS_L_COMMAND_INSERT . "|content/command_apply" => "javascript:ifc_return();"];

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        $menu,
        ["object"  => $object,
         "range"   => $range,
         "type"    => "href",
         "value"   => $value,  //multi-language url
         "_value"  => $_value, //link code
         "left"    => 0,
         "top"     => 0,
         "id"      => $id],
        NULL,
        CMS_L_COMMAND_EDIT);

?><script>

function content_edit_href_select(value,
                                  text = "")
{
    const language = "<?php echo(q($language));?>";
    const _default = "<?php echo(q(CMS_PROTOCOL));?>://";
    value = value.replace(/^\s+/, ""); //remove preceding space
    value = value.replace(/\s+$/, ""); //remove trailing space

    while (value && (value !== _default))
    {
        //multi-language url
        ifc_set("value", ifc_language_set(ifc_get("value"), value, language));

        //remove trailing enumeration
        if (value.startsWith("directory://"))
        {
            if (value.length === 12) break; //empty
            text = text.replace(/\(\d+\)$/, "");
        }

        //remove preceding content index
        else if (value.startsWith("content://"))
        {
            if (value.length === 10) break; //empty
            text = text.replace(/^\(\d+\)/, "");
        };

        //clean up text
        text = text.replace(/\[/g, "\\\\["); //quote block start
        text = text.replace(/^\s+/, "");     //preceding space
        text = text.replace(/\s+$/, "");     //trailing space
        text = text.replace(/\s+/g, " ");    //multiple space

        ifc_set("_value", value + " " + text); //link code
        ifc_set("__value", value);             //url
        return;
    };

    //clear
    ifc_set("value", ifc_language_set(ifc_get("value"), "", language));
    ifc_del("_value");
    ifc_set("__value", _default);
};

function content_edit_href_save()
{
    if (! this.opener.closed)
    {
        document.forms["ifc"].target = this.opener.name;
        document.forms["ifc"].action = this.opener.document.URL;
        ifc_set("left", this.opener.fx_position_left());
        ifc_set("top",  this.opener.fx_position_top());
        ifc_submit();
    };

    window.close();
};

</script><?php

    $value = language_get($value, $language, TRUE);
    if ($_value = template_parse_reference($value, TRUE))
    {
        $value = $_value["url"];
        $type  = $_value["scheme"];
        $index = $_value["host"];
        $flag  = stre($_value["query"] . $_value["fragment"]);
    }
    else
    {
        $type  = NULL;
        $index = NULL;
        $flag  = FALSE;
    };

    $ifc->set(CMS_L_IFC_CONTENT_093, "text w", NULL, NULL, "content_edit_href_filter");

    //directory
    $array = directory_get_select();
    array_shift($array);
    $_flag = $flag && streq($type, "directory");

    $ifc->set(CMS_L_IFC_CONTENT_011, "description b");
    echo("<select id=\"content-edit-href-directory\" " .
                 "class=\"fullwidth\" " .
                 "onchange=\"content_edit_href_select(" .
                 "'directory://'+ifc_get(this)," .
                 "this.selectedOptions[0]?.text);\" " .
                 "onfocus=\"this.onchange();\">" .
         "<option></option>");

    foreach ($array AS $key => $_value)
    {
        $name = strabridge($key, 60);
        echo("<option value=\"" . x($_value) . "\"" .
                      (($_flag && streq($_value, $index)) ? " selected>" : ">") .
             x($name) .
             "</option>");
    };

    echo("</select>");

    //publication
    $result = mysql_query(
        "SELECT " .   CMS_DB_CONTENT_INDEX . ", " .
                      CMS_DB_CONTENT_TITLE . " " .
        "FROM " .     CMS_DB_CONTENT . " " .
        "WHERE " .    CMS_DB_CONTENT_STATUS .
        "='" .        CMS_CONTENT_STATUS_PUBLICATION . "' " .
        "ORDER BY " . CMS_DB_CONTENT_INDEX . " DESC");

    if ($result)
    {
        $_flag = $flag && streq($type, "content");
        $ifc->set(CMS_L_IFC_CONTENT_042, "description b");
        echo("<select id=\"content-edit-href-content\" " .
                     "class=\"fullwidth\" " .
                     "onchange=\"content_edit_href_select(" .
                     "'content://'+ifc_get(this)," .
                     "this.selectedOptions[0]?.text);\" " .
                     "onfocus=\"this.onchange();\">" .
             "<option></option>");

        while ($resultrow = mysql_fetch_assoc($result))
        {
            $_value = $resultrow[CMS_DB_CONTENT_INDEX];
            $name   = strabridge("($_value) " . l($resultrow[CMS_DB_CONTENT_TITLE]), 60);
            echo("<option value=\"" . x($_value) . "\"" .
                          (($_flag && streq($_value, $index)) ? " selected>" : ">") .
                 x($name) .
                 "</option>");
        };

        echo("</select>");
    };

    //address
    $ifc->set(CMS_L_URL, "description b");
    echo("<input name=\"__value\" " .
                "type=\"text\" " .
                "value=\"" . (stre($value) ? x(CMS_PROTOCOL) . "://" : x($value)) . "\" " .
                "class=\"fullwidth\" " .
                "onchange=\"content_edit_href_select(this.value);\">");

    //language
    language_selector($language, "javascript:ifc_post('edit_href_select_language','%language%');");

?><script>

fx_event_listen(ifc_object("content_edit_href_filter"), "input", () =>
{
    const value   = ifc_get("content_edit_href_filter").toLowerCase();
    const array  = [ifc_object("content-edit-href-directory"),
                    ifc_object("content-edit-href-content")];

    array.forEach((select) =>
    {
        let option = null;
        let flag   = false;

        for (option of select)
        {
            option.hidden = ! option.text.toLowerCase().includes(value);
            if (! flag && ! option.hidden) flag = option.selected = true;
        };

        if (! flag) select.selectedIndex = -1;
    });

    array[0].dispatchEvent(new Event("change"));
});

</script><?php

    $ifc->close();

case "create":
//..................................................................................................

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        TRUE,
        ["user"             => $user,
         "object"           => $object,
         "directory_object" => $directory_object],
        "_create",
        CMS_L_IFC_CONTENT_049);

?><script>

function content_template_preview()
{
    const value = ifc_get("ifc_param2");
    if (value) load_page("<?php
        echo(str_replace("%replace%",
                         "\"+encodeURIComponent(value)+\"",
                         q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "template_preview",
                                    "object"      => "\x1B%replace%"]))));?>");
};

</script><?php

    $ifc->set(CMS_L_IFC_CONTENT_027,     "text 50 bl", CMS_L_IFC_CONTENT_021);
    $ifc->set(CMS_L_IFC_CONTENT_013,     "label");
    $ifc->set(content_template_select(), "select 40",  cms_cache("template." . CMS_USER . ".page"));
    $ifc->set(CMS_L_COMMAND_SHOW,        "button b",   "javascript:content_template_preview();");
    $ifc->set(CMS_L_IFC_CONTENT_017,     "textarea 50x8");

    $ifc->close();

case "_create":
//..................................................................................................

    if ($_object = $content->create($ifc_param1,
                                    $ifc_param2,
                                    $ifc_param3))
    {
        $object       = $_object;
        $ifc_response = CMS_MSG_DONE;
        cms_cache("template." . CMS_USER . ".page",  $ifc_param2, TRUE);
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "display":
//..................................................................................................

    echo(content_parse($content, $object));
    exit();

case "apply":
//..................................................................................................

    $object = $ifc_param;

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        TRUE,
        ["user"             => $user,
         "object"           => $object,
         "directory_object" => $directory_object],
        "_apply",
        CMS_L_IFC_CONTENT_087);

    //immediate application
    $ifc->set(CMS_L_IFC_CONTENT_109, "radio b", 0, TRUE);

    //scheduled application
    echo("<div class=\"element\">");
    $ifc->set(NULL, "radio",   1);
    $ifc->set(NULL, "datetime");
    echo("</div>");

?><script>

fx_event_listen(ifc_object("ifc_param2"), "change", () =>
{
    ifc_object("ifc_param1", 1).click();
});

</script><?php

    $ifc->close();

case "_apply":
//..................................................................................................

    switch ($ifc_param1)
    {
    case 0: //immediate application

        $ifc_response = $content->apply($object) ?
                        CMS_MSG_DONE : CMS_MSG_ERROR;
        break;

    case 1: //scheduled application

        $time         = strtotime($ifc_param2);
        $ifc_response = $content->apply($object, $time) ?
                        CMS_MSG_DONE : CMS_MSG_ERROR;
        break;
    };

    break;

case "apply_all":
//..................................................................................................

    $flag   = TRUE;
    $result = mysql_query(
        "SELECT " . CMS_DB_CONTENT_INDEX . " " .
        "FROM " .   CMS_DB_CONTENT . " " .
        "WHERE " .  CMS_DB_CONTENT_WRITER_TIME .
        "!=" .      CMS_DB_CONTENT_TIME);

    if ($result)
    {
        while ($resultrow = mysql_fetch_assoc($result))
            $flag &= $content->apply($resultrow[CMS_DB_CONTENT_INDEX]);
    };

    $ifc_response = $flag ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "revert":
//..................................................................................................

    $object       = $ifc_param;
    $ifc_response = $content->revert($object) ?
                    CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "authorize":
//..................................................................................................

    $object    = $ifc_param;
    $result    = mysql_query(
        "SELECT " . CMS_DB_CONTENT_EDITOR_COMMENT . " " .
        "FROM " .   CMS_DB_CONTENT . " " .
        "WHERE " .  CMS_DB_CONTENT_INDEX .
        "='" .      sqlesc($object) . "' " .
        "LIMIT 1");
    if (! ($result && ($resultrow = mysql_fetch_assoc($result))))
    {
        $ifc_response = CMS_MSG_ERROR;
        break;
    };

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        TRUE,
        ["user"             => $user,
         "object"           => $object,
         "directory_object" => $directory_object],
        "_authorize",
        CMS_L_IFC_CONTENT_029);

    $ifc->set(
        CMS_L_IFC_CONTENT_017,
        "textarea 50x8",
        $resultrow[CMS_DB_CONTENT_EDITOR_COMMENT]);

    $ifc->close();

case "_authorize":
//..................................................................................................

    $ifc_response = $content->authorize($object, $ifc_param1) ?
                    CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "derive_draft":
//..................................................................................................

    $object       = $ifc_param;
    $ifc_response = $content->derive_draft($object) ?
                    CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "publish":
//..................................................................................................

    $object = $ifc_param;
    $result = mysql_query(
        "SELECT " . CMS_DB_CONTENT_PUBLISHER_COMMENT . ", " .
                    CMS_DB_CONTENT_TITLE . " " .
        "FROM " .   CMS_DB_CONTENT . " " .
        "WHERE " .  CMS_DB_CONTENT_INDEX .
        "='" .      sqlesc($object) . "' " .
        "LIMIT 1");

    if (! ($result && ($resultrow = mysql_fetch_assoc($result))))
    {
        $ifc_response = CMS_MSG_ERROR;
        break;
    };

    //preset value
    $ifc_param  = $directory_object;
    $ifc_param1 = $resultrow[CMS_DB_CONTENT_TITLE];
    $ifc_param2 = 0;    //immediate publication
    $ifc_param3 = NULL; //scheduled publication
    $ifc_param4 = 0;    //no scheduled withdrawal
    $ifc_param5 = NULL; //scheduled withdrawal
    $ifc_param6 = $resultrow[CMS_DB_CONTENT_PUBLISHER_COMMENT];

case "_publish":
//..................................................................................................

    $data     = new data("#system/directory");
    $flexview = new flexview();
    $flexview->import_data($data);

    //menu
    $menu = [CMS_L_COMMAND_CANCEL . "|content/command_cancel" => FALSE];

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        $menu,
        ["user"             => $user,
         "object"           => $object,
         "directory_object" => $directory_object],
        "__publish",
        CMS_L_IFC_CONTENT_040);

    ifc_table_open();
    echo("<colspan>" .
         "<col style=\"WIDTH:0\">" .
         "<col>" .
         "</colspan>" .

         "<tr>" .
         "<th>" .
         CMS_L_IFC_CONTENT_116 .
         "</th>" .

         "<th>" .
         CMS_L_IFC_CONTENT_041 .
         "</th>" .
         "</tr>" .

    //title
         "<tr>" .
         "<td>");
    $ifc->set(CMS_L_IFC_CONTENT_027, "text 50 bl",    $ifc_param1);

    //publication
    $ifc->set(CMS_L_IFC_CONTENT_040, "title");

    //immediate publication
    $ifc->set(CMS_L_IFC_CONTENT_109, "radio b",       0, $ifc_param2 == 0);

    //scheduled publication
    echo("<div class=\"element\">");
    $ifc->set(NULL,                  "radio",         1, $ifc_param2 == 1);
    $ifc->set(NULL,                  "datetime",      $ifc_param3);
    echo("</div><br>");

    //withdrawal
    $ifc->set(CMS_L_IFC_CONTENT_054, "title");

    //no scheduled withdrawal
    $ifc->set(CMS_L_IFC_CONTENT_117, "new radio b",   0, $ifc_param4 == 0);

    //scheduled withdrawal
    echo("<div class=\"element\">");
    $ifc->set(NULL,                  "radio",         1, $ifc_param4 == 1);
    $ifc->set(NULL,                  "datetime",      $ifc_param5);
    echo("</div><br>");

    //comment
    $ifc->set(CMS_L_IFC_CONTENT_118, "title");
    $ifc->set(CMS_L_IFC_CONTENT_017, "textarea 50x8", $ifc_param6);
    echo("</td>" .

    //directory
         "<td class=\"target\">");
    $flexview->set_encoding_function(__NAMESPACE__ . "\\q");
    $flexview->set_display_function(__NAMESPACE__ . "\\directory_flexview_display_function");
    $flexview->show_target(
        $ifc_param,
        "javascript:ifc_post('_publish','%index%');",
        "javascript:ifc_post('publish_insert','%index%');",
        "javascript:ifc_post('publish_append','%index%');");
    echo("</td>" .
         "</tr>" .

    //no entry
         "<tr>" .
         "<td class=\"varied\" " .
             "style=\"TEXT-ALIGN:right\">");
    $ifc->set(CMS_L_IFC_CONTENT_128, "button", "___publish");
    echo("</td>" .

    //replace
         "<td class=\"varied\" " .
             "style=\"TEXT-ALIGN:right\">");
    if ($ifc_param)
        $ifc->set(CMS_L_IFC_CONTENT_119,
        "button",
        "javascript:ifc_post('publish_replace','" . q($ifc_param) . "');");
    echo("</td>" .
         "</tr>");
    ifc_table_close();

?><script>

fx_event_listen(ifc_object("ifc_param3"), "change", () =>
{
    ifc_object("ifc_param2", 1).click();
});

fx_event_listen(ifc_object("ifc_param5"), "change", () =>
{
    ifc_object("ifc_param4", 1).click();
});

</script><?php

    $ifc->close();

case "__publish":
case "___publish":
case "publish_replace":
case "publish_insert":
case "publish_append":
//..................................................................................................

    //action
    switch (CMS_IFC_MESSAGE)
    {
    case "publish_replace":

        $action = "replace";
        break;

    case "publish_insert":

        $action = "insert";
        break;

    case "publish_append":

        $action = "append";
        break;

    default:

        $action = NULL;
        break;
    };

    //publication
    switch ($ifc_param2)
    {
    case 0: //immediate publication

        $time_publish = 0;
        break;

    case 1: //scheduled publication

        $time_publish = strtotime($ifc_param3);
        break;
    };

    //withdrawal
    switch ($ifc_param4)
    {
    case 0: //no scheduled withdrawal

        $time_withdraw = 0;
        break;

    case 1: //scheduled withdrawal

        $time_withdraw = strtotime($ifc_param5);
        break;
    };

    //publish
    if (streq(CMS_IFC_MESSAGE, "___publish"))
    {
        if ($content->publish($object,        //index
                              $ifc_param6,    //comment
                              $time_publish,  //time_publish
                              $time_withdraw) //time_withdraw
            !== FALSE)
        {
            $ifc_response = CMS_MSG_DONE;
            break;
        };
    }

    //publish and link
    elseif ($content->publish($object,        //index
                              $ifc_param6,    //comment
                              $time_publish,  //time_publish
                              $time_withdraw, //time_withdraw
                              $ifc_param,     //directory_index
                              $action,        //directory_action
                              $ifc_param1)    //directory_title
            !== FALSE)
    {
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "withdraw":
//..................................................................................................

    $object       = $ifc_param;
    $ifc_response = $content->withdraw($object) ?
                    CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "duplicate":
//..................................................................................................

    $object = $ifc_param;

    if ($_object = $content->duplicate($object))
    {
        $object       = $_object;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "copy":
//..................................................................................................

    $object = $ifc_param;

    if ($_object = $content->copy($object))
    {
        $object       = $_object;
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "send":
//..................................................................................................

    $object = $ifc_param;
    $result = mysql_query(
        "SELECT " . CMS_DB_CONTENT_TYPE . ", " .
                    CMS_DB_CONTENT_STATUS . ", " .
                    CMS_DB_CONTENT_WRITER . ", " .
                    CMS_DB_CONTENT_SENDER_COMMENT . " " .
        "FROM " .   CMS_DB_CONTENT . " " .
        "WHERE " .  CMS_DB_CONTENT_INDEX .
        "='" .      sqlesc($object) . "' " .
        "LIMIT 1");

    if (! ($result && ($resultrow = mysql_fetch_assoc($result))))
    {
        $ifc_response = CMS_MSG_ERROR;
        break;
    };

    //get receiver list
    $data      = new data("#system/permission");
    $type      = $resultrow[CMS_DB_CONTENT_TYPE];
    $status    = $resultrow[CMS_DB_CONTENT_STATUS];
    $author    = $resultrow[CMS_DB_CONTENT_WRITER];
    $receiver  = content_get_receiver($content, $type, $status);
    $duplicate = $content->test_duplicate($object);
    $copy      = $content->test_copy($object);
    $array     = [];

    //iterate user list
    $data->move("first");
    while (($key = $data->move("next")) !== NULL)
    {
        if (! permission_is_user($key)) continue; //not a user
        $key = substr($key, 5);
        if (streq($key, $user))         continue; //user equals sender

        //check permission
        $original   = isset($receiver[CMS_CONTENT_TYPE_ORIGINAL][$key]);
        $_duplicate = $duplicate && isset($receiver[CMS_CONTENT_TYPE_DUPLICATE][$key]);
        $_copy      = $copy      && isset($receiver[CMS_CONTENT_TYPE_COPY][$key]);
        if (! ($original || $_duplicate || $_copy)) continue; //no permission

        //add receiver
        $array[$key] =
            [0 => $data->get("user.$key", "name"),
             1 => $original,
             2 => $_duplicate,
             3 => $_copy];
    };

    //no receiver
    if (empty($array))
    {
        $ifc_response = CMS_L_IFC_CONTENT_045;
        break;
    };

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        TRUE,
        ["user"             => $user,
         "object"           => $object,
         "directory_object" => $directory_object],
        "_send",
        CMS_L_IFC_CONTENT_046);

    ifc_table_open();
    echo("<tr>" .

    //receiver
         "<th>" .
         CMS_L_IFC_CONTENT_048 .
         "</th>" .

    //role
         "<th>" .
         CMS_L_IFC_CONTENT_044 .
         "</th>" .

    //original
         "<th>" .
         CMS_L_IFC_CONTENT_034 .
         "</th>" .

    //duplicate
         "<th>" .
         CMS_L_IFC_CONTENT_035 .
         "</th>" .

    //copy
         "<th>" .
         CMS_L_IFC_CONTENT_036 .
         "</th>" .
         "</tr>" .

    //blank
         "<tr>" .
         "<td colspan=\"2\" " .
             "class=\"select\">" .
         "</td>" .
         "<td class=\"select\">");
    $ifc->set(NULL, "radio", NULL, TRUE, "original");
    echo("</td>" .
         "<td colspan=\"2\" " .
             "class=\"select\">" .
         "</td>" .
         "</tr>");

    $i = 0;

    ksort($array, SORT_NATURAL | SORT_FLAG_CASE);

    //interate receiver list
    foreach ($array AS $key => $value)
    {
        //role
        $role = [];
        if (streq($key, $author))                                               $role[] = "<strong>" .
                                                                                          CMS_L_IFC_CONTENT_047 .
                                                                                          "</strong>";
        if (cms_permission(CMS_CONTENT_PERMISSION_WRITER,    TRUE, TRUE, $key)) $role[] = CMS_L_IFC_CONTENT_009;
        if (cms_permission(CMS_CONTENT_PERMISSION_EDITOR,    TRUE, TRUE, $key)) $role[] = CMS_L_IFC_CONTENT_015;
        if (cms_permission(CMS_CONTENT_PERMISSION_PUBLISHER, TRUE, TRUE, $key)) $role[] = CMS_L_IFC_CONTENT_019;
        $role = implode(", ", $role);

        //permitted action
        $original  = $value[1] ? "" : " d";
        $duplicate = $value[2] ? "" : " d";
        $copy      = $value[3] ? "" : " d";

        echo("<tr" . ifc_varied() . ">" .

        //receiver
             "<td>" .
             x($value[0]) .
             "</td>" .

        //role
             "<td>" .
             $role .
             "</td>" .

        //original
             "<td>");
        $ifc->set(NULL, "radio$original",     $key, NULL, "original");
        echo("</td>" .

        //duplicate
             "<td>");
        $ifc->set(NULL, "checkbox$duplicate", $key, NULL, "duplicate[$i]");
        echo("</td>" .

        //copy
             "<td>");
        $ifc->set(NULL, "checkbox$copy",      $key, NULL, "copy[$i]");
        echo("</td>" .
             "</tr>");

        $i++;
    };
    ifc_table_close();

    //comment
    $ifc->set(CMS_L_IFC_CONTENT_017, "textarea 50x8", $resultrow[CMS_DB_CONTENT_SENDER_COMMENT]);

    $ifc->close();

case "_send":
//..................................................................................................

    $error = FALSE;

    //send copy
    if (isset($copy) && is_array($copy))
    {
        foreach ($copy AS $value)
        {
            if (($_object = $content->copy($object)) === FALSE)
            {
                $error |= TRUE;
                continue;
            };

            if (! $content->send($_object, $value, $ifc_param1))
            {
                $content->delete($_object);
                $error |= TRUE;
            };
        };
    };

    //send duplicate
    if (isset($duplicate) && is_array($duplicate))
    {
        foreach ($duplicate AS $value)
        {
            if (($_object = $content->duplicate($object)) === FALSE)
            {
                $error |= TRUE;
                continue;
            };

            if (! $content->send($_object, $value, $ifc_param1))
            {
                $content->delete($_object);
                $error |= TRUE;
            };
        };
    };

    //send original
    if ($original)
    {
        if ($content->send($object, $original, $ifc_param1))
            $object  = NULL;
        else
            $error  |= TRUE;
    };

    $ifc_response = $error ? CMS_MSG_ERROR : CMS_MSG_DONE;
    break;

case "delete":
//..................................................................................................

    if (empty($list)) break; //no selection

    $flag = TRUE;

    foreach ($list AS $value) $flag &= $content->delete($value);

    $ifc_response = $flag ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "clear_cache":
//..................................................................................................

    if (

    //verify permission
         $content->publisher
         &&

    //delete cache
         cms_cache_clean(CMS_DATA_PATH . "#content/cache/", TRUE)
       )
    {
        $ifc_response = CMS_MSG_DONE;
        break;
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "version":
//..................................................................................................

    $object = $ifc_param;
    $result = mysql_query(
        "SELECT " .   CMS_DB_CONTENT_VERSION_INDEX . ", " .
                      CMS_DB_CONTENT_VERSION_TIME . " " .
        "FROM " .     CMS_DB_CONTENT_VERSION . " " .
        "WHERE " .    CMS_DB_CONTENT_VERSION_CONTENT .
        "='" .        sqlesc($object) . "' " .
        "ORDER BY " . CMS_DB_CONTENT_VERSION_TIME . " DESC");

    if (! ($result && ($resultrow = mysql_fetch_assoc($result)))) break; //no versions

    $version = $resultrow[CMS_DB_CONTENT_VERSION_INDEX];

    //menu
    $menu = [];
    if ($content->test_update($object))
        $menu[CMS_L_IFC_CONTENT_086 . "|content/command_restore"] = "version_retrieve";
    $menu[CMS_L_COMMAND_CLOSE . "|content/command_cancel"] = NULL;

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        $menu,
        ["user"             => $user,
         "object"           => $object,
         "directory_object" => $directory_object],
        NULL,
        CMS_L_IFC_CONTENT_085);

?><script>

function content_version_select(value)
{
    document.getElementById("content-display").src = "<?php
        echo(str_replace("%replace%",
                         "\"+encodeURIComponent(value)+\"",
                         q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "version_display",
                                    "object"      => $object,
                                    "version"     => "\x1B%replace%"]))));?>";
};

</script><?php

    ifc_table_open();
    echo("<colgroup>" .
         "<col style=\"WIDTH:0\">" .
         "<col>" .
         "</colgroup>" .

         "<tr>" .
         "<td>");

    //list
    $ifc->set(CMS_L_IFC_CONTENT_089, "description b");
    echo("<div id=\"version\" " .
              "data-size=\"14\" " .
              "class=\"select list\" " .
              "data-onchange=\"content_version_select(this.value);\">");
    $i    = mysql_num_rows($result);
    $flag = TRUE;
    do
    {
        $name = friendly_date($resultrow[CMS_DB_CONTENT_VERSION_TIME]) . " – " .
                sprintf(CMS_L_IFC_CONTENT_135, $i--);
        echo("<label>" .
             "<input name=\"version\" " .
                    "type=\"radio\" " .
                    "value=\"" . x($resultrow[CMS_DB_CONTENT_VERSION_INDEX]) . "\"" .
                    ($flag ? " checked>" : ">") .
             x($name) .
             "</label>");
        $flag = FALSE;
    }
    while ($resultrow = mysql_fetch_assoc($result));
    echo("</div>" .
         jscript("ifc_custom_select(\"version\");") .
         "</td>" .

    //display version
         "<td class=\"iframe\">" .
         "<iframe id=\"content-display\" " .
                 "src=\"" . x(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                       "ifc_message" => "version_display",
                                       "object"      => $object,
                                       "version"     => $version])) . "\" " .
                 "sandbox=\"allow-same-origin\">" .
         CMS_L_IFC_CONTENT_090 .
         "</iframe>" .
         "</td>" .
         "</tr>");
    ifc_table_close();

    $ifc->close();

case "version_display":
//..................................................................................................

    $result = mysql_query(
        "SELECT " . CMS_DB_CONTENT_VERSION_TITLE . ", " .
                    CMS_DB_CONTENT_VERSION_DESCRIPTION . ", " .
                    CMS_DB_CONTENT_VERSION_KEYWORD . ", " .
                    CMS_DB_CONTENT_VERSION_TEXT . ", " .
                    CMS_DB_CONTENT_VERSION_TEMPLATE . " " .
        "FROM " .   CMS_DB_CONTENT_VERSION . " " .
        "WHERE " .  CMS_DB_CONTENT_VERSION_INDEX .
        "='" .      sqlesc($version) . "' " .
        "LIMIT 1");

    if ($result && ($resultrow = mysql_fetch_assoc($result)))
    {
        $document = new document($resultrow[CMS_DB_CONTENT_VERSION_TEXT]);
        $template = new template();

        define("CMS_CONTENT_INDEX", $object);

        echo($template->parse(
            $document,
            $resultrow[CMS_DB_CONTENT_VERSION_TEMPLATE],
            $resultrow[CMS_DB_CONTENT_VERSION_TITLE],
            $resultrow[CMS_DB_CONTENT_VERSION_DESCRIPTION],
            $resultrow[CMS_DB_CONTENT_VERSION_KEYWORD],
            ["edit" => ["lock" => TRUE]]));
        exit();
    };

    header("Location: " . CMS_URL . "blank.htm", TRUE, 302);
    exit();

case "version_store":
//..................................................................................................

    $object       = $ifc_param;
    $ifc_response = $content->version_store($object) ?
                    CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "version_retrieve":
//..................................................................................................

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        TRUE,
        ["user"             => $user,
         "object"           => $object,
         "directory_object" => $directory_object],
        "_version_retrieve",
        CMS_L_IFC_CONTENT_086);

    //immediate application
    $ifc->set(CMS_L_IFC_CONTENT_109, "radio b", 0, TRUE);

    //scheduled application
    echo("<div class=\"element\">");
    $ifc->set(NULL,                  "radio",   1);
    $ifc->set(NULL,                  "datetime");
    echo("</div>");

?><script>

fx_event_listen(ifc_object("ifc_param2"), "change", () =>
{
    ifc_object("ifc_param1", 1).click();
});

</script><?php

    $ifc->close();

case "_version_retrieve":
//..................................................................................................

    switch ($ifc_param1)
    {
    case 0: //immediate application

        $ifc_response = $content->version_retrieve($version) ?
                        CMS_MSG_DONE : CMS_MSG_ERROR;
        break;

    case 1: //scheduled application

        $time         = strtotime($ifc_param2);
        $ifc_response = $content->version_retrieve($version, $time) ?
                        CMS_MSG_DONE : CMS_MSG_ERROR;
        break;
    };

    break;

case "schedule":
case "schedule_save":
case "schedule_delete":
//..................................................................................................

    switch (CMS_IFC_MESSAGE)
    {
    case "schedule":
//..................................................................................................

        $object = $ifc_param;
        break;

    case "schedule_save":
//..................................................................................................

        if (empty($time)) break; //no selection

        $flag = TRUE;

        foreach ($time AS $key => $value)
        {
            $value  = strtotime($value);
            $flag  &= mysql_query(
                "UPDATE " . CMS_DB_CONTENT_SCHEDULE . " " .
                "SET " .    CMS_DB_CONTENT_SCHEDULE_TIME .
                "='" .      sqlesc($value) . "' " .
                "WHERE " .  CMS_DB_CONTENT_SCHEDULE_HASH .
                "='" .      sqlesc($key) . "' " .
                "LIMIT 1");
        };

        $ifc_response = $flag ? CMS_MSG_DONE : CMS_MSG_ERROR;
        break;

    case "schedule_delete":
//..................................................................................................

        if (empty($list)) break; //no selection

        $flag = TRUE;

        foreach ($list AS $value) $flag &= $content->_schedule_delete($value);

        $ifc_response = $flag ? CMS_MSG_DONE : CMS_MSG_ERROR;
        break;
    };

    //retrieve versions
    $result = mysql_query(
        "SELECT " .   CMS_DB_CONTENT_SCHEDULE_TIME . ", " .
                      CMS_DB_CONTENT_SCHEDULE_TYPE . ", " .
                      CMS_DB_CONTENT_SCHEDULE_CONTENT . ", " .
                      CMS_DB_CONTENT_SCHEDULE_VALUE_1 . ", " .
                      CMS_DB_CONTENT_SCHEDULE_VALUE_2 . ", " .
                      CMS_DB_CONTENT_SCHEDULE_HASH . " " .
        "FROM " .     CMS_DB_CONTENT_SCHEDULE . " " .
        "WHERE " .    CMS_DB_CONTENT_SCHEDULE_CONTENT .
        "='" .        sqlesc($object) . "' " .
        "ORDER BY " . CMS_DB_CONTENT_SCHEDULE_TIME);

    if (! ($result && mysql_num_rows($result))) break; //no versions

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        [CMS_L_COMMAND_SAVE .            "|content/command_save"   => "schedule_save",
         CMS_L_COMMAND_DELETE_SELECTED . "|content/command_delete" => "#schedule_delete",
         CMS_L_COMMAND_CLOSE .           "|content/command_cancel" => NULL],
        ["user"             => $user,
         "object"           => $object,
         "directory_object" => $directory_object],
        NULL,
        CMS_L_IFC_CONTENT_120);

    ifc_table_open();
    echo("<colgroup>" .
         "<col style=\"WIDTH:0\">" .
         "<col span=\"4\">" .
         "</colgroup>" .

    //header
         "<tr>" .

    //spacer
         "<td class=\"select\">" .
         "</td>" .

    //time
         "<th>" .
         CMS_L_IFC_CONTENT_001 .
         "</th>" .

    //type
         "<th>" .
         CMS_L_IFC_CONTENT_005 .
         "</th>" .

    //value
         "<th>" .
         CMS_L_IFC_CONTENT_023 .
         "</th>" .
         "</tr>");

    $directory = new directory();

    //go through schedule
    while ($resultrow = mysql_fetch_assoc($result))
    {
        //time
        $time = (int)$resultrow[CMS_DB_CONTENT_SCHEDULE_TIME];
        $type = $resultrow[CMS_DB_CONTENT_SCHEDULE_TYPE];
        $hash = $resultrow[CMS_DB_CONTENT_SCHEDULE_HASH];

        echo("<tr" . ifc_varied() . ">" .

        //selection
             "<td class=\"select\">");
        $ifc->set(NULL, "checkbox", $hash, FALSE, "list[]");
        echo("</td>" .

        //time
             "<td>");
        $ifc->set(friendly_date($time), "datetime", $time, NULL, "time[$hash]");
        echo("</td>");

        //type
        switch ($type)
        {
        case CMS_CONTENT_SCHEDULE_TYPE_APPLY:

            echo("<td>" .
                 image("content/icon_apply") . " " .
                 CMS_L_IFC_CONTENT_087 .
                 "</td>");
            break;

        case CMS_CONTENT_SCHEDULE_TYPE_RETRIEVE:

            echo("<td>" .
                 image("content/icon_restore") . " " .
                 CMS_L_IFC_CONTENT_122 .
                 "</td>");
            break;

        case CMS_CONTENT_SCHEDULE_TYPE_PUBLISH:

            echo("<td class=\"green\">" .
                 image("content/icon_publish") . " " .
                 CMS_L_IFC_CONTENT_040 .
                 "</td>");
            break;

        case CMS_CONTENT_SCHEDULE_TYPE_WITHDRAW:

            echo("<td class=\"red\">" .
                 image("content/icon_withdraw") . " " .
                 CMS_L_IFC_CONTENT_054 .
                 "</td>");
            break;
        };

        //value
        echo("<td>");

        switch ($type)
        {
        case CMS_CONTENT_SCHEDULE_TYPE_APPLY:
//..................................................................................................

            break;

        case CMS_CONTENT_SCHEDULE_TYPE_RETRIEVE:
//..................................................................................................

            $value = $resultrow[CMS_DB_CONTENT_SCHEDULE_VALUE_1];

            if (
                 ($_result = mysql_query("SELECT " . CMS_DB_CONTENT_VERSION_TIME . ", " .
                                                     CMS_DB_CONTENT_VERSION_TITLE . " " .
                                         "FROM " .   CMS_DB_CONTENT_VERSION . " " .
                                         "WHERE " .  CMS_DB_CONTENT_VERSION_INDEX .
                                         "='" .      sqlesc($value) . "' " .
                                         "LIMIT 1"))
                 &&
                 ($_resultrow = mysql_fetch_assoc($_result))
               )
                echo("<small>" .
                     CMS_L_IFC_CONTENT_125 .
                     "</small><br>" .
                     "<a href=\"javascript:load_page('" .
                         q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "version_display",
                                    "version"     => $value])) . "');\">" .
                     x(friendly_date($_resultrow[CMS_DB_CONTENT_VERSION_TIME]) . " – " .
                       l($_resultrow[CMS_DB_CONTENT_VERSION_TITLE])) .
                     "</a>");
            else
                echo(CMS_L_IFC_CONTENT_126);

            break;

        case CMS_CONTENT_SCHEDULE_TYPE_PUBLISH:
        case CMS_CONTENT_SCHEDULE_TYPE_WITHDRAW:
//..................................................................................................

            $key   = $resultrow[CMS_DB_CONTENT_SCHEDULE_VALUE_1];
            $value = l($resultrow[CMS_DB_CONTENT_SCHEDULE_VALUE_2]);

            if (l($directory->data->get($key, "name")))
            {
                while ($key = $directory->data->move("parent", $key))
                    $value = l($directory->data->get($key, "name")) . "/$value";

                echo("<small>" .
                     CMS_L_IFC_CONTENT_123 .
                     "</small><br>" .
                     x($value));
            }
            else
            {
                echo(CMS_L_IFC_CONTENT_124);
            };

            break;
        };

        echo("</td>" .
             "</tr>");
    };

    //selection control
    echo("<tr>" .
         "<td colspan=\"4\" " .
             "class=\"select\" " .
             "style=\"TEXT-ALIGN:right\">");
    $ifc->set(CMS_L_ALL,    "button", "javascript:ifc_list_activate();");
    $ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert();");
    $ifc->set(CMS_L_NONE,   "button", "javascript:ifc_list_deactivate();");
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    $ifc->close();

case "pool":
case "pool_select":
case "pool_select_language":
case "pool_display":
case "pool_buffer":
case "pool_add":
case "_pool_add":
case "pool_edit":
case "_pool_edit":
case "pool_source":
case "pool_delete":
case "pool_category_rename":
case "_pool_category_rename":
case "pool_synchronize_all":
//..................................................................................................

    init($pool_object);
    init($pool_type);
    init($pool_language);

    $pool = new content_pool();

    switch (CMS_IFC_MESSAGE)
    {
    case "pool_select":
//..................................................................................................

        $pool_object = language_set($pool_object, $ifc_param, $pool_language);
        break;

    case "pool_select_language":
//..................................................................................................

        $pool_language = $ifc_param;
        break;

    case "pool_display":
//..................................................................................................

        if ($type = $pool->data->get($pool_object, "type"))
        {
            //assign data to preview base
            $document = new document($pool->get_text($pool_object));
            $array    = [];

            foreach ($document->data AS $key => $value)
            {
                if ($key === "")                         $key = "preview";
                elseif (streq(substr($key, 0, 2), "..")) $key = substr($key, 2);
                else                                     $key = "preview.$key";

                $array[$key] = $value;
            };

            $document->data = $array;

            //fallback
            $template = new template();
            if (! isset($template->tlist[$type])) $type = "text";

            //generate preview template
            $ttype = $template->tlist[$type];
            $code  = ($ttype & CMS_TEMPLATE_TYPE_SPAN) ?
                     "<CMS:$type id=\"preview\">" .
                     $template->tname[$ttype] .
                     "</CMS:$type>" :
                     "<CMS:$type id=\"preview\"/>";

            template_preview($code, FALSE, $document);
        };

        $category = $pool->data->get($pool_object, "category");
        cms_cache("content_pool." . CMS_USER . ".category", $category, TRUE);
        exit();

    case "pool_buffer":
//..................................................................................................

        $_pool_object = language_get($pool_object, $pool_language, TRUE);
        $buffer       = $pool->get_text($_pool_object);

        //write to editing buffer of user
        cms_cache("content." . CMS_USER . ".buffer", $buffer, TRUE);

        $ifc_response = CMS_MSG_DONE;
        break;

    case "pool_add":
//..................................................................................................

        $array    = content_pool_get_select();
        $category = (string)cms_cache("content_pool." . CMS_USER . ".category");
        if (! in_array($category, $array)) $category = "";

        //display
        $ifc = new ifc(
            $ifc_response,
            $ifc_page,
            [CMS_L_COMMAND_CONFIRM . "|content/command_apply"  => "_pool_add",
             CMS_L_COMMAND_CANCEL .  "|content/command_cancel" => "pool"],
            ["content_index" => $content_index,
             "content_range" => $content_range,
             "pool_type"     => $pool_type],
             "_pool_add",
            CMS_L_COMMAND_ADD);

        //name
        $ifc->set(CMS_L_NAME,             "text 40 b", "($content_index) $content_range");

        //category
        $ifc->set(CMS_L_IFC_TEMPLATE_005, "label");
        $ifc->set($array,                 "list 40",   $category);

        $ifc->close();

    case "_pool_add":
//..................................................................................................

        if ($_pool_object = $pool->add($ifc_param1,    //name
                                       $ifc_param2,    //category
                                       $content_index, //content_index
                                       $content_range, //range
                                       $pool_type))    //type
        {
            $pool_object  = language_set($pool_object, $_pool_object, $pool_language);
            $ifc_response = CMS_MSG_DONE;
            break;
        };

        $ifc_response = CMS_MSG_ERROR;
        break;

    case "pool_edit":
//..................................................................................................

        $_pool_object = language_get($pool_object, $pool_language, TRUE);
        if (stre($_pool_object)) break; //no selection

        //display
        $ifc = new ifc(
            $ifc_response,
            $ifc_page,
            [CMS_L_COMMAND_CONFIRM . "|content/command_apply"  => "_pool_edit",
             CMS_L_COMMAND_CANCEL .  "|content/command_cancel" => "pool"],
            ["pool_object"   => $pool_object,
             "pool_language" => $pool_language],
            "_pool_edit",
            CMS_L_COMMAND_EDIT);

        //name
        $ifc->set(CMS_L_NAME,                "text 40 b", $pool->data->get($_pool_object, "name"));

        //category
        $ifc->set(CMS_L_IFC_TEMPLATE_005,    "label");
        $ifc->set(content_pool_get_select(), "list 40",   $pool->data->get($_pool_object, "category"));

        $ifc->close();

    case "_pool_edit":
//..................................................................................................

        $_pool_object = language_get($pool_object, $pool_language, TRUE);
        $ifc_response = $pool->set($_pool_object, $ifc_param1, $ifc_param2) ?
                        CMS_MSG_DONE : CMS_MSG_ERROR;
        break;

    case "pool_source":
//..................................................................................................

        if (blank($object)) ifc_close_external();

        $index = $pool->data->get($object, "index");
        $range = $pool->data->get($object, "range");

        header("Location: " . cms_url(CMS_MODULES_URL . "content.php",
                                      ["content_index"  => $index,
                                       "content_option" => CMS_TEMPLATE_OPTION_ALL]) .
               "#tp-dd-" . r(crc32_base62($range)),
               TRUE, 302);
        exit();

    case "pool_delete":
//..................................................................................................

        if (blank($_pool_object)) break; //no selection

        $value    = reset($_pool_object);
        $category = $pool->data->get($value, "category");
        $flag     = TRUE;

        //delete selected references
        foreach ($_pool_object AS $value)
            $flag &= ! $pool->delete($value);

        //remove invalid references from selection
        $array = explode(",", CMS_LANGUAGE_ENABLED);
        if (nstre(CMS_LANGUAGE_ENABLED)) $array[] = FALSE;

        foreach ($array AS $value)
        {
            $_pool_object = language_get($pool_object, $value, TRUE);

            if (! $pool->data->get($_pool_object))
            {
                //active language
                if (streq($value, $pool_language))
                {
                    $pool->data->move("first");
                    $_pool_object = $pool->data->seek(["category" => $category]);
                    $pool_object  = language_set($pool_object, $_pool_object, $value);
                }

                //inactive language(s)
                else
                {
                    $pool_object = language_set($pool_object, NULL, $value);
                };
            };
        };

        $ifc_response = $flag ? CMS_MSG_ERROR : CMS_MSG_DONE;
        break;

    case "pool_category_rename":
//..................................................................................................

        $_pool_object = language_get($pool_object, $pool_language, TRUE);
        $category     = $pool->data->get($_pool_object, "category");

        //display
        $ifc = new ifc(
            $ifc_response,
            $ifc_page,
            [CMS_L_COMMAND_CONFIRM . "|content/command_apply"  => "_pool_category_rename",
             CMS_L_COMMAND_CANCEL .  "|content/command_cancel" => "pool"],
            ["pool_object"   => $pool_object,
             "pool_language" => $pool_language],
            "_pool_category_rename",
            CMS_L_IFC_CONTENT_063);

        //category
        $ifc->set(CMS_L_NAME, "text 40 b", $category);

        $ifc->close();

    case "_pool_category_rename":
//..................................................................................................

        $_pool_object = language_get($pool_object, $pool_language, TRUE);
        $category     = $pool->data->get($_pool_object, "category");

        $pool->data->move("first");
        while ($key = $pool->data->move("next"))
        {
            $_category = $pool->data->get($key, "category");
            if (streq($_category, $category))
                $pool->data->set($ifc_param1, $key, "category");
        };

        $ifc_response = $pool->data->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
        break;

    case "pool_synchronize_all":
//..................................................................................................

        $ifc_response = $pool->synchronize_all() ? CMS_MSG_DONE : CMS_MSG_ERROR;
        break;
    };

    $_pool_object = language_get($pool_object, $pool_language, TRUE);
    $array        = content_pool_get_array(CMS_IFC_SELECT ? $pool_type : NULL);

    //get category from object
    if (nstre($_pool_object))
    {
        $category = (string)$pool->data->get($_pool_object, "category");
    }

    //get object from category
    else
    {
        $category = (string)cms_cache("content_pool." . CMS_USER . ".category");
        if (! isset($array[$category])) $category = "";

        if (is_array($array[$category]))
        {
            $_pool_object = current($array[$category]);
            $pool_object  = language_set($pool_object, $_pool_object, $pool_language);
        };
    };

    //menu
    $menu = [];
    if (nstre($_pool_object))
    {
        if (CMS_IFC_SELECT)
            $menu[CMS_L_COMMAND_INSERT . "|content/command_apply"]    = "javascript:ifc_return();";
        $menu[CMS_L_IFC_CONTENT_133 . "|content/command_restore"]     = "pool_buffer";
        if ($pool->operator)
            $menu[CMS_L_COMMAND_EDIT .    "|content/command_edit"]    = "pool_edit";
        $menu[CMS_L_IFC_CONTENT_025 . "|content/command_source"]      =
            "javascript:load_page('" .
            str_replace("%replace%",
                        "'+ifc_get('_pool_object')+'",
                        q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                   "ifc_message" => "pool_source",
                                   "object"      => "\x1B%replace%"]))) . "');";
        if ($pool->operator)
        {
            $menu[CMS_L_COMMAND_DELETE .  "|content/command_delete"]  = "#pool_delete";
            $menu[CMS_L_IFC_CONTENT_063 . "|content/command_rename"]  = "pool_category_rename";
        };
    };
    if ($pool->operator)
        $menu[CMS_L_IFC_CONTENT_132 . "|content/command_synchronize"] = "pool_synchronize_all";
    if (nstreq(CMS_IFC_OPTION, "external"))
        $menu[CMS_L_COMMAND_CLOSE . "|content/command_cancel"]        = NULL;

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        $menu,
        ["pool_object"   => $pool_object,
         "pool_type"     => $pool_type,
         "pool_language" => $pool_language]);

?><script>

function content_pool_select(value)
{
    const language = "<?php echo(q($pool_language));?>";
    if (ifc_language_get(ifc_get("pool_object"), language) === value) return;
    ifc_set("pool_object", ifc_language_set(ifc_get("pool_object"), value, language));
    document.getElementById("content-pool-display").src = "<?php
        echo(str_replace("%replace%",
                         "\"+encodeURIComponent(value)+\"",
                         q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "pool_display",
                                    "pool_object" => "\x1B%replace%"]))));?>";
};

</script><?php

    ifc_table_open();
    echo("<colgroup>" .
         "<col style=\"WIDTH:0\">" .
         "<col>" .
         "</colgroup>" .

         "<tr>" .
         "<td>");

    //category
    $ifc->set(CMS_L_IFC_CONTENT_129, "description b");
    echo("<select class=\"list\" " .
                 "onchange=\"ifc_post('pool_select',this.value);\">");
    foreach ($array AS $key => $value)
    {
        $name  = strabridge($key, 40);
        $value = is_array($value) ? current($value) : NULL;
        echo("<option value=\"" . x($value) . "\"" .
                      (streq($key, $category) ? " selected>" : ">") .
             x($name) .
             "</option>");
    };
    echo("</select>");

    //list
    if (is_array($array[$category]))
    {
        $template = new template();

        echo("<br>");

        $ifc->set(CMS_L_IFC_CONTENT_130, "description b");
        echo("<div id=\"_pool_object\" " .
                  "data-size=\"14\" " .
                  "class=\"select list\" " .
                  "data-onchange=\"content_pool_select(this.value);\"" .
                  "ondblclick=\"ifc_return();\">");
        foreach ($array[$category] AS $key => $value)
        {
            //type
            $type  = (string)$pool->data->get($value, "type");
            $type  = isset($template->tlist[$type]) ?
                     $template->tlist[$type] : CMS_TEMPLATE_TYPE_NONE;
            $type  = $template->tname[$type];

            //name
            echo("<label title=\"" . x($type) . "\">" .
                 "<input name=\"_pool_object[]\" " .
                        "type=\"checkbox\" " .
                        "value=\"" . x($value) . "\"" .
                        (streq($value, $_pool_object) ? " checked>" : ">") .
                 x($key) .
                 "</label>");
        };
        echo("</div>" .
             "<div style=\"TEXT-ALIGN:right\">");
        $ifc->set(CMS_L_ALL,    "button", "javascript:ifc_list_activate(\"_pool_object\");");
        $ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert(\"_pool_object\");");
        $ifc->set(CMS_L_NONE,   "button", "javascript:ifc_list_deactivate(\"_pool_object\");");
        echo("</div>" .
             jscript("ifc_custom_select(\"_pool_object\");"));
    };

    //language
    if (CMS_IFC_SELECT)
        language_selector($pool_language, "javascript:ifc_post('pool_select_language','%language%');");

    echo("</td>" .

    //display reference
         "<td class=\"iframe\">" .
         "<iframe id=\"content-pool-display\" " .
                 "src=\"" . x(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                       "ifc_message" => "pool_display",
                                       "pool_object" => $_pool_object])) . "\" " .
                 "sandbox=\"allow-same-origin\">" .
         CMS_L_IFC_CONTENT_090 .
         "</iframe>" .
         "</td>" .
         "</tr>");
    ifc_table_close();

    $ifc->close();

case "rss":
case "rss_select":
case "rss_add":
case "rss_save":
case "rss_delete":
case "rss_default":
case "rss_assign":
case "_rss_assign":
//..................................................................................................

    if (! $content->publisher) break; //user must be publisher

    if (! cms_load("rss"))
    {
        $ifc_response = CMS_MSG_ERROR;
        break;
    };

    init($rss_object);

    $rss = new rss();

    switch (CMS_IFC_MESSAGE)
    {
    case "rss_select":
//..................................................................................................

        $rss_object = $ifc_param;
        break;

    case "rss_add":
//..................................................................................................

        $_rss_object = $rss->add_channel(
            CMS_L_IFC_CONTENT_043,                      //name
            sprintf(CMS_L_IFC_CONTENT_058, CMS_DOMAIN), //description
            CMS_ROOT_URL);                              //link

        //create template
        $template           = new template();
        $template->operator = TRUE;
        $template_index     = $template->add(
            CMS_L_IFC_CONTENT_043, //name
            NULL,                  //category
            NULL,                  //page
            "<a href=\"" . x(u(CMS_MODULES_URL . "rss.php",
                               ["rss_channel" => $_rss_object,
                                "rss_limit"   => 25,
                                "rss_order"   => "published"])) . "\">\n" .
            image("rss/icon_rss", NULL, NULL, CMS_L_IFC_CONTENT_078) . "\n" .
            CMS_L_IFC_CONTENT_079 . "\n" .
            "</a>");

        if ($template_index !== FALSE)
            $rss->data->set($template_index, $_rss_object, "template");

        if ($rss->save())
        {
            $rss_object   = $_rss_object;
            $ifc_response = CMS_MSG_DONE;
            break;
        };

        $ifc_response = CMS_MSG_ERROR;
        break;

    case "rss_save":
//..................................................................................................

        if (empty($ifc_param1)) $ifc_param1 = CMS_L_IFC_CONTENT_043;
        if (empty($ifc_param3)) $ifc_param3 = sprintf(CMS_L_IFC_CONTENT_058, CMS_DOMAIN);
        if (empty($ifc_param4)) $ifc_param4 = CMS_ROOT_URL;

        $rss->set_channel(
            $rss_object,         //index
            $ifc_param1,         //name
            $ifc_param3,         //description
            $ifc_param4,         //link
            $ifc_param5,         //image
            $ifc_param6,         //category
            isset($ifc_param2)); //default

        //update template
        if ($template_index = $rss->data->get($rss_object, "template"))
        {
            $template           = new template();
            $template->operator = TRUE;

            if ($array = $template->get($template_index))
                $template->set(
                    $template_index,                                        //index
                    language_get($ifc_param1, FALSE),                       //name
                    isset($array["category"]) ? $array["category"] : NULL,  //category
                    isset($array["page"])     ? $array["page"]     : NULL); //page
        };

        $ifc_response = $rss->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
        break;

    case "rss_delete":
//..................................................................................................

        if (empty($_list)) break; //no selection

        $array = [];

        foreach ($_list AS $value)
        {
            if ($_value = $rss->data->get($value, "template")) $array[] = $_value;

            $rss->del_channel($value);

            //remove deleted object from content
            $value = sqlesc("/$value/");
            mysql_query(
                "UPDATE " . CMS_DB_CONTENT . " " .
                "SET " .    CMS_DB_CONTENT_CHANNEL .
                "=" .      "REPLACE(" . CMS_DB_CONTENT_CHANNEL . ", '$value', '') " .
                "WHERE " .  CMS_DB_CONTENT_CHANNEL . " LIKE '%$value%'");
        };

        if ($rss->save())
        {
            //delete templates
            $template           = new template();
            $template->operator = TRUE;

            foreach ($array AS $value) $template->delete($value);

            //deselect object if deleted
            if (in_array($rss_object, $_list)) $rss_object = 0;

            $ifc_response = CMS_MSG_DONE;
            break;
        };

        $ifc_response = CMS_MSG_ERROR;
        break;

    case "rss_default":
//..................................................................................................

        if (empty($_list)) $_list = [];

        $rss->data->move("first");
        while ($key = $rss->data->move("next"))
            $rss->data->set(in_array($key, $_list), $key, "default");

        $ifc_response = $rss->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
        break;

    case "rss_assign":
//..................................................................................................

        $object  = $ifc_param;
        $flag    = TRUE;
        $channel = NULL;
        $result  = mysql_query(
            "SELECT " . CMS_DB_CONTENT_CHANNEL . " " .
            "FROM " .   CMS_DB_CONTENT . " " .
            "WHERE " .  CMS_DB_CONTENT_INDEX .
            "='" .      sqlesc($object) . "' " .
            "LIMIT 1");

        if ($result && ($resultrow = mysql_fetch_row($result))) $channel = $resultrow[0];
        break;

    case "_rss_assign":
//..................................................................................................

        if (empty($_list)) $channel = NULL;
        else               $channel = "/" . implode("//", $_list). "/";

        if ($content->channel_set($object, $channel))
        {
            $ifc_message = CMS_MSG_DONE;
            break 2;
        };

        $ifc_message = CMS_MSG_ERROR;
        break;

    default:
//..................................................................................................

        $channel = NULL;
        $flag    = NULL;
        break;
    };

    //selected elements
    $array = $flag ? $channel : rss_get_default();
    $array = explode("//", substr($array, 1, -1));
    $array = array_flip($array);

    //menu
    $menu = [CMS_L_COMMAND_ADD .             "|content/command_add"    => "rss_add",
             CMS_L_COMMAND_DELETE_SELECTED . "|content/command_delete" => "#rss_delete"];
    if ($flag) $menu[CMS_L_IFC_CONTENT_075 . "|content/command_save"]  =  "_rss_assign";
    else       $menu[CMS_L_IFC_CONTENT_064 . "|content/command_save"]  =  "rss_default";
    $menu[CMS_L_COMMAND_CLOSE . "|content/command_cancel"]             =  NULL;

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        $menu,
        ["user"             => $user,
         "object"           => $object,
         "directory_object" => $directory_object,
         "rss_object"       => $rss_object,
         "flag"             => $flag,
         "channel"          => $channel],
        NULL,
        CMS_L_IFC_CONTENT_061);

?><script>

function s(index) //select
{
    ifc_post("rss_select", index);
};

function content_select_active()
{
    const list = document.querySelectorAll("INPUT[name=\"_list[]\"]");

    list.forEach(object =>
    {
        switch (object.value)
        {
<?php

foreach ($array AS $key => $_)
    echo("case \"" . q($key) . "\":\n");

?>
            if (! object.checked) object.click();
            break;

        default:

            if (object.checked)   object.click();
            break;
        };
    });
};

</script><?php

    echo("<table class=\"layout\">" .

         "<colgroup>" .
         "<col>");
    if ($rss_object)
        echo("<col style=\"WIDTH:0\">");
    echo("</colgroup>" .

         "<tr>" .
         "<td>");

    //channel list
    ifc_table_open();
    echo("<colgroup>" .
         "<col style=\"WIDTH:0\">" .
         "<col colspan=\"2\">" .
         "</colgroup>" .

    //header
         "<tr>" .

    //dummy
         "<td class=\"select\">" .
         "</td>" .

    //name
         "<th>" .
         CMS_L_IFC_CONTENT_043 .
         "</th>" .

    //url
         "<th>" .
         CMS_L_URL .
         "</th>" .
         "</tr>");

    data_sort($rss->data, "name");

    $rss->data->move("first");
    while ($key = $rss->data->move("next"))
    {
        //name
        $name  = l($rss->data->get($key, "name"));
        $_name = x($name);
        if (streq($key, $rss_object)) $_name = "<strong>$_name</strong>";

        //url
        $url  = u(CMS_MODULES_URL . "rss.php", ["rss_channel" => $key]);
        $_url = cms_url($url, NULL, TRUE);

        echo("<tr" . ifc_varied() . ">" .

        //selection
             "<td class=\"" . (isset($array[$key]) ? "enabled" : "select") . "\">");
        $ifc->set(NULL, "checkbox", $key, streq($key, $rss_object), "_list[]");
        echo("</td>" .

        //name
             "<td>" .
             "<a href=\"javascript:s('" . qx($key) . "');\">" .
             image("content/icon_rss") . " $_name" .
             "</a>" .
             "</td>" .

        //url
             "<td>" .
             "<a href=\"" . x($url) . "\" " .
                "onclick=\"load_page('" . qx($_url) . "');return false;\">" .
             x(strabridge($url)) .
             "</a>" .
             "</td>" .
             "</tr>");
    };

    //selection control
    echo("<tr>" .
         "<td colspan=\"3\" " .
             "class=\"select\" " .
             "style=\"TEXT-ALIGN:right\">");
    $ifc->set(CMS_L_ALL,    "button", "javascript:ifc_list_activate(\"_list\");");
    $ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert(\"_list\");");
    $ifc->set(CMS_L_NONE,   "button", "javascript:ifc_list_deactivate(\"_list\");");
    $ifc->set($flag ? CMS_L_IFC_CONTENT_076 : CMS_L_IFC_CONTENT_074,
              "button",
              "javascript:content_select_active();");
    echo("</td>" .
         "</tr>");
    ifc_table_close();

    //channel settings
    if ($rss_object)
    {
        //data
        echo("<td>");

        ifc_table_open();
        echo("<tr>" .

        //title
             "<th>" .
             CMS_L_IFC_CONTENT_023 .
             "</th>" .
             "</tr>" .

             "<tr>" .
             "<td>");

        //name
        $ifc->set(CMS_L_NAME,            "text 40 l",        $rss->data->get($rss_object, "name"));

        //save button
        $ifc->set(CMS_L_COMMAND_SAVE,    "button b",         "rss_save");

        //default
        $ifc->set(CMS_L_IFC_CONTENT_134, "checkbox b",       TRUE, $rss->data->get($rss_object, "default"));

        //description
        $ifc->set(CMS_L_DESCRIPTION,     "textarea 50x8 bl", $rss->data->get($rss_object, "description"));

        //link
        $ifc->set(CMS_L_IFC_CONTENT_111, "text 50 bl",       $rss->data->get($rss_object, "link"));

        //image
        $buffer = "javascript:load_page('" .
                  str_replace("%replace%",
                              "'+encodeURIComponent(ifc_get('ifc_param5'))+'",
                              q(cms_url(["ifc_page"          => "image",
                                         "ifc_option"        => "external",
                                         "object"            => "\x1B%replace%",
                                         "ifc_select"        => "object",
                                         "ifc_select_action" => "\x1Bjavascript:this.opener.ifc_set('ifc_param5','%return%');"]))) . "');";

        $ifc->set(CMS_L_IMAGE,           "text 40 l",        $rss->data->get($rss_object, "image"));
        $ifc->set("…",                   "button b",         $buffer);

        //category
        $ifc->set(CMS_L_IFC_CONTENT_062, "text 50 l",        $rss->data->get($rss_object, "category"));
        echo("</td>" .
             "</tr>");
        ifc_table_close();
    };

    echo("</tr>" .
         "</table>");

    $ifc->close();

case "configuration":
//..................................................................................................

    if (! $content->operator) break; //user must be operator

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        TRUE,
        ["user"             => $user,
         "object"           => $object,
         "directory_object" => $directory_object],
        "_configuration",
        CMS_L_IFC_CONTENT_140);

    $system = new system();

    //extra column
    $ifc->set(CMS_L_IFC_CONTENT_121, "title");
    $ifc->set(CMS_L_IFC_CONTENT_144, "text 50 bl", $system->getval("content", "extra_label"));
    $ifc->set(CMS_L_IFC_CONTENT_108, "text 50",    $system->getval("content", "extra_id"));

    $ifc->close();

case "_configuration":
//..................................................................................................

    if (! $content->operator) break; //user must be operator

    $system = new system();
    $system->setval($ifc_param1, "content", "extra_label");
    $system->setval($ifc_param2, "content", "extra_id");

    $ifc_response = $system->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "flag":
//..................................................................................................

    $object = $ifc_param;
    if (! $content->test_flag_set($object)) break;

    //retrieve flag setting of active object
    $result = mysql_query(
        "SELECT " . CMS_DB_CONTENT_FLAG . " " .
        "FROM " .   CMS_DB_CONTENT . " " .
        "WHERE " .  CMS_DB_CONTENT_INDEX . "='" . sqlesc($object) . "' " .
        "LIMIT 1");

    if ($result && ($resultrow = mysql_fetch_assoc($result)))
    {
        //display
        $ifc = new ifc(
            $ifc_response,
            $ifc_page,
            TRUE,
            ["user"             => $user,
             "object"           => $object,
             "directory_object" => $directory_object],
            "_flag",
            CMS_L_IFC_CONTENT_140);

        $flag = $resultrow[CMS_DB_CONTENT_FLAG];

        //sitemap
        $ifc->set(CMS_L_IFC_CONTENT_139, "title");

        //exclude from sitemap
        $ifc->set(CMS_L_IFC_CONTENT_057, "checkbox", TRUE, $flag & CMS_CONTENT_FLAG_SITEMAP_EXCLUDE);

        //meta robots
        $ifc->set(CMS_L_IFC_CONTENT_056, "title");

        //noindex
        $ifc->set(CMS_L_IFC_CONTENT_037, "checkbox", TRUE, $flag & CMS_CONTENT_FLAG_META_ROBOTS_NOINDEX);

        //nofollow
        $ifc->set(CMS_L_IFC_CONTENT_038, "checkbox", TRUE, $flag & CMS_CONTENT_FLAG_META_ROBOTS_NOFOLLOW);

        $ifc->close();
    };

    $ifc_response = CMS_MSG_ERROR;
    break;

case "_flag":
//..................................................................................................

    $flag  = 0;
    $flag |= isset($ifc_param1) ? CMS_CONTENT_FLAG_SITEMAP_EXCLUDE      : 0;
    $flag |= isset($ifc_param2) ? CMS_CONTENT_FLAG_META_ROBOTS_NOINDEX  : 0;
    $flag |= isset($ifc_param3) ? CMS_CONTENT_FLAG_META_ROBOTS_NOFOLLOW : 0;

    $ifc_response = $content->flag_set($object, $flag) ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;

case "analyze":
case "analyze_span":
//..................................................................................................

    //retrieve document data
    $result = mysql_query(
        "SELECT " . CMS_DB_CONTENT_BUFFER_TITLE . ", " .
                    CMS_DB_CONTENT_BUFFER_DESCRIPTION . ", " .
                    CMS_DB_CONTENT_BUFFER_KEYWORD . ", " .
                    CMS_DB_CONTENT_BUFFER_TEXT . ", " .
                    CMS_DB_CONTENT_BUFFER_TEMPLATE . " " .
        "FROM " .   CMS_DB_CONTENT . " " .
        "WHERE " .  CMS_DB_CONTENT_INDEX .
        "='" .      sqlesc($object) . "' " .
        "LIMIT 1");

    if (! ($result && ($resultrow = mysql_fetch_assoc($result)))) ifc_close_external();

    define("CMS_CONTENT_INDEX",           $object);
    define("CMS_CONTENT_DIRECTORY_INDEX", content_get_directory_index(CMS_CONTENT_INDEX));

    //parse document
    $document = new document($resultrow[CMS_DB_CONTENT_BUFFER_TEXT]);
    $template = new template();
    cms_param(FALSE, "cms_instance");
    $code     = $template->parse(
        $document,
        $resultrow[CMS_DB_CONTENT_BUFFER_TEMPLATE],
        $resultrow[CMS_DB_CONTENT_BUFFER_TITLE],
        $resultrow[CMS_DB_CONTENT_BUFFER_DESCRIPTION],
        $resultrow[CMS_DB_CONTENT_BUFFER_KEYWORD]);
    $size     = utf8_strlen($code);
    $text     = htmltoplain($code, TRUE, TRUE, TRUE);
    $_size    = utf8_strlen($text);
    $language = (CMS_LANGUAGE !== "") ? CMS_LANGUAGE : language_detect($text);
    $_text    = language_strip_stopword($text, $language);
    $text     = x($text);
    $list     = tokenize_text($_text);
    $_list    = [];
    $count    = count($list);
    $span     = streq(CMS_IFC_MESSAGE, "analyze_span") ? $ifc_param : (empty($span) ? 0 : $span);
    $buffer   = [0 => ""];

    //initiate word span window
    for ($i = 0; ($i < $span) && ($i < $count); $i++)
        array_push($buffer, utf8_strtolower($list[$i]));

    //count word span occurrences
    for (; $i < $count; $i++)
    {
        array_shift($buffer);
        array_push($buffer, utf8_strtolower($list[$i]));

        $value = implode(" ", $buffer);

        if (isset($_list[$value])) $_list[$value] += 1;
        else                       $_list[$value]  = 1;
    };

    //sort by occurrence then word
    array_multisort(
        array_values($_list), SORT_NUMERIC,                  SORT_DESC,
        array_keys($_list),   SORT_NATURAL | SORT_FLAG_CASE, SORT_ASC,
        $_list);

    //keyword highlighting
    $keyword = l($resultrow[CMS_DB_CONTENT_BUFFER_KEYWORD]);
    $keyword = tokenize_text($keyword);
    if (! empty($keyword))
    {
        $keyword  = array_unique($keyword);
        $keyword  = array_map(__NAMESPACE__ . "\x", $keyword);
        $_keyword = array_map(function($value) { return preg_quote($value, "/"); }, $keyword);
        $text     = preg_replace("/(?:" . implode("|", $_keyword) . ")/iu",
                                 "<strong>\\0</strong>", $text);
    };

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        NULL,
        ["user"   => $object,
         "object" => $object,
         "span"   => $span],
        NULL,
        CMS_L_IFC_CONTENT_097);

    //plain text
    ifc_tab_open(CMS_L_IFC_CONTENT_096);

    echo("<div class=\"text\">" .

    //title
         "<h1>" .
         x(l($resultrow[CMS_DB_CONTENT_BUFFER_TITLE])) .
         "</h1>");

    //description
    if ($buffer = stripspaces(remove_format(l($resultrow[CMS_DB_CONTENT_BUFFER_DESCRIPTION]))))
        echo("<div style=\"FONT-STYLE:italic\">" .
             x($buffer) .
             "</div>");

    //keyword
    if ($buffer = implode("</strong>, <strong>", $keyword))
        echo("<div style=\"FONT-STYLE:italic;MARGIN-TOP:5px\">" .
             CMS_L_IFC_CONTENT_012 . ": " .
             "<strong>" .
             $buffer .
             "</strong>" .
             "</div>");

    //content
    echo("<hr>" .
         $text .
         "</div>");

    //word span selection
    ifc_tab_next(CMS_L_IFC_CONTENT_066);

    ifc_table_open();
    echo("<tr>" .
         "<td>" .
         CMS_L_IFC_CONTENT_102 . ":");
    for ($i = 0; $i <= 4; )
    {
        $flag = $i == $span;
        echo(" <a href=\"javascript:ifc_post('analyze_span','$i');\">");
        if ($flag) echo("<strong>");
        echo(++$i);
        if ($flag) echo("</strong>");
        echo("</a>");
    };
    echo("</td>" .

    //markup quota
         "<th style=\"TEXT-ALIGN:right\">" .
         CMS_L_IFC_CONTENT_104 . ":" .
         "</th>" .
         "<td style=\"TEXT-ALIGN:right\">" .
         "<strong>" .
         number_format(
            100 * $_size / $size, 2,
            CMS_L_DECIMAL_SEPARATOR,
            CMS_L_THOUSAND_SEPARATOR) . "%" .
         "</strong>" .
         "</td>" .

    //content quota
         "<th style=\"TEXT-ALIGN:right\">" .
         CMS_L_IFC_CONTENT_096 . ":" .
         "</th>" .
         "<td style=\"TEXT-ALIGN:right\">" .
         "<strong>" .
         number_format(
            100 * ($size - $_size) / $size, 2,
            CMS_L_DECIMAL_SEPARATOR,
            CMS_L_THOUSAND_SEPARATOR) . "%" .
         "</strong>" .
         "</td>" .

    //spacer
         "<th rowspan=\"2\">" .
         "</th>" .
         "</tr>" .

    //word list
         "<tr>" .
         "<th colspan=\"2\">" .
         CMS_L_IFC_CONTENT_098 .
         "</th>" .
         "<th colspan=\"2\" " .
             "style=\"TEXT-ALIGN:right\">" .
         CMS_L_IFC_CONTENT_099 .
         "</th>" .
         "<th style=\"TEXT-ALIGN:right\">" .
         CMS_L_IFC_CONTENT_100 .
         "</th>" .
         "</tr>");

    $maximum = reset($_list);

    foreach ($_list AS $key => $value)
    {
        if ($value < 2) break;

        echo("<tr" . ifc_varied() . ">" .
             "<td colspan=\"2\">" .
             x($key) .
             "</td>" .
             "<td colspan=\"2\" " .
                 "style=\"TEXT-ALIGN:right\">" .
             x($value) .
             "</td>" .
             "<td style=\"TEXT-ALIGN:right\">" .
             number_format(
                100 * $value / $count, 2,
                CMS_L_DECIMAL_SEPARATOR,
                CMS_L_THOUSAND_SEPARATOR) . "%" .
             "</td>" .
             "<td>" .
             "<div class=\"beam\" " .
                  "style=\"--width:" . (int)(100 * $value / $maximum) . "\">" .
             "</div> " .
             "</td>" .
             "</tr>");
    };

    echo("<tr" . ifc_varied() . ">" .

    //total word count
         "<th colspan=\"2\" " .
             "style=\"TEXT-ALIGN:right\">" .
         CMS_L_IFC_CONTENT_101 . ":" .
         "</th>" .
         "<td colspan=\"2\" " .
             "style=\"TEXT-ALIGN:right\">" .
         "<strong>" .
         x($count) .
         "</strong>" .
         "</td>" .

    //total percent
         "<td style=\"TEXT-ALIGN:right\">" .
         "<strong>" .
         number_format(
             100, 2,
             CMS_L_DECIMAL_SEPARATOR,
             CMS_L_THOUSAND_SEPARATOR) .
         "%" .
         "</strong>" .
         "</td>" .

    //spacer
         "<th>" .
         "</th>" .
         "</tr>");
    ifc_table_close();

    $ifc->set(CMS_L_IFC_CONTENT_103, "info");

    //source code
    ifc_tab_next(CMS_L_IFC_CONTENT_080);

    $ifc->set(NULL, "code_html f", $code);

    ifc_tab_close();

    $ifc->close();

//..................................................................................................

case "debug":

    //template operators only
    if (! cms_permission("interface.template.operator", NULL, NULL)) ifc_close_external();

    //retrieve document data
    $result = mysql_query(
        "SELECT " . CMS_DB_CONTENT_BUFFER_TEXT . ", " .
                    CMS_DB_CONTENT_BUFFER_TEMPLATE . " " .
        "FROM " .   CMS_DB_CONTENT . " " .
        "WHERE " .  CMS_DB_CONTENT_INDEX .
        "='" .      sqlesc($object) . "' " .
        "LIMIT 1");

    if (! ($result && ($resultrow = mysql_fetch_assoc($result)))) ifc_close_external();

    define("CMS_CONTENT_INDEX", $object);

    //parse document
    $document = new document(
        $resultrow[CMS_DB_CONTENT_BUFFER_TEXT],
        $resultrow[CMS_DB_CONTENT_BUFFER_TEMPLATE]);

    //group elements by parent
    $array = [];
    foreach ($document->structure AS $key => $value)
        $array[$value[2]][] = $key;

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        NULL,
        NULL,
        NULL,
        CMS_L_IFC_CONTENT_146);

    //generate tree view
    $stack = [];
    $key   = 0;

    echo("<ul>");
    while (TRUE)
    {
        while (list(, $value) = each($array[$key]))
        {
            $data = &$document->structure[$value];
            $id   = $data[0];

            echo("<li>" .
                 "<strong>id:</strong> " .      x($id) . " ︱ " .
                 "<strong>path:</strong> " .    x($data[1]) . " ︱ " .
                 "<strong>type:</strong> " .    x($data[3]) . " ︱ " .
                 "<strong>value:</strong> " .   x(first_words($document->data[$id][CMS_DOCUMENT_VALUE] ?? "", 50)) . " ︱ " .
                 "<strong>default:</strong> " . x(first_words($document->default[$id][CMS_DOCUMENT_VALUE] ?? "", 50)));

            if (isset($array[$value]))
            {
                echo("<ul>");
                $stack[] = $key;
                $key     = $value;
                continue;
            };

            echo("</li>");
        };

        echo("</ul>");

        if (($key = array_pop($stack)) === NULL) break;

        echo("</li>");
    };

    $ifc->close();

case "reset":
//..................................................................................................

    $filter       = NULL;
    $search_field = NULL;
    $search_value = NULL;
    $order        = NULL;
    $limit        = NULL;

    cms_cache_delete(
        ["content." . CMS_USER . ".filter",
         "content." . CMS_USER . ".search_field",
         "content." . CMS_USER . ".search_value",
         "content." . CMS_USER . ".order",
         "content." . CMS_USER . ".page",
         "content." . CMS_USER . ".limit"]);
    break;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

//sync settings 1
//..................................................................................................

cms_cache_sync($object,           "content." . CMS_USER . ".object.$user", NULL, TRUE);
cms_cache_sync($directory_object, "directory." . CMS_USER . ".object");
cms_cache_sync($filter,           "content." . CMS_USER . ".filter.$user", CMS_L_IFC_CONTENT_094);
cms_cache_sync($search_value,     "content." . CMS_USER . ".search_value", "");
cms_cache_sync($order,            "content." . CMS_USER . ".order",         9);
cms_cache_sync($page,             "content." . CMS_USER . ".page.$user",    0);
cms_cache_sync($limit,            "content." . CMS_USER . ".limit",        25);

//flags
//..................................................................................................

$flag_sender = streq($filter, CMS_L_IFC_CONTENT_055);
$flag_author = nstreq($filter, CMS_L_IFC_CONTENT_022);
$flag_extra  = FALSE;
$system      = new system();
$extra_label = l($system->getval("content", "extra_label"));
$extra_id    = $system->getval("content", "extra_id");

if (nstre($extra_label . $extra_id))
{
    if (stre($extra_label)) $extra_label = CMS_L_IFC_CONTENT_121;
    $flag_extra = TRUE;
};

//user list
//..................................................................................................

$data      = new data("#system/permission");
$user_list = [];

$data->move("first");
while ($key = $data->move("next"))
{
    if (! permission_is_user($key)) continue;
    $_key = substr($key, 5);

    if (

    //not current user
         nstreq($_key, CMS_SUPERUSER)
         &&

    //target user can access content module
         cms_permission(cms_application(), NULL, NULL, $_key)
         &&

    //current user can access target user's content
         cms_permission($_key)
       )
        $user_list[$_key] = $data->get($key, "name");
};

//filter list
//..................................................................................................

$_user        = sqlesc($user);
$filter_list  = [CMS_L_IFC_CONTENT_094 =>          NULL,
                 CMS_L_IFC_CONTENT_055 =>          CMS_DB_CONTENT_SENDER . "!=''"];

if ($content->action(CMS_CONTENT_TYPE_ORIGINAL, CMS_CONTENT_STATUS_DRAFT, CMS_CONTENT_ACTION_RECEIVE))
    $filter_list[CMS_L_IFC_CONTENT_143] =          CMS_DB_CONTENT_STATUS .  "='" . CMS_CONTENT_STATUS_DRAFT . "'";

if ($content->action(CMS_CONTENT_TYPE_ORIGINAL, CMS_CONTENT_STATUS_DRAFT, CMS_CONTENT_ACTION_RECEIVE))
    $filter_list[CMS_L_IFC_CONTENT_022] =          CMS_DB_CONTENT_STATUS .  "='" . CMS_CONTENT_STATUS_DRAFT . "' " .
                                          "AND " . CMS_DB_CONTENT_WRITER .  "='" . $_user . "'";

if ($content->action(CMS_CONTENT_TYPE_ORIGINAL, CMS_CONTENT_STATUS_DRAFT, CMS_CONTENT_ACTION_RECEIVE))
    $filter_list[CMS_L_IFC_CONTENT_039] =          CMS_DB_CONTENT_STATUS .  "='" . CMS_CONTENT_STATUS_DRAFT . "' " .
                                          "AND " . CMS_DB_CONTENT_WRITER . "!='" . $_user . "'";

if ($content->action(CMS_CONTENT_TYPE_ORIGINAL, CMS_CONTENT_STATUS_DOCUMENT, CMS_CONTENT_ACTION_RECEIVE))
    $filter_list[CMS_L_IFC_CONTENT_030] =          CMS_DB_CONTENT_STATUS .  "='" . CMS_CONTENT_STATUS_DOCUMENT . "'";

if ($content->action(CMS_CONTENT_TYPE_ORIGINAL, CMS_CONTENT_STATUS_PUBLICATION, CMS_CONTENT_ACTION_RECEIVE))
    $filter_list[CMS_L_IFC_CONTENT_024] =          CMS_DB_CONTENT_STATUS .  "='" . CMS_CONTENT_STATUS_PUBLICATION . "' " .
                                          "AND " . CMS_DB_CONTENT_TYPE .    "='" . CMS_CONTENT_TYPE_ORIGINAL . "'";

//search field list
//..................................................................................................

$search_field_list =  [CMS_L_IFC_CONTENT_065 => CMS_DB_CONTENT_INDEX,
                       CMS_L_IFC_CONTENT_027 => CMS_DB_CONTENT_TITLE,
                       CMS_L_IFC_CONTENT_008 => CMS_DB_CONTENT_DESCRIPTION,
                       CMS_L_IFC_CONTENT_012 => CMS_DB_CONTENT_KEYWORD,
                       CMS_L_IFC_CONTENT_028 => CMS_DB_CONTENT_TEXT];
if ($flag_sender)
    $search_field_list[CMS_L_IFC_CONTENT_003] = CMS_DB_CONTENT_SENDER;
if ($flag_author)
    $search_field_list[CMS_L_IFC_CONTENT_047] = CMS_DB_CONTENT_AUTHOR;
if ($flag_extra)
{
    $_extra_label = $extra_label;
    $i            = 0;
    while (isset($search_field_list[$_extra_label]))
        $_extra_label = $extra_label . " (" . ++$i . ")";
    $search_field_list[$_extra_label] = CMS_DB_CONTENT_EXTRA_VALUE;
};

//order list
//..................................................................................................

$order_list =
    [ 0 => CMS_DB_CONTENT_INDEX,        1 => CMS_DB_CONTENT_INDEX .       " DESC",
      2 => CMS_DB_CONTENT_SENDER,       3 => CMS_DB_CONTENT_SENDER .      " DESC",
      4 => CMS_DB_CONTENT_TITLE,        5 => CMS_DB_CONTENT_TITLE .       " DESC",
      6 => CMS_DB_CONTENT_AUTHOR,       7 => CMS_DB_CONTENT_AUTHOR .      " DESC",
      8 => CMS_DB_CONTENT_WRITER_TIME,  9 => CMS_DB_CONTENT_WRITER_TIME . " DESC",
     10 => CMS_DB_CONTENT_EXTRA_VALUE, 11 => CMS_DB_CONTENT_EXTRA_VALUE . " DESC"];

//sync settings 2
//..................................................................................................

cms_cache_sync($search_field, "content." . CMS_USER . ".search_field", array_values($search_field_list));

//prepare directory
//..................................................................................................

$directory_data = new data("#system/directory");
$flexview       = new flexview();
$flexview->import_data($directory_data);

//get linked documents
$directory_list = [];
$directory_mark = [];
$depth          = -1;
$found          = FALSE;

$directory_data->move("first");
while ($key = $directory_data->move("next"))
{
    //depth
    if ($depth >= 0)
    {
        switch ($directory_data->get($key, "#type"))
        {
        case "container":  $depth++; break;
        case "/container": $depth--; break;
        };
    };

    //active entry
    if (streq($key, $directory_object))
    {
        //start listing
        $depth = 0;

        //mark active entry
        $directory_mark[$key] = "content/icon_directory_selected";
    };

    $url = $directory_data->get($key, "url");

    //directory dereferencing
    $list = [$key => TRUE];
    while (strncmp($url, "directory://", 12) === 0)
    {
        //get directory index
        $_key = substr($url, 12, strcspn($url, "?#", 12));

        //check for loop
        if (isset($list[$_key])) break;
        $_list[$_key] = TRUE;

        $url = $directory_data->get($_key, "url");
    };

    //linked entry
    if (strncmp($url, "content://", 10) === 0)
    {
        //get content index
        $_key = substr($url, 10);
        $_key = substr($_key, 0, strspn($_key, "1234567890"));

        //check active entry
        $_found  = streq($_key, $object);
        $found  |= $_found;

        //active entry
        if ($depth === 0)
        {
            $directory_list[$_key] = TRUE;
        }

        else
        {
            //contained entry
            if ($depth === 1)
            {
                $directory_list[$_key] = FALSE;
                $directory_mark[$key]  = "content/icon_directory_contained";
            };

            //linked entry
            if ($_found) $directory_mark[$key] = "content/icon_directory_linked";
        };
    };
};

if (! empty($directory_list)) $directory_list = array_reverse($directory_list, TRUE);

//menu
//..................................................................................................

$menu = [];
if ($content->test_create())
    $menu[CMS_L_IFC_CONTENT_049 . "|content/command_create"]        = "create";
$menu[CMS_L_COMMAND_DELETE_SELECTED . "|content/command_delete"]    = "#delete";
if ($content->writer)
    $menu[CMS_L_IFC_CONTENT_095 . "|content/command_publish"]       = "#apply_all";
if ($content->publisher)
{
    $menu[CMS_L_IFC_CONTENT_061 . "|content/command_rss"]           = "rss";
    $menu[CMS_L_IFC_CONTENT_050 . "|content/command_clear"]         = "#clear_cache";
};
if ($content->operator)
    $menu[CMS_L_IFC_CONTENT_140 . "|content/command_configuration"] = "configuration";

//display
//..................................................................................................

$ifc = new ifc(
    $ifc_response,
    $ifc_page,
    $menu,
    ["object"           => $object,
     "directory_object" => $directory_object,
     "order"            => $order,
     "page"             => $page]);

//user
//..................................................................................................

if (! empty($user_list))
{

?><script>

function content_select_user()
{
    ifc_del("object");
    ifc_del("filter");
    ifc_del("page");
    ifc_submit();
};

</script><?php

    //sort and add current user at top
    ksort($user_list, SORT_NATURAL | SORT_FLAG_CASE);
    $user_list = array_merge([CMS_SUPERUSER => CMS_NAME], $user_list);

    echo("<label title=\"" . CMS_L_IFC_CONTENT_091 . "\" " .
                "class=\"element\">" .
         "<span>" .
         image("content/icon_user") .
         "</span>" .
         "<select name=\"user\" " .
                 "onchange=\"content_select_user();\">");

    foreach ($user_list AS $key => $value)
        echo("<option value=\"" . x($key) . "\"" .
                      (streq($key, $user) ? " selected>" : ">") .
             x($value) .
             "</option>");

    echo("</select>" .
         "</label>");
};

//filter
//..................................................................................................

?><script>

function content_select_filter()
{
    ifc_del("page");
    ifc_submit();
};

</script><?php

$array = [];
foreach ($filter_list AS $key => $value)
{
    if ($value === NULL) $value = 1;
    $array[] = "SUM(CASE WHEN $value THEN 1 ELSE 0 END) AS `" . sqlesc($key, TRUE) . "`";
};

$result = mysql_query(
    "SELECT " . implode(", ", $array) . " " .
    "FROM " .   CMS_DB_CONTENT . " " .
    "WHERE " .  CMS_DB_CONTENT_OWNER .
    "='" .      sqlesc($user) . "'");

if ($result && ($resultrow = mysql_fetch_assoc($result)))
{
    echo("<label title=\"" . CMS_L_IFC_CONTENT_092 . "\" " .
                "class=\"element\">" .
         "<span>" .
         image("content/icon_box") .
         "</span>" .
         "<select name=\"filter\" " .
                 "onchange=\"content_select_filter();\">");

    foreach ($resultrow AS $key => $value)
    {
        echo("<option value=\"$key\"" .
                      (streq($key, $filter) ? " selected>" : ">") .
             "$key (" . (int)$value . ")" .
             "</option>");
    };

    echo("</select>" .
         "</label>");
};

//search
//..................................................................................................

echo("<div class=\"element\">");
ifc_popover_open(CMS_L_IFC_CONTENT_093);

foreach ($search_field_list AS $key => $value)
    $ifc->set($key, "checkbox w", $value, in_array($value, $search_field), "search_field[]");

ifc_popover_close();

$ifc->set(NULL, "text 20", $search_value, NULL, "search_value");
echo("</div>");

//rows per page
//..................................................................................................

$ifc->set(CMS_L_IFC_CONTENT_060, "label");
$ifc->set(
    [ 10 =>  "10",
      25 =>  "25",
      50 =>  "50",
     100 => "100",
     250 => "250",
     500 => "500"],
    "select :",
    $limit,
    NULL,
    "limit");

//apply/reset button
//..................................................................................................

$ifc->set(CMS_L_COMMAND_CONFIRM, "button");
$ifc->set(CMS_L_IFC_CONTENT_073, "button b", "reset");

//display directory
//..................................................................................................

ifc_popover_open(CMS_L_IFC_CONTENT_011);

//active object not found note
if ((! $found) && nstre($object . $directory_object))
    $ifc->set(CMS_L_IFC_CONTENT_137, "info b");

echo("<div class=\"content-directory\">");

//directory
$flexview->set_encoding_function(__NAMESPACE__ ."\\q");
$flexview->set_display_function(__NAMESPACE__ ."\\directory_flexview_display_function");
$flexview->show_tree(
    $directory_object,
    "javascript:ifc_post('directory_select','%index%');",
    "name",
    $directory_mark,
    directory_get_type());

echo("</div>");
ifc_popover_close();

//object list query
//..................................................................................................

$flag_type   = isset($filter_list[$filter]);
$flag_search = (! empty($search_field)) && nstre($search_value);
$flag_list   = $object || (! empty($directory_list));

//search filter
if ($flag_search)
{
    $search = [];
    foreach ($search_field_list AS $value)
    {
        if (! in_array($value, $search_field)) continue;
        $search[] = "`" .   sqlesc($value, TRUE) . "` LIKE '%" . sqlesc($search_value) . "%'";
    };
    $search = "(" . implode(" OR ", $search) . ")";
};

//directory list
if ($flag_list)
{
    $list  = array_keys($directory_list);
    if ($object && (! isset($directory_list[$object]))) array_push($list, $object);
    $list  = implode("', '", sqlesc($list));
    $_list = CMS_DB_CONTENT_INDEX . " IN ('$list')";
};

$query_version  =
    "SELECT DISTINCT " . CMS_DB_CONTENT_VERSION_CONTENT . " " .
    "FROM " .            CMS_DB_CONTENT_VERSION;

$query_schedule =
    "SELECT DISTINCT " . CMS_DB_CONTENT_SCHEDULE_CONTENT . " " .
    "FROM " .            CMS_DB_CONTENT_SCHEDULE;

$query =
    "SELECT " . CMS_DB_CONTENT_INDEX . ", " .
                CMS_DB_CONTENT_TYPE . ", " .
                CMS_DB_CONTENT_STATUS . ", " .
                CMS_DB_CONTENT_WRITER_TIME . ", " .
                CMS_DB_CONTENT_TIME . ", " .
                CMS_DB_CONTENT_TITLE . ", " .
                CMS_DB_CONTENT_AUTHOR . ", " .
                CMS_DB_CONTENT_INDEX . " IN ($query_version) AS `#version`, " .
                CMS_DB_CONTENT_INDEX . " IN ($query_schedule) AS `#schedule` ";

//sender
if ($flag_sender) $query .= ", " .
                CMS_DB_CONTENT_SENDER;

//extra
if ($flag_extra) $query  .= ", " .
                CMS_DB_CONTENT_EXTRA_VALUE . ", " .
                CMS_DB_CONTENT_EXTRA_TYPE . ", " .
                CMS_DB_CONTENT_EXTRA_COLOR;

//filter
$query .= " " .
    "FROM " .   CMS_DB_CONTENT . " " .
    "WHERE " .  CMS_DB_CONTENT_OWNER .
    "='" .      sqlesc($user) . "' ";

if ($flag_type && $flag_search && $flag_list) $query .=
    "AND ((" .  $filter_list[$filter] . " " .
    "AND " .    $search . ") " .
    "OR " .     $_list .  ") ";

elseif ($flag_type && $flag_search) $query .=
    "AND " .    $filter_list[$filter] . " " .
    "AND " .    $search . " ";

elseif ($flag_type && $flag_list) $query .=
    "AND (" .   $filter_list[$filter] . " " .
    "OR " .     $_list .  ") ";

elseif ($flag_search && $flag_list) $query .=
    "AND (" .   $search . " " .
    "OR " .     $_list .  ") ";

elseif ($flag_type) $query .=
    "AND " .    $filter_list[$filter] . " ";

elseif ($flag_search) $query .=
    "AND " .    $search . " ";

//order by
$query .=
    "ORDER BY ";

if ($flag_list) $query .=
    "FIELD(" .  CMS_DB_CONTENT_INDEX . ", '$list') DESC, ";

$query .=       $order_list[$order] ?? CMS_DB_CONTENT_INDEX;

//empty result
//..................................................................................................

if (
     (! $result = mysql_query($query))
     ||
     (! $count = mysql_num_rows($result))
   )
{
    echo("<br>");
    $ifc->set(CMS_L_IFC_CONTENT_026, "info");
    $ifc->close();
};

//javascript helper functions
//..................................................................................................

?><script>

function a1(index) //apply
{
    ifc_post("apply", index);
};

function a2(index) //authorize
{
    ifc_post("authorize", index);
};

function c(index) //copy
{
    ifc_post("copy", index);
};

function d1(index) //display
{
    load_page("<?php
        echo(str_replace("%replace%",
                         "\"+encodeURIComponent(index)+\"",
                         q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "display",
                                    "ifc_option"  => "external",
                                    "object"      => "\x1B%replace%"]))));?>");
};

function d2(index) //duplicate
{
    ifc_post("duplicate", index);
};

function dd(index) //derive draft
{
    ifc_post("derive_draft", index);
};

function e(index) //edit
{
    load_page("<?php
        echo(str_replace("%replace%",
                         "\"+encodeURIComponent(index)+\"",
                         q(cms_url(CMS_MODULES_URL . "content.php",
                                   ["content_index"  => "\x1B%replace%",
                                    "content_option" => CMS_TEMPLATE_OPTION_ALL,
                                    "content_user"   => $user],
                                   TRUE))));?>");
};

function e2(url) //extra
{
    load_page(url);
    return false;
};

function f(index) //flag
{
    ifc_post("flag", index);
};

function i(index) //info
{
    load_page("<?php
        echo(str_replace("%replace%",
                         "\"+encodeURIComponent(index)+\"",
                         q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "info",
                                    "ifc_option"  => "external",
                                    "object"      => "\x1B%replace%"]))));?>");
};

function m(index) //meta
{
    ifc_post("meta", index);
};

function o(value) //order
{
    ifc_set("order", value);
    ifc_submit();
};

function p1(index) //publish
{
    ifc_post("publish", index);
};

function p2(number) //page
{
    ifc_set("page", number);
    ifc_submit();
};

function r1(index) //revert
{
    if (confirm("<?php echo(q(CMS_L_IFC_CONTENT_088))?>"))
        ifc_post("revert", index);
};

function r2(index) //rss
{
    ifc_post("rss_assign", index);
};

function s1(index) //select
{
    ifc_post("select", index);
};

function s2(index) //send
{
    ifc_post("schedule", index);
};

function s3(index) //send
{
    ifc_post("send", index);
};

function v(index) //version
{
    ifc_post("version", index);
};

function vs(index) //version store
{
    ifc_post("version_store", index);
};

function w(index) //withdraw
{
    if (confirm("<?php echo(q(CMS_L_IFC_CONTENT_054))?>?"))
        ifc_post("withdraw", index);
};

</script><?php

//pagination
//..................................................................................................

if ($count > $limit)
{
    $count = ceil($count / $limit);
    $page  = min((int)$page, $count - 1);

    pagination(
        "javascript:p2(%page%);",
        $page,
        $count,
        CMS_L_COMMAND_NEXT,
        "pagination");

    mysql_data_seek($result, $page * $limit);
};

//helper functions
//..................................................................................................

$f1 = function($order, $asc, $dsc)
{
    switch ((int)$order)
    {
    case $asc: return " ↗";
    case $dsc: return " ↘";
    };

    return "";
};

$f2 = function($order, $asc, $dsc)
{
    switch ((int)$order)
    {
    default:
    case $dsc: return "javascript:o($asc);";
    case $asc: return "javascript:o($dsc);";
    };
};

//object table
//..................................................................................................

ifc_table_open("content-table");
echo("<colgroup>" .
     "<col style=\"WIDTH:0\">" . //selection
     "<col style=\"WIDTH:0\">"); //index
if ($flag_sender) echo("<col>"); //sender
echo("<col>" .                   //title
     "<col style=\"WIDTH:0\">"); //control
if ($flag_author) echo("<col>"); //author
echo("<col>" .                   //date
     "</colgroup>" .

     "<tr>" .

//dummy
     "<td class=\"select\">" .
     "</td>" .

//index
     "<th>");
$ifc->set(CMS_L_IFC_CONTENT_065 . $f1($order, 0, 1), "button", $f2($order, 0, 1));
echo("</th>");

//sender
if ($flag_sender)
{
    echo("<th>");
    $ifc->set(CMS_L_IFC_CONTENT_003 . $f1($order, 2, 3), "button", $f2($order, 2, 3));
    echo("</th>");
};

//title
echo("<th>");
$ifc->set(CMS_L_IFC_CONTENT_027 . $f1($order, 4, 5), "button", $f2($order, 4, 5));
echo("</th>" .

//control
     "<th>" .
     CMS_L_IFC_CONTENT_127 .
     "</th>");

//author
if ($flag_author)
{
    echo("<th>");
    $ifc->set(CMS_L_IFC_CONTENT_047 . $f1($order, 6, 7), "button", $f2($order, 6, 7));
    echo("</th>");
};

//date
echo("<th>");
$ifc->set(CMS_L_IFC_CONTENT_010 . $f1($order, 8, 9), "button", $f2($order, 8, 9));
echo("</th>");

//extra
if ($flag_extra)
{
    echo("<th>");
    $ifc->set($_extra_label . $f1($order, 10, 11), "button", $f2($order, 10, 11));
    echo("</th>");
};

echo("</tr>");

//object table body
//..................................................................................................

$i = 0;

while (
        ($i++ < $limit)
        &&
        ($resultrow = mysql_fetch_assoc($result))
      )
{
    $index = $resultrow[CMS_DB_CONTENT_INDEX];
    $title = x(strabridge(l($resultrow[CMS_DB_CONTENT_TITLE]), 60));

    //edited
    if ($resultrow[CMS_DB_CONTENT_WRITER_TIME] !== $resultrow[CMS_DB_CONTENT_TIME])
        $title = "<em>$title (" . CMS_L_IFC_CONTENT_141 . ")</em>";

    //active
    if ($flag_active = streq($resultrow[CMS_DB_CONTENT_INDEX], $object))
        $title = "<strong>$title</strong>";

    //directory
    $flag_directory = isset($directory_list[$index]);

    //row start
    echo("<tr" . ifc_varied($flag_directory ? "related" : "") . ">" .

    //selection
         "<td class=\"select\">");
    $ifc->set(NULL, "checkbox", $index, streq($index, $object), "list[]");
    echo("</td>" .

    //index
         "<td style=\"TEXT-ALIGN:right\">" .
         x($index) .
         "</td>");

    //sender
    if ($flag_sender)
        echo("<td>" .
             x($resultrow[CMS_DB_CONTENT_SENDER]) .
             "</td>");

    //title
    $_index  = (int)$index;
    $_status = (int)$resultrow[CMS_DB_CONTENT_STATUS];
    $_type   = (int)$resultrow[CMS_DB_CONTENT_TYPE];

    echo("<td>");

    //directory
    if ($flag_directory)
        echo(image("content/icon_directory_" . (($directory_list[$index] === TRUE) ? "selected" : "contained")));
    elseif ($flag_active)
        echo(image("content/icon_pinned"));

    echo("<a href=\"javascript:s1($_index);\"" .
            "title=\"" . x($status[$_status] . " " . $type[$_type]) . "\"" .
            (($_status === CMS_CONTENT_STATUS_PUBLICATION) ? " class=\"green\"" : "") . ">" .
         image($icon[$_status][$_type]) . " $title" .
         "</a>" .
         "</td>" .

//editing functions
//..................................................................................................

         "<td>" .

    //display
         "<a href=\"javascript:d1($_index);\" " .
            "title=\"" . CMS_L_COMMAND_SHOW . "\">" .
         image("content/button_display") .
         "</a>" .

    //info
         "<a href=\"javascript:i($_index);\" " .
            "title=\"" . CMS_L_IFC_CONTENT_138 . "\">" .
         image("content/button_info") .
         "</a>");

    //separator
    echo(image("content/separator"));

    if ($content->test_update($index))
    {
        //edit
        echo("<a href=\"javascript:e($_index);\" " .
                "title=\"" . CMS_L_COMMAND_EDIT . "\">" .
             image("content/button_edit") .
             "</a>" .

        //meta
             "<a href=\"javascript:m($_index);\" " .
                "title=\"" . CMS_L_IFC_CONTENT_077 . "\">" .
             image("content/button_meta") .
             "</a>");

        //edited
        if ($resultrow[CMS_DB_CONTENT_WRITER_TIME] !== $resultrow[CMS_DB_CONTENT_TIME])
        {
            //apply
            echo("<a href=\"javascript:a1($_index);\" " .
                    "title=\"" . CMS_L_IFC_CONTENT_087 . "\">" .
                 image("content/button_apply") .
                 "</a>" .

            //revert
                 "<a href=\"javascript:r1($_index);\" " .
                    "title=\"" . CMS_L_IFC_CONTENT_088 . "\">" .
                 image("content/button_revert") .
                 "</a>");
        }
        else
        {
            echo(image("content/button_apply_disabled") .
                 image("content/button_revert_disabled"));
        };
    }
    else
    {
        echo(image("content/button_edit_disabled") .
             image("content/button_apply_disabled") .
             image("content/button_revert_disabled") .
             image("content/button_meta_disabled"));
    };

    //separator
    echo(image("content/separator"));

    //version store
    echo($content->test_version_store($index) ?
         "<a href=\"javascript:vs($_index);\" " .
            "title=\"" . CMS_L_IFC_CONTENT_110 . "\">" .
         image("content/button_version_store") .
         "</a>" :
         image("content/button_version_store_disabled"));

    //version
    echo((($_type !== CMS_CONTENT_TYPE_COPY) && $resultrow["#version"]) ?
         "<a href=\"javascript:v($_index);\" " .
            "title=\"" . CMS_L_IFC_CONTENT_085 . "\">" .
         image("content/button_version") .
         "</a>" :
         image("content/button_version_disabled"));

    //separator
    echo(image("content/separator"));

    //authorize
    if ($content->test_authorize($index))
        echo("<a href=\"javascript:a2($_index);\" " .
                "title=\"" . CMS_L_IFC_CONTENT_029 . "\">" .
             image("content/button_authorize") .
             "</a>");

    //derive draft
    elseif ($content->test_derive_draft($index))
        echo("<a href=\"javascript:dd($_index);\" " .
                "title=\"" . CMS_L_IFC_CONTENT_053 . "\">" .
             image("content/button_derive_draft") .
             "</a>");

    else
        echo(image("content/button_authorize_disabled"));

    //publish
    echo($content->test_publish($index) ?
         "<a href=\"javascript:p1($_index);\" " .
            "title=\"" . CMS_L_IFC_CONTENT_040 . "\">" .
         image("content/button_publish") .
         "</a>" :
         image("content/button_publish_disabled"));

    //withdraw
    echo($content->test_withdraw($index) ?
         "<a href=\"javascript:w($_index);\" " .
            "title=\"" . CMS_L_IFC_CONTENT_054 . "\">" .
         image("content/button_withdraw") .
         "</a>" :
         image("content/button_withdraw_disabled"));

    //separator
    echo(image("content/separator"));

    //rss
    echo($content->test_channel_set($index) ?
         "<a href=\"javascript:r2($_index);\" " .
            "title=\"" . CMS_L_IFC_CONTENT_061 . "\">" .
         image("content/button_rss") .
         "</a>" :
         image("content/button_rss_disabled"));

    //flags
    echo($content->test_flag_set($index) ?
         "<a href=\"javascript:f($_index);\" " .
            "title=\"" . CMS_L_IFC_CONTENT_140 . "\">" .
         image("content/button_flag") .
         "</a>" :
         image("content/button_flag_disabled"));

    //separator
    echo(image("content/separator"));

    //schedule
    echo($resultrow["#schedule"] ?
         "<a href=\"javascript:s2($_index);\" " .
            "title=\"" . CMS_L_IFC_CONTENT_120 . "\">" .
         image("content/button_schedule") .
         "</a>" :
         image("content/button_schedule_disabled"));

    //separator
    echo(image("content/separator"));

    //duplicate
    echo($content->test_duplicate($index) ?
         "<a href=\"javascript:d2($_index);\" " .
            "title=\"" . CMS_L_IFC_CONTENT_051 . "\">" .
         image("content/button_duplicate") .
         "</a>" :
         image("content/button_duplicate_disabled"));

    //copy
    echo($content->test_copy($index) ?
         "<a href=\"javascript:c($_index);\" " .
            "title=\"" . CMS_L_IFC_CONTENT_052 . "\">" .
         image("content/button_copy") .
         "</a>" :
         image("content/button_copy_disabled"));

    //separator
    echo(image("content/separator"));

    //send
    echo("<a href=\"javascript:s3($_index);\" " .
            "title=\"" . CMS_L_IFC_CONTENT_046 . "\">" .
         image("content/button_send") .
         "</a>" .

    "</td>");

//end of editing functions
//..................................................................................................

    //author
    if ($flag_author)
        echo("<td>" .
             x($resultrow[CMS_DB_CONTENT_AUTHOR]) .
             "</td>");

    //date
    echo("<td>" .
         friendly_date($resultrow[CMS_DB_CONTENT_WRITER_TIME]) .
         "</td>");

//extra column
//..................................................................................................

    if ($flag_extra)
    {
        $extra_value = l($resultrow[CMS_DB_CONTENT_EXTRA_VALUE]);
        $extra_type  = $resultrow[CMS_DB_CONTENT_EXTRA_TYPE];
        $extra_color = $resultrow[CMS_DB_CONTENT_EXTRA_COLOR];

        echo("<td>" .
             "<div class=\"content-extra\"" .
             (nstre($extra_color) ? " style=\"BACKGROUND-COLOR:" . x($extra_color) . "\">" : ">"));

        //process extra value
        switch ($extra_type)
        {
        case "href":

            if (! preg_match("/^([^:]+):(.+)$/s", $extra_value, $value)) break;

            $_type = $value[1];
            $value = $value[2];

            switch ($_type)
            {
            case "directory":

                $data  = new data("#system/directory");
                $url   = cms_url(["ifc_page"   => "directory",
                                  "object"     => $value,
                                  "ifc_option" => "external"]);
                $value = l($data->get($value, "name"));
                break;

            case "content":

                $url   = translate_url("$_type://$value");
                $value = CMS_L_IFC_CONTENT_145 . ": " . x($value);
                break;

            default:
            case "address":

                $url = translate_url($value);
                break;
            };

            echo("<a href=\"" . x($url) . "\" " .
                    "onclick=\"return e2(this.href);\">" .
                 x($value) .
                 "</a>");
            break;

        case "plugin":

            $url = translate_url($extra_value);
            echo("<a href=\"" . x($url) . "\" " .
                    "onclick=\"return e2(this.href);\">" .
                 x($extra_value) .
                 "</a>");
            break;

        case "text":

            echo(parse_text($extra_value));
            break;

        case "image":
        case "thumbnail":

            $data  = new data("#system/image");
            $url   = cms_url(["ifc_page"   => "image",
                              "object"     => $extra_value,
                              "ifc_option" => "external"]);
            $value = translate_url($extra_value);
            echo("<a href=\"" . x($url) . "\" " .
                    "onclick=\"return e2(this.href);\">" .
                 image($value, 75, 75) .
                 "</a>");
            break;

        case "download":
        case "media":
        case "template":

            $data  = new data("#system/$extra_type");
            $url   = cms_url(["ifc_page"   => $extra_type,
                              "object"     => $extra_value,
                              "ifc_option" => "external"]);
            $value = l($data->get($extra_value, "name"));
            echo("<a href=\"" . x($url) . "\" " .
                    "onclick=\"return e2(this.href);\">" .
                 x($value) .
                 "</a>");
            break;

        case "value":
        case "repeat":
        case "switch":
        default:

            echo(nl2br(x($extra_value)));
            break;
        };

        echo("</div>" .
             "</td>");
    };

    echo("</tr>");
};

//selection control
//..................................................................................................

echo("<tr>" .
     "<td colspan=\"" . (5 + $flag_sender + $flag_author + $flag_extra) . "\" " .
         "class=\"select\" " .
         "style=\"TEXT-ALIGN:right\">");
$ifc->set(CMS_L_ALL,    "button", "javascript:ifc_list_activate();");
$ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert();");
$ifc->set(CMS_L_NONE,   "button", "javascript:ifc_list_deactivate();");
echo("</td>" .
     "</tr>");
ifc_table_close();

$ifc->close();