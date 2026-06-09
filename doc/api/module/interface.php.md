# PWNC API Documentation

[← Index](../README.md) | [`module/interface.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/interface.php)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Module Interface Management (`module/interface.php`)

This file serves as the central entry point for PWNC's modular interface system. It dynamically loads, orders, and executes interface modules based on user permissions and configuration. The file handles:

1. **Module Discovery**: Scans the interface directory for valid modules (files matching `ifc.*.inc`).
2. **Permission Validation**: Checks if the current user has access to each module.
3. **Ordering**: Applies a custom order defined in an `order` file, with fallback to alphabetical sorting.
4. **Execution**: Loads the selected module or falls back to a default interface if none is specified.

---

### Constants and Paths

| Name | Value | Description |
|------|-------|-------------|
| `CMS_INTERFACE_PATH` | `../interface/` | Base directory where interface modules are stored. |
| `CMS_IFC_PAGE` | Dynamic (from request/cookie) | Currently selected interface module identifier. |
| `CMS_IFC_OPTION` | Dynamic (from request) | Additional options for the module (e.g., `external`). |

---

### Functions

---

#### `ifc_module_list()`

**Purpose**:
Generates an ordered list of available interface modules, filtered by user permissions and sorted according to a custom order file.

**Parameters**:
None.

**Return Values**:

| Type | Description |
|------|-------------|
| `array` | Associative array of module identifiers (keys) and their display names (values). Modules not listed in the `order` file are appended alphabetically. Separators (`-`) are inserted as visual dividers. |

**Inner Mechanisms**:
1. **Directory Scanning**: Uses `opendir()` and `readdir()` to iterate over files in `CMS_INTERFACE_PATH`.
2. **File Validation**: Checks if each file:
   - Is a regular file.
   - Matches the pattern `ifc.*.inc`.
   - Passes permission validation via `cms_permission()`.
3. **Display Name Resolution**: Uses a constant (`CMS_L_IFC_<MODULE>`) if defined; otherwise, falls back to the module identifier.
4. **Ordering**:
   - Reads the `order` file (if it exists) line by line.
   - Processes each line to:
     - Insert a separator (`-`) if the line is `-`.
     - Include the module if it exists in the discovered list.
   - Appends remaining modules not listed in `order` to the end of the array.

**Usage Context**:
- Called during interface initialization to populate the module navigation menu.
- Used by `ifc_default()` to render the module selection interface.

**Example**:
```php
$modules = ifc_module_list();
// Output: ["dashboard" => "Dashboard", "content" => "Content Management", "-" => "-", "settings" => "Settings"]
```

---

### Execution Flow

1. **Module List Generation**:
   ```php
   $ifc_page = ifc_module_list();
   ```
   - Retrieves the list of available modules.

2. **Module Selection**:
   - If `CMS_IFC_PAGE` is set and the module exists in `$ifc_page`:
     - Sets a cookie to persist the selection.
     - Skips loading the module list if `external` is in `CMS_IFC_OPTION`.
     - Requires the module file (e.g., `ifc.dashboard.inc`).
   - Otherwise:
     - Calls `ifc_default($ifc_page)` to render a module selection interface.

**Example**:
```php
// URL: /interface?page=content
// CMS_IFC_PAGE = "content"
// CMS_IFC_OPTION = ""
// Result: Loads ifc.content.inc
```

---

### Key Dependencies

1. **`cms_load("ifc", TRUE)`**:
   - Loads the core interface library (`ifc.inc`), which provides `ifc_default()` and other shared functions.

2. **`cms_permission($module, TRUE, NULL)`**:
   - Validates if the current user has access to the module. Returns `TRUE` if allowed.

3. **`cms_set_cookie(["cms_ifc_page" => CMS_IFC_PAGE])`**:
   - Persists the selected module in a cookie for future requests.

4. **`unique_id()`**:
   - Generates a unique key for separators in the module list.

---

### Error Handling

- If the `order` file cannot be read, the function falls back to alphabetical sorting.
- Invalid or inaccessible modules are silently excluded from the list.
- Missing modules in `CMS_IFC_PAGE` trigger the default interface.


<!-- HASH:54aa4baa80578d117c5b4a34655f442b -->
