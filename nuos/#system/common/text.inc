<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

function parse_text($text,
                    $token    = TRUE,
                    $base_url = CMS_HOST)
{
    if ((! $base_url) || (! $base_url = analyze_url($base_url))) $base_url = analyze_url(CMS_HOST);
    $base_url = "/^" . preg_quote($base_url["scheme"], "/") . ":\/\/" .
                "([^\.]+\.)?" . preg_quote($base_url["host"], "/") . "/i";

    static $format =
        ["+"     => ["<b>",
                     "</b>"],
         "/"     => ["<i>",
                     "</i>"],
         "_"     => ["<u>",
                     "</u>"],
         "<"     => ["<span style=\"FONT-SIZE:larger\">",
                     "</span>"],
         ">"     => ["<small>",
                     "</small>"],
         "<-"    => ["<span style=\"DISPLAY:block;TEXT-ALIGN:left;TEXT-ALIGN-LAST:left\">",
                     "</span>"],
         "<->"   => ["<span style=\"DISPLAY:block;TEXT-ALIGN:center;TEXT-ALIGN-LAST:center\">",
                     "</span>"],
         "->"    => ["<span style=\"DISPLAY:block;TEXT-ALIGN:right;TEXT-ALIGN-LAST:right\">",
                     "</span>"],
         "*"     => ["<h1 style=\"DISPLAY:inline\">",
                     "</h1>"],
         "\""    => ["<q>",
                     "</q>"],
         "-"     => ["<pre style=\"DISPLAY:inline\">",
                     "</pre>"],
         "IMG"   => ["",
                     ""],
         "<-IMG" => ["",
                     ""],
         "IMG->" => ["",
                     ""],
         "#"     => ["<table class=\"layout\">" .
                     "<tr>" .
                     "<td style=\"PADDING:0 1em 1em 0\">",
                     "</td>" .
                     "</tr>" .
                     "</table>"]];

    static $html_chars =
        ["<"  => "&lt;",
         ">"  => "&gt;",
         "\"" => "&quot;",
         "&"  => "&amp;"];

    $image       = cms_load("image") ? new image() : NULL;
    $depth       = 0;              //output buffer level
    $buffer      = [$depth => ""]; //output buffer
    $mode        = 0;              //current parsing mode
    $stack       = [];             //format marker stack
    $marker      = "";             //format marker
    $table_depth = 0;              //table nesting depth
    $table_char  = "";             //table character
    $char        = "";             //current character
    $_char       = "";             //previous character
    $__char      = "";             //antepenultimate character
    $whitespace  = "";             //whitespace buffer
    $i           = 0;              //text position
    $c           = strlen($text);  //length of input text

    while (TRUE)
    {
        for (; $i < $c; $i++)
        {
            $char = $text[$i];

            switch ($mode)
            {
            case 1: //token
//..................................................................................................

                //append current character
                $buffer[$depth] .= $char;

                //unescaped token delimiter
                if (($char === "%") && ($_char === "%") && ($__char !== "\\"))
                    $mode = 0; //reset mode
                break;

            case 2: //formatting
//..................................................................................................

                if ($char === "\r") break;

                //unescaped space or linebreak
                if (
                     (($char === " ")                       && ($_char  !== "\\"))
                     ||
                     (($char === "\n") && ($_char !== "\r") && ($_char  !== "\\"))
                     ||
                     (($char === "\n") && ($_char === "\r") && ($__char !== "\\"))
                   )
                {
                    //marker
                    if (isset($format[$marker]))
                    {
                        //memorize format marker
                        array_push($stack, $marker);

                        //table
                        if ($marker === "#")
                        {
                            $table_char = ""; //reset table character
                            $table_depth++;   //increase table nesting depth
                        };

                        //append format initiation
                        $buffer[$depth] .= $format[$marker][0];
                    }

                    //link, marker represents url
                    else
                    {
                        //memorize format completion
                        array_push($stack, "</a>");

                        //evaluate url
                        $url = translate_url($marker);

                        //append format initiation
                        $buffer[$depth] .= "<a href=\"" . x($url) . "\"";

                        //retrieve description of directory entry
                        if (
                             (($_url = analyze_url($marker)) !== FALSE)
                             &&
                             streq($_url["scheme"], "directory")
                           )
                        {
                            $directory = new data("#system/directory");
                            $title     = l($directory->get($_url["host"], "description"));

                            //add title attribute
                            if (nstre($title))
                            {
                                $title = htmltoplain(remove_format($title, TRUE, TRUE));
                                $buffer[$depth] .= " title=\"" . x($title) . "\"";
                            };
                        };

                        //not internal or javascript
                        if (
                             (! preg_match($base_url, $url))
                             &&
                             (! preg_match("/^(?:javascript|mailto|tel):/i", $url))
                           )
                            $buffer[$depth] .=
                                //" rel=\"nofollow\"" .
                                " onclick=\"window.open(this.href);return false;\"";

                        $buffer[$depth] .= ">";
                    };

                    $char             = ""; //consume current character
                    $buffer[++$depth] = ""; //increase buffer level
                    $mode             = 0;  //reset mode
                    break;
                };

                //append current character
                $marker .= $char;
                break;

            default:
//..................................................................................................

                //replace html/xml characters with entities
                if (isset($html_chars[$char])) $char = $html_chars[$char];

                //collect adjacent whitespace
                if ($_char === " ")
                    $whitespace .= ($char === " ") ? "&nbsp;" : " ";
                if ($char === "\n")
                {
                    $whitespace .= ($_char === "\n") ? "<br class=\"multiple\">" : "<br>";
                    break;
                };
                if ($char === "\r") break;
                if ($char === " ")  break;

                //table
                if ($table_depth > 0)
                {
                    if ($char === "|") //table delimiter
                    {
                        if ($table_char === "|") //table row delimiter
                        {
                            $buffer[$depth] .=
                                "</td>" .
                                "</tr>" .
                                "<tr>" .
                                "<td style=\"PADDING:0 1em 1em 0\">";

                            //consume memorized table delimiter
                            $table_char = "";
                        }
                        else
                        {
                            //memorize table delimiter
                            $table_char = $char;
                        };

                        $char       = ""; //consume current character
                        $whitespace = ""; //discard whitespace
                    }
                    else
                    {
                        if ($table_char === "|") //table cell delimiter
                        {
                            $buffer[$depth] .=
                                "</td>" .
                                "<td style=\"PADDING:0 1em 1em 0\">";
                            $whitespace = ""; //discard whitespace
                        }
                        elseif ($table_char === "") //initial table cell
                        {
                            $whitespace = ""; //discard whitespace
                        };

                        //memorize table character
                        $table_char = $char;
                    };
                };

                switch ($char)
                {
                case "%": //token

                    if (

                    //unescaped token delimiter
                         (($_char === "%") && ($__char !== "\\"))
                         &&

                    //token support enabled
                         $token
                       )
                    {
                        //append current character
                        $buffer[$depth] .= $whitespace . $char;
                        $whitespace      = ""; //consume whitespace
                        $mode            = 1;  //switch to token mode
                        break 2;
                    };

                    break;

                case "[": //start formatting

                    if ($_char === "\\") //escaped
                    {
                        $_char = ""; //consume escape character
                        break;
                    };

                    //append whitespace
                    $buffer[$depth] .= $whitespace;
                    $whitespace      = ""; //consume whitespace
                    $marker          = ""; //reset marker
                    $mode            = 2;  //switch to formatting mode
                    break 2;

                case "]": //end formatting

                    if ($_char === "\\") //escaped
                    {
                        $_char = ""; //consume escape character
                        break;
                    };

                    //retrieve memorized marker
                    if ($marker = array_pop($stack))
                    {
                        //format marker
                        if (isset($format[$marker]))
                        {
                            switch ($marker)
                            {

                            //image
                            case "IMG":
                            case "<-IMG":
                            case "IMG->":

                                $_buffer = preg_split("/\s+/", $buffer[$depth], 3);
                                $url     = $_buffer[0];
                                $title   = NULL;
                                if (streq(substr($url, 0, 8), "image://"))
                                {
                                    if ($image !== NULL) $title = l($image->data->get($url, "name"));
                                    $url = translate_url($url);
                                };
                                $width   = (isset($_buffer[1]) && nstreq($_buffer[1], "*")) ?
                                           $_buffer[1] : NULL;
                                $height  = (isset($_buffer[2]) && nstreq($_buffer[2], "*")) ?
                                           $_buffer[2] : NULL;

                                switch ($marker)
                                {
                                case "IMG":   $style = "MARGIN:0 1em 1em 0;MAX-WIDTH:100%"; break;
                                case "<-IMG": $style = "FLOAT:left;MARGIN:0 1em 1em 0;MAX-WIDTH:100%"; break;
                                case "IMG->": $style = "FLOAT:right;MARGIN:0 0 1em 1em;MAX-WIDTH:100%"; break;
                                };

                                $buffer[$depth] = image($url, $width, $height, $title, $style, TRUE);
                                break;

                            //table
                            case "#":

                                $whitespace = ""; //discard whitespace
                                $table_depth--;   //decrease table nesting depth
                                break;
                            };

                            //replace marker with format completion
                            $marker = $format[$marker][1];
                        };

                        //decrease buffer level and append collected data
                        $buffer[--$depth] .= array_pop($buffer) . $whitespace . $marker;
                    }

                    //no formatting initiated
                    else
                    {
                        //append current character
                        $buffer[$depth] .= $whitespace . $char;
                    };

                    $whitespace = ""; //consume whitespace
                    break 2;
                };

                //append unconsumed backslash from previous pass
                if ($_char === "\\")
                {
                    $buffer[$depth] .= $_char;
                    $whitespace      = ""; //consume whitespace
                };

                //collect backslash
                if ($char === "\\")  break;

                //append current character
                $buffer[$depth] .= $whitespace . $char;
                $whitespace      = ""; //consume whitespace
            };

            //shift character
            $__char = $_char;
            $_char  = $char;
        };

        if ($depth === 0) break; //end parsing

        switch ($mode)
        {
        case 1: //token
            $mode = 0; //reset mode
            break;

        case 2: //formatting
            $text .= " ]"; //complete formatting
            break;
        };

        //close open formattings
        $text .= str_repeat("]", $depth);
        $c     = strlen($text);
    };

    //append remaining whitespace
    $buffer[0] .= $whitespace;
    if ($_char === " ")
        $buffer[0] .= " ";

    //append unconsumed backslash
    elseif ($_char === "\\")
        $buffer[0] .= $_char;

    //token
    if ($token)
        $buffer[0]  = parse_token($buffer[0]);

    return $buffer[0];
};

