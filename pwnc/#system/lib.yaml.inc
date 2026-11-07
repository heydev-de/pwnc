<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   YAML 1.2 CLASS
//==================================================================================================

//related functions
//--------------------------------------------------------------------------------------------------

function yaml_parse($input)
{
    return yaml_parser::parse($input);
};

class yaml_parser
{

//properties
//--------------------------------------------------------------------------------------------------

    public static $source; //input document
    public static $length; //document length
    public static $pos;    //document position
    public static $tag;    //tag registry
    public static $anchor; //anchor registry
    public static $error;  //error flag
    public static $errmsg; //failure reason with line number

    const TAG_PREFIX = "tag:yaml.org,2002:"; //core schema tag prefix

    const CHOMP_CLIP  = 0; //keep final line break
    const CHOMP_STRIP = 1; //remove line breaks
    const CHOMP_KEEP  = 2; //keep line breaks

    const BLOCK     = 1; //block collections may start here
    const FLOW      = 2; //inside flow collection
    const NO_INDENT = 4; //sequence may share node indent

//methods
//--------------------------------------------------------------------------------------------------

    static function parse($input)
    {
        //normalize line breaks
        $input = preg_replace("/\r\n?/", "\n", (string)$input);

        //strip byte order mark
        if (substr($input, 0, 3) === "\xEF\xBB\xBF") $input = substr($input, 3);

        //reset
        self::$source = $input;
        self::$length = strlen($input);
        self::$pos    = 0;
        self::$error  = FALSE;
        self::$errmsg = "";

        $result = []; //all documents in stream order

        while (TRUE)
        {
            //directives and anchors reset per document
            self::$tag    = ["!" => "!", "!!" => self::TAG_PREFIX];
            self::$anchor = [];

            $directive = FALSE; //directive demands explicit document start
            $version   = FALSE; //one %YAML per document
            $handle    = [];    //one %TAG per handle

            //directives
            while (TRUE)
            {
                //stream end, unless directive pending
                if (! self::next_line())
                    return $directive ? self::error("Directive without document.") : $result;

                //check directive start
                if ((self::line_indent() !== 0) || (self::char() !== "%")) break; //indented -> content

                //tokenize directive, separation and trailing comment excluded
                $eol   = self::eol();
                $text  = substr(self::$source, self::$pos + 1, $eol - self::$pos - 1);
                $text  = preg_replace("/^[\t ]+|[\t ]+(#.*)?$/", "", $text);
                $array = preg_split("/[\t ]+/", $text);

                //check directive
                switch (strtolower($array[0] ?? ""))
                {
                case "yaml": //version

                    if ($version || (count($array) !== 2))
                        return self::error("Invalid %YAML directive.");

                    if (! preg_match("/^1[.][0-9]+$/", $array[1]))
                        return self::error("Invalid YAML version.");

                    $version = TRUE;
                    break;

                case "tag": //register tag

                    //check handle
                    if (
                         (count($array) !== 3)
                         ||
                         (($array[1][0] ?? "") !== "!")
                         ||
                         (substr($array[1], -1) !== "!")
                       )
                        return self::error("Invalid %TAG directive.");

                    if (isset($handle[$array[1]]))
                        return self::error("Duplicate %TAG handle.");

                    $handle[$array[1]]    = TRUE;
                    self::$tag[$array[1]] = $array[2];
                };

                $directive = TRUE;

                //next line
                self::$pos = min($eol + 1, self::$length);
            };

            //directive requires explicit document
            if ($directive && (self::get_marker() !== "-"))
                return self::error("Directive without document.");

//..................................................................................................

            //document start

            $context = self::BLOCK;

            switch (self::get_marker())
            {
            case ".": //end marker in front of content -> empty document

                self::$pos += 3;
                self::skip_space();

                if (! self::at_line_end())
                    return self::error("Content behind end marker.");

                continue 2;

            case "-": //document start marker

                self::$pos += 3;
                self::skip_space();

                //no block collection on marker line
                $context = self::at_line_end() ? self::BLOCK : 0;
            };

            //parse document
            $value = self::parse_node(-1, $context);
            if (self::$error) return FALSE;

            $result[] = $value;

            //stream end
            if (! self::next_line()) return $result;

            //trailing content
            if (self::line_indent() !== 0)
                return self::error("Trailing content.");

            switch (self::get_marker())
            {
            case "": //missing marker

                return self::error("No document marker.");

            case ".": //document end marker

                self::$pos += 3;
                self::skip_space();

                //check for further content
                if (! self::at_line_end())
                    return self::error("Content behind end marker.");
            };
        };
    }

//--------------------------------------------------------------------------------------------------
//   NODE
//--------------------------------------------------------------------------------------------------

