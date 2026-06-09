# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/snippet.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/snippet.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Snippet Utilities (`snippet.inc`)

This file provides utility functions for generating common UI components and handling frontend interactions in the PWNC Web Platform. These functions abstract repetitive HTML generation, permission checks, and dynamic content insertion, ensuring consistency and security across the platform.

---

## Functions

### `permission`

Displays a permission management interface for the current application or a specific permission key.

#### Parameters

| Name    | Value/Default | Description                                                                 |
|---------|---------------|-----------------------------------------------------------------------------|
| `$array` | -             | Associative array of permission keys (e.g., `"read" => "Read Access"`).     |
| `$return` | `FALSE`       | If `TRUE`, returns the HTML as a string; otherwise, echoes it directly.     |

#### Return Values

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| `void`  | If `$return` is `FALSE` (default), echoes the HTML directly.               |
| `string`| If `$return` is `TRUE`, returns the generated HTML as a string.            |

#### Inner Mechanisms

1. **Visibility Check**: Hides the interface if the user is in editing view mode (`CMS_TEMPLATE_OPTION_NONE`).
2. **Permission Check**: Verifies if the user has the `interface.permission` permission.
3. **Application Context**: Retrieves the current application name using `cms_application()`.
4. **HTML Generation**: Constructs a `<div>` with links to the permission management interface for each key in `$array`.
5. **URL Construction**: Uses `cms_url()` to generate secure, CSRF-protected links for permission updates.

#### Usage Context

- **Typical Scenario**: Displaying a permission management UI for an application or module.
- **Example**:
  ```php
  // Display permission links for "read" and "write" access
  permission([
      "read" => "Read Access",
      "write" => "Write Access"
  ]);
  ```
  **Explanation**: Renders a clickable interface allowing users to manage permissions for the current application. Each link redirects to the permission management page with the appropriate parameters.

---

### `insert`

Dynamically inserts content or an edit button for a specific position in the current application.

#### Parameters

| Name       | Value/Default | Description                                                                 |
|------------|---------------|-----------------------------------------------------------------------------|
| `$position` | `NULL`        | Optional sub-key for the insertion point (e.g., `"header"`).               |

#### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| `void` | Echoes the HTML directly.                                                  |

#### Inner Mechanisms

1. **Visibility Check**: Hides the interface if in editing view mode (`CMS_TEMPLATE_OPTION_NONE`).
2. **Key Construction**: Combines the application name with `$position` (if provided) to form a unique key.
3. **Data Retrieval**: Fetches insertion data from the `#system/insert` storage.
4. **Edit Button**: Displays an edit button if the user has `interface.insert` permission and is in editing mode.
5. **Content Insertion**: If content is assigned to the key, retrieves and renders it from `#system/insert.code`.

#### Usage Context

- **Typical Scenario**: Embedding dynamic content (e.g., banners, widgets) or providing an edit interface for such content.
- **Example**:
  ```php
  // Insert content for the "header" position
  insert("header");
  ```
  **Explanation**: Renders either the stored content for the `header` position or an edit button if the user has permission and is in editing mode.

---

### `class_varied`

Generates alternating CSS class names for styling repeated elements (e.g., table rows).

#### Parameters

| Name     | Value/Default | Description                                                                 |
|----------|---------------|-----------------------------------------------------------------------------|
| `$option` | `NULL`        | If `TRUE`, resets the alternation flag. If a string, appends it to the class. |
| `$index`  | `0`           | Index for managing multiple independent alternation sequences.             |

#### Return Values

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Returns a `class` attribute with alternating values (`"varied"` or custom). |

#### Inner Mechanisms

1. **Static Flag**: Uses a static array to track alternation state across function calls.
2. **Index Handling**: Supports multiple independent sequences via `$index`.
3. **Class Construction**: Returns `" class=\"varied\""` or a custom class string if `$option` is provided.

#### Usage Context

- **Typical Scenario**: Styling alternating rows in tables or lists for better readability.
- **Example**:
  ```php
  // Alternate row classes in a table
  foreach ($items as $i => $item) {
      echo "<tr" . class_varied("row", $i) . "><td>" . x($item) . "</td></tr>";
  }
  ```
  **Explanation**: Alternates between `<tr class="row">` and `<tr class="row varied">` for each row.

---

### `jscript`

Wraps JavaScript code in `<script>` tags with automatic escaping for closing tags.

