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

define("CMS_MIME_TYPE_TEXT",                  "text");
define("CMS_MIME_TYPE_IMAGE",                 "image");
define("CMS_MIME_TYPE_AUDIO",                 "audio");
define("CMS_MIME_TYPE_VIDEO",                 "video");
define("CMS_MIME_TYPE_APPLICATION",           "application");
define("CMS_MIME_TYPE_MESSAGE_RFC822",        "message/rfc822");
define("CMS_MIME_TYPE_MESSAGE_PARTIAL",       "message/partial");
define("CMS_MIME_TYPE_MESSAGE_EXTERNAL_BODY", "message/external-body");

define("CMS_MIME_ENCODING_7BIT",              "7bit");
define("CMS_MIME_ENCODING_8BIT",              "8bit");
define("CMS_MIME_ENCODING_BASE64",            "base64");
define("CMS_MIME_ENCODING_BINARY",            "binary");
define("CMS_MIME_ENCODING_QUOTED_PRINTABLE",  "quoted-printable");

define("CMS_MIME_DISPOSITION_INLINE",         "inline");
define("CMS_MIME_DISPOSITION_ATTACHMENT",     "attachment");

define("CMS_MIME_HEADER_TYPE_ADDRESS",        1);
define("CMS_MIME_HEADER_TYPE_MIME",           2);
define("CMS_MIME_HEADER_TYPE_IDENTIFICATION", 3);
define("CMS_MIME_HEADER_TYPE_INFORMATIONAL",  4);

define("CMS_MIME_RFC2231_PARAM_CONTINUATION", FALSE); //not supported by many clients

//related functions
//--------------------------------------------------------------------------------------------------

function mime_quote_rfc2045($string)
{
    $string = preg_replace("/[\\x00-\\x1F\\x7F]/", "", $string);
    return preg_match("/^[^()<>@,;:\\\\\"\/[\]?= ]+$/", $string) ?
           $string : "\"" . addcslashes($string, "\"\\") . "\"";
};

//--------------------------------------------------------------------------------------------------

function mime_rfc2047_decode($string)
{
    $offset = -1;

    return preg_replace_callback(
        "/(\s*)=\?([^\?]+)\?(q|b)\?([^\?]*)\?=/i",
        function($match) use (&$offset)
        {
            $string = $match[4][0];

            switch ($match[3][0])
            {
            case "B":
            case "b":
//..................................................................................................
                $string = base64_decode($string);
                break;

            case "Q":
            case "q":
//..................................................................................................
                $string = quoted_printable_decode($string);
                $string = str_replace("_", " ", $string);
                break;
            };

            //convert and normalize
            $charset = strtolower($match[2][0]);
            if (nstreq($charset, "utf-8"))
            {
                $string = utf8_convert($string, $charset);
                $string = utf8_normalize($string);
            };

            $_offset = $match[0][1];
            $space   = ($offset === $_offset) ? "" : $match[1][0];
            $offset  = $_offset + strlen($match[0][0]);

            return $space . $string;
        },
        $string,
        -1,
        $count,
        PREG_OFFSET_CAPTURE);
};

//--------------------------------------------------------------------------------------------------

