# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.insert.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.insert.inc)

- **Version:** `26.6.9.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Insert Interface Module (`ifc.insert.inc`)

The **Insert Interface Module** provides a user interface for managing reusable code snippets (inserts) within the PWNC Web Platform. It allows users to create, edit, delete, and assign these snippets to hierarchical container structures. The module integrates with the platform's data management system (`data` class) and interface framework (`ifc` class) to deliver a tree-based navigation and form-based editing experience.

---

## Core Functionality

### Overview
This file handles two primary interface contexts:
1. **Main Display**: A tree view of container objects with options to add, delete, or assign inserts.
2. **Message Handling**: Processes user actions (e.g., `insert`, `add`, `delete`, `save`) and renders sub-displays for editing inserts or container properties.

The module relies on the following key components:
- **`data` class**: Manages structured data storage (e.g., `#system/insert` for containers, `#system/insert.code` for snippets).
- **`ifc` class**: Renders forms, tables, and interactive controls.
- **`flexview` class**: Displays hierarchical data as a navigable tree.

---

## Constants and Variables

| Name               | Value/Default       | Description                                                                 |
|--------------------|---------------------|-----------------------------------------------------------------------------|
| `$object`          | `NULL`              | Currently selected container object (e.g., `container.subcontainer`).       |
| `$list`            | `[]`                | List of selected objects (for batch operations).                            |
| `$insert_object`   | `NULL`              | Currently selected insert snippet (e.g., `snippet_key`).                    |
| `$ifc_response`    | `NULL`              | Response code for interface feedback (e.g., `CMS_MSG_DONE`, `CMS_MSG_ERROR`).|
| `$data`            | `data` instance     | Data handler for `#system/insert` or `#system/insert.code`.                 |
| `$menu`            | `[]`                | Associative array of menu items (label => action).                          |

---

## Message Handling

### Switch: `CMS_IFC_MESSAGE`
Routes interface actions to specific sub-displays or operations.

#### **Case: `insert` (and variants)**
Handles insert snippet management (CRUD operations).

##### **Sub-Case: `insert_select`**
**Purpose**: Selects an insert snippet for editing.
**Parameters**:
- `$ifc_param`: Key of the snippet to select.
**Mechanism**:
- Sets `$insert_object` to the provided key.
**Usage**:
- Triggered when a user clicks a snippet in the list.

---

##### **Sub-Case: `insert_display`**
**Purpose**: Renders a preview of the selected snippet.
**Parameters**:
- `$insert_object`: Key of the snippet to preview.
**Mechanism**:
- Retrieves the snippet's `code` and `html` flag from `#system/insert.code`.
- If `html` is `TRUE`, renders the code as-is; otherwise, processes it as plain text via `parse_text()`.
- Exits to prevent further output.
**Usage**:
```php
// Example: Preview a snippet named "alert_box"
load_page(cms_url([
    "ifc_page" => CMS_IFC_PAGE,
    "ifc_message" => "insert_display",
    "insert_object" => "alert_box"
]));
```

---

##### **Sub-Case: `insert_add`**
**Purpose**: Creates a new snippet with a default name.
**Mechanism**:
- Buffers a new entry with the name `CMS_L_IFC_INSERT_002` (localized "New Insert").
- Appends the entry to `#system/insert.code` and saves.
- Sets `$insert_object` to the new key and returns `CMS_MSG_DONE` on success.
**Usage**:
- Triggered via the "Add" button in the insert management interface.

---

##### **Sub-Case: `insert_save`**
**Purpose**: Saves changes to a snippet's name, HTML flag, or code.
**Parameters**:
- `$ifc_param1`: New name (falls back to existing name if empty).
- `$ifc_param2`: HTML flag (`TRUE`/`FALSE`).
- `$ifc_param3`: Code content.
**Mechanism**:
- Updates the snippet's properties in `#system/insert.code`.
- Returns `CMS_MSG_DONE` on success.
**Usage**:
```php
// Example: Save a snippet with updated code
ifc_post("insert_save", [
    "snippet_key",  // $ifc_param1 (name)
    TRUE,           // $ifc_param2 (HTML flag)
    "<div>Updated content</div>"  // $ifc_param3 (code)
]);
```

---

##### **Sub-Case: `insert_delete`**
**Purpose**: Deletes selected snippets and cleans up references.
**Parameters**:
- `$_list`: Array of snippet keys to delete.
**Mechanism**:
- Deletes each snippet from `#system/insert.code`.
- Removes references to deleted snippets from `#system/insert`.
- Deselects `$insert_object` if deleted.
**Usage**:
- Triggered via the "Delete Selected" button.

