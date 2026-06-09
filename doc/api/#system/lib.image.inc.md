# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.image.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.image.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Image Management Module (`lib.image.inc`)

The `lib.image.inc` file provides a comprehensive image management system for the PWNC Web Platform. It handles both **internal images** (uploaded and stored locally) and **external images** (linked via URL), offering CRUD operations, categorization, and permission-based access control.

The module consists of:
- A **utility function** (`image_get_array`, `image_get_select`) for generating UI-ready image lists.
- A **class** (`image`) encapsulating all image operations with permission checks and data persistence.

---

## Utility Functions

### `image_get_array()`
Generates a nested associative array of all images, grouped by category, for use in UI selection elements (e.g., `<select>`).

#### Return Values
| Type | Description |
|------|-------------|
| `array` | Nested associative array: `["Category" => ["Display Name" => "image://index"]]`. Empty category (`""`) contains uncategorized images. |

#### Inner Mechanisms
1. Loads image data from the `#system/image` dataset.
2. Iterates through all images, extracting `category` and localized `name`.
3. Handles name collisions by appending `(1)`, `(2)`, etc.
4. Sorts categories and names naturally (case-insensitive).

#### Usage Example
```php
$image_options = image_get_array();
/*
Example Output:
[
    "" => ["Uncategorized Image" => "image://abc123"],
    "Logos" => ["Company Logo" => "image://def456"]
]
*/
echo '<select name="image">';
foreach ($image_options as $category => $images) {
    echo "<optgroup label='" . q($category) . "'>";
    foreach ($images as $name => $index) {
        echo "<option value='" . q($index) . "'>" . q($name) . "</option>";
    }
    echo "</optgroup>";
}
echo '</select>';
```

---

### `image_get_select()`
Generates a flat associative array of unique categories for filtering or grouping images.

#### Return Values
| Type | Description |
|------|-------------|
| `array` | Associative array: `["Category" => "Category"]`. Empty key (`""`) represents uncategorized images. |

#### Inner Mechanisms
1. Loads image data from the `#system/image` dataset.
2. Extracts unique `category` values.
3. Sorts categories naturally (case-insensitive).

#### Usage Example
```php
$categories = image_get_select();
/*
Example Output:
["" => "", "Logos" => "Logos", "Icons" => "Icons"]
*/
echo '<select name="category">';
foreach ($categories as $value => $label) {
    echo "<option value='" . q($value) . "'>" . q($label) . "</option>";
}
echo '</select>';
```

---

## `image` Class
Manages image assets with permission-based operations. Requires `operator` permission (defined by `CMS_IMAGE_PERMISSION_OPERATOR`).

### Properties
| Name | Type | Description |
|------|------|-------------|
| `data` | `data` | Instance of the `#system/image` dataset for persistence. |
| `operator` | `bool` | `TRUE` if the current user has `operator` permission. |

### Constructor
```php
function __construct()
```
Initializes the class by:
1. Loading the `#system/image` dataset.
2. Checking `operator` permission.
3. Ensuring the image storage directory (`CMS_DATA_PATH . "image"`) exists.

---

### `add($uploaded_file, $uploaded_filename, $name = NULL, $category = NULL, $filename = NULL)`
Uploads and stores an image file, creating a new dataset.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$uploaded_file` | `string` | Temporary path to the uploaded file (e.g., `$_FILES['file']['tmp_name']`). |
| `$uploaded_filename` | `string` | Original filename of the uploaded file (e.g., `$_FILES['file']['name']`). |
| `$name` | `string\|array\|NULL` | Localized name for the image. Supports language arrays (e.g., `["en" => "Logo", "de" => "Symbol"]`). Defaults to `$filename` or `$uploaded_filename`. |
| `$category` | `string\|NULL` | Category for grouping images. |
| `$filename` | `string\|NULL` | Desired filename (without extension). Defaults to `$name` or `$uploaded_filename`. |

#### Return Values
| Type | Description |
|------|-------------|
| `string\|FALSE` | Image index (e.g., `"image://abc123"`) on success, `FALSE` on failure. |

#### Inner Mechanisms
1. **Permission Check**: Fails if the user lacks `operator` permission.
2. **File Validation**: Ensures the file exists and has a valid extension (`gif`, `jpg`, `png`, `svg`, `webp`).
3. **Name Handling**: Falls back to `$filename` or `$uploaded_filename` if `$name` is empty.
4. **Filename Generation**: Cleans `$filename` and appends a unique suffix if the file already exists.
5. **File Storage**: Moves the uploaded file to `CMS_DATA_PATH . "image/"`.
6. **Dataset Creation**: Stores `name`, `category`, and `filename` in the dataset.

#### Usage Example
```php
$image = new image();
$index = $image->add(
    $_FILES['file']['tmp_name'],
    $_FILES['file']['name'],
    ["en" => "Profile Picture", "de" => "Profilbild"],
    "Avatars",
    "user_" . $_SESSION['user_id']
);
if ($index) {
    echo "Image uploaded successfully: " . q($index);
} else {
    echo "Upload failed.";
}
```

---

### `set($index, $name, $category, $filename_or_url)`
Updates an existing image's metadata or switches it between internal/external storage.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Image index (e.g., `"image://abc123"`). |
| `$name` | `string\|array` | Localized name for the image. |
| `$category` | `string` | New category for the image. |
| `$filename_or_url` | `string` | For **internal images**: New filename (without extension). For **external images**: URL. |

#### Return Values
| Type | Description |
|------|-------------|
| `string\|FALSE` | Image index on success, `FALSE` on failure. |

#### Inner Mechanisms
1. **Permission Check**: Fails if the user lacks `operator` permission.
2. **Security Check**: Fails if `$index` is empty.
3. **Internal Images**:
   - Updates `name` and `category`.
   - If `$filename` is provided, renames the file and updates the dataset.
4. **External Images**:
   - Updates `name`, `category`, and `url`.
   - Removes the local file if the image was previously internal.

#### Usage Example
```php
$image = new image();
$index = "image://abc123";

