<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   MIME CLASS
//==================================================================================================

//related functions
//--------------------------------------------------------------------------------------------------

function mime_extract_rfc2822_address($string)
{
    //lexer
    $array  = [];
    $buffer = "";
    $char   = "";

    for ($i = 0, $c = strlen($string); $i < $c; $i++)
    {
        $_char = $char;
        $char  = str_replace(["\t", "\n", "\r"], " ", $string[$i]);
        if ("$_char$char" === "  ") continue;

        switch ($char)
        {
        case "(": //comment block
//..................................................................................................

            $depth = 1;

            while (($depth > 0) && (++$i < $c))
            {
                $_char = ("$_char$char" === "\\\\") ? "" : $char;
                $char  = $string[$i];

                if ($_char !== "\\")
                {
                    switch ($char)
                    {
                    case "(": $depth++; break;
                    case ")": $depth--; break;
                    };
                };
            };

            if ($buffer !== "")
            {
                $array[] = $buffer;
                $buffer  = "";
            };

            continue 2;

        case "<": //address block
//..................................................................................................

            if ($buffer !== "")
            {
                $array[] = $buffer;
                $buffer  = "";
            };

            $flag = FALSE;

            for ($i++; $i < $c; $i++)
            {
                $_char = ("$_char$char" === "\\\\") ? "" : $char;
                $char  = $string[$i];

                switch ($char)
                {
                case ">":

                    if ($flag) break;
                    break 2;

                case "\"":

                    $flag = $flag ? ($_char === "\\") : TRUE;
                    break;
                };

                if (($char !== "\\") || ($_char === "\\")) $buffer .= $char;
            };

            $array[] = $buffer;
            $array[] = ">";
            $buffer  = "";
            continue 2;

        case "[": //domain literal block
//..................................................................................................

            $buffer .= "[";

            for ($i++; $i < $c; $i++)
            {
                $_char = ("$_char$char" === "\\\\") ? "" : $char;
                $char  = $string[$i];

                if (($_char !== "\\") && ($char === "]"))  break;
                if (($char !== "\\") || ($_char === "\\")) $buffer .= $char;
            };

            break;

        case ":": //group start
        case ";": //group end
        case ",": //address delimiter
//..................................................................................................

            if ($buffer !== "")
            {
                $array[] = $buffer;
                $buffer  = "";
            };

            $array[] = $char;
            continue 2;

        case " ":
//..................................................................................................

            if ($buffer !== "")
            {
                $array[] = $buffer;
                $buffer  = "";
            };

            continue 2;

        case "\"": //quoted string block
//..................................................................................................

            for ($i++; $i < $c; $i++)
            {
                $_char = ("$_char$char" === "\\\\") ? "" : $char;
                $char  = $string[$i];

                if (($_char !== "\\") && ($char === "\""))
                {
                    $array[] = $buffer;
                    $buffer  = "";
                    break;
                };

                if (($char !== "\\") || ($_char === "\\")) $buffer .= $char;
            };

            continue 2;
        };

        $buffer .= $char;
    };

    if ($buffer !== "") $array[] = $buffer;

//..................................................................................................

    //parser
    $return = [];
    $buffer = [];
    $group  = "";
    $name   = "";
    $email  = FALSE;
    $flag   = FALSE;

    foreach ($array AS $key => $value)
    {
        switch ($value[0] ?? "")
        {
        case ">":
//..................................................................................................

            $email = array_pop($buffer);
            continue 2;

        case ":":
//..................................................................................................

            $group  = implode(" ", $buffer);
            $buffer = [];
            continue 2;

        case ";":
//..................................................................................................

            $flag = TRUE;
            continue 2;

        case ",":
//..................................................................................................

            if ($email === FALSE) $email = array_pop($buffer);

            if (isset($email))
            {
                $name = implode(" ", $buffer);
                $return[$group][$email] = $name;
            };

            if ($flag)
            {
                $group = "";
                $flag  = FALSE;
            };

            $email  = FALSE;
            $name   = "";
            $buffer = [];
            continue 2;
        };

        $buffer[] = $value;
    };

    if ($email === FALSE) $email = array_pop($buffer);

    if (isset($email))
    {
        $name = implode(" ", $buffer);
        $return[$group][$email] = $name;
    };

    return $return; //[group] => [address] => [name]
};

