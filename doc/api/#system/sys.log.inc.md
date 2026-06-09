# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.log.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.log.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Log System Module (`sys.log.inc`)

This file provides logging and reporting functionality for the PWNC Web Platform. It consists of two main components:

1. **`log_report_option` class** – Defines display options for log reports.
2. **`log` class** – Handles access logging, user tracking, and log management.

---

## `log_report_option` Class

Defines customization options for log report columns, including display type, links, and value transformations.

### Properties

| Name              | Value/Default | Description                                                                 |
|-------------------|---------------|-----------------------------------------------------------------------------|
| `$name`           | `NULL`        | Custom column name. If `NULL`, uses the database field name.               |
| `$type`           | `NULL`        | Display type (`CMS_LOG_REPORT_OPTION_TYPE_*`).                             |
| `$link`           | `NULL`        | Link template (e.g., `"user.php?id=%s"`).                                  |
| `$link_encoding`  | `NULL`        | Callable function to encode link values (default: `x()` for XML escaping). |
| `$link_source`    | `NULL`        | Source field for link values (default: current field).                     |
| `$value_function` | `NULL`        | Callable function to transform displayed values.                           |

### Constants

| Name                                      | Value | Description                          |
|-------------------------------------------|-------|--------------------------------------|
| `CMS_LOG_REPORT_OPTION_TYPE_NONE`         | `0`   | Hide column.                         |
| `CMS_LOG_REPORT_OPTION_TYPE_TEXT`         | `1`   | Display as text.                     |
| `CMS_LOG_REPORT_OPTION_TYPE_BEAM`         | `2`   | Display as a proportional bar graph. |

### Constructor

#### `__construct()`

**Purpose:**
Initializes a `log_report_option` instance.

**Parameters:**

| Name              | Type       | Description                                                                 |
|-------------------|------------|-----------------------------------------------------------------------------|
| `$name`           | `string`   | Custom column name.                                                         |
| `$type`           | `int`      | Display type (`CMS_LOG_REPORT_OPTION_TYPE_*`).                             |
| `$link`           | `string`   | Link template (e.g., `"user.php?id=%s"`).                                  |
| `$link_encoding`  | `callable` | Function to encode link values (default: `x()`).                           |
| `$link_source`    | `string`   | Source field for link values (default: current field).                     |
| `$value_function` | `callable` | Function to transform displayed values.                                    |

**Return Value:**
`void`

**Inner Mechanisms:**
- Assigns provided values to corresponding properties.

**Usage Example:**
```php
$option = new log_report_option(
    "User",
    CMS_LOG_REPORT_OPTION_TYPE_TEXT,
    "user.php?id=%s",
    NULL,
    "userid"
);
```
*Creates a column named "User" that links to a user profile page using the `userid` field.*

---

## `log_report()` Function

**Purpose:**
Executes a SQL query and displays the results as an HTML table with customizable columns.

**Parameters:**

| Name      | Type                          | Description                                                                 |
|-----------|-------------------------------|-----------------------------------------------------------------------------|
| `$sql`    | `string`                      | SQL query to execute.                                                       |
| `$options`| `array\|log_report_option[]`  | Associative array of field names to `log_report_option` instances or strings. |

**Return Value:**
- `FALSE` on query failure.
- `TRUE` for non-SELECT queries (e.g., INSERT, UPDATE).
- `array` of rows if `$options === FALSE`.
- `int` (number of rows) for SELECT queries with `$options`.

**Inner Mechanisms:**
1. Executes the SQL query and measures execution time.
2. For non-SELECT queries, displays affected rows if `$options` is provided.
3. For SELECT queries:
   - If `$options === FALSE`, returns raw results as an array.
   - Otherwise, renders an HTML table with customizable columns.
   - Supports text, links, and proportional bar graphs (`BEAM`).
   - Escapes all output using `x()` for XML safety.

