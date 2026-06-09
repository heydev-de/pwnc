# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/misc.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/misc.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Overview

This file (`#system/common/misc.inc`) provides a collection of utility functions for the PWNC Web Platform. These functions handle common tasks such as variable initialization, type checking, MIME type resolution, recursive array operations, IP anonymization, and content preview generation. The utilities are designed to be lightweight, context-aware, and reusable across the platform.

---

## Functions

### `each`

**Purpose:**
Replicates the deprecated `each()` function for PHP versions ≥ 7.2.0. Iterates over an array, returning the current key-value pair and advancing the internal pointer.

**Parameters:**

| Name   | Type   | Description                          |
|--------|--------|--------------------------------------|
| array  | array  | Reference to the array to iterate.   |

**Return Values:**
- `array|false`: Associative array with keys `0`, `1`, `key`, and `value` representing the current key-value pair, or `false` if the end of the array is reached.

**Inner Mechanisms:**
- Uses `key()`, `current()`, and `next()` to traverse the array.
- Returns `false` if the internal pointer is beyond the array bounds.

**Usage Context:**
- Used as a drop-in replacement for the deprecated `each()` function in legacy code.
- Useful for manual array iteration where pointer control is needed.

**Example:**
```php
$array = ['a' => 1, 'b' => 2];
while ($entry = each($array)) {
    echo "Key: {$entry['key']}, Value: {$entry['value']}\n";
}
// Output: Key: a, Value: 1
//         Key: b, Value: 2
```

---

### `init`

**Purpose:**
Initializes a variable with a default value if it is considered "blank" (undefined, empty, or falsy).

**Parameters:**

| Name            | Type   | Default | Description                          |
|-----------------|--------|---------|--------------------------------------|
| variable        | mixed  | —       | Reference to the variable to check.  |
| default_value   | mixed  | `NULL`  | Value to assign if variable is blank.|

**Return Values:**
- `mixed`: The initialized variable (either its original value or the default).

**Inner Mechanisms:**
- Delegates to `blank()` to determine if the variable is blank.
- Assigns the default value if the variable is blank.

**Usage Context:**
- Used to ensure variables have sensible defaults before use.
- Common in configuration or parameter handling.

**Example:**
```php
$color = '';
init($color, 'blue');
echo $color; // Output: blue
```

---

### `blank`

**Purpose:**
Determines if a variable is "blank" (undefined, empty string, or evaluates to falsy).

**Parameters:**

| Name     | Type  | Description                     |
|----------|-------|---------------------------------|
| variable | mixed | Reference to the variable to check. |

**Return Values:**
- `bool`: `true` if the variable is blank, `false` otherwise.

**Inner Mechanisms:**
- Checks for `isset()` first.
- Handles scalar types (boolean, integer, double, string) by casting to string and comparing to `""`.
- Falls back to `empty()` for other types.

**Usage Context:**
- Core utility for input validation and default value assignment.
- Used by `init()` and other platform functions.

**Example:**
```php
$var1 = 0;
$var2 = '';
var_dump(blank($var1)); // true
var_dump(blank($var2)); // true
```

---

### `yesno`

**Purpose:**
Converts a boolean value into a localized "Yes" or "No" string.

**Parameters:**

| Name     | Type    | Description                     |
|----------|---------|---------------------------------|
| boolean  | bool    | Boolean value to convert.       |

**Return Values:**
- `string`: Localized string (`CMS_L_COMMON_008` for `true`, `CMS_L_COMMON_009` for `false`).

**Usage Context:**
- Used in user interfaces to display boolean values in a human-readable format.

**Example:**
```php
echo yesno(true);  // Output: Yes (or localized equivalent)
echo yesno(false); // Output: No
```

---

### `option`

**Purpose:**
Returns a value if a condition is `true`, otherwise returns `NULL`.

**Parameters:**

| Name     | Type    | Description                     |
|----------|---------|---------------------------------|
| boolean  | bool    | Condition to evaluate.          |
| value    | mixed   | Value to return if condition is `true`. |

**Return Values:**
- `mixed|null`: The provided value if `boolean` is `true`, otherwise `NULL`.

**Usage Context:**
- Used for conditional value assignment in templates or configuration.

**Example:**
```php
$isAdmin = true;
$role = option($isAdmin, 'administrator');
echo $role; // Output: administrator
```

---

### `get_mime_type`

**Purpose:**
Resolves a file extension or filename to its corresponding MIME type.

