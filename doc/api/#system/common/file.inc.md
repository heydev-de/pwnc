# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/file.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/file.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## File Handling Utilities (`file.inc`)

This file provides core file and path manipulation utilities for the PWNC Web Platform. It includes functions for:
- Creating directories recursively
- Reading files safely with locking
- Forcing file downloads with proper headers
- String sanitization for filenames
- File metadata extraction (size, name, extension)
- Safe filename generation
- Remote file retrieval

---

## Functions

### `mkpath($path)`
Creates directories recursively from a given path.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$path`   | `string` | Absolute or relative path to create. Relative paths are resolved from `CMS_ROOT_PATH`. |

**Return Values:**
- `TRUE` if the directory exists or was successfully created.
- `FALSE` if directory creation failed.

**Inner Mechanisms:**
1. Checks if the path already exists as a directory.
2. Strips `CMS_ROOT_PATH` from the path to handle relative paths.
3. Iterates through each directory segment, creating them sequentially.

**Usage Context:**
- Used when saving files to ensure the target directory exists.
- Safe for nested directory structures (e.g., `uploads/2023/12/`).

**Example:**
```php
// Ensure a nested upload directory exists
if (!mkpath(CMS_ROOT_PATH . "/uploads/2023/12")) {
    die("Failed to create directory.");
}
```

---

### `read_file($file)`
Reads a file's contents with shared locking to prevent concurrent modifications.

| Parameter | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$file`   | `string` | Path to the file to read.       |

**Return Values:**
- `string` containing the file's contents on success.
- `FALSE` if the file doesn’t exist or cannot be read.

**Inner Mechanisms:**
1. Opens the file in binary read mode (`"rb"`).
2. Acquires a shared lock (`LOCK_SH`) to allow concurrent reads but block writes.
3. Reads the entire file content using `stream_get_contents()`.

**Usage Context:**
- Reading configuration files or templates.
- Safe for concurrent access (e.g., in high-traffic environments).

**Example:**
```php
$config = read_file(CMS_ROOT_PATH . "/config/settings.ini");
if ($config === FALSE) {
    die("Failed to read configuration.");
}
```

---

### `download($file, $name = NULL)`
Forces a file download with proper HTTP headers, ETag support, and chunked transfer.

| Parameter | Type      | Description                                                                 |
|-----------|-----------|-----------------------------------------------------------------------------|
| `$file`   | `string`  | Path to the file to download.                                               |
| `$name`   | `string`  | Optional custom filename. If `NULL`, the original filename is used.         |

**Return Values:**
- `int` (bytes sent) on success.
- `FALSE` if the file doesn’t exist, headers were already sent, or the download failed.

**Inner Mechanisms:**
1. Validates file existence and checks for sent headers.
2. Generates an ETag from the file’s modification time and size.
3. Sends `304 Not Modified` if the client’s `If-None-Match` header matches the ETag.
4. Streams the file in 512KB chunks to avoid memory issues.
5. Handles Excel files (`xls`) with UTF-16LE encoding.

**Usage Context:**
- User-initiated downloads (e.g., reports, media files).
- Supports large files without memory exhaustion.

**Example:**
```php
// Force download of a generated report
if (download(CMS_ROOT_PATH . "/reports/2023_summary.pdf", "Annual_Report_2023.pdf") === FALSE) {
    die("Download failed.");
}
```

---

### `ansi_transliteration($string)`
Converts accented and special characters to their closest ASCII equivalents.

| Parameter | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$string` | `string` | Input string to transliterate.  |

**Return Values:**
- `string` with transliterated characters.

**Inner Mechanisms:**
- Uses a static associative array to map non-ASCII characters to their ASCII equivalents.
- Handles common European characters (e.g., `ä` → `ae`, `ß` → `ss`).

**Usage Context:**
- Sanitizing filenames or URLs for compatibility.
- Used internally by `stringtofilename()` and `safe_filename()`.

**Example:**
```php
$title = "Café & Résumé";
echo ansi_transliteration($title); // Output: "Cafe & Resume"
```

---

### `stringtofilename($string, $replacement = "-")`
Converts a string to a URL-safe filename by transliterating and replacing non-alphanumeric characters.

| Parameter      | Type     | Description                                                                 |
|----------------|----------|-----------------------------------------------------------------------------|
| `$string`      | `string` | Input string to convert.                                                    |
| `$replacement` | `string` | Character to replace non-alphanumeric sequences (default: `"-"`).          |

**Return Values:**
- `string` with a sanitized filename.

**Inner Mechanisms:**
1. Applies `ansi_transliteration()` to the input.
2. Strips leading/trailing non-alphanumeric characters.
3. Replaces internal non-alphanumeric sequences with `$replacement`.
4. Converts the result to lowercase.

**Usage Context:**
- Generating filenames from user input (e.g., blog post titles).
- Ensures compatibility with filesystems and URLs.

**Example:**
```php
$post_title = "How to Use PWNC: A Beginner's Guide!";
echo stringtofilename($post_title); // Output: "how-to-use-pwnc-a-beginners-guide"
```

---

### `basename($path, $suffix = NULL)`
Extracts the filename from a path, optionally removing a suffix.

| Parameter | Type      | Description                                                                 |
|-----------|-----------|-----------------------------------------------------------------------------|
| `$path`   | `string`  | File path (e.g., `/var/www/file.txt`).                                      |
| `$suffix` | `string`  | Optional suffix to remove (e.g., `.txt`).                                   |

**Return Values:**
- `string` containing the filename (e.g., `file.txt` or `file` if `$suffix` is provided).

**Inner Mechanisms:**
1. Trims trailing slashes from the path.
2. Extracts the last segment after the final `/`.
3. Removes the suffix if it matches the end of the filename.

**Usage Context:**
- Extracting filenames from full paths (e.g., for display or logging).

**Example:**
```php
$path = "/var/www/uploads/image.jpg";
echo basename($path);        // Output: "image.jpg"
echo basename($path, ".jpg"); // Output: "image"
```

---

### `file_size($path)`
Retrieves the size of a local or remote file.

| Parameter | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$path`   | `string` | Local path or remote URL.       |

