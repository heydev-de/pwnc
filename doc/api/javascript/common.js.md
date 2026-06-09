# PWNC API Documentation

[← Index](../README.md) | [`javascript/common.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/common.js)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## JavaScript Utility Functions for PWNC Web Platform

This file (`common.js`) provides core JavaScript utility functions for the PWNC Web Platform, covering string manipulation, text formatting controls, page navigation, cookie management, and various helper functions. These utilities follow PWNC's no-bloat, zero-dependency philosophy while ensuring high performance and flexibility.

---

## **1. STRING**

Functions for string manipulation, formatting, and generation.

---

### **`string_repeat(string, count)`**

**Purpose:**
Repeats a given string a specified number of times.

**Parameters:**

| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| string  | string | The string to repeat.                |
| count   | number | The number of times to repeat.       |

**Return Value:**
`string` – The repeated string.

**Inner Mechanisms:**
Uses the native `String.prototype.repeat()` method.

**Usage Context:**
Useful for generating padding, separators, or repeated patterns in dynamic content.

**Example:**
```javascript
// Generate a separator line
const separator = string_repeat("-", 50);
console.log(separator); // "--------------------------------------------------"
```

---

### **`htmlspecialchars(string)`**

**Purpose:**
Escapes special HTML characters to their corresponding HTML entities to prevent XSS attacks and ensure proper rendering.

**Parameters:**

| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| string  | string | The input string to escape.          |

**Return Value:**
`string` – The escaped string.

**Inner Mechanisms:**
Replaces `&`, `"`, `'`, `<`, and `>` with their HTML entity equivalents using a regex-based replacement.

**Usage Context:**
Essential for safely outputting user-generated content in HTML contexts.

**Example:**
```javascript
const userInput = '<script>alert("XSS")</script>';
const safeOutput = htmlspecialchars(userInput);
document.body.innerHTML = safeOutput; // Renders as text, not script
```

---

### **`unique_id(count)`**

**Purpose:**
Generates a random alphanumeric string of a specified length.

**Parameters:**

| Name  | Type   | Default | Description                          |
|-------|--------|---------|--------------------------------------|
| count | number | —       | Length of the generated ID.          |

**Return Value:**
`string` – A random alphanumeric string.

**Inner Mechanisms:**
Uses `Math.random()` to select characters from a predefined set (`0-9A-Za-z`).

**Usage Context:**
Useful for generating unique IDs for DOM elements, temporary keys, or nonces.

**Example:**
```javascript
const tempId = unique_id(16);
console.log(tempId); // e.g., "aB3x9FgH2pQ7rT5y"
```

---

### **`strabridge(string, length = 50, cut_end = false)`**

**Purpose:**
Truncates a string to a specified length, adding an ellipsis (`…`) to indicate truncation. Supports smart truncation (middle or end).

**Parameters:**

| Name     | Type    | Default | Description                                      |
|----------|---------|---------|--------------------------------------------------|
| string   | string  | —       | The input string.                                |
| length   | number  | 50      | Maximum length of the output string.             |
| cut_end  | boolean | false   | If `true`, truncates from the end; otherwise, middle. |

**Return Value:**
`string` – The truncated string with ellipsis.

**Inner Mechanisms:**
- If `cut_end` is `true`, truncates from the end.
- Otherwise, splits the string into 65% start and 35% end, joining with `…`.

**Usage Context:**
Useful for displaying long strings in UIs (e.g., filenames, URLs, or descriptions).

**Example:**
```javascript
const longText = "This is a very long string that needs to be truncated for display.";
console.log(strabridge(longText, 30));
// "This is a very long …for display."
console.log(strabridge(longText, 30, true));
// "This is a very long strin …"
```

---

### **`addslashes(string)`**

**Purpose:**
Escapes quotes and backslashes in a string to make it safe for JavaScript string literals.

**Parameters:**

| Name   | Type   | Description                          |
|--------|--------|--------------------------------------|
| string | string | The input string to escape.          |

**Return Value:**
`string` – The escaped string.

**Inner Mechanisms:**
Uses `JSON.stringify()` to escape special characters, then strips the surrounding quotes.

**Usage Context:**
Useful for embedding dynamic strings in JavaScript code or inline event handlers.

**Example:**
```javascript
const userInput = 'Hello "World"';
const escaped = addslashes(userInput);
console.log(escaped); // Hello \"World\"
```

---

### **`strtocolor(string, lightness = 75, diff_min = 0)`**

**Purpose:**
Generates a deterministic HSL color from a string, ensuring adjacent colors are visually distinct.

**Parameters:**

| Name      | Type   | Default | Description                                      |
|-----------|--------|---------|--------------------------------------------------|
| string    | string | —       | The input string (used as seed).                 |
| lightness | number | 75      | Lightness value (0–100) for the HSL color.       |
| diff_min  | number | 0       | Minimum hue difference (in degrees) from previous color. |

**Return Value:**
`string` – An HSL color string (e.g., `"hsl(120, 75%, 75%)"`).

**Inner Mechanisms:**
- Uses the `djb2` hash function to generate a hue value.
- Adjusts hue to ensure minimum difference from the last generated color.
- Maintains state via `strtocolor.hue` to track the last hue.

**Usage Context:**
Useful for generating consistent, visually distinct colors for tags, categories, or user avatars.

**Example:**
```javascript
console.log(strtocolor("admin"));    // e.g., "hsl(120, 75%, 75%)"
console.log(strtocolor("moderator")); // e.g., "hsl(240, 75%, 75%)" (distinct from "admin")
```

---

## **2. TEXTCONTROL**

Functions for rich text formatting in textareas or contenteditable elements.

---

### **`textcontrol(object, image_path, extension = "")`**

**Purpose:**
Creates a toolbar for rich text formatting (bold, italic, links, images, etc.) and inserts it into the DOM near the target input element.

**Parameters:**

| Name       | Type   | Default | Description                                      |
|------------|--------|---------|--------------------------------------------------|
| object     | string | —       | CSS selector for the target input element.       |
| image_path | string | —       | Base path to the textcontrol icon images.        |
| extension  | string | ""      | Additional HTML to append to the toolbar.        |

**Return Value:**
`void` – Inserts the toolbar into the DOM.

**Inner Mechanisms:**
- Creates a `div` with class `textcontrol`.
- Adds buttons for formatting options (bold, italic, links, etc.).
- Uses `addslashes` to escape the target selector for JavaScript.
- Inserts the toolbar before the current `<script>` tag (if available).

**Usage Context:**
Useful for adding WYSIWYG-like formatting to textareas or contenteditable elements.

**Example:**
```html
<textarea id="content"></textarea>
<script>
    textcontrol("#content", "/images/");
</script>
```

---

### **`textcontrol_set(object, format, data = "")`**

**Purpose:**
Applies formatting to selected text in a target input element (textarea or contenteditable) using PWNC's custom markup syntax.

**Parameters:**

| Name    | Type   | Default | Description                                      |
|---------|--------|---------|--------------------------------------------------|
| object  | string | —       | CSS selector for the target input element.       |
| format  | string | —       | Formatting command (e.g., `+`, `#link`, `#remove`). |
| data    | string | ""      | Additional data for the format (e.g., URL for links). |

**Return Value:**
`void` – Modifies the target element's content.

**Inner Mechanisms:**
- Detects the selection range in the target element.
- Applies formatting based on the `format` parameter (e.g., `[bold text]` for `+`).
- Handles special cases like links, images, and tables.
- Preserves cursor position and scroll state.

**Usage Context:**
Used internally by `textcontrol` to apply formatting when buttons are clicked.

**Example:**
```javascript
// Apply bold formatting to selected text in a textarea
textcontrol_set("#content", "+");
```

---

### **`textcontrol_remove(text, start, end)`**

**Purpose:**
Removes PWNC's custom formatting markup from a selected range in a text string.

**Parameters:**

| Name  | Type   | Description                                      |
|-------|--------|--------------------------------------------------|
| text  | string | The input text with formatting markup.           |
| start | number | Start index of the selection.                    |
| end   | number | End index of the selection.                      |

**Return Value:**
`Array` – `[cleanedText, newStart, newEnd]` where:
- `cleanedText`: The text with formatting removed.
- `newStart`: Adjusted start index after cleaning.
- `newEnd`: Adjusted end index after cleaning.

**Inner Mechanisms:**
- Parses the text to identify PWNC's custom markup (e.g., `[bold text]`).
- Removes formatting while preserving the underlying text.
- Handles nested formatting and tables.

**Usage Context:**
Used by `textcontrol_set` to remove formatting when the `#remove` command is invoked.

**Example:**
```javascript
const formattedText = "This is [+ bold] text.";
const [cleanedText, start, end] = textcontrol_remove(formattedText, 8, 14);
console.log(cleanedText); // "This is bold text."
```

---

## **3. LOCATION**

Functions for page navigation and window management.

---

### **`load_page(url, target)`**

**Purpose:**
Opens a URL in a new window or tab, centered on the screen. Handles external URLs securely.

**Parameters:**

| Name    | Type   | Default   | Description                                      |
|---------|--------|-----------|--------------------------------------------------|
| url     | string | —         | The URL to open.                                 |
| target  | string | `_blank`  | The target window name (or URL if omitted).      |

**Return Value:**
`void` – Opens a new window.

**Inner Mechanisms:**
- Detects external URLs using the `URL` API.
- Centers the window on the screen.
- Adds security attributes (`noopener`, `noreferrer`) for external URLs.
- Listens for the `load` event to adjust window size.

**Usage Context:**
Useful for opening external links or popups in a controlled manner.

**Example:**
```javascript
// Open an internal page in a centered popup
load_page("/profile", "profile_window");

// Open an external URL securely
load_page("https://example.com");
```

---

## **4. FORM**

Functions for form input control.

---

### **`limit(object, limit)`**

**Purpose:**
Enforces a maximum character limit on a text input or textarea, preventing further input beyond the limit.

**Parameters:**

| Name  | Type          | Description                                      |
|-------|---------------|--------------------------------------------------|
| object| HTMLInputElement or HTMLTextAreaElement | The input element to limit. |
| limit | number        | Maximum allowed characters.                      |

**Return Value:**
`void` – Truncates the input value if it exceeds the limit.

**Inner Mechanisms:**
- Checks the input's length and truncates if necessary.
- Preserves the cursor position and selection range.

**Usage Context:**
Useful for enforcing character limits in forms (e.g., tweets, comments).

**Example:**
```html
<textarea id="comment" maxlength="200"></textarea>
<script>
    document.getElementById("comment").addEventListener("input", function() {
        limit(this, 200);
    });
</script>
```

---

## **5. COOKIE**

Functions for cookie management.

---

### **`getcookie(name)`**

**Purpose:**
Retrieves the value of a cookie by name.

**Parameters:**

| Name | Type   | Description                          |
|------|--------|--------------------------------------|
| name | string | The name of the cookie.              |

**Return Value:**
`string` – The cookie value, or an empty string if not found.

**Inner Mechanisms:**
Uses a regex to parse `document.cookie`.

**Usage Context:**
Useful for reading user preferences or session data.

**Example:**
```javascript
const theme = getcookie("theme");
console.log(theme); // e.g., "dark"
```

---

### **`setcookie(name, value, expires = null)`**

**Purpose:**
Sets a cookie with a name, value, and optional expiration.

**Parameters:**

| Name     | Type            | Default | Description                                      |
|----------|-----------------|---------|--------------------------------------------------|
| name     | string          | —       | The cookie name.                                 |
| value    | string          | —       | The cookie value.                                |
| expires  | Date or boolean | null    | Expiration date or `true` for session-only.      |

**Return Value:**
`void` – Sets the cookie via `document.cookie`.

**Inner Mechanisms:**
- Encodes the value using `encodeURIComponent`.
- Sets `path=/` and `samesite=Strict` for security.
- Adds `secure` flag for HTTPS connections.

**Usage Context:**
Useful for storing user preferences or session data.

**Example:**
```javascript
// Set a persistent cookie
setcookie("theme", "dark", new Date(Date.now() + 30 * 24 * 60 * 60 * 1000));

// Set a session cookie
setcookie("session_id", "abc123", true);
```

---

### **`delcookie(name)`**

**Purpose:**
Deletes a cookie by setting its expiration to a past date.

**Parameters:**

| Name | Type   | Description                          |
|------|--------|--------------------------------------|
| name | string | The name of the cookie to delete.    |

**Return Value:**
`void` – Deletes the cookie via `document.cookie`.

**Inner Mechanisms:**
Sets the cookie's expiration to `Thu, 01 Jan 1970 00:00:00 GMT`.

**Usage Context:**
Useful for logging out users or clearing preferences.

**Example:**
```javascript
delcookie("session_id");
```

---

## **6. VARIOUS**

Miscellaneous utility functions.

---

### **`djb2(string)`**

**Purpose:**
Computes a 32-bit hash of a string using the DJB2 algorithm.

**Parameters:**

| Name   | Type   | Description                          |
|--------|--------|--------------------------------------|
| string | string | The input string to hash.            |

**Return Value:**
`number` – The 32-bit hash value.

**Inner Mechanisms:**
- Converts the string to UTF-8 bytes.
- Applies the DJB2 hashing algorithm.

**Usage Context:**
Used by `strtocolor` to generate deterministic colors.

**Example:**
```javascript
console.log(djb2("admin")); // e.g., 210714636432
```

---

### **`crc32(string)`**

**Purpose:**
Computes a 32-bit CRC hash of a string.

**Parameters:**

| Name   | Type   | Description                          |
|--------|--------|--------------------------------------|
| string | string | The input string to hash.            |

**Return Value:**
`number` – The 32-bit CRC value.

**Inner Mechanisms:**
- Converts the string to UTF-8 bytes.
- Applies the CRC32 algorithm.

**Usage Context:**
Useful for checksums or quick data integrity checks.

**Example:**
```javascript
console.log(crc32("data")); // e.g., 3058687959
```

---

### **`load_script(url, onload)`**

**Purpose:**
Dynamically loads a JavaScript file and executes a callback on load.

**Parameters:**

| Name   | Type     | Default | Description                                      |
|--------|----------|---------|--------------------------------------------------|
| url    | string   | —       | The URL of the script to load.                   |
| onload | function | —       | Callback to execute after the script loads.      |

**Return Value:**
`void` – Appends the script to `document.head`.

**Inner Mechanisms:**
- Checks if the script is already loaded.
- Sets `async=true` for non-blocking loading.

**Usage Context:**
Useful for lazy-loading JavaScript libraries.

**Example:**
```javascript
load_script("/js/analytics.js", function() {
    console.log("Analytics script loaded.");
});
```

---

### **`load_css(url)`**

**Purpose:**
Dynamically loads a CSS file.

**Parameters:**

| Name | Type   | Description                          |
|------|--------|--------------------------------------|
| url  | string | The URL of the CSS file to load.     |

**Return Value:**
`void` – Appends the CSS link to `document.head`.

**Inner Mechanisms:**
- Uses `rel="preload"` for performance.
- Switches to `rel="stylesheet"` after loading.

**Usage Context:**
Useful for lazy-loading stylesheets.

**Example:**
```javascript
load_css("/css/theme-dark.css");
```

---

### **`document_write(output)`**

**Purpose:**
Inserts HTML content into the DOM at the current script's position.

**Parameters:**

| Name   | Type   | Description                          |
|--------|--------|--------------------------------------|
| output | string | The HTML content to insert.          |

**Return Value:**
`void` – Inserts the HTML before the current script.

**Inner Mechanisms:**
Uses `insertAdjacentHTML` to insert content before the current `<script>` tag.

**Usage Context:**
Useful for injecting dynamic content during script execution.

**Example:**
```javascript
document_write("<div>Dynamic content</div>");
```


<!-- HASH:cc6e52357eb6b3077617610f0dd8b7c4 -->