    static function parse_node($indent,
                               $context = 0,
                               $inherit = NULL)
    {
        $anchor   = NULL;
        $tag      = NULL;
        $is_plain = FALSE;
        $offset   = self::$pos;

        //process node properties
        while (TRUE)
        {
            switch (self::char())
            {
            case "!": //tag

                if ($tag !== NULL) return self::error("Two tags.");
                if (($tag = self::parse_property()) === FALSE) return FALSE;
                self::skip_space(); //separation before next property
                continue 2;

            case "&": //anchor

                if ($anchor !== NULL) return self::error("Two anchors.");
                if (($anchor = self::parse_property()) === FALSE) return FALSE;
                self::skip_space(); //separation before next property
                continue 2;

            default: //node start

                break 2;
            };
        };

        //property read here, not on line above
        $property = ($anchor !== NULL) || ($tag !== NULL);

        //tag may sit on line above
        $tag ??= $inherit;

//..................................................................................................

        //context-based nodes

        //process flow node
        if ($context & self::FLOW)
        {
            //flow node may continue below
            self::skip_blank();

            $char = self::char();

            //check if empty
            if (
                 ($char === "")                   //input end
                 ||
                 (strpos(",]}", $char) !== FALSE) //delimiter
                 ||
                 self::at_value()                 //empty key
               )

                //empty node takes value from tag
                return self::register_anchor($anchor, self::empty_node($tag));
        }

        //process non-flow node
        else
        {
            //check if empty
            if (self::at_line_end())
            {
                //placeholder while reading below
                self::register_anchor($anchor, NULL);

                $value = self::parse_below($indent, $anchor, $tag);
                if (self::$error) return FALSE;

                //sequence may share node indent
                if (
                     ($value === NULL)
                     &&
                     ($context & self::NO_INDENT)
                     &&
                     (self::line_indent() === $indent)
                     &&
                     (self::char() === "-")
                     &&
                     self::has_separator()
                   )
                {
                    if (! self::check_tag($tag, "seq")) return FALSE;
                    $value = self::parse_sequence($indent);
                };

                //nothing below -> tag decides
                if ($value === NULL) $value = self::empty_node($tag);

                return self::$error ? FALSE :
                       self::register_anchor($anchor, $value);
            };

            $_indent = self::line_indent();
            $char    = self::char();

            //next document begins -> node stays empty
            if (self::get_marker() !== "")
                return self::register_anchor($anchor, self::empty_node($tag));

            //process block
            if ($context & self::BLOCK)
            {
                //tab cannot indent nested collection
                $tabbed = self::char(-1) === "\t";

                //sequence indicator
                if (($char === "-") && self::has_separator())
                {
                    //block collection properties require own line
                    if ($property) return self::error("Property before collection.");
                    if ($tabbed)   return self::error("Tab indentation.");
                    return self::check_tag($tag, "seq") ?
                           self::register_anchor($anchor, self::parse_sequence($_indent)) :
                           FALSE;
                };

                //explicit key indicator
                if (($char === "?") && self::has_separator())
                {
                    if ($property) return self::error("Property before collection.");
                    if ($tabbed)   return self::error("Tab indentation.");
                    return self::check_tag($tag, "map") ?
                           self::register_anchor($anchor, self::parse_mapping($_indent)) :
                           FALSE;
                };

                //properties belong to empty key -> reread below
                if (($char === ":") && self::has_separator() && ! $property)
                {
                    if ($tabbed) return self::error("Tab indentation.");
                    return self::check_tag($tag, "map") ?
                           self::register_anchor($anchor, self::parse_mapping($_indent)) :
                           FALSE;
                };

                //keyed mapping context change -> reread
                if (self::key_end(self::eol()) !== FALSE)
                {
                    self::$pos = $offset; //rewind
                    if ($tabbed) return self::error("Tab indentation.");
                    return self::check_tag($inherit, "map") ?
                           self::parse_mapping(self::line_indent($offset)) :
                           FALSE;
                };
            };

            //empty key, value indicator next
            if (($char === ":") && self::has_separator())
                return self::register_anchor($anchor, self::empty_node($tag));
        };

//..................................................................................................

        //indicator-based nodes

        switch ($char)
        {
        case "*": //alias

            //alias carries no properties
            if (($anchor !== NULL) || ($tag !== NULL))
                return self::error("Property on alias.");

            self::$pos++;
            return array_key_exists($name = self::token(), self::$anchor) ?
                   self::$anchor[$name] : self::error("Unknown alias.");

        case "[": //flow collection
        case "{":

            return self::check_tag($tag, ($char === "{") ? "map" : "seq") ?
                   self::register_anchor($anchor, self::parse_flow($indent)) :
                   FALSE;

        case "\"": //quoted scalar
        case "'":

            $text = self::parse_quoted($char, $indent);
            break;

        case "|": //block scalar
        case ">":

            if ($context & self::FLOW)
                return self::error("Block scalar in flow.");
            $text = self::parse_block_scalar($indent, $char);
            break;

        default: //plain scalar

            $text     = self::parse_plain($indent, $context);
            $is_plain = TRUE;
        };

        return self::$error ? FALSE :
               self::register_anchor($anchor, self::resolve($text, $tag, $is_plain));
    }

//--------------------------------------------------------------------------------------------------

    static function parse_property()
    {
        //anchor
        if (self::char() === "&")
        {
            $token = self::token();
            return ($token === "&") ? self::error("Anchor without name.") : substr($token, 1);
        };

        //verbatim tag
        if (self::char(1) === "<")
        {
            $start = self::$pos + 2;
            $end   = strpos(self::$source, ">", $start);

            //closing bracket on current line
            if (($end === FALSE) || ($end >= self::eol()))
                return self::error("Unterminated verbatim tag.");

            $value = substr(self::$source, $start, $end - $start);

            //at least one ns-uri-char
            if (($value === "") || (strpbrk($value, "\t ") !== FALSE))
                return self::error("Invalid verbatim tag.");

            self::$pos = $end + 1;
        }

        //shorthand tag
        else
        {
            $token = self::token();

            //non-specific -> kind resolves
            if ($token === "!") return "!";

            //named handle -> registered prefix, including "!!"
            if (($n = strpos($token, "!", 1)) !== FALSE)
            {
                $handle = substr($token, 0, $n + 1);
                if (! isset(self::$tag[$handle]))
                    return self::error("Unknown tag handle."); //unregistered

                $value = self::$tag[$handle] . substr($token, $n + 1);
            }

            //local
            else
            {
                $value = $token;
            };
        };

        //only core schema tags affect resolution
        return (strncmp($value, self::TAG_PREFIX, strlen(self::TAG_PREFIX)) === 0) ? $value : "";
    }

//--------------------------------------------------------------------------------------------------

