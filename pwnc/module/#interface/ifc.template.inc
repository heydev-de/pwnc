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
if (! cms_load("template")) ifc_inactive($ifc_page);

//permission
ifc_permission([""                               => t("access"),
                CMS_TEMPLATE_PERMISSION_OPERATOR => t("operator")]);

//retrieve selected object from permanent cache
cms_cache_init($object, "template." . CMS_USER . ".object");

//initialize variables
init($language);

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "select":
//..................................................................................................

    $object = language_set($object, $ifc_param, $language);
    break;

case "select_language":
//..................................................................................................

    $language = $ifc_param;
    break;

case "display":
//..................................................................................................

    $template = new template();

    if (nstre($object))
    {
        cms_cache("template." . CMS_USER . ".object", $object, TRUE);
        template_preview($object);
    };

    $category = $template->data->get($object, "category") ?? "";
    cms_cache("template." . CMS_USER . ".category", $category, TRUE);
    exit();

case "_display":
//..................................................................................................

    if ($object) template_preview($object, TRUE, NULL, FALSE);
    exit();

case "add":
case "edit":
//..................................................................................................

    $template = new template();
    $_object  = language_get($object, $language, TRUE);

    if ($flag = streq(CMS_IFC_MESSAGE, "edit"))
    {
        if (blank($object)) break; //no selection
        $ifc_param1 = $template->data->get($_object, "name");
        $ifc_param2 = $template->data->get($_object, "page");
    };

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object"   => $object,
                    "language" => $language],
                   "_" . CMS_IFC_MESSAGE,
                   streq(CMS_IFC_MESSAGE, "add") ? t("command_add") : t("command_edit"));

    //name
    $ifc->set(t("name"), "text 40 40 bl", $ifc_param1 ?? NULL);

    //page
    $ifc->set(t("page_template"), "checkbox b", TRUE, isset($ifc_param2));

    echo("<hr>");

    //keep code
    $ifc->set($flag ? t("keep_source_code") : t("blank_source_code"),
                                      "radio b",        1, TRUE);

    //retrieve code from url
    echo("<label class=\"element\">");
    $ifc->set(NULL,                   "radio",          2);
    echo("<span>" . t("source_code_from_url") . "</span><br>");
    $ifc->set(NULL,                   "text 40 256 b:", $ifc_param4 ?? "https://");
    echo("</label><br>" .

    //upload code
         "<label class=\"element\">");
    $ifc->set(NULL,                   "radio",          3);
    echo("<span>" . t("source_code_from_file") . "</span><br>");
    $ifc->set(NULL,                   "file 40 b:");
    echo("</label>" .

         "<hr>");

    //category
    $ifc->set(t("category"),         "label");
    $ifc->set(template_get_select(), "list 40",        $template->data->get($_object, "category"));

    $ifc->close();

case "_add":
case "_edit":
//..................................................................................................

    $template = new template();
    $_object  = language_get($object, $language, TRUE);
    $code     = FALSE;

    switch ($ifc_param3)
    {
    case 1: //empty template / keep template

        $code = streq(CMS_IFC_MESSAGE, "_edit") ?
                read_file(CMS_DATA_PATH . "#template/$_object.htm") : "";
        break;

    case 2: //read from address

        $code = template_read_plugin($ifc_param4);

        if (stre($ifc_param1))
        {
            $ifc_param4 = analyze_url($ifc_param4);
            $ifc_param1 = $ifc_param4["basename"];
        };
        break;

    case 3: //read from file

        if ($ifc_file1_name && is_file($ifc_file1))
        {
            $code = read_file($ifc_file1);
            unlink($ifc_file1);

            if (stre($ifc_param1)) $ifc_param1 = $ifc_file1_name;
        };
        break;
    };

    if (streq(CMS_IFC_MESSAGE, "_add"))
    {
        if ($_object = $template->add($ifc_param1,        //name
                                      $ifc_param5,        //category
                                      isset($ifc_param2), //page
                                      $code))             //code
        {
            $object       = language_set($object, $_object, $language);
            $ifc_response = t("msg_done");
            break;
        };
    }
    else
    {
        if (
             $template->set($_object,           //index
                            $ifc_param1,        //name
                            $ifc_param5,        //category
                            isset($ifc_param2)) //page
             &&
             $template->set_code($_object, $code) //code
           )
        {
            $ifc_response = t("msg_done");
            break;
        };
    };

    $ifc_response = t("msg_error");
    break;

