# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.mailform.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.mailform.inc)

- **Version:** `26.6.9.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Mailform Interface (`ifc.mailform.inc`)

This file provides the **Mailform Interface** for the PWNC Web Platform, enabling the creation, configuration, and management of customizable mail forms. It integrates with the platform's **FlexView** and **Data** modules to offer a hierarchical, drag-and-drop interface for building forms with various field types (e.g., text, checkboxes, radio buttons, select dropdowns, hidden fields, and custom code).

The interface supports:
- **Form structure management** (containers, fields, and page breaks).
- **Field configuration** (validation, default values, and display options).
- **Email settings** (SMTP, sender/receiver addresses, and test emails).
- **Template integration** for embedding forms in pages.

---

## Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_MAILFORM_PERMISSION_OPERATOR` | `"operator"` | Permission level required for advanced operations (e.g., SMTP configuration). |

---

## Message Handling

The interface processes messages via `CMS_IFC_MESSAGE` to perform actions like adding fields, saving configurations, or deleting elements. Below are the key message handlers:

---

### `select`
**Purpose**: Selects a form object (field or container) for editing.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Index of the object to select. |
**Return**: None.
**Mechanism**: Updates the global `$object` variable and caches the selection.
**Usage**:
```php
// Select a form object with index "123"
CMS_IFC_MESSAGE = "select";
$ifc_param = "123";
include("module/#interface/ifc.mailform.inc");
```

---

### `display`
**Purpose**: Renders a preview of the selected form in a template.
**Parameters**: None.
**Return**: None (exits script after rendering).
**Mechanism**: Uses `template_preview()` to generate a PHP snippet embedding the form.
**Usage**:
```php
// Preview the form with index "123"
CMS_IFC_MESSAGE = "display";
$object = "123";
include("module/#interface/ifc.mailform.inc");
```

---

### `add_receiver` / `add_element`
**Purpose**: Initiates the addition of a receiver (form container) or field (element).
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Target parent object index. |
| `$ifc_param1` | `string` | Default name for the new object. |
| `$ifc_param2` | `string` | Field subtype (e.g., `"text"`, `"checkbox"`). |
**Return**: None (displays a FlexView interface for target selection).
**Mechanism**:
- For `add_receiver`, defaults to a container with a generic name.
- For `add_element`, allows selection of field type (e.g., text, radio).
**Usage**:
```php
// Add a text field to container "123"
CMS_IFC_MESSAGE = "add_element";
$object = "123";
$ifc_param2 = "text";
include("module/#interface/ifc.mailform.inc");
```

---

### `add_receiver_target` / `add_element_target`
**Purpose**: Displays a form to configure the new receiver/element before insertion.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Target parent object index. |
| `$ifc_param1` | `string` | Name of the new object. |
| `$ifc_param2` | `string` | Field subtype (for elements). |
**Return**: None (renders an IFC form for configuration).
**Mechanism**:
- Validates the name and subtype.
- Uses `flexview->show_target()` to display insertion/append options.
**Usage**:
```php
// Configure a new checkbox field
CMS_IFC_MESSAGE = "add_element_target";
$ifc_param = "123";
$ifc_param1 = "Agree to Terms";
$ifc_param2 = "checkbox";
include("module/#interface/ifc.mailform.inc");
```

---

### `add_receiver_insert` / `add_receiver_append`
**Purpose**: Inserts/appends a new receiver (container) into the form hierarchy.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Target parent object index. |
| `$ifc_param1` | `string` | Name of the new container. |
**Return**: Updates `$ifc_response` with `CMS_MSG_DONE` or `CMS_MSG_ERROR`.
**Mechanism**:
- Creates a container with a template for embedding.
- Updates the global `$object` to the new container's index.
**Usage**:
```php
// Append a new form container
CMS_IFC_MESSAGE = "add_receiver_append";
$ifc_param = "123";
$ifc_param1 = "Contact Form";
include("module/#interface/ifc.mailform.inc");
```

---

### `add_element_insert` / `add_element_append`
**Purpose**: Inserts/appends a new field (element) into the form hierarchy.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Target parent object index. |
| `$ifc_param1` | `string` | Name of the new field. |
| `$ifc_param2` | `string` | Field subtype (e.g., `"text"`). |
**Return**: Updates `$ifc_response` with `CMS_MSG_DONE` or `CMS_MSG_ERROR`.
**Mechanism**:
- Sets default values (e.g., HTML code for `"code"` fields).
- Updates the global `$object` to the new field's index.
**Usage**:
```php
// Insert a text field
CMS_IFC_MESSAGE = "add_element_insert";
$ifc_param = "123";
$ifc_param1 = "Full Name";
$ifc_param2 = "text";
include("module/#interface/ifc.mailform.inc");
```

---