    static function register_anchor($anchor,
                                    $value)
    {
        if ($anchor !== NULL) self::$anchor[$anchor] = $value;
        return $value;
    }

//--------------------------------------------------------------------------------------------------

    static function empty_node($tag)
    {
        //collection tag -> empty collection
        switch ($tag)
        {
        case self::TAG_PREFIX . "seq":
        case self::TAG_PREFIX . "map":

            return [];
        };

        //resolve empty scalar
        return self::resolve("", $tag, TRUE);
    }

//--------------------------------------------------------------------------------------------------

    static function parse_below($indent,
                                $anchor = NULL,
                                $tag    = NULL)
    {
        //no content remaining
        if (! self::next_line()) return NULL;

        //not deeper -> different node
        if (self::line_indent() <= $indent) return NULL;

        //anchor and tag may split across lines
        $char   = self::char();
        $repeat = (($char === "&") && ($anchor !== NULL)) || (($char === "!") && ($tag !== NULL));

        //only key may repeat anchor or tag
        if ($repeat && (self::key_end(self::eol()) === FALSE))
            return self::error("Property repeated below.");

        //node below, tag from here applies to it
        return self::parse_node($indent, self::BLOCK, $tag);
    }

//--------------------------------------------------------------------------------------------------
//   BLOCK COLLECTION
//--------------------------------------------------------------------------------------------------

    static function parse_sequence($indent)
    {
        $result = [];

        while (TRUE)
        {
            self::$pos++;
            self::skip_space();

            //retrieve content
            $entry = self::at_line_end() ?
                     self::parse_below($indent) :
                     self::parse_node($indent, self::BLOCK);
            if (self::$error) return FALSE;

            $result[] = $entry;

            $_indent = self::next_entry();

            //not current entry
            if (
                 ($_indent !== $indent)
                 ||
                 (self::char() !== "-")
                 ||
                 ! self::has_separator()
               )
                return $result;
        };
    }

//--------------------------------------------------------------------------------------------------

    static function parse_mapping($indent)
    {
        $result = [];

        while (TRUE)
        {
            $key   = NULL;
            $value = NULL;

            //explicit key
            if ((self::char() === "?") && self::has_separator())
            {
                self::$pos++;

                //key is block indented node, like value
                $key = self::parse_mapping_value($indent, self::BLOCK);
                if (self::$error) return FALSE;

                $_indent = self::next_entry();

                //retrieve value if specified
                if (($_indent === $indent) && (self::char() === ":"))
                {
                    self::$pos++;
                    $value = self::parse_mapping_value($indent, self::BLOCK);
                    if (self::$error) return FALSE;
                };
            }

            //implicit key
            else
            {
                //locate separator
                $eol   = self::eol();
                $colon = self::key_end($eol);

                if ($colon === FALSE)
                    return self::error("No mapping entry.");

                //key is node -> no recursion
                $key = self::parse_node($indent, 0);
                if (self::$error) return FALSE;

                self::skip_space();

                //key must end at indicator position
                if (self::$pos !== $colon)
                    return self::error("Key runs past its colon.");

                self::$pos++;

                //retrieve value
                $value = self::parse_mapping_value($indent, 0);
                if (self::$error) return FALSE;
            };

            //convert to php array key
            $key = self::to_array_key($key);
            if (array_key_exists($key, $result)) //not unique
                return self::error("Duplicate key.");

            //store key => value
            $result[$key] = $value;

            $_indent = self::next_entry();

            //not current entry
            if ($_indent !== $indent) return $result;
        };
    }

//--------------------------------------------------------------------------------------------------

    static function parse_mapping_value($indent,
                                        $context)
    {
        self::skip_space();

        //value on current line
        if (! self::at_line_end())
        {
            $value = self::parse_node($indent, $context | self::NO_INDENT);
            if (self::$error) return FALSE;

            //compact collection may consume colon
            return (! ($context & self::BLOCK) && (self::char() === ":") && self::has_separator()) ?
                   self::error("Colon in value.") : $value;
        };

        //end of document
        if (! self::next_line())
            return self::$error ? FALSE : NULL;

        $_indent = self::line_indent();

        //only sequence may share key indent
        if (($_indent === $indent) && (self::char() === "-") && self::has_separator())
            return self::parse_sequence($indent);

        //not deeper -> empty
        return ($_indent <= $indent) ? NULL :
               self::parse_node($indent, self::BLOCK | self::NO_INDENT);
    }

//--------------------------------------------------------------------------------------------------
//   FLOW COLLECTION
//--------------------------------------------------------------------------------------------------

