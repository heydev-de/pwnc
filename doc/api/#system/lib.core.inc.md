# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.core.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.core.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Core Communication Relay Class

The `core` class serves as the central communication hub for the PWNC Web Platform, managing real-time user interactions, channel operations, and status tracking. It handles user connections, message passing, profile management, and channel administration with thread-safe resource locking.

---

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_CORE_PERMISSION_CONTROL` | `"control"` | Permission identifier for control operations. |
| `CMS_CORE_PERMISSION_OPERATOR` | `"operator"` | Permission identifier for operator-level access. |
| `CMS_CORE_PORTAL` | `"Lobby"` | Default channel name for unassigned users. |
| `CMS_CORE_STATUS_NONE` | `0` | No status flags set. |
| `CMS_CORE_STATUS_OWNER` | `1` | User is the owner of a channel. |
| `CMS_CORE_STATUS_OPERATOR` | `2` | User is an operator in a channel. |
| `CMS_CORE_STATUS_SPY` | `4` | User can see private messages. |
| `CMS_CORE_STATUS_INVISIBLE` | `8` | User is invisible to others. |
| `CMS_CORE_STATUS_ABSENT` | `16` | User is marked as absent. |
| `CMS_CORE_STATUS_MUTE` | `32` | User is muted and cannot send messages. |
| `CMS_CORE_STATUS_BANNED` | `64` | User is banned from the system. |
| `CMS_CORE_DATA_DEFAULT` | `0` | Default message type. |
| `CMS_CORE_DATA_SYSTEM` | `1` | System-generated message. |
| `CMS_CORE_DATA_PRIVATE` | `2` | Private message. |
| `CMS_CORE_DATA_PRIVATE_META` | `4` | Metadata for private messages (e.g., recipient list). |
| `CMS_CORE_DATA_PRIVATE_DATA` | `8` | Content of a private message. |
| `CMS_CORE_DATA_RESPONSE` | `16` | Response to the sender (echo). |
| `CMS_CORE_DATA_SPY_META` | `32` | Metadata visible to spies. |
| `CMS_CORE_DATA_SPY_DATA` | `64` | Message content visible to spies. |

---

### Properties

| Name | Default | Description |
|------|---------|-------------|
| `guid` | `NULL` | Unique identifier for the current user. |
| `timeout` | `15` | Connection timeout in seconds. |
| `enabled` | `FALSE` | Indicates if the core is active for the current user. |
| `operator` | `FALSE` | Indicates if the current user has operator privileges. |
| `index` | `NULL` | Resource object tracking connected users. |
| `profile` | `NULL` | Resource object storing user profiles. |
| `channel` | `NULL` | Resource object storing channel definitions. |
| `status` | `NULL` | Resource object tracking user statuses. |
| `data` | `NULL` | Resource object for message exchange. |

---

### Constructor: `__construct`

#### Purpose
Initializes the core communication system, loads required resources, and validates user permissions.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$timeout` | `int` | Connection timeout in seconds. Default: `15`. |
| `$datapath` | `string` | Relative path to core data files. Default: `"#core"`. |

#### Return Value
- `void`

#### Inner Mechanisms
1. Loads the `core_resource` library.
2. Generates a `guid` for the current user.
3. Initializes resource objects for `status`, `index`, `profile`, `channel`, and `data`.
4. Validates user permissions and bans if necessary.
5. Schedules periodic cleanup via `cms_cache`.

#### Usage Example
```php
$core = new \cms\core(30, "custom_path");
```
Initializes the core with a 30-second timeout and custom data path.

---

### Method: `unique_name`

#### Purpose
Checks if a username is available (not already in use by another user).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Username to check. |

#### Return Value
- `bool`: `TRUE` if the name is available, `FALSE` otherwise.

#### Inner Mechanisms
1. Locks the `profile` resource to prevent race conditions.
2. Iterates through existing profiles to check for name collisions.
3. Skips the current user’s own profile.

#### Usage Example
```php
if ($core->unique_name("Alice")) {
    // Proceed with registration
}
```

---

### Method: `connect`

#### Purpose
Registers a user in the system, assigns a profile, and notifies the channel.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Username for guests. Default: `""`. |

#### Return Value
- `bool`: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. Validates core activity and checks for existing connections.
2. Assigns a profile (new or existing).
3. Generates a random color for guest users.
4. Updates the `index` and notifies the channel.

#### Usage Example
```php
if ($core->connect("Alice")) {
    // User is now connected
}
```

---

### Method: `disconnect`

#### Purpose
Removes a user from the system, cleans up their profile (if applicable), and notifies the channel.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$clean` | `bool` | If `TRUE`, deletes the profile for non-banned users. Default: `TRUE`. |

#### Return Value
- `bool`: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. Locks resources to prevent race conditions.
2. Notifies the channel of the disconnection.
3. Deletes the user’s profile if they are not banned and `$clean` is `TRUE`.
4. Removes all messages addressed to the user.

#### Usage Example
```php
$core->disconnect(); // Clean up profile
$core->disconnect(FALSE); // Keep profile
```

---

### Method: `get_status`

#### Purpose
Retrieves the combined status of a user (global + channel-specific).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$guid` | `string` | User identifier. Default: `NULL` (current user). |
| `$channel` | `string` | Channel name. Default: `NULL` (global status). |