//--------------------------------------------------------------------------------------------------

function mime_extract_rfc2822_header($string)
{
    //lexer
    $array  = [];
    $buffer = "";
    $char   = "";

    for ($i = 0, $c = strlen($string); $i < $c; $i++)
    {
        $_char = $char;
        $char  = str_replace(["\t", "\n", "\r"], " ", $string[$i]);
        if (streq("$_char$char", "  ")) continue;

        switch ($char)
        {
        case "(": //comment block
//..................................................................................................

            $depth = 1;

            while (($depth > 0) && (++$i < $c))
            {
                $_char = ("$_char$char" === "\\\\") ? "" : $char;
                $char  = $string[$i];

                if (nstreq($_char, "\\"))
                {
                    switch ($char)
                    {
                    case "(": $depth++; break;
                    case ")": $depth--; break;
                    };
                };
            };

            if ($buffer)
            {
                $array[] = $buffer;
                $buffer  = "";
            };

            continue 2;

        case ";": //parameter delimiter
        case "=": //value delimiter
//..................................................................................................

            if ($buffer !== "")
            {
                $array[] = $buffer;
                $buffer  = "";
            };

            $array[] = $char;
            continue 2;

        case " ":
//..................................................................................................

            if ($buffer !== "")
            {
                $array[] = $buffer;
                $buffer  = "";
            };

            continue 2;

        case "\"": //quoted string block
//..................................................................................................

            for ($i++; $i < $c; $i++)
            {
                $_char = ("$_char$char" === "\\\\") ? "" : $char;
                $char  = $string[$i];

                if (($_char !== "\\") && ($char === "\""))
                {
                    $array[] = $buffer;
                    $buffer  = "";
                    break;
                };

                if (($char !== "\\") || ($_char === "\\")) $buffer .= $char;
            };

            continue 2;
        };

        $buffer .= $char;
    };

    if ($buffer) $array[] = $buffer;

//..................................................................................................

    //parser
    $return = [];
    $buffer = [];
    $param  = "#value";

    foreach ($array AS $key => $value)
    {
        switch ($value[0] ?? "")
        {
        case ";":
//..................................................................................................

            $return[$param] = implode(" ", $buffer);
            $buffer         = [];
            $param          = "";
            continue 2;

        case "=":
//..................................................................................................

            $param  = strtolower(implode(" ", $buffer));
            $buffer = [];
            continue 2;
        };

        $buffer[] = $value;
    };

    $return[$param] = implode(" ", $buffer);

    return $return; //["#value"] => [value]
                    //[param]    => [value]
};

//--------------------------------------------------------------------------------------------------

function mime_encode_header($value)
{
    //ascii without control characters
    if (preg_match("/^[\\x20-\\x7E]*$/", $value))
        return wordwrap($value, 76, "\r\n ", TRUE);

    //quoted printable encode value, including space character

    $value = preg_replace_callback(
    //                             !"#  $    =    ?    @    [\]   ^    `    {|}~
        "/[\\x00-\\x08\\x0A-\\x1F\\x20-\\x24\\x3D\\x3F\\x40\\x5B-\\x5E\\x60\\x7B-\\xFF]/",
        function($value) { return sprintf("=%02X", ord($value[0])); },
        $value);

    //fold value, preferably break at space character
    $break   = "?=\r\n =?utf-8?q?";
    $search  = "/.{1,60}(?:[^=]{0,3})?(?:=20|$)|.{60}(?:[^=]{0,3})?/Su";
    $replace = "$0" . $break;
    $value   = "=?utf-8?q?" . preg_replace($search, $replace, $value) . "?=";
    $value   = substr($value, 0, -strlen($break));
    return $value;
};

