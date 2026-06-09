# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.media.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.media.inc)

- **Version:** `26.6.9.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Media Interface Module (`ifc.media.inc`)

This file implements the **Media Interface** for the PWNC Web Platform, providing a user interface for managing media assets (upload, edit, delete, categorize, and preview). It integrates with the `media` and `media_type` libraries to handle media storage, retrieval, and type-specific rendering.

The interface supports:
- Single and bulk uploads
- External media linking
- Media replacement
- Category management
- Media type configuration
- Language-aware media selection

---

## Core Functionality

### Message Handling Switch (`CMS_IFC_MESSAGE`)
The interface processes user actions via a message-driven switch. Each case corresponds to a specific operation (e.g., `upload`, `edit`, `delete`).

---

## Key Methods and Workflows

### `select`
**Purpose**: Updates the selected media object in the current language context.
**Parameters**:
| Name       | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$object`  | string | Media identifier (logical address).  |
| `$ifc_param` | string | New object identifier.               |
| `$language` | string | Current language code.               |

**Mechanism**:
- Calls `language_set()` to update the object reference in the current language.
- Used when selecting a media item from the category list.

**Usage Example**:
```php
// Switch to a different media object in the current language
$object = language_set($object, "media/image123", $language);
```

---

### `select_language`
**Purpose**: Changes the active language for media selection.
**Parameters**:
| Name       | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$ifc_param` | string | Language code (e.g., `en`, `de`).    |

**Mechanism**:
- Updates the `$language` variable, triggering a UI refresh.

---

### `display`
**Purpose**: Renders a preview of the selected media in an iframe.
**Parameters**:
| Name       | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$object`  | string | Media identifier.                    |

**Return**: Exits script after rendering.

**Mechanism**:
1. Validates the object exists.
2. Caches the object for persistence.
3. Instantiates the `media` class and retrieves metadata (name, URL, size, MIME type).
4. Generates a preview using `media->parse()` with fixed dimensions (600x400).
5. Outputs an iframe with the preview and metadata.

**Usage Example**:
```php
// Triggered when a user clicks a media item in the list
cms_url(["ifc_page" => CMS_IFC_PAGE, "ifc_message" => "display", "object" => "media/image123"]);
```

---

### `upload` / `_upload`
**Purpose**: Handles single-file uploads.
**Parameters (Form Fields)**:
| Name            | Type   | Description                          |
|-----------------|--------|--------------------------------------|
| `$ifc_file1`    | file   | Uploaded file.                       |
| `$ifc_file1_name` | string | Original filename.                   |
| `$ifc_param1`   | string | Media name.                          |
| `$ifc_param2`   | string | Media type (e.g., `image/jpeg`).     |
| `$ifc_param3`   | string | Category.                            |
| `$ifc_param4`   | string | Custom filename (optional).          |

**Mechanism**:
1. **`upload`**: Renders a form with fields for name, file, type, and category.
2. **`_upload`**: Processes the uploaded file via `media->add()` and updates the object reference.

**Usage Example**:
```php
// After form submission, the file is stored and linked to the category
$media->add($uploadedFile, "logo.png", "Company Logo", "image/png", "Logos");
```

---

### `upload_multi` / `_upload_multi`
**Purpose**: Handles bulk file uploads.
**Parameters (Form Fields)**:
| Name            | Type     | Description                          |
|-----------------|----------|--------------------------------------|
| `$ifc_file1[]`  | file[]   | Array of uploaded files.             |
| `$ifc_file1_name[]` | string[] | Array of original filenames.     |
| `$ifc_param1`   | string   | Media type.                          |
| `$ifc_param2`   | string   | Category.                            |

**Mechanism**:
1. **`upload_multi`**: Renders a form with a multi-file input and type/category selectors.
2. **`_upload_multi`**: Processes each file via `media->add()` and aggregates results.

---

### `add` / `_add`
**Purpose**: Links an external media resource (URL) to the system.
**Parameters (Form Fields)**:
| Name          | Type   | Description                          |
|---------------|--------|--------------------------------------|
| `$ifc_param1` | string | Media name.                          |
| `$ifc_param2` | string | URL (e.g., `https://example.com/image.jpg`). |
| `$ifc_param3` | string | Media type.                          |
| `$ifc_param4` | string | Category.                            |

**Mechanism**:
- Uses `media->link()` to create a reference without uploading a file.

---

### `edit` / `_edit`
**Purpose**: Modifies metadata for an existing media item.
**Parameters (Form Fields)**:
| Name          | Type   | Description                          |
|---------------|--------|--------------------------------------|
| `$ifc_param1` | string | New name.                            |
| `$ifc_param2` | string | New URL (external media only).       |
| `$ifc_param3` | string | New type.                            |
| `$ifc_param4` | string | New category.                        |
| `$ifc_param5` | string | New filename (internal media only).  |

**Mechanism**:
- For **internal media**, the filename field is editable.
- For **external media**, the URL field is editable.
- Uses `media->set()` to update metadata.

