# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.filemanager.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.filemanager.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## File Manager Functions

The `lib.filemanager.inc` file provides core file management utilities for the PWNC Web Platform. These functions handle directory traversal, file operations (copying, deletion, compression), and structured file system representation for UI components.

---

## Functions

### `filemanager_flexview_compare`

**Purpose:**
Comparison function for sorting file system objects (directories and files) in a user-friendly order. Directories appear before files, and items are sorted case-insensitively by extension and name.

**Parameters:**

| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$value1` | string | First file/directory path to compare |
| `$value2` | string | Second file/directory path to compare |

**Return Values:**
- `int`: `-1` if `$value1` should come before `$value2`, `1` if after, `0` if equal.

**Inner Mechanisms:**
1. Checks if either value is a directory (ends with `/`).
2. If one is a directory and the other is not, directories are prioritized.
3. Extracts file extensions using `strrchr()`.
4. Compares extensions lexicographically.
5. Falls back to case-insensitive UTF-8 string comparison (`utf8_strcasecmp`).

**Usage Context:**
Used as a callback for `uasort()` in `filemanager_sort()` and `uksort()` in `filemanager_flexview()` to ensure consistent, human-readable ordering of files and directories.

**Example:**
```php
$files = [
    "document.pdf",
    "images/",
    "notes.txt",
    "archive.zip"
];
uasort($files, "cms\\filemanager_flexview_compare");
// Result: ["images/", "archive.zip", "document.pdf", "notes.txt"]
```

---

### `filemanager_sort`

**Purpose:**
Sorts an array of file/directory paths in-place using `filemanager_flexview_compare`.

**Parameters:**

| Name    | Type  | Description                     |
|---------|-------|---------------------------------|
| `&$array` | array | Reference to array to be sorted |

**Return Values:**
- `void`: Modifies the input array directly.

**Inner Mechanisms:**
- Uses `uasort()` to maintain key associations while sorting.

**Usage Context:**
Prepares file lists for display in UIs where consistent ordering is required.

**Example:**
```php
$files = ["b.txt", "a/", "c.js"];
filemanager_sort($files);
// $files is now ["a/", "c.js", "b.txt"]
```

---

### `filemanager_flexview`

**Purpose:**
Recursively scans a directory and returns a structured representation (`flexview` object) of its contents, including files and subdirectories.

**Parameters:**

| Name    | Type   | Default         | Description                          |
|---------|--------|-----------------|--------------------------------------|
| `$root`   | string | `NULL` (CMS_ROOT_PATH) | Root directory to scan               |
| `$path`   | string | `NULL` (same as `$root`) | Current path for active traversal    |

**Return Values:**
- `flexview|FALSE`: `flexview` object on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Opens the root directory and initializes a `flexview` object.
2. Uses a loop to traverse directories recursively.
3. Skips `.` and `..` entries.
4. For directories:
   - Appends `/` to the path.
   - Sets a `container` type in the `flexview` object.
   - Recursively scans if the path matches the active traversal path.
5. For files:
   - Extracts the extension using `file_extension()`.
   - Sets a `file` type with a `#subtype` (extension) in the `flexview` object.
6. Sorts entries using `filemanager_flexview_compare`.

**Usage Context:**
Generates hierarchical data for file browsers, asset managers, or any UI requiring a tree-like view of the file system.

**Example:**
```php
$view = filemanager_flexview("/var/www/assets/");
/*
$view->object contains:
[
    "/var/www/assets/images/" => [
        "name" => "images",
        "#type" => "container"
    ],
    "/var/www/assets/styles.css" => [
        "name" => "styles.css",
        "#type" => "file",
        "#subtype" => "css"
    ]
]
*/
```

---

### `filemanager_flexview_directory`

**Purpose:**
Similar to `filemanager_flexview`, but **only includes directories** (excludes files).

**Parameters:**

| Name    | Type   | Default         | Description                          |
|---------|--------|-----------------|--------------------------------------|
| `$root`   | string | `NULL` (CMS_ROOT_PATH) | Root directory to scan               |
| `$path`   | string | `NULL` (same as `$root`) | Current path for active traversal    |

**Return Values:**
- `flexview|FALSE`: `flexview` object on success, `FALSE` on failure.

**Inner Mechanisms:**
- Identical to `filemanager_flexview`, but skips non-directory entries during traversal.

**Usage Context:**
Used when only directory structures are needed (e.g., for folder selection UIs).

**Example:**
```php
$dirs = filemanager_flexview_directory("/var/www/uploads/");
// $dirs->object contains only directory entries
```

---

### `filemanager_get_select`

**Purpose:**
Generates an associative array of directory paths formatted for HTML `<select>` elements, with indentation to represent hierarchy.

**Parameters:**

