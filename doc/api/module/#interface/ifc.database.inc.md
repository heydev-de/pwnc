# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.database.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.database.inc)

- **Version:** `26.6.9.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Database Interface Module (`ifc.database.inc`)

This file provides a comprehensive web-based database management interface for the PWNC Web Platform. It allows developers and administrators to interact with MySQL databases through a visual interface, supporting operations such as table creation, field management, data editing, SQL console execution, and data import/export.

---

## Overview

The interface is structured around a hierarchical object model:
- **Database**: The root level, listing all tables.
- **Table**: Represents a database table, with options to view structure, data, and perform operations.
- **Field**: A column within a table, with type-specific editing capabilities.
- **Index**: An index on a field, with configuration options.

The interface dynamically adapts its display and available actions based on the currently selected object (table, field, or index). It leverages the platform's caching, permission, and form generation utilities to provide a consistent and secure user experience.

---

## Core Functionality

### Message Handling

The interface processes different actions (messages) based on the `CMS_IFC_MESSAGE` parameter. Each message triggers a specific workflow, such as editing a table, creating an index, or importing data.

#### Key Messages and Workflows

| Message               | Purpose                                                                                     |
|-----------------------|---------------------------------------------------------------------------------------------|
| `select`              | Selects a database object (table, field, or index) for viewing or editing.                 |
| `sql_console`         | Provides an SQL console for executing raw SQL queries.                                     |
| `edit_table`          | Displays and allows editing of table data with filtering, sorting, and pagination.         |
| `alter_table`         | Modifies table properties (name, engine, collation, comment).                              |
| `create_table`        | Creates a new table, optionally from a predefined SQL definition file.                     |
| `add_field`           | Adds a new field to a table with type-specific configuration.                              |
| `change_field`        | Modifies an existing field's properties.                                                   |
| `create_index`        | Creates a new index (INDEX, UNIQUE, PRIMARY KEY, FULLTEXT) on selected fields.             |
| `export_definition`   | Exports a table's schema as an SQL file.                                                   |
| `export_table`        | Exports table data in HTML, XLS, or CSV format.                                            |
| `import_table`        | Imports data from a CSV file into a new or existing table.                                 |
| `delete`              | Deletes selected objects (tables, fields, or indexes).                                     |
| `backup`              | Creates a backup of the entire database.                                                   |
| `restore`             | Restores the database from the most recent backup.                                         |
| `maintain`            | Repairs and optimizes all tables in the database.                                          |
| `config`              | Configures database connection settings (host, database name, user, password).             |

---

## Key Functions and Workflows

### SQL Console (`sql_console`)

#### Purpose
Provides an interactive SQL console for executing raw SQL queries against the database. The last executed command is cached for convenience.

#### Parameters
None (uses `ifc_param1` for the SQL query).

#### Return Values
- Displays the result of the query or an error message if execution fails.

#### Inner Mechanisms
1. Retrieves the last executed SQL command from the cache.
2. Displays a textarea for entering SQL queries.
3. Executes the query when the "Execute" button is clicked.
4. Caches the last command permanently for future sessions.
5. Displays the query result or an error message using `log_report()`.

#### Usage Example
```php
// Execute a query to create a new table
$ifc_param1 = "CREATE TABLE users (id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(255))";
```
- The query is entered into the textarea and executed. The result (success or error) is displayed below the console.

---

### Table Editor (`edit_table`)

#### Purpose
Provides a visual interface for viewing, filtering, sorting, and editing table data. Supports pagination, inline editing, and bulk operations.

#### Parameters

| Parameter      | Type     | Description                                                                                 |
|----------------|----------|---------------------------------------------------------------------------------------------|
| `object`       | string   | The table name to edit.                                                                    |
| `row`          | mixed    | The primary key value of the row being edited.                                             |
| `field`        | string   | The field (column) being edited.                                                           |
| `ifc_param1`   | string   | Field name for filtering.                                                                  |
| `ifc_param2`   | string   | Filter operator (e.g., `=`, `LIKE`, `>`).                                                  |
| `ifc_param3`   | string   | Filter value.                                                                              |
| `ifc_param4`   | int      | Number of rows per page (default: 25).                                                     |
| `ifc_param5`   | int      | Number of characters to display per field (default: 25).                                   |
| `ifc_param6`   | string   | Field name for sorting (default: primary key).                                             |
| `ifc_param7`   | mixed    | New value for the field being edited.                                                      |
| `offset`       | int      | Pagination offset.                                                                         |
| `list`         | array    | List of selected rows for bulk operations.                                                 |

#### Return Values
- Displays a table with data, filtering controls, and pagination.
- Updates the table data if edits are submitted.

#### Inner Mechanisms
1. Retrieves the table structure (columns, types, primary key) using `SHOW COLUMNS`.
2. Applies filtering, sorting, and pagination to the data query.
3. Displays the data in a table with inline editing capabilities for each field.
4. Handles updates, additions, and deletions of records.
5. Supports bulk selection and operations (e.g., delete selected rows).

#### Usage Example
```php
// Edit a record in the "users" table
$object = "users";
$row = 1; // Primary key of the row to edit
$field = "name"; // Field to edit
$ifc_param7 = "John Doe"; // New value
```
- The interface displays the "users" table, highlights the row with `id = 1`, and allows editing the "name" field inline.

---

### Table Creation (`create_table`)

#### Purpose
Creates a new table with configurable properties (name, engine, collation, comment). Supports creation from a predefined SQL definition file.

#### Parameters

| Parameter      | Type     | Description                                                                                 |
|----------------|----------|---------------------------------------------------------------------------------------------|
| `ifc_param1`   | string   | Table name.                                                                                 |
| `ifc_param2`   | string   | Storage engine (e.g., `InnoDB`, `MyISAM`).                                                 |
| `ifc_param3`   | string   | Collation (e.g., `utf8mb4_general_ci`).                                                     |
| `ifc_param4`   | string   | Table comment.                                                                              |
| `ifc_param5`   | string   | Path to an SQL definition file (optional).                                                 |

#### Return Values
- Creates the table and displays a success or error message.

#### Inner Mechanisms
1. Validates the table name.
2. If no definition file is provided, creates a minimal table with an auto-incrementing `id` field.
3. If a definition file is provided, executes the SQL from the file and applies additional settings (engine, collation, comment).
4. Displays a form for configuring table properties.

#### Usage Example
```php
// Create a table named "products" with InnoDB engine and utf8mb4 collation
$ifc_param1 = "products";
$ifc_param2 = "innodb";
$ifc_param3 = "utf8mb4_general_ci";
$ifc_param4 = "Stores product information";
```
- The interface creates the "products" table with the specified settings.

---

### Field Management (`add_field`, `change_field`)

#### Purpose
Adds a new field to a table or modifies an existing field's properties. Supports all MySQL data types with type-specific configuration options.

#### Parameters

| Parameter      | Type     | Description                                                                                 |
|----------------|----------|---------------------------------------------------------------------------------------------|
| `object`       | string   | The table name (and field name for `change_field`, e.g., `table.field`).                   |
| `ifc_param1`   | string   | Field type (e.g., `int`, `varchar`, `enum`).                                                |
| `ifc_param2`   | string   | Field name.                                                                                 |
| `ifc_param3`   | bool     | Whether the field allows NULL values.                                                       |
| `ifc_param4`   | mixed    | Default value for the field.                                                                |
| `ifc_param5`   | int      | Length for `bit` fields.                                                                    |
| `ifc_param6`   | int      | Length for `char` or `varchar` fields.                                                      |
| `ifc_param7`   | string   | Collation for `char`, `varchar`, `text`, or `enum` fields.                                  |
| `ifc_param8`   | int      | Length for integer fields.                                                                  |
| `ifc_param9`   | string   | Attributes for integer fields (e.g., `UNSIGNED`, `UNSIGNED ZEROFILL`).                     |
| `ifc_param10`  | bool     | Whether the field is auto-incrementing.                                                     |
| `ifc_param11`  | string   | Collation for `text` fields.                                                                |
| `ifc_param12`  | int      | Length for `decimal`, `double`, or `float` fields.                                          |
| `ifc_param13`  | int      | Number of decimals for `decimal`, `double`, or `float` fields.                              |
| `ifc_param14`  | string   | Attributes for `decimal`, `double`, or `float` fields.                                      |
| `ifc_param15`  | int      | Length for `binary` or `varbinary` fields.                                                  |
| `ifc_param16`  | int      | Length for `year` fields (2 or 4 digits).                                                   |
| `ifc_param17`  | string   | Values for `enum` or `set` fields (one per line).                                           |
| `ifc_param18`  | string   | Collation for `enum` or `set` fields.                                                       |
| `ifc_param19`  | string   | Field comment.                                                                              |

#### Return Values
- Adds or modifies the field and displays a success or error message.

#### Inner Mechanisms
1. For `change_field`, retrieves the existing field properties using `SHOW FULL COLUMNS`.
2. Displays a form with type-specific configuration options.
3. Constructs and executes an `ALTER TABLE` query to add or modify the field.
4. Handles type-specific validation and escaping.

#### Usage Example
```php
// Add a "price" field of type DECIMAL(10,2) to the "products" table
$object = "products";
$ifc_param1 = "decimal";
$ifc_param2 = "price";
$ifc_param12 = 10; // Length
$ifc_param13 = 2;  // Decimals
$ifc_param14 = "UNSIGNED"; // Attributes
```
- The interface adds the "price" field to the "products" table with the specified configuration.

---

### Index Creation (`create_index`)

#### Purpose
Creates a new index (INDEX, UNIQUE, PRIMARY KEY, or FULLTEXT) on one or more fields of a table.

#### Parameters

| Parameter      | Type     | Description                                                                                 |
|----------------|----------|---------------------------------------------------------------------------------------------|
| `object`       | string   | The table name (and optional field name, e.g., `table.field`).                             |
| `ifc_param1`   | string   | Index type (`INDEX`, `UNIQUE`, `PRIMARY KEY`, `FULLTEXT`).                                 |
| `ifc_param2`   | string   | Index name (optional for PRIMARY KEY).                                                     |
| `ifc_param3`   | string   | Index format (`BTREE` or `HASH`).                                                          |
| `list`         | array    | List of selected fields to include in the index.                                           |
| `length`       | array    | Length for each field in the index (optional).                                             |

#### Return Values
- Creates the index and displays a success or error message.

#### Inner Mechanisms
1. Retrieves the list of fields in the table using `SHOW COLUMNS`.
2. Displays a form for selecting fields and configuring the index.
3. Constructs and executes an `ALTER TABLE` query to create the index.
4. Supports partial indexes by allowing a length to be specified for each field.

#### Usage Example
```php
// Create a UNIQUE index named "idx_email" on the "email" field of the "users" table
$object = "users";
$ifc_param1 = "UNIQUE";
$ifc_param2 = "idx_email";
$list = ["email"];
```
- The interface creates a unique index on the "email" field.

---

### Data Export (`export_table`)

#### Purpose
Exports table data in HTML, XLS (Excel), or CSV format. Supports customization of CSV settings (separator, delimiter).

#### Parameters

| Parameter      | Type     | Description                                                                                 |
|----------------|----------|---------------------------------------------------------------------------------------------|
| `object`       | string   | The table name to export.                                                                   |
| `ifc_param1`   | string   | File name for the exported data (default: `table_YYYY-MM-DD`).                             |
| `ifc_param2`   | int      | Export format (`0` = HTML, `1` = XLS, `2` = CSV).                                          |
| `ifc_param3`   | bool     | Whether to export all fields (default: `TRUE`).                                            |
| `ifc_param4`   | string   | Separator for CSV export (e.g., `,`, `;`, `\t`).                                           |
| `ifc_param5`   | string   | Delimiter for CSV export (e.g., `"`, `'`).                                                 |

