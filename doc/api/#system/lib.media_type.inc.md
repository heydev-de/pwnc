# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.media_type.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.media_type.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Media Type Management

The `lib.media_type.inc` file provides functionality for managing and rendering media types in the PWNC Web Platform. It includes a `media_type` class that handles the storage, retrieval, and parsing of media type definitions, which are used to render embedded media content (e.g., videos, audio, custom objects) in a flexible and type-safe manner.

The system allows operators to define custom media types with associated MIME types and rendering templates (HTML code). If no custom template is found, a fallback `<object>` tag is generated. Media types are cached for performance and stored in both a database table (`#system/media.type`) and physical files (`#media.type/*.htm`).

---

### Global Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_MEDIA_TYPE_PERMISSION_OPERATOR` | `"operator"` | Permission identifier required to modify media types. |

---

### Utility Function: `media_type_get_select()`

#### Purpose
Generates an associative array of media type names and their database keys, suitable for use in HTML `<select>` elements. The array is sorted naturally and case-insensitively.

#### Parameters
None.

#### Return Values
- **Type:** `array`
- **Description:** Associative array where keys are media type names (or a space if name is empty) and values are database identifiers. The first entry is a placeholder (`CMS_L_MEDIA_TYPE_001`, typically "Select...") with a `NULL` value.

#### Inner Mechanisms
- Creates a `data` object linked to the `#system/media.type` table.
- Iterates through all records, extracting the `name` field.
- Sorts the resulting array using `ksort()` with `SORT_NATURAL | SORT_FLAG_CASE`.
- Returns the sorted array.

#### Usage Context
Used in administrative interfaces to populate dropdown menus for selecting media types.

#### Example
```php
$select_options = media_type_get_select();
// Output in HTML:
// <select name="media_type">
//   <option value="">Select a media type...</option>
//   <option value="123">MP4 Video</option>
//   <option value="456">MP3 Audio</option>
// </select>
```

---

## Class: `media_type`

Manages the lifecycle and rendering of media types, including CRUD operations, type mapping, and HTML generation.

---

### Properties

| Name | Type/Default | Description |
|------|--------------|-------------|
| `data` | `data` object | Instance of the `data` class linked to `#system/media.type`. |
| `operator` | `bool` | Indicates whether the current user has operator permissions. |
| `type` | `array` | Associative array mapping file extensions (e.g., `"mp4"`) to media type IDs. |

---

### Constructor: `__construct()`

#### Purpose
Initializes the `media_type` object, loads the data source, checks operator permissions, ensures the media type directory exists, and loads or rebuilds the type cache.

#### Parameters
None.

#### Return Values
None (constructor).

#### Inner Mechanisms
- Instantiates a `data` object for the `#system/media.type` table.
- Checks operator permissions via `cms_permission()`.
- Creates the media type directory (`#media.type`) if it doesn’t exist.
- Attempts to load the type cache; if not found, calls `update_type()` to rebuild it.

#### Usage Context
Called automatically when a `media_type` object is instantiated.

#### Example
```php
$media = new media_type();
```

---

### Method: `add($name, $type = NULL, $code = NULL)`

#### Purpose
Creates a new media type with a unique ID, name, associated MIME types, and HTML template code.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Display name of the media type (e.g., "MP4 Video"). |
| `$type` | `string|null` | Comma-separated list of file extensions (e.g., `"mp4,m4v"`). Optional. |
| `$code` | `string|null` | HTML template code with placeholders (e.g., `%src%`, `%width%`). Optional. |

#### Return Values
- **Type:** `string|false`
- **Description:** The generated unique ID of the new media type on success; `false` on failure (e.g., no permission).

#### Inner Mechanisms
- Checks operator permission.
- Generates a unique ID using `unique_id()`.
- Delegates to `set()` to store the data.

#### Usage Context
Used in administrative interfaces to create new media types.

#### Example
```php
$media = new media_type();
$id = $media->add("MP4 Video", "mp4,m4v", "<video src=\"%src%\" controls width=\"%width%\" height=\"%height%\"></video>");
if ($id) {
    echo "Media type created with ID: $id";
}
```

---

### Method: `set($index, $name, $type = NULL, $code = NULL)`

#### Purpose
Updates an existing media type with new metadata and/or template code.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Unique identifier of the media type. |
| `$name` | `string` | New display name. |
| `$type` | `string|null` | New comma-separated list of file extensions. Optional. |
| `$code` | `string|null` | New HTML template code. Optional. |

#### Return Values
- **Type:** `bool`
- **Description:** `true` on success; `false` on failure (e.g., no permission, file write error).

#### Inner Mechanisms
- Checks operator permission.
- Writes the template code to a file (`#media.type/$index.htm`) with exclusive lock.
- Updates the record in the `data` object with `name` and `type`.
- Calls `save()` to persist changes and update the cache.

#### Usage Context
Used to modify existing media types.

#### Example
```php
$media = new media_type();
$success = $media->set("abc123", "HD MP4", "mp4,m4v,mp4v", "<video src=\"%src%\" controls></video>");
if ($success) {
    echo "Media type updated.";
}
```

---

### Method: `get_index($type)`

