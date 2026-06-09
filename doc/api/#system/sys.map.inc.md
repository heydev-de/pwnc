# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.map.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.map.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Map Class

The `map` class provides a high-performance key-value storage system optimized for fast lookups using CRC32 hashing. It stores data in a structured binary format on disk while maintaining in-memory indexes for quick access. This class is ideal for scenarios requiring persistent, bidirectional mappings (key-to-value and value-to-key) with minimal overhead.

### Constants

| Name                | Value | Description                          |
|---------------------|-------|--------------------------------------|
| `CMS_MAP_INDEX_KEY` | 0     | Index for key-based lookup array     |
| `CMS_MAP_INDEX_VALUE` | 1   | Index for value-based lookup array   |
| `CMS_MAP_DATA_KEY`  | 2     | Index for key storage array          |
| `CMS_MAP_DATA_VALUE` | 3    | Index for value storage array        |

### Properties

| Name  | Default Value | Description                          |
|-------|---------------|--------------------------------------|
| `$file` | `NULL` | Path to the map file on disk         |
| `$data` | Structured array with four sub-arrays | Internal storage containing indexes and data |

---

### `__construct($name = NULL)`

**Purpose:**
Initializes a new map instance. Optionally opens an existing map file if a name is provided.

**Parameters:**

| Name   | Type   | Description                          |
|--------|--------|--------------------------------------|
| `$name` | string | (Optional) Name of the map file to open |

**Return Values:**
- None (constructor)

**Inner Mechanisms:**
- If `$name` is provided, calls `open()` to load the map file.
- Initializes the `$data` property with empty arrays for all four storage components.

**Usage Context:**
- Used when creating a new map or loading an existing one.

**Example:**
```php
// Create a new empty map
$map = new \cms\map();

// Load an existing map
$map = new \cms\map('user_preferences');
```

---

### `open($name)`

**Purpose:**
Loads a map file from disk into memory, populating the internal data structure.

**Parameters:**

| Name   | Type   | Description                          |
|--------|--------|--------------------------------------|
| `$name` | string | Name of the map file to open         |

**Return Values:**
- `TRUE` on success
- `FALSE` on failure (implicit)

**Inner Mechanisms:**
1. Sets the file path using `CMS_DATA_PATH`.
2. Attempts to retrieve cached data using `cms_cache()`.
3. If not cached, reads the file line by line, parsing key-value pairs.
4. Populates the four internal arrays:
   - `CMS_MAP_INDEX_KEY`: Maps CRC32 hashes of keys to their index.
   - `CMS_MAP_INDEX_VALUE`: Maps CRC32 hashes of values to their index.
   - `CMS_MAP_DATA_KEY`: Stores keys by index.
   - `CMS_MAP_DATA_VALUE`: Stores values by index.
5. Caches the loaded data for future use.

**Usage Context:**
- Used to load an existing map file into memory for manipulation.

**Example:**
```php
$map = new \cms\map();
$map->open('product_categories');
// Now the map is ready for read/write operations
```

---

### `save($name = NULL)`

**Purpose:**
Saves the current in-memory map data to disk.

**Parameters:**

| Name   | Type   | Description                          |
|--------|--------|--------------------------------------|
| `$name` | string | (Optional) Name of the map file to save as |

**Return Values:**
- `TRUE` on success
- `FALSE` on failure

**Inner Mechanisms:**
1. If `$name` is provided, updates the `$file` property.
2. Creates the directory path if it doesn't exist using `mkpath()`.
3. Opens the file for writing and locks it exclusively.
4. Writes each key-value pair to the file, separated by newlines.
5. Updates the cache with the current data.

**Usage Context:**
- Used to persist changes made to the map in memory.

**Example:**
```php
$map = new \cms\map('user_preferences');
$map->set('theme', 'dark');
$map->set('language', 'en_US');
$map->save(); // Saves to user_preferences.map
```

---

### `set($key, $value)`

**Purpose:**
Adds or updates a key-value pair in the map.

**Parameters:**

| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$key`  | string | Key to set                           |
| `$value` | string | Value to associate with the key      |

**Return Values:**
- `TRUE` on success

**Inner Mechanisms:**
1. Computes CRC32 hashes for the key and value.
2. If the key already exists, removes the old value from the value index.
3. If the key is new, assigns it the next available index.
4. Updates all four internal arrays with the new key-value pair.

**Usage Context:**
- Used to populate or update the map with new data.

**Example:**
```php
$map = new \cms\map('settings');
$map->set('timezone', 'UTC');
$map->set('notifications', 'enabled');
```

---

### `get_value($key)`

**Purpose:**
Retrieves the value associated with a given key.

**Parameters:**

| Name  | Type   | Description                          |
|-------|--------|--------------------------------------|
| `$key` | string | Key to look up                       |

**Return Values:**
- The associated value as a string
- `NULL` if the key does not exist

**Inner Mechanisms:**
1. Computes the CRC32 hash of the key.
2. Checks the key index for the hash.
3. Returns the corresponding value from the value data array.

**Usage Context:**
- Used to retrieve values by their keys.

**Example:**
```php
$map = new \cms\map('settings');
$map->open('settings');
$timezone = $map->get_value('timezone'); // Returns 'UTC'
```

---

### `get_value_list()`

**Purpose:**
Retrieves all values stored in the map as a flat array.

**Parameters:**
- None

**Return Values:**
- Array of all values

**Inner Mechanisms:**
- Returns the values from `CMS_MAP_DATA_VALUE` as a numerically indexed array.

**Usage Context:**
- Used when all values need to be processed or displayed.

**Example:**
```php
$map = new \cms\map('settings');
$map->open('settings');
$allValues = $map->get_value_list();
// $allValues might contain ['UTC', 'enabled', ...]
```

---

### `get_key($value)`

**Purpose:**
Retrieves the key associated with a given value (reverse lookup).

**Parameters:**

| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$value` | string | Value to look up                     |

**Return Values:**
- The associated key as a string
- `NULL` if the value does not exist

**Inner Mechanisms:**
1. Computes the CRC32 hash of the value.
2. Checks the value index for the hash.
3. Returns the corresponding key from the key data array.

**Usage Context:**
- Used for bidirectional lookups when values are unique.

**Example:**
```php
$map = new \cms\map('settings');
$map->open('settings');
$key = $map->get_key('UTC'); // Returns 'timezone'
```

---

### `del_key($key)`

**Purpose:**
Removes a key-value pair from the map by its key.

**Parameters:**

| Name  | Type   | Description                          |
|-------|--------|--------------------------------------|
| `$key` | string | Key to remove                        |

**Return Values:**
- None

**Inner Mechanisms:**
1. Computes the CRC32 hash of the key.
2. If the key exists, removes all references to it and its associated value from the four internal arrays.

**Usage Context:**
- Used to remove entries from the map.

**Example:**
```php
$map = new \cms\map('settings');
$map->open('settings');
$map->del_key('notifications'); // Removes the 'notifications' entry
```

---

### `del_value($value)`

**Purpose:**
Removes a key-value pair from the map by its value.

**Parameters:**

| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$value` | string | Value to remove                      |

**Return Values:**
- None

**Inner Mechanisms:**
1. Computes the CRC32 hash of the value.
2. If the value exists, removes all references to it and its associated key from the four internal arrays.

**Usage Context:**
- Used for bidirectional removal when values are unique.

**Example:**
```php
$map = new \cms\map('settings');
$map->open('settings');
$map->del_value('UTC'); // Removes the entry with value 'UTC'
```


<!-- HASH:1b99ce285114b8f6cefc7be2b3bc72ce -->
