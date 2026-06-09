# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.menu.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.menu.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Menu Class

The `menu` class in the PWNC Web Platform is responsible for generating hierarchical navigation menus from a directory structure. It leverages the `directory` module to fetch and traverse a tree of directory entries, rendering them as nested HTML lists (`<ul>` and `<li>` elements) with configurable depth, filtering, and styling options.

This class is particularly useful for creating dynamic, multi-level navigation systems where entries may be expanded or collapsed, and where visual cues (icons, images, hover effects) enhance usability. The menu can be filtered to show only active branches, open branches, or a combination of both, making it adaptable to various navigation scenarios.

---

### Constants

| Name                          | Value | Description                                                                                     |
|-------------------------------|-------|-------------------------------------------------------------------------------------------------|
| `CMS_MENU_FILTER_NORMAL`      | `0`   | No filtering; all entries are displayed according to level and depth.                          |
| `CMS_MENU_FILTER_OPEN`        | `1`   | Only entries in currently open branches are displayed.                                         |
| `CMS_MENU_FILTER_ACTIVE`      | `2`   | Only entries in the active branch (path to the current page) are displayed.                    |
| `CMS_MENU_FILTER_ACTIVE_OPEN` | `3`   | Only entries in the active branch or currently open branches are displayed.                    |

---

### Properties

| Name            | Default/Initial Value | Description                                                                                     |
|-----------------|-----------------------|-------------------------------------------------------------------------------------------------|
| `$flexview`     | `NULL`                | Holds the `flexview` object from the `directory` module, which manages the directory tree.      |
| `$level`        | `0`                   | The starting indentation level of the menu.                                                    |
| `$depth`        | `NULL`                | Maximum depth of the menu relative to `$level`. If `NULL`, no depth limit is applied.          |
| `$filter`       | `CMS_MENU_FILTER_NORMAL` | Filter mode for the menu (see constants above).                                            |
| `$show_images`  | `NULL`                | Boolean flag indicating whether to display images for menu entries.                            |
| `$exclude`      | `NULL`                | Array of directory entry indices to exclude from the menu.                                     |
| `$start`        | `0`                   | Index of the first entry to display (for pagination or slicing).                               |
| `$end`          | `NULL`                | Index of the last entry to display (exclusive). If `NULL`, no upper limit is applied.          |

---

### Constructor: `__construct()`

#### Purpose
Initializes a new `menu` instance, configuring how the directory tree should be rendered as a navigation menu. It sets up the underlying `flexview` object, applies display templates, and configures visibility, filtering, and exclusion rules.

#### Parameters

| Name                | Type(s)               | Description                                                                                     |
|---------------------|-----------------------|-------------------------------------------------------------------------------------------------|
| `$index`            | `int`                 | The currently selected directory entry (default: `0`).                                         |
| `$base`             | `int`                 | The base directory entry from which the menu branch starts (default: `0`).                     |
| `$level`            | `int` or `string`     | Starting level for the menu. Can be absolute (e.g., `1`) or relative (e.g., `+1`, `-1`).       |
| `$depth`            | `int` or `NULL`       | Maximum depth of the menu. If `NULL`, no depth limit is applied (default: `NULL`).             |
| `$filter`           | `int` or `string`     | Filter mode for the menu. Can be a constant (`CMS_MENU_FILTER_*`) or string (`"open"`, etc.).  |
| `$show_icons`       | `bool`                | Whether to display icons next to menu entries (default: `FALSE`).                              |
| `$show_images`      | `bool`                | Whether to display images for menu entries (default: `FALSE`).                                 |
| `$show_description` | `bool`                | Whether to display descriptions below menu entries (default: `FALSE`).                         |
| `$show_hidden`      | `bool`                | Whether to include hidden directory entries (default: `FALSE`).                                |
| `$exclude`          | `array` or `string`   | Entries to exclude. Can be an array of indices or a space-separated string (default: `NULL`).  |
| `$start`            | `int`                 | Index of the first entry to display (default: `0`).                                            |
| `$length`           | `int` or `NULL`       | Number of entries to display. If `NULL`, all entries are displayed (default: `NULL`).          |

#### Return Value
None (constructor).

#### Inner Mechanisms
1. **Library Check**: Ensures the `directory` module is loaded; exits early if not.
2. **Flexview Setup**: Retrieves a `flexview` object from the `directory` module, optionally excluding hidden entries.
3. **Index Handling**: Adjusts the selected index to the nearest visible entry if hidden entries are excluded.
4. **Base Branch**: Sets the base entry to define the root of the menu branch.
5. **Display Template**: Configures the HTML template for each menu entry, including optional images, icons, and descriptions.
6. **Level Calculation**: Parses `$level` as absolute or relative (e.g., `+1` adds one level to the current path).
7. **Filter Parsing**: Converts string filter values (e.g., `"open"`) to their corresponding constants.
8. **Icon/Image Setup**: Enables icons or images if requested.
9. **Exclusion Handling**: Converts `$exclude` to an array if provided as a string.
10. **Pagination**: Sets `$start` and `$end` for slicing the menu entries.
11. **Display Trigger**: Invokes the `flexview`'s `show_custom()` method with the `show()` callback.

#### Usage Example
```php
// Create a menu starting at the root (index 0), showing 2 levels deep,
// only open branches, with icons and images enabled.
$menu = new \cms\menu(
    $index = 0,
    $base = 0,
    $level = 0,
    $depth = 2,
    $filter = "open",
    $show_icons = true,
    $show_images = true,
    $show_description = false,
    $show_hidden = false,
    $exclude = [42, 84], // Exclude entries with indices 42 and 84
    $start = 0,
    $length = 10
);
```
This creates a menu that starts at the root, shows only open branches, includes icons and images, and excludes specific entries.

