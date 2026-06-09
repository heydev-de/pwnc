# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.directory.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.directory.inc)

- **Version:** `26.6.9.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Directory Interface Module (`ifc.directory.inc`)

This file implements the **Directory Management Interface** for the PWNC Web Platform. It provides a hierarchical, drag-and-drop enabled UI for managing website navigation structures, including:

- **Directory entries** (containers and links)
- **Directory types** (visual styling and behavior presets)
- **Multilingual canonical URLs**
- **Image assets** (button, hover, active states)
- **Dynamic/placeholder entries**

The interface is built on top of the `directory` and `flexview` modules, offering CRUD operations, tree manipulation (copy/cut/paste, sorting, cleaning), and type management.

---

### **Core Functionality by Message Type**

The interface processes messages via `CMS_IFC_MESSAGE`, triggering different workflows. Below is the documentation of each message handler.

---

## `init()`
**Purpose:**
Initializes the `$type_object` variable with the subtype of the currently selected directory object.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$type_object` | `mixed` (out) | Reference to store the subtype of the current object. |
| `$subtype` | `string` | Subtype value fetched from the directory data. |

**Inner Mechanisms:**
- Uses the `data` class to fetch the `#subtype` field of the current `$object`.
- Stores the result in `$type_object` for later use in type management.

**Usage Example:**
```php
init($type_object, (new data("#system/directory"))->get($object, "#subtype"));
```
*Initializes `$type_object` when loading the type management view.*

---

## `type` Message Handler
**Purpose:**
Displays a list of directory types (visual presets) and allows selection, addition, editing, and deletion.

**Parameters (via `$ifc_param` and `$_POST`):**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Selected type key (for `type_select`). |
| `$ifc_param1` | `string` | New name for a type (for `type_save`). |
| `$ifc_file1` | `string` | Uploaded file for the normal icon (for `type_save`). |
| `$ifc_file1_name` | `string` | Filename of `$ifc_file1`. |
| `$ifc_file2` | `string` | Uploaded file for the active icon (for `type_save`). |
| `$ifc_file2_name` | `string` | Filename of `$ifc_file2`. |
| `$ifc_param2` | `bool` | Flag to delete the normal icon (for `type_save`). |
| `$ifc_param3` | `bool` | Flag to delete the active icon (for `type_save`). |
| `list[]` | `array` | Array of type keys to delete (for `type_delete`). |

**Return/Output:**
- Renders a table of types with selection checkboxes.
- For a selected type, displays an edit form with name and icon upload fields.

**Inner Mechanisms:**
- **`type_add`:** Appends a new type with a default name (`CMS_L_IFC_DIRECTORY_017`).
- **`type_save`:**
  - Updates the type name.
  - Handles icon uploads/deletions for normal (`format`) and active (`+format`) states.
  - Validates file extensions (GIF, JPG, PNG, SVG, WEBP).
- **`type_delete`:**
  - Deletes selected types and their icons.
  - Removes references to deleted types from directory entries.
  - Adjusts the selected type if it was deleted.

**Usage Example:**
```php
// Add a new type
ifc_post("type_add");

// Save changes to a type
ifc_post("type_save", [
    "ifc_param1" => "New Type Name",
    "ifc_file1" => [/* uploaded file data */],
    "ifc_file2" => [/* uploaded file data */]
]);
```
*Manages visual presets for directory entries.*

---

## `select` Message Handler
**Purpose:**
Selects a directory object for editing.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Key of the object to select. |

**Inner Mechanisms:**
- Sets `$object` to the provided key, triggering a reload of the interface with the selected object.

**Usage Example:**
```php
ifc_post("select", "home");
```
*Selects the "home" directory entry for editing.*

---

## `save` Message Handler
**Purpose:**
Saves changes to a directory object, including multilingual canonical URLs.

**Parameters (via `$_POST`):**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | Name of the object. |
| `$ifc_param2` | `bool` | Hidden flag. |
| `$ifc_param3` | `bool` | Placeholder flag. |
| `$ifc_param4` | `bool` | Dynamic flag. |
| `$ifc_param5` | `string` | Description. |
| `$ifc_param6` | `string` | URL. |
| `$ifc_param7` | `string` | Path override. |
| `$ifc_param8` | `string` | Canonical URL (multilingual). |
| `$ifc_param9` | `string` | Subtype. |
| `$ifc_param10` | `string` | Image button asset key. |
| `$ifc_param11` | `string` | Image hover asset key. |
| `$ifc_param12` | `string` | Image active asset key. |

