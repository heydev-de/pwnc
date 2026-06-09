<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   DOCUMENT CLASS
//==================================================================================================

//constants
define("CMS_DOCUMENT_SEPARATOR", "\x1E"); //ascii record separator

//type
define("CMS_DOCUMENT_TYPE",      "0");
define("CMS_DOCUMENT_VALUE",     "1");
define("CMS_DOCUMENT_REFERENCE", "2");

class document
{

//properties
//--------------------------------------------------------------------------------------------------

    public $data           = [];
    public $default        = [];
    public $template_index = NULL;
    public $structure      = NULL;

//constructor
//--------------------------------------------------------------------------------------------------

    function __construct($text           = NULL,
                         $template_index = NULL)
    {
        if ($text !== NULL)           $this->import($text);
        if ($template_index !== NULL) $this->set_structure($template_index);
    }

//methods
//--------------------------------------------------------------------------------------------------

    function import($text)
    {
        $this->data = [];
        $buffer     = explode(CMS_DOCUMENT_SEPARATOR, $text);

        foreach ($buffer AS $key => $value)
        {
            if (preg_match("/^([0-9a-zA-Z_\.]*):(#?[a-z]*):(.*)$/s", $value, $_value))
            {
                $key  = $_value[1];
                $type = $_value[2];

                $this->data[$key] = [CMS_DOCUMENT_TYPE      => $type,
                                     CMS_DOCUMENT_VALUE     => $_value[3],
                                     CMS_DOCUMENT_REFERENCE => FALSE];

                if (streq($type, "#reference"))
                    $this->resolve_reference($key);
            }

            else
            {
                $this->data[(string)$key] = [CMS_DOCUMENT_TYPE      => NULL,
                                             CMS_DOCUMENT_VALUE     => $value,
                                             CMS_DOCUMENT_REFERENCE => FALSE];
            };
        };
    }

//--------------------------------------------------------------------------------------------------

    function resolve_reference($id)
    {
        if (! isset($this->data[$id]))  return; //invalid index
        if (! cms_load("content_pool")) return; //library unavailable

        $content_pool = new content_pool();
        $reference    = $this->data[$id][CMS_DOCUMENT_VALUE];
        $text         = $content_pool->get_text($reference);
        $buffer       = explode(CMS_DOCUMENT_SEPARATOR, $text);

        foreach ($buffer AS $key => $value)
        {
            if (preg_match("/^([0-9a-zA-Z_\.]*):(#?[a-z]*):(.*)$/s", $value, $_value))
            {
                $key   = $_value[1];
                $type  = $_value[2];
                $value = $_value[3];
            }

            else
            {
                $key   = (string)$key;
                $type  = NULL;
            };

            if ($key === "")                         $key = $id;
            elseif (streq(substr($key, 0, 2), "..")) $key = substr($key, 2);
            else                                     $key = "$id.$key";

            //do not overwrite an existing entry unless it contains a reference
            if (
                 (! isset($this->data[$key]))
                 ||
                 streq($this->data[$key][CMS_DOCUMENT_TYPE], "#reference")
                 ||
                 ($this->data[$key][CMS_DOCUMENT_REFERENCE] !== FALSE)
               )
                $this->data[$key] = [CMS_DOCUMENT_TYPE      => $type,
                                     CMS_DOCUMENT_VALUE     => $value,
                                     CMS_DOCUMENT_REFERENCE => $reference];

            $reference = TRUE;
        };
    }

//--------------------------------------------------------------------------------------------------

    function set_structure($template_index)
    {
        if (! cms_load("template")) return FALSE; //library unavailable

        $template  = new template();
        $structure = $template->structure($this, $template_index);
        if ($structure === FALSE)   return FALSE; //error parsing template

        $this->template_index = $template_index;
        $this->structure      = $structure;
        return TRUE;
    }

//--------------------------------------------------------------------------------------------------

    function update_structure()
    {
        if ($this->template_index === NULL) return FALSE; //no structure

        return $this->set_structure($this->template_index);
    }

//--------------------------------------------------------------------------------------------------

