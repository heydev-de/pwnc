# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.data.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.data.inc)

- **Version:** `26.6.9.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Data Management System (`sys.data.inc`)

The `sys.data.inc` file provides the `data` class and related utility functions for hierarchical data storage and manipulation in the PWNC Web Platform. It enables structured data handling with support for containers, encryption, prefix-based addressing, and hierarchical sorting.

---

## Utility Functions

### `data_sort(&$data, $property, $key = NULL)`

Sorts hierarchical data structures in-place based on a specified property.

| Parameter  | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$data`    | `data`   | Reference to a `data` object containing the hierarchical dataset to sort.   |
| `$property`| `string` | Property name used as the sorting key (e.g., `"#name"`).                   |
| `$key`     | `string` | Optional root key to start sorting from (defaults to the dataset root).    |

**Return Value:**
- `void`: Modifies the `$data` object in-place.

**Inner Mechanisms:**
1. **Tree Construction:** Builds a hierarchical array representation of the data using a stack-based approach.
2. **Sorting:** Recursively sorts containers using `uasort` and a custom comparator (`_data_sort`).
3. **Reconstruction:** Rebuilds the dataset from the sorted tree, preserving container relationships.

**Usage Context:**
- Sorting nested data (e.g., menus, taxonomies) by a property like `title` or `order`.
- Used internally by modules requiring ordered hierarchical output.

**Example:**
```php
$data = new \cms\data("menu");
data_sort($data, "#title"); // Sorts all menu items by title
```

---

### `_data_sort($value1, $value2)`

Custom comparator for hierarchical sorting using `utf8_strnatcasecmp`.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$value1` | `array`| First item in the comparison.   |
| `$value2` | `array`| Second item in the comparison.  |

**Return Value:**
- `int`: Comparison result (`-1`, `0`, `1`).

**Inner Mechanisms:**
- Compares the `"#!"` property of two array items using natural case-insensitive sorting.

---

## `data` Class

Manages hierarchical datasets stored in `.dat` files with support for encryption, prefixes, and container-based nesting.

### Properties

| Name              | Default | Description                                                                 |
|-------------------|---------|-----------------------------------------------------------------------------|
| `$file`           | `NULL`  | Path to the `.dat` file.                                                    |
| `$data`           | `NULL`  | In-memory dataset (associative array).                                      |
| `$buffer`         | `NULL`  | Temporary buffer for cut/copy/paste operations.                             |
| `$password`       | `NULL`  | Encryption key (hashed).                                                    |
| `$prefix`         | `NULL`  | URI-style prefix (e.g., `image://`) for keys.                               |
| `$prefix_length`  | `0`     | Length of the prefix (for removal).                                         |

---

### Constructor: `__construct($name = NULL, $password = NULL, $prefix = NULL)`

Initializes a `data` object and optionally opens a dataset.

| Parameter   | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$name`     | `string` | Dataset name (e.g., `"menu"`). Appends `.dat` and prepends `CMS_DATA_PATH`.|
| `$password` | `string` | Encryption key (hashed internally).                                         |
| `$prefix`   | `string` | URI-style prefix (e.g., `image://`).                                        |

**Usage Example:**
```php
$menu = new \cms\data("menu", NULL, "menu://");
```

---

### `open($name, $password = NULL, $prefix = NULL)`

Loads a dataset from a `.dat` file or cache.

| Parameter   | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$name`     | `string` | Dataset name or full file path.                                             |
| `$password` | `string` | Encryption key (overrides constructor).                                     |
| `$prefix`   | `string` | Prefix (overrides constructor).                                             |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. **Caching:** Checks temporary cache (`cms_cache`) before file I/O.
2. **File Parsing:** Reads line-by-line, decodes keys/values, and decrypts if a password is set.
3. **Expiration:** Skips expired entries (based on `#expire` property).

**Usage Example:**
```php
$images = new \cms\data();
$images->open("image", "secret_key", "image://");
```

