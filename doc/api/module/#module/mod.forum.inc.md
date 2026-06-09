# PWNC API Documentation

[← Index](../../README.md) | [`module/#module/mod.forum.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23module/mod.forum.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Forum Module (`mod.forum.inc`)

The **Forum Module** provides a hierarchical, thread-based discussion system with support for:
- **Topics** (root-level containers)
- **Posts** (replies to topics or other posts)
- **Search** (full-text search across titles and content)
- **User-specific views** (filtering posts by author)
- **Cut/Copy/Paste operations** (for reorganizing posts)
- **Access control** (reader/writer/operator permissions)

This file handles **frontend rendering** and **user interactions** (adding, editing, deleting, moving posts). It relies on the `forum` class (loaded via `cms_load("forum")`) for backend operations.

---

### Global Variables

| Name | Type | Description |
|------|------|-------------|
| `$forum_message` | `string` | Current action state (e.g., `"add"`, `"edit"`, `"delete"`). |
| `$forum_index` | `string` | Unique identifier of the current post/topic. |
| `$forum_buffer` | `string` | Temporary storage for post IDs during cut/paste operations. |
| `$forum_search` | `string` | Search query for filtering posts. |
| `$forum_user` | `string` | User identifier for filtering posts by author. |
| `$forum_page` | `string` | Pagination offset for search/user views. |
| `$forum_edit_*` | `string` | Form fields for post editing (title, text, email notification). |

---

### Core Workflow

1. **Initialization**
   - Loads the `forum` library and checks if the module is enabled.
   - Validates user permissions (`reader`, `writer`, or `operator`).
   - Resolves the current `$forum_index` and builds a navigation path.

2. **Message Handling**
   - Processes user actions (e.g., `add`, `edit`, `delete`) via a state machine (`$forum_message`).
   - Validates inputs and transitions between states (e.g., `_add` → `__add` for preview → save).

3. **Rendering**
   - Displays different views based on context:
     - **Overview**: List of root topics.
     - **Topic/Post**: Hierarchical thread with replies.
     - **Search**: Filtered results with snippets.
     - **User**: Posts by a specific author.

---

### Key Functions

#### `command($index)`
Renders action buttons for a post/topic (cut, paste, delete, edit, add).

**Parameters:**
| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Post/topic identifier. |

**Usage Example:**
```php
// Render buttons for post #123
$command("123");
```
**Output:**
- **Cut**: Only shown if the user is an `operator` and the post is not already buffered.
- **Paste**: Only shown if a post is buffered (`$forum_buffer`) and the target is valid.
- **Delete**: Requires confirmation and `operator` permissions.
- **Edit/Add**: Conditional on user permissions and post hierarchy.

---

### State Machine Logic

The module uses `$forum_message` to manage workflows:

| State | Description | Next State |
|-------|-------------|------------|
| `add` | Initiate a new post. | `_add` (editing) |
| `_add` | Edit form for a new post. | `__add` (preview) or back to `add` (cancel) |
| `__add` | Preview a new post. | `___add` (save) or back to `_add` (edit) |
| `edit` | Load an existing post for editing. | `_edit` (editing) |
| `_edit` | Edit form for an existing post. | `__edit` (preview) or back to main view (cancel) |
| `__edit` | Preview an edited post. | `___edit` (save) or back to `_edit` (edit) |
| `delete` | Delete a post. | Redirects to parent post. |
| `insert` | Paste a buffered post. | Updates `$forum_buffer` and redirects. |

**Example Workflow (Adding a Post):**
```php
// 1. User clicks "Add" → $forum_message = "add"
// 2. System loads the form → $forum_message = "_add"
// 3. User submits → $forum_message = "__add" (preview)
// 4. User confirms → $forum_message = "___add" (save)
```

---

### Rendering Logic

#### Overview View
- Lists root topics with:
  - **Title**
  - **Teaser text** (first paragraph)
  - **Latest 3 replies** (if any)
  - **Action buttons** (via `command()`)

#### Topic/Post View
- Displays a hierarchical thread:
  - **Topic**: Root post with replies (paginated).
  - **Post**: Individual post with nested replies.
- **Metadata**: Author, date, reply count.
- **Navigation**: Breadcrumbs for parent posts.

#### Search/User Views
- **Search**: Full-text search across titles and content with snippets.
- **User**: Lists posts by a specific author (paginated).

**Example (Rendering a Topic):**
```php
// Displays topic #456 with replies
$forum_index = "456";
include "mod.forum.inc";
```
**Output:**
```html
<section id="forum" class="forum">
  <div class="forum-item">
    <div class="forum-topic">
      <h2 class="forum-title">Topic Title</h2>
      <div class="forum-text">...</div>
      <div class="p">
        <a href="?forum_index=456&forum_message=add" class="button">Reply</a>
      </div>
    </div>
    <!-- Replies -->
    <div class="forum-post">...</div>
  </div>
</section>
```

---

### Security & Validation

1. **Permissions**
   - Actions are gated by `forum->test_*` methods (e.g., `test_add()`, `test_edit()`).
   - Anonymous users can only add posts if enabled (with optional email notifications).

2. **Input Sanitization**
   - All user inputs are escaped via `sqlesc()` (SQL) and `x()` (HTML).
   - Text formatting uses `parse_text()` (BBCode-like syntax).

3. **CSRF Protection**
   - Form submissions use `cms_url()` to generate tokens.

**Example (Validation):**
```php
// Check if user can edit post #123
if ($forum->test_edit("123")) {
    // Load edit form
}
```

---

### Dependencies

| Library | Purpose |
|---------|---------|
| `forum` | Backend operations (CRUD, permissions). |
| `ifc` | Extended editing tools (images, links, tokens). |
| `permission` | User name resolution and access control. |
| `template` | Metadata injection (title, description). |

---

### Error Handling

| Error | Description | Resolution |
|-------|-------------|------------|
| `CMS_L_MOD_FORUM_013` | Post not found. | Redirects to parent post. |
| `CMS_L_MOD_FORUM_017` | Empty title/text. | Returns to edit form. |
| `CMS_L_MOD_FORUM_018` | Edit permission denied. | Displays error message. |
| `CMS_L_MOD_FORUM_023` | Add failed. | Displays error message. |
| `CMS_L_MOD_FORUM_025` | Edit failed. | Displays error message. |
| `CMS_L_MOD_FORUM_027` | Paste failed. | Displays error message. |

**Example (Error Display):**
```php
if (nstre($error)) {
    echo("<div class=\"response-error\">$error</div>");
}
```

---

### Usage Example

**Scenario**: Add a new topic to the forum.

1. **User Action**:
   - Clicks "Add Topic" on the overview page.
   - Fills in the title and text, then clicks "Preview".

2. **Code Flow**:
   ```php
   // 1. User clicks "Add Topic" → $forum_message = "add"
   // 2. System sets $forum_message = "_add" and loads the form
   // 3. User submits → $forum_message = "__add" (preview)
   // 4. User confirms → $forum_message = "___add" (save)
   // 5. System redirects to the new post
   ```

3. **Result**:
   - A new topic is created with the user’s input.
   - The user is redirected to the topic’s URL (e.g., `?forum_index=789`).

---

### Notes

- **Pagination**: Uses `pagination()` helper for search/user views (10 items per page).
- **Text Processing**: `parse_text()` supports BBCode-like syntax (e.g., `[b]bold[/b]`).
- **Multibyte Support**: Uses `utf8_trim()` and `utf8_*` functions for non-ASCII text.
- **Performance**: Caches post counts via SQL joins to avoid N+1 queries.


<!-- HASH:2156b25f73000c20e7719b1ef28e4ff7 -->
