# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.permission.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.permission.inc)

- **Version:** `26.6.9.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Permission Interface Module (`ifc.permission.inc`)

This file implements the **Permission Management Interface** for the PWNC Web Platform. It provides a user interface for managing users, groups, and access permissions within the system. The interface allows administrators to:

- Create, modify, and delete users and groups
- Assign permissions and exclusions
- Enable/disable accounts
- Manage group memberships
- Set access control rules

The interface works in conjunction with the `permission` class to persist changes to the system's permission data.

---

### Interface Initialization

The interface begins by checking for basic access permissions and initializing default values for the selected object and type.

| Variable | Default Value | Description |
|----------|---------------|-------------|
| `$object` | `NULL` | Currently selected user/group (e.g., `"user.jdoe"` or `"group.admins"`) |
| `$type` | `"user"` | Type of object being managed (`"user"` or `"group"`) |

---

## Message Handling

The interface processes various messages (commands) sent by the frontend, each triggering specific actions.

### `select`

**Purpose:**
Sets the currently selected object based on user interaction.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | The object identifier (e.g., `"user.jdoe"`) |

**Usage Context:**
Triggered when a user clicks on a user or group in the list to view or edit its properties.

---

### `select_user` / `select_group`

**Purpose:**
Switches the interface context to either users or groups, clearing the selected object if the type changes.

**Inner Mechanisms:**
- Checks if the current `$type` differs from the new type.
- If so, resets `$object` to `NULL` to avoid displaying mismatched data.
- Sets `$type` to `"user"` or `"group"`.

**Usage Context:**
Triggered when the user clicks the "Users" or "Groups" button to switch the view.

---

### `add_user`

**Purpose:**
Displays a form for creating a new user.

**Inner Mechanisms:**
- Creates a new `ifc` (interface control) instance with a form for:
  - Name
  - User ID
  - Password (with confirmation)
- The form submits to the `_add_user` message.

**Usage Example:**
```php
// Triggered by clicking "Create User" in the UI
$ifc = new ifc(NULL, $ifc_page, TRUE, ["object" => $object, "type" => $type], "_add_user", CMS_L_IFC_PERMISSION_001);
$ifc->set(CMS_L_NAME, "text 40 40 b");
$ifc->set(CMS_L_IFC_PERMISSION_002, "text 40 40 b"); // User ID
$ifc->set(CMS_L_PASSWORD, "password 40 40 b");
$ifc->set(CMS_L_IFC_PERMISSION_003, "password 40 40"); // Confirm Password
$ifc->close();
```

---

### `_add_user`

**Purpose:**
Processes the form submission from `add_user` to create a new user.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | User's display name |
| `$ifc_param2` | `string` | User ID |
| `$ifc_param3` | `string` | Password |
| `$ifc_param4` | `string` | Password confirmation |

**Return/Response:**
- `CMS_MSG_DONE`: User created successfully.
- `CMS_MSG_ERROR`: User creation failed (e.g., user exists, passwords don't match).

**Inner Mechanisms:**
1. Validates that the password and confirmation match.
2. Checks if the user ID already exists.
3. Creates the user using the `permission` class.
4. Saves changes and updates the selected object.

**Usage Example:**
```php
if (streq($ifc_param3, $ifc_param4)) {
    $permission = new permission();
    if (!$permission->data->get("user.$ifc_param2")) {
        if ($_object = $permission->user($ifc_param2, TRUE, $ifc_param1, $ifc_param3)) {
            if ($permission->save()) {
                $object = $_object;
                $type = "user";
                $ifc_response = CMS_MSG_DONE;
            }
        }
    }
}
```

---

### `add_group` / `_add_group`

**Purpose:**
Displays a form for creating a new group (`add_group`) and processes the submission (`_add_group`).

**Parameters (for `_add_group`):**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | Group name |
| `$ifc_param2` | `string` | Group ID |

**Inner Mechanisms:**
- Similar to `add_user`/`_add_user`, but for groups.
- Validates that the group ID does not already exist.

**Usage Example:**
```php
$permission = new permission();
if (!$permission->data->get("group.$ifc_param2")) {
    if ($_object = $permission->group($ifc_param2, TRUE, $ifc_param1)) {
        if ($permission->save()) {
            $object = $_object;
            $type = "group";
            $ifc_response = CMS_MSG_DONE;
        }
    }
}
```

---

### `save_user`

**Purpose:**
Saves changes to an existing user.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | Name |
| `$ifc_param2` | `bool` | Disabled flag |
| `$ifc_param3` | `string` | Password (optional) |
| `$ifc_param4` | `string` | Password confirmation |
| `$ifc_param5` | `array` | Selected groups (as array) |
| `$ifc_param6` | `string` | Permissions (newline-separated) |
| `$ifc_param7` | `string` | Exclusions (newline-separated) |
| `$ifc_param8` | `string` | Email |
| `$ifc_param9` | `string` | Timezone |
| `$ifc_param10` | `string` | Comment |
| `$ifc_param11` | `int` | Expiration in days (converted to seconds) |

**Inner Mechanisms:**
1. Validates password (if provided) and confirmation.
2. Converts the groups array to a newline-separated string.
3. Applies changes using the `permission->user()` method.
4. Saves changes and returns a success/error message.

**Usage Example:**
```php
$permission = new permission();
if (nstre($ifc_param3) && nstreq($ifc_param3, $ifc_param4)) {
    $ifc_param3 = NULL; // Passwords don't match; ignore
}
$ifc_param5 = blank($ifc_param5) ? NULL : implode("\n", $ifc_param5);
if ($permission->user(substr($object, 5), isset($ifc_param2), $ifc_param1, $ifc_param3, $ifc_param5, ...)) {
    if ($permission->save()) {
        $ifc_response = CMS_MSG_DONE;
    }
}
```

---

### `save_group`

**Purpose:**
Saves changes to an existing group.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | Name |
| `$ifc_param2` | `bool` | Disabled flag |
| `$ifc_param3` | `array` | Users to assign to the group |
| `$ifc_param4` | `string` | Permissions (newline-separated) |
| `$ifc_param5` | `string` | Exclusions (newline-separated) |
| `$ifc_param6` | `string` | Comment |

**Inner Mechanisms:**
1. Applies changes to the group using `permission->group()`.
2. Iterates through all users and updates their group assignments.
3. Saves changes and returns a success/error message.

**Usage Example:**
```php
$permission = new permission();
if ($permission->group(substr($object, 6), isset($ifc_param2), $ifc_param1, $ifc_param4, $ifc_param5, $ifc_param6)) {
    init($ifc_param3, []);
    $permission->data->move("first");
    while ($key = $permission->data->move("next")) {
        if (!permission_is_user($key)) continue;
        $_key = substr($key, 5);
        if (in_array($key, $ifc_param3)) $permission->add_group($_key, $object);
        else $permission->del_group($_key, $object);
    }
    if ($permission->save()) {
        $ifc_response = CMS_MSG_DONE;
    }
}
```

---

### `activate` / `deactivate`

**Purpose:**
Enables or disables selected users/groups.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$list` | `array` | Array of object identifiers (e.g., `["user.jdoe", "group.admins"]`) |

