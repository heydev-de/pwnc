<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   TRIGGER
//==================================================================================================

//constants
define("CMS_TRIGGER_DATA_PATH",       CMS_DATA_PATH . "#trigger/");
define("CMS_TRIGGER_DATA_FILE",       "#system/trigger");
define("CMS_TRIGGER_USER",            "trigger");
define("CMS_TRIGGER_OUTPUT_LIMIT",    1048576);
define("CMS_TRIGGER_CRON_LIMIT",      1000);
define("CMS_TRIGGER_REGEX_TIME",      "/^\d{4}-\d{2}-\d{2}\s+\d{2}:\d{2}$/");
define("CMS_TRIGGER_REGEX_INTERVAL",  "/^(?:(\d+)d)?(?:(\d+)h)?(?:(\d+)m)?$/");
define("CMS_TRIGGER_REGEX_CRON_PART", "/^(?:\*|(\d+)(?:-(\d+))?)(?:\/(\d+))?$/");

class trigger
{
    static function process()
    {
        $data        = new data(CMS_TRIGGER_DATA_FILE);
        $time        = gmdate(t("datetime_format")) . " " . CMS_TIMEZONE_DEFAULT;
        $request_uri = CMS_HOST . ($_SERVER["REQUEST_URI"] ?? "");
        $url         = normalize_url($request_uri) ?: "";
        $content     = FALSE;

        $data->move("first");
        while (($key = $data->move("next")) !== NULL)
        {
            $trigger = $data->get($key);
            if ($trigger["disabled"] ?? FALSE)  continue; //trigger disabled
            if (stre($trigger["target"] ?? "")) continue; //no target

            //trigger name
            $name = $trigger["name"] ?? $key;

            //check schedule
            if (self::due($key, $trigger["schedule"] ?? ""))
            {
                //set replacement values
                $replace = ["name"        => $name,
                            "application" => "",
                            "url"         => "",
                            "time"        => $time,
                            "user"        => ""];
            }

            //check conditions
            else
            {
                //retrieve conditions
                $_url       = l($trigger["_url"]       ?? "");
                $_param     = l($trigger["_param"]     ?? "");
                $_inclusion = l($trigger["_inclusion"] ?? "");
                $_exclusion = l($trigger["_exclusion"] ?? "");

                //at least one condition must be set
                if (stre($_url . $_param . $_inclusion . $_exclusion)) continue;

                //url matching
                if (nstre($_url) && (! self::match($_url, $url))) continue;

                //parameter matching
                if (nstre($_param) && (! self::match($_param, NULL, TRUE))) continue;

                //output matching
                if (nstre($_inclusion) || nstre($_exclusion))
                {
                    //retrieve output
                    if ($content === FALSE)
                    {
                        $content = NULL;

                        //merge buffer down to one remaining level
                        while (ob_get_level() > 1) echo(ob_get_clean());

                        //check output buffer length
                        if ((($length = ob_get_length()) !== FALSE) && ($length <= CMS_TRIGGER_OUTPUT_LIMIT))
                        {
                            $content = ob_get_contents();
                            if (! utf8_detect($content)) $content = ""; //binary
                        };
                    };

                    //no output
                    if ($content === NULL) continue;

                    //output matching
                    if (nstre($_inclusion) && (! self::match($_inclusion, $content))) continue;
                    if (nstre($_exclusion) && self::match($_exclusion, $content))     continue;
                };

                //set replacement values
                $replace = ["name"        => $name,
                            "application" => CMS_APPLICATION,
                            "url"         => $request_uri,
                            "time"        => $time,
                            "user"        => CMS_USER];
            };

            //fire trigger
            if (stre($message = l($trigger["message"] ?? ""))) $message = t("trigger_message_default");
            $message = replace_placeholder($message, $replace, NULL);

            self::metric($key, TRUE);

            cms_daemon(
                "trigger::notify(" . var_export($key, TRUE) . ", " .
                                     var_export($message, TRUE) . ");",
                "trigger.$key." . unique_id(),
                0,
                "Trigger: $name");
        };
    }

//--------------------------------------------------------------------------------------------------

