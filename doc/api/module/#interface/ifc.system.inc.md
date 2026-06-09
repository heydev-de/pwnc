# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.system.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.system.inc)

- **Version:** `26.6.9.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## System Interface Module (`ifc.system.inc`)

This file implements the **System Interface** for the PWNC Web Platform, providing a user interface for managing hierarchical data structures (e.g., configuration trees, content taxonomies, or module settings). It enables **CRUD operations** (Create, Read, Update, Delete) on nested data entries, with support for **copying, cutting, and pasting** subtrees. The interface integrates with the platform's **FlexView** component for tree visualization and **IFC (Interface Controller)** for form handling.

---

### Core Responsibilities

1. **Data Management**
   - Load, modify, and save hierarchical data from `.dat` files using the `data` class.
   - Support for **containers** (folders) and **leaf nodes** (entries).
   - Recursive operations (e.g., delete entire subtrees).

2. **User Interface**
   - Render a **tree view** of the data structure (via `flexview`).
   - Provide **contextual actions** (add, copy, cut, delete) with dynamic form generation.
   - Handle **multi-selection** and **target-based operations** (e.g., paste into a container).

3. **State Management**
   - Track the **current object**, **selection list**, and **clipboard status** (copy/cut).
   - Persist changes to disk and update the UI accordingly.

4. **Message Handling**
   - Process **IFC messages** (e.g., `save`, `add`, `delete`) to trigger actions.
   - Return **response codes** (`CMS_MSG_DONE`, `CMS_MSG_ERROR`) to the frontend.

---

### Key Components

#### 1. **Data Initialization**
| Variable       | Type      | Description                                                                 |
|----------------|-----------|-----------------------------------------------------------------------------|
| `$file`        | `string`  | Path to the data file (defaults to `CMS_DATA_PATH . "#system/system.dat`). |
| `$data`        | `data`    | Instance of the `data` class loaded from `$file`.                           |
| `$object`      | `string`  | Current selected object (key in the data structure).                        |
| `$list`        | `array`   | List of selected objects (for batch operations).                            |
| `$target`      | `string`  | Target container for copy/cut operations.                                   |
| `$status`      | `string`  | Clipboard status (`"copy"` or `"cut"`).                                     |

**Mechanism**:
- The file is loaded into a `data` object, which provides methods for traversal (`move`), modification (`set`, `del`), and persistence (`save`).
- If no `$object` is specified, the root (`0`) is selected.

---

#### 2. **Message Handling**
The module processes **IFC messages** (passed via `CMS_IFC_MESSAGE`) to perform actions. Each case in the `switch` statement corresponds to a user-triggered command.

##### ### `select`
**Purpose**: Update the current selection.
**Parameters**:
| Parameter      | Type      | Description                          |
|----------------|-----------|--------------------------------------|
| `$ifc_param`   | `string`  | Key of the object to select.         |

**Mechanism**:
- Sets `$object` to `$ifc_param` and resets `$list` to include only the selected object.
- Clears the clipboard status (`$status = NULL`).

**Usage Example**:
```javascript
// Frontend: Trigger selection via JavaScript
ifc_post('select', 'some_key');
```

---

##### ### `save`
**Purpose**: Save changes to the data structure.
**Parameters**:
| Parameter          | Type      | Description                                                                 |
|--------------------|-----------|-----------------------------------------------------------------------------|
| `$ifc_param1`      | `string`  | New attribute name (if adding a new attribute).                             |
| `$ifc_param2`      | `string`  | New attribute value (if adding a new attribute).                            |
| `$attribute{N}`    | `string`  | Dynamically generated form fields for existing attributes (name/value pairs). |

**Mechanism**:
1. Adds a new attribute if `$ifc_param1` is provided.
2. Updates existing attributes from form fields (e.g., `attribute3`, `attribute4`).
3. Saves the data to disk. If the object has no attributes left, it is deleted.
4. Returns `CMS_MSG_DONE` on success or `CMS_MSG_ERROR` on failure.

