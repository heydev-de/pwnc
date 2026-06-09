# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.mysql.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.mysql.inc)

- **Version:** `26.6.3.4`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## MySQL Database Class

The `mysql` class provides a comprehensive interface for interacting with MySQL/MariaDB databases in the PWNC Web Platform. It handles connection management, CRUD operations, schema verification, data export/import, and backup/restore functionality. The class follows PWNC's zero-dependency approach by using custom wrappers around native MySQL functions.

---

### Properties

| Name        | Default Value       | Description                                                                 |
|-------------|---------------------|-----------------------------------------------------------------------------|
| `database`  | `NULL`              | Database name retrieved from system configuration.                          |
| `host`      | `NULL`              | Database host retrieved from system configuration.                          |
| `user`      | `NULL`              | Database username retrieved from system configuration.                      |
| `password`  | `NULL`              | Database password retrieved from system configuration.                      |
| `software`  | `NULL`              | Detected database software (MySQL or MariaDB).                              |
| `version`   | `NULL`              | Detected database version.                                                  |
| `engine`    | `"InnoDB"`          | Default storage engine for tables.                                          |
| `charset`   | `"utf8mb4"`         | Default character set for tables.                                           |
| `collation` | `"utf8mb4_unicode_ci"` | Default collation for tables.                                           |

---

### Constructor

#### `mysql::__construct()`

**Purpose:**
Initializes the MySQL connection by retrieving credentials from the system configuration and establishing a connection.

**Parameters:**
None.

**Return Values:**
- `void`

**Inner Mechanisms:**
1. Instantiates the `system` class to access configuration values.
2. Retrieves database credentials (`database`, `host`, `user`, `password`).
3. Calls `connection()` to establish a connection.

**Usage Context:**
Automatically invoked when creating a new instance of the `mysql` class. Typically used at the start of a script to initialize database access.

**Example:**
```php
$db = new \cms\mysql();
```

---

### Methods

---

#### `mysql::connection()`

**Purpose:**
Establishes and manages a persistent database connection. Validates the database software and version, and initializes session settings.

**Parameters:**
None.

**Return Values:**
- `mysqli|FALSE`: Returns the MySQL connection resource on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Retrieves an existing connection using `mysql_get_link_identifier()`.
2. If no connection exists, establishes a new one using `mysql_connect()`.
3. Validates the database software and version (MySQL ≥ 5.6 or MariaDB ≥ 10).
4. Initializes session settings (SQL mode, character set, time zone).
5. Selects the database using `mysql_select_db()`.

**Usage Context:**
Called internally by other methods to ensure a valid connection. Can be used to manually re-establish a connection if needed.

**Example:**
```php
if ($db->connection() === FALSE) {
    die("Failed to connect to the database.");
}
```

---

#### `mysql::get($index, $column, $table, $index_key = "id")`

**Purpose:**
Retrieves a single value from a specified table column based on a primary key.

**Parameters:**

| Name        | Type     | Description                                      |
|-------------|----------|--------------------------------------------------|
| `$index`    | `mixed`  | Value of the primary key to query.               |
| `$column`   | `string` | Column name to retrieve.                         |
| `$table`    | `string` | Table name.                                      |
| `$index_key`| `string` | Primary key column name (default: `"id"`).      |

**Return Values:**
- `mixed|FALSE`: The retrieved value on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Ensures a valid connection via `connection()`.
2. Executes a `SELECT` query with a `LIMIT 1` clause.
3. Returns the first column of the first row using `mysql_result()`.

**Usage Context:**
Used to fetch a single value from a table, such as retrieving a user's email by their ID.

**Example:**
```php
$user_email = $db->get(42, "email", "users");
if ($user_email !== FALSE) {
    echo "User email: " . x($user_email);
}
```

---

#### `mysql::set($index, $column, $table, $value, $index_key = "id")`

**Purpose:**
Updates a single column value in a table row identified by a primary key.

**Parameters:**

