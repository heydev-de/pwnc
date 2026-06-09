# PWNC API Documentation

[← Index](README.md) | [`index.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/index.php)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Module: Root Redirector (`index.php`)

### Overview
This file serves as the entry point for the PWNC Web Platform. Its sole purpose is to redirect incoming root-level requests (`/`) to the platform's primary module, the desktop interface. This ensures users and developers are immediately directed to the main application environment without manual navigation.

The redirect uses HTTP 303 ("See Other") to explicitly indicate that the target resource should be fetched using a `GET` request, regardless of the original request method. This is a best practice for web applications that perform redirects after form submissions or other non-idempotent operations.

---

### Inner Mechanisms
1. **Immediate Execution**
   The code is wrapped in an immediately-invoked anonymous function `(function() { ... })();` to ensure the redirect logic executes as soon as the file is loaded, without polluting the global namespace.

2. **Header-Based Redirect**
   The `header()` function sends a raw HTTP `Location` header to the client, triggering the browser to navigate to the specified URL. The `TRUE` parameter replaces any previously set `Location` header, and `303` enforces the HTTP status code.

3. **URL Resolution**
   `cms_url()` is used to generate the target URL dynamically. It resolves the logical path `CMS_MODULES_URL . "desktop.php"` (typically `/modules/desktop.php`) into a fully qualified URL, incorporating any global parameters (e.g., session tokens, CSRF protection) managed by the platform's state system.

4. **Termination**
   `exit()` halts further script execution after the redirect header is sent, preventing any unintended side effects.

---

### Usage Context
- **Primary Entry Point**: This file is intended to be placed in the web server's document root (e.g., `/var/www/html/index.php` or `public_html/index.php`).
- **Fallback Mechanism**: Acts as a safeguard for users who access the root URL directly (e.g., `https://example.com/`).
- **Development/Production**: Works identically in both environments, as it relies on the platform's core URL resolution system.

---

### Example Usage
#### Scenario: Root-Level Access
A user visits `https://pwnc.it/` in their browser. The web server routes the request to `index.php`, which executes the following logic:

```php
header("Location: " . cms_url(CMS_MODULES_URL . "desktop.php"), TRUE, 303);
exit();
```

**Result**:
- The browser receives a `303 See Other` response with the `Location` header set to `https://pwnc.it/modules/desktop.php?csrf_token=abc123&session_id=xyz456` (or similar, depending on the platform's state).
- The user is automatically redirected to the desktop module, where they can interact with the PWNC environment.

---

### Notes
- **No Direct Output**: This file does not generate any HTML or body content. The redirect is purely header-based.
- **Security**: The use of `cms_url()` ensures CSRF tokens and other security parameters are included in the redirect URL, maintaining session integrity.
- **Performance**: The redirect adds minimal overhead (a single HTTP round-trip) and is cached by browsers for subsequent visits.


<!-- HASH:4186a2bb651e5ec25c4d53b206e9a5b0 -->
