# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.template.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.template.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Template System Overview

The `lib.template.inc` file provides the core template engine for the PWNC Web Platform. It enables the creation, management, and rendering of reusable HTML templates with dynamic content injection, conditional logic, and structural organization. The system supports multilingual templates, asset management (stylesheets, JavaScript), and an intuitive editing interface for content manipulation.

The template engine processes custom XML-like tags (`<CMS:*>`) to generate HTML output. It handles data binding, conditional rendering, loops, and nested templates while maintaining a clean separation between content and presentation.

---

## Constants

### Permission Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_PERMISSION_OPERATOR` | `"operator"` | Permission identifier for template management. |

### Template Cache Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_CACHE_SEPARATOR` | `"\x1C"` | ASCII file separator used to delimit cached and dynamic content. |

### Option Flag Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_OPTION_NONE` | `0` | No options enabled. |
| `CMS_TEMPLATE_OPTION_HREF` | `1` | Enable href elements. |
| `CMS_TEMPLATE_OPTION_PLUGIN` | `2` | Enable plugin elements. |
| `CMS_TEMPLATE_OPTION_TEXT` | `4` | Enable text elements. |
| `CMS_TEMPLATE_OPTION_VALUE` | `8` | Enable value elements. |
| `CMS_TEMPLATE_OPTION_DOWNLOAD` | `16` | Enable download elements. |
| `CMS_TEMPLATE_OPTION_IMAGE` | `32` | Enable image elements. |
| `CMS_TEMPLATE_OPTION_THUMBNAIL` | `64` | Enable thumbnail elements. |
| `CMS_TEMPLATE_OPTION_MEDIA` | `128` | Enable media elements. |
| `CMS_TEMPLATE_OPTION_TEMPLATE` | `256` | Enable template elements. |
| `CMS_TEMPLATE_OPTION_GROUP` | `512` | Enable group elements. |
| `CMS_TEMPLATE_OPTION_REPEAT` | `1024` | Enable repeat elements. |
| `CMS_TEMPLATE_OPTION_SHIFT` | `2048` | Enable shift elements. |
| `CMS_TEMPLATE_OPTION_CALT` | `4096` | Enable conditional alternative elements. |
| `CMS_TEMPLATE_OPTION_CBLOCK` | `8192` | Enable conditional block elements. |
| `CMS_TEMPLATE_OPTION_DEBUG` | `16384` | Enable debug mode. |
| `CMS_TEMPLATE_OPTION_SWITCH` | `32768` | Enable switch elements. |
| `CMS_TEMPLATE_OPTION_LAYOUT` | `CMS_TEMPLATE_OPTION_VALUE \| CMS_TEMPLATE_OPTION_TEMPLATE \| CMS_TEMPLATE_OPTION_GROUP \| CMS_TEMPLATE_OPTION_REPEAT \| CMS_TEMPLATE_OPTION_SHIFT \| CMS_TEMPLATE_OPTION_CBLOCK \| CMS_TEMPLATE_OPTION_SWITCH` | Layout-related options. |
| `CMS_TEMPLATE_OPTION_EDIT` | `CMS_TEMPLATE_OPTION_HREF \| CMS_TEMPLATE_OPTION_PLUGIN \| CMS_TEMPLATE_OPTION_TEXT \| CMS_TEMPLATE_OPTION_DOWNLOAD \| CMS_TEMPLATE_OPTION_IMAGE \| CMS_TEMPLATE_OPTION_THUMBNAIL \| CMS_TEMPLATE_OPTION_MEDIA \| CMS_TEMPLATE_OPTION_CBLOCK` | Edit-related options. |
| `CMS_TEMPLATE_OPTION_ALL` | `CMS_TEMPLATE_OPTION_LAYOUT \| CMS_TEMPLATE_OPTION_EDIT` | All options enabled. |

### Action Type Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_ACTION` | `0` | Action type identifier. |
| `CMS_TEMPLATE_CONTROL` | `1` | Control type identifier. |
| `CMS_TEMPLATE_CODE` | `2` | Code type identifier. |
| `CMS_TEMPLATE_IMAGE` | `3` | Image type identifier. |
| `CMS_TEMPLATE_COMMAND` | `4` | Command type identifier. |
| `CMS_TEMPLATE_SWITCH` | `5` | Switch type identifier. |

