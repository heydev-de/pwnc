# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.blog.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.blog.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Blog Class

The `blog` class provides a comprehensive interface for managing blog articles, metadata, and associated code snippets within the PWNC Web Platform. It handles permissions, article lifecycle (creation, editing, deletion), metadata tagging, and custom code injection at predefined positions in the blog interface.

---

### Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| **Permission Constants** | | |
| `CMS_BLOG_PERMISSION_OPERATOR` | `"operator"` | Permission level for full blog management. |
| `CMS_BLOG_PERMISSION_WRITER` | `"writer"` | Permission level for creating and editing articles. |
| `CMS_BLOG_PERMISSION_READER` | `"reader"` | Permission level for reading articles. |
| **Status Constants** | | |
| `CMS_DB_BLOG_STATUS_INACTIVE` | `0` | Article is inactive and not visible. |
| `CMS_DB_BLOG_STATUS_ACTIVE` | `1` | Article is active and visible. |
| **Sticky Constants** | | |
| `CMS_DB_BLOG_STICKY_OFF` | `0` | Article is not sticky (default order). |
| `CMS_DB_BLOG_STICKY_ON` | `1` | Article is sticky (appears at the top). |
| **Code Position Constants** | | |
| `CMS_DB_BLOG_CODE_POSITION_CONTROL` | `0` | Code position for control logic (e.g., JavaScript). |
| `CMS_DB_BLOG_CODE_POSITION_TEASER` | `1` | Code position for teaser content (before article). |
| `CMS_DB_BLOG_CODE_POSITION_BEFORE` | `2` | Code position before the article body. |
| `CMS_DB_BLOG_CODE_POSITION_AFTER` | `3` | Code position after the article body. |
| **Database Constants** | | |
| `CMS_DB_BLOG` | `CMS_DB_PREFIX . "blog"` | Main blog articles table. |
| `CMS_DB_BLOG_INDEX` | `"id"` | Primary key for articles. |
| `CMS_DB_BLOG_INSTANCE` | `"instance"` | Blog instance identifier. |
| `CMS_DB_BLOG_STATUS` | `"status"` | Article status (active/inactive). |
| `CMS_DB_BLOG_TIME` | `"time"` | Article publication timestamp. |
| `CMS_DB_BLOG_STICKY` | `"sticky"` | Article sticky flag. |
| `CMS_DB_BLOG_OWNER` | `"owner"` | Article owner (user identifier). |
| `CMS_DB_BLOG_TITLE` | `"title"` | Article title. |
| `CMS_DB_BLOG_META` | `"meta"` | Article metadata (comma-separated tags). |
| `CMS_DB_BLOG_TEXT` | `"text"` | Article content. |
| `CMS_DB_BLOG_META_TERM` | `CMS_DB_PREFIX . "blog_meta_term"` | Metadata terms table. |
| `CMS_DB_BLOG_META_TERM_INDEX` | `"id"` | Primary key for metadata terms. |
| `CMS_DB_BLOG_META_TERM_TEXT` | `"text"` | Metadata term text. |
| `CMS_DB_BLOG_META_TERM_LANGUAGE` | `"language"` | Metadata term language. |
| `CMS_DB_BLOG_META_LINK` | `CMS_DB_PREFIX . "blog_meta_link"` | Link table between articles and metadata terms. |
| `CMS_DB_BLOG_META_LINK_ARTICLE` | `"article"` | Foreign key to `CMS_DB_BLOG_INDEX`. |
| `CMS_DB_BLOG_META_LINK_TERM` | `"term"` | Foreign key to `CMS_DB_BLOG_META_TERM_INDEX`. |
| `CMS_DB_BLOG_CODE` | `CMS_DB_PREFIX . "blog_code"` | Custom code snippets table. |
| `CMS_DB_BLOG_CODE_INSTANCE` | `"instance"` | Blog instance identifier. |
| `CMS_DB_BLOG_CODE_POSITION` | `"position"` | Code position (see constants above). |
| `CMS_DB_BLOG_CODE_TEXT` | `"text"` | Custom code content. |

---

### Properties

| Name | Default | Description |
|------|---------|-------------|
| `$instance` | `""` | Blog instance identifier. |
| `$enabled` | `FALSE` | Whether the blog instance is enabled and tables exist. |
| `$operator` | `FALSE` | Whether the current user has operator permissions. |
| `$writer` | `FALSE` | Whether the current user has writer permissions. |
| `$reader` | `FALSE` | Whether the current user has reader permissions. |

