# PWNC API Documentation

[← Index](../../README.md) | [`module/#desktop/desktop.address.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23desktop/desktop.address.inc)

- **Version:** `26.6.9.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Desktop Address Module (`desktop.address.inc`)

This file implements the address book functionality within the PWNC Web Platform's desktop interface. It handles the display, creation, modification, and deletion of address entries, as well as providing a categorized view of contacts by their initial letter. The module integrates with the desktop environment's messaging system (`CMS_IFC_MESSAGE`) to respond to user actions such as selecting, adding, saving, or deleting address records.

---

## Overview

### Purpose
The module serves as a **contact management system** within the desktop interface. It:
- Displays a list of contacts grouped by company and sorted alphabetically.
- Allows filtering contacts by their initial letter (A-Z or miscellaneous "…").
- Provides forms for viewing, editing, and creating address entries.
- Supports returning selected contact details (e.g., email) to other desktop modules via inter-frame communication (IFC).
- Integrates with the mailbox module to allow direct email composition from a contact.

### Key Features
- **Alphabetical Navigation**: Contacts are grouped by the first letter of their name or company.
- **CRUD Operations**: Create, Read, Update, Delete functionality for address records.
- **Inter-Module Communication**: Uses `CMS_IFC_MESSAGE` to handle commands and return values.
- **Contextual UI**: Adapts the interface based on whether a contact is selected or being edited.
- **Mail Integration**: Enables direct emailing from a contact’s email address if a mailbox is available.

---

## Core Logic Flow

The module operates in two main phases:

1. **Message Handling** (`CMS_IFC_MESSAGE` switch):
   - Processes incoming commands (`select`, `initial`, `add`, `save`, `delete`).
   - Modifies the internal state (`$object`, `$initial`) and sets the response (`$ifc_response`).

2. **Main Display**:
   - Renders the UI based on current state.
   - Generates the alphabetical index, contact list, and detail/edit form.

---

## Variables and State

| Name | Type | Description |
|------|------|-------------|
| `$object` | `string` or `null` | The currently selected address object key (identifier). |
| `$initial` | `string` | The currently selected initial letter (A-Z or "-"). |
| `$array` | `array` | Nested array of contacts: `$array[company][name][] = key`. |
| `$initial_count` | `array` | Count of contacts per initial: `$initial_count["A"] = 5`. |
| `$mailbox_user` | `string` or `null` | The user associated with the mailbox (if any). |
| `$mailbox_object` | `string` or `null` | The mailbox object key (if any). |
| `$ifc_param`, `$ifc_param1`, `$ifc_param2`, etc. | `mixed` | Parameters passed via IFC message. |
| `$ifc_response` | `string` | Response code sent back to the desktop (`CMS_MSG_DONE`, `CMS_MSG_ERROR`). |

---

## Message Handling

The module responds to the following `CMS_IFC_MESSAGE` values:

| Message | Parameters | Description |
|--------|------------|-------------|
| `select` | `$ifc_param` = object key | Selects an address object for viewing/editing. |
| `initial` | `$ifc_param` = initial letter | Filters the contact list by the given initial. |
| `add` | `$ifc_param` = company name (optional) | Creates a new address entry. If a company name is provided, it is pre-filled. |
| `save` | `$ifc_param1` to `$ifc_param7` = name, company, phone, fax, email, address, comment | Saves the current address object with the provided data. |
| `delete` | None | Deletes the currently selected address object. |

---

### `case "select"`

#### What it does
Sets the current address object to the one specified in `$ifc_param`. This object will be displayed in the detail/edit pane.

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | The key (identifier) of the address object to select. |

#### Return/State
- `$object` is set to `$ifc_param`.
- `$initial` is set to `NULL` (will be recalculated during display).

#### Usage Example
```javascript
// In a desktop module, trigger selection of an address
ifc_post('select', 'addr_12345');
```
This will load the address with key `addr_12345` into the detail view.

---

### `case "initial"`

#### What it does
Filters the contact list to show only entries whose name or company starts with the specified letter (or miscellaneous).

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` | The initial letter (A-Z) or "-" for miscellaneous. |

#### Return/State
- `$initial` is set to `$ifc_param`.
- `$object` is set to `NULL` (no object selected).

#### Usage Example
```javascript
// Filter contacts starting with 'M'
ifc_post('initial', 'M');
```

---

### `case "add"`

#### What it does
Creates a new address entry. If a company name is provided in `$ifc_param`, it is pre-filled in the new record. The new object is inserted into the desktop data store.

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param` | `string` or `null` | Optional company name to pre-fill. |
| `$initial` | `string` | If `$initial === "-"`, the name field is left empty. |

#### Inner Mechanisms
1. Checks if a subtype "address" exists in the desktop data.
2. Sets up a new buffer with default values:
   - `#type` = "address"
   - `company` = `$ifc_param` (if provided)
   - `name` = `NULL` if `$ifc_param` is set or `$initial === "-"`, otherwise `$initial`
3. Attempts to insert the new object.
4. If successful, sets `$object` to the new object key and returns `CMS_MSG_DONE`.
5. On failure, returns `CMS_MSG_ERROR`.

#### Return/State
- On success: `$object` = new object key, `$ifc_response = CMS_MSG_DONE`
- On failure: `$ifc_response = CMS_MSG_ERROR`

#### Usage Example
```javascript
// Create a new address with company "Acme Inc"
ifc_post('add', 'Acme Inc');
```

---

### `case "save"`

#### What it does
Saves the current address object (`$object`) with the provided data. Updates all fields (name, company, phone, fax, email, address, comment).

#### Parameters
| Parameter | Type | Description |
|-----------|------|-------------|
| `$ifc_param1` | `string` | Name |
| `$ifc_param2` | `string` | Company |
| `$ifc_param3` | `string` | Phone |
| `$ifc_param4` | `string` | Fax |
| `$ifc_param5` | `string` | Email |
| `$ifc_param6` | `string` | Address (multiline) |
| `$ifc_param7` | `string` | Comment |

#### Inner Mechanisms
1. Uses `$desktop->object_set()` to update each field of the current object.
2. Calls `$desktop->save()` to persist changes.
3. On success, sets `$ifc_response = CMS_MSG_DONE`.
4. On failure, sets `$ifc_response = CMS_MSG_ERROR`.

#### Return/State
- On success: `$ifc_response = CMS_MSG_DONE`
- On failure: `$ifc_response = CMS_MSG_ERROR`

#### Usage Example
```javascript
// Save the current address with new data
ifc_post('save', 'John Doe', 'Acme Inc', '+123456789', '+123456780', 'john@example.com', '123 Main St\nAnytown', 'VIP client');
```

---

### `case "delete"`

#### What it does
Deletes the currently selected address object (`$object`).

#### Parameters
None.

#### Inner Mechanisms
1. Calls `$desktop->delete_object($object)`.
2. On success, sets `$object = NULL` and `$ifc_response = CMS_MSG_DONE`.
3. On failure, sets `$ifc_response = CMS_MSG_ERROR`.

#### Return/State
- On success: `$object = NULL`, `$ifc_response = CMS_MSG_DONE`
- On failure: `$ifc_response = CMS_MSG_ERROR`

#### Usage Example
```javascript
// Delete the currently selected address
ifc_post('delete');
```

---

## Main Display Logic

### Initial Letter Calculation

If `$initial` is not set (i.e., `blank($initial)`), it is calculated as:
- The first character of the concatenation of `company` and `name` of the current object.
- Converted to uppercase.
- If not in A-Z, set to "-".

```php
$initial = $desktop->object_get($object, "company") . $desktop->object_get($object, "name");
$initial = $initial ? strtoupper($initial[0]) : "-";
if (strpos("ABCDEFGHIJKLMNOPQRSTUVWXYZ", $initial) === FALSE) $initial = "-";
```

---

### Contact List Generation

The module iterates through all desktop objects and:
- For each "address" object:
  - Extracts `name` and `company`.
  - Determines the initial letter (`$char`).
  - Increments the count for that letter in `$initial_count`.
  - If the letter matches `$initial`, adds the object to `$array[company][name][]`.
- For each "mailbox" object:
  - Sets `$mailbox_user` and `$mailbox_object` for email integration.

---

### Menu Construction

The menu is dynamically built based on context:

| Condition | Menu Item | Action |
|-----------|-----------|--------|
| Always | `CMS_L_COMMAND_ADD . "|desktop/command_create"` | Triggers `add` message. |
| If `$object` is set | `CMS_L_COMMAND_SAVE . "|desktop/command_save"` | Triggers `save` message. |
| If `$object` is set | `CMS_L_COMMAND_DELETE . "|desktop/command_delete"` | Triggers `delete` message. |
| If `CMS_IFC_SELECT` is set and object has the field | `CMS_L_COMMAND_INSERT` | Returns the value via `ifc_return()`. |

#### Return Value Handling
If `CMS_IFC_SELECT` is set (e.g., `"email"`), the module returns a formatted value:
- For `"email"`: `"Name" <email@example.com>`
- For other fields: the value of the `name` field.

```javascript
// Example: Return the email of the selected contact
ifc_return('"John Doe" <john@example.com>');
```

---

## UI Components

### Alphabetical Index

- Renders a vertical list of letters A-Z and "…".
- Highlights the current letter (`$initial`) in bold.
- Shows the count of contacts for each letter.
- Clicking a letter triggers `ifc_post('initial', letter)`.

```php
for ($i = 65; $i <= 90; $i++) {
    $char = chr($i);
    echo("<tr><td" . (($char === $initial) ? " class=\"enabled\"" : (isset($initial_count[$char]) ? " class=\"highlight\"" : "")) . ">");
    echo("<a href=\"javascript:ifc_post('initial','$char');\">");
    echo(($char === $initial) ? sprintf("<strong>%s</strong>", $char) : $char);
    echo(isset($initial_count[$char]) ? sprintf(" (%d)", $initial_count[$char]) : NULL);
    echo("</a></td></tr>");
}
```

---

### Contact List

- Displays a table with columns: **Name**, **Phone**, **Email**.
- Contacts are grouped by company.
- Each company header includes an "Add" button to create a new contact in that company.
- Clicking a contact triggers `ifc_post('select', key)`.
- If a mailbox is available, the email is rendered as a clickable link that opens the mailbox with the "to" field pre-filled.

```php
echo("<tr><td$varied>");
echo("<a href=\"javascript:ifc_post('select','" . qx($__value) . "');\">");
echo(image("desktop/icon_address") . " " . (streq($__value, $object) ? sprintf("<strong>%s</strong>", $_key) : $_key));
echo("</a></td>");
```

---

### Detail/Edit Form

- Only displayed if `$object` is set.
- Uses the `ifc` class to render form fields for all address fields.
- Fields:
  - **Name** (text, 40 chars)
  - **Company** (text, 40 chars, bold)
  - **Phone** (text, 40 chars, bold)
  - **Fax** (text, 40 chars, bold)
  - **Email** (text, 80 chars, bold)
  - **Address** (textarea, 40x4, 256 chars, bold)
  - **Comment** (textarea, 40x4, 256 chars)

```php
$ifc->set(CMS_L_NAME, "text 40 40", $desktop->object_get($object, "name"));
$ifc->set(CMS_L_DESKTOP_ADDRESS_003, "text 40 80 b", $desktop->object_get($object, "company"));
// ... other fields
```

---

## Integration with Other Modules

### Mailbox Integration

If a mailbox object is found (`$mailbox_user` and `$mailbox_object` are set), the email field becomes a clickable link:

```php
$_param = array_merge(
    $param,
    ["desktop_display" => "interface",
     "user" => $mailbox_user,
     "object" => $mailbox_object,
     "ifc_message" => "mail",
     "to" => $email]);
echo($mailbox_user ?
    "<a href=\"" . x(cms_url($_param)) . "\">" .
    image("desktop/icon_mailbox") . " " . x($email) .
    "</a>" :
    x($email));
```

This allows users to compose an email to the contact directly from the address book.

---

## Usage Example: Full Workflow

### Scenario: Adding and Editing a Contact

1. **Open the Address Book**:
   ```javascript
   // Navigate to the address module
   desktop_display('address');
   ```

2. **Filter by Letter 'A'**:
   ```javascript
   ifc_post('initial', 'A');
   ```

3. **Add a New Contact**:
   ```javascript
   ifc_post('add', 'Acme Corporation');
   ```
   - The form appears with "Acme Corporation" pre-filled in the company field.

4. **Fill in Details and Save**:
   ```javascript
   ifc_post('save', 'John Smith', 'Acme Corporation', '+1 555-1234', '+1 555-1235', 'john.smith@acme.com', '123 Business Ave\nSuite 100\nMetropolis', 'Primary contact for sales');
   ```

5. **Select the Contact Later**:
   ```javascript
   ifc_post('select', 'addr_12345'); // Use the actual object key
   ```

6. **Delete the Contact**:
   ```javascript
   ifc_post('delete');
   ```

---

## Security and Escaping

The module uses the following escaping functions to prevent XSS and injection:

| Function | Purpose | Example |
|----------|---------|---------|
| `q($s)` | JavaScript/JSON string encoding | `ifc_return('" . q($value) . "');` |
| `qx($s)` | `q()` + `x()` (JS + XML escaping) | `ifc_post('select','" . qx($__value) . "');` |
| `x($s)` | XML/HTML escaping | `x($desktop->object_get($__value, "phone"))` |

All dynamic values inserted into HTML or JavaScript are properly escaped.

---

## Dependencies

- **Desktop Environment**: Requires `$desktop` object with methods:
  - `data->seek()`, `data->set_buffer()`, `data->insert()`, `data->move()`
  - `object_get()`, `object_set()`, `object_type()`, `delete_object()`, `save()`
- **IFC System**: Uses `ifc` class for form rendering and inter-frame communication.
- **Language Constants**: Uses `CMS_L_*` constants for UI labels.
- **Utility Functions**: `blank()`, `q()`, `qx()`, `x()`, `image()`, `ifc_table_open()`, `ifc_table_close()`, `ifc_varied()`

---

## Best Practices for Developers

1. **Use IFC Messages for Interaction**:
   Always use `ifc_post()` to trigger actions (e.g., `select`, `add`, `save`, `delete`) rather than direct function calls.

2. **Handle Return Values**:
   If your module needs to receive a contact (e.g., for an email), set `CMS_IFC_SELECT` to the desired field (e.g., `"email"`) before opening the address book.

3. **Leverage Mail Integration**:
   Ensure a mailbox is available for users to take full advantage of the email link feature.

4. **Respect Escaping**:
   Never output unescaped dynamic data. Always use `x()`, `q()`, or `qx()` as appropriate.

5. **Performance**:
   The module efficiently groups contacts by initial and company, minimizing DOM updates during filtering.

6. **Extensibility**:
   To add a new field, update:
   - The `save` case (add `$desktop->object_set()` call).
   - The detail form (add `$ifc->set()` call).
   - The database schema (if persistent).


<!-- HASH:8c627e080f378f878502f11e144b8017 -->
