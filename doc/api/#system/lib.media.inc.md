# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.media.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.media.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Media Management Module (`lib.media.inc`)

The `lib.media.inc` file provides a comprehensive media management system for the PWNC Web Platform. It handles both **internal media files** (uploaded and stored locally) and **external media links** (referenced by URL). The module includes:

- **Media Class**: Core functionality for adding, modifying, replacing, and deleting media entries.
- **Utility Functions**: Helper functions for generating UI-friendly media arrays and category lists.
- **Permission Control**: Operator-level access restriction for media management operations.

---

## Utility Functions

### `media_get_array()`

Generates a nested associative array of all media entries, grouped by category, for use in UI selection elements (e.g., `<select>`).

#### Parameters
None.

#### Return Values
| Type | Description |
|------|-------------|
| `array` | Nested associative array: `["Category" => ["Display Name" => "Media Index"]]`. Empty categories are included as `["" => []]`. |

#### Inner Mechanisms
- Uses the `data` class to fetch all media entries from `#system/media`.
- For each entry:
  - Extracts `category` and localized `name`.
  - Falls back to the media key if no name is set.
  - Ensures unique display names by appending `(1)`, `(2)`, etc., if duplicates exist.
- Sorts the array recursively using `ksort_recursive()` with natural case-insensitive order.

#### Usage Example
```php
$media_options = media_get_array();
/*
Example Output:
[
    "" => [],
    "Documents" => [
        "User Guide" => "media://abc123",
        "Contract Template" => "media://def456"
    ],
    "Images" => [
        "Logo" => "media://ghi789"
    ]
]
*/
// Used in a form select element:
echo '<select name="media">';
foreach ($media_options as $category => $items) {
    if (!empty($category)) echo "<optgroup label=\"" . x($category) . "\">";
    foreach ($items as $name => $index) {
        echo '<option value="' . x($index) . '">' . x($name) . '</option>';
    }
    if (!empty($category)) echo "</optgroup>";
}
echo '</select>';
```

---

### `media_get_select()`

Generates a flat associative array of all unique media categories for use in category selection UIs.

#### Parameters
None.

#### Return Values
| Type | Description |
|------|-------------|
| `array` | Associative array: `["Category" => "Category"]`. Includes an empty string key for uncategorized media. |

#### Inner Mechanisms
- Uses the `data` class to fetch all media entries.
- Extracts the `category` field from each entry.
- Removes duplicate categories and sorts naturally (case-insensitive).

#### Usage Example
```php
$categories = media_get_select();
/*
Example Output:
[
    "" => "",
    "Documents" => "Documents",
    "Images" => "Images"
]
*/
// Used in a category filter dropdown:
echo '<select name="category">';
foreach ($categories as $value => $label) {
    echo '<option value="' . x($value) . '">' . x($label) . '</option>';
}
echo '</select>';
```

---

## `media` Class

Manages media entries, including internal files and external links. Requires `CMS_MEDIA_PERMISSION_OPERATOR` permission for write operations.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `data` | `data` | Instance of the `data` class for `#system/media` storage. |
| `operator` | `bool` | `TRUE` if the current user has operator permissions. |

### Constructor

```php
function __construct()
```

#### Inner Mechanisms
- Initializes the `data` property to interact with the `#system/media` dataset.
- Checks operator permissions using `cms_permission()`.
- Ensures the media storage directory (`CMS_DATA_PATH . "media"`) exists using `mkpath()`.

---

### `add()`

Adds a new internal media file from an uploaded file.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$uploaded_file` | `string` | - | Temporary path to the uploaded file (e.g., `$_FILES['file']['tmp_name']`). |
| `$uploaded_filename` | `string` | - | Original filename of the uploaded file (e.g., `$_FILES['file']['name']`). |
| `$name` | `string\|array` | `NULL` | Display name (supports language arrays for localization). |
| `$type` | `string` | `NULL` | Media type (e.g., `image`, `document`). |
| `$category` | `string` | `NULL` | Media category (e.g., `Images`, `Documents`). |
| `$filename` | `string` | `NULL` | Desired filename (without extension). Falls back to `$name` or uploaded filename. |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|FALSE` | Media index (e.g., `media://abc123`) on success, `FALSE` on failure. |

