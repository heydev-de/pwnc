# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.flexview.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.flexview.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## FlexView Module (`lib.flexview.inc`)

The **FlexView** module provides a flexible, hierarchical data visualization system for the PWNC Web Platform. It enables developers to render structured data (e.g., file systems, menus, or organizational trees) in multiple formats (hierarchies, trees, columns, folders, paths, and drag-and-drop targets) with minimal configuration.

The module consists of two core classes:
- `flexview_entry`: Represents a single node in the hierarchy, storing metadata like type, position, and indentation.
- `flexview`: Manages the data structure, rendering logic, and user interactions (e.g., checkboxes, drag-and-drop).

---

## Constants

| Name                          | Value | Description                                                                 |
|-------------------------------|-------|-----------------------------------------------------------------------------|
| `CMS_FLEXVIEW_ENTRY_TYPE_NONE` | `0`   | No entry type (default).                                                    |
| `CMS_FLEXVIEW_ENTRY_TYPE_BASE` | `1`   | Root/base entry of the hierarchy.                                           |
| `CMS_FLEXVIEW_ENTRY_TYPE_ENTRY`| `2`   | Regular entry/node in the hierarchy.                                        |
| `CMS_FLEXVIEW_ENTRY_TYPE_END`  | `3`   | End-of-hierarchy marker (cleanup/closing logic).                            |

---

## Class: `flexview_entry`

Represents a node in the FlexView hierarchy with metadata for rendering and traversal.

### Properties

| Name         | Default                     | Description                                                                 |
|--------------|-----------------------------|-----------------------------------------------------------------------------|
| `type`       | `CMS_FLEXVIEW_ENTRY_TYPE_NONE` | Entry type (base/entry/end).                                                |
| `index`      | `NULL`                      | Unique identifier for the entry.                                            |
| `parent`     | `NULL`                      | Parent entry's index.                                                       |
| `position`   | `0`                         | Position among siblings (1-based).                                          |
| `count`      | `0`                         | Total siblings in the current level.                                        |
| `subcount`   | `0`                         | Number of child entries.                                                    |
| `indentation`| `0`                         | Depth level in the hierarchy.                                               |
| `open`       | `FALSE`                     | Whether the entry is expanded (for containers).                             |

---

## Class: `flexview`

Manages hierarchical data and renders it in various formats.

### Properties

| Name                  | Default/Value                                                                 | Description                                                                 |
|-----------------------|-------------------------------------------------------------------------------|-----------------------------------------------------------------------------|
| `object`              | `["" => ["#data" => ["#type" => "base"]]]`                                    | Hierarchical data structure (nested associative arrays).                   |
| `icon_default`        | Loaded from `#system/flexview.icon` or fallback defaults                      | Default icons for base/entry/container states.                             |
| `value_function`      | `NULL`                                                                       | Callback to transform entry indices (e.g., for encoding).                  |
| `encoding_function`   | `__NAMESPACE__ . "\\x"`                                                       | Default: XML-escaping function.                                            |
| `display_function`    | `NULL`                                                                       | Custom rendering callback (overrides `display` template).                  |
| `display`             | `"%checkbox%<a[...]>%mark%[%icon% ]%name%</a>"`                              | Template string for entry rendering (placeholders: `%name%`, `%icon%`, etc).|
| `index`               | `""`                                                                         | Currently selected entry.                                                  |
| `checkbox_identifier` | `NULL`                                                                       | Name attribute for checkboxes (enables multi-selection).                   |
| `checkbox_list`       | `NULL`                                                                       | Array of pre-checked indices.                                              |
| `mark`                | `NULL`                                                                       | Custom markers (e.g., status icons) for entries.                           |
| `icon_custom`         | `NULL`                                                                       | Custom icons for specific entry types/subtypes.                            |
| `action`              | `"%index%"`                                                                  | URL template for entry links (placeholder: `%index%`).                     |
| `name_key`            | `"name"`                                                                     | Key in `#data` for the entry's display name.                               |
| `image_*_key`         | `"image_button"`, `"image_hover"`, `"image_active"`                          | Keys for image states (button/hover/active).                               |
| `description_key`     | `"description"`                                                              | Key for entry descriptions.                                                |
| `base`                | `""`                                                                         | Root entry of the hierarchy.                                                |
| `param`               | `NULL`                                                                       | Internal parameter storage (e.g., for drag-and-drop).                      |

