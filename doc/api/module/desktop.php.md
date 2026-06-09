# PWNC API Documentation

[← Index](../README.md) | [`module/desktop.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/desktop.php)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Desktop Module (`module/desktop.php`)

The **Desktop Module** is the core interface of the PWNC Web Platform, providing a graphical desktop environment for users. It manages objects like links, notes, appointments, addresses, containers, and mailboxes, allowing users to organize, manipulate, and interact with them in a drag-and-drop interface. The module handles user sessions, permissions, object lifecycle (creation, renaming, moving, deletion), and integrates with other modules like the Instant Messenger (IMS) and calendar.

---

## Overview

### Purpose
- Provides a **graphical desktop environment** for users to manage objects (links, notes, appointments, etc.).
- Handles **user sessions**, **permissions**, and **object lifecycle** (create, rename, move, delete, send).
- Integrates with **IMS**, **calendar**, and **address book** modules.
- Supports **drag-and-drop** operations and **context menus**.
- Manages **background images**, **timezone settings**, and **user preferences**.

### Key Features
| Feature | Description |
|---------|-------------|
| **Object Types** | Links, notes, appointments, addresses, containers, mailboxes. |
| **Drag-and-Drop** | Move objects between containers or to trash/receiver. |
| **Quick Access** | Pin objects to a quick-access bar. |
| **User Switching** | Switch between users with appropriate permissions. |
| **Background Customization** | Upload and manage desktop background images. |
| **Timezone Support** | Set and persist user timezone preferences. |
| **Messaging** | Send objects to other users. |
| **Real-Time Updates** | Dynamic UI updates via JavaScript and IFC (Inter-Frame Communication). |

---

## Constants and Global Variables

### Constants
| Name | Value/Default | Description |
|------|---------------|-------------|
| `DESKTOP_USER` | `CMS_SUPERUSER` or user ID | Current desktop user. |
| `DESKTOP_PATH` | `CMS_DATA_PATH . "#desktop/" . safe_filename(DESKTOP_USER) . "/"` | Path to user-specific desktop data. |

### Global Variables
| Name | Type | Description |
|------|------|-------------|
| `$desktop_display` | `string` | Determines the display mode (`interface`, `link`, `background`). |
| `$ifc_option` | `string` | Set to `"external"` if `$desktop_display` is `interface` or `link`. |
| `$user` | `string` | Current user ID. Falls back to `CMS_SUPERUSER` if invalid. |
| `$object` | `string` | Current selected object ID. |
| `$parent` | `string` | Parent container ID for the current object. |
| `$load` | `bool` | Whether to load the object in a popup. |
| `$desktop_icon` | `array` | Maps object types to their icon paths. |
| `$desktop_type` | `array` | Maps object types to their numeric type constants. |
| `$desktop_accept` | `array` | Defines which object types can be dropped into containers. |

---

## Core Logic

### Initialization
1. **Load Libraries**: `ifc` (Inter-Frame Communication) and `desktop` (core desktop logic).
2. **Sync Cache**: Loads user-specific desktop settings from cache.
3. **Set User**: Validates and sets the current user, defaulting to `CMS_SUPERUSER` if invalid.
4. **Permission Check**: Ensures the user has access to the desktop module.
5. **Message Handling**: Processes IFC messages (e.g., `activate`, `select`, `create`, `rename`, `drop`, `move`, `delete`, `send`, `config`).

---

## Message Handling

The module processes IFC messages to perform actions on desktop objects. Each message corresponds to a user action (e.g., creating an object, renaming, moving, or deleting).

### Message Types
| Message | Parameters | Description |
|---------|------------|-------------|
| `activate` | `$ifc_param` (object ID) | Activates an object or container. |
| `select` | `$ifc_param` (object ID) | Selects an object. |
| `create` | `$ifc_param` (type;name) | Creates a new object of the specified type. |
| `quickaccess` | `$ifc_param` (object ID) | Toggles quick-access status for an object. |
| `rename` | `$ifc_param` (new name) | Renames the selected object. |
| `drop` | `$ifc_param` (object ID,x,y) | Updates the position of an object. |
| `move` | `$ifc_param` (object ID,parent ID) | Moves an object to a new parent container. |
| `delete` | `$ifc_param` (object ID) | Deletes an object. |
| `send` | `$ifc_param` (object ID) | Sends an object to other users. |
| `config` | `$ifc_param1` to `$ifc_param6` | Configures user settings (password, timezone, background). |

---

## Class: `desktop`

The `desktop` class (loaded via `cms_load("desktop", TRUE)`) manages desktop objects and their lifecycle. While the full class is not shown here, its methods are used extensively in the module.

### Key Methods
| Method | Description |
|--------|-------------|
| `object_type($id)` | Returns the type of the object with the given ID. |
| `create_object($parent, $type, $name)` | Creates a new object of the specified type under the given parent. |
| `object_set($id, $key, $value)` | Sets a property (`$key`) for the object with ID `$id`. |
| `object_get($id, $key)` | Gets a property (`$key`) for the object with ID `$id`. |
| `move_object($id, $parent)` | Moves an object to a new parent container. |
| `delete_object($id)` | Deletes an object. |
| `save()` | Saves the desktop state to persistent storage. |

---

## Functions and Logic Blocks

### `desktop_activate($id, $type = 0)`
**Purpose**: Activates an object (e.g., opens a link, note, or appointment in a popup).
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$id` | `string` | Object ID to activate. |
| `$type` | `int` | Object type (e.g., `CMS_DESKTOP_TYPE_LINK`). |

**Usage Example**:
```javascript
desktop_activate('obj123', <?php echo(q(CMS_DESKTOP_TYPE_LINK)); ?>);
```
- Opens a popup for the object with ID `obj123` if it is a link.

---

### `desktop_event($event, $source, $target)`
**Purpose**: Handles drag-and-drop events and object activation.
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$event` | `string` | Event type (`activate`, `select`, `dropon`, `drop`). |
| `$source` | `DOMElement` | Source element (e.g., dragged object). |
| `$target` | `DOMElement` | Target element (e.g., container, trashbin). |

**Event Types**:
| Event | Description |
|-------|-------------|
| `activate` | Activates the source object. |
| `select` | Selects the source object. |
| `dropon` | Handles dropping an object onto a target (e.g., trashbin, receiver). |
| `drop` | Updates the position of an object after dragging. |

**Usage Example**:
```javascript
dd_set_callback(desktop_event);
```
- Registers `desktop_event` as the callback for drag-and-drop operations.

---

### `desktop_create($type)`
**Purpose**: Creates a new object of the specified type.
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$type` | `string` | Object type (`link`, `note`, `appointment`, `address`, `container`, `mailbox`). |

**Usage Example**:
```javascript
desktop_create('note');
```
- Prompts the user for a name and creates a new note.

---

### `desktop_rename()`
**Purpose**: Renames the currently selected object.
**Usage Example**:
```javascript
desktop_rename();
```
- Opens a prompt for the user to enter a new name for the selected object.

---

### `desktop_user_select($value)`
**Purpose**: Switches the current desktop user.
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | `string` | User ID to switch to. |

**Usage Example**:
```javascript
desktop_user_select('user123');
```
- Switches the desktop to the user with ID `user123`.

---

## Display Modes

### `default` Mode
- **Purpose**: Renders the full desktop interface with objects, quick-access bar, and controls.
- **Key Elements**:
  - **Desktop Background**: Displays a user-uploaded background image if available.
  - **Desktop Control**: Logo, user selection, quick-access bar, object creation, appointments, and logout.
  - **Objects**: Renders objects in containers or on the desktop, with drag-and-drop support.
  - **Path Navigation**: Shows the path to the current container.
  - **Drag-and-Drop Targets**: Trashbin and receiver for object deletion and sending.

### `interface` Mode
- **Purpose**: Opens a specific object in an interface (e.g., note editor, appointment viewer).
- **Logic**:
  - Loads the appropriate interface file (`desktop.$desktop_interface.inc`) based on the object type.
  - Passes parameters (`desktop_interface`, `user`, `object`) to the interface.

**Usage Example**:
```php
$desktop_display = "interface";
$object = "obj123"; // Appointment object
require("module/desktop.php");
```
- Opens the appointment interface for `obj123`.

### `background` Mode
- **Purpose**: Serves the user's desktop background image.
- **Logic**:
  - Checks for the existence of a background image (JPG, PNG, or WEBP).
  - Sends the image with appropriate caching headers.

**Usage Example**:
```php
$desktop_display = "background";
$user = "user123";
$extension = "jpg";
require("module/desktop.php");
```
- Serves the background image for `user123` in JPG format.

---

## Usage Examples

### Example 1: Creating a New Note
1. **User Action**: Clicks the "Note" button in the desktop control.
2. **JavaScript**:
   ```javascript
   desktop_create('note');
   ```
3. **Prompt**: User enters the note name (e.g., "Meeting Notes").
4. **IFC Message**: `create` is sent with parameters `note;Meeting Notes`.
5. **PHP Logic**:
   ```php
   $ifc_param = explode(";", "note;Meeting Notes");
   $desktop->create_object($parent, $ifc_param[0], $ifc_param[1]);
   ```
6. **Result**: A new note object is created and displayed on the desktop.

---

### Example 2: Moving an Object to a Container
1. **User Action**: Drags an object (e.g., a link) onto a container.
2. **JavaScript**:
   ```javascript
   desktop_event("dropon", sourceElement, targetElement);
   ```
3. **IFC Message**: `move` is sent with parameters `link123,container456`.
4. **PHP Logic**:
   ```php
   $ifc_param = explode(",", "link123,container456");
   $desktop->move_object($ifc_param[0], $ifc_param[1]);
   ```
5. **Result**: The link is moved into the container.

---

### Example 3: Sending an Object to Another User
1. **User Action**: Drags an object onto the "Receiver" target.
2. **JavaScript**:
   ```javascript
   desktop_event("dropon", sourceElement, receiverElement);
   ```
3. **IFC Message**: `send` is sent with the object ID.
4. **PHP Logic**:
   ```php
   $desktop->data->copy($object);
   foreach ($ifc_param1 AS $user) {
       $_desktop = new desktop($user);
       $_desktop->data->set_buffer($desktop->data->buffer);
       $_desktop->save();
   }
   ```
5. **Result**: The object is copied to the selected users' desktops.

---

### Example 4: Configuring User Settings
1. **User Action**: Clicks the "Configuration" menu item.
2. **IFC Message**: `config` is sent.
3. **PHP Logic**:
   ```php
   $ifc = new ifc(..., "_config", ...);
   $ifc->set(CMS_L_MOD_DESKTOP_014, "password 30 40");
   $ifc->set(timezone_identifiers_list(), "select 40 b", CMS_TIMEZONE);
   $ifc->close();
   ```
4. **Result**: A form is displayed for the user to update their password, timezone, and background image.

---

## Integration with Other Modules

### Instant Messenger (IMS)
- **Indicator**: A blinking icon appears if there are unread IMS messages.
- **Action**: Clicking the IMS icon opens the IMS interface.
- **Code**:
  ```php
  if (is_file(DESKTOP_PATH . "ims.flag")) {
      echo("<span class=\"blink\">" . image("desktop/icon_ims") . "</span>");
  }
  ```

### Calendar
- **Action**: Clicking the calendar icon opens the appointment interface.
- **Code**:
  ```php
  echo("<a href=\"javascript:load_page('', '" .
       qr(cms_url(["desktop_display" => "interface", "desktop_interface" => "appointment"])) .
       "');\">" . image("desktop/icon_appointment") . "</a>");
  ```

### Address Book
- **Action**: Clicking the address book icon opens the address interface.
- **Code**:
  ```php
  echo("<a href=\"javascript:load_page('', '" .
       qr(cms_url(["desktop_display" => "interface", "desktop_interface" => "address"])) .
       "');\">" . image("desktop/icon_address") . "</a>");
  ```

---

## Security Considerations
- **Permissions**: All actions are gated by `cms_permission()` checks.
- **CSRF Protection**: IFC messages are protected by CSRF tokens via `cms_url()`.
- **Input Validation**: User inputs (e.g., object names, passwords) are validated and escaped.
- **File Uploads**: Background images are validated for allowed extensions (JPG, PNG, WEBP).

---

## Performance Considerations
- **Caching**: User desktop state is cached to avoid repeated database queries.
- **Lazy Loading**: Objects are loaded only when needed (e.g., when a container is opened).
- **Minimal Dependencies**: The module avoids external libraries to maintain performance.


<!-- HASH:96d9522d981cf01e8390162e286e4508 -->
