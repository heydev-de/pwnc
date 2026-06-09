# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/mysql.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/mysql.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## MySQL Compatibility Layer for PWNC Web Platform

This file provides a **MySQL compatibility layer** for the PWNC Web Platform, implementing the legacy `mysql_*` functions using PHP's modern `mysqli` extension. It ensures backward compatibility for older codebases while maintaining security and performance.

The layer includes:
- A global MySQL connection handler (`$cms_mysql_connection`)
- Result type constants (`MYSQL_ASSOC`, `MYSQL_NUM`, `MYSQL_BOTH`)
- Wrapper functions for all major `mysql_*` operations
- Automatic error handling with `trigger_error` for failed queries

---

### Global Variables

| Name                     | Type       | Description                                                                 |
|--------------------------|------------|-----------------------------------------------------------------------------|
| `$cms_mysql_connection`  | `mysqli`   | Global MySQL connection object. Automatically used if no link identifier is provided. |

---

### Constants

| Name          | Value       | Description                                                                 |
|---------------|-------------|-----------------------------------------------------------------------------|
| `MYSQL_ASSOC` | `MYSQLI_ASSOC` | Return associative arrays from query results.                               |
| `MYSQL_NUM`   | `MYSQLI_NUM`   | Return numeric arrays from query results.                                   |
| `MYSQL_BOTH`  | `MYSQLI_BOTH`  | Return both associative and numeric arrays from query results (default).    |

---

### Core Functions

---

#### `mysql_get_link_identifier`
**Purpose:**
Resolves the active MySQL connection link identifier. Prioritizes explicit identifiers, falls back to the global connection, or returns `NULL` if no connection exists.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `mysqli`   | Optional. Explicit MySQLi connection object. If omitted, uses global connection. |

**Return Values:**
- `mysqli` – Active connection object.
- `NULL` – No connection available.

**Inner Mechanisms:**
- Checks if the provided identifier is a valid `mysqli` object.
- Falls back to the global `$cms_mysql_connection` if no identifier is provided.
- Returns `NULL` if neither is available.

**Usage Context:**
Used internally by all other functions to resolve the connection context. Developers should rarely call this directly.

---

#### `mysql_affected_rows`
**Purpose:**
Returns the number of rows affected by the last `INSERT`, `UPDATE`, or `DELETE` query.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `mysqli`   | Optional. MySQLi connection object.                                         |

**Return Values:**
- `int` – Number of affected rows.
- `0` – No rows affected or no result available.

**Inner Mechanisms:**
- Resolves the connection using `mysql_get_link_identifier`.
- Delegates to `mysqli_affected_rows`.

**Usage Example:**
```php
mysql_query("UPDATE users SET status = 'active' WHERE last_login > NOW() - INTERVAL 30 DAY");
$affected = mysql_affected_rows();
echo "Updated $affected users.";
```

---

#### `mysql_client_encoding`
**Purpose:**
Returns the character set used by the current connection.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `mysqli`   | Optional. MySQLi connection object.                                         |

**Return Values:**
- `string` – Character set name (e.g., `utf8mb4`).
- `FALSE` – On failure.

**Inner Mechanisms:**
- Resolves the connection and calls `mysqli_character_set_name`.

**Usage Example:**
```php
$charset = mysql_client_encoding();
echo "Current connection charset: $charset";
```

---

#### `mysql_close`
**Purpose:**
Closes the MySQL connection.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `mysqli`   | Optional. MySQLi connection object.                                         |

**Return Values:**
- `bool` – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Resolves the connection and calls `mysqli_close`.

**Usage Example:**
```php
mysql_close(); // Closes the global connection
```

---

#### `mysql_connect`
**Purpose:**
Establishes a new MySQL connection.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$server`          | `string`   | Server address (e.g., `localhost`, `127.0.0.1:3306`, `/var/run/mysql.sock`). |
| `$username`        | `string`   | MySQL username.                                                             |
| `$password`        | `string`   | MySQL password.                                                             |
| `$new_link`        | `bool`     | Ignored (compatibility placeholder).                                        |
| `$client_flag`     | `int`      | Ignored (compatibility placeholder).                                        |
| `$persistent`      | `bool`     | If `TRUE`, uses a persistent connection.                                    |

**Return Values:**
- `mysqli` – Connection object on success.
- `FALSE` – On failure.

**Inner Mechanisms:**
- Parses `$server` for port or socket specifications.
- Prefixes the server with `p:` for persistent connections.
- Stores the connection in `$cms_mysql_connection`.

**Usage Example:**
```php
$conn = mysql_connect("localhost", "user", "password");
if (!$conn) {
    die("Connection failed: " . mysql_error());
}
```

---

#### `mysql_data_seek`
**Purpose:**
Moves the internal result pointer to a specified row.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$result`          | `mysqli_result` | MySQL result object.                                                        |
| `$row_number`      | `int`      | Row index to seek to (0-based).                                             |

