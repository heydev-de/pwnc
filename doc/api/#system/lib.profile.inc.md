# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.profile.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.profile.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Profile Data Management (`lib.profile.inc`)

This file provides the core profile management functionality for the PWNC Web Platform, including:

1. **`profile_data` class** – A data container for user profile information with automatic normalization and validation.
2. **`profile` class** – A database-backed service for creating, reading, updating, and deleting (CRUD) user profiles, including permission checks and custom field support.

---

## Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_PROFILE_PERMISSION_OPERATOR` | `"operator"` | Permission identifier required to modify profiles. |
| `CMS_DB_PROFILE` | `CMS_DB_PREFIX . "profile"` | Main profile table name. |
| `CMS_DB_PROFILE_INDEX` | `"id"` | Primary key field. |
| `CMS_DB_PROFILE_TIME_CREATED` | `"time_created"` | Timestamp of profile creation. |
| `CMS_DB_PROFILE_TIME_UPDATED` | `"time_updated"` | Timestamp of last update. |
| `CMS_DB_PROFILE_CODE` | `"code"` | Unique internal identifier. |
| `CMS_DB_PROFILE_USER` | `"user"` | Login username (unique). |
| `CMS_DB_PROFILE_PASSWORD` | `"password"` | Hashed password. |
| `CMS_DB_PROFILE_SUPERUSER` | `"superuser"` | Superuser identifier. |
| `CMS_DB_PROFILE_ENABLED` | `"enabled"` | Boolean flag (`'1'` or `'0'`). |
| `CMS_DB_PROFILE_COMPANY` … `CMS_DB_PROFILE_COMMENT` | Various | Standard profile fields. |
| `CMS_DB_PROFILE_CUSTOM` | `CMS_DB_PREFIX . "profile_custom"` | Custom fields table. |
| `CMS_DB_PROFILE_CUSTOM_INDEX` | `"id"` | Foreign key to `CMS_DB_PROFILE_INDEX`. |
| `CMS_DB_PROFILE_CUSTOM_FIELD` | `"field"` | Prefix for custom fields (`field1` … `field20`). |

---

## `profile_data` Class

### Overview
A data transfer object (DTO) that holds all profile fields. The constructor automatically normalizes, validates, and formats the data for storage or display.

### Properties

| Name | Default | Description |
|------|---------|-------------|
| `$id` | `NULL` | Numeric profile ID. |
| `$time_created`, `$time_updated` | `NULL` | Unix timestamps. |
| `$code` | `NULL` | Unique internal code (auto-generated if empty). |
| `$user` | `NULL` | Login username (defaults to `$email` or `$code`). |
| `$password` | `NULL` | Plain-text password (hashed before storage). |
| `$superuser` | `NULL` | Superuser identifier. |
| `$enabled` | `NULL` | Boolean flag. |
| `$company`, `$prename`, `$surname`, … | `NULL` | Personal, contact, bank, and credit card data. |
| `$field1` … `$field20` | `NULL` | Custom fields. |
| `$name` | `NULL` | Full name (`prename surname` or `surname, prename`). |
| `$address` | `NULL` | Formatted multi-line address. |

### `__construct()`

#### Purpose
Normalizes and validates all properties, ensuring consistent formatting and auto-generating missing values.

#### Parameters
None.

#### Return Value
None (constructor).

#### Inner Mechanisms
- **Index**: Casts `$id` to integer.
- **Code**: Strips whitespace; generates a unique ID if empty.
- **User**: Defaults to `$email` or `$code` if empty.
- **Names & Address**: Strips whitespace; builds `$name` and `$address` from components.
- **Credit Card Validity**: Parses `MM/YYYY` format; pads month/year and validates ranges; defaults to current month/year if invalid.

#### Usage Context
Called automatically when a `profile_data` object is instantiated. Also invoked manually before database operations to ensure data integrity.

#### Example
```php
$data = new profile_data();
$data->prename = "John";
$data->surname = "Doe";
$data->street = "123 Main St";
$data->zipcode = "10001";
$data->city = "New York";
$data->email = "john.doe@example.com";
$data->__construct(); // Normalizes data
echo $data->name;     // "John Doe"
echo $data->address;  // "123 Main St\n10001 New York"
```

---

## `profile` Class

### Overview
A service class that manages profile persistence, permission checks, and custom field handling. Requires the `operator` permission unless explicitly overridden.

### Properties

| Name | Default | Description |
|------|---------|-------------|
| `$operator` | `FALSE` | Whether the current user has `operator` permission. |
| `$enabled` | `FALSE` | Whether the database tables exist and are accessible. |

### `__construct($override_permission = FALSE)`

