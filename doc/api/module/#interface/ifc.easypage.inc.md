# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.easypage.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.easypage.inc)

- **Version:** `26.6.9.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## EasyPage Interface Module

The `ifc.easypage.inc` file provides a user interface for managing hierarchical directory structures (navigation trees) and their associated content entries. It integrates with PWNC's content, directory, flexview, and template modules to offer a drag-and-drop interface for organizing pages, containers, and content links.

This module handles:
- Directory tree visualization and manipulation
- Content creation and linking
- Template selection and preview
- Metadata management (titles, descriptions, keywords)
- Drag-and-drop reordering and deletion

---

### Constants and Dependencies

| Name | Value | Description |
|------|-------|-------------|
| Required Libraries | `content`, `directory`, `flexview`, `template` | Core modules loaded at initialization |
| Permission | `CMS_L_ACCESS` | Minimum access level required to use this interface |
| Cache Keys | `directory.{USER}.object`, `directory.{USER}.hidden`, `template.{USER}.page` | User-specific persistent settings |

---

### Message Handling

The interface responds to several message types that drive its behavior:

| Message | Parameters | Description |
|---------|------------|-------------|
| `select` | `$ifc_param` (object ID) | Selects a directory entry for viewing/editing |
| `add` | `$ifc_param1` (name), `$ifc_param2` (hidden flag), `$ifc_param3` (template) | Prepares to add a new entry |
| `add_target` | `$ifc_param` (target parent) | Shows target selection interface for new entry |
| `add_insert`/`add_append` | `$ifc_param` (target index) | Inserts/appends new content at target |
| `save` | `$ifc_param1` (name), `$ifc_param2` (hidden), `$ifc_param3` (title), `$ifc_param4` (description), `$ifc_param5` (keywords) | Saves metadata for selected entry |
| `delete` | `$ifc_param` (object ID) | Deletes an entry and its children |
| `insert`/`append` | `$ifc_param` (source,target) | Moves an entry to new position |
| `template_preview` | `$object` (template name) | Shows template preview |

---

### Core Functionality

#### Directory Tree Display
```php
$flexview->show_hierarchy(
    $object, // Current selected object
    "javascript:ifc_post('select','%index%');", // Click action
    "name", // Field to display
    NULL, // No special marking
    directory_get_type(), // Icon function
    "", // No base path
    "directory_easypage_event" // Drag-and-drop handler
);
```

**Purpose**: Renders the directory hierarchy with interactive elements.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$index` | int | Starting object ID (0 for root) |
| `$action` | string | JavaScript action for item clicks |
| `$name_key` | string | Data field containing display names |
| `$mark` | mixed | Special marking function (NULL for none) |
| `$icon` | callable | Function returning icon for each item |
| `$base` | string | Base path for relative URLs |
| `$dragdrop_event_function` | string | JavaScript function handling drag events |

**Usage Example**:
```php
// Display full directory tree starting from root
$flexview->show_hierarchy(
    0,
    "javascript:console.log('%index%');",
    "name",
    NULL,
    function($type) { return $type === "container" ? "folder" : "file"; },
    "",
    "myDragHandler"
);
```

---

#### Content Creation Flow
```php
case "add_insert":
    // Set name if none exists
    if (!language_get($ifc_param1, FALSE))
        $ifc_param1 = language_set($ifc_param1, CMS_L_UNKNOWN, FALSE);

    $content->writer = TRUE; // Temporary permission
    if ($index = $content->create($ifc_param1, $ifc_param3)) {
        // Additional publishing logic...
    }
```

**Purpose**: Creates new content entries with proper permissions and directory linking.

**Inner Mechanisms**:
1. Validates/sets default name using language system
2. Temporarily elevates permissions
3. Creates content with selected template
4. Publishes content and links to directory
5. Updates user preferences cache

**Usage Context**:
- Called when user confirms new content creation
- Handles both standalone pages and container entries
- Maintains referential integrity between directory and content

---

#### Metadata Management
```php
case "save":
    $directory->data->set($ifc_param1, $object, "name");
    $directory->data->set(isset($ifc_param2), $object, "hidden");

    if (streq(substr($url, 0, 10), "content://")) {
        // Update linked content metadata
        mysql_query("UPDATE " . CMS_DB_CONTENT . " SET ...");
    }
```

**Purpose**: Synchronizes metadata between directory entries and linked content.

**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | string | Display name |
| `$ifc_param2` | bool | Hidden flag |
| `$ifc_param3` | string | Page title |
| `$ifc_param4` | string | Description |
| `$ifc_param5` | string | Keywords |

**Return Values**:
- `CMS_MSG_DONE` on success
- `CMS_MSG_ERROR` on failure

**Usage Example**:
```php
// Update a page's metadata
$ifc_param1 = "New Page Title";
$ifc_param2 = FALSE; // Not hidden
$ifc_param3 = "SEO Optimized Title";
$ifc_param4 = "Detailed description for search engines";
$ifc_param5 = "pwnc, cms, web development";
ifc_post("save", $object); // Where $object is the directory entry ID
```