**Parameters:**

| Name                  | Type   | Default | Description                                      |
|-----------------------|--------|---------|--------------------------------------------------|
| filename_or_extension | string | `NULL`  | File extension or filename. If `NULL`, returns the full MIME type list. |

**Return Values:**
- `string|array`: MIME type for the given extension, or the full list of MIME types if no argument is provided. Defaults to `application/octet-stream` for unknown extensions.

**Inner Mechanisms:**
- Loads MIME types from `CMS_PATH . "mimetype"` on first call.
- Uses `file_extension()` to extract the extension if a filename is provided.
- Validates the extension using a regex (`/^[0-9a-z]+$/i`).

**Usage Context:**
- Used for file uploads, downloads, and asset management.

**Example:**
```php
echo get_mime_type('image.png'); // Output: image/png
$allTypes = get_mime_type();     // Returns associative array of all MIME types
```

---

### `get_mime_list`

**Purpose:**
Generates a list of file extensions with their associated MIME type icons.

**Parameters:**
None.

**Return Values:**
- `array`: Associative array where keys are file extensions and values are paths to MIME type icons.

**Inner Mechanisms:**
- Calls `get_mime_type()` to retrieve the full MIME type list.
- Uses `get_mime_icon()` to resolve the icon path for each MIME type.

**Usage Context:**
- Used in file browsers or asset managers to display file type icons.

**Example:**
```php
$mimeList = get_mime_list();
print_r($mimeList['png']); // Output: mimetype/image
```

---

### `get_mime_icon`

**Purpose:**
Resolves a MIME type to its corresponding icon path.

**Parameters:**

| Name     | Type   | Default               | Description                     |
|----------|--------|-----------------------|---------------------------------|
| type     | string | —                     | MIME type (e.g., `image/png`).  |
| default  | string | `"mimetype/default"`  | Default icon path if no match is found. |

**Return Values:**
- `string`: Path to the MIME type icon.

**Inner Mechanisms:**
- Checks for an exact match (`mimetype/{type}`).
- Falls back to the primary type (`mimetype/{primary_type}`).
- Uses `image_exists()` to verify icon existence.

**Usage Context:**
- Used by `get_mime_list()` and UI components to display file type icons.

**Example:**
```php
echo get_mime_icon('image/png'); // Output: mimetype/image
```

---

### `in_array_recursive`

**Purpose:**
Searches for a value in a nested (multidimensional) array.

**Parameters:**

| Name     | Type    | Default | Description                     |
|----------|---------|---------|---------------------------------|
| needle   | mixed   | —       | Value to search for.            |
| haystack | array   | —       | Array to search.                |
| strict   | bool    | `false` | Whether to use strict comparison. |

**Return Values:**
- `bool`: `true` if the value is found, `false` otherwise.

**Inner Mechanisms:**
- Uses `RecursiveIteratorIterator` and `RecursiveArrayIterator` to traverse nested arrays.
- Supports both loose (`==`) and strict (`===`) comparison.

**Usage Context:**
- Used for deep array searches where the structure is unknown or variable.

**Example:**
```php
$array = ['a' => ['b' => ['c' => 1]]];
var_dump(in_array_recursive(1, $array)); // true
```

---

### `ksort_recursive`

**Purpose:**
Recursively sorts an array by keys while maintaining nested structure.

**Parameters:**

| Name   | Type   | Default       | Description                     |
|--------|--------|---------------|---------------------------------|
| array  | array  | —             | Reference to the array to sort. |
| flag   | int    | `SORT_REGULAR`| Sorting flags (e.g., `SORT_STRING`). |

**Return Values:**
None (modifies the array in-place).

**Inner Mechanisms:**
- Calls `ksort()` on the top-level array.
- Recursively processes nested arrays.

**Usage Context:**
- Used to normalize array structures for consistent output or comparison.

**Example:**
```php
$array = ['b' => ['d' => 2, 'c' => 1], 'a' => 0];
ksort_recursive($array);
print_r($array);
// Output: ['a' => 0, 'b' => ['c' => 1, 'd' => 2]]
```

---

### `bitstring`

**Purpose:**
Converts a binary string into its binary representation (e.g., `"A"` → `"01000001"`).

**Parameters:**

| Name    | Type   | Default | Description                     |
|---------|--------|---------|---------------------------------|
| value   | string | —       | Binary string to convert.       |
| length  | int    | `NULL`  | Truncate the result to this length (from the right). |