case "export":
//..................................................................................................

    if (
         cms_available("content")
         &&
         isset($content_index)
       )
    {
        //param
        $param = ["object"        => $object,
                  "language"      => $language,
                  "content_index" => $content_index];
        if (isset($content_range)) $param["content_range"] = $content_range;

        //display
        $ifc = new ifc($ifc_response,
                       $ifc_page,
                       TRUE,
                       $param,
                       "_export",
                       t("command_export"));

        //name
        $ifc->set(t("name"), "text 40 40 b");

        //category
        $ifc->set(t("category"),         "label");
        $ifc->set(template_get_select(), "list 40");

        $ifc->close();
    };

case "_export":
//..................................................................................................

    if (cms_load("content"))
    {
        $content = new content();

        if ($content->enabled)
        {
            $buffer   = content_template_export($content,                //content
                                                $content_index,          //index
                                                $content_range ?? NULL); //range
            $template = new template();

            if ($_object = $template->add($ifc_param1,             //name
                                          $ifc_param2,             //category
                                          ! isset($content_range), //page
                                          $buffer["code"],         //code
                                          $buffer["stylesheet"],   //stylesheet
                                          $buffer["javascript"]))  //javascript
            {
                $object       = language_set($object, $_object, $language);
                $ifc_response = t("msg_done");
                break;
            };
        };
    };

    $ifc_response = t("msg_error");
    break;

