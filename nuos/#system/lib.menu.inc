<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

if (! defined("CMS_MENU_FILTER_NORMAL"))
{
    define("CMS_MENU_FILTER_NORMAL",      0);
    define("CMS_MENU_FILTER_OPEN",        1);
    define("CMS_MENU_FILTER_ACTIVE",      2);
    define("CMS_MENU_FILTER_ACTIVE_OPEN", 3);
};

//==================================================================================================
//   MENU CLASS
//==================================================================================================

class menu
{

//properties
//--------------------------------------------------------------------------------------------------

    protected $flexview    = NULL;
    protected $level       = 0;
    protected $depth       = NULL;
    protected $filter      = CMS_MENU_FILTER_NORMAL;
    protected $show_images = NULL;
    protected $exclude     = NULL;
    protected $start       = 0;
    protected $end         = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($index            = 0,
                         $base             = 0,
                         $level            = 0,
                         $depth            = NULL,
                         $filter           = CMS_MENU_FILTER_NORMAL,
                         $show_icons       = FALSE,
                         $show_images      = FALSE,
                         $show_description = FALSE,
                         $show_hidden      = FALSE,
                         $exclude          = NULL,
                         $start            = 0,
                         $length           = NULL)
    {
        if (! cms_load("directory")) return; //library unavailable

        //get display object containing the directory tree structure
        $this->flexview = directory_get_flexview(! $show_hidden);

        //set currently selected directory entry (or nearest visible entry above)
        if (! $show_hidden) $index = directory_get_visible($index);
        $this->flexview->set_index($index);

        //set base entry of directory branch to be displayed
        //all elements outside that branch are ignored
        $this->flexview->set_base($base);

        //set display string
        $this->flexview->display =
            "<a[ href=\"%action%\"]" .
              "[ title=\"%description_plain%\"]" .
              "[ class=\"%class%\"]>" .
            ($show_images      ? "[<img src=\"%image_hover_url%\" " .
                                       "alt=\"%name%\" " .
                                       "class=\"hover\">]" .
                                 "[<img src=\"%image_url%\" " .
                                       "alt=\"%name%\">]"
                               : "") .
            ($show_icons       ? "[%icon% ]"
                               : "") .
            "%name%" .
            "</a>" .
            ($show_description ? "[<br><span>%description%</span>]"
                               : "");

        //set visibility parameters

        //level
        $this->level = max(preg_match("/^(?:\+|-)/", $level) ?
                           count($this->flexview->get_path()) + (int)$level : //relative
                           (int)$level,                                       //absolute
                           0);

        //depth
        if ($depth !== NULL) $this->depth = max((int)$depth, 0);

        //filter
        switch (utf8_strtolower($filter))
        {
        case "open":        $this->filter = CMS_MENU_FILTER_OPEN;        break;
        case "active":      $this->filter = CMS_MENU_FILTER_ACTIVE;      break;
        case "active-open": $this->filter = CMS_MENU_FILTER_ACTIVE_OPEN; break;
        default:            $this->filter = CMS_MENU_FILTER_NORMAL;      break;
        };

        //show icons
        if ($show_icons) $this->flexview->set_icon(directory_get_type());

        //show images
        $this->show_images = $show_images;

        //exclusion
        if ($exclude !== NULL)
            $this->exclude = is_array($exclude) ?
                             $exclude :
                             preg_split("/\s+/", trim($exclude));

        //start
        $this->start = max((int)$start, 0);

        //end
        if ($length !== NULL) $this->end = $this->start + max((int)$length, 0);

        //evoke display method
        $this->flexview->show_custom([$this, "show"]);
    }

//methods
//--------------------------------------------------------------------------------------------------

