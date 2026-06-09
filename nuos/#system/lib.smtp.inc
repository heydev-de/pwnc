<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   SMTP CLASS
//==================================================================================================

//related functions
//--------------------------------------------------------------------------------------------------

function smtp_send($to,
                   $subject,
                   $body,
                   $html     = FALSE,
                   $reply_to = NULL)
{
    //build message
    if (! cms_load("mime")) return FALSE;
    $mime = new mime();

    //html message
    if ($html)
    {
        $key = $mime->add_multipart(
            "alternative", //subtype
            $subject);     //subject

        $mime->add_text(
            NULL,                      //subject
            $body,                     //body
            "html",                    //subtype
            CMS_MIME_CHARSET_UTF_8,    //charset
            $key);                     //parent_key

        $mime->add_text(
            NULL,                      //subject
            htmltoplain($body, TRUE),  //body
            "plain",                   //subtype
            CMS_MIME_CHARSET_UTF_8,    //charset
            $key);                     //parent_key
    }

    //plain message
    else
    {
        $mime->add_text(
            $subject, //subject
            $body);   //body
    };

    //open connection
    $smtp = new smtp();
    if (! $smtp->enabled) return FALSE;

    //reply-to header
    switch ($reply_to)
    {
    case NULL:

        $reply_to = (new system())->getval("email", "reply_to");
        break;

    case FALSE:

        $reply_to = NULL;
        break;
    };

    //send message
    $return = $smtp->send(
        $mime,           //mime
        CMS_EMAIL_AGENT, //from
        $to,             //to
        NULL,            //cc
        NULL,            //bcc
        $reply_to);      //reply_to

    //close connection
    $smtp->quit();

    return $return;
};

class smtp
{

//properties
//--------------------------------------------------------------------------------------------------

    public $hfile    = NULL;
    public $enabled  = NULL;
    public $mail     = NULL;
    public $username = NULL;
    public $password = NULL;
    public $response = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($host     = NULL,
                         $username = NULL,
                         $password = NULL)
    {
        $this->enabled = TRUE;

        if (stre($host))
        {
            $system = new system();

            //use mail function
            if ($this->mail = streq($system->getval("email", "method"), "mail")) return;

            $host     = $system->getval("email", "smtp");
            $username = $system->getval("email", "username");
            $password = $system->getval("email", "password");
        };

        if (! preg_match("/^([^:]+)(?::([0-9]+))?$/", $host, $match, PREG_UNMATCHED_AS_NULL))
        {
            $this->response = sprintf(CMS_L_SMTP_002, $host);
            $this->enabled  = FALSE;
            return;
        };

        $host           = $match[1];
        $port           = (int)($match[2] ?? 25);
        $this->username = $username;
        $this->password = $password;

        //open connection
        if (($this->hfile = fsockopen($host, $port)) === FALSE)
        {
            $this->response = sprintf(CMS_L_SMTP_001, $host);
            $this->enabled  = FALSE;
            return;
        };

        if (

        //initial response
             (($line = $this->receive_line()) !== FALSE)
             &&
             streq($line[0], 220)
             &&

        //authentication
             $this->authenticate()
           )
            return;

        $this->enabled = FALSE;
        fclose($this->hfile);
    }

//methods
//--------------------------------------------------------------------------------------------------