**Return Values:**
- `bool` – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Delegates to `mysqli_data_seek`.

**Usage Example:**
```php
$result = mysql_query("SELECT * FROM users");
mysql_data_seek($result, 5); // Move to the 6th row
$row = mysql_fetch_assoc($result);
```

---

#### `mysql_db_name`
**Purpose:**
Retrieves the database name from a `SHOW DATABASES` result.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$result`          | `mysqli_result` | Result from `mysql_list_dbs()`.                                             |
| `$row`             | `int`      | Row index (0-based).                                                        |
| `$field`           | `string`   | Optional. Field name (e.g., `Database`). If omitted, returns the first field. |

**Return Values:**
- `string` – Database name.
- `FALSE` – On failure.

**Inner Mechanisms:**
- Seeks to the specified row.
- Fetches the row and returns the requested field.

**Usage Example:**
```php
$result = mysql_list_dbs();
$first_db = mysql_db_name($result, 0);
echo "First database: $first_db";
```

---

#### `mysql_errno`
**Purpose:**
Returns the error code for the last MySQL operation.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `mysqli`   | Optional. MySQLi connection object.                                         |

**Return Values:**
- `int` – Error code (0 if no error).

**Inner Mechanisms:**
- Resolves the connection and calls `mysqli_errno`.

**Usage Example:**
```php
mysql_query("INVALID QUERY");
if (mysql_errno()) {
    echo "Error: " . mysql_error();
}
```

---

#### `mysql_error`
**Purpose:**
Returns the error message for the last MySQL operation.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `mysqli`   | Optional. MySQLi connection object.                                         |

**Return Values:**
- `string` – Error message.
- Empty string if no error.

**Inner Mechanisms:**
- Resolves the connection and calls `mysqli_error`.

**Usage Example:**
```php
mysql_query("SELECT * FROM non_existent_table");
if (mysql_errno()) {
    echo "Error: " . mysql_error(); // Outputs "Table 'db.non_existent_table' doesn't exist"
}
```

---

#### `mysql_fetch_array`
**Purpose:**
Fetches a result row as an associative, numeric, or both types of array.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$result`          | `mysqli_result` | MySQL result object.                                                        |
| `$result_type`     | `int`      | Optional. One of `MYSQL_ASSOC`, `MYSQL_NUM`, or `MYSQL_BOTH` (default).     |

**Return Values:**
- `array` – Result row.
- `FALSE` – No more rows or error.

**Inner Mechanisms:**
- Delegates to `mysqli_fetch_array`.
- Returns `FALSE` if no more rows are available.

**Usage Example:**
```php
$result = mysql_query("SELECT id, name FROM users");
while ($row = mysql_fetch_array($result, MYSQL_ASSOC)) {
    echo "ID: {$row['id']}, Name: {$row['name']}";
}
```

---

#### `mysql_fetch_assoc`
**Purpose:**
Fetches a result row as an associative array.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$result`          | `mysqli_result` | MySQL result object.                                                        |

**Return Values:**
- `array` – Associative array of the row.
- `FALSE` – No more rows or error.

**Inner Mechanisms:**
- Delegates to `mysqli_fetch_assoc`.

**Usage Example:**
```php
$result = mysql_query("SELECT * FROM products");
while ($product = mysql_fetch_assoc($result)) {
    echo "Product: {$product['name']}, Price: {$product['price']}";
}
```

---

#### `mysql_fetch_field`
**Purpose:**
Returns metadata for a specific field in a result set.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$result`          | `mysqli_result` | MySQL result object.                                                        |
| `$field_offset`    | `int`      | Optional. Field index (0-based). If omitted, returns the next field.        |

**Return Values:**
- `object` – Field metadata (properties: `name`, `table`, `type`, `length`, `flags`, etc.).
- `FALSE` – On failure.