**Usage Example:**
```php
$options = [
    "userid" => new log_report_option("User", CMS_LOG_REPORT_OPTION_TYPE_TEXT, "user.php?id=%s"),
    "action" => new log_report_option("Action", CMS_LOG_REPORT_OPTION_TYPE_TEXT),
    "time"   => "Timestamp"
];
log_report("SELECT userid, action, time FROM " . CMS_DB_LOG_ACCESS . " LIMIT 10", $options);
```
*Displays a log report with custom column names and a link to user profiles.*

---

## `log` Class

Handles access logging, user tracking, and log management.

### Properties

| Name               | Value/Default | Description                                                                 |
|--------------------|---------------|-----------------------------------------------------------------------------|
| `$limit`           | `NULL`        | Maximum log retention in days (`-1` to disable).                           |
| `$anonymize`       | `NULL`        | Whether to anonymize IP addresses.                                          |
| `$privacy`         | `NULL`        | Whether to enforce privacy (e.g., pseudonyms for anonymous users).          |
| `$bot_limit`       | `5`           | Maximum allowed requests in 10 seconds before flagging as a bot.            |
| `$bad_bot_limit`   | `10`          | Maximum allowed 404 errors in 10 seconds before flagging as a bad bot.      |
| `$bad_bot_delay`   | `5`           | Delay in seconds before re-checking bad bot status.                         |
| `$bad_bot_block`   | `FALSE`       | Whether to block bad bots.                                                  |
| `$bot_reset`       | `14`          | Days of inactivity before resetting bot status to provisional.              |
| `$bot_retention`   | `30`          | Days to retain bot access logs.                                             |
| `$operator`        | `FALSE`       | Whether the current user has operator permissions.                          |
| `$enabled`         | `NULL`        | Whether logging is enabled.                                                 |

### Constants

#### Database Tables

| Name                     | Value                     | Description                          |
|--------------------------|---------------------------|--------------------------------------|
| `CMS_DB_LOG_ACCESS`      | `"{prefix}log_access"`    | Access log table.                    |
| `CMS_DB_LOG_USER`        | `"{prefix}log_user"`      | User information table.              |
| `CMS_DB_LOG_UA_LIST`     | `"{prefix}log_ua_list"`   | Unique user agent hashes.            |
| `CMS_DB_LOG_UA_FREQ`     | `"{prefix}log_ua_freq"`   | User agent token frequencies.        |

#### Log Status

| Name                              | Value | Description                          |
|-----------------------------------|-------|--------------------------------------|
| `CMS_LOG_STATUS_USER_FIXED`       | `-1`  | Confirmed human user.                |
| `CMS_LOG_STATUS_USER_PROVISIONAL` | `0`   | Provisional human user.              |
| `CMS_LOG_STATUS_BOT_PROVISIONAL`  | `1`   | Provisional bot.                     |
| `CMS_LOG_STATUS_BOT_LIMIT_EXCEEDED`| `2`  | Bot exceeding access rate limits.    |
| `CMS_LOG_STATUS_BOT_FIXED`        | `3`   | Confirmed bot.                       |
| `CMS_LOG_STATUS_BAD_BOT`          | `4`   | Malicious bot.                       |

### Constructor

#### `__construct()`

**Purpose:**
Initializes the `log` class and verifies database tables.

**Parameters:**
None.

**Return Value:**
`void`

**Inner Mechanisms:**
- Verifies the existence of required database tables (`log_access`, `log_user`, `log_ua_list`, `log_ua_freq`).
- Loads configuration values (e.g., `limit`, `anonymize`, `privacy`) from the system.
- Sets `$enabled` to `TRUE` if tables exist and are accessible.

---

### Methods

#### `access()`

**Purpose:**
Logs an access event, including user actions, IP, agent, and other metadata. Detects and flags bots based on access patterns.

**Parameters:**

| Name     | Type     | Description                          |
|----------|----------|--------------------------------------|
| `$action`| `string` | Action performed (e.g., `"login"`).  |
| `$info`  | `string` | Additional information (e.g., `"success"`). |

**Return Value:**
`void`