case "source":
case "_source":
case "__source":
case "___source":
//..................................................................................................

    $_object = language_get($object, $language, TRUE);
    if (stre($_object)) break; //no selection

    $template   = new template();
    $active_url = q(cms_url());

    switch (CMS_IFC_MESSAGE)
    {
    case "_source":
    case "__source":
    case "___source":

        if (
             $template->set_code($_object, $ifc_param1)
             &&
             $template->set_stylesheet($_object, $ifc_param2)
             &&
             $template->set_javascript($_object, $ifc_param3)
           )
            $ifc_response = t("msg_done");
        else
            $ifc_response = t("msg_error");

        //save and close
        if (streq(CMS_IFC_MESSAGE, "_source"))
            break 2; //exit source editor

        echo($ifc_response);
        exit();
    };

    //menu
    $menu = [t("command_confirm") . "|template/command_apply"
             => "javascript:template_source_save(\"_source\");",
             t("command_save") . "|template/command_save"
             => "javascript:template_source_save(\"__source\");",
             t("save_and_display") . "|template/command_show"
             => "javascript:template_source_save(\"___source\");"];

    $menu[t("command_cancel") . "|template/command_cancel"] = FALSE;

    //display
    $ifc = new ifc(
        $ifc_response,
        $ifc_page,
        $menu,
        ["object"   => $object,
         "language" => $language],
        "_source",
        t("source_code"));

    //code
    ifc_tab_open(l($template->data->get($_object, "name")));

    if (
         ifc_available("directory")
         &&
         cms_permission("directory", TRUE, NULL)
       )
    {
        //set up select action
        $buffer = "javascript:this.opener.textcontrol_set(" .
                  "'#l_ifc_param1'," .
                  "'#insert'," .
                  "'<CMS:href default=\"directory://%return%\" " .
                             "title=\"%title%\">" .
                  t("link") .
                  "</CMS:href>');";
        $ifc->set(t("add_link") . "|template/command_directory",
                  "button *",
                  "javascript:load_page('template','" .
                  q(cms_url(["ifc_page"          => "directory",
                             "ifc_option"        => "external",
                             "ifc_select"        => "object",
                             "ifc_select_action" => $buffer])) . "');");

        //set up select action for code
        $buffer = "javascript:this.opener.textcontrol_set(" .
                  "'#l_ifc_param1'," .
                  "'#insert'," .
                  "'<?=x(translate_url(\"directory://%return%\"))?>');";
        $ifc->set(t("add_link_php_code") . "|template/command_directory_code",
                  "button *",
                  "javascript:load_page('template','" .
                  q(cms_url(["ifc_page"          => "directory",
                             "ifc_option"        => "external",
                             "ifc_select"        => "object",
                             "ifc_select_action" => $buffer])) . "');");
    };

    if (
         ifc_available("image")
         &&
         cms_permission("image", TRUE, NULL)
       )
    {
        //set up select action
        $buffer = "javascript:this.opener.textcontrol_set(" .
                  "'#l_ifc_param1'," .
                  "'#insert'," .
                  "'<CMS:image default=\"%return%\" " .
                              "title=\"%title%\"/>');";
        $ifc->set(t("add_image") . "|template/command_image",
                  "button *",
                  "javascript:load_page('template','" .
                  q(cms_url(["ifc_page"          => "image",
                             "ifc_option"        => "external",
                             "ifc_select"        => "_object",
                             "ifc_select_action" => $buffer])) . "');");

        //set up select action for code
        $buffer = "javascript:this.opener.textcontrol_set(" .
                  "'#l_ifc_param1'," .
                  "'#insert'," .
                  "'<?=x(translate_url(\"%return%\"))?>');";
        $ifc->set(t("add_image_php_code") . "|template/command_image_code",
                  "button *",
                  "javascript:load_page('template','" .
                  q(cms_url(["ifc_page"          => "image",
                             "ifc_option"        => "external",
                             "ifc_select"        => "_object",
                             "ifc_select_action" => $buffer])) . "');");
    };

    if (
         ifc_available("media")
         &&
         cms_permission("media", TRUE, NULL)
       )
        //set up select action
        $buffer = "javascript:this.opener.textcontrol_set(" .
                  "'#l_ifc_param1'," .
                  "'#insert'," .
                  "'<CMS:media default=\"%return%\" " .
                              "title=\"%title%\"/>');";
        $ifc->set(t("add_media") . "|template/command_media",
                  "button *",
                  "javascript:load_page('template','" .
                  q(cms_url(["ifc_page"          => "media",
                             "ifc_option"        => "external",
                             "ifc_select"        => "_object",
                             "ifc_select_action" => $buffer])) . "');");

    if (
         ifc_available("download")
         &&
         cms_permission("download", TRUE, NULL)
       )
        $buffer = "javascript:this.opener.textcontrol_set(" .
                  "'#l_ifc_param1'," .
                  "'#insert'," .
                  "'<CMS:download default=\"%return%\" title=\"%title%\">" .
                  q(t("download")) .
                  "</CMS:download>');";
        $ifc->set(t("add_download") . "|template/command_download",
                  "button *",
                  "javascript:load_page('template','" .
                  q(cms_url(["ifc_page"          => "download",
                             "ifc_option"        => "external",
                             "ifc_select"        => "_object",
                             "ifc_select_action" => $buffer])) . "');");

    if (
         ifc_available("template")
         &&
         cms_permission("template", TRUE, NULL)
       )
    {
        //set up select action
        $buffer = "javascript:this.opener.textcontrol_set(" .
                  "'#l_ifc_param1'," .
                  "'#insert'," .
                  "'<CMS:template default=\"%return%\" title=\"%title%\"/>');";
        $ifc->set(t("add_template") . "|template/command_template",
                  "button *",
                  "javascript:load_page('template','" .
                  q(cms_url(["ifc_page"          => CMS_IFC_PAGE,
                             "ifc_option"        => "external",
                             "ifc_select"        => "_object",
                             "ifc_select_action" => $buffer])) . "');");

        //set up select action for stylesheet
        $buffer = "javascript:this.opener.textcontrol_set(" .
                  "'#l_ifc_param1'," .
                  "'#insert'," .
                  "'<CMS:stylesheet default=\"%return%\" title=\"%title%\"/>');";
        $ifc->set(t("add_stylesheet_from_template") . "|template/command_stylesheet",
                  "button *",
                  "javascript:load_page('template','" .
                  q(cms_url(["ifc_page"          => CMS_IFC_PAGE,
                             "ifc_option"        => "external",
                             "ifc_select"        => "_object",
                             "ifc_select_action" => $buffer])) . "');");

        //set up select action for javascript
        $buffer = "javascript:this.opener.textcontrol_set(" .
                  "'#l_ifc_param1'," .
                  "'#insert'," .
                  "'<CMS:javascript default=\"%return%\" title=\"%title%\"/>');";
        $ifc->set(t("add_javascript_from_template") . "|template/command_javascript",
                  "button *",
                  "javascript:load_page('template','" .
                  q(cms_url(["ifc_page"          => CMS_IFC_PAGE,
                             "ifc_option"        => "external",
                             "ifc_select"        => "_object",
                             "ifc_select_action" => $buffer])) . "');");
    };
    if (
         ifc_available("token")
         &&
         cms_permission("token", TRUE, NULL)
       )
        //set up select action
        $buffer = "javascript:this.opener.textcontrol_set(" .
                  "'#l_ifc_param1'," .
                  "'#token'," .
                  "'%return%');";
        $ifc->set(t("add_placeholder") . "|template/command_token",
                  "button *",
                  "javascript:load_page('template','" .
                  q(cms_url(["ifc_page"          => "token",
                             "ifc_option"        => "external",
                             "ifc_select"        => "token",
                             "ifc_select_action" => $buffer])) . "');");

    //tools

?><script>