//--------------------------------------------------------------------------------------------------

function remove_format($text,
                       $token         = TRUE,
                       $discard_links = FALSE)
{
    static $format =
        ["+"     => TRUE,
         "/"     => TRUE,
         "_"     => TRUE,
         "<"     => TRUE,
         ">"     => TRUE,
         "<-"    => TRUE,
         "<->"   => TRUE,
         "->"    => TRUE,
         "*"     => TRUE,
         "\""    => TRUE,
         "-"     => TRUE,
         "IMG"   => TRUE,
         "<-IMG" => TRUE,
         "IMG->" => TRUE,
         "#"     => TRUE];

    $depth       = 0;              //output buffer level
    $buffer      = [$depth => ""]; //output buffer
    $mode        = 0;              //current parsing mode
    $stack       = [];             //format marker stack
    $marker      = "";             //format marker
    $table_depth = 0;              //table nesting depth
    $table_char  = "";             //table character
    $char        = "";             //current character
    $_char       = "";             //previous character
    $__char      = "";             //antepenultimate character
    $whitespace  = "";             //whitespace buffer
    $i           = 0;              //text position
    $c           = strlen($text);  //length of input text

    while (TRUE)
    {
        for (; $i < $c; $i++)
        {
            $char = $text[$i];

            switch ($mode)
            {
            case 1: //token
//..................................................................................................

                //append current character
                $buffer[$depth] .= $char;

                //unescaped token delimiter
                if (($char === "%") && ($_char === "%") && ($__char !== "\\"))
                    $mode = 0; //reset mode
                break;

            case 2: //formatting
//..................................................................................................

                if ($char === "\r") break;

                //unescaped space or linebreak
                if (
                     (($char === " ")                       && ($_char  !== "\\"))
                     ||
                     (($char === "\n") && ($_char !== "\r") && ($_char  !== "\\"))
                     ||
                     (($char === "\n") && ($_char === "\r") && ($__char !== "\\"))
                   )
                {
                    //marker
                    if (isset($format[$marker]))
                    {
                        //memorize format marker
                        array_push($stack, $marker);

                        //table
                        if ($marker === "#")
                        {
                            $buffer[$depth] .= "\n" . str_repeat("\t", $table_depth);
                            $table_char      = ""; //reset table character
                            $table_depth++;        //increase table nesting depth
                        };
                    }

                    //link, marker represents url
                    else
                    {
                        //discard url
                        if ($discard_links || preg_match("/^javascript:/i", $marker))
                        {
                            array_push($stack, "");
                        }

                        else
                        {
                            //evaluate url
                            $marker = translate_url($marker);
                            $marker = preg_replace("/^(?:mailto|tel):/i", "", $marker);
                            array_push($stack, " ($marker)");
                        };
                    };

                    $char             = ""; //consume current character
                    $buffer[++$depth] = ""; //increase buffer level
                    $mode             = 0;  //reset mode
                    break;
                };

                //append current character
                $marker .= $char;
                break;

            default:
//..................................................................................................

                //collect adjacent whitespace
                if (
                     ($char === " ")
                     ||
                     ($char === "\r")
                     ||
                     ($char === "\n")
                   )
                {
                    $whitespace .= $char;
                    break;
                };

                //table
                if ($table_depth > 0)
                {
                    if ($char === "|") //table delimiter
                    {
                        if ($table_char === "|") //table row delimiter
                        {
                            $buffer[$depth] .= "\n" . str_repeat("\t", $table_depth - 1);

                            //consume memorized table delimiter
                            $table_char      = "";
                        }
                        else
                        {
                            //memorize table delimiter
                            $table_char      = $char;
                        };

                        $char       = ""; //consume current character
                        $whitespace = ""; //discard whitespace
                    }
                    else
                    {
                        if ($table_char === "|") //table cell delimiter
                        {
                            $buffer[$depth] .= "\t";
                            $whitespace      = ""; //discard whitespace
                        }
                        elseif ($table_char === "") //initial table cell
                        {
                            $whitespace      = ""; //discard whitespace
                        };

                        //memorize table character
                        $table_char = $char;
                    };
                };

                switch ($char)
                {
                case "%": //token

                    if (

                    //unescaped token delimiter
                         (($_char === "%") && ($__char !== "\\"))
                         &&

                    //token support enabled
                         $token
                       )
                    {
                        //append current character
                        $buffer[$depth] .= $whitespace . $char;
                        $whitespace      = ""; //consume whitespace
                        $mode            = 1;  //switch to token mode
                        break 2;
                    };

                    break;

                case "[": //start formatting

                    if ($_char === "\\") //escaped
                    {
                        $_char = ""; //consume escape character
                        break;
                    };

                    //append whitespace
                    $buffer[$depth] .= $whitespace;
                    $whitespace      = ""; //consume whitespace
                    $marker          = ""; //reset marker
                    $mode            = 2;  //switch to formatting mode
                    break 2;

                case "]": //end formatting

                    if ($_char === "\\") //escaped
                    {
                        $_char = ""; //consume escape character
                        break;
                    };

                    //retrieve memorized marker
                    if (($marker = array_pop($stack)) !== NULL)
                    {
                        //format marker
                        if (isset($format[$marker]))
                        {
                            switch ($marker)
                            {

                            //image
                            case "IMG":
                            case "<-IMG":
                            case "IMG->":

                                //discard url
                                $buffer[$depth] = "";
                                break;

                            //table
                            case "#":

                                $whitespace = "\n"; //overwrite whitespace
                                $table_depth--;     //decrease table nesting depth
                                break;
                            };

                            $marker = ""; //clear marker
                        };

                        //decrease buffer level and append collected data
                        $buffer[--$depth] .= array_pop($buffer) . $whitespace . $marker;
                    }

                    //no formatting initiated
                    else
                    {
                        //append current character
                        $buffer[$depth] .= $whitespace . $char;
                    };

                    $whitespace = ""; //consume whitespace
                    break 2;
                };

                //append unconsumed backslash from previous pass
                if ($_char === "\\")
                {
                    $buffer[$depth] .= $whitespace . $_char;
                    $whitespace      = ""; //consume whitespace
                };

                //collect backslash
                if ($char === "\\")  break;

                //append current character
                $buffer[$depth] .= $whitespace . $char;
                $whitespace      = ""; //consume whitespace
            };

            //shift character
            $__char = $_char;
            $_char  = $char;
        };

        if ($depth === 0) break; //end parsing

        switch ($mode)
        {
        case 1: //token

            $mode = 0; //reset mode
            break;

        case 2: //formatting

            $text .= " ]"; //complete formatting
            break;
        };

        //close open formattings
        $text .= str_repeat("]", $depth);
        $c     = strlen($text);
    };

    //append unconsumed backslash
    if ($_char === "\\")
        $buffer[0] .= $whitespace . $_char;

    //token
    if ($token)
        $buffer[0]  = parse_token($buffer[0]);

    return $buffer[0];
};

