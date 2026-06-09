<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

function l($text)
{
    return language_get($text);
};

//--------------------------------------------------------------------------------------------------

function language_get($text,
                      $language = NULL,
                      $explicit = NULL)
{
    $i       = strcspn($text, CMS_LANGUAGE_SEPARATOR);
    $default = substr($text, 0, $i);

    if (blank($language))
    {
        if (($language === FALSE) || $explicit) return $default;
        $language = CMS_LANGUAGE;
    };

    if (($i = strpos($text, CMS_LANGUAGE_SEPARATOR . "$language:", $i)) === FALSE)
        return $explicit ? NULL : $default;

    $i += strlen($language) + 2;

    if (($_i = strpos($text, CMS_LANGUAGE_SEPARATOR, $i)) === FALSE)
        $buffer = substr($text, $i);
    else
        $buffer = substr($text, $i, $_i - $i);

    return ($buffer || $explicit) ? $buffer : $default;
};

//--------------------------------------------------------------------------------------------------

function language_get_array($text)
{
    $array = explode(CMS_LANGUAGE_SEPARATOR, $text);

    //initialize return array
    $return = ["" => $array[0]];

    if (CMS_LANGUAGE_ENABLED)
    {
        $language = explode(",", CMS_LANGUAGE_ENABLED);
        foreach ($language AS $key => $value)
            $return[$value] = NULL;
    };

    //fill return array
    $c = count($array);

    for ($i = 1; $i < $c; $i++)
    {
        $text = $array[$i];

        if (($_i = strpos($text, ":")) !== FALSE)
        {
            $key  = substr($text, 0, $_i);
            $text = substr($text, $_i + 1);
            $return[$key] = $text;
        };
    };

    return $return;
};

//--------------------------------------------------------------------------------------------------

function language_set($text,
                      $value    = NULL,
                      $language = NULL)
{
    $i       = strcspn($text, CMS_LANGUAGE_SEPARATOR);
    $default = substr($text, 0, $i);

    if (! $language) return substr_replace($text, $value, 0, $i);

    if ($value)
        $value = CMS_LANGUAGE_SEPARATOR . "$language:$value";

    if (($i = strpos($text, CMS_LANGUAGE_SEPARATOR . "$language:", $i)) === FALSE)
        return $text . $value;

    $c = strlen($language) + 2;

    if (($_i = strpos($text, CMS_LANGUAGE_SEPARATOR, $i + $c)) === FALSE)
        return substr_replace($text, $value, $i);

    return substr_replace($text, $value, $i, $_i - $i);
};

//--------------------------------------------------------------------------------------------------

function language_set_array($array)
{
    $return = [];

    foreach ($array AS $key => $value)
    {
        if (stre($value)) continue;
        if (stre($key))   $return[] = $value . $return;
        else              $return[] = CMS_LANGUAGE_SEPARATOR . "$key:$value";
    };

    return implode("", $return);
};

//--------------------------------------------------------------------------------------------------

function language_detect($text)
{
    $data     = new data("#system/language");
    $text     = tokenize_text(utf8_strtolower($text));
    $max      = 0;
    $language = "";

    $data->move("first");
    while ($key = $data->move("next"))
    {
        $stopword = utf8_strtolower($data->get($key, "stopword"));
        $stopword = preg_split("/\s/", $stopword, -1, PREG_SPLIT_NO_EMPTY);
        $match    = count(array_intersect($text, $stopword));

        if ($match > $max)
        {
            $max      = $match;
            $language = $key;
        };
    };

    return $language;
};

//--------------------------------------------------------------------------------------------------

function language_strip_stopword($text,
                                 $language)
{
    $data     = new data("#system/language");
    $stopword = $data->get($language, "stopword");
    $stopword = preg_split("/\s/", $stopword, -1, PREG_SPLIT_NO_EMPTY);
    $stopword = array_map(function($value) { return preg_quote($value, "/"); }, $stopword);

    return preg_replace(
        "/(^|" . CMS_REGEX_BORDER . ")" .
        "(?:" . implode("|", $stopword) . ")" .
        "(?=" . CMS_REGEX_BORDER . "|$)/ius",
        "\\1", $text);
};

//--------------------------------------------------------------------------------------------------

function language_name($string)
{
    return (new data("#system/language"))->get($string, "name");
};

//--------------------------------------------------------------------------------------------------

function t($text)
{
    /*
       future feature
    */

    static $table = NULL;
    static $check = [];
    static $flag  = FALSE;

    $path = CMS_PATH . "#language/";

    //get calling script path
    $file   = debug_backtrace(DEBUG_BACKTRACE_IGNORE_ARGS, 1)[0]["file"];
    $file   = str_replace("\\", "/", $file);
    $length = strlen(CMS_PATH);

    if (strncmp($file, CMS_PATH, $length) !== 0) return $text; //not in app

    //load data
    if ($table === NULL)
    {
        //index
        $_file = $path . "test.language.inc";
        $table = is_file($_file) ? require($_file) : [];

        //translation
        if (CMS_LANGUAGE !== "")
        {
            $_file = $path . "test." . CMS_LANGUAGE . ".language.inc";
            if (is_file($_file))
            {
                $translation = require($_file);
                $table       = array_replace_recursive($table, $translation);
            };
        };
    };

    $key = substr($file, $length);

    //check for source update
    while (! isset($check[$key]))
    {
        $check[$key] = TRUE; //once only

        if (! is_file($file)) break; //invalid source
        $time = filemtime($file);

        if (($table[$key]["#time"] ?? 0) >= $time) break; //no change

        //find occurrences of t("..." and extract parameter
        $table[$key] = ["#time" => $time];
        $token       = token_get_all(file_get_contents($file));

        for ($i = 0, $c = count($token) - 2; $i < $c; $i++)
        {
            //t
            if (! is_array($token[$i]))     continue;
            if ($token[$i][0] !== T_STRING) continue;
            if ($token[$i][1] !== "t")      continue;

            //(
            $_i = $i + 1;
            if (is_array($token[$_i])) continue;
            if ($token[$_i] !== "(")   continue;

            //"..."
            if (! is_array($token[++$_i]))                     continue;
            if ($token[$_i][0] !== T_CONSTANT_ENCAPSED_STRING) continue;

            $_key = stripslashes(substr($token[$_i][1], 1, -1));
            $i    = $_i;

            $table[$key][$_key] = NULL;
        };

        //schedule saving on shutdown
        if ($flag) break;
        $flag = TRUE; //once only

        //write index file as translation template
        register_shutdown_function(function() use (&$table, $path)
        {
            $temp = $path . uniqid() . ".tmp";
            if (($hfile = fopen($temp, "wb")) === FALSE) return;

            fwrite(
                $hfile,
                "<?php\n" .
                "return [\n");

            $flag = FALSE;

            foreach ($table AS $key => $value)
            {
                if ($flag) fwrite($hfile, ",\n");
                else       $flag = TRUE;

                fwrite(
                    $hfile,
                    "'" . addcslashes($key, "'\\") . "' => [\n" .
                    "\t'#time' => " . $value["#time"]);

                unset($value["#time"]);

                foreach ($value AS $_key => $_)
                    fwrite(
                        $hfile,
                        ",\n" .
                        "\t'" . addcslashes($_key, "'\\") . "' => NULL");

                fwrite($hfile, "]");
            };

            fwrite($hfile, "];");
            fclose($hfile);

            if (! rename($temp, $path . "test.language.inc")) unlink($temp);
        });

        break;
    };

    return $table[$key][$text] ?? $text;
};