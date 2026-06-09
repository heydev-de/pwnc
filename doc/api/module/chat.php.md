# PWNC API Documentation

[← Index](../README.md) | [`module/chat.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/chat.php)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Chat Module (`module/chat.php`)

The **Chat Module** provides a real-time, web-based chat system with support for public/private messaging, emoticons, user profiles, and system notifications. It integrates with PWNC's core utilities for session management, user authentication, and data handling.

---

### **Constants**

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_CHAT_TIMEOUT` | `30` | User inactivity timeout (seconds) before automatic disconnection. |
| `CMS_CHAT_REFRESH` | `5000` | Interval (milliseconds) for polling new messages. |
| `CMS_CHAT_AWAY_TIMER_TIMEOUT` | `600000` | Time (milliseconds) before marking a user as "away". |
| `CMS_CHAT_AWAY_TIMER_REFRESH` | `30000` | Interval (milliseconds) for checking away status. |

---

## **Display Modes**

The module operates in multiple display modes, controlled via the `$chat_display` parameter.

---

### **Default Mode (Login Screen)**
Handles user authentication and initial connection to the chat.

#### **Purpose**
- Validates user session and permissions.
- Renders a login form for username input.
- Redirects to the chat interface upon successful connection.

#### **Mechanism**
1. Instantiates the `core` class with a timeout.
2. Checks if the user is already connected or banned.
3. If valid, connects the user and redirects to the interface.
4. If invalid, displays an error or login form.

#### **Usage Example**
```php
// Access the chat module without parameters (default mode)
header("Location: " . cms_url(["chat_display" => ""]));
```
**Scenario**: User navigates to the chat for the first time and is presented with a login form.

---

### **Interface Mode**
Renders the main chat interface with message display, input controls, and user settings.

#### **Purpose**
- Displays messages, emoticons, and user controls.
- Handles real-time message polling via JavaScript.
- Provides UI for sending messages, toggling settings, and managing user status.

#### **Mechanism**
1. Validates user session.
2. Renders HTML structure with:
   - Message output area (`#chat-output`).
   - Input form with emoticon support.
   - Control panel for settings (sound, focus, away status).
3. Loads JavaScript for real-time updates and interactions.

#### **Key JavaScript Functions**
| Function | Purpose |
|----------|---------|
| `chat_onload()` | Initializes chat, sets focus, and starts polling. |
| `chat_receive()` | Fetches new messages from the server. |
| `chat_send()` | Sends a message to the server. |
| `chat_append()` | Appends text (e.g., emoticons) to the input field. |
| `chat_write()` | Renders a message in the output area. |
| `chat_notify()` | Flashes the browser title for new messages. |
| `chat_sound()` | Plays sound notifications. |

#### **Usage Example**
```php
// Redirect to the chat interface after login
header("Location: " . cms_url(["chat_display" => "interface"]));
```
**Scenario**: User logs in and is redirected to the chat interface to start messaging.

---

### **Control Mode**
Renders a control panel for managing user profiles, permissions, and chat settings.

#### **Purpose**
- Provides administrative controls (e.g., user profiles, permissions).
- Loads the `core_control` module for interactive elements.

#### **Mechanism**
1. Validates user permissions (`CMS_CORE_PERMISSION_CONTROL`).
2. Renders an iframe with controls for:
   - Viewing/editing user profiles.
   - Managing chat permissions.
3. Uses `core_control()` to load dynamic control elements.

#### **Usage Example**
```php
// Open the control panel for a specific user
$url = cms_url([
    "chat_display" => "control",
    "core_control_object" => "user:12345"
]);
```
**Scenario**: An admin opens the control panel to manage a user's profile.

---

### **Send Mode**
Processes outgoing messages and commands.

#### **Purpose**
- Handles message submission and special commands (e.g., `/away`, `/disconnect`).
- Updates user status based on input.

#### **Parameters**
| Parameter | Type | Description |
|-----------|------|-------------|
| `$chat_data` | `string` | Raw message or command from the user. |

#### **Mechanism**
1. Validates user session.
2. Trims and processes the input:
   - Commands (e.g., `/away`) update user status.
   - Regular messages are sent to the chat.
3. Exits silently on success.

#### **Usage Example**
```javascript
// JavaScript: Send a message
asr_send(
    "<?php echo(q(cms_url(["chat_display" => "send", "chat_data" => "\x1B%data%"])));?>".replace("%data%", encodeURIComponent("Hello!")),
    _chat_send
);
```
**Scenario**: User types a message and submits it via AJAX.

---

### **Receive Mode**
Fetches and formats incoming messages for the client.

#### **Purpose**
- Polls the server for new messages.
- Returns messages in JSON format for client-side rendering.

#### **Return Value**
- JSON array of messages, each with a type (`m`, `p`, `s`, `i`, `d`) and associated data.

#### **Mechanism**
1. Validates user session.
2. Retrieves messages from the `core` class.
3. Formats messages into JSON:
   - **Public messages (`m`)**: Sender GUID, name, color, image, text.
   - **Private messages (`p`)**: Includes metadata for recipients.
   - **System messages (`s`)**: Text-only notifications.
   - **Info messages (`i`)**: Spy/operator notifications.
   - **Disconnect (`d`)**: Forces client disconnection.

#### **Usage Example**
```javascript
// JavaScript: Poll for new messages
asr_send("<?php echo(q(cms_url(["chat_display" => "receive"])));?>", _chat_receive);
```
**Scenario**: The client polls the server every 5 seconds for new messages.

---

### **Disconnect/Disconnected Modes**
Handles user disconnection and renders a reconnection screen.

#### **Purpose**
- Disconnects the user (if active).
- Displays a "disconnected" message with a reconnect button.

#### **Mechanism**
1. Disconnects the user (if in `disconnect` mode).
2. Renders a static HTML page with a reconnect link.

#### **Usage Example**
```php
// Redirect to the disconnected screen
header("Location: " . cms_url(["chat_display" => "disconnected"]));
```
**Scenario**: User closes the chat or is disconnected due to inactivity.

---

## **Core Dependencies**

| Dependency | Purpose |
|------------|---------|
| `core` | Manages user sessions, message storage, and chat state. |
| `core_control` | Provides interactive control elements (e.g., user profiles). |
| `log` | Logs chat access and user activity. |

---

## **Key Features**

1. **Real-Time Messaging**
   - Polling-based updates with configurable refresh rates.
   - Support for public and private messages.

2. **Emoticon Support**
   - Dynamically loads emoticons from `CMS_IMAGES_PATH/emoticon`.
   - Converts text shortcuts (e.g., `:)`) to images.

3. **User Status**
   - Away status triggered by inactivity or commands (`/away`).
   - Visual indicators for user presence.

4. **Notifications**
   - Browser title flashing for new messages.
   - Sound alerts (configurable).

5. **Security**
   - CSRF protection via `cms_url()`.
   - Permission checks for control panel access.

---

## **Example Workflow**

1. **User Login**
   - Accesses `chat.php` (default mode).
   - Enters a username and submits the form.
   - Redirects to `interface` mode.

2. **Sending a Message**
   - Types a message in the input field.
   - JavaScript sends the message to `send` mode via AJAX.
   - Server processes the message and updates the chat log.

3. **Receiving Messages**
   - JavaScript polls `receive` mode every 5 seconds.
   - New messages are rendered in the output area.

4. **Disconnecting**
   - User clicks "Disconnect" or closes the browser.
   - Server updates the user's status in `disconnect` mode.
   - Redirects to `disconnected` mode with a reconnect option.


<!-- HASH:cf5442525313ca20babcedf888ae8c51 -->
