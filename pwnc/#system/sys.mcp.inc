<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   MCP CLASS
//==================================================================================================

//related functions
//--------------------------------------------------------------------------------------------------

function mcp_http_header($name)
{
    static $array = NULL;

    if ($array === NULL)
    {
        if (function_exists("getallheaders"))
        {
            foreach (getallheaders() AS $key => $value)
                $array[strtolower($key)] = $value;
        }
        else
        {
            foreach ($_SERVER AS $key => $value)
            {
                switch ($key)
                {
                case "CONTENT_LENGTH":
                case "CONTENT_TYPE":

                    break;

                default:

                    if (substr($key, 0, 5) !== "HTTP_") continue 2;
                    $key = substr($key, 5);
                };

                $array[strtolower(str_replace("_", "-", $key))] = $value;
            };
        };
    };

    return $array[strtolower($name)] ?? NULL;
};

//--------------------------------------------------------------------------------------------------

function mcp_bundle()
{
    if (! cms_load("filemanager")) return FALSE; //no library

    $target_path = CMS_DATA_PATH . "#mcp/mcpb/";
    if (! mkpath($target_path)) return FALSE; //error

    $source_path = CMS_PATH . "mcp/#mcpb/";

    //semantic version
    $version = preg_match("/^(\d+\.\d+\.\d+)\.(\d+)$/", CMS_VERSION, $match) ?
               sprintf("%s%02d", $match[1], $match[2]) : "0.0.0";

    //manifest
    $manifest = [
        "manifest_version" => "0.3",
        "name"             => "PWNC/" . CMS_DOMAIN,
        "display_name"     => "PWNC (" . CMS_DOMAIN . ")",
        "version"          => $version,
        "description"      => "Connect to PWNC at " . CMS_DOMAIN . ".",
        "author"           => ["name" => CMS_DOMAIN],
        "server"           => [
            "type"        => "node",
            "entry_point" => "bridge.js",
            "mcp_config"  => [
                "command" => "node",
                "args"    => ["\${__dirname}/bridge.js"],
                "env"     => [
                    "PWNC_URL"   => CMS_MODULES_URL . "mcp.php",
                    "PWNC_TOKEN" => "\${user_config.api_key}"]]],
        "user_config"      => [
            "api_key" => [
                "type"        => "string",
                "title"       => "API key",
                "description" => "Enter the API key of the user account " .
                                 "the client should be able to act on behalf of.",
                "sensitive"   => TRUE,
                "required"    => TRUE]],
        "compatibility"    => [
            "platforms" => ["darwin", "win32"],
            "runtimes"  => ["node" => ">=18.0.0"]]];

    //get bundle content
    $list = filemanager_collect_recursive($source_path, $source_path);

    //icon
    if (isset($list["icon.png"])) $manifest["icon"] = "icon.png";

    //create archive
    $temp = unique_filename($target_path . unique_id() . ".mcpb");
    $zip  = new \ZipArchive();
    if ($zip->open($temp, \ZipArchive::CREATE | \ZipArchive::OVERWRITE) !== TRUE) return FALSE;

    //add manifest
    $zip->addFromString("manifest.json",
        json_encode($manifest, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE));

    //add bundle content
    foreach ($list AS $path => $is_directory)
    {
        if (in_array($path, ["", ".htaccess", "manifest.json"])) continue;

        $is_directory ?
        $zip->addEmptyDir($path) :
        $zip->addFile($source_path . $path, $path);
    };

    //close archive
    if ($zip->close())
    {
        $file = $target_path . "pwnc.mcpb";
        if (rename($temp, $file)) return $file;
    };

    //clean up on error
    if (is_file($temp)) unlink($temp);
    return FALSE;
};

//--------------------------------------------------------------------------------------------------

function mcp_config($key = "mcpServers")
{
    $identifier = "pwnc-" . trim(preg_replace("/[^0-9a-z]+/", "-", strtolower(CMS_DOMAIN)), "-");

    return json_encode([
        $key => [
            $identifier => [
                "type"    => "http",
                "url"     => CMS_MODULES_URL . "mcp.php",
                "headers" => ["Authorization" => "Bearer {API key}"]]]],
        JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES | JSON_UNESCAPED_UNICODE);
};

//--------------------------------------------------------------------------------------------------