**Inner Mechanisms:**
- Seeks to the specified field offset if provided.
- Delegates to `mysqli_fetch_field`.

**Usage Example:**
```php
$result = mysql_query("SELECT * FROM users");
$field = mysql_fetch_field($result, 0); // First field
echo "Field name: {$field->name}, Type: {$field->type}";
```

---

#### `mysql_fetch_lengths`
**Purpose:**
Returns the lengths of each field in the current row.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$result`          | `mysqli_result` | MySQL result object.                                                        |

**Return Values:**
- `array` – Numeric array of field lengths.
- `FALSE` – On failure.

**Inner Mechanisms:**
- Delegates to `mysqli_fetch_lengths`.

**Usage Example:**
```php
$result = mysql_query("SELECT name, description FROM products");
$row = mysql_fetch_row($result);
$lengths = mysql_fetch_lengths($result);
echo "Name length: {$lengths[0]}, Description length: {$lengths[1]}";
```

---

#### `mysql_fetch_object`
**Purpose:**
Fetches a result row as an object.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$result`          | `mysqli_result` | MySQL result object.                                                        |
| `$class_name`      | `string`   | Optional. Class name to instantiate (default: `stdClass`).                  |
| `$params`          | `array`    | Optional. Constructor parameters for the class.                             |

**Return Values:**
- `object` – Row as an object.
- `FALSE` – No more rows or error.

**Inner Mechanisms:**
- Delegates to `mysqli_fetch_object`.

**Usage Example:**
```php
class User {
    public function __construct($id, $name) {
        $this->id = $id;
        $this->name = $name;
    }
}

$result = mysql_query("SELECT id, name FROM users");
while ($user = mysql_fetch_object($result, "User", [null, null])) {
    echo "User ID: {$user->id}, Name: {$user->name}";
}
```

---

#### `mysql_fetch_row`
**Purpose:**
Fetches a result row as a numeric array.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$result`          | `mysqli_result` | MySQL result object.                                                        |

**Return Values:**
- `array` – Numeric array of the row.
- `FALSE` – No more rows or error.

**Inner Mechanisms:**
- Delegates to `mysqli_fetch_row`.

**Usage Example:**
```php
$result = mysql_query("SELECT id, name FROM users");
while ($row = mysql_fetch_row($result)) {
    echo "ID: {$row[0]}, Name: {$row[1]}";
}
```

---

#### `mysql_field_flags`
**Purpose:**
Returns the flags associated with a field (e.g., `primary_key`, `not_null`).

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$result`          | `mysqli_result` | MySQL result object.                                                        |
| `$field_offset`    | `int`      | Field index (0-based).                                                      |

**Return Values:**
- `int` – Bitmask of field flags.
- `FALSE` – On failure.

**Inner Mechanisms:**
- Seeks to the specified field.
- Returns the `flags` property of the field object.

**Usage Example:**
```php
$result = mysql_query("SELECT * FROM users");
$flags = mysql_field_flags($result, 0); // First field
echo "Flags: $flags";
```

---

#### `mysql_field_len`
**Purpose:**
Returns the length of a field.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$result`          | `mysqli_result` | MySQL result object.                                                        |
| `$field_offset`    | `int`      | Field index (0-based).                                                      |

**Return Values:**
- `int` – Field length.
- `FALSE` – On failure.

**Inner Mechanisms:**
- Seeks to the specified field.
- Returns the `length` property of the field object.

**Usage Example:**
```php
$result = mysql_query("SELECT name FROM users");
$length = mysql_field_len($result, 0);
echo "Name field length: $length";
```

---

#### `mysql_field_name`
**Purpose:**
Returns the name of a field.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$result`          | `mysqli_result` | MySQL result object.                                                        |
| `$field_offset`    | `int`      | Field index (0-based).                                                      |

**Return Values:**
- `string` – Field name.
- `FALSE` – On failure.

**Inner Mechanisms:**
- Seeks to the specified field.
- Returns the `name` property of the field object.

**Usage Example:**
```php
$result = mysql_query("SELECT id, name FROM users");
$field_name = mysql_field_name($result, 1);
echo "Second field: $field_name"; // Outputs "name"
```

---

