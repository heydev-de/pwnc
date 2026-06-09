# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.forum.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.forum.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Forum Module (`lib.forum.inc`)

The `forum` class provides a lightweight, permission-based forum system for the PWNC Web Platform. It handles post creation, editing, moving, deletion, and searching while enforcing user permissions. The module integrates with the platform's database, logging, and notification systems.

---

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_FORUM_PERMISSION_OPERATOR` | `"operator"` | Permission identifier for forum operators (full control). |
| `CMS_FORUM_PERMISSION_WRITER` | `"writer"` | Permission identifier for forum writers (create/edit own posts). |
| `CMS_FORUM_PERMISSION_READER` | `"reader"` | Permission identifier for forum readers (read-only access). |
| `CMS_DB_FORUM` | `CMS_DB_PREFIX . "forum"` | Base table name for forum data. |
| `CMS_DB_FORUM_INDEX` | `"id"` | Primary key column for forum posts. |
| `CMS_DB_FORUM_CONTAINER` | `"container"` | Parent post ID (0 for root posts). |
| `CMS_DB_FORUM_USER` | `"user"` | Author username. |
| `CMS_DB_FORUM_TITLE` | `"title"` | Post title. |
| `CMS_DB_FORUM_TEXT` | `"text"` | Post content. |
| `CMS_DB_FORUM_EMAIL` | `"email"` | Email notification flag (1 = enabled). |
| `CMS_DB_FORUM_TIME` | `"time"` | Unix timestamp of post creation. |
| `CMS_DB_FORUM_ACCESS` | `"access"` | View counter for post analytics. |

---

### Utility Function: `forum_quote`

#### Purpose
Formats text for quoting in forum replies by prepending `>` to each line. Handles nested quotes and multibyte word-wrapping.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Raw text to be quoted. |

#### Return Values
| Type | Description |
|------|-------------|
| `string` | Quoted text with `>` prefixes. |

#### Inner Mechanisms
1. Splits input into lines using `\n` or `\r\n`.
2. Preserves existing quoted lines (starting with `>`).
3. Wraps long lines using `utf8_wordwrap()` and adds `>` to each wrapped segment.

#### Usage Example
```php
$reply = forum_quote("Hello!\nThis is a test.");
echo $reply;
// Output:
// > Hello!
// > This is a test.
```

---

### Class: `forum`

#### Properties

| Name | Default | Description |
|------|---------|-------------|
| `$instance` | `NULL` | Forum instance identifier (appended to table name). |
| `$table` | `CMS_DB_FORUM` | Database table name (modified if `$instance` is set). |
| `$mysql` | `NULL` | Database connection handler. |
| `$operator` | `NULL` | Boolean: User has operator permissions. |
| `$writer` | `NULL` | Boolean: User has writer permissions. |
| `$reader` | `NULL` | Boolean: User has reader permissions. |
| `$enabled` | `NULL` | Boolean: Forum is initialized and usable. |

---

### Constructor: `__construct`

#### Purpose
Initializes the forum instance, creates the database table if needed, and sets user permissions.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$instance` | `string` | `NULL` | Optional forum identifier (e.g., `"support"`). |

#### Inner Mechanisms
1. Initializes a `mysql` connection.
2. Appends `$instance` to the table name if provided.
3. Creates the table with full-text indexes (using `ngram` parser for MySQL ≥5.7.6).
4. Sets permissions via `cms_permission()`.

#### Usage Example
```php
$forum = new forum("support"); // Creates/uses table `cms_forum_support`
```

---

### Method: `add`

#### Purpose
Creates a new forum post with optional email notifications to parent post authors.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | - | Parent post ID (0 for root posts). |
| `$title` | `string` | - | Post title. |
| `$text` | `string` | - | Post content. |
| `$email` | `bool` | - | Enable email notifications. |
| `$test` | `bool` | `FALSE` | Dry-run mode (no database changes). |

#### Return Values
| Type | Description |
|------|-------------|
| `int` | New post ID on success. |
| `bool` | `FALSE` on failure. |

#### Inner Mechanisms
1. Validates permissions (`$writer` or `$operator`).
2. Inserts post into the database with current timestamp.
3. Logs the action via the `log` class.
4. Sends email notifications if:
   - Parent post exists.
   - Parent post has email notifications enabled.
   - SMTP module is loaded.

#### Usage Example
```php
$post_id = $forum->add(0, "Help Needed", "How do I install PWNC?", TRUE);
// Creates a root post and enables email notifications
```

---

### Method: `test_add`

#### Purpose
Validates if a user can create a post without actually saving it.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Parent post ID to test. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` if allowed, `FALSE` otherwise. |

#### Usage Example
```php
if ($forum->test_add(123)) {
    echo "You can reply to post #123!";
}
```

---

### Method: `edit`

#### Purpose
Updates an existing post if the user has permissions.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | - | Post ID to edit. |
| `$title` | `string` | - | New title. |
| `$text` | `string` | - | New content. |
| `$email` | `bool` | - | New email notification setting. |
| `$test` | `bool` | `FALSE` | Dry-run mode. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms
1. Allows edits if:
   - User is the post author (and not anonymous).
   - User is an `$operator`.
2. Updates the post in the database.

#### Usage Example
```php
if ($forum->edit(42, "Updated Title", "New content...", FALSE)) {
    echo "Post updated!";
}
```

---

### Method: `move`

#### Purpose
Moves a post to a new parent, preventing circular references.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | - | Post ID to move. |
| `$parent` | `int` | - | New parent post ID. |
| `$test` | `bool` | `FALSE` | Dry-run mode. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Inner Mechanisms
1. Validates `$operator` permissions.
2. Checks for circular references using `mysql->is_child()`.
3. Updates the post's `container` field.

#### Usage Example
```php
$forum->move(42, 10); // Moves post #42 under post #10
```

---

### Method: `delete`

#### Purpose
Deletes a post and its children (via `mysql->delete`).

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$index` | `int` | - | Post ID to delete. |
| `$test` | `bool` | `FALSE` | Dry-run mode. |

#### Return Values
| Type | Description |
|------|-------------|
| `bool` | `TRUE` on success, `FALSE` on failure. |

#### Usage Example
```php
$forum->delete(42); // Deletes post #42
```

---

### Method: `search`

#### Purpose
Searches posts using MySQL full-text search with relevance scoring.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Search query. |

#### Return Values
| Type | Description |
|------|-------------|
| `resource` | MySQL result set with `relevance` column. |
| `bool` | `FALSE` if forum is disabled. |

#### Inner Mechanisms
1. Logs the search via the `log` class.
2. Sanitizes input for boolean mode (appends `*` to words).
3. Combines natural language and boolean mode searches with weighted relevance:
   - Title matches ×2.
   - Text matches ×1.

#### Usage Example
```php
$result = $forum->search("installation guide");
while ($row = mysql_fetch_assoc($result)) {
    echo "Found: {$row['title']} (Relevance: {$row['relevance']})";
}
```

---

### Method: `log_access`

#### Purpose
Increments the view counter for a post.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Post ID to log. |

#### Usage Example
```php
$forum->log_access(42); // Increments access counter for post #42
```


<!-- HASH:b85eaa51ad151f08a4887e16acb95b60 -->
