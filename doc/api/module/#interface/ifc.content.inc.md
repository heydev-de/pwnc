# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.content.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.content.inc)

- **Version:** `26.6.9.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Content Module Interface (`ifc.content.inc`)

The `ifc.content.inc` file serves as the primary interface for the PWNC Web Platform's **Content Management System (CMS)**. It handles user interactions, permissions, content operations (create, edit, publish, etc.), and integrates with other modules like directories, templates, and pools.

---

## **Constants & Variables**

| Name | Value/Default | Description |
|------|---------------|-------------|
| `$user` | `CMS_USER` | Current user accessing the content module. |
| `$object` | `NULL` | Active content object index. |
| `$directory_object` | `NULL` | Active directory object index. |
| `$content` | `new content($user)` | Instance of the `content` class for managing content operations. |
| `$icon` | Array | Maps content status and type to corresponding icons. |
| `$status` | Array | Maps content status codes to human-readable labels. |
| `$type` | Array | Maps content type codes to human-readable labels. |

---

## **Message Handling & Sub-Displays**

The interface processes different actions via `CMS_IFC_MESSAGE`. Each case represents a distinct operation (e.g., `select`, `edit`, `publish`).

---

### **`select`**
**Purpose:**
Selects a content object and locates its topmost directory linkage.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Content object index. |

**Inner Mechanisms:**
1. Searches the directory for the content URL (`content://$object`).
2. Sets `$directory_object` to the topmost linked directory entry.
3. Clears cached directory object if not found.

**Usage Example:**
```php
// Select content object "123" and find its directory linkage.
$ifc_param = "123";
CMS_IFC_MESSAGE = "select";
include "module/#interface/ifc.content.inc";
```

---

### **`directory_select`**
**Purpose:**
Selects a directory object and resolves its linked content object.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Directory object index. |

**Inner Mechanisms:**
1. Resolves directory dereferencing (e.g., `directory://123`).
2. Verifies user permissions for the linked content object.
3. Sets `$object` to the content index if accessible.

**Usage Example:**
```php
// Select directory object "456" and resolve its linked content.
$ifc_param = "456";
CMS_IFC_MESSAGE = "directory_select";
include "module/#interface/ifc.content.inc";
```

---

### **`info`**
**Purpose:**
Displays metadata (title, description, status, etc.) of a content object.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$object` | `string` | Content object index. |

**Inner Mechanisms:**
1. Fetches content data from the database.
2. Renders a table with metadata (status, type, title, description, etc.).
3. Includes timestamps and comments from writers, editors, and publishers.

**Usage Example:**
```php
// Show info for content object "123".
$object = "123";
CMS_IFC_MESSAGE = "info";
include "module/#interface/ifc.content.inc";
```

---

### **`_meta` & `meta`**
**Purpose:**
Handles metadata editing (title, description, keywords, image, template) for a content object.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Content object index. |
| `$ifc_param1` | `string` | Title. |
| `$ifc_param2` | `string` | Description. |
| `$ifc_param3` | `string` | Keywords. |
| `$ifc_param4` | `string` | Image path. |
| `$ifc_param5` | `string` | Comment. |
| `$ifc_param6` | `string` | Template index. |

**Inner Mechanisms:**
1. `_meta`: Saves metadata changes to the database.
2. `meta`: Displays a form for editing metadata with a template preview option.

**Usage Example:**
```php
// Edit metadata for content object "123".
$object = "123";
CMS_IFC_MESSAGE = "meta";
include "module/#interface/ifc.content.inc";
```

---

### **`edit_range`, `edit_value`, `edit_plugin`, `edit_href`**
**Purpose:**
Provides interfaces for editing specific parts of a content object (text ranges, values, plugins, hyperlinks).

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$object` | `string` | Content object index. |
| `$range` | `string` | Range identifier in the document. |
| `$id` | `string` | Element ID. |

**Inner Mechanisms:**
1. Fetches the specified range/value from the content object.
2. Displays an editor (text, textarea, or URL selector) for modification.
3. Submits changes back to the parent window.

**Usage Example:**
```php
// Edit a text range in content object "123".
$object = "123";
$range = "tp-range-1";
CMS_IFC_MESSAGE = "edit_range";
include "module/#interface/ifc.content.inc";
```

---

### **`create` & `_create`**
**Purpose:**
Creates a new content object.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | Title. |
| `$ifc_param2` | `string` | Template index. |
| `$ifc_param3` | `string` | Comment. |

**Inner Mechanisms:**
1. `create`: Displays a form for entering title, template, and comment.
2. `_create`: Calls `$content->create()` to generate the object and sets `$object` to the new index.

**Usage Example:**
```php
// Create a new content object.
CMS_IFC_MESSAGE = "create";
include "module/#interface/ifc.content.inc";
```

---

### **`apply` & `_apply`**
**Purpose:**
Applies changes from the buffer to the active content object (immediately or scheduled).

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Content object index. |
| `$ifc_param1` | `int` | `0` (immediate) or `1` (scheduled). |
| `$ifc_param2` | `string` | Scheduled time (if `$ifc_param1 = 1`). |

**Inner Mechanisms:**
1. `apply`: Displays a form for selecting immediate or scheduled application.
2. `_apply`: Calls `$content->apply()` with the specified time.

**Usage Example:**
```php
// Apply changes to content object "123" immediately.
$object = "123";
CMS_IFC_MESSAGE = "apply";
include "module/#interface/ifc.content.inc";
```