---

#### **Case: `select`**
**Purpose**: Selects a container object for editing.
**Parameters**:
- `$ifc_param`: Key of the container to select.
**Mechanism**:
- Updates `$object` and `$list` to the provided key.
**Usage**:
- Triggered when a user clicks a container in the tree view.

---

#### **Case: `_add` / `add`**
**Purpose**: Adds a new container object to the hierarchy.
**Parameters**:
- `$ifc_param1`: Name/path of the new container (e.g., `parent.child`).
**Mechanism**:
- Splits the path into segments and creates missing containers recursively.
- Updates `$object` and `$list` to the new container.
**Usage**:
```php
// Example: Add a container "forms.contact"
ifc_post("add", ["forms.contact"]);
```

---

#### **Case: `delete`**
**Purpose**: Deletes selected container objects.
**Parameters**:
- `$list`: Array of container keys to delete.
**Mechanism**:
- Deletes each container and its children from `#system/insert`.
- Falls back to the nearest existing parent in the hierarchy.
**Usage**:
- Triggered via the "Delete Selected" button in the main interface.

---

#### **Case: `save`**
**Purpose**: Assigns an insert snippet to a container.
**Parameters**:
- `$insert_object`: Key of the snippet to assign.
- `$object`: Target container key.
**Mechanism**:
- Updates the container's `insert` property in `#system/insert`.
**Usage**:
```php
// Example: Assign "alert_box" to "notifications"
ifc_post("save", ["alert_box", "notifications"]);
```

---

## Main Display

### Tree View (`flexview`)
**Purpose**: Renders a navigable tree of container objects.
**Mechanism**:
- Uses `flexview` to display `#system/insert` data as a collapsible tree.
- Encodes keys with `qr()` for URL safety.
- Highlights the selected container (`$object`).
**Usage**:
- Default view when accessing the insert interface.

---

### Container Settings
**Purpose**: Edits properties of the selected container.
**Fields**:
1. **Insert Selection**: Dropdown of available snippets (from `#system/insert.code`).
2. **Save Button**: Assigns the selected snippet to the container.
**Mechanism**:
- Populates the dropdown with snippet names, deduplicating via numeric suffixes (e.g., "Alert (1)").
- Displays a "Show" button if a snippet is assigned.

---

## Utility Functions

### `ifc_inactive($page)`
**Purpose**: Terminates the interface if a required library fails to load.
**Parameters**:
- `$page`: Page identifier to redirect to.
**Usage**:
```php
if (!cms_load("flexview")) ifc_inactive($ifc_page);
```

---

### `ifc_permission($permissions)`
**Purpose**: Checks user permissions for the interface.
**Parameters**:
- `$permissions`: Associative array of required permissions (e.g., `["" => CMS_L_ACCESS]`).
**Usage**:
```php
ifc_permission(["" => CMS_L_ACCESS]);
```

---

### `ifc_table_open()` / `ifc_table_close()`
**Purpose**: Outputs standardized table headers/footers for interface consistency.
**Usage**:
```php
ifc_table_open();
echo("<tr><th>Name</th></tr>");
ifc_table_close();
```

---

### `ifc_varied()`
**Purpose**: Returns a CSS class (`"varied"`) for alternating row colors in tables.
**Return Value**: String (`" class=\"varied\""` or empty).
**Usage**:
```php
echo("<tr" . ifc_varied() . "><td>Row content</td></tr>");
```

---

## Example Workflow

### Creating and Assigning an Insert Snippet
1. **Add a Snippet**:
   ```php
   // Trigger the "Add" action
   load_page(cms_url([
       "ifc_page" => CMS_IFC_PAGE,
       "ifc_message" => "insert_add"
   ]));
   ```
2. **Edit the Snippet**:
   - Set a name (e.g., "Alert Box").
   - Enable the HTML flag if the snippet contains markup.
   - Add code (e.g., `<div class="alert">Warning!</div>`).
   - Save via the "Save" button.
3. **Assign to a Container**:
   - Navigate to the target container in the tree view.
   - Select the snippet from the dropdown and click "Save".

### Deleting a Container
1. Select containers in the tree view.
2. Click "Delete Selected".
3. The interface automatically falls back to the nearest existing parent.


<!-- HASH:bb8647b5677e26ecfd503e712c8b9c62 -->
