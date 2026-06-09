# PWNC API Documentation

[← Index](../../README.md) | [`module/#module/mod.comment.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23module/mod.comment.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Comment Module (`mod.comment.inc`)

The **Comment Module** provides a complete commenting system for PWNC-based websites. It handles:
- **Comment submission** (with optional CAPTCHA)
- **Comment moderation** (approval, editing, deletion, spam marking)
- **Comment rating** (upvote/downvote)
- **Pagination** and **display** of comments
- **Email notifications** for new comments and moderation actions

The module integrates with PWNC’s permission system, allowing fine-grained control over who can read, write, or moderate comments.

---

### Global Variables

| Name | Type | Description |
|------|------|-------------|
| `$comment_message` | `string` | Action to perform (e.g., `"add"`, `"edit"`, `"delete"`). |
| `$comment_index` | `string` | Unique identifier of the comment being processed. |
| `$comment_page` | `int` | Current pagination page. |
| `$comment_add_*` | `string` | Form fields for adding a new comment. |
| `$comment_edit_*` | `string` | Form fields for editing an existing comment. |

---

### Class: `comment`

The `comment` class encapsulates all comment-related logic, including database operations, permission checks, and spam detection.

#### Properties

| Name | Type | Description |
|------|------|-------------|
| `$instance` | `string` | Identifier for the content instance (e.g., a blog post). |
| `$enabled` | `bool` | Whether the comment system is enabled for this instance. |
| `$reader` | `bool` | Whether the current user can read comments. |
| `$writer` | `bool` | Whether the current user can post comments. |
| `$operator` | `bool` | Whether the current user can moderate comments. |
| `$default_status` | `int` | Default status for new comments (`CMS_DB_COMMENT_STATUS_ACTIVE` or `CMS_DB_COMMENT_STATUS_INACTIVE`). |
| `$spam_threshold` | `int` | Spam probability threshold (0–100) for automatic rejection. |

---

### Methods

---

#### `comment::__construct($instance)`

**Purpose:**
Initializes the comment system for a specific content instance.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$instance` | `string` | Content instance identifier (e.g., `"blog/123"`). |

**Return Values:**
- `void`

**Inner Mechanisms:**
1. Sets the `$instance` property.
2. Checks if the comment system is enabled for this instance.
3. Validates the current user’s permissions (`reader`, `writer`, `operator`).

**Usage Context:**
Called automatically when the module is loaded. Typically used to bind comments to a specific page or post.

**Example:**
```php
$comment = new comment("blog/123");
if ($comment->enabled) {
    // Render comment section
}
```

---

#### `comment->add($name, $email, $url, $text)`

**Purpose:**
Adds a new comment to the database.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Commenter’s name. |
| `$email` | `string` | Commenter’s email (validated). |
| `$url` | `string` | Commenter’s homepage URL (optional). |
| `$text` | `string` | Comment text. |

**Return Values:**
- `string|int|false`:
  - **`string`**: Unique comment index on success.
  - **`-1`**: Comment flagged as spam.
  - **`-2`**: Duplicate comment detected.
  - **`false`**: Database error.

**Inner Mechanisms:**
1. Validates input data (e.g., email format, non-empty text).
2. Checks for spam using a probability score (stored in `CMS_DB_COMMENT_SPAM_PROBABILITY`).
3. Rejects duplicates by comparing hashes of the comment text.
4. Inserts the comment into the database with the default status.

**Usage Context:**
Called when a user submits a new comment. Handles both anonymous and logged-in users.

**Example:**
```php
$index = $comment->add(
    "John Doe",
    "john@example.com",
    "https://example.com",
    "This is a great post!"
);
if ($index === -1) {
    echo "Your comment was flagged as spam.";
} elseif ($index === false) {
    echo "An error occurred. Please try again.";
} else {
    echo "Comment posted successfully!";
}
```

---

#### `comment->edit($index, $name, $email, $url, $text)`

**Purpose:**
Updates an existing comment.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Comment index. |
| `$name` | `string` | Updated name. |
| `$email` | `string` | Updated email. |
| `$url` | `string` | Updated URL. |
| `$text` | `string` | Updated text. |

**Return Values:**
- `bool`: `true` on success, `false` on failure.

**Inner Mechanisms:**
1. Validates input data.
2. Updates the comment in the database.
3. Logs the edit (if enabled).

**Usage Context:**
Used by moderators to edit comments.

**Example:**
```php
if ($comment->edit("abc123", "Jane Doe", "jane@example.com", "", "Updated text")) {
    echo "Comment updated!";
} else {
    echo "Failed to update comment.";
}
```

---

#### `comment->status($index, $status)`

**Purpose:**
Changes the visibility status of a comment.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Comment index. |
| `$status` | `int` | New status (`CMS_DB_COMMENT_STATUS_ACTIVE`, `CMS_DB_COMMENT_STATUS_HIDDEN`, or `CMS_DB_COMMENT_STATUS_INACTIVE`). |

**Return Values:**
- `bool`: `true` on success, `false` on failure.

**Inner Mechanisms:**
1. Updates the `CMS_DB_COMMENT_STATUS` field in the database.
2. Sends email notifications if the status changes from `INACTIVE` to `ACTIVE`.

**Usage Context:**
Used by moderators to approve, hide, or disable comments.

**Example:**
```php
if ($comment->status("abc123", CMS_DB_COMMENT_STATUS_ACTIVE)) {
    echo "Comment approved!";
}
```

---

#### `comment->delete($index, $spam = false)`

**Purpose:**
Deletes a comment or marks it as spam.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Comment index. |
| `$spam` | `bool` | If `true`, marks the comment as spam (default: `false`). |

**Return Values:**
- `bool`: `true` on success, `false` on failure.

**Inner Mechanisms:**
1. Deletes the comment from the database.
2. If `$spam` is `true`, updates spam statistics.

**Usage Context:**
Used by moderators to remove comments or flag them as spam.

**Example:**
```php
if ($comment->delete("abc123", true)) {
    echo "Comment marked as spam and deleted.";
}
```

---

#### `comment->rate_good($index)` and `comment->rate_bad($index)`

**Purpose:**
Records an upvote or downvote for a comment.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Comment index. |

**Return Values:**
- `bool`: `true` on success, `false` on failure.

**Inner Mechanisms:**
1. Checks if the user has already voted.
2. Updates the `CMS_DB_COMMENT_RATING_VALUE` and `CMS_DB_COMMENT_RATING_COUNT` fields.
3. Records the user’s IP hash in `CMS_DB_COMMENT_RATING_USERID` to prevent duplicate votes.

**Usage Context:**
Called when a user clicks the "Like" or "Dislike" button.

**Example:**
```php
if ($comment->rate_good("abc123")) {
    echo "Upvoted!";
} else {
    echo "You have already voted.";
}
```

---

### Module Workflow

1. **Initialization:**
   - Loads the `comment` library and checks if the module is enabled.
   - Validates user permissions (`reader`, `writer`, `operator`).

2. **Action Handling:**
   - Processes form submissions (e.g., `add`, `edit`, `delete`).
   - Validates input data and checks for spam.
   - Sends email notifications for new comments or moderation actions.

3. **Display:**
   - Renders comments with pagination.
   - Shows rating bars and moderation controls (if permitted).
   - Displays forms for adding/editing comments.

---

### Example: Adding a Comment Section to a Page

```php
// Include the comment module
include_once "module/#module/mod.comment.inc";

// Initialize for a blog post
$comment = new comment("blog/123");

// Render the comment section
if ($comment->enabled) {
    // Display existing comments
    $comment->display();

    // Show the comment form if the user can post
    if ($comment->writer) {
        echo '<form method="post" action="' . x(cms_url(["comment_message" => "add"])) . '">';
        echo '<textarea name="comment_add_text"></textarea>';
        echo '<button type="submit">Post Comment</button>';
        echo '</form>';
    }
}
```

---

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_DB_COMMENT_STATUS_ACTIVE` | `1` | Comment is visible. |
| `CMS_DB_COMMENT_STATUS_HIDDEN` | `0` | Comment is hidden. |
| `CMS_DB_COMMENT_STATUS_INACTIVE` | `-1` | Comment requires approval. |
| `CMS_COMMENT_PERMISSION_READER` | `"comment.reader"` | Permission to read comments. |
| `CMS_COMMENT_PERMISSION_WRITER` | `"comment.writer"` | Permission to post comments. |
| `CMS_COMMENT_PERMISSION_OPERATOR` | `"comment.operator"` | Permission to moderate comments. |

---

### Error Messages

| Constant | Description |
|----------|-------------|
| `CMS_L_MOD_COMMENT_001` | "Name is required." |
| `CMS_L_MOD_COMMENT_002` | "Invalid email address." |
| `CMS_L_MOD_COMMENT_004` | "Comment text is required." |
| `CMS_L_MOD_COMMENT_022` | "Failed to add comment." |
| `CMS_L_MOD_COMMENT_038` | "Comment flagged as spam." |
| `CMS_L_MOD_COMMENT_040` | "Duplicate comment detected." |
| `CMS_L_MOD_COMMENT_045` | "CAPTCHA verification failed." |

---


<!-- HASH:32d21ca47cfb80d55ac7042c9136ca6f -->