---

### Constructor: `__construct()`

**Purpose**:
Initializes the FlexView object, loading default icons and setting the encoding function.

**Inner Mechanisms**:
1. Loads default icons from `#system/flexview.icon` (fallback to built-in defaults if missing).
2. Sets `encoding_function` to `x()` (XML-escaping).

**Usage Example**:
```php
$flexview = new flexview();
```

---

### Method: `set_value_function($callback_function)`

**Purpose**:
Sets a callback to transform entry indices before rendering (e.g., for encoding or formatting).

**Parameters**:

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$callback_function` | `callable` | Function accepting an index and returning a transformed value.             |

**Return Value**:
`void`

**Inner Mechanisms**:
Validates the callback with `is_callable()` before assignment.

**Usage Example**:
```php
$flexview->set_value_function(function($index) {
    return "ID_" . $index; // Prefix indices with "ID_"
});
```

---

### Method: `set_encoding_function($callback_function)`

**Purpose**:
Sets a callback to escape/encode entry indices (e.g., for HTML/XML/URL contexts).

**Parameters**:

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$callback_function` | `callable` | Function accepting a string and returning an escaped version.              |

**Return Value**:
`void`

**Usage Example**:
```php
$flexview->set_encoding_function(function($str) {
    return htmlspecialchars($str, ENT_QUOTES, "UTF-8");
});
```

---

### Method: `set_display_function($callback_function)`

**Purpose**:
Overrides the default rendering logic with a custom callback.

**Parameters**:

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$callback_function` | `callable` | Function accepting `(flexview $this, string $index, bool $open)`.          |

**Return Value**:
`void`

**Usage Example**:
```php
$flexview->set_display_function(function($flexview, $index, $open) {
    echo "<div class='custom-entry'>" . $flexview->get_name($index) . "</div>";
});
```

---

### Method: `set_index($value)`

**Purpose**:
Sets the currently selected entry (validates existence).

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value` | `string` | Entry index. Falls back to `base` if invalid.                              |

**Return Value**:
`void`

**Inner Mechanisms**:
Falls back to `base` if the index doesn’t exist in `object`.

---

### Method: `set_checkbox_identifier($value)`

**Purpose**:
Enables checkboxes for multi-selection and sets their `name` attribute.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value` | `string` | HTML `name` attribute for checkboxes (e.g., `"selected_ids"`).             |

**Return Value**:
`void`

**Usage Example**:
```php
$flexview->set_checkbox_identifier("selected_files");
```

---

### Method: `set_checkbox_list($value)`

**Purpose**:
Pre-selects checkboxes based on an array of indices.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value` | `array`  | Array of indices to pre-check.                                              |

**Return Value**:
`void`

**Inner Mechanisms**:
Flips the array for O(1) lookup.

---

### Method: `get_checkbox($index)`

**Purpose**:
Generates an HTML checkbox for an entry, with JavaScript for styling.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$index` | `string` | Entry index.                                                                |

**Return Value**:
`string` HTML checkbox markup.

**Inner Mechanisms**:
1. Outputs JavaScript for checkbox styling (once per FlexView instance).
2. Uses `value_function` to transform the index if set.
3. Generates a hidden SVG spacer for layout.

**Usage Example**:
```php
echo $flexview->get_checkbox("file123");
```

---

### Method: `set_mark($value)`

**Purpose**:
Sets custom markers (e.g., status icons) for entries.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value` | `array`  | Associative array of `index => marker` (e.g., `["file123" => "icon_warning"]`). |