### Template Type Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_TYPE_NONE` | `0` | No type. |
| `CMS_TEMPLATE_TYPE_HEAD` | `1` | Head element. |
| `CMS_TEMPLATE_TYPE_HREF` | `2` | Hyperlink element. |
| `CMS_TEMPLATE_TYPE_PLUGIN` | `4` | Plugin element. |
| `CMS_TEMPLATE_TYPE_TEXT` | `8` | Text element. |
| `CMS_TEMPLATE_TYPE_VALUE` | `16` | Value element. |
| `CMS_TEMPLATE_TYPE_DOWNLOAD` | `32` | Download element. |
| `CMS_TEMPLATE_TYPE_IMAGE` | `64` | Image element. |
| `CMS_TEMPLATE_TYPE_THUMBNAIL` | `128` | Thumbnail element. |
| `CMS_TEMPLATE_TYPE_MEDIA` | `256` | Media element. |
| `CMS_TEMPLATE_TYPE_TEMPLATE` | `512` | Template element. |
| `CMS_TEMPLATE_TYPE_GROUP` | `1024` | Group element. |
| `CMS_TEMPLATE_TYPE_REPEAT` | `2048` | Repeat element. |
| `CMS_TEMPLATE_TYPE_SHIFT` | `4096` | Shift element. |
| `CMS_TEMPLATE_TYPE_MENU` | `8192` | Menu element. |
| `CMS_TEMPLATE_TYPE_CBLOCK` | `16384` | Conditional block element. |
| `CMS_TEMPLATE_TYPE_CALT` | `32768` | Conditional alternative element. |
| `CMS_TEMPLATE_TYPE_BASE` | `65536` | Base element. |
| `CMS_TEMPLATE_TYPE_NAMESPACE` | `131072` | Namespace element. |
| `CMS_TEMPLATE_TYPE_NOCACHE` | `262144` | No-cache element. |
| `CMS_TEMPLATE_TYPE_CONTROL` | `524288` | Control element. |
| `CMS_TEMPLATE_TYPE_BACKLINK` | `1048576` | Backlink element. |
| `CMS_TEMPLATE_TYPE_DEBUG` | `2097152` | Debug element. |
| `CMS_TEMPLATE_TYPE_STYLESHEET` | `4194304` | Stylesheet element. |
| `CMS_TEMPLATE_TYPE_SWITCH` | `8388608` | Switch element. |
| `CMS_TEMPLATE_TYPE_CEDIT` | `16777216` | Editable block element. |
| `CMS_TEMPLATE_TYPE_CNOEDIT` | `33554432` | Non-editable block element. |
| `CMS_TEMPLATE_TYPE_JAVASCRIPT` | `67108864` | JavaScript element. |
| `CMS_TEMPLATE_TYPE_ALL` | `4294967295` | All types enabled. |

### Template Type Filter Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_TYPE_EDIT` | `CMS_TEMPLATE_TYPE_HREF \| CMS_TEMPLATE_TYPE_PLUGIN \| CMS_TEMPLATE_TYPE_TEXT \| CMS_TEMPLATE_TYPE_VALUE \| CMS_TEMPLATE_TYPE_DOWNLOAD \| CMS_TEMPLATE_TYPE_IMAGE \| CMS_TEMPLATE_TYPE_THUMBNAIL \| CMS_TEMPLATE_TYPE_MEDIA \| CMS_TEMPLATE_TYPE_TEMPLATE \| CMS_TEMPLATE_TYPE_GROUP \| CMS_TEMPLATE_TYPE_REPEAT \| CMS_TEMPLATE_TYPE_SHIFT \| CMS_TEMPLATE_TYPE_SWITCH` | Editable element types. |
| `CMS_TEMPLATE_TYPE_SPAN` | `CMS_TEMPLATE_TYPE_HREF \| CMS_TEMPLATE_TYPE_DOWNLOAD \| CMS_TEMPLATE_TYPE_GROUP \| CMS_TEMPLATE_TYPE_REPEAT \| CMS_TEMPLATE_TYPE_SHIFT \| CMS_TEMPLATE_TYPE_CBLOCK \| CMS_TEMPLATE_TYPE_CALT \| CMS_TEMPLATE_TYPE_BASE \| CMS_TEMPLATE_TYPE_NAMESPACE \| CMS_TEMPLATE_TYPE_NOCACHE \| CMS_TEMPLATE_TYPE_CEDIT \| CMS_TEMPLATE_TYPE_CNOEDIT` | Spannable element types. |
| `CMS_TEMPLATE_TYPE_PATH` | `CMS_TEMPLATE_TYPE_TEMPLATE \| CMS_TEMPLATE_TYPE_GROUP \| CMS_TEMPLATE_TYPE_REPEAT \| CMS_TEMPLATE_TYPE_SHIFT \| CMS_TEMPLATE_TYPE_BASE \| CMS_TEMPLATE_TYPE_NAMESPACE` | Path-extending element types. |