define("CMS_MIME_TYPE_TEXT",                  "text");
define("CMS_MIME_TYPE_IMAGE",                 "image");
define("CMS_MIME_TYPE_AUDIO",                 "audio");
define("CMS_MIME_TYPE_VIDEO",                 "video");
define("CMS_MIME_TYPE_APPLICATION",           "application");
define("CMS_MIME_TYPE_MESSAGE_RFC822",        "message/rfc822");
define("CMS_MIME_TYPE_MESSAGE_PARTIAL",       "message/partial");
define("CMS_MIME_TYPE_MESSAGE_EXTERNAL_BODY", "message/external-body");

define("CMS_MIME_CHARSET_UTF_8",              "utf-8");
define("CMS_MIME_CHARSET_ISO_8859_1",         "iso-8859-1");
define("CMS_MIME_CHARSET_ISO_8859_2",         "iso-8859-2");
define("CMS_MIME_CHARSET_ISO_8859_3",         "iso-8859-3");
define("CMS_MIME_CHARSET_ISO_8859_4",         "iso-8859-4");
define("CMS_MIME_CHARSET_ISO_8859_5",         "iso-8859-5");
define("CMS_MIME_CHARSET_ISO_8859_6",         "iso-8859-6");
define("CMS_MIME_CHARSET_ISO_8859_7",         "iso-8859-7");
define("CMS_MIME_CHARSET_ISO_8859_8",         "iso-8859-8");
define("CMS_MIME_CHARSET_ISO_8859_9",         "iso-8859-9");
define("CMS_MIME_CHARSET_ISO_8859_10",        "iso-8859-10");
define("CMS_MIME_CHARSET_ISO_8859_11",        "iso-8859-11");
define("CMS_MIME_CHARSET_ISO_8859_13",        "iso-8859-13");
define("CMS_MIME_CHARSET_ISO_8859_14",        "iso-8859-14");
define("CMS_MIME_CHARSET_ISO_8859_15",        "iso-8859-15");
define("CMS_MIME_CHARSET_ISO_8859_16",        "iso-8859-16");
define("CMS_MIME_CHARSET_US_ASCII",           "us-ascii");

define("CMS_MIME_ENCODING_7BIT",              "7bit");
define("CMS_MIME_ENCODING_8BIT",              "8bit");
define("CMS_MIME_ENCODING_BASE64",            "base64");
define("CMS_MIME_ENCODING_BINARY",            "binary");
define("CMS_MIME_ENCODING_QUOTED_PRINTABLE",  "quoted-printable");

define("CMS_MIME_DISPOSITION_INLINE",         "inline");
define("CMS_MIME_DISPOSITION_ATTACHMENT",     "attachment");

class mime
{

//properties
//--------------------------------------------------------------------------------------------------

    public $data = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($name = NULL)
    {
        $this->data = new data($name);
    }

//methods
//--------------------------------------------------------------------------------------------------

    function add_text($subject    = NULL,
                      $body       = NULL,
                      $subtype    = "plain",
                      $charset    = CMS_MIME_CHARSET_UTF_8,
                      $parent_key = NULL,
                      $content_id = NULL)
    {
        return $this->add_part(
            CMS_MIME_TYPE_TEXT,                 //content_type
            $subtype,                           //subtype
            $subject,                           //subject
            $body,                              //body
            CMS_MIME_ENCODING_QUOTED_PRINTABLE, //content_transfer_encoding
            $charset,                           //charset
            CMS_MIME_DISPOSITION_INLINE,        //content_disposition
            NULL,                               //name
            $parent_key,                        //parent_key
            $content_id);                       //content_id
    }

//--------------------------------------------------------------------------------------------------