**Return Value**:
`void`

---

### Method: `get_mark($index)`

**Purpose**:
Retrieves the marker for an entry.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$index` | `string` | Entry index.                                                                |

**Return Value**:
`string|null` Marker value or `NULL` if unset.

---

### Method: `set_icon($value)`

**Purpose**:
Sets custom icons for entry types/subtypes.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value` | `array`  | Associative array of `type => icon` (e.g., `["container" => "folder_icon"]`). |

**Return Value**:
`void`

---

### Method: `get_icon($index, $open = FALSE)`

**Purpose**:
Retrieves the icon for an entry, considering its type, subtype, and open state.

**Parameters**:

| Name    | Type      | Description                                                                 |
|---------|-----------|-----------------------------------------------------------------------------|
| `$index` | `string`  | Entry index.                                                                |
| `$open`  | `bool`    | Whether the entry is expanded (for containers).                             |

**Return Value**:
`string` Icon identifier (e.g., `"flexview/icon_entry"`).

**Inner Mechanisms**:
1. Checks custom icons first, then falls back to defaults.
2. Prioritizes `#subtype` over `#type` if both exist.

---

### Method: `set_action($value)`

**Purpose**:
Sets the URL template for entry links (placeholder: `%index%`).

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value` | `string` | URL template (e.g., `"edit.php?id=%index%"`).                              |

**Return Value**:
`void`

---

### Method: `get_action($index)`

**Purpose**:
Generates the URL for an entry by replacing `%index%` in the action template.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$index` | `string` | Entry index.                                                                |

**Return Value**:
`string` Resolved URL.

---

### Method: `set_name_key($value)`

**Purpose**:
Sets the key in `#data` for the entry's display name.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value` | `string` | Key name (e.g., `"title"`).                                                |

**Return Value**:
`void`

---

### Method: `get_name($index)`

**Purpose**:
Retrieves the display name for an entry, falling back to localized strings if missing.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$index` | `string` | Entry index.                                                                |

**Return Value**:
`string` Display name.

---

### Method: `set_image_key($button, $hover, $active)`

**Purpose**:
Sets keys for image states (button/hover/active).

**Parameters**:

| Name      | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$button` | `string` | Key for default image.                                                      |
| `$hover`  | `string` | Key for hover image.                                                        |
| `$active` | `string` | Key for active (open) image.                                                |

**Return Value**:
`void`

---

### Method: `get_image($index, $open = FALSE)`

**Purpose**:
Retrieves the image URL for an entry, considering its state (hover/active).

**Parameters**:

| Name    | Type      | Description                                                                 |
|---------|-----------|-----------------------------------------------------------------------------|
| `$index` | `string`  | Entry index.                                                                |
| `$open`  | `bool`    | Whether the entry is expanded.                                              |

**Return Value**:
`string|null` Image URL or `NULL` if unset.

---

### Method: `set_base($value)`

**Purpose**:
Sets the root entry of the hierarchy.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$value` | `string` | Entry index. Falls back to `""` if invalid.                                 |

**Return Value**:
`void`

---

### Method: `display($index, $open = FALSE)`

**Purpose**:
Renders an entry using the `display` template or a custom `display_function`.

**Parameters**:

| Name    | Type      | Description                                                                 |
|---------|-----------|-----------------------------------------------------------------------------|
| `$index` | `string`  | Entry index.                                                                |
| `$open`  | `bool`    | Whether the entry is expanded.                                              |

**Return Value**:
`void` (Outputs HTML directly.)

**Inner Mechanisms**:
1. Replaces placeholders in `display` (e.g., `%name%`, `%icon%`) with resolved values.
2. Omits optional sections (e.g., `[%icon% ]`) if the value is empty.
3. Escapes values using the `encoding_function`.

**Usage Example**:
```php
$flexview->display("folder123", TRUE); // Renders an expanded folder
```

---

### Method: `get_value($index)`

**Purpose**:
Transforms an entry index using `value_function` and `encoding_function`.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$index` | `string` | Entry index.                                                                |