**Return/Output:**
- Updates the directory object in the data store.
- Returns `CMS_MSG_DONE` or `CMS_MSG_ERROR` via `$ifc_response`.

**Inner Mechanisms:**
- **Canonical URL Processing:**
  - Splits `$ifc_param8` into language-specific values.
  - Completes partial URLs (e.g., `/about` → `https://example.com/about`).
  - Reassembles the multilingual canonical string.
- **Directory Update:**
  - Uses the `directory` class to update all fields.

**Usage Example:**
```php
ifc_post("save", [
    "ifc_param1" => "About Us",
    "ifc_param6" => "/about",
    "ifc_param8" => "en:/about,de:/ueber-uns"
]);
```
*Saves a directory entry with multilingual canonical URLs.*

---

## `add` / `add_target` Message Handlers
**Purpose:**
Displays a form to add a new directory object, either as a root entry or as a child of an existing object.

**Parameters (via `$ifc_param` and `$_POST`):**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Target parent object key (for `add_target`). |
| `$ifc_param1` | `string` | Name of the new object. |
| `$ifc_param2` | `bool` | Hidden flag. |
| `$ifc_param3` | `bool` | Placeholder flag. |
| `$ifc_param4` | `bool` | Dynamic flag. |
| `$ifc_param5` | `string` | Description. |
| `$ifc_param6` | `string` | URL. |
| `$ifc_param7` | `string` | Subtype. |

**Return/Output:**
- Renders a form with fields for name, visibility, description, URL, and subtype.
- Includes a `flexview` panel to select the target parent object.

**Inner Mechanisms:**
- **Default Values:**
  - Loads cached values for `hidden`, `placeholder`, `dynamic`, and `subtype`.
  - Sets the name to `CMS_L_IFC_DIRECTORY_001` if empty.
- **URL Validation:**
  - Uses a modal dialog (`content/edit_href`) to validate external URLs.

**Usage Example:**
```php
ifc_post("add", "home"); // Add a child under "home"
```
*Displays the add form with "home" pre-selected as the parent.*

---

## `add_insert` / `add_append` Message Handlers
**Purpose:**
Inserts or appends a new directory object to the hierarchy.

**Parameters:**
Same as `add_target`, plus:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Target parent object key. |

**Return/Output:**
- Creates the new object and saves it to the directory.
- Updates the cache with the new object's properties.
- Returns `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Inner Mechanisms:**
- **Insert/Append Logic:**
  - `add_insert`: Places the new object before the target.
  - `add_append`: Places the new object after the target.
- **Multilingual Name Handling:**
  - Falls back to `CMS_L_UNKNOWN` if no name is provided.

**Usage Example:**
```php
ifc_post("add_insert", "home", [
    "ifc_param1" => "Products",
    "ifc_param6" => "/products"
]);
```
*Inserts a "Products" entry before the "home" object.*

---

## `copy_insert` / `copy_append` / `cut_insert` / `cut_append` Message Handlers
**Purpose:**
Copies or moves a directory subtree to a new location in the hierarchy.

**Parameters (via `$ifc_param`):**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Comma-separated string: `"source_key,target_key"`. |

**Return/Output:**
- Updates the directory hierarchy.
- Returns `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Inner Mechanisms:**
- **Copy vs. Cut:**
  - **Copy:** Duplicates the subtree.
  - **Cut:** Moves the subtree (deletes the original).
- **Circular Reference Check:**
  - Prevents moving a parent into its own child.
- **Buffer Handling:**
  - Uses the `data` class's buffer to temporarily store the subtree.

**Usage Example:**
```php
ifc_post("cut_append", "about,products");
```
*Moves the "about" subtree to become a child of "products."*

---

## `sort` Message Handler
**Purpose:**
Sorts the children of a directory object alphabetically by name.

**Parameters:**
None (uses `$object` as the parent key).

**Return/Output:**
- Sorts the subtree and saves the directory.
- Returns `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Inner Mechanisms:**
- Uses `data_sort()` to reorder children by the `name` field.

**Usage Example:**
```php
ifc_post("sort", "products");
```
*Sorts all children of the "products" directory entry.*

---

## `clean` Message Handler
**Purpose:**
Removes "dead-end" directory entries (non-placeholder objects without URLs) from a subtree.

**Parameters:**
None (uses `$object` as the root of the subtree to clean).

**Return/Output:**
- Deletes invalid entries and saves the directory.
- Returns `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Inner Mechanisms:**
- **Stack-Based Traversal:**
  - Uses a stack to track the path to the current object.
  - Deletes objects that are:
    - Not placeholders.
    - Have no URL.
    - Are beyond the last valid object in the path (`$limit`).
