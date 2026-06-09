# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.setup.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.setup.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Interface: Setup Module (`ifc.setup.inc`)

This file implements the **Setup Interface** for the PWNC Web Platform, handling initial system configuration, database setup, SMTP configuration, password management, and system updates. It provides a user-friendly interface for administrators to configure core system components during the initial setup or later adjustments.

---

## Overview

The setup interface is triggered when:
- The system detects an unconfigured state (e.g., default admin password, missing database connection).
- The administrator manually accesses the setup page.
- A system update or backup is required.

The interface processes user input via `CMS_IFC_MESSAGE` and displays forms for:
- Changing the administrator password.
- Configuring MySQL database connection.
- Restoring the database from backup.
- Configuring SMTP settings for email delivery.
- Performing system updates and backups.

---

## Functions and Message Handlers

The file is structured as a **message-driven interface**, where each `case` in the `switch (CMS_IFC_MESSAGE)` block handles a specific setup task.

---

### `setup_password`

Handles the administrator password change.

#### Purpose
Validates and updates the superuser password. Ensures:
- The password is not empty.
- The password and confirmation match.
- The password is securely hashed and stored.

#### Parameters
| Name          | Type   | Description                                                                 |
|---------------|--------|-----------------------------------------------------------------------------|
| `$ifc_param1` | string | New password (plaintext).                                                  |
| `$ifc_param2` | string | Password confirmation (plaintext).                                         |

#### Return / Response
- On success: `CMS_MSG_DONE` (password updated and cookie set).
- On error:
  - `CMS_MSG_ERROR . CMS_L_IFC_SETUP_020` (empty password).
  - `CMS_MSG_ERROR . CMS_L_IFC_SETUP_021` (password mismatch).

#### Inner Mechanisms
1. Checks if the password is empty using `stre()`.
2. Validates password confirmation using `nstreq()`.
3. Hashes the password using `hash64()`.
4. Stores the hashed password in the `#system/permission` data store.
5. Sets a secure cookie with a salted hash of the password.

#### Usage Example
```php
// Simulate user input (via interface)
$ifc_param1 = "SecurePass123!";
$ifc_param2 = "SecurePass123!";

// Process via setup_password
// → Password is hashed, stored, and cookie is set.
```

---

### `setup_database`

Configures the MySQL database connection.

#### Purpose
Saves MySQL connection parameters (host, database name, user, password) to the system configuration.

#### Parameters
| Name          | Type   | Description                     |
|---------------|--------|---------------------------------|
| `$ifc_param1` | string | MySQL host (e.g., `localhost`). |
| `$ifc_param2` | string | Database name.                  |
| `$ifc_param3` | string | MySQL username.                 |
| `$ifc_param4` | string | MySQL password.                 |

#### Return / Response
- `CMS_MSG_DONE` on successful save.
- `CMS_MSG_ERROR` on failure.

#### Inner Mechanisms
1. Instantiates the `system` class.
2. Calls `setval()` to store each parameter under the `mysql` group.
3. Saves the configuration via `save()`.

#### Usage Example
```php
$ifc_param1 = "localhost";
$ifc_param2 = "pwnc_db";
$ifc_param3 = "db_user";
$ifc_param4 = "db_password123";

// Process via setup_database
// → MySQL settings are saved to system config.
```

---

### `mysql_restore`

Restores the database from a backup.

#### Purpose
Triggers a database restore if the user confirms. Removes the restore flag afterward.

#### Parameters
| Name          | Type   | Description                     |
|---------------|--------|---------------------------------|
| `$ifc_param1` | string | Confirmation flag (`"yes"`).    |

#### Return / Response
- `CMS_MSG_DONE` on success.
- `CMS_MSG_ERROR . CMS_L_IFC_SETUP_031` if restore fails.

#### Inner Mechanisms
1. Checks if `$ifc_param1` is set and equals `"yes"`.
2. Instantiates the `mysql` class and calls `restore()`.
3. Clears the `mysql.restore` flag in the system config.

#### Usage Example
```php
$ifc_param1 = "yes";

// Process via mysql_restore
// → Database is restored from backup.
```

---

### `setup_smtp`

Configures SMTP settings for email delivery.

#### Purpose
Saves SMTP configuration (sender address, reply-to, method, host, credentials) and tests the connection.

#### Parameters
| Name          | Type   | Description                                      |
|---------------|--------|--------------------------------------------------|
| `$ifc_param1` | string | Sender email address.                            |
| `$ifc_param2` | string | Reply-to address.                                |
| `$ifc_param3` | string | Email method (`"mail"` or custom SMTP).          |
| `$ifc_param4` | string | SMTP host.                                       |
| `$ifc_param5` | string | SMTP username.                                   |
| `$ifc_param6` | string | SMTP password.                                   |

#### Return / Response
- `CMS_MSG_DONE` on success.
- `CMS_MSG_ERROR` with error message if SMTP test fails.

#### Inner Mechanisms
1. Saves all parameters to the `email` group in system config.
2. If method is not `"mail"`, tests SMTP connection:
   - Loads `smtp` and `mime` libraries.
   - Creates a test email.
   - Attempts to send via `smtp->send()`.
   - Captures and displays any errors.

#### Usage Example
```php
$ifc_param1 = "noreply@pwnc.it";
$ifc_param2 = "support@pwnc.it";
$ifc_param3 = "smtp";
$ifc_param4 = "smtp.pwnc.it";
$ifc_param5 = "smtp_user";
$ifc_param6 = "smtp_pass";

// Process via setup_smtp
// → SMTP settings saved and tested.
```

---

### `do_not_show`

Hides the setup interface after login for non-admin users.