    static function notify($key,
                           $message)
    {
        $trigger = (new data(CMS_TRIGGER_DATA_FILE))->get($key);
        if (! $trigger) return;

        $permission = new data("#system/permission");
        $name       = $trigger["name"] ?? $key;

        foreach (self::line($trigger["target"] ?? "") AS $target)
        {
            $user = $permission->get("user.$target");
            if ($user["disabled"] ?? FALSE) continue;

            //email
            if (
                 nstre($email = permission_get_email($target))
                 &&
                 verify_email($email)
                 &&
                 cms_load("smtp")
               )
                smtp_send($email, $name, $message);

            //agent
            if (nstre($user["api"] ?? "") && cms_load("agent"))
                agent_start($target, $message, CMS_TRIGGER_USER);

            //ims
            if (cms_permission("desktop", NULL, NULL, $target))
                notify($target, $message, CMS_TRIGGER_USER);
        };
    }

//--------------------------------------------------------------------------------------------------

    static function due($key,
                        $schedule = NULL,
                        $refresh  = FALSE)
    {
        $path = CMS_TRIGGER_DATA_PATH . $key;
        if ($schedule === NULL) return (int)read_file($path); //retrieve

        //no schedule
        if (stre($schedule))
        {
            if (is_file($path)) unlink($path); //delete
            return FALSE;
        };

        //create path
        if (! mkpath(CMS_TRIGGER_DATA_PATH)) return FALSE;

        //open file and acquire lock
        if (($hfile = fopen($path, "c+")) === FALSE) return FALSE;
        flock($hfile, LOCK_EX);

        $time   = time();
        $raw    = stream_get_contents($hfile);
        $_time  = (int)$raw;
        $result = $_time && ($_time <= $time);

        //write next due date
        if ($refresh || $result || stre($raw))
        {
            rewind($hfile);
            ftruncate($hfile, 0);
            fwrite($hfile, (string)self::schedule_next($schedule, $time));
        };

        //release lock
        flock($hfile, LOCK_UN);
        fclose($hfile);

        return $result;
    }

//--------------------------------------------------------------------------------------------------

