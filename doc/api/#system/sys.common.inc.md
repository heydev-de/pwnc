# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.common.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.common.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Common Library (`sys.common.inc`)

The `sys.common.inc` file serves as the central **autoloader and dependency manager** for the PWNC Web Platform's core utility libraries. It establishes the `cms` namespace and ensures all foundational modules are available for use throughout the system.

This file does **not** contain executable logic itself—its sole purpose is to **aggregate and load** the platform's essential utility libraries, which provide reusable functions for database operations, string manipulation, URL handling, caching, encoding, and more.

---

### **Purpose**
- **Namespace Declaration**: Defines the root `cms` namespace for all included libraries.
- **Dependency Resolution**: Loads all core utility libraries in a single, predictable order.
- **Modularity**: Enables lazy-loading of functionality by requiring only the necessary components in other parts of the system.

---

### **Included Libraries**

| Library File | Description |
|--------------|-------------|
| `common/codec.inc` | Encoding/decoding utilities (e.g., base64, JSON, serialization). |
| `common/comm.inc` | Communication utilities (e.g., HTTP requests, email, sockets). |
| `common/date.inc` | Date/time manipulation and formatting. |
| `common/file.inc` | File system operations (read/write, permissions, paths). |
| `common/hash.inc` | Cryptographic hashing (e.g., SHA, bcrypt, HMAC). |
| `common/ice.inc` | Internal caching engine (RAM + persistent storage). |
| `common/image.inc` | Image processing (resizing, cropping, format conversion). |
| `common/language.inc` | Localization and translation helpers. |
| `common/math.inc` | Mathematical operations and number formatting. |
| `common/misc.inc` | Miscellaneous utilities (e.g., UUID generation, random strings). |
| `common/mysql.inc` | Database abstraction layer (MySQL wrappers). |
| `common/snippet.inc` | Code snippet management (reusable templates). |
| `common/string.inc` | String manipulation (multibyte-safe, escaping, validation). |
| `common/text.inc` | Text processing (parsing, sanitization, typography). |
| `common/url.inc` | URL/parameter management (routing, escaping, state handling). |

---

### **Usage Context**
- **Automatic Loading**: This file is included **once** at the start of every PWNC request (typically via `index.php` or a bootstrap script).
- **Global Availability**: All functions from the included libraries become available in the `cms` namespace after loading.
- **Dependency Order**: Libraries are loaded in a **fixed order** to ensure dependencies are resolved (e.g., `mysql.inc` is loaded before `url.inc` to enable database-backed URL resolution).

---

### **Example: Loading and Using a Utility Function**
```php
<?php
// Bootstrap the platform (automatically loads sys.common.inc)
require_once "system/index.php";

// Use a function from the loaded libraries (e.g., URL generation)
$url = cms\cms_url("user/profile", ["id" => 42]);
echo "Profile URL: " . $url; // Output: /user/profile?id=42
```
**Explanation**:
1. `sys.common.inc` is loaded implicitly via `system/index.php`.
2. The `cms_url()` function (from `common/url.inc`) is called to generate a URL with parameters.
3. The function merges the provided parameters with the global state (e.g., CSRF tokens, language settings).

---

### **Key Notes**
- **No Direct Execution**: This file **cannot** be executed standalone—it must be included by another script.
- **Namespace Isolation**: All functions are scoped under `cms\` to avoid conflicts with userland code.
- **Performance**: Libraries are loaded **on-demand** in other parts of the system (e.g., `cms_load()`), but this file ensures they are available when needed.


<!-- HASH:f9b55bde3ccae61b333165bafaeb4f29 -->
