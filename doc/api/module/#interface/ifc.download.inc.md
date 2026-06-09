# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.download.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.download.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Download Interface Module (`ifc.download.inc`)

This file implements the **Download Management Interface** for the PWNC Web Platform. It provides a complete user interface for uploading, organizing, previewing, and managing downloadable files (e.g., documents, binaries, media) through a structured category-based system. The interface supports multilingual metadata (name, description), file replacement, bulk operations, and permission-based access control.

The module integrates with the platform’s **Interface Controller (IFC)** system, allowing it to be embedded in CMS pages, dialogs, or standalone views. It uses the `download` class (loaded via `cms_load`) to interact with the underlying data storage and file system.

---

## Core Workflow

The interface operates in two main modes:

1. **Main Display** – Shows a categorized list of downloads with a preview pane.
2. **Message Handling** – Processes user actions (e.g., upload, edit, delete) via `CMS_IFC_MESSAGE`.

State is maintained using:
- `cms_cache()` for user-specific preferences (e.g., last selected category).
- `language_*` functions for multilingual object identifiers.
- `ifc` class for UI rendering and form handling.

---

## Constants & Configuration

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_DOWNLOAD_PERMISSION_OPERATOR` | (Platform constant) | Permission level required to manage downloads (upload, edit, delete). |
| `CMS_DATA_PATH . "#download/"` | (Platform path) | Physical storage location for download files. |

---

## Key Functions & Logic

### `init($language)`
**Purpose**: Initializes the interface state for the current user and language.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$language` | `string` | Current UI language code. |

**Return**: `void`
**Mechanism**:
- Restores the last selected download object and category from cache.
- Ensures backward compatibility for legacy object identifiers (e.g., converts `"file123"` to `"download://file123"`).

**Usage Context**:
Called at the start of every interface load to synchronize state with user preferences.

---

### Message Handlers (Switch Cases)

Each `CMS_IFC_MESSAGE` case processes a specific user action. Below are the most critical handlers.

---

### `case "display"`
**Purpose**: Renders a detailed preview of a selected download (metadata + download button).
**Parameters** (via `$_GET`/`$object`):
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Download identifier (e.g., `"download://doc123"`). |

**Return**: `void` (outputs HTML)
**Mechanism**:
1. Fetches metadata (name, filename, description, size) from the `download` class.
2. Displays a table with:
   - Name (bold)
   - File extension + MIME type
   - File size (formatted)
   - Description (parsed as HTML)
3. Shows a download button if the user has permission.
4. Caches the selected object for future reference.

**Example**:
```php
// User clicks a download link in the list
// URL: ?ifc_page=download&ifc_message=display&object=download://manual.pdf
// Output: Preview pane with name, size, and "Download" button.
```

---

### `case "download"`
**Purpose**: Forces a file download via HTTP headers.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Download identifier. |

**Return**: `void` (exits script)
**Mechanism**:
1. Resolves the physical file path from the `download` class.
2. Sends headers:
   ```php
   header("Content-Type: " . get_mime_type($filename));
   header("Content-Disposition: attachment; filename=\"" . $name . "\"");
   ```
3. Streams the file to the client using `readfile()`.

**Usage Context**:
Triggered when a user clicks the "Download" button in the preview pane.

---

### `case "upload"`
**Purpose**: Displays a form to upload a new download.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Base identifier for the new download (used for multilingual variants). |
| `$language` | `string` | Target language for metadata. |

**Return**: `void` (outputs HTML/JS)
**Mechanism**:
1. Creates an `ifc` form with fields:
   - Name (text input, 40 chars max)
   - Description (rich text editor)
   - File (file upload, auto-clicked via JS)
   - Category (dropdown from `download_get_select()`)
2. Uses `language_get()` to handle multilingual identifiers.

**Example**:
```php
// Admin navigates to "Upload New Download"
// Form appears with fields for name, description, and file upload.
```

---

### `case "_upload"`
**Purpose**: Processes the uploaded file and saves metadata.
**Parameters** (via `$_POST`/`$ifc_*`):
| Name | Type | Description |
|------|------|-------------|
| `$ifc_file1` | `string` | Temporary file path. |
| `$ifc_file1_name` | `string` | Original filename. |
| `$ifc_param1` | `string` | Name. |
| `$ifc_param2` | `string` | Description. |
| `$ifc_param3` | `string` | Category. |

**Return**: `void` (sets `$ifc_response`)
**Mechanism**:
1. Calls `$download->add()` to:
   - Move the file to `CMS_DATA_PATH . "#download/"`.
   - Store metadata in the database.
2. Updates the multilingual object identifier.
3. Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Usage Context**:
Triggered when the user submits the upload form.

---

### `case "edit"`
**Purpose**: Displays a form to edit an existing download’s metadata.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Download identifier. |
| `$language` | `string` | Target language. |

**Return**: `void` (outputs HTML)
**Mechanism**:
1. Fetches current metadata using `$download->data->get()`.
2. Renders an `ifc` form with pre-filled fields (name, description, category).

