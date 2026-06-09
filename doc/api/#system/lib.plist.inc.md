# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.plist.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.plist.inc)

- **Version:** `26.5.30.4`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## PLIST Class

The `plist` class provides a persistent, file-based list storage mechanism optimized for high-performance, concurrent access. It implements a fixed-record-length binary file format with CRC32 hashing for integrity and efficient lookups. This class is ideal for managing ordered collections of strings (e.g., logs, queues, or caches) where fast append, retrieval, and removal operations are required.

### Properties

| Name              | Default Value | Description                                                                 |
|-------------------|---------------|-----------------------------------------------------------------------------|
| `$hfile`          | `NULL`        | File handle for the underlying `.lst` file.                                |
| `$record_length`  | `NULL`        | Maximum length (in bytes) of each stored record.                           |
| `$buffer_length`  | `65536`       | Internal buffer size (in bytes) for file operations.                       |

---

### `__construct`

#### Purpose
Initializes a new `plist` instance, creating or opening a binary list file with the specified record length.

#### Parameters

| Name              | Type      | Description                                                                 |
|-------------------|-----------|-----------------------------------------------------------------------------|
| `$name`           | `string`  | Base name of the list file. If no `.lst` extension is provided, it is appended automatically. The file is stored in `CMS_DATA_PATH`. |
| `$record_length`  | `int`     | (Optional) Maximum length of each record in bytes. Default: `500`.          |

#### Return Values
- **`void`**: Constructor does not return a value.

#### Inner Mechanisms
1. **File Path Resolution**: Appends `.lst` to `$name` if not present and prepends `CMS_DATA_PATH`.
2. **Directory Creation**: Uses `mkpath()` to ensure the parent directory exists.
3. **File Opening**: Opens the file in `c+b` mode (create, read/write, binary) and stores the handle in `$hfile`.
4. **Record Length**: Sets the instance’s `$record_length` property.

#### Usage Context
Use this constructor to initialize a persistent list for storing strings (e.g., user activity logs, job queues, or session histories).

#### Example
```php
$log = new \cms\plist("user_activity", 256);
// Creates/opens CMS_DATA_PATH/user_activity.lst with 256-byte records
```

---

### `add`

#### Purpose
Appends a value to the list. Optionally removes any existing identical value before adding.

#### Parameters

| Name                 | Type      | Description                                                                 |
|----------------------|-----------|-----------------------------------------------------------------------------|
| `$value`             | `string`  | The string value to add. Truncated to `$record_length` if longer.          |
| `$remove_existing`   | `bool`    | (Optional) If `TRUE`, removes all existing occurrences of `$value` before adding. Default: `FALSE`. |

#### Return Values
- **`bool`**: `TRUE` on success, `FALSE` if the file handle is invalid.

#### Inner Mechanisms
1. **Locking**: Acquires an exclusive lock (`LOCK_EX`) on the file.
2. **Value Preparation**: Truncates and right-pads `$value` with null bytes to `$record_length`.
3. **Hashing**: Computes a 4-byte CRC32 hash of the padded value for fast comparison.
4. **Existing Value Removal** (if `$remove_existing`):
   - Scans the file backward from the end.
   - For each record, compares its hash and value with the new one.
   - If a match is found, shifts subsequent records left to overwrite it.
   - Truncates the file to remove the last record.
5. **Appending**: Writes the hash and padded value to the end of the file.
6. **Unlocking**: Releases the lock (`LOCK_UN`).

#### Usage Context
Use `add()` to insert values into the list. Enable `$remove_existing` to enforce uniqueness (e.g., for deduplicated logs or queues).

#### Example
```php
$queue = new \cms\plist("task_queue", 100);
$queue->add("process_user_data:12345", TRUE);
// Ensures "process_user_data:12345" is added only once
```

---

### `get`

#### Purpose
Retrieves one or more values from the list, starting from a specified offset (counted from the end). Optionally removes the retrieved values.

#### Parameters

| Name        | Type      | Description                                                                 |
|-------------|-----------|-----------------------------------------------------------------------------|
| `$offset`   | `int`     | (Optional) Zero-based offset from the end of the list. Default: `0` (newest value). |
| `$length`   | `int`     | (Optional) Number of values to retrieve. If `0` or negative, retrieves all values from `$offset`. Default: `1`. |
| `$remove`   | `bool`    | (Optional) If `TRUE`, removes the retrieved values from the list. Default: `FALSE`. |

#### Return Values
- **`array`|`FALSE`**: Array of retrieved values (trimmed of null bytes), or `FALSE` if the file handle is invalid.

#### Inner Mechanisms
1. **Locking**: Acquires an exclusive lock (`LOCK_EX`).
2. **Offset Calculation**: Converts `$offset` to a file position (from the end).
3. **Reading**: Reads records backward from the calculated position, trimming null bytes.
4. **Removal** (if `$remove`): Calls `remove()` with the same `$offset` and the number of retrieved values.
5. **Unlocking**: Releases the lock.

#### Usage Context
Use `get()` to fetch values (e.g., processing a queue or reading logs). Set `$remove` to implement a destructive read (e.g., for job queues).

#### Example
```php
$queue = new \cms\plist("task_queue", 100);
$tasks = $queue->get(0, 5, TRUE);
// Retrieves and removes the 5 newest tasks
```

---

### `length`

#### Purpose
Returns the number of records in the list.

#### Parameters
- None.

#### Return Values
- **`int`|`FALSE`**: Number of records, or `FALSE` if the file handle is invalid.

#### Inner Mechanisms
1. **File Size**: Retrieves the file size using `fstat()`.
2. **Record Count**: Divides the file size by the record length (including the 4-byte hash) and casts to an integer.

#### Usage Context
Use `length()` to monitor the size of the list (e.g., for capacity checks or progress tracking).

#### Example
```php
$log = new \cms\plist("access_log");
if ($log->length() > 10000) {
    // Rotate log if it exceeds 10,000 entries
}
```

---

### `remove`

#### Purpose
Removes one or more records from the list, starting from a specified offset (counted from the end).

#### Parameters

| Name        | Type      | Description                                                                 |
|-------------|-----------|-----------------------------------------------------------------------------|
| `$offset`   | `int`     | Zero-based offset from the end of the list.                                |
| `$length`   | `int`     | (Optional) Number of records to remove. If `0` or negative, removes all records from `$offset` to the start. Default: `1`. |

#### Return Values
- **`bool`**: `TRUE` on success, `FALSE` if the file handle is invalid.

#### Inner Mechanisms
1. **Locking**: Acquires an exclusive lock (`LOCK_EX`).
2. **Offset Calculation**: Converts `$offset` to a file position (from the end).
3. **Shifting**: Reads chunks of data from `$offset + $length` and writes them to `$offset`, effectively overwriting the removed records.
4. **Truncation**: Reduces the file size to exclude the removed records.
5. **Unlocking**: Releases the lock.

#### Usage Context
Use `remove()` to prune old records (e.g., log rotation) or clear processed items from a queue.

#### Example
```php
$log = new \cms\plist("access_log");
$log->remove(1000);
// Removes the 1000 oldest records
```

---

### `__destruct`

#### Purpose
Closes the file handle when the `plist` instance is destroyed.

#### Parameters
- None.

#### Return Values
- **`void`**: Destructor does not return a value.

#### Inner Mechanisms
1. **File Handle Check**: Verifies `$hfile` is valid.
2. **Closing**: Calls `fclose()` on the file handle.

#### Usage Context
Automatically invoked when the object is garbage-collected or explicitly unset. Ensures proper resource cleanup.


<!-- HASH:35ce73bfa168b2206284e300b4801f9f -->