**Return Values:**
- `string`: Binary representation of the input string.

**Inner Mechanisms:**
- Iterates over each byte in the string.
- Converts each byte to its 8-bit binary representation using bitwise operations.

**Usage Context:**
- Used for debugging or low-level data inspection.

**Example:**
```php
echo bitstring('A'); // Output: 01000001
echo bitstring('A', 4); // Output: 0001
```

---

### `set_time_limit`

**Purpose:**
Extends the script execution time limit if safe mode is disabled and the requested time exceeds the current limit.

**Parameters:**

| Name | Type | Description                     |
|------|------|---------------------------------|
| time | int  | Desired execution time in seconds. |

**Return Values:**
None.

**Inner Mechanisms:**
- Checks `safe_mode` and `max_execution_time` INI settings.
- Calls PHP’s `set_time_limit()` only if conditions are met.

**Usage Context:**
- Used in long-running scripts (e.g., imports, exports, or batch processing).

**Example:**
```php
set_time_limit(300); // Extend time limit to 5 minutes
```

---

### `anonymize_ip`

**Purpose:**
Anonymizes an IP address by zeroing the last octet (IPv4) or the last 80 bits (IPv6).

**Parameters:**

| Name     | Type   | Description                     |
|----------|--------|---------------------------------|
| address  | string | IP address to anonymize.        |

**Return Values:**
- `string`: Anonymized IP address (e.g., `192.168.1.0` for `192.168.1.42`).

**Inner Mechanisms:**
- Uses `inet_pton()` to convert the IP address to binary format.
- Zeroes the last portion of the address and converts back to string format using `inet_ntop()`.

**Usage Context:**
- Used for privacy compliance (e.g., GDPR) in logging or analytics.

**Example:**
```php
echo anonymize_ip('192.168.1.42'); // Output: 192.168.1.0
echo anonymize_ip('2001:db8::42'); // Output: 2001:db8::
```

---

### `preview`

**Purpose:**
Generates a standalone HTML preview of a content snippet with optional styling.

**Parameters:**

| Name         | Type   | Default               | Description                     |
|--------------|--------|-----------------------|---------------------------------|
| content_code | string | —                     | HTML content to preview.        |
| stylesheet   | string | `CMS_DATA_URL . "stylesheet.css"` | Path to the stylesheet. |
| body_class   | string | `"preview"`           | CSS class for the `<body>` tag. |

**Return Values:**
None (outputs HTML directly).

**Inner Mechanisms:**
- Outputs a complete HTML document with meta tags, base URL, and scripts.
- Escapes dynamic content using `x()` for XML safety.
- Includes platform JavaScript files (`common.js`, `fx.js`, `defer.js`).

**Usage Context:**
- Used for WYSIWYG previews, email templates, or content previews.

**Example:**
```php
preview('<h1>Hello, World!</h1>', 'custom.css', 'preview-dark');
```

---

### `preview_inert`

**Purpose:**
Generates a sandboxed HTML preview using an `<iframe>` with the `inert` attribute for isolation.

**Parameters:**

| Name         | Type   | Description                     |
|--------------|--------|---------------------------------|
| content_code | string | HTML content to preview.        |

**Return Values:**
None (outputs HTML directly).

**Inner Mechanisms:**
- Outputs a minimal HTML document with an `<iframe>`.
- Uses `srcdoc` to embed the content and `sandbox` for security.
- Sets the `inert` attribute on the iframe’s document to prevent interaction.

**Usage Context:**
- Used for secure previews of untrusted content (e.g., user-generated HTML).

**Example:**
```php
preview_inert('<script>alert("XSS")</script><p>Safe content</p>');
```

---

### `force_flush`

**Purpose:**
Forces output buffering to flush, ensuring all content is sent to the client immediately.

**Parameters:**

| Name | Type | Default | Description                     |
|------|------|---------|---------------------------------|
| size | int  | `65536` | Size of padding data to send.   |

**Return Values:**
None.

**Inner Mechanisms:**
- Outputs a `<script type="text/plain">` block with random padding data.
- Ends all output buffers and flushes the output.

**Usage Context:**
- Used in long-running scripts to provide real-time feedback to the user.

**Example:**
```php
echo "Processing...";
force_flush();
sleep(2);
echo "Done!";
```


<!-- HASH:f7138a31638c092b7c511ebf5b4a57b7 -->
