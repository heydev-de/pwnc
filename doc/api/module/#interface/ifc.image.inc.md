# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.image.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.image.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Image Interface Module (`ifc.image.inc`)

This file implements the **Image Management Interface** for the PWNC Web Platform. It provides a complete UI for uploading, organizing, editing, and configuring images within the CMS. The interface supports:

- **Single/Multi-upload** of images (GIF, JPG, PNG, SVG, WEBP)
- **External image linking** (via URL)
- **Image replacement** (overwriting existing files)
- **Category management** (renaming, filtering)
- **Cache clearing** (for image and content caches)
- **Configuration** (preferred format, resolution limits, daemon processing)
- **Multilingual support** (via language-specific object references)

The interface is **permission-aware**, requiring at least `CMS_L_ACCESS` for basic usage and `CMS_IMAGE_PERMISSION_OPERATOR` for administrative actions (e.g., cache clearing, configuration).

---

## Core Workflow

### Initialization
```php
// Load required libraries
if (!cms_load("image")) ifc_inactive($ifc_page);
// Check permissions
ifc_permission(["" => CMS_L_ACCESS, CMS_IMAGE_PERMISSION_OPERATOR => CMS_L_OPERATOR]);
// Initialize cache and language
cms_cache_init($object, "image." . CMS_USER . ".object");
init($language);
```

**Purpose**:
- Ensures the `image` library is loaded.
- Validates user permissions (basic access or operator privileges).
- Initializes caching for the current user’s image selection and language context.

---

## Message Handling

The interface processes user actions via `CMS_IFC_MESSAGE` cases. Each case corresponds to a specific operation (e.g., `upload`, `edit`, `delete`).

---

### `case "select"`
**Purpose**:
Updates the selected image object based on user input and refreshes the display.

**Parameters**:
| Name          | Type   | Description                          |
|---------------|--------|--------------------------------------|
| `$object`     | string | Current object identifier (multilingual). |
| `$ifc_param`  | string | New object identifier from user input. |
| `$language`   | string | Current language code.               |

**Mechanism**:
- Calls `language_set()` to update the object reference for the current language.
- Relies on `cms_cache()` to persist the selection.

**Usage Example**:
```php
// User selects an image from the category list
$object = language_set($object, "image_123", "en");
```

---

### `case "select_language"`
**Purpose**:
Switches the active language for image display/editing.

**Parameters**:
| Name          | Type   | Description                          |
|---------------|--------|--------------------------------------|
| `$ifc_param`  | string | Language code (e.g., `"en"`, `"de"`). |

**Mechanism**:
- Directly assigns the new language to `$language`.

**Usage Example**:
```php
// User switches from English to German
$language = "de";
```

---

### `case "display"`
**Purpose**:
Renders a preview of the selected image in an iframe, including metadata (dimensions, MIME type).

**Parameters**:
| Name      | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `$object` | string | Image object identifier.             |

**Return/Output**:
- HTML iframe with:
  - Clickable thumbnail (links to full-size image).
  - Metadata (e.g., `1920 x 1080 px | image/jpeg`).

**Mechanism**:
1. Validates the object exists.
2. Caches the object for the current user.
3. Uses `translate_url()` to resolve the physical image path.
4. Generates a resized preview via `image_process()` (max 600x600px).
5. Outputs HTML with escaped values (`x()` for XML, `q()` for JS).

**Usage Example**:
```php
// Display image "image_123" in the preview pane
$image = translate_url("image_123");
$size = getimagesize($image);
$_image = image_process($image, 600, 600);
echo "<img src='" . x($_image) . "' alt='" . x(file_name($image)) . "'>";
```

---

### `case "upload"`
**Purpose**:
Displays a form for uploading a single image with metadata (name, category, filename).

**Parameters**:
| Name          | Type   | Description                          |
|---------------|--------|--------------------------------------|
| `$object`     | string | Current object (for language context). |
| `$language`   | string | Current language code.               |

**Form Fields**:
| Field         | Type       | Description                          |
|---------------|------------|--------------------------------------|
| Name          | Text       | Display name (40 chars max).         |
| File          | File       | Upload field (GIF/JPG/PNG/SVG/WEBP). |
| Category      | Dropdown   | Predefined categories from `image_get_select()`. |
| Filename      | Text       | Custom filename (extension auto-appended). |

**Mechanism**:
- Uses the `ifc` class to generate the form.
- Auto-triggers the file input via JavaScript (`ifc_object("ifc_file1").click()`).

