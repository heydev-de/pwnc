# PWNC API Documentation

[← Index](../../README.md) | [`module/#module/mod.blog.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23module/mod.blog.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Blog Module (`mod.blog.inc`)

The **Blog Module** provides a complete blogging system with article management, tagging, archiving, and RSS feed generation. It integrates with the PWNC platform's templating, permission, and comment systems to deliver a full-featured blog experience.

---

### **Global Variables**

| Name | Default | Description |
|------|---------|-------------|
| `$blog_message` | `NULL` | Controls the current view mode (e.g., `edit`, `add`, `rss`). |
| `$blog_index` | `NULL` | Unique identifier of the currently displayed or edited article. |
| `$blog_date` | `NULL` | Date filter for article listings (format: `YYYYMMDD`). |
| `$blog_meta` | `NULL` | Tag filter for article listings. |
| `$blog_page` | `NULL` | Pagination control for article listings. |
| `$blog_edit_*` | `NULL` | Form fields for article editing (title, meta, text, etc.). |
| `$blog_code_edit_*` | `NULL` | Form fields for custom code editing (position, text). |

---

### **Module Initialization**

1. **Loads Dependencies**
   - Requires `blog` and `comment` libraries. If unavailable, displays an error message and exits.

2. **Template Integration**
   - Merges `blog_index` into the template's query data for SEO and tracking.

3. **Instance Handling**
   - Uses `CMS_CONTENT_INDEX` as the default instance if none is specified.
   - Creates a `blog` object and checks if the blog is enabled.

4. **Permission Validation**
   - Exits if the user lacks read, write, or operator permissions.

---

### **Core Workflow**

The module processes the `$blog_message` to determine the current action:

| Message | Action |
|---------|--------|
| `_edit` | Handles article creation, editing, or deletion. |
| `_code_edit` | Handles custom code updates for blog sections. |
| `edit`/`add` | Displays the article editing form. |
| `code_edit` | Displays the code editing form. |
| `rss` | Generates an RSS feed. |
| *(default)* | Displays the article list or a single article. |

---

### **Key Functions and Logic**

#### **Article Management**
- **Editing/Adding Articles**
  - Validates form inputs (title, meta tags, publication status, sticky flag).
  - Supports scheduled publication via date/time selection.
  - Uses `blog->add()` and `blog->edit()` for database operations.

- **Deleting Articles**
  - Removes the article and its associated comments from the database.

#### **Code Customization**
- **Code Editing**
  - Allows modification of HTML/JS snippets for specific positions (e.g., `before`, `after`, `teaser`, `control`).
  - Provides placeholders (`%url%`, `%title%`, `%author%`, `%user%`) for dynamic content.

#### **RSS Feed Generation**
- **Caching**
  - Uses `cms_cache()` to store the RSS feed for 60 seconds.
- **Content**
  - Includes article titles, links, descriptions, and enclosures (images).
  - Supports customization of feed title, description, and category.

#### **Article Display**
- **Single Article View**
  - Shows the full article with metadata (author, date, tags).
  - Integrates comments via the `comment` module.
  - Supports custom code insertion before/after the article.

- **Overview (List View)**
  - Displays articles in reverse chronological order with pagination.
  - Supports filtering by date (`blog_date`) and tags (`blog_meta`).
  - Shows teaser text and a "Read more" link.

#### **Navigation and Filtering**
- **Archive**
  - Provides a hierarchical date-based navigation (year → month → day).
- **Tag Cloud**
  - Displays tags with font sizes proportional to their usage frequency.

---

### **Class: `blog`**

The `blog` class (defined in `lib/blog.inc`) encapsulates all blog-related operations. Below are its key methods as used in this module:

| Method | Description |
|--------|-------------|
| `__construct($instance)` | Initializes the blog for a specific instance. |
| `add($title, $meta, $text, $status, $time, $sticky)` | Creates a new article. Returns the article index or `FALSE` on failure. |
| `edit($index, $title, $meta, $text, $status, $time, $sticky)` | Updates an existing article. Returns `TRUE` on success. |
| `delete($index)` | Deletes an article. Returns `TRUE` on success. |
| `code_get($position)` | Retrieves custom code for a given position. |
| `code_set($position, $text)` | Saves custom code for a given position. Returns `TRUE` on success. |
| `code_parse($position, $replacement)` | Parses custom code with placeholders replaced by dynamic values. |
| `test_add()`, `test_edit($index)`, `test_delete($index)`, `test_code_set()` | Check user permissions for the respective actions. |

---

### **Usage Examples**

#### **1. Displaying the Blog Overview**
```php
// Load the blog module in a template or page.
cms_application("blog");
```
- **Output**: Renders the blog overview with the latest articles, archive, and tag cloud.

#### **2. Adding a New Article**
```php
// Set the blog message to trigger the "add" form.
cms_param("add", "blog_message");
cms_application("blog");
```
- **Output**: Displays a form to create a new article with fields for title, tags, publication status, and content.

#### **3. Editing an Article**
```php
// Specify the article index and set the blog message to "edit".
cms_param("123", "blog_index");
cms_param("edit", "blog_message");
cms_application("blog");
```
- **Output**: Displays the article editing form pre-filled with the existing article data.

#### **4. Generating an RSS Feed**
```php
// Set the blog message to "rss" to generate the feed.
cms_param("rss", "blog_message");
cms_application("blog");
```
- **Output**: Returns an RSS 2.0 feed with the latest 50 articles.

#### **5. Customizing Blog Code**
```php
// Set the position and trigger the code editing form.
cms_param(CMS_DB_BLOG_CODE_POSITION_CONTROL, "blog_code_edit_position");
cms_param("code_edit", "blog_message");
cms_application("blog");
```
- **Output**: Displays a form to edit the custom HTML/JS for the control section (e.g., sidebar).

---

### **Database Schema**

The module interacts with the following database tables:

| Table | Description |
|-------|-------------|
| `CMS_DB_BLOG` | Stores articles with fields for `INSTANCE`, `INDEX`, `TITLE`, `META`, `TEXT`, `STATUS`, `TIME`, `STICKY`, and `OWNER`. |
| `CMS_DB_BLOG_META_LINK` | Links articles to tags via `ARTICLE` and `TERM` fields. |
| `CMS_DB_BLOG_META_TERM` | Stores tag names (`INDEX`, `TEXT`). |
| `CMS_DB_COMMENT` | Stores comments associated with articles. |

---

### **Security and Permissions**

- **Permissions**
  - `CMS_BLOG_PERMISSION_READER`: Allows viewing published articles.
  - `CMS_BLOG_PERMISSION_WRITER`: Allows creating and editing articles.
  - `CMS_BLOG_PERMISSION_OPERATOR`: Allows deleting articles and managing custom code.

- **CSRF Protection**
  - Uses `cms_url()` and `cms_param()` to generate and validate URLs with CSRF tokens.

- **Input Sanitization**
  - Uses `sqlesc()` for SQL escaping and `x()` for HTML escaping.

---

### **Integration with Other Modules**

1. **Comments**
   - Each article can have comments managed by the `comment` module.
   - The comment instance is set to `"{blog_instance}.{article_index}"`.

2. **Images and Links**
   - The text editor integrates with the `image` and `content` modules for inserting media and links.

3. **Tokens**
   - Supports dynamic tokens (e.g., `%url%`, `%title%`) for custom code.

4. **Templates**
   - Merges data into the global template for SEO and tracking.

---

### **Error Handling**

- **Database Errors**
  - Displays user-friendly error messages (e.g., `CMS_L_MOD_BLOG_045` for database failures).

- **Missing Articles**
  - Returns a `410 Gone` HTTP status for deleted articles.

- **Permission Denied**
  - Silently exits if the user lacks the required permissions.

---

### **Performance Considerations**

- **Caching**
  - The RSS feed is cached for 60 seconds to reduce database load.

- **Pagination**
  - Limits article listings to 10 per page to improve performance.

- **Lazy Loading**
  - Uses `defer` for images to improve page load times.

---


<!-- HASH:1b9de66deae8ef331af6720046472444 -->