### Command Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_COMMAND_NONE` | `0` | No command. |
| `CMS_TEMPLATE_COMMAND_BUFFER` | `1` | Buffer command. |
| `CMS_TEMPLATE_COMMAND_PASTE` | `2` | Paste command. |
| `CMS_TEMPLATE_COMMAND_SWAP` | `4` | Swap command. |
| `CMS_TEMPLATE_COMMAND_KICK1` | `8` | Kick1 command. |
| `CMS_TEMPLATE_COMMAND_KICK2` | `16` | Kick2 command. |
| `CMS_TEMPLATE_COMMAND_DROP1` | `32` | Drop1 command. |
| `CMS_TEMPLATE_COMMAND_DROP2` | `64` | Drop2 command. |
| `CMS_TEMPLATE_COMMAND_RELEASE` | `128` | Release command. |
| `CMS_TEMPLATE_COMMAND_REFERENCE` | `256` | Reference command. |
| `CMS_TEMPLATE_COMMAND_EXPORT` | `512` | Export command. |
| `CMS_TEMPLATE_COMMAND_CLEAR` | `1024` | Clear command. |
| `CMS_TEMPLATE_COMMAND_DRAGDROP1` | `2048` | Drag-and-drop command 1. |
| `CMS_TEMPLATE_COMMAND_DRAGDROP2` | `4096` | Drag-and-drop command 2. |
| `CMS_TEMPLATE_COMMAND_ALL` | `4294967295` | All commands enabled. |

### Structure Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_STRUCTURE_INDEX` | `0` | Index of the element in the structure array. |
| `CMS_TEMPLATE_STRUCTURE_PATH` | `1` | Path identifier of the element. |
| `CMS_TEMPLATE_STRUCTURE_PARENT` | `2` | Parent identifier of the element. |
| `CMS_TEMPLATE_STRUCTURE_TYPE` | `3` | Type of the element. |

### Asset Type Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_TEMPLATE_ASSET_TYPE_CODE` | `1` | Template code asset. |
| `CMS_TEMPLATE_ASSET_TYPE_STYLESHEET` | `2` | Stylesheet asset. |
| `CMS_TEMPLATE_ASSET_TYPE_JAVASCRIPT` | `3` | JavaScript asset. |

---

## Utility Functions

### `template_get_array`

**Purpose:**
Retrieves a list of templates grouped by category for use in selection interfaces.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$page` | `bool\|NULL` | Filter templates by page status. `TRUE` for page templates, `FALSE` for modular templates, `NULL` for all. |

**Return Values:**
- `array`: Associative array of templates grouped by category.

**Inner Mechanisms:**
- Queries the `#system/template` data source.
- Groups templates by their `category` field.
- Uses localized names if available; falls back to the template index.
- Sorts categories and template names naturally and case-insensitively.

**Usage Context:**
Used to populate dropdown menus or selection lists in the admin interface.

**Example:**
```php
$templates = template_get_array();
foreach ($templates as $category => $items) {
    echo "<optgroup label=\"" . x($category) . "\">";
    foreach ($items as $name => $index) {
        echo "<option value=\"" . x($index) . "\">" . x($name) . "</option>";
    }
    echo "</optgroup>";
}
```

---

### `template_get_select`

**Purpose:**
Retrieves a list of template categories for use in selection interfaces.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$page` | `bool\|NULL` | Filter templates by page status. `TRUE` for page templates, `FALSE` for modular templates, `NULL` for all. |

**Return Values:**
- `array`: Associative array of categories with empty values.

**Inner Mechanisms:**
- Queries the `#system/template` data source.
- Extracts unique categories from templates.
- Sorts categories naturally and case-insensitively.

**Usage Context:**
Used to populate category selection dropdowns in the admin interface.

**Example:**
```php
$categories = template_get_select();
echo "<select name=\"category\">";
foreach ($categories as $value => $label) {
    echo "<option value=\"" . x($value) . "\">" . x($label) . "</option>";
}
echo "</select>";
```

---

### `template_get_attribute`

**Purpose:**
Extracts attributes from an HTML/XML tag string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | The tag string to parse. |
| `$name` | `string\|NULL` | Attribute name to retrieve. If `NULL`, all attributes are returned. |

**Return Values:**
- `array\|string\|NULL`: Associative array of all attributes, the value of the specified attribute, or `NULL` if parsing fails.

**Inner Mechanisms:**
- Uses regular expressions to parse attribute names and values.
- Handles quoted and unquoted attribute values.
- Decodes HTML entities in attribute values.

**Usage Context:**
Used internally to parse template tags and extract attributes.

**Example:**
```php
$tag = '<CMS:text id="header" default="Welcome" title="Page Header"/>';
$attributes = template_get_attribute($tag);
echo $attributes["id"]; // Outputs: header
echo $attributes["default"]; // Outputs: Welcome
```