#### Return Values
- Generates the export file and provides a download link.

#### Inner Mechanisms
1. Displays a form for selecting the export format and settings.
2. Calls the appropriate export method (`export_html`, `export_excel`, or `export_csv`) from the `mysql` class.
3. Provides a download link for the generated file.

#### Usage Example
```php
// Export the "users" table as a CSV file with semicolon separator
$object = "users";
$ifc_param2 = 2; // CSV
$ifc_param4 = ";"; // Separator
```
- The interface exports the "users" table as a CSV file with semicolon-separated values.

---

### Data Import (`import_table`)

#### Purpose
Imports data from a CSV file into a new or existing table. Supports field mapping and configuration of CSV settings (separator, delimiter).

#### Parameters

| Parameter              | Type     | Description                                                                                 |
|------------------------|----------|---------------------------------------------------------------------------------------------|
| `object`               | string   | The table name to import into (optional for new tables).                                   |
| `ifc_param1`           | string   | Path to the CSV file.                                                                      |
| `ifc_param2`           | string   | Separator for CSV import (e.g., `,`, `;`, `\t`).                                           |
| `ifc_param3`           | string   | Delimiter for CSV import (e.g., `"`, `'`).                                                 |
| `ifc_param4`           | bool     | Whether the first row contains field names.                                                |
| `ifc_param5`           | bool     | Whether to ignore existing records (for existing tables).                                  |
| `ignore_first_row`     | bool     | Alias for `ifc_param4`.                                                                     |
| `ignore_existing`      | bool     | Alias for `ifc_param5`.                                                                     |
| `mapping`              | array    | Field mapping (column number to field name).                                               |

