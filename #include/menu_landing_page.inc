<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//should never happen if software is installed correctly
if (! cms_load("directory")) return;

class menu_landing_page
{
    var $flexview        = NULL;

    //key of landing page menu container
    var $container_key   = "_lp";

    //number of links to display per document
    var $number_of_links = 3;

    function __construct()
    {
        //get display object containing the directory tree structure
        $this->flexview = directory_get_flexview(FALSE);

        //set base entry of directory branch to be displayed
        //all elements outside that branch are ignored
        $this->flexview->set_base($this->container_key);
        if (! $this->flexview->base) return; //base not found

        //set display string
        $this->flexview->display = "<a[ href=\"%action%\"]>%name%</a>";

        //evoke display method
        $this->flexview->show_custom([$this, "show"]);
    }

    function show($flexview_entry)
    {
        //the passed variable flexview_entry is a structure containing
        //all necessary information about the current entry
        //each entry below the given base entry is processed
        //beginning with the top of the tree and following the branches

        static $list = NULL;

        switch($flexview_entry->type)
        {
        case CMS_FLEXVIEW_ENTRY_TYPE_BASE:

            if (! $flexview_entry->subcount) return TRUE; //no landing pages, skip subentries

            //reset list in case this menu is used more than once
            $list = NULL;

            return; //subentries are processed

        case CMS_FLEXVIEW_ENTRY_TYPE_ENTRY:

            //test for hidden flag
            if (isset($this->flexview->object[$flexview_entry->index]["#data"]["hidden"]) &&
                $this->flexview->object[$flexview_entry->index]["#data"]["hidden"])
                return; //skip current entry and process subentries

            //test for language match
            $name   = $this->flexview->object[$flexview_entry->index]["#data"][$this->flexview->name_key];
            $name   = language_get($name, CMS_LANGUAGE, TRUE);
            if ((string)$name == "") return; //skip current entry and process subentries

            //add current landing page to list
            $list[] = $flexview_entry->index;

            return; //subentries are processed

        case CMS_FLEXVIEW_ENTRY_TYPE_END:

            if ($list === NULL) return; //no landing pages

            $count  = count($list);
            if ($this->number_of_links > $count) $this->number_of_links = $count;
            $pace   = $count / $this->number_of_links;
            $offset = $this->string_to_number(CMS_ACTIVE_URL, $pace);

            //process from offset to number of elements
            for ($i = $offset, $_i = 0; $i < $count; $i += $pace, $_i++)
            {
                $key = floor($i);
                $this->flexview->display($list[$key]);
            };

            //process from overflow to offset
            for ($i = $i - $count; $_i < $this->number_of_links; $i += $pace, $_i++)
            {
                $key = floor($i);
                $this->flexview->display($list[$key]);
            };
        };
    }

    function string_to_number($string,
                              $maximum = 10)
    {
        $value = 0;
        $c     = strlen($string);

        for ($i = 0; $i < $c; $i++)
            $value += ord($string[$i]);

        $_maximum = ($maximum < 9) ? 9 : $maximum;

        while ($value > $_maximum)
        {
            $string = (string)$value;
            $value  = 0;
            $c      = strlen($string);

            for ($i = 0; $i < $c; $i++)
                $value += (int)$string[$i];
        };

        if ($maximum < 9) $value = $maximum / 9 * $value;

        return $value;
    }
};

new menu_landing_page();