---

### `template_set_attribute`

**Purpose:**
Adds or updates an attribute in an HTML/XML tag string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | The tag string to modify. |
| `$name` | `string` | The attribute name to set. |
| `$value` | `string` | The attribute value to set. |

**Return Values:**
- `string`: The modified tag string.

**Inner Mechanisms:**
- Removes any existing attribute with the same name.
- Appends the new attribute to the opening tag.

**Usage Context:**
Used internally to modify template tags during processing.

**Example:**
```php
$tag = '<CMS:text id="header"/>';
$tag = template_set_attribute($tag, "default", "Welcome");
echo $tag; // Outputs: <CMS:text id="header" default="Welcome"/>
```

---

### `template_remove_attribute`

**Purpose:**
Removes an attribute from an HTML/XML tag string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | The tag string to modify. |
| `$name` | `string\|NULL` | The attribute name to remove. If `NULL`, all attributes are removed. |

**Return Values:**
- `string`: The modified tag string.

**Inner Mechanisms:**
- Uses regular expressions to remove the specified attribute.

**Usage Context:**
Used internally to clean up template tags.

**Example:**
```php
$tag = '<CMS:text id="header" default="Welcome"/>';
$tag = template_remove_attribute($tag, "default");
echo $tag; // Outputs: <CMS:text id="header"/>
```

---

### `template_parse_reference`

**Purpose:**
Parses a reference string (e.g., `content://123`, `directory://456`) into a structured array with URL, name, and description.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The reference string to parse. |
| `$analyze` | `bool` | If `TRUE`, returns the analyzed URL components instead of the processed reference. |

**Return Values:**
- `array\|FALSE`: Associative array with keys `name`, `description`, and `url`, or `FALSE` if parsing fails.

**Inner Mechanisms:**
- Uses `analyze_url` to break down the reference.
- Supports `content://`, `directory://`, and `address:` schemes.
- Retrieves localized names and descriptions from the database.

**Usage Context:**
Used to resolve references in href, image, and download elements.

**Example:**
```php
$reference = template_parse_reference("content://123");
echo $reference["url"]; // Outputs the resolved URL
echo $reference["name"]; // Outputs the content title
```

---

### `template_read_plugin`

**Purpose:**
Fetches content from a remote URL for use in plugin elements.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$url` | `string` | The URL to fetch. |

**Return Values:**
- `string\|FALSE`: The fetched content or `FALSE` on failure.

**Inner Mechanisms:**
- Uses the `http` library to fetch remote content.

**Usage Context:**
Used to embed external content via `<CMS:plugin>`.

**Example:**
```php
$content = template_read_plugin("https://example.com/plugin");
if ($content !== FALSE) {
    echo $content;
}
```

---

### `template_preview`

**Purpose:**
Generates a preview of a template for the admin interface.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index_or_code` | `string` | Template index or raw template code. |
| `$is_index` | `bool` | If `TRUE`, `$index_or_code` is treated as a template index. If `FALSE`, it is treated as raw code. |
| `$document` | `document\|NULL` | The document object to use. If `NULL`, a new one is created. |
| `$inert` | `bool` | If `TRUE`, wraps the output in an inert preview container. |

**Return Values:**
- `void`: Outputs the preview directly.

**Inner Mechanisms:**
- Defines dummy actions for editable elements.
- Uses the template engine to parse the template.
- Wraps the output in a basic HTML structure with necessary stylesheets and scripts.

**Usage Context:**
Used in the admin interface to preview templates before saving.

**Example:**
```php
template_preview("header_template", TRUE);
```

---

### `template_error`

**Purpose:**
Generates a standardized error message for template-related errors.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$code` | `int` | Error code. |
| `$message` | `string` | Error message. |
| `$path` | `string\|NULL` | File path where the error occurred. |
| `$line` | `int\|NULL` | Line number where the error occurred. |

**Return Values:**
- `string`: The formatted error message.

**Inner Mechanisms:**
- Uses `cms_error` to generate the error message.
- Includes template name and file path if available.

**Usage Context:**
Used internally to handle template parsing and execution errors.

**Example:**
```php
template_error(E_USER_ERROR, "Invalid template syntax", NULL, 42);
```

---

## `template` Class

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$data` | `data` | Data object for template metadata. |
| `$operator` | `bool` | Whether the current user has operator permissions. |
| `$compat_mode` | `bool` | Whether compatibility mode is enabled for older templates. |
| `$tlist` | `array` | Mapping of template tag names to type constants. |
| `$tname` | `array` | Mapping of type constants to localized names. |
| `$toption` | `array` | Mapping of type constants to option flags. |
| `$action` | `array\|NULL` | Action definitions for editable elements. |
| `$image` | `image\|NULL` | Image library instance. |
| `$media` | `media\|NULL` | Media library instance. |
| `$download` | `download\|NULL` | Download library instance. |
| `$structure_id` | `int\|NULL` | Current structure index during parsing. |
| `$parent_id` | `int\|NULL` | Current parent index during parsing. |
| `$execute_vars` | `array` | Variables for template execution. |
| `$title` | `string` | Document title. |
| `$description` | `string` | Document description. |
| `$keyword` | `string` | Document keywords. |
| `$header` | `string` | Additional header content. |
| `$query_data` | `array` | Query string data. |