    function export($resolve_references = TRUE,
                    $cleanup            = TRUE)
    {
        //cleanup
        if ($this->template_index === NULL)
        {
            $cleanup = FALSE; //no structure
        }
        elseif ($cleanup)
        {
            //collect element ids and types from structure
            $id_list = NULL;

            for ($i = 1, $c = count($this->structure); $i <= $c; $i++)
            {
                $id           = $this->structure[$i][CMS_TEMPLATE_STRUCTURE_INDEX];
                $type         = $this->structure[$i][CMS_TEMPLATE_STRUCTURE_TYPE];
                $id_list[$id] = $type;
            };
        };

        $buffer = "";

        foreach ($this->data AS $key => $value)
        {
            //skip unlisted elements
            if ($cleanup)
            {
                if (! isset($id_list[$key]))                                      continue;
                if (nstreq($id_list[$key], $this->data[$key][CMS_DOCUMENT_TYPE])) continue;
            };

            $reference = $value[CMS_DOCUMENT_REFERENCE];

            //export resolved reference
            if ($resolve_references || ($reference === FALSE))
                $buffer .= $key . ":" .                                         //id
                           $value[CMS_DOCUMENT_TYPE] . ":" .                    //type
                           $value[CMS_DOCUMENT_VALUE] . CMS_DOCUMENT_SEPARATOR; //value

            //skip resolved elements
            elseif ($reference === TRUE)
                continue;

            //export unresolved reference
            else
                $buffer .= $key . ":" .                         //id
                           "#reference:" .                      //type
                           $reference . CMS_DOCUMENT_SEPARATOR; //value
        };

        return substr($buffer, 0, -1);
    }

//--------------------------------------------------------------------------------------------------

    function get($id,
                 $type        = NULL,
                 $use_default = TRUE)
    {
        if (streq($type, "group")) return TRUE;

        if (
             isset($this->data[$id])
             &&
             nstre($this->data[$id][CMS_DOCUMENT_VALUE])
             &&
             (
               ($type == NULL)
               ||
               ($this->data[$id][CMS_DOCUMENT_TYPE] == NULL)
               ||
               streq($type, $this->data[$id][CMS_DOCUMENT_TYPE])
             )
           )
            return $this->data[$id][CMS_DOCUMENT_VALUE];

        if (
             $use_default
             &&
             isset($this->default[$id])
             &&
             nstre($this->default[$id][CMS_DOCUMENT_VALUE])
             &&
             (
               ($type == NULL)
               ||
               ($this->default[$id][CMS_DOCUMENT_TYPE] == NULL)
               ||
               streq($type, $this->default[$id][CMS_DOCUMENT_TYPE])
             )
           )
            return $this->default[$id][CMS_DOCUMENT_VALUE];

        return NULL;
    }

//--------------------------------------------------------------------------------------------------

    function get_reference($id,
                           $type = NULL)
    {
        if (
             isset($this->data[$id])
             &&
             (
               ($type == NULL)
               ||
               ($this->data[$id][CMS_DOCUMENT_TYPE] == NULL)
               ||
               streq($type, $this->data[$id][CMS_DOCUMENT_TYPE])
             )
           )
            return $this->data[$id][CMS_DOCUMENT_REFERENCE];

        return NULL;
    }

//--------------------------------------------------------------------------------------------------

    function get_parent_template($path)
    {
        if ($this->template_index === NULL) return FALSE; //no structure

        //seek element
        for ($i = 1, $c = count($this->structure); $i <= $c; $i++)
        {
            if (streq($this->structure[$i][CMS_TEMPLATE_STRUCTURE_PATH], $path)) break; //found
        };

        if ($i > $c) return NULL; //element not found

        //seek parent template
        do
        {
            //element is template
            if (streq($this->structure[$i][CMS_TEMPLATE_STRUCTURE_TYPE], "template"))
                return $this->structure[$i][CMS_TEMPLATE_STRUCTURE_PATH];

            //get parent
            $i = $this->structure[$i][CMS_TEMPLATE_STRUCTURE_PARENT];
        }
        while ($i > 0);

        return NULL;
    }

//--------------------------------------------------------------------------------------------------

    function set($id,
                 $type,
                 $value)
    {
        $this->data[(string)$id] = [CMS_DOCUMENT_TYPE      => $type,
                                    CMS_DOCUMENT_VALUE     => $value,
                                    CMS_DOCUMENT_REFERENCE => FALSE];

        if (streq($type, "#reference"))
            $this->resolve_reference((string)$id);
    }

//--------------------------------------------------------------------------------------------------

    function set_default($id,
                         $type,
                         $value)
    {
        $this->default[(string)$id] = [CMS_DOCUMENT_TYPE  => $type,
                                       CMS_DOCUMENT_VALUE => $value];
    }

//--------------------------------------------------------------------------------------------------