    static function parse_flow($indent)
    {
        $mapping = self::char() === "{";
        $close   = $mapping ? "}" : "]";
        $result  = [];

        self::$pos++;

        while (TRUE)
        {
            self::skip_blank();

            $_indent = self::line_indent();

            if (self::get_marker() !== "")
                return self::error("Document marker in flow.");

            //flow content stays right of node
            if ($_indent <= $indent)
                return self::error("Continuation not indented.");

            switch (self::char())
            {
            case "":     return self::error("Unterminated flow."); //input end before entry
            case ",":    return self::error("Empty flow entry.");  //nothing before it
            case $close: self::$pos++; return $result;             //empty or trailing comma
            };

            //explicit key indicator
            $explicit = ((self::char() === "?") && self::has_separator());
            if ($explicit) { self::$pos++; self::skip_blank(); };

            //quoted scalar or flow collection contains json
            $json = strpos("\"'[{", self::char()) !== FALSE;

            //flow mapping
            if ($mapping)
            {
                $key   = NULL;
                $value = NULL;

                //value indicator -> empty key
                if (! self::at_value())
                {
                    $key = self::parse_node($indent, self::FLOW);
                    if (self::$error) return FALSE;

                    self::skip_blank();
                    if (self::char() === "")
                        return self::error("Unterminated flow.");
                };

                //retrieve value if specified
                if (self::char() === ":")
                {
                    //no separation -> empty value
                    $adjacent = $json || self::has_separator();

                    self::$pos++;
                    self::skip_blank();

                    //delimiter -> empty node
                    if ($adjacent)
                    {
                        $value = self::parse_node($indent, self::FLOW);
                        if (self::$error) return FALSE;
                    };
                };

                //convert to php array key
                $key = self::to_array_key($key);
                if (array_key_exists($key, $result)) //not unique
                    return self::error("Duplicate key.");

                //store key => value
                $result[$key] = $value;
            }

            //flow sequence
            else
            {
                $line = self::eol();

                //retrieve entry
                $entry = self::parse_node($indent, self::FLOW);
                if (self::$error) return FALSE;

                self::skip_blank();

                //entry is key and value
                if (self::char() === ":")
                {
                    //implicit key shares line with colon, explicit need not
                    if (! $explicit && (self::$pos > $line))
                        return self::error("Key spans lines.");

                    //no separation -> empty value
                    $adjacent = $json || self::has_separator();

                    self::$pos++;
                    self::skip_blank();

                    $pair = NULL;

                    //delimiter -> empty node
                    if ($adjacent)
                    {
                        $pair = self::parse_node($indent, self::FLOW);
                        if (self::$error) return FALSE;
                    };

                    $entry = [self::to_array_key($entry) => $pair];
                }

                //entry is key only
                elseif ($explicit)
                {
                    $entry = [self::to_array_key($entry) => NULL];
                };

                $result[] = $entry;
            };

            self::skip_blank();

            switch (self::char())
            {
            case ",":    self::$pos++; continue 2;     //next entry
            case $close: self::$pos++; return $result; //collection end
            };

            //missing delimiter
            return self::error("Unterminated flow.");
        };
    }

//--------------------------------------------------------------------------------------------------
//   SCALAR
//--------------------------------------------------------------------------------------------------

    static function parse_plain($indent,
                                $context)
    {
        $array   = [];
        $is_flow = ($context & self::FLOW) !== 0;

        while (TRUE)
        {
            $eol = self::eol();

            if (! $array)
            {
                $char = self::char();

                //c-indicator cannot open plain scalar
                if (strpos("#%,@]`}", $char) !== FALSE) return self::error("Invalid scalar.");

                //these three only when content follows
                if (($char === "-") || ($char === ":") || ($char === "?"))
                {
                    $next = self::char(1);

                    if (($next === "") || ($next === " ") || ($next === "\t"))
                        return self::error("Invalid scalar.");

                    if ($is_flow && (strpos(",[]{}", $next) !== FALSE))
                        return self::error("Invalid scalar.");
                };
            };

            //process current line
            $offset = self::$pos;
            $stop   = FALSE;

            while (self::$pos < $eol)
            {
                $char = self::$source[self::$pos];

                //content unless followed by separator or delimiter
                if ($char === ":")
                {
                    if (
                         ((self::$pos + 1) >= $eol)
                         ||
                         (strpos("\t ", self::$source[self::$pos + 1]) !== FALSE)
                         ||
                         ($is_flow && (strpos(",[]{}", self::$source[self::$pos + 1]) !== FALSE))
                       )
                    {
                        //no content
                        $stop = TRUE;
                        break;
                    };

                    self::$pos++;
                    continue;
                };

                //content unless followed by line end or delimiter
                if (($char === "\t") || ($char === " "))
                {
                    $pos = self::space_end();

                    //trailing space -> end segment
                    if ($pos >= $eol) break;

                    $char = self::$source[$pos];

                    //comment -> end scalar
                    if ($char === "#")
                    {
                        $stop = TRUE;
                        break;
                    };

                    //flow indicator after space -> end scalar
                    if ($is_flow && (strpos(",[]{}", $char) !== FALSE))
                    {
                        self::$pos = $pos;
                        $stop      = TRUE;
                        break;
                    };

                    self::$pos = $pos;
                    continue;
                };

                //flow indicator -> end scalar
                if ($is_flow && (strpos(",[]{}", $char) !== FALSE))
                {
                    $stop = TRUE;
                    break;
                };

                self::$pos++;
            };

            //strip trailing separation
            $array[] = rtrim(substr(self::$source, $offset, self::$pos - $offset), "\t ");

            if ($stop) break; //scalar end

//..................................................................................................

            //continue scalar below

            $end   = self::$pos;                   //scalar end position
            $pos   = min($eol + 1, self::$length); //next line
            $count = 0;                            //line break count

            while (TRUE)
            {
                $pos = self::indent_end($pos);

                //tab past indentation is separation
                $pos = self::space_end($pos);

                //nothing below
                if ($pos >= self::$length)
                {
                    self::$pos = $end;
                    return implode("", $array);
                };

                //first character of line below
                switch (self::$source[$pos])
                {
                case "\n": //blank line

                    $count++;
                    $pos++;
                    continue 2;

                case "#": //comment

                    self::$pos = $end;
                    return implode("", $array);
                };

                //document marker -> end scalar
                if (self::get_marker($pos) !== "")
                {
                    self::$pos = $end;
                    return implode("", $array);
                };

                //flow: delimiter or value indicator ends scalar
                if ($is_flow)
                {
                    if (
                         (strpos(",[]{}", self::$source[$pos]) !== FALSE)
                         ||
                         ((self::$source[$pos] === ":") && self::has_separator($pos))
                       )
                    {
                        self::$pos = $end;
                        return implode("", $array);
                    };
                }

                //block: only deeper lines continue
                elseif (self::line_indent($pos) <= $indent)
                {
                    self::$pos = $end;
                    return implode("", $array);
                };

                self::$pos = $pos;

                //single break folds to space, blanks remain
                $array[] = ($count === 0) ? " " : str_repeat("\n", $count);
                break;
            };
        };

        return implode("", $array);
    }

//--------------------------------------------------------------------------------------------------

