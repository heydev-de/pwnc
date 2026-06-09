# PWNC API Documentation

[← Index](../../README.md) | [`module/#desktop/desktop.link.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23desktop/desktop.link.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Desktop Link Module (`desktop.link.inc`)

This module provides the interface and logic for managing desktop shortcut links within the PWNC Web Platform. It allows users to create, modify, and interact with web-based shortcuts that load external or internal URLs in an embedded iframe. The module integrates with the desktop environment's object storage system to persist link configurations and supports user interactions such as saving, reloading, and breaking out of the iframe into a full browser window.

---

### **Constants & Static Variables**
The module relies on the following predefined constants (typically defined in language files or core configuration):

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_IFC_MESSAGE` | Runtime-defined | Determines the current interface action (e.g., `reload`, `save`). |
| `CMS_L_DESKTOP_LINK_001` | Language string | Label for the "Breakout" command. |
| `CMS_L_DESKTOP_LINK_002` | Language string | Title or description of the desktop link interface. |
| `CMS_L_DESKTOP_LINK_003` | Language string | Label for the "Reload" command. |
| `CMS_L_COMMAND_SAVE` | Language string | Label for the "Save" command. |
| `CMS_L_URL` | Language string | Label for the URL input field. |
| `CMS_URL` | Core constant | Base URL of the PWNC installation. |
| `DESKTOP_USER` | Runtime-defined | Current user identifier for desktop object ownership. |

---

### **Core Logic & Workflow**

#### **1. Message Handling (`CMS_IFC_MESSAGE` Switch)**
The module processes interface commands via a `switch` statement on `CMS_IFC_MESSAGE`. This pattern is used across PWNC to handle user-triggered actions (e.g., button clicks).

| Case | Purpose | Inner Mechanism |
|------|---------|-----------------|
| `reload` | Resets the interface state. | Clears `$ifc_param1` to force a fresh load. |
| `save` | Persists the current URL to the desktop object. | Calls `$desktop->object_set()` to update the object's `url` property, then invokes `$desktop->save()`. Sets `$ifc_response` to `CMS_MSG_DONE` or `CMS_MSG_ERROR` based on success. |

**Usage Context**:
- Triggered when the user clicks "Save" or "Reload" in the interface.
- Relies on the `$desktop` object (assumed to be pre-instantiated) for object storage.

---

#### **2. Main Display Logic**
The module dynamically generates the interface using the `ifc` class (PWNC's interface controller) and embeds an iframe for URL rendering.

##### **Parameter Setup**
```php
$param = ["user" => DESKTOP_USER, "object" => $object];
```
| Parameter | Type | Description |
|-----------|------|-------------|
| `user` | `string` | Current user identifier (used for object ownership). |
| `object` | `string` | Unique identifier of the desktop link object. |

##### **URL Resolution**
The URL is determined in the following order of precedence:
1. **Explicit Parameter**: Uses `$ifc_param1` if set and non-empty (`nstre($ifc_param1)`).
2. **Stored Object**: Falls back to the URL stored in the desktop object (`$desktop->object_get($object, "url")`).
3. **Default Blank Page**: Uses `CMS_URL . "blank.htm"` if no URL is stored.

The resolved URL is then passed through `cms_url()` to ensure proper formatting and CSRF protection.

##### **Interface Construction**
- **`ifc` Class Initialization**:
  ```php
  $ifc = new ifc(NULL, $ifc_page, [...], $param, NULL, CMS_L_DESKTOP_LINK_002);
  ```
  - **Commands**: Maps user actions (e.g., "Save", "Reload") to `CMS_IFC_MESSAGE` values.
  - **Parameters**: Passes `user` and `object` to the interface context.

- **URL Input Field**:
  ```php
  $ifc->set(CMS_L_URL, "text 60 256 :", $url);
  ```
  - **Label**: `CMS_L_URL` (e.g., "URL").
  - **Type**: Text input with a maximum length of 256 characters.
  - **Default Value**: The resolved `$url`.

- **Load Button**:
  ```php
  $ifc->set("Laden", "button b", "javascript:desktop_link_load()");
  ```
  - Triggers `desktop_link_load()` to reload the iframe with the current input URL.

- **Menu Button & Iframe**:
  - A static HTML anchor (`ifc-desktop-link-menu`) is rendered for future extensibility (e.g., dropdown menus).
  - The iframe (`desktop-link-output`) embeds the resolved URL with extensive permission attributes for modern web APIs (e.g., camera, microphone, fullscreen).

---

### **JavaScript Functions**
The module includes inline JavaScript to handle dynamic interactions.

#### **`desktop_link_breakout()`**
**Purpose**: Opens the current iframe URL in the full browser window.
**Parameters**: None.
**Return Value**: None.
**Mechanism**:
- Uses `window.location.replace()` to navigate the top-level window to the URL stored in `$url` (escaped via `q()` for JSON safety).
**Usage Example**:
```javascript
desktop_link_breakout(); // Redirects the browser to the iframe's URL.
```

#### **`desktop_link_load()`**
**Purpose**: Reloads the iframe with the URL from the input field.
**Parameters**: None.
**Return Value**: None.
**Mechanism**:
- Retrieves the current value of the `ifc_param1` input field (the URL input) and sets it as the iframe's `src`.
**Usage Example**:
```javascript
desktop_link_load(); // Refreshes the iframe with the new URL.
```

#### **Event Listeners**
| Listener | Trigger | Purpose |
|----------|---------|---------|
| `ifc_param1` `keydown` | `Enter` key | Calls `desktop_link_load()` to reload the iframe. |
| `iframe` `load` | Iframe load completion | Updates `ifc_param1` with the iframe's current URL (via `contentWindow.location.href`). |
| `window_load` | Window load | Blurs the `ifc_param1` input to remove focus. |

---

### **Usage Example**
#### **Scenario: Creating a Desktop Shortcut to an External Website**
1. **Backend Setup**:
   - A desktop object is created with a unique `$object` identifier.
   - The module is loaded via `cms_load("desktop/link")` or through the desktop interface.

2. **User Interaction**:
   - The user enters a URL (e.g., `https://example.com`) into the input field.
   - The user clicks "Save" to persist the URL to the desktop object.
   - The iframe loads the URL, and the user interacts with it.

3. **Code Snippet**:
   ```php
   // Assume $desktop is initialized and $object is set.
   $ifc_param1 = "https://example.com";
   CMS_IFC_MESSAGE = "save"; // Simulate a save action.
   include("module/#desktop/desktop.link.inc");
   ```
   - This will save the URL to the desktop object and display the iframe.

4. **Breakout**:
   - The user clicks the "Breakout" button to open the URL in the full browser window.

---

### **Key Integration Points**
| Function/Class | Purpose | Integration Example |
|----------------|---------|---------------------|
| `$desktop->object_set()` | Stores the URL in the desktop object. | `$desktop->object_set($object, "url", "https://example.com");` |
| `$desktop->object_get()` | Retrieves the stored URL. | `$url = $desktop->object_get($object, "url");` |
| `cms_url()` | Formats URLs with CSRF protection. | `$url = cms_url("https://example.com", NULL, TRUE);` |
| `ifc` class | Renders the interface. | `$ifc->set("Label", "text 60 256 :", $value);` |
| `q()` | Escapes strings for JavaScript/JSON. | `echo(q($url));` |
| `x()` | Escapes strings for HTML/XML. | `echo("<iframe src=\"" . x($url) . "\">");` |

---

### **Error Handling & Edge Cases**
- **Empty URL**: Falls back to `CMS_URL . "blank.htm"` if no URL is provided.
- **Permission Errors**: The iframe's `allow` attributes are permissive by default, but modern browsers may block certain APIs (e.g., camera) unless the parent page is served over HTTPS.
- **Cross-Origin Restrictions**: The `load` event listener may fail to read `contentWindow.location.href` due to same-origin policy. The `try-catch` block silently handles this.
- **CSRF Protection**: `cms_url()` ensures all generated URLs include a CSRF token.


<!-- HASH:89008f8a8051e6cc3126d30a6ff59967 -->
