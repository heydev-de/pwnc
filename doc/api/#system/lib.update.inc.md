# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.update.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.update.inc)

- **Version:** `26.5.30.4`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Update Class

The `update` class provides a self-contained, atomic update mechanism for the PWNC Web Platform. It handles version checks, backup creation, download, installation, and cleanup of platform updates from a remote GitHub repository. The class maintains a persistent state across multiple HTTP requests, allowing long-running update processes to resume after interruptions.

### Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_UPDATE_PATH` | `CMS_ROOT_PATH . "#update/"` | Local filesystem path for update-related files. |
| `CMS_UPDATE_URL_VERSION` | `"https://raw.githubusercontent.com/heydev-de/pwnc/refs/heads/main/nuos/version.txt"` | Remote URL to fetch the latest platform version. |
| `CMS_UPDATE_URL_ARCHIVE` | `"https://github.com/heydev-de/pwnc/archive/main.zip"` | Remote URL to download the latest platform archive. |
| `CMS_UPDATE_TOKEN` | `""` | Optional GitHub API token for authenticated requests. |
| `CMS_UPDATE_CHECK_INTERVAL` | `3600` | Minimum interval (in seconds) between version checks. |
| `CMS_UPDATE_STATUS_ERROR` | `-1` | Update process failed. |
| `CMS_UPDATE_STATUS_NONE` | `0` | No update in progress. |
| `CMS_UPDATE_STATUS_DONE` | `1` | Current step completed successfully. |
| `CMS_UPDATE_STATUS_BACKUP` | `2` | Backup step in progress. |
| `CMS_UPDATE_STATUS_DOWNLOAD` | `3` | Download step in progress. |
| `CMS_UPDATE_STATUS_INSTALL` | `4` | Installation step in progress. |
| `CMS_UPDATE_STATUS_CLEANUP` | `5` | Cleanup step in progress. |

---

### `__construct()`

**Purpose**
Initializes the update environment by ensuring the update directory exists.

**Parameters**
None.

**Return Values**
None.

**Inner Mechanisms**
- Calls `mkpath()` to create `CMS_UPDATE_PATH` if it does not exist.

**Usage Context**
Automatically invoked when an `update` object is instantiated. No manual invocation required.

---

### `log($text, $reset = FALSE, $newline = TRUE)`

**Purpose**
Appends or resets the update log file with the provided text.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$text` | `string` | — | Text to write to the log. |
| `$reset` | `bool` | `FALSE` | If `TRUE`, overwrites the log; otherwise appends. |
| `$newline` | `bool` | `TRUE` | If `TRUE` and not resetting, prepends a newline. |

**Return Values**
`bool` – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**
- Opens `CMS_UPDATE_PATH . "update.log"` in append or overwrite mode.
- Locks the file exclusively during write.
- Returns `TRUE` if `file_put_contents()` succeeds.

**Usage Example**
```php
$update = new update();
$update->log("Starting update process", TRUE); // Reset log
$update->log("Downloading archive");          // Append
```

---

### `get_log()`

**Purpose**
Retrieves the entire content of the update log.

**Parameters**
None.

**Return Values**
`string` – Log content or empty string if the log does not exist.

**Inner Mechanisms**
- Checks if `CMS_UPDATE_PATH . "update.log"` exists.
- Returns its content via `file_get_contents()`.

**Usage Example**
```php
$log = $update->get_log();
echo "<pre>$log</pre>";
```

---

### `progress()`

**Purpose**
Appends an ellipsis (`…`) to the log to indicate ongoing activity.

**Parameters**
None.

**Return Values**
`bool` – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**
- Calls `log("…", FALSE, FALSE)` to append without newline.

**Usage Example**
```php
while ($running) {
    $update->progress();
    sleep(1);
}
```

---

### `status($value = CMS_UPDATE_STATUS_NONE, $text = "")`

**Purpose**
Sets or retrieves the current update status.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$value` | `int` | `CMS_UPDATE_STATUS_NONE` | Status constant to set. |
| `$text` | `string` | `""` | Optional log message to write. |

**Return Values**
`bool` – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**
- If `$value` is not `CMS_UPDATE_STATUS_NONE` and the current status is `CMS_UPDATE_STATUS_NONE`, resets the log.
- Writes `$text` to the log if provided.
- Writes `$value` to `CMS_UPDATE_PATH . "update.status"` with exclusive lock.

**Usage Example**
```php
$update->status(CMS_UPDATE_STATUS_DOWNLOAD, "Downloading update archive");
```

---

### `get_status()`

**Purpose**
Retrieves the current update status.

**Parameters**
None.

**Return Values**
`int` – One of the `CMS_UPDATE_STATUS_*` constants.

