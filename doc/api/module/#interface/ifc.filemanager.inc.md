# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.filemanager.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.filemanager.inc)

- **Version:** `26.6.9.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## File Manager Interface (`ifc.filemanager.inc`)

The `ifc.filemanager.inc` file provides the **File Manager** interface for the PWNC Web Platform. It enables users to navigate, manage, and manipulate files and directories within the CMS root path. The interface supports operations such as file uploads, directory creation, file editing, compression/decompression, copying, moving, and deletion. It also integrates with other modules (e.g., `directory`) for advanced functionality.

---

## **Core Functionality**
The file manager operates as an **interface controller** for the `filemanager` module. It handles:
- **Navigation**: Browse directories and files.
- **File Operations**: Create, edit, rename, copy, move, delete, and transfer files.
- **Permissions**: Modify file access permissions (read/write/execute for owner/group/all).
- **Compression**: Compress/decompress files using Gzip.
- **Uploads/Downloads**: Upload files via HTTP or download selected files.
- **State Management**: Persist the current directory and selected files across requests.

---

## **Initialization & State Management**
### **Dependencies & Permissions**
| Library | Purpose |
|---------|---------|
| `filemanager` | Core file operations (e.g., `filemanager_copy`, `filemanager_delete`). |
| `flexview` | Display directory contents in a flexible table view. |
| `plist` | Manage persistent lists (e.g., recently edited files). |

**Permission Check**:
- Requires `CMS_L_ACCESS` permission to interact with the file manager.

### **State Variables**
| Variable | Type | Default | Description |
|----------|------|---------|-------------|
| `$object` | `string` | `CMS_ROOT_PATH` | Current directory or file path. Validated to ensure it exists and is within `CMS_ROOT_PATH`. |
| `$list` | `array` | `[]` | List of selected files/directories. |
| `$target` | `string` | `NULL` | Target directory for copy/move operations. |
| `$status` | `string` | `NULL` | Tracks pending operations (`"copy"` or `"cut"`). |

---

## **Message Handling**
The file manager processes **interface messages** (`CMS_IFC_MESSAGE`) to perform actions. Each message triggers a specific operation and updates the interface state.

### **`select`**
**Purpose**: Navigate to a directory or select a file.
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Target path. If empty, resets to `CMS_ROOT_PATH`. |

**Usage Example**:
```php
// Navigate to "/data/" directory
ifc_post("select", CMS_ROOT_PATH . "data/");
```

---

### **`save`**
**Purpose**: Rename a file/directory and update permissions.
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | New name/path. |
| `$ifc_param2`–`$ifc_param10` | `bool` | Permission flags (owner/group/all read/write/execute). |

**Inner Mechanism**:
1. Constructs the new path by combining the parent directory and new basename.
2. Updates permissions using `chmod()`.
3. Renames the file/directory using `rename()`.
4. Updates `$object` and `$list` on success.

**Usage Example**:
```php
// Rename "old.txt" to "new.txt" and set permissions to 0755
ifc_post("save", "new.txt", true, false, false, true, false, false, true, false);
```

---

### **`upload`**
**Purpose**: Upload files to the current directory.
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_file1` | `string` or `array` | Uploaded file(s) (from `$_FILES`). |
| `$ifc_file1_name` | `string` or `array` | Original filenames. |

**Inner Mechanism**:
1. Maps uploaded files to their original names.
2. Moves files to the target directory using `move_uploaded_file()`.
3. Updates `$object` to the first uploaded file and clears `$list`.

**Usage Example**:
```php
// Upload a file via HTML form
<form method="post" enctype="multipart/form-data">
    <input type="file" name="ifc_file1[]" multiple>
    <button type="submit" name="ifc_message" value="upload">Upload</button>
</form>
```

---

### **`mkdir` / `_mkdir`**
**Purpose**: Create a new directory.
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | Directory name. |

**Inner Mechanism**:
1. Uses `mkpath()` (a helper function) to create the directory.
2. Updates `$object` and `$list` on success.

**Usage Example**:
```php
// Create a directory named "backup" in the current path
ifc_post("mkdir", "backup");
```

---

### **`mkfile` / `_mkfile`**
**Purpose**: Create a new file with optional content.
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | Filename. |
| `$ifc_param2` | `string` | File content (optional). |

**Inner Mechanism**:
1. Opens the file in binary write mode (`"wb"`).
2. Locks the file (`flock`) and writes content.
3. Updates `$object` and `$list` on success.

**Usage Example**:
```php
// Create "notes.txt" with content "Hello, PWNC!"
ifc_post("mkfile", "notes.txt", "Hello, PWNC!");
```

---

### **`edit` / `_edit`**
**Purpose**: Edit a file's content.
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | New file content. |

**Inner Mechanism**:
1. Opens the file in binary write mode (`"wb"`).
2. Locks the file and overwrites its content.
3. Updates the "recently edited" list in `plist`.