---

### `__construct`

**Purpose:**
Initializes the template engine.

**Parameters:**
None.

**Return Values:**
None.

**Inner Mechanisms:**
- Initializes the `#system/template` data source.
- Checks operator permissions.
- Determines compatibility mode based on system settings.
- Creates necessary directories for template assets.

**Usage Context:**
Called when creating a new `template` instance.

**Example:**
```php
$template = new template();
```

---

### `add`

**Purpose:**
Adds a new template to the system.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Template name. |
| `$category` | `string\|NULL` | Template category. |
| `$page` | `bool\|NULL` | Whether the template is a page template. |
| `$code` | `string\|NULL` | Template code. |
| `$stylesheet` | `string\|NULL` | Stylesheet code. |
| `$javascript` | `string\|NULL` | JavaScript code. |

**Return Values:**
- `string\|FALSE`: The new template index or `FALSE` on failure.

**Inner Mechanisms:**
- Validates operator permissions.
- Sets a default name if none is provided.
- Inserts the template metadata into the data source.
- Saves the template code, stylesheet, and JavaScript assets.

**Usage Context:**
Used in the admin interface to create new templates.

**Example:**
```php
$index = $template->add("Header", "Layout", TRUE, "<CMS:head/>");
if ($index !== FALSE) {
    echo "Template created with index: $index";
}
```

---

### `set`

**Purpose:**
Updates an existing template's metadata.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index. |
| `$name` | `string\|NULL` | New template name. |
| `$category` | `string\|NULL` | New template category. |
| `$page` | `bool\|NULL` | New page template status. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates operator permissions.
- Updates the template metadata in the data source.

**Usage Context:**
Used in the admin interface to edit template properties.

**Example:**
```php
if ($template->set("header_template", "New Header", "Layout", TRUE)) {
    echo "Template updated successfully.";
}
```

---

### `get`

**Purpose:**
Retrieves metadata for a template.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index. |

**Return Values:**
- `array`: Associative array of template metadata.

**Inner Mechanisms:**
- Retrieves the template record from the data source.

**Usage Context:**
Used to display template information in the admin interface.

**Example:**
```php
$metadata = $template->get("header_template");
echo "Template Name: " . x($metadata["name"]);
```

---

### `asset_path`

