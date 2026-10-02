<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   HTTP FUNCTIONS
//==================================================================================================

//constants
define("CMS_HTTP_TIMEOUT_TOTAL",      10);
define("CMS_HTTP_TIMEOUT_CHUNK",       3);
define("CMS_HTTP_SIZE_CHUNK",     524289);
define("CMS_HTTP_LIMIT",         1048576);

//related functions
//--------------------------------------------------------------------------------------------------

function http_fopen($url,
                    $post_data = NULL,
                    &$header   = NULL,
                    $option    = NULL)
{
    //break address into components
    $url = analyze_url($url);
    if ($url === FALSE)        return FALSE; //invalid
    if ($url["host"] === NULL) return FALSE; //no host

    //scheme
    switch ($url["scheme"])
    {
    case "":
    case "http":

        $transport    = "tcp://";
        $port_default = 80;
        $context      = NULL;
        break;

    case "https":

        $transport    = "ssl://";
        $port_default = 443;
        $verify       = (bool)($option["verify"] ?? FALSE);
        $context      = stream_context_create(
            ["ssl" => ["verify_peer"      => $verify,
                       "verify_peer_name" => $verify]]);
        break;

    default:

        return FALSE; //impermissible scheme
    };

    //address components
    $host  = $url["host"];
    $path  = ($url["path"] === NULL)  ? "/"           : $url["path"];
    $port  = ($url["port"] === NULL)  ? $port_default : (int)$url["port"];
    $query = ($url["query"] === NULL) ? ""            : "?" . $url["query"];

    //default http headers
    $default = ["Host"            => $host,
                "User-Agent"      => CMS_USER_AGENT,
                "Accept-Encoding" => "identity",
                "Connection"      => "close"];

    //authorization
    if ($url["user"] !== NULL)
    {
        $authorization = $url["user"];
        if (isset($url["pass"])) $authorization .= ":" . $url["pass"];
        $default["Authorization"] = "Basic " . base64_encode($authorization);
    };

    //prepare request
    $head = "";
    $body = [];

    //form data
    if (is_array($post_data))
    {
        list($head, $body) = http_build_post($post_data);
    }

    //raw body
    elseif (isset($option["body"]))
    {
        $body = [(string)$option["body"]];
        $default["Content-Length"] = strlen($body[0]);
    };

    //method
    $command = $body ? "POST" : "GET";

    //prepare header
    $array = (array)($option["header"] ?? []);
    $array = array_merge($array, array_diff_ukey($default, $array, "strcasecmp"));
    foreach ($array AS $key => $value) $head .= "$key: $value\r\n";

    //transfer budget
    $timeout_total = (int)($option["timeout_total"] ?? CMS_HTTP_TIMEOUT_TOTAL);
    $timeout_chunk = (int)($option["timeout_chunk"] ?? CMS_HTTP_TIMEOUT_CHUNK);

    //connect
    $hfile = stream_socket_client(
        $transport . $host . ":$port",
        $error_number,
        $error,
        $timeout_total,
        STREAM_CLIENT_CONNECT,
        $context);
    if ($hfile === FALSE) return FALSE; //connection error

    //set handle budget
    http_state($hfile, "timeout_total", $timeout_total);
    http_state($hfile, "timeout_chunk", $timeout_chunk);
    http_state($hfile, "limit", (int)($option["limit"] ?? CMS_HTTP_LIMIT));

    //send request header
    $status = http_send(
        $hfile,
        "$command $path$query HTTP/1.1\r\n" .
        $head .
        "\r\n");

    //send request body
    if ($status !== FALSE)
    {
        foreach ($body AS $value)
        {
            $status = http_send($hfile, $value);
            if ($status === FALSE) break;
        };
    };

    if ($status === FALSE) { fclose($hfile); return FALSE; }; //error

    //receive http status code
    for ($i = 0; $i < 10; $i++)
    {
        $_header = http_fetch_header($hfile);
        if ($_header === FALSE) { fclose($hfile); return FALSE; }; //error

        //check http status code
        if (! preg_match("/^(HTTP\/1\.\d)\s+([1-5]\d{2})(?:\s+(.+))?/", $_header, $match))
        {
            //invalid http status
            fclose($hfile);
            return FALSE;
        };

        //continue on 1xx status
        if ($match[2] >= 200) break;
    };

    //store status
    $header = ["#version" => $match[1],
               "#status"  => $match[2]];
    if (isset($match[3])) $header["#message"] = $match[3];

    //retrieve header fields
    while (($_header = http_fetch_header($hfile)) !== FALSE)
    {
        //check for end of header (empty line)
        if ($_header === "") break; //end of header

        //keep header field
        if ($offset = strpos($_header, ":"))
        {
            $key   = strtolower(substr($_header, 0, $offset));
            $value = ltrim(substr($_header, $offset + 1));
            if (streq($key, "set-cookie")) $header[$key][]  = $value;
            elseif (isset($header[$key]))  $header[$key]   .= ", " . $value;
            else                           $header[$key]    = $value;
        };
    };

    //set chunked state
    http_state(
        $hfile, "chunk_remaining",
        preg_match("/\bchunked\b/i", $header["transfer-encoding"] ?? "") ? 0 : FALSE);

    //return connection
    return $hfile;
};