---

### **`publish` & `_publish`**
**Purpose:**
Publishes a content object to a directory (immediately or scheduled).

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$object` | `string` | Content object index. |
| `$ifc_param` | `string` | Directory object index. |
| `$ifc_param1` | `string` | Title override. |
| `$ifc_param2` | `int` | `0` (immediate) or `1` (scheduled). |
| `$ifc_param3` | `string` | Scheduled publication time. |
| `$ifc_param4` | `int` | `0` (no withdrawal) or `1` (scheduled withdrawal). |
| `$ifc_param5` | `string` | Scheduled withdrawal time. |
| `$ifc_param6` | `string` | Publisher comment. |

**Inner Mechanisms:**
1. `publish`: Displays a form for selecting directory, title, publication time, and withdrawal time.
2. `_publish`: Calls `$content->publish()` with the specified parameters.

**Usage Example:**
```php
// Publish content object "123" to directory "456" immediately.
$object = "123";
$ifc_param = "456";
CMS_IFC_MESSAGE = "publish";
include "module/#interface/ifc.content.inc";
```

---

### **`pool` & Related Sub-Messages**
**Purpose:**
Manages content pool references (add, edit, delete, synchronize).

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$pool_object` | `string` | Pool object index. |
| `$pool_type` | `string` | Content type filter. |
| `$pool_language` | `string` | Language filter. |

**Inner Mechanisms:**
1. `pool`: Displays a list of pool references with options to add/edit/delete.
2. `pool_add`: Adds a new reference to the pool.
3. `pool_edit`: Edits an existing reference.
4. `pool_delete`: Deletes selected references.

**Usage Example:**
```php
// Display the content pool interface.
CMS_IFC_MESSAGE = "pool";
include "module/#interface/ifc.content.inc";
```

---

### **`rss` & Related Sub-Messages**
**Purpose:**
Manages RSS channels and assigns content objects to them.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$rss_object` | `string` | RSS channel index. |
| `$object` | `string` | Content object index. |

**Inner Mechanisms:**
1. `rss`: Displays a list of RSS channels with options to add/delete.
2. `rss_assign`: Assigns content objects to selected channels.

**Usage Example:**
```php
// Display the RSS channel management interface.
CMS_IFC_MESSAGE = "rss";
include "module/#interface/ifc.content.inc";
```

---

### **`configuration` & `_configuration`**
**Purpose:**
Configures extra content fields (label and ID).

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | Extra field label. |
| `$ifc_param2` | `string` | Extra field ID. |

**Inner Mechanisms:**
1. `configuration`: Displays a form for setting the extra field label and ID.
2. `_configuration`: Saves the configuration to the system settings.

**Usage Example:**
```php
// Configure the extra content field.
CMS_IFC_MESSAGE = "configuration";
include "module/#interface/ifc.content.inc";
```

---

### **`flag` & `_flag`**
**Purpose:**
Sets SEO-related flags (sitemap exclusion, meta robots).

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$object` | `string` | Content object index. |
| `$ifc_param1` | `bool` | Exclude from sitemap. |
| `$ifc_param2` | `bool` | `noindex` meta tag. |
| `$ifc_param3` | `bool` | `nofollow` meta tag. |

**Inner Mechanisms:**
1. `flag`: Displays checkboxes for setting flags.
2. `_flag`: Calls `$content->flag_set()` to update the flags.

**Usage Example:**
```php
// Set flags for content object "123".
$object = "123";
CMS_IFC_MESSAGE = "flag";
include "module/#interface/ifc.content.inc";
```

---

### **`analyze` & `analyze_span`**
**Purpose:**
Analyzes the content object's text (word frequency, markup quota, plain text).

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$object` | `string` | Content object index. |
| `$span` | `int` | Word span size for analysis (default: `0`). |

**Inner Mechanisms:**
1. Extracts plain text from the content object.
2. Tokenizes text and counts word spans.
3. Displays word frequency, markup quota, and source code.

**Usage Example:**
```php
// Analyze content object "123" with a word span of 2.
$object = "123";
$span = 2;
CMS_IFC_MESSAGE = "analyze_span";
include "module/#interface/ifc.content.inc";
```

---

### **`debug`**
**Purpose:**
Displays the parsed document structure for debugging (template operators only).

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$object` | `string` | Content object index. |

**Inner Mechanisms:**
1. Parses the content object's document and template.
2. Displays a tree view of the document structure (IDs, paths, types, values).

**Usage Example:**
```php
// Debug content object "123".
$object = "123";
CMS_IFC_MESSAGE = "debug";
include "module/#interface/ifc.content.inc";
```

---

## **Main Display**

**Purpose:**
Renders the primary content management interface with:
- User selection.
- Content filtering and searching.
- Directory navigation.
- Content object listing with actions (edit, publish, delete, etc.).

**Inner Mechanisms:**
1. **User List:** Displays available users with content access.
2. **Filter List:** Filters content by status (draft, document, publication).
3. **Search:** Searches across fields (title, description, keywords, etc.).
4. **Directory:** Shows a tree view of linked directories.
5. **Content Table:** Lists content objects with actions (edit, publish, delete, etc.).

**Usage Example:**
```php
// Display the main content management interface.
include "module/#interface/ifc.content.inc";
```


<!-- HASH:4a5c28c6d581c9971068c0d28ed61057 -->