---

### `save($name = NULL)`

Persists the dataset to a `.dat` file.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$name`   | `string` | Optional new dataset name or path.                                          |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. **Path Creation:** Uses `mkpath` to ensure the directory exists.
2. **Encryption:** Encrypts values if a password is set.
3. **Caching:** Updates the temporary cache.

**Usage Example:**
```php
$menu = new \cms\data("menu");
$menu->set("Home", "home", "#title");
$menu->save(); // Writes to CMS_DATA_PATH/menu.dat
```

---

### `set($value = NULL, $key = NULL, $property = NULL)`

Sets a value in the dataset (public wrapper for `_set`).

| Parameter  | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$value`   | `mixed`  | Value to set (array for bulk operations).                                   |
| `$key`     | `string` | Key (prefix is automatically removed).                                      |
| `$property`| `string` | Property name (e.g., `"#title"`).                                           |

**Return Value:**
- `void`

**Usage Example:**
```php
$menu->set("Home", "home", "#title");
$menu->set(["#title" => "About", "#url" => "/about"], "about");
```

---

### `_set($value = NULL, $key = NULL, $property = NULL)`

Internal method for setting values (handles prefix removal and type coercion).

| Parameter  | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$value`   | `mixed`  | Value to set.                                                               |
| `$key`     | `string` | Key (no prefix).                                                            |
| `$property`| `string` | Property name.                                                              |

**Inner Mechanisms:**
- Deletes the property if `$value` is empty.
- Coerces `$value` to an array for bulk operations.

---

### `get($key = NULL, $property = NULL)`

Retrieves a value from the dataset (public wrapper for `_get`).

| Parameter  | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$key`     | `string` | Key (prefix is automatically removed).                                      |
| `$property`| `string` | Property name.                                                              |

**Return Value:**
- `mixed`: The requested value or `NULL` if not found.

**Usage Example:**
```php
$title = $menu->get("home", "#title"); // Returns "Home"
```

---

### `_get($key = NULL, $property = NULL)`

Internal method for retrieving values.

| Parameter  | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$key`     | `string` | Key (no prefix).                                                            |
| `$property`| `string` | Property name.                                                              |

**Return Value:**
- `mixed`: The requested value or `NULL` if not found.

---

### `has($key = NULL, $property = NULL)`

Checks if a key/property exists (public wrapper for `_has`).

| Parameter  | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$key`     | `string` | Key (prefix is automatically removed).                                      |
| `$property`| `string` | Property name.                                                              |

**Return Value:**
- `bool`: `TRUE` if the key/property exists, `FALSE` otherwise.

**Usage Example:**
```php
if ($menu->has("home", "#title")) { /* ... */ }
```

---

### `_has($key = NULL, $property = NULL)`

Internal method for checking existence.

| Parameter  | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$key`     | `string` | Key (no prefix).                                                            |
| `$property`| `string` | Property name.                                                              |

**Return Value:**
- `bool`: `TRUE` if the key/property exists, `FALSE` otherwise.

---

### `del($key = NULL, $property = NULL, $recursive = TRUE)`

Deletes a key/property (public wrapper for `_del`).

| Parameter   | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$key`      | `string` | Key (prefix is automatically removed).                                      |
| `$property` | `string` | Property name.                                                              |
| `$recursive`| `bool`   | If `TRUE`, deletes all child containers recursively.                        |

**Return Value:**
- `bool`: `TRUE` on success.

**Usage Example:**
```php
$menu->del("home"); // Deletes the "home" key and its children
```

---

### `_del($key = NULL, $property = NULL, $recursive = TRUE)`

Internal method for deletion.