function mime_rfc2047_encode($string,
                             &$filling_level = NULL)
{
    $filling_level ??= 0;

    //no encoding required
    if (! preg_match("/[\\x00-\\x19;=\\x7F-\\xFF]| {2,}/", $string))
    {
        //apply header folding
        $n      = max($filling_level - 1, 0);
        $string = str_repeat("_", $n) . $string;
        $string = wordwrap($string, 77, "\r\n ", TRUE);
        $string = substr($string, $n);
        $n      = strlen($string);

        //compute last line length
        $filling_level = (($pos = strrpos($string, "\r\n ")) !== FALSE) ?
                         $n - ($pos + 2) : $filling_level + $n;

        return $string;
    };

    //byte encoding lookup table
    static $map = [];

    //quoted printable encode utf8 char
    $encode = function($char) use (&$map)
    {
        $buffer = "";

        for ($i = 0, $c = strlen($char); $i < $c; $i++)
        {
            $byte = $char[$i];
            $n    = ord($byte);

            //space character
            if ($n === 0x20)
                $buffer .= "_";

            //use self-extending byte encoding lookup table
            elseif (FALSE !== ($map[$n] ??= (preg_match("/[\\x00-\\x19;=?_\\x7F-\\xFF]/", $byte) ?
                                             sprintf("=%02X", $n) : FALSE)))
                $buffer .= $map[$n];

            //literal
            else
                $buffer .= $byte;
        };

        return $buffer;
    };

    //create utf8 char array
    $array       = preg_split("//u", $string, -1, PREG_SPLIT_NO_EMPTY);
    $result      = [];
    $buffer      = "";
    $length      = 0;
    $wrap_length = strlen("=?utf-8?q??=");
    $max_length  = min(78 - $filling_level, 75) - $wrap_length;
    $first_line  = TRUE;

    //process utf8 chars
    foreach ($array AS $value)
    {
        $char = $encode($value);
        $n    = strlen($char);

        if (($length + $n) > $max_length)
        {
            //flush buffer
            $result[] = $buffer;

            //reset buffer
            $buffer = "";
            $length = 0;

            //set up for following lines
            if ($first_line)
            {
                $first_line = FALSE;
                $max_length = 75 - $wrap_length;
            };
        };

        $buffer .= $char;
        $length += $n;
    };

    //flush buffer
    if ($buffer !== "") $result[] = $buffer;

    //empty result
    if (! $result) return "";

    //first line empty
    $buffer = "";
    if ($result[0] === "")
    {
        $buffer     .= "\r\n ";
        $result      = array_slice($result, 1);
        $first_line  = FALSE;
    };

    //return last line length
    $filling_level = $length + $wrap_length + ($first_line ? $filling_level : 1);

    //concatenate
    $buffer .= "=?utf-8?q?" . implode("?=\r\n =?utf-8?q?", $result) . "?=";
    return $buffer;
};

//--------------------------------------------------------------------------------------------------

function mime_unquote_rfc2822($string)
{
    $result = [];
    $buffer = "";
    $char   = "";
    $quote  = FALSE;
    $escape = FALSE;

    $token = function() use (&$result, &$buffer, &$quote)
    {
        $result[] = $quote ? $buffer : mime_rfc2047_decode($buffer);
        $buffer   = "";
    };

    for ($i = 0, $c = strlen($string); $i < $c; $i++)
    {
        $char = $string[$i];

        switch ($char)
        {
        case "\\": //escape
//..................................................................................................

            if (! $escape && $quote)
            {
                $escape = TRUE;
                continue 2;
            };
            break;

        case "\"": //quote
//..................................................................................................

            if (! $escape && $quote)
            {
                $token();
                $quote = FALSE;
                continue 2;
            };

            if ($quote) break;

            $token();
            $quote = TRUE;
            continue 2;
        };

        $escape  = FALSE;
        $buffer .= $char;
    };

    $token();
    return implode("", $result);
};

//--------------------------------------------------------------------------------------------------

function mime_quote_rfc2822($string)
{
    return preg_match("/[\"(),\.:;<=>?@[\\\\\]]|^ |  | $/", $string) ?
           "\"" . addcslashes($string, "\"\\") . "\"" : $string;
};

//--------------------------------------------------------------------------------------------------