---

### `__construct($instance = "")`

#### **Purpose**
Initializes a blog instance, verifies database tables, and sets user permissions.

#### **Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$instance` | `string` | Blog instance identifier. Defaults to `""`. |

#### **Return Values**
- **`void`**: No explicit return. Sets object properties (`$enabled`, `$operator`, `$writer`, `$reader`).

#### **Inner Mechanisms**
1. Creates a `mysql` instance to verify database tables.
2. Checks for the existence of:
   - `CMS_DB_BLOG` (articles)
   - `CMS_DB_BLOG_META_TERM` (metadata terms)
   - `CMS_DB_BLOG_META_LINK` (article-term links)
   - `CMS_DB_BLOG_CODE` (custom code snippets)
3. If tables exist, sets `$enabled` to `TRUE` and checks user permissions using `cms_permission()`.

#### **Usage Context**
- Called when initializing a blog instance for a specific module or site section.
- Ensures database integrity before any operations.

#### **Example**
```php
$blog = new \cms\blog("news");
if ($blog->enabled) {
    // Blog is ready for operations
}
```

---

### `add($title, $meta, $text, $status = CMS_DB_BLOG_STATUS_ACTIVE, $time = NULL, $sticky = CMS_DB_BLOG_STICKY_OFF, $test = FALSE)`

#### **Purpose**
Creates a new blog article with metadata and optional sticky status.

#### **Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$title` | `string` | Article title. |
| `$meta` | `string` | Comma-separated metadata tags. |
| `$text` | `string` | Article content. |
| `$status` | `int` | Article status (`CMS_DB_BLOG_STATUS_ACTIVE` or `CMS_DB_BLOG_STATUS_INACTIVE`). Defaults to `CMS_DB_BLOG_STATUS_ACTIVE`. |
| `$time` | `int\|NULL` | Publication timestamp. Defaults to current time if `NULL`. |
| `$sticky` | `int` | Sticky flag (`CMS_DB_BLOG_STICKY_ON` or `CMS_DB_BLOG_STICKY_OFF`). Defaults to `CMS_DB_BLOG_STICKY_OFF`. |
| `$test` | `bool` | If `TRUE`, performs a permission test without creating the article. Defaults to `FALSE`. |

#### **Return Values**
- **`int\|FALSE`**: Article ID on success, `FALSE` on failure.

#### **Inner Mechanisms**
1. Checks if the blog is enabled and the user has writer permissions.
2. If `$test` is `TRUE`, returns `TRUE` immediately (permission test).
3. Sets `$time` to current time if `NULL`.
4. Inserts the article into `CMS_DB_BLOG`.
5. Links metadata terms to the article using `meta_link()`.
6. Returns the new article ID or `FALSE` on failure.

#### **Usage Context**
- Used to publish new articles in a blog instance.
- Metadata tags are normalized and linked to the article.

#### **Example**
```php
$articleId = $blog->add(
    "New Features in PWNC",
    "release, features, update",
    "PWNC now supports custom code injection...",
    CMS_DB_BLOG_STATUS_ACTIVE,
    time(),
    CMS_DB_BLOG_STICKY_ON
);
if ($articleId) {
    echo "Article published with ID: $articleId";
}
```

---

### `test_add()`

#### **Purpose**
Tests if the current user has permission to add articles.

#### **Parameters**
- None.

#### **Return Values**
- **`bool`**: `TRUE` if the user has writer permissions, `FALSE` otherwise.

#### **Inner Mechanisms**
- Calls `add()` with `NULL` values and `$test = TRUE`.

#### **Usage Context**
- Used to check permissions before rendering UI elements (e.g., "Add Article" button).

#### **Example**
```php
if ($blog->test_add()) {
    echo '<button>Add Article</button>';
}
```

---

### `edit($index, $title, $meta, $text, $status = NULL, $time = NULL, $sticky = NULL, $test = FALSE)`

#### **Purpose**
Updates an existing blog article.

#### **Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Article ID. |
| `$title` | `string` | New article title. |
| `$meta` | `string` | New comma-separated metadata tags. |
| `$text` | `string` | New article content. |
| `$status` | `int\|NULL` | New article status. If `NULL`, retains current value. |
| `$time` | `int\|NULL` | New publication timestamp. If `NULL`, retains current value. |
| `$sticky` | `int\|NULL` | New sticky flag. If `NULL`, retains current value. |
| `$test` | `bool` | If `TRUE`, performs a permission test without updating the article. Defaults to `FALSE`. |

