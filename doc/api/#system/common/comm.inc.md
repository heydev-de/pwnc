# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/comm.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/comm.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Notification Utility: `comm.inc`

This file provides core notification functionality for the PWNC Web Platform. It contains utilities to send administrative notifications, which are stored as inter-module system (IMS) messages and flagged for admin attention.

---

### `notify_admin()`

Sends a system notification to the administrator. The notification is stored in the IMS core resource and a flag file is created to alert the admin interface of a new message.

#### **Purpose**
- Creates a persistent, auditable notification for the platform administrator.
- Uses the IMS (Inter-Module System) core resource to store the message.
- Generates a flag file to trigger UI alerts in the admin dashboard.

#### **Parameters**

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$string` | `string` | The notification text to be sent to the administrator.                     |

#### **Return Values**

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `bool`    | `TRUE` if the notification was successfully created and flagged.            |
| `FALSE`   | If the `core_resource` library fails to load.                               |

#### **Inner Mechanisms**
1. **Library Loading**: Checks if `core_resource` is available. If not, returns `FALSE`.
2. **Core Resource Initialization**: Instantiates a `core_resource` object with a schema for IMS messages.
3. **Message Construction**:
   - Generates a unique 8-character ID and thread ID.
   - Sets the current timestamp.
   - Prepends a localization token (`CMS_L_COMMON_001`) to the message.
   - Computes a 32-character hash for message integrity.
4. **Message Storage**: Uses `seek()` and `set()` to insert the message into the IMS core.
5. **Flag Creation**: Opens (or creates) a flag file in the admin desktop directory and writes a single byte to signal a new notification.

#### **Usage Context**
- Used by system components to report critical events (e.g., failed backups, security alerts, module errors).
- Designed for internal system use, not user-facing notifications.
- Notifications persist until manually cleared by an admin.

#### **Usage Example**
```php
// Report a failed backup attempt
if (! backup_database()) {
    \cms\notify_admin("Automated database backup failed at " . date("Y-m-d H:i:s"));
}
```
**Explanation**:
When a scheduled backup fails, this call logs a timestamped alert to the admin. The message is stored in the IMS core and a flag file is created, ensuring the admin sees the alert on next login.


<!-- HASH:21ecc15b24a7d9440c0cf1b4d78d4777 -->
