# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.profile.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.profile.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Profile Interface Module (`ifc.profile.inc`)

This file implements the **Profile Management Interface** for the PWNC Web Platform. It provides a complete user interface for:
- **Viewing, creating, editing, and deleting** user profiles
- **Configuring** which profile fields are visible, editable, or required
- **Searching and filtering** profiles with pagination
- **Selecting profiles** for use in other modules (via `CMS_IFC_SELECT`)

The interface is permission-aware, distinguishing between regular users (read-only) and operators (full CRUD + configuration).

---

## Core Workflow

### Initialization
1. **Loads the `profile` library** – Fails if unavailable
2. **Checks permissions** – Requires at least `CMS_L_ACCESS`; operators get `CMS_L_OPERATOR`
3. **Instantiates the `profile` object** – If disabled, the interface deactivates
4. **Initializes UI state variables** – Filtering, sorting, grouping, pagination

---

## Interface Messages & Handlers

The interface reacts to messages sent via `CMS_IFC_MESSAGE`. Each message triggers a specific action.

| Message | Purpose | Operator-Only? |
|---------|---------|----------------|
| `add` | Creates a new profile | ✅ |
| `delete` | Deletes selected profiles | ✅ |
| `display` | Shows a profile in read-only mode | ❌ |
| `display_save` | Saves changes to a profile | ✅ |
| `config_base` | Configures base fields (name, email, etc.) | ✅ |
| `config_custom` | Configures custom fields (1–20) | ✅ |
| `sql_order` | Changes sort column/order | ❌ |
| `page` | Changes pagination page | ❌ |

---

### `add`
**Purpose**: Creates a new empty profile.
**Parameters**: None (uses a fresh `profile_data` object).
**Returns**: `CMS_MSG_DONE` on success, `CMS_MSG_ERROR` on failure.
**Mechanism**:
- Instantiates a new `profile_data` object
- Calls `$_profile->add($profile_data)`
- On success, sets `$object` to the new profile ID and returns `CMS_MSG_DONE`
**Usage**:
```php
// Triggered via UI button "Add Profile"
CMS_IFC_MESSAGE = "add";
```

---

### `delete`
**Purpose**: Deletes one or more profiles by ID.
**Parameters**:
- `$list` (array) – Array of profile IDs to delete
**Returns**: `CMS_MSG_DONE` if all deletions succeed, `CMS_MSG_ERROR` if any fail.
**Mechanism**:
- Iterates over `$list`
- Calls `$_profile->del($value)` for each ID
- Sets `$error` if any deletion fails
**Usage**:
```php
// Triggered via UI button "Delete Selected"
CMS_IFC_MESSAGE = "delete";
$list = [1001, 1002, 1003]; // Selected via checkboxes
```

---

### `display` / `display_save`
**Purpose**: Shows or saves a single profile.
**Parameters**:
- `$object` (int) – Profile ID to display/edit
- `$ifc_param1`–`$ifc_param48` – Form fields (code, user, password, etc.)
**Returns**: `CMS_MSG_DONE` on save success, `CMS_MSG_ERROR` on failure.
**Mechanism**:
- **`display`**: Loads profile data via `$_profile->get($object)`
- **`display_save`**: Maps `$ifc_param*` to `profile_data` fields, then calls `$_profile->set($profile_data)`
- Renders a two-tab form (Base Data, Custom Data)
- Uses `ifc` helper to generate form controls
**Usage**:
```php
// Display profile 1001
CMS_IFC_MESSAGE = "display";
$object = 1001;

// Save changes to profile 1001
CMS_IFC_MESSAGE = "display_save";
$object = 1001;
$ifc_param1 = "jdoe"; // code
$ifc_param2 = "john.doe"; // user
$ifc_param8 = "John"; // prename
$ifc_param9 = "Doe"; // surname
```

---

### `config_base`
**Purpose**: Configures visibility, editability, and required status for base profile fields.
**Parameters**:
- `$ifc_param1`–`$ifc_param78` – Checkbox states for each field
**Returns**: `CMS_MSG_DONE` on save success, `CMS_MSG_ERROR` on failure.
**Mechanism**:
- Loads `#system/profile` data
- Maps `$ifc_param*` to field settings (`visible`, `editable`, `required`)
- Saves via `$data->save()`
**Usage**:
```php
// Make "email" required
CMS_IFC_MESSAGE = "_config_base";
$ifc_param48 = "on"; // email required checkbox
```

---