#### **Return Values**
- **`bool`**: `TRUE` on success, `FALSE` on failure.

#### **Inner Mechanisms**
1. Checks if the blog is enabled and the user has writer permissions.
2. If the user is not an operator, verifies ownership of the article.
3. If `$test` is `TRUE`, returns `TRUE` immediately (permission test).
4. Updates the article in `CMS_DB_BLOG`.
5. Re-links metadata terms using `meta_link()`.
6. Returns `TRUE` on success, `FALSE` on failure.

#### **Usage Context**
- Used to update article content, metadata, or status.
- Non-operator users can only edit their own articles.

#### **Example**
```php
if ($blog->edit(
    42,
    "Updated Features in PWNC",
    "release, features, update, improvements",
    "PWNC now includes a new caching layer..."
)) {
    echo "Article updated successfully";
}
```

---

### `test_edit($index)`

#### **Purpose**
Tests if the current user has permission to edit a specific article.

#### **Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Article ID. |

#### **Return Values**
- **`bool`**: `TRUE` if the user has permission, `FALSE` otherwise.

#### **Inner Mechanisms**
- Calls `edit()` with `NULL` values and `$test = TRUE`.

#### **Usage Context**
- Used to conditionally render edit controls in the UI.

#### **Example**
```php
if ($blog->test_edit(42)) {
    echo '<a href="/edit/42">Edit Article</a>';
}
```

---

### `delete($index, $test = FALSE)`

#### **Purpose**
Deletes a blog article and cleans up associated metadata.

#### **Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Article ID. |
| `$test` | `bool` | If `TRUE`, performs a permission test without deleting the article. Defaults to `FALSE`. |

#### **Return Values**
- **`bool`**: `TRUE` on success, `FALSE` on failure.

#### **Inner Mechanisms**
1. Checks if the blog is enabled and the user has writer permissions.
2. If the user is not an operator, verifies ownership of the article.
3. If `$test` is `TRUE`, returns `TRUE` immediately (permission test).
4. Deletes the article from `CMS_DB_BLOG`.
5. Cleans up orphaned metadata using `meta_clean()`.
6. Returns `TRUE` on success, `FALSE` on failure.

#### **Usage Context**
- Used to remove articles and associated metadata.
- Non-operator users can only delete their own articles.

#### **Example**
```php
if ($blog->delete(42)) {
    echo "Article deleted successfully";
}
```

---

### `test_delete($index)`

#### **Purpose**
Tests if the current user has permission to delete a specific article.

#### **Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Article ID. |

#### **Return Values**
- **`bool`**: `TRUE` if the user has permission, `FALSE` otherwise.

#### **Inner Mechanisms**
- Calls `delete()` with `$test = TRUE`.

#### **Usage Context**
- Used to conditionally render delete controls in the UI.

#### **Example**
```php
if ($blog->test_delete(42)) {
    echo '<button>Delete Article</button>';
}
```

---

### `meta_link($index, $meta)`

#### **Purpose**
Links metadata terms to a blog article.

#### **Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Article ID. |
| `$meta` | `string` | Comma-separated metadata tags. |

#### **Return Values**
- **`bool`**: `TRUE` on success, `FALSE` on failure.

#### **Inner Mechanisms**
1. Checks if the blog is enabled and the user has writer permissions.
2. Deletes existing metadata links for the article.
3. Normalizes metadata tags:
   - Splits by comma.
   - Trims whitespace.
   - Filters empty tags.
   - Converts to lowercase.
   - Removes duplicates.
4. Inserts new terms into `CMS_DB_BLOG_META_TERM` (ignores duplicates).
5. Retrieves term IDs and links them to the article in `CMS_DB_BLOG_META_LINK`.
6. Returns `TRUE` on success, `FALSE` on failure.

#### **Usage Context**
- Called internally by `add()` and `edit()` to manage article metadata.
- Can be used to update metadata independently.

#### **Example**
```php
$blog->meta_link(42, "php, mysql, performance");
```

---

### `meta_clean()`

#### **Purpose**
Removes orphaned metadata terms and links.

#### **Parameters**
- None.

#### **Return Values**
- **`bool`**: `TRUE` on success, `FALSE` on failure.

