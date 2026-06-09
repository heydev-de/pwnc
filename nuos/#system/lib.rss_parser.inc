<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   RSS PARSER CLASS
//==================================================================================================

class rss_parser_attribute_list
{

//methods
//--------------------------------------------------------------------------------------------------

    function add($name,
                 $value)
    {
        $this->{$name} = new rss_parser_attribute($value);
    }

//--------------------------------------------------------------------------------------------------

    function __get($name)
    {
        if (isset($this->{$name})) return $this->{$name};
        return NULL;
    }

//--------------------------------------------------------------------------------------------------

    function __toString()
    {
        return "";
    }
};

class rss_parser_attribute
{

//properties
//--------------------------------------------------------------------------------------------------

    public $value = "";

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($value)
    {
        $this->value = $value;
    }

//methods
//--------------------------------------------------------------------------------------------------

    function __get($name)
    {
        return $this->value;
    }

//--------------------------------------------------------------------------------------------------

    function __toString()
    {
        return $this->value;
    }
};

class rss_parser_node
{

//properties
//--------------------------------------------------------------------------------------------------

    public $_parent = NULL;
    public $_name   = "";
    public $_data   = "";
    public $_attrib = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($name = "")
    {
        $this->_name   = $name;
        $this->_attrib = new rss_parser_attribute_list();
    }

//methods
//--------------------------------------------------------------------------------------------------

    function add_attribute($name,
                           $value)
    {
        $this->_attrib->add($name, $value);
    }

//--------------------------------------------------------------------------------------------------

    function add_node($name)
    {
        $node          = new rss_parser_node($name);
        $node->_parent = $this;

        if (isset($this->{$name}))
        {
            if (is_array($this->{$name}))
                $this->{$name}[] = $node;
            else
                $this->{$name}   = [$this->{$name}, $node];

            $buffer = end($this->{$name});
            return $buffer;
        };

        $this->{$name} = $node;
        return $this->{$name};
    }

//--------------------------------------------------------------------------------------------------

    function get_parent()
    {
        return $this->_parent;
    }

//--------------------------------------------------------------------------------------------------

    function get_path()
    {
        $buffer = $this->_name;
        $object = $this;

        while ($object = $object->_parent)
        {
            if ($object->_name !== "")
                $buffer = $object->_name . ":$buffer";
        };

        return $buffer;
    }

//--------------------------------------------------------------------------------------------------

    function __get($name)
    {
        if (isset($this->{$name})) return $this->{$name};
        return NULL;
    }

//--------------------------------------------------------------------------------------------------

    function __toString()
    {
        return $this->_data;
    }
};

class rss_parser
{

//properties
//--------------------------------------------------------------------------------------------------

    public $enable_html_filter           = FALSE;
    public $max_text_length              = 100;
    public $max_description_length       = 400; //html filter only
    public $max_item_number              = 20;

    public $show_channel                 = TRUE;
    public $show_channel_link            = TRUE;
    public $show_channel_image           = TRUE;
    public $channel_image_max_width      = 120;
    public $channel_image_max_height     = 120;
    public $show_channel_title           = TRUE;
    public $show_channel_category        = TRUE;
    public $show_channel_description     = TRUE;
    public $show_channel_pub_date        = TRUE;
    public $show_channel_last_build_date = TRUE;
    public $show_channel_copyright       = TRUE;
    public $show_channel_managing_editor = TRUE;
    public $show_channel_web_master      = TRUE;
    public $show_channel_generator       = TRUE;

    public $show_item                    = TRUE;
    public $show_item_link               = TRUE;
    public $show_item_title              = TRUE;
    public $show_item_category           = TRUE;
    public $show_item_enclosure          = TRUE;
    public $show_item_description        = TRUE;
    public $show_item_pub_date           = TRUE;
    public $show_item_author             = TRUE;
    public $show_item_source             = TRUE;
    public $show_item_comments           = TRUE;
    public $show_item_guid               = TRUE;