function template_source_save(message)
{
    if (message !== "_source")
    {
        ifc_set("ifc_message", message);
        asr_form_post(document.forms["ifc"], template_source_save_callback);
    }
    else
    {
        ifc_post(message);
    };
};

function template_source_save_callback(value)
{
    ifc_response(value);

    //display after saving
    if (ifc_get("ifc_message") === "___source")
        load_page("template", "<?php
            echo(q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                            "ifc_message" => "_display",
                            "object"      => $object],
                           TRUE)));?>");
};

function template_source_insert(value)
{
    var code   = "";
    var action = "";

    switch (value)
    {
<?php

    if ($flag_page = $template->data->get($_object, "page"))
    {

?>
    case "":

        return;

    case "page":

        code   = "<?php echo(q(CMS_DOCTYPE_HTML));?>\n" +
                 "<html lang=\"<\?=x(CMS_LANGUAGE)?>\">\n" +
                 "<head>\n" +
                 "\n" +
                 "<CMS:head/>\n" +
                 "\n" +
                 "<meta name=\"robots\" content=\"<\?=x(CMS_CONTENT_META_ROBOTS)?>\">\n" +
                 "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n" +
                 "\n" +
                 "<\/head>\n" +
                 "<body><CMS:control/>\n" +
                 "\n" +
                 "<header id=\"head\">\n" +
                 "\n" +
                 "<\/header>\n" +
                 "\n" +
                 "<main id=\"body\">\n" +
                 "\n" +
                 "  <CMS:template id=\"tp1\"/>\n" +
                 "\n" +
                 "<\/main>\n" +
                 "\n" +
                 "<footer id=\"foot\">\n" +
                 "\n" +
                 "  <CMS:backlink/>\n" +
                 "\n" +
                 "<\/footer>\n" +
                 "\n" +
                 "<\/body>\n" +
                 "<\/html>";
        action = "#replace_all";
        break;

    case "head":

        code   = "<CMS:head [base=\"{url}\"]\n" +
                 "          [stylesheet=\"on|yes|true|off|no|false|{url}\"]\n" +
                 "          [title=\"{text}\"]/>\n";
        action = "#replace";
        break;

    case "control":

        code   = "<CMS:control/>\n";
        action = "#replace";
        break;
<?php

    };

?>
    case "menu":

        code   = "<CMS:menu [base=\"{id}\"]\n" +
                 "          [level=\"{number}|+{number}|-{number}\"]\n" +
                 "          [depth=\"off|no|false|{number}\"]\n" +
                 "          [filter=\"normal|open|active|active-open\"]\n" +
                 "          [show-icons=\"on|yes|true|off|no|false\"]\n" +
                 "          [show-images=\"on|yes|true|off|no|false\"]\n" +
                 "          [show-description=\"on|yes|true|off|no|false\"]\n" +
                 "          [show-hidden=\"on|yes|true|off|no|false\"]\n" +
                 "          [exclude=\"{id}[ {id}]\"]/>";
        action = "#replace";
        break;

    case "backlink":

        code   = "<CMS:backlink/>\n";
        action = "#replace";
        break;

    case "cblock":

        code   = "<CMS:cblock [idref=\"+{number}|-{number}|{alphanumeric_.}\"]\n" +
                 "            [regex=\"{pcre}\"]>\n" +
                 "%text%\n" +
                 "<\/CMS:cblock>\n";
        action = "#imbed";
        break;

    case "calt":

        code   = "<CMS:calt [idref=\"+{number}|-{number}|{alphanumeric_.}\"]>\n" +
                 "%text%\n" +
                 "<\/CMS:calt>\n";
        action = "#imbed";
        break;

    case "group":

        code   = "<CMS:group [id=\"+{number}|-{number}|{alphanumeric_.}\"]\n" +
                 "           [edit=\"on|yes|true|off|no|false\"]\n" +
                 "           [title=\"{text}\"]>\n" +
                 "%text%\n" +
                 "<\/CMS:group>\n";
        action = "#imbed";
        break;

    case "base":

        code   = "<CMS:base [id=\"+{number}|-{number}|{alphanumeric_.}\"]\n" +
                 "          [scope=\"local|global\"]>\n" +
                 "%text%\n" +
                 "<\/CMS:base>\n";
        action = "#imbed";
        break;

    case "namespace":

        code   = "<CMS:namespace [id=\"+{number}|-{number}|{alphanumeric_.}\"]\n" +
                 "               [auto-id=\"contain\"]>\n" +
                 "%text%\n" +
                 "<\/CMS:namespace>\n";
        action = "#imbed";
        break;

    case "repeat":

        code   = "<CMS:repeat [id=\"+{number}|-{number}|{alphanumeric_.}\"]\n" +
                 "            [default=\"{number}\"]\n" +
                 "            [edit=\"on|yes|true|off|no|false\"]\n" +
                 "            [title=\"{text}\"]>\n" +
                 "%text%\n" +
                 "<\/CMS:repeat>\n";
        action = "#imbed";
        break;

    case "shift":

        code   = "<CMS:shift [id=\"+{number}|-{number}|{alphanumeric_.}\"]\n" +
                 "           [default=\"{number}\"]\n" +
                 "           [edit=\"on|yes|true|off|no|false\"]\n" +
                 "           [title=\"{text}\"]>\n" +
                 "%text%\n" +
                 "<\/CMS:shift>\n";
        action = "#imbed";
        break;

    case "template":

        code   = "<CMS:template [id=\"+{number}|-{number}|{alphanumeric_.}\"]\n" +
                 "              [default=\"{identifier}\"]\n" +
                 "              [edit=\"on|yes|true|off|no|false\"]\n" +
                 "              [title=\"{text}\"]/>\n";
        action = "#replace";
        break;

    case "stylesheet":

        code   = "<CMS:stylesheet default=\"{identifier}\"/>\n";
        action = "#replace";
        break;

    case "javascript":

        code   = "<CMS:javascript default=\"{identifier}\"/>\n";
        action = "#replace";
        break;

    case "text":

        code   = "<CMS:text [id=\"+{number}|-{number}|{alphanumeric_.}\"]\n" +
                 "          [default=\"{text}\"]\n" +
                 "          [edit=\"on|yes|true|off|no|false\"]\n" +
                 "          [title=\"{text}\"]/>\n";
        action = "#replace";
        break;

    case "value":

        code   = "<CMS:value [id=\"+{number}|-{number}|{alphanumeric_.}\"]\n" +
                 "           [default=\"{text}\"]\n" +
                 "           [regex=\"{text}\"]\n" +
                 "           [edit=\"on|yes|true|off|no|false\"]\n" +
                 "           [title=\"{text}\"]/>\n";
        action = "#replace";
        break;

    case "switch":

        code   = "<CMS:switch [id=\"+{number}|-{number}|{alphanumeric_.}\"]\n" +
                 "            [default=\"{text}\"]\n" +
                 "            [edit=\"on|yes|true|off|no|false\"]\n" +
                 "            [title=\"{text}\"]/>\n";
        action = "#replace";
        break;

    case "href":

        code   = "<CMS:href [id=\"+{number}|-{number}|{alphanumeric_.}\"]\n" +
                 "          [_id=\"{text}\"]\n" +
                 "          [default=\"{url}\"]\n" +
                 "          [target=\"{identifier}\"]\n" +
                 "          [rel=\"{text}\"]\n" +
                 "          [edit=\"on|yes|true|off|no|false\"]\n" +
                 "          [title=\"{text}\"]\n" +
                 "          [class=\"{text}\"]\n" +
                 "          [style=\"{text}\"]>\n" +
                 "%text%\n" +
                 "<\/CMS:href>\n";
        action = "#imbed";
        break;

    case "image":

        code   = "<CMS:image [id=\"+{number}|-{number}|{alphanumeric_.}\"]\n" +
                 "           [_id=\"{text}\"]\n" +
                 "           [default=\"{url}\"]\n" +
                 "           [width=\"{width}[%]\"]\n" +
                 "           [height=\"{height}[%]\"]\n" +
                 "           [alt=\"{text}\"]\n" +
                 "           [edit=\"on|yes|true|off|no|false\"]\n" +
                 "           [title=\"{text}\"]\n" +
                 "           [class=\"{text}\"]\n" +
                 "           [style=\"{text}\"]/>\n";
        action = "#replace";
        break;

    case "thumbnail":

        code   = "<CMS:thumbnail [id=\"+{number}|-{number}|{alphanumeric_.}\"]\n" +
                 "               [_id=\"{text}\"]\n" +
                 "               [default=\"{url}\"]\n" +
                 "               [twidth=\"{width}[%]\"]\n" +
                 "               [theight=\"{height}[%]\"]\n" +
                 "               [width=\"{width}[%]\"]\n" +
                 "               [height=\"{height}[%]\"]\n" +
                 "               [alt=\"{text}\"]\n" +
                 "               [edit=\"on|yes|true|off|no|false\"]\n" +
                 "               [title=\"{text}\"]\n" +
                 "               [class=\"{text}\"]\n" +
                 "               [style=\"{text}\"]/>\n";
        action = "#replace";
        break;

    case "media":

        code   = "<CMS:media [id=\"+{number}|-{number}|{alphanumeric_.}\"]\n" +
                 "           [_id=\"{text}\"]\n" +
                 "           [default=\"{url}\"]\n" +
                 "           [width=\"{width}[%]\"]\n" +
                 "           [height=\"{height}[%]\"]\n" +
                 "           [alt=\"{text}\"]\n" +
                 "           [edit=\"on|yes|true|off|no|false\"]\n" +
                 "           [title=\"{text}\"]\n" +
                 "           [class=\"{text}\"]\n" +
                 "           [style=\"{text}\"]/>\n";
        action = "#replace";
        break;

    case "download":

        code   = "<CMS:download [id=\"+{number}|-{number}|{alphanumeric_.}\"]\n" +
                 "              [_id=\"{text}\"]\n" +
                 "              [default=\"{identifier}\"]\n" +
                 "              [edit=\"on|yes|true|off|no|false\"]\n" +
                 "              [title=\"{text}\"]\n" +
                 "              [class=\"{text}\"]\n" +
                 "              [style=\"{text}\"]>\n" +
                 "%text%\n" +
                 "<\/CMS:download>\n";
        action = "#imbed";
        break;

    case "plugin":

        code   = "<CMS:plugin [id=\"+{number}|-{number}|{alphanumeric_.}\"]\n" +
                 "            [default=\"{url}\"]\n" +
                 "            [edit=\"on|yes|true|off|no|false\"]\n" +
                 "            [title=\"{text}\"]/>\n";
        action = "#replace";
        break;

    case "nocache":

        code   = "<CMS:nocache>\n" +
                 "%text%\n" +
                 "<\/CMS:nocache>\n";
        action = "#imbed";
        break;

    case "edit":

        code   = "<CMS:edit [option=\"href plugin text value\n" +
                 "                   download image thumbnail\n" +
                 "                   media template group\n" +
                 "                   repeat shift switch\"]>\n" +
                 "%text%\n" +
                 "<\/CMS:edit>\n";
        action = "#imbed";
        break;

    case "noedit":

        code   = "<CMS:noedit>\n" +
                 "%text%\n" +
                 "<\/CMS:noedit>\n";
        action = "#imbed";
        break;

    case "comment":

        code   = "<!--\n" +
                 "\n" +
                 "%text%\n" +
                 "\n" +
                 "-->\n";
        action = "#imbed";
        break;

    case "php":

        code   = "<\?php\n" +
                 "\n" +
                 "%text%\n" +
                 "\n" +
                 "?>";
        action = "#imbed";
        break;

    case "include":

        code   = "<\?php\n" +
                 "\n" +
                 "include(CMS_ROOT_PATH . \"path/to/filename.inc\");\n" +
                 "\n" +
                 "?>\n";
        action = "#replace";
        break;

    case "echo":

        code   = "<\?=" +
                 "%text%" +
                 "?>";
        action = "#imbed";
        break;

    default:

        code   = value;
        action = "#replace";
        break;
    };

    textcontrol_set("#l_ifc_param1", action, code);
    event.target.selectedIndex = 0; //reset
};

