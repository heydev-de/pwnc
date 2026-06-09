# PWNC API Documentation

[← Index](../../README.md) | [`module/#desktop/desktop.note.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23desktop/desktop.note.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Desktop Note Module (`desktop.note.inc`)

This file implements a **desktop note-taking interface** within the PWNC Web Platform. It provides a simple, persistent text editor for users to create, edit, and save notes directly from the desktop environment. The module integrates with PWNC's **Inter-Face Controller (IFC)** system to handle user input, display forms, and manage state transitions.

---

### **Purpose**
The module serves as a **sub-display handler** for the desktop environment, specifically managing:
- **Note persistence** – Saving and retrieving note content from the desktop object store.
- **User interaction** – Processing save commands via IFC and responding with success/error feedback.
- **Form rendering** – Displaying a text area for note editing with appropriate labels and constraints.

---

### **Core Logic Flow**
1. **Message Handling** (`CMS_IFC_MESSAGE` switch):
   - Processes the `save` command to persist note content.
2. **Main Display**:
   - Initializes an IFC form with a text area bound to the note's content.
   - Pre-fills the form with the note's current name and text.
   - Renders the form for user interaction.

---

### **Key Components**

#### **1. Message Handling (`save` case)**
Handles the `save` command triggered by the IFC form submission.

| **Parameter**      | **Type**       | **Description**                                                                 |
|--------------------|----------------|---------------------------------------------------------------------------------|
| `$object`          | `string`       | Identifier of the note object being edited (passed via IFC or URL parameters).  |
| `$ifc_param1`      | `string`       | Raw content submitted via the form (note text).                                |
| `$ifc_response`    | `string`       | Response code set to `CMS_MSG_DONE` (success) or `CMS_MSG_ERROR` (failure).    |

**Mechanism**:
- Uses `$desktop->object_set()` to update the note's `text` property with the submitted content.
- Calls `$desktop->save()` to persist changes to storage.
- Sets `$ifc_response` based on the success/failure of the save operation.

**Usage Context**:
- Triggered when the user submits the note-editing form.
- Relies on the desktop environment's object store for persistence.

---

#### **2. Main Display (IFC Form Initialization)**
Renders the note-editing interface using PWNC's IFC system.

| **Parameter**               | **Type**       | **Description**                                                                 |
|-----------------------------|----------------|---------------------------------------------------------------------------------|
| `$ifc_response`             | `string`       | Response code from the message handler (e.g., `CMS_MSG_DONE`).                 |
| `$ifc_page`                 | `string`       | Current page identifier (unused in this context).                              |
| **Command Array**           | `array`        | Maps the `save` command to the `desktop/command_save` endpoint.                |
| `$param`                    | `array`        | URL/query parameters (e.g., `object` identifier).                              |
| `NULL`                      | `null`         | Placeholder for additional IFC options (unused here).                          |
| `CMS_L_DESKTOP_NOTE_001`    | `string`       | Localized label for the form (e.g., "Edit Note").                              |

**Form Fields**:
- **Title Field**:
  - **Label**: Note name (retrieved via `$desktop->object_get($object, "name")`).
  - **Type**: Hidden or read-only (not editable in this form).
- **Text Area**:
  - **Label**: `CMS_L_DESKTOP_NOTE_001` (e.g., "Note Content").
  - **Attributes**: `textarea 80x20 102400 f` (80 columns, 20 rows, 102400-byte limit, full-width).
  - **Value**: Current note text (retrieved via `$desktop->object_get($object, "text")`).

**Mechanism**:
- The IFC form is configured to submit to the `desktop/command_save` endpoint with the `save` command.
- The text area is pre-populated with the note's existing content.
- The form is closed and rendered via `$ifc->close()`.

**Usage Context**:
- Displayed when the user opens a note for editing.
- Integrates with the desktop environment's object store to fetch/save note data.

---

### **Usage Example**
#### **Scenario: Editing a Desktop Note**
1. **Trigger**:
   - User clicks an "Edit" button for a note with `object = "note_123"`.
   - The desktop environment routes the request to `desktop.note.inc`.

2. **Code Execution**:
   ```php
   // Assume $param contains ['object' => 'note_123']
   $ifc = new ifc(
       $ifc_response,          // Response from prior save (or empty)
       $ifc_page,              // Current page (e.g., "desktop")
       [CMS_L_COMMAND_SAVE . "|desktop/command_save" => "save"],
       $param,
       NULL,
       CMS_L_DESKTOP_NOTE_001  // "Edit Note"
   );

   // Pre-fill the form with the note's name and text
   $ifc->set(
       $desktop->object_get("note_123", "name"),  // e.g., "Shopping List"
       "textarea 80x20 102400 f",
       $desktop->object_get("note_123", "text")   // e.g., "Milk, Eggs, Bread"
   );

   $ifc->close();  // Renders the form
   ```

3. **User Interaction**:
   - User edits the text and clicks "Save".
   - The IFC submits the form to `desktop/command_save` with `CMS_IFC_MESSAGE = "save"`.
   - The `save` case in the switch block processes the submission:
     ```php
     $desktop->object_set("note_123", "text", $_POST['textarea_field']);
     $ifc_response = $desktop->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
     ```

4. **Outcome**:
   - On success, the note is updated, and the form re-renders with `CMS_MSG_DONE`.
   - On failure, the form re-renders with `CMS_MSG_ERROR`.

---

### **Dependencies**
- **Desktop Environment**:
  - Relies on `$desktop` (global desktop object store) for note persistence.
  - Uses `$desktop->object_get()` and `$desktop->object_set()` to access/modify note data.
- **IFC System**:
  - Leverages PWNC's IFC for form rendering and command processing.
- **Localization**:
  - Uses `CMS_L_*` constants for labels (e.g., `CMS_L_DESKTOP_NOTE_001`).

---

### **Error Handling**
- **Save Failures**:
  - If `$desktop->save()` fails, `$ifc_response` is set to `CMS_MSG_ERROR`.
  - The form re-renders with the error state (e.g., red border or error message).
- **Missing Object**:
  - If `$object` is not provided, the form may render empty or trigger a 404.
  - Assumes the desktop environment validates object existence prior to routing.

---

### **Best Practices**
1. **Object Validation**:
   - Ensure the `$object` parameter is validated before use (e.g., check if the note exists).
2. **CSRF Protection**:
   - The IFC system includes built-in CSRF protection via `cms_param()` and `cms_url()`.
3. **Localization**:
   - Use `CMS_L_*` constants for all user-facing text to support multilingual interfaces.
4. **Performance**:
   - The desktop object store is optimized for frequent small updates (e.g., notes).
   - Avoid storing large binaries in notes (use `media://` or `download://` for files).


<!-- HASH:67043015d920e2b73976bd6f80027a88 -->
