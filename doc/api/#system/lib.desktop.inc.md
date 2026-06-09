# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.desktop.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.desktop.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Desktop Class

The `desktop` class manages user-specific desktop objects in the PWNC Web Platform. It provides functionality for creating, modifying, moving, and deleting desktop items such as links, notes, appointments, addresses, containers, and mailboxes. The class ensures that essential default containers (mailbox, appointments, addresses) are always present for each user.

### Constants

| Name                     | Value | Description                                                                 |
|--------------------------|-------|-----------------------------------------------------------------------------|
| `CMS_DESKTOP_TYPE_NONE`  | 0     | No desktop type                                                             |
| `CMS_DESKTOP_TYPE_LINK`  | 1     | Link object type                                                            |
| `CMS_DESKTOP_TYPE_NOTE`  | 2     | Note object type                                                            |
| `CMS_DESKTOP_TYPE_APPOINTMENT` | 4 | Appointment object type                                                     |
| `CMS_DESKTOP_TYPE_ADDRESS`     | 8     | Address object type                                                         |
| `CMS_DESKTOP_TYPE_CONTAINER`   | 16    | Container object type (can hold other objects)                              |
| `CMS_DESKTOP_TYPE_MAILBOX`     | 32    | Mailbox object type                                                         |
| `CMS_DESKTOP_TYPE_ALL`   | 255   | All desktop object types combined                                           |

### Properties

| Name   | Value/Default | Description                                                                 |
|--------|---------------|-----------------------------------------------------------------------------|
| `user` | `NULL`        | User identifier for whom the desktop is managed. Defaults to `CMS_SUPERUSER`. |
| `data` | `NULL`        | Instance of the `data` class handling storage and retrieval of desktop objects. |

---

### `__construct($user = NULL)`

**Purpose:**
Initializes a desktop instance for a specified user. Ensures default containers (mailbox, appointments, addresses) exist.

**Parameters:**

| Name  | Type     | Description                                                                 |
|-------|----------|-----------------------------------------------------------------------------|
| `$user` | `string` | User identifier. If `NULL`, defaults to `CMS_SUPERUSER`.                    |

**Return Values:**
- None (constructor).

**Inner Mechanisms:**
1. Sets the user property.
2. Initializes a `data` instance for the user's desktop storage path.
3. Checks for existing default containers (mailbox, appointments, addresses).
4. Creates missing default containers with localized names (`CMS_L_DESKTOP_001`, `CMS_L_DESKTOP_002`, `CMS_L_DESKTOP_003`).
5. Saves changes to the data store if any defaults were created.

**Usage Context:**
- Called when initializing a user session or accessing the desktop module.
- Ensures a consistent starting state for all users.

**Example:**
```php
// Initialize desktop for the current user
$desktop = new \cms\desktop();
```

---

### `object_get($index, $property)`

**Purpose:**
Retrieves a property value of a desktop object by its index.

**Parameters:**

| Name       | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$index`   | `mixed`  | Index/key of the desktop object.                                            |
| `$property`| `string` | Property name to retrieve (e.g., `name`, `color`, `#type`).                 |

**Return Values:**
- `mixed`: The property value if found; `NULL` otherwise.

**Inner Mechanisms:**
- Delegates to the `data->get()` method of the underlying data store.

**Usage Context:**
- Used to fetch object properties for display or processing.

**Example:**
```php
// Get the name of the first desktop object
$name = $desktop->object_get("first", "name");
```

---

### `object_set($index, $property, $value = NULL)`

**Purpose:**
Sets a property value of a desktop object by its index.

**Parameters:**

| Name       | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$index`   | `mixed`  | Index/key of the desktop object.                                            |
| `$property`| `string` | Property name to set.                                                       |
| `$value`   | `mixed`  | Value to assign to the property.                                            |

**Return Values:**
- `bool`: `TRUE` if the property was set successfully; `FALSE` if the object does not exist.

**Inner Mechanisms:**
- Checks if the object exists using `data->get($index)`.
- Delegates to `data->set()` if the object exists.

**Usage Context:**
- Used to update object properties (e.g., renaming, recoloring).

**Example:**
```php
// Rename the first desktop object
$success = $desktop->object_set("first", "name", "New Name");
```

---

### `save()`

**Purpose:**
Persists all changes made to the desktop objects to the data store.

**Parameters:**
- None.

**Return Values:**
- `bool`: `TRUE` if the save operation succeeded; `FALSE` otherwise.

**Inner Mechanisms:**
- Delegates to the `data->save()` method.

**Usage Context:**
- Called after making multiple changes to ensure persistence.

**Example:**
```php
// Save all changes to the desktop
$desktop->save();
```

---

### `create_object($index, $type, $name)`

**Purpose:**
Creates a new desktop object of a specified type at a given position.

**Parameters:**

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$index`| `mixed`  | Position/index where the object should be inserted.                         |
| `$type` | `string` | Type of the object (`link`, `note`, `appointment`, `address`, `container`).  |
| `$name` | `string` | Name/label of the object.                                                   |