---

### Method: `show()`

#### Purpose
Callback method used by the `flexview` object to render each directory entry as part of the menu. It handles the logic for filtering, exclusion, indentation, and HTML output.

#### Parameters

| Name               | Type                  | Description                                                                                     |
|--------------------|-----------------------|-------------------------------------------------------------------------------------------------|
| `$flexview_entry`  | `object`              | A `flexview` entry object containing data about the current directory entry.                   |

#### Return Value
- `void`: For entries that should be displayed.
- `bool` (`TRUE`): To skip subsequent entries (e.g., due to filtering or exclusion).

#### Inner Mechanisms
1. **Static Variables**: Tracks the current state across multiple calls:
   - `$count`: Number of entries processed.
   - `$indentation`: Current indentation level in the HTML output.
   - `$open`: Array mapping indentation levels to whether the branch is open.
   - `$instance`: Unique identifier for the menu instance (used for HTML IDs).
2. **Event Handling**: Processes three types of events from `flexview`:
   - `CMS_FLEXVIEW_ENTRY_TYPE_BASE`: Resets state before processing entries.
   - `CMS_FLEXVIEW_ENTRY_TYPE_ENTRY`: Renders an individual entry if it passes filters and exclusions.
   - `CMS_FLEXVIEW_ENTRY_TYPE_END`: Closes all open HTML tags after processing.
3. **Filtering Logic**:
   - **Active/Open Filter**: Skips entries not in open or active branches if the filter is set.
   - **Exclusion**: Skips entries listed in `$exclude`.
   - **Visibility**: Checks if the entry is within the specified `$level` and `$depth`, and within the `$start`/`$end` range.
4. **HTML Output**:
   - Opens `<ul>` and `<li>` tags as needed when increasing indentation.
   - Closes tags when decreasing indentation.
   - Uses the `flexview`'s `display()` method to render the entry's HTML.
5. **Early Termination**: Returns `TRUE` to skip subsequent entries if the filter condition is not met.

#### Usage Example
This method is not called directly but is passed to `flexview->show_custom()` in the constructor. The following demonstrates how the menu is rendered:
```php
// Assuming $menu is an instance of \cms\menu
$menu->flexview->show_custom([$menu, "show"]);
// The menu is output directly to the buffer (e.g., <ul><li>...</li></ul>).
```

---

### Method: `_ul_li()`

#### Purpose
Outputs the opening tags for a new sublist (`<ul>`) and its first list item (`<li>`), including an HTML ID for styling or scripting.

#### Parameters

| Name        | Type    | Description                                                                                     |
|-------------|---------|-------------------------------------------------------------------------------------------------|
| `$instance` | `int`   | Unique identifier for the menu instance.                                                        |
| `$index`    | `int`   | Index of the current directory entry.                                                           |
| `$open`     | `bool`  | Whether the current branch is open.                                                             |

#### Return Value
None (outputs HTML directly).

#### Inner Mechanisms
1. Generates a unique HTML ID for the `<li>` element (e.g., `menu1-42`).
2. Calls `_style()` to inject CSS for background images (if enabled).
3. Outputs the opening `<ul><li id="...">` tags.

#### Usage Example
This method is called internally by `show()` when increasing indentation:
```php
// Output: <ul><li id="menu1-42">
$this->_ul_li(1, 42, true);
```

---

### Method: `_li()`

#### Purpose
Outputs the opening tag for a new list item (`<li>`) at the same indentation level, including an HTML ID.

#### Parameters

| Name        | Type    | Description                                                                                     |
|-------------|---------|-------------------------------------------------------------------------------------------------|
| `$instance` | `int`   | Unique identifier for the menu instance.                                                        |
| `$index`    | `int`   | Index of the current directory entry.                                                           |
| `$open`     | `bool`  | Whether the current branch is open.                                                             |

#### Return Value
None (outputs HTML directly).

#### Inner Mechanisms
1. Generates a unique HTML ID for the `<li>` element.
2. Calls `_style()` to inject CSS for background images (if enabled).
3. Outputs the opening `<li id="...">` tag.

#### Usage Example
This method is called internally by `show()` when continuing at the same indentation level:
```php
// Output: <li id="menu1-42">
$this->_li(1, 42, true);
```

---

### Method: `_style()`

#### Purpose
Generates and outputs CSS rules for background images and hover effects for menu items. This method is conditionally enabled based on `$show_images`.

#### Parameters

| Name     | Type    | Description                                                                                     |
|----------|---------|-------------------------------------------------------------------------------------------------|
| `$id`    | `string`| HTML ID of the `<li>` element.                                                                  |
| `$index` | `int`   | Index of the current directory entry.                                                           |
| `$open`  | `bool`  | Whether the current branch is open.                                                             |

#### Return Value
None (outputs CSS directly).

#### Inner Mechanisms
1. **Early Exit**: Returns immediately if `$show_images` is `FALSE`.
2. **Image URLs**: Retrieves the default and hover image URLs for the entry.
3. **CSS Generation**: Outputs `<style>` tags with rules for:
   - Default background image (e.g., `#menu1-42 { BACKGROUND-IMAGE: url("..."); }`).
   - Hover background image (e.g., `#menu1-42:hover { BACKGROUND-IMAGE: url("..."); }`).

#### Usage Example
This method is called internally by `_ul_li()` and `_li()`:
```php
// Output (if $show_images is true):
// <style>#menu1-42{BACKGROUND-IMAGE:url("path/to/image.png");}</style>
$this->_style("menu1-42", 42, true);
```


<!-- HASH:90ad92e21385e81b7f2c12122f469e26 -->