### `save`
**Purpose**: Saves the configuration of the currently selected object.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` | Index of the object to save. |
| `$ifc_param1` | `string` | Name of the object. |
| `$ifc_param2`–`$ifc_param11` | `mixed` | Field-specific values (e.g., description, validation rules). |
**Return**: Updates `$ifc_response` with `CMS_MSG_DONE` or `CMS_MSG_ERROR`.
**Mechanism**:
- Validates and updates object properties based on its type (e.g., `container`, `text`).
- Updates associated templates if applicable.
**Usage**:
```php
// Save a text field with validation
CMS_IFC_MESSAGE = "save";
$object = "123";
$ifc_param1 = "Email"; // Name
$ifc_param2 = "email"; // ID
$ifc_param3 = "Enter your email"; // Description
$ifc_param4 = 50; // Width
$ifc_param5 = 256; // Length
$ifc_param6 = ""; // Default
$ifc_param7 = "/^.+@.+\..+$/"; // Validation (email)
$ifc_param8 = TRUE; // Required
include("module/#interface/ifc.mailform.inc");
```

---

### `copy_insert` / `copy_append` / `cut_insert` / `cut_append`
**Purpose**: Copies or moves objects within the form hierarchy.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Comma-separated source and target indices (e.g., `"123,456"`). |
**Return**: Updates `$ifc_response` with `CMS_MSG_DONE` or `CMS_MSG_ERROR`.
**Mechanism**:
- Clones or moves objects between containers.
- Handles template updates for copied containers.
**Usage**:
```php
// Copy field "123" and append to container "456"
CMS_IFC_MESSAGE = "copy_append";
$ifc_param = "123,456";
include("module/#interface/ifc.mailform.inc");
```

---

### `del`
**Purpose**: Deletes the specified object and its children.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Index of the object to delete. |
**Return**: Updates `$ifc_response` with `CMS_MSG_DONE` or `CMS_MSG_ERROR`.
**Mechanism**:
- Removes the object and its associated template.
- Selects the nearest existing parent object.
**Usage**:
```php
// Delete field "123"
CMS_IFC_MESSAGE = "del";
$ifc_param = "123";
include("module/#interface/ifc.mailform.inc");
```

---

### `config`
**Purpose**: Displays the SMTP/email configuration form.
**Parameters**: None.
**Return**: None (renders an IFC form for email settings).
**Mechanism**:
- Loads current settings from the `system` module.
- Requires `CMS_MAILFORM_PERMISSION_OPERATOR` permission.
**Usage**:
```php
// Open email configuration
CMS_IFC_MESSAGE = "config";
include("module/#interface/ifc.mailform.inc");
```

---

### `_config`
**Purpose**: Saves SMTP/email configuration.
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Sender email address. |
| `$ifc_param2` | `string` | Reply-to address. |
| `$ifc_param3` | `string` | Email method (`"mail"` or custom SMTP). |
| `$ifc_param4` | `string` | SMTP server address. |
| `$ifc_param5` | `string` | SMTP username. |
| `$ifc_param6` | `string` | SMTP password. |
**Return**: Updates `$ifc_response` with `CMS_MSG_DONE` or `CMS_MSG_ERROR`.
**Mechanism**:
- Validates and saves settings to the `system` module.
**Usage**:
```php
// Save SMTP configuration
CMS_IFC_MESSAGE = "_config";
$ifc_param1 = "noreply@example.com";
$ifc_param3 = "smtp";
$ifc_param4 = "smtp.example.com";
include("module/#interface/ifc.mailform.inc");
```

---

### `test`
**Purpose**: Sends a test email using the current SMTP configuration.
**Parameters**: None.
**Return**: Updates `$ifc_response` with `CMS_MSG_DONE` or an error message.
**Mechanism**:
- Uses the `smtp` and `mime` modules to send a test message.
- Falls back to the system's default email if no reply-to is set.
**Usage**:
```php
// Send a test email
CMS_IFC_MESSAGE = "test";
include("module/#interface/ifc.mailform.inc");
```

---

## Main Display

The default view renders a **hierarchical form editor** with:
1. **FlexView**: Drag-and-drop interface for managing form structure.
2. **Object Editor**: Configuration panel for the selected object (e.g., field properties).
3. **Trash Bin**: Drop zone for deleting objects.

### Key Features
- **Drag-and-Drop**: Reorder fields/containers via JavaScript (`mailform_flexview_event`).
- **Type-Specific Forms**: Dynamic fields based on the selected object type (e.g., validation rules for `text` fields).
- **Template Integration**: Automatically generates templates for containers.

### Example Workflow
1. **Create a Form**:
   ```php
   CMS_IFC_MESSAGE = "add_receiver";
   $ifc_param1 = "Contact Us";
   include("module/#interface/ifc.mailform.inc");
   ```
2. **Add Fields**:
   ```php
   CMS_IFC_MESSAGE = "add_element";
   $object = "123"; // Container index
   $ifc_param2 = "text";
   include("module/#interface/ifc.mailform.inc");
   ```
3. **Configure Fields**:
   ```php
   CMS_IFC_MESSAGE = "save";
   $object = "456"; // Field index
   $ifc_param1 = "Email";
   $ifc_param7 = "/^.+@.+\..+$/"; // Email validation
   include("module/#interface/ifc.mailform.inc");
   ```
4. **Embed in a Page**:
   Use the generated template snippet in a page:
   ```php
   $GLOBALS["mailform_form"] = "123"; // Container index
   cms_application("mailform");
   ```

---
## Helper Function: `qr()`
**Purpose**: Encodes strings for safe display in FlexView (uses `q()` for JSON-style encoding).
**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$s` | `string` | String to encode. |
**Return**: `string` – Encoded string.
**Usage**:
```php
echo qr("Hello <World>"); // Outputs: "Hello \u003CWorld\u003E"
```


<!-- HASH:a19d617692f9c487035504166a14e74f -->