// Update internal image
$success = $image->set($index, "Updated Logo", "Branding", "new_logo");
if ($success) {
    echo "Image updated successfully.";
}

// Convert internal image to external
$success = $image->set($index, "External Logo", "Branding", "https://example.com/logo.png");
if ($success) {
    echo "Image converted to external link.";
}
```

---

### `replace($index, $uploaded_file, $uploaded_filename)`
Replaces an internal image's file while preserving its metadata.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Image index (e.g., `"image://abc123"`). |
| `$uploaded_file` | `string` | Temporary path to the new uploaded file. |
| `$uploaded_filename` | `string` | Original filename of the new uploaded file. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms
1. **Permission Check**: Fails if the user lacks `operator` permission.
2. **Validation**: Ensures the image is internal and the new file has a valid extension.
3. **Extension Handling**:
   - If the new extension matches the old one, overwrites the file directly.
   - Otherwise, deletes the old file and generates a new filename.
4. **File Replacement**: Moves the new file to the storage directory and updates the dataset.

#### Usage Example
```php
$image = new image();
$index = "image://abc123";
$success = $image->replace(
    $index,
    $_FILES['new_file']['tmp_name'],
    $_FILES['new_file']['name']
);
if ($success) {
    echo "Image replaced successfully.";
} else {
    echo "Replacement failed.";
}
```

---

### `link($url, $name = NULL, $category = NULL)`
Creates an external image reference (URL) without storing a local file.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$url` | `string` | URL of the external image. |
| `$name` | `string\|array\|NULL` | Localized name for the image. Defaults to the filename extracted from `$url`. |
| `$category` | `string\|NULL` | Category for grouping images. |

#### Return Values
| Type | Description |
|------|-------------|
| `string\|FALSE` | Image index (e.g., `"image://abc123"`) on success, `FALSE` on failure. |

#### Inner Mechanisms
1. **Permission Check**: Fails if the user lacks `operator` permission.
2. **Name Handling**: Falls back to the filename extracted from `$url` if `$name` is empty.
3. **Dataset Creation**: Stores `name`, `category`, and `url` in the dataset.

#### Usage Example
```php
$image = new image();
$index = $image->link(
    "https://example.com/logo.png",
    ["en" => "External Logo", "de" => "Externes Logo"],
    "Branding"
);
if ($index) {
    echo "External image linked successfully: " . q($index);
}
```

---

### `unlink($index)`
Deletes an image (internal or external) from the dataset. For internal images, also deletes the local file.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Image index (e.g., `"image://abc123"`). |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms
1. **Permission Check**: Fails if the user lacks `operator` permission.
2. **Security Check**: Fails if `$index` is empty.
3. **Internal Images**: Deletes the local file from `CMS_DATA_PATH . "image/"`.
4. **Dataset Deletion**: Removes the image from the dataset.

#### Usage Example
```php
$image = new image();
$index = "image://abc123";
$success = $image->unlink($index);
if ($success) {
    echo "Image deleted successfully.";
} else {
    echo "Deletion failed.";
}
```

---

### `internal($index)`
Checks if an image is stored internally (local file) or externally (URL).

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Image index (e.g., `"image://abc123"`). |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if the image is internal, `FALSE` if external. |

#### Inner Mechanisms
1. Checks if the dataset contains a `url` field (external) or a `filename` field (internal).
2. For internal images, verifies the file exists in `CMS_DATA_PATH . "image/"`.

#### Usage Example
```php
$image = new image();
$index = "image://abc123";
if ($image->internal($index)) {
    echo "This is an internal image.";
} else {
    echo "This is an external image.";
}
```


<!-- HASH:161052cd3c0effe4c5a20c3834c8b61f -->