**Inner Mechanisms:**
1. Skips logging if disabled or `$limit < 0`.
2. Generates a unique user ID based on the IP hash.
3. Checks the user's bot status and access rate.
4. Flags bots if access rate exceeds `$bot_limit` or 404 errors exceed `$bad_bot_limit`.
5. Logs metadata (IP, user agent, language, referrer, etc.).
6. Blocks bad bots if `$bad_bot_block` is `TRUE`.

**Usage Example:**
```php
$log = new log();
$log->access("login", "success");
```
*Logs a successful login event.*

---

#### `user()`

**Purpose:**
Updates or creates a user record in the `log_user` table.

**Parameters:**

| Name       | Type      | Description                                                                 |
|------------|-----------|-----------------------------------------------------------------------------|
| `$name`    | `string`  | User's name.                                                                |
| `$email`   | `string`  | User's email.                                                               |
| `$info`    | `string`  | Additional user information.                                                |
| `$bot`     | `int`     | Bot status (`CMS_LOG_STATUS_*`).                                            |
| `$userid`  | `string`  | User ID (default: `CMS_IPHASH`).                                            |
| `$append`  | `bool`    | Whether to append to existing values (e.g., names, emails).                 |

**Return Value:**
`bool` – `TRUE` if the record was updated/created, `FALSE` otherwise.

**Inner Mechanisms:**
1. Retrieves existing user data if the user exists.
2. Handles privacy settings (e.g., pseudonyms for anonymous users).
3. Appends new values to existing ones if `$append` is `TRUE`.
4. Updates bot status and retroactively applies it to access logs if changed.

**Usage Example:**
```php
$log = new log();
$log->user("John Doe", "john@example.com", "Admin", CMS_LOG_STATUS_USER_FIXED);
```
*Updates or creates a user record with the provided details.*

---

#### `set_user()`

**Purpose:**
Convenience method to set user details without appending to existing values.

**Parameters:**

| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$userid` | `string` | User ID.                             |
| `$name`   | `string` | User's name.                         |
| `$email`  | `string` | User's email.                        |
| `$info`   | `string` | Additional user information.         |
| `$bot`    | `int`    | Bot status (`CMS_LOG_STATUS_*`).     |

**Return Value:**
`bool` – `TRUE` if the record was updated/created, `FALSE` otherwise.

**Inner Mechanisms:**
- Calls `user()` with `$append = FALSE`.

**Usage Example:**
```php
$log = new log();
$log->set_user(CMS_IPHASH, "John Doe", "john@example.com", "", CMS_LOG_STATUS_USER_FIXED);
```
*Sets user details for the current user.*

---

#### `cleanup()`

**Purpose:**
Performs log maintenance, including:
- Removing old bot access logs.
- Removing old access logs.
- Removing orphaned user records.
- Resetting bot statuses for inactive users.

**Parameters:**
None.

**Return Value:**
`void`

**Inner Mechanisms:**
1. Removes bot access logs older than `$bot_retention` days.
2. Removes access logs older than `$limit` days.
3. Removes orphaned user records (no access logs).
4. Resets bot statuses to provisional for inactive users (based on `$bot_reset`).

**Usage Example:**
```php
$log = new log();
$log->cleanup();
```
*Performs log maintenance (e.g., in a cron job).*

---

#### `browser()`

**Purpose:**
Extracts and normalizes browser names from a user agent string.

**Parameters:**

| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$string` | `string` | User agent string.                   |
| `$count`  | `int`    | Number of browser tokens to return.  |

**Return Value:**
`string` – Normalized browser name (e.g., `"Firefox-Mobile"`).

**Inner Mechanisms:**
1. Tokenizes the user agent string to extract browser names.
2. Retrieves token frequencies from the database.
3. Increments frequencies for new token combinations.
4. Returns the most frequent tokens (e.g., `"Chrome-Mobile"`).

**Usage Example:**
```php
$log = new log();
$browser = $log->browser($_SERVER["HTTP_USER_AGENT"]);
```
*Extracts the browser name from the user agent string.*


<!-- HASH:40d97dbe6cae1dc8be1462afc471efa1 -->