</script><?php

    //prefabs
    echo("<select onchange=\"template_source_insert(this.value);\">" .
         "<option value=\"\" hidden> ".    t("add") . "</option>" .
         "<optgroup label=\"" .            t("cms_elements") . "\">" .
         ($flag_page ?
         "<option value=\"page\">" .       t("page_frame") . "</option>" .
         "<option value=\"head\">" .       t("header_data") . "</option>" .
         "<option value=\"control\">" .    t("control_pad") . "</option>" : "") .
         "<option value=\"menu\">" .       t("navigation") . "</option>" .
         "<option value=\"backlink\">" .   t("backlink") . "</option>" .
         "<option value=\"cblock\">" .     t("conditional_block") . "</option>" .
         "<option value=\"calt\">" .       t("conditional_alternative") . "</option>" .
         "<option value=\"group\">" .      t("group") . "</option>" .
         "<option value=\"base\">" .       t("base_namespace") . "</option>" .
         "<option value=\"namespace\">" .  t("namespace") . "</option>" .
         "<option value=\"repeat\">" .     t("repeat") . "</option>" .
         "<option value=\"shift\">" .      t("position_shift") . "</option>" .
         "<option value=\"template\">" .   t("template") . "</option>" .
         "<option value=\"stylesheet\">" . t("stylesheet") . "</option>" .
         "<option value=\"javascript\">" . t("javascript") . "</option>" .
         "<option value=\"text\">" .       t("text") . "</option>" .
         "<option value=\"value\">" .      t("value") . "</option>" .
         "<option value=\"switch\">" .     t("switch") . "</option>" .
         "<option value=\"href\">" .       t("link") . "</option>" .
         "<option value=\"image\">" .      t("image") . "</option>" .
         "<option value=\"thumbnail\">" .  t("thumbnail") . "</option>" .
         "<option value=\"media\">" .      t("media") . "</option>" .
         "<option value=\"download\">" .   t("download") . "</option>" .
         "<option value=\"plugin\">" .     t("plugin") . "</option>" .
         "<option value=\"nocache\">" .    t("no_caching") . "</option>" .
         "<option value=\"edit\">" .       t("editing_only") . "</option>" .
         "<option value=\"noedit\">" .     t("output_only") . "</option>" .
         "</optgroup>" .
         "<optgroup label=\"" .            t("html") . "\">" .
         "<option value=\"comment\">" .    t("comment") . "</option>" .
         "</optgroup>" .
         "<optgroup label=\"" .            t("php") . "\">" .
         "<option value=\"php\">" .        t("script") . "</option>" .
         "<option value=\"include\">" .    t("php_include") . "</option>" .
         "<option value=\"echo\">" .       t("php_output") . "</option>" .
         "<option value=\"\$cms_template_document\">\$cms_template_document</option>" .
         "<option value=\"\$cms_template_base_id\">\$cms_template_base_id</option>" .
         "<option value=\"\$cms_template_path_id\">\$cms_template_path_id</option>" .
         "<option value=\"\$cms_template_temp_id\">\$cms_template_temp_id</option>" .
         "<option value=\"\$cms_template_default\">\$cms_template_default</option>" .
         "</optgroup>" .
         "</select>");

    $ifc->set(NULL,
              "code 80x20 fl",
              $template->get_code($_object, FALSE));

    //stylesheet
    ifc_tab_next(t("css"));

    $ifc->set(NULL,
              "code_style 80x20 fl",
              $template->get_stylesheet($_object, FALSE));

    //javascript
    ifc_tab_next(t("javascript"));

    $ifc->set(NULL,
              "code_script 80x20 fl",
              $template->get_javascript($_object, FALSE));

    ifc_tab_close();

    $ifc->close();