**Usage Example**:
```php
// Edit "config.php" via the interface
ifc_post("edit", "config.php", "<?php\n\$config = [];");
```

---

### **`compress` / `decompress`**
**Purpose**: Compress/decompress files using Gzip.
**Inner Mechanism**:
1. Iterates over `$list` and applies `filemanager_gzcompress()` or `filemanager_gzdecompress()`.
2. Updates `$list` with the new paths on success.

**Usage Example**:
```php
// Compress all selected files
ifc_post("compress");
```

---

### **`copy` / `cut` / `target` / `copy_insert` / `cut_insert`**
**Purpose**: Copy or move files/directories to a target location.
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | Target directory path. |

**Inner Mechanism**:
1. **`copy`/`cut`**: Sets `$status` to track the operation.
2. **`target`**: Sets `$target` to the selected directory.
3. **`copy_insert`/`cut_insert`**:
   - Resolves naming conflicts by appending underscores (e.g., `file_` → `file__`).
   - Uses `filemanager_copy()` or `rename()` to perform the operation.
   - Updates `$object` and `$list` on success.

**Usage Example**:
```php
// Copy selected files to "/backup/"
ifc_post("copy");
ifc_post("target", CMS_ROOT_PATH . "backup/");
ifc_post("copy_insert");
```

---

### **`download`**
**Purpose**: Download a file.
**Inner Mechanism**:
- Uses the `download()` helper function to send the file to the browser.

**Usage Example**:
```php
// Download "report.pdf"
ifc_post("download", CMS_ROOT_PATH . "report.pdf");
```

---

### **`delete`**
**Purpose**: Delete selected files/directories.
**Inner Mechanism**:
- Uses `filemanager_delete()` to recursively delete items in `$list`.

**Usage Example**:
```php
// Delete all selected files
ifc_post("delete");
```

---

### **`transfer` / `_transfer`**
**Purpose**: Download a file from a remote URL to the current directory.
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | Remote URL. |
| `$ifc_param2` | `string` | Local filename (optional). |

**Inner Mechanism**:
- Uses `retrieve_file()` to fetch the remote file and save it locally.

**Usage Example**:
```php
// Download a file from "https://example.com/image.jpg" to the current directory
ifc_post("transfer", "https://example.com/image.jpg", "image.jpg");
```

---

## **Main Display**
The interface renders a **three-column layout**:
1. **Directory Browser**: Lists files/directories with checkboxes for selection.
2. **Target Panel** (if `$status` is set): Shows the target directory for copy/move operations.
3. **File Details**: Displays metadata (permissions, size, timestamps) and action buttons.

### **Key UI Components**
| Component | Description |
|-----------|-------------|
| **Path Bar** | Shows the current path and allows manual URL entry. |
| **Shortcuts** | Quick links to common directories (e.g., `/design/`, `/data/`). |
| **Recently Edited** | Displays the last 7 edited files (from `plist`). |
| **FlexView Table** | Lists directory contents with icons (based on MIME type). |
| **Selection Controls** | Buttons to select all/invert/none. |
| **Permissions Editor** | Checkboxes to modify read/write/execute permissions. |
| **File Upload** | Multi-file upload form. |

---

## **Helper Functions**
### **`filemanager_flexview()`**
**Purpose**: Generates a `flexview` instance for directory listing.
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$_` | `mixed` | Unused. |
| `$path` | `string` | Directory path. |

**Return**: `flexview` object configured for file browsing.

---

### **`filemanager_flexview_directory()`**
**Purpose**: Generates a `flexview` instance for directory selection (target panel).
**Parameters**:
| Parameter | Type | Description |
|-----------|------|-------------|
| `$_` | `mixed` | Unused. |
| `$path` | `string` | Directory path. |

**Return**: `flexview` object configured for directory selection.

---

## **Usage Example: Full Workflow**
```php
// 1. Navigate to "/data/"
ifc_post("select", CMS_ROOT_PATH . "data/");

// 2. Create a new directory
ifc_post("mkdir", "reports");

// 3. Upload a file
ifc_post("upload", $_FILES["ifc_file1"]);

// 4. Edit the uploaded file
ifc_post("edit", "data/reports/report.txt", "Quarterly Report\n...");

// 5. Copy the file to "/backup/"
ifc_post("copy");
ifc_post("target", CMS_ROOT_PATH . "backup/");
ifc_post("copy_insert");

// 6. Download the file
ifc_post("download", CMS_ROOT_PATH . "data/reports/report.txt");
```

---

## **Security Considerations**
1. **Path Validation**: All paths are validated to ensure they are within `CMS_ROOT_PATH`.
2. **Permissions**: File operations respect Unix permissions and require write access.
3. **CSRF Protection**: Interface messages use `ifc_post()` to prevent CSRF attacks.
4. **Escaping**: Output is escaped using `x()` (XML) and `q()` (JavaScript) to prevent XSS.


<!-- HASH:b6eb9bd6936d08fcab8ed3700d0463fa -->
