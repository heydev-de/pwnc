# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/ice.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/ice.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## File System Utilities (`ice.inc`)

This file provides secure wrappers for PHP's native file system functions. All functions enforce path safety by rejecting directory traversal attempts (e.g., `../` or `./`) via `ice_check_path()`. Most wrappers preserve the original function signatures while adding security checks and automatic permission management.

---

## Functions

### `ice_check_path($path)`
Validates a filesystem path to prevent directory traversal attacks.

| Parameter | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `$path`   | string | Filesystem path to validate.         |

**Mechanism:**
- Uses regex to detect sequences like `./`, `../`, or trailing/leading dots.
- Terminates script execution (`exit()`) if traversal is detected.

**Usage Context:**
- Called internally by all filesystem wrappers before delegating to native PHP functions.
- Should not be called directly in application code.

---

### `chgrp($filename, $group, ...$args)`
Changes the group ownership of a file or directory.

| Parameter  | Type            | Description                          |
|------------|-----------------|--------------------------------------|
| `$filename`| string          | Target file/directory path.          |
| `$group`   | string\|integer | Group name or ID.                    |
| `...$args` | mixed           | Additional arguments for `chgrp()`.  |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Mechanism:**
1. Validates `$filename` via `ice_check_path()`.
2. Delegates to PHP's native `chgrp()`.

**Example:**
```php
// Set group ownership to 'www-data'
\cms\chgrp('/var/www/uploads/image.jpg', 'www-data');
```

---

### `chmod($filename, $mode, ...$args)`
Changes the file mode (permissions) of a file or directory.

| Parameter  | Type            | Description                          |
|------------|-----------------|--------------------------------------|
| `$filename`| string          | Target file/directory path.          |
| `$mode`    | integer         | Permission mode (e.g., `0755`).      |
| `...$args` | mixed           | Additional arguments for `chmod()`.  |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Mechanism:**
1. Validates `$filename` via `ice_check_path()`.
2. Delegates to PHP's native `chmod()`.

**Example:**
```php
// Set directory permissions to 0755
\cms\chmod('/var/www/uploads', 0755);
```

---

### `chown($filename, $user, ...$args)`
Changes the owner of a file or directory.

| Parameter  | Type            | Description                          |
|------------|-----------------|--------------------------------------|
| `$filename`| string          | Target file/directory path.          |
| `$user`    | string\|integer | User name or ID.                     |
| `...$args` | mixed           | Additional arguments for `chown()`.  |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Mechanism:**
1. Validates `$filename` via `ice_check_path()`.
2. Delegates to PHP's native `chown()`.

**Example:**
```php
// Set ownership to user 'www-data'
\cms\chown('/var/www/uploads/image.jpg', 'www-data');
```

---

### `copy($source, $dest, ...$args)`
Copies a file while preserving permissions.

| Parameter  | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$source`  | string | Source file path.                    |
| `$dest`    | string | Destination file path.               |
| `...$args` | mixed  | Additional arguments for `copy()`.   |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Mechanism:**
1. Validates `$source` and `$dest` via `ice_check_path()`.
2. Copies the file using PHP's native `copy()`.
3. If successful, applies the source file's permissions to the destination (or `0666`/`0644` if source is not a file).

**Example:**
```php
// Copy a file and preserve permissions
\cms\copy('/var/www/uploads/image.jpg', '/var/www/backups/image.jpg');
```

---

### `file_get_contents($filename, ...$args)`
Reads a file into a string.

| Parameter  | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$filename`| string | File path to read.                   |
| `...$args` | mixed  | Additional arguments for `file_get_contents()`. |

**Return Value:**
- `string|false`: File contents or `FALSE` on failure.

**Mechanism:**
1. Validates `$filename` via `ice_check_path()`.
2. Delegates to PHP's native `file_get_contents()`.

**Example:**
```php
// Read a configuration file
$config = \cms\file_get_contents('/etc/app/config.json');
```

---

### `file_put_contents($filename, $data, ...$args)`
Writes data to a file and sets default permissions.

| Parameter  | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$filename`| string | Target file path.                    |
| `$data`    | mixed  | Data to write.                       |
| `...$args` | mixed  | Additional arguments for `file_put_contents()`. |

**Return Value:**
- `int|false`: Number of bytes written or `FALSE` on failure.

**Mechanism:**
1. Validates `$filename` via `ice_check_path()`.
2. Writes data using PHP's native `file_put_contents()`.
3. If the file is newly created, sets permissions to `0666` (Apache) or `0644` (CGI).

**Example:**
```php
// Write data to a file with default permissions
\cms\file_put_contents('/var/www/logs/app.log', 'Log entry');
```

---

### `file($filename, ...$args)`
Reads a file into an array of lines.

| Parameter  | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$filename`| string | File path to read.                   |
| `...$args` | mixed  | Additional arguments for `file()`.   |

**Return Value:**
- `array|false`: Array of file lines or `FALSE` on failure.

**Mechanism:**
1. Validates `$filename` via `ice_check_path()`.
2. Delegates to PHP's native `file()`.

**Example:**
```php
// Read a CSV file into an array
$rows = \cms\file('/var/www/data/import.csv');
```

---