function mime_extract_rfc2822_address($string)
{
    //lexer
    $array  = [];
    $buffer = "";
    $char   = "";

    $quote   = FALSE;
    $address = FALSE;
    $literal = FALSE;
    $escape  = FALSE;
    $comment = 0;

    $token = function($token = "") use (&$array, &$buffer)
    {
        if ($token !== "")      $array[] = $token;
        elseif ($buffer !== "") $array[] = $buffer;
        $buffer = "";
    };

    $string = str_replace(["\t", "\n", "\r"], " ", $string);

    for ($i = 0, $c = strlen($string); $i < $c; $i++)
    {
        $_char = $char;
        $char  = $string[$i];

        switch ($char)
        {
        case "\\": //escape
//..................................................................................................

            if (! $escape && ($quote || $comment))
            {
                if ($quote) $buffer .= $char;
                $escape = TRUE;
                continue 2;
            };
            break;

        case "\"": //quote
//..................................................................................................

            if (! $escape && $quote)
            {
                $quote = FALSE;
                break;
            };

            if ($quote)   break;
            if ($literal) break;
            if ($comment) break;

            $quote = TRUE;
            break;

        case " ": //space
//..................................................................................................

            if ($quote)   break;
            if ($address) continue 2;
            if ($comment) break;

            if ($_char !== " ")
            {
                $token();
                $token($char);
            };
            continue 2;

        case "(": //comment start
//..................................................................................................

            if ($quote)   break;
            if ($literal) break;

            if (! $escape || ! $comment) $comment++;
            break;

        case ")": //comment end
//..................................................................................................

            if ($comment < 1) break;

            if (! $escape && ! --$comment)
            {
                $char = " ";
                continue 2;
            };
            break;

        case "<": //address start
//..................................................................................................

            if ($quote)   break;
            if ($address) break;
            if ($comment) break;

            $token();

            $address = TRUE;
            continue 2;

        case ">": //address end
//..................................................................................................

            if (! $address) break;

            $token();
            $token($char);

            $address = FALSE;
            continue 2;

        case "[": //literal start
//..................................................................................................

            if ($quote)   break;
            if ($comment) break;

            $literal = TRUE;
            break;

        case "]": //literal end
//..................................................................................................

            if (! $literal) break;

            $literal = FALSE;
            break;

        case ",": //address delimiter
        case ":": //group start
        case ";": //group end
//..................................................................................................

            if ($quote)   break;
            if ($address) break;
            if ($literal) break;
            if ($comment) break;

            $token();
            $token($char);
            continue 2;
        };

        $escape = FALSE;

        if ($comment) continue;

        $buffer .= $char;
    };

    $token();
    $token(";");

//..................................................................................................

    //parser
    $result = [];
    $buffer = [];
    $group  = "";
    $name   = "";
    $email  = FALSE;

    foreach ($array AS $key => $value)
    {
        switch ($value[0])
        {
        case " ": //space
//..................................................................................................

            continue 2;

        case ">": //address
//..................................................................................................

            $email = array_pop($buffer);
            continue 2;

        case ":": //group
//..................................................................................................

            $group  = mime_unquote_rfc2822(implode(" ", $buffer));
            $buffer = [];
            continue 2;

        case ",": //address delimiter
        case ";": //group end
//..................................................................................................

            if ($email === FALSE) $email = implode("",  $buffer);
            else                  $name  = implode(" ", $buffer);

            if (nstre($email)) $result[$group][$email] = mime_unquote_rfc2822($name);

            $email  = FALSE;
            $name   = "";
            $buffer = [];

            if ($value[0] === ";") $group = "";
            continue 2;
        };

        $buffer[] = $value;
    };

    return $result; //[group] => [address] => [name]
};

//--------------------------------------------------------------------------------------------------

function mime_build_rfc2822_address($array,
                                    &$filling_level = NULL)
{
    $result          = "";
    $filling_level ??= 0;

    //append token to buffer folding lines
    $append = function($token) use (&$result, &$filling_level)
    {
        $filling_level += $n = strlen($token);

        if ($filling_level > 78)
        {
            $result        = rtrim($result, " ") . "\r\n ";
            $filling_level = $n + 1;
        };

        $result .= $token;
    };

    //process groups
    $c = count($array);
    $i = 0;

    foreach ($array AS $group => $address)
    {
        if ($is_group = nstre($group))
            $append(mime_rfc2047_encode($group, $filling_level) . ": ");

        //process addresses
        $_c = count($address);
        $_i = 0;

        foreach ($address AS $email => $name)
        {
            if (nstre($name))
            {
                $append(mime_rfc2047_encode($name, $filling_level) . " ");
                $append("<$email>");
            }
            else
            {
                $append($email);
            };

            if (++$_i < $_c) $append(", ");
        };

        if ($is_group) $append(";");
        if (++$i < $c) $append(", ");
    };

    return rtrim($result, " ");
};

//--------------------------------------------------------------------------------------------------

