<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   SEARCH CLASS
//==================================================================================================

//permission
define("CMS_SEARCH_PERMISSION_SUBMIT",        "submit");

//database
define("CMS_DB_SEARCH_ENTRY",                 CMS_DB_PREFIX . "search_entry");
define("CMS_DB_SEARCH_ENTRY_INDEX",           "id");
define("CMS_DB_SEARCH_ENTRY_ADDRESS",         "address");
define("CMS_DB_SEARCH_ENTRY_ADDRESS_HASH",    "address_hash");
define("CMS_DB_SEARCH_ENTRY_TITLE",           "title");
define("CMS_DB_SEARCH_ENTRY_TEXT",            "text");
define("CMS_DB_SEARCH_ENTRY_TEXT_HASH",       "text_hash");
define("CMS_DB_SEARCH_ENTRY_TIME",            "time");
define("CMS_DB_SEARCH_ENTRY_UPDATE_INTERVAL", "update_interval");
define("CMS_DB_SEARCH_ENTRY_UPDATE_TIME",     "update_time");
define("CMS_DB_SEARCH_ENTRY_SCORE",           "score");
define("CMS_DB_SEARCH_ENTRY_LINK_COUNT",      "link_count");
define("CMS_DB_SEARCH_ENTRY_ERROR",           "error");
define("CMS_DB_SEARCH_ENTRY_CANONICAL",       "canonical");

define("CMS_DB_SEARCH_WORD",                  CMS_DB_PREFIX . "search_word");
define("CMS_DB_SEARCH_WORD_INDEX",            "id");
define("CMS_DB_SEARCH_WORD_TEXT",             "text");
define("CMS_DB_SEARCH_WORD_LANGUAGE",         "language");

define("CMS_DB_SEARCH_WEIGHT",                CMS_DB_PREFIX . "search_weight");
define("CMS_DB_SEARCH_WEIGHT_WORD",           "word");
define("CMS_DB_SEARCH_WEIGHT_SOURCE",         "source");
define("CMS_DB_SEARCH_WEIGHT_TARGET",         "target");
define("CMS_DB_SEARCH_WEIGHT_VALUE",          "value");

define("CMS_DB_SEARCH_LINK",                  CMS_DB_PREFIX . "search_link");
define("CMS_DB_SEARCH_LINK_SOURCE",           "source");
define("CMS_DB_SEARCH_LINK_TARGET_HASH",      "target_hash");
define("CMS_DB_SEARCH_LINK_TARGET",           "target");
define("CMS_DB_SEARCH_LINK_TEXT",             "text");
define("CMS_DB_SEARCH_LINK_LEVEL",            "level");

define("CMS_DB_SEARCH_CLUSTER",               CMS_DB_PREFIX . "search_cluster");
define("CMS_DB_SEARCH_CLUSTER_SOURCE",        "source");
define("CMS_DB_SEARCH_CLUSTER_TARGET",        "target");

define("CMS_DB_SEARCH_QUEUE",                 CMS_DB_PREFIX . "search_queue");
define("CMS_DB_SEARCH_QUEUE_ADDRESS",         "address");
define("CMS_DB_SEARCH_QUEUE_TYPE",            "type");
define("CMS_DB_SEARCH_QUEUE_TIME",            "time");
define("CMS_DB_SEARCH_QUEUE_CODE",            "code");
define("CMS_DB_SEARCH_QUEUE_ERROR",           "error");
define("CMS_DB_SEARCH_QUEUE_DONE",            "done");

//scan result code
define("CMS_SEARCH_SCAN_UNKNOWN_ERROR",                  1);
define("CMS_SEARCH_SCAN_REDIRECTION_LIMIT_EXCEEDED",     2);
define("CMS_SEARCH_SCAN_INVALID_ADDRESS",                4);
define("CMS_SEARCH_SCAN_ADDRESS_REJECTED",               8);
define("CMS_SEARCH_SCAN_NO_CONNECTION",                 16);
define("CMS_SEARCH_SCAN_UNSUPPORTED_RESOURCE_FORMAT",   32);
define("CMS_SEARCH_SCAN_NO_MODIFICATION",               64);
define("CMS_SEARCH_SCAN_DATA_FETCH_FAILED",            128);
define("CMS_SEARCH_SCAN_NO_CONTENT",                   256);
define("CMS_SEARCH_SCAN_INDEXING_UNDESIRED",           512);
define("CMS_SEARCH_SCAN_INDEXED",                     1024);
define("CMS_SEARCH_SCAN_INDEXING_FAILED",             2048);
define("CMS_SEARCH_SCAN_DATABASE_ERROR",              4096);
define("CMS_SEARCH_SCAN_FATAL_ERROR",                 CMS_SEARCH_SCAN_REDIRECTION_LIMIT_EXCEEDED  |
                                                      CMS_SEARCH_SCAN_INVALID_ADDRESS             |
                                                    //CMS_SEARCH_SCAN_ADDRESS_REJECTED            |
                                                      CMS_SEARCH_SCAN_UNSUPPORTED_RESOURCE_FORMAT |
                                                      CMS_SEARCH_SCAN_NO_CONTENT);
define("CMS_SEARCH_SCAN_ERROR",                       CMS_SEARCH_SCAN_FATAL_ERROR                 |
                                                      CMS_SEARCH_SCAN_UNKNOWN_ERROR               |
                                                      CMS_SEARCH_SCAN_NO_CONNECTION               |
                                                      CMS_SEARCH_SCAN_DATA_FETCH_FAILED           |
                                                      CMS_SEARCH_SCAN_INDEXING_FAILED             |
                                                      CMS_SEARCH_SCAN_DATABASE_ERROR);

//queue type
define("CMS_SEARCH_QUEUE_TYPE_NONE",                     0);
define("CMS_SEARCH_QUEUE_TYPE_INTERNAL",                 1);
define("CMS_SEARCH_QUEUE_TYPE_SELECTION",                2);
define("CMS_SEARCH_QUEUE_TYPE_SUBMISSION",               4);
define("CMS_SEARCH_QUEUE_TYPE_REFERENCE",                8);
define("CMS_SEARCH_QUEUE_TYPE_UPDATE",                  16);
define("CMS_SEARCH_QUEUE_TYPE_ALL",                    255);

class search
{

//properties
//--------------------------------------------------------------------------------------------------

    public $enabled                          = FALSE;

    //list of regular expressions used for address exclusion
    public $address_accepted_blacklist       = [];

    //list of regular expressions used for address inclusion
    //applied after blacklist check
    public $address_accepted_whitelist       = [];

    //maximum number of permitted subsequent address redirects
    public $scan_routing_limit               = 5;

    //time in seconds that defines the initial requeue interval
    public $update_interval                  = 86400;  //1 day

    //time in seconds that defines the minimum requeue interval
    public $update_interval_min              = 3600;   //1 hour

    //time in seconds that defines the maximum requeue interval
    public $update_interval_max              = 604800; //1 week

    //factor that decreases the requeue interval
    public $update_interval_decrease_factor  = 0.5;

    //factor that increases the requeue interval
    public $update_interval_increase_factor  = 1.5;

    //maximum number of simhash bit differences for assuming similarity
    public $entry_set_maximum_bit_difference = 2;

    //maximum weight of indexed information types
    public $entry_set_weight_factor_title    = 0.1;
    public $entry_set_weight_factor_h1       = 1.3;
    public $entry_set_weight_factor_h2       = 1.2;
    public $entry_set_weight_factor_h3       = 1.1;
    public $entry_set_weight_factor_copy     = 1.0;
    public $entry_set_weight_factor_side     = 0.1;
    public $entry_set_weight_factor_address  = 0.1;
    public $entry_set_weight_factor_link     = 0.1;

    //number of subsequent remove calls before entry is removed permanently
    public $entry_remove_error_limit         = 3;

    //time in seconds after which a failed scan is retried
    public $queue_process_retry_time         = 3600; //1 hour

    //number of retries on failure
    public $queue_process_error_limit        = 3;

    //number of results to display per page
    public $find_results_per_page            = 10;

    //number of score refinement iterations
    public $score_compute_iteration_number   = 10;

