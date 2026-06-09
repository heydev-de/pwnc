# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.core_control.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.core_control.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Core Control Functions

This file provides the core control interface for the PWNC Web Platform's real-time communication system. It handles user profiles, channel management, status modifications, and direct messaging between users. The functions generate interactive UI components for managing chat channels, user profiles, and permissions.

---

## `core_control_profile`

Renders a user profile card with status indicators, avatar, and descriptive information.

### Parameters

| Name      | Type       | Description                                                                 |
|-----------|------------|-----------------------------------------------------------------------------|
| `$core`   | `object`   | Core system object (passed by reference). Provides access to user data.    |
| `$guid`   | `string`   | Unique user identifier.                                                    |
| `$channel`| `string`   | Channel name (optional). Used to determine user status in the channel.     |
| `$url`    | `string`   | Target URL for the profile link (optional). If empty, renders as a `<div>`.|

### Return Value

None. Outputs HTML directly.

### Inner Mechanisms

1. **Status Determination**: Checks user status flags (owner, operator, banned, etc.) to determine visual styling and labels.
2. **Visibility Handling**: Skips rendering if the user is invisible and the current user is not an operator.
3. **Color Calculation**: Dynamically calculates text color (black or white) for contrast against the user's chosen background color.
4. **Image Processing**: Uses `image_process()` to resize the user's avatar to a fixed width (35px).
5. **Conditional Linking**: Renders as a clickable link if `$url` is provided, otherwise as a static `<div>`.

### Usage Context

- Displaying user profiles in chat channels, private messages, or admin panels.
- Used by `core_control()` to render user lists in channels or direct message interfaces.

### Example

```php
// Render the current user's profile in the active channel
core_control_profile($core, $core->guid, $core->index->get("channel"), cms_url(["core_control_object" => "profile:{$core->guid}"]));
```

---

## `core_control`

Generates the core control interface for chat channels, user profiles, and status management.

### Parameters

| Name                  | Type       | Description                                                                 |
|-----------------------|------------|-----------------------------------------------------------------------------|
| `$core`               | `object`   | Core system object (passed by reference).                                   |
| `$core_control_object`| `string`   | Object identifier (e.g., `"channel:general"`, `"profile:user123"`).         |
| `$core_control_command`| `string`   | Command to execute (e.g., `"connect"`, `"edit"`, `"message"`).             |
| `$core_control_value` | `array`    | Command parameters (e.g., `["name" => "New Name"]`).                        |

### Return Value

None. Outputs HTML and JavaScript directly.

### Inner Mechanisms

1. **Object Parsing**: Splits `$core_control_object` into type and identifier (e.g., `"channel:general"` → type `"channel"`, object `"general"`).
2. **Command Handling**: Executes backend logic for commands (e.g., connecting to a channel, editing a profile).
3. **Dynamic UI Generation**: Renders different interfaces based on the object type:
   - **Channel**: Lists users, handles connection/disconnection.
   - **Channel Create/Edit/Delete**: Forms for channel management.
   - **User**: Direct messaging interface.
   - **Profile**: Profile editing form with image upload support.
   - **Status**: Permission management for users/channels.
4. **File Upload Handling**: Processes profile images with size/type validation.
5. **Cross-Frame Communication**: Uses `postMessage` to trigger refreshes in parent frames.

### Usage Context

- Primary interface for the chat system.
- Used in iframes or modals to manage channels, users, and profiles.

### Example

```php
// Render the channel interface for the "general" channel
core_control($core, "channel:general");

// Render the profile editing interface for the current user
core_control($core, "profile:{$core->guid}");
```

---

## Constants and Flags

| Name                          | Value               | Description                                                                 |
|-------------------------------|---------------------|-----------------------------------------------------------------------------|
| `CMS_CORE_STATUS_OWNER`       | Bitmask             | User is the owner of a channel.                                             |
| `CMS_CORE_STATUS_OPERATOR`    | Bitmask             | User is a channel operator.                                                 |
| `CMS_CORE_STATUS_INVISIBLE`   | Bitmask             | User is invisible in the channel.                                           |
| `CMS_CORE_STATUS_BANNED`      | Bitmask             | User is banned from the channel.                                            |
| `CMS_CORE_STATUS_MUTE`        | Bitmask             | User is muted in the channel.                                               |
| `CMS_CORE_STATUS_SPY`         | Bitmask             | Operator is spying on the channel (admin-only).                             |
| `CMS_CORE_STATUS_ABSENT`      | Bitmask             | User is marked as absent.                                                   |
| `CMS_CORE_PERMISSION_OPERATOR`| Permission flag     | Permission to act as an operator.                                           |

---

## Helper Functions

### `flag($status, $flag)`

Checks if a bitmask flag is set in a status value.

#### Parameters

| Name     | Type     | Description                     |
|----------|----------|---------------------------------|
| `$status`| `int`    | Status bitmask.                 |
| `$flag`  | `int`    | Flag to check (e.g., `CMS_CORE_STATUS_OWNER`). |

#### Return Value

`bool`: `TRUE` if the flag is set, `FALSE` otherwise.

#### Example

```php
if (flag($status, CMS_CORE_STATUS_BANNED)) {
    echo "User is banned.";
}
```

---

## JavaScript Functions

### `core_control_channel_switch(input)`

Toggles visibility of channel description and password fields based on the "permanent" checkbox.

#### Parameters

| Name    | Type      | Description                     |
|---------|-----------|---------------------------------|
| `input` | `DOMElement` | Checkbox element.              |

#### Example

```javascript
// Called when the "permanent" checkbox is toggled
core_control_channel_switch(document.querySelector('input[name="core_control_value[permanent]"]'));
```

---

### `core_control_profile_image_select(link)`

Triggers the hidden file input for profile image selection.

#### Parameters

| Name   | Type      | Description                     |
|--------|-----------|---------------------------------|
| `link` | `DOMElement` | Clickable link element.        |

#### Return Value

`false`: Prevents default link behavior.

#### Example

```html
<a href="#" onclick="return core_control_profile_image_select(this);">Select Image</a>
```

---

### `core_control_profile_image_add(input)`

Handles profile image preview and validation before upload.

#### Parameters

| Name    | Type      | Description                     |
|---------|-----------|---------------------------------|
| `input` | `DOMElement` | File input element.            |

#### Example

```html
<input type="file" onchange="core_control_profile_image_add(this);">
```

---

## Typical Scenarios

1. **Channel Management**:
   - Create, edit, or delete channels.
   - Connect to channels with optional password protection.

2. **User Interaction**:
   - Send direct messages to other users.
   - View or edit user profiles.

3. **Status Control**:
   - Mute, ban, or promote users in channels.
   - Set visibility or spy on channels (admin-only).

4. **Profile Customization**:
   - Update profile name, color, description, or avatar.


<!-- HASH:4352eb2b12e52242e9dd915a5a502da3 -->
