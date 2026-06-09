# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.download.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.download.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Download Management Module (`lib.download.inc`)

This file provides the core functionality for managing downloadable files in the PWNC Web Platform. It includes:
- A **Download class** for handling file uploads, updates, replacements, and deletions
- **Utility functions** for generating UI-friendly data structures (arrays for selection lists)
- **Permission-based access control** for operator-level operations

The module stores metadata in the `#system/download` data store and physical files in `CMS_DATA_PATH . "#download/"`.

---

### Utility Functions

#### `download_get_array()`
Generates a nested associative array of all downloadable files, organized by category.

**Return Values:**
| Type | Description |
|------|-------------|
| `array` | Nested associative array with categories as top-level keys and file names as second-level keys. Values are download identifiers. |

**Inner Mechanisms:**
1. Loads the `#system/download` data store
2. Iterates through all entries
3. Organizes entries by category
4. Handles duplicate names by appending `(1)`, `(2)`, etc.
5. Sorts recursively using natural case-insensitive order

**Usage Example:**
```php
$downloads = download_get_array();
/*
Returns:
[
    "Documents" => [
        "User Manual" => "download://abc123",
        "API Reference" => "download://def456"
    ],
    "Software" => [
        "Installer" => "download://ghi789"
    ]
]
*/
```

---

#### `download_get_select()`
Generates a flat associative array of all unique categories for use in `<select>` elements.

**Return Values:**
| Type | Description |
|------|-------------|
| `array` | Associative array with categories as both keys and values. Empty string key represents uncategorized items. |

**Inner Mechanisms:**
1. Loads the `#system/download` data store
2. Extracts unique category values
3. Sorts using natural case-insensitive order

**Usage Example:**
```php
$categories = download_get_select();
/*
Returns:
[
    "" => "",
    "Documents" => "Documents",
    "Software" => "Software"
]
*/
```

---

## Class: `download`

Manages downloadable files with operator-level permissions.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `data` | `data` | Instance of the `#system/download` data store |
| `operator` | `bool` | Permission flag (TRUE if current user has operator privileges) |

---

### `__construct()`
Initializes the download manager.

**Inner Mechanisms:**
1. Loads the `#system/download` data store
2. Checks operator permissions
3. Ensures the download directory exists

---

### `add()`
Uploads a new downloadable file and stores its metadata.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$uploaded_file` | `string` | Temporary path to the uploaded file |
| `$uploaded_filename` | `string` | Original filename from the upload |
| `$name` | `string` | (Optional) Display name (supports language placeholders) |
| `$description` | `string` | (Optional) Description text |
| `$category` | `string` | (Optional) Category name |

**Return Values:**
| Type | Description |
|------|-------------|
| `string` | Download identifier (e.g., `download://abc123`) on success |
| `bool` | FALSE on failure |

**Inner Mechanisms:**
1. Validates operator permissions and file existence
2. Generates a unique filename with original extension
3. Moves the file to the download directory
4. Stores metadata in the data store
5. Handles default names using language placeholders

**Usage Example:**
```php
$download = new download();
$index = $download->add(
    $_FILES['file']['tmp_name'],
    $_FILES['file']['name'],
    "User Manual",
    "Complete system documentation",
    "Documents"
);
if ($index) {
    echo "File uploaded with ID: $index";
}
```

---

### `set()`
Updates metadata for an existing downloadable file.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Download identifier (e.g., `download://abc123`) |
| `$name` | `string` | New display name (supports language placeholders) |
| `$description` | `string` | New description text |
| `$category` | `string` | New category name |

**Return Values:**
| Type | Description |
|------|-------------|
| `string` | Download identifier on success |
| `bool` | FALSE on failure |

**Inner Mechanisms:**
1. Validates operator permissions and index
2. Preserves existing default name if none provided
3. Updates all metadata fields in the data store

**Usage Example:**
```php
$download = new download();
$result = $download->set(
    "download://abc123",
    "Updated User Manual",
    "Revised documentation with new features",
    "Documentation"
);
if ($result) {
    echo "Metadata updated successfully";
}
```

---

### `replace()`
Replaces the physical file for an existing download while preserving metadata.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Download identifier (e.g., `download://abc123`) |
| `$uploaded_file` | `string` | Temporary path to the new uploaded file |
| `$uploaded_filename` | `string` | Original filename of the new upload |

**Return Values:**
| Type | Description |
|------|-------------|
| `bool` | TRUE on success, FALSE on failure |

**Inner Mechanisms:**
1. Validates operator permissions and file existence
2. Checks if new file has the same extension as the original
3. Deletes the old file if extensions differ
4. Moves the new file to the download directory
5. Updates the filename in metadata if extensions differ

**Usage Example:**
```php
$download = new download();
$result = $download->replace(
    "download://abc123",
    $_FILES['new_file']['tmp_name'],
    $_FILES['new_file']['name']
);
if ($result) {
    echo "File replaced successfully";
}
```

---

### `unlink()`
Deletes both the physical file and its metadata.

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Download identifier (e.g., `download://abc123`) |

**Return Values:**
| Type | Description |
|------|-------------|
| `bool` | TRUE on success, FALSE on failure |

**Inner Mechanisms:**
1. Validates operator permissions and index
2. Deletes the physical file from the download directory
3. Removes the metadata from the data store

**Usage Example:**
```php
$download = new download();
$result = $download->unlink("download://abc123");
if ($result) {
    echo "File deleted successfully";
}
```


<!-- HASH:c09bf5e15bc19a3e0fc49c03f50580cf -->
