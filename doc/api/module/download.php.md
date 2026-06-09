# PWNC API Documentation

[← Index](../README.md) | [`module/download.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/download.php)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Module: `module/download.php`

**Overview:**
This file implements the download module for the PWNC Web Platform. It handles the secure delivery of files stored in the system's download repository. The module provides a two-step process:
1. **Preview Page:** Displays metadata (name, size, description) and initiates the download after a short delay.
2. **Download Handler:** Streams the file to the client with proper headers and logs the access.

The module integrates with the platform's data management, URL generation, and logging systems. It ensures files are only accessible if they exist in the database and on disk.

---

### Constants & Global Variables

| Name               | Value/Default | Description                                                                 |
|--------------------|---------------|-----------------------------------------------------------------------------|
| `$download_index`  | (Global)      | Identifier for the requested download (from URL parameters).                |
| `$download_start`  | (Global)      | Flag indicating whether the download should start immediately (bypasses preview). |

---

### Core Logic Flow

1. **Initialization:**
   - Loads the `download` library if `$download_index` is provided.
   - Validates the file's existence in the database and filesystem.

2. **Preview Mode (`$download_start` is empty):**
   - Renders an HTML page with file metadata.
   - Uses JavaScript to redirect to the download URL after 2.5 seconds.

3. **Download Mode (`$download_start` is set):**
   - Streams the file to the client using the `download()` function.
   - Logs the access event via the `log` class.

---

### Key Functions & Methods

#### `download($path)`
**Purpose:**
Streams a file to the client with appropriate HTTP headers for forced download.

**Parameters:**

| Name   | Type     | Description                          |
|--------|----------|--------------------------------------|
| `$path`| `string` | Absolute filesystem path to the file.|

**Return Values:**
- `bool`: `TRUE` if the file was successfully streamed, `FALSE` on failure.

**Inner Mechanisms:**
- Sets headers for:
  - Content-Type (derived from file extension).
  - Content-Disposition (forces download with the original filename).
  - Content-Length (file size in bytes).
- Uses `readfile()` to output the file in chunks (memory-efficient for large files).
- Terminates script execution after streaming.

**Usage Context:**
- Called when `$download_start` is set to `1`.
- Ensures files are delivered securely without exposing filesystem paths.

**Example:**
```php
// Stream a file directly (e.g., for API endpoints)
if (download("/var/www/data/downloads/report.pdf")) {
    exit(); // Prevent further output
}
```

---

### Helper Classes

#### `download` Class
**Purpose:**
Manages downloadable files, including metadata retrieval and path resolution.

**Methods:**

##### `data->get($index, $key)`
**Purpose:**
Retrieves metadata for a downloadable file from the database.

**Parameters:**

| Name    | Type     | Description                          |
|---------|----------|--------------------------------------|
| `$index`| `string` | Download identifier.                 |
| `$key`  | `string` | Metadata key (e.g., `"name"`, `"description"`). |

**Return Values:**
- `string|null`: The value if found, `NULL` otherwise.

**Inner Mechanisms:**
- Queries the database using the platform's `data` abstraction layer.
- Caches results to avoid repeated queries.

**Usage Context:**
- Used to fetch metadata for the preview page (e.g., name, description).

**Example:**
```php
$download = new download();
$name = $download->data->get("manual_v2", "name"); // Returns "User Manual"
```

##### `data->remove_prefix($index)`
**Purpose:**
Fallback method to extract a filename from an index (for legacy compatibility).

**Parameters:**

| Name    | Type     | Description          |
|---------|----------|----------------------|
| `$index`| `string` | Download identifier. |

**Return Values:**
- `string`: The filename without the prefix.

**Inner Mechanisms:**
- Strips a predefined prefix (e.g., `"dl_"`) from the index to derive the filename.

**Usage Context:**
- Used when metadata is missing to ensure backward compatibility.

**Example:**
```php
$filename = $download->data->remove_prefix("dl_archive.zip"); // Returns "archive.zip"
```

---

### URL & Parameter Handling

#### `cms_url()`
**Purpose:**
Generates a URL for the download module with the required parameters.

**Parameters:**

| Name      | Type               | Description                                                                 |
|-----------|--------------------|-----------------------------------------------------------------------------|
| `$addr`   | `array`            | Associative array of parameters (e.g., `["download_index" => "file123"]`). |
| `$param`  | `array` (Optional) | Additional parameters to merge.                                             |
| `$omit`   | `bool` (Optional)  | If `TRUE`, omits global parameters.                                         |

**Return Values:**
- `string`: The generated URL.

**Usage Context:**
- Used to create the download link in the preview page.

**Example:**
```php
$url = cms_url(["download_index" => "manual_v2", "download_start" => 1]);
// Returns: "/?download_index=manual_v2&download_start=1&csrf=..."
```

---

### Security & Validation

1. **File Existence Check:**
   - Validates that the file exists in `CMS_DATA_PATH . "#download/"` before streaming.

2. **CSRF Protection:**
   - All URLs generated via `cms_url()` include a CSRF token.

3. **Metadata Validation:**
   - Ensures the file has a valid `name` in the database before proceeding.

4. **Path Sanitization:**
   - Uses `is_file()` to prevent directory traversal attacks.

---

### Usage Example: Adding a Downloadable File

**Scenario:**
A developer wants to add a downloadable PDF to the system.

**Steps:**
1. **Upload the File:**
   - Place the file in `CMS_DATA_PATH . "#download/"` (e.g., `report.pdf`).

2. **Add Metadata to Database:**
   ```sql
   INSERT INTO downloads (index, filename, name, description)
   VALUES ('report_2026', 'report.pdf', 'Annual Report 2026', 'Financial summary for 2026.');
   ```

3. **Link to the Download:**
   - Use `cms_url()` to generate a link for the preview page:
     ```php
     $link = cms_url(["download_index" => "report_2026"]);
     echo "<a href=\"" . x($link) . "\">Download Report</a>";
     ```

**Result:**
- Users click the link, see the preview page, and the file downloads automatically after 2.5 seconds.
- The download is logged in the system.


<!-- HASH:c7fdf3b8c70bce477ee63a04b171ab58 -->
