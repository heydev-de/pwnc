# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.core_resource.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.core_resource.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Core Resource Class

The `core_resource` class provides a low-level, high-performance binary recordset manager for structured flat-file data storage. It enables efficient reading, writing, and searching of fixed-length records with typed fields (strings, UTF-8 strings, and bytes). This class is designed for scenarios requiring fast, direct file access without a database, such as caching, logging, or configuration storage.

### Properties

| Name                | Default/Value | Description                                                                                     |
|---------------------|---------------|-------------------------------------------------------------------------------------------------|
| `file`              | `""`          | Path to the binary data file.                                                                  |
| `hfile`             | `NULL`        | File handle resource.                                                                          |
| `field_type`        | `NULL`        | Associative array mapping field names to their types (`string`, `_string`, `byte`).            |
| `field_offset`      | `NULL`        | Associative array mapping field names to their byte offsets within a record.                   |
| `field_length`      | `NULL`        | Associative array mapping field names to their byte lengths.                                   |
| `recordset_length`  | `0`           | Total byte length of a single record.                                                          |
| `offset`            | `0`           | Current byte offset within the file (used for navigation).                                     |
| `zero`              | `""`          | A string of null bytes (`\0`) with the same length as a record (used for deletion).            |
| `lock`              | `[]`          | Stack of lock states (shared or exclusive) for concurrency control.                            |

---

### Constructor: `__construct($file, $structure)`

#### Purpose
Initializes a new binary recordset file with a given structure. Creates the file if it does not exist.

#### Parameters

| Name        | Type         | Description                                                                                     |
|-------------|--------------|-------------------------------------------------------------------------------------------------|
| `$file`     | `string`     | Path to the binary data file.                                                                  |
| `$structure`| `array`      | Associative array defining the record structure. Keys are field names; values are type specs.  |

Type Specifications:
- `string[length]`: Fixed-length ASCII string (e.g., `string[10]`).
- `_string[length]`: Fixed-length UTF-8 string (stored as 4 bytes per character).
- `byte`: Single byte (0–255).

#### Return Value
None (constructor).

#### Inner Mechanisms
- Parses the structure to compute field offsets and lengths.
- Calculates the total record length.
- Opens the file in read/write mode (`r+b` or `w+b`).
- Disables write buffering for immediate I/O.
- Registers a shutdown function to ensure the file is closed.

#### Usage Example
```php
$resource = new \cms\core_resource(
    "/data/cache/users.dat",
    [
        "id"    => "byte",
        "name"  => "string[32]",
        "email" => "_string[64]"
    ]
);
```
This creates a binary file where each record is 1 + 32 + 256 = 289 bytes long.

---

### Method: `current()`

#### Purpose
Returns the current byte offset within the file, adjusted to the start of the nearest record.

#### Parameters
None.

#### Return Value
- `int`: Current byte offset (always ≥ 0).

#### Inner Mechanisms
- Returns `max(0, $this->offset)` to ensure non-negative values.

#### Usage Example
```php
$offset = $resource->current(); // e.g., 289 (start of second record)
```

---

### Method: `reset()`

#### Purpose
Resets the internal offset to the position before the first record (i.e., `-recordset_length`).

#### Parameters
None.

#### Return Value
None.

#### Inner Mechanisms
- Sets `$this->offset = -$this->recordset_length` to enable `next()` to start from the beginning.

#### Usage Example
```php
$resource->reset();
while ($resource->next()) {
    $record = $resource->get();
    // Process record
}
```

---

### Method: `next($filter = NULL, $reset = FALSE, $limit = NULL)`

#### Purpose
Advances the internal offset to the next record, optionally filtering by field values.

#### Parameters

| Name     | Type          | Description                                                                                     |
|----------|---------------|-------------------------------------------------------------------------------------------------|
| `$filter`| `array\|null` | Associative array of field-value pairs. Only records matching all pairs are considered.        |
| `$reset` | `bool`        | If `TRUE`, resets the offset to the start before searching.                                    |
| `$limit` | `int\|null`   | Maximum byte offset to search up to (defaults to file size).                                   |

Filter Values:
- `TRUE`: Field must be non-empty.
- `NULL` or `FALSE`: Field must be empty.
- Any other value: Field must match exactly.

#### Return Value
- `bool`: `TRUE` if a matching record was found; `FALSE` otherwise.

#### Inner Mechanisms
- Locks the file for shared access.
- Advances the offset record by record.
- For each record, checks if all filter conditions are met.
- Unlocks the file before returning.

#### Usage Example
```php
// Find the next record where "id" is 5 and "name" is not empty
if ($resource->next(["id" => 5, "name" => TRUE])) {
    $user = $resource->get();
    echo "Found user: " . $user["name"];
}
```

---

### Method: `seek($filter, $next = FALSE)`

#### Purpose
Searches for a record matching the filter, starting from the current position (or the next record if `$next` is `TRUE`).

#### Parameters

| Name     | Type    | Description                                                                                     |
|----------|---------|-------------------------------------------------------------------------------------------------|
| `$filter`| `array` | Associative array of field-value pairs (same as `next()`).                                     |
| `$next`  | `bool`  | If `TRUE`, starts searching from the next record.                                               |

#### Return Value
- `bool`: `TRUE` if a matching record was found; `FALSE` otherwise.

#### Inner Mechanisms
- Saves the current offset.
- If `$next` is `TRUE`, advances the offset by one record.
- Calls `next()` twice: once from the current position, and once from the start if no match is found.