    public $parser                       = NULL;
    public $data                         = NULL;
    public $node                         = NULL;
    public $stack                        = [];
    public $structure                    = [
        "rss",
        "rss.version",
        "rss:channel",
        "rss:channel:title",
        "rss:channel:link",
        "rss:channel:description",
        "rss:channel:language",
        "rss:channel:copyright",
        "rss:channel:managingEditor",
        "rss:channel:webMaster",
        "rss:channel:pubDate",
        "rss:channel:lastBuildDate",
        "rss:channel:category",
        "rss:channel:generator",
        "rss:channel:docs",
        "rss:channel:cloud",
        "rss:channel:cloud.domain",
        "rss:channel:cloud.port",
        "rss:channel:cloud.path",
        "rss:channel:cloud.registerProcedure",
        "rss:channel:cloud.protocol",
        "rss:channel:ttl",
        "rss:channel:image",
        "rss:channel:image:url",
        "rss:channel:image:title",
        "rss:channel:image:link",
        "rss:channel:image:width",
        "rss:channel:image:height",
        "rss:channel:image:description",
        "rss:channel:rating",
        "rss:channel:textInput",
        "rss:channel:textInput:title",
        "rss:channel:textInput:description",
        "rss:channel:textInput:name",
        "rss:channel:textInput:link",
        "rss:channel:skipHours",
        "rss:channel:skipDays",
        "rss:channel:item",
        "rss:channel:item:title",
        "rss:channel:item:link",
        "rss:channel:item:description",
        "rss:channel:item:author",
        "rss:channel:item:category",
        "rss:channel:item:category.domain",
        "rss:channel:item:comments",
        "rss:channel:item:enclosure",
        "rss:channel:item:enclosure.url",
        "rss:channel:item:enclosure.length",
        "rss:channel:item:enclosure.type",
        "rss:channel:item:guid",
        "rss:channel:item:guid.isPermaLink",
        "rss:channel:item:pubDate",
        "rss:channel:item:source",
        "rss:channel:item:source.url"];

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct()
    {
        //create and setup xml parser
        $this->parser = xml_parser_create("UTF-8");
        xml_set_object($this->parser, $this);
        xml_set_element_handler($this->parser, "_start", "_end");
        xml_set_character_data_handler($this->parser, "_data");
        xml_parser_set_option($this->parser, XML_OPTION_CASE_FOLDING, FALSE);
    }

//methods
//--------------------------------------------------------------------------------------------------

