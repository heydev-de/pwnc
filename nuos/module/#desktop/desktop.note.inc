<?php /*

   PWNC Web Platform
   Copyright © 2026–present Patrick Heyer
   https://pwnc.it

   This software is subject to the included license.
   Please see /LICENSE.md for full details.

*/

namespace cms;

//==================================================================================================
//   DESKTOP INTERFACE
//==================================================================================================

//==================================================================================================
//   MESSAGE HANDLING / SUB DISPLAY
//==================================================================================================

switch (CMS_IFC_MESSAGE)
{
case "save":
//..................................................................................................

    $desktop->object_set($object, "text", $ifc_param1);
    $ifc_response = $desktop->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
};

//==================================================================================================
//   MAIN DISPLAY
//==================================================================================================

//display
$ifc = new ifc($ifc_response,
               $ifc_page,
               [CMS_L_COMMAND_SAVE . "|desktop/command_save" => "save"],
               $param,
               NULL,
               CMS_L_DESKTOP_NOTE_001);

$ifc->set($desktop->object_get($object, "name"),
          "textarea 80x20 102400 f",
          $desktop->object_get($object, "text"));

$ifc->close();