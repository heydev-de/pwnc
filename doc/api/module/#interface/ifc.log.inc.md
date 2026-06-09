# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.log.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.log.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Log Interface Module (`ifc.log.inc`)

This file implements the **Log Interface** for the PWNC Web Platform, providing a comprehensive frontend for viewing, filtering, and analyzing access logs. It includes:

- **Visualization** of access data (graphs, tables, and reports).
- **Filtering** by time, user, IP, action, and other criteria.
- **User management** (editing user details and bot status).
- **Configuration** of logging settings (retention, privacy, bot detection).

The interface is permission-controlled, requiring at least `CMS_L_ACCESS` for basic access and `CMS_L_OPERATOR` for configuration.

---

### Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_LOG_PERMISSION_OPERATOR` | `CMS_L_OPERATOR` | Permission level required for configuration. |
| `CMS_DB_LOG_ACCESS_*` | Database column names | Columns in the `log_access` table (e.g., `CMS_DB_LOG_ACCESS_TIME`, `CMS_DB_LOG_ACCESS_USERID`). |
| `CMS_DB_LOG_USER_*` | Database column names | Columns in the `log_user` table (e.g., `CMS_DB_LOG_USER_NAME`, `CMS_DB_LOG_USER_BOT`). |
| `CMS_LOG_STATUS_*` | Status codes | User/bot statuses (e.g., `CMS_LOG_STATUS_USER_FIXED`, `CMS_LOG_STATUS_BAD_BOT`). |

---

## Core Logic

### Time Resolution and Filtering
The module dynamically adjusts time resolution (hourly, daily, weekly, monthly, quarterly, yearly) and applies filters to generate SQL queries for log data retrieval.

#### Key Variables
| Name | Type | Description |
|------|------|-------------|
| `$resolution` | `array` | Maps resolution keys (`h`, `d`, `w`, etc.) to SQL date functions. |
| `$interval` | `array` | Predefined time intervals (e.g., "last week", "last month") with associated resolutions. |
| `$filter_field` | `array` | Log fields available for filtering (e.g., `CMS_DB_LOG_ACCESS_USERID`, `CMS_DB_LOG_ACCESS_IP`). |
| `$filter_operation` | `array` | Filter operations (`LIKE`, `NOT LIKE`, `=`). |

---

### Message Handling
The interface processes messages via `CMS_IFC_MESSAGE` to trigger specific actions:

| Message | Description |
|---------|-------------|
| `config` | Displays configuration form for logging settings. |
| `_config` | Saves configuration changes. |
| `save` | Updates user data (name, email, info, bot status). |
| `load_raw` | Loads raw log entries (paginated). |
| `load_origin` | Loads top referrer domains. |
| `load_content` | Loads top accessed paths. |
| `load_activity` | Loads top actions. |
| `load_region` | Loads top regions. |
| `load_language` | Loads top languages. |
| `load_technology` | Loads top browsers. |
| `load_identity` | Loads top users. |
| `load_time` | Loads hourly activity distribution. |

---

## Functions and Methods

### `log_report($query, $columns)`
Generates an HTML report from a SQL query.

#### Parameters
| Name | Type | Description |
|------|------|-------------|
| `$query` | `string` | SQL query to execute. |
| `$columns` | `array` | Column definitions (keys: DB column names; values: `log_report_option` objects). |

#### Return Value
- `int`: Number of rows returned (for pagination).

#### Inner Mechanisms
1. Executes the query and fetches results.
2. Renders an HTML table with columns formatted according to `log_report_option` settings.
3. Applies callbacks for value transformation (e.g., `yesno`, `strtocolor`).

#### Usage Example
```php
log_report(
    "SELECT user_id, COUNT(*) AS count FROM log_access GROUP BY user_id",
    [
        "user_id" => new log_report_option("User", CMS_LOG_REPORT_OPTION_TYPE_TEXT),
        "count"   => new log_report_option("Count", CMS_LOG_REPORT_OPTION_TYPE_BEAM)
    ]
);
```
**Explanation**: Displays a table of user IDs and their access counts.

---

### `log_report_option` Class
Defines how a column is rendered in a report.

#### Properties
| Name | Type | Description |
|------|------|-------------|
| `label` | `string` | Column header text. |
| `type` | `int` | Rendering type (`CMS_LOG_REPORT_OPTION_TYPE_TEXT`, `CMS_LOG_REPORT_OPTION_TYPE_BEAM`). |
| `link` | `string` | JavaScript link template (e.g., `"javascript:f(%d)"`). |
| `link_callback` | `callable` | Callback to escape/format link values. |
| `value_callback` | `callable` | Callback to transform values (e.g., `yesno` for boolean display). |
| `color_callback` | `callable` | Callback to generate colors (e.g., `strtocolor` for user IDs). |

#### Usage Example
```php
new log_report_option(
    "IP Address",
    CMS_LOG_REPORT_OPTION_TYPE_TEXT,
    "javascript:f(2,2,'%s')",
    __NAMESPACE__ . "\\q",
    NULL,
    NULL
);
```
**Explanation**: Renders IP addresses as clickable links that trigger a filter.

---

## JavaScript Integration
The module embeds JavaScript for dynamic interactions:

| Function | Description |
|----------|-------------|
| `log_load_raw(offset, append)` | Loads raw log entries (paginated). |
| `log_load_stats(object, target, message)` | Loads statistical reports (e.g., top users, regions). |
| `d(value)` | Sets the date filter. |
| `f(field, operator, value)` | Sets the field filter. |
| `l(value)` | Navigates to a URL. |
| `b(value)` | Sets the bot filter. |

#### Example: Filtering by IP
```javascript
f(2, 2, "192.168.1.1"); // Filters logs by IP "192.168.1.1"
```
**Explanation**: Updates the interface to show only entries matching the IP.

---

## Graph Rendering
The module generates an SVG graph of access data with:
- **Accesses** (blue line).
- **Unique users** (red line).
- **Mobile accesses** (green line).
- **Time navigation** (previous/next buttons, zoom out).

#### Key Variables
| Name | Type | Description |
|------|------|-------------|
| `log` | `object` | JavaScript object containing log data (keys: timestamps; values: `[accesses, unique_users, mobile]`). |
| `factor_x`, `factor_y` | `float` | Scaling factors for the graph. |

#### Example: Clicking a Data Point
```javascript
// Clicking a circle in the graph updates the date filter and reloads the interface.
circle.addEventListener("click", () => {
    ifc_set("ifc_param1", "2023-10-01");
    ifc_submit();
});
```

---

## User Data Management
The interface allows operators to edit user details and bot status.

#### Example: Saving User Data
```php
case "save":
    $log->set_user(
        $object,    // User ID
        $ifc_param7, // Name
        $ifc_param8, // Email
        $ifc_param9, // Info
        $ifc_param10 // Bot status
    );
    break;
```
**Explanation**: Updates the user record in the database.

---

## Configuration
Operators can configure:
- **Log retention** (e.g., 30 days, 1 year).
- **IP anonymization** (privacy compliance).
- **Bot detection thresholds** (request limits, blocking).

#### Example: Setting Retention
```php
$system->setval(30, "log", "limit"); // Retain logs for 30 days
$system->save();
```
**Explanation**: Updates the system configuration to purge logs older than 30 days.


<!-- HASH:8f026caa9c3172b1984af4353c249f47 -->