    static function parse_quoted($char,
                                 $indent)
    {
        //escapes valid in double quotes
        static $escape_char =
            ["0" => "\x00", "a"  => "\x07", "b" => "\x08", "t"  => "\t", "\t" => "\t",
             "n" => "\n",   "v"  => "\x0B", "f" => "\x0C", "r"  => "\r", "e"  => "\x1B",
             " " => " ",    "\"" => "\"",   "/" => "/",    "\\" => "\\",
             "N" => "\xC2\x85",
             "_" => "\xC2\xA0",
             "L" => "\xE2\x80\xA8",
             "P" => "\xE2\x80\xA9"];

        //hex digit count per numeric escape
        static $hex_length = ["x" => 2, "u" => 4, "U" => 8];

        //characters ending literal text
        $stop_char = ($char === "\"") ? "\n\"\\" : "\n'";

        $result = "";
        self::$pos++;

        while (TRUE)
        {
            $pos    = self::$pos;
            $closed = FALSE;
            $escape = FALSE;

            //process current line
            while ($pos < self::$length)
            {
                $byte = self::$source[$pos];

                //closing quote
                if ($byte === $char)
                {
                    //collapse doubled quote
                    if (
                         ($char === "'")
                         &&
                         (($pos + 1) < self::$length)
                         &&
                         (self::$source[$pos + 1] === "'")
                       )
                    {
                        $result .= "'";
                        $pos    += 2;
                        continue;
                    };

                    $pos++;
                    $closed = TRUE;
                    break;
                };

                //line break -> fold below
                if ($byte === "\n") break;

                //escape sequence
                if (($char === "\"") && ($byte === "\\"))
                {
                    //trailing backslash escapes line break
                    if (($pos + 1 >= self::$length) || (self::$source[$pos + 1] === "\n"))
                    {
                        $pos++;
                        $escape = TRUE;
                        break;
                    };

                    $code = self::$source[$pos + 1];

                    //numeric escape
                    if (isset($hex_length[$code]))
                    {
                        //digits must fit in input
                        $digits = $hex_length[$code];
                        if (($pos + 2 + $digits) > self::$length)
                            return self::error("Truncated escape.");

                        //digits must be hex
                        $hex = substr(self::$source, $pos + 2, $digits);
                        if (strspn($hex, "0123456789abcdefABCDEF") !== $digits)
                            return self::error("Invalid escape.");

                        //code point must encode
                        $encoded = utf8_chr(hexdec($hex));
                        if ($encoded === FALSE)
                            return self::error("Invalid code point.");

                        $result .= $encoded;
                        $pos    += 2 + $digits;
                        continue;
                    };

                    //named escape
                    if (! isset($escape_char[$code]))
                        return self::error("Invalid escape."); //not found

                    $result .= $escape_char[$code];
                    $pos    += 2;
                    continue;
                };

                //append complete run
                $run  = strcspn(self::$source, $stop_char, $pos, self::$length - $pos);
                $text = substr(self::$source, $pos, $run);

                //folding removes trailing white space
                if ((($pos + $run) < self::$length) && (self::$source[$pos + $run] === "\n"))
                    $text = rtrim($text, "\t ");

                $result .= $text;
                $pos    += $run;
            };

            //quote closed -> done
            if ($closed) { self::$pos = $pos; return $result; };

//..................................................................................................

            //fold break, continue below

            $count = 0;
            $pos++;

            //count blank lines
            while ($pos < self::$length)
            {
                $_pos = self::space_end($pos);

                //input ends in space -> missing closing quote
                if ($_pos >= self::$length)
                {
                    $pos = self::$length;
                    break;
                };

                //blank line
                if (self::$source[$_pos] === "\n")
                {
                    $count++;
                    $pos = $_pos + 1;
                    continue;
                };

                break; //content reached
            };

            //missing closing quote
            if ($pos >= self::$length)
                return self::error("Unterminated quote.");

            //quoted scalar may not cross document marker
            if (self::get_marker($pos) !== "")
                return self::error("Quote crosses document marker.");

            //continuation stays right of node
            if ((self::indent_end($pos) - $pos) <= $indent)
                return self::error("Continuation not indented.");

            //remove escaped line break
            if ($escape)
            {
                $result .= str_repeat("\n", $count);
            }

            //fold unescaped line break
            else
            {
                $result .= ($count === 0) ? " " : str_repeat("\n", $count);
            };

            self::$pos = self::space_end($pos);
        };
    }

//--------------------------------------------------------------------------------------------------