    //inherited portion of score
    public $score_compute_dampening_factor   = 0.85;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct()
    {
        //establish database connection
        $mysql = new mysql();

        //verify database
        if (
             $mysql->verify_table(CMS_DB_SEARCH_ENTRY,
                                  [CMS_DB_SEARCH_ENTRY_INDEX           => "INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT",
                                   CMS_DB_SEARCH_ENTRY_ADDRESS         => "VARBINARY(255) NOT NULL DEFAULT _binary'' UNIQUE",
                                   CMS_DB_SEARCH_ENTRY_ADDRESS_HASH    => "BINARY(16) NOT NULL DEFAULT _binary'' UNIQUE",
                                   CMS_DB_SEARCH_ENTRY_TITLE           => "VARCHAR(255) NOT NULL DEFAULT ''",
                                   CMS_DB_SEARCH_ENTRY_TEXT            => "TEXT NOT NULL",
                                   CMS_DB_SEARCH_ENTRY_TEXT_HASH       => "BIT(64) NOT NULL DEFAULT b'" .
                                                                          "00000000000000000000000000000000" .
                                                                          "00000000000000000000000000000000'",
                                   CMS_DB_SEARCH_ENTRY_TIME            => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_SEARCH_ENTRY_UPDATE_INTERVAL => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_SEARCH_ENTRY_UPDATE_TIME     => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_SEARCH_ENTRY_SCORE           => "FLOAT UNSIGNED NOT NULL DEFAULT '1.0'",
                                   CMS_DB_SEARCH_ENTRY_LINK_COUNT      => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_SEARCH_ENTRY_ERROR           => "TINYINT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_SEARCH_ENTRY_CANONICAL       => "TINYINT(1) UNSIGNED NOT NULL DEFAULT '0'"],
                                  ["INDEX (" . CMS_DB_SEARCH_ENTRY_UPDATE_TIME . ")",
                                   "INDEX (" . CMS_DB_SEARCH_ENTRY_CANONICAL . ", " .
                                               CMS_DB_SEARCH_ENTRY_SCORE . ", " .
                                               CMS_DB_SEARCH_ENTRY_INDEX . ")",
                                   "INDEX (" . CMS_DB_SEARCH_ENTRY_CANONICAL . ", " .
                                               CMS_DB_SEARCH_ENTRY_TEXT_HASH . ")"])
             &&
             $mysql->verify_table(CMS_DB_SEARCH_WORD,
                                  [CMS_DB_SEARCH_WORD_INDEX            => "INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT",
                                   CMS_DB_SEARCH_WORD_TEXT             => "VARBINARY(255) NOT NULL DEFAULT _binary''",
                                   CMS_DB_SEARCH_WORD_LANGUAGE         => "VARCHAR(40) CHARACTER SET ascii COLLATE ascii_general_ci NOT NULL DEFAULT ''"],
                                  ["UNIQUE (" . CMS_DB_SEARCH_WORD_TEXT . ", " .
                                                CMS_DB_SEARCH_WORD_LANGUAGE . ")"])
             &&
             $mysql->verify_table(CMS_DB_SEARCH_WEIGHT,
                                  [CMS_DB_SEARCH_WEIGHT_WORD           => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_SEARCH_WEIGHT_SOURCE         => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_SEARCH_WEIGHT_TARGET         => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_SEARCH_WEIGHT_VALUE          => "FLOAT UNSIGNED NOT NULL DEFAULT '0.0'"],
                                  ["INDEX (" . CMS_DB_SEARCH_WEIGHT_WORD . ", " .
                                               CMS_DB_SEARCH_WEIGHT_TARGET . ", " .
                                               CMS_DB_SEARCH_WEIGHT_VALUE . ")",
                                   "UNIQUE (" . CMS_DB_SEARCH_WEIGHT_TARGET . ", " .
                                                CMS_DB_SEARCH_WEIGHT_SOURCE . ", " .
                                                CMS_DB_SEARCH_WEIGHT_WORD . ")"])
             &&
             $mysql->verify_table(CMS_DB_SEARCH_LINK,
                                  [CMS_DB_SEARCH_LINK_SOURCE           => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_SEARCH_LINK_TARGET_HASH      => "BINARY(16) NOT NULL DEFAULT _binary''",
                                   CMS_DB_SEARCH_LINK_TARGET           => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_SEARCH_LINK_TEXT             => "VARBINARY(255) NOT NULL DEFAULT _binary''",
                                   CMS_DB_SEARCH_LINK_LEVEL            => "TINYINT UNSIGNED NOT NULL DEFAULT '0'"],
                                  ["INDEX (" . CMS_DB_SEARCH_LINK_SOURCE . ", " .
                                               CMS_DB_SEARCH_LINK_LEVEL . ")",
                                   "INDEX (" . CMS_DB_SEARCH_LINK_TARGET_HASH . ")",
                                   "INDEX (" . CMS_DB_SEARCH_LINK_TARGET . ")",
                                   "INDEX (" . CMS_DB_SEARCH_LINK_LEVEL . ", " .
                                               CMS_DB_SEARCH_LINK_SOURCE . ", " .
                                               CMS_DB_SEARCH_LINK_TARGET . ")"])
             &&
             $mysql->verify_table(CMS_DB_SEARCH_CLUSTER,
                                  [CMS_DB_SEARCH_CLUSTER_SOURCE        => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_SEARCH_CLUSTER_TARGET        => "INT UNSIGNED NOT NULL DEFAULT '0'"],
                                  ["UNIQUE (" . CMS_DB_SEARCH_CLUSTER_SOURCE . ", " .
                                                CMS_DB_SEARCH_CLUSTER_TARGET . ")"])
             &&
             $mysql->verify_table(CMS_DB_SEARCH_QUEUE,
                                  [CMS_DB_SEARCH_QUEUE_ADDRESS         => "VARBINARY(255) NOT NULL DEFAULT _binary'' UNIQUE",
                                   CMS_DB_SEARCH_QUEUE_TYPE            => "TINYINT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_SEARCH_QUEUE_TIME            => "INT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_SEARCH_QUEUE_CODE            => "BINARY(8) NOT NULL DEFAULT _binary''",
                                   CMS_DB_SEARCH_QUEUE_ERROR           => "TINYINT UNSIGNED NOT NULL DEFAULT '0'",
                                   CMS_DB_SEARCH_QUEUE_DONE            => "TINYINT(1) UNSIGNED NOT NULL DEFAULT '0'"],
                                  ["INDEX (" . CMS_DB_SEARCH_QUEUE_CODE . ")",
                                   "INDEX (" . CMS_DB_SEARCH_QUEUE_DONE . ", " .
                                               CMS_DB_SEARCH_QUEUE_TYPE . ", " .
                                               CMS_DB_SEARCH_QUEUE_TIME . ")"])
           )
        {
            //load settings
            $system = new system();

            $this->address_accepted_blacklist = preg_split("/\r?\n/", $system->getval("search", "blacklist"),
                                                           -1, PREG_SPLIT_NO_EMPTY);
            $this->address_accepted_whitelist = preg_split("/\r?\n/", $system->getval("search", "whitelist"),
                                                           -1, PREG_SPLIT_NO_EMPTY);

            if (($temp = $system->getval("search", "difference")) !== NULL)
                $this->entry_set_maximum_bit_difference = (int)$temp;

            $this->enabled = TRUE;
        };
    }

//methods
//--------------------------------------------------------------------------------------------------

    function scan($address,
                  $follow_links = FALSE)
    {
        if (! $this->enabled)   return CMS_SEARCH_SCAN_UNKNOWN_ERROR;
        if (! cms_load("html")) return CMS_SEARCH_SCAN_UNKNOWN_ERROR;
        if (! cms_load("http")) return CMS_SEARCH_SCAN_UNKNOWN_ERROR;

        //standardize address
        if (($address = $this->address_standardize($address)) === FALSE)
            return CMS_SEARCH_SCAN_INVALID_ADDRESS;

        //check if entry is updated
        if (
             ($result = mysql_query("SELECT " .   CMS_DB_SEARCH_ENTRY_INDEX . ", " .
                                                  CMS_DB_SEARCH_ENTRY_TIME . " " .
                                    "FROM " .     CMS_DB_SEARCH_ENTRY . " " .
                                    "WHERE " .    CMS_DB_SEARCH_ENTRY_ADDRESS_HASH .
                                    "=_binary'" . sqlesc(hash32($address, TRUE)) . "' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_row($result))
           )
        {
            //scan
            $value = $this->_scan($address,       //address
                                  $follow_links,  //follow links
                                  $resultrow[1]); //entry time

            //remove on error
            if (CMS_SEARCH_SCAN_ERROR & $value)
                $this->entry_remove($resultrow[0],
                                    (bool)(CMS_SEARCH_SCAN_FATAL_ERROR & $value));

            return $value;
        };

        //scan
        return $this->_scan($address, $follow_links);
    }

//--------------------------------------------------------------------------------------------------

    private function _scan($address,
                           $follow_links,
                           $entry_time = 0)
    {
        //connect and retrieve header
        $_address = $address;
        $i        = 0;

        while (TRUE)
        {
            //check address admission
            if ($this->address_accepted($_address) === FALSE)
                return CMS_SEARCH_SCAN_ADDRESS_REJECTED;

            //retrieve http header
            if ((! $header = http_header($_address)) || is_string($header))
                return CMS_SEARCH_SCAN_NO_CONNECTION;

            //rerouting
            if (isset($header["location"]))
            {
                //exit multiple routings
                if (++$i > $this->scan_routing_limit)
                    return CMS_SEARCH_SCAN_REDIRECTION_LIMIT_EXCEEDED;

                //standardize address
                if (($_address = $this->address_standardize($header["location"])) === FALSE)
                    return CMS_SEARCH_SCAN_INVALID_ADDRESS;

                continue;
            };

            break;
        };

        //check robots header directive
        if (isset($header["x-robots-tag"]) && preg_match("/\b(?:noindex|none)\b/i", $header["x-robots-tag"]))
            return CMS_SEARCH_SCAN_INDEXING_UNDESIRED;

        //verify resource type
        if (isset($header["content-type"]) && cms_load("mime"))
        {
            $content_type = mime_extract_rfc2822_header($header["content-type"]);
            $content_type = strtolower($content_type["#value"]);

            switch ($content_type)
            {
            case "text/html":
            case "application/xhtml+xml":
            case "application/xml":

                break;

            default:

                return CMS_SEARCH_SCAN_UNSUPPORTED_RESOURCE_FORMAT;
            };
        };

        //address is reachable -> reset error count
        mysql_query("UPDATE " .   CMS_DB_SEARCH_ENTRY . " " .
                    "SET " .      CMS_DB_SEARCH_ENTRY_ERROR .
                    "='" .       "0' " .
                    "WHERE " .    CMS_DB_SEARCH_ENTRY_ADDRESS_HASH .
                    "=_binary'" . sqlesc(hash32($address, TRUE)) . "' " .
                    "LIMIT 1");

        //verify modification date
        $current_time = time();

        if (
             (! isset($header["last-modified"]))
             ||
             (! $header["last-modified"] = strtotime($header["last-modified"]))
           )
            $header["last-modified"] = $current_time;

        //check if modified
        if ($entry_time >= $header["last-modified"])
            return CMS_SEARCH_SCAN_NO_MODIFICATION;

        //read content
        if (! $data = html_page_info($_address))
            return CMS_SEARCH_SCAN_DATA_FETCH_FAILED;

        //verify content
        if (stre($data["text"]))
            return CMS_SEARCH_SCAN_NO_CONTENT;

        //add links to queue
        if (
             $follow_links
             &&
             (! (isset($header["x-robots-tag"]) && preg_match("/\b(?:nofollow|none)\b/i", $header["x-robots-tag"])))
             &&
             (! (isset($data["meta"]["robots"]) && preg_match("/\b(?:nofollow|none)\b/i", $data["meta"]["robots"])))
           )
        {
            foreach ($data["links"] AS $value)
                $this->queue_add($value["url"], CMS_SEARCH_QUEUE_TYPE_REFERENCE);
        };

        //check robots directive
        if (isset($data["meta"]["robots"]) && preg_match("/\b(?:noindex|none)\b/i", $data["meta"]["robots"]))
            return CMS_SEARCH_SCAN_INDEXING_UNDESIRED;

        //title
        if (stre($data["title"])) $data["title"] = $address;

        //language
        if (isset($data["meta"]["content-language"]))
            $language = $data["meta"]["content-language"];
        elseif (isset($header["content-language"]))
            $language = $header["content-language"];
        else
            $language = language_detect($data["title"] . " " . $data["text"] . " " . $data["side"]);

        $data_language = new data("#system/language");
        $language_list = implode(",", array_keys($data_language->data));
        $language      = cms_language_extract($language, $language_list);

        //index
        $value = $this->entry_set($address, $data, $language);

        return is_string($value) ? CMS_SEARCH_SCAN_DATABASE_ERROR :
               ($value ? CMS_SEARCH_SCAN_INDEXED : CMS_SEARCH_SCAN_INDEXING_FAILED);
    }

//--------------------------------------------------------------------------------------------------

    function entry_set($address,
                       $data,
                       $language,
                       $links = NULL)
    {
        if (! $this->enabled) return FALSE;

        //standardize and check address admission
        if (($address = $this->address_standardize($address)) === FALSE) return FALSE; //invalid address
        if ($this->address_accepted($address) === FALSE)                 return TRUE;  //address rejected

        //prepare values
        $address_hash = hash32($address, TRUE);
        $title        = first_words($data["title"], 255);
        $text         = first_words($data["text"], 65535);
        $text_hash    = simhash($text);
        $time         = time();
        if (! is_array($links)) $links = [];
        $link_count   = 0;

        //preprocess outbound links
        $_links = [];

        foreach ($data["links"] AS $value)
        {
            //standardize link address
            $link_url = $this->address_standardize($value["url"]);
            if ($link_url === FALSE) continue; //invalid address

            //hash link address
            $link_url = hash32($link_url, TRUE);
            if (streq($link_url, $address_hash)) continue; //link to self

            //cleanse link text
            $link_text = htmltoplain($value["text"]);
            $link_text = preg_match("/^\s*(.{0,254}\S)(?:$|\s)/s", $link_text, $link_text) ?
                         $link_text[1] : "";

            //keep link
            $_links[] = [0 => $link_url,
                         1 => $link_text];
            $link_count++;
        };

        //check if entry is already indexed
        $query = "SELECT " .   CMS_DB_SEARCH_ENTRY_INDEX . ", " .
                               CMS_DB_SEARCH_ENTRY_TITLE . ", " .
                               CMS_DB_SEARCH_ENTRY_TEXT . ", " .
                 "LPAD(BIN(" . CMS_DB_SEARCH_ENTRY_TEXT_HASH . "), 64, '0') " .
                 "FROM " .     CMS_DB_SEARCH_ENTRY . " " .
                 "WHERE " .    CMS_DB_SEARCH_ENTRY_ADDRESS_HASH .
                 "=_binary'" . sqlesc($address_hash) . "' " .
                 "LIMIT 1";

        //execute
        $result = mysql_query($query);
        if (mysql_errno() !== 0) return mysql_error(); //error check

        //process existing entry
        if ($resultrow = mysql_fetch_row($result))
        {
            $entry_index  = $resultrow[0];
            $_entry_index = sqlesc($entry_index);

            //check if entry has changed
            if (
                 streq($title, $resultrow[1])
                 &&
                 streq($text, $resultrow[2])
               )
            {
                //increase requeue interval
                mysql_query("UPDATE " . CMS_DB_SEARCH_ENTRY . " " .
                            "SET " .    CMS_DB_SEARCH_ENTRY_TIME .
                            "='" .      sqlesc($time) . "', " .
                                        CMS_DB_SEARCH_ENTRY_UPDATE_INTERVAL .
                            "=LEAST(" . CMS_DB_SEARCH_ENTRY_UPDATE_INTERVAL .
                            "*" .       (float)$this->update_interval_increase_factor . ", " .
                                        (int)$this->update_interval_max . "), " .
                                        CMS_DB_SEARCH_ENTRY_UPDATE_TIME .
                            "=(" .      CMS_DB_SEARCH_ENTRY_TIME .
                            "+" .       CMS_DB_SEARCH_ENTRY_UPDATE_INTERVAL . ") " .
                            "WHERE " .  CMS_DB_SEARCH_ENTRY_INDEX .
                            "='" .      $_entry_index . "' " .
                            "LIMIT 1");

                return TRUE; //no change, skip update
            };

            //remove previous word weights, inbound and outbound
            mysql_query("DELETE FROM " . CMS_DB_SEARCH_WEIGHT . " " .
                        "WHERE " .       CMS_DB_SEARCH_WEIGHT_SOURCE .
                        "='" .           $_entry_index . "' " .
                        "OR " .          CMS_DB_SEARCH_WEIGHT_TARGET .
                        "='" .           $_entry_index . "'");
            if (mysql_errno() !== 0) return mysql_error(); //error check

            //remove previous outbound links
            mysql_query("DELETE FROM " . CMS_DB_SEARCH_LINK . " " .
                        "WHERE " .       CMS_DB_SEARCH_LINK_SOURCE .
                        "='" .           $_entry_index . "'");
            if (mysql_errno() !== 0) return mysql_error(); //error check

            //remove previous cluster associations
            mysql_query("DELETE FROM " . CMS_DB_SEARCH_CLUSTER . " " .
                        "WHERE " .       CMS_DB_SEARCH_CLUSTER_SOURCE .
                        "='" .           $_entry_index . "' " .
                        "OR " .          CMS_DB_SEARCH_CLUSTER_TARGET .
                        "='" .           $_entry_index . "'");
            if (mysql_errno() !== 0) return mysql_error(); //error check

            //compute requeue factor by degree of modification
            $delta  = hamming_distance($text_hash, $resultrow[3]);
            $factor = ($delta <= $this->entry_set_maximum_bit_difference) ?
                      $this->update_interval_increase_factor :
                      $this->update_interval_decrease_factor;

            //update entry, decrease requeue interval
            $query = "UPDATE " .    CMS_DB_SEARCH_ENTRY . " " .
                     "SET " .       CMS_DB_SEARCH_ENTRY_TITLE .
                     "='" .         sqlesc($title) . "', " .
                                    CMS_DB_SEARCH_ENTRY_TEXT .
                     "='" .         sqlesc($text) . "', " .
                                    CMS_DB_SEARCH_ENTRY_TEXT_HASH .
                     "=b'" .        sqlesc($text_hash) . "', " .
                                    CMS_DB_SEARCH_ENTRY_TIME .
                     "='" .         sqlesc($time) . "', " .
                                    CMS_DB_SEARCH_ENTRY_UPDATE_INTERVAL .
                     "=GREATEST(" . (int)$this->update_interval_min . ", " .
                     "LEAST(" .     (int)$this->update_interval_max . ", " .
                                    CMS_DB_SEARCH_ENTRY_UPDATE_INTERVAL .
                     "*" .          (float)$factor . ")), " .
                                    CMS_DB_SEARCH_ENTRY_UPDATE_TIME .
                     "=(" .         CMS_DB_SEARCH_ENTRY_TIME .
                     "+" .          CMS_DB_SEARCH_ENTRY_UPDATE_INTERVAL . "), " .
                                    CMS_DB_SEARCH_ENTRY_LINK_COUNT .
                     "='" .         sqlesc($link_count) . "', " .
                                    CMS_DB_SEARCH_ENTRY_ERROR .
                     "='" .        "0' " .
                     "WHERE " .     CMS_DB_SEARCH_ENTRY_INDEX .
                     "='" .         $_entry_index . "' " .
                     "LIMIT 1";

            //execute
            mysql_query($query);
            if (mysql_errno() !== 0) return mysql_error(); //error check
        }

        //new entry
        else
        {
            //look for similar canonical entry
            $result    = mysql_query("SELECT " .        CMS_DB_SEARCH_ENTRY_INDEX . " " .
                                     "FROM " .          CMS_DB_SEARCH_ENTRY . " " .
                                     "WHERE " .         CMS_DB_SEARCH_ENTRY_CANONICAL .
                                     "='" .            "1' " .
                                     "AND BIT_COUNT(" . CMS_DB_SEARCH_ENTRY_TEXT_HASH .
                                     "^b'" .            sqlesc($text_hash) . "') " .
                                     "<='" .            sqlesc($this->entry_set_maximum_bit_difference) . "' " .
                                     "LIMIT 1");
            $canonical = (mysql_num_rows($result) > 0) ? "0" : "1";

            //add entry
            $query = "INSERT " .  CMS_DB_SEARCH_ENTRY . " " .
                     "(" .        CMS_DB_SEARCH_ENTRY_ADDRESS . ", " .
                                  CMS_DB_SEARCH_ENTRY_ADDRESS_HASH . ", " .
                                  CMS_DB_SEARCH_ENTRY_TITLE . ", " .
                                  CMS_DB_SEARCH_ENTRY_TEXT . ", " .
                                  CMS_DB_SEARCH_ENTRY_TEXT_HASH . ", " .
                                  CMS_DB_SEARCH_ENTRY_TIME . ", " .
                                  CMS_DB_SEARCH_ENTRY_UPDATE_INTERVAL . ", " .
                                  CMS_DB_SEARCH_ENTRY_UPDATE_TIME . ", " .
                                  CMS_DB_SEARCH_ENTRY_LINK_COUNT . ", " .
                                  CMS_DB_SEARCH_ENTRY_CANONICAL . ") " .
                     "VALUES (" .
                     "_binary'" . sqlesc($address) . "', " .
                     "_binary'" . sqlesc($address_hash) . "', " .
                     "'" .        sqlesc($title) . "', " .
                     "'" .        sqlesc($text) . "', " .
                     "b'" .       sqlesc($text_hash) . "', " .
                     "'" .        sqlesc($time) . "', " .
                     "'" .        sqlesc($this->update_interval) . "', " .
                     "(" .        CMS_DB_SEARCH_ENTRY_TIME .
                     "+" .        CMS_DB_SEARCH_ENTRY_UPDATE_INTERVAL . "), " .
                     "'" .        sqlesc($link_count) . "', " .
                     "'" .        $canonical . "')";

            //execute
            mysql_query($query);
            if (mysql_errno() !== 0) return mysql_error(); //error check

            $entry_index  = mysql_insert_id();
            $_entry_index = sqlesc($entry_index);
        };

        //cut title for indexing
        $data["title"] = first_words($data["title"], 255, "");

        //process address for indexing
        $address = analyze_url($address);
        $host    = preg_replace(["/^www\./i", "/\.[^\.]+$/", "/[^\p{L}\p{M}\p{N}]+/u"],
                                ["",          "",            " "],
                                $address["host"]);
        $path    = preg_replace(["/\.[^\.]+$/", "/[^\p{L}\p{M}\p{N}]+/u"],
                                ["",            " "],
                                $address["path"]);
        $data["address"] = "$host $path";

        //add data to search index
        $array = ["title"   => $this->entry_set_weight_factor_title,
                  "h1"      => $this->entry_set_weight_factor_h1,
                  "h2"      => $this->entry_set_weight_factor_h2,
                  "h3"      => $this->entry_set_weight_factor_h3,
                  "copy"    => $this->entry_set_weight_factor_copy,
                  "side"    => $this->entry_set_weight_factor_side,
                  "address" => $this->entry_set_weight_factor_address];

        foreach ($array AS $key => $value)
        {
            $result = $this->_entry_set($entry_index, //source index
                                        $entry_index, //target index
                                        $data[$key],  //text
                                        $language,    //language
                                        $value);      //maximum weight
            if ($result !== TRUE) return $result;
        };

        //links
        if ($link_count > 0)
        {
            //process outbound links
            $string        = "";
            $weight_factor = $this->entry_set_weight_factor_link / $link_count;

            foreach ($_links AS $value)
            {
                $link_url = sqlesc($value[0]);

                //append link data to sql insert string
                $string .= "('$_entry_index', _binary'$link_url', _binary'" . sqlesc($value[1]) . "'), ";

                //retrieve index of link target entry
                $query = "SELECT " .   CMS_DB_SEARCH_ENTRY_INDEX . " " .
                         "FROM " .     CMS_DB_SEARCH_ENTRY . " " .
                         "WHERE " .    CMS_DB_SEARCH_ENTRY_ADDRESS_HASH .
                         "=_binary'" . $link_url . "' " .
                         "LIMIT 1";

                //execute
                $result = mysql_query($query);
                if (mysql_errno() !== 0) return mysql_error(); //error check

                $target_index = mysql_result($result, 0);

                //add link text to search index
                $result = $this->_entry_set($entry_index,    //source index
                                            $target_index,   //target index
                                            $value[1],       //text
                                            $language,       //language
                                            $weight_factor); //maximum weight
                if ($result !== TRUE) return $result;
            };

            //store link data
            $query = "INSERT IGNORE " . CMS_DB_SEARCH_LINK . " " .
                     "(" .              CMS_DB_SEARCH_LINK_SOURCE . ", " .
                                        CMS_DB_SEARCH_LINK_TARGET_HASH . ", " .
                                        CMS_DB_SEARCH_LINK_TEXT . ") " .
                     "VALUES " .        substr($string, 0, -2);

            //execute
            mysql_query($query);
            if (mysql_errno() !== 0) return mysql_error(); //error check
        };

        //process inbound links
        $query = "SELECT " .   CMS_DB_SEARCH_LINK_SOURCE . ", " .
                               CMS_DB_SEARCH_LINK_TEXT . " " .
                 "FROM " .     CMS_DB_SEARCH_LINK . " " .
                 "WHERE " .    CMS_DB_SEARCH_LINK_TARGET_HASH .
                 "=_binary'" . sqlesc($address_hash) . "'";

        //execute
        $result = mysql_query($query);
        if (mysql_errno() !== 0) return mysql_error(); //error check

        if (($link_count = mysql_num_rows($result)) > 0)
        {
            $weight_factor = $this->entry_set_weight_factor_link / $link_count;

            while ($resultrow = mysql_fetch_row($result))
            {
                //add link text to search index
                $_result = $this->_entry_set($resultrow[0],   //source index
                                             $entry_index,    //target index
                                             $resultrow[1],   //text
                                             $language,       //language
                                             $weight_factor); //maximum weight
                if ($_result !== TRUE) return $_result;
            };
        };

        //find similar entries
        $query = "SELECT " .          CMS_DB_SEARCH_ENTRY_INDEX . " " .
                 "FROM " .            CMS_DB_SEARCH_ENTRY . " " .
                 "WHERE BIT_COUNT(" . CMS_DB_SEARCH_ENTRY_TEXT_HASH .
                 "^b'" .              sqlesc($text_hash) . "')" .
                 "<='" .              sqlesc($this->entry_set_maximum_bit_difference) . "' " .
                 "AND " .             CMS_DB_SEARCH_ENTRY_INDEX .
                 "!='" .              $_entry_index . "'";

        //execute
        $result = mysql_query($query);
        if (mysql_errno() !== 0) return mysql_error(); //error check

        //cluster similar entries
        if (mysql_num_rows($result) > 0)
        {
            $list = "";
            while ($resultrow = mysql_fetch_row($result))
            {
                $value  = sqlesc($resultrow[0]);
                $list  .= "('$entry_index', '$value'), " .
                          "('$value', '$entry_index'), ";
            };

            //connect similar entries
            $query = "INSERT IGNORE " . CMS_DB_SEARCH_CLUSTER . " " .
                     "(" .              CMS_DB_SEARCH_CLUSTER_SOURCE . ", " .
                                        CMS_DB_SEARCH_CLUSTER_TARGET . ") " .
                     "VALUES " .        substr($list, 0, -2);

            //execute
            mysql_query($query);
            if (mysql_errno() !== 0) return mysql_error(); //error check
        };

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    private function _entry_set($source_index,
                                $target_index,
                                $text,
                                $language,
                                $weight_factor = 1.0)
    {
        //strip stop words
        $text = language_strip_stopword($text, $language);

        //generate word list
        $list = tokenize_text($text);
        if (empty($list)) return TRUE;

        //count word occurences
        $_list  = [];
        $__list = [];
        $total  = 0;

        foreach ($list AS $value)
        {
            $value = utf8_strtolower($value);
            $value = utf8_strcut($value, 0, 255);

            //already listed
            if (isset($_list[$value]))
            {
                $_list[$value] += 1;
            }

            //new word
            else
            {
                $_list[$value]  = 1;
                $__list[]       = sqlesc($value);
            };

            $total++;
        };

        //insert words into word table
        $query = "INSERT IGNORE " .   CMS_DB_SEARCH_WORD . " " .
                 "(" .                CMS_DB_SEARCH_WORD_TEXT . ", " .
                                      CMS_DB_SEARCH_WORD_LANGUAGE . ") " .
                 "VALUES (_binary'" . implode("', '" . sqlesc($language) . "'), (_binary'", $__list) .
                 "', '" .             sqlesc($language) . "')";

        //execute
        mysql_query($query);
        if (mysql_errno() !== 0) return mysql_error(); //error check

        //retrieve word indices
        $query = "SELECT " .      CMS_DB_SEARCH_WORD_INDEX . ", " .
                                  CMS_DB_SEARCH_WORD_TEXT . " " .
                 "FROM " .        CMS_DB_SEARCH_WORD . " " .
                 "WHERE " .       CMS_DB_SEARCH_WORD_LANGUAGE .
                 "='" .           sqlesc($language) . "' " .
                 "AND " .         CMS_DB_SEARCH_WORD_TEXT . " " .
                 "IN (_binary'" . implode("', _binary'", $__list) . "')";

        //execute
        $result = mysql_query($query);
        if (mysql_errno() !== 0) return mysql_error(); //error check

        //compute word ratings
        $source_index = sqlesc($source_index);
        $target_index = sqlesc($target_index);
        $factor       = 1.0 / $total * $weight_factor;
        $__list       = [];

        while ($resultrow = mysql_fetch_row($result))
        {
            $word_index = sqlesc($resultrow[0]);
            $text       = $resultrow[1];
            $value      = sqlesc($factor * sqrt($_list[$text]));
            $__list[]   = "'$word_index', '$source_index', '$target_index', '$value'";
        };

        //insert word ratings into weight table
        $query = "INSERT " .                  CMS_DB_SEARCH_WEIGHT . " " .
                 "(" .                        CMS_DB_SEARCH_WEIGHT_WORD . ", " .
                                              CMS_DB_SEARCH_WEIGHT_SOURCE . ", " .
                                              CMS_DB_SEARCH_WEIGHT_TARGET . ", " .
                                              CMS_DB_SEARCH_WEIGHT_VALUE . ") " .
                 "VALUES (" .                 implode("), (", $__list) . ") " .
                 "ON DUPLICATE KEY UPDATE " . CMS_DB_SEARCH_WEIGHT_VALUE .
                 "=" .                        CMS_DB_SEARCH_WEIGHT_VALUE .
                 "+VALUES(" .                 CMS_DB_SEARCH_WEIGHT_VALUE . ")";

        //execute
        mysql_query($query);
        if (mysql_errno() !== 0) return mysql_error(); //error check

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function entry_remove($index,
                          $force = TRUE)
    {
        if (! $this->enabled) return FALSE;

        $index = sqlesc($index);

        if (! $force)
        {
            $result = mysql_query("SELECT " . CMS_DB_SEARCH_ENTRY_ERROR . " " .
                                  "FROM " .   CMS_DB_SEARCH_ENTRY . " " .
                                  "WHERE " .  CMS_DB_SEARCH_ENTRY_INDEX . "='$index' " .
                                  "LIMIT 1");
            if ($result === FALSE) return FALSE;
            $force = mysql_result($result, 0) >= $this->entry_remove_error_limit;
        };

        //increase error count -> loses score and canonical status
        if (! $force)
            return (bool)mysql_query("UPDATE " . CMS_DB_SEARCH_ENTRY . " " .
                                     "SET " .    CMS_DB_SEARCH_ENTRY_ERROR .
                                     "=" .       CMS_DB_SEARCH_ENTRY_ERROR . "+1, " .
                                                 CMS_DB_SEARCH_ENTRY_CANONICAL .
                                     "='" .     "0' " .
                                     "WHERE " .  CMS_DB_SEARCH_ENTRY_INDEX . "='$index' " .
                                     "LIMIT 1");

        //remove permanently
        if (mysql_query("DELETE FROM " . CMS_DB_SEARCH_ENTRY . " " .
                        "WHERE " .       CMS_DB_SEARCH_ENTRY_INDEX . "='$index' " .
                        "LIMIT 1"))
        {
            //remove word weights, inbound and outbound
            mysql_query("DELETE FROM " . CMS_DB_SEARCH_WEIGHT . " " .
                        "WHERE " .       CMS_DB_SEARCH_WEIGHT_SOURCE .  "='$index' " .
                        "OR " .          CMS_DB_SEARCH_WEIGHT_TARGET .  "='$index'");

            //remove outbound links
            mysql_query("DELETE FROM " . CMS_DB_SEARCH_LINK . " " .
                        "WHERE " .       CMS_DB_SEARCH_LINK_SOURCE .    "='$index'");

            //remove cluster associations
            mysql_query("DELETE FROM " . CMS_DB_SEARCH_CLUSTER . " " .
                        "WHERE " .       CMS_DB_SEARCH_CLUSTER_SOURCE . "='$index' " .
                        "OR " .          CMS_DB_SEARCH_CLUSTER_TARGET . "='$index'");

            return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function queue_add($address,
                       $type = CMS_SEARCH_QUEUE_TYPE_NONE)
    {
        if (! $this->enabled) return FALSE;

        //standardize and check address
        if (($address = $this->address_standardize($address)) === FALSE) return FALSE;
        if ($this->address_accepted($address) === FALSE)                 return FALSE;

        //add discovered link to queue
        if (CMS_SEARCH_QUEUE_TYPE_REFERENCE & $type)
            mysql_query("INSERT IGNORE " .           CMS_DB_SEARCH_QUEUE . " " .
                        "(" .                        CMS_DB_SEARCH_QUEUE_ADDRESS . ", " .
                                                     CMS_DB_SEARCH_QUEUE_TYPE . ", " .
                                                     CMS_DB_SEARCH_QUEUE_TIME . ") " .
                        "VALUES (" .
                        "_binary'" .                 sqlesc($address) . "', " .
                        "'" .                        sqlesc($type) . "', " .
                        "'" .                        sqlesc(time()) . "')");

        //add task to queue
        else
            mysql_query("INSERT " .                  CMS_DB_SEARCH_QUEUE . " " .
                        "(" .                        CMS_DB_SEARCH_QUEUE_ADDRESS . ", " .
                                                     CMS_DB_SEARCH_QUEUE_TYPE . ", " .
                                                     CMS_DB_SEARCH_QUEUE_TIME . ") " .
                        "VALUES (" .
                        "_binary'" .                 sqlesc($address) . "', " .
                        "'" .                        sqlesc($type) . "', " .
                        "'" .                        sqlesc(time()) . "' ) " .
                        "ON DUPLICATE KEY UPDATE " . CMS_DB_SEARCH_QUEUE_TYPE .
                        "=VALUES(" .                 CMS_DB_SEARCH_QUEUE_TYPE . "), " .
                                                     CMS_DB_SEARCH_QUEUE_TIME .
                        "=VALUES(" .                 CMS_DB_SEARCH_QUEUE_TIME . "), " .
                                                     CMS_DB_SEARCH_QUEUE_ERROR .
                        "='" .                      "0', " .
                                                     CMS_DB_SEARCH_QUEUE_DONE .
                        "='" .                      "0'");

        return mysql_affected_rows() > 0;
    }

//--------------------------------------------------------------------------------------------------

    function queue_add_update()
    {
        return $this->queue_add_all(TRUE);
    }

//--------------------------------------------------------------------------------------------------

    function queue_add_all($update = FALSE)
    {
        if (! $this->enabled) return FALSE;

        $time = sqlesc(time());

        mysql_query("INSERT  " .                 CMS_DB_SEARCH_QUEUE . " " .
                    "(" .                        CMS_DB_SEARCH_QUEUE_ADDRESS . ", " .
                                                 CMS_DB_SEARCH_QUEUE_TYPE . ", " .
                                                 CMS_DB_SEARCH_QUEUE_TIME . ") " .
                    "SELECT " .                  CMS_DB_SEARCH_ENTRY_ADDRESS . ", " .
                    "'" .                        sqlesc(CMS_SEARCH_QUEUE_TYPE_UPDATE) . "', " .
                    "'" .                        $time . "' " .
                    "FROM " .                    CMS_DB_SEARCH_ENTRY . " " .
                    ($update ?
                    "WHERE " .                   CMS_DB_SEARCH_ENTRY_UPDATE_TIME .
                    "<'" .                       $time . "' " : "") .
                    "ON DUPLICATE KEY UPDATE " . CMS_DB_SEARCH_QUEUE_TYPE .
                    "=VALUES(" .                 CMS_DB_SEARCH_QUEUE_TYPE . "), " .
                                                 CMS_DB_SEARCH_QUEUE_TIME .
                    "=VALUES(" .                 CMS_DB_SEARCH_QUEUE_TIME . "), " .
                                                 CMS_DB_SEARCH_QUEUE_ERROR .
                    "='" .                      "0', " .
                                                 CMS_DB_SEARCH_QUEUE_DONE .
                    "='" .                      "0'");

        return mysql_affected_rows() > 0;
    }

//--------------------------------------------------------------------------------------------------

    function queue_remove($type = CMS_SEARCH_QUEUE_TYPE_ALL)
    {
        if (! $this->enabled) return FALSE;

        mysql_query("UPDATE " . CMS_DB_SEARCH_QUEUE . " " .
                    "SET " .    CMS_DB_SEARCH_QUEUE_DONE .
                    "='" .     "1'" .
                    (($type == CMS_SEARCH_QUEUE_TYPE_ALL) ? "" :
                   " WHERE " .  CMS_DB_SEARCH_QUEUE_TYPE .
                    "&" .       (int)$type));

        return mysql_affected_rows() > 0;
    }

//--------------------------------------------------------------------------------------------------

    function queue_length($type = CMS_SEARCH_QUEUE_TYPE_ALL)
    {
        if (! $this->enabled) return FALSE;

        $result = mysql_query("SELECT COUNT(*) " .
                              "FROM " .  CMS_DB_SEARCH_QUEUE . " " .
                              "WHERE " . CMS_DB_SEARCH_QUEUE_DONE .
                              "='" .    "0'" .
                              (($type == CMS_SEARCH_QUEUE_TYPE_ALL) ? "" :
                             " AND " .   CMS_DB_SEARCH_QUEUE_TYPE .
                              "&" .      (int)$type));

        if ($resultrow = mysql_fetch_row($result)) return $resultrow[0];
        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function queue_process($type         = CMS_SEARCH_QUEUE_TYPE_ALL,
                           $follow_links = FALSE)
    {
        if (! $this->enabled) return FALSE;

        set_time_limit(600);

        $code = sqlesc(unique_id());
        $time = time();

        if (

        //mark entry to process
             mysql_query("UPDATE " .   CMS_DB_SEARCH_QUEUE . " " .
                         "SET " .      CMS_DB_SEARCH_QUEUE_TIME .
                         "='" .        sqlesc($time + $this->queue_process_retry_time) . "', " .
                                       CMS_DB_SEARCH_QUEUE_CODE .
                         "=_binary'" . $code . "' " .
                         "WHERE " .    CMS_DB_SEARCH_QUEUE_DONE .
                         "='" .       "0' " .

                         (($type == CMS_SEARCH_QUEUE_TYPE_ALL) ? "" :
                         "AND " .      CMS_DB_SEARCH_QUEUE_TYPE .
                         "&" .         (int)$type . " ") .

                         "AND (" .     CMS_DB_SEARCH_QUEUE_CODE .
                         "=_binary'' " .
                         "OR " .       CMS_DB_SEARCH_QUEUE_TIME .
                         "<'" .        sqlesc($time) . "') " .

                         "ORDER BY " . CMS_DB_SEARCH_QUEUE_TIME . " " .
                         "LIMIT 1")
             &&

        //fetch data
             ($result = mysql_query("SELECT " .   CMS_DB_SEARCH_QUEUE_ADDRESS . ", " .
                                                  CMS_DB_SEARCH_QUEUE_ERROR . " " .
                                    "FROM " .     CMS_DB_SEARCH_QUEUE . " " .
                                    "WHERE " .    CMS_DB_SEARCH_QUEUE_CODE .
                                    "=_binary'" . $code . "' " .
                                    "LIMIT 1"))
             &&
             ($resultrow = mysql_fetch_row($result))
           )
        {
            //scan address
            $value = $this->scan($resultrow[0], $follow_links);

            if (

            //non fatal error
                 (CMS_SEARCH_SCAN_ERROR & ~CMS_SEARCH_SCAN_FATAL_ERROR & $value)
                 &&

            //error count below limit
                 (++$resultrow[1] < $this->queue_process_error_limit)
                 &&

            //increase error count
                 mysql_query("UPDATE " .   CMS_DB_SEARCH_QUEUE . " " .
                             "SET " .      CMS_DB_SEARCH_QUEUE_ERROR .
                             "='" .        sqlesc($resultrow[1]) . "' " .
                             "WHERE " .    CMS_DB_SEARCH_QUEUE_CODE .
                             "=_binary'" . $code . "' " .
                             "LIMIT 1")
               )
                return $value;

            //mark queue entry as done
            mysql_query("UPDATE " .   CMS_DB_SEARCH_QUEUE . " " .
                        "SET " .      CMS_DB_SEARCH_QUEUE_DONE .
                        "='" .       "1' " .
                        "WHERE " .    CMS_DB_SEARCH_QUEUE_CODE .
                        "=_binary'" . $code . "' " .
                        "LIMIT 1");
            return $value;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function daemon_status($enabled      = TRUE,
                           $type         = CMS_SEARCH_QUEUE_TYPE_ALL,
                           $follow_links = FALSE)
    {
        if (! $this->enabled) return FALSE;

        $path  = CMS_DATA_PATH . "#system/search.daemon.status";
        $array = [(int)$enabled,
                  (int)$type,
                  (int)$follow_links];

        return file_put_contents($path, implode(";", $array), LOCK_EX) !== FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function daemon_get_status()
    {
        if (! $this->enabled) return FALSE;

        $path = CMS_DATA_PATH . "#system/search.daemon.status";
        if (! is_file($path)) return FALSE;

        $hfile = fopen($path, "rb");
        if ($hfile === FALSE) return FALSE;

        if (! flock($hfile, LOCK_SH))
        {
            fclose($hfile);
            return FALSE;
        };

        $array = explode(";", stream_get_contents($hfile));
        flock($hfile, LOCK_UN);
        fclose($hfile);

        return ["enabled"      => (bool)($array[0] ?? FALSE),
                "type"         => (int) ($array[1] ?? CMS_SEARCH_QUEUE_TYPE_NONE),
                "follow_links" => (bool)($array[2] ?? FALSE)];
    }

//--------------------------------------------------------------------------------------------------

    function daemon($time_limit = 60)
    {
        if (! $this->enabled) return FALSE;

        //process queue
        $time_limit += time();

        while (TRUE)
        {
            //get status
            $status = $this->daemon_get_status();
            if ($status === FALSE)            return FALSE;
            if ($status["enabled"] === FALSE) return FALSE;

            //process next queue entry
            $value = $this->queue_process($status["type"],
                                          $status["follow_links"]);

            if (time() > $time_limit) return FALSE;
            if ($value === FALSE)     return TRUE;

            sleep(1);
        };
    }

//--------------------------------------------------------------------------------------------------

    function find($term,
                  $page     = 0,
                  $language = CMS_LANGUAGE)
    {
        if (! $this->enabled) return FALSE;

        //start time
        $time = microtime(TRUE);

        //tokenize search term
        $list = tokenize_text($term);

        //preprocess search tokens
        $list  = array_unique($list);
        $list  = array_map(__NAMESPACE__ . "\\utf8_strtolower", $list);
        $_list = array_filter($list, function($value) { return utf8_strlen($value) > 2; });
        $_list = array_map(function($value) { return addcslashes($value, "%_"); }, $_list);
        $list  = array_map(__NAMESPACE__ . "\\sqlesc", $list);
        $_list = array_map(__NAMESPACE__ . "\\sqlesc", $_list);

        //retrieve word indices matching search tokens
        $subquery = "SELECT " .            CMS_DB_SEARCH_WORD_INDEX . " " .
                    "FROM " .              CMS_DB_SEARCH_WORD . " " .
                    (nstre($language) ?
                    "WHERE " .             CMS_DB_SEARCH_WORD_LANGUAGE .
                    "='" .                 sqlesc($language) . "' " .
                    "AND (" : "WHERE (") . CMS_DB_SEARCH_WORD_TEXT . " " .
                    "IN (_binary'" .       implode("', _binary'", $list) . "')" .
                    (empty($_list) ? "" :
                    "OR " .                CMS_DB_SEARCH_WORD_TEXT . " " .
                    "LIKE _binary'" .      implode("%' " .
                    "OR " .                CMS_DB_SEARCH_WORD_TEXT . " " .
                    "LIKE _binary'",       $_list) . "%'") . ")";

        //retrieve result entries
        $query    = "SELECT " .   "`#a`." . CMS_DB_SEARCH_WEIGHT_TARGET . " " .
                    "FROM " .               CMS_DB_SEARCH_WEIGHT . " AS `#a`, " .
                                            CMS_DB_SEARCH_ENTRY .  " AS `#b` " .
                    "WHERE " .    "`#b`." . CMS_DB_SEARCH_ENTRY_CANONICAL . " " .
                    "='" .         "1' " .
                    "AND " .      "`#a`." . CMS_DB_SEARCH_WEIGHT_WORD . " " .
                    "IN (" .                $subquery . ") " .
                    "AND " .      "`#a`." . CMS_DB_SEARCH_WEIGHT_TARGET .
                    "=" .         "`#b`." . CMS_DB_SEARCH_ENTRY_INDEX . " " .
                    "GROUP BY " . "`#a`." . CMS_DB_SEARCH_WEIGHT_TARGET . " " .

        //order by number of search tokens matched
                    "ORDER BY " .          "COUNT(*) DESC, " .

        //order by total weight of matched search tokens
                    "SUM(" .      "`#a`." . CMS_DB_SEARCH_WEIGHT_VALUE . ")" .

        //multiply by page score
                    "*" .         "`#b`." . CMS_DB_SEARCH_ENTRY_SCORE . " DESC " .

        //limit to result page
                    "LIMIT " .    ($page * $this->find_results_per_page) . ", " .
                                  (int)$this->find_results_per_page;

        //execute
        if (
             ($result = mysql_query($query))
             &&
             (mysql_num_rows($result) > 0)
           )
        {
            //retrieve result entries
            $list = [];
            while ($resultrow = mysql_fetch_row($result))
                $list[] = sqlesc($resultrow[0]);

            //extract requested result page
            $string = "'" . implode("', '", $list) . "'";

            //retrieve result entries and number of similar entries
            $query = "SELECT " .         "`#a`." . CMS_DB_SEARCH_ENTRY_INDEX . ", " .
                                         "`#a`." . CMS_DB_SEARCH_ENTRY_ADDRESS . ", " .
                                         "`#a`." . CMS_DB_SEARCH_ENTRY_TITLE . ", " .
                                         "`#a`." . CMS_DB_SEARCH_ENTRY_TEXT . ", " .
                                         "`#a`." . CMS_DB_SEARCH_ENTRY_TIME . ", " .
                                         "`#a`." . CMS_DB_SEARCH_ENTRY_SCORE . ", " .
                     "COUNT(" .          "`#b`." . CMS_DB_SEARCH_CLUSTER_TARGET . ") " .
                     "FROM " .                     CMS_DB_SEARCH_ENTRY .   " AS `#a` " .
                     "LEFT JOIN " .                CMS_DB_SEARCH_CLUSTER . " AS `#b` " .
                     "ON " .             "`#a`." . CMS_DB_SEARCH_ENTRY_INDEX .
                     "=" .               "`#b`." . CMS_DB_SEARCH_CLUSTER_SOURCE . " " .
                     "WHERE " .          "`#a`." . CMS_DB_SEARCH_ENTRY_INDEX . " " .
                     "IN (" .                      $string . ") " .
                     "GROUP BY " .       "`#a`." . CMS_DB_SEARCH_ENTRY_INDEX . " " .
                     "ORDER BY FIELD(" . "`#a`." . CMS_DB_SEARCH_ENTRY_INDEX . ", " .
                                                   $string . ")";

            //retrieve entry data
            if (
                 ($result = mysql_query($query))
                 &&
                 (mysql_num_rows($result) > 0)
               )
            {
                //collect entry data
                $list = [];

                while ($resultrow = mysql_fetch_row($result))
                {
                    //extract quotation
                    $buffer = quote_text($resultrow[3], $term, 1);

                    //alternatively display beginning of text
                    if (stre($buffer)) $buffer = first_words($resultrow[3]);

                    //add data to list
                    $list[$resultrow[0]] =
                        ["address"      => $resultrow[1],
                         "title"        => $resultrow[2],
                         "text"         => $buffer,
                         "time"         => $resultrow[4],
                         "score"        => $resultrow[5],
                         "supplemental" => $resultrow[6]];
                };

                return $list;
            };
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function tag($entry_index = NULL,
                 $limit       = 10,
                 $language    = CMS_LANGUAGE)
    {
        if (! $this->enabled) return FALSE;

        //retrieve most frequent words of all entries
        if ($entry_index === NULL)
        {
            $subquery_total_weight =
                "SELECT SUM(" .           CMS_DB_SEARCH_WEIGHT_VALUE . ") " .
                "FROM " .                 CMS_DB_SEARCH_WEIGHT;

            $subquery_tag_list =
                "SELECT " .               CMS_DB_SEARCH_WEIGHT_WORD . ", " .
                "(100/(" .                $subquery_total_weight . ")*" .
                "SUM(" .                  CMS_DB_SEARCH_WEIGHT_VALUE . ")) " .
                "AS `" .                 "#score` " .
                "FROM " .                 CMS_DB_SEARCH_WEIGHT . " " .
                "GROUP BY " .             CMS_DB_SEARCH_WEIGHT_WORD;

            $query =
                "SELECT " .               CMS_DB_SEARCH_WORD_TEXT . ", " .
                                        "`#score` " .
                "FROM (" .                $subquery_tag_list . ") AS `#tag_list`, " .
                                          CMS_DB_SEARCH_WORD . " " .
                "WHERE " .                CMS_DB_SEARCH_WORD . "." . CMS_DB_SEARCH_WORD_INDEX .
                "=`" .                   "#tag_list`" .        "." . CMS_DB_SEARCH_WEIGHT_WORD . " " .
                (stre($language) ? "" :
                "AND " .                  CMS_DB_SEARCH_WORD . "." . CMS_DB_SEARCH_WORD_LANGUAGE .
                "='" .                    sqlesc($language) . "' ") .
                "AND CHAR_LENGTH(CAST(" . CMS_DB_SEARCH_WORD . "." . CMS_DB_SEARCH_WORD_TEXT .
                                        " AS CHAR CHARACTER SET utf8))" .
                ">'" .                   "2' " .
                "ORDER BY `" .           "#score` DESC " .
                "LIMIT " .                sqlesc($limit);
        }

        //retrieve most frequent words of selected entry
        else
        {
            $subquery_tag_list =
                "SELECT " .               CMS_DB_SEARCH_WEIGHT_WORD . ", " .
                                          CMS_DB_SEARCH_WEIGHT_VALUE . " " .
                "FROM " .                 CMS_DB_SEARCH_WEIGHT . " " .
                "WHERE " .                CMS_DB_SEARCH_WEIGHT_TARGET . " " .
                "='" .                    sqlesc($entry_index) . "' " .
                "GROUP BY " .             CMS_DB_SEARCH_WEIGHT_WORD;

            $query =
                "SELECT " .               CMS_DB_SEARCH_WORD_TEXT . ", " .
                                          CMS_DB_SEARCH_WEIGHT_VALUE . " " .
                "FROM (" .                $subquery_tag_list . ") AS `#tag_list`, " .
                                          CMS_DB_SEARCH_WORD . " " .
                "WHERE " .                CMS_DB_SEARCH_WORD . "." . CMS_DB_SEARCH_WORD_INDEX .
                "=" .                    "`#tag_list`" .       "." . CMS_DB_SEARCH_WEIGHT_WORD . " " .
                (stre($language) ? "" :
                "AND " .                  CMS_DB_SEARCH_WORD . "." . CMS_DB_SEARCH_WORD_LANGUAGE .
                "='" .                    sqlesc($language) . "' ") .
                "AND CHAR_LENGTH(CAST(" . CMS_DB_SEARCH_WORD . "." . CMS_DB_SEARCH_WORD_TEXT .
                                        " AS CHAR CHARACTER SET utf8))" .
                ">'" .                   "2' " .
                "ORDER BY " .             CMS_DB_SEARCH_WEIGHT_VALUE . " DESC " .
                "LIMIT " .                sqlesc($limit);
        };

        //execute and return
        if ($result = mysql_query($query))
        {
            $list = [];

            while ($resultrow = mysql_fetch_row($result))
                $list[$resultrow[0]] = $resultrow[1];

            return $list;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function score_compute()
    {
        if (! $this->enabled) return FALSE;

        while (TRUE)
        {
            //check for new entry
            $query = "SHOW TABLE STATUS LIKE '" . CMS_DB_SEARCH_ENTRY . "'";

            if (
                 (! $result = mysql_query($query))
                 ||
                 (! $resultrow = mysql_fetch_assoc($result))
               )
                return FALSE;

            $hash  = cms_cache("search.score_compute.hash");
            $_hash = hash32($resultrow["Auto_increment"]);
            if (nstreq($hash, $_hash)) break;

            //check for updated entry
            $time  = cms_cache("search.score_compute.time");
            $query = "SELECT 1 " .
                     "FROM " .  CMS_DB_SEARCH_ENTRY . " " .
                     "WHERE " . CMS_DB_SEARCH_ENTRY_TIME .
                     ">'" .     (int)$time . "' " .
                     "LIMIT 1";

            if (! $result = mysql_query($query)) return FALSE;
            if (mysql_num_rows($result))         break;

            return FALSE;
        };

        //initialize
        $result = $this->score_compute_initialize();
        if ($result !== TRUE) return $result;
        sleep(1);

        //iterate
        for ($i = 0; $i < $this->score_compute_iteration_number; $i++)
        {
            $result = $this->score_compute_iterate();
            if ($result !== TRUE) return $result;
            sleep(1);
        };

        //finalize
        $result = $this->score_compute_finalize();
        if ($result !== TRUE) return $result;
        sleep(1);

        //retrieve dangling link level
        $link_level = $this->score_compute_link_level();
        if ($link_level === FALSE) return FALSE;
        sleep(1);

        //iterate dangling link scores
        for ($i = $link_level; $i > 0; $i--)
        {
            $result = $this->score_compute_iterate($i);
            if ($result !== TRUE) return $result;
            sleep(1);
        };

        //set modification markers
        cms_cache("search.score_compute.hash", $_hash, TRUE);
        cms_cache("search.score_compute.time", time(), TRUE);

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function score_compute_initialize()
    {
        if (! $this->enabled) return FALSE;

        set_time_limit(600);

        //preprocess links

        //reset link targets
        mysql_query("UPDATE " . CMS_DB_SEARCH_LINK . " " .
                    "SET " .    CMS_DB_SEARCH_LINK_TARGET .
                    "='" .     "0', " .
                                CMS_DB_SEARCH_LINK_LEVEL .
                    "='" .     "1'");
        if (mysql_errno() !== 0) return mysql_error(); //error check

        //seek and store link targets
        mysql_query("UPDATE " . CMS_DB_SEARCH_LINK . ", " .
                                CMS_DB_SEARCH_ENTRY . " " .
                    "SET " .    CMS_DB_SEARCH_LINK .  "." . CMS_DB_SEARCH_LINK_TARGET .
                    "=" .       CMS_DB_SEARCH_ENTRY . "." . CMS_DB_SEARCH_ENTRY_INDEX . ", " .
                                CMS_DB_SEARCH_LINK .  "." . CMS_DB_SEARCH_LINK_LEVEL .
                    "='" .     "0' " .
                    "WHERE " .  CMS_DB_SEARCH_LINK .  "." . CMS_DB_SEARCH_LINK_TARGET_HASH .
                    "=" .       CMS_DB_SEARCH_ENTRY . "." . CMS_DB_SEARCH_ENTRY_ADDRESS_HASH);
        if (mysql_errno() !== 0) return mysql_error(); //error check

        //remove self links
        mysql_query("DELETE FROM " . CMS_DB_SEARCH_LINK . " " .
                    "WHERE " .       CMS_DB_SEARCH_LINK_TARGET .
                    "=" .            CMS_DB_SEARCH_LINK_SOURCE);
        if (mysql_errno() !== 0) return mysql_error(); //error check

        //recursively deactivate dangling links
        $link_level = 1;

        do
        {
            //retrieve dangling links
            $result = mysql_query("SELECT " .     CMS_DB_SEARCH_LINK_SOURCE . " " .
                                  "FROM " .       CMS_DB_SEARCH_LINK . " " .
                                  "GROUP BY " .   CMS_DB_SEARCH_LINK_SOURCE . " " .
                                  "HAVING MIN(" . CMS_DB_SEARCH_LINK_LEVEL . ")" .
                                  "!='" .        "0'");
            if (mysql_errno() !== 0) return mysql_error(); //error check

            $num_rows      = mysql_num_rows($result);
            $current_row   = 0;
            $affected_rows = 0;

            while ($current_row < $num_rows)
            {
                //collect link block
                $string = "";
                for ($i = 0; $i < 1000; $i++)
                {
                    if (! $resultrow = mysql_fetch_row($result)) break;
                    $string .= "'" . sqlesc($resultrow[0]) . "', ";
                };

                //deactivate dangling links
                mysql_query("UPDATE " . CMS_DB_SEARCH_LINK . " " .
                            "SET " .    CMS_DB_SEARCH_LINK . "." . CMS_DB_SEARCH_LINK_LEVEL .
                            "='" .      sqlesc(++$link_level) . "' " .
                            "WHERE " .  CMS_DB_SEARCH_LINK . "." . CMS_DB_SEARCH_LINK_LEVEL .
                            "='" .     "0' " .
                            "AND " .    CMS_DB_SEARCH_LINK . "." . CMS_DB_SEARCH_LINK_TARGET . " " .
                            "IN (" .    substr($string, 0, -2) . ")");
                if (mysql_errno() !== 0) return mysql_error(); //error check

                $current_row   += $i;
                $affected_rows += mysql_affected_rows();
            };
        }
        while ($affected_rows > 0);

        //preprocess entries

        //reset scores and outbound link counts
        mysql_query("UPDATE " . CMS_DB_SEARCH_ENTRY . " " .
                    "SET " .    CMS_DB_SEARCH_ENTRY_SCORE .
                    "=IF(" .    CMS_DB_SEARCH_ENTRY_ERROR .
                    "='" .     "0', '1.0', '0.0'), " .
                                CMS_DB_SEARCH_ENTRY_LINK_COUNT .
                    "='" .     "0'");
        if (mysql_errno() !== 0) return mysql_error(); //error check

        //count links per source omitting dangling links (source => count)
        $subquery = "SELECT " .   CMS_DB_SEARCH_LINK_SOURCE . ", " .
                                 "COUNT(*) AS `#count` " .
                    "FROM " .     CMS_DB_SEARCH_LINK . " " .
                    "WHERE " .    CMS_DB_SEARCH_LINK_LEVEL .
                    "='" .       "0' " .
                    "GROUP BY " . CMS_DB_SEARCH_LINK_SOURCE;

        //store links per source
        mysql_query("UPDATE " . CMS_DB_SEARCH_ENTRY . ", " .
                    "(" .       $subquery . ") AS `#link` " .
                    "SET " .    CMS_DB_SEARCH_ENTRY . "." .   CMS_DB_SEARCH_ENTRY_LINK_COUNT .
                    "=`" .     "#link`" .             "." . "`#count` " .
                    "WHERE " .  CMS_DB_SEARCH_ENTRY . "." .   CMS_DB_SEARCH_ENTRY_INDEX .
                    "=`" .     "#link`" .             "." .   CMS_DB_SEARCH_LINK_SOURCE);
        if (mysql_errno() !== 0) return mysql_error(); //error check

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function score_compute_iterate($link_level = 0)
    {
        if (! $this->enabled) return FALSE;

        set_time_limit(600);

        $dampening_factor_inverted = 1.0 - $this->score_compute_dampening_factor;

        //retrieve inherited score portion per source (target => score portion)
        $subquery_inherited_score_portion =
                     "SELECT " . CMS_DB_SEARCH_LINK .  "." . CMS_DB_SEARCH_LINK_TARGET . ", " .
                                 CMS_DB_SEARCH_ENTRY . "." . CMS_DB_SEARCH_ENTRY_SCORE .
                     "/" .       CMS_DB_SEARCH_ENTRY . "." . CMS_DB_SEARCH_ENTRY_LINK_COUNT . " " .
                     "AS `" .   "#score_portion` " .
                     "FROM " .   CMS_DB_SEARCH_LINK . ", " .
                                 CMS_DB_SEARCH_ENTRY . " " .
                     "WHERE " .  CMS_DB_SEARCH_LINK .  "." . CMS_DB_SEARCH_LINK_LEVEL .
                     "='" .      sqlesc($link_level) . "' " .
                     "AND " .    CMS_DB_SEARCH_LINK .  "." . CMS_DB_SEARCH_LINK_SOURCE .
                     "=" .       CMS_DB_SEARCH_ENTRY . "." . CMS_DB_SEARCH_ENTRY_INDEX;

        //compute total score per target (entry => score)
        $subquery_entry_score =
                     "SELECT " .   CMS_DB_SEARCH_ENTRY . "." .   CMS_DB_SEARCH_ENTRY_INDEX . ", " .
                     "'" .         sqlesc($dampening_factor_inverted) . "'" .
                     "+'" .        sqlesc($this->score_compute_dampening_factor) . "'" .
                     "*SUM(`" .   "#link`" .             "." . "`#score_portion`) AS `#score` " .
                     "FROM " .     CMS_DB_SEARCH_ENTRY . ", " .
                     "(" .         $subquery_inherited_score_portion . ") AS `#link` " .
                     "WHERE " .    CMS_DB_SEARCH_ENTRY . "." .   CMS_DB_SEARCH_ENTRY_INDEX .
                     "=`" .       "#link`" .             "." .   CMS_DB_SEARCH_LINK_TARGET . " " .
                     "GROUP BY " . CMS_DB_SEARCH_ENTRY . "." .   CMS_DB_SEARCH_ENTRY_INDEX;

        //store score
        $query_score =
                     "UPDATE " . CMS_DB_SEARCH_ENTRY . ", " .
                     "(" .       $subquery_entry_score . ") AS `#entry` " .
                     "SET " .    CMS_DB_SEARCH_ENTRY . "." .   CMS_DB_SEARCH_ENTRY_SCORE .
                     "=`" .     "#entry`" .            "." . "`#score` " .
                     "WHERE " .  CMS_DB_SEARCH_ENTRY . "." .   CMS_DB_SEARCH_ENTRY_INDEX .
                     "=`" .     "#entry`" .            "." .   CMS_DB_SEARCH_ENTRY_INDEX;

        //execute
        mysql_query($query_score);
        if (mysql_errno() !== 0) return mysql_error(); //error check

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function score_compute_finalize()
    {
        if (! $this->enabled) return FALSE;

        set_time_limit(600);

        //count links per source (source => count)
        $subquery = "SELECT " .   CMS_DB_SEARCH_LINK_SOURCE . ", " .
                                 "COUNT(*) AS `#count` " .
                    "FROM " .     CMS_DB_SEARCH_LINK . " " .
                    "GROUP BY " . CMS_DB_SEARCH_LINK_SOURCE;

        //store links per source
        mysql_query("UPDATE " . CMS_DB_SEARCH_ENTRY . ", " .
                    "(" .       $subquery . ") AS `#link` " .
                    "SET " .    CMS_DB_SEARCH_ENTRY . "." .   CMS_DB_SEARCH_ENTRY_LINK_COUNT .
                    "=`" .     "#link`" .             "." . "`#count` " .
                    "WHERE " .  CMS_DB_SEARCH_ENTRY . "." .   CMS_DB_SEARCH_ENTRY_INDEX .
                    "=`" .     "#link`" .             "." .   CMS_DB_SEARCH_LINK_SOURCE);

        return (mysql_errno() !== 0) ? mysql_error() : TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function score_compute_link_level()
    {
        if (! $this->enabled) return FALSE;

        if (
             ($result = mysql_query("SELECT MAX(" . CMS_DB_SEARCH_LINK_LEVEL . ") " .
                                    "FROM " .       CMS_DB_SEARCH_LINK))
             &&
             ($resultrow = mysql_fetch_row($result))
           )
            return $resultrow[0];

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    function score_compute_canonical()
    {
        if (! $this->enabled) return FALSE;

        //reset
        mysql_query("UPDATE " . CMS_DB_SEARCH_ENTRY . " " .
                    "SET " .    CMS_DB_SEARCH_ENTRY_CANONICAL .
                    "='" .     "1'");

        //set all entries with a higher score alternative to 0
        mysql_query("UPDATE " .       CMS_DB_SEARCH_ENTRY .   " a " .
                    "JOIN " .         CMS_DB_SEARCH_CLUSTER . " b " .
                    "ON " .    "a." . CMS_DB_SEARCH_ENTRY_INDEX .
                    "=" .      "b." . CMS_DB_SEARCH_CLUSTER_SOURCE . " " .
                    "JOIN " .         CMS_DB_SEARCH_ENTRY .   " c " .
                    "ON ".     "b." . CMS_DB_SEARCH_CLUSTER_TARGET .
                    "=" .      "c." . CMS_DB_SEARCH_ENTRY_INDEX . " " .
                    "SET " .   "a." . CMS_DB_SEARCH_ENTRY_CANONICAL .
                    "='" .     "0' " .
                    "WHERE " . "c." . CMS_DB_SEARCH_ENTRY_SCORE .
                    ">" .      "a." . CMS_DB_SEARCH_ENTRY_SCORE . " " .
                    "OR (" .   "c." . CMS_DB_SEARCH_ENTRY_SCORE .
                    "=" .      "a." . CMS_DB_SEARCH_ENTRY_SCORE . " " .
                    "AND " .   "c." . CMS_DB_SEARCH_ENTRY_INDEX .
                    "<" .      "a." . CMS_DB_SEARCH_ENTRY_INDEX . ")");

        return (mysql_errno() !== 0) ? mysql_error() : TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function clean()
    {
        if (! $this->enabled) return FALSE;

        //clean up weight table
        mysql_query("DELETE " .    "a " .
                    "FROM " .             CMS_DB_SEARCH_WEIGHT . " a " .
                    "LEFT JOIN " .        CMS_DB_SEARCH_ENTRY .  " b " .
                    "ON " .        "a." . CMS_DB_SEARCH_WEIGHT_TARGET .
                    "=" .          "b." . CMS_DB_SEARCH_ENTRY_INDEX . " " .
                    "OR " .        "a." . CMS_DB_SEARCH_WEIGHT_SOURCE .
                    "=" .          "b." . CMS_DB_SEARCH_ENTRY_INDEX . " " .
                    "WHERE " .     "b." . CMS_DB_SEARCH_ENTRY_INDEX . " IS NULL");

        //clean up word table, not really necessary
        mysql_query("DELETE " .    "a " .
                    "FROM " .             CMS_DB_SEARCH_WORD .   " a " .
                    "LEFT JOIN " .        CMS_DB_SEARCH_WEIGHT . " b " .
                    "ON " .        "a." . CMS_DB_SEARCH_WORD_INDEX .
                    "=" .          "b." . CMS_DB_SEARCH_WEIGHT_WORD . " " .
                    "WHERE " .     "b." . CMS_DB_SEARCH_WEIGHT_WORD . " IS NULL");

        //clean up link table
        mysql_query("DELETE " .    "a " .
                    "FROM " .             CMS_DB_SEARCH_LINK .  " a " .
                    "LEFT JOIN " .        CMS_DB_SEARCH_ENTRY . " b " .
                    "ON " .        "a." . CMS_DB_SEARCH_LINK_SOURCE .
                    "=" .          "b." . CMS_DB_SEARCH_ENTRY_INDEX . " " .
                    "WHERE " .     "b." . CMS_DB_SEARCH_ENTRY_INDEX . " IS NULL");

        //clean up cluster table
        mysql_query("DELETE " .    "a " .
                    "FROM " .             CMS_DB_SEARCH_CLUSTER . " a " .
                    "LEFT JOIN " .        CMS_DB_SEARCH_ENTRY .   " b " .
                    "ON " .        "a." . CMS_DB_SEARCH_CLUSTER_SOURCE .
                    "=" .          "b." . CMS_DB_SEARCH_ENTRY_INDEX . " " .
                    "OR " .        "a." . CMS_DB_SEARCH_CLUSTER_TARGET .
                    "=" .          "b." . CMS_DB_SEARCH_ENTRY_INDEX . " " .
                    "WHERE " .     "b." . CMS_DB_SEARCH_ENTRY_INDEX . " IS NULL");

        $table = [CMS_DB_SEARCH_ENTRY,
                  CMS_DB_SEARCH_WORD,
                  CMS_DB_SEARCH_WEIGHT,
                  CMS_DB_SEARCH_LINK,
                  CMS_DB_SEARCH_CLUSTER,
                  CMS_DB_SEARCH_QUEUE];
        $error = FALSE;

        //optimize tables
        foreach ($table AS $value)
            $error |= ! mysql_query("OPTIMIZE TABLE `" . sqlesc($value, TRUE) . "`");

        return ! $error;
    }

//--------------------------------------------------------------------------------------------------

    function address_standardize($address)
    {
        if (! $this->enabled) return FALSE;

        //verify address
        if (($address = analyze_url($address)) === FALSE) return FALSE; //invalid url

        switch ($address["scheme"])
        {
        case "":
        case "http":
        case "https":

            break;

        default:

            return FALSE;
        };

        //clean up querystring
        parse_str($address["query"], $array);
        $address["query"] = "";

        //order by key
        ksort($array, SORT_NATURAL | SORT_FLAG_CASE);

        foreach ($array AS $key => $value)
            $address["query"] .= "$key=$value&";

        $address["query"] = substr($address["query"], 0, -1);

        //filter parts
        $address = array_intersect_key(
            $address,
            ["scheme" => "", "host"  => "",
             "path"   => "", "query" => ""]);

        //reassemble address
        return cms_build_url($address);
    }

//--------------------------------------------------------------------------------------------------

    function address_accepted($address)
    {
        if (! $this->enabled) return FALSE;

        //check for address exclusion
        foreach ($this->address_accepted_blacklist AS $value)
        {
            if ($value === "") continue; //empty expression

            //seek inversion marker
            if ($value[0] === "!")
            {
                $value = substr($value, 1);
                $flag  = FALSE;
            }
            else
            {
                $flag = TRUE;
            };

            //exclusion match
            if ((bool)preg_match($value, $address) === $flag)
            {
                //check for address inclusion overriding exclusion
                foreach ($this->address_accepted_whitelist AS $value)
                {
                    if ($value === "") continue; //empty expression

                    //seek inversion marker
                    if ($value[0] === "!")
                    {
                        $value = substr($value, 1);
                        $flag  = FALSE;
                    }
                    else
                    {
                        $flag = TRUE;
                    };

                    //inclusion match
                    if ((bool)preg_match($value, $address) === $flag)
                        return TRUE; //accepted
                };

                return FALSE; //rejected
            };
        };

        return TRUE; //accepted
    }
};