#### `mysql_field_seek`
**Purpose:**
Moves the internal field pointer to a specified field.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$result`          | `mysqli_result` | MySQL result object.                                                        |
| `$field_offset`    | `int`      | Field index (0-based).                                                      |

**Return Values:**
- `bool` – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Delegates to `mysqli_field_seek`.

**Usage Example:**
```php
$result = mysql_query("SELECT id, name FROM users");
mysql_field_seek($result, 1); // Move to the second field
$field = mysql_fetch_field($result);
echo "Field name: {$field->name}"; // Outputs "name"
```

---

#### `mysql_field_table`
**Purpose:**
Returns the name of the table a field belongs to.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$result`          | `mysqli_result` | MySQL result object.                                                        |
| `$field_offset`    | `int`      | Field index (0-based).                                                      |

**Return Values:**
- `string` – Table name.
- `FALSE` – On failure.

**Inner Mechanisms:**
- Seeks to the specified field.
- Returns the `table` property of the field object.

**Usage Example:**
```php
$result = mysql_query("SELECT users.id, orders.total FROM users JOIN orders ON users.id = orders.user_id");
$table = mysql_field_table($result, 1);
echo "Second field belongs to table: $table"; // Outputs "orders"
```

---

#### `mysql_field_type`
**Purpose:**
Returns the type of a field (e.g., `int`, `varchar`).

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$result`          | `mysqli_result` | MySQL result object.                                                        |
| `$field_offset`    | `int`      | Field index (0-based).                                                      |

**Return Values:**
- `int` – Field type constant (e.g., `MYSQLI_TYPE_VAR_STRING`).
- `FALSE` – On failure.

**Inner Mechanisms:**
- Seeks to the specified field.
- Returns the `type` property of the field object.

**Usage Example:**
```php
$result = mysql_query("SELECT id, name FROM users");
$type = mysql_field_type($result, 0);
echo "First field type: $type"; // Outputs e.g., "3" (MYSQLI_TYPE_LONG)
```

---

#### `mysql_free_result`
**Purpose:**
Frees the memory associated with a result set.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$result`          | `mysqli_result` | MySQL result object.                                                        |

**Return Values:**
- `bool` – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Delegates to `mysqli_free_result`.

**Usage Example:**
```php
$result = mysql_query("SELECT * FROM users");
mysql_free_result($result); // Free memory
```

---

#### `mysql_get_client_info`
**Purpose:**
Returns the MySQL client library version.

**Return Values:**
- `string` – Version string (e.g., `mysqlnd 8.1.2`).

**Inner Mechanisms:**
- Delegates to `mysqli_get_client_info`.

**Usage Example:**
```php
$version = mysql_get_client_info();
echo "MySQL client version: $version";
```

---

#### `mysql_get_host_info`
**Purpose:**
Returns the host information for the current connection.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `mysqli`   | Optional. MySQLi connection object.                                         |

**Return Values:**
- `string` – Host information (e.g., `localhost via TCP/IP`).

**Inner Mechanisms:**
- Resolves the connection and calls `mysqli_get_host_info`.

**Usage Example:**
```php
$host_info = mysql_get_host_info();
echo "Host info: $host_info";
```

---

#### `mysql_get_proto_info`
**Purpose:**
Returns the protocol version used by the current connection.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `mysqli`   | Optional. MySQLi connection object.                                         |

**Return Values:**
- `int` – Protocol version (e.g., `10`).

**Inner Mechanisms:**
- Resolves the connection and calls `mysqli_get_proto_info`.

**Usage Example:**
```php
$proto = mysql_get_proto_info();
echo "Protocol version: $proto";
```

---

#### `mysql_get_server_info`
**Purpose:**
Returns the MySQL server version.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `mysqli`   | Optional. MySQLi connection object.                                         |

**Return Values:**
- `string` – Server version (e.g., `8.0.33`).

**Inner Mechanisms:**
- Resolves the connection and calls `mysqli_get_server_info`.

**Usage Example:**
```php
$server_version = mysql_get_server_info();
echo "MySQL server version: $server_version";
```

---

#### `mysql_info`
**Purpose:**
Returns detailed information about the last query (e.g., rows matched/affected for `INSERT`, `UPDATE`, `DELETE`).

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `mysqli`   | Optional. MySQLi connection object.                                         |

**Return Values:**
- `string` – Query information (e.g., `Records: 3 Duplicates: 0 Warnings: 0`).
- `NULL` – No information available.

**Inner Mechanisms:**
- Resolves the connection and calls `mysqli_info`.