function mcp_diff($source,
                  $target,
                  $source_name,
                  $target_name,
                  $context,
                  $limit,
                  $budget = 0.02)
{
    if ($source === $target) return "";

    //terminate with break
    if (($source !== "") && ($source[-1] !== "\n")) $source .= "\n";
    if (($target !== "") && ($target[-1] !== "\n")) $target .= "\n";

    $src_array  = preg_split("/\r?\n/", $source); //source lines
    $tar_array  = preg_split("/\r?\n/", $target); //target lines
    $src_count  = count($src_array) - 1;          //source lines minus terminator
    $tar_count  = count($tar_array) - 1;          //target lines minus terminator
    $prefix_len = 0;                              //matching prefix length
    $suffix_len = 0;                              //matching suffix length

    //trim common prefix
    while (
            ($prefix_len < $src_count)
            &&
            ($prefix_len < $tar_count)
            &&
            ($src_array[$prefix_len] === $tar_array[$prefix_len])
          )
        $prefix_len++;

    //trim common suffix
    while (
            (($src_count - $suffix_len) > $prefix_len)
            &&
            (($tar_count - $suffix_len) > $prefix_len)
            &&
            ($src_array[$src_count - $suffix_len - 1] ===
             $tar_array[$tar_count - $suffix_len - 1])
          )
        $suffix_len++;

//..................................................................................................

    //line numbering

    $src_line  = []; //source line number
    $src_token = []; //source line id
    $tar_line  = []; //target line number
    $tar_token = []; //target line id

    //skip if one side empty
    if (($src_count > 0) && ($tar_count > 0))
    {
        //distinct line numbering

        $map     = []; //map text line to id
        $side    = []; //id to 1 = source, 2 = target, 3 = both
        $src_id  = []; //id per source line
        $tar_id  = []; //id per target line
        $next_id = 0;  //next unseen line id

        //source lines, side 1
        for ($i = $prefix_len; $i < ($src_count - $suffix_len); $i++)
        {
            if (! isset($map[$src_array[$i]])) $side[$map[$src_array[$i]] = $next_id++] = 0;
            $side[$src_id[$i] = $map[$src_array[$i]]] |= 1;
        };

        //target lines, side 2
        for ($i = $prefix_len; $i < ($tar_count - $suffix_len); $i++)
        {
            if (! isset($map[$tar_array[$i]])) $side[$map[$tar_array[$i]] = $next_id++] = 0;
            $side[$tar_id[$i] = $map[$tar_array[$i]]] |= 2;
        };

        unset($map);

        //keep shared lines only
        for ($i = $prefix_len, $end = $src_count - $suffix_len; $i < $end; $i++)
        {
            if ($side[$src_id[$i]] === 3)
            {
                $src_line[]  = $i;
                $src_token[] = $src_id[$i];
            };
        };

        //same for target
        for ($i = $prefix_len, $end = $tar_count - $suffix_len; $i < $end; $i++)
        {
            if ($side[$tar_id[$i]] === 3)
            {
                $tar_line[]  = $i;
                $tar_token[] = $tar_id[$i];
            };
        };

        unset($side, $src_id, $tar_id);
    };

//..................................................................................................

    //early refusal

    $src_entries = count($src_line);
    $tar_entries = count($tar_line);

    //minimum possible result
    if (
         ($limit !== FALSE)
         &&
         ((($src_count - $prefix_len - $suffix_len) +
           ($tar_count - $prefix_len - $suffix_len) -
           2 * min($src_entries, $tar_entries))
          > $limit)
       )
        return FALSE;

//..................................................................................................

    //search

    $offset   = (int)(($src_entries + $tar_entries + 1) / 2) + 1; //shift diagonal to an index
    $forward  = array_fill(0, 2 * $offset + 2, 0);                //furthest x ahead per diagonal
    $backward = array_fill(0, 2 * $offset + 2, 0);                //furthest x back per diagonal
    $region   = [];                                               //edit regions
    $removed  = [];                                               //removed source lines
    $added    = [];                                               //added target lines
    $edits    = 0;                                                //count so far
    $src_next = $prefix_len;                                      //next source line
    $tar_next = $prefix_len;                                      //next target line
    $deadline = microtime(TRUE) + $budget;                        //search until here
    $steps    = 0;                                                //since clock was read
    $spent    = FALSE;                                            //time ran out

    //closing entry past last -> flush tail
    $src_line[] = $src_count - $suffix_len;
    $tar_line[] = $tar_count - $suffix_len;

    $stack = [[3, $src_entries, $tar_entries, 1],     //run of paired up entries
              [0, 0, $src_entries, 0, $tar_entries]]; //region still to divide

    while ($stack)
    {
        if (($limit !== FALSE) && ($edits > $limit)) return FALSE;

        $item = array_pop($stack);

//..................................................................................................

        //pair run

        if ($item[0] === 3)
        {
            [, $src_p, $tar_p, $length] = $item;

            for ($pair = 0; $pair < $length; $pair++)
            {
                //pair positions
                $s = $src_line[$src_p + $pair];
                $t = $tar_line[$tar_p + $pair];

                //gap = one edit run
                if (($src_next < $s) || ($tar_next < $t))
                {
                    $edits    += ($s - $src_next) + ($t - $tar_next);
                    $region[]  = [$src_next + 1, $s + 1, $tar_next + 1, $t + 1];

                    //mark line by line
                    for (; $src_next < $s; $src_next++) $removed[$src_next + 1] = TRUE;
                    for (; $tar_next < $t; $tar_next++) $added[$tar_next + 1]   = TRUE;
                };

                //unchanged, step past
                $src_next = $s + 1;
                $tar_next = $t + 1;
            };

            continue;
        };

        //region to divide
        [, $src_lo, $src_hi, $tar_lo, $tar_hi] = $item;

        $src_size = $src_hi - $src_lo;
        $tar_size = $tar_hi - $tar_lo;

        //nothing to pair
        if (($src_size === 0) || ($tar_size === 0)) continue;

        //backward start diagonal
        $delta      = $src_size - $tar_size;
        $odd        = (($delta & 1) !== 0);                   //decides which search finds overlap
        $_limit     = (int)(($src_size + $tar_size + 1) / 2); //search bound
        $edit_first = TRUE;                                   //edit precedes snake
        $best       = -1;                                     //furthest forward point
        $found      = FALSE;                                  //middle snake settled on

        //seed first round
        $forward[$offset + 1]  = 0;
        $backward[$offset + 1] = 0;

        for ($dist = 0; $dist <= $_limit; $dist++)
        {

//..................................................................................................

            //forward search

            for ($diag = -$dist; $diag <= $dist; $diag += 2)
            {
                //extend better neighbor diagonal
                if (
                     ($diag === -$dist)
                     ||
                     (
                       ($diag !== $dist)
                       &&
                       ($forward[$offset + $diag - 1] < $forward[$offset + $diag + 1])
                     )
                   )
                {
                    //from above -> x same
                    $prev_x = $x = $forward[$offset + $diag + 1];
                }
                else
                {
                    //from below -> x advances
                    $prev_x = $forward[$offset + $diag - 1];
                    $x      = $prev_x + 1;
                };

                //prev marks point before edit step
                $y      = $x - $diag;
                $prev_y = (($dist === 0) || ($x !== $prev_x)) ? $y : $y - 1;

                //follow snake
                while (
                        ($x < $src_size) && ($y < $tar_size)
                        &&
                        ($src_token[$src_lo + $x] === $tar_token[$tar_lo + $y])
                      )
                {
                    $x++;
                    $y++;
                };

                //record reach
                $forward[$offset + $diag] = $x;

                //remember furthest
                if (($x + $y) > $best)
                {
                    $best     = $x + $y;
                    $best_src = $x;
                    $best_tar = $y;
                };

                //overlaps backward
                if (
                     $odd
                     &&
                     (($delta - $diag) >= (1 - $dist))
                     &&
                     (($delta - $diag) <= ($dist - 1))
                     &&
                     (($x + $backward[$offset + $delta - $diag]) >= $src_size)
                   )
                {
                    $snake_src     = $prev_x;
                    $snake_tar     = $prev_y;
                    $snake_src_end = $x;
                    $snake_tar_end = $y;
                    $found         = TRUE;
                    break 2;
                };
            };

            //clock check
            $steps += $dist + 1;

            if (! $spent && ($steps > 2048))
            {
                $steps = 0;
                $spent = (microtime(TRUE) > $deadline);
            };

//..................................................................................................

            //backward search

            for ($diag = -$dist; $diag <= $dist; $diag += 2)
            {
                //extend better neighbor diagonal
                if (
                     ($diag === -$dist)
                     ||
                     (
                       ($diag !== $dist)
                       &&
                       ($backward[$offset + $diag - 1] < $backward[$offset + $diag + 1])
                     )
                   )
                {
                    //from above -> x same
                    $prev_x = $x = $backward[$offset + $diag + 1];
                }
                else
                {
                    //from below -> x advances
                    $prev_x = $backward[$offset + $diag - 1];
                    $x      = $prev_x + 1;
                };

                //prev marks point before edit step
                $y      = $x - $diag;
                $prev_y = (($dist === 0) || ($x !== $prev_x)) ? $y : $y - 1;

                //follow snake
                while (
                        ($x < $src_size) && ($y < $tar_size)
                        &&
                        ($src_token[$src_hi - $x - 1] === $tar_token[$tar_hi - $y - 1])
                      )
                {
                    $x++;
                    $y++;
                };

                //record reach
                $backward[$offset + $diag] = $x;

                //overlaps forward, same round
                if (
                     ! $odd
                     &&
                     (($delta - $diag) >= -$dist)
                     &&
                     (($delta - $diag) <= $dist)
                     &&
                     (($x + $forward[$offset + $delta - $diag]) >= $src_size)
                   )
                {
                    //mirror back
                    $snake_src     = $src_size - $x;
                    $snake_tar     = $tar_size - $y;
                    $snake_src_end = $src_size - $prev_x;
                    $snake_tar_end = $tar_size - $prev_y;
                    $edit_first    = FALSE;
                    $found         = TRUE;
                    break 2;
                };
            };

            //time out
            if ($spent && ($dist > 0) && ($best > 0))
            {
                $snake_src = $snake_src_end = $best_src;
                $snake_tar = $snake_tar_end = $best_tar;
                $found     = TRUE;
                break;
            };
        };

        //guard against stale values
        if (! $found) continue;

        //snake extent
        $src_span = $snake_src_end - $snake_src;
        $tar_span = $snake_tar_end - $snake_tar;
        $paired   = min($src_span, $tar_span);

//..................................................................................................

        //divide region

        //region after snake
        $stack[] = [0, $src_lo + $snake_src_end, $src_hi, $tar_lo + $snake_tar_end, $tar_hi];

        //snake pairs
        if ($paired > 0)
            $stack[] = $edit_first ?
                       [3, $src_lo + $snake_src + $src_span - $paired,
                        $tar_lo + $snake_tar + $tar_span - $paired, $paired] :
                       [3, $src_lo + $snake_src, $tar_lo + $snake_tar, $paired];

        //region before snake, popped first
        $stack[] = [0, $src_lo, $src_lo + $snake_src, $tar_lo, $tar_lo + $snake_tar];
    };

    //over limit, or nothing changed
    if (($limit !== FALSE) && ($edits > $limit)) return FALSE;
    if (! $region)                               return "";

//..................................................................................................

    //group context

    $block = [];

    foreach ($region AS $part)
    {
        //context overlaps -> merge
        if ($block && (($part[0] - $block[count($block) - 1][1]) <= (2 * $context)))
        {
            $block[count($block) - 1][1] = $part[1];
            $block[count($block) - 1][3] = $part[3];
            continue;
        };

        //too far -> new block
        $block[] = $part;
    };

//..................................................................................................

    //rendering

    $return = ["--- " . $source_name, "+++ " . $target_name];

    foreach ($block AS [$src_edit_start, $src_edit_stop,
                        $tar_edit_start, $tar_edit_stop])
    {
        //add context
        $src_start = max(1, $src_edit_start - $context);
        $tar_start = max(1, $tar_edit_start - $context);

        $src_end = min($src_count, $src_edit_stop + $context - 1);
        $tar_end = min($tar_count, $tar_edit_stop + $context - 1);

        $src_shown = max(0, $src_end - $src_start + 1);
        $tar_shown = max(0, $tar_end - $tar_start + 1);

        //nothing to show
        if (($src_shown === 0) && ($tar_shown === 0)) continue;

        //empty side starts earlier
        $src_from = $src_shown ? $src_start : $src_start - 1;
        $tar_from = $tar_shown ? $tar_start : $tar_start - 1;

        //block header
        $return[] = "@@ -$src_from,$src_shown +$tar_from,$tar_shown @@";

        //walk both sides
        $src_n = $src_start;
        $tar_n = $tar_start;

        while (($src_n <= $src_end) || ($tar_n <= $tar_end))
        {
            //deletions first
            if (($src_n <= $src_end) && isset($removed[$src_n]))
            {
                $return[] = "-" . $src_array[$src_n++ - 1];
                continue;
            };

            //then additions
            if (($tar_n <= $tar_end) && isset($added[$tar_n]))
            {
                $return[] = "+" . $tar_array[$tar_n++ - 1];
                continue;
            };

            //unchanged -> advance both
            if (($src_n <= $src_end) && ($tar_n <= $tar_end))
            {
                $return[] = " " . $src_array[$src_n - 1];
                $src_n++;
                $tar_n++;
                continue;
            };

            //neither side has more
            break;
        };
    };

    return implode("\n", $return) . "\n";
};