//--------------------------------------------------------------------------------------------------

function http_send($hfile,
                   $data)
{
    $length = strlen($data); //data size
    $offset = 0;             //data offset
    $time   = time();        //chunk time

    stream_set_blocking($hfile, FALSE);

    while ($offset < $length)
    {
        //send data
        $_length = fwrite($hfile, substr($data, $offset, CMS_HTTP_SIZE_CHUNK));
        if ($_length === FALSE) return FALSE; //error

        if ($_length === 0)
        {
            //check chunk time
            if ((time() - $time) > CMS_HTTP_TIMEOUT_CHUNK) return FALSE; //timeout

            //wait
            usleep(10000);
            continue;
        };

        //advance offset
        $offset += $_length;
    };

    stream_set_blocking($hfile, TRUE);

    return TRUE;
};

//--------------------------------------------------------------------------------------------------

function http_fetch_header($hfile)
{
    $time    = time(); //chunk time
    $timeout = http_state($hfile, "timeout_chunk");

    stream_set_blocking($hfile, FALSE);

    while (! feof($hfile))
    {
        //retrieve single line
        $buffer = stream_get_line($hfile, CMS_HTTP_SIZE_CHUNK, "\n");

        if ($buffer === FALSE)
        {
            //check chunk time
            if ((time() - $time) > $timeout) return FALSE; //timeout

            //wait
            usleep(10000);
            continue;
        };

        if (($buffer === "") || ($buffer === "\r")) return ""; //end of header

        $buffer = trim($buffer);
        if ($buffer === "") continue; //ignore
        return $buffer; //data
    };

    stream_set_blocking($hfile, TRUE);

    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function http_fetch_data($hfile)
{
    stream_set_blocking($hfile, FALSE);
    $buffer = http_get_contents($hfile, http_state($hfile, "limit"));

    fclose($hfile);
    http_state($hfile); //delete

    return $buffer;
};

//--------------------------------------------------------------------------------------------------

function http_state($hfile,
                    $key   = NULL,
                    $value = NULL)
{
    static $array = [];

    $id = (int)$hfile;

    //check if identical
    if (isset($array[$id]) && ($array[$id]["#id"] !== $hfile)) unset($array[$id]);

    //resource has been closed
    if (! is_resource($hfile)) { unset($array[$id]); return NULL; };

    //discard
    if ($key === NULL) { unset($array[$id]); return TRUE; };

    //read
    if ($value === NULL) return $array[$id][$key] ?? NULL;

    //write
    $array[$id] ??= ["#id" => $hfile];
    $array[$id][$key] = $value;

    return TRUE;
};

//--------------------------------------------------------------------------------------------------

function http_get_contents($hfile,
                           $length = NULL)
{
    //retrieve remaining value
    $remaining = http_state($hfile, "chunk_remaining");
    if ($remaining === NULL) return FALSE; //invalid or closed handle

    $chunked = $remaining !== FALSE;
    if (! $chunked) $remaining = CMS_HTTP_SIZE_CHUNK;

    $buffer  = "";     //data buffer
    $_length = 0;      //data buffer length
    $time    = time(); //total time
    $_time   = $time;  //chunk time

    //transfer budget
    $timeout_total = http_state($hfile, "timeout_total");
    $timeout_chunk = http_state($hfile, "timeout_chunk");

    while (($length === NULL) || ($_length < $length))
    {
        //start of chunk
        if ($chunked && ($remaining === 0))
        {
            //get chunk size, loop until complete
            while (($size = stream_get_line($hfile, 1024, "\n")) === FALSE)
            {
                $__time = time(); //current time

                //check timeout
                if (
                     (($__time - $time) > $timeout_total)
                     ||
                     (($__time - $_time) > $timeout_chunk)
                   )
                    return FALSE; //timeout

                usleep(10000);
            };

            //check chunk size
            $size = trim($size);
            if (! ctype_xdigit($size)) return FALSE; //invalid

            $remaining = hexdec($size);
            if ($remaining === 0) return $buffer; //end of stream

            $_time = time(); //reset chunk time
        };

        $__length = ($length === NULL) ? $remaining : min($length - $_length, $remaining);

        //retrieve data chunk
        $_buffer = stream_get_contents($hfile, $__length);
        if ($_buffer === FALSE) return FALSE; //error

        if ($_buffer === "") //no data
        {
            if (feof($hfile)) break; //end of stream

            $__time = time(); //current time

            //check timeout
            if (
                 (($__time - $time) > $timeout_total)
                 ||
                 (($__time - $_time) > $timeout_chunk)
               )
                return FALSE; //timeout

            //wait
            usleep(10000);
            continue;
        };

        //append to buffer
        $buffer    .= $_buffer;
        $__length   = strlen($_buffer);
        $_length   += $__length;
        $remaining -= $__length;

        //chunk has been completed
        if ($remaining === 0)
        {
            if ($chunked) stream_get_line($hfile, 1024, "\n"); //skip end bytes
            else          $remaining = CMS_HTTP_SIZE_CHUNK;    //reset chunk size
        };

        //reset chunk time
        $_time = time();
    };

    //store remaining value
    if ($chunked) http_state($hfile, "chunk_remaining", $remaining);

    return $buffer;
};

//--------------------------------------------------------------------------------------------------

function http_header($url)
{
    $hfile = http_fopen($url, NULL, $header);
    if ($hfile === FALSE) return FALSE;
    fclose($hfile);
    return $header;
};

//--------------------------------------------------------------------------------------------------

function http_request($url,
                      $option  = NULL,
                      &$header = NULL)
{
    $option["verify"] ??= TRUE; //verify ssl certificate

    //connect and send if option has body
    $hfile = http_fopen($url, NULL, $header, $option);
    if ($hfile === FALSE) return FALSE;

    //fetch and return reply
    return http_fetch_data($hfile);
};

//--------------------------------------------------------------------------------------------------

function http_post($url,
                   $data,
                   &$header = NULL)
{
    if (! is_array($data)) return FALSE; //no post data

    //connect and send data
    $hfile = http_fopen($url, $data, $header);
    if ($hfile === FALSE) return FALSE;

    //fetch and return reply
    return http_fetch_data($hfile);
};

//--------------------------------------------------------------------------------------------------

function http_build_post($data)
{
    //overload method so it treats file arrays as leaves
    $iterator = new \RecursiveIteratorIterator(
        new class($data) extends \RecursiveArrayIterator
        {
            public function hasChildren(): bool
            {
                $value = $this->current();
                return is_array($value) && ! isset($value["data"]);
            }
        },
        \RecursiveIteratorIterator::LEAVES_ONLY);

    $body     = [];
    $boundary = unique_id() . "-" . unique_id() . "-" . unique_id() . "-" . unique_id();

    //process data
    foreach ($iterator AS $value)
    {
        //reconstruct php array notation
        $depth = $iterator->getDepth();
        $name  = (string)$iterator->getSubIterator(0)->key();
        for ($i = 1; $i <= $depth; $i++)
            $name .= "[" . (string)$iterator->getSubIterator($i)->key() . "]";

        //file
        if (is_array($value) && isset($value["data"]))
        {
            $filename = $value["filename"] ?? $value["name"] ?? $name;
            $type     = $value["type"] ?? get_mime_type($filename);

            $body[] = "--$boundary\r\n" .
                      "Content-Disposition: form-data; name=" . quote_http_param($name) .
                      (($filename !== "") ? "; filename=" . quote_http_param($filename) : "") . "\r\n" .
                      "Content-Type: $type\r\n" .
                      "Content-Transfer-Encoding: base64\r\n" .
                      "\r\n";

            //important: memory optimization, keep in separate line
            if ($value["data"] !== "") $body[] = $value["data"];

            $body[] = "\r\n";
            continue;
        };

        //scalar
        $body[] = "--$boundary\r\n" .
                  "Content-Disposition: form-data; name=" . quote_http_param($name) . "\r\n" .
                  "\r\n" .
                  "$value\r\n";
    };

    $body[] = "--$boundary--\r\n"; //end of body

    //compute content length
    $length = 0;
    foreach ($body AS $value) $length += strlen($value);

    //header
    $head = "Content-Type: multipart/form-data; boundary=$boundary\r\n" .
            "Content-Length: $length\r\n";

    return [$head, $body];
};