**Usage Example**:
```php
// Frontend form submission (simplified)
$ifc->set("New Attribute", "text 50 b", "attr_name");
$ifc->set("New Value", "textarea 50x4 bl", "attr_value");
$ifc->param("attribute3", "existing_attr");
$ifc->set("existing_attr", "textarea 50x4 bl", "updated_value");
```

---

##### ### `add` / `add_target`
**Purpose**: Add a new entry or container to the data structure.
**Parameters**:
| Parameter      | Type      | Description                                                                 |
|----------------|-----------|-----------------------------------------------------------------------------|
| `$ifc_param`   | `string`  | Target parent container (defaults to `$object`).                            |
| `$ifc_param1`  | `string`  | Key for the new entry (auto-generated if empty).                            |
| `$ifc_param2`  | `bool`    | If `TRUE`, creates a container (folder); otherwise, creates a leaf node.    |

**Mechanism**:
1. Renders a form with fields for:
   - **Key**: Unique identifier for the new entry.
   - **Container Flag**: Checkbox to create a container.
   - **Target**: Tree view to select the parent container.
2. On submission (`add_insert`/`add_append`), generates a unique key if none is provided and inserts the new entry.

**Usage Example**:
```javascript
// Frontend: Trigger "add" action
ifc_post('add', 'parent_key');
```

---

##### ### `copy` / `cut` / `target` / `copy_insert` / `copy_append` / `cut_insert` / `cut_append`
**Purpose**: Copy or move subtrees within the data structure.
**Parameters**:
| Parameter      | Type      | Description                                                                 |
|----------------|-----------|-----------------------------------------------------------------------------|
| `$list`        | `array`   | List of objects to copy/cut.                                                |
| `$ifc_param`   | `string`  | Target container for pasting.                                               |

**Mechanism**:
1. **Copy/Cut**: Stores selected objects in a temporary buffer (using a clone of `$data` to avoid modifying the original).
2. **Paste**: Inserts or appends the buffered objects to the target container.
3. Handles edge cases (e.g., pasting a container into itself).

**Usage Example**:
```javascript
// Frontend: Copy and paste
ifc_post('copy', ''); // Copy selected objects
ifc_post('target', 'target_key'); // Set target container
ifc_post('copy_insert', 'target_key'); // Paste into target
```

---

##### ### `delete`
**Purpose**: Delete selected objects.
**Parameters**:
| Parameter      | Type      | Description                                                                 |
|----------------|-----------|-----------------------------------------------------------------------------|
| `$list`        | `array`   | List of objects to delete.                                                  |

**Mechanism**:
1. Deletes all objects in `$list` and their subtrees.
2. On success, selects the nearest existing ancestor of the deleted objects.

**Usage Example**:
```javascript
// Frontend: Delete selected objects
ifc_post('delete', '');
```

---

##### ### `filemanager`
**Purpose**: Track recently accessed files (e.g., for the file manager).
**Mechanism**:
- Uses the `plist` class to maintain a list of the 10 most recently accessed files.

---

#### 3. **Main Display**
Renders the UI with three main sections:
1. **Tree View** (`flexview`):
   - Displays the hierarchical data structure.
   - Supports checkboxes for multi-selection.
   - Icons distinguish containers (`+container`) from leaf nodes.
2. **Clipboard Target** (if `$status` is set):
   - Shows the target container for copy/cut operations.
3. **Data Editor** (if `$object` is selected):
   - Lists all attributes of the selected object.
   - Provides fields to add/edit attributes.

**Key UI Elements**:
| Element               | Type          | Description                                                                 |
|-----------------------|---------------|-----------------------------------------------------------------------------|
| `$menu`               | `array`       | Contextual actions (add, copy, cut, delete).                                |
| `flexview->show_tree` | `method`      | Renders the tree view with clickable nodes.                                 |
| `ifc->set()`          | `method`      | Generates form fields for attributes.                                       |

