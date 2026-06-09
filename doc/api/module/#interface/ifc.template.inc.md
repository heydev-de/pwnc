# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.template.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.template.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Template Interface (`ifc.template.inc`)

This file implements the **Template Management Interface** for the PWNC Web Platform. It provides a user interface for creating, editing, previewing, exporting, and deleting templates, as well as managing template categories and source code. The interface supports multilingual templates and integrates with other modules (e.g., content, image, media) for seamless asset insertion.

---

## Overview

### Purpose
- **Template Lifecycle Management**: Add, edit, delete, and preview templates.
- **Source Code Editing**: Modify HTML, CSS (stylesheets), and JavaScript (scripts) for templates.
- **Category Management**: Organize templates into categories and rename categories.
- **Content Export**: Export content as reusable templates.
- **Multilingual Support**: Manage templates across multiple languages.
- **Integration**: Insert assets (images, media, downloads) and tokens into templates via UI buttons.

### Key Features
- **Dual-Pane UI**: Left pane for template selection/category management; right pane for live preview.
- **Code Editor**: Syntax-highlighted editor for HTML, CSS, and JavaScript with prefab snippets.
- **Dynamic Preview**: Real-time preview of template changes.
- **Permission-Based Access**: Restricts actions based on user permissions (e.g., `CMS_L_OPERATOR`).

### Dependencies
- **`template` Library**: Core template management logic.
- **`ifc` Class**: Interface controller for form generation and handling.
- **`language` Utilities**: Multilingual support functions.
- **`cms_cache`**: Caching for template objects and categories.

---

## Constants and Variables

### Constants
| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_L_ACCESS` | System-defined | Minimum permission level to access the template interface. |
| `CMS_TEMPLATE_PERMISSION_OPERATOR` | System-defined | Permission key for operator-level template actions. |
| `CMS_IFC_MESSAGE` | Request parameter | Determines the action to perform (e.g., `add`, `edit`, `source`). |
| `CMS_IFC_SELECT` | Request parameter | Flag indicating if the interface is in selection mode. |
| `CMS_LANGUAGE_ENABLED` | System-defined | Comma-separated list of enabled languages. |

### Variables
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Current template identifier (multilingual-aware). |
| `$language` | `string` | Current language code (e.g., `en`, `de`). |
| `$category` | `string` | Current template category. |
| `$template` | `template` | Instance of the `template` class. |
| `$ifc_param` | `array` | Form parameters submitted via the interface. |
| `$ifc_response` | `string` | Response message (e.g., `CMS_MSG_DONE`, `CMS_MSG_ERROR`). |

---

## Message Handling

The interface processes actions via `CMS_IFC_MESSAGE`. Each case handles a specific workflow.

---

### `select`
**Purpose**: Updates the current template selection based on user input.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Template identifier to select. |
| `$ifc_param` | `string` | New template identifier from form input. |
| `$language` | `string` | Current language. |

**Return**: None (updates `$object` via `language_set`).
**Mechanism**:
- Uses `language_set` to update the multilingual template identifier.
- Caches the new selection for persistence.

**Usage**:
```php
// Switch to template "homepage" in English
$object = language_set($object, "homepage", "en");
```

---

### `select_language`
**Purpose**: Changes the current language for template management.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Language code (e.g., `de`). |

**Return**: None (updates `$language`).
**Mechanism**:
- Directly assigns the language code to `$language`.

**Usage**:
```php
// Switch to German
$language = "de";
```

---

### `display`
**Purpose**: Renders a template preview in an iframe.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Template identifier to preview. |

**Return**: None (exits script after rendering).
**Mechanism**:
- Caches the template object and category.
- Instantiates the `template` class and calls `template_preview` for rendering.

**Usage**:
```php
// Preview template "homepage"
template_preview("homepage");
exit();
```

---

### `_display`
**Purpose**: Internal preview handler (bypasses caching).
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Template identifier. |

**Return**: None (exits script).
**Mechanism**:
- Calls `template_preview` with `skip_cache=TRUE` and `skip_head=FALSE`.

---

### `add` / `edit`
**Purpose**: Displays forms for adding or editing a template.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Template identifier (for edit mode). |
| `$language` | `string` | Current language. |
| `$ifc_param1` | `string` | Template name (pre-filled in edit mode). |
| `$ifc_param2` | `bool` | Page flag (pre-filled in edit mode). |

**Return**: None (outputs HTML form).
**Mechanism**:
- Initializes the `template` class and retrieves multilingual data.
- Uses the `ifc` class to generate a form with fields for:
  - **Name**: Template name (text input).
  - **Page**: Checkbox to mark as a full page template.
  - **Code Source**: Radio buttons to choose between:
    - Empty template.
    - URL import.
    - File upload.

**Usage Example**:
```php
// Display edit form for template "homepage"
$ifc = new ifc($ifc_response, $ifc_page, TRUE,
    ["object" => "homepage", "language" => "en"],
    "_edit", CMS_L_COMMAND_EDIT);
