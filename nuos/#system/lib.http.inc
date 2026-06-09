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
                    $get_header_only = FALSE,
                    $post_data       = NULL)
{
    //break address into components
    $url = analyze_url($url);
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
        $context      = stream_context_create(["ssl" => ["verify_peer" => FALSE]]);
        break;

    default:

        return FALSE; //impermissible scheme
    };

    //address components
    $host  = $url["host"];
    $path  = ($url["path"] === NULL)  ? "/"           : $url["path"];
    $port  = ($url["port"] === NULL)  ? $port_default : (int)$url["port"];
    $query = ($url["query"] === NULL) ? ""            : "?" . $url["query"];

    //authorization
    $authorization = "";

    if ($url["user"] !== NULL)
    {
        $authorization = $url["user"];
        if (isset($url["pass"])) $authorization .= ":" . $url["pass"];
        $authorization = "Authorization: Basic " . base64_encode($authorization) . "\r\n";
    };

    //connect
    $hfile = stream_socket_client(
        $transport . $host . ":$port",
        $error_number,
        $error,
        CMS_HTTP_TIMEOUT_TOTAL,
        STREAM_CLIENT_CONNECT,
        $context);
    if ($hfile === FALSE) return FALSE; //connection error

    //prepare request
    if (is_array($post_data))
    {
        $command = "POST";

        //create name-value-pair string
        $body = "";
        foreach ($post_data AS $key => $value)
            $body .= r($key) . "=" . r($value) . "&";
        $body = substr($body, 0, -1);

        $head = "Content-Type: application/x-www-form-urlencoded; charset=utf-8\r\n" .
                "Content-Length: " . strlen($body) . "\r\n";
    }
    else
    {
        $command = $get_header_only ? "HEAD" : "GET";
        $head    = "";
        $body    = "";
    };

    //send request
    $status  = http_send(
        $hfile,
        "$command $path$query HTTP/1.0\r\n" .
        "Host: $host\r\n" .
        "User-Agent: " . CMS_USER_AGENT . "\r\n" .
        $authorization .
        "Connection: close\r\n" .
        $head .
        "\r\n" .
        $body);

    if ($status === FALSE) { fclose($hfile); return FALSE; }; //error

    //receive http status code
    $header = http_fetch_header($hfile);
    if ($header === FALSE) { fclose($hfile); return FALSE; }; //error

    //check http status code
    if (! preg_match("/^HTTP\/[0-9]\.[0-9]\s+([1-4][0-9]{2})/i", $header, $_header))
    {
        //invalid http status
        fclose($hfile);
        return $get_header_only ? $header : FALSE;
    };

    //store status
    $return = ["#status" => $_header[1]];

    //retrieve header fields
    while (($header = http_fetch_header($hfile)) !== FALSE)
    {
        //check for end of header (empty line)
        if ($header === "") break; //end of header

        //keep header field
        if ($offset = strpos($header, ":"))
            $return[strtolower(substr($header, 0, $offset))] =
                ltrim(substr($header, $offset + 1));
    };

    //return header information
    if ($get_header_only) { fclose($hfile); return $return; };

    //return connection
    return $hfile;
};

//--------------------------------------------------------------------------------------------------

function http_send($hfile,
                   $data)
{
    $length = strlen($data); //data size
    $time   = time();        //chunk time

    stream_set_blocking($hfile, FALSE);

    while ($length > 0)
    {
        //send data
        $_length = fwrite($hfile, $data);
        if ($_length === FALSE) return FALSE; //error

        if ($_length === 0)
        {
            //check chunk time
            if ((time() - $time) > CMS_HTTP_TIMEOUT_CHUNK) return FALSE; //timeout

            //wait
            usleep(10000);
            continue;
        };

        //extract remaining data
        $length -= $_length;
        $data    = substr($data, -$length);
    };

    stream_set_blocking($hfile, TRUE);

    return TRUE;
};

//--------------------------------------------------------------------------------------------------

function http_fetch_header($hfile)
{
    $time = time(); //chunk time

    stream_set_blocking($hfile, FALSE);

    while (! feof($hfile))
    {
        //retrieve single line
        $buffer = stream_get_line($hfile, CMS_HTTP_SIZE_CHUNK, "\n");

        if ($buffer === FALSE)
        {
            //check chunk time
            if ((time() - $time) > CMS_HTTP_TIMEOUT_CHUNK) return FALSE; //timeout

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
    $buffer = "";     //data buffer
    $time   = time(); //total time
    $_time  = $time;  //chunk time

    stream_set_blocking($hfile, FALSE);

    while (! feof($hfile))
    {
        //retrieve data chunk
        $_buffer = stream_get_contents($hfile, CMS_HTTP_SIZE_CHUNK);
        if ($_buffer === FALSE) { fclose($hfile); return FALSE; }; //error

        if ($_buffer === "")
        {
            $__time = time(); //current time

            //check timeout
            if (
                 (($__time - $time) > CMS_HTTP_TIMEOUT_TOTAL)
                 ||
                 (($__time - $_time) > CMS_HTTP_TIMEOUT_CHUNK)
               )
            {
                fclose($hfile);
                return FALSE;
            };

            //wait
            usleep(10000);
            continue;
        };

        //append to buffer
        $buffer .= $_buffer;

        //limit data
        if (strlen($buffer) > CMS_HTTP_LIMIT) break;

        //reset chunk time
        $_time = time();
    };

    fclose($hfile);
    return $buffer;
};

//--------------------------------------------------------------------------------------------------

function http_header($url)
{
    return http_fopen($url, TRUE);
};

//--------------------------------------------------------------------------------------------------

function http_post($url,
                   $data)
{
    if (! is_array($data)) return FALSE; //no post data

    //connect and send data
    $hfile = http_fopen($url, FALSE, $data);
    if ($hfile === FALSE) return FALSE;

    //fetch and return reply
    return http_fetch_data($hfile);
};