| Name        | Type     | Description                                      |
|-------------|----------|--------------------------------------------------|
| `$index`    | `mixed`  | Value of the primary key to update.              |
| `$column`   | `string` | Column name to update.                           |
| `$table`    | `string` | Table name.                                      |
| `$value`    | `mixed`  | New value to set.                                |
| `$index_key`| `string` | Primary key column name (default: `"id"`).      |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Ensures a valid connection via `connection()`.
2. Executes an `UPDATE` query with a `LIMIT 1` clause.
3. Returns the result of `mysql_query()`.

**Usage Context:**
Used to update a single field in a table, such as changing a user's status.

**Example:**
```php
$success = $db->set(42, "status", "users", "active");
if ($success) {
    echo "User status updated successfully.";
}
```

---

#### `mysql::delete($index, $table, $index_key = "id", $parent_key = "container")`

**Purpose:**
Recursively deletes a row and all its child rows in a hierarchical table structure.

**Parameters:**

| Name         | Type     | Description                                      |
|--------------|----------|--------------------------------------------------|
| `$index`     | `mixed`  | Value of the primary key to delete.              |
| `$table`     | `string` | Table name.                                      |
| `$index_key` | `string` | Primary key column name (default: `"id"`).      |
| `$parent_key`| `string` | Parent reference column name (default: `"container"`). |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Ensures a valid connection via `connection()`.
2. Recursively queries child rows using the parent reference column.
3. Deletes child rows before deleting the parent row to maintain referential integrity.

**Usage Context:**
Used to delete hierarchical data, such as removing a category and all its subcategories.

**Example:**
```php
$success = $db->delete(5, "categories");
if ($success) {
    echo "Category and its subcategories deleted successfully.";
}
```

---

#### `mysql::is_child($index, $parent, $table, $index_key = "id", $parent_key = "container")`

**Purpose:**
Checks if a row is a descendant of a specified parent row in a hierarchical table structure.

**Parameters:**

| Name         | Type     | Description                                      |
|--------------|----------|--------------------------------------------------|
| `$index`     | `mixed`  | Value of the primary key to check.               |
| `$parent`    | `mixed`  | Value of the parent primary key to compare.      |
| `$table`     | `string` | Table name.                                      |
| `$index_key` | `string` | Primary key column name (default: `"id"`).      |
| `$parent_key`| `string` | Parent reference column name (default: `"container"`). |

**Return Values:**
- `bool`: `TRUE` if the row is a descendant, `FALSE` otherwise.

**Inner Mechanisms:**
1. Ensures a valid connection via `connection()`.
2. Traverses the hierarchy from the child to the parent using a `do-while` loop.
3. Returns `TRUE` if the parent is found during traversal.

**Usage Context:**
Used to validate hierarchical relationships, such as checking if a product belongs to a specific category.

**Example:**
```php
if ($db->is_child(10, 5, "products")) {
    echo "Product 10 is a descendant of category 5.";
}
```

---

#### `mysql::verify_table($table, $column, $index = NULL, $mapping = NULL)`

**Purpose:**
Verifies and synchronizes the structure of a table with a target schema. Creates the table if it does not exist or modifies it to match the target schema.

**Parameters:**

| Name      | Type               | Description                                                                 |
|-----------|--------------------|-----------------------------------------------------------------------------|
| `$table`  | `string`           | Table name to verify.                                                       |
| `$column` | `array`            | Associative array of column definitions (e.g., `["id" => "INT UNSIGNED"]`). |
| `$index`  | `array` (optional) | Array of index definitions (e.g., `["PRIMARY KEY (id)"]`).                  |
| `$mapping`| `array` (optional) | Associative array mapping old column names to new ones.                    |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Ensures a valid connection via `connection()`.
2. Retrieves the list of existing tables.
3. Computes a schema hash to detect changes.
4. Creates the table if it does not exist.
5. Compares the existing table structure with the target schema and applies modifications:
   - Adds, modifies, or drops columns.
   - Adds or drops indices.
   - Handles auto-increment columns.