//--------------------------------------------------------------------------------------------------

function get_first_image($text)
{
    return preg_match("/\[(?:IMG|<-IMG|IMG->)\s+([^\]]+)\]/s", $text, $text) ?
           $text[1] : NULL;
};

//--------------------------------------------------------------------------------------------------

function parse_token($text)
{
    static $token = NULL;

    if ($token === NULL)
    {
        if (! cms_load("token")) return $text;
        $token = new token();
    };

    return $token->apply($text);
};

//--------------------------------------------------------------------------------------------------

function quote_text($text,
                    $keyword,
                    $minimum_keyword_length = 3)
{
    $range    = []; //match range list
    $_keyword = []; //preg quoted keyword list

    //extract keywords
    preg_match_all("/" . CMS_REGEX_WORD . "/u", $keyword, $keyword);

    //remove repetitive keywords
    $keyword  = array_unique($keyword[0]);

    //sort by descending word length
    usort($keyword, function($a, $b) {
        return (utf8_strlen($b) - utf8_strlen($a)); });

    //go through keyword list to find relevant passages
    foreach ($keyword AS $value)
    {
        if (strlen($value) <= $minimum_keyword_length) continue; //keyword length too low
        $value   = preg_quote($value, "/");

        //find quotation
        $pattern = "/(?:^|" . CMS_REGEX_BORDER . ")(.{0,50}%s.{0,150})(?:$|" . CMS_REGEX_BORDER . ")/isu";

        if (

        //match exact word
             preg_match(sprintf($pattern, "(?:^|" . CMS_REGEX_BORDER . ")$value(?=$|" . CMS_REGEX_BORDER . ")"),
                        $text, $match, PREG_OFFSET_CAPTURE)
             ||

        //match beginning of word
             preg_match(sprintf($pattern, "(?:^|" . CMS_REGEX_BORDER . ")$value"),
                        $text, $match, PREG_OFFSET_CAPTURE)
             ||

        //match word ending
             preg_match(sprintf($pattern, "$value(?=$|" . CMS_REGEX_BORDER . ")"),
                        $text, $match, PREG_OFFSET_CAPTURE)
             ||

        //match any part of word
             preg_match(sprintf($pattern, $value),
                        $text, $match, PREG_OFFSET_CAPTURE)
           )
        {
            $_keyword[] = $value;                         //add keyword to highlight list
            $offset     = $match[1][1];                   //get offset position
            $end        = $offset + strlen($match[1][0]); //get end position

            //store quotation range
            if ((! isset($range[$offset])) || ($range[$offset] < $end))
                $range[$offset] = $end;
        };
    };

    if (count($range) === 0) return ""; //no matches found

    //merge relevant passages
    $buffer = "";
    $offset = 0;
    $end    = 0;

    ksort($range, SORT_NUMERIC); //sort by offset

    //iterate quotation ranges
    foreach ($range AS $key => $value)
    {
        if ($key > $end) //offset outside of previous quotation range
        {
            if ($end) $buffer .= substr($text, $offset, $end - $offset) . " … ";
            $offset = $key; //set new offset
        };

        $end = $value; //set current end
    };
    $buffer .= substr($text, $offset, $end - $offset) . " …";

    //highlight keywords
    $buffer = preg_replace("/(" . implode("|", $_keyword) . ")/iu",
                           "<strong>\\1</strong>",
                           x($buffer));

    return $buffer;
};