#### Parameters

| Name   | Value/Default | Description                                                                 |
|--------|---------------|-----------------------------------------------------------------------------|
| `$code` | -             | JavaScript code to wrap.                                                    |

#### Return Values

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Returns the code wrapped in `<script>` tags.                               |

#### Inner Mechanisms

1. **Escaping**: Replaces `</` with `<\/` to prevent premature script termination.
2. **Tag Wrapping**: Encloses the code in `<script>` and `</script>`.

#### Usage Context

- **Typical Scenario**: Embedding inline JavaScript in HTML.
- **Example**:
  ```php
  echo jscript("alert('Hello, world!');");
  ```
  **Explanation**: Outputs `<script>alert('Hello, world!');</script>`.

---

### `stylesheet`

Generates a `<link>` tag for CSS stylesheets with optional asynchronous loading.

#### Parameters

| Name     | Value/Default | Description                                                                 |
|----------|---------------|-----------------------------------------------------------------------------|
| `$url`    | -             | URL of the stylesheet.                                                      |
| `$async`  | `TRUE`        | If `TRUE`, uses `preload` with `onload` for asynchronous loading.           |

#### Return Values

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Returns the `<link>` tag(s) for the stylesheet.                            |

#### Inner Mechanisms

1. **URL Escaping**: Escapes the URL using `x()`.
2. **Async Loading**: Uses `<link rel="preload">` with `onload` to switch to `stylesheet` if `$async` is `TRUE`.
3. **Fallback**: Includes a `<noscript>` fallback for browsers without JavaScript.

#### Usage Context

- **Typical Scenario**: Loading CSS files with performance optimizations.
- **Example**:
  ```php
  echo stylesheet("styles/main.css");
  ```
  **Explanation**: Outputs a `<link>` tag with async loading for `styles/main.css`.

---

### `javascript`

Generates a `<script>` tag for external JavaScript files with optional `async` or `defer`.

#### Parameters

| Name     | Value/Default | Description                                                                 |
|----------|---------------|-----------------------------------------------------------------------------|
| `$url`    | -             | URL of the JavaScript file.                                                 |
| `$async`  | `TRUE`        | If `TRUE`, adds the `async` attribute.                                      |
| `$defer`  | `FALSE`       | If `TRUE`, adds the `defer` attribute.                                      |

#### Return Values

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Returns the `<script>` tag for the JavaScript file.                        |

#### Inner Mechanisms

1. **URL Escaping**: Escapes the URL using `x()`.
2. **Attribute Handling**: Adds `async` or `defer` based on parameters.

#### Usage Context

- **Typical Scenario**: Loading external JavaScript files with execution control.
- **Example**:
  ```php
  echo javascript("scripts/app.js", TRUE, FALSE);
  ```
  **Explanation**: Outputs `<script src="scripts/app.js" async></script>`.

---

### `select`

Generates an HTML `<select>` dropdown with options.

#### Parameters

| Name         | Value/Default | Description                                                                 |
|--------------|---------------|-----------------------------------------------------------------------------|
| `$option`     | -             | Associative array of options (keys as values, values as labels).           |
| `$preset`     | `NULL`        | Preselected value(s). Can be a string or an array for multiple selections.  |
| `$name`       | `NULL`        | Name attribute for the `<select>` element.                                  |
| `$set_value`  | `FALSE`       | If `TRUE`, uses option values as both values and labels.                    |
| `$disabled`   | `FALSE`       | If `TRUE`, disables the dropdown.                                           |
| `$height`     | `NULL`        | If set, enables multiple selection with the specified height.               |

#### Return Values

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `bool`   | Returns `TRUE` on success, `FALSE` if `$option` is not an array.           |

#### Inner Mechanisms

1. **Validation**: Checks if `$option` is an array.
2. **Name Handling**: Appends `[]` to the name if `$height` is set (for multiple selections).
3. **Option Generation**: Iterates over `$option` to create `<option>` tags.
4. **Preset Handling**: Marks options as `selected` if they match `$preset`.

#### Usage Context

- **Typical Scenario**: Creating dropdown menus for forms.
- **Example**:
  ```php
  $options = ["red" => "Red", "green" => "Green", "blue" => "Blue"];
  select($options, "green", "color");
  ```
  **Explanation**: Generates a dropdown with "Green" preselected.

---

### `info`

Generates an informational message with an icon.