case "delete":
//..................................................................................................

    if (blank($_object)) break; //no selection

    $value      = reset($_object);
    $template   = new template();
    $category   = $template->data->get($value, "category") ?? "";
    $flag_error = TRUE;

    //delete selected templates
    foreach ($_object AS $value)
        $flag_error &= ! $template->delete($value);

    //remove invalid templates from selection
    $array = explode(",", CMS_LANGUAGE_ENABLED);
    if (CMS_LANGUAGE_ENABLED) $array[] = FALSE;

    foreach ($array AS $value)
    {
        if (($_object = language_get($object, $value, TRUE)) === NULL) continue;
        if ($template->data->get($_object) === NULL)                      continue;

        //active language
        if (streq($value, $language))
        {
            $template->data->move("first");
            $_object = $template->data->seek(["category" => $category]);
            $object  = language_set($object, $_object, $value);
        }

        //inactive language(s)
        else
        {
            $object  = language_set($object, NULL, $value);
        };
    };

    $ifc_response = $flag_error ? t("msg_error") : t("msg_done");
    break;

case "category_rename":
//..................................................................................................

    $_object = language_get($object, $language, TRUE);
    if (stre($_object)) break; //no selection

    $template = new template();
    $category = $template->data->get($_object, "category") ?? "";

    //display
    $ifc = new ifc($ifc_response,
                   $ifc_page,
                   TRUE,
                   ["object"   => $object,
                    "language" => $language],
                   "_category_rename",
                   t("rename_category"));

    //category
    $ifc->set(t("name"), "text 40 40 b", $category);

    $ifc->close();