**Purpose:**
Generates the file path for a template asset.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$type` | `int` | Asset type constant (`CMS_TEMPLATE_ASSET_TYPE_*`). |

**Return Values:**
- `string\|FALSE`: The file path pattern or `FALSE` on failure.

**Inner Mechanisms:**
- Returns a sprintf pattern for the asset path based on type.

**Usage Context:**
Used internally to manage template assets.

**Example:**
```php
$path = $template->asset_path(CMS_TEMPLATE_ASSET_TYPE_CODE);
echo CMS_DATA_PATH . sprintf($path, "", "header_template"); // Outputs: /path/to/data/#template/header_template.htm
```

---

### `set_asset`

**Purpose:**
Saves a template asset (code, stylesheet, or JavaScript) to disk.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index. |
| `$text` | `string` | Asset content. Passed by reference. |
| `$type` | `int` | Asset type constant (`CMS_TEMPLATE_ASSET_TYPE_*`). |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Retrieves localized versions of the asset.
- Writes each localized version to a separate file.
- Deletes files if the content is empty.

**Usage Context:**
Used internally to save template assets.

**Example:**
```php
$template->set_asset("header_template", "<CMS:head/>", CMS_TEMPLATE_ASSET_TYPE_CODE);
```

---

### `get_asset`

**Purpose:**
Retrieves a template asset from disk.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index. |
| `$language` | `string\|NULL\|FALSE` | Language code. If `FALSE`, returns all localized versions. If `NULL`, uses the current language. |
| `$check` | `bool` | If `TRUE`, checks for file existence instead of reading content. |
| `$type` | `int` | Asset type constant (`CMS_TEMPLATE_ASSET_TYPE_*`). |

**Return Values:**
- `string\|array\|FALSE`: The asset content, an array of localized contents, the file path, or `FALSE` on failure.

**Inner Mechanisms:**
- Reads the asset file for the specified language.
- Falls back to the default language if the specified language is not available.
- Returns an array of all localized versions if `$language` is `FALSE`.

**Usage Context:**
Used internally to load template assets.

**Example:**
```php
$code = $template->get_asset("header_template", NULL, FALSE, CMS_TEMPLATE_ASSET_TYPE_CODE);
echo $code;
```

---

### `set_code`

**Purpose:**
Saves the template code asset.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index. |
| `$text` | `string` | Template code. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Uses `set_asset` to save the code.
- Invalidates the template cache by touching the data file.

**Usage Context:**
Used in the admin interface to save template code.

**Example:**
```php
$template->set_code("header_template", "<CMS:head/>");
```

---

### `get_code`

**Purpose:**
Retrieves the template code asset.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index. |
| `$language` | `string\|NULL\|FALSE` | Language code. If `FALSE`, returns all localized versions. If `NULL`, uses the current language. |

**Return Values:**
- `string`: The template code.

**Inner Mechanisms:**
- Uses `get_asset` to retrieve the code.
- Implements temporary caching for performance.

**Usage Context:**
Used internally to load template code for parsing.

**Example:**
```php
$code = $template->get_code("header_template");
echo $code;
```

---

### `set_stylesheet`

**Purpose:**
Saves the template stylesheet asset.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index. |
| `$text` | `string` | Stylesheet code. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Uses `set_asset` to save the stylesheet.

**Usage Context:**
Used in the admin interface to save template stylesheets.

**Example:**
```php
$template->set_stylesheet("header_template", "body { font-family: Arial; }");
```

---

### `get_stylesheet`

**Purpose:**
Retrieves the template stylesheet asset.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index. |
| `$language` | `string\|NULL\|FALSE` | Language code. If `FALSE`, returns all localized versions. If `NULL`, uses the current language. |
| `$check` | `bool` | If `TRUE`, checks for file existence instead of reading content. |

**Return Values:**
- `string\|array\|FALSE`: The stylesheet content, an array of localized contents, the file path, or `FALSE` on failure.

**Inner Mechanisms:**
- Uses `get_asset` to retrieve the stylesheet.

**Usage Context:**
Used internally to load stylesheets for templates.

**Example:**
```php
$stylesheet = $template->get_stylesheet("header_template", NULL, TRUE);
if ($stylesheet !== FALSE) {
    echo stylesheet(CMS_DATA_URL . $stylesheet, FALSE);
}
```

---

### `set_javascript`

**Purpose:**
Saves the template JavaScript asset.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index. |
| `$text` | `string` | JavaScript code. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Uses `set_asset` to save the JavaScript.

**Usage Context:**
Used in the admin interface to save template JavaScript.

**Example:**
```php
$template->set_javascript("header_template", "console.log('Template loaded');");
```

---

### `get_javascript`

**Purpose:**
Retrieves the template JavaScript asset.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index. |
| `$language` | `string\|NULL\|FALSE` | Language code. If `FALSE`, returns all localized versions. If `NULL`, uses the current language. |
| `$check` | `bool` | If `TRUE`, checks for file existence instead of reading content. |

**Return Values:**
- `string\|array\|FALSE`: The JavaScript content, an array of localized contents, the file path, or `FALSE` on failure.

**Inner Mechanisms:**
- Uses `get_asset` to retrieve the JavaScript.

**Usage Context:**
Used internally to load JavaScript for templates.

**Example:**
```php
$javascript = $template->get_javascript("header_template", NULL, TRUE);
if ($javascript !== FALSE) {
    echo javascript(CMS_DATA_URL . $javascript, FALSE);
}
```

---

### `delete`

**Purpose:**
Deletes a template and its assets.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Template index. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Validates operator permissions.
- Deletes all localized versions of the template code, stylesheet, and JavaScript.
- Removes the template record from the data source.

**Usage Context:**
Used in the admin interface to delete templates.

**Example:**
```php
if ($template->delete("header_template")) {
    echo "Template deleted successfully.";
}
```

---

### `parse`

**Purpose:**
Parses a template and generates HTML output.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$document` | `document` | Document object containing content data. Passed by reference. |
| `$index` | `string` | Template index. |
| `$title` | `string\|NULL` | Document title. |
| `$description` | `string\|NULL` | Document description. |
| `$keyword` | `string\|NULL` | Document keywords. |
| `$action` | `array\|NULL` | Action definitions for editable elements. |
| `$header` | `string\|NULL` | Additional header content. |
| `$base_id` | `string\|NULL` | Base ID for content paths. |
| `$cache` | `bool` | Whether to enable caching. |

