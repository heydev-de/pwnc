# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.directory.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.directory.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Directory Management Module

This file provides core functionality for managing the PWNC Web Platform's directory structure, which serves as the primary navigation and content organization system. It includes utility functions for canonical path resolution, directory visualization, filesystem synchronization, and a class for programmatic directory manipulation.

---

## Functions

### `directory_get_canonical`

**Purpose:**
Retrieves a canonical URL path for a given directory index from the system's canonical map.

**Parameters:**

| Name  | Type   | Default | Description                          |
|-------|--------|---------|--------------------------------------|
| index | int    | NULL    | Directory index. If NULL, defaults to 0. |

**Return Values:**
- `string`: Canonical URL path for the specified index.

**Inner Mechanisms:**
1. Uses a `map` object to load canonical paths from a language-specific file (`#system/[LANG.]directory.canonical`).
2. Falls back to the default language if `CMS_LANGUAGE` is not set.

**Usage Context:**
Used when generating absolute URLs for directory entries, ensuring consistent path resolution across the system.

**Example:**
```php
$canonical_path = directory_get_canonical(5); // Returns "/products/electronics/"
```

---

### `directory_flexview_display_function`

**Purpose:**
Custom display function for rendering directory entries in a `flexview` UI component. Handles icons, styling, and visibility based on entry metadata.

**Parameters:**

| Name      | Type      | Default | Description                                      |
|-----------|-----------|---------|--------------------------------------------------|
| flexview  | flexview  | —       | The flexview object rendering the directory.     |
| index     | string    | —       | Current entry index.                             |
| open      | bool      | —       | Whether the entry is expanded in the UI.         |

**Return Values:**
- None. Outputs HTML directly via `echo`.

**Inner Mechanisms:**
1. Determines if the current entry is active (matches `flexview->index`).
2. Resolves entry name, visibility, and placeholder status.
3. Selects appropriate icons based on entry type and state (hidden, placeholder, used).
4. Applies CSS classes and HTML markup for visual feedback.
5. Uses `preg_replace` to inject values into the flexview’s display template.

**Usage Context:**
Used as a callback in `flexview` instances to render directory trees in the admin UI.

**Example:**
```php
$flexview = new flexview();
$flexview->set_display_function("cms\\directory_flexview_display_function");
$flexview->set_data($directory_data);
$flexview->render();
```

---

### `directory_get_select`

**Purpose:**
Generates an associative array suitable for use in HTML `<select>` elements, mapping human-readable directory paths to their internal keys.

**Parameters:**
- None.

**Return Values:**
- `array`: Keys are indented directory names; values are internal keys.

**Inner Mechanisms:**
1. Loads directory data from `#system/directory`.
2. Traverses the tree, applying indentation to reflect hierarchy.
3. Skips entries with empty names.
4. Wraps container names in parentheses if they lack a URL.

**Usage Context:**
Used in forms where users select a directory (e.g., content assignment, navigation settings).

**Example:**
```php
$options = directory_get_select();
echo "<select name='target_directory'>";
foreach ($options as $label => $value) {
    echo "<option value='" . x($value) . "'>" . x($label) . "</option>";
}
echo "</select>";
```

---

### `directory_get_type`

**Purpose:**
Retrieves URLs for directory entry type icons (both standard and expanded states).

**Parameters:**
- None.

**Return Values:**
- `array`: Keys are type identifiers (e.g., `article`, `+article`); values are absolute URLs to icon files.

**Inner Mechanisms:**
1. Loads type definitions from `#system/directory.type`.
2. Resolves icon paths using `CMS_DATA_URL`.
3. Supports both standard (`format`) and expanded (`+format`) icons.

**Usage Context:**
Used to render consistent icons across directory views.

**Example:**
```php
$icons = directory_get_type();
echo "<img src='" . $icons['article'] . "' alt='Article'>";
```

---

### `directory_get_type_select`

**Purpose:**
Generates a selectable list of directory entry types for use in forms.

**Parameters:**
- None.

**Return Values:**
- `array`: Keys are formatted strings (name + icon path); values are type identifiers.

**Inner Mechanisms:**
1. Loads type data from `#system/directory.type`.
2. Ensures unique display names via suffixing.
3. Sorts types naturally and case-insensitively.
4. Appends icon paths to keys for visual feedback in dropdowns.

**Usage Context:**
Used in forms where users assign types to directory entries.