**Usage Example**:
```php
// Generate upload form for a new image
$ifc = new ifc($ifc_response, $ifc_page, TRUE, ["object" => $object, "language" => $language], "_upload");
$ifc->set("Name", "text 40 40 bl");
$ifc->set("File (GIF, JPG, PNG, SVG, WEBP)", "file 40 b");
$ifc->close();
```

---

### `case "_upload"`
**Purpose**:
Processes the uploaded image and saves it to the CMS.

**Parameters**:
| Name            | Type   | Description                          |
|-----------------|--------|--------------------------------------|
| `$ifc_file1`    | string | Temporary file path.                 |
| `$ifc_file1_name`| string | Original filename.                   |
| `$ifc_param1`   | string | Name (from form).                    |
| `$ifc_param2`   | string | Category (from form).                |
| `$ifc_param3`   | string | Custom filename (from form).         |

**Return**:
- `CMS_MSG_DONE` on success.
- `CMS_MSG_ERROR` on failure.

**Mechanism**:
1. Instantiates the `image` class.
2. Calls `image->add()` to process the upload.
3. Updates the object reference for the current language.

**Usage Example**:
```php
// Process an uploaded file
$image = new image();
if ($new_object = $image->add($tmp_file, "logo.png", "Company Logo", "logos", "logo")) {
    $object = language_set($object, $new_object, "en");
}
```

---

### `case "upload_multi"`
**Purpose**:
Displays a form for uploading multiple images at once.

**Form Fields**:
| Field         | Type       | Description                          |
|---------------|------------|--------------------------------------|
| File          | Multi-File | Upload multiple files.               |
| Category      | Dropdown   | Predefined categories.               |

**Mechanism**:
- Similar to `upload`, but uses `multifile` input.
- Auto-triggers the file input via JavaScript.

---

### `case "_upload_multi"`
**Purpose**:
Processes multiple uploaded images.

**Parameters**:
| Name            | Type     | Description                          |
|-----------------|----------|--------------------------------------|
| `$ifc_file1`    | array    | Array of temporary file paths.       |
| `$ifc_file1_name`| array    | Array of original filenames.         |
| `$ifc_param1`   | string   | Category (from form).                |

**Return**:
- `CMS_MSG_DONE` if at least one image succeeds.
- `CMS_MSG_ERROR` if all fail.

**Mechanism**:
1. Combines files and filenames into an associative array.
2. Processes each file via `image->add()`.
3. Updates the object reference for the last successful upload.

---

### `case "add"` / `case "edit"`
**Purpose**:
Displays forms for adding (external URL) or editing an image.

**Parameters**:
| Name          | Type   | Description                          |
|---------------|--------|--------------------------------------|
| `$object`     | string | Current object (for language context). |
| `$language`   | string | Current language code.               |

**Form Fields (Add)**:
| Field         | Type   | Description                          |
|---------------|--------|--------------------------------------|
| Name          | Text   | Display name.                        |
| URL           | Text   | External image URL.                  |
| Category      | Dropdown| Predefined categories.               |

**Form Fields (Edit)**:
| Field         | Type   | Description                          |
|---------------|--------|--------------------------------------|
| Name          | Text   | Display name.                        |
| URL           | Text   | External image URL (if not internal).|
| Category      | Dropdown| Predefined categories.               |
| Filename      | Text   | Custom filename (for internal images). |

**Mechanism**:
- For edits, checks if the image is internal (`image->internal()`).
- Pre-fills form fields with existing data.

---

### `case "_add"`
**Purpose**:
Processes the addition of an external image via URL.

**Parameters**:
| Name          | Type   | Description                          |
|---------------|--------|--------------------------------------|
| `$ifc_param1` | string | Name.                                |
| `$ifc_param2` | string | URL.                                 |
| `$ifc_param3` | string | Category.                            |

**Return**:
- `CMS_MSG_DONE` on success.
- `CMS_MSG_ERROR` on failure.

**Mechanism**:
- Calls `image->link()` to create a reference to the external image.

---

### `case "_edit"`
**Purpose**:
Processes edits to an image (name, category, URL/filename).

**Parameters**:
| Name          | Type   | Description                          |
|---------------|--------|--------------------------------------|
| `$object`     | string | Image object identifier.             |
| `$ifc_param1` | string | Name.                                |
| `$ifc_param2` | string | URL (external) or filename (internal).|
| `$ifc_param3` | string | Category.                            |