$ifc->set(CMS_L_NAME, "text 40 40 bl", "Homepage");
$ifc->set(CMS_L_IFC_TEMPLATE_008, "checkbox b", TRUE, TRUE);
$ifc->close();
```

---

### `_add` / `_edit`
**Purpose**: Processes form submissions for adding/editing templates.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Template name. |
| `$ifc_param2` | `bool` | Page flag. |
| `$ifc_param3` | `int` | Code source (1=empty, 2=URL, 3=file). |
| `$ifc_param4` | `string` | URL (if source=2). |
| `$ifc_param5` | `string` | Category. |
| `$ifc_file1` | `string` | Uploaded file path (if source=3). |

**Return**: Updates `$ifc_response` with success/error status.
**Mechanism**:
- **Code Handling**:
  - **Empty**: Uses existing code (edit) or blank (add).
  - **URL**: Fetches code via `template_read_plugin`.
  - **File**: Reads uploaded file and deletes it afterward.
- **Template Creation/Update**:
  - **Add**: Calls `template->add`.
  - **Edit**: Calls `template->set` and `template->set_code`.
- **Multilingual Update**: Uses `language_set` to update identifiers.

**Usage Example**:
```php
// Add a new template from a URL
$template = new template();
$code = template_read_plugin("https://example.com/template.html");
$object = $template->add("New Template", "pages", TRUE, $code);
```

---

### `export`
**Purpose**: Displays a form to export content as a template.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$content_index` | `string` | Content identifier to export. |
| `$content_range` | `string` | Range of content items (optional). |

**Return**: None (outputs HTML form).
**Mechanism**:
- Checks if the `content` module is available.
- Generates a form with fields for:
  - **Name**: Template name.
  - **Category**: Dropdown of existing categories.

---

### `_export`
**Purpose**: Processes content export to a template.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Template name. |
| `$ifc_param2` | `string` | Category. |
| `$content_index` | `string` | Content identifier. |
| `$content_range` | `string` | Range of content items. |

**Return**: Updates `$ifc_response` with success/error status.
**Mechanism**:
- Uses `content_template_export` to generate template code, stylesheet, and JavaScript.
- Calls `template->add` to create the new template.

**Usage Example**:
```php
// Export content "blog_post" to a template
$buffer = content_template_export($content, "blog_post");
$template->add("Blog Post", "content", TRUE, $buffer["code"], $buffer["stylesheet"], $buffer["javascript"]);
```

---

### `source` / `_source` / `__source` / `___source`
**Purpose**: Manages template source code editing (HTML, CSS, JavaScript).
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Template identifier. |
| `$ifc_param1` | `string` | HTML code (for `_source` variants). |
| `$ifc_param2` | `string` | CSS stylesheet (for `_source` variants). |
| `$ifc_param3` | `string` | JavaScript (for `_source` variants). |

**Return**:
- For `_source`/`__source`/`___source`: Updates `$ifc_response` and exits.
- For `source`: Outputs the code editor UI.

**Mechanism**:
- **Code Editor UI**:
  - **Tabs**: HTML, CSS, JavaScript.
  - **Prefab Snippets**: Dropdown to insert common CMS tags (e.g., `<CMS:menu>`, `<CMS:image>`).
  - **Asset Integration**: Buttons to insert assets (images, media, downloads) via modal dialogs.
- **Save Actions**:
  - **Confirm**: Saves and closes the editor.
  - **Save**: Saves without closing.
  - **Show**: Saves and reloads the preview.

**Usage Example**:
```php
// Insert a prefab snippet (e.g., <CMS:menu>)
echo("<select onchange=\"template_source_insert(this.value);\">
    <option value=\"menu\">Menu</option>
</select>");
```

---