#### Inner Mechanisms
1. **Permission Check**: Fails if the user lacks operator permissions.
2. **File Validation**: Checks if the uploaded file exists.
3. **Name Handling**:
   - Uses `language_get()` to resolve localized names.
   - Falls back to `$filename`, then the uploaded filename, then a random ID.
4. **Filename Generation**:
   - Cleans the filename using `stringtofilename()`.
   - Appends a counter (e.g., `-1`, `-2`) if the filename already exists.
5. **File Storage**:
   - Moves the file to `CMS_DATA_PATH . "media/"` using `move_uploaded_file()`.
6. **Dataset Creation**:
   - Generates a unique index (e.g., `media://abc123`).
   - Stores metadata (`name`, `type`, `category`, `filename`) in the dataset.
7. **Persistence**: Saves the dataset and returns the index.

#### Usage Example
```php
if (isset($_FILES['file'])) {
    $media = new media();
    $index = $media->add(
        $_FILES['file']['tmp_name'],
        $_FILES['file']['name'],
        ["en" => "User Guide", "es" => "Guía del Usuario"],
        "document",
        "Documents"
    );
    if ($index) {
        echo "Media added: " . x($index);
    } else {
        echo "Failed to add media.";
    }
}
```

---

### `set()`

Updates an existing media entry (internal or external).

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `string` | - | Media index (e.g., `media://abc123`). |
| `$name` | `string\|array` | - | Updated display name (supports language arrays). |
| `$type` | `string` | - | Updated media type. |
| `$category` | `string` | - | Updated category. |
| `$filename_or_url` | `string` | - | For **internal media**: New filename (without extension). For **external media**: New URL. |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|FALSE` | Media index on success, `FALSE` on failure. |

#### Inner Mechanisms
1. **Permission Check**: Fails if the user lacks operator permissions.
2. **Security Check**: Fails if `$index` is empty.
3. **Internal Media**:
   - Resolves the default name using `language_get()` and falls back to the old name.
   - If `$filename_or_url` is empty, retains the old filename.
   - Otherwise:
     - Cleans the new filename using `stringtofilename()`.
     - Generates a unique filename (appends `-1`, `-2`, etc., if needed).
     - Renames the file on disk if the filename changes.
   - Updates the dataset with new metadata (`name`, `type`, `category`, `filename`).
4. **External Media**:
   - Resolves the default name using `language_get()` and falls back to the old name.
   - Updates the dataset with new metadata (`name`, `type`, `category`, `url`).
5. **Persistence**: Saves the dataset and returns the index.

#### Usage Example
```php
$media = new media();
$index = "media://abc123";

// Update internal media filename
$media->set($index, "Updated Name", "image", "Photos", "new-filename");

// Update external media URL
$media->set($index, "External Link", "video", "Videos", "https://example.com/video.mp4");
```

---

### `replace()`

Replaces the file of an existing internal media entry while preserving metadata.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `string` | - | Media index (e.g., `media://abc123`). |
| `$uploaded_file` | `string` | - | Temporary path to the new uploaded file. |
| `$uploaded_filename` | `string` | - | Original filename of the new uploaded file. |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms
1. **Permission Check**: Fails if the user lacks operator permissions.
2. **Validation**:
   - Checks if the media is internal using `internal()`.
   - Validates the existence of the uploaded file.
3. **Extension Handling**:
   - If the new file has the same extension as the old file, replaces it directly.
   - Otherwise:
     - Deletes the old file.
     - Generates a new unique filename (appends `-1`, `-2`, etc., if needed).
4. **File Replacement**:
   - Moves the new file to the media directory.
   - Updates the `filename` field in the dataset.
5. **Persistence**: Saves the dataset and returns `TRUE`.

#### Usage Example
```php
$media = new media();
$index = "media://abc123";

if (isset($_FILES['new_file'])) {
    if ($media->replace($index, $_FILES['new_file']['tmp_name'], $_FILES['new_file']['name'])) {
        echo "File replaced successfully.";
    } else {
        echo "Failed to replace file.";
    }
}
```

---

### `link()`

