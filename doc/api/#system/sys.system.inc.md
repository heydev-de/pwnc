# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.system.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.system.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## System Class

The `system` class serves as a centralized configuration and state management utility for the PWNC Web Platform. It provides a simple interface to read, modify, and persist system-wide settings stored in a structured data file. The class abstracts file location resolution (development vs. production) and delegates data operations to the `data` class.

### Properties

| Name  | Type   | Description                                                                 |
|-------|--------|-----------------------------------------------------------------------------|
| data  | object | Instance of the `data` class responsible for handling the underlying system data file. |

---

### `__construct()`

#### Purpose
Initializes the `system` class by determining the appropriate data file path (development or production) and loading the system configuration into memory via the `data` class.

#### Parameters
None.

#### Return Values
None (constructor).

#### Inner Mechanisms
- Uses a static variable `$file` to cache the resolved file path across multiple instances.
- Checks for the existence of a development data file (`#system/system.dev.dat`). If found, it uses the development file; otherwise, it defaults to the production file (`#system/system`).
- Instantiates a `data` object with the resolved file path and assigns it to the `data` property.

#### Usage Context
- Called automatically when a new `system` object is created.
- Ensures consistent access to system settings regardless of the environment (development or production).

#### Example
```php
$sys = new \cms\system();
```
*Creates a new system object, loading the appropriate configuration file based on the environment.*

---

### `getval()`

#### Purpose
Retrieves a specific value or option from the system configuration.

#### Parameters

| Name      | Type   | Default | Description                                                                 |
|-----------|--------|---------|-----------------------------------------------------------------------------|
| property  | string | -       | The key or path to the desired value in the system configuration.           |
| option    | string | "value" | The specific option to retrieve (e.g., "value", "default", "description").  |

#### Return Values
- **Mixed**: The requested value or option. Type depends on the stored data (e.g., string, integer, array).

#### Inner Mechanisms
- Delegates the retrieval operation to the `get` method of the `data` class.
- Supports nested property access via dot notation (e.g., `"database.host"`).

#### Usage Context
- Used to fetch system settings such as database credentials, feature flags, or paths.
- Ideal for read-only access to configuration values.

#### Example
```php
$dbHost = $sys->getval("database.host");
```
*Retrieves the database host from the system configuration.*

---

### `setval()`

#### Purpose
Updates or sets a value or option in the system configuration.

#### Parameters

| Name      | Type   | Default | Description                                                                 |
|-----------|--------|---------|-----------------------------------------------------------------------------|
| value     | mixed  | NULL    | The new value to set.                                                       |
| property  | string | NULL    | The key or path to the value in the system configuration.                   |
| option    | string | "value" | The specific option to update (e.g., "value", "default", "description").    |

#### Return Values
None.

#### Inner Mechanisms
- Delegates the update operation to the `set` method of the `data` class.
- Supports nested property access via dot notation (e.g., `"database.host"`).

#### Usage Context
- Used to modify system settings dynamically (e.g., enabling/disabling features, updating paths).
- Changes are not persisted to disk until `save()` is called.

#### Example
```php
$sys->setval("127.0.0.1", "database.host");
```
*Updates the database host in the system configuration.*

---

### `save()`

#### Purpose
Persists the current state of the system configuration to the data file.

#### Parameters
None.

#### Return Values
- **Boolean**: `TRUE` if the save operation was successful, `FALSE` otherwise.

#### Inner Mechanisms
- Delegates the save operation to the `save` method of the `data` class.
- Writes the in-memory configuration to the file system.

#### Usage Context
- Must be called explicitly to persist changes made via `setval()`.
- Useful for batch updates to avoid frequent disk I/O.

#### Example
```php
$sys->setval("127.0.0.1", "database.host");
$sys->setval(3306, "database.port");
$sys->save();
```
*Updates the database host and port, then persists the changes to disk.*


<!-- HASH:066a7b00119e905867263a071fdd46cb -->