6. Stores the schema hash in the cache to avoid redundant checks.

**Usage Context:**
Used during application setup or updates to ensure the database schema matches the expected structure.

**Example:**
```php
$columns = [
    "id" => "INT UNSIGNED AUTO_INCREMENT",
    "name" => "VARCHAR(255) NOT NULL",
    "created_at" => "TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
];
$indices = [
    "PRIMARY KEY (id)"
];
$success = $db->verify_table("users", $columns, $indices);
if ($success) {
    echo "Table 'users' verified successfully.";
}
```

---

#### `mysql::export_sql($table, $file = NULL)`

**Purpose:**
Exports the SQL definition of a table to a file.

**Parameters:**

| Name     | Type     | Description                                      |
|----------|----------|--------------------------------------------------|
| `$table` | `string` | Table name to export.                            |
| `$file`  | `string` (optional) | Output file path (default: `CMS_DATA_PATH . "#database/$table"`). |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Ensures a valid connection via `connection()`.
2. Retrieves the table definition using `SHOW CREATE TABLE`.
3. Writes the definition to a `.sql` file.

**Usage Context:**
Used for database backups or migrations.

**Example:**
```php
$success = $db->export_sql("users", "/path/to/backup/users");
if ($success) {
    echo "SQL export completed successfully.";
}
```

---

#### `mysql::export_csv($table, $separator = ",", $delimiter = "\"", $file = NULL, $set_fields = FALSE)`

**Purpose:**
Exports table data to a CSV file.

**Parameters:**

| Name         | Type      | Description                                      |
|--------------|-----------|--------------------------------------------------|
| `$table`     | `string`  | Table name to export.                            |
| `$separator` | `string`  | Field separator (default: `","`).                |
| `$delimiter` | `string`  | Text delimiter (default: `"\""`).                |
| `$file`      | `string` (optional) | Output file path (default: `CMS_DATA_PATH . "#database/$table"`). |
| `$set_fields`| `bool`    | Whether to include a header row (default: `FALSE`). |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Ensures a valid connection via `connection()`.
2. Retrieves column names using `SHOW COLUMNS`.
3. Executes a query to format data as CSV using `CONCAT_WS` and `REPLACE`.
4. Writes the data to a `.csv` file.

**Usage Context:**
Used for data exports or backups in a portable format.

**Example:**
```php
$success = $db->export_csv("users", ",", "\"", "/path/to/backup/users", TRUE);
if ($success) {
    echo "CSV export completed successfully.";
}
```

---

#### `mysql::export_excel($table, $file = NULL, $set_fields = FALSE)`

**Purpose:**
Exports table data to an Excel-compatible file (UTF-16LE encoded TSV).

**Parameters:**

| Name         | Type      | Description                                      |
|--------------|-----------|--------------------------------------------------|
| `$table`     | `string`  | Table name to export.                            |
| `$file`      | `string` (optional) | Output file path (default: `CMS_DATA_PATH . "#database/$table"`). |
| `$set_fields`| `bool`    | Whether to include a header row (default: `FALSE`). |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Ensures the `mbstring` extension is loaded.
2. Calls `export_csv()` to generate a temporary TSV file.
3. Converts the TSV file to UTF-16LE encoding and adds a byte order mark (BOM).
4. Renames the file to `.xls`.

**Usage Context:**
Used for generating Excel-compatible exports.

**Example:**
```php
$success = $db->export_excel("users", "/path/to/backup/users", TRUE);
if ($success) {
    echo "Excel export completed successfully.";
}
```

---

#### `mysql::export_html($table, $file = NULL, $set_fields = FALSE)`

**Purpose:**
Exports table data to an HTML file.

**Parameters:**

| Name         | Type      | Description                                      |
|--------------|-----------|--------------------------------------------------|
| `$table`     | `string`  | Table name to export.                            |
| `$file`      | `string` (optional) | Output file path (default: `CMS_DATA_PATH . "#database/$table"`). |
| `$set_fields`| `bool`    | Whether to include a header row (default: `FALSE`). |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Ensures a valid connection via `connection()`.
2. Retrieves column names using `SHOW COLUMNS`.
3. Executes a query to fetch all rows.
4. Generates an HTML file with a table structure.