#### Purpose
Initializes the service, verifies database tables, and checks permissions.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$override_permission` | `bool` | If `TRUE`, bypasses permission check. |

#### Return Value
None (constructor).

#### Inner Mechanisms
- Creates a `mysql` instance.
- Verifies the existence and schema of `CMS_DB_PROFILE` and `CMS_DB_PROFILE_CUSTOM`.
- Sets `$operator` based on `cms_permission(CMS_PROFILE_PERMISSION_OPERATOR)` unless overridden.
- Sets `$enabled` to `TRUE` only if both tables are verified.

#### Usage Context
Instantiate the class to interact with profiles. Use `$override_permission` only in trusted administrative contexts.

#### Example
```php
$profileService = new profile(); // Checks operator permission
$adminService = new profile(TRUE); // Bypasses permission check
```

---

### `add(&$profile_data)`

#### Purpose
Inserts a new profile into the database.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$profile_data` | `profile_data` | Profile data object (passed by reference). |

#### Return Value
`bool` – `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
- Validates `$enabled` and `$operator`.
- Calls `$profile_data->__construct()` to normalize data.
- Inserts into `CMS_DB_PROFILE` and `CMS_DB_PROFILE_CUSTOM` in a transaction-like manner (no explicit transaction, but both queries must succeed).
- Sets `$profile_data->id` to the auto-incremented ID on success.

#### Usage Context
Use when creating new user profiles. The caller retains the updated `$profile_data` object with the assigned ID.

#### Example
```php
$data = new profile_data();
$data->prename = "Jane";
$data->surname = "Smith";
$data->email = "jane.smith@example.com";
$data->password = "secure123";
if ($profileService->add($data)) {
    echo "Profile created with ID: " . $data->id;
}
```

---

### `set(&$profile_data)`

#### Purpose
Updates an existing profile.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$profile_data` | `profile_data` | Profile data object (passed by reference). Must have `$id` set. |

#### Return Value
`bool` – `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
- Validates `$enabled` and `$operator`.
- Calls `$profile_data->__construct()` to normalize data.
- Hashes the password only if it is not empty.
- Updates `CMS_DB_PROFILE` and `CMS_DB_PROFILE_CUSTOM` using `INSERT … ON DUPLICATE KEY UPDATE` for custom fields.
- If the password was updated, re-hashes it with the global salt and updates the session cookie.

#### Usage Context
Use when modifying existing profiles. The password is only updated if explicitly provided.

#### Example
```php
$data = $profileService->get(42);
$data->prename = "Janet";
$data->password = "newsecure123";
if ($profileService->set($data)) {
    echo "Profile updated.";
}
```

---

### `get($index, $index_field = CMS_DB_PROFILE_INDEX)`

#### Purpose
Retrieves a profile by its ID or another unique field (e.g., `user` or `code`).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `mixed` | Value of the index field. |
| `$index_field` | `string` | Field to search by (default: `CMS_DB_PROFILE_INDEX`). |

#### Return Value
`profile_data|FALSE` – A populated `profile_data` object on success, `FALSE` on failure.

#### Inner Mechanisms
- Joins `CMS_DB_PROFILE` and `CMS_DB_PROFILE_CUSTOM` to fetch all fields in a single query.
- Maps database columns to `profile_data` properties.
- Calls `$profile_data->__construct()` to normalize the retrieved data.

#### Usage Context
Use to load a profile for display or editing. Supports lookup by any unique field.

#### Example
```php
// By ID
$user = $profileService->get(42);
if ($user) {
    echo "Hello, " . $user->name;
}

// By username
$user = $profileService->get("jane.smith", CMS_DB_PROFILE_USER);
```

---

### `del($index)`

#### Purpose
Deletes a profile by its ID.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Profile ID. |

#### Return Value
`bool` – `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
- Validates `$enabled` and `$operator`.
- Deletes from both `CMS_DB_PROFILE` and `CMS_DB_PROFILE_CUSTOM`.

#### Usage Context
Use to permanently remove a profile. Ensure proper authorization and backups.

#### Example
```php
if ($profileService->del(42)) {
    echo "Profile deleted.";
}
```

---

### `get_permission($user)`

#### Purpose
Retrieves permission-related data for a given username.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$user` | `string` | Username. |

#### Return Value
`array|FALSE` – Associative array with keys `index`, `password`, `superuser`, and `name` on success; `FALSE` if the user does not exist or is disabled.

#### Inner Mechanisms
- Uses a static buffer to cache results and avoid repeated database queries.
- Only returns data for enabled users (`CMS_DB_PROFILE_ENABLED = '1'`).

#### Usage Context
Used internally for authentication and permission checks.

#### Example
```php
$perm = $profileService->get_permission("jane.smith");
if ($perm) {
    echo "User ID: " . $perm["index"];
    echo "Is superuser: " . ($perm["superuser"] ? "Yes" : "No");
}
```


<!-- HASH:7d93694b7c2c3adbfe362d363a7709fd -->