### `config_custom`
**Purpose**: Configures visibility, editability, and required status for custom fields (1–20).
**Parameters**:
- `$ifc_param1`–`$ifc_param80` – Title, visible, editable, required for each field
**Returns**: `CMS_MSG_DONE` on save success, `CMS_MSG_ERROR` on failure.
**Mechanism**:
- Loads `#system/profile` data
- Loops over 20 custom fields, setting `title`, `visible`, `editable`, `required`
- Saves via `$data->save()`
**Usage**:
```php
// Configure custom field 3
CMS_IFC_MESSAGE = "_config_custom";
$ifc_param7 = "Department"; // title
$ifc_param8 = "on"; // visible
$ifc_param9 = "on"; // editable
$ifc_param10 = "on"; // required
```

---

### `sql_order`
**Purpose**: Toggles sort order (ascending/descending) for a column.
**Parameters**:
- `$ifc_param` (string) – Column name (e.g., `CMS_DB_PROFILE_SURNAME`)
**Returns**: None (modifies `$sql_order`).
**Mechanism**:
- If current order is `COLUMN DESC`, sets to `COLUMN`
- Otherwise, sets to `COLUMN DESC`
**Usage**:
```php
// Sort by surname descending
CMS_IFC_MESSAGE = "sql_order";
$ifc_param = CMS_DB_PROFILE_SURNAME;
```

---

### `page`
**Purpose**: Changes the current pagination page.
**Parameters**:
- `$ifc_param` (int) – Page number (0-based)
**Returns**: None (modifies `$page`).
**Mechanism**:
- Casts `$ifc_param` to integer
**Usage**:
```php
// Go to page 3
CMS_IFC_MESSAGE = "page";
$ifc_param = 3;
```

---

## Main Display

### Search & Filter
- **Field**: Dropdown of all profile fields
- **Condition**: `LIKE`, `=`, `>`, `<`, etc.
- **Value**: Text input
- **Group By**: Dropdown of groupable fields
- **Limit**: Rows per page (10–500)

### Profile List
- **Columns**: Selection, Index, Code, Name, Phone, Email
- **Actions**:
  - Click name to **view/edit**
  - Click return icon (if `CMS_IFC_SELECT`) to **select** profile
- **Selection Controls**: All, Invert, None

### Pagination
- Uses `pagination()` helper
- Navigates via `javascript:p(%page%)`

---

## JavaScript Helpers

| Function | Purpose |
|----------|---------|
| `r(value)` | Returns a selected profile value to the calling interface |
| `d(index)` | Opens the profile editor for the given ID |
| `p(number)` | Navigates to the specified page |

---

## Constants & Database Fields

| Constant | Value | Description |
|----------|-------|-------------|
| `CMS_DB_PROFILE_INDEX` | `profile_index` | Primary key |
| `CMS_DB_PROFILE_CODE` | `profile_code` | Internal code |
| `CMS_DB_PROFILE_USER` | `profile_user` | Username |
| `CMS_DB_PROFILE_SURNAME` | `profile_surname` | Last name |
| `CMS_DB_PROFILE_CUSTOM_FIELD` | `profile_custom_field_` | Prefix for custom fields (1–20) |

---

## Usage Example: Selecting a Profile

**Scenario**: A module needs to select a profile for a "Contact Person" field.

```php
// In the calling module
$url = cms_url([
    "ifc_page" => "profile",
    "ifc_message" => "main",
    "ifc_option" => "external",
    "ifc_select" => CMS_DB_PROFILE_EMAIL // Return email field
]);
echo("<a href=\"$url\">Select Contact</a>");

// After selection, the interface calls r(value) with the email
// The calling module receives the value via ifc_return()
```

---

## Usage Example: Configuring Fields

**Scenario**: An admin wants to make "mobile" and "email" required.

```php
// Step 1: Open configuration
$url = cms_url([
    "ifc_page" => "profile",
    "ifc_message" => "config_base"
]);
load_page($url);

// Step 2: Check the required checkboxes for mobile and email
// Step 3: Submit the form (triggers _config_base)
```

---

## Security & Permissions

- **Non-operators** can only view profiles and change sort/page
- **Operators** can add, edit, delete, and configure fields
- **CSRF protection** via `cms_param()` and `cms_url()`
- **SQL injection** prevented via `sqlesc()`
- **XSS protection** via `x()`, `qx()`, `rq()`

---

## Dependencies

- `profile` library (loaded via `cms_load()`)
- `data` helper (for `#system/profile` configuration)
- `ifc` helper (for UI generation)
- `pagination()` helper (for pagination controls)


<!-- HASH:a34de909f67f48923ecc8ad7556189eb -->
