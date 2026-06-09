# PWNC API Documentation

[← Index](../README.md) | [`module/security.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/security.php)

- **Version:** `26.5.30.4`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Module: `security.php`

**Overview:**
This file implements the security module for the PWNC Web Platform, responsible for handling security-related events, primarily CSRF (Cross-Site Request Forgery) violations. When triggered, it renders a user-friendly error page with navigation options to return to the previous location or the homepage. The module is designed to be lightweight, context-aware, and integrated with the platform's URL and parameter management utilities.

---

### Constants
| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_SECURITY_EVENT_CSRF` | (Platform-defined) | Event identifier for CSRF violations. |
| `CMS_L_MOD_SECURITY_001` | (Localized string) | Title for the security error page. |
| `CMS_L_MOD_SECURITY_002` | (Localized string) | Error message template with placeholder for location. |
| `CMS_L_MOD_SECURITY_003` | (Localized string) | Fallback text when no location is provided. |
| `CMS_L_MOD_SECURITY_004` | (Localized string) | Link text for returning to the homepage. |
| `CMS_L_MOD_SECURITY_005` | (Localized string) | Link text for returning to the previous location. |
| `CMS_ROOT_URL` | (Platform-defined) | Base URL of the platform. |
| `CMS_DOCTYPE_HTML` | (Platform-defined) | HTML5 doctype declaration. |
| `CMS_HTML_HEADER` | (Platform-defined) | Standard HTML head content (meta tags, title, etc.). |
| `CMS_STYLESHEET` | (Platform-defined) | Link to the platform's stylesheet. |
| `CMS_CLASS` | (Platform-defined) | CSS class for the `<body>` element. |

---

### Global Variables
| Name | Type | Description |
|------|------|-------------|
| `$event` | `string` | The security event being handled (e.g., `CMS_SECURITY_EVENT_CSRF`). |
| `$location` | `string` | The URL or logical identifier of the page where the security violation occurred. |

---

### Module Logic
The module operates as an immediately-invoked function expression (IIFE) and performs the following steps:
1. **Outputs HTML Structure**: Renders a minimal HTML5 document with platform-standard headers and styles.
2. **Handles Security Events**: Uses a `switch` statement to process the `$event`. Currently, only `CMS_SECURITY_EVENT_CSRF` is supported.
3. **Displays Error Page**:
   - Shows a title and error message, incorporating the `$location` if available.
   - Provides navigation links to return to the previous location (if valid) or the homepage.
4. **Terminates Execution**: Calls `exit()` to halt further processing after rendering the error page.

---

### Key Functions/Utilities Used
#### `stre($v)`
- **Purpose**: Checks if a variable is empty (string, array, or `null`).
- **Parameters**:
  | Name | Type | Description |
  |------|------|-------------|
  | `$v` | `mixed` | Variable to check. |
- **Return**: `bool` – `TRUE` if empty, `FALSE` otherwise.
- **Usage Context**: Determines if `$location` is provided to customize the error message.

#### `nstre($v)`
- **Purpose**: Checks if a variable is **not** empty.
- **Parameters**: Same as `stre($v)`.
- **Return**: `bool` – `TRUE` if not empty, `FALSE` otherwise.
- **Usage Context**: Conditionally renders the "return to previous location" link.

#### `x($s)`
- **Purpose**: Escapes strings for XML/HTML output (converts `&`, `"`, `'`, `<`, `>` to entities).
- **Parameters**:
  | Name | Type | Description |
  |------|------|-------------|
  | `$s` | `string` | Input string to escape. |
- **Return**: `string` – Escaped string.
- **Usage Context**: Sanitizes dynamic content (e.g., `$location`, CSS classes) in HTML output.

#### `cms_url($addr, $param = [], $omit = FALSE)`
- **Purpose**: Generates a URL by merging a base address with parameters, respecting the platform's state and CSRF protection.
- **Parameters**:
  | Name | Type | Description |
  |------|------|-------------|
  | `$addr` | `string` | Base URL or logical identifier (e.g., `CMS_ROOT_URL`). |
  | `$param` | `array` | Additional query parameters. |
  | `$omit` | `bool` | If `TRUE`, omits global parameters. |
- **Return**: `string` – Fully qualified URL.
- **Usage Context**: Creates safe navigation links in the error page.

---

### Usage Example
#### Scenario: CSRF Violation Handling
When a CSRF token mismatch occurs, the platform triggers this module by setting `$event = CMS_SECURITY_EVENT_CSRF` and passing the referring URL in `$location`. The module renders an error page like this:

```php
// Simulate a CSRF event (e.g., in a form handler)
$event = CMS_SECURITY_EVENT_CSRF;
$location = "content://user/profile"; // Logical identifier for the user profile page
require("module/security.php");
```

**Output**:
- A page with the title "Security Violation" and message:
  *"A CSRF token mismatch occurred. You were redirected from [User Profile]."*
- Links to:
  - Return to the **User Profile** page (if `$location` is valid).
  - Return to the **Homepage**.

---

### Inner Mechanisms
1. **Localization**: Error messages and link texts are pulled from localized strings (e.g., `CMS_L_MOD_SECURITY_001`) for multilingual support.
2. **Context-Aware Escaping**: All dynamic content is escaped using `x()` to prevent XSS vulnerabilities.
3. **URL Resolution**: Logical identifiers (e.g., `content://user/profile`) are resolved to physical URLs via `cms_url()`.
4. **State Management**: The platform's global parameter state (e.g., session data) is preserved in generated URLs.

---

### When to Use
- **Triggering the Module**: Call this module when a security violation (e.g., CSRF) is detected. Set `$event` and `$location` before including the file.
- **Extending Events**: To add new security events (e.g., rate limiting), extend the `switch` statement and define corresponding localized strings.
- **Customization**: Override constants (e.g., `CMS_L_MOD_SECURITY_001`) in the platform's configuration to tailor messages.


<!-- HASH:2f82f7f0d0e292046ee78bbe22a5e6e6 -->
