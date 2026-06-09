# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.ifc.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.ifc.inc)

- **Version:** `26.6.9.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## IFC Class and Utility Functions

The `lib.ifc.inc` file provides the **Interface Control (IFC)** system for the PWNC Web Platform. It handles modal dialogs, form generation, and user interaction within the CMS interface. The IFC system is responsible for:

- Rendering modal dialogs with configurable menus, titles, and content
- Generating form elements (inputs, textareas, selects, etc.) with consistent styling
- Managing interface permissions and navigation
- Supporting multilingual input fields
- Handling external window interactions and return values

---

## Global Variables and Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `$ifc_message` | - | Control message for IFC operations |
| `$ifc_page` | - | Currently selected IFC page |
| `$ifc_option` | - | External window options (e.g., `"external"`) |
| `$ifc_select` | - | Field name for return value in external windows |
| `$ifc_select_action` | - | URL or JavaScript action for return value handling |
| `$ifc_response` | - | Response message (e.g., `CMS_MSG_DONE`, `CMS_MSG_ERROR`) |
| `$ifc_param` | - | Default parameter for IFC forms |

| Constant | Value | Description |
|----------|-------|-------------|
| `CMS_IFC_MESSAGE` | `$ifc_message` | Alias for `$ifc_message` |
| `CMS_IFC_PAGE` | `CMS_INSTANCE` | Alias for the current IFC page |
| `CMS_IFC_OPTION` | `$ifc_option` | Alias for external window options |
| `CMS_IFC_SELECT` | `$ifc_select` | Alias for return value field name |
| `CMS_IFC_SELECT_ACTION` | `$ifc_select_action` | Alias for return action URL/JS |
| `CMS_IFC_INPUT_PLACEHOLDER` | `CMS_L_IFC_012 . " …"` | Default placeholder text for inputs |

---

## Utility Functions

### `ifc_permission($array = NULL)`
Manages interface permissions for the current user.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$array` | `array\|null` | Associative array of permissions. If `NULL`, returns current permissions. |

**Return Values:**
- `array\|null`: Current permissions if `$array` is `NULL`, otherwise `NULL`.

**Inner Mechanisms:**
- Uses a static variable to store permissions.
- If `$array` is provided, validates it and updates the static store.

**Usage Example:**
```php
// Set permissions
ifc_permission(["content" => "Content Management", "image" => "Image Management"]);

// Get permissions
$permissions = ifc_permission();
```

---

### `ifc_available($module)`
Checks if an IFC module is available.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$module` | `string` | Module name (e.g., `"image"`, `"content"`). |

**Return Values:**
- `bool`: `TRUE` if the module exists, `FALSE` otherwise.

**Inner Mechanisms:**
- Checks for the existence of the module file in `CMS_MODULES_PATH . "#interface/ifc.$module.inc"`.

**Usage Example:**
```php
if (ifc_available("image")) {
    // Load image interface
}
```

---

### `ifc_default($ifc_page = NULL)`
Renders the default IFC page (dashboard).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$ifc_page` | `array\|null` | Optional IFC page configuration. |

**Inner Mechanisms:**
- Sets default permissions.
- Displays system information (version, PHP details, license).
- Checks for updates if the user has permission.

**Usage Example:**
```php
ifc_default(); // Renders the default dashboard
```

---

### `ifc_inactive($ifc_page = NULL)`
Renders an "inactive" IFC page (e.g., for unauthorized access).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$ifc_page` | `array\|null` | Optional IFC page configuration. |

**Usage Example:**
```php
ifc_inactive(); // Shows "inactive" message
```

---

### `ifc_table_open($class = NULL)`
Opens an HTML table with optional CSS class.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$class` | `string\|null` | CSS class for the table. |

**Usage Example:**
```php
ifc_table_open("data-table");
```

---

### `ifc_table_close()`
Closes an HTML table.

**Usage Example:**
```php
ifc_table_close();
```

---

### `ifc_tab_open($label, $command = NULL)`
Opens a tabbed interface section.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$label` | `string` | Tab label (supports icons via `|icon.svg`). |
| `$command` | `string\|null` | Command (`"next"` or `"close"`). Defaults to opening a new tab. |

**Inner Mechanisms:**
- Uses static variables to track tab levels and counts.
- Generates radio inputs for tab selection.

**Usage Example:**
```php
ifc_tab_open("General Settings");
ifc_set("Site Name", "text 30");
ifc_tab_next("Advanced Settings");
ifc_set("Cache Enabled", "checkbox");
ifc_tab_close();
```

---

### `ifc_tab_next($label)`
Moves to the next tab in a tabbed interface.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$label` | `string` | Label for the next tab. |

**Usage Example:**
```php
ifc_tab_next("SEO Settings");
```

---

### `ifc_tab_close()`
Closes the current tabbed interface section.

**Usage Example:**
```php
ifc_tab_close();
```

---