#### Return Values
- Imports the data and displays a success or error message.

#### Inner Mechanisms
1. Displays a form for selecting the CSV file and configuring settings.
2. For existing tables, displays a field mapping interface to match CSV columns to table fields.
3. For new tables, creates a table with BLOB fields and imports the data.
4. Calls the `import_csv` method from the `mysql` class to perform the import.

#### Usage Example
```php
// Import data from "users.csv" into the "users" table, ignoring the first row
$object = "users";
$ifc_param1 = "users.csv";
$ifc_param2 = ",";
$ifc_param3 = "\"";
$ifc_param4 = TRUE; // First row contains field names
```
- The interface imports the data from "users.csv" into the "users" table, using the first row as field names.

---

### Database Configuration (`config`)

#### Purpose
Configures the database connection settings (host, database name, user, password). Updates the system configuration file.

#### Parameters

| Parameter      | Type     | Description                                                                                 |
|----------------|----------|---------------------------------------------------------------------------------------------|
| `ifc_param1`   | string   | Database host (e.g., `localhost`).                                                          |
| `ifc_param2`   | string   | Database name.                                                                              |
| `ifc_param3`   | string   | Database user.                                                                              |
| `ifc_param4`   | string   | Database password.                                                                          |

#### Return Values
- Updates the configuration and displays a success or error message.