#### **Inner Mechanisms**
1. Checks if the blog is enabled and the user has writer permissions.
2. Deletes metadata links where the associated article no longer exists.
3. Deletes metadata terms that are no longer linked to any article.
4. Returns `TRUE` on success.

#### **Usage Context**
- Called internally by `delete()` to clean up metadata.
- Can be used as a maintenance task to remove unused terms.

#### **Example**
```php
$blog->meta_clean();
```

---

### `code_set($position, $text, $test = FALSE)`

#### **Purpose**
Sets custom code for a specific position in the blog interface.

#### **Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$position` | `int` | Code position (see constants). |
| `$text` | `string` | Custom code content. |
| `$test` | `bool` | If `TRUE`, performs a permission test without setting the code. Defaults to `FALSE`. |

#### **Return Values**
- **`bool`**: `TRUE` on success, `FALSE` on failure.

#### **Inner Mechanisms**
1. Checks if the blog is enabled and the user has operator permissions.
2. If `$test` is `TRUE`, returns `TRUE` immediately (permission test).
3. Uses `REPLACE` to insert or update the code in `CMS_DB_BLOG_CODE`.
4. Returns `TRUE` on success, `FALSE` on failure.

#### **Usage Context**
- Used to inject custom HTML, JavaScript, or PHP into predefined positions in the blog interface.
- Positions include control logic, teaser, before/after article content.

#### **Example**
```php
$blog->code_set(
    CMS_DB_BLOG_CODE_POSITION_TEASER,
    '<div class="alert">New Feature: Custom Code Injection</div>'
);
```

---

### `test_code_set()`

#### **Purpose**
Tests if the current user has permission to set custom code.

#### **Parameters**
- None.

#### **Return Values**
- **`bool`**: `TRUE` if the user has operator permissions, `FALSE` otherwise.

#### **Inner Mechanisms**
- Calls `code_set()` with `NULL` values and `$test = TRUE`.

#### **Usage Context**
- Used to conditionally render code editing controls in the UI.

#### **Example**
```php
if ($blog->test_code_set()) {
    echo '<button>Edit Custom Code</button>';
}
```

---

### `code_get($position)`

#### **Purpose**
Retrieves custom code for a specific position.

#### **Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$position` | `int` | Code position (see constants). |

#### **Return Values**
- **`string\|FALSE`**: Custom code content on success, `FALSE` on failure.

#### **Inner Mechanisms**
1. Checks if the blog is enabled.
2. Retrieves the code from `CMS_DB_BLOG_CODE` for the given position and instance.
3. Returns the code or `FALSE` on failure.

#### **Usage Context**
- Used to fetch custom code for rendering in the blog interface.

#### **Example**
```php
$teaserCode = $blog->code_get(CMS_DB_BLOG_CODE_POSITION_TEASER);
if ($teaserCode) {
    echo $teaserCode;
}
```

---

### `code_parse($position, $replacement = NULL)`

#### **Purpose**
Parses custom code, replacing placeholders with provided values.

#### **Parameters**

| Name | Type | Description |
|------|------|-------------|
| `$position` | `int` | Code position (see constants). |
| `$replacement` | `array\|NULL` | Associative array of placeholder-value pairs. Defaults to `NULL`. |

#### **Return Values**
- **`string\|FALSE`**: Parsed code on success, empty string if no code exists, `FALSE` on failure.

#### **Inner Mechanisms**
1. Retrieves the code using `code_get()`.
2. If no code exists, returns an empty string.
3. Replaces placeholders in the format `%key%` with corresponding values from `$replacement`.
   - Placeholders can be wrapped in optional brackets (e.g., `[%key%]`).
   - If a value is `FALSE` or empty, the placeholder and its brackets are removed.
   - Values are XML-escaped using `x()`.
4. Returns the parsed code.

#### **Usage Context**
- Used to dynamically render custom code with runtime values.
- Supports conditional rendering of code segments based on placeholder values.

#### **Example**
```php
$parsedCode = $blog->code_parse(
    CMS_DB_BLOG_CODE_POSITION_BEFORE,
    [
        "username" => "admin",
        "new_messages" => 3
    ]
);
echo $parsedCode;
// Input: "[%username%] has [%new_messages%] new messages."
// Output: "admin has 3 new messages."
```


<!-- HASH:26173e6ddab6c11f00a3d74e7a1a9040 -->