**Inner Mechanisms**
- Opens `CMS_UPDATE_PATH . "update.status"` with shared lock.
- Reads and returns its content as integer.

**Usage Example**
```php
switch ($update->get_status()) {
    case CMS_UPDATE_STATUS_DOWNLOAD:
        echo "Download in progress";
        break;
    case CMS_UPDATE_STATUS_DONE:
        echo "Update complete";
        break;
}
```

---

### `error($text)`

**Purpose**
Sets the update status to `CMS_UPDATE_STATUS_ERROR` and logs the error message.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$text` | `string` | — | Error message to log. |

**Return Values**
`bool` – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**
- Calls `status(CMS_UPDATE_STATUS_ERROR, $text)`.

**Usage Example**
```php
if (! $mysql->backup()) {
    $update->error("Database backup failed");
}
```

---

### `available($enforce = FALSE)`

**Purpose**
Checks if a newer platform version is available.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$enforce` | `bool` | `FALSE` | If `TRUE`, bypasses the check interval. |

**Return Values**
`string|bool|null` – Latest version string if newer, `FALSE` on error, `NULL` if up-to-date.

**Inner Mechanisms**
- Checks `CMS_UPDATE_PATH . "update.check"` for cached version and timestamp.
- If cache is stale or `$enforce` is `TRUE`, fetches `CMS_UPDATE_URL_VERSION` via HTTP GET.
- Compares fetched version with `CMS_VERSION` using `version_compare()`.

**Usage Example**
```php
if ($version = $update->available()) {
    echo "Update available: $version";
}
```

---

### `start($skip_backup = FALSE)`

**Purpose**
Orchestrates the entire update process: backup, download, install, and cleanup.

**Parameters**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$skip_backup` | `bool` | `FALSE` | If `TRUE`, skips the backup step. |

**Return Values**
`bool` – `TRUE` if all steps succeed, `FALSE` otherwise.

**Inner Mechanisms**
- Checks if an update is already in progress.
- Calls `backup()`, `download()`, `install()`, and `cleanup()` in sequence.
- Sets final status to `CMS_UPDATE_STATUS_DONE` on completion.

**Usage Example**
```php
if ($update->start()) {
    echo "Update completed successfully";
} else {
    echo "Update failed";
}
```

---

### `backup()`

**Purpose**
Creates a full backup of the platform: database dump and filesystem archive.

**Parameters**
None.

**Return Values**
`bool` – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**
- Sets status to `CMS_UPDATE_STATUS_BACKUP`.
- Uses `mysql` class to create a database backup.
- Recursively archives `CMS_ROOT_PATH` into a ZIP file, excluding cache and Git directories.
- Handles large file sets with periodic `set_time_limit()` calls.
- Moves previous backup to `_backup.zip` for rollback.

**Usage Example**
```php
if ($update->backup()) {
    echo "Backup created";
}
```

---

### `download()`

**Purpose**
Downloads the latest platform archive from `CMS_UPDATE_URL_ARCHIVE`.

**Parameters**
None.

**Return Values**
`bool` – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**
- Sets status to `CMS_UPDATE_STATUS_DOWNLOAD`.
- Opens a stream to `CMS_UPDATE_URL_ARCHIVE` with `User-Agent` and optional `Authorization` headers.
- Writes the response to `CMS_UPDATE_PATH . "update.zip"` with exclusive lock.
- Handles large downloads with periodic `set_time_limit()` calls.

**Usage Example**
```php
if ($update->download()) {
    echo "Archive downloaded";
}
```

---

### `install()`

**Purpose**
Installs the downloaded update by replacing platform directories.

**Parameters**
None.

**Return Values**
`bool` – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms**
- Sets status to `CMS_UPDATE_STATUS_INSTALL`.
- Extracts `update.zip` to `CMS_UPDATE_PATH . "new/"`.
- Moves current `pwnc` and `nuos` directories to a timestamped rollback directory.
- Moves extracted directories to `CMS_ROOT_PATH`.
- Handles temporary migration from `nuos` to `pwnc` if needed.
- Rolls back on error.

**Usage Example**
```php
if ($update->install()) {
    echo "Update installed";
}
```

---

### `cleanup()`

**Purpose**
Removes temporary update files and directories.

**Parameters**
None.

**Return Values**
`bool` – Always `TRUE`.

**Inner Mechanisms**
- Deletes `update.zip`, `new/`, and `old/` directories.
- Uses `filemanager_delete()` if the `filemanager` library is loaded.

**Usage Example**
```php
$update->cleanup();
```


<!-- HASH:e044b259f77e7fdd61f08d197218c2ec -->