    static function parse_block_scalar($indent,
                                       $style)
    {
        $pos      = self::$pos + 1;
        $chomp    = self::CHOMP_CLIP;
        $explicit = 0;

        //header
        while ($pos < self::$length)
        {
            $byte = self::$source[$pos];

            //chomping indicator
            if (strpos("+-", $byte) !== FALSE)
            {
                if ($chomp !== self::CHOMP_CLIP)
                    return self::error("Two chomping indicators."); //already set
                $chomp = ($byte === "+") ? self::CHOMP_KEEP : self::CHOMP_STRIP;
            }

            //indentation indicator
            elseif (($byte >= "1") && ($byte <= "9"))
            {
                if ($explicit !== 0)
                    return self::error("Two indentation indicators."); //already set
                $explicit = (int)$byte;
            }

            //header end
            else
            {
                break;
            };

            $pos++;
        };

        //comment requires separation, cannot follow header
        if (($pos < self::$length) && (self::$source[$pos] === "#"))
            return self::error("Comment without separation.");

        $pos = self::space_end($pos);

        //only line break or comment allowed
        if (($pos < self::$length) && (strpos("\n#", self::$source[$pos]) === FALSE))
            return self::error("Content behind header.");

//..................................................................................................

        //collect content

        $array = [];
        $pos   = min(self::eol($pos) + 1, self::$length); //next line

        //indicator defines content indentation
        $_indent = ($explicit > 0) ? max($indent, 0) + $explicit : -1;
        $leading = 0; //widest blank line before first content line

        //process lines
        while ($pos < self::$length)
        {
            $n = self::indent_end($pos);

            //whitespace only line
            if (($n >= self::$length) || (self::$source[$n] === "\n"))
            {
                //spaces beyond content indentation are content
                $array[] = (($_indent >= 0) && (($n - $pos) > $_indent)) ?
                           substr(self::$source, $pos + $_indent, $n - $pos - $_indent) : "";

                if ($_indent < 0) $leading = max($leading, $n - $pos);

                $pos = min($n + 1, self::$length);
                continue;
            };

            //document marker ends scalar
            if (self::get_marker($n) !== "")
            {
                $pos = $n;
                break;
            };

            //not deeper or shorter indentation -> end scalar
            if (self::line_indent($n) < max($indent + 1, $_indent))
            {
                //tab is neither indentation nor empty line
                if (self::$source[$n] === "	")
                    return self::error("Tab indentation.");
                break;
            };

            //first content line defines indentation
            if ($_indent < 0)
            {
                $_indent = $n - $pos;

                //no earlier blank line may be wider
                if ($leading > $_indent) return self::error("Blank line over indented.");
            };

            $eol = self::eol($n);

            //store line without indentation
            $array[] = substr(self::$source, $pos + $_indent, $eol - $pos - $_indent);

            //next line
            $pos = min($eol + 1, self::$length);
        };

        //trailing empty lines -> chomping
        $count = count($array);
        while (end($array) === "") array_pop($array);
        $count -= count($array);

        //fold joins lines, literal keeps line breaks
        $text = ($style === ">") ? self::fold_lines($array) : implode("\n", $array);

        //final break: content line or input end
        $trailing = ($array && (($pos >= self::$length) || (self::$source[$pos - 1] === "\n"))) ? 1 : 0;

        switch ($chomp)
        {
        case self::CHOMP_CLIP: //keep final line break

            if (($text !== "") && ($trailing > 0)) $text .= "\n";
            break;

        case self::CHOMP_KEEP: //keep trailing line breaks

            $text .= str_repeat("\n", $count + $trailing);
        };

        self::$pos = $pos;
        return $text;
    }

//--------------------------------------------------------------------------------------------------

    static function fold_lines($array)
    {
        $result   = "";
        $count    = 0;
        $indented = FALSE;

        foreach ($array AS $value)
        {
            //blank line, counted for next separator
            if ($value === "") { $count++; continue; };

            $_indented = ($value[0] === " ") || ($value[0] === "\t");

            //indent either side prevents folding
            $fold = ! ($indented || $_indented);

            //first line requires no separator
            if ($result !== "")
            {
                //single line break folds into space
                if ($fold && ($count === 0))
                    $result .= " ";

                //line breaks remain, folding consumes one
                else
                    $result .= str_repeat("\n", $fold ? $count : $count + 1);
            }

            //leading blank lines are content
            elseif ($count > 0)
            {
                $result .= str_repeat("\n", $count);
            };

            $result  .= $value;
            $indented = $_indented;
            $count    = 0;
        };

        return $result;
    }

//--------------------------------------------------------------------------------------------------
//   VALUE
//--------------------------------------------------------------------------------------------------