    function authenticate()
    {
        if ($this->mail) return FALSE;

        //initial handshake
        if (($response = $this->send_line(250, "EHLO " . CMS_DOMAIN)) === FALSE)
            return $this->send_line(250, "HELO " . CMS_DOMAIN) !== FALSE;

        //starttls support
        if (
             extension_loaded("openssl")
             &&
             preg_match("/(?:^|\r\n)STARTTLS(?:$|\r\n)/i", $response)
           )
        {
            if ($this->send_line(220, "STARTTLS") !== FALSE)
            {
                //enable encryption
                if (! stream_socket_enable_crypto($this->hfile, TRUE, STREAM_CRYPTO_METHOD_TLS_CLIENT)) return FALSE;

                //reinitiate handshake
                if (($response = $this->send_line(250, "EHLO " . CMS_DOMAIN)) === FALSE) return FALSE;
            };
        };

        //get sasl authentication methods
        if (! preg_match("/(?:^|\r\n)AUTH (.*)(?:$|\r\n)/i", $response, $response)) return TRUE;

        $response = $response[1];

        //anonymous access
        if (preg_match("/(?:^| )ANONYMOUS(?:$| )/i", $response)) return TRUE;

        //cram-md5 authentication
        if (preg_match("/(?:^| )CRAM-MD5(?:$| )/i", $response))
        {
            if (($message = $this->send_line(334, "AUTH CRAM-MD5")) !== FALSE)
            {
                $message = base64_decode($message);
                $message = hmac_md5($message, $this->password);
                $message = base64_encode($this->username . " " . $message);

                if ($this->send_line(235, $message) !== FALSE) return TRUE;
            };
        }

        //login authentication
        if (preg_match("/(?:^| )LOGIN(?:$| )/i", $response))
        {
            if (
                 ($this->send_line(334, "AUTH LOGIN") !== FALSE)
                 &&
                 ($this->send_line(334, base64_encode($this->username)) !== FALSE)
                 &&
                 ($this->send_line(235, base64_encode($this->password)) !== FALSE)
               )
                return TRUE;
        };

        //plain authentication
        if (preg_match("/(?:^| )PLAIN(?:$| )/i", $response))
        {
            $credentials = base64_encode(
                "\0" . $this->username .
                "\0" . $this->password);
            if ($this->send_line(235, "AUTH PLAIN $credentials") !== FALSE) return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function send(&$mime,
                  $from     = NULL,
                  $to       = NULL,
                  $cc       = NULL,
                  $bcc      = NULL,
                  $reply_to = NULL)
    {
        if (! $this->enabled)   return FALSE;
        if (! cms_load("mime")) return FALSE;

        $key = $mime->data->move("first"); //main header

        //get sender address
        $sender = nstre($from) ? $from : $mime->data->get($key, "from");
        $sender = mime_extract_rfc2822_address($sender);
        $sender = key(reset($sender));

        //get reply-to address
        $reply_to = nstre($reply_to) ? $reply_to : $mime->data->get($key, "reply-to");

        if ($this->mail)
        {
            //get recipient address
            if (stre($to)) $to = $mime->data->get($key, "to");

            //extract and remove subject
            $subject = mime_encode_header($mime->data->get($key, "subject"));
            $mime->data->del($key, "subject");

            //build message
            $message = $mime->build($from, NULL, $cc, $bcc, $reply_to);
            if (preg_match("/^win/i", PHP_OS)) //windows only
                $message = preg_replace("/\r?\n(\.)?/", "\r\n\\1\\1", $message);

            //split header and body
            list($header, $body) = explode("\r\n\r\n", $message, 2);

            //send
            if (mail($to, $subject, $body, $header, escapeshellarg("-f" . $sender))) return TRUE;

            //error
            $this->response = CMS_L_SMTP_003;
            return FALSE;
        };

        //initiate message
        if ($this->send_line(250, "MAIL FROM:<$sender>") === FALSE) return FALSE;

        //get recipient address
        $recipient = (nstre($to)  ? $to  : $mime->data->get($key, "to")) . "," .
                     (nstre($cc)  ? $cc  : $mime->data->get($key, "cc")) . "," .
                     (nstre($bcc) ? $bcc : $mime->data->get($key, "bcc"));
        $recipient = mime_extract_rfc2822_address($recipient);

        foreach ($recipient AS $key => $value)
        {
            foreach ($value AS $key => $_)
            {
                if ($this->send_line(250, "RCPT TO:<$key>") === FALSE) return FALSE;
            };
        };

        //build message
        $message = $mime->build($from, $to, $cc, $bcc, $reply_to);
        $message = preg_replace("/\r?\n(\.)?/", "\r\n\\1\\1", $message);

        //send
        return ($this->send_line(354, "DATA") !== FALSE)
               &&
               (fwrite($this->hfile, $message) !== FALSE)
               &&
               ($this->send_line(250, ".") !== FALSE);
    }

//--------------------------------------------------------------------------------------------------

    function receive_line()
    {
        if (! $this->enabled) return FALSE;
        if ($this->mail)      return FALSE;

        $message = NULL;

        while (
                (($response = fgets($this->hfile, 1024)) !== FALSE)
                &&
                preg_match("/^([0-9]{3})(-| )(.*)\r\n$/", $response, $response)
              )
        {
            $code     = $response[1];
            $message .= $response[3] . "\r\n";

            if ($response[2] === " ")
            {
                $message        = substr($message, 0, -2);
                $this->response = "$code $message";
                return [$code, $message];
            };
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function send_line($code,
                       $message)
    {
        if (! $this->enabled) return FALSE;
        if ($this->mail)      return FALSE;

        fwrite($this->hfile, "$message\r\n");
        $message = $this->receive_line();

        if ($message === FALSE) return FALSE;
        return streq($message[0], $code) ?
               (nstre($message[1]) ? $message[1] : TRUE) :
               FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function quit()
    {
        if ($this->mail) return TRUE;

        if ($this->enabled)
        {
            $this->enabled = FALSE;
            $this->send_line(221, "QUIT");
            fclose($this->hfile);
        };

        return TRUE;
    }
};