#### Parameters

| Name   | Value/Default | Description                                                                 |
|--------|---------------|-----------------------------------------------------------------------------|
| `$text` | -             | Message text.                                                               |

#### Return Values

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Returns the HTML for the info message.                                      |

#### Inner Mechanisms

1. **Icon**: Uses `image("icon_info")` for the info icon.
2. **Styling**: Wraps the text in a `<div>` with a `<strong>` tag.

#### Usage Context

- **Typical Scenario**: Displaying informational messages to users.
- **Example**:
  ```php
  echo info("Your changes have been saved.");
  ```
  **Explanation**: Outputs a styled info message with an icon.

---

### `alert`

Generates an alert message with an icon and red text.

#### Parameters

| Name   | Value/Default | Description                                                                 |
|--------|---------------|-----------------------------------------------------------------------------|
| `$text` | -             | Alert text.                                                                 |

#### Return Values

| Type     | Description                                                                 |
|----------|-----------------------------------------------------------------------------|
| `string` | Returns the HTML for the alert message.                                     |

#### Inner Mechanisms

1. **Icon**: Uses `image("icon_alert")` for the alert icon.
2. **Styling**: Wraps the text in a `<div>` with a red `<strong>` tag.

#### Usage Context

- **Typical Scenario**: Displaying error or warning messages.
- **Example**:
  ```php
  echo alert("Invalid input detected.");
  ```
  **Explanation**: Outputs a styled alert message with an icon.

---

### `pagination`

Generates a pagination navigation bar.

#### Parameters

| Name      | Value/Default               | Description                                                                 |
|-----------|-----------------------------|-----------------------------------------------------------------------------|
| `$url`     | -                           | URL template with `%page%` placeholder for page numbers.                   |
| `$page`    | -                           | Current page number.                                                        |
| `$count`   | -                           | Total number of pages.                                                      |
| `$next`    | `CMS_L_COMMAND_NEXT`        | Text for the "next" button.                                                 |
| `$class`   | `NULL`                      | CSS class for the `<nav>` element.                                          |
| `$offset`  | `0`                         | Starting page number (e.g., `1` for 1-based indexing).                      |

#### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| `void` | Echoes the pagination HTML directly.                                       |

#### Inner Mechanisms

1. **Edge Cases**: Returns early if `$count` is `1` or less.
2. **Page Calculation**: Adjusts `$page` to stay within bounds.
3. **Range Calculation**: Determines the range of pages to display around the current page.
4. **HTML Generation**: Constructs `<nav>` with links for previous, next, first, last, and individual pages.

#### Usage Context

- **Typical Scenario**: Navigating large datasets (e.g., search results, lists).
- **Example**:
  ```php
  pagination("?page=%page%", 3, 10, "Next", "pagination");
  ```
  **Explanation**: Generates pagination for 10 pages with the current page set to 3.

---

### `language_selector`

Generates a language selection interface with flags or icons.

#### Parameters

| Name        | Value/Default                          | Description                                                                 |
|-------------|----------------------------------------|-----------------------------------------------------------------------------|
| `$language`  | `NULL`                                 | Currently selected language.                                                |
| `$url`       | `"javascript:console.log('%language%');"` | URL template with `%language%` placeholder.                                |
| `$encoding`  | `NULL` (defaults to `q()`)             | Function to encode language values (e.g., `q`, `x`).                        |
| `$width`     | `16`                                   | Width of the language icons.                                                |
| `$height`    | `12`                                   | Height of the language icons.                                               |

#### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| `void` | Echoes the language selector HTML directly.                                |

#### Inner Mechanisms

1. **Early Return**: Exits if multilingual support is disabled (`CMS_LANGUAGE_ENABLED` is empty).
2. **Default Language**: Displays a default language option if no language is selected.
3. **Language Iteration**: Loops through enabled languages (`CMS_LANGUAGE_ENABLED`).
4. **Icon Handling**: Uses language-specific icons from `#system/language.image` or a default icon.
5. **URL Construction**: Replaces `%language%` in `$url` with the encoded language value.

#### Usage Context

- **Typical Scenario**: Providing a language switcher for multilingual sites.
- **Example**:
  ```php
  language_selector("en", "?lang=%language%");
  ```
  **Explanation**: Generates a language selector with English preselected and links to switch languages.


<!-- HASH:f88f4cc80242723f55b8a15c2ea16811 -->