case "_category_rename":
//..................................................................................................

    $template = new template();
    $_object  = language_get($object, $language, TRUE);
    $category = $template->data->get($_object, "category") ?? "";

    $template->data->move("first");
    while ($key = $template->data->move("next"))
    {
        $_category = $template->data->get($key, "category") ?? "";
        if (streq($_category, $category))
            $template->data->set($ifc_param1, $key, "category");
    };

    $ifc_response = $template->data->save() ? t("msg_done") : t("msg_error");
    break;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

$template = new template();
$_object  = language_get($object, $language, TRUE);
$array    = template_get_array(CMS_IFC_SELECT ? FALSE : NULL);

//get category from object
if (nstre($_object) && in_array_recursive($_object, $array))
{
    $category = $template->data->get($_object, "category") ?? "";
}

//get object from category
else
{
    $category = cms_cache("template." . CMS_USER . ".category") ?? "";
    if (! isset($array[$category])) $category = "";

    if (is_array($array[$category]))
    {
        $_object = current($array[$category]);
        $object  = language_set($object, $_object, $language);
    };
};

//menu
$menu = NULL;
if (CMS_IFC_SELECT && $_object)
    $menu[t("command_insert") . "|template/command_apply"] = "javascript:ifc_return();";
if ($template->operator)
{
    $menu[t("command_add") . "|template/command_add"] = "add";
    if ($_object)
    {
        $menu[t("command_edit") .    "|template/command_edit"]   = "edit";
        $menu[t("source_code") .     "|template/command_code"]   = "source";
        $menu[t("command_delete") .  "|template/command_delete"] = "#delete";
        $menu[t("rename_category") . "|template/command_rename"] = "category_rename";
    };
};