    function extract($path)
    {
        if ($this->template_index === NULL) return FALSE; //no structure

        $excerpt = new document(); //excerpt

        //seek initial element
        for ($i = 1, $c = count($this->structure); $i <= $c; $i++)
        {
            if (streq($this->structure[$i][CMS_TEMPLATE_STRUCTURE_PATH], $path)) break; //found
        };

        if ($i > $c) return $excerpt; //initial element not found

        //add initial element to excerpt
        $id   = $this->structure[$i][CMS_TEMPLATE_STRUCTURE_INDEX];
        $type = $this->structure[$i][CMS_TEMPLATE_STRUCTURE_TYPE];

        $excerpt->data[""] = [CMS_DOCUMENT_TYPE      => $type,
                              CMS_DOCUMENT_VALUE     => $this->get($id, $type, FALSE),
                              CMS_DOCUMENT_REFERENCE => $this->get_reference($id, $type)];

        //no template or group
        if (! preg_match("/^(template|group)$/", $type)) return $excerpt;

        $offset  = $i;            //initial element index
        $length  = strlen($path); //length of initial path
        $_length = $length + 1;

        //collect element data in range
        for ($i++; $i <= $c; $i++)
        {
            //current element must be ancilliary to initial element
            if (($i > $offset) && ($this->structure[$i][CMS_TEMPLATE_STRUCTURE_PARENT] < $offset)) break;

            //add element to excerpt
            $id   = $this->structure[$i][CMS_TEMPLATE_STRUCTURE_INDEX]; //current id
            $_id  = streq(substr($id, 0, $length), $path) ? substr($id, $_length) : "..$id";
            $type = $this->structure[$i][CMS_TEMPLATE_STRUCTURE_TYPE];

            $excerpt->data[$_id] = [CMS_DOCUMENT_TYPE      => $type,
                                    CMS_DOCUMENT_VALUE     => $this->get($id, $type, FALSE),
                                    CMS_DOCUMENT_REFERENCE => $this->get_reference($id, $type)];
        };

        return $excerpt;
    }

//--------------------------------------------------------------------------------------------------

    function inject($path,
                    $document)
    {
        if ($this->template_index === NULL) return; //no structure

        //seek initial element
        for ($i = 1, $c = count($this->structure); $i <= $c; $i++)
        {
            if (streq($this->structure[$i][CMS_TEMPLATE_STRUCTURE_PATH], $path)) break; //found
        };

        if ($i > $c) return; //initial element not found

        $id      = $this->structure[$i][CMS_TEMPLATE_STRUCTURE_INDEX]; //initial element id
        $id_list = [];                                                 //injected elements list

        //generate list of injected elements
        foreach ($document->data AS $key => $_)
        {
            if ($key === "")                         $_id = $id;
            elseif (streq(substr($key, 0, 2), "..")) $_id = substr($key, 2);
            else                                     $_id = "$id.$key";

            $id_list[$_id] = $key;
        };

        $this->del($path); //clear target position

        //inject all elements ignoring target structure
        //injected elements may change target structure
        //redundant elements are removed later
        foreach ($id_list AS $key => $value)
        {
            $this->data[$key] = $document->data[$value];
            $this->data[$key][CMS_DOCUMENT_REFERENCE] = FALSE;
        };

        //recompute structure
        $this->update_structure();

        //reseek initial element
        for ($i = 1, $c = count($this->structure); $i <= $c; $i++)
        {
            if (streq($this->structure[$i][CMS_TEMPLATE_STRUCTURE_PATH], $path)) break; //found
        };

        if ($i > $c) return; //initial element not found

        $offset = $i; //initial element index

        //generate list of still unassigned elements
        //by finding and removing already assigned elements
        for (; isset($this->structure[$i]); $i++)
        {
            //current element must be ancilliary to initial element
            if (($i > $offset) && ($this->structure[$i][CMS_TEMPLATE_STRUCTURE_PARENT] < $offset)) break;

            $id = $this->structure[$i][CMS_TEMPLATE_STRUCTURE_INDEX]; //id of current target element

            if (

            //current element is in list
                 isset($id_list[$id])
                 &&

            //current element type matches target type
                 streq($document->data[$id_list[$id]][CMS_DOCUMENT_TYPE],
                       $this->structure[$i][CMS_TEMPLATE_STRUCTURE_TYPE])
               )
            {
                unset($id_list[$id]);             //remove element from list
                if (count($id_list) === 0) break; //no more elements in list
            };
        };

        //fit in unassigned elements at topmost target position that is not occupied
        foreach ($id_list AS $key => $_)
        {
            //remove redundant element from target
            //at this point all elements in the list have previously been added
            //to the target but are not connected to the structure
            unset($this->data[$key]);

            $id   = $id_list[$key];                          //id of current element
            $type = $document->data[$id][CMS_DOCUMENT_TYPE]; //type of current element

            //start processing at initial element
            for ($i = $offset; isset($this->structure[$i]); $i++)
            {
                ///current element must be ancilliary to initial element
                if (($i > $offset) && ($this->structure[$i][CMS_TEMPLATE_STRUCTURE_PARENT] < $offset)) break;

                $_id = $this->structure[$i][CMS_TEMPLATE_STRUCTURE_INDEX]; //id of current target element

                //current target element already has value of matching type
                if (
                     isset($this->data[$_id])
                     &&
                     streq($this->data[$_id][CMS_DOCUMENT_TYPE],
                           $this->structure[$i][CMS_TEMPLATE_STRUCTURE_TYPE])
                   )
                    continue;

                //current element must match type
                if (nstreq($type, $this->structure[$i][CMS_TEMPLATE_STRUCTURE_TYPE])) continue;

                $this->data[$_id] = $document->data[$id];
                $this->data[$_id][CMS_DOCUMENT_REFERENCE] = FALSE;
                break;
            };
        };
    }

//--------------------------------------------------------------------------------------------------