**Return Value**:
`string` Transformed/escaped index.

---

### Method: `set($index, $data = NULL, $parent = "")`

**Purpose**:
Adds an entry to the hierarchy.

**Parameters**:

| Name     | Type     | Description                                                                 |
|----------|----------|-----------------------------------------------------------------------------|
| `$index`  | `string` | Entry index.                                                                |
| `$data`   | `array`  | Entry data (e.g., `["name" => "File 1"]`).                                  |
| `$parent` | `string` | Parent entry index.                                                         |

**Return Value**:
`void`

**Inner Mechanisms**:
1. Stores data in `object[$index]["#data"]`.
2. Sets parent-child relationships.

---

### Method: `get_path()`

**Purpose**:
Retrieves the path from `base` to the current `index` as an array of indices.

**Return Value**:
`array` Path indices (e.g., `["root", "folder123"]`).

---

### Method: `import_data(&$data)`

**Purpose**:
Imports hierarchical data from a `data` object (e.g., from a database or file).

**Parameters**:

| Name   | Type      | Description                                                                 |
|--------|-----------|-----------------------------------------------------------------------------|
| `$data` | `data`    | Data object implementing `move()` and `get()`.                              |

**Return Value**:
`void`

**Inner Mechanisms**:
1. Traverses the data object, handling nested containers.
2. Uses a stack to track parent-child relationships.

**Usage Example**:
```php
$data = new data("#filesystem");
$flexview->import_data($data);
```

---

### Method: `import_database(&$result, $index_key = "id", $parent_key = "container")`

**Purpose**:
Imports hierarchical data from a MySQL result set.

**Parameters**:

| Name         | Type      | Description                                                                 |
|--------------|-----------|-----------------------------------------------------------------------------|
| `$result`    | `resource`| MySQL result resource.                                                      |
| `$index_key` | `string`  | Column name for entry indices.                                              |
| `$parent_key`| `string`  | Column name for parent indices (use `NULL` for root entries).               |

**Return Value**:
`void`

**Usage Example**:
```php
$result = mysql_query("SELECT id, name, container FROM menu_items");
$flexview->import_database($result, "id", "container");
```

---

### Method: `show_custom($callback_function)`

**Purpose**:
Renders the hierarchy using a custom callback, traversing entries in depth-first order.

