# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.permission.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.permission.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Permission Management System

This file provides the core permission management system for the PWNC Web Platform. It includes functions and a class to handle user/group permissions, access control, and authentication.

---

## Functions

### `permission_delete($user)`

Removes all user-related data from the system, including IMS (Instant Messaging System) data, desktop data, and reassigns content ownership.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$user`   | string | Username to be deleted          |

**Inner Mechanisms:**
1. Removes IMS data associated with the user
2. Deletes desktop data if the desktop module is available
3. Reassigns content ownership to the administrator

**Usage Example:**
```php
permission_delete("john_doe");
// Removes all traces of user "john_doe" from the system
```

---

### `permission_match($access, $permission, $exclusion)`

Checks if a given access string matches the permission set while respecting exclusions.

| Parameter    | Type   | Description                     |
|--------------|--------|---------------------------------|
| `$access`    | string | Access string to check          |
| `$permission`| string | Permission set                  |
| `$exclusion` | string | Exclusion set                   |

**Return Values:**
- `TRUE` if access is permitted
- `FALSE` if access is denied

**Inner Mechanisms:**
1. Checks for always-allowed permissions
2. Verifies specific exclusions
3. Checks global and specific permissions
4. Evaluates hierarchical access patterns

**Usage Example:**
```php
$allowed = permission_match(
    "content.edit",
    "*\ncontent.*\ncontent.edit",
    "content.delete"
);
// Returns TRUE as "content.edit" is permitted
```

---

### `permission_merge($value1, $value2)`

Merges two permission/exclusion strings while removing duplicates.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$value1` | string | First permission/exclusion set  |
| `$value2` | string | Second permission/exclusion set |

**Return Values:**
- Merged string without duplicates

**Usage Example:**
```php
$merged = permission_merge(
    "content.view\ncontent.edit",
    "content.edit\ncontent.delete"
);
// Returns "content.view\ncontent.edit\ncontent.delete"
```

---

### `permission_is_user($key)`

Checks if a permission key represents a user.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$key`    | string | Permission key to check         |

**Return Values:**
- `TRUE` if key represents a user
- `FALSE` otherwise

**Usage Example:**
```php
$isUser = permission_is_user("user.john_doe");
// Returns TRUE
```

---

### `permission_is_group($key)`

Checks if a permission key represents a group.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$key`    | string | Permission key to check         |

**Return Values:**
- `TRUE` if key represents a group
- `FALSE` otherwise

**Usage Example:**
```php
$isGroup = permission_is_group("group.administrators");
// Returns TRUE
```

---

### `permission_get_name($user, $email = FALSE)`

Retrieves the name or email of a user.

| Parameter | Type    | Description                     |
|-----------|---------|---------------------------------|
| `$user`   | string  | Username                        |
| `$email`  | boolean | If TRUE, returns email instead  |

**Return Values:**
- User's name or email
- `NULL` if not found

**Usage Example:**
```php
$name = permission_get_name("john_doe");
// Returns "John Doe"

$email = permission_get_name("john_doe", TRUE);
// Returns "john@example.com"
```

---

### `permission_get_email($user)`

Convenience function to get a user's email.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$user`   | string | Username                        |

**Return Values:**
- User's email
- `NULL` if not found

**Usage Example:**
```php
$email = permission_get_email("john_doe");
// Returns "john@example.com"
```

---

## `permission` Class

Core class for managing users, groups, and permissions.

### Properties

| Name     | Type  | Description                     |
|----------|-------|---------------------------------|
| `data`   | object| Data storage handler            |
| `buffer` | mixed | Temporary storage buffer        |

---

### `__construct()`

Initializes the permission system and sets up default users.

**Inner Mechanisms:**
1. Creates data storage handler
2. Configures administrator account
3. Sets up anonymous user
4. Configures profile and daemon users

**Usage Example:**
```php
$permission = new permission();
// Initializes the permission system
```

---

### `user($user, $disabled = NULL, $name = NULL, $password = NULL, $group = NULL, $permission = NULL, $exclusion = NULL, $email = NULL, $timezone = NULL, $comment = NULL, $expire = NULL)`

Creates or updates a user account.

| Parameter    | Type    | Description                     |
|--------------|---------|---------------------------------|
| `$user`      | string  | Username                        |
| `$disabled`  | boolean | Whether user is disabled        |
| `$name`      | string  | Display name                    |
| `$password`  | string  | Password (plaintext)            |
| `$group`     | string  | Group memberships               |
| `$permission`| string  | Permission set                  |
| `$exclusion` | string  | Exclusion set                   |
| `$email`     | string  | Email address                   |
| `$timezone`  | string  | Timezone identifier             |
| `$comment`   | string  | Comment                         |
| `$expire`    | integer | Days until expiration           |

**Return Values:**
- User key on success
- `FALSE` on failure

**Inner Mechanisms:**
1. Validates and refines input data
2. Handles password hashing
3. Manages special users (admin, anonymous)
4. Validates timezone
5. Stores user data

**Usage Example:**
```php
$permission->user(
    "john_doe",
    FALSE,
    "John Doe",
    "secure123",
    "editors\nreviewers",
    "content.view\ncontent.edit",
    "",
    "john@example.com",
    "America/New_York",
    "Content editor",
    30
);
// Creates a new user with 30-day expiration
```

---

### `group($group, $disabled = NULL, $name = NULL, $permission = NULL, $exclusion = NULL, $comment = NULL)`

Creates or updates a group.

