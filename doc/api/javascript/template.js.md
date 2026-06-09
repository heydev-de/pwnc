# PWNC API Documentation

[← Index](../README.md) | [`javascript/template.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/template.js)

- **Version:** `26.5.30.4`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Template Control System (`template.js`)

This file provides the frontend JavaScript logic for PWNC's **Template Control System**, a visual editing interface for dynamic web content. It enables interactive manipulation of template elements, including drag-and-drop operations, control panel management, and state persistence for folded/unfolded template sections.

---

### Global Variables

| Name                  | Value/Default | Description                                                                 |
|-----------------------|---------------|-----------------------------------------------------------------------------|
| `tp_ctrl_opt_value`   | `0`           | Bitmask storing the current state of template control options (e.g., visibility, behavior flags). |
| `tp_ctrl_opt_img_url` | `""`          | Base URL for SVG icons used in the control panel buttons.                  |

---

### `tp_event()`
**Purpose:**
Initializes event listeners for template-related UI elements. Sets up hover effects, touchscreen context menu suppression, and tooltip cleanup.

**Parameters:**
None.

**Return Values:**
None.

**Inner Mechanisms:**
1. **Dropdown Hover Effects:**
   - Targets all elements with classes `.tp-dd` or `.tp-dd100`.
   - Adds `data-tp-hover="1"` on `mouseover` and removes it on `mouseout` to trigger CSS hover states.
2. **Touchscreen Context Menu Suppression:**
   - Disables right-click context menus on `.tp-edt > BUTTON` elements for touch devices (`e.pointerType === "touch"`).
3. **Tooltip Suppression:**
   - Clears `title` attributes for `.module-settings` elements to prevent native browser tooltips.

**Usage Context:**
Called during page initialization (e.g., `DOMContentLoaded`) to enable interactive template editing features.

**Example:**
```javascript
document.addEventListener("DOMContentLoaded", tp_event);
```

---

### `tp_beforedragstart()`
**Purpose:**
Prepares the UI for drag-and-drop operations by hiding the control pad and removing focus from active elements.

**Parameters:**
None.

**Return Values:**
None.

**Inner Mechanisms:**
1. Hides the template marker (`#tp-marker`) and control panel (`#tp-ctrl`) using `fx_style()`.
2. Blurs the currently focused element to prevent interference during drag operations.

**Usage Context:**
Attached to drag-and-drop event listeners (e.g., `dragstart`) to ensure a clean UI state.

**Example:**
```javascript
document.getElementById("draggable-element").addEventListener("dragstart", tp_beforedragstart);
```

---

### `tp_drop()`
**Purpose:**
Restores the control pad visibility after a drag-and-drop operation completes.

**Parameters:**
None.

**Return Values:**
None.

**Inner Mechanisms:**
Reverses `tp_beforedragstart()` by showing the template marker and control panel.

**Usage Context:**
Attached to `drop` or `dragend` events.

**Example:**
```javascript
document.addEventListener("drop", tp_drop);
```

---

### `tp_ctrl_opt_set(value)`
**Purpose:**
Sets the control panel option bitmask to a specific value and triggers an update.

**Parameters:**

| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `value` | Number | New bitmask value for `tp_ctrl_opt_value`. |

**Return Values:**
None.

**Inner Mechanisms:**
1. Updates `tp_ctrl_opt_value` with the provided `value`.
2. Simulates a click on the `#tp-ctrl-opt-apply` button to apply changes.

**Usage Context:**
Used to programmatically set template options (e.g., via keyboard shortcuts or external UI controls).

**Example:**
```javascript
// Enable "text" and "href" options (bitmask: 4 | 1 = 5)
tp_ctrl_opt_set(5);
```

---

### `tp_ctrl_opt_switch(value)`
**Purpose:**
Toggles a specific bit in the control panel option bitmask.

**Parameters:**

| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `value` | Number | Bitmask value to toggle (e.g., `4` for "text" option). |

**Return Values:**
None.

**Inner Mechanisms:**
1. Uses bitwise XOR (`^=`) to toggle the specified bit in `tp_ctrl_opt_value`.
2. Calls `tp_ctrl_opt_img()` to update the UI.

**Usage Context:**
Triggered by control panel buttons to enable/disable individual options.

**Example:**
```javascript
// Toggle the "image" option (bitmask: 32)
document.getElementById("tp-ctrl-opt-image").addEventListener("click", () => tp_ctrl_opt_switch(32));
```

---

### `tp_ctrl_opt_apply(url)`
**Purpose:**
Applies the current control panel options by redirecting to a URL with the options embedded.

**Parameters:**

| Name  | Type   | Description                                                                 |
|-------|--------|-----------------------------------------------------------------------------|
| `url` | String | URL template with placeholders (`%value%`, `%left%`, `%top%`) to replace. |

**Return Values:**
None.

**Inner Mechanisms:**
1. Replaces placeholders in the URL:
   - `%value%`: Current `tp_ctrl_opt_value`.
   - `%left%`/`%top%`: Cursor position from `fx_position_left()`/`fx_position_top()`.
2. Redirects using `location.replace(url)`.

**Usage Context:**
Called when the "Apply" button is clicked to persist template changes.

**Example:**
```javascript
// URL: "edit.php?options=%value%&x=%left%&y=%top%"
tp_ctrl_opt_apply("edit.php?options=%value%&x=%left%&y=%top%");
```

---

### `tp_ctrl_opt_img()`
**Purpose:**
Updates the visual state of control panel buttons based on the current bitmask.

**Parameters:**
None.

**Return Values:**
None.

**Inner Mechanisms:**
1. Maps bitmask values to button IDs and icon filenames:
   - `array1`: Bitmask values (e.g., `4` for "text").
   - `array2`: Button identifiers (e.g., `"text"`).
2. For each button, updates its SVG icon to reflect enabled/disabled state using `fx_change_image()`.

**Usage Context:**
Called after `tp_ctrl_opt_value` changes to refresh the UI.

**Example:**
```javascript
// Initialize control panel icons
tp_ctrl_opt_img_url = "/assets/icons/";
tp_ctrl_opt_img();
```

---

### `tp_flp(id)`
**Purpose:**
Toggles the folded/unfolded state of a template section (`#tp-dd-{id}`) and updates related sections.

**Parameters:**

| Name | Type   | Description                          |
|------|--------|--------------------------------------|
| `id` | String | ID of the template section to toggle. |

**Return Values:**
Boolean (`false`) to prevent default event behavior.

**Inner Mechanisms:**
1. **Modifier Key Logic:**
   - If **Shift/Ctrl/Alt** is pressed, toggles all sibling sections (including nested children).
   - Otherwise, toggles only the specified section.
2. **State Persistence:**
   - Calls `tp_flp_store()` to save the current state to a cookie.

**Usage Context:**
Attached to click events on template section headers.

**Example:**
```javascript
document.getElementById("tp-dd-header-1").addEventListener("click", () => tp_flp("1"));
```

---

### `tp_flp_store()`
**Purpose:**
Saves the current folded/unfolded state of all template sections to a cookie.

**Parameters:**
None.

**Return Values:**
None.

**Inner Mechanisms:**
1. Iterates over all `.tp-dd100` elements.
2. Constructs a string (e.g., `"/1/3/"`) of IDs for unfolded sections.
3. Stores the string in the `cms_tp_flp_value` cookie.

**Usage Context:**
Called automatically by `tp_flp()` to persist state.

---

### `tp_flp_restore(content_index)`
**Purpose:**
Restores the folded/unfolded state of template sections from a cookie.

**Parameters:**

| Name            | Type   | Description                          |
|-----------------|--------|--------------------------------------|
| `content_index` | String | Current page ID to validate cookie relevance. |

**Return Values:**
None.

**Inner Mechanisms:**
1. **Validation:**
   - Checks if the cookie (`cms_tp_flp_id`) matches the current `content_index`. If not, clears the state cookie.
2. **Restoration:**
   - Parses the `cms_tp_flp_value` cookie and updates the `data-tp-flp-on` attribute for each section.
3. **Visual Feedback:**
   - Adds the `tp-flp-restored` class to the `<html>` element after a delay for CSS transitions.

**Usage Context:**
Called during page load to restore the user's previous editing state.

**Example:**
```javascript
// Restore state for page ID "home"
tp_flp_restore("home");
```


<!-- HASH:a85e1e81e81e008c9a8aac5e37d548de -->