**Parameters**:

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$callback_function` | `callable` | Function accepting a `flexview_entry` object.                              |

**Return Value**:
`void`

**Inner Mechanisms**:
1. Traverses the hierarchy recursively.
2. Calls the callback for each entry (base/entry/end).
3. Tracks indentation, position, and open state.

**Usage Example**:
```php
$flexview->show_custom(function($entry) {
    if ($entry->type == CMS_FLEXVIEW_ENTRY_TYPE_ENTRY) {
        echo str_repeat("&nbsp;", $entry->indentation * 4) . $entry->index . "<br>";
    }
});
```

---

### Method: `show_hierarchy($index = "", $action = NULL, $name_key = "name", $mark = NULL, $icon = NULL, $base = "", $dragdrop_event_function = NULL, $dragdrop_type_accept = NULL)`

**Purpose**:
Renders the hierarchy as an interactive nested list with drag-and-drop support.

**Parameters**:

| Name                     | Type       | Description                                                                 |
|--------------------------|------------|-----------------------------------------------------------------------------|
| `$index`                 | `string`   | Selected entry index.                                                       |
| `$action`                | `string`   | URL template for entry links.                                               |
| `$name_key`              | `string`   | Key for entry names.                                                        |
| `$mark`                  | `array`    | Custom markers for entries.                                                 |
| `$icon`                  | `array`    | Custom icons for entry types.                                               |
| `$base`                  | `string`   | Root entry index.                                                           |
| `$dragdrop_event_function`| `string`  | JavaScript callback for drag-and-drop events.                               |
| `$dragdrop_type_accept`  | `array`    | Defines which entry types can be dropped where.                             |

**Return Value**:
`void`

**Inner Mechanisms**:
1. Outputs JavaScript for drag-and-drop initialization (once per FlexView instance).
2. Uses `_show_hierarchy()` for rendering.

**Usage Example**:
```php
$flexview->show_hierarchy(
    "folder123",
    "edit.php?id=%index%",
    "title",
    ["folder123" => "icon_warning"],
    ["container" => "folder_icon"],
    "",
    "handleDragDrop",
    ["container" => ["insert" => 1, "append" => 1]]
);
```

---

### Method: `_show_hierarchy($flexview_entry)`

**Purpose**:
Internal callback for `show_hierarchy()` to render individual entries.

**Parameters**:

| Name              | Type              | Description                                                                 |
|-------------------|-------------------|-----------------------------------------------------------------------------|
| `$flexview_entry` | `flexview_entry`  | Entry metadata.                                                             |

**Return Value**:
`void`

**Inner Mechanisms**:
1. Handles indentation and sublist toggling.
2. Outputs drag-and-drop attributes (`data-fv-hir-type`, `data-fv-hir-accept`).
3. Uses `display()` for entry rendering.

---

### Method: `show_tree($index = "", $action = NULL, $name_key = "name", $mark = NULL, $icon = NULL, $base = "")`

**Purpose**:
Renders the hierarchy as a traditional tree with indentation and branch icons.

**Parameters**:

| Name        | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$index`    | `string` | Selected entry index.                                                       |
| `$action`   | `string` | URL template for entry links.                                               |
| `$name_key` | `string` | Key for entry names.                                                        |
| `$mark`     | `array`  | Custom markers for entries.                                                 |
| `$icon`     | `array`  | Custom icons for entry types.                                               |
| `$base`     | `string` | Root entry index.                                                           |

**Return Value**:
`void`

**Usage Example**:
```php
$flexview->show_tree("folder123", "edit.php?id=%index%");
```

---

### Method: `_show_tree($flexview_entry)`

**Purpose**:
Internal callback for `show_tree()` to render individual entries.

**Parameters**:

| Name              | Type              | Description                                                                 |
|-------------------|-------------------|-----------------------------------------------------------------------------|
| `$flexview_entry` | `flexview_entry`  | Entry metadata.                                                             |

**Return Value**:
`void`

**Inner Mechanisms**:
1. Uses indentation images (e.g., `tree_branchopen.svg`) for visual hierarchy.
2. Tracks open/closed state for containers.

---

### Method: `show_target($index = "", $action = NULL, $action_insert = NULL, $action_append = NULL, $name_key = "name", $base = "", $type_insert = NULL, $subtype_insert = NULL, $type_append = NULL, $subtype_append = NULL)`

**Purpose**:
Renders a hierarchy as a target for drag-and-drop operations, with insert/append actions.

**Parameters**:

| Name               | Type     | Description                                                                 |
|--------------------|----------|-----------------------------------------------------------------------------|
| `$index`           | `string` | Selected entry index.                                                       |
| `$action`          | `string` | URL template for entry links.                                               |
| `$action_insert`   | `string` | URL template for insert actions.                                            |
| `$action_append`   | `string` | URL template for append actions.                                            |
| `$name_key`        | `string` | Key for entry names.                                                        |
| `$base`            | `string` | Root entry index.                                                           |
| `$type_insert`     | `array`  | Defines which entry types can be inserted.                                  |
| `$subtype_insert`  | `array`  | Defines which entry subtypes can be inserted.                               |
| `$type_append`     | `array`  | Defines which entry types can be appended.                                  |
| `$subtype_append`  | `array`  | Defines which entry subtypes can be appended.                               |

**Return Value**:
`void`

**Usage Example**:
```php
$flexview->show_target(
    "folder123",
    "view.php?id=%index%",
    "insert.php?parent=%index%",
    "append.php?parent=%index%",
    "name",
    "",
    ["container" => TRUE],
    NULL,
    ["container" => TRUE],
    NULL
);
```