**Usage Example**:
```php
// Render the interface
$ifc = new ifc($ifc_response, $ifc_page, $menu, [
    "object" => $object,
    "status" => $status,
    "file" => $file
]);
$flexview = new flexview();
$flexview->import_data($data);
$flexview->show_tree($object, "javascript:ifc_post('select','%index%');", "name");
```

---

### Helper Functions
| Function               | Purpose                                                                                     |
|------------------------|---------------------------------------------------------------------------------------------|
| `ifc_inactive()`       | Displays an "inactive" message if a required library (e.g., `flexview`) fails to load.      |
| `ifc_permission()`     | Checks user permissions (e.g., `CMS_L_ACCESS`).                                             |
| `ifc_table_open()`     | Opens a styled HTML table for the interface.                                                |
| `ifc_table_close()`    | Closes the HTML table.                                                                      |
| `unique_id()`          | Generates a unique key for new entries (not shown in code but referenced).                  |

---

### Constants
| Constant               | Value               | Description                                                                 |
|------------------------|---------------------|-----------------------------------------------------------------------------|
| `CMS_L_ACCESS`         | (Localized)         | Permission label for accessing the system interface.                       |
| `CMS_L_COMMAND_*`      | (Localized)         | Labels for UI actions (e.g., `CMS_L_COMMAND_ADD` = "Add").                  |
| `CMS_L_IFC_SYSTEM_*`   | (Localized)         | Labels for form fields (e.g., `CMS_L_IFC_SYSTEM_001` = "New Attribute").    |
| `CMS_MSG_DONE`         | `1`                 | Success response code.                                                      |
| `CMS_MSG_ERROR`        | `0`                 | Error response code.                                                        |

---

### Usage Scenarios

#### 1. **Managing a Configuration Tree**
**Scenario**: A developer wants to edit a nested configuration (e.g., module settings).
**Steps**:
1. Navigate to the system interface (`/system/interface`).
2. Use the tree view to select a configuration node.
3. Edit attributes in the data editor and click "Save".
4. Use "Add" to create new entries or containers.

**Example**:
```php
// Programmatically add a new setting
$data = new data(CMS_DATA_PATH . "#system/module_settings.dat");
$data->set("timeout", "30", "api_settings");
$data->save();
```

---

#### 2. **Copying a Subtree**
**Scenario**: Reuse a set of settings in another part of the tree.
**Steps**:
1. Select the source subtree and click "Copy".
2. Navigate to the target container and click "Paste (Insert)".

**Example**:
```javascript
// Frontend: Copy and paste via JavaScript
ifc_post('copy', '');
ifc_post('target', 'target_container');
ifc_post('copy_insert', 'target_container');
```

---

#### 3. **Deleting Obsolete Entries**
**Scenario**: Remove deprecated configuration entries.
**Steps**:
1. Select the entries in the tree view (checkboxes).
2. Click "Delete Selected".

**Example**:
```javascript
// Frontend: Delete via JavaScript
ifc_post('delete', '');
```

---

### Error Handling
- **Missing Data File**: Falls back to `CMS_DATA_PATH . "#system/system.dat"`.
- **Invalid Object**: Resets to the root (`0`) if the selected object does not exist.
- **Save Failures**: Returns `CMS_MSG_ERROR` and retains unsaved changes in the UI.
- **Permission Denied**: Redirects or displays an error via `ifc_permission()`.

---

### Dependencies
| Dependency       | Purpose                                                                                     |
|------------------|---------------------------------------------------------------------------------------------|
| `flexview`       | Renders the tree view and target selection UI.                                              |
| `ifc`            | Generates forms and handles IFC messages.                                                   |
| `data`           | Manages hierarchical data structures (load, save, traverse).                                |
| `plist`          | Tracks recently accessed files (for `filemanager`).                                         |

---

### Performance Considerations
- **Caching**: Uses `cms_cache()` to temporarily store the data structure in RAM.
- **Cloning**: Uses a clone of `$data` for copy/cut operations to avoid modifying the original until save.
- **Batch Operations**: Processes deletions and pastes in bulk to minimize disk I/O.


<!-- HASH:483e63ead4ae8ecd5574a07f927fda35 -->