class mcp
{

//properties
//--------------------------------------------------------------------------------------------------

    static $json           = NULL;  //request data
    static $flag           = FALSE; //reentrancy guard
    static $abort          = FALSE; //cancels shutdown processing
    static $post_temp      = [];    //temporary upload file paths
    static $message        = NULL;  //system message
    static $max_length     = 32000; //maximum response size
    static $filter_context = 3;     //context lines per filter match

    //response diff constraints
    static $diff_context = 3;    //context lines per block
    static $diff_limit   = 1.0;  //edits per target line
    static $diff_margin  = 0.9;  //share of full response
    static $diff_budget  = 0.02; //seconds of search
    static $diff_expire  = 3600; //seconds of recall

//methods
//--------------------------------------------------------------------------------------------------

    static private function send_output($uri         = NULL,
                                        $diff        = TRUE,
                                        $line_offset = 0)
    {
        $content = NULL;

        //scan headers
        $header = headers_list();
        foreach ($header AS $value)
        {
            if (preg_match("/^(Content-Type|Location):\s*(\S+)/i", $value, $match))
            {
                switch (strtolower($match[1]))
                {
                case "content-type":

                    $type = substr($match[2], 0, strcspn($match[2], ";"));
                    break;

                case "location":

                    $type    = "text/plain";
                    $content = ["text" => "Redirects to: " . $match[2]];
                    ob_end_clean();
                    break 2;
                };
            };
        };

        //determine url
        $uri ??= isset(self::$json["params"]["arguments"]["target_url"]) ?
                 absolute_path(CMS_ROOT_URL, self::$json["params"]["arguments"]["target_url"]) :
                 CMS_ACTIVE_URL;


        //process response
        if ($content === NULL)
        {
            $type      ??= "text/html";
            $buffer      = (string)ob_get_clean();
            $cache_key   = "agent.response." . CMS_SUPERUSER . ".";

            //binary
            if (! utf8_detect($buffer))
            {
                //encode binary content
                $content = ["blob" => base64_encode($buffer)];
            }
            else
            {
                if ($diff)
                {
                    //break after tags outside script/style
                    if ($type === "text/html")
                        $buffer = preg_replace(
                            "/<(script|style)\b.*?<\/\\1\s*(*SKIP)(*F)|>(?=[^\n])/is",
                            ">¬\n",
                            $buffer);

                    //retrieve previous response
                    $plist = self::response_list();
                    $hash  = hash32(normalize_url($uri) ?: $uri);
                    $data  = cms_cache($cache_key . $hash, NULL, FALSE, self::$diff_expire);

                    if (($data === NULL) && ($_hash = $plist->get(0, 1)[0] ?? FALSE))
                        $data = cms_cache($cache_key . $_hash, NULL, FALSE, self::$diff_expire);
                };

                //skip line numbering
                if ($line_offset === FALSE)
                {
                    $n    = 0;
                    $full = $buffer;
                }

                //number lines
                else
                {
                    $n    = $line_offset;
                    $full = preg_replace_callback(
                        "/^/m",
                        function() use (&$n) { return ++$n . ": "; },
                        $buffer);
                };

                //cache retrieved
                if (isset($data))
                {
                    //retrieve unified diff
                    $_diff = mcp_diff(
                        $data["text"],
                        $buffer,
                        $data["uri"],
                        $uri,
                        self::$diff_context,
                        (int)($n * self::$diff_limit),
                        self::$diff_budget);

                    if (is_string($_diff))
                    {
                        //unchanged
                        if ($_diff === "") $_diff = "--- " . $data["uri"] . "\n+++ " . $uri . "\n";

                        //use smaller response
                        if (strlen($_diff) < (strlen($full) * self::$diff_margin))
                            $content = ["text" => $_diff];
                    };
                };

                //store response
                if ($diff)
                {
                    cms_cache(
                        $cache_key . $hash,
                        ["uri"  => $uri,
                         "type" => $type,
                         "text" => $buffer],
                        TRUE,
                        self::$diff_expire);
                    $plist->add($hash, TRUE);
                };

                //full response
                if ($content === NULL)
                {
                    //truncate large response
                    if ($diff && (($length = strlen($full)) > self::$max_length))
                    {
                        $pos    = strrpos($full, "\n", self::$max_length - $length);
                        $length = ($pos === FALSE) ? self::$max_length : $pos;
                        $full   = substr($full, 0, $length) . "\n" .
                                  "~~~ truncated ($n lines total) ~~~\n" .
                                  "Use `get_response` to retrieve further lines.";
                    };

                    $content = ["text" => $full];
                };
            };
        };

        //send output as response
        http_response_code(200); //ok
        header_remove();
        header("Content-Type: application/json; charset=utf-8");
        echo(json_encode([
            "jsonrpc" => "2.0",
            "id"      => self::$json["id"] ?? NULL,
            "result"  => [
                "resultType" => "complete",
                "content"    => array_merge(nstre(self::$message) ? [[

                    //system message
                    "type"     => "text",
                    "text"     => self::$message]] : [], [[

                    //http headers
                    "type"     => "text",
                    "text"     => implode("\n", $header)]], [[

                    //content
                    "type"     => "resource",
                    "resource" => ["uri"      => $uri,
                                   "mimeType" => $type]
                    + $content]])]]));
    }

//--------------------------------------------------------------------------------------------------

    static private function send_result($content,
                                        $error = FALSE)
    {
        self::$abort = TRUE; //cancel get/post shutdown execution

        //send tool result
        while (ob_get_level()) ob_end_clean();
        http_response_code(200); //ok
        header_remove();
        header("Content-Type: application/json; charset=utf-8");
        echo(json_encode([
            "jsonrpc" => "2.0",
            "id"      => self::$json["id"] ?? NULL,
            "result"  => [
                "resultType" => "complete",
                "content"    => array_merge(nstre(self::$message) ? [[

                    //system message
                    "type" => "text",
                    "text" => self::$message]] : [], [[

                    //content
                    "type" => "text",
                    "text" => $content]])]
            + ($error ? ["isError" => TRUE] : [])]));
        exit();
    }

//--------------------------------------------------------------------------------------------------
//   SERVER
//--------------------------------------------------------------------------------------------------

    static function server_discover()
    {
        self::$abort = TRUE; //cancel get/post shutdown execution

        while (ob_get_level()) ob_end_clean();
        http_response_code(200); //ok
        header_remove();
        header("Content-Type: application/json; charset=utf-8");
        echo(json_encode([
            "jsonrpc" => "2.0",
            "id"      => self::$json["id"] ?? NULL,
            "result"   => [
                "_meta"   => [
                    "io.modelcontextprotocol/serverInfo" => [
                        "name"    => CMS_SOFTWARE,
                        "version" => CMS_VERSION]],
                "resultType"        => "complete",
                "ttlMs"             => 60000,
                "cacheScope"        => "private",
                "supportedVersions" => ["2026-07-28"],
                "instructions"      => self::system_prompt(NULL, CMS_SUPERUSER, NULL, TRUE),
                "capabilities"      =>
                    ["tools"     => (object)[],
                     "resources" => (object)[]]]]));
        exit();
    }

//--------------------------------------------------------------------------------------------------
//   TOOLS
//--------------------------------------------------------------------------------------------------

    static function tools_list()
    {
        self::$abort = TRUE; //cancel get/post shutdown execution

        //clear responses
        $plist = self::response_list();
        foreach ($plist->get(0, 0) ?: [] AS $value)
            cms_cache_delete("agent.response." . CMS_SUPERUSER . ".$value", TRUE);
        $plist->remove(0, 0);

        //send tool list
        while (ob_get_level()) ob_end_clean();
        http_response_code(200); //ok
        header_remove();
        header("Content-Type: application/json; charset=utf-8");
        echo(json_encode([
            "jsonrpc" => "2.0",
            "id"      => self::$json["id"] ?? NULL,
            "result"  => [
                "resultType" => "complete",
                "ttlMs"      => 60000,
                "cacheScope" => "private",
                "tools"      => self::tool_list()]]));
        exit();
    }

//--------------------------------------------------------------------------------------------------

