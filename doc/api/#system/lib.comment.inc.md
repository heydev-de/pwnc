# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.comment.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.comment.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Comment Class

The `comment` class provides a comprehensive system for managing user comments within the PWNC Web Platform. It handles comment creation, modification, status management, rating, and deletion while integrating with spam detection and permission systems.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| **Permission Constants** | | |
| `CMS_COMMENT_PERMISSION_OPERATOR` | `"operator"` | Permission level for full comment management |
| `CMS_COMMENT_PERMISSION_WRITER` | `"writer"` | Permission level for creating comments |
| `CMS_COMMENT_PERMISSION_READER` | `"reader"` | Permission level for reading and rating comments |
| **Status Constants** | | |
| `CMS_DB_COMMENT_STATUS_INACTIVE` | `0` | Comment is inactive (not visible) |
| `CMS_DB_COMMENT_STATUS_ACTIVE` | `1` | Comment is active (visible) |
| `CMS_DB_COMMENT_STATUS_HIDDEN` | `2` | Comment is hidden (visible only to operators) |
| **Database Constants** | | |
| `CMS_DB_COMMENT` | `CMS_DB_PREFIX . "comment"` | Database table name for comments |
| `CMS_DB_COMMENT_INDEX` | `"id"` | Primary key column |
| `CMS_DB_COMMENT_INSTANCE` | `"instance"` | Instance identifier column |
| `CMS_DB_COMMENT_STATUS` | `"status"` | Status column |
| `CMS_DB_COMMENT_TIME` | `"time"` | Timestamp column |
| `CMS_DB_COMMENT_NAME` | `"name"` | Author name column |
| `CMS_DB_COMMENT_EMAIL` | `"email"` | Author email column |
| `CMS_DB_COMMENT_URL` | `"url"` | Author URL column |
| `CMS_DB_COMMENT_TEXT` | `"text"` | Comment text column |
| `CMS_DB_COMMENT_HASH` | `"hash"` | Content hash column (for duplicate detection) |
| `CMS_DB_COMMENT_RATING_VALUE` | `"rating_value"` | Rating value column (sum of all ratings) |
| `CMS_DB_COMMENT_RATING_COUNT` | `"rating_count"` | Rating count column |
| `CMS_DB_COMMENT_RATING_USERID` | `"rating_userid"` | User IDs who rated (prevents duplicate ratings) |
| `CMS_DB_COMMENT_SPAM_PROBABILITY` | `"spam_probability"` | Spam probability score |

### Properties

| Name | Default | Description |
|------|---------|-------------|
| `$instance` | `""` | Instance identifier for scoping comments |
| `$enabled` | `FALSE` | Whether the comment system is enabled |
| `$operator` | `FALSE` | Whether current user has operator permissions |
| `$writer` | `FALSE` | Whether current user has writer permissions |
| `$reader` | `FALSE` | Whether current user has reader permissions |
| `$default_status` | `CMS_DB_COMMENT_STATUS_INACTIVE` | Default status for new comments |
| `$spam_threshold` | `95` | Spam probability threshold (0-100) |

---

### `__construct($instance = "")`