**Return Values:**
- `string`: The generated HTML output.

**Inner Mechanisms:**
- Loads the template code, stylesheet, and JavaScript.
- Processes the template using `_parse`.
- Injects meta data, stylesheets, and scripts into the output.

**Usage Context:**
Used to render templates for display.

**Example:**
```php
$document = new document();
$html = $template->parse($document, "header_template", "Home", "Welcome to our site", "home, welcome");
echo $html;
```

---

### `parse_code`

**Purpose:**
Parses raw template code and generates HTML output.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$document` | `document` | Document object containing content data. Passed by reference. |
| `$template` | `string` | Raw template code. |
| `$title` | `string\|NULL` | Document title. |
| `$description` | `string\|NULL` | Document description. |
| `$keyword` | `string\|NULL` | Document keywords. |
| `$action` | `array\|NULL` | Action definitions for editable elements. |
| `$header` | `string\|NULL` | Additional header content. |
| `$base_id` | `string\|NULL` | Base ID for content paths. |

**Return Values:**
- `string`: The generated HTML output.

**Inner Mechanisms:**
- Uses `_parse` to process the template code.

**Usage Context:**
Used to render inline template code.

**Example:**
```php
$document = new document();
$html = $template->parse_code($document, "<CMS:head/><h1>Welcome</h1>");
echo $html;
```

---

### `_parse`

**Purpose:**
Internal method to parse template code and generate HTML output.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$document` | `document` | Document object containing content data. Passed by reference. |
| `$template` | `string` | Template code. |
| `$base_id` | `string\|NULL` | Base ID for content paths. |
| `$cache` | `bool\|int` | Caching mode. `TRUE` for caching, `FALSE` for no caching, `-1` to finish a nocache span. |
| `$template_index` | `string\|NULL` | Template index for caching purposes. |

**Return Values:**
- `string`: The generated HTML output.

**Inner Mechanisms:**
- Processes template tags (`<CMS:*>`) and PHP code blocks.
- Handles conditional logic, loops, and nested templates.
- Manages editable elements and their actions.
- Supports caching and dynamic content injection.

**Usage Context:**
Used internally by `parse` and `parse_code`.

**Example:**
```php
$html = $template->_parse($document, "<CMS:head/><h1>Welcome</h1>", NULL, TRUE);
echo $html;
```

---

### `export`

**Purpose:**
Exports a template and its content into a standalone template string, optionally including combined assets.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$document` | `document` | Document object containing content data. Passed by reference. |
| `$index` | `string` | Template index. |
| `$base_id` | `string\|NULL` | Base ID for content paths. |
| `$return_asset` | `bool` | If `TRUE`, returns combined stylesheets and JavaScript. |

**Return Values:**
- `string\|array`: The exported template code, or an array with `code`, `stylesheet`, and `javascript` keys if `$return_asset` is `TRUE`.

**Inner Mechanisms:**
- Processes the template to resolve dynamic content and conditional logic.
- Combines stylesheets and JavaScript from all employed templates if `$return_asset` is `TRUE`.

**Usage Context:**
Used to export templates for sharing or backup.

**Example:**
```php
$export = $template->export($document, "header_template", NULL, TRUE);
file_put_contents("header_template.html", $export["code"]);
file_put_contents("header_template.css", $export["stylesheet"]);
file_put_contents("header_template.js", $export["javascript"]);
```

---

### `structure`

**Purpose:**
Generates a structural representation of a template for editing purposes.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$document` | `document` | Document object containing content data. Passed by reference. |
| `$index` | `string` | Template index. |
| `$base_id` | `string\|NULL` | Base ID for content paths. |

**Return Values:**
- `array`: Associative array representing the template structure, with keys for index, path, parent, and type.

**Inner Mechanisms:**
- Processes the template to identify editable and structural elements.
- Builds a hierarchical representation of the template.

**Usage Context:**
Used in the admin interface to visualize and edit template structure.

**Example:**
```php
$structure = $template->structure($document, "header_template");
print_r($structure);
```

---

### `create_cache`