//--------------------------------------------------------------------------------------------------

function htmltoplain($string,
                     $format         = FALSE,
                     $discard_links  = FALSE,
                     $discard_images = FALSE)
{
    //clean up
    $search[]  = "/[\p{Cc}\p{Cn}\p{Cs}]+/u";
    $replace[] = "";

    $search[]  = "/[\s\p{Co}]+/u";
    $replace[] = " ";

    //comment
    $search[]  = "/<!--.*?-->/su";
    $replace[] = "";

    //head
    $search[]  = "/<head(?:\s[^>]*)?>.*?<\/head\s*>/isu";
    $replace[] = "";

    //script
    $search[]  = "/<script(?:\s[^>]*)?>.*?<\/script\s*>/isu";
    $replace[] = "";

    //style
    $search[]  = "/<style(?:\s[^>]*)?>.*?<\/style\s*>/isu";
    $replace[] = "";

    if ($format)
    {
        //br,
        $search[]  = "/<br(?:\s[^>]*)?>/iu";
        $replace[] = "\n";

        //div, ol, p, table, ul, h1 - h6
        $search[]  = "/<\/?(?:div|h[1-6]|ol|p|table|ul)(?:\s[^>]*)?>/iu";
        $replace[] = "\n\n";

        //li
        $search[]  = "/<li(?:\s[^>]*)?>/iu";
        $replace[] = "* ";

        $search[]  = "/<\/li\s*>/iu";
        $replace[] = "\n";

        //a
        if (! $discard_links)
        {
            $search[]  = "/<a\s[^>]*href=" .
                         "(?:([\"'])\s*(.+?)\s*\\1|([^>\s]+))" .
                         "[^>]*>" .
                         "(.*?)(\s*)" .
                         "<\/a\s*>/isu";
            $replace[] = "\\4 &lt;\\2\\3&gt;\\5";
        };

        //hr
        $search[]  = "/<hr(?:\s[^>]*)?>/iu";
        $replace[] = "\n--------------------------\n";

        //tr
        $search[]  = "/<\/tr\s*>/iu";
        $replace[] = "\n";

        //td
        $search[]  = "/<td(?:\s[^>]*)?>/iu";
        $replace[] = "\t";

        //th
        $search[]  = "/<th(?:\s[^>]*)?>/iu";
        $replace[] = "\t";
    };

    //img
    if (! $discard_images)
    {
        $search[]  = "/<img\s[^>]*alt=" .
                     "(?:([\"'])\s*((?:(?!\\1).){0,49}(?!\\1|[\s]).)(?=\\1|[\p{C}\p{P}\p{Z}])(?:(?!\\1).)*\\1|" .
                     "([\"'])\s*((?:(?!\\3).){0,50})|" .
                     "([^>\s]{0,49})(?=[>\s\p{C}\p{P}\p{Z}])|" .
                     "([^>\s]{0,50}))" .
                     "[^>]*>/isu";
        $replace[] = " [\\2\\4\\5\\6] ";
    };

    //strip remaining markup
    $search[]  = "/<[^>]+>/u";
    $replace[] = " ";

    //execute
    $string = preg_replace($search, $replace, $string);
    $search = $replace = [];

    //decode entities
    $string = html_entity_decode($string, ENT_QUOTES | ENT_HTML5, "UTF-8");

    if ($format)
    {
        //clean up
        $search[]  = "/(?:(?!\n|\t)[\p{Cc}\p{Cn}\p{Cs}])+/u";
        $replace[] = "";

        $search[]  = "/(?:[^\S\t\n]|\p{Co})+/u";
        $replace[] = " ";

        //remove line break adjacent whitespace
        $search[]  = "/[ \t]*(\n+)[ \t]*/u";
        $replace[] = "\\1";

        //conflate line break
        $search[]  = "/\n{3,}/u";
        $replace[] = "\n\n";
    }
    else
    {
        //clean up
        $search[]  = "/[\p{Cc}\p{Cn}\p{Cs}]+/u";
        $replace[] = "";

        $search[]  = "/[\s\p{Co}]+/u";
        $replace[] = " ";
    };

    //trim
    $search[]  = "/^\s+|\s+$/u";
    $replace[] = "";

    return preg_replace($search, $replace, $string);
};

