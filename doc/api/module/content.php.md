# PWNC API Documentation

[← Index](../README.md) | [`module/content.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/content.php)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Content Module (`module/content.php`)

The **Content Module** is the core component of the PWNC Web Platform responsible for handling content delivery, rendering, and editing. It processes requests for content pages, manages permissions, handles redirects, and provides an interactive editing interface for authorized users.

This module:
- Serves published content to visitors
- Provides a real-time editing interface for content writers
- Handles URL resolution and redirects (including directory-based routing)
- Manages content versioning, undo/redo operations, and partial updates
- Integrates with other modules (image, media, download, template) for rich content editing
- Implements caching and performance optimizations (ETags, Last-Modified headers)

---

### Constants and Global Variables

| Name | Value/Default | Description |
|------|---------------|-------------|
| `$content_display` | Global | Determines how content is displayed (e.g., "directory" mode) |
| `$content_index` | Global | Unique identifier of the content being accessed/edited |
| `$content_directory_index` | Global | Identifier for directory-based content routing |
| `$content_option` | Global | Bitmask controlling which editing features are enabled |
| `$content_user` | Global | User identifier for content ownership/permissions |
| `$content_message` | Global | Command to execute (e.g., "apply", "revert", "undo", "redo") |
| `$range` | Global | Specifies a sub-section of content for partial updates |
| `$type` | Global | Type of content operation (e.g., "value", "image", "#buffer") |
| `$value` | Global | New value for the specified content range/type |
| `$left`, `$top` | Global | Scroll position coordinates for the editing interface |
| `$id` | Global | DOM element identifier for targeted operations |

---

### Core Logic Flow

1. **404 Handling**
   - Checks for `REDIRECT_STATUS=404` and serves a custom 404 page unless the request is for static assets (images, CSS, JS, etc.).

2. **Initialization**
   - Loads required libraries (`content`, `template`)
   - Instantiates the `content` object and verifies it is enabled
   - Sets up static editing parameters if the user is a writer

3. **Directory Resolution**
   - Resolves directory-based URLs to their target content
   - Handles redirects for language-specific homepages or internal content references
   - Detects and prevents infinite redirect loops

4. **Partial Content Updates**
   - Processes AJAX requests for partial content updates (e.g., `#buffer` type)
   - Extracts and caches specific content ranges for dynamic updates

5. **Permission Verification**
   - Checks if the user has read access to the requested content
   - Redirects unauthorized users to the login page

6. **Content Status Check**
   - Verifies if the content exists and is published
   - Logs access for published content

7. **Output Generation**
   - **Read Mode**: Generates static output with caching headers (ETag, Last-Modified)
   - **Edit Mode**: Sets up an interactive editing interface with action hooks for content manipulation

8. **Command Processing**
   - Handles editing commands (apply, revert, undo, redo)
   - Updates content ranges based on user input

9. **Action Hooks Setup**
   - Configures template actions for editing (e.g., text, image, media, template edits)
   - Adds control commands (copy, paste, swap, debug, etc.)

10. **Performance Metrics**
    - Measures and outputs the total processing time

---

### Key Functions and Methods

#### `content_parse()`
*(Implicitly used, defined in `content` library)*

**Purpose**:
Renders content into HTML, applying templates and processing dynamic elements.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$content` | `content` | Content object instance |
| `$content_index` | `int` | Content identifier |
| `$action` | `array\|null` | Action hooks for editing interface |
| `$header` | `string` | Additional HTML headers (e.g., JavaScript includes) |
| `$is_dynamic` | `bool` (out) | Set to `TRUE` if the content has uncached parts |

**Return Values**:
- `string`: Rendered HTML output

**Inner Mechanisms**:
- Retrieves content and template data from the database
- Applies template transformations
- Injects editing controls if in edit mode
- Handles dynamic content fragments (e.g., `#buffer`)

**Usage Context**:
- Called during both read and edit modes to generate the final output.

**Example**:
```php
// Render content with editing controls
$output = content_parse($content, $content_index, $action, $header, $is_dynamic);
echo($output);
```

---

#### `content_set_range()`
*(Implicitly used, defined in `content` library)*

**Purpose**:
Updates a specific range of content with a new value.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$content` | `content` | Content object instance |
| `$content_index` | `int` | Content identifier |
| `$range` | `string` | Path to the content range (e.g., "body/section[1]/p[2]") |
| `$type` | `string` | Type of update (e.g., "value", "image", "template") |
| `$value` | `mixed` | New value for the range |

**Return Values**:
- `void`

**Inner Mechanisms**:
- Validates the range and type
- Updates the content buffer in the database
- Logs the change for undo/redo functionality

**Usage Context**:
- Called when a user submits an edit (e.g., changing text, updating an image reference).

**Example**:
```php
// Update a text range
content_set_range($content, $content_index, "body/section[1]/p[2]", "value", "New paragraph text");
```

---

### Directory Resolution

**Purpose**:
Handles URL routing for directory-based content, enabling clean URLs and redirects.

**Mechanism**:
1. Checks if `$content_index` is empty or if `$content_display` is "directory".
2. Retrieves the target URL from the directory data store.
3. Resolves internal references (e.g., `directory://`, `content://`) recursively.
4. Issues HTTP 301/302 redirects to the resolved URL.

**Example**:
```php
// Redirect from /about to /company/overview
$url = $data->get("about", "url"); // Returns "content://123"
$_url = analyze_url($url);
if ($_url["scheme"] === "content") {
    $content_index = (int)$_url["host"];
    header("Location: " . translate_url($url), TRUE, 301);
    exit();
}
```

---

### Edit Mode Actions

The module dynamically builds an `$action` array to configure the editing interface. Actions are categorized into:

| Category | Description |
|----------|-------------|
| `CMS_TEMPLATE_CONTROL` | Top-level commands (e.g., open interface, logout) |
| `CMS_TEMPLATE_COMMAND` | Content manipulation commands (e.g., copy, paste, undo) |
| `CMS_TEMPLATE_ACTION` | Type-specific edit actions (e.g., edit text, image, template) |
| `CMS_TEMPLATE_SWITCH` | Option switches (e.g., change template) |

**Example Action Setup**:
```php
// Configure a text edit action
$action[CMS_TEMPLATE_ACTION][CMS_TEMPLATE_TYPE_TEXT] = [
    CMS_TEMPLATE_CODE => "content_edit_open('" .
        q(cms_url($interface_url, [
            "ifc_page" => "content",
            "ifc_message" => "edit_range",
            "object" => $content_index,
            "range" => "\x1B%index%",
            "id" => "\x1B%id%"
        ], TRUE)) . "');",
    CMS_TEMPLATE_IMAGE => CMS_IMAGES_URL . "content/button_text.svg"
];
```

---

### JavaScript Integration

The module outputs JavaScript to:
- Restore the editing interface state
- Register drag-and-drop handlers
- Apply visual feedback for edited elements
- Restore scroll position after page load

**Key Functions**:
| Function | Purpose |
|----------|---------|
| `tp_flp_restore()` | Restores the editing interface state |
| `dd_register()` | Registers drag-and-drop zones |
| `content_edit_*` | Suite of functions for content manipulation (defined in `content.js`) |

**Example**:
```javascript
// Register a drag-and-drop zone
var func = function(object) {
    var type = object.getAttribute("data-tp-dd-type");
    var accept = object.getAttribute("data-tp-dd-accept");
    dd_register(object.id, type, accept);
};
Array.prototype.forEach.call(document.getElementsByClassName("tp-dd"), func);
```

---

### Usage Scenarios

#### 1. Serving Published Content
**Context**: A visitor requests a published page.
**Flow**:
1. Module verifies read permissions.
2. Checks content status (must be `CMS_CONTENT_STATUS_PUBLICATION`).
3. Logs the access.
4. Generates static output with caching headers.
5. Outputs the HTML.

**Example URL**:
```
https://example.com/content.php?content_index=123
```

#### 2. Editing Content
**Context**: A writer edits a page.
**Flow**:
1. Module verifies write permissions.
2. Sets up editing actions based on `$content_option`.
3. Processes commands (e.g., "apply", "undo").
4. Updates content ranges via `content_set_range()`.
5. Outputs the editable interface with action hooks.

**Example URL**:
```
https://example.com/content.php?content_index=123&content_option=15&left=50&top=20
```

#### 3. Directory-Based Routing
**Context**: A visitor accesses a clean URL (e.g., `/about`).
**Flow**:
1. Module resolves the directory entry to a content reference.
2. Issues a 301 redirect to the target content URL.
3. If the target is external, redirects to the absolute URL.

**Example**:
```
/about → 301 → /content.php?content_index=123
```

#### 4. Partial Content Update
**Context**: A dynamic update via AJAX (e.g., editing a single paragraph).
**Flow**:
1. JavaScript requests a partial update with `type=#buffer`.
2. Module extracts the specified range and caches it.
3. Returns the updated HTML fragment.

**Example Request**:
```
POST /content.php?content_index=123&type=#buffer&range=body/section[1]/p[2]
```

---

### Error Handling

| Error | HTTP Status | Description |
|-------|-------------|-------------|
| Missing content | 404 | Content does not exist or is not accessible |
| Unpublished content | 410 | Content exists but is not published |
| Redirect loop | 508 | Infinite loop detected in directory resolution |
| Permission denied | 302 | Redirects to login page |
| Internal error | 500 | Content or template library failed to load |

**Example**:
```php
if (! cms_permission(CMS_CONTENT_PERMISSION_READER . ".$content_index")) {
    header("Location: " . CMS_MODULES_URL . "identification.php" .
        cms_url(["location" => CMS_ACTIVE_URL . "?$query_string"]), TRUE, 302);
    exit();
}
```

---

### Performance Optimizations

1. **Caching**:
   - ETag and Last-Modified headers for static content
   - Permanent caching for partial updates (`content.buffer`)

2. **Partial Updates**:
   - `#buffer` type avoids full page reloads during editing

3. **Static Asset Bypass**:
   - Skips 404 handling for static files (images, CSS, JS)

4. **Microtime Measurement**:
   - Outputs processing time for performance monitoring

**Example**:
```php
$etag = "\"$mod_time\"";
if (strpos($_SERVER["HTTP_IF_NONE_MATCH"] ?? "", $etag) !== FALSE) {
    http_response_code(304); // Not Modified
    exit();
}
header("ETag: $etag");
```

---

### Integration with Other Modules

| Module | Integration Point | Purpose |
|--------|-------------------|---------|
| `identification` | Permission checks | Redirects unauthorized users to login |
| `interface` | Action hooks | Provides editing dialogs (e.g., `ifc.content.inc`) |
| `image` | Image editing | Configures image upload/editing actions |
| `media` | Media embedding | Configures media selection/editing actions |
| `download` | File downloads | Configures download link editing |
| `template` | Template editing | Configures template selection/export |


<!-- HASH:039fd622d501da29418abcee6b7c2690 -->