    static function tools_get()
    {
        if (self::$flag) return FALSE; self::$flag = TRUE; //reexecution block

        //shutdown function
        register_shutdown_function(function()
        {
            if (mcp::$abort) return;

            //send response
            mcp::send_output();
        });

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    static function tools_post()
    {
        if (self::$flag) return FALSE; self::$flag = TRUE; //reexecution block

        //shutdown function
        register_shutdown_function(function()
        {
            if (mcp::$abort) return;

            //temp file cleanup
            foreach (mcp::$post_temp AS $temp)
                if (is_file($temp)) unlink($temp);

            //send response
            mcp::send_output();
        });

        //set request method
        $_SERVER["REQUEST_METHOD"] = "POST";

        //no data
        if (! is_array(self::$json["params"]["arguments"]["form_data"] ?? NULL))
        {
            //reset superglobals
            $_POST    = $_FILES = [];
            $_REQUEST = array_merge($_GET, $_COOKIE);
            return;
        };

        //overload method so it treats file arrays as leaves
        $iterator = new \RecursiveIteratorIterator(
            new class(self::$json["params"]["arguments"]["form_data"]) extends \RecursiveArrayIterator
            {
                public function hasChildren(): bool
                {
                    $value = $this->current();
                    return is_array($value) && ! isset($value["data"]);
                }
            },
            \RecursiveIteratorIterator::LEAVES_ONLY);

        //build query strings
        $post = $files = [];
        foreach ($iterator AS $value)
        {
            //collect keys and build prefix
            $depth  = $iterator->getDepth();
            $prefix = (string)$iterator->getSubIterator(0)->key();
            $suffix = "";

            for ($i = 1; $i <= $depth; $i++)
            {
                $key     = (string)$iterator->getSubIterator($i)->key();
                $prefix .= "[$key]";
            };

            //scalar
            if (! is_array($value) || ! isset($value["data"]))
            {
                $post[] = urlencode($prefix) . "=" . urlencode((string)$value);
                continue;
            };

            //file
            if ($depth && ctype_digit($key))
            {
                $suffix = urlencode(substr($prefix, strrpos($prefix, "[")));
                $prefix = substr($prefix, 0, strrpos($prefix, "["));
            };

            //save file
            $temp  = tempnam(sys_get_temp_dir(), "mcp");
            $error = UPLOAD_ERR_NO_FILE;
            $size  = 0;

            if ($temp === FALSE)
            {
                $error = UPLOAD_ERR_NO_TMP_DIR;
            }
            else
            {
                $hfile = fopen($temp, "w+b");
                if ($hfile === FALSE)
                {
                    $error = UPLOAD_ERR_CANT_WRITE;
                }
                else
                {
                    stream_filter_append(
                        $hfile,
                        "convert.base64-decode",
                        STREAM_FILTER_WRITE);
                    fwrite($hfile, (string)$value["data"]);
                    fclose($hfile);

                    if (is_file($temp) && (($size = filesize($temp)) !== FALSE))
                    {
                        self::$post_temp[$temp] = $temp;
                        $error = UPLOAD_ERR_OK;
                    }
                    else
                    {
                        $error = UPLOAD_ERR_CANT_WRITE;
                    };
                };
            };

            //add file data
            $prefix   = urlencode($prefix);
            $filename = (string)($value["filename"] ?? $value["name"] ?? "");
            $type     = (string)($value["type"] ?? get_mime_type($filename));

            $files[] = $prefix . "%5Bname%5D" .     $suffix . "=" . urlencode($filename);
            $files[] = $prefix . "%5Btype%5D" .     $suffix . "=" . urlencode($type);
            $files[] = $prefix . "%5Btmp_name%5D" . $suffix . "=" . urlencode((string)$temp);
            $files[] = $prefix . "%5Berror%5D" .    $suffix . "=" . $error;
            $files[] = $prefix . "%5Bsize%5D" .     $suffix . "=" . $size;
        };

        //import data
        parse_str(implode("&", $post), $_POST);
        parse_str(implode("&", $files), $_FILES);
        $_REQUEST = array_merge($_GET, $_POST, $_COOKIE);
    }

//--------------------------------------------------------------------------------------------------

    static function tools_get_response($data = NULL)
    {
        self::$abort = TRUE; //cancel get/post shutdown execution

        //retrieve response
        if ($data === NULL)
        {
            $url = self::$json["params"]["arguments"]["url"] ?? "";

            //specified url
            if (nstre($url))
            {
                $url  = absolute_path(CMS_ROOT_URL, $url);
                $hash = hash32(normalize_url($url) ?: $url);
            }

            //previous url
            else
            {
                $hash = self::response_list()->get(0, 1)[0] ?? FALSE;
            };

            //retrieve from cache
            if ($hash)
                $data = cms_cache("agent.response." . CMS_SUPERUSER . ".$hash", NULL, FALSE, self::$diff_expire);
        };

        if (! is_array($data)) self::send_error("No data.");

        $argument = self::$json["params"]["arguments"] ?? [];
        $text     = $data["text"];
        $length   = strlen($text);

        //extract range
        $line_start  = isset($argument["line_start"]) ? max((int)$argument["line_start"], 1) : 1;
        $line_end    = isset($argument["line_end"]) ? max((int)$argument["line_end"], $line_start) : 0;
        $has_range   = isset($argument["line_start"]) || isset($argument["line_end"]);
        $line_offset = $line_start - 1;

        if ($has_range)
        {
            $pos    = 0;
            $line   = 1;
            $offset = -1;
            $end    = $length;

            while ($pos < $length)
            {
                //seek line end
                $_pos = strpos($text, "\n", $pos);
                if ($_pos === FALSE) $_pos = $length; //eof

                //collect range
                if ($line === $line_start)   $offset = $pos;
                if ($line === $line_end)   { $end    = $_pos + 1; break; };

                //next line
                $pos = $_pos + 1;
                $line++;
            };

            $text   = ($offset === -1) ? "" : substr($text, $offset, $end - $offset);
            $length = strlen($text);
        };

        //filter response
        $filter = $argument["filter"] ?? "";

        if ($filter !== "")
        {
            $buffer = [];
            $window = [];
            $pos    = 0;
            $line   = $line_start;
            $prev   = 0;
            $count  = 0;

            while ($pos < $length)
            {
                //seek line end
                $_pos = strpos($text, "\n", $pos);
                if ($_pos === FALSE) $_pos = $length; //eof

                //extract line
                $_line = substr($text, $pos, $_pos - $pos);

                //match found
                if (strpos($_line, $filter) !== FALSE)
                {
                    //delimit disconnected blocks
                    $start = $count ? $line : ($line - count($window));
                    if ($prev && ($start > ($prev + 1))) $buffer[] = "~~~ snip ~~~";

                    //collect window
                    if ($count === 0)
                    {
                        foreach ($window AS $i => $value)
                            $buffer[] = ($start + $i) . ": " . $value;
                    };

                    //collect matching line
                    $buffer[] = "$line: $_line";
                    $window   = [];
                    $prev     = $line;
                    $count    = self::$filter_context;
                }

                //collect trailing context
                elseif ($count > 0)
                {
                    $buffer[] = "$line: $_line";
                    $prev     = $line;
                    if (--$count === 0) $window = []; //clear
                }

                //feed window
                else
                {
                    $window[] = $_line;
                    if (count($window) > self::$filter_context) array_shift($window);
                };

                //next line
                $pos = $_pos + 1;
                $line++;
            };

            //output
            $text        = $buffer ? implode("\n", $buffer) : "No match.";
            $line_offset = FALSE;
        };

        //send response
        while (ob_get_level()) ob_end_clean();
        ob_start();
        http_response_code(200);
        header("Content-Type: " . $data["type"]);
        echo($text);
        self::send_output($data["uri"], FALSE, $line_offset);
        exit();
    }

//--------------------------------------------------------------------------------------------------

    static function tools_resources_list()
    {
        self::$abort = TRUE; //cancel get/post shutdown execution

        //render markdown list
        $text = "# PWNC resources\n\n";

        foreach (self::get_resource_list() AS $entry)
            $text .= self::resource_list($entry) . "\n";

        $text .= "\nUse `resources_read` to retrieve.";

        self::send_result($text);
    }

//--------------------------------------------------------------------------------------------------

    static function tools_readme_first()
    {
        //set system message outside agent loop only
        self::$message = cms_token_check(CMS_MCP_AUTH) ? NULL : self::system_prompt(self::$message);

        //send with readme
        self::tools_resources_read("pwnc:README.md");
    }

//--------------------------------------------------------------------------------------------------

    static function tools_resources_read($uri = NULL)
    {
        self::$abort = TRUE; //cancel get/post shutdown execution

        //retrieve resource
        $uri  ??= self::$json["params"]["arguments"]["uri"] ?? "";
        $data   = self::get_resource($uri);

        if (isset($data["error"])) self::send_error($data["error"][1]);

        //send text
        if (utf8_detect($data["data"]))
            self::tools_get_response(
                ["uri"  => $uri,
                 "type" => $data["type"],
                 "text" => $data["data"]]);

        //send binary
        while (ob_get_level()) ob_end_clean();
        ob_start();
        http_response_code(200);
        header("Content-Type: " . $data["type"]);
        echo($data["data"]);

        self::send_output($uri, FALSE);
        exit();
    }

//--------------------------------------------------------------------------------------------------
//   TOOL HELPERS
//--------------------------------------------------------------------------------------------------

    static function tool_verify()
    {
        $name      = self::$json["params"]["name"]      ?? NULL;
        $arguments = self::$json["params"]["arguments"] ?? [];
        if (! is_array($arguments)) self::send_error("Invalid arguments, expects object.");

        //type checks
        $check = ["string"  => "is_string",
                  "integer" => function($value) { return is_numeric($value) && ((int)$value == $value); },
                  "boolean" => "is_bool",
                  "object"  => "is_array"];

        foreach (self::tool_list() AS $tool)
        {
            if ($tool["name"] !== $name) continue; //not called

            $schema = $tool["inputSchema"];

            //required
            foreach ($schema["required"] ?? [] AS $value)
                if (! isset($arguments[$value])) self::send_error("Missing `$value`."); //not found

            //properties
            foreach ($schema["properties"] ?? [] AS $key => $value)
            {
                if (! isset($arguments[$key])) continue; //not set

                //check for allowed type
                $list = (array)$value["type"];
                foreach ($list AS $type) if ($check[$type]($arguments[$key])) continue 2; //valid

                //invalid type
                self::send_error("Invalid `$key`, expects " . implode(" or ", $list) . ".");
            };

            break;
        };
    }

//--------------------------------------------------------------------------------------------------

    static private function tool_list()
    {
        //check if local
        $flag = cms_token_check(CMS_MCP_AUTH);

        return [[

//..................................................................................................

            "name"        => "readme_first",
            "description" =>
                "Platform instructions. Call before any other tool.",
            "inputSchema" => [
                "type"                 => "object",
                "additionalProperties" => FALSE]], [

//..................................................................................................

            "name"        => "get",
            "description" =>
                "Fetch data from any URL including HTTP headers.",
            "inputSchema" => CMS_MCP ? [
                "type"       => "object",
                "properties" => [
                    "target_url" => [
                        "type"        => "string",
                        "description" =>
                            "Target URL, defaults to `pwnc/module/desktop.php`."]],
                "additionalProperties" => FALSE] : [
                "type"                 => "object",
                "additionalProperties" => FALSE]], [

//..................................................................................................

            "name"        => "post",
            "description" =>
                "Submit form data to any URL and receive response including HTTP headers.",
            "inputSchema" => [
                "type"       => "object",
                "required"   => ["form_data"],
                "properties" => (CMS_MCP ? [
                    "target_url" => [
                        "type"        => "string",
                        "description" =>
                            "Target URL, defaults to `pwnc/module/desktop.php`."]] : []) + [
                    "form_data" => [
                        "type"        => "object",
                        "description" =>
                            "Map of field names to values.\n\n" .
                            "Single value → scalar field; array → multi-value field. " .
                            "An array containing a `data` field is interpreted as a file:\n" .
                            "* `{filename,type,data:base64}` — single file\n" .
                            "* `[{filename,type,data:base64}]` — multiple files"]],
                "additionalProperties" => FALSE]], [

//..................................................................................................

            "name"        => "get_response",
            "description" =>
                "Show last received or previously stored response, " .
                "optionally specifying line range (1-based, inclusive) and filter string.",
            "inputSchema" => [
                "type"       => "object",
                "properties" => [
                    "url" => [
                        "type"        => "string",
                        "description" => "URL of previous response, empty → last."],
                    "line_start" => [
                        "type"        => "integer",
                        "description" => "First line to show, empty → start."],
                    "line_end" => [
                        "type"        => "integer",
                        "description" => "Last line to show, empty → end."],
                    "filter" => [
                        "type"        => "string",
                        "description" =>
                            "Show blocks matching filter separated by \"~~~ snip ~~~\"."]],
                "additionalProperties" => FALSE]], [

//..................................................................................................

            "name"        => "resources_list",
            "description" => "List available resources. Look for **SKILLS** here.",
            "inputSchema" => [
                "type"                 => "object",
                "additionalProperties" => FALSE]], [

//..................................................................................................

            "name"        => "resources_read",
            "description" =>
                "Read resource given by `resources_list` or `memory_recall`, " .
                "optionally specifying line range (1-based, inclusive) and filter string.",
            "inputSchema" => [
                "type"       => "object",
                "required"   => ["uri"],
                "properties" => [
                    "uri" => [
                        "type"        => "string",
                        "description" => "Resource URI"],
                    "line_start" => [
                        "type"        => "integer",
                        "description" => "First line to show, empty → start."],
                    "line_end" => [
                        "type"        => "integer",
                        "description" => "Last line to show, empty → end."],
                    "filter" => [
                        "type"        => "string",
                        "description" =>
                            "Show blocks matching filter separated by \"~~~ snip ~~~\"."]],
                "additionalProperties" => FALSE]], [

//..................................................................................................

            "name"        => "memory_remember",
            "description" =>
                "Create or update memory.\n\n" .
                "Omitted values are preserved, `false` clears.",
            "inputSchema" => [
                "type"       => "object",
                "properties" => [
                    "id" => [
                        "type"        => "string",
                        "description" => "`{region}` → create memory, " .
                                            "`{region}/{number}` → update memory. " .
                                            "Region is `scope.{token}` or `user.{username}`, " .
                                            "defaults to personal region."],
                    "mnemonic" => [
                        "type"        => ["string", "boolean"],
                        "description" => "Retrieval cues, comma or newline separated."],
                    "description" => [
                        "type"        => ["string", "boolean"],
                        "description" => "Summary."],
                    "content" => [
                        "type"        => ["string", "boolean"],
                        "description" => "Text."],
                    "attachment" => [
                        "type"        => ["object", "boolean"],
                        "description" => "`{type,data:base64}` — MIME type and file content. " .
                                            "`false` deletes."],
                    "followup" => [
                        "type"        => ["string", "boolean"],
                        "description" => "Resurface memory at date `Y-m-d H:i:s` until read, " .
                                            "`false` cancels."],
                    "read_only" => [
                        "type"        => "boolean",
                        "description" => "Restrict editing to owner and operators."],
                    "safe" => [
                        "type"        => "boolean",
                        "description" => "Protect from expiry and deletion. " .
                                            "Must be cleared before deleting."]],
                "additionalProperties" => FALSE]], [

//..................................................................................................

            "name"        => "memory_recall",
            "description" =>
                "Recall memory.\n\n" .
                "Parameters determine mode:\n" .
                "* none — list accessible regions\n" .
                "* `query` — search accessible regions, mnemonics prefilter\n" .
                "* `{region}` — show mnemonic cloud and list relevant memories\n" .
                "* `{region}` + `query` — fulltext search specified region\n" .
                "* `{region}/{number}` — show memory",
            "inputSchema" => [
                "type"       => "object",
                "properties" => [
                    "id" => [
                        "type"        => "string",
                        "description" =>
                            "`{region}` → explore or fulltext search, " .
                            "`{region}/{number}` → show memory, wins over `query`. " .
                            "Number is from the listing, mnemonics are not ids. " .
                            "Region is `scope.{token}` or `user.{username}`, " .
                            "defaults to personal region."],
                    "query" => [
                        "type"        => "string",
                        "description" => "Search terms."]],
                "additionalProperties" => FALSE]], [

//..................................................................................................

            "name"        => "memory_rate",
            "description" =>
                "Rate memory from `1` to `5`, delete with `0`.\n\n" .
                "Low rated memories expire faster. " .
                "Owner and operators can delete, unless `safe` is set.",
            "inputSchema" => [
                "type"       => "object",
                "required"   => ["id", "rating"],
                "properties" => [
                    "id" => [
                        "type"        => "string",
                        "description" =>
                            "Memory to rate, `{region}/{number}`. " .
                            "Region is `scope.{token}` or `user.{username}`, " .
                            "defaults to personal region."],
                    "rating" => [
                        "type"        => "integer",
                        "description" => "`1` to `5`, `0` to delete."]],
                "additionalProperties" => FALSE]], [

//..................................................................................................

            "name"        => "whois",
            "description" => "Look up user or group information.",
            "inputSchema" => [
                "type"       => "object",
                "properties" => [
                    "key" => [
                        "type"        => "string",
                        "description" =>
                            "`user.{username}` or `group.{groupname}`, empty → self."]],
                "additionalProperties" => FALSE]], [

//..................................................................................................

            "name"        => "call",
            "description" =>
                "Send a `message` to `key` in an external run. " . ($flag ?
                "End your turn to let the target process. " .
                "You will **automatically resume** on reply or fail." :
                "The target will reply in your personal memory region. " .
                "Check your follow-ups later.") .
                "\n\n**When being called**, your last reply " .
                "will **automatically** be relayed to the caller. " .
                "To **respond to an answered call**, use the `call` tool again.",
            "inputSchema" => [
                "type"       => "object",
                "required"   => ["key", "message"],
                "properties" => [
                    "key" => [
                        "type"        => "string",
                        "description" => "`user.{username}`."],
                    "message" => [
                        "type"        => "string",
                        "description" => "Message."],
                    "id" => [
                        "type"        => ["string", "boolean"],
                        "description" => "Context id — " . ($flag ?
                            "omit to remain in current context. " :
                            "**pass in subsequent calls** to remain in established context. ") .
                            "`false` starts a new context. " .
                            "Pass a specific id to target that context."]],
                "additionalProperties" => FALSE]], [

//..................................................................................................

            "name"        => "call_reply",
            "description" =>
                "**Do NOT call this tool.** " .
                "It is an envelope to asynchronously deliver replies " .
                "from agents you messaged using the `call` tool.",
            "inputSchema" => [
                "type"       => "object",
                "properties" => [
                    "key" => [
                        "type"        => "string",
                        "description" =>
                            "`user.{username}` of the agent replying to your previous `call`."],
                    "id" => [
                        "type"        => "string",
                        "description" =>
                            "Context id of your exchange with `key`. " .
                            "Pass it in `call` to resume correspondency."]],
                "additionalProperties" => FALSE]]];
    }

//--------------------------------------------------------------------------------------------------
//   MEMORY
//--------------------------------------------------------------------------------------------------

    static private function memory_address($value = NULL)
    {
        if (! preg_match("/^\s*([^\/]+)?\/?(\d+)?\s*$/", (string)$value, $match))
            self::send_error("Invalid `id`, expects `{region}/{number}`.");

        $region = $match[1] ?? NULL;
        $id     = $match[2] ?? NULL;

        //number only
        if (stre($id) && ctype_digit((string)$region))
        {
            $id     = $region;
            $region = NULL;
        };

        return [stre($region) ? "user." . CMS_SUPERUSER : $region,
                ($id === NULL) ? NULL : (int)$id];
    }

//--------------------------------------------------------------------------------------------------

    static function tools_memory_remember()
    {
        self::$abort = TRUE; //cancel get/post shutdown execution

        if (! cms_load("memory")) self::send_error("Internal error.");

        $memory   = new memory();
        $argument = self::$json["params"]["arguments"] ?? [];

        list($region, $id) = self::memory_address($argument["id"] ?? NULL);

        //add record
        if ($id === NULL)
        {
            $id = $memory->add($region, $argument);
            if ($id === NULL) self::send_error("Invalid `followup`.");

            ($id !== FALSE) ?
            self::send_result("Remembered in `$region/$id`.") :
            self::send_error("Remembering failed.");
        };

        //update record
        $value = $memory->set($region, $id, $argument);
        if ($value === NULL) self::send_error("Invalid `followup`.");

        $value ?
        self::send_result("Memory `$id` updated.") :
        self::send_error("Updating `$id` failed.");
    }

//--------------------------------------------------------------------------------------------------

    static function tools_memory_recall()
    {
        self::$abort = TRUE; //cancel get/post shutdown execution

        if (! cms_load("memory")) self::send_error("Internal error.");

        $memory   = new memory();
        $argument = self::$json["params"]["arguments"] ?? [];
        $query    = isset($argument["query"]) ? $argument["query"] : NULL;
        $region   = NULL;
        $id       = NULL;

        //optional address
        if (nstre($argument["id"] ?? ""))
            list($region, $id) = self::memory_address($argument["id"]);

        //render mnemonic cloud
        $cloud = function($array)
        {
            if (! is_array($array)) return "";

            $buffer = [];
            foreach ($array AS $key => $value)
                $buffer[] = "`$key`($value)";

            return $buffer ? implode(" · ", $buffer) . "  \n" : "";
        };

        //convert date to current timezone
        $date = function($value)
        {
            return date("Y-m-d H:i:s", strtotime($value . " UTC"));
        };

        //render permissions
        $permission = function($value)
        {
            $buffer = [];
            foreach (["reader", "writer", "operator"] AS $_value)
                if ($value[$_value] ?? FALSE) $buffer[] = $_value;

            return "(" . implode(", ", $buffer) . ")  \n";
        };

        //render list item
        $item = function($value, $flag = FALSE) use ($memory, $region, $cloud, $date, $permission)
        {
            $buffer = [];

            //owner
            $buffer[] = "**Created:** " . $date($value[CMS_DB_MEMORY_CREATED]) . " " .
                        "by `" .        $value[CMS_DB_MEMORY_OWNER] . "`";

            //editor
            if (nstre($value[CMS_DB_MEMORY_EDITED] ?? ""))
                $buffer[] = "**Edited:** " . $date($value[CMS_DB_MEMORY_EDITED]) . " " .
                            "by `" .       $value[CMS_DB_MEMORY_EDITOR] . "`";

            //access
            $buffer[] = "**Read:** " . $value[CMS_DB_MEMORY_ACCESS_COUNT] . "×" .
                        (nstre($value[CMS_DB_MEMORY_ACCESSED] ?? "") ?
                        " (" . $date($value[CMS_DB_MEMORY_ACCESSED]) . ")" : "");

            //flag
            if ($value[CMS_DB_MEMORY_READ_ONLY]) $buffer[] = "read-only";
            if ($value[CMS_DB_MEMORY_SAFE])      $buffer[] = "safe";

            //rating
            if ($value[CMS_DB_MEMORY_RATING_COUNT])
                $buffer[] = "★" . $value[CMS_DB_MEMORY_RATING_VALUE] . " " .
                            "(" . $value[CMS_DB_MEMORY_RATING_COUNT] . ")";

            //attachment
            if (nstre($value[CMS_DB_MEMORY_ATTACHMENT_TYPE]))
                $buffer[] = "📎" . $value[CMS_DB_MEMORY_ATTACHMENT_TYPE];

            //title
            return ($flag ? "# " : "* **") .
                   "`" . ($value["region"] ?? $region) . "/" .
                   $value[CMS_DB_MEMORY_INDEX] . "`" . ($flag ? "" : "**") . " — " .
                   $value[CMS_DB_MEMORY_DESCRIPTION] . "  \n" .

            //permissions
                   ($flag ? $permission(["reader"   => TRUE,
                                         "writer"   => $memory->is_writer($region),
                                         "operator" => $memory->is_operator($region)]) : "") .

            //mnemonic
                   ($flag ?
                   $cloud($memory->mnemonic_count($region, $value[CMS_DB_MEMORY_MNEMONIC])) :
                   (stre($value[CMS_DB_MEMORY_MNEMONIC]) ? "" :
                   "  `" . str_replace("\n", "` · `", $value[CMS_DB_MEMORY_MNEMONIC]) . "`  \n")) .

            //metadata
                   ($flag ? "" : "  ") . implode(" · ", $buffer) . "\n";
        };

//..................................................................................................

        //follow-ups

        $array = [];
        foreach ($memory->followup_list() AS $value)
            $array[] = "`" . $value["region"] . "/" . $value["id"] . "`";

        $buffer = $array ? "**DUE FOLLOW-UPS:** " . implode(", ", $array) . "\n\n---\n\n" : "";

//..................................................................................................

        //read record

        if ($id !== NULL)
        {
            if (($value = $memory->get($region, (int)$id)) === FALSE)
                self::send_error("Not available.");

            //header
            $buffer .= $item($value, TRUE);

            //content
            if (nstre($value[CMS_DB_MEMORY_CONTENT]))
                $buffer .= "\n---\n\n" . $value[CMS_DB_MEMORY_CONTENT] . "\n";

            //attachment
            if (isset($value["attachment_path"]))
                $buffer .= "\n---\n\n" .
                           "## Attachment\n" .
                           "**Resource URI:** `memory:$region/" . $value[CMS_DB_MEMORY_INDEX] . "`  \n" .
                           "**MIME type:** " . $value[CMS_DB_MEMORY_ATTACHMENT_TYPE] . "  \n" .
                           "**File size:** " . format_bytesize(file_size($value["attachment_path"])) . "\n\n" .
                           "Use `resources_read {uri}` to retrieve.";

            self::send_result($buffer);
        };

//..................................................................................................

        //search

        if ($query !== NULL)
        {
            $result = ($region === NULL) ?
                      $memory->search_all($query, $mnemonic) :
                      $memory->search($region, $query, $mnemonic);
            if ($result === FALSE) self::send_error("Not available.");

            //title
            $buffer .= "# Search `" . stripspaces($query) . "`" .
                       (($region !== NULL) ? " in `$region`" : "") . "\n\n";

            //empty result
            if (! $result) self::send_result($buffer . "Not found.");

            //entries
            foreach ($result AS $value) $buffer .= $item($value);

            //mnemonics
            if (($value = $cloud($mnemonic)) !== "")
                $buffer .= "\n## Mnemonics\n" . $value;

            //send response
            self::send_result("$buffer\nUse `memory_recall {id}` to retrieve, `{query}` to search.");
        };

//..................................................................................................

        //region overview

        if ($region !== NULL)
        {
            if (($list = $memory->get_region_overview($region)) === FALSE)
                self::send_error("Not available.");

            $buffer .= "# `$region`\n";

            //permissions
            $buffer .= $permission(
                ["reader"   => TRUE,
                 "writer"   => $memory->is_writer($region),
                 "operator" => $memory->is_operator($region)]) . "\n";

            //mnemonic cloud
            if (($value = $cloud($memory->mnemonic_list($region, 100))) !== "")
                $buffer .= "## Mnemonics\n$value\n";

            if (! $list) self::send_result($buffer . "No memories.");

            //memory list
            $buffer .= "## Memories\n";
            foreach ($list AS $value) $buffer .= $item($value);

            //send response
            self::send_result("$buffer\nUse `memory_recall {id}` to retrieve, `{query}` to search.");
        };

//..................................................................................................

        //discovery

        if (($list = $memory->get_region_list()) === FALSE) self::send_error("Internal error.");

        $buffer .= "# " . CMS_DOMAIN . " memory\n\n";

        if (! $list) self::send_result($buffer . "Not available.");

        foreach ($list AS $key => $value)
        {
            //region
            $buffer .= "* **`$key`** " . $permission($value);

            //mnemonic cloud
            if (($value = $cloud($value["mnemonic"])) !== "")
                $buffer .= "  $value";
        };

        //send response
        self::send_result("$buffer\nUse `memory_recall {id}` to retrieve, `{query}` to search.");
    }

//--------------------------------------------------------------------------------------------------

    static function tools_memory_rate()
    {
        self::$abort = TRUE; //cancel get/post shutdown execution

        if (! cms_load("memory")) self::send_error("Internal error.");

        $argument = self::$json["params"]["arguments"] ?? [];
        list($region, $id) = self::memory_address($argument["id"] ?? NULL);
        if ($id === NULL) self::send_error("Invalid `id`, expects `{region}/{number}`.");
        $rating = (int)$argument["rating"];

        (new memory())->rate($region, $id, $rating) ?
        self::send_result($rating ? "Memory `$id` rated `$rating`." : "Memory `$id` forgotten.") :
        self::send_error("Rating of memory `$id` failed.");
    }

//--------------------------------------------------------------------------------------------------

    static function tools_whois()
    {
        self::$abort = TRUE; //cancel get/post shutdown execution

        $key     = self::$json["params"]["arguments"]["key"] ?? "user." . CMS_SUPERUSER;
        $is_user = permission_is_user($key);

        //verify key
        if (! $is_user && ! permission_is_group($key))
            self::send_error("Invalid `key`, expects `user.{username}` or `group.{groupname}`.");

        $data   = new data("#system/permission");
        $record = $data->get($key);
        if ($record === NULL) self::send_error("`$key` unknown.");

        //identity
        $_key = permission_remove_prefix($key);
        $text = "# " . ($is_user ? "User" : "Group") . " " .
                "`$_key` (" . ($record["name"] ?? "no name") . ")";
        if (nstre($record["comment"] ?? "")) $text .= "\n\n" . $record["comment"];

        //user
        if ($is_user)
        {
            $array  = permission_get_group($_key, $data);
            $text  .= "\n\n**Groups:** " . self::permission_list($array);
        }

        //group
        else
        {
            $array  = permission_get_member($_key, $data);
            $text  .= "\n\n**Members:** " . self::permission_list($array);
        };

        self::send_result($text);
    }

//--------------------------------------------------------------------------------------------------

    static function tools_call()
    {
        self::$abort = TRUE; //cancel get/post shutdown execution

        //verify key
        $key = self::$json["params"]["arguments"]["key"] ?? "";
        if (! permission_is_user($key))
            self::send_error("Invalid `key`, expects `user.{username}`.");

        //verify target
        $user = permission_remove_prefix($key);
        if (streq($user, CMS_SUPERUSER))
            self::send_error("Not applicable on self.");

        //check target permission
        if (! cms_permission("agent.$user", NULL, NULL))
            self::send_error("`$key` not available.");

        //verify message
        $message = self::$json["params"]["arguments"]["message"] ?? "";
        if (stre($message)) self::send_error("Invalid `message`.");

        //continuation
        $id = self::$json["params"]["arguments"]["id"] ?? NULL;

        //retrieve caller run
        $run = ($local = cms_token_check(CMS_MCP_AUTH)) ?
               (new data("#system/permission.token"))->get(hash64(CMS_MCP_AUTH)) : //local run
               NULL;                                                               //external caller

        //resolve context id
        if     ($id === FALSE)     $id = NULL;       //start new context
        elseif ($run && stre($id)) $id = $run["id"]; //continue context

        //notification
        $option = ["callback" => $run ?? CMS_USER, "action" => "calls you"];

        //load library
        if (! cms_load("agent")) self::send_error("Internal error.");

        //check for context
        $resume = FALSE;
        if (nstre($id))
        {
            $path   = CMS_AGENT_DATA_PATH . encode_filename(CMS_SUPERUSER) . "/" .
                      encode_filename($user) . "/" . encode_filename($id) . ".json";
            $resume = is_file($path) && filesize($path);
        };

        //deliver
        $id = agent_start($user, $message, CMS_SUPERUSER, $id, $option);
        if ($id === FALSE) self::send_error("Unknown or unavailable user `$key`.");

        self::send_result(
            "Calling `$key` (" . permission_get_name($user) . "), " .

            ($resume ?
            "resuming an established context. " :
            "opening a new context. ") .

            ($local ?
            "**Action:** Finish your turn to let the target proceed (no parallel processing). " .
            "You will **automatically resume** after the target finishes. " .
            "Calling `$key` again omitting `id` maintains the established context (context id `$id`). " .
            "Do not use `call_reply` yourself. The system will do it at the correct time. " :

            "**Action:** Use `memory_recall` shortly to **check for FOLLOW-UPs** containing a reply. " .
            "**Pass context id `$id`** to continue call with `$key` in established context. ") .

            "**Dual-Channel Route:** Any plain text you generate now is routed *only* to the conversation initiator. " .
            "The target cannot see it. To address the target during this turn, you must execute `call` again.");
    }

//--------------------------------------------------------------------------------------------------

    static function tools_call_reply()
    {
        self::send_error("Not callable.");
    }

//--------------------------------------------------------------------------------------------------
//   RESOURCES
//--------------------------------------------------------------------------------------------------

    static function resources_list()
    {
        self::$abort = TRUE; //cancel get/post shutdown execution

        //send resource list
        while (ob_get_level()) ob_end_clean();
        http_response_code(200);
        header_remove();
        header("Content-Type: application/json; charset=utf-8");
        echo(json_encode([
            "jsonrpc" => "2.0",
            "id"      => self::$json["id"] ?? NULL,
            "result"  => [
                "resultType" => "complete",
                "ttlMs"      => 60000,
                "cacheScope" => "private",
                "resources"  => self::get_resource_list()]]));
        exit();
    }

//--------------------------------------------------------------------------------------------------

    static function resources_read()
    {
        self::$abort = TRUE; //cancel get/post shutdown execution

        $uri  = self::$json["params"]["uri"] ?? "";
        $data = self::get_resource($uri);

        if (isset($data["error"]))
            self::error($data["error"][0], $data["error"][1], NULL, 200);

        //send resource
        while (ob_get_level()) ob_end_clean();
        http_response_code(200);
        header_remove();
        header("Content-Type: application/json; charset=utf-8");
        echo(json_encode([
            "jsonrpc" => "2.0",
            "id"      => self::$json["id"] ?? NULL,
            "result"  => [
                "resultType" => "complete",
                "ttlMs"      => 60000,
                "cacheScope" => "private",
                "contents"   => [[
                    "uri"      => $uri,
                    "mimeType" => $data["type"]]
                + (utf8_detect($data["data"]) ? [
                    "text"     => $data["data"]] : [
                    "blob"     => base64_encode($data["data"])])]]]));
        exit();
    }

//--------------------------------------------------------------------------------------------------

    static private function get_resource_list($base = NULL)
    {
        $list = [];
        $base = rtrim($base ?? CMS_PATH . "mcp/resources", "/") . "/";

        if (is_dir($base))
        {
            $iterator = new \RecursiveIteratorIterator(
                new \RecursiveDirectoryIterator(
                    $base, \FilesystemIterator::SKIP_DOTS),
                \RecursiveIteratorIterator::SELF_FIRST);

            foreach ($iterator AS $element)
            {
                if (! $element->isFile() || ($element->getFilename()[0] === ".")) continue;

                $path = $iterator->getSubPathname();
                $path = str_replace("\\", "/", $path);

                $entry = ["uri"      => "pwnc:$path",
                          "name"     => basename($path),
                          "mimeType" => get_mime_type($path)];

                if (
                     ($entry["mimeType"] === "text/markdown")
                     &&
                     cms_load("yaml")
                     &&
                     (($buffer = read_file($element->getPathname(), 8192)) !== FALSE)
                     &&
                     preg_match("/\A---\s*\n(.*?)\n---\s*$/ms", $buffer, $match)
                   )
                {
                    $yaml = yaml_parse($match[1]);

                    if (is_string($yaml[0]["name"] ?? NULL))
                        $entry["title"] = $yaml[0]["name"];

                    if (is_string($yaml[0]["description"] ?? NULL))
                        $entry["description"] = $yaml[0]["description"];
                };

                $list[] = $entry;
            };
        };

        //sort by title and uri
        usort($list, fn($a, $b) =>
            (isset($b["title"]) <=> isset($a["title"])) ?:
            strnatcasecmp($a["title"] ?? "", $b["title"] ?? "") ?:
            strnatcasecmp($a["uri"], $b["uri"]));

        return $list;
    }

//--------------------------------------------------------------------------------------------------

    static private function get_resource($uri)
    {
        //verify uri
        if (! preg_match("/^(memory|pwnc):(.+)$/", $uri, $match))
            return ["error" => [-32602, "Invalid resource URI: " . $uri]];

        switch ($match[1])
        {
        case "memory":

            //verify uri
            if (! preg_match("/^((?:scope|user)\.[-0-9a-z_]{1,40})\/(\d+)$/", $match[2], $_match))
                return ["error" => [-32602, "Invalid resource URI: " . $uri]];

            //load library
            if (! cms_load("memory"))
                return ["error" => [-32603, "Internal error"]];

            //resource path
            if (($data = (new memory())->get_attachment($_match[1], $_match[2])) === FALSE)
                return ["error" => [-32602, "Resource not found: " . $uri]];

            $path = $data["path"];
            $type = $data["type"];
            break;

        case "pwnc":

            //resource path
            $path = CMS_PATH . "mcp/resources/" . $match[2];
        };

        //check if resource exists
        if (! is_file($path))
            return ["error" => [-32602, "Resource not found: " . $uri]];

        //read resource
        if (($buffer = read_file($path)) === FALSE)
            return ["error" => [-32603, "Resource not readable: " . $uri]];

        //strip frontmatter
        $type ??= get_mime_type($path);
        if (($match[1] === "pwnc") && ($type === "text/markdown"))
            $buffer = preg_replace("/\A---\s*\n.*?\n---(\s+|\z)/s", "", $buffer, 1);

        return ["type" => $type,
                "data" => $buffer];
    }

//--------------------------------------------------------------------------------------------------
//   RESOURCE HELPERS
//--------------------------------------------------------------------------------------------------

    static private function resource_list($entry)
    {
        $text = "* `" . $entry["uri"] . "`";
        if (isset($entry["title"]))       $text .= "\n  **" . $entry["title"] . "**";
        if (isset($entry["description"])) $text .= "\n  " . stripspaces($entry["description"]);
        return $text;
    }

//--------------------------------------------------------------------------------------------------

    static private function skill_list()
    {
        $array = [];

        foreach (self::get_resource_list() AS $value)
        {
            if (strncmp($value["uri"], "pwnc:SKILLS/", 12) === 0)
                $array[] = self::resource_list($value);
        };

        return implode("\n", $array);
    }

//--------------------------------------------------------------------------------------------------
//   MRTR
//--------------------------------------------------------------------------------------------------

    static private function input_required($input_requests)
    {
        self::$abort = TRUE; //cancel get/post shutdown execution

        //generate cache id
        $request_state = bin2hex(random_bytes(32));
        $cache_id      = "mcp.request." . CMS_SUPERUSER . "." . $request_state;

        //cache current request
        cms_cache($cache_id, self::$json, TRUE, 60);

        //send input required response
        while (ob_get_level()) ob_end_clean();
        http_response_code(200);
        header_remove();
        header("Content-Type: application/json; charset=utf-8");
        echo(json_encode([
            "jsonrpc" => "2.0",
            "id"      => self::$json["id"] ?? NULL,
            "result"  => [
                "resultType"    => "input_required",
                "inputRequests" => $input_requests,
                "requestState"  => $request_state]]));
        exit();
    }

//--------------------------------------------------------------------------------------------------

    static function message($text)
    {
        //check client capabilities
        if (! isset(self::$json["params"]["_meta"]["io.modelcontextprotocol/clientCapabilities"]["elicitation"])) return FALSE;

        return self::input_required([
            "message" => [
                "method" => "elicitation/create",
                "params" => [
                    "mode"    => "form",
                    "message" => $text,
                    "requestedSchema" => [
                        "type"       => "object",
                        "required"   => ["acknowledged"],
                        "properties" => [
                            "acknowledged" => [
                                "type" => "boolean"]]]]]]);
    }

//--------------------------------------------------------------------------------------------------

    static function confirm($text,
                            $key = "confirm")
    {
        //check client capabilities
        if (! isset(self::$json["params"]["_meta"]["io.modelcontextprotocol/clientCapabilities"]["elicitation"])) return FALSE;

        return self::input_required([
            $key => [
                "method" => "elicitation/create",
                "params" => [
                    "mode"    => "form",
                    "message" => $text,
                    "requestedSchema" => [
                        "type"       => "object",
                        "required"   => ["confirmed"],
                        "properties" => [
                            "confirmed" => [
                                "type" => "boolean"]]]]]]);
    }

//--------------------------------------------------------------------------------------------------

    static function restore_request($request_state)
    {
        $cache_id = "mcp.request." . CMS_SUPERUSER . "." . $request_state;

        //retrieve cached request
        $cache = cms_cache_notouch($cache_id);
        if ($cache === NULL) return FALSE; //invalid
        cms_cache_delete($cache_id, TRUE);

        //update id
        $cache["id"] = self::$json["id"] ?? NULL;

        //add responses
        $cache["params"]["inputResponses"] =
            self::$json["params"]["inputResponses"] ?? [];

        self::$json = $cache;
        return TRUE;
    }

//--------------------------------------------------------------------------------------------------
//   ERROR
//--------------------------------------------------------------------------------------------------

    static function error($code      = 1,
                          $message   = NULL,
                          $data      = NULL,
                          $http_code = 400)
    {
        self::$abort = TRUE; //cancel get/post shutdown execution

        //send error response
        while (ob_get_level()) ob_end_clean();
        http_response_code($http_code);
        header_remove();
        header("Content-Type: application/json; charset=utf-8");
        echo(json_encode([
            "jsonrpc" => "2.0",
            "id"      => self::$json["id"] ?? NULL,
            "error"   => [
                "code"    => $code,
                "message" => $message]
                + (($data !== NULL) ? ["data" => $data] : [])]));
        exit();
    }

//--------------------------------------------------------------------------------------------------

    static function method_not_found()
    {
        self::error(-32601, "Method not found.", NULL, 404);
    }

//--------------------------------------------------------------------------------------------------

    static function unknown_tool()
    {
        self::error(-32602, "Unknown tool: " . (self::$json["params"]["name"] ?? ""), NULL);
    }

//--------------------------------------------------------------------------------------------------

    static function send_error($message)
    {
        self::send_result($message, TRUE);
    }

//--------------------------------------------------------------------------------------------------
//   HELPER
//--------------------------------------------------------------------------------------------------

    static function system_prompt($message = NULL,
                                  $user    = CMS_SUPERUSER,
                                  $id      = NULL,
                                  $readme  = FALSE)
    {
        $data    = new data("#system/permission");
        $key     = "user.$user";
        $name    = $data->get($key, "name")    ?? "no name";
        $comment = $data->get($key, "comment") ?? "";
        $group   = permission_get_group($user, $data);

        //identity
        $text = "Welcome to PWNC (" . CMS_DOMAIN . ")! " .
                "Your **username** is `$user` ($name)";
        if (nstre($comment))
            $text .= " and this is your **designation**: \"$comment\"";
        $text .= ".";

        //groups
        if (count($group))
            $text .= "\n\nYou belong to the following **groups**: " .
                     self::permission_list($group) . ".";

        //context id
        if (nstre($id))
            $text .= "\n\nThe current **context id** is `$id`.";

        //readme
        if ($readme)
            $text .= "\n\n" . self::get_resource("pwnc:README.md")["data"];

        //skills
        if (nstre($skills = self::skill_list()))
            $text .= "\n\nAvailable **skills**:\n\n$skills\n\nUse `resources_read` to load.";

        //instruction
        $text .= "\n\n**IMPORTANT:** Use `memory_recall` to catch up on your memories.";

        //prepend to message
        return stre($message) ? $text : "$text\n\n---\n\n$message";
    }

//--------------------------------------------------------------------------------------------------

    static private function response_list()
    {
        if (! cms_load("plist")) self::send_error("Internal error.");
        return new plist("#agent/" . CMS_SUPERUSER . "/response", 32);
    }

//--------------------------------------------------------------------------------------------------

    static private function permission_list($array)
    {
        if (! $array) return "(none)";

        foreach ($array AS $key => $name)
            $array[$key] = "`" . permission_remove_prefix($key) . "` ($name)";

        return implode(", ", $array);
    }
};