**Purpose:**
Initializes a comment instance, verifies database table structure, and sets user permissions.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$instance` | `string` | Instance identifier (e.g., `"blog_post_123"`) |

**Return Values:**
- `void`

**Inner Mechanisms:**
1. Creates a `mysql` instance to verify the database table structure.
2. Checks if the `comment` table exists with the required columns and indexes.
3. Sets permissions (`operator`, `writer`, `reader`) based on the current user's role for the given instance.
4. Enables the comment system if the table is valid.

**Usage Context:**
- Called when initializing a comment system for a specific content instance (e.g., a blog post or product page).
- Ensures the database is ready before any comment operations.

**Example:**
```php
$comments = new comment("blog_post_42");
if ($comments->enabled) {
    // Comment system is ready for use
}
```

---

### `add($name, $email, $url, $text)`

**Purpose:**
Adds a new comment to the database after spam evaluation and duplicate checks.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Author name |
| `$email` | `string` | Author email |
| `$url` | `string` | Author URL (optional) |
| `$text` | `string` | Comment text |

**Return Values:**
- `int` (positive): Comment ID on success.
- `-1`: Comment flagged as spam.
- `-2`: Duplicate comment detected.
- `FALSE`: Failure (e.g., no permissions or database error).

**Inner Mechanisms:**
1. Checks if the comment system is enabled and the user has writer permissions.
2. Evaluates spam probability using the `category` class (if loaded).
3. Rejects comments exceeding the spam threshold (`$spam_threshold`).
4. Checks for duplicate comments (same content within the last hour).
5. Inserts the comment into the database with a content hash (for future duplicate checks).
6. Automatically trains the spam filter if the comment is active by default.
7. Logs the action.

**Usage Context:**
- Used in frontend forms where users submit comments.
- Integrates with spam detection to filter low-quality content.

**Example:**
```php
$comment_id = $comments->add(
    "John Doe",
    "john@example.com",
    "https://example.com",
    "This is a great article!"
);
if ($comment_id > 0) {
    echo "Comment posted successfully!";
} elseif ($comment_id == -1) {
    echo "Your comment was flagged as spam.";
}
```

---

### `edit($index, $name, $email, $url, $text)`

**Purpose:**
Edits an existing comment (operator-only).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Comment ID |
| `$name` | `string` | Updated author name |
| `$email` | `string` | Updated author email |
| `$url` | `string` | Updated author URL |
| `$text` | `string` | Updated comment text |

**Return Values:**
- `TRUE`: Success.
- `FALSE`: Failure (e.g., no permissions or database error).

**Inner Mechanisms:**
1. Checks if the comment system is enabled and the user has operator permissions.
2. Retrieves the current comment status and text.
3. Updates the comment in the database with new values.
4. Adjusts spam filter training if the comment was previously active.

**Usage Context:**
- Used in admin interfaces for moderating comments.
- Ensures spam filters are updated if comment content changes.

**Example:**
```php
if ($comments->edit(42, "Jane Doe", "jane@example.com", "", "Updated comment text.")) {
    echo "Comment updated successfully!";
}
```

---

### `status($index, $status)`

**Purpose:**
Changes the status of a comment (operator-only).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Comment ID |
| `$status` | `int` | New status (`CMS_DB_COMMENT_STATUS_ACTIVE` or `CMS_DB_COMMENT_STATUS_HIDDEN`) |

**Return Values:**
- `TRUE`: Success or no change needed.
- `FALSE`: Failure (e.g., no permissions or invalid status).

**Inner Mechanisms:**
1. Checks if the comment system is enabled and the user has operator permissions.
2. Validates the new status (cannot set to `INACTIVE`).
3. Retrieves the current status and text.
4. Updates the status in the database.
5. Adjusts spam filter training based on status changes:
   - If hiding an active comment, undoes its "nospam" training.
   - If activating a comment, trains it as "nospam".

**Usage Context:**
- Used in moderation workflows to approve or hide comments.
- Ensures spam filters remain accurate.

**Example:**
```php
if ($comments->status(42, CMS_DB_COMMENT_STATUS_ACTIVE)) {
    echo "Comment approved!";
}
```

---

### `rate_good($index, $invert = FALSE)`

**Purpose:**
Increments the positive rating of a comment (or decrements if `$invert` is `TRUE`).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Comment ID |
| `$invert` | `bool` | If `TRUE`, decrements the rating (used for "bad" ratings) |

**Return Values:**
- `TRUE`: Success.
- `FALSE`: Failure (e.g., no permissions or user already rated).

**Inner Mechanisms:**
1. Checks if the comment system is enabled and the user has reader permissions.
2. Uses the user's IP hash (`CMS_IPHASH`) to prevent duplicate ratings.
3. Updates the comment's rating value and count if the user hasn't rated it before.

**Usage Context:**
- Used in frontend interfaces for upvoting/downvoting comments.
- Prevents duplicate ratings from the same user.

**Example:**
```php
if ($comments->rate_good(42)) {
    echo "Thanks for your feedback!";
}
```

---

### `rate_bad($index)`

**Purpose:**
Decrements the rating of a comment (alias for `rate_good($index, TRUE)`).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Comment ID |

**Return Values:**
- `TRUE`: Success.
- `FALSE`: Failure.

**Usage Context:**
- Used for downvoting comments.

**Example:**
```php
$comments->rate_bad(42);
```

---

### `delete($index, $spam = FALSE)`

**Purpose:**
Deletes a comment (operator-only) and optionally trains the spam filter.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Comment ID |
| `$spam` | `bool` | If `TRUE`, trains the comment as spam |

**Return Values:**
- `TRUE`: Success.
- `FALSE`: Failure.

**Inner Mechanisms:**
1. Checks if the comment system is enabled and the user has operator permissions.
2. If `$spam` is `TRUE`, retrieves the comment text and trains the spam filter:
   - Undoes "nospam" training if the comment was active.
   - Trains the comment as spam.
3. Deletes the comment from the database.

**Usage Context:**
- Used in moderation workflows to remove inappropriate comments.
- Integrates with spam detection to improve future filtering.

**Example:**
```php
if ($comments->delete(42, TRUE)) {
    echo "Comment deleted and marked as spam.";
}
```


<!-- HASH:ce7b48148a11fe421671808a064327e5 -->