//--------------------------------------------------------------------------------------------------

function text_similarity($text1,
                         $text2)
{
    $fingerprint1 = fingerprint($text1);
    $fingerprint2 = fingerprint($text2);

    $count_total  = count($fingerprint1) + count($fingerprint1);
    if ($count_total === 0) return 100; //both texts are empty and therefore identical

    $count_match  = count(array_intersect($fingerprint1, $fingerprint2)) +
                    count(array_intersect($fingerprint2, $fingerprint1));

    return 100 / $count_total * $count_match;
};

//--------------------------------------------------------------------------------------------------

function tokenize_text($text,
                       $cleanup_repeats = FALSE)
{
    //clean up and isolate ideograms
    $search  = ["/(?:(?!\s)[\p{Cc}\p{Cn}\p{Cs}\x{00AD}])+/u",
                "/(?:[\s\p{Co}]|\p{So}[\p{Cf}\p{M}\p{Sk}]*)+/u",
                "/[\p{Han}\p{Hangul}\p{Hiragana}\p{Katakana}]/u"];
    $replace = ["", " ", " \\0 "];

    //extra cleanup
    if ($cleanup_repeats)
    {
        $search[]  = "/(.)\\1{2,}/u";
        $replace[] = "\\1\\1";
    };

    //preprocess
    $text = preg_replace($search, $replace, $text);

    //separate tokens
    return preg_split("/" . CMS_REGEX_BORDER . "/u", $text, -1, PREG_SPLIT_NO_EMPTY);
};