---

### Method: `_show_target($flexview_entry)`

**Purpose**:
Internal callback for `show_target()` to render individual entries.

**Parameters**:

| Name              | Type              | Description                                                                 |
|-------------------|-------------------|-----------------------------------------------------------------------------|
| `$flexview_entry` | `flexview_entry`  | Entry metadata.                                                             |

**Return Value**:
`void`

**Inner Mechanisms**:
1. Validates insert/append targets based on type/subtype.
2. Outputs action links (e.g., "Insert Here") for valid targets.

---

### Method: `show_path($index = "", $action = NULL, $name_key = "name", $delimiter = "›", $base = "")`

**Purpose**:
Renders a breadcrumb path from `base` to `index`.

**Parameters**:

| Name        | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$index`    | `string` | Selected entry index.                                                       |
| `$action`   | `string` | URL template for entry links.                                               |
| `$name_key` | `string` | Key for entry names.                                                        |
| `$delimiter`| `string` | Separator between path elements (default: `›`).                             |
| `$base`     | `string` | Root entry index.                                                           |

**Return Value**:
`void`

**Usage Example**:
```php
$flexview->show_path("file123", "view.php?id=%index%");
```

---

### Method: `show_column($index = "", $action = NULL, $name_key = "name", $mark = NULL, $icon = NULL, $base = "")`

**Purpose**:
Renders the hierarchy as a column view (e.g., macOS Finder).

**Parameters**:

| Name        | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$index`    | `string` | Selected entry index.                                                       |
| `$action`   | `string` | URL template for entry links.                                               |
| `$name_key` | `string` | Key for entry names.                                                        |
| `$mark`     | `array`  | Custom markers for entries.                                                 |
| `$icon`     | `array`  | Custom icons for entry types.                                               |
| `$base`     | `string` | Root entry index.                                                           |

**Return Value**:
`void`

**Usage Example**:
```php
$flexview->show_column("folder123");
```

---

### Method: `_show_column()`

**Purpose**:
Internal method for `show_column()` to render the column view.

**Return Value**:
`void`

**Inner Mechanisms**:
1. Renders the base path as a horizontal list.
2. Lists child entries in a vertical column.

---

### Method: `show_folder($index = "", $action = NULL, $name_key = "name", $mark = NULL, $icon = NULL, $base = "")`

**Purpose**:
Renders the hierarchy as a folder view (flat list of child entries).

**Parameters**:

| Name        | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$index`    | `string` | Selected entry index.                                                       |
| `$action`   | `string` | URL template for entry links.                                               |
| `$name_key` | `string` | Key for entry names.                                                        |
| `$mark`     | `array`  | Custom markers for entries.                                                 |
| `$icon`     | `array`  | Custom icons for entry types.                                               |
| `$base`     | `string` | Root entry index.                                                           |

**Return Value**:
`void`

**Usage Example**:
```php
$flexview->show_folder("folder123");
```

---

### Method: `_show_folder()`

**Purpose**:
Internal method for `show_folder()` to render the folder view.

**Return Value**:
`void`

**Inner Mechanisms**:
1. Renders the base path as a horizontal list.
2. Lists child entries in a flat vertical list.

---

### Method: `space($count = 1, $size = 18)`

**Purpose**:
Generates an invisible SVG spacer for layout.

**Parameters**:

| Name     | Type     | Description                                                                 |
|----------|----------|-----------------------------------------------------------------------------|
| `$count` | `int`    | Number of spaces (default: `1`).                                            |
| `$size`  | `int`    | Width of each space in pixels (default: `18`).                              |

**Return Value**:
`string` HTML `<img>` tag with SVG spacer.

**Usage Example**:
```php
echo $flexview->space(2); // Two spaces
```


<!-- HASH:c91aef103b3fb2d540c5eb4511aa9a37 -->