**Return Values:**
- `int` (file size in bytes) on success.
- `FALSE` if the size cannot be determined.

**Inner Mechanisms:**
1. For local files, uses `filesize()`.
2. For remote files, sends a `HEAD` request and reads the `Content-Length` header.

**Usage Context:**
- Validating file uploads or downloads.
- Displaying file sizes to users.

**Example:**
```php
$size = file_size("https://example.com/image.jpg");
if ($size !== FALSE) {
    echo "File size: " . round($size / 1024) . " KB";
}
```

---

### `file_name($path, $extension = FALSE)`
Extracts the filename or extension from a path, with caching for performance.

| Parameter    | Type      | Description                                                                 |
|--------------|-----------|-----------------------------------------------------------------------------|
| `$path`      | `string`  | File path or URL.                                                           |
| `$extension` | `boolean` | If `TRUE`, returns the extension; otherwise, returns the filename.          |

**Return Values:**
- `string` (filename or extension) on success.
- `FALSE` if the path cannot be parsed.

**Inner Mechanisms:**
1. Uses `pathinfo()` for local files or `analyze_url()` for remote URLs.
2. Caches results in a static array to avoid repeated parsing.

**Usage Context:**
- Extracting metadata from file paths (e.g., for storage or display).

**Example:**
```php
$path = "/var/www/uploads/document.pdf";
echo file_name($path);        // Output: "document"
echo file_name($path, TRUE);  // Output: "pdf"
```

---

### `file_extension($path)`
Alias for `file_name($path, TRUE)`.

| Parameter | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$path`   | `string` | File path or URL.               |

**Return Values:**
- `string` (file extension) on success.
- `FALSE` if the path cannot be parsed.

**Example:**
```php
echo file_extension("/var/www/image.png"); // Output: "png"
```

---

### `safe_filename($string)`
Generates a filesystem-safe filename with a CRC32 hash suffix for uniqueness.

| Parameter | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$string` | `string` | Input string to convert.        |

**Return Values:**
- `string` with a sanitized filename (e.g., `my_file-1a2b3c4d`).

**Inner Mechanisms:**
1. Checks if the string is already safe (alphanumeric + `.`).
2. Transliterates and replaces non-safe characters with underscores.
3. Appends a CRC32 hash of the original string for uniqueness.

**Usage Context:**
- Storing user-uploaded files with predictable but unique names.

**Example:**
```php
$user_input = "My Document #1.pdf";
echo safe_filename($user_input); // Output: "My_Document_1-1a2b3c4d.pdf"
```

---

### `retrieve_file($source_url, $target_path, $timeout = 60)`
Downloads a remote file to a local path with timeout and error handling.

| Parameter      | Type      | Description                                                                 |
|----------------|-----------|-----------------------------------------------------------------------------|
| `$source_url`  | `string`  | URL of the remote file.                                                     |
| `$target_path` | `string`  | Local path to save the file.                                                |
| `$timeout`     | `int`     | Timeout in seconds (default: `60`).                                         |

**Return Values:**
- `TRUE` on success.
- `FALSE` on failure (e.g., timeout, invalid URL).

**Inner Mechanisms:**
1. Uses a temporary file to avoid partial downloads.
2. Sets a custom `User-Agent` header with `CMS_USER_AGENT`.
3. Follows redirects and respects timeouts.

**Usage Context:**
- Downloading assets (e.g., images, PDFs) from external sources.

**Example:**
```php
if (!retrieve_file("https://example.com/logo.png", CMS_ROOT_PATH . "/assets/logo.png")) {
    die("Failed to download file.");
}
```


<!-- HASH:cf99604bcc0cfeab8bac4b84403e08ce -->