**Example:**
```php
$types = directory_get_type_select();
echo "<select name='entry_type'>";
foreach ($types as $label => $value) {
    $parts = explode("|", $label, 2);
    echo "<option value='" . x($value) . "' data-icon='" . x($parts[1]) . "'>" . x($parts[0]) . "</option>";
}
echo "</select>";
```

---

### `directory_get_visible`

**Purpose:**
Finds the nearest visible ancestor of a given directory index, accounting for hidden containers.

**Parameters:**

| Name  | Type   | Default | Description                                      |
|-------|--------|---------|--------------------------------------------------|
| index | string | NULL    | Directory index. If NULL, uses `CMS_CONTENT_DIRECTORY_INDEX`. |

**Return Values:**
- `string`: Key of the nearest visible ancestor, or 0 if none found.

**Inner Mechanisms:**
1. Traverses the directory tree from root.
2. Tracks visibility state using a stack.
3. Returns the last visible key before encountering the target or a hidden container.

**Usage Context:**
Used to resolve fallback URLs when a requested directory is hidden.

**Example:**
```php
$visible_key = directory_get_visible("12"); // Returns "5" if parent is visible
```

---

### `directory_value`

**Purpose:**
Resolves a full URL for a directory entry, optionally appending query parameters.

**Parameters:**

| Name  | Type   | Default | Description                          |
|-------|--------|---------|--------------------------------------|
| index | string | —       | Directory index.                     |

**Return Values:**
- `string`: Absolute URL including `CMS_ROOT_URL`.

**Inner Mechanisms:**
1. Loads the language-specific directory map.
2. Retrieves the base path.
3. Appends current query string if a `#` placeholder exists in the path.

**Usage Context:**
Used to generate clickable links in navigation menus and content listings.

**Example:**
```php
$url = directory_value("7"); // Returns "https://example.com/products/#sort=price"
```

---

### `directory_get_flexview`

**Purpose:**
Creates a `flexview` object pre-configured for rendering the directory tree, with optional filtering of hidden/placeholder entries.

**Parameters:**

| Name            | Type | Default | Description                                      |
|-----------------|------|---------|--------------------------------------------------|
| remove_hidden   | bool | TRUE    | Whether to exclude hidden or unused placeholder entries. |

**Return Values:**
- `flexview|bool`: Configured flexview object, or `FALSE` if `flexview` library is not loaded.

**Inner Mechanisms:**
1. Loads the `flexview` library.
2. Traverses directory data, skipping entries based on `remove_hidden`.
3. Configures the flexview with a value function (`directory_value`) and hierarchical structure.

**Usage Context:**
Used in admin interfaces to display and manage the directory structure.

**Example:**
```php
$flexview = directory_get_flexview();
if ($flexview) {
    $flexview->render();
}
```

---

### `directory_create_filesystem`

**Purpose:**
Synchronizes the virtual directory structure with the physical filesystem, generating PHP entry points and auxiliary files.

**Parameters:**

| Name      | Type   | Default               | Description                                      |
|-----------|--------|-----------------------|--------------------------------------------------|
| language  | string | `CMS_LANGUAGE_ENABLED`| Comma-separated list of languages to generate.   |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Reverts prior filesystem changes using `directory_remove_filesystem()`.
2. Copies auxiliary files (e.g., `.htaccess`) to each directory.
3. Generates PHP files for each directory entry, setting appropriate GET parameters.
4. Handles language-specific paths and canonical URLs.
5. Resolves directory references (e.g., `directory://other_key`).
6. Writes a log file (`directory.log`) for rollback purposes.
7. Saves updated path and canonical maps.

**Usage Context:**
Called automatically after directory modifications, or manually to rebuild the filesystem.

**Example:**
```php
if (directory_create_filesystem("en,es,de")) {
    echo "Filesystem synchronized successfully.";
}
```

---

### `directory_remove_filesystem`

**Purpose:**
Reverts all filesystem changes made by `directory_create_filesystem()` using the log file.

**Parameters:**
- None.

**Return Values:**
- None.

**Inner Mechanisms:**
1. Reads `directory.log` to identify generated files and directories.
2. Deletes files and removes empty directories in reverse order.

**Usage Context:**
Used during cleanup or before regenerating the filesystem.

**Example:**
```php
directory_remove_filesystem(); // Reverts all directory-related filesystem changes
```

---

## Class: `directory`

**Purpose:**
Provides an object-oriented interface for manipulating the directory structure stored in `#system/directory`.

### Properties