//display
$ifc = new ifc($ifc_response,
               $ifc_page,
               $menu,
               ["object"   => $object,
                "language" => $language]);

//javascript

?><script>

function template_select(value)
{
    var language = "<?php echo(q($language));?>";
    if (ifc_language_get(ifc_get("object"), language) === value) return;
    ifc_set("object", ifc_language_set(ifc_get("object"), value, language));
    document.getElementById("template-display").src = "<?php
        echo(str_replace("%replace%",
                         "\"+encodeURIComponent(value)+\"",
                         q(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                    "ifc_message" => "display",
                                    "object"      => "\x1B%replace%"]))));?>";
};

</script><?php

ifc_table_open("fullsize");
echo("<colgroup>" .
     "<col style=\"WIDTH:0\">" .
     "<col>" .
     "</colgroup>" .

     "<tr>" .
     "<td>");

//category
$ifc->set(t("category"), "description b");
echo("<select class=\"list\" " .
             "onchange=\"ifc_post('select',this.value);\">");
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
    echo("<br>");

    $ifc->set(t("template"), "description b");
    echo("<div id=\"_object\" " .
              "data-size=\"14\" " .
              "class=\"select list\" " .
              "data-onchange=\"template_select(this.value);\" " .
              "ondblclick=\"ifc_return();\">");
    foreach ($array[$category] AS $key => $value)
    {
        $title = $template->data->get($value, "page") ?
                 " title=\"" . t("page_template") . "\"" : "";
        echo("<label$title>" .
             "<input name=\"_object[]\" " .
                    "type=\"checkbox\" " .
                    "value=\"" . x($value) . "\"" .
                    (streq($value, $_object) ? " checked>" : ">") .
             x($key) .
             "</label>");
    };
    echo("</div>" .
         "<div style=\"TEXT-ALIGN:right\">");
    $ifc->set(t("all"),    "button", "javascript:ifc_list_activate(\"_object\");");
    $ifc->set(t("invert"), "button", "javascript:ifc_list_invert(\"_object\");");
    $ifc->set(t("none"),   "button", "javascript:ifc_list_deactivate(\"_object\");");
    echo("</div>" .
         jscript("ifc_custom_select(\"_object\");"));
};

//language
if (CMS_IFC_SELECT && CMS_LANGUAGE_ENABLED)
{
    $map       = new map("#system/language.image");
    $_language = explode(",", CMS_LANGUAGE_ENABLED);

    echo("<div class=\"language\">" .
         "<a href=\"javascript:ifc_post('select_language','');\" " .
            "title=\"" . t("language_default") . "\" " .
            "class=\"language-" . (stre($language) ? "on" : "off") . "\">" .
         image("language/default", 16, 12) .
         "</a>");

    foreach ($_language AS $value)
        echo("<a href=\"javascript:ifc_post('select_language','" . qx($value) . "');\" " .
                "title=\"" . x($value) . "\" " .
                "class=\"language-" . (streq($language, $value) ? "on" : "off") . "\">" .
             image(((($image = $map->get_value($value)) !== NULL) &&
                   is_file(CMS_DATA_PATH . "language/$image")) ?
                   CMS_DATA_URL . "language/$image" :
                   "language/default",
                   16, 12) .
             "</a>");

    echo("</div>");
};
echo("</td>" .

//display template
     "<td class=\"iframe\">" .
     "<iframe id=\"template-display\" " .
             "src=\"" . x(cms_url(["ifc_page"    => CMS_IFC_PAGE,
                                   "ifc_message" => "display",
                                   "object"      => $_object])) . "\" " .
             "sandbox=\"allow-same-origin allow-scripts\">" .
     t("preview_unavailable") .
     "</iframe>" .
     "</td>" .
     "</tr>");
ifc_table_close();

$ifc->close();