function mime_extract_rfc2822_header($string)
{
    //lexer
    $array  = [];
    $buffer = "";
    $char   = "";

    $quote   = FALSE;
    $comment = 0;
    $escape  = FALSE;

    $token = function($token = "") use (&$array, &$buffer)
    {
        if ($token !== "")      $array[] = $token;
        elseif ($buffer !== "") $array[] = $buffer;
        $buffer = "";
    };

    $string = str_replace(["\t", "\n", "\r"], " ", $string);

    for ($i = 0, $c = strlen($string); $i < $c; $i++)
    {
        $char = $string[$i];

        switch ($char)
        {
        case "\\": //escape
//..................................................................................................

            if (! $escape && ($quote || $comment))
            {
                $escape = TRUE;
                continue 2;
            };
            break;

        case "\"": //quote
//..................................................................................................

            if (! $escape && $quote)
            {
                $quote = FALSE;
                continue 2;
            };

            if ($quote)   break;
            if ($comment) break;

            $quote = TRUE;
            continue 2;

        case " ": //space
//..................................................................................................

            if ($quote)   break;
            if ($comment) break;

            continue 2;

        case "(": //comment start
//..................................................................................................

            if ($quote) break;

            if (! $escape || ! $comment) $comment++;
            break;

        case ")": //comment end
//..................................................................................................

            if ($comment < 1) break;

            if (! $escape && ! --$comment)
            {
                $char = " ";
                continue 2;
            };
            break;

        case "=": //value delimiter
        case ";": //parameter delimiter
//..................................................................................................

            if ($quote)   break;
            if ($comment) break;

            $token();
            $token($char);
            continue 2;
        };

        $escape = FALSE;

        if ($comment) continue;

        $buffer .= $char;
    };

    $token();
    $token(";");

//..................................................................................................

    //parser
    $_array = ["#value" => ""]; //main value
    $buffer = "";
    $param  = "#value";

    foreach ($array AS $key => $value)
    {
        switch ($value)
        {
        case "=":
//..................................................................................................

            $param  = $buffer;
            $buffer = "";
            continue 2;

        case ";":
//..................................................................................................

            if (($param !== "") && ($buffer !== ""))
                $_array[strtolower($param)] = $buffer;

            $param  = "";
            $buffer = "";
            continue 2;
        };

        $buffer = $value;
    };

//..................................................................................................

    //rfc 2231 defragmentation
    $result = [];

    //collect fragments
    foreach ($_array AS $key => $value)
    {
        if (! preg_match("/^([^*]+)(?:\*([0-9]+))?(\*)?$/", $key, $_key)) continue; //invalid

        $param   = $_key[1];
        $n       = (int)($_key[2] ?? 0);
        $encoded = isset($_key[3]);

        if (! $encoded) { $result[$param][$n] = $value; continue; }; //not encoded

        //first fragment with charset
        if (($n === 0) && preg_match("/^([^']+)'[^']*'(.*)$/", $value, $_value))
        {
            $result[$param][-1] = $_value[1]; //charset
            $value = $_value[2];
        };

        $result[$param][$n] = urldecode($value);
    };

    //combine fragments
    foreach ($result AS $key => &$value)
    {
        ksort($value);

        if (isset($value[-1]))
        {
            $charset = $value[-1];
            unset($value[-1]);
            $value = utf8_convert(implode("", $value), $charset);
            continue;
        };

        $value = implode("", $value);
    };

    return $result; //["#value"] => [value]
                    //[param]    => [value]
};

//--------------------------------------------------------------------------------------------------