#### Usage Example
```php
// Find the first record where "email" is "user@example.com"
if ($resource->seek(["email" => "user@example.com"])) {
    $user = $resource->get();
}
```

---

### Method: `get($key = NULL, $raw = FALSE)`

#### Purpose
Retrieves the current record or a specific field from it.

#### Parameters

| Name   | Type     | Description                                                                                     |
|--------|----------|-------------------------------------------------------------------------------------------------|
| `$key` | `string` | Field name to retrieve. If `NULL`, returns the entire record.                                  |
| `$raw` | `bool`   | If `TRUE`, returns raw binary data; otherwise, decodes the field value.                        |

#### Return Value
- `array\|string\|int\|null`:
  - If `$key` is `NULL`: Associative array of all fields.
  - If `$key` is valid: Decoded field value (or raw binary if `$raw` is `TRUE`).
  - If `$key` is invalid: `NULL`.

#### Inner Mechanisms
- Locks the file for shared access.
- Reads the record at the current offset.
- Decodes each field using `field_decode()` unless `$raw` is `TRUE`.
- Unlocks the file before returning.

#### Usage Example
```php
$record = $resource->get(); // Get all fields
$name = $resource->get("name"); // Get "name" field
$raw_id = $resource->get("id", TRUE); // Get raw binary "id" (1 byte)
```

---

### Method: `set($value)`

#### Purpose
Updates the current record with new field values.

#### Parameters

| Name    | Type    | Description                                                                                     |
|---------|---------|-------------------------------------------------------------------------------------------------|
| `$value`| `array` | Associative array of field-value pairs. Only provided fields are updated.                      |

#### Return Value
- `bool`: `TRUE` on success.

#### Inner Mechanisms
- Locks the file for exclusive access.
- Reads the current record (or uses a zero-filled record if none exists).
- Merges new values with existing ones.
- Encodes each field using `field_encode()`.
- Packs fields into binary format using `pack("a<length>", ...)`.
- Writes the updated record to the file.
- Unlocks the file.

#### Usage Example
```php
$resource->set([
    "name"  => "Alice",
    "email" => "alice@example.com"
]);
```

---

### Method: `del()`

#### Purpose
Deletes the current record by overwriting it with null bytes.

#### Parameters
None.

#### Return Value
- `bool`: `TRUE` on success.

#### Inner Mechanisms
- Locks the file for exclusive access.
- Writes `$this->zero` (a string of null bytes) to the current offset.
- Unlocks the file.

#### Usage Example
```php
$resource->del(); // Delete the current record
```

---

### Method: `field_encode($key, $value)`

#### Purpose
Encodes a field value into its binary representation.

#### Parameters

| Name    | Type     | Description                                                                                     |
|---------|----------|-------------------------------------------------------------------------------------------------|
| `$key`  | `string` | Field name.                                                                                    |
| `$value`| `mixed`  | Field value to encode.                                                                         |

#### Return Value
- `string`: Binary-encoded value.

#### Inner Mechanisms
- For `byte` fields: Converts the value to a single byte using `chr()`.
- For `string` or `_string` fields: Returns the value as-is.

#### Usage Example
```php
$binary = $resource->field_encode("id", 42); // Returns "\x2A" (ASCII 42)
```

---

### Method: `field_decode($key, $value)`

#### Purpose
Decodes a binary field value into its PHP representation.

#### Parameters

| Name    | Type     | Description                                                                                     |
|---------|----------|-------------------------------------------------------------------------------------------------|
| `$key`  | `string` | Field name.                                                                                    |
| `$value`| `string` | Binary-encoded field value.                                                                    |

#### Return Value
- `string\|int`: Decoded value.

#### Inner Mechanisms
- For `string` or `_string` fields: Trims trailing null bytes using `rtrim()`.
- For `byte` fields: Converts the byte to an integer using `ord()`.

#### Usage Example
```php
$decoded = $resource->field_decode("id", "\x2A"); // Returns 42
```

---

### Method: `lock($exclusive = FALSE)`

#### Purpose
Acquires a shared or exclusive lock on the file.

#### Parameters

| Name         | Type  | Description                                                                                     |
|--------------|-------|-------------------------------------------------------------------------------------------------|
| `$exclusive` | `bool`| If `TRUE`, acquires an exclusive lock (`LOCK_EX`); otherwise, a shared lock (`LOCK_SH`).       |

#### Return Value
None.

#### Inner Mechanisms
- Pushes the lock state onto the `$lock` stack.
- Only acquires a new lock if the previous state is not already locked in the same mode.

#### Usage Example
```php
$resource->lock(); // Shared lock
$resource->lock(TRUE); // Upgrade to exclusive lock
```

---

### Method: `unlock()`

#### Purpose
Releases the most recent lock or downgrades an exclusive lock to a shared lock.

#### Parameters
None.

#### Return Value
None.

#### Inner Mechanisms
- Pops the current lock state from the `$lock` stack.
- If the previous state was shared, releases the lock (`LOCK_UN`).
- If the previous state was exclusive and the current state was shared, downgrades to a shared lock.

#### Usage Example
```php
$resource->unlock(); // Release or downgrade lock
```

---

### Method: `close()`

#### Purpose
Closes the file handle.

#### Parameters
None.

#### Return Value
None.

#### Inner Mechanisms
- Closes the file handle if it is a valid resource.

#### Usage Example
```php
$resource->close();
```


<!-- HASH:b6633ca7a228ae21f041af2c96c601b9 -->