**Usage Example:**
```php
mysql_query("INSERT INTO users (name) VALUES ('Alice'), ('Bob')");
$info = mysql_info();
echo "Query info: $info"; // Outputs e.g., "Records: 2 Duplicates: 0 Warnings: 0"
```

---

#### `mysql_insert_id`
**Purpose:**
Returns the auto-increment ID generated by the last `INSERT` query.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `mysqli`   | Optional. MySQLi connection object.                                         |

**Return Values:**
- `int` – Last insert ID.
- `0` – No ID generated or error.

**Inner Mechanisms:**
- Resolves the connection and calls `mysqli_insert_id`.

**Usage Example:**
```php
mysql_query("INSERT INTO users (name) VALUES ('Charlie')");
$user_id = mysql_insert_id();
echo "New user ID: $user_id";
```

---

#### `mysql_list_dbs`
**Purpose:**
Lists all databases available on the MySQL server.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `mysqli`   | Optional. MySQLi connection object.                                         |

**Return Values:**
- `mysqli_result` – Result set containing database names.
- `FALSE` – On failure.

**Inner Mechanisms:**
- Executes `SHOW DATABASES` and returns the result.

**Usage Example:**
```php
$result = mysql_list_dbs();
while ($row = mysql_fetch_row($result)) {
    echo "Database: {$row[0]}";
}
```

---

#### `mysql_list_processes`
**Purpose:**
Lists all active MySQL processes.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `mysqli`   | Optional. MySQLi connection object.                                         |

**Return Values:**
- `mysqli_result` – Result set containing process information.
- `FALSE` – On failure.

**Inner Mechanisms:**
- Executes `SHOW PROCESSLIST` and returns the result.

**Usage Example:**
```php
$result = mysql_list_processes();
while ($process = mysql_fetch_assoc($result)) {
    echo "Process ID: {$process['Id']}, State: {$process['State']}";
}
```

---

#### `mysql_num_fields`
**Purpose:**
Returns the number of fields in a result set.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$result`          | `mysqli_result` | MySQL result object.                                                        |

**Return Values:**
- `int` – Number of fields.

**Inner Mechanisms:**
- Delegates to `mysqli_num_fields`.

**Usage Example:**
```php
$result = mysql_query("SELECT id, name, email FROM users");
$field_count = mysql_num_fields($result);
echo "Number of fields: $field_count"; // Outputs 3
```

---

#### `mysql_num_rows`
**Purpose:**
Returns the number of rows in a result set.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$result`          | `mysqli_result` | MySQL result object.                                                        |

**Return Values:**
- `int` – Number of rows.

**Inner Mechanisms:**
- Delegates to `mysqli_num_rows`.

**Usage Example:**
```php
$result = mysql_query("SELECT * FROM users");
$row_count = mysql_num_rows($result);
echo "Number of users: $row_count";
```

---

#### `mysql_pconnect`
**Purpose:**
Establishes a persistent MySQL connection.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$server`          | `string`   | Server address.                                                             |
| `$username`        | `string`   | MySQL username.                                                             |
| `$password`        | `string`   | MySQL password.                                                             |
| `$client_flag`     | `int`      | Ignored (compatibility placeholder).                                        |

**Return Values:**
- `mysqli` – Persistent connection object.
- `FALSE` – On failure.

**Inner Mechanisms:**
- Calls `mysql_connect` with `$persistent = TRUE`.

**Usage Example:**
```php
$conn = mysql_pconnect("localhost", "user", "password");
```

---

#### `mysql_ping`
**Purpose:**
Checks if the connection to the MySQL server is alive.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `mysqli`   | Optional. MySQLi connection object.                                         |

**Return Values:**
- `bool` – `TRUE` if the connection is alive, `FALSE` otherwise.

**Inner Mechanisms:**
- Executes `SELECT 1` and returns `TRUE` if successful.

**Usage Example:**
```php
if (!mysql_ping()) {
    echo "Connection lost. Reconnecting...";
    mysql_connect("localhost", "user", "password");
}
```

---

#### `mysql_query`
**Purpose:**
Executes a SQL query and returns the result.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$query`           | `string`   | SQL query to execute.                                                       |
| `$link_identifier` | `mysqli`   | Optional. MySQLi connection object.                                         |

**Return Values:**
- `mysqli_result` – For `SELECT`, `SHOW`, `DESCRIBE`, `EXPLAIN` queries.
- `bool` – `TRUE` for successful `INSERT`, `UPDATE`, `DELETE` queries.
- `FALSE` – On failure.