### `delete`
**Purpose**: Deletes selected templates.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$_object` | `array` | Array of template identifiers to delete. |

**Return**: Updates `$ifc_response` with success/error status.
**Mechanism**:
- Iterates over `$_object` and calls `template->delete` for each.
- Updates multilingual identifiers if a deleted template was selected.

**Usage Example**:
```php
// Delete templates "old_template" and "deprecated_template"
$template = new template();
foreach (["old_template", "deprecated_template"] as $id) {
    $template->delete($id);
}
```

---

### `category_rename`
**Purpose**: Displays a form to rename a template category.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Template identifier (to infer category). |

**Return**: None (outputs HTML form).
**Mechanism**:
- Retrieves the category of the selected template.
- Generates a form with a text input for the new category name.

---

### `_category_rename`
**Purpose**: Processes category renaming.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | New category name. |

**Return**: Updates `$ifc_response` with success status.
**Mechanism**:
- Iterates over all templates and updates the category if it matches the old name.

**Usage Example**:
```php
// Rename category "old_category" to "new_category"
$template->data->move("first");
while ($key = $template->data->move("next")) {
    if ($template->data->get($key, "category") === "old_category") {
        $template->data->set("new_category", $key, "category");
    }
}
$template->data->save();
```

---

## Main Display

### Purpose
Renders the primary template management interface with:
- **Category/Template Selection**: Left pane with dropdowns and checkboxes.
- **Live Preview**: Right pane with an iframe for template rendering.
- **Action Menu**: Buttons for add, edit, delete, etc.

### Mechanism
1. **Template Data Retrieval**:
   - Uses `template_get_array` to fetch all templates grouped by category.
   - Restores the last selected category/object from cache.
2. **UI Generation**:
   - **Category Dropdown**: Lists all categories.
   - **Template List**: Checkbox list of templates in the selected category.
   - **Language Selector**: Icons to switch languages (if multilingual support is enabled).
   - **Preview Iframe**: Displays the selected template.
3. **JavaScript**:
   - `template_select`: Updates the preview when a template is selected.
   - List utilities (`ifc_list_activate`, `ifc_list_invert`) for bulk selection.

### Usage Example
```php
// Render the main template interface
$ifc = new ifc($ifc_response, $ifc_page, $menu,
    ["object" => $object, "language" => $language]);
ifc_table_open();
echo("<td><select onchange=\"ifc_post('select',this.value);\">" .
    "<option value=\"pages\">Pages</option>" .
    "</select></td>");
echo("<td class=\"iframe\"><iframe id=\"template-display\" " .
    "src=\"" . x(cms_url(["ifc_page" => CMS_IFC_PAGE, "ifc_message" => "display", "object" => $object])) . "\"></iframe></td>");
ifc_table_close();
$ifc->close();
```

---

## Helper Functions

### `template_get_select()`
**Purpose**: Generates a dropdown of template categories for forms.
**Return**: `string` HTML `<select>` element.
**Usage**:
```php
$ifc->set(template_get_select(), "list 40", "pages");
```

### `template_get_array($filter = NULL)`
**Purpose**: Retrieves all templates grouped by category.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$filter` | `bool` | If `FALSE`, returns only page templates. If `NULL`, returns all. |

**Return**: `array` Associative array of categories to template arrays.
**Usage**:
```php
$templates = template_get_array();
foreach ($templates["pages"] as $id => $name) {
    echo("<option value=\"$id\">$name</option>");
}
```

---

## Integration with Other Modules

The template interface integrates with other PWNC modules via modal dialogs and JavaScript callbacks:

| Module | Purpose | Example Usage |
|--------|---------|---------------|
| **Directory** | Insert links to directory items. | `<CMS:href default="directory://item_id">Link</CMS:href>` |
| **Image** | Insert images. | `<CMS:image default="image://image_id" alt="Example"/>` |
| **Media** | Insert media files. | `<CMS:media default="media://media_id"/>` |
| **Download** | Insert download links. | `<CMS:download default="download://file_id">Download</CMS:download>` |
| **Token** | Insert tokens (e.g., CSRF). | `%token%` |

**Example**:
```javascript
// Insert an image into the template code
buffer = "javascript:this.opener.textcontrol_set('#l_ifc_param1', '#insert', '<CMS:image default=\"%return%\" title=\"%title%\"/>');";
```

---


<!-- HASH:75aaeafafc08521b49953755ef077d38 -->