### `ifc_popover_open($label)`
Opens a popover element.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$label` | `string` | Popover trigger label. |

**Inner Mechanisms:**
- Uses the HTML `popover` API.
- Generates a close button inside the popover.

**Usage Example:**
```php
ifc_popover_open("Help");
echo("This is a help popover.");
ifc_popover_close();
```

---

### `ifc_popover_close()`
Closes a popover element.

**Usage Example:**
```php
ifc_popover_close();
```

---

### `ifc_close_external()`
Closes an external IFC window and terminates script execution.

**Inner Mechanisms:**
- Outputs a minimal HTML document with a JavaScript `close()` call.

**Usage Example:**
```php
ifc_close_external(); // Closes the current window
```

---

### `ifc_varied($option = NULL, $index = 0)`
Generates varied CSS class names (e.g., for alternating row colors).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$option` | `string\|null` | Base class name. |
| `$index` | `int` | Index for variation. |

**Return Values:**
- `string`: Varied class name (e.g., `"varied-0"`, `"varied-1"`).

**Usage Example:**
```php
$class = ifc_varied("row", 0); // Returns "row-0"
```

---

### `ifc_parse_label($text)`
Parses a label string to extract text, icon, and URL.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Label text (supports `|icon.svg` for icons). |

**Return Values:**
- `array`: `[text, url, formatted_label, img_html]`.

**Inner Mechanisms:**
- Supports icons via `|icon.svg` syntax.
- Escapes special characters (e.g., `\|` becomes `|`).

**Usage Example:**
```php
[$text, $url, $label, $img] = ifc_parse_label("Settings|settings.svg");
```

---

## `ifc` Class

### Constructor: `__construct($ifc_response = NULL, $ifc_page = NULL, $menu = TRUE, $param = NULL, $message = NULL, $subpage = NULL, $content_container_id = NULL)`
Initializes an IFC dialog.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$ifc_response` | `string\|null` | Response message (e.g., success/error). |
| `$ifc_page` | `array\|string\|null` | IFC page configuration or page name. |
| `$menu` | `bool\|array` | Menu configuration (`TRUE` for default, `FALSE` for no menu, or custom array). |
| `$param` | `array\|string\|null` | Default parameters for the form. |
| `$message` | `string\|null` | Additional message. |
| `$subpage` | `string\|null` | Subpage title. |
| `$content_container_id` | `string\|null` | ID for the content container. |

**Inner Mechanisms:**
- Sets up the HTML structure, including form, head, and body.
- Restores scroll position if returning to the same page.
- Generates navigation menus and language selectors.

**Usage Example:**
```php
$ifc = new ifc("Settings saved!", "settings", [
    "General" => "settings/general",
    "Advanced" => "settings/advanced"
]);
```

---

### `param($param, $value = NULL)`
Adds hidden form parameters.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$param` | `string\|array` | Parameter name or associative array of parameters. |
| `$value` | `mixed` | Parameter value (if `$param` is a string). |

**Return Values:**
- `int`: Number of parameters added.

**Inner Mechanisms:**
- Supports nested arrays (e.g., `param("user[name]", "John")`).

**Usage Example:**
```php
$ifc->param("action", "save");
$ifc->param(["id" => 123, "token" => "abc"]);
```

---

### `set($text = NULL, $type = "new text 40 60 b", $value = NULL, $checked = NULL, $name = NULL, $language = NULL)`
Generates a form element.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string\|null` | Label text (supports `|icon.svg`). |
| `$type` | `string` | Element type and settings (e.g., `"text 30"`, `"checkbox"`). |
| `$value` | `mixed` | Default value. |
| `$checked` | `bool\|null` | Whether a checkbox/radio is checked. |
| `$name` | `string\|null` | Element name (auto-generated if `NULL`). |
| `$language` | `string\|null` | Comma-separated list of languages for multilingual fields. |

**Type Syntax:**
- `new`: Reset element counter.
- `text 30`: Input type and size (e.g., `30` characters wide).
- `b`: Line break after element.
- `c`: Checked by default.
- `d`: Disabled.
- `f`: Full size.
- `l`: Language support.
- `n`: No word wrap.
- `w`: Full width.

**Supported Types:**
- `button`, `text`, `password`, `date`, `datetime`, `file`, `multifile`, `texteditor`, `textarea`, `code`, `code_html`, `code_php`, `code_style`, `code_script`, `checkbox`, `select`, `multiselect`, `list`, `radio`, `title`, `label`, `description`, `info`, `dummy`.

**Usage Example:**
```php
$ifc->set("Username", "text 30", "admin");
$ifc->set("Password", "password 30");
$ifc->set("Enabled", "checkbox", 1, TRUE);
$ifc->set("Language", "select 10", "en", FALSE, NULL, "en,de,fr");
```

---

### `dummy($count = 1)`
Skips form elements (e.g., for alignment).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$count` | `int` | Number of elements to skip. |

**Usage Example:**
```php
$ifc->set("Left Column", "text 20");
$ifc->dummy(2); // Skip 2 columns
$ifc->set("Right Column", "text 20");
```

---

### `close()`
Closes the IFC dialog and terminates script execution.

**Inner Mechanisms:**
- Outputs closing HTML tags.
- Handles return values for external windows.

**Usage Example:**
```php
$ifc->close(); // Renders the dialog and exits
```


<!-- HASH:007ca82d995491d769e93e8d603b1998 -->