**Return**:
- `CMS_MSG_DONE` on success.
- `CMS_MSG_ERROR` on failure.

**Mechanism**:
- Calls `image->set()` with the updated values.
- Handles internal/external images differently.

---

### `case "replace"`
**Purpose**:
Displays a form to replace an internal image with a new file.

**Parameters**:
| Name          | Type   | Description                          |
|---------------|--------|--------------------------------------|
| `$object`     | string | Image object identifier.             |

**Form Fields**:
| Field         | Type   | Description                          |
|---------------|--------|--------------------------------------|
| File          | File   | New image file.                      |

**Mechanism**:
- Validates the image is internal (`image->internal()`).
- Auto-submits the form on file selection (`ifc_autopost`).

---

### `case "_replace"`
**Purpose**:
Processes the replacement of an internal image.

**Parameters**:
| Name            | Type   | Description                          |
|-----------------|--------|--------------------------------------|
| `$object`       | string | Image object identifier.             |
| `$ifc_file1`    | string | Temporary file path.                 |
| `$ifc_file1_name`| string | Original filename.                   |

**Return**:
- `CMS_MSG_DONE` on success.
- `CMS_MSG_ERROR` on failure.

**Mechanism**:
- Calls `image->replace()` to overwrite the file.

---

### `case "delete"`
**Purpose**:
Deletes selected images and updates the object reference.

**Parameters**:
| Name          | Type     | Description                          |
|---------------|----------|--------------------------------------|
| `$_object`    | array    | Array of object identifiers to delete.|

**Return**:
- `CMS_MSG_DONE` if all deletions succeed.
- `CMS_MSG_ERROR` if any fail.

**Mechanism**:
1. Deletes each image via `image->unlink()`.
2. Updates the object reference to the first image in the same category (or `NULL` if none remain).
3. Handles multilingual references.

---

### `case "category_rename"`
**Purpose**:
Displays a form to rename a category.

**Form Fields**:
| Field         | Type   | Description                          |
|---------------|--------|--------------------------------------|
| Category      | Text   | New category name.                   |

**Mechanism**:
- Pre-fills the current category name.

---

### `case "_category_rename"`
**Purpose**:
Processes the category rename operation.

**Parameters**:
| Name          | Type   | Description                          |
|---------------|--------|--------------------------------------|
| `$ifc_param1` | string | New category name.                   |

**Return**:
- `CMS_MSG_DONE` on success.
- `CMS_MSG_ERROR` on failure.

**Mechanism**:
1. Iterates through all images in the old category.
2. Updates their category to the new name via `image->data->set()`.
3. Saves changes via `image->data->save()`.

---

### `case "clear_cache"`
**Purpose**:
Clears the image and content caches.

**Return**:
- `CMS_MSG_DONE` on success.
- `CMS_MSG_ERROR` on failure.

**Mechanism**:
1. Validates operator permissions.
2. Deletes cache directories via `filemanager_delete()`:
   - `CMS_DATA_PATH . "image/cache"`
   - `CMS_DATA_PATH . "#content/cache"`

---

### `case "config"`
**Purpose**:
Displays the image configuration form.

**Form Fields**:
| Field               | Type       | Description                          |
|---------------------|------------|--------------------------------------|
| Preferred Format    | Dropdown   | WebP or JPEG.                        |
| Maximum Resolution  | Dropdown   | UHD-II, UHD-I, FHD, or HD.           |
| Daemon Processing   | Checkbox   | Enable/disable background processing.|

**Mechanism**:
- Pre-fills values from the `system` class.

---

### `case "_config"`
**Purpose**:
Saves the image configuration.

**Parameters**:
| Name          | Type    | Description                          |
|---------------|---------|--------------------------------------|
| `$ifc_param1` | string  | Preferred format (e.g., `"webp"`).   |
| `$ifc_param2` | string  | Resolution (e.g., `"1920x1080"`).    |
| `$ifc_param3` | boolean | Daemon processing enabled.           |

**Return**:
- `CMS_MSG_DONE` on success.
- `CMS_MSG_ERROR` on failure.

**Mechanism**:
- Updates values in the `system` class and saves.

---

## Main Display

### Overview
Renders the primary interface with:
1. **Category selector** (left pane).
2. **Image list** (thumbnails with checkboxes).
3. **Preview pane** (right iframe).

### Key Components