    function parse($source)
    {
        if (! cms_load("http")) return FALSE;

        //caching
        $cache_key  = "rss_parser.$source";
        $cache_time = cms_cache_time($cache_key);
        $time       = time();

        if (($cache_time !== FALSE) && ($cache_time > ($time - 60))) //60 seconds delay
        {
            $this->data  = unserialize(cms_cache_notouch($cache_key));
            $this->node  = $this->data;
            $this->stack = [];
            return TRUE;
        };

        //initialize
        $this->data  = new rss_parser_node();
        $this->node  = $this->data;
        $this->stack = [];

        //open source
        if (! $hfile = http_fopen($source)) return FALSE;

        //process data
        while ($buffer = stream_get_contents($hfile, CMS_HTTP_SIZE_CHUNK))
        {
            if (! xml_parse($this->parser, $buffer, feof($hfile)))
            {
                //parse error
                fclose($hfile);
                return FALSE;
            };
        };

        //close source
        fclose($hfile);

        //cache permanently
        cms_cache($cache_key, serialize($this->data), TRUE);

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function display()
    {
        if (
             (! isset($this->data->rss))
             ||
             nstreq($this->data->rss->_attrib->version, "2.0")
           )
            return FALSE;

        echo("<section class=\"rss\">");

        if ($this->show_channel && isset($this->data->rss->channel))
        {
            $channel = $this->data->rss->channel;
            echo("<div class=\"rss-channel p\">");

//..................................................................................................

            //link
            $link = NULL;

            if (
                 $this->show_channel_link
                 &&
                 ($buffer = $channel->link)
               )
            {
                if (! is_array($buffer)) $buffer = [$buffer];

                foreach ($buffer AS $value)
                {
                    if (stre($value)) continue;
                    $link = $value;
                    break;
                };
            };

//..................................................................................................

            //image
            if (
                 $this->show_channel_image
                 &&
                 ($buffer = is_array($channel->image) ?
                            reset($channel->image) :
                            $channel->image)
                 &&
                 $buffer->url
                 &&
                 $buffer->title
                 &&
                 $buffer->link
               )
            {
                echo("<div class=\"rss-channel-image\">");

                if ($link !== NULL)
                    echo("<a href=\"" . x(cms_url($link)) . "\"" .
                             ($buffer->description ? " title=\"" . x($buffer->description) . "\"" : "") . ">" .
                         image((string)$buffer->url,
                               $this->channel_image_max_width,
                               $this->channel_image_max_height,
                               $buffer->title) .
                         "</a>");
                else
                    echo(image((string)$buffer->url,
                               $this->channel_image_max_width,
                               $this->channel_image_max_height,
                               $buffer->title));

                echo("</div>"); //rss-channel-image
            };

//..................................................................................................

            echo("<div class=\"rss-channel-data\">");

//..................................................................................................

            //title
            if (
                 $this->show_channel_title
                 &&
                 nstre($buffer = is_array($channel->title) ?
                                 reset($channel->title) :
                                 $channel->title)
               )
            {
                echo("<h2 class=\"rss-channel-title\">");

                if ($link !== NULL)
                    echo("<a href=\"" . x(cms_url($link)) . "\">" .
                         $this->ft($channel->title) .
                         "</a>");
                else
                    echo($this->ft($channel->title));

                echo("</h2>"); //rss-channel-title
            };

//..................................................................................................

            //category
            if (
                 $this->show_channel_category
                 &&
                 ($buffer = $channel->category)
               )
            {
                if (! is_array($buffer)) $buffer = [$buffer];
                $flag = TRUE;

                foreach ($buffer AS $value)
                {
                    if (stre($value)) continue;

                    if ($flag)
                    {
                        echo("<div class=\"rss-channel-category\">");
                        $flag = FALSE;
                    }
                    else
                    {
                        echo(" // ");
                    };

                    if ($_buffer = $value->_attrib->domain)
                        echo("<a href=\"" . x(cms_url($_buffer)) . "\">" .
                             $this->ft($value) .
                             "</a>");
                    else
                        echo($this->ft($value));
                };

                if (! $flag) echo("</div>"); //rss-channel-category
            };

//..................................................................................................

            //description
            if (
                 $this->show_channel_description
                 &&
                 nstre($buffer = is_array($channel->description) ?
                                 reset($channel->description) :
                                 $channel->description)
               )
            {
                echo("<div class=\"rss-channel-description p\">" .

                     $this->fd($buffer) .

                     "</div>"); //rss-channel-description
            };

//..................................................................................................

            //publication date
            if (
                 $this->show_channel_pub_date
                 &&
                 nstre($buffer = is_array($channel->pubDate) ?
                                 reset($channel->pubDate) :
                                 $channel->pubDate)
               )
            {
                $_buffer = strtotime($buffer);

                echo("<div class=\"rss-channel-pub-date\">" .

                     CMS_L_RSS_PARSER_001 . ": " .
                     "<time datetime=\"" . x(date("c", $_buffer)) . "\">" .
                     x(friendly_date($_buffer)) .
                     "</time>" .

                     "</div>"); //rss-channel-pub-date
            };

//..................................................................................................

            //last build date
            if (
                 $this->show_channel_last_build_date
                 &&
                 nstre($buffer = is_array($channel->lastBuildDate) ?
                                 reset($channel->lastBuildDate) :
                                 $channel->lastBuildDate)
               )
            {
                $_buffer = strtotime($buffer);

                echo("<div class=\"rss-channel-last-build-date\">" .

                     CMS_L_RSS_PARSER_002 . ": " .
                     "<time datetime=\"" . x(date("c", $_buffer)) . "\">" .
                     x(friendly_date($_buffer)) .
                     "</time>" .

                     "</div>"); //rss-channel-last-build-date
            };

//..................................................................................................

            //copyright
            if (
                 $this->show_channel_copyright
                 &&
                 nstre($buffer = is_array($channel->copyright) ?
                                 reset($channel->copyright) :
                                 $channel->copyright)
               )
            {
                $buffer = $this->ft($buffer);
                echo("<div class=\"rss-channel-copyright\">" .
                     (nstreq(utf8_substr($buffer, 0, 1), "©") ? "© " : "") . $buffer .
                     "</div>"); //rss-channel-copyright
            };

//..................................................................................................

            //managing Editor
            if (
                 $this->show_channel_managing_editor
                 &&
                 nstre($buffer = is_array($channel->managingEditor) ?
                                 reset($channel->managingEditor) :
                                 $channel->managingEditor)
               )
            {
                echo("<div class=\"rss-channel-managing-editor\">" .

                     "<a href=\"mailto:" . x($buffer) . "\">" .
                     CMS_L_RSS_PARSER_004 . ": " . $this->ft($buffer) .
                     "</a>" .

                     "</div>"); //rss-channel-managing-editor
            };

//..................................................................................................

            //web master
            if (
                 $this->show_channel_web_master
                 &&
                 nstre($buffer = is_array($channel->webMaster) ?
                                 reset($channel->webMaster) :
                                 $channel->webMaster)
               )
            {
                echo("<div class=\"rss-channel-web-master\">" .

                     "<a href=\"mailto:" . x($buffer) . "\">" .
                     CMS_L_RSS_PARSER_005 . ": " . $this->ft($buffer) .
                     "</a>" .

                     "</div>"); //rss-channel-web-master
            };

//..................................................................................................

            //generator
            if (
                 $this->show_channel_generator
                 &&
                 nstre($buffer = is_array($channel->generator) ?
                                 reset($channel->generator) :
                                 $channel->generator)
               )
            {
                echo("<div class=\"rss-channel-generator\">" .

                     CMS_L_RSS_PARSER_003 . ": " . $this->ft($buffer) .

                     "</div>"); //rss-channel-generator
            };

//..................................................................................................

            echo("</div>" . //rss-channel-data
                 "</div>"); //rss-channel
        };

        if ($this->show_item && isset($this->data->rss->channel->item))
        {
            $item = $this->data->rss->channel->item;
            $item = is_array($item) ? $item : [$item];
            $i    = 0;

            echo("<div class=\"rss-list\">");

            foreach ($item AS $value)
            {
                if ($i++ >= $this->max_item_number) break;

                echo("<article class=\"rss-item p\">");

//..................................................................................................

                //link
                $link = NULL;

                if (
                     $this->show_item_link
                     &&
                     nstre($buffer = is_array($value->link) ?
                                     reset($value->link) :
                                     $value->link)
                   )
                    $link = $buffer;

//..................................................................................................

                //title
                if (
                     $this->show_item_title
                     &&
                     nstre($buffer = is_array($value->title) ?
                                     reset($value->title) :
                                     $value->title)
                   )
                {
                    echo("<h3 class=\"rss-item-title\">");

                    if ($this->show_item_link && $value->link)
                        echo("<a href=\"" . x(cms_url($value->link)) . "\">" .
                             $this->ft($value->title) .
                             "</a>");
                    else
                        echo($this->ft($value->title));

                    echo("</h3>"); //rss-item-title
                };

//..................................................................................................

                //category
                if (
                     $this->show_item_category
                     &&
                     ($buffer = $value->category)
                   )
                {
                    if (! is_array($buffer)) $buffer = [$buffer];
                    $flag = TRUE;

                    foreach ($buffer AS $_value)
                    {
                        if (stre($_value)) continue;

                        if ($flag)
                        {
                            echo("<div class=\"rss-item-category\">");
                            $flag = FALSE;
                        }
                        else
                        {
                            echo(" // ");
                        };

                        if ($_buffer = $_value->_attrib->domain)
                            echo("<a href=\"" . x(cms_url($_buffer)) . "\">" .
                                 $this->ft($_value) .
                                 "</a>");
                        else
                            echo($this->ft($_value));
                    };

                    if (! $flag) echo("</div>"); //rss-item-category
                };

//..................................................................................................

                //enclosure
                if (
                     $this->show_item_enclosure
                     &&
                     ($buffer = is_array($value->enclosure) ?
                                reset($value->enclosure) :
                                $value->enclosure)
                     &&
                     $buffer->_attrib->url
                     &&
                     $buffer->_attrib->length
                     &&
                     $buffer->_attrib->type
                     &&
                     cms_load("media_type")
                   )
                {
                    $media_type = new media_type();

                    echo("<div class=\"rss-item-enclosure\">" .

                         $media_type->parse(NULL,                                //index
                                            x($buffer->_attrib->url),            //url
                                            "rss_item_enclosure_" . unique_id(), //id
                                            x($buffer->_attrib->type)) .         //type

                         "</div>"); //rss-item-enclosure
                };

//..................................................................................................

                //description
                if (
                     $this->show_item_description
                     &&
                     nstre($buffer = is_array($value->description) ?
                                     reset($value->description) :
                                     $value->description)
                   )
                {
                    echo("<div class=\"rss-item-description p\">" .

                         $this->fd($buffer) .

                         "</div>"); //rss-item-description
                };

//..................................................................................................

                //publication date
                if (
                     $this->show_item_pub_date
                     &&
                     nstre($buffer = is_array($value->pubDate) ?
                                     reset($value->pubDate) :
                                     $value->pubDate)
                   )
                {
                    $_buffer = strtotime($buffer);

                    echo("<div class=\"rss-item-pub-date\">" .

                         "<time datetime=\"" . x(date("c", $_buffer)) . "\">" .
                         x(friendly_date($_buffer)) .
                         "</time>" .

                         "</div>"); //rss-item-pub-date
                };

//..................................................................................................

                //author
                if (
                     $this->show_item_author
                     &&
                     nstre($buffer = is_array($value->author) ?
                                     reset($value->author) :
                                     $value->author)
                   )
                {
                    echo("<div class=\"rss-item-author\">" .

                         "<a href=\"mailto:" . x($buffer) . "\">" .
                         CMS_L_RSS_PARSER_007 . ": " . $this->ft($buffer) .
                         "</a>" .

                         "</div>"); //rss-item-author
                };

//..................................................................................................

                //source
                if (
                     $this->show_item_source
                     &&
                     nstre($buffer = is_array($value->source) ?
                                     reset($value->source) :
                                     $value->source)
                   )
                {
                    echo("<div class=\"rss-item-source\">");

                    if ($_buffer = $buffer->_attrib->url)
                        echo("<a href=\"" . x(cms_url($_buffer)) . "\">" .
                             CMS_L_RSS_PARSER_008 . ": " . $this->ft($buffer) .
                             "</a>");
                    else
                        echo(CMS_L_RSS_PARSER_008 . ": " . $this->ft($buffer));

                    echo("</div>"); //rss-item-source
                };

//..................................................................................................

                //comments
                if (
                     $this->show_item_comments
                     &&
                     nstre($buffer = is_array($value->comments) ?
                                     reset($value->comments) :
                                     $value->comments)
                   )
                {
                    echo("<div class=\"rss-item-comments\">" .

                         "<a href=\"" . x(cms_url($buffer)) . "\">" .
                         CMS_L_RSS_PARSER_006 .
                         "</a>" .

                         "</div>"); //rss-item-comments
                };

//..................................................................................................

                //guid
                if (
                     $this->show_item_guid
                     &&
                     nstre($buffer = is_array($value->guid) ?
                                     reset($value->guid) :
                                     $value->guid)
                   )
                {
                    echo("<div class=\"rss-item-guid\">");

                    if (streq($buffer->_attrib->isPermaLink, "false"))
                        echo(CMS_L_RSS_PARSER_010 . ": " . $this->ft($buffer));
                    else
                        echo("<a href=\"" . x(cms_url($buffer)) . "\">" .
                             CMS_L_RSS_PARSER_009 .
                             "</a>");

                    echo("</div>"); //rss-item-guid
                };

                echo("</article>"); //rss-item
            };

            echo("</div>"); //rss-list
        };

        echo("</section>"); //rss

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function fd($value) //filter description
    {
        if (! $this->enable_html_filter) return $value; //filter disabled

        //filter html markup
        $value = htmltoplain($value);

        //limit string length
        if (strlen($value) > $this->max_description_length)
            $value = utf8_substr($value, 0, $this->max_description_length - 2) . " …";

        //escape xml special characters
        $value = x($value);

        return $value;
    }

//--------------------------------------------------------------------------------------------------

    function ft($value) //filter text
    {
        $value = html_entity_decode($value, ENT_QUOTES | ENT_HTML5, "UTF-8");

        //limit string length
        if (strlen($value) > $this->max_text_length)
            $value = utf8_substr($value, 0, $this->max_text_length - 2) . " …";

        //escape xml special characters
        $value = x($value);

        return $value;
    }

//--------------------------------------------------------------------------------------------------

    function _start($parser,
                    $tag,
                    $attribute)
    {
        //current path
        $path  = $this->node->get_path();
        $path .= ($path === "") ? $tag : ":$tag";

        //ignore invalid or misplaced tag
        if (! in_array($path, $this->structure)) return;

        //create and select node
        $this->node = $this->node->add_node($tag);

        //add attributes
        foreach ($attribute AS $key => $value)
        {
            //validate attribute
            if (in_array("$path.$key", $this->structure))
                $this->node->add_attribute($key, $value);
        };

        //open tag
        array_push($this->stack, $tag);
    }

//--------------------------------------------------------------------------------------------------

    function _data($parser,
                   $data)
    {
        //set value
        $this->node->_data .= $data;
    }

//--------------------------------------------------------------------------------------------------

    function _end($parser,
                  $tag)
    {
        //ignore invalid or misplaced closing tag
        if (nstreq($tag, end($this->stack))) return;

        //select parent node
        $this->node = $this->node->get_parent();

        //close tag
        array_pop($this->stack);
    }
};