| Parameter   | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$key`      | `string` | Key (no prefix).                                                            |
| `$property` | `string` | Property name.                                                              |
| `$recursive`| `bool`   | If `TRUE`, deletes all child containers recursively.                        |

**Inner Mechanisms:**
- Uses `_copy` for recursive deletion of containers.

---

### `set_buffer($array)`

Sets the internal buffer for cut/copy/paste operations.

| Parameter | Type    | Description                                                                 |
|-----------|---------|-----------------------------------------------------------------------------|
| `$array`  | `array` | Array of values to buffer.                                                  |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Usage Example:**
```php
$menu->set_buffer([["#title" => "New Item"]]);
```

---

### `cut($key)`

Cuts a key and its children to the buffer (public wrapper for `_cut`).

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key (prefix is automatically removed).                                      |

**Return Value:**
- `bool`: `TRUE` on success.

**Usage Example:**
```php
$menu->cut("about"); // Moves "about" to the buffer
```

---

### `_cut($key)`

Internal method for cutting.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key (no prefix).                                                            |

**Return Value:**
- `bool`: `TRUE` on success.

---

### `insert($key = NULL)`

Inserts buffered items at a specified position (public wrapper for `_insert`).

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key before which to insert (prefix is automatically removed).               |

**Return Value:**
- `string|bool`: The first inserted key or `TRUE` on success.

**Usage Example:**
```php
$menu->insert("home"); // Inserts buffered items before "home"
```

---

### `_insert($key = NULL)`

Internal method for insertion.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key (no prefix).                                                            |

**Return Value:**
- `string|bool`: The first inserted key or `TRUE` on success.

---

### `append($key = NULL)`

Appends buffered items to a container (public wrapper for `_append`).

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Container key (prefix is automatically removed).                            |

**Return Value:**
- `string|bool`: The first inserted key or `TRUE` on success.

**Usage Example:**
```php
$menu->append("main"); // Appends buffered items to the "main" container
```

---

### `_append($key = NULL)`

Internal method for appending.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Container key (no prefix).                                                  |

**Return Value:**
- `string|bool`: The first inserted key or `TRUE` on success.

---

### `copy($key, $action = NULL)`

Copies a key and its children to the buffer (public wrapper for `_copy`).

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key (prefix is automatically removed).                                      |
| `$action` | `string` | Action (`"append"`, `"del"`, or `NULL` for copy).                           |

**Return Value:**
- `string|bool`: The first copied key or `TRUE` on success.

**Usage Example:**
```php
$menu->copy("about"); // Copies "about" to the buffer
```

---

### `_copy($key, $action = NULL)`

Internal method for copying.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key (no prefix).                                                            |
| `$action` | `string` | Action (`"append"`, `"del"`, or `NULL` for copy).                           |

**Return Value:**
- `string|bool`: The first copied key or `TRUE` on success.

**Inner Mechanisms:**
- Handles recursive copying of containers.
- Supports `"append"` (insert after `$key`) and `"del"` (delete after copy).

---

### `seek($condition)`

Searches for a key matching all conditions (public wrapper for `_seek`).

| Parameter   | Type    | Description                                                                 |
|-------------|---------|-----------------------------------------------------------------------------|
| `$condition`| `array` | Associative array of `property => value` pairs.                             |

**Return Value:**
- `string|bool`: The matching key (with prefix) or `FALSE`.

**Usage Example:**
```php
$key = $menu->seek(["#title" => "Home"]); // Returns "home" if found
```

---

### `_seek($condition)`

Internal method for searching.

| Parameter   | Type    | Description                                                                 |
|-------------|---------|-----------------------------------------------------------------------------|
| `$condition`| `array` | Associative array of `property => value` pairs (no prefixes).               |

**Return Value:**
- `string|bool`: The matching key or `FALSE`.

---

### `move($target = "current", $key = NULL)`

Moves the internal pointer to a specified position (public wrapper for `_move`).

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$target` | `string` | Target position (`"current"`, `"first"`, `"last"`, `"prev"`, `"next"`, `"to"`, `"parent"`). |
| `$key`    | `string` | Key for `"to"` or `"parent"` targets (prefix is automatically removed).     |