#### Inner Mechanisms
1. Displays a form for entering database connection settings.
2. Updates the system configuration using the `system` class.
3. Reinitializes the database connection with the new settings.

#### Usage Example
```php
// Update database configuration
$ifc_param1 = "localhost";
$ifc_param2 = "pwnc_db";
$ifc_param3 = "pwnc_user";
$ifc_param4 = "secure_password";
```
- The interface updates the database configuration and reinitializes the connection.

---

## Helper Functions

### `bitstring($value, $length)`

#### Purpose
Converts a bit field value to a binary string representation.

#### Parameters

| Parameter | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `$value`  | string | The bit field value.                 |
| `$length` | int    | The length of the bit field.         |

#### Return Values
- Returns a binary string representation of the bit field.

#### Usage Example
```php
$binaryString = bitstring("\x05", 8); // Returns "00000101"
```

---

### `format_bytesize($bytes)`

#### Purpose
Formats a byte size into a human-readable string (e.g., `1.2 KB`, `3.4 MB`).

#### Parameters

| Parameter | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `$bytes`  | int    | The size in bytes.                   |

#### Return Values
- Returns a formatted string representing the byte size.

#### Usage Example
```php
$formattedSize = format_bytesize(1024); // Returns "1.0 KB"
```

---

## Usage Context

This interface is designed for:
- **Database Administrators**: Managing database structure, data, and performance.
- **Developers**: Debugging and testing SQL queries, inspecting table structures, and importing/exporting data.
- **Content Managers**: Editing table data directly without writing SQL.

### Typical Scenarios
1. **Data Management**:
   - View and edit records in a table using the `edit_table` interface.
   - Filter, sort, and paginate through large datasets.
2. **Schema Management**:
   - Create, alter, or delete tables using `create_table` and `alter_table`.
   - Add, modify, or delete fields using `add_field` and `change_field`.
   - Create indexes to optimize query performance.
3. **Data Migration**:
   - Export data to CSV, XLS, or HTML using `export_table`.
   - Import data from CSV files using `import_table`.
4. **Database Maintenance**:
   - Backup and restore the database using `backup` and `restore`.
   - Repair and optimize tables using `maintain`.
5. **SQL Execution**:
   - Execute raw SQL queries using the `sql_console`.

---

## Example Workflow: Creating a Table and Adding Data

1. **Create a Table**:
   - Navigate to the database interface.
   - Click "Create Table" and enter the table name (e.g., `products`).
   - Select the engine (e.g., `InnoDB`), collation (e.g., `utf8mb4_general_ci`), and add a comment.
   - Submit the form to create the table.

2. **Add Fields**:
   - Select the `products` table.
   - Click "Create Field" and configure a field (e.g., `name` of type `VARCHAR(255)`).
   - Add additional fields (e.g., `price` of type `DECIMAL(10,2)`).
   - Submit the form to add the fields.

3. **Edit Data**:
   - Click "Edit Table" to view the `products` table.
   - Add records by clicking "Add Record" and filling in the field values.
   - Edit existing records by clicking on a field and entering a new value.

4. **Export Data**:
   - Click "Export Data" and select the export format (e.g., CSV).
   - Configure CSV settings (e.g., separator `,`, delimiter `"`).
   - Download the exported file.

---

## Error Handling

The interface provides detailed error messages for all operations, including:
- MySQL errors (e.g., syntax errors, constraint violations).
- Validation errors (e.g., invalid table names, missing required fields).
- File operation errors (e.g., missing import files, permission issues).

Error messages are displayed at the top of the interface and are escaped using the platform's `x()` function to prevent XSS attacks.


<!-- HASH:41286c9c98546aa0b72a698a2a01ed77 -->