    static private function match($ruleset,
                                  $content = NULL,
                                  $param   = FALSE)
    {
        $value = $content;

        foreach (self::line($ruleset) AS $rule)
        {
            if ($param)
            {
                if (! ($pos = strpos($rule, "="))) continue; //no param name
                $name  = substr($rule, 0, $pos);
                $value = $GLOBALS[$name] ?? "";
                if (! is_scalar($value)) continue;
                $value = (string)$value;
                $rule  = substr($rule, $pos + 1);
            };

            if ($rule === "")                                   continue;
            if ($negate = ($rule[0] === "!"))                   $rule = substr($rule, 1);
            if (($match = preg_match($rule, $value)) === FALSE) continue;
            if ($match != $negate)                              return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    static function schedule_next($ruleset,
                                  $time = NULL)
    {
        $time ??= time();
        $next   = PHP_INT_MAX;

        foreach (self::line($ruleset) AS $rule)
        {
            //point in time
            if (preg_match(CMS_TRIGGER_REGEX_TIME, $rule))
            {
                sscanf($rule, "%d-%d-%d %d:%d", $year, $month, $day, $hour, $minute);
                $_time = (checkdate($month, $day, $year) && ($hour < 24) && ($minute < 60)) ?
                         strtotime("$rule " . CMS_TIMEZONE_DEFAULT) : FALSE;
                $_time = ($_time > $time) ? $_time : FALSE;
            }

            //interval
            elseif (preg_match(CMS_TRIGGER_REGEX_INTERVAL, $rule, $array))
            {
                $_time = ((int)($array[1] ?? 0) * 86400) +
                         ((int)($array[2] ?? 0) *  3600) +
                         ((int)($array[3] ?? 0) *    60);
                $_time = $_time ? $time + $_time : self::cron_next($rule, $time);
            }

            //cron expression
            else
            {
                $_time = self::cron_next($rule, $time);
            };

            if ($_time === FALSE) continue;
            if ($_time < $next)   $next = $_time;
        };

        return ($next === PHP_INT_MAX) ? 0 : $next;
    }

//--------------------------------------------------------------------------------------------------

    static private function cron_next($expression,
                                      $time)
    {
        static $range = [[0, 59], [0, 23], [1, 31], [1, 12], [0, 7]];

        $part = preg_split("/\s+/", trim($expression), -1, PREG_SPLIT_NO_EMPTY);
        if (count($part) !== 5) return FALSE; //invalid

        //translate parts to value sets
        $set = [];
        foreach ($part AS $key => $value)
        {
            //get range limits
            list($min, $max) = $range[$key];

            //process value list of part
            $array = [];
            foreach (explode(",", $value) AS $_value)
            {
                if (! preg_match(CMS_TRIGGER_REGEX_CRON_PART, $_value, $match)) return FALSE; //invalid

                //wildcard
                if (($match[1] ?? "") === "")
                {
                    $first = $min;
                    $last  = $max;
                }

                //numeric
                else
                {
                    $first = (int)$match[1];
                    if ($first < $min) return FALSE; //out of range

                    $last = (($match[2] ?? "") !== "") ? (int)$match[2] : $first;
                    if (($last > $max) || ($first > $last)) return FALSE; //out of range
                };

                //step
                $step = (($match[3] ?? "") !== "") ? (int)$match[3] : 1;
                if ($step < 1) return FALSE; //invalid

                //build valid range value list
                for ($i = $first; $i <= $last; $i += $step) $array[$i] = TRUE;
            };

            $set[$key] = $array;
        };

        //normalize sunday
        if (isset($set[4][7])) $set[4][0] = TRUE;

        //wildcard weekday or day of month
        $wildcard = ($part[2][0] === "*") || ($part[4][0] === "*");

        //sorted list of valid values
        $hour_list   = array_keys($set[1]);
        $minute_list = array_keys($set[0]);

        sort($hour_list);
        sort($minute_list);

        //start at next minute
        $zone = new \DateTimeZone(CMS_TIMEZONE_DEFAULT);
        $date = (new \DateTime("@" . ((int)floor(((int)$time + 60) / 60) * 60)))->setTimezone($zone);

        //get dst unambiguous date
        $clock = function($date, $hour, $minute) use ($zone, &$floor)
        {
            //retrieve timestamp from iso date
            $time = (new \DateTime($date->format("Y-m-d") . sprintf(" %02d:%02d:00", $hour, $minute), $zone))->getTimestamp();

            //convert to date string
            $wall = (new \DateTime("@$time"))->setTimezone($zone)->format("Y-m-d H:i");

            //check for same timestamp with earlier time (-1h, -45m, -30m)
            foreach ([3600, 2700, 1800] AS $shift)
            {
                $prev = $time - $shift;

                if (

                //not before floor time
                     ($prev > $floor)
                     &&

                //check for equality
                     ((new \DateTime("@$prev"))->setTimezone($zone)->format("Y-m-d H:i") === $wall)
                   )
                {
                    $time = $prev; //set to previous pass time
                    break;
                };
            };

            //rebuild from timestamp
            return (new \DateTime("@$time"))->setTimezone($zone);
        };

        //advance by whole units until full match
        for ($i = 0; $i < CMS_TRIGGER_CRON_LIMIT; $i++)
        {
            $floor = $date->getTimestamp(); //current time position

            //month
            if (! isset($set[3][(int)$date->format("n")])) //month not matching
            {
                //proceed to next month
                $date->modify("first day of next month");
                $date = $clock($date, 0, 0);
                continue;
            };

            //weekday or day of month
            $weekday = isset($set[4][(int)$date->format("w")]);
            $day     = isset($set[2][(int)$date->format("j")]);

            if (! ($wildcard ? ($weekday && $day) : ($weekday || $day))) //day not matching
            {
                //proceed to next day
                $date->modify("+1 day");
                $date = $clock($date, 0, 0);
                continue;
            };

            //hour
            $hour = (int)$date->format("G");

            if (! isset($set[1][$hour]))
            {
                $next = array_filter($hour_list, function($value) use ($hour) { return $value > $hour; });

                if ($next === []) //no hour left in day
                {
                    //proceed to next day
                    $date->modify("+1 day");
                    $date = $clock($date, $hour_list[0], 0);
                    continue;
                };

                $date = $clock($date, reset($next), 0);
                continue;
            };

            //minute
            $minute = (int)$date->format("i");

            if (! isset($set[0][$minute]))
            {
                $next = array_filter($minute_list, function($value) use ($minute) { return $value > $minute; });
                $date = (new \DateTime("@" . ($date->getTimestamp() + 60 * (($next === []) ? 60 - $minute : reset($next) - $minute))))->setTimezone($zone);
                continue;
            };

            //current time position
            $time = $date->getTimestamp();

            //prevent double triggering of hour values
            if ($part[1][0] !== "*")
            {
                //current date string
                $wall = $date->format("Y-m-d H:i");

                //check for same timestamp with earlier time (-1h, -45m, -30m)
                foreach ([3600, 2700, 1800] as $shift)
                {
                    $prev = $time - $shift;
                    if ((new \DateTime("@$prev"))->setTimezone($zone)->format("Y-m-d H:i") !== $wall) continue; //no previous turn

                    //skip turn
                    $time += 60;
                    $date  = (new \DateTime("@$time"))->setTimezone($zone);
                    continue 2;
                };
            };

            //return smallest match
            return $time;
        };

        return FALSE; //no match
    }

//--------------------------------------------------------------------------------------------------

    static function compile($value,
                            $param = FALSE)
    {
        foreach (language_get_array($value) AS $language => $ruleset)
        {
            $result = [];
            foreach (self::line($ruleset ?? "") AS $rule)
            {
                $negate = ($rule[0] === "!");
                $rule   = $negate ? substr($rule, 1) : $rule;
                if ($rule === "") continue;

                $name = "";
                if ($param)
                {
                    if (! ($pos = strpos($rule, "="))) continue; //no param name
                    $name = substr($rule, 0, ++$pos);
                    $rule = substr($rule, $pos);
                };

                $regex  = "";
                $buffer = "";

                for ($i = 0, $length = strlen($rule); $i < $length; $i++)
                {
                    $char = $rule[$i];

                    //unescape
                    if (($char === "\\") && ($i + 1 < $length))
                    {
                        $buffer .= $rule[++$i];
                        continue;
                    };

                    //wildcard
                    if ($char === "*")
                    {
                        $regex  .= preg_quote($buffer, "/") . ".*?";
                        $buffer  = "";
                        continue;
                    };

                    //whitespace
                    if (ctype_space($char))
                    {
                        //collapse whitespace
                        while ((($i + 1) < $length) && ctype_space($rule[$i + 1])) $i++;

                        $regex  .= preg_quote($buffer, "/") . "\\s+";
                        $buffer  = "";
                        continue;
                    };

                    $buffer .= $char;
                };

                $result[] = $name . ($negate ? "!" : "") . "/" . $regex . preg_quote($buffer, "/") . "/s";
            };

            $array[$language] = implode("\n", $result);
        };

        return language_set_array($array);
    }

//--------------------------------------------------------------------------------------------------

    static function line($ruleset)
    {
        return preg_split("/\s*\n\s*/", trim($ruleset), -1, PREG_SPLIT_NO_EMPTY);
    }

//--------------------------------------------------------------------------------------------------

    static function metric($key,
                           $increase = FALSE)
    {
        $time     = time();
        $cache_id = "trigger.metric.$key";
        $data     = cms_cache($cache_id) ?? ["value" => NULL, "count" => 0, "time" => $time];
        $save     = FALSE;

        //new day
        if ((int)($data["time"] / 86400) !== (int)($time / 86400))
        {
            $data["value"] = ($data["value"] === NULL) ? $data["count"] : ($data["value"] + $data["count"]) / 2;
            $data["count"] = 0;
            $data["time"]  = $time;
            $save          = TRUE;
        };

        //count up
        if ($increase)
        {
            $data["count"]++;
            $save = TRUE;
        };

        if ($save) cms_cache($cache_id, $data, TRUE);

        return $data["value"] ?? $data["count"];
    }

//--------------------------------------------------------------------------------------------------

    static function cleanup()
    {
        if (! is_dir(CMS_TRIGGER_DATA_PATH)) return;

        $hdir = opendir(CMS_TRIGGER_DATA_PATH);
        if ($hdir === FALSE) return;

        $data = new data(CMS_TRIGGER_DATA_FILE);
        while (($file_object = readdir($hdir)) !== FALSE)
        {
            if ($file_object[0] === ".")    continue;
            if (! $data->has($file_object)) unlink(CMS_TRIGGER_DATA_PATH . $file_object);
        };

        closedir($hdir);
    }
};