---

### `replace` / `_replace`
**Purpose**: Replaces the file of an internal media item.
**Parameters (Form Fields)**:
| Name            | Type   | Description                          |
|-----------------|--------|--------------------------------------|
| `$ifc_file1`    | file   | New file.                            |
| `$ifc_file1_name` | string | Original filename.                   |

**Mechanism**:
- Validates the media is internal (not a URL).
- Uses `media->replace()` to swap the file while preserving metadata.

---

### `delete`
**Purpose**: Removes selected media items.
**Parameters**:
| Name       | Type     | Description                          |
|------------|----------|--------------------------------------|
| `$_object` | string[] | Array of media identifiers.          |

**Mechanism**:
1. Deletes each item via `media->unlink()`.
2. Cleans up language references for deleted items.
3. Falls back to the first item in the category if the active item is deleted.

---

### `category_rename` / `_category_rename`
**Purpose**: Renames a media category.
**Parameters (Form Fields)**:
| Name          | Type   | Description                          |
|---------------|--------|--------------------------------------|
| `$ifc_param1` | string | New category name.                   |

**Mechanism**:
- Updates the `category` field for all media in the old category.

---

### Media Type Management (`type`, `type_select`, `type_add`, `type_set`, `type_delete`)
**Purpose**: Configures media types (e.g., `image/jpeg`, `video/mp4`) and their rendering templates.
**Parameters (Form Fields)**:
| Name          | Type     | Description                          |
|---------------|----------|--------------------------------------|
| `$type_object` | string   | Selected media type identifier.      |
| `$ifc_param1` | string   | Type name.                           |
| `$ifc_param2` | string   | File extensions (e.g., `jpg,png`).   |
| `$ifc_param3` | string   | HTML template code.                  |
| `$list`       | string[] | Array of types to delete.            |

**Mechanism**:
- Uses the `media_type` class to manage types.
- **`type_add`**: Creates a new type with a default name.
- **`type_set`**: Updates name, extensions, or template.
- **`type_delete`**: Removes selected types.

**Usage Example**:
```php
// Add a new media type for SVG files
$media_type->add("SVG Image");
$media_type->set("svg", "SVG Image", "svg", "<img src=\"%src%\" alt=\"%alt%\">");
```

---

## Main Display Workflow
**Purpose**: Renders the primary media management UI with:
- Category selector
- Media list
- Preview iframe
- Action menu

**Mechanism**:
1. **Category Handling**:
   - Retrieves the last selected category from cache or defaults to the first category.
   - Updates the object reference if the category changes.
2. **Menu Generation**:
   - Conditional actions based on permissions (e.g., upload, edit, delete).
   - Insert command for selectable interfaces.
3. **UI Components**:
   - **Category Selector**: Dropdown of all categories.
   - **Media List**: Checkbox list of media in the selected category.
   - **Preview Iframe**: Displays the selected media via the `display` message.
   - **Language Selector**: For multi-language sites.

**JavaScript Integration**:
- `media_select()`: Updates the preview when a media item is selected.
- `ifc_list_*` functions: Manage checkbox selections.

---

## Helper Functions

### `media_get_array()`
**Purpose**: Retrieves a structured array of all media grouped by category.
**Return**: `array` (Category → [Media Name → Identifier]).

### `media_get_select()`
**Purpose**: Generates a `<select>` element for media categories.
**Return**: `string` (HTML).

### `media_type_get_select()`
**Purpose**: Generates a `<select>` element for media types.
**Return**: `string` (HTML).

---

## Usage Example: Uploading a File
```php
// 1. Render the upload form (triggered by menu action)
$ifc = new ifc($response, $page, TRUE, ["object" => "", "language" => "en"], "_upload", "Upload Media");
$ifc->set("Name", "text 40 40 bl");
$ifc->set("File", "file 40 b");
$ifc->set("Type", "select 40 b", media_type_get_select());
$ifc->set("Category", "list 40 40 b", "Logos");
$ifc->close();

// 2. Process the uploaded file (after form submission)
if (isset($ifc_file1)) {
    $media = new media();
    $newObject = $media->add(
        $ifc_file1,          // Uploaded file
        $ifc_file1_name,     // Original filename
        $ifc_param1,         // Name (e.g., "Company Logo")
        $ifc_param2,         // Type (e.g., "image/png")
        $ifc_param3          // Category (e.g., "Logos")
    );
    if ($newObject) {
        $object = language_set("", $newObject, "en");
        $response = CMS_MSG_DONE;
    }
}
```

---

## Key Classes Used
| Class         | Purpose                                                                 |
|---------------|-------------------------------------------------------------------------|
| `media`       | Core media management (upload, link, delete, metadata).                 |
| `media_type`  | Media type configuration (extensions, HTML templates).                  |
| `ifc`         | Interface controller for form generation and message handling.          |


<!-- HASH:da1ccf31b059c9a69356532c82415713 -->