#### Return Value
- `int`: Bitmask of status flags.

#### Inner Mechanisms
1. Locks the `status` resource.
2. Combines global and channel-specific statuses using bitwise OR.

#### Usage Example
```php
$status = $core->get_status();
if (flag($status, CMS_CORE_STATUS_BANNED)) {
    // User is banned
}
```

---

### Method: `set_status`

#### Purpose
Sets the status of a user for a specific channel.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$status` | `int` | Status bitmask. Default: `CMS_CORE_STATUS_NONE`. |
| `$guid` | `string` | User identifier. Default: `NULL` (current user). |
| `$channel` | `string` | Channel name. Default: `NULL` (global status). |

#### Return Value
- `bool`: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. Locks the `status` resource.
2. Updates or inserts the status record.

#### Usage Example
```php
$core->set_status(CMS_CORE_STATUS_OWNER, NULL, "General");
```

---

### Method: `delete_status`

#### Purpose
Removes all status records for a user.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$guid` | `string` | User identifier. Default: `NULL` (current user). |

#### Return Value
- `bool`: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. Locks the `status` resource.
2. Deletes all records matching the `guid`.

#### Usage Example
```php
$core->delete_status("!admin");
```

---

### Method: `update_status`

#### Purpose
Removes status records for a channel if it no longer exists (temporary or permanent).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$channel` | `string` | Channel name. |

#### Return Value
- `bool`: `TRUE` if the channel was cleaned up, `FALSE` otherwise.

#### Inner Mechanisms
1. Locks `index`, `channel`, and `status` resources.
2. Checks if the channel exists in either `index` or `channel`.
3. Deletes all status records for the channel if it does not exist.

#### Usage Example
```php
$core->update_status("OldChannel");
```

---

### Method: `update_index`

#### Purpose
Disconnects users who have timed out and updates the current user’s timestamp.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$query_connection` | `bool` | If `TRUE`, skips updating the current user’s timestamp. Default: `FALSE`. |

#### Return Value
- `bool`: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. Locks the `index` resource.
2. Iterates through connected users and disconnects those who have timed out.
3. Updates the current user’s timestamp unless `$query_connection` is `TRUE`.

#### Usage Example
```php
$core->update_index(); // Update all users
$core->update_index(TRUE); // Skip current user
```

---

### Method: `connect_channel`

#### Purpose
Moves a user to a new channel, validating permissions and notifying both channels.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$channel` | `string` | Target channel name. |
| `$password` | `string` | Channel password. Default: `""`. |

#### Return Value
- `bool`: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. Validates user permissions and channel password.
2. Notifies the current channel of the departure.
3. Grants owner/operator status if the channel is new.
4. Updates the user’s channel in the `index`.

#### Usage Example
```php
if ($core->connect_channel("General", "secret")) {
    // User moved to "General"
}
```

---

### Method: `create_channel`

#### Purpose
Creates a new permanent channel (administrators only).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$channel` | `string` | Channel name. |
| `$text` | `string` | Channel description. Default: `""`. |
| `$password` | `string` | Channel password. Default: `""`. |

#### Return Value
- `bool`: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. Validates the channel name and administrator permissions.
2. Locks the `channel` resource and inserts the new channel.
3. Grants owner/operator status to the creator.

#### Usage Example
```php
$core->create_channel("Support", "User support channel", "support123");
```

---

### Method: `set_channel`

#### Purpose
Updates the description or password of an existing channel.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$channel` | `string` | Channel name. |
| `$text` | `string` | New description. Default: `""`. |
| `$password` | `string` | New password. Default: `""`. |

#### Return Value
- `bool`: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. Validates administrator or owner permissions.
2. Locks the `channel` resource and updates the record.

#### Usage Example
```php
$core->set_channel("General", "Main discussion channel", "newpass");
```

---

### Method: `delete_channel`

#### Purpose
Deletes a permanent channel and cleans up its status records.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$channel` | `string` | Channel name. |

#### Return Value
- `bool`: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. Validates administrator or owner permissions.
2. Locks the `channel` resource and deletes the record.
3. Cleans up status records for the channel.

#### Usage Example
```php
$core->delete_channel("OldChannel");
```

---

### Method: `send`

#### Purpose
Sends a message to all users in a channel or privately to specific users.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$data` | `string` | Message content. |
| `$receiver` | `string|array` | Recipient GUID(s). Default: `NULL` (channel-wide). |
| `$status` | `int` | Message type bitmask. Default: `CMS_CORE_DATA_DEFAULT`. |

#### Return Value
- `bool`: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. Validates sender permissions and connection status.
2. Handles private messages, spies, and invisible users.
3. Locks the `index` and `data` resources to prevent race conditions.
4. Inserts messages into the `data` resource for each recipient.

#### Usage Example
```php
// Channel-wide message
$core->send("Hello, everyone!");

