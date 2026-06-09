<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   POP CLASS
//==================================================================================================

class pop
{

//properties
//--------------------------------------------------------------------------------------------------

    public $hfile    = NULL;
    public $response = NULL;
    public $enabled  = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($host,
                         $username,
                         $password)
    {
        if (! $host) return;

        if (! $this->hfile = fsockopen($host, 110))
        {
            $this->response = sprintf(CMS_L_POP_001, $host);
            return;
        };

        if (
             ($response = fgets($this->hfile, 512))
             &&
             streq(substr($response, 0, 3), "+OK")
           )
        {
            if (
                 extension_loaded("openssl")
                 &&
                 $this->execute("STLS")
                 &&
                 streq(substr($this->response, 0, 3), "+OK")
               )
                stream_socket_enable_crypto($this->hfile, TRUE, STREAM_CRYPTO_METHOD_TLS_CLIENT);

            if (
                 (
                   preg_match("/<([^>]*)>/", $response, $response)
                   &&
                   $this->execute("APOP $username " . md5("<" . $response[1] . ">$password"))
                 )
                 ||
                 (
                   $this->execute("USER $username")
                   &&
                   $this->execute("PASS $password")
                 )
               )
            {
                $this->enabled = TRUE;
                return;
            };
        };

        $this->quit();
        fclose($this->hfile);
    }

//methods
//--------------------------------------------------------------------------------------------------

    function get_statistics()
    {
        if (
             (! $this->enabled)
             ||
             (! $this->execute("STAT"))
           )
            return FALSE;

        $response = explode(" ", $this->response);
        return ["count" => $response[1],
                "size"  => $response[2]];
    }

//--------------------------------------------------------------------------------------------------

    function get_list()
    {
        if (
             (! $this->enabled)
             ||
             (! $this->execute("LIST"))
           )
            return FALSE;

        $array = NULL;
        while (
                $this->receive()
                &&
                nstreq($this->response, ".\r\n")
              )
        {
            $response            = explode(" ", $this->response);
            $array[$response[0]] = $response[1];
        };

        return $array;
    }

//--------------------------------------------------------------------------------------------------

    function get_unique_id_list()
    {
        if (
             (! $this->enabled)
             ||
             (! $this->execute("UIDL"))
           )
            return FALSE;

        $array = NULL;
        while (
                $this->receive()
                &&
                nstreq($this->response, ".\r\n")
              )
        {
            $response            = explode(" ", $this->response);
            $array[$response[0]] = $response[1];
        };

        return $array;
    }

//--------------------------------------------------------------------------------------------------

    function get_header($index)
    {
        if (
             (! $this->enabled)
             ||
             (! $this->execute("TOP $index 0"))
           )
            return FALSE;

        $array = $this->receive_header();

        while ($this->receive());

        return $array;
    }

//--------------------------------------------------------------------------------------------------

    function receive_header()
    {
        $array = NULL;

        while (
                $this->receive()
                &&
                nstreq($this->response, "\r\n")
              )
        {
            if (preg_match("/^\s/", $this->response))
            {
                if (isset($_array[1])) $array[$_array[1]] .= " " . trim($this->response);
            }
            elseif (preg_match("/^([^:]+):(.*)$/", $this->response, $_array))
            {
                $_array[1]         = strtolower($_array[1]);
                $_array[2]         = trim($_array[2]);
                $array[$_array[1]] = $_array[2];
            };
        };

        if (! is_array($array)) return NULL;

        //decode
        foreach ($array AS $key => $value)
        {
            while (preg_match("/(.*)=\?[^\?]+\?(q|b)\?([^\?]*)\?=(.*)/i", $value, $_array))
            {
                switch (strtolower($_array[2]))
                {
                case "b":
//..................................................................................................

                    $_array[3] = base64_decode($_array[3]);
                    break;

                case "q":
//..................................................................................................

                    $_array[3] = quoted_printable_decode($_array[3]);
                    $_array[3] = str_replace("_", " ", $_array[3]);
                };

                $value = $_array[1] . $_array[3] . $_array[4];
            };

            $array[$key] = $value;
        };

        return $array;
    }

//--------------------------------------------------------------------------------------------------