Adds a new external media link (e.g., YouTube video, remote image).

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$url` | `string` | - | URL of the external media. |
| `$name` | `string\|array` | `NULL` | Display name (supports language arrays). Falls back to the URL filename. |
| `$type` | `string` | `NULL` | Media type (e.g., `video`, `image`). |
| `$category` | `string` | `NULL` | Media category. |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|FALSE` | Media index (e.g., `media://abc123`) on success, `FALSE` on failure. |

#### Inner Mechanisms
1. **Permission Check**: Fails if the user lacks operator permissions.
2. **Name Handling**:
   - Uses `language_get()` to resolve localized names.
   - Falls back to the filename extracted from the URL using `file_name()`.
3. **Dataset Creation**:
   - Generates a unique index (e.g., `media://abc123`).
   - Stores metadata (`name`, `type`, `category`, `url`) in the dataset.
4. **Persistence**: Saves the dataset and returns the index.

#### Usage Example
```php
$media = new media();
$index = $media->link(
    "https://example.com/video.mp4",
    ["en" => "Promo Video", "es" => "Video Promocional"],
    "video",
    "Videos"
);
if ($index) {
    echo "External media linked: " . x($index);
}
```

---

### `unlink()`

Deletes a media entry (internal or external).

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `string` | - | Media index (e.g., `media://abc123`). |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms
1. **Permission Check**: Fails if the user lacks operator permissions.
2. **Security Check**: Fails if `$index` is empty.
3. **Internal Media**:
   - Retrieves the filename from the dataset (or falls back to the index for compatibility).
   - Deletes the file from disk if it exists.
4. **Dataset Deletion**: Removes the media entry from the dataset.
5. **Persistence**: Saves the dataset and returns `TRUE`.

#### Usage Example
```php
$media = new media();
$index = "media://abc123";

if ($media->unlink($index)) {
    echo "Media deleted successfully.";
} else {
    echo "Failed to delete media.";
}
```

---

### `internal()`

Checks if a media entry is internal (stored locally) or external (linked by URL).

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `string` | - | Media index (e.g., `media://abc123`). |

#### Return Values

| Type | Description |
|------|-------------|
| `bool` | `TRUE` if internal, `FALSE` if external. |

#### Inner Mechanisms
- Checks if the `url` field is `NULL` in the dataset.
- For internal media, verifies the file exists on disk (falls back to the index for compatibility).

#### Usage Example
```php
$media = new media();
$index = "media://abc123";

if ($media->internal($index)) {
    echo "This is an internal media file.";
} else {
    echo "This is an external media link.";
}
```

---

### `parse()`

Generates HTML output for a media entry (e.g., `<img>`, `<video>`, `<audio>`) based on its type.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `string` | - | Media index (e.g., `media://abc123`). |
| `$id` | `string` | `NULL` | HTML `id` attribute. Defaults to `media_<unique_id>`. |
| `$width` | `int\|string` | `NULL` | Width attribute (e.g., `100`, `"100px"`). |
| `$height` | `int\|string` | `NULL` | Height attribute. |
| `$alt` | `string` | `NULL` | `alt` attribute (for images). |
| `$title` | `string` | `NULL` | `title` attribute. |
| `$class` | `string` | `NULL` | `class` attribute. |
| `$style` | `string` | `NULL` | `style` attribute. |

#### Return Values

| Type | Description |
|------|-------------|
| `string\|FALSE` | HTML output (e.g., `<img src="...">`) or `FALSE` on failure. |

#### Inner Mechanisms
1. **Dependency Check**: Fails if the `media_type` library is not loaded.
2. **ID Generation**: Creates a unique ID if none is provided.
3. **URL Resolution**: Uses `translate_url()` to convert the media index to a physical URL.
4. **Type Handling**: Delegates HTML generation to the `media_type` class based on the media type.

#### Usage Example
```php
$media = new media();
$index = "media://abc123";

// Render an image
echo $media->parse($index, "logo", 200, 100, "Company Logo", "Logo", "img-fluid");

// Render a video
echo $media->parse("media://def456", "promo", 640, 360, NULL, "Promo Video", "video-js");
```


<!-- HASH:e57aab565d2d37aa33a9e6d89bd11f9c -->