| Name | Type  | Description                          |
|------|-------|--------------------------------------|
| data | data  | Internal `data` object for directory storage. |

### Constructor

**Purpose:**
Initializes the directory object and loads the directory data.

**Example:**
```php
$dir = new directory();
```

---

### Method: `append`

**Purpose:**
Appends a new container entry after the specified key.

**Parameters:**

| Name            | Type    | Default | Description                                      |
|-----------------|---------|---------|--------------------------------------------------|
| key             | string  | —       | Key after which the new entry is inserted.       |
| name            | string  | —       | Display name of the entry.                       |
| description     | string  | ""      | Optional description.                            |
| url             | string  | ""      | URL or internal reference (e.g., `content://5`). |
| subtype         | string  | ""      | Entry type (e.g., `article`, `product`).         |
| hidden          | bool    | FALSE   | Whether the entry is hidden.                     |
| placeholder     | bool    | FALSE   | Whether the entry is a placeholder.              |
| dynamic         | bool    | TRUE    | Whether the entry is dynamic.                    |
| image_button    | string  | ""      | Path to button image.                            |
| image_hover     | string  | ""      | Path to hover image.                             |
| image_active    | string  | ""      | Path to active image.                            |
| path            | string  | ""      | Override filesystem path.                        |
| canonical       | string  | ""      | Override canonical URL.                          |

**Return Values:**
- `string|bool`: The new entry’s key, or `FALSE` on failure.

**Example:**
```php
$new_key = $dir->append("5", "New Product", "Latest addition", "content://12", "product");
```

---

### Method: `insert`

**Purpose:**
Inserts a new container entry before the specified key.

**Parameters:**
Same as `append`.

**Return Values:**
- `string|bool`: The new entry’s key, or `FALSE` on failure.

**Example:**
```php
$new_key = $dir->insert("5", "Featured", "", "content://10", "category");
```

---

### Method: `set`

**Purpose:**
Updates one or more properties of an existing directory entry.

**Parameters:**

| Name            | Type    | Default | Description                                      |
|-----------------|---------|---------|--------------------------------------------------|
| key             | string  | —       | Entry key to update.                             |
| name            | string  | NULL    | New name (NULL to skip).                         |
| description     | string  | NULL    | New description (NULL to skip).                  |
| url             | string  | NULL    | New URL (NULL to skip).                          |
| subtype         | string  | NULL    | New subtype (NULL to skip).                      |
| hidden          | bool    | NULL    | New hidden status (NULL to skip).                |
| placeholder     | bool    | NULL    | New placeholder status (NULL to skip).           |
| dynamic         | bool    | NULL    | New dynamic status (NULL to skip).               |
| image_button    | string  | NULL    | New button image (NULL to skip).                 |
| image_hover     | string  | NULL    | New hover image (NULL to skip).                  |
| image_active    | string  | NULL    | New active image (NULL to skip).                 |
| path            | string  | NULL    | New path override (NULL to skip).                |
| canonical       | string  | NULL    | New canonical URL (NULL to skip).                |

**Return Values:**
- `string|bool`: The entry’s key, or `FALSE` if the entry does not exist.

**Example:**
```php
$dir->set("7", NULL, "Updated description", NULL, "premium_product", TRUE);
```

---

### Method: `del`

**Purpose:**
Deletes a directory entry and its children.

**Parameters:**

| Name | Type   | Description          |
|------|--------|----------------------|
| key  | string | Entry key to delete. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Example:**
```php
$dir->del("10");
```

---

### Method: `parse_placeholder`

**Purpose:**
Marks placeholder entries as "used" if they contain active (non-placeholder) children, or clears the "used" flag otherwise.

**Parameters:**
- None.

**Return Values:**
- None.

**Inner Mechanisms:**
1. Traverses the directory tree.
2. Tracks depth and the last non-placeholder entry.
3. Updates the `used` flag based on child status.

**Usage Context:**
Called automatically during `save()` to maintain placeholder semantics.

**Example:**
```php
$dir->parse_placeholder(); // Internal use only
```

---

### Method: `save`

**Purpose:**
Saves the directory data and regenerates the filesystem.

**Parameters:**
- None.

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Calls `parse_placeholder()` to update placeholder status.
2. Saves the underlying `data` object.
3. Triggers `directory_create_filesystem()` to synchronize the filesystem.

**Example:**
```php
if ($dir->save()) {
    echo "Directory saved and filesystem updated.";
}
```


<!-- HASH:7b4c5e836ce2d182362932383ca3bbc6 -->