**Inner Mechanisms:**
- Resolves the connection using `mysql_get_link_identifier`*.
- Executes the query using `mysqli_query`.
- Triggers a user error if the query fails, including the error message and query.

**Usage Example:**
```php
$result = mysql_query("SELECT * FROM products WHERE price > 100");
if ($result) {
    while ($product = mysql_fetch_assoc($result)) {
        echo "Product: {$product['name']}";
    }
}
```

---

#### `mysql_real_escape_string`
**Purpose:**
Escapes special characters in a string for use in a SQL query.

**Parameters:**

| Name                 | Type       | Description                                                                 |
|----------------------|------------|-----------------------------------------------------------------------------|
| `$unescaped_string`  | `string`   | String to escape.                                                           |
| `$link_identifier`   | `mysqli`   | Optional. MySQLi connection object.                                         |

**Return Values:**
- `string` – Escaped string.
- `FALSE` – On failure.

**Inner Mechanisms:**
- Resolves the connection and calls `mysqli_real_escape_string`.

**Usage Example:**
```php
$user_input = "O'Reilly";
$escaped = mysql_real_escape_string($user_input);
mysql_query("INSERT INTO authors (name) VALUES ('$escaped')");
```

---

#### `mysql_result`
**Purpose:**
Returns the value of a specific field in a result set.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$result`          | `mysqli_result` | MySQL result object.                                                        |
| `$row`             | `int`      | Row index (0-based).                                                        |
| `$field`           | `string`   | Optional. Field name or offset. If omitted, returns the first field.        |

**Return Values:**
- `mixed` – Field value.
- `FALSE` – On failure.

**Inner Mechanisms:**
- Seeks to the specified row.
- Fetches the row and returns the requested field.

**Usage Example:**
```php
$result = mysql_query("SELECT name FROM users WHERE id = 1");
$name = mysql_result($result, 0, 0);
echo "User name: $name";
```

---

#### `mysql_select_db`
**Purpose:**
Selects the active database for the connection.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$database_name`   | `string`   | Database name to select.                                                    |
| `$link_identifier` | `mysqli`   | Optional. MySQLi connection object.                                         |

**Return Values:**
- `bool` – `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Resolves the connection and calls `mysqli_select_db`.

**Usage Example:**
```php
if (mysql_select_db("my_database")) {
    echo "Database selected successfully.";
}
```

---

#### `mysql_stat`
**Purpose:**
Returns the current server status.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `mysqli`   | Optional. MySQLi connection object.                                         |

**Return Values:**
- `string` – Server status (e.g., `Uptime: 12345 Threads: 2 Questions: 1000`).
- `FALSE` – On failure.

**Inner Mechanisms:**
- Resolves the connection and calls `mysqli_stat`.

**Usage Example:**
```php
$status = mysql_stat();
echo "Server status: $status";
```

---

#### `mysql_thread_id`
**Purpose:**
Returns the thread ID for the current connection.

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$link_identifier` | `mysqli`   | Optional. MySQLi connection object.                                         |

**Return Values:**
- `int` – Thread ID.

**Inner Mechanisms:**
- Resolves the connection and calls `mysqli_thread_id`.

**Usage Example:**
```php
$thread_id = mysql_thread_id();
echo "Thread ID: $thread_id";
```

---

#### `mysql_unbuffered_query`
**Purpose:**
Executes an unbuffered SQL query (does not fetch the entire result set into memory).

**Parameters:**

| Name               | Type       | Description                                                                 |
|--------------------|------------|-----------------------------------------------------------------------------|
| `$query`           | `string`   | SQL query to execute.                                                       |
| `$link_identifier` | `mysqli`   | Optional. MySQLi connection object.                                         |

**Return Values:**
- `mysqli_result` – For queries that return results.
- `bool` – `TRUE` for successful queries without results.
- `FALSE` – On failure.

**Inner Mechanisms:**
- Uses `mysqli_real_query` and `mysqli_use_result` for unbuffered results.
- Triggers a user error on failure.

**Usage Example:**
```php
$result = mysql_unbuffered_query("SELECT * FROM large_table");
while ($row = mysql_fetch_assoc($result)) {
    // Process rows one at a time to save memory
}
```


<!-- HASH:16ceffb63f4575303cb87ce07b375eeb -->