---

#### Recursive Deletion
```php
case "delete":
    // Collect all content links in branch
    while ($key = $directory->data->move("next")) {
        if (streq(substr($url, 0, 10), "content://")) {
            $array_document[$index] = TRUE;
        }
    }

    // Delete content entries
    foreach ($array_document AS $key => $_)
        $error |= ! $content->delete($key, TRUE, TRUE);
```

**Purpose**: Safely deletes directory entries and all associated content.

**Inner Mechanisms**:
1. Traverses directory tree to collect all content links
2. Verifies no remaining references exist
3. Deletes content entries with publisher permissions
4. Removes schedule entries
5. Updates directory structure

**Usage Context**:
- When user confirms deletion in trash bin
- Handles both containers and content links
- Maintains database consistency

---

### JavaScript Integration

#### Template Preview
```javascript
function easypage_template_preview() {
    var value = ifc_get("ifc_param3");
    if (value) load_page("<?php echo q(cms_url([...])); ?>");
}
```

**Purpose**: Provides live preview of selected templates.

**Parameters**:
- Uses `ifc_param3` (template name) from form
- Constructs preview URL with current context

**Usage Example**:
```javascript
// Add to any template selection interface
document.getElementById("preview-btn").onclick = easypage_template_preview;
```

---

#### Drag-and-Drop Handling
```javascript
function directory_easypage_event(event, source, target) {
    if (source_id === target_id) return;

    switch (event) {
    case "dropon":
        if (target.id === "trashbin") ifc_post("delete", source_id);
        else if (source_id !== target_id) ifc_post(action, source_id + "," + target_id);
    }
}
```

**Purpose**: Handles drag-and-drop operations for directory reorganization.

**Event Types**:
| Event | Parameters | Description |
|-------|------------|-------------|
| `dropon` | `source`, `target` | Triggered when item is dropped on target |

**Usage Context**:
- Registered via `dd_register()` for drag sources
- Handles both reordering and deletion
- Maintains visual feedback during operations

---

### Display Components

#### Main Interface Structure
```php
echo("<table class=\"layout\">" .
    "<colgroup>" .
    "<col>" .
    ($object ? "<col style=\"WIDTH:0\">" : "") .
    "</colgroup>" .
    "<tr>" .
    "<td>"); // Directory tree
    if ($object) echo("<td>"); // Metadata panel
```

**Purpose**: Creates responsive two-panel layout showing:
1. Directory hierarchy (left)
2. Selected item metadata (right, when applicable)

**Usage Example**:
```php
// Custom interface with additional panels
echo("<table class=\"layout\">
    <tr>
        <td>"); // Directory
        if ($show_details) echo("<td>"); // Metadata
        if ($show_preview) echo("<td>"); // Preview
    </tr>
</table>");
```

---

### Helper Functions

#### `directory_flexview_display_function()`
**Purpose**: Custom display formatter for directory entries in flexview.

**Parameters**:
- Receives raw data from directory module
- Returns formatted HTML for each entry

**Usage Context**:
- Used by `show_hierarchy()` and `show_target()`
- Handles special formatting for different entry types

#### `content_template_select()`
**Purpose**: Generates `<select>` element with available templates.

**Return Value**: HTML string containing template options

**Usage Example**:
```php
// Add template selection to any form
$form->addField(
    "Template",
    "select",
    content_template_select(),
    $current_template
);
```

---

### Error Handling

The module employs several error handling patterns:

1. **Permission Checks**:
```php
ifc_permission(["" => CMS_L_ACCESS]);
if (! $content->enabled) ifc_inactive($ifc_page);
```

2. **Operation Results**:
```php
if ($directory->save()) {
    $ifc_response = CMS_MSG_DONE;
} else {
    $ifc_response = CMS_MSG_ERROR;
}
```

3. **Recursive Safety**:
```php
// Prevent circular references
if (! $directory->data->is_child($ifc_param, $value)) {
    // Perform move operation
}
```

---

### Typical Usage Scenarios

1. **Creating a New Page**:
```php
// 1. User clicks "Add" button
// 2. System shows target selection interface (add_target)
// 3. User selects parent and template
// 4. System creates content and links to directory (add_insert/add_append)
// 5. New page appears in directory tree
```

2. **Reorganizing Navigation**:
```javascript
// User drags "About Us" page onto "Company Info" container
// System calls: ifc_post("append", "123,456")
// Where 123 is source ID and 456 is target ID
```

3. **Updating Metadata**:
```php
// 1. User selects page in directory
// 2. Edits title, description, keywords
// 3. Clicks "Save"
// 4. System updates both directory entry and linked content
```

4. **Deleting Content**:
```javascript
// User drags page to trash bin
// System calls: ifc_post("delete", "123")
// Where 123 is the object ID to delete
// System recursively deletes all child content
```


<!-- HASH:568c51158bec2f73c494f1104502140a -->