    function add_file($file,
                      $filename   = NULL,
                      $attachment = TRUE,
                      $parent_key = NULL,
                      $content_id = NULL)
    {
        if (! $filename) $filename = basename($file);

        preg_match("/^([^\/]+)(?:\/(.+))?$/", get_mime_type($filename), $type);

        return $this->add_part(
            $type[1],                            //content_type
            $type[2],                            //subtype
            NULL,                                //subject
            read_file($file),                    //body
            ($type[1] === CMS_MIME_TYPE_TEXT) ?  //content_transfer_encoding
            CMS_MIME_ENCODING_QUOTED_PRINTABLE :
            CMS_MIME_ENCODING_BASE64,
            NULL,                                //charset
            $attachment ?                        //content_disposition
            CMS_MIME_DISPOSITION_ATTACHMENT :
            CMS_MIME_DISPOSITION_INLINE,
            $filename,                           //name
            $parent_key,                         //parent_key
            $content_id);                        //content_id
    }

//--------------------------------------------------------------------------------------------------

    function add_part($content_type              = CMS_MIME_TYPE_TEXT,
                      $subtype                   = "plain",
                      $subject                   = NULL,
                      $body                      = NULL,
                      $content_transfer_encoding = CMS_MIME_ENCODING_QUOTED_PRINTABLE,
                      $charset                   = CMS_MIME_CHARSET_UTF_8,
                      $content_disposition       = CMS_MIME_DISPOSITION_INLINE,
                      $name                      = NULL,
                      $parent_key                = NULL,
                      $content_id                = NULL)
    {
        //check parent type
        if ($parent_key && (! $this->data->is_container($parent_key)))
            return FALSE;

        if ($subtype)
            $content_type        .= "/$subtype";

        if ($charset)
        {
            $charset              = addcslashes($charset, "\"\\");
            $content_type        .= "; charset=\"$charset\"";
        };

        if ($name)
        {
            $name                 = addcslashes($name, "\"\\");
            $content_type        .= "; name=\"$name\"";
            $content_disposition .= "; filename=\"$name\"";
        };

        if (stre($content_id)) $content_id = $this->content_id();

        $this->data->set_buffer(
            [["content-type"              => $content_type,
              "content-transfer-encoding" => $content_transfer_encoding,
              "content-disposition"       => $content_disposition,
              "content-id"                => $content_id,
              "subject"                   => $subject,
              "#body"                     => $body,
              "#size"                     => strlen($body)]]);

        return $this->data->insert($parent_key);
    }

//--------------------------------------------------------------------------------------------------

    function add_multipart($subtype    = "mixed",
                           $subject    = NULL,
                           $parent_key = NULL)
    {
        //check parent type
        if ($parent_key && (! $this->data->is_container($parent_key))) return FALSE;

        $boundary = "----=_" . unique_id() . "_" . unique_id() . "_" . unique_id() . "_" . unique_id() . "_=----";

        $this->data->set_buffer(
            [["content-type" => "multipart/$subtype; boundary=\"$boundary\"",
              "subject"      => $subject,
              "#body"        => "This is a multipart message in MIME format.",
              "#boundary"    => $boundary,
              "#type"        => "container"],
             ["#type"        => "/container"]]);

        return $this->data->insert($parent_key);
    }

//--------------------------------------------------------------------------------------------------

    function add_message($subtype    = "rfc822",
                         $data       = NULL,
                         $parent_key = NULL)
    {
        //check parent type
        if ($parent_key && (! $this->data->is_container($parent_key))) return FALSE;

        $this->data->set_buffer(array_merge(
            [["content-type"        => "message/$subtype",
              "content-disposition" => CMS_MIME_DISPOSITION_ATTACHMENT,
              "#type"               => "container"]],
            $data->data,
            [["#type"               => "/container"]]));

        return $this->data->insert($parent_key);
    }

//--------------------------------------------------------------------------------------------------

