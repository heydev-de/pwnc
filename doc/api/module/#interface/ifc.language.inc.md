# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.language.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.language.inc)

- **Version:** `26.6.9.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Language Interface Module (`ifc.language.inc`)

This module provides a user interface for managing languages in the PWNC Web Platform. It handles language selection, creation, modification, deletion, and activation, including associated icons and stopwords. The module interacts with system configuration, data storage, and file management utilities to maintain language settings.

---

### **Constants and Global Variables**

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_L_ACCESS` | - | Permission required to access the language interface. |
| `CMS_IFC_MESSAGE` | - | Current interface action/message (e.g., `select`, `add`, `set`, `del`, `enable`). |
| `CMS_USER` | - | Current user identifier. |
| `CMS_DATA_PATH` | - | Filesystem path to the data directory. |
| `CMS_DATA_URL` | - | URL prefix for data assets. |
| `$object` | - | Currently selected language tag (e.g., `en`, `de`). |
| `$list` | `[]` | List of selected language tags for batch operations. |
| `$ifc_param`, `$ifc_param1`, `$ifc_param2`, etc. | - | Interface parameters passed during actions. |
| `$ifc_file1`, `$ifc_file1_name` | - | Uploaded file data for language icons. |

---

### **Core Components**

#### **1. Initialization and Caching**
```php
cms_cache_init($object, "language." . CMS_USER . ".object");
init($list, []);
$system = new system();
$data = new data("#system/language");
$map = new map("#system/language.image");
```
- **Purpose**: Initializes the module, loads cached language selection, and instantiates system, data, and map objects.
- **Mechanism**:
  - `cms_cache_init()` restores the last selected language for the current user.
  - `init($list, [])` ensures `$list` is an array.
  - `system()` manages global system settings (e.g., default languages).
  - `data("#system/language")` handles language metadata (names, stopwords).
  - `map("#system/language.image")` maps language tags to icon filenames.

---

#### **2. Message Handling (Actions)**
The module processes interface messages via a `switch` on `CMS_IFC_MESSAGE`.

---

##### **`select`**
```php
case "select":
    $object = $ifc_param;
    break;
```
- **Purpose**: Updates the currently selected language.
- **Parameters**:
  - `$ifc_param`: Language tag (e.g., `en`).
- **Usage**: Triggered when a user clicks a language in the list.

---

##### **`add`**
```php
case "add":
    if (! $data->get("x-undefined"))
        $data->set(["name" => CMS_L_IFC_LANGUAGE_010], "x-undefined");
    $object = "x-undefined";
    $ifc_response = $data->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;
```
- **Purpose**: Adds a new language with a placeholder tag (`x-undefined`).
- **Mechanism**:
  - Checks if `x-undefined` exists; if not, creates it with a default name.
  - Sets `$object` to `x-undefined` and saves the data.
- **Usage**: Triggered by the "Add Language" button.

---

##### **`set`**
```php
case "set":
    mkpath(CMS_DATA_PATH . "language");
    $ifc_param1 = preg_replace("/[^-0-9A-Za-z]+/", "", $ifc_param1);
    $ifc_param1 = strtolower($ifc_param1);
    // ... (name change, icon handling, stopwords)
    $ifc_response = ($data->save() & $map->save()) ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;
```
- **Purpose**: Updates language settings (tag, name, icon, stopwords, default status).
- **Parameters**:
  - `$ifc_param1`: New language tag (sanitized to alphanumeric + hyphens).
  - `$ifc_param2`: Display name.
  - `$ifc_param3`: Flag to set as default language.
  - `$ifc_param4`: Flag to delete the icon.
  - `$ifc_param5`: Stopwords (comma-separated).
  - `$ifc_file1`, `$ifc_file1_name`: Uploaded icon file.
- **Mechanism**:
  1. **Tag Sanitization**: Removes non-alphanumeric characters (except hyphens).
  2. **Name Change**: Renames the language tag and updates the icon filename if needed.
  3. **Default Language**: Prepends the language to the system's default language list.
  4. **Icon Handling**:
     - Deletes the icon if `$ifc_param4` is set or a new icon is uploaded.
     - Processes uploads (GIF, JPEG, PNG, SVG, WEBP) and moves the file to `CMS_DATA_PATH/language/`.
  5. **Stopwords**: Updates stopwords for the language.
- **Usage**: Triggered when saving language settings.

**Example**:
```php
// Set "en" as the default language with name "English" and upload an icon
$ifc_param1 = "en";       // Tag
$ifc_param2 = "English";  // Name
$ifc_param3 = TRUE;       // Set as default
$ifc_file1 = $_FILES["icon"]["tmp_name"];  // Uploaded file
$ifc_file1_name = "en.png";
```