| Name    | Type   | Default         | Description               |
|---------|--------|-----------------|---------------------------|
| `$root`   | string | `NULL` (CMS_ROOT_PATH) | Root directory to scan    |

**Return Values:**
- `array|FALSE`: Associative array (`display_name => path`) or `FALSE` on failure.

**Inner Mechanisms:**
1. Opens the root directory and initializes an empty array.
2. Uses indentation (`\xE2\x80\x83`, an em-space) to represent nesting levels.
3. Handles duplicate display names by appending `(1)`, `(2)`, etc.
4. Recursively scans subdirectories, incrementing indentation level.

**Usage Context:**
Generates options for directory selection dropdowns in forms.

**Example:**
```php
$options = filemanager_get_select("/var/www/");
/*
$options contains:
[
    "uploads" => "/var/www/uploads/",
    "  images" => "/var/www/uploads/images/",
    "  documents" => "/var/www/uploads/documents/",
    "themes" => "/var/www/themes/"
]
*/
```

---

### `filemanager_copy`

**Purpose:**
Recursively copies a file or directory to a new location.

**Parameters:**

| Name            | Type   | Description                          |
|-----------------|--------|--------------------------------------|
| `$source_path`    | string | Path to source file/directory        |
| `$target_path`    | string | Path to target location              |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Uses a static `$flag` to prevent infinite recursion (e.g., copying a directory into itself).
2. Validates paths using `ice_check_path()`.
3. For directories:
   - Creates the target directory with the same permissions as the source.
   - Uses a stack to process files/directories in reverse order (depth-first).
4. For files:
   - Opens source and target streams.
   - Locks files during copy (`LOCK_SH` for source, `LOCK_EX` for target).
   - Uses `stream_copy_to_stream()` for efficient copying.
   - Preserves file permissions.

**Usage Context:**
Used for duplicating assets, backups, or deploying files to new locations.

**Example:**
```php
// Copy a directory
filemanager_copy("/var/www/old_site/", "/var/www/new_site/");

// Copy a file
filemanager_copy("/var/www/config.php", "/var/www/config_backup.php");
```

---

### `filemanager_delete`

**Purpose:**
Recursively deletes a file or directory.

**Parameters:**

| Name    | Type   | Description               |
|---------|--------|---------------------------|
| `$path`   | string | Path to file/directory    |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. For directories:
   - Recursively deletes all contents.
   - Uses `rmdir()` to remove the empty directory.
2. For files:
   - Uses `unlink()`.

**Usage Context:**
Used for cleanup, asset removal, or temporary file management.

**Example:**
```php
// Delete a directory
filemanager_delete("/var/www/temp_uploads/");

// Delete a file
filemanager_delete("/var/www/logs/old.log");
```

---

### `filemanager_gzcompress`

**Purpose:**
Compresses a file using Gzip (level 9) and saves it to a target path.

**Parameters:**

| Name      | Type   | Default | Description                          |
|-----------|--------|---------|--------------------------------------|
| `$source`   | string |         | Path to source file                  |
| `$target`   | string | `NULL`  | Path to compressed file (defaults to `$source.gz`) |

**Return Values:**
- `string|FALSE`: Path to the compressed file on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Validates the source file exists and is readable.
2. Appends `.gz` to the target path if not already present.
3. Opens the source file in binary read mode and the target in Gzip write mode (`wb9`).
4. Locks both files during compression.
5. Reads the source in 64KB chunks and writes to the Gzip stream.

**Usage Context:**
Used for reducing file sizes (e.g., log files, backups, or large assets).

**Example:**
```php
$compressed = filemanager_gzcompress("/var/www/logs/access.log");
// Result: "/var/www/logs/access.log.gz"
```

---

### `filemanager_gzdecompress`

**Purpose:**
Decompresses a Gzip file and saves it to a target path.

**Parameters:**

| Name      | Type   | Default | Description                          |
|-----------|--------|---------|--------------------------------------|
| `$source`   | string |         | Path to compressed file (must end in `.gz`) |
| `$target`   | string | `NULL`  | Path to decompressed file (defaults to `$source` without `.gz`) |

**Return Values:**
- `string|FALSE`: Path to the decompressed file on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Validates the source file exists, ends in `.gz`, and is readable.
2. Strips `.gz` from the target path if not provided.
3. Opens the source in Gzip read mode and the target in binary write mode.
4. Locks both files during decompression.
5. Reads the Gzip stream in 64KB chunks and writes to the target.

**Usage Context:**
Used to restore compressed files (e.g., backups or archived logs).

**Example:**
```php
$decompressed = filemanager_gzdecompress("/var/www/backups/database.sql.gz");
// Result: "/var/www/backups/database.sql"
```


<!-- HASH:34f46638baaba72e6f54acd1fcaeda47 -->