**Purpose:**
Generates a cacheable version of a template with dynamic content placeholders.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$document` | `document` | Document object containing content data. Passed by reference. |
| `$index` | `string` | Template index. |
| `$title` | `string\|NULL` | Document title. |
| `$description` | `string\|NULL` | Document description. |
| `$keyword` | `string\|NULL` | Document keywords. |
| `$action` | `array\|NULL` | Action definitions for editable elements. |
| `$header` | `string\|NULL` | Additional header content. |
| `$is_dynamic` | `bool` | Output parameter indicating whether dynamic content is present. |

**Return Values:**
- `array`: Associative array with `cache` and `output` keys. `cache` contains the cacheable template, and `output` contains the fully processed output.

**Inner Mechanisms:**
- Uses `parse` to generate the template output.
- Splits the output into static and dynamic parts using `CMS_TEMPLATE_CACHE_SEPARATOR`.

**Usage Context:**
Used to generate cacheable templates for performance optimization.

**Example:**
```php
$result = $template->create_cache($document, "header_template", "Home", NULL, NULL, NULL, NULL, $is_dynamic);
if ($is_dynamic) {
    echo "Template contains dynamic content.";
}
file_put_contents("header_template.cache", $result["cache"]);
```

---

### `process_cache`

**Purpose:**
Processes a cached template, injecting dynamic content.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$document` | `document` | Document object containing content data. Passed by reference. |
| `$template` | `string` | Cached template string. |
| `$title` | `string\|NULL` | Document title. |
| `$description` | `string\|NULL` | Document description. |
| `$keyword` | `string\|NULL` | Document keywords. |
| `$action` | `array\|NULL` | Action definitions for editable elements. |
| `$header` | `string\|NULL` | Additional header content. |
| `$is_dynamic` | `bool` | Output parameter indicating whether dynamic content was processed. |

**Return Values:**
- `string`: The fully processed HTML output.

**Inner Mechanisms:**
- Splits the cached template into static and dynamic parts.
- Processes dynamic parts using `_parse`.

**Usage Context:**
Used to render cached templates with dynamic content.

**Example:**
```php
$cache = file_get_contents("header_template.cache");
$html = $template->process_cache($document, $cache, "Home", NULL, NULL, NULL, NULL, $is_dynamic);
echo $html;
```

---

### `execute`

**Purpose:**
Executes PHP code within a template context.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$cms_template_code` | `string` | PHP code to execute. |
| `$cms_template_document` | `document` | Document object. |
| `$cms_template_base_id` | `string` | Base ID for content paths. |
| `$cms_template_path_id` | `string` | Path ID for content paths. |
| `$cms_template_temp_id` | `string` | Temporary ID for variable storage. |

**Return Values:**
- `mixed`: The return value of the executed code.

**Inner Mechanisms:**
- Temporarily replaces the error handler with `template_error`.
- Extracts and stores variables for the template context.
- Executes the code within the current namespace.

**Usage Context:**
Used internally to execute PHP code blocks in templates.

**Example:**
```php
$result = $template->execute('return 2 + 2;', $document, "", "", "temp1");
echo $result; // Outputs: 4
```

---

### `attribute`

**Purpose:**
Generates an HTML attribute string from an associative array.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$param` | `array` | Associative array of attribute names and values. |
| `$alter` | `array\|NULL` | Additional attributes to merge. |
| `$skip` | `string\|array\|NULL` | Attribute names to skip. |

**Return Values:**
- `string`: The generated attribute string.

**Inner Mechanisms:**
- Merges additional attributes.
- Skips internal and specified attributes.
- Escapes attribute values using `x`.

**Usage Context:**
Used internally to generate HTML attributes for template tags.

**Example:**
```php
$attributes = ["id" => "header", "class" => "title", "_title" => "Page Header"];
echo "<div" . $template->attribute($attributes) . ">Welcome</div>";
// Outputs: <div id="header" class="title">Welcome</div>
```

---

### `parse_id`

**Purpose:**
Parses a template ID string into its components, handling relative and absolute paths.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | ID string to parse. |
| `$id_base` | `string` | Base ID for relative paths. |
| `$id_path` | `string` | Current path ID for relative paths. |
| `$id_auto` | `int` | Current automatic index for relative paths. |
| `$id_data_new` | `string` | Output parameter for the resolved data index. |
| `$id_path_new` | `string` | Output parameter for the resolved path index. |
| `$id_elem_new` | `string` | Output parameter for the resolved element index. |

**Return Values:**
- `bool`: `TRUE` if the ID is valid, `FALSE` if an automatic index was used.

**Inner Mechanisms:**
- Processes relative automatic indexes (e.g., `+1`, `-2`).
- Handles relative (`.`), absolute (`..`), and base-relative paths.
- Falls back to an automatic index if the ID is invalid.

**Usage Context:**
Used internally to resolve element IDs in templates.

**Example:**
```php
$valid = $template->parse_id("+1.title", "base.", "base.current.", 0, $data, $path, $element);
echo $data; // Outputs: base.title
echo $path; // Outputs: base.current.title
echo $element; // Outputs: 1.title
```


<!-- HASH:580259ba4255b9320774b181073fb33c -->