| Parameter    | Type    | Description                     |
|--------------|---------|---------------------------------|
| `$group`     | string  | Group name                      |
| `$disabled`  | boolean | Whether group is disabled       |
| `$name`      | string  | Display name                    |
| `$permission`| string  | Permission set                  |
| `$exclusion` | string  | Exclusion set                   |
| `$comment`   | string  | Comment                         |

**Return Values:**
- Group key on success
- `FALSE` on failure

**Usage Example:**
```php
$permission->group(
    "editors",
    FALSE,
    "Content Editors",
    "content.view\ncontent.edit",
    "",
    "Can edit content"
);
// Creates a new editors group
```

---

### `permit($key, $access, $explicit = TRUE)`

Grants a permission to a user or group.

| Parameter  | Type    | Description                     |
|------------|---------|---------------------------------|
| `$key`     | string  | User/group key                  |
| `$access`  | string  | Access string to grant          |
| `$explicit`| boolean | Whether to explicitly add       |

**Return Values:**
- `TRUE` on success

**Usage Example:**
```php
$permission->permit("user.john_doe", "content.edit");
// Grants content editing permission to John Doe
```

---

### `exclude($key, $access, $explicit = TRUE)`

Adds an exclusion to a user or group.

| Parameter  | Type    | Description                     |
|------------|---------|---------------------------------|
| `$key`     | string  | User/group key                  |
| `$access`  | string  | Access string to exclude        |
| `$explicit`| boolean | Whether to explicitly add       |

**Return Values:**
- `TRUE` on success

**Usage Example:**
```php
$permission->exclude("group.editors", "content.delete");
// Prevents editors from deleting content
```

---

### `optimize($value)`

Optimizes a permission or exclusion set by removing redundant entries.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$value`  | string | Permission/exclusion set        |

**Return Values:**
- Optimized string

**Usage Example:**
```php
$optimized = $permission->optimize("content.view\ncontent.*\ncontent.edit");
// Returns "content.*" as it covers all others
```

---

### `add_group($user, $group)`

Adds a user to one or more groups.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$user`   | string | Username                        |
| `$group`  | string | Group(s) to add (newline-separated) |

**Return Values:**
- `TRUE` on success

**Usage Example:**
```php
$permission->add_group("john_doe", "editors\nreviewers");
// Adds John Doe to editors and reviewers groups
```

---

### `del_group($user, $group)`

Removes a user from one or more groups.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$user`   | string | Username                        |
| `$group`  | string | Group(s) to remove (newline-separated) |

**Return Values:**
- `TRUE` on success

**Usage Example:**
```php
$permission->del_group("john_doe", "reviewers");
// Removes John Doe from reviewers group
```

---

### `delete($key)`

Deletes a user or group and performs cleanup.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$key`    | string | User/group key to delete        |

**Return Values:**
- `TRUE` on success
- `FALSE` on failure

**Usage Example:**
```php
$permission->delete("user.john_doe");
// Deletes user "john_doe" and cleans up related data
```

---

### `save()`

Optimizes and saves all permission data.

**Return Values:**
- Result of the save operation

**Usage Example:**
```php
$permission->save();
// Saves all permission changes
```

---

### `verify_user($user, $password)`

Verifies user credentials and returns user data.

| Parameter  | Type   | Description                     |
|------------|--------|---------------------------------|
| `$user`    | string | Username                        |
| `$password`| string | Password (plaintext)            |

**Return Values:**
- Array with user data on success
- `FALSE` on failure

**Usage Example:**
```php
$userData = $permission->verify_user("john_doe", "secure123");
/*
Returns:
[
    "superuser" => "john_doe",
    "name" => "John Doe",
    "profile" => NULL
]
*/
```

---

### `get_user_permission($user)`

Retrieves the complete permission set for a user (including group permissions).

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$user`   | string | Username                        |

**Return Values:**
- Permission string
- `NULL` if user is disabled

**Usage Example:**
```php
$permissions = $permission->get_user_permission("john_doe");
// Returns "content.view\ncontent.edit"
```

---

### `get_user_exclusion($user)`

Retrieves the complete exclusion set for a user (including group exclusions).

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$user`   | string | Username                        |

**Return Values:**
- Exclusion string
- `"*"` if user is disabled

**Usage Example:**
```php
$exclusions = $permission->get_user_exclusion("john_doe");
// Returns "content.delete"
```

---

### `given($access = NULL, $user = NULL, $password = NULL, $test = NULL)`

Checks if a user has permission for a specific access.

| Parameter | Type    | Description                     |
|-----------|---------|---------------------------------|
| `$access` | string  | Access string to check          |
| `$user`   | string  | Username                        |
| `$password`| string | Password (plaintext)            |
| `$test`   | boolean | Whether to skip password check  |

**Return Values:**
- `TRUE` if access is permitted
- `FALSE` if access is denied

**Inner Mechanisms:**
1. Handles default access (current application)
2. Checks anonymous permissions first
3. Verifies user credentials
4. Evaluates permissions and exclusions

**Usage Example:**
```php
$allowed = $permission->given("content.edit", "john_doe", "secure123");
// Returns TRUE if John Doe can edit content
```

---

### `test($access, $user)`

Tests if a user has permission without password verification.

| Parameter | Type   | Description                     |
|-----------|--------|---------------------------------|
| `$access` | string | Access string to check          |
| `$user`   | string | Username                        |

**Return Values:**
- `TRUE` if access is permitted
- `FALSE` if access is denied

**Usage Example:**
```php
$allowed = $permission->test("content.edit", "john_doe");
// Returns TRUE if John Doe has edit permission
```


<!-- HASH:19a494214e5c67c0eecd86cd2cf7c67a -->