**Example**:
```php
// Admin selects a download and clicks "Edit"
// Form appears with existing name/description/category.
```

---

### `case "_edit"`
**Purpose**: Saves edited metadata.
**Parameters** (via `$_POST`):
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | New name. |
| `$ifc_param2` | `string` | New description. |
| `$ifc_param3` | `string` | New category. |

**Return**: `void` (sets `$ifc_response`)
**Mechanism**:
1. Calls `$download->set()` to update the database.
2. Sets `$ifc_response` to indicate success/failure.

---

### `case "delete"`
**Purpose**: Deletes selected downloads and cleans up multilingual references.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$_object` | `array` | Array of download identifiers to delete. |

**Return**: `void` (sets `$ifc_response`)
**Mechanism**:
1. Deletes files and database entries using `$download->unlink()`.
2. For each language, checks if the download still exists. If not:
   - For the active language: Selects the first download in the same category.
   - For inactive languages: Clears the reference.
3. Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR`.

**Example**:
```php
// Admin selects 3 downloads and clicks "Delete"
// Files are removed, and the UI updates to show the next available download.
```

---

### `case "category_rename"`
**Purpose**: Renames a category and updates all downloads in that category.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Download identifier (used to fetch the current category). |

**Return**: `void` (outputs HTML)
**Mechanism**:
1. Displays a form with the current category name.
2. On submission (`_category_rename`), iterates through all downloads and updates their category field.

---

## Helper Functions

### `download_get_select()`
**Purpose**: Generates a dropdown list of all categories.
**Parameters**: None
**Return**: `string` (HTML `<select>` options)
**Mechanism**:
1. Calls `$download->data->get_categories()` to fetch unique category names.
2. Formats them as `<option>` elements.

**Usage Context**:
Used in upload/edit forms to assign downloads to categories.

---

### `download_get_array()`
**Purpose**: Builds a nested array of downloads grouped by category.
**Parameters**: None
**Return**: `array` (Structure: `["Category" => ["ID" => "Name"]]`)
**Mechanism**:
1. Iterates through all downloads using `$download->data->move()`.
2. Groups them by category, with names localized via `l()`.

**Usage Context**:
Populates the category list and download selection UI.

---

## Main Display Logic

### Category & Object Selection
1. **Category Handling**:
   - Restores the last selected category from `cms_cache()`.
   - If no category is set, defaults to the first available.
2. **Object Handling**:
   - If `$_object` is invalid, selects the first download in the category.
   - Updates the preview iframe to show the selected download.

### UI Components
| Component | Description |
|-----------|-------------|
| **Category Dropdown** | Lists all categories; triggers a reload on change. |
| **Download List** | Checkbox list of downloads in the selected category. Supports select-all/invert/none. |
| **Language Selector** | Shown if multilingual support is enabled. |
| **Preview Iframe** | Displays the "display" message for the selected download. |

### JavaScript Integration
- `download_select(value)`: Updates the preview iframe when a download is selected.
- `ifc_list_*` functions: Handle bulk selection in the download list.

---

## Example Workflow: Uploading a New Download

1. **Admin Action**:
   - Navigates to the Download interface.
   - Clicks "Upload New Download".
2. **Interface Response**:
   - `CMS_IFC_MESSAGE = "upload"` triggers the upload form.
   - Form includes fields for name, description, file, and category.
3. **Submission**:
   - User fills in metadata and selects a file.
   - Form submits to `CMS_IFC_MESSAGE = "_upload"`.
4. **Processing**:
   - File is moved to `CMS_DATA_PATH . "#download/"`.
   - Metadata is saved to the database.
   - `$ifc_response` is set to `CMS_MSG_DONE`, and the UI refreshes.

---

## Example Workflow: Deleting Downloads

1. **Admin Action**:
   - Selects multiple downloads using checkboxes.
   - Clicks "Delete".
2. **Interface Response**:
   - `CMS_IFC_MESSAGE = "delete"` is triggered.
3. **Processing**:
   - Files and database entries are deleted.
   - For each language, the UI updates to show the next available download in the same category.
4. **Result**:
   - `$ifc_response` is set, and the UI refreshes to reflect changes.

---

## Security & Permissions

- **Access Control**: Requires `CMS_L_ACCESS` for viewing and `CMS_L_OPERATOR` for management.
- **CSRF Protection**: All forms use `ifc` class methods, which include CSRF tokens.
- **File Validation**: The `download` class handles file type/size validation during upload.

---

## Dependencies

| Dependency | Purpose |
|------------|---------|
| `download` class | Handles file storage, metadata, and database operations. |
| `ifc` class | Renders UI components and manages form state. |
| `language_*` functions | Manages multilingual identifiers. |
| `cms_cache()` | Persists user preferences (e.g., last selected category). |
| `cms_url()` | Generates URLs for interface actions. |


<!-- HASH:2ceaafc1ef13f08ace44611843a9317e -->