**Return Values:**
- `mixed`: The index/key of the newly created object if successful; `FALSE` otherwise.

**Inner Mechanisms:**
1. Generates a color based on the object name using `strtocolor()`.
2. Prepares a buffer for the new object:
   - For containers: Includes opening and closing tags.
   - For other types: Single object definition.
3. Inserts the object at the specified index using `data->insert()`.
4. Saves changes if insertion succeeds.

**Usage Context:**
- Used to add new objects to the desktop (e.g., via user action or automated process).

**Example:**
```php
// Create a new note object
$noteIndex = $desktop->create_object("first", "note", "Meeting Notes");
```

---

### `move_object($source, $target)`

**Purpose:**
Moves a desktop object from one position to another.

**Parameters:**

| Name     | Type    | Description                                                                 |
|----------|---------|-----------------------------------------------------------------------------|
| `$source`| `mixed` | Index/key of the object to move.                                            |
| `$target`| `mixed` | Target position/index where the object should be moved.                     |

**Return Values:**
- `mixed`: The new index/key of the moved object if successful; `FALSE` otherwise.

**Inner Mechanisms:**
1. Cuts the object from its source position using `data->cut()`.
2. Inserts the object at the target position using `data->insert()`.
3. Saves changes if both operations succeed.

**Usage Context:**
- Used to reorganize the desktop layout (e.g., drag-and-drop functionality).

**Example:**
```php
// Move the first object to the end
$newIndex = $desktop->move_object("first", "last");
```

---

### `delete_object($index)`

**Purpose:**
Deletes a desktop object by its index and performs type-specific cleanup.

**Parameters:**

| Name    | Type    | Description                                                                 |
|---------|---------|-----------------------------------------------------------------------------|
| `$index`| `mixed` | Index/key of the object to delete.                                          |

**Return Values:**
- `bool`: `TRUE` if deletion and cleanup succeeded; `FALSE` otherwise.

**Inner Mechanisms:**
1. Retrieves the object type before deletion.
2. Deletes the object using `data->del()`.
3. Saves changes.
4. Performs cleanup based on object type:
   - For mailboxes: Deletes associated files using `filemanager_delete()` if the `filemanager` module is loaded.

**Usage Context:**
- Used to remove objects from the desktop (e.g., user-initiated deletion).

**Example:**
```php
// Delete the first object
$success = $desktop->delete_object("first");
```

---

### `object_type($index)`

**Purpose:**
Retrieves the type of a desktop object by its index.

**Parameters:**

| Name    | Type    | Description                                                                 |
|---------|---------|-----------------------------------------------------------------------------|
| `$index`| `mixed` | Index/key of the object.                                                    |

**Return Values:**
- `string`: The object type (e.g., `link`, `note`, `container`).

**Inner Mechanisms:**
- Delegates to `data->get($index, "#type")`.

**Usage Context:**
- Used to determine how to handle or display an object.

**Example:**
```php
// Check if the first object is a container
$isContainer = ($desktop->object_type("first") === "container");
```

---

### `get_parent($index)`

**Purpose:**
Retrieves the parent index/key of a desktop object.

**Parameters:**

| Name    | Type    | Description                                                                 |
|---------|---------|-----------------------------------------------------------------------------|
| `$index`| `mixed` | Index/key of the object.                                                    |

**Return Values:**
- `mixed`: The parent index/key if found; `FALSE` otherwise.

**Inner Mechanisms:**
- Delegates to `data->move("parent", $index)`.

**Usage Context:**
- Used to navigate the desktop hierarchy (e.g., for breadcrumbs or tree views).

**Example:**
```php
// Get the parent of the first object
$parentIndex = $desktop->get_parent("first");
```


<!-- HASH:510feb227f858fd793abee9982f29de9 -->