    static function resolve($value,
                            $tag,
                            $is_plain)
    {
        //explicit tag
        if (($tag !== NULL) && ($tag !== ""))
        {
            //suffix behind core schema prefix names type, "" for "!"
            $type = substr($tag, strlen(self::TAG_PREFIX));

            switch ($type)
            {
            case "null":

                return NULL;

            case "bool":

                switch ($value)
                {
                case "true":
                case "True":
                case "TRUE":

                    return TRUE;

                case "false":
                case "False":
                case "FALSE":

                    return FALSE;
                };

                return self::error("Invalid bool.");

            case "int":
            case "float":

                return self::to_number($value, $type) ?? self::error("Invalid $type.");

            case "seq": //collection tag on scalar
            case "map":

                return self::error("Wrong tag for scalar.");
            };

            return $value; //string
        };

        //quoted text is always string
        if (! $is_plain) return $value;

//..................................................................................................

        //default plain resolution

        switch ($value)
        {
        case "":
        case "~":
        case "null":
        case "Null":
        case "NULL":

            return NULL;

        case "true":
        case "True":
        case "TRUE":

            return TRUE;

        case "false":
        case "False":
        case "FALSE":

            return FALSE;
        };

        return self::to_number($value, NULL) ?? $value;
    }

//--------------------------------------------------------------------------------------------------

