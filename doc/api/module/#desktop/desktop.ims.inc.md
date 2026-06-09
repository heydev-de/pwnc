# PWNC API Documentation

[← Index](../../README.md) | [`module/#desktop/desktop.ims.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23desktop/desktop.ims.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Desktop Instant Messaging System (IMS)

This file implements the **Instant Messaging System (IMS)** module for the PWNC Web Platform's desktop interface. It provides real-time messaging capabilities between users with thread-based conversations, filtering, and message management.

The module handles:
- Message sending, replying, and deletion
- User and thread filtering
- Message display with sender/receiver metadata
- Notification management
- Permission-based receiver selection

---

### Constants

| Name                          | Value/Default | Description                                                                 |
|-------------------------------|---------------|-----------------------------------------------------------------------------|
| `CMS_DESKTOP_IMS_STATUS_NONE` | `0`           | Message status: Not sent (draft or received)                               |
| `CMS_DESKTOP_IMS_STATUS_SENT` | `1`           | Message status: Sent (outgoing)                                             |

---

### Initialization

#### Core Resource Setup
```php
$ims = new core_resource(
    CMS_DATA_PATH . "#desktop/ims.core",
    ["id"       => "string[8]",
     "thread"   => "string[8]",
     "time"     => "string[20]",
     "owner"    => "_string[40]",
     "receiver" => "_string[40]",
     "sender"   => "_string[40]",
     "status"   => "byte",
     "text"     => "_string[1500]",
     "hash"     => "string[32]"]);
```
**Purpose**:
Creates a persistent storage object for IMS messages with the specified schema.

**Schema Fields**:
| Field     | Type           | Description                                                                 |
|-----------|----------------|-----------------------------------------------------------------------------|
| `id`      | `string[8]`    | Unique message identifier                                                   |
| `thread`  | `string[8]`    | Thread identifier (groups related messages)                                 |
| `time`    | `string[20]`   | Timestamp of message creation                                               |
| `owner`   | `_string[40]`  | User who owns the message (receiver for incoming, sender for outgoing)      |
| `receiver`| `_string[40]`  | Message recipient (username)                                                |
| `sender`  | `_string[40]`  | Message sender (username)                                                   |
| `status`  | `byte`         | Message status (see constants)                                              |
| `text`    | `_string[1500]`| Message content (max 1500 chars)                                            |
| `hash`    | `string[32]`   | Unique hash for message grouping (identical for replies in the same thread) |

---

### Receiver List Generation
```php
$list_receiver = NULL;
$data = new data("#system/permission");
$data->move("first");
while ($key = $data->move("next")) {
    if (!permission_is_user($key)) continue;
    $_key = substr($key, 5);
    if (nstreq($_key, DESKTOP_USER) && cms_permission(CMS_APPLICATION, NULL, NULL, $_key))
        $list_receiver[$data->get($key, "name")] = $_key;
};
```
**Purpose**:
Generates a list of users who can receive messages, excluding the current user and those without application permissions.

**Mechanism**:
1. Iterates through all system permissions
2. Filters for user entries (`permission_is_user`)
3. Excludes the current user (`DESKTOP_USER`)
4. Checks application permissions (`cms_permission`)
5. Stores usernames as keys with display names as values

**Usage Context**:
Used to populate the recipient selection dropdown in the message composition interface.

---

## Message Handling

### `filter_user` / `filter_thread` / `filter_reset`
**Purpose**:
Handles message filtering based on user or thread.

| Case            | Action                                                                 |
|-----------------|------------------------------------------------------------------------|
| `filter_user`   | Sets `$filter_user` to the provided parameter                          |
| `filter_thread` | Sets `$filter_thread` to the provided parameter                        |
| `filter_reset`  | Resets both filters to `NULL`                                          |

**Parameters**:
| Parameter      | Type     | Description                          |
|----------------|----------|--------------------------------------|
| `$ifc_param`   | `string` | Username or thread ID to filter by   |

**Usage Example**:
```javascript
// Filter messages by user "admin"
ifc_post('filter_user', 'admin');

// Filter messages by thread "a1b2c3d4"
ifc_post('filter_thread', 'a1b2c3d4');

// Reset all filters
ifc_post('filter_reset');
```

---

### `_send` / `_reply` (Internal Handlers)
**Purpose**:
Internal message creation handlers that prepare data for the UI.

| Case    | Action                                                                 |
|---------|------------------------------------------------------------------------|
| `_send` | Creates a new thread and prepares an empty message form                |
| `_reply`| Loads an existing message to populate reply form                      |

**Parameters**:
| Parameter      | Type     | Description                          |
|----------------|----------|--------------------------------------|
| `$ifc_param`   | `string` | Message ID (for replies)             |

**Return Values**:
- Sets `$thread`, `$text`, `$ifc_param1`, and `$ifc_param2` for UI rendering
- Triggers the appropriate UI display via `$message = "_" . CMS_IFC_MESSAGE`

**Mechanism**:
1. For replies: Loads the original message and extracts thread ID and text
2. For new messages: Generates a new thread ID
3. Passes data to the UI handler

---

### `send` / `reply` (UI Handlers)
**Purpose**:
Displays the message composition interface with appropriate pre-filled data.

**Parameters**:
| Parameter      | Type               | Description                          |
|----------------|--------------------|--------------------------------------|
| `$ifc_param`   | `string`           | Message ID (for replies)             |
| `$ifc_param1`  | `array`/`null`     | Selected recipients                  |
| `$ifc_param2`  | `string`/`null`    | Message text                         |

**UI Elements**:
1. **Recipient Selection**:
   - Multiselect dropdown with all available users
   - Pre-selects sender for replies
2. **Message Display**:
   - Shows original message for replies
3. **Text Input**:
   - 55x20 textarea with 1500-character limit

**Usage Example**:
```javascript
// Open new message form
ifc_post('send');

// Open reply form for message ID "a1b2c3d4"
ifc_post('reply', 'a1b2c3d4');
```

---

### `_send` / `_reply` (Execution Handlers)
**Purpose**:
Processes submitted messages and stores them in the system.

**Parameters**:
| Parameter      | Type               | Description                          |
|----------------|--------------------|--------------------------------------|
| `$ifc_param1`  | `array`            | Array of recipient usernames         |
| `$ifc_param2`  | `string`           | Message text                         |

**Mechanism**:
1. Generates a unique hash for the message thread
2. Creates two entries for each recipient:
   - One marked as `SENT` (for the sender)
   - One marked as `NONE` (for the recipient)
3. Creates a notification file (`ims.flag`)
4. Returns `CMS_MSG_DONE` on success

**Error Handling**:
- Returns `CMS_MSG_ERROR` if no recipients are selected
- Returns `CMS_L_DESKTOP_IMS_004` if no receivers are available

**Usage Example**:
```javascript
// Send message to "admin" and "moderator"
ifc_post('_send', ['admin', 'moderator'], 'Hello world!');
```

---

### `delete`
**Purpose**:
Deletes selected messages based on their hash values.

**Parameters**:
| Parameter      | Type               | Description                          |
|----------------|--------------------|--------------------------------------|
| `$list`        | `array`            | Array of message hashes to delete    |

**Mechanism**:
1. Filters messages by:
   - Current user (`owner = DESKTOP_USER`)
   - Thread (if `$filter_thread` is set)
2. For sent messages:
   - Only deletes if no user filter is set OR the filtered user is the receiver
3. For received messages:
   - Only deletes if no user filter is set OR the filtered user is the sender
4. Returns `CMS_MSG_DONE` on completion

**Usage Example**:
```javascript
// Delete messages with hashes "abc123" and "def456"
ifc_post('delete', ['abc123', 'def456']);
```

---

## Main Display

### Notification Handling
```php
$path = DESKTOP_PATH . "ims.flag";
if (is_file($path)) unlink($path);
```
**Purpose**:
Removes the notification flag file when the IMS interface is loaded.

---

### Menu Generation
```php
$menu = [CMS_L_COMMAND_REFRESH . "|desktop/command_refresh" => NULL];
if ($list_receiver) $menu[CMS_L_DESKTOP_IMS_001 . "|desktop/command_create"] = "send";
$menu[CMS_L_COMMAND_DELETE_SELECTED . "|desktop/command_delete"] = "delete";
if ($filter_user || $filter_thread) $menu[CMS_L_COMMAND_PREVIOUS . "|desktop/command_cancel"] = "filter_reset";
```
**Purpose**:
Generates the action menu for the IMS interface.

**Menu Items**:
| Item                          | Condition               | Action               |
|-------------------------------|-------------------------|----------------------|
| Refresh                       | Always                  | Refreshes interface  |
| New Message                   | Receivers available     | Opens send form      |
| Delete Selected               | Always                  | Deletes selected     |
| Reset Filters                 | Filters active          | Clears filters       |

---

### Message Display
**Purpose**:
Renders all messages matching the current filters in a threaded view.

**Mechanism**:
1. Applies filters:
   - Current user as owner
   - Thread filter (if set)
2. Groups messages by hash (for sent messages)
3. Sorts messages by timestamp (newest first)
4. Renders each message with:
   - Selection checkbox
   - Sender/receiver information
   - Message content
   - Reply button (for received messages)

**Display Logic**:
| Message Type | Styling          | Metadata Display                     | Actions                     |
|--------------|------------------|---------------------------------------|-----------------------------|
| Sent         | Default          | All recipients listed                 | Thread filter               |
| Received     | `related`/`related2` | Sender only                        | Thread filter, Reply button |

**Usage Example**:
```php
// The display automatically shows all messages for the current user
// with optional filtering applied via $filter_user and $filter_thread
```

---

### Selection Controls
```php
$ifc->set(CMS_L_ALL, "button", "javascript:ifc_list_activate();");
$ifc->set(CMS_L_INVERT, "button", "javascript:ifc_list_invert();");
$ifc->set(CMS_L_NONE, "button", "javascript:ifc_list_deactivate();");
```
**Purpose**:
Provides bulk selection controls for message management.

**Controls**:
| Label  | Action                          | JavaScript Function       |
|--------|---------------------------------|---------------------------|
| All    | Selects all messages            | `ifc_list_activate()`     |
| Invert | Inverts current selection       | `ifc_list_invert()`       |
| None   | Deselects all messages          | `ifc_list_deactivate()`   |

---


<!-- HASH:418b25ff13ec1dc55e0b2e5836cb1481 -->