function mime_build_rfc2231_param($name,
                                  $value,
                                  &$filling_level = NULL)
{
    //determine encoding mode
    $mode = preg_match("/[^\x20-\x7E]/", $value);

    //rfc2231 parameter value continuation disabled
    if (! CMS_MIME_RFC2231_PARAM_CONTINUATION)
    {
        $result        = "; $name" . ($mode ? "*=utf-8''" . r($value) : "=" . mime_quote_rfc2045($value));
        $length        = strlen($result);
        $filling_level = ($filling_level ?? 0) + $length;

        if ($filling_level <= 78) return $result;

        $filling_level = $length + 1;
        return "\r\n $result";
    };

    //create utf8 char array
    $array       = preg_split("//u", $value, -1, PREG_SPLIT_NO_EMPTY);
    $result      = [];
    $buffer      = "";
    $length      = 0;
    $i           = 0;
    $wrap_length = strlen("; $name*$i" . ($mode ? "*=utf-8''" : "=\"\""));
    $max_length  = 78 - ($filling_level ?? 0) - $wrap_length;

    //process utf8 chars
    foreach ($array AS $value)
    {
        $char = $mode ? r($value) : addcslashes($value, "\"\\");
        $n    = strlen($char);

        if (($length + $n) > $max_length)
        {
            //flush buffer
            $result[] = $buffer;

            //set up for next line
            $i++;
            $wrap_length = ($buffer === "") ?
                           $wrap_length + 1 :
                           strlen(" ; $name*$i" . ($mode ? "*=" : "=\"\""));
            $max_length  = 78 - $wrap_length;

            //reset buffer
            $buffer = "";
            $length = 0;
        };

        $buffer .= $char;
        $length += $n;
    };

    //flush buffer
    if ($buffer !== "") $result[] = $buffer;

    //empty result
    if (! $result) return "";

    //first line empty
    $buffer = "";
    if ($result[0] === "")
    {
        $buffer        .= "\r\n ";
        $result         = array_slice($result, 1);
        $filling_level  = 1;
    };

    //last line length
    $filling_level += $length + $wrap_length;

    //single line
    if (count($result) === 1)
        return "$buffer; $name" . ($mode ? "*=utf-8''" . $result[0] : "=\"" . $result[0] . "\"");

    //multiple lines
    foreach ($result AS $key => $value)
        $buffer .= ($key === 0) ?
                   "; $name*$key" . ($mode ? "*=utf-8''$value" : "=\"$value\"") :
                   "\r\n ; $name*$key" . ($mode ? "*=$value" : "=\"$value\"");

    return $buffer;
};

//--------------------------------------------------------------------------------------------------

function mime_build_rfc2822_header($array,
                                   &$filling_level = NULL)
{
    //main value
    $filling_level ??= 0;
    $result          = mime_rfc2047_encode($array["#value"] ?? "", $filling_level);
    unset($array["#value"]);

    //parameters
    foreach ($array AS $key => $value)
        $result .= mime_build_rfc2231_param($key, $value, $filling_level);

    return $result;
};

//--------------------------------------------------------------------------------------------------

function mime_convert_header($name,
                             $value)
{
    $type          = mime_header_type($name);
    $filling_level = strlen("$name: ");

    switch ($type)
    {
    case CMS_MIME_HEADER_TYPE_ADDRESS:
//..................................................................................................

        $value = mime_extract_rfc2822_address($value);
        return mime_build_rfc2822_address($value, $filling_level);

    case CMS_MIME_HEADER_TYPE_MIME:
//..................................................................................................

        $value = mime_extract_rfc2822_header($value);
        return mime_build_rfc2822_header($value, $filling_level);

    case CMS_MIME_HEADER_TYPE_IDENTIFICATION:
//..................................................................................................

        return $value;

    default:
    case CMS_MIME_HEADER_TYPE_INFORMATIONAL:
//..................................................................................................

        $value = mime_rfc2047_decode($value);
        return mime_rfc2047_encode($value, $filling_level);
    };
};

//--------------------------------------------------------------------------------------------------

function mime_encode_body($value,
                          $content_transfer_encoding)
{
    switch (strtolower($content_transfer_encoding))
    {
    case CMS_MIME_ENCODING_7BIT:
//..................................................................................................

        return preg_replace("/[^\\x00-\\x7F]/", "", $value);

    case CMS_MIME_ENCODING_BASE64:
//..................................................................................................

        return chunk_split(base64_encode($value));

    case CMS_MIME_ENCODING_QUOTED_PRINTABLE:
//..................................................................................................

        return quoted_printable_encode($value);
    };

    return $value;
};

//--------------------------------------------------------------------------------------------------