    function show($flexview_entry)
    {
        static $count       = 0;
        static $indentation = -1;
        static $open        = TRUE;
        static $instance    = 0;

        //the passed variable flexview_entry is a structure containing
        //all necessary information about the current entry
        //each entry below the given base entry is processed
        //beginning with the top of the tree and following the branches

        switch ($flexview_entry->type) //the type of event that is passed
        {
        case CMS_FLEXVIEW_ENTRY_TYPE_BASE: //before first entry
//..................................................................................................

            //reset
            $count        = 0;
            $indentation  = -1;
            $open         = [-1 => FALSE];
            $instance    += 1;
            return;

        case CMS_FLEXVIEW_ENTRY_TYPE_ENTRY: //current entry
//..................................................................................................

            $open[$flexview_entry->indentation] = $flexview_entry->open;

            //check if entry is to be displayed
            if (($this->filter === CMS_MENU_FILTER_ACTIVE_OPEN) &&
                (! $flexview_entry->open) &&
                (! $open[$flexview_entry->indentation - 1]))
                return TRUE; //skip subsequent entries

            //check if entry is excluded
            if (($this->exclude !== NULL) &&
                in_array($flexview_entry->index, $this->exclude))
                return TRUE; //skip subsequent entries

            //check if entry is to be displayed
            $visible = ($flexview_entry->indentation >=  $this->level) &&
                       (($this->depth === NULL) ||
                       ($flexview_entry->indentation < ($this->level + $this->depth))) &&
                       (($this->filter !== CMS_MENU_FILTER_ACTIVE) || $open[0]) &&

                       ($count >= $this->start) &&
                       (($this->end === NULL) ||
                       ($count < $this->end));

            $count++;

            if (! $visible) return;

            //start sublist
            if ($indentation < $flexview_entry->indentation)
            {
                $this->_ul_li($instance, $flexview_entry->index, $flexview_entry->open);
                $indentation = $flexview_entry->indentation;
            }

            //end sublist
            else
            {
                for (; $indentation > $flexview_entry->indentation; $indentation--)
                    echo("</li></ul>");

                //separate entry
                echo("</li>");
                $this->_li($instance, $flexview_entry->index, $flexview_entry->open);
            };

            //display entry
            $this->flexview->display($flexview_entry->index,
                                     $flexview_entry->open);

            //check if subsequent entries are to be displayed
            if (($this->filter === CMS_MENU_FILTER_OPEN) &&
                (! $flexview_entry->open))
                return TRUE; //skip subsequent entries

            return; //display subsequent entries

        case CMS_FLEXVIEW_ENTRY_TYPE_END: //after last entry
//..................................................................................................

            //end sublist
            for (; $indentation >= $this->level; $indentation--)
                echo("</li></ul>");
        };
    }

//--------------------------------------------------------------------------------------------------

    protected function _ul_li($instance, $index, $open)
    {
        $id = "menu" . x($instance) . "-" . x($index);
        $this->_style($id, $index, $open);
        echo("<ul><li id=\"" . x($id) . "\">");
    }

//--------------------------------------------------------------------------------------------------

    protected function _li($instance, $index, $open)
    {
        $id = "menu" . x($instance) . "-" . x($index);
        $this->_style($id, $index, $open);
        echo("<li id=\"" . x($id) . "\">");
    }

//--------------------------------------------------------------------------------------------------

    protected function _style($id, $index, $open)
    {
        return;
        if (! $this->show_images) return;

        $image_url   = $this->flexview->get_image($index, $open);
        $image_hover = isset($this->flexview->object[$index]["#data"][$this->flexview->image_hover_key]) ?
                       translate_url(l($this->flexview->object[$index]["#data"][$this->flexview->image_hover_key])) : NULL;
        $flag        = ($image_url !== NULL) || ($image_hover !== NULL);

        if ($flag)                 echo("<style>");
        if ($image_url   !== NULL) echo("#" . x($id) .       "{BACKGROUND-IMAGE:url(\"" . x($image_url) .   "\");}");
        if ($image_hover !== NULL) echo("#" . x($id) . ":hover{BACKGROUND-IMAGE:url(\"" . x($image_hover) . "\");}");
        if ($flag)                 echo("</style>");
    }
};