---

##### **`del`**
```php
case "del":
    if (empty($list)) break;
    foreach ($list AS $value) {
        if (($image = $map->get_value($value)) !== NULL) {
            $map->del_key($value);
            if (is_file(CMS_DATA_PATH . "language/$image"))
                unlink(CMS_DATA_PATH . "language/$image");
        };
        $data->del($value);
    };
    $object = NULL;
    $ifc_response = ($data->save() & $map->save()) ? CMS_MSG_DONE : CMS_MSG_ERROR;
    break;
```
- **Purpose**: Deletes selected languages and their icons.
- **Parameters**:
  - `$list`: Array of language tags to delete.
- **Mechanism**:
  - Iterates over `$list`, deletes icons, and removes language data.
  - Resets `$object` to `NULL`.
- **Usage**: Triggered by the "Delete Selected" button.

---

##### **`enable`**
```php
case "enable":
    if (count($list)) {
        $array = explode(",", $system->getval("language", "default"));
        $primary = reset($array);
        if (in_array($primary, $list)) array_unshift($list, $primary);
        $list = array_unique($list);
        $list = implode(",", $list);
    } else {
        $list = NULL;
    };
    $system->setval($list, "language", "default");
    $ifc_response = ($system->save() && cms_load("directory") && directory_create_filesystem($list)) ?
        CMS_MSG_DONE : CMS_MSG_ERROR;
    break;
```
- **Purpose**: Enables/disables languages by updating the system's default language list.
- **Parameters**:
  - `$list`: Array of language tags to enable.
- **Mechanism**:
  - If `$list` is empty, clears the default language list.
  - Otherwise, updates the list, ensuring the primary language remains first.
  - Triggers filesystem updates via `directory_create_filesystem()`.
- **Usage**: Triggered by the "Enable" button.

---

#### **3. Main Display**
Renders the language management interface with two panels:
1. **Language List**: Displays all languages with selection checkboxes.
2. **Language Editor**: Shows settings for the selected language (if any).

##### **Key UI Elements**
- **Language List Table**:
  - Columns: Selection checkbox, name (with icon), tag.
  - Highlights the selected language in bold.
  - Uses `ifc_varied()` to alternate row colors.
- **Selection Controls**:
  - Buttons to select all/invert/none or active languages (via `language_select_active()`).
- **Language Editor**:
  - Fields for tag, name, default status, icon upload/delete, and stopwords.
  - Save button triggers the `set` action.

##### **JavaScript Helpers**
```javascript
function s(index) { ifc_post("select", index); }
function language_select_active() {
    var list = document.querySelectorAll("INPUT[name=\"list[]\"]");
    list.forEach(object => {
        switch (object.value) {
            case "en": case "de": // Dynamically generated from $array
                if (!object.checked) object.click();
                break;
            default:
                if (object.checked) object.click();
        };
    });
};
```
- **`s(index)`**: Posts a `select` message to update the selected language.
- **`language_select_active()`**: Toggles checkboxes for enabled languages.

---

### **Usage Examples**

#### **1. Adding a Language**
1. Click "Add Language" in the interface.
2. The system creates a placeholder (`x-undefined`) and opens the editor.
3. Set the tag (e.g., `fr`), name (e.g., `French`), and upload an icon.
4. Click "Save" to trigger the `set` action.

#### **2. Deleting Languages**
1. Check the checkboxes for languages to delete (e.g., `fr`, `es`).
2. Click "Delete Selected" to trigger the `del` action.
3. The system removes the languages and their icons.

#### **3. Enabling Languages**
1. Check the checkboxes for languages to enable (e.g., `en`, `de`).
2. Click "Enable" to trigger the `enable` action.
3. The system updates the default language list and filesystem.

---

### **Dependencies**
- **`data` Class**: Manages language metadata (names, stopwords).
- **`map` Class**: Maps language tags to icon filenames.
- **`system` Class**: Handles global settings (e.g., default languages).
- **`ifc` Class**: Renders the interface and form controls.
- **`cms_cache`**: Caches the selected language per user.
- **`directory_create_filesystem`**: Updates filesystem structures for enabled languages.

---

### **File Structure**
- **Icons**: Stored in `CMS_DATA_PATH/language/` (e.g., `en.png`).
- **Data**: Stored in `#system/language` (metadata) and `#system/language.image` (icon mappings).


<!-- HASH:b27846778d2ebde6348169437bb4e420 -->
