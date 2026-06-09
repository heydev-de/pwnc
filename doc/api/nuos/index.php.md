# PWNC API Documentation

[← Index](../README.md) | [`nuos/index.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/nuos/index.php)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## File Overview: `nuos/index.php`

This file serves as a **permanent redirect** from the `/nuos/` directory to the `/pwnc/` directory within the PWNC Web Platform. It uses an HTTP 301 (Moved Permanently) status code to instruct browsers and search engines to update their references to the new location.

This is a **minimalist, non-interactive** file with no functions, classes, or reusable logic. Its sole purpose is to enforce a consistent URL structure by redirecting legacy or alternative paths to the canonical entry point of the platform (`pwnc/index.php`).

---

## Core Mechanism

### HTTP 301 Redirect
The file issues a **server-side redirect** using PHP's `header()` function with the following components:

| Component | Value | Description |
|-----------|-------|-------------|
| **Location** | `../pwnc/index.php` | Target path relative to the current file (`nuos/index.php`). Resolves to the platform's main entry point. |
| **Replace** | `TRUE` | Ensures the new header replaces any existing `Location` header, preventing conflicts. |
| **HTTP Status Code** | `301` | Indicates a **permanent redirect**, signaling to clients (browsers, crawlers) that the resource has moved permanently. |

The `exit()` call immediately terminates script execution to prevent further output or processing.

---

## Usage Context

### When to Use
- **Legacy Path Handling**: Redirect users or bots accessing deprecated or alternative paths (e.g., `/nuos/`) to the canonical location.
- **URL Normalization**: Enforce a single entry point for the application to simplify routing and maintenance.
- **SEO Optimization**: Use 301 redirects to consolidate link equity and avoid duplicate content penalties.

### Typical Scenarios
1. **Platform Rebranding or Restructuring**:
   If the platform was previously accessible via `/nuos/` (e.g., during development or under a different name), this redirect ensures backward compatibility.
2. **Directory Consolidation**:
   Merging multiple entry points (e.g., `/nuos/`, `/pwnc/`) into one to streamline access and reduce confusion.

---

## Example Usage

### Scenario: Redirecting a Legacy Path
**File**: `nuos/index.php`
```php
<?php
header("Location: ../pwnc/index.php", TRUE, 301);
exit();
```

**Explanation**:
- A user or crawler requests `https://example.com/nuos/index.php`.
- The server responds with a **301 Moved Permanently** header, redirecting the client to `https://example.com/pwnc/index.php`.
- The client updates its bookmarks or indexes to use the new URL.

---

## Notes
- **No Dynamic Logic**: This file contains no parameters, functions, or conditional logic. It is a static redirect.
- **Performance**: The redirect is **server-side**, minimizing client-side processing and latency.
- **Security**: No user input is processed, eliminating risks like header injection or path traversal.
- **Alternatives**: For temporary redirects, use HTTP 302 or 307. For dynamic redirects, combine `header()` with logic (e.g., database lookups).


<!-- HASH:02958ea04807c90992ad8f826801a060 -->