**Usage Context:**
Used for generating human-readable reports.

**Example:**
```php
$success = $db->export_html("users", "/path/to/backup/users", TRUE);
if ($success) {
    echo "HTML export completed successfully.";
}
```

---

#### `mysql::import_csv($file, $separator = ",", $delimiter = "\"", $table = NULL, $get_fields = FALSE, $ignore_first_row = FALSE, $ignore_existing = TRUE, $mapping = NULL)`

**Purpose:**
Imports data from a CSV file into a table.

**Parameters:**

| Name               | Type               | Description                                                                 |
|--------------------|--------------------|-----------------------------------------------------------------------------|
| `$file`            | `string`           | Path to the CSV file.                                                       |
| `$separator`       | `string`           | Field separator (default: `","`).                                           |
| `$delimiter`       | `string`           | Text delimiter (default: `"\""`).                                           |
| `$table`           | `string` (optional)| Target table name.                                                          |
| `$get_fields`      | `bool`             | Whether to return the fields of the first row (default: `FALSE`).           |
| `$ignore_first_row`| `bool`             | Whether to skip the first row (default: `FALSE`).                           |
| `$ignore_existing` | `bool`             | Whether to use `INSERT IGNORE` (default: `TRUE`).                           |
| `$mapping`         | `array` (optional) | Associative array mapping CSV column indices to table columns.              |

**Return Values:**
- `bool|array`: `TRUE` on success, `FALSE` on failure, or an array of field names if `$get_fields` is `TRUE`.

**Inner Mechanisms:**
1. Ensures a valid connection via `connection()`.
2. Opens the CSV file and parses it using a custom parser (faster than `fgetcsv` for large files).
3. Supports column mapping and batch inserts (100 rows per query).
4. Handles escaped delimiters and line breaks.

**Usage Context:**
Used for importing data from external sources or restoring backups.

**Example:**
```php
$mapping = [
    0 => "id",
    1 => "name",
    2 => "email"
];
$success = $db->import_csv("/path/to/users.csv", ",", "\"", "users", FALSE, TRUE, TRUE, $mapping);
if ($success) {
    echo "CSV import completed successfully.";
}
```

---

#### `mysql::backup()`

**Purpose:**
Creates a backup of all non-backup tables in the database.

**Parameters:**
None.

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Ensures a valid connection via `connection()`.
2. Retrieves the list of tables using `SHOW TABLES`.
3. Exports the SQL definition and CSV data for each non-backup table to `CMS_DATA_PATH . "#database/backup/"`.

**Usage Context:**
Used for creating database backups before performing critical operations.

**Example:**
```php
$success = $db->backup();
if ($success) {
    echo "Database backup completed successfully.";
}
```

---

#### `mysql::restore()`

**Purpose:**
Restores the database from the most recent backup.

**Parameters:**
None.

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Ensures a valid connection via `connection()`.
2. Scans the backup directory for `.sql` files.
3. Creates a backup of existing tables before restoring.
4. Executes the SQL definition and imports CSV data for each table.

**Usage Context:**
Used to restore the database after a failure or during migration.

**Example:**
```php
$success = $db->restore();
if ($success) {
    echo "Database restored successfully.";
}
```

---

#### `mysql::drop_table($table)`

**Purpose:**
Drops a table from the database.

**Parameters:**

| Name     | Type     | Description               |
|----------|----------|---------------------------|
| `$table` | `string` | Table name to drop.       |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
1. Executes a `DROP TABLE` query.

**Usage Context:**
Used to remove tables during cleanup or schema updates.

**Example:**
```php
$success = $db->drop_table("temp_users");
if ($success) {
    echo "Table dropped successfully.";
}
```


<!-- HASH:deb206f340b5b5e736340ec1051ffe84 -->