#### Purpose
Maps a file extension (e.g., `"mp4"`) to the corresponding media type ID.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$type` | `string` | File extension (e.g., `"mp4"`). |

#### Return Values
- **Type:** `string|false`
- **Description:** The media type ID if found; `false` otherwise.

#### Inner Mechanisms
- Converts the input to lowercase using `utf8_strtolower()`.
- Checks the internal `$this->type` array for a match.

#### Usage Context
Used to resolve file extensions to media type templates during rendering.

#### Example
```php
$media = new media_type();
$index = $media->get_index("mp4");
if ($index) {
    echo "MP4 files use media type: $index";
}
```

---

### Method: `get_code($index)`

#### Purpose
Retrieves the HTML template code for a given media type ID.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Media type ID. |

#### Return Values
- **Type:** `string|false`
- **Description:** The template code if found; `false` otherwise.

#### Inner Mechanisms
- Reads the content of the corresponding `.htm` file using `read_file()`.

#### Usage Context
Used internally by `parse()` to fetch the template for rendering.

#### Example
```php
$media = new media_type();
$code = $media->get_code("abc123");
if ($code) {
    echo "Template: " . htmlspecialchars($code);
}
```

---

### Method: `delete($index)`

#### Purpose
Deletes one or more media types and their associated template files.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string|array` | Single ID or array of IDs to delete. |

#### Return Values
- **Type:** `bool`
- **Description:** `true` if all deletions succeeded; `false` if any failed or no permission.

#### Inner Mechanisms
- Checks operator permission.
- Converts single ID to array for uniform processing.
- Deletes each record from the `data` object and removes the corresponding `.htm` file.
- Calls `save()` to persist changes and update the cache.

#### Usage Context
Used in administrative interfaces to remove obsolete media types.

#### Example
```php
$media = new media_type();
$success = $media->delete(["abc123", "def456"]);
if ($success) {
    echo "Media types deleted.";
}
```

---

### Method: `parse($index, $url, $id = NULL, $type = NULL, $width = NULL, $height = NULL, $alt = NULL, $title = NULL, $class = NULL, $style = NULL)`

#### Purpose
Generates HTML markup for embedding media using the specified media type template and provided parameters.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Media type ID. |
| `$url` | `string` | Source URL of the media file. |
| `$id` | `string|null` | `id` attribute for the HTML element. Optional. |
| `$type` | `string|null` | MIME type (e.g., `"video/mp4"`). Auto-detected if not provided. Optional. |
| `$width` | `int|string|null` | `width` attribute. Optional. |
| `$height` | `int|string|null` | `height` attribute. Optional. |
| `$alt` | `string|null` | Alternative text for accessibility. Optional. |
| `$title` | `string|null` | `title` attribute. Optional. |
| `$class` | `string|null` | `class` attribute. Optional. |
| `$style` | `string|null` | `style` attribute. Optional. |

#### Return Values
- **Type:** `string`
- **Description:** Generated HTML markup.

#### Inner Mechanisms
- Attempts to load the template code for the given `$index`.
- If not found, falls back to type detection via `file_extension()` and tries again.
- If still not found, uses a default `<object>` template.
- Replaces placeholders (e.g., `%src%`, `%width%`) with provided values. Optional placeholders (e.g., `[ width="%width%"]`) are removed if the value is empty.
- Auto-detects MIME type using `get_mime_type()` if not provided.

#### Usage Context
Used in frontend rendering to embed media (e.g., videos, audio) in a consistent and secure manner.

#### Example
```php
$media = new media_type();
$html = $media->parse(
    "abc123",                     // Media type ID
    "https://example.com/video.mp4",
    "video-player",               // id
    "video/mp4",                  // type
    640,                          // width
    360,                          // height
    "Promotional video",          // alt
    "Company Promo",              // title
    "media-element",              // class
    "border: 1px solid #ccc;"     // style
);
echo $html;
// Output: <video src="https://example.com/video.mp4" controls width="640" height="360" id="video-player" class="media-element" style="border: 1px solid #ccc;" title="Company Promo"></video>
```

---

### Method: `update_type()`

#### Purpose
Rebuilds the internal `$this->type` array that maps file extensions to media type IDs.

#### Parameters
None.

#### Return Values
None.

#### Inner Mechanisms
- Resets the `$this->type` array.
- Sorts the `data` object by `name`.
- Iterates through all records, splitting the `type` field by commas.
- Trims and lowercases each extension, then maps it to the media type ID.
- Caches the result using `cms_cache()`.

#### Usage Context
Called automatically during initialization and after any modification to media types.

#### Example
```php
$media = new media_type();
$media->update_type(); // Force cache rebuild
```

---

### Method: `save()`

#### Purpose
Persists changes to the `data` object and updates the type cache.

#### Parameters
None.

#### Return Values
- **Type:** `bool`
- **Description:** `true` on success; `false` on failure (e.g., no permission).

#### Inner Mechanisms
- Checks operator permission.
- Calls `update_type()` to refresh the cache.
- Saves the `data` object to the database.

#### Usage Context
Called internally after modifications (e.g., `add`, `set`, `delete`).

#### Example
```php
$media = new media_type();
$media->data->set(["name" => "New Type", "type" => "xyz"], "new123");
$success = $media->save();
if ($success) {
    echo "Changes saved.";
}
```


<!-- HASH:c4ca8d1291850d6c4e4f672048378a37 -->
