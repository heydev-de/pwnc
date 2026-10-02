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
            NULL,   //subject
            $body,  //body
            "html", //subtype
            $key);  //parent_key

        $mime->add_text(
            NULL,                      //subject
            htmltoplain($body, TRUE),  //body
            "plain",                   //subtype
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
            $this->response = sprintf(t("err_host_invalid"), $host);
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
            $this->response = sprintf(t("err_host_unavailable"), $host);
            $this->enabled  = FALSE;
            return;
        };

        //timeout
        stream_set_timeout($this->hfile, 30);

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
        $sender = $from ?? ($mime->data->get($key, "from") ?? "");
        $sender = mime_extract_rfc2822_address($sender);
        $sender = key(reset($sender) ?: []);

        //no sender
        if ($sender === NULL)
        {
            $this->response = t("err_sender_missing");
            return FALSE;
        };

        //php mail
        if ($this->mail)
        {
            //get recipient addresses
            $to    ??= $mime->data->get($key, "to") ?? "";
            $to      = mime_extract_rfc2822_address($to);
            $array   = [];

            //extract
            foreach ($to AS $group => $address)
            {
                foreach ($address AS $email => $name) $array[$email] = TRUE;
            };

            //prepare recipient list for mail function
            $to = implode(", ", array_keys($array));

            //extract and remove subject
            $subject = $mime->data->get($key, "subject") ?? "";
            $subject = mime_convert_header("subject", $subject);
            $mime->data->del($key, "subject");

            //build message
            $message = $mime->build($from, NULL, $cc, $bcc, $reply_to);

            if (preg_match("/^win/i", PHP_OS)) //windows only

                //replace all \n with \r\n and double all dots at line start
                $message = preg_replace("/\r?\n(\.)?/", "\r\n\\1\\1", $message);

            //split header and body
            list($header, $body) = explode("\r\n\r\n", $message, 2);

            //send
            if (mail($to, $subject, $body, $header, "-f" . escapeshellarg($sender))) return TRUE;

            //error
            $this->response = t("the_mail_function_failed");
            return FALSE;
        };

        //set sender
        if ($this->send_line(250, "MAIL FROM:<$sender>") === FALSE) return FALSE;

        //get recipient addresses
        $recipient = ($to  ?? $mime->data->get($key, "to")) . "," .
                     ($cc  ?? $mime->data->get($key, "cc")) . "," .
                     ($bcc ?? $mime->data->get($key, "bcc"));
        $recipient = mime_extract_rfc2822_address($recipient);
        $array     = [];

        //extract
        foreach ($recipient AS $group => $address)
        {
            foreach ($address AS $email => $name) $array[$email] = TRUE;
        };

        //no recipients
        if (! $array)
        {
            $this->response = t("err_recipient_missing");
            return FALSE;
        };

        //set recipients
        foreach ($array AS $email => $_)
        {
            if ($this->send_line(250, "RCPT TO:<$email>") === FALSE) return FALSE;
        };

        //remove bcc header
        $mime->data->del($key, "bcc");

        //build message
        $message = $mime->build($from, $to, $cc, NULL, $reply_to);

        //replace all \n with \r\n and double all dots at line start
        $message = preg_replace("/\r?\n(\.)?/", "\r\n\\1\\1", $message);

        //send data header
        if ($this->send_line(354, "DATA") === FALSE) return FALSE;

        //send data
        if (fwrite($this->hfile, $message) === FALSE)
        {
            $this->response = t("a_connection_error_occurred");
            return FALSE;
        };

        //send end of message marker
        if ($this->send_line(250, ".") === FALSE) return FALSE;

        return TRUE;
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