#### Category Selection
```php
echo("<select class='list' onchange='ifc_post(\"select\",this.value);'>");
foreach ($array AS $key => $value) {
    echo("<option value='" . x($value) . "'" . (streq($key, $category) ? " selected>" : ">") . x($key) . "</option>");
}
echo("</select>");
```
**Purpose**:
- Lists all available categories.
- Triggers a reload when changed.

#### Image List
```php
echo("<div id='_object' class='select image-list' data-onchange='image_select(this.value);'>");
foreach ($array[$category] AS $key => $value) {
    $_image = image_process(translate_url($value), 100, 100);
    echo("<label><input type='checkbox' value='" . x($value) . "'" . (streq($value, $_object) ? " checked>" : ">") .
         "<image src='" . x($_image) . "' alt=''>" . x($key) . "</label>");
}
echo("</div>");
```
**Purpose**:
- Displays thumbnails for images in the selected category.
- Supports selection/deselection via checkboxes.
- Double-clicking an image triggers insertion (if in select mode).

#### JavaScript Helpers
```javascript
function image_select(value) {
    var language = "<?php echo(q($language));?>";
    if (ifc_language_get(ifc_get("object"), language) === value) return;
    ifc_set("object", ifc_language_set(ifc_get("object"), value, language));
    document.getElementById("image-display").src = "<?php echo(q(cms_url(...)));?>";
}
```
**Purpose**:
- Updates the selected image when a thumbnail is clicked.
- Refreshes the preview iframe.

#### Menu Actions
```php
$menu = NULL;
if (CMS_IFC_SELECT && $_object) {
    $menu[CMS_L_COMMAND_INSERT . "|image/command_apply"] = "javascript:ifc_return();";
}
if ($image->operator) {
    $menu[CMS_L_IFC_IMAGE_001 . "|image/command_upload"] = "upload";
    // ... other actions
}
```
**Purpose**:
- Dynamically generates a menu based on:
  - **Select mode** (insert button).
  - **Operator permissions** (upload, edit, delete, etc.).

---

## Helper Functions

### `image_get_select()`
**Purpose**:
Returns an array of available categories for dropdowns.

**Return**:
- Associative array of categories (e.g., `["Logos" => "logos", "Banners" => "banners"]`).

### `image_get_array()`
**Purpose**:
Returns a nested array of all images grouped by category.

**Return**:
- Structure: `["Category" => ["image_1" => "Name 1", "image_2" => "Name 2"]]`.

### `image_process()`
**Purpose**:
Generates a resized/processed version of an image.

**Parameters**:
| Name      | Type    | Description                          |
|-----------|---------|--------------------------------------|
| `$image`  | string  | Image path/URL.                      |
| `$width`  | int     | Maximum width.                       |
| `$height` | int     | Maximum height.                      |
| `$mode`   | mixed   | Processing mode (e.g., `NULL` for default). |
| `$force`  | boolean | Force reprocessing.                  |
| `$daemon` | boolean | Use daemon processing.               |

**Return**:
- Path to the processed image.

---

## Usage Examples

### 1. Uploading an Image
```php
// Display the upload form
$ifc = new ifc($response, "image", TRUE, ["object" => "", "language" => "en"], "_upload");
$ifc->set("Name", "text 40 40 bl");
$ifc->set("File (GIF, JPG, PNG, SVG, WEBP)", "file 40 b");
$ifc->set(image_get_select(), "list 40 40 b", "logos");
$ifc->close();
```

### 2. Editing an Image
```php
// Display the edit form for an internal image
$image = new image();
$is_internal = $image->internal("image_123");
$ifc = new ifc($response, "image", TRUE, ["object" => "image_123", "language" => "en"], "_edit");
$ifc->set("Name", "text 40 40 bl", $image->data->get("image_123", "name"));
if (!$is_internal) {
    $ifc->set("URL", "text 40 256 b", $image->data->get("image_123", "url"));
}
$ifc->close();
```

### 3. Clearing the Cache
```php
// Clear image and content caches (operator-only)
if (cms_permission(CMS_IMAGE_PERMISSION_OPERATOR)) {
    cms_load("filemanager");
    filemanager_delete(CMS_DATA_PATH . "image/cache");
    filemanager_delete(CMS_DATA_PATH . "#content/cache");
}
```

### 4. Generating a Preview URL
```php
// Get a resized preview URL for an image
$preview_url = image_process(translate_url("image_123"), 200, 200);
echo "<img src='" . x($preview_url) . "' alt='Preview'>";
```


<!-- HASH:5ded7e1fae8287c0629181ff317c3b7d -->
