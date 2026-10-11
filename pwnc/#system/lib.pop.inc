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

define("CMS_POP_STATUS_ERROR",        0);
define("CMS_POP_STATUS_OK",           1);
define("CMS_POP_STATUS_BOUNDARY",     2);
define("CMS_POP_STATUS_BOUNDARY_END", 3);
define("CMS_POP_STATUS_MESSAGE_END",  4);

class pop
{

//properties
//--------------------------------------------------------------------------------------------------

    public $hfile    = NULL;
    public $response = NULL;
    public $enabled  = NULL;
    public $error    = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($host,
                         $username,
                         $password)
    {
        $this->enabled = FALSE;
        $this->error   = FALSE;

        if (stre($host)) return; //required

        //connect
        if (($this->hfile = fsockopen($host, 110)) === FALSE)
        {
            //error
            $this->response = sprintf(t("err_host_unavailable"), $host);
            return;
        };

        //timeout
        stream_set_timeout($this->hfile, 30);

        //initial response
        if (
             (($response = fgets($this->hfile, 8192)) !== FALSE)
             &&
             streq(substr($response, 0, 3), "+OK")
           )
        {
            //use secure transport if possible
            if (
                 extension_loaded("openssl")
                 &&
                 $this->execute("STLS")
                 &&
                 streq(substr($this->response, 0, 3), "+OK")
               )
            {
                if (! stream_socket_enable_crypto($this->hfile, TRUE, STREAM_CRYPTO_METHOD_TLS_CLIENT))
                {
                    //failed
                    fclose($this->hfile);
                    return;
                };
            };

            //authentification
            if (

            //apop cram-md5
                 (
                   preg_match("/<([^>]*)>/", $response, $response)
                   &&
                   $this->execute("APOP $username " . md5("<" . $response[1] . ">$password"))
                 )
                 ||

            //plain login
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

        fclose($this->hfile);
    }

//methods
//--------------------------------------------------------------------------------------------------

    function get_statistics()
    {
        if (! $this->enabled || ! $this->execute("STAT")) return FALSE;

        $response = preg_split("/\s+/", $this->response, -1, PREG_SPLIT_NO_EMPTY);
        return ["count" => $response[1] ?? NULL,
                "size"  => $response[2] ?? NULL];
    }

//--------------------------------------------------------------------------------------------------

    function get_list()
    {
        if (! $this->enabled || ! $this->execute("LIST")) return FALSE;

        $array = [];
        while (($status = $this->receive()) === CMS_POP_STATUS_OK)
        {
            if ($this->error) return FALSE; //error

            $response = preg_split("/\s+/", $this->response, -1, PREG_SPLIT_NO_EMPTY);
            if ($response) $array[$response[0]] = $response[1] ?? NULL;
        };

        return $array ?: NULL;
    }

//--------------------------------------------------------------------------------------------------

    function get_unique_id_list()
    {
        if (! $this->enabled || ! $this->execute("UIDL")) return FALSE;

        $array = [];
        while (($status = $this->receive()) === CMS_POP_STATUS_OK)
        {
            if ($this->error) return FALSE; //error

            $response = preg_split("/\s+/", $this->response, -1, PREG_SPLIT_NO_EMPTY);
            if ($response) $array[$response[0]] = $response[1] ?? NULL;
        };

        return $array ?: NULL;
    }

//--------------------------------------------------------------------------------------------------

    function get_header($index)
    {
        if (! $this->enabled || ! $this->execute("TOP $index 0")) return FALSE;

        if (($array = $this->receive_header()) === FALSE) return FALSE; //error
        $this->receive(); //end of message

        return $array;
    }

//--------------------------------------------------------------------------------------------------

    function receive_header($boundary = NULL)
    {
        //receive header
        $header = [];

        while (TRUE)
        {
            $status = $this->receive($boundary);
            if ($status !== CMS_POP_STATUS_OK) return $status;

            //end of header
            if ($this->response === "\r\n") return $header;

            //unfold header
            if (preg_match("/^\s/", $this->response))
            {
                if (isset($param)) $header[$param] .= " " . trim($this->response);
            }

            //new header
            elseif (preg_match("/^([^:]+):(.*)$/", $this->response, $array))
            {
                $param = strtolower($array[1]);
                $value = trim($array[2]);

                if (isset($header[$param])) $header[$param] .= "\n" . $value;
                else                        $header[$param]  = $value;
            };
        };
    }

//--------------------------------------------------------------------------------------------------

    function get_message($index)
    {
        if (
             ! $this->enabled
             ||
             ! cms_load("mime")
             ||
             ! $this->execute("RETR $index")
           )
            return FALSE;

        $data           = new data();
        $i              = 0;
        $boundary       = [0 => NULL];
        $message        = [0 => NULL];
        $boundary_depth = 0;
        $message_depth  = 0;
        $size           = 0;

        $up = function() use (&$data, &$boundary, &$message, &$i, &$boundary_depth, &$message_depth)
        {
            //close message container
            if (($message_depth > 0) && ($message[$message_depth] === $boundary_depth))
            {
                $data->set("/container", ++$i, "#type");
                unset($message[$message_depth--]);
            };

            //close multipart container
            if ($boundary_depth > 0)
            {
                $data->set("/container", ++$i, "#type");
                unset($boundary[$boundary_depth--]);
            };
        };

        do
        {
            //receive header
            $header = $this->receive_header($boundary[$boundary_depth]);

            if (! is_array($header))
            {
                switch ($header)
                {
                case CMS_POP_STATUS_ERROR:        return FALSE;
                case CMS_POP_STATUS_BOUNDARY_END: $up();
                case CMS_POP_STATUS_BOUNDARY:     continue 2;
                case CMS_POP_STATUS_MESSAGE_END:  break 2;
                };
            };

            //empty header
            if (! $header) continue;

            //save header
            $data->set($header, ++$i);

            //process content type
            $content_type  = mime_extract_rfc2822_header($data->get($i, "content-type") ?? "");
            $_content_type = preg_match("/^([^\/]+)/", $content_type["#value"], $type) ?
                             strtolower($type[1]) : "";

            switch ($_content_type)
            {
            case "multipart":
//..................................................................................................

                if (! isset($content_type["boundary"])) return FALSE; //boundary required

                //open multipart container
                $data->set("container", $i, "#type");

                //boundary
                $boundary[++$boundary_depth] = $content_type["boundary"];
                $data->set($boundary[$boundary_depth], $i, "#boundary");
                break;

            case "message":
//..................................................................................................

                //open message container
                $data->set("container", $i, "#type");
                $message[++$message_depth] = $boundary_depth;
                continue 2;
            };

            //receive body
            $body = "";
            while (TRUE)
            {
                $status = $this->receive($boundary[$boundary_depth]);

                switch ($status)
                {
                case CMS_POP_STATUS_ERROR:

                    return FALSE;

                case CMS_POP_STATUS_BOUNDARY:
                case CMS_POP_STATUS_BOUNDARY_END:
                case CMS_POP_STATUS_MESSAGE_END:

                    break 2;
                };

                $body .= $this->response;
            };

            //decode body
            $encoding = mime_extract_rfc2822_header(
                $data->get($i, "content-transfer-encoding") ?? "")["#value"];
            $charset  = $content_type["charset"] ?? "";
            $body     = mime_decode_body($body, $encoding, $charset);

            //update header
            $content_type["charset"] = $charset;
            $data->set(mime_build_rfc2822_header($content_type), $i, "content-type");

            //save body
            $data->set($body, $i, "#body");
            $_size  = strlen($body);
            $data->set($_size, $i, "#size");
            $size  += $_size;

            if ($status === CMS_POP_STATUS_BOUNDARY_END) $up();
        }
        while ($status !== CMS_POP_STATUS_MESSAGE_END);

        //close remaining containers
        while ($boundary_depth > 0) $up();

        //save total size
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

    private function execute($command)
    {
        if (fwrite($this->hfile, $command . "\r\n") === FALSE)
        {
            //error
            $this->error = TRUE;
            return FALSE;
        };

        return $this->receive() !== CMS_POP_STATUS_ERROR;
    }

//--------------------------------------------------------------------------------------------------

    private function receive($boundary = NULL)
    {
        if (
             (($this->response = fgets($this->hfile, 8192)) === FALSE)
             ||
             streq(substr($this->response, 0, 4), "-ERR")
           )
        {
            //error
            $this->error = TRUE;
            return CMS_POP_STATUS_ERROR;
        };

        //end of message
        if ($this->response === ".\r\n") return CMS_POP_STATUS_MESSAGE_END;

        //mime boundary
        if ($boundary !== NULL)
        {
            switch ($this->response)
            {
            case "--$boundary\r\n":   return CMS_POP_STATUS_BOUNDARY;
            case "--$boundary--\r\n": return CMS_POP_STATUS_BOUNDARY_END;
            };
        };

        //byte stuffing
        if (substr($this->response, 0, 2) === "..")
            $this->response = substr($this->response, 1);

        return CMS_POP_STATUS_OK;
    }

//--------------------------------------------------------------------------------------------------

    function quit()
    {
        if (! $this->enabled) return FALSE;
        $this->enabled = FALSE;

        $return = $this->error ? TRUE : $this->execute("QUIT");
        if ($this->hfile) fclose($this->hfile);

        return $return;
    }
};