- **Selection Adjustment:**
  - Updates `$object` to the rightmost valid parent if the original selection is deleted.

**Usage Example:**
```php
ifc_post("clean", "products");
```
*Removes all non-placeholder, URL-less entries under "products."*

---

## `del` Message Handler
**Purpose:**
Deletes a directory object and its subtree.

**Parameters (via `$ifc_param`):**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Key of the object to delete. |

**Return/Output:**
- Deletes the object and saves the directory.
- Returns `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Inner Mechanisms:**
- **Selection Adjustment:**
  - Updates `$object` to the last valid parent in the path if the original selection is deleted.

**Usage Example:**
```php
ifc_post("del", "old-products");
```
*Deletes the "old-products" directory entry and its children.*

---

## **Main Display**
**Purpose:**
Renders the primary directory management interface, including:
- A hierarchical view of the directory.
- A trash bin for drag-and-drop deletion.
- An edit form for the selected object.

**Key Components:**
| Component | Description |
|-----------|-------------|
| **Canonical URL** | Displays the canonical URL of the selected object with a clickable link. |
| **Flexview Hierarchy** | Drag-and-drop enabled tree view of the directory. |
| **Trash Bin** | Drop target for deleting objects. |
| **Edit Form** | Fields for name, visibility, description, URL, canonical URL, images, and subtype. |

**Inner Mechanisms:**
- **Drag-and-Drop Events:**
  - Uses JavaScript functions (`directory_flexview_event`) to handle:
    - `dropon`: Move or delete objects.
    - `dropon_alt`: Copy objects.
- **Flexview Integration:**
  - Uses `directory_flexview_display_function` to render each node.
  - Encodes node data with `qr()` for safe JavaScript transmission.
- **Menu Actions:**
  - **Add:** Opens the add form.
  - **Sort:** Sorts the selected subtree.
  - **Clean:** Removes dead-end entries.
  - **Type:** Opens the type management interface.

**Usage Example:**
```php
// Render the directory interface
$flexview->show_hierarchy(
    $object,
    "javascript:ifc_post('select','%index%');",
    "name",
    NULL,
    directory_get_type(),
    "",
    "directory_flexview_event"
);
```
*Displays the directory hierarchy with drag-and-drop support.*

---

## **Helper Functions (Implicit)**
### `directory_get_type()`
**Purpose:**
Returns the icon path for a directory object based on its subtype.

**Parameters:**
None (uses the current object's subtype).

**Return Value:**
| Type | Description |
|------|-------------|
| `string` | URL to the icon image. |

**Usage Example:**
```php
$icon = directory_get_type();
```
*Fetches the icon for the current object's subtype.*

---

### `directory_get_type_select()`
**Purpose:**
Generates a `<select>` element for directory subtypes.

**Parameters:**
None.

**Return Value:**
| Type | Description |
|------|-------------|
| `string` | HTML `<select>` element with subtype options. |

**Usage Example:**
```php
$select = directory_get_type_select();
```
*Renders a dropdown for selecting a directory type.*

---

### `directory_get_canonical()`
**Purpose:**
Generates the canonical URL for a directory object.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$object` | `string` | Key of the directory object. |

**Return Value:**
| Type | Description |
|------|-------------|
| `string` | Canonical URL. |

**Usage Example:**
```php
$url = directory_get_canonical("home");
```
*Fetches the canonical URL for the "home" object.*

---

### `directory_flexview_display_function()`
**Purpose:**
Custom display function for `flexview` to render directory nodes.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | `string` | Object key. |
| `$data` | `data` | Data object containing directory entries. |
| `$mark` | `mixed` | Unused. |
| `$icon` | `string` | Icon URL. |

**Return Value:**
| Type | Description |
|------|-------------|
| `string` | HTML for the node. |

**Usage Example:**
```php
$flexview->set_display_function(__NAMESPACE__ . "\\directory_flexview_display_function");
```
*Configures `flexview` to use this function for rendering nodes.*


<!-- HASH:2c0bf219120b0bcf68ca0caec304f6bc -->