**Inner Mechanisms:**
- For `activate`: Removes the `disabled` flag from selected objects.
- For `deactivate`: Sets the `disabled` flag for selected objects.
- Saves changes and returns a success/error message.

**Usage Example:**
```php
$permission = new permission();
foreach ($list AS $value) {
    $permission->data->del($value, "disabled"); // Activate
    // or
    $permission->data->set(TRUE, $value, "disabled"); // Deactivate
}
$ifc_response = $permission->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
```

---

### `delete`

**Purpose:**
Deletes selected users or groups.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$list` | `array` | Array of object identifiers to delete |

**Inner Mechanisms:**
1. Deletes each object in `$list` using `permission->delete()`.
2. If the currently selected object is deleted, resets `$object` to `NULL`.
3. Saves changes and returns a success/error message.

**Usage Example:**
```php
$permission = new permission();
foreach ($list AS $value) {
    $permission->delete($value);
}
if (in_array($object, $list)) $object = NULL;
$ifc_response = $permission->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
```

---

### `set` / `_set` / `set_add` / `set_add_ex` / `set_del` / `set_del_ex`

**Purpose:**
Manages access permissions for users and groups.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | Access rule (e.g., `"module.content"` or `"page.admin"`) |
| `$ifc_param2` | `array` | Users/groups to exclude (for `set_del`/`set_del_ex`) |
| `$ifc_param3` | `array` | Users/groups to permit (for `set_add`/`set_add_ex`) |

**Inner Mechanisms:**
- **`set_add`/`set_add_ex`**: Grants permission to selected users/groups. `_ex` variants apply explicit permissions.
- **`set_del`/`set_del_ex`**: Revokes permission from selected users/groups. `_ex` variants remove explicit permissions.
- Uses `permission->permit()` and `permission->exclude()` methods.
- Displays a dual-list interface showing permitted and excluded users/groups.

**Usage Example:**
```php
$permission = new permission();
if (!empty($ifc_param3)) {
    foreach ($ifc_param3 AS $value) {
        $permission->permit($value, $ifc_param1, $explicit); // $explicit = TRUE for _ex variants
    }
}
if (!empty($ifc_param2)) {
    foreach ($ifc_param2 AS $value) {
        $permission->exclude($value, $ifc_param1, $explicit);
    }
}
$ifc_response = $permission->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
```

---

## Main Display

The main display renders the interface for managing users and groups, including:

1. **Type Selection**: Buttons to switch between users and groups.
2. **Object List**: A sortable list of users or groups with selection checkboxes.
3. **Properties Panel**: Displays and allows editing of the selected object's properties.

### Key Variables

| Variable | Type | Description |
|----------|------|-------------|
| `$count` | `int` | Length of the current `$type` (e.g., `4` for `"user"`). |
| `$menu` | `array` | Associative array of menu items (e.g., `"Create User" => "add_user"`). |
| `$data` | `data` | Reference to the `permission` class's data store. |
| `$_list` | `array` | List of all users/groups (for group/user assignment). |
| `$__list` | `array` | List of users assigned to the selected group. |

### Display Logic

1. **Type Selection**: Renders buttons to switch between users and groups.
2. **Object List**:
   - Displays users or groups in a table with name and comment.
   - Groups are shown in uppercase and bracketed (e.g., `[ADMINS]`).
   - Disabled objects are shown with a disabled icon.
   - Supports selection controls (All/Invert/None).
3. **Properties Panel**:
   - **User Properties**: Name, disabled flag, password, groups, permissions, exclusions, email, timezone, comment, and expiration.
   - **Group Properties**: Name, disabled flag, users, permissions, exclusions, and comment.

**Usage Example:**
```php
// Render the user list
$data->move("first");
while ($key = $data->move("next")) {
    if (strncmp($key, $type, $count)) continue; // Skip non-matching types
    echo("<tr><td class=\"select\">");
    $ifc->set(NULL, "checkbox", $key, streq($key, $object), "list[]");
    echo("</td><td>" . x($data->get($key, "name")) . "</td><td>" . x($data->get($key, "comment")) . "</td></tr>");
}
```

---

## Helper Functions

### `permission_is_user($key)`

**Purpose:**
Checks if a key represents a user.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | `string` | Object key (e.g., `"user.jdoe"`) |

**Return Value:**
- `bool`: `TRUE` if the key represents a user, `FALSE` otherwise.

---

### `permission_is_group($key)`

**Purpose:**
Checks if a key represents a group.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$key` | `string` | Object key (e.g., `"group.admins"`) |

**Return Value:**
- `bool`: `TRUE` if the key represents a group, `FALSE` otherwise.

---

### `permission_match($access, $permission, $exclusion)`

**Purpose:**
Determines if a given access rule is permitted or excluded for a user/group.

**Parameters:**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$access` | `string` | Access rule to check (e.g., `"module.content"`) |
| `$permission` | `string` | Newline-separated list of permitted rules |
| `$exclusion` | `string` | Newline-separated list of excluded rules |

**Return Value:**
- `bool`: `TRUE` if the access rule is permitted, `FALSE` if excluded.

**Inner Mechanisms:**
- Checks if `$access` matches any rule in `$permission` (permitted) or `$exclusion` (excluded).
- Exclusions take precedence over permissions.

**Usage Example:**
```php
if (permission_match("module.content", $user_permission, $user_exclusion)) {
    // User has access to the content module
}
```


<!-- HASH:3e80850378c423b4ce88a037477c2929 -->