    function copy($path_source,
                  $path_target)
    {
        if (($source = $this->extract($path_source)) === FALSE) return;
        $this->inject($path_target, $source);
    }

//--------------------------------------------------------------------------------------------------

    function swap($path_source,
                  $path_target)
    {
        if (($source = $this->extract($path_source)) === FALSE) return;
        if (($target = $this->extract($path_target)) === FALSE) return;
        $this->inject($path_target, $source);
        $this->inject($path_source, $target);
    }

//--------------------------------------------------------------------------------------------------

    function kick($path,
                  $value)
    {
        if (($value = (int)$value) === 0)   return; //invalid kick value
        if ($this->template_index === NULL) return; //no structure

        //seek initial element
        for ($i = 1, $c = count($this->structure); $i <= $c; $i++)
        {
            if (streq($this->structure[$i][CMS_TEMPLATE_STRUCTURE_PATH], $path)) break; //found
        };

        if ($i > $c) return; //initial element not found

        //seek relevant parent of initial element
        for ($parent = $this->structure[$i][CMS_TEMPLATE_STRUCTURE_PARENT];
             $parent > 0;
             $parent = $this->structure[$parent][CMS_TEMPLATE_STRUCTURE_PARENT])
        {
            if (preg_match("/^(template|group)$/", $this->structure[$parent][CMS_TEMPLATE_STRUCTURE_TYPE])) break;
        };

        $offset = $i;          //initial element index
        $array  = [];          //document excerpts
        $sgn    = sgn($value); //sign of shift value
        $value  = abs($value); //discard sign

        //initial element level
        for ($depth = 0; ($i = $this->structure[$i][CMS_TEMPLATE_STRUCTURE_PARENT]) > 0; $depth++);

        //start processing at initial element proceeding in kick direction
        for ($i = $offset; isset($this->structure[$i]); $i += $sgn)
        {
            //kicked and initial element must share the same relevant parent
            if ($i < $parent)                                                  break;
            if ($this->structure[$i][CMS_TEMPLATE_STRUCTURE_PARENT] < $parent) break;

            //level of kicked element
            for ($_i = $i, $_depth = 0; ($_i = $this->structure[$_i][CMS_TEMPLATE_STRUCTURE_PARENT]) > 0; $_depth++);

            //kicked and initial element must be on the same level
            if ($_depth !== $depth) continue;

            $_path = $this->structure[$i][CMS_TEMPLATE_STRUCTURE_PATH]; //path of kicked element
            $type  = $this->structure[$i][CMS_TEMPLATE_STRUCTURE_TYPE]; //type of kicked element
            $count = 0;                                                 //kick target count

            //find nth position of compatible type in kick direction
            for ($_i = $i + $sgn; isset($this->structure[$_i]); $_i += $sgn)
            {
                //target and initial element must share the same relevant parent
                if ($_i < $parent)                                                  break;
                if ($this->structure[$_i][CMS_TEMPLATE_STRUCTURE_PARENT] < $parent) break;

                $_type  = $this->structure[$_i][CMS_TEMPLATE_STRUCTURE_TYPE]; //type of target

                //kicked element and target must be compatible
                if (nstreq($_type, $type))  continue;

                //level of target element
                for ($__i = $_i, $_depth = 0; ($__i = $this->structure[$__i][CMS_TEMPLATE_STRUCTURE_PARENT]) > 0; $_depth++);

                //target and initial element must be on the same level
                if ($_depth !== $depth)     continue;

                $count++; //increase kick target count

                $__path = $this->structure[$_i][CMS_TEMPLATE_STRUCTURE_PATH]; //path of target position

                //target may not be used more than once
                if (isset($array[$__path])) continue;

                if ($count >= $value) //target reached
                {
                    $array[$__path] = $this->extract($_path); //extract kicked element
                    break;
                };
            };

            //remove kicked element
            $this->del($_path);
        };

        //apply changes to current document
        foreach ($array AS $key => $value)
            $this->inject($key, $value);
    }

//--------------------------------------------------------------------------------------------------