**Return Value:**
- `string|NULL`: The current key (with prefix) or `NULL`.

**Usage Example:**
```php
$menu->move("to", "about"); // Moves pointer to "about"
```

---

### `_move($target = "current", $key = NULL)`

Internal method for pointer movement.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$target` | `string` | Target position.                                                            |
| `$key`    | `string` | Key for `"to"` or `"parent"` targets (no prefix).                           |

**Return Value:**
- `string|NULL`: The current key or `NULL`.

**Inner Mechanisms:**
- Uses `reset`, `end`, `prev`, `next`, and `key` for pointer manipulation.
- For `"parent"`, traverses the dataset to find the parent container.

---

### `is_container($key)`

Checks if a key is a container (public wrapper for `_is_container`).

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key (prefix is automatically removed).                                      |

**Return Value:**
- `bool`: `TRUE` if the key is a container, `FALSE` otherwise.

**Usage Example:**
```php
if ($menu->is_container("main")) { /* ... */ }
```

---

### `_is_container($key)`

Internal method for container checks.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key (no prefix).                                                            |

**Return Value:**
- `bool`: `TRUE` if the key is a container, `FALSE` otherwise.

---

### `is_child($key, $parent)`

Checks if a key is a child of a container (public wrapper for `_is_child`).

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key (prefix is automatically removed).                                      |
| `$parent` | `string` | Parent container key (prefix is automatically removed).                     |

**Return Value:**
- `bool`: `TRUE` if `$key` is a child of `$parent`, `FALSE` otherwise.

**Usage Example:**
```php
if ($menu->is_child("about", "main")) { /* ... */ }
```

---

### `_is_child($key, $parent)`

Internal method for child checks.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key (no prefix).                                                            |
| `$parent` | `string` | Parent container key (no prefix).                                           |

**Return Value:**
- `bool`: `TRUE` if `$key` is a child of `$parent`, `FALSE` otherwise.

**Inner Mechanisms:**
- Traverses the dataset to verify the parent-child relationship.

---

### `has_children($key)`

Checks if a container has children (public wrapper for `_has_children`).

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Container key (prefix is automatically removed).                            |

**Return Value:**
- `bool`: `TRUE` if the container has children, `FALSE` otherwise.

**Usage Example:**
```php
if ($menu->has_children("main")) { /* ... */ }
```

---

### `_has_children($key)`

Internal method for checking children.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Container key (no prefix).                                                  |

**Return Value:**
- `bool`: `TRUE` if the container has children, `FALSE` otherwise.

---

### `set_password($value = NULL)`

Sets the encryption password.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$value`  | `string` | Password (hashed internally).                                               |

**Usage Example:**
```php
$menu->set_password("secret_key");
```

---

### `set_prefix($value = NULL)`

Sets the URI-style prefix for keys.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$value`  | `string` | Prefix (e.g., `image://`). If `NULL`, uses a default based on `$file`.      |

**Inner Mechanisms:**
- Defaults to predefined prefixes for system datasets (e.g., `image://` for `image.dat`).

**Usage Example:**
```php
$menu->set_prefix("menu://");
```

---

### `apply_prefix($key)`

Applies the prefix to a key.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key to prefix.                                                              |

**Return Value:**
- `string`: The prefixed key.

**Usage Example:**
```php
$prefixed = $menu->apply_prefix("home"); // Returns "menu://home"
```

---

### `remove_prefix($key)`

Removes the prefix from a key.

| Parameter | Type     | Description                                                                 |
|-----------|----------|-----------------------------------------------------------------------------|
| `$key`    | `string` | Key to unprefix.                                                            |

**Return Value:**
- `string`: The unprefixed key.

**Usage Example:**
```php
$unprefixed = $menu->remove_prefix("menu://home"); // Returns "home"
```


<!-- HASH:a12959fab7118b9616f8db912e0e6759 -->