    function build($from     = NULL,
                   $to       = NULL,
                   $cc       = NULL,
                   $bcc      = NULL,
                   $reply_to = NULL)
    {
        //generate message id
        $message_id = $this->content_id();

        //set header data
        $key = $this->data->move("first");
        $this->data->set("1.0", $key, "mime-version");
        if (nstre($from))     $this->data->set($from,     $key, "from");
        if (nstre($to))       $this->data->set($to,       $key, "to");
        if (nstre($cc))       $this->data->set($cc,       $key, "cc");
        if (nstre($bcc))      $this->data->set($bcc,      $key, "bcc");
        if (nstre($reply_to)) $this->data->set($reply_to, $key, "reply-to");
        $this->data->set(date("r"),      $key, "date");
        $this->data->set($message_id,    $key, "message-id");
        $this->data->set(CMS_USER_AGENT, $key, "x-mailer");

        $buffer    = NULL;
        $boundary  = [0 => NULL];
        $_boundary = NULL;
        $i         = 0;

        while ($key = $this->data->move("next"))
        {
            $value                     = $this->data->get($key);
            $content_type              = $this->data->get($key, "content-type");
            $content_type              = mime_extract_rfc2822_header($content_type);
            $content_transfer_encoding = $this->data->get($key, "content-transfer-encoding");
            $content_transfer_encoding = mime_extract_rfc2822_header($content_transfer_encoding);
            $type                      = $this->data->get($key, "#type");

            if (nstreq($type, "/container"))
            {
                if ($_boundary) $buffer .= "--$_boundary\r\n";

                //header
                $array   = array_keys($this->data->data[$key]);

                foreach ($array AS $_key => $value)
                {
                    if ($value[0] !== "#")
                    {
                        $_value = $this->data->get($key, $value);

                        //workaround for misinterpreting clients
                        if (streq($value, "content-disposition") &&
                            streq($_value, CMS_MIME_DISPOSITION_INLINE))
                            continue;

                        //encode subject
                        if (streq($value, "subject"))
                            $_value = mime_encode_header($_value);

                        $buffer .= preg_replace_callback("/\b((?:id|mime)\b|\w)/i",
                                                         function($value) { return strtoupper($value[1]); },
                                                         $value) . ": " .
                                   "$_value\r\n";
                    };
                };

                $buffer .= "\r\n";

                if (! preg_match("/^message(?:\/.+)?$/i", $content_type["#value"]))
                {
                    //body
                    switch ($content_transfer_encoding["#value"])
                    {
                    case CMS_MIME_ENCODING_7BIT:

                        $buffer .= preg_replace("/[^\\x00-\\x7F]/", "", $this->data->get($key, "#body"));
                        break;

                    case CMS_MIME_ENCODING_BASE64:

                        $buffer .= chunk_split(base64_encode($this->data->get($key, "#body")));
                        break;

                    case CMS_MIME_ENCODING_QUOTED_PRINTABLE:

                        $buffer .= quoted_printable_encode($this->data->get($key, "#body"));
                        break;

                    default:

                        $buffer .= $this->data->get($key, "#body");
                        break;
                    };

                    $buffer .= "\r\n";
                };
            };

            //process nesting
            switch ($type)
            {
            case "container":

                $_boundary      = $this->data->get($key, "#boundary");
                $boundary[++$i] = $_boundary;
                break;

            case "/container":

                if ($_boundary) $buffer .= "--$_boundary--\r\n";

                $_boundary      = $boundary[--$i];
                break;
            };
        };

        //increase message count of current process
        $count = (int)cms_cache("mime.message_count");
        cms_cache("mime.message_count", $count + 1);

        return $buffer;
    }

//--------------------------------------------------------------------------------------------------

    function save($name = NULL)
    {
        $this->data->save($name);
    }

//--------------------------------------------------------------------------------------------------

    function content_id()
    {
        //get number of parts in current message
        $count = 0;

        $this->data->move("first");
        while ($key = $this->data->move("next"))
        {
            switch ($this->data->get($key, "#type"))
            {
            case "container":
            case "/container":

                break;

            default:

                $count++;
            };
        };

        //generate content id
        return
            "<" . $count .                               //number of parts in current message
            "." . (int)cms_cache("mime.message_count") . //message count of current process
            "." . getmypid() .                           //process id
            "." . time() .                               //unix timestamp
            "@" . CMS_DOMAIN . ">";                      //hostname
    }
};