    function drop($path,
                  $value)
    {
        if (($value = sgn((int)$value)) === 0) return; //invalid drop value
        if ($this->template_index === NULL)    return; //no structure

        //seek initial element
        for ($i = 1, $c = count($this->structure); $i <= $c; $i++)
        {
            if (streq($this->structure[$i][CMS_TEMPLATE_STRUCTURE_PATH], $path)) break; //found
        };

        if ($i > $c) return; //initial element not found

        //seek relevant parent of initial element
        for ($parent = $this->structure[$i][CMS_TEMPLATE_STRUCTURE_PARENT];
             $parent > 0;
             $parent = $this->structure[$parent][CMS_TEMPLATE_STRUCTURE_PARENT])
        {
            if (preg_match("/^(template|group)$/", $this->structure[$parent][CMS_TEMPLATE_STRUCTURE_TYPE])) break;
        };

        $offset = $i; //initial element index
        $array  = []; //document excerpts

        //initial element level
        for ($depth = 0; ($i = $this->structure[$i][CMS_TEMPLATE_STRUCTURE_PARENT]) > 0; $depth++);

        //remove initial element
        $this->del($path);

        //start processing after initial element proceeding in drop direction
        for ($i = $offset + $value; isset($this->structure[$i]); $i += $value)
        {
            //dropped and initial element must share the same relevant parent
            if ($i < $parent)                                                  break;
            if ($this->structure[$i][CMS_TEMPLATE_STRUCTURE_PARENT] < $parent) break;

            //level of dropped element
            for ($_i = $i, $_depth = 0; ($_i = $this->structure[$_i][CMS_TEMPLATE_STRUCTURE_PARENT]) > 0; $_depth++);

            //dropped and initial element must be on the same level
            if ($_depth !== $depth) continue;

            $_path = $this->structure[$i][CMS_TEMPLATE_STRUCTURE_PATH]; //path of dropped element
            $type  = $this->structure[$i][CMS_TEMPLATE_STRUCTURE_TYPE]; //type of dropped element

            //find next position of compatible type contrary to drop direction
            //starting after dropped and stopping at initial element
            for ($_i = $i - $value; ($_i * $value) >= ($offset * $value); $_i -= $value)
            {
                //target and initial element must share the same relevant parent
                if ($_i < $parent)                                                  break;
                if ($this->structure[$_i][CMS_TEMPLATE_STRUCTURE_PARENT] < $parent) break;

                $__path = $this->structure[$_i][CMS_TEMPLATE_STRUCTURE_PATH]; //path of target position

                //target may not be used more than once
                if (isset($array[$__path])) continue;

                $_type  = $this->structure[$_i][CMS_TEMPLATE_STRUCTURE_TYPE]; //type of target

                //dropped element and target must be compatible
                if (nstreq($_type, $type))  continue;

                //level of target element
                for ($__i = $_i, $_depth = 0; ($__i = $this->structure[$__i][CMS_TEMPLATE_STRUCTURE_PARENT]) > 0; $_depth++);

                //target and initial element must be on the same level
                if ($_depth !== $depth)     continue;

                $array[$__path] = $this->extract($_path); //extract dropped element
                break;
            };

            //remove dropped element
            $this->del($_path);
        };

        //apply changes to current document
        foreach ($array AS $key => $value)
            $this->inject($key, $value);
    }

//--------------------------------------------------------------------------------------------------