#### Purpose
Prevents the setup interface from appearing after initial configuration.

#### Parameters
None (uses global `CMS_USER`).

#### Return / Response
- `CMS_MSG_DONE` on success.
- No action if user is not `"admin"`.

#### Inner Mechanisms
1. Checks if `CMS_USER` is `"admin"`.
2. Sets `setup.done = TRUE` and clears `mysql.restore` flag.
3. Saves the system config.

#### Usage Example
```php
// Called when user clicks "Do not show again"
// → Setup interface is hidden on next login.
```

---

### `update`

Triggers a system update in the background via `cms_daemon()`.

#### Purpose
Starts an asynchronous update process, optionally skipping backup.

#### Parameters
| Name               | Type    | Description                                      |
|--------------------|---------|--------------------------------------------------|
| `$update_skip_backup` | bool | If `TRUE`, skips backup during update.       |

#### Return / Response
None (launches daemon).

#### Inner Mechanisms
1. Calls `cms_daemon()` with a script that:
   - Loads the `update` library.
   - Calls `update->start()` with backup skip flag.
   - Cleans cache on success.

#### Usage Example
```php
$update_skip_backup = TRUE; // Skip backup
// Process via update
// → Daemon starts update process.
```

---

### `update_status`

Returns the current update status as JSON.

#### Purpose
Provides real-time status and log output for the update process.

#### Parameters
None.

#### Return / Response
- JSON array: `[status, log]`
  - `status`: Integer (see `CMS_UPDATE_STATUS_*` constants).
  - `log`: String (progress log or message).

#### Inner Mechanisms
1. Loads the `update` library.
2. Gets status via `update->get_status()`.
3. Returns JSON-encoded status and log.

#### Usage Example
```php
// Called via AJAX every 5 seconds
// → Returns [3, "Downloading update..."]
```

---

### `backup`

Triggers a system backup via `cms_daemon()`.

#### Purpose
Starts an asynchronous backup process.

#### Parameters
None.

#### Return / Response
None (launches daemon).

#### Inner Mechanisms
1. Calls `cms_daemon()` with a script that:
   - Loads the `update` library.
   - Calls `update->backup()`.

#### Usage Example
```php
// Process via backup
// → Daemon starts backup process.
```

---

### `_daemon`

Returns the current daemon status.

#### Purpose
Used by the frontend to poll daemon progress.

#### Parameters
None.

#### Return / Response
- Outputs the result of `cms_daemon_status()`.
- Exits immediately.

#### Usage Example
```php
// Called via AJAX
// → Returns "Backup in progress: 45% complete"
```

---

## Main Display Logic

The second half of the file renders the setup interface based on system state.

### Key Scenarios

1. **Admin Password Change**
   - Triggered if `CMS_SUPERUSER` is `"admin"` and password is default or empty.
   - Displays a form with password and confirmation fields.

2. **Database Configuration**
   - Triggered if MySQL connection fails.
   - Displays form for host, database, user, password.

3. **Database Restore**
   - Triggered if `mysql.restore` flag is set.
   - Asks for confirmation before restoring.

4. **SMTP Configuration**
   - Always displayed until valid settings are saved.
   - Includes radio buttons for `mail` vs. custom SMTP.
   - Tests connection on submission.

5. **Update & Backup**
   - Checks for available updates.
   - Displays progress bar and log during update/backup.
   - Allows downloading backup files.

---

## Usage Example: Full Setup Flow

```php
// 1. User logs in as "admin" with default password
// → Setup interface appears.

// 2. User changes password
$ifc_param1 = "NewSecurePass123!";
$ifc_param2 = "NewSecurePass123!";
// → setup_password updates password and sets cookie.

// 3. User configures database
$ifc_param1 = "localhost";
$ifc_param2 = "pwnc_db";
$ifc_param3 = "db_user";
$ifc_param4 = "db_pass";
// → setup_database saves settings.

// 4. User configures SMTP
$ifc_param1 = "noreply@pwnc.it";
$ifc_param3 = "smtp";
$ifc_param4 = "smtp.pwnc.it";
$ifc_param5 = "smtp_user";
$ifc_param6 = "smtp_pass";
// → setup_smtp saves and tests settings.

// 5. User triggers update
$update_skip_backup = FALSE;
// → update starts daemon.

// 6. Frontend polls update_status
// → Displays progress and log.
```

---

## Constants and Dependencies

| Constant                     | Description                                      |
|------------------------------|--------------------------------------------------|
| `CMS_L_ACCESS`               | Permission level required to access setup.       |
| `CMS_SUPERUSER`              | Default admin username (`"admin"`).              |
| `CMS_MSG_DONE` / `CMS_MSG_ERROR` | Response codes.                              |
| `CMS_L_IFC_SETUP_*`          | Localized strings for UI labels and messages.    |
| `CMS_UPDATE_STATUS_NONE`     | No update in progress.                           |
| `CMS_VERSION`                | Current system version.                          |

### Dependencies
- `data` class: For storing permissions.
- `system` class: For system configuration.
- `mysql` class: For database connection and restore.
- `smtp` and `mime` classes: For email testing.
- `update` class: For version checks and updates.
- `ifc` class: For rendering the interface.
- `cms_daemon()`, `cms_cache()`, `cms_url()`: Core utilities.

---

## Security Notes

- All passwords are hashed using `hash64()`.
- Cookies are set with salted hashes.
- Database and SMTP credentials are stored in system config (not exposed in UI after save).
- CSRF protection is handled by `cms_url()` and `ifc` form generation.
- Error messages are escaped using `x()` for XML safety.


<!-- HASH:2c26a9bce649b7e5f7bc323765fcf30e -->