// Private message
$core->send("Hello, Alice!", "!alice", CMS_CORE_DATA_PRIVATE);
```

---

### Method: `receive`

#### Purpose
Retrieves all messages addressed to the current user and optionally deletes them.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$delete` | `bool` | If `TRUE`, deletes messages after retrieval. Default: `TRUE`. |

#### Return Value
- `array`: List of messages, each as an associative array with keys: `guid`, `receiver`, `status`, `data`.

#### Inner Mechanisms
1. Locks the `data` resource.
2. Iterates through messages addressed to the current user.
3. Deletes messages if `$delete` is `TRUE`.

#### Usage Example
```php
$messages = $core->receive();
foreach ($messages as $message) {
    echo $message["data"];
}
```

---

### Method: `get_profile`

#### Purpose
Retrieves the profile information for a user.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$guid` | `string` | User identifier. Default: `NULL` (current user). |

#### Return Value
- `array|bool`: Associative array of profile data or `FALSE` if not found.

#### Inner Mechanisms
1. Locks the `profile` resource.
2. Retrieves the profile record for the specified `guid`.

#### Usage Example
```php
$profile = $core->get_profile("!admin");
if ($profile) {
    echo $profile["name"];
}
```

---

### Method: `set_profile`

#### Purpose
Updates the profile information for a user.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$guid` | `string` | User identifier. Default: `NULL` (current user). |
| `$name` | `string` | New username. |
| `$color` | `string` | New color. Default: `""`. |
| `$text` | `string` | New description. Default: `""`. |
| `$image` | `string` | New image path. Default: `""`. |

#### Return Value
- `bool`: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. Validates permissions (self, administrator, or guest).
2. Locks the `profile` resource.
3. Validates the username and updates the profile.
4. Handles image updates (deletes old image if a new one is provided).

#### Usage Example
```php
$core->set_profile(NULL, "Alice", "#ff0000", "Hello!", "alice.png");
```

---

### Method: `delete_profile`

#### Purpose
Deletes a user’s profile and associated image.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$guid` | `string` | User identifier. Default: `NULL` (current user). |
| `$system` | `bool` | If `TRUE`, bypasses permission checks. Default: `FALSE`. |

#### Return Value
- `bool`: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. Validates permissions (self, administrator, or system override).
2. Locks the `profile` resource.
3. Deletes the profile and associated image.

#### Usage Example
```php
$core->delete_profile("!guest123");
```

---

### Method: `status`

#### Purpose
Modifies the status of a user (e.g., mute, ban, invisible) and notifies affected parties.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$value` | `int` | Status flag to set or remove (use negative values to remove). |
| `$guid` | `string` | User identifier. Default: `NULL` (current user). |
| `$channel` | `string` | Channel name. Default: `NULL` (global status). |
| `$system` | `bool` | If `TRUE`, bypasses permission checks. Default: `FALSE`. |
| `$test` | `bool` | If `TRUE`, only checks permissions without applying changes. Default: `FALSE`. |

#### Return Value
- `bool`: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. Validates permissions based on the status change.
2. Locks `index` and `status` resources.
3. Applies the status change and notifies affected users.
4. Handles special cases (e.g., banning, invisibility).

#### Usage Example
```php
// Mute a user
$core->status(CMS_CORE_STATUS_MUTE, "!user123", "General");

// Unban a user
$core->status(-CMS_CORE_STATUS_BANNED, "!user123");
```

---

### Method: `clean`

#### Purpose
Removes orphaned profiles, statuses, and messages for users who no longer exist.

#### Return Value
- `bool`: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. Locks all resources (`index`, `profile`, `status`, `data`).
2. Retrieves a list of valid user accounts from permissions and database.
3. Deletes profiles, statuses, and messages for non-existent users.

#### Usage Example
```php
$core->clean();
```

---

### Method: `switch_guid`

#### Purpose
Temporarily switches the current user context to another user (e.g., for impersonation).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$guid` | `string` | Target user identifier. |
| `$reset` | `bool` | If `TRUE`, restores the previous context. Default: `FALSE`. |

#### Return Value
- `void`

#### Inner Mechanisms
1. Uses a static stack to track context switches.
2. Updates the `guid` and `operator` properties.

#### Usage Example
```php
$core->switch_guid("!admin");
// Perform actions as admin
$core->reset_guid(); // Restore original context
```

---

### Method: `reset_guid`

#### Purpose
Restores the previous user context after a `switch_guid` call.

#### Return Value
- `void`

#### Usage Example
```php
$core->reset_guid();
```


<!-- HASH:ac17225ae1f5f963d890be6aaaacbb89 -->