### `fopen($filename, $mode, ...$args)`
Opens a file or URL and sets default permissions for new files.

| Parameter  | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$filename`| string | File path or URL.                    |
| `$mode`    | string | File mode (e.g., `'r'`, `'w'`).      |
| `...$args` | mixed  | Additional arguments for `fopen()`.  |

**Return Value:**
- `resource|false`: File handle or `FALSE` on failure.

**Mechanism:**
1. Validates `$filename` via `ice_check_path()`.
2. Opens the file using PHP's native `fopen()`.
3. If the file is newly created, sets permissions to `0666` (Apache) or `0644` (CGI).

**Example:**
```php
// Open a file for writing
$handle = \cms\fopen('/var/www/logs/app.log', 'a');
fwrite($handle, 'Log entry');
fclose($handle);
```

---

### `mkdir($pathname, ...$args)`
Creates a directory with automatic permission and `.htaccess` management.

| Parameter  | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$pathname`| string | Directory path to create.            |
| `...$args` | mixed  | Additional arguments for `mkdir()`.  |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Mechanism:**
1. Validates `$pathname` via `ice_check_path()`.
2. Determines permissions:
   - `0777` for Apache.
   - `0700` for protected directories (names starting with `#` or `!`).
   - `0755` otherwise.
3. Creates the directory using PHP's native `mkdir()`.
4. If the directory is protected:
   - Creates a `.htaccess` file with `Deny from all`.
   - Adds `Options +ExecCGI` if the directory name starts with `!`.

**Example:**
```php
// Create a protected directory
\cms\mkdir('/var/www/uploads/#private');

// Create an executable directory
\cms\mkdir('/var/www/cgi-bin/!scripts');
```

---

### `move_uploaded_file($filename, $destination, ...$args)`
Moves an uploaded file to a new location and sets default permissions.

| Parameter      | Type   | Description                          |
|----------------|--------|--------------------------------------|
| `$filename`    | string | Temporary uploaded file path.        |
| `$destination` | string | Target file path.                    |
| `...$args`     | mixed  | Additional arguments for `move_uploaded_file()`. |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Mechanism:**
1. Validates `$filename` and `$destination` via `ice_check_path()`.
2. Moves the file using PHP's native `move_uploaded_file()`.
3. Sets permissions to `0666` (Apache) or `0644` (CGI).

**Example:**
```php
// Handle file upload
if (\cms\move_uploaded_file($_FILES['file']['tmp_name'], '/var/www/uploads/image.jpg')) {
    echo 'File uploaded successfully!';
}
```

---

### `opendir($path, ...$args)`
Opens a directory handle.

| Parameter  | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$path`    | string | Directory path to open.              |
| `...$args` | mixed  | Additional arguments for `opendir()`.|

**Return Value:**
- `resource|false`: Directory handle or `FALSE` on failure.

**Mechanism:**
1. Validates `$path` via `ice_check_path()`.
2. Delegates to PHP's native `opendir()`.

**Example:**
```php
// List files in a directory
$handle = \cms\opendir('/var/www/uploads');
while (($file = readdir($handle)) !== false) {
    echo $file . "\n";
}
closedir($handle);
```

---

### `readfile($filename, ...$args)`
Outputs a file directly to the browser.

| Parameter  | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$filename`| string | File path to read.                   |
| `...$args` | mixed  | Additional arguments for `readfile()`. |

**Return Value:**
- `int|false`: Number of bytes read or `FALSE` on failure.

**Mechanism:**
1. Validates `$filename` via `ice_check_path()`.
2. Delegates to PHP's native `readfile()`.

**Example:**
```php
// Serve a static file
\cms\readfile('/var/www/downloads/manual.pdf');
```

---

### `rename($oldname, $newname, ...$args)`
Renames or moves a file or directory.

| Parameter  | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$oldname` | string | Current file/directory path.         |
| `$newname` | string | New file/directory path.             |
| `...$args` | mixed  | Additional arguments for `rename()`. |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Mechanism:**
1. Validates `$oldname` and `$newname` via `ice_check_path()`.
2. Delegates to PHP's native `rename()`.

**Example:**
```php
// Rename a file
\cms\rename('/var/www/uploads/old.jpg', '/var/www/uploads/new.jpg');
```

---

### `rmdir($dirname, ...$args)`
Removes an empty directory.

| Parameter  | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$dirname` | string | Directory path to remove.            |
| `...$args` | mixed  | Additional arguments for `rmdir()`.  |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Mechanism:**
1. Validates `$dirname` via `ice_check_path()`.
2. Delegates to PHP's native `rmdir()`.

**Example:**
```php
// Remove an empty directory
\cms\rmdir('/var/www/uploads/temp');
```

---

### `unlink($filename, ...$args)`
Deletes a file.

| Parameter  | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$filename`| string | File path to delete.                 |
| `...$args` | mixed  | Additional arguments for `unlink()`. |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Mechanism:**
1. Validates `$filename` via `ice_check_path()`.
2. Delegates to PHP's native `unlink()`.

**Example:**
```php
// Delete a file
\cms\unlink('/var/www/uploads/old.jpg');
```


<!-- HASH:954567b3fd664c4842fbe32adc862c0b -->