    static function check_tag($tag,
                              $kind)
    {
        //type behind core schema prefix, "" for other tags and "!"
        $type = (($tag === NULL) || ($tag === "")) ? "" : substr($tag, strlen(self::TAG_PREFIX));

        switch ($type)
        {
        case "null": //scalar type
        case "bool":
        case "int":
        case "float":
        case "str":

            return self::error("Scalar tag on collection.");

        case "seq": //collection type, names one kind only
        case "map":

            if ($type !== $kind) return self::error("Wrong tag for $kind.");
        };

        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    static function to_number($value,
                              $type)
    {
        if (($value === "") || (strcspn($value, "+-.0123456789", 0, 1) !== 0)) return NULL;

        //core schema float specials
        if ($type !== "int")
        {
            switch ($value)
            {
            case ".inf":
            case ".Inf":
            case ".INF":
            case "+.inf":
            case "+.Inf":
            case "+.INF":

                return INF;

            case "-.inf":
            case "-.Inf":
            case "-.INF":

                return -INF;

            case ".nan":
            case ".NaN":
            case ".NAN":

                return NAN;
            };
        };

        //decimal, "+" widens past PHP_INT_MAX instead of clamping
        if (preg_match("/^[+-]?[0-9]+$/", $value))
            return ($type === "float") ? (float)$value : +$value;

        //octal and hexadecimal take no sign
        if (preg_match("/^0o[0-7]+$/", $value))
            return self::to_base_number($value, 8, $type);

        if (preg_match("/^0x[0-9a-fA-F]+$/", $value))
            return self::to_base_number($value, 16, $type);

        //float
        if (
             ($type !== "int")
             &&
             preg_match("/^[+-]?(?:\.[0-9]+|[0-9]+(?:\.[0-9]*)?)(?:[eE][+-]?[0-9]+)?$/", $value)
           )
            return (float)$value;

        //no number
        return NULL;
    }

//--------------------------------------------------------------------------------------------------

    static function to_base_number($value,
                                   $base,
                                   $type)
    {
        $digits = substr($value, 2);

        //octdec and hexdec return float on overflow
        $_value = ($base === 8) ? octdec($digits) : hexdec($digits);

        return (($type === "float") || is_float($_value)) ? (float)$_value : (int)$_value;
    }

//--------------------------------------------------------------------------------------------------

    static function to_array_key($value)
    {
        //php admits string and int keys
        switch (gettype($value))
        {
        case "NULL":    return "";
        case "integer": return $value;
        case "boolean": return $value ? "true" : "false";
        case "double":  return (string)$value;
        case "array":   return json_encode($value);
        };

        return $value; //string
    }

//--------------------------------------------------------------------------------------------------
//   POSITION
//--------------------------------------------------------------------------------------------------

    static function char($offset = 0)
    {
        $pos = self::$pos + $offset;
        return (($pos >= 0) && ($pos < self::$length)) ? self::$source[$pos] : "";
    }

//--------------------------------------------------------------------------------------------------

    static function token()
    {
        $pos = self::$pos;
        self::$pos += strcspn(self::$source, "\t\n ,[]{}", self::$pos, self::$length - self::$pos);
        return substr(self::$source, $pos, self::$pos - $pos);
    }

//--------------------------------------------------------------------------------------------------

    static function eol($pos = NULL)
    {
        $pos = strpos(self::$source, "\n", $pos ?? self::$pos);
        return ($pos === FALSE) ? self::$length : $pos;
    }

//--------------------------------------------------------------------------------------------------

    static function indent_end($pos = NULL)
    {
        $pos ??= self::$pos;
        return $pos + strspn(self::$source, " ", $pos, self::$length - $pos);
    }

//--------------------------------------------------------------------------------------------------

    static function space_end($pos = NULL)
    {
        $pos ??= self::$pos;
        return $pos + strspn(self::$source, "\t ", $pos, self::$length - $pos);
    }

//--------------------------------------------------------------------------------------------------

    static function line_indent($pos = NULL)
    {
        $pos  ??= self::$pos;
        $line   = $pos;

        //back to line start
        while (($line > 0) && (self::$source[$line - 1] !== "\n")) $line--;

        //tab cannot define indentation
        return strcspn(self::$source, "\t", $line, $pos - $line);
    }

//--------------------------------------------------------------------------------------------------

    static function skip_space()
    {
        self::$pos = self::space_end();
    }

//--------------------------------------------------------------------------------------------------

    static function skip_blank()
    {
        while (self::$pos < self::$length)
        {
            switch (self::$source[self::$pos])
            {
            case "\t": //whitespace
            case "\n":
            case " ":

                self::$pos++;
                continue 2;

            case "#": //comment, only when separated

                if (! self::at_comment()) return;

                self::$pos = self::eol();
                continue 2;

            default: //content

                return;
            };
        };
    }

//--------------------------------------------------------------------------------------------------

    static function next_line()
    {
        while (self::$pos < self::$length)
        {
            self::$pos = self::indent_end();
            if (self::$pos >= self::$length) return FALSE;

            switch (self::$source[self::$pos])
            {
            case "\t": //tab

                $n = self::space_end();
                if ($n >= self::$length) return FALSE;

                //skip empty or comment line
                switch (self::$source[$n])
                {
                case "\n": self::$pos = $n + 1;        continue 3;
                case "#":  self::$pos = self::eol($n); continue 3;
                };

                self::$pos = $n;
                return TRUE;

            case "\n": //line break

                self::$pos++;
                continue 2;

            case "#": //comment, only when separated

                if (! self::at_comment()) return TRUE;

                self::$pos = self::eol();
                continue 2;
            };

            return TRUE;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------

    static function next_entry()
    {
        //no line remaining
        if (! self::next_line()) return FALSE;

        //document marker
        if (self::get_marker() !== "") return FALSE;

        //tab indentation
        if (self::char(-1) === "\t") return FALSE;

        return self::line_indent();
    }

//--------------------------------------------------------------------------------------------------

    static function has_separator($pos = NULL)
    {
        $pos = ($pos ?? self::$pos) + 1;
        return ($pos >= self::$length) ? TRUE : strpos("\t\n ", self::$source[$pos]) !== FALSE;
    }

//--------------------------------------------------------------------------------------------------

    static function at_value()
    {
        if (self::char() !== ":")  return FALSE;
        if (self::has_separator()) return TRUE;

        return strpos(",[]{}", self::char(1)) !== FALSE; //flow indicator
    }

//--------------------------------------------------------------------------------------------------

    static function at_comment()
    {
        if (self::char() !== "#") return FALSE;

        //separation or input start must precede
        $prev = self::char(-1);

        return ($prev === "") || (strpos("\t\n ", $prev) !== FALSE);
    }

//--------------------------------------------------------------------------------------------------

    static function at_line_end()
    {
        $char = self::char();
        return ($char === "") || ($char === "\n") || self::at_comment();
    }

//--------------------------------------------------------------------------------------------------

    static function get_marker($pos = NULL)
    {
        $pos  ??= self::$pos;
        $char   = ($pos < self::$length) ? self::$source[$pos] : "";
        $rest   = self::$length - $pos; //bytes remaining

        //marker requires column zero
        if (($pos > 0) && (self::$source[$pos - 1] !== "\n")) return "";

        //too short, or no marker character
        if (($rest < 3) || (($char !== "-") && ($char !== "."))) return "";

        //marker requires separator behind it
        if (($rest > 3) && (strpos("\t\n ", self::$source[$pos + 3]) === FALSE)) return "";

        //all three characters equal
        return ((self::$source[$pos + 1] === $char) && (self::$source[$pos + 2] === $char)) ? $char : "";
    }

//--------------------------------------------------------------------------------------------------

    static function key_end($eol)
    {
        $quote = "";
        $prev  = " ";
        $depth = 0;

        for ($i = self::$pos; $i < $eol; $i++)
        {
            $char = self::$source[$i];

            //inside quoted run, skip to close
            if ($quote !== "")
            {
                //single quoted, doubling escapes
                if ($quote === "'")
                {
                    if ($char === "'")
                    {
                        //two quotes -> one
                        if ((($i + 1) < $eol) && (self::$source[$i + 1] === "'")) { $i++; continue; };
                        $quote = "";
                    };
                }

                //closing quote
                elseif ($char === "\"")
                {
                    $quote = "";
                }

                //double quoted, backslash escapes
                elseif ($char === "\\")
                {
                    $i++;
                };

                continue;
            };

            switch ($char)
            {
            case "\"": //quote, only where node can start
            case "'":

                if (strpos("\t ,[{", $prev) !== FALSE) $quote = $char;
                break;

            case "&": //property or alias token, colon belongs to name
            case "!":
            case "*":

                if (strpos("\t ,[{", $prev) !== FALSE)
                {
                    $i   += strcspn(self::$source, "\t\n ,[]{}", $i, $eol - $i) - 1;
                    $char = self::$source[$i]; //token end precedes what follows
                };
                break;

            case "[": //flow start, only where node can start
            case "{":

                if (strpos("\t ,[{", $prev) !== FALSE) $depth++;
                break;

            case "]": //flow end, only inside one
            case "}":

                if ($depth > 0) $depth--;
                break;

            case ":": //terminator

                if ($depth !== 0) break;
                if (($i + 1 >= $eol) || (strpos("\t ", self::$source[$i + 1]) !== FALSE)) return $i;
                break;

            case "#": //comment

                if (($prev === "\t") || ($prev === " ")) return FALSE;
                break;
            };

            $prev = $char;
        };

        return FALSE;
    }

//--------------------------------------------------------------------------------------------------
//   ERROR
//--------------------------------------------------------------------------------------------------

    static function error($errmsg)
    {
        self::$error = TRUE;

        if (self::$errmsg === "")
        {
            $line = (substr_count(self::$source, "\n", 0, self::$pos) + 1);
            self::$errmsg = "line $line: " . $errmsg;
        };

        return FALSE;
    }
};