    function get_message($index)
    {
        if (
             (! $this->enabled)
             ||
             (! cms_load("mime"))
             ||
             (! $this->execute("RETR $index"))
           )
            return FALSE;

        $data           = new data();
        $boundary       = [0 => NULL];
        $message        = [0 => NULL];
        $size           = 0;
        $i              = 0;
        $boundary_depth = 0;
        $message_depth  = 0;

        do
        {
            //receive header
            $data->set($this->receive_header(), ++$i);

            $body         = NULL;
            $content_type = mime_extract_rfc2822_header($data->get($i, "content-type"));

            if (preg_match("/^multipart(?:\/.+)?$/i", $content_type["#value"]))
            {
                $data->set("container", $i, "#type");

                if (isset($content_type["boundary"]))
                {
                    $boundary[++$boundary_depth] = $content_type["boundary"];
                    $data->set($boundary[$boundary_depth], $i, "#boundary");
                }
                else
                {
                    return FALSE;
                };
            }
            elseif (preg_match("/^message(?:\/.+)?$/i", $content_type["#value"]))
            {
                $data->set("container", $i, "#type");
                $message[++$message_depth] = $boundary_depth;
                continue;
            };

            //receive body
            while ($this->receive($boundary[$boundary_depth]))
                $body .= $this->response;

            $content_transfer_encoding = mime_extract_rfc2822_header($data->get($i, "content-transfer-encoding"));

            if (preg_match("/^(?:base64|quoted-printable)/i", $content_transfer_encoding["#value"]))
            {
                switch ($content_transfer_encoding["#value"][0])
                {
                case "B":
                case "b":
//..................................................................................................

                    $body  = base64_decode($body);
                    break;

                case "Q":
                case "q":
//..................................................................................................

                    $body  = quoted_printable_decode($body);
                    break;
                };
            };

            $data->set($body, $i, "#body");
            $_size  = strlen($body);
            $data->set($_size, $i, "#size");
            $size  += $_size;

            if (
                 $message_depth
                 &&
                 ($message[$message_depth] === $boundary_depth)
               )
            {
                $data->set("/container", ++$i, "#type");
                unset($message[$message_depth]);
                $message_depth--;
            };

            if (streq($this->response, "--" . $boundary[$boundary_depth] . "--\r\n"))
            {
                $data->set("/container", ++$i, "#type");
                unset($boundary[$boundary_depth]);
                $boundary_depth--;
                if (! $boundary_depth) break;
            };
        }
        while ($this->response && nstreq($this->response, ".\r\n"));

        //ignore trailing message lines
        socket_set_blocking($this->hfile, 0);
        while (fgets($this->hfile, 512));
        socket_set_blocking($this->hfile, 1);

        $data->set($size, 1, "#size");

        return $data;
    }

//--------------------------------------------------------------------------------------------------

    function delete($index)
    {
        if (! $this->enabled) return FALSE;

        return $this->execute("DELE $index");
    }

//--------------------------------------------------------------------------------------------------

    function execute($command)
    {
        fwrite($this->hfile, $command . "\r\n");

        return $this->receive();
    }

//--------------------------------------------------------------------------------------------------

    function receive($boundary = NULL)
    {
        if (
             (! $this->response = fgets($this->hfile, 512))
             ||
             streq($this->response, ".\r\n")
             ||
             (
               $boundary
               &&
               preg_match("/^--" . addcslashes(preg_quote($boundary), "/") . "(?:--)?\r\n$/", $this->response)
             )
             ||
             streq(substr($this->response, 0, 4), "-ERR")
           )
            return FALSE;

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function quit()
    {
        if (! $this->enabled) return FALSE;

        $return        = $this->execute("QUIT");
        $this->enabled = FALSE;
        fclose($this->hfile);

        return $return;
    }
};