function mime_decode_body($value,
                          &$encoding,
                          &$charset)
{
    //decode
    switch (strtolower($encoding))
    {
    case CMS_MIME_ENCODING_BASE64:
//..................................................................................................

        $value    = base64_decode($value);
        $encoding = CMS_MIME_ENCODING_8BIT;
        break;

    case CMS_MIME_ENCODING_QUOTED_PRINTABLE:
//..................................................................................................

        $value    = quoted_printable_decode($value);
        $encoding = CMS_MIME_ENCODING_8BIT;
        break;
    };

    //convert
    switch (strtolower($charset))
    {
    default:
//..................................................................................................

        $value   = utf8_convert($value, $charset);
        $value   = utf8_normalize($value);
        $charset = "utf-8";
        break;

    case "utf-8":
    case "":
//..................................................................................................

        break;
    };

    return $value;
};

//--------------------------------------------------------------------------------------------------

function mime_header_type($name)
{
    switch (strtolower($name))
    {
    case "bcc":
    case "cc":
    case "from":
    case "reply-to":
    case "resent-bcc":
    case "resent-cc":
    case "resent-from":
    case "resent-sender":
    case "resent-to":
    case "sender":
    case "to":
//..................................................................................................

        return CMS_MIME_HEADER_TYPE_ADDRESS;

    case "content-disposition":
    case "content-type":
//..................................................................................................

        return CMS_MIME_HEADER_TYPE_MIME;

    case "content-id":
    case "content-transfer-encoding":
    case "date":
    case "dkim-signature":
    case "in-reply-to":
    case "message-id":
    case "mime-version":
    case "received":
    case "references":
    case "resent-date":
    case "resent-message-id":
    case "return-path":
//..................................................................................................

        return CMS_MIME_HEADER_TYPE_IDENTIFICATION;
    };

    return CMS_MIME_HEADER_TYPE_INFORMATIONAL;
};

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
                      $parent_key = NULL,
                      $content_id = NULL)
    {
        return $this->add_part(
            CMS_MIME_TYPE_TEXT,                 //content_type
            $subtype,                           //subtype
            $subject,                           //subject
            $body,                              //body
            CMS_MIME_ENCODING_QUOTED_PRINTABLE, //content_transfer_encoding
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
        $filename ??= basename($file);

        list($content_type, $subtype) =
            preg_match("/^([^\/]+)\/([^\/]+)$/", get_mime_type($file), $type) ?
            array_slice($type, 1) : [CMS_MIME_TYPE_APPLICATION, "octet-stream"];

        return $this->add_part(
            $content_type,                           //content_type
            $subtype,                                //subtype
            NULL,                                    //subject
            read_file($file),                        //body
            ($content_type === CMS_MIME_TYPE_TEXT) ? //content_transfer_encoding
            CMS_MIME_ENCODING_QUOTED_PRINTABLE :
            CMS_MIME_ENCODING_BASE64,
            $attachment ?                            //content_disposition
            CMS_MIME_DISPOSITION_ATTACHMENT :
            CMS_MIME_DISPOSITION_INLINE,
            $filename,                               //name
            $parent_key,                             //parent_key
            $content_id);                            //content_id
    }

//--------------------------------------------------------------------------------------------------

    function add_part($content_type              = CMS_MIME_TYPE_TEXT,
                      $subtype                   = "plain",
                      $subject                   = NULL,
                      $body                      = NULL,
                      $content_transfer_encoding = CMS_MIME_ENCODING_QUOTED_PRINTABLE,
                      $content_disposition       = CMS_MIME_DISPOSITION_INLINE,
                      $name                      = NULL,
                      $parent_key                = NULL,
                      $content_id                = NULL)
    {
        //check parent type
        if (isset($parent_key) && ! $this->data->is_container($parent_key)) return FALSE;

        if (nstre($subtype)) $content_type .= "/$subtype";
        $content_type .= "; charset=\"utf-8\"";

        if (nstre($name))
        {
            $content_type        .= mime_build_rfc2231_param("name",     $name);
            $content_disposition .= mime_build_rfc2231_param("filename", $name);
        };

        if (stre($content_id)) $content_id = $this->content_id();

        $this->data->set_buffer(
            [["content-type"              => $content_type,
              "content-transfer-encoding" => $content_transfer_encoding,
              "content-disposition"       => $content_disposition,
              "content-id"                => $content_id,
              "subject"                   => mime_rfc2047_encode($subject ?? ""),
              "#body"                     => $body,
              "#size"                     => strlen($body ?? "")]]);

        return $this->data->insert($parent_key);
    }

//--------------------------------------------------------------------------------------------------

    function add_multipart($subtype    = "mixed",
                           $subject    = NULL,
                           $parent_key = NULL)
    {
        //check parent
        if (isset($parent_key) && ! $this->data->is_container($parent_key)) return FALSE;

        $boundary = unique_id() . "-" . unique_id() . "-" . unique_id() . "-" . unique_id();

        $this->data->set_buffer(
            [["content-type" => "multipart/$subtype; boundary=$boundary",
              "subject"      => mime_rfc2047_encode($subject ?? ""),
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
        //check parent
        if (isset($parent_key) && ! $this->data->is_container($parent_key)) return FALSE;

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
        //set primary header data
        $key = $this->data->move("first");

        $this->data->set("1.0",               $key, "mime-version");
        $this->data->set($this->content_id(), $key, "message-id");
        $this->data->set(date("r"),           $key, "date");
        $this->data->set(CMS_USER_AGENT,      $key, "x-mailer");

        if (nstre($from))     $this->data->set($from,     $key, "from");
        if (nstre($to))       $this->data->set($to,       $key, "to");
        if (nstre($cc))       $this->data->set($cc,       $key, "cc");
        if (nstre($bcc))      $this->data->set($bcc,      $key, "bcc");
        if (nstre($reply_to)) $this->data->set($reply_to, $key, "reply-to");

        $result     = "";
        $i          = 0;
        $boundary   = [$i => NULL];
        $_boundary  = NULL;

        $this->data->move("first");
        while ($key = $this->data->move("next"))
        {
            $array                     = $this->data->get($key);
            $content_type              = $array["content-type"] ?? "";
            $content_type              = mime_extract_rfc2822_header($content_type);
            $content_transfer_encoding = $array["content-transfer-encoding"] ?? "";
            $content_transfer_encoding = mime_extract_rfc2822_header($content_transfer_encoding);
            $type                      = $array["#type"] ?? "";

            if (nstreq($type, "/container"))
            {
                //opening boundary
                if (nstre($_boundary)) $result .= "--$_boundary\r\n";

                //add header
                foreach ($array AS $name => $value)
                {
                    if (nstre($value) && ($name[0] !== "#"))
                    {
                        //workaround for misinterpreting clients
                        if (

                        //content-disposition
                             streq($name, "content-disposition")
                             &&

                        //inline
                             streq($value, CMS_MIME_DISPOSITION_INLINE)
                             &&

                        //skip inline for text parts
                             preg_match("/^text(?:\/|$)/i", $content_type["#value"])
                           )
                            continue;

                        //convert header name to conventional format
                        $name = preg_replace_callback(
                            "/\b((?:id|mime)\b|\w)/i",
                            function($match) { return strtoupper($match[1]); },
                            $name);

                        //convert header value to conventional format
                        $value = mime_convert_header($name, $value);

                        //append header to message
                        $result .= "$name: $value\r\n";
                    };
                };

                $result .= "\r\n"; //end of header

                //content type message has no body -> skip
                if (! preg_match("/^message(?:\/|$)/i", $content_type["#value"]))

                    //add body
                    $result .= mime_encode_body($array["#body"] ?? "",
                                                $content_transfer_encoding["#value"]) .
                               "\r\n";
            };

            //process nesting
            switch ($type)
            {
            case "container":

                $_boundary      = $array["#boundary"] ?? NULL;
                $boundary[++$i] = $_boundary;
                break;

            case "/container":

                //closing boundary
                if (nstre($_boundary)) $result .= "--$_boundary--\r\n";

                $_boundary = $boundary[--$i];
                break;
            };
        };

        //increase message count of current process
        $count = (int)cms_cache("mime.message_count");
        cms_cache("mime.message_count", $count + 1);

        return $result;
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