    function shift($id,
                   $value)
    {
        if (($value = (int)$value) === 0)   return; //invalid shift value
        if ($this->template_index === NULL) return; //no structure

        $array  = []; //container elements
        $_array = []; //first level child elements

        //get involved elements from structure
        for ($i = 1, $c = count($this->structure); $i <= $c; $i++)
        {
            //add container element
            if (
                 streq($this->structure[$i][CMS_TEMPLATE_STRUCTURE_INDEX], $id)
                 &&
                 streq($this->structure[$i][CMS_TEMPLATE_STRUCTURE_TYPE], "shift")
               )
                $array[$i] = TRUE;

            //add first level child element
            if (isset($array[$this->structure[$i][CMS_TEMPLATE_STRUCTURE_PARENT]]))
                $_array[]  = $i;
        };

        $array = [];          //document extractions
        $sgn   = sgn($value); //sign of shift value
        $value = abs($value); //discard sign

        //process first level child elements
        foreach ($_array AS $key => $_value)
        {
            $path  = $this->structure[$_value][CMS_TEMPLATE_STRUCTURE_PATH]; //path of shifted element
            $type  = $this->structure[$_value][CMS_TEMPLATE_STRUCTURE_TYPE]; //type of shifted element
            $count = 0;                                                      //shift target count

            //find nth position of compatible type in shift direction
            for ($i = $key + $sgn; isset($_array[$i]); $i += $sgn)
            {
                $_i    = $_array[$i];                                        //dereference index
                $_path = $this->structure[$_i][CMS_TEMPLATE_STRUCTURE_PATH]; //path of target
                $_type = $this->structure[$_i][CMS_TEMPLATE_STRUCTURE_TYPE]; //type of target element

                //shifted element and target must be compatible
                if (nstreq($_type, $type)) continue;

                $count++; //increase shift target count

                //target may not be used more than once
                if (isset($array[$_path])) continue;

                if ($count >= $value) //target reached
                {
                    $array[$_path] = $this->extract($path); //extract shifted element
                    break;
                };
            };

            //remove shifted element
            $this->del($path);
        };

        //apply changes to current document
        foreach ($array AS $key => $value)
            $this->inject($key, $value);
    }

//--------------------------------------------------------------------------------------------------

    function del($path)
    {
        if ($this->template_index === NULL) return; //no structure

        //seek initial element
        for ($i = 1, $c = count($this->structure); $i <= $c; $i++)
        {
            if (streq($this->structure[$i][CMS_TEMPLATE_STRUCTURE_PATH], $path)) break; //found
        };

        if ($i > $c) return; //initial element not found

        $id     = $this->structure[$i][CMS_TEMPLATE_STRUCTURE_INDEX]; //initial element id
        $offset = $i;                                                 //initial element index

        //delete initial element data
        if (isset($this->data[$id])) unset($this->data[$id]);

        //no template or group
        if (! preg_match("/^(template|group)$/", $this->structure[$i][CMS_TEMPLATE_STRUCTURE_TYPE])) return;

        //delete element data in range
        for ($i++; $i <= $c; $i++)
        {
            //current element must be ancilliary to initial element
            if (($i > $offset) && ($this->structure[$i][CMS_TEMPLATE_STRUCTURE_PARENT] < $offset)) break;

            $id = $this->structure[$i][CMS_TEMPLATE_STRUCTURE_INDEX]; //current id

            //delete element data
            if (isset($this->data[$id])) unset($this->data[$id]);
        };
    }

//--------------------------------------------------------------------------------------------------

    function cleanup()
    {
        if ($this->template_index === NULL) return; //no structure

        //collect element ids and types from structure
        $id_list = NULL;

        for ($i = 1, $c = count($this->structure); $i <= $c; $i++)
        {
            $id           = $this->structure[$i][CMS_TEMPLATE_STRUCTURE_INDEX];
            $type         = $this->structure[$i][CMS_TEMPLATE_STRUCTURE_TYPE];
            $id_list[$id] = $type;
        };

        $data = NULL; //new data array

        //collect listed element data
        foreach ($this->data AS $key => $_)
        {
            if (
                 isset($id_list[$key])
                 &&
                 streq($id_list[$key], $this->data[$key][CMS_DOCUMENT_TYPE])
               )
                $data[$key] = $this->data[$key];
        };

        //apply clean element data
        $this->data = $data;
    }
};