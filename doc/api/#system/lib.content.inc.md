# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.content.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.content.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Content Management System (CMS) - Content Module

The `lib.content.inc` file provides the core content management functionality for the PWNC Web Platform. It includes constants, utility functions, and the `content` class for handling content creation, modification, publication, and versioning.

---

## Constants

### Content Types
| Name                     | Value | Description                          |
|--------------------------|-------|--------------------------------------|
| `CMS_CONTENT_TYPE_ORIGINAL` | 1     | Original content                     |
| `CMS_CONTENT_TYPE_DUPLICATE` | 2     | Duplicated content                   |
| `CMS_CONTENT_TYPE_COPY`     | 3     | Copied content                       |

### Content Statuses
| Name                          | Value | Description                          |
|-------------------------------|-------|--------------------------------------|
| `CMS_CONTENT_STATUS_DRAFT`      | 1     | Draft status                         |
| `CMS_CONTENT_STATUS_DOCUMENT`   | 2     | Document status (reviewed)           |
| `CMS_CONTENT_STATUS_PUBLICATION`| 3     | Published status                     |
| `CMS_CONTENT_STATUS_MAIL`       | 4     | Reserved for future use              |
| `CMS_CONTENT_STATUS_POOL`       | 5     | Reserved for future use              |

### Content Flags
| Name                                  | Value          | Description                          |
|---------------------------------------|----------------|--------------------------------------|
| `CMS_CONTENT_FLAG_NONE`               | 0              | No flags                             |
| `CMS_CONTENT_FLAG_SITEMAP_EXCLUDE`    | 1              | Exclude from sitemap                 |
| `CMS_CONTENT_FLAG_META_ROBOTS_NOINDEX`| 2              | Set meta robots to noindex           |
| `CMS_CONTENT_FLAG_META_ROBOTS_NOFOLLOW`| 4             | Set meta robots to nofollow          |
| `CMS_CONTENT_FLAG_ALL`                | 4294967295     | All flags                            |

### Content Actions
| Name                                 | Value | Description                          |
|--------------------------------------|-------|--------------------------------------|
| `CMS_CONTENT_ACTION_NONE`            | 0     | No action                            |
| `CMS_CONTENT_ACTION_CREATE`          | 1     | Create content                       |
| `CMS_CONTENT_ACTION_UPDATE`          | 2     | Update content                       |
| `CMS_CONTENT_ACTION_AUTHORIZE`       | 3     | Authorize content                    |
| `CMS_CONTENT_ACTION_DERIVE_DRAFT`    | 4     | Derive draft from content            |
| `CMS_CONTENT_ACTION_PUBLISH`         | 5     | Publish content                      |
| `CMS_CONTENT_ACTION_WITHDRAW`        | 6     | Withdraw content                     |
| `CMS_CONTENT_ACTION_DUPLICATE`       | 7     | Duplicate content                    |
| `CMS_CONTENT_ACTION_COPY`            | 8     | Copy content                         |
| `CMS_CONTENT_ACTION_DELETE`          | 9     | Delete content                       |
| `CMS_CONTENT_ACTION_RECEIVE`         | 10    | Receive content                      |
| `CMS_CONTENT_ACTION_CHANNEL`         | 11    | Set channel                          |
| `CMS_CONTENT_ACTION_FLAG`            | 12    | Set flags                            |
| `CMS_CONTENT_ACTION_EXTRA`           | 13    | Set extra data                       |

### Content Roles
| Name                                      | Value | Description                          |
|-------------------------------------------|-------|--------------------------------------|
| `CMS_CONTENT_ROLE_NONE`                   | 0     | No role                              |
| `CMS_CONTENT_ROLE_WRITER`                 | 1     | Writer role                          |
| `CMS_CONTENT_ROLE_EDITOR`                 | 2     | Editor role                          |
| `CMS_CONTENT_ROLE_PUBLISHER`              | 4     | Publisher role                       |
| `CMS_CONTENT_ROLE_ALL`                    | 7     | All roles                            |
| `CMS_CONTENT_ROLE_WRITER_EDITOR`          | 8     | Writer and editor roles              |
| `CMS_CONTENT_ROLE_WRITER_PUBLISHER`       | 16    | Writer and publisher roles           |
| `CMS_CONTENT_ROLE_EDITOR_PUBLISHER`       | 32    | Editor and publisher roles           |
| `CMS_CONTENT_ROLE_WRITER_EDITOR_PUBLISHER`| 64    | Writer, editor, and publisher roles  |

### Schedule Types
| Name                                 | Value | Description                          |
|--------------------------------------|-------|--------------------------------------|
| `CMS_CONTENT_SCHEDULE_TYPE_APPLY`    | 1     | Apply content changes                |
| `CMS_CONTENT_SCHEDULE_TYPE_RETRIEVE` | 2     | Retrieve content version             |
| `CMS_CONTENT_SCHEDULE_TYPE_PUBLISH`  | 3     | Publish content                      |
| `CMS_CONTENT_SCHEDULE_TYPE_WITHDRAW` | 4     | Withdraw content                     |

### Database Fields
| Name (Content Table)                     | Value (Constant)                     | Description                          |
|------------------------------------------|--------------------------------------|--------------------------------------|
| `CMS_DB_CONTENT`                         | `CMS_DB_PREFIX . "content"`          | Content table name                   |
| `CMS_DB_CONTENT_INDEX`                   | `"id"`                               | Primary key                          |
| `CMS_DB_CONTENT_OWNER`                   | `"owner"`                            | Owner of the content                 |
| `CMS_DB_CONTENT_TYPE`                    | `"type"`                             | Content type                         |
| `CMS_DB_CONTENT_STATUS`                  | `"status"`                           | Content status                       |
| `CMS_DB_CONTENT_FLAG`                    | `"flag"`                             | Content flags                        |
| `CMS_DB_CONTENT_CHANNEL`                 | `"channel"`                          | Channel for content                  |
| `CMS_DB_CONTENT_WRITER`                  | `"writer"`                           | Writer of the content                |
| `CMS_DB_CONTENT_WRITER_TIME`             | `"writer_time"`                      | Time of last write                   |
| `CMS_DB_CONTENT_WRITER_COMMENT`          | `"writer_comment"`                   | Writer's comment                     |
| `CMS_DB_CONTENT_EDITOR`                  | `"editor"`                           | Editor of the content                |
| `CMS_DB_CONTENT_EDITOR_TIME`             | `"editor_time"`                      | Time of last edit                    |
| `CMS_DB_CONTENT_EDITOR_COMMENT`          | `"editor_comment"`                   | Editor's comment                     |
| `CMS_DB_CONTENT_PUBLISHER`               | `"publisher"`                        | Publisher of the content             |
| `CMS_DB_CONTENT_PUBLISHER_TIME`          | `"publisher_time"`                   | Time of last publish                 |
| `CMS_DB_CONTENT_PUBLISHER_COMMENT`       | `"publisher_comment"`                | Publisher's comment                  |
| `CMS_DB_CONTENT_TIME`                    | `"time"`                             | Last modification time               |
| `CMS_DB_CONTENT_TITLE`                   | `"title"`                            | Title of the content                 |
| `CMS_DB_CONTENT_AUTHOR`                  | `"author"`                           | Author of the content                |
| `CMS_DB_CONTENT_DESCRIPTION`             | `"description"`                      | Description of the content           |
| `CMS_DB_CONTENT_KEYWORD`                 | `"keyword"`                          | Keywords for the content             |
| `CMS_DB_CONTENT_IMAGE`                   | `"image"`                            | Image associated with the content    |
| `CMS_DB_CONTENT_TEXT`                    | `"text"`                             | Content text                         |
| `CMS_DB_CONTENT_TEMPLATE`                | `"template"`                         | Template for the content             |
| `CMS_DB_CONTENT_BUFFER_*`                | Various                              | Buffered fields for editing          |
| `CMS_DB_CONTENT_SENDER`                  | `"sender"`                           | Sender of the content                |
| `CMS_DB_CONTENT_SENDER_TIME`             | `"sender_time"`                      | Time of sending                      |
| `CMS_DB_CONTENT_SENDER_COMMENT`          | `"sender_comment"`                   | Sender's comment                     |
| `CMS_DB_CONTENT_EXTRA_VALUE`             | `"extra_value"`                      | Extra value                          |
| `CMS_DB_CONTENT_EXTRA_TYPE`              | `"extra_type"`                       | Extra type                           |
| `CMS_DB_CONTENT_EXTRA_COLOR`             | `"extra_color"`                      | Extra color                          |

| Name (Version Table)                     | Value (Constant)                     | Description                          |
|------------------------------------------|--------------------------------------|--------------------------------------|
| `CMS_DB_CONTENT_VERSION`                 | `CMS_DB_PREFIX . "content_version"`  | Content version table name           |
| `CMS_DB_CONTENT_VERSION_INDEX`           | `"id"`                               | Primary key                          |
| `CMS_DB_CONTENT_VERSION_CONTENT`         | `"content"`                          | Content ID                           |
| `CMS_DB_CONTENT_VERSION_TIME`            | `"time"`                             | Version time                         |
| `CMS_DB_CONTENT_VERSION_TITLE`           | `"title"`                            | Version title                        |
| `CMS_DB_CONTENT_VERSION_AUTHOR`          | `"author"`                           | Version author                       |
| `CMS_DB_CONTENT_VERSION_DESCRIPTION`     | `"description"`                      | Version description                  |
| `CMS_DB_CONTENT_VERSION_KEYWORD`         | `"keyword"`                          | Version keywords                     |
| `CMS_DB_CONTENT_VERSION_IMAGE`           | `"image"`                            | Version image                        |
| `CMS_DB_CONTENT_VERSION_TEXT`            | `"text"`                             | Version text                         |
| `CMS_DB_CONTENT_VERSION_TEMPLATE`        | `"template"`                         | Version template                     |
| `CMS_DB_CONTENT_VERSION_HASH`            | `"hash"`                             | Version hash                         |

| Name (Schedule Table)                    | Value (Constant)                     | Description                          |
|------------------------------------------|--------------------------------------|--------------------------------------|
| `CMS_DB_CONTENT_SCHEDULE`                | `CMS_DB_PREFIX . "content_schedule"` | Content schedule table name          |
| `CMS_DB_CONTENT_SCHEDULE_TIME`           | `"time"`                             | Scheduled time                       |
| `CMS_DB_CONTENT_SCHEDULE_TYPE`           | `"type"`                             | Schedule type                        |
| `CMS_DB_CONTENT_SCHEDULE_CONTENT`        | `"content"`                          | Content ID                           |
| `CMS_DB_CONTENT_SCHEDULE_VALUE_1`        | `"value1"`                           | Additional value 1                   |
| `CMS_DB_CONTENT_SCHEDULE_VALUE_2`        | `"value2"`                           | Additional value 2                   |
| `CMS_DB_CONTENT_SCHEDULE_HASH`           | `"hash"`                             | Schedule hash                        |

---

## Utility Functions

### `content_get_range`
Retrieves a specific range of content from a document.

#### Parameters
| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$content`| `content`| Content object (passed by reference) |
| `$index`  | `int`    | Content index                        |
| `$range`  | `mixed`  | Range identifier                     |
| `$type`   | `string` | Type of content to retrieve          |

#### Return Values
| Type     | Description                          |
|----------|--------------------------------------|
| `mixed`  | Retrieved content or `NULL`          |

#### Inner Mechanisms
- Checks if the content object is enabled.
- Attempts to retrieve content from cache first.
- Falls back to database retrieval if cache is empty.
- Uses the `document` class to parse and retrieve the specified range.

#### Usage Example
```php
$content = new content();
$text = content_get_range($content, 123, "body");
```
Retrieves the "body" range of content with index 123.

---

### `content_set_range`
Sets or modifies a specific range of content in a document.

#### Parameters
| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$content`| `content`| Content object (passed by reference) |
| `$index`  | `int`    | Content index                        |
| `$range`  | `mixed`  | Range identifier                     |
| `$type`   | `string` | Type of operation                    |
| `$text`   | `string` | Text to set                          |

#### Return Values
| Type     | Description                          |
|----------|--------------------------------------|
| `bool`   | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Supports various operations like paste, swap, copy, kick, drop, clear, and shift.
- Uses the `document` class to manipulate content ranges.
- Updates the content in the database after modification.

#### Usage Example
```php
$content = new content();
content_set_range($content, 123, "body", "#paste", "New content");
```
Pastes "New content" into the "body" range of content with index 123.

---

### `content_get_directory_index`
Finds the directory index linked to a specific content index.

#### Parameters
| Name             | Type  | Description                          |
|------------------|-------|--------------------------------------|
| `$content_index` | `int` | Content index                        |

#### Return Values
| Type  | Description                          |
|-------|--------------------------------------|
| `int` | Directory index or `0` if not found  |

#### Inner Mechanisms
- Iterates through the directory data to find a link to the specified content.

#### Usage Example
```php
$directory_index = content_get_directory_index(123);
```
Retrieves the directory index linked to content with index 123.

---

### `content_parse`
Parses content and generates output based on the content's template.

#### Parameters
| Name           | Type      | Description                          |
|----------------|-----------|--------------------------------------|
| `$content`     | `content` | Content object (passed by reference) |
| `$index`       | `int`     | Content index                        |
| `$action`      | `array`   | Action parameters                    |
| `$header`      | `array`   | Header parameters                    |
| `$is_dynamic`  | `bool`    | Indicates if output is dynamic       |
| `$mod_time`    | `int`     | Modification time                    |

#### Return Values
| Type     | Description                          |
|----------|--------------------------------------|
| `string` | Parsed content or `FALSE`            |

#### Inner Mechanisms
- Checks if the content is enabled and loads required libraries.
- Retrieves content from the database or cache.
- Handles caching of parsed content.
- Uses the `template` class to process and generate output.

#### Usage Example
```php
$content = new content();
$output = content_parse($content, 123);
```
Parses and outputs content with index 123.

---

### `content_template_export`
Exports a content template.

#### Parameters
| Name      | Type      | Description                          |
|-----------|-----------|--------------------------------------|
| `$content`| `content` | Content object (passed by reference) |
| `$index`  | `int`     | Content index                        |
| `$range`  | `mixed`   | Range identifier (optional)          |

#### Return Values
| Type     | Description                          |
|----------|--------------------------------------|
| `string` | Exported template or `FALSE`         |

#### Inner Mechanisms
- Retrieves the content's buffered text and template.
- Uses the `template` class to export the template.

#### Usage Example
```php
$content = new content();
$template = content_template_export($content, 123);
```
Exports the template of content with index 123.

---

### `content_template_select`
Retrieves a list of available templates.

#### Return Values
| Type    | Description                          |
|---------|--------------------------------------|
| `array` | Associative array of templates       |

#### Inner Mechanisms
- Loads template data and filters for page templates.
- Returns an associative array of template names and keys.

#### Usage Example
```php
$templates = content_template_select();
```
Retrieves all available templates.

---

### `content_get_receiver`
Retrieves a list of users who can receive content of a specific type and status.

#### Parameters
| Name      | Type      | Description                          |
|-----------|-----------|--------------------------------------|
| `$content`| `content` | Content object (passed by reference) |
| `$type`   | `int`     | Content type                         |
| `$status` | `int`     | Content status                       |

#### Return Values
| Type    | Description                          |
|---------|--------------------------------------|
| `array` | Array of receivers                   |

#### Inner Mechanisms
- Checks permissions for each user to determine if they can receive the content.
- Returns an array of users grouped by content type.

#### Usage Example
```php
$content = new content();
$receivers = content_get_receiver($content, CMS_CONTENT_TYPE_ORIGINAL, CMS_CONTENT_STATUS_DRAFT);
```
Retrieves users who can receive original draft content.

---

## `content` Class

### Properties
| Name         | Type     | Description                          |
|--------------|----------|--------------------------------------|
| `$action`    | `array`  | Permission action table              |
| `$user`      | `string` | Current user                         |
| `$writer`    | `bool`   | Writer permission flag               |
| `$editor`    | `bool`   | Editor permission flag               |
| `$publisher` | `bool`   | Publisher permission flag            |
| `$operator`  | `bool`   | Operator permission flag             |
| `$enabled`   | `bool`   | Indicates if the object is enabled   |

---

### `__construct`
Initializes the content object and sets up permissions.

#### Parameters
| Name   | Type     | Description                          |
|--------|----------|--------------------------------------|
| `$user`| `string` | User identifier                      |

#### Inner Mechanisms
- Initializes the permission action table.
- Verifies and creates database tables if necessary.
- Processes scheduled actions.
- Sets user permissions based on the provided user identifier.

#### Usage Example
```php
$content = new content("admin");
```
Initializes a content object for the "admin" user.

---

### `test_create`
Tests if content creation is permitted.

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if creation is permitted      |

#### Usage Example
```php
if ($content->test_create()) {
    $content->create("New Title");
}
```
Tests if content creation is permitted before creating new content.

---

### `create`
Creates new content.

#### Parameters
| Name       | Type     | Description                          |
|------------|----------|--------------------------------------|
| `$title`   | `string` | Content title                        |
| `$template`| `string` | Template identifier (optional)       |
| `$comment` | `string` | Comment (optional)                   |
| `$test`    | `bool`   | Test mode flag                       |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `int`  | Content index or `FALSE`             |

#### Inner Mechanisms
- Validates permissions and creates a new content entry in the database.
- Sets default values and initializes buffered fields.

#### Usage Example
```php
$index = $content->create("New Title", "default_template");
```
Creates new content with the title "New Title" and the "default_template".

---

### `test_update`
Tests if content update is permitted.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if update is permitted        |

#### Usage Example
```php
if ($content->test_update(123)) {
    $content->update(123, "Updated Title");
}
```
Tests if updating content with index 123 is permitted.

---

### `update`
Updates existing content.

#### Parameters
| Name           | Type     | Description                          |
|----------------|----------|--------------------------------------|
| `$index`       | `int`    | Content index                        |
| `$title`       | `string` | Title (optional)                     |
| `$description` | `string` | Description (optional)               |
| `$keyword`     | `string` | Keywords (optional)                  |
| `$image`       | `string` | Image (optional)                     |
| `$text`        | `string` | Text (optional)                      |
| `$comment`     | `string` | Comment (optional)                   |
| `$template`    | `string` | Template (optional)                  |
| `$test`        | `bool`   | Test mode flag                       |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Validates permissions and updates the content in the database.
- Stores the current step for undo functionality.

#### Usage Example
```php
$content->update(123, "Updated Title", "New description");
```
Updates the title and description of content with index 123.

---

### `test_copy`
Tests if content can be copied.

#### Parameters
| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$index`| `int`  | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if copy is permitted          |

#### Usage Example
```php
if ($content->test_copy(123)) {
    $new_index = $content->copy(123);
}
```
Tests if content with index 123 can be copied.

---

### `copy`
Copies content.

#### Parameters
| Name        | Type   | Description                          |
|-------------|--------|--------------------------------------|
| `$index`    | `int`  | Content index                        |
| `$duplicate`| `bool` | Duplicate flag                       |
| `$test`     | `bool` | Test mode flag                       |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `int`  | New content index or `FALSE`         |

#### Inner Mechanisms
- Validates permissions and creates a copy or duplicate of the content.
- Copies all relevant fields to the new content entry.

#### Usage Example
```php
$new_index = $content->copy(123);
```
Creates a copy of content with index 123.

---

### `test_duplicate`
Tests if content can be duplicated.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if duplication is permitted   |

#### Usage Example
```php
if ($content->test_duplicate(123)) {
    $new_index = $content->duplicate(123);
}
```
Tests if content with index 123 can be duplicated.

---

### `duplicate`
Duplicates content.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `int`  | New content index or `FALSE`         |

#### Inner Mechanisms
- Calls `copy` with the `duplicate` flag set to `TRUE`.

#### Usage Example
```php
$new_index = $content->duplicate(123);
```
Creates a duplicate of content with index 123.

---

### `test_authorize`
Tests if content can be authorized.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if authorization is permitted |

#### Usage Example
```php
if ($content->test_authorize(123)) {
    $content->authorize(123);
}
```
Tests if content with index 123 can be authorized.

---

### `authorize`
Authorizes content, moving it from draft to document status.

#### Parameters
| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$index`  | `int`    | Content index                        |
| `$comment`| `string` | Comment (optional)                   |
| `$test`   | `bool`   | Test mode flag                       |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Validates permissions and updates the content status to document.
- Clears the step buffer.

#### Usage Example
```php
$content->authorize(123, "Approved for review");
```
Authorizes content with index 123.

---

### `test_derive_draft`
Tests if a draft can be derived from content.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if derivation is permitted    |

#### Usage Example
```php
if ($content->test_derive_draft(123)) {
    $content->derive_draft(123);
}
```
Tests if a draft can be derived from content with index 123.

---

### `derive_draft`
Derives a draft from existing content.

#### Parameters
| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$index`| `int`  | Content index                        |
| `$test` | `bool` | Test mode flag                       |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Validates permissions and changes the content status to draft.
- Clears the step buffer.

#### Usage Example
```php
$content->derive_draft(123);
```
Derives a draft from content with index 123.

---

### `test_publish`
Tests if content can be published.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if publication is permitted   |

#### Usage Example
```php
if ($content->test_publish(123)) {
    $content->publish(123);
}
```
Tests if content with index 123 can be published.

---

### `publish`
Publishes content.

#### Parameters
| Name               | Type     | Description                          |
|--------------------|----------|--------------------------------------|
| `$index`           | `int`    | Content index                        |
| `$comment`         | `string` | Comment (optional)                   |
| `$time_publish`    | `int`    | Scheduled publish time (optional)    |
| `$time_withdraw`   | `int`    | Scheduled withdraw time (optional)   |
| `$directory_index` | `int`    | Directory index (optional)           |
| `$directory_action`| `string` | Directory action (optional)          |
| `$directory_title` | `string` | Directory title (optional)           |
| `$test`            | `bool`   | Test mode flag                       |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Validates permissions and updates the content status to publication.
- Handles scheduled publication and directory linkage.

#### Usage Example
```php
$content->publish(123, "Published", 0, 0, 456, "replace");
```
Publishes content with index 123 and links it to directory index 456.

---

### `test_apply`
Tests if content changes can be applied.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if application is permitted   |

#### Usage Example
```php
if ($content->test_apply(123)) {
    $content->apply(123);
}
```
Tests if changes to content with index 123 can be applied.

---

### `apply`
Applies buffered changes to content.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |
| `$time` | `int` | Scheduled apply time (optional)      |
| `$test` | `bool`| Test mode flag                       |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Validates permissions and applies buffered changes to the content.
- Handles scheduled application and version storage.

#### Usage Example
```php
$content->apply(123);
```
Applies buffered changes to content with index 123.

---

### `test_revert`
Tests if content can be reverted.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if reversion is permitted     |

#### Usage Example
```php
if ($content->test_revert(123)) {
    $content->revert(123);
}
```
Tests if content with index 123 can be reverted.

---

### `revert`
Reverts content to its last applied state.

#### Parameters
| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$index`| `int`  | Content index                        |
| `$test` | `bool` | Test mode flag                       |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Validates permissions and reverts the content to its last applied state.
- Stores the current state for undo functionality.

#### Usage Example
```php
$content->revert(123);
```
Reverts content with index 123 to its last applied state.

---

### `test_version_store`
Tests if a content version can be stored.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if version storage is permitted |

#### Usage Example
```php
if ($content->test_version_store(123)) {
    $content->version_store(123);
}
```
Tests if a version of content with index 123 can be stored.

---

### `version_store`
Stores a version of the content.

#### Parameters
| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$index`| `int`  | Content index                        |
| `$test` | `bool` | Test mode flag                       |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Validates permissions and stores a version of the content in the version table.

#### Usage Example
```php
$content->version_store(123);
```
Stores a version of content with index 123.

---

### `_version_store`
Internal method to store a content version.

#### Parameters
| Name           | Type  | Description                          |
|----------------|-------|--------------------------------------|
| `$version_index`| `int`| Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Inserts a new version of the content into the version table using a hash for uniqueness.

---

### `test_version_retrieve`
Tests if a content version can be retrieved.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$version_index`| `int`| Version index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if retrieval is permitted     |

#### Usage Example
```php
if ($content->test_version_retrieve(456)) {
    $content->version_retrieve(456);
}
```
Tests if version 456 can be retrieved.

---

### `version_retrieve`
Retrieves a content version.

#### Parameters
| Name       | Type   | Description                          |
|------------|--------|--------------------------------------|
| `$version_index`| `int`| Version index                        |
| `$time`    | `int`  | Scheduled retrieval time (optional)  |
| `$apply`   | `bool` | Apply flag                           |
| `$test`    | `bool` | Test mode flag                       |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Validates permissions and retrieves a version of the content.
- Can apply the version directly or store it in the buffer.

#### Usage Example
```php
$content->version_retrieve(456, 0, TRUE);
```
Retrieves and applies version 456 to content.

---

### `schedule_add`
Adds a scheduled action for content.

#### Parameters
| Name           | Type     | Description                          |
|----------------|----------|--------------------------------------|
| `$time`        | `int`    | Scheduled time                       |
| `$type`        | `int`    | Schedule type                        |
| `$content_index`| `int`   | Content index                        |
| `$value1`      | `string` | Additional value 1 (optional)        |
| `$value2`      | `string` | Additional value 2 (optional)        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Inserts a new scheduled action into the schedule table.

#### Usage Example
```php
$content->schedule_add(time() + 3600, CMS_CONTENT_SCHEDULE_TYPE_PUBLISH, 123);
```
Schedules content with index 123 to be published in one hour.

---

### `schedule_delete`
Deletes a scheduled action.

#### Parameters
| Name           | Type     | Description                          |
|----------------|----------|--------------------------------------|
| `$type`        | `int`    | Schedule type                        |
| `$content_index`| `int`   | Content index                        |
| `$value1`      | `string` | Additional value 1 (optional)        |
| `$value2`      | `string` | Additional value 2 (optional)        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Deletes a scheduled action from the schedule table using a hash.

---

### `_schedule_delete`
Internal method to delete a scheduled action by hash.

#### Parameters
| Name   | Type     | Description                          |
|--------|----------|--------------------------------------|
| `$hash`| `string` | Schedule hash                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

---

### `test_withdraw`
Tests if content can be withdrawn.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if withdrawal is permitted    |

#### Usage Example
```php
if ($content->test_withdraw(123)) {
    $content->withdraw(123);
}
```
Tests if content with index 123 can be withdrawn.

---

### `withdraw`
Withdraws content from publication.

#### Parameters
| Name             | Type  | Description                          |
|------------------|-------|--------------------------------------|
| `$index`         | `int` | Content index                        |
| `$directory_index`| `int`| Directory index (optional)           |
| `$test`          | `bool`| Test mode flag                       |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Validates permissions and changes the content status from publication to document.
- Removes directory linkages if specified.

#### Usage Example
```php
$content->withdraw(123);
```
Withdraws content with index 123 from publication.

---

### `_withdraw`
Internal method to withdraw content and manage directory linkages.

#### Parameters
| Name             | Type  | Description                          |
|------------------|-------|--------------------------------------|
| `$index`         | `int` | Content index                        |
| `$directory_index`| `int`| Directory index (optional)           |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Removes directory linkages for the content.
- Handles scheduled publications.

---

### `test_send`
Tests if content can be sent to another user.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if sending is permitted       |

#### Usage Example
```php
if ($content->test_send(123)) {
    $content->send(123, "receiver_user");
}
```
Tests if content with index 123 can be sent to "receiver_user".

---

### `send`
Sends content to another user.

#### Parameters
| Name       | Type     | Description                          |
|------------|----------|--------------------------------------|
| `$index`   | `int`    | Content index                        |
| `$receiver`| `string` | Receiver user identifier             |
| `$comment` | `string` | Comment (optional)                   |
| `$test`    | `bool`   | Test mode flag                       |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Validates permissions and updates the content owner to the receiver.
- Clears the step buffer.

#### Usage Example
```php
$content->send(123, "receiver_user", "Please review this content");
```
Sends content with index 123 to "receiver_user" with a comment.

---

### `test_delete`
Tests if content can be deleted.

#### Parameters
| Name               | Type   | Description                          |
|--------------------|--------|--------------------------------------|
| `$index`           | `int`  | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if deletion is permitted      |

#### Usage Example
```php
if ($content->test_delete(123)) {
    $content->delete(123);
}
```
Tests if content with index 123 can be deleted.

---

### `delete`
Deletes content.

#### Parameters
| Name               | Type   | Description                          |
|--------------------|--------|--------------------------------------|
| `$index`           | `int`  | Content index                        |
| `$ignore_directory`| `bool` | Ignore directory flag                |
| `$override_owner`  | `bool` | Override owner flag                  |
| `$test`            | `bool` | Test mode flag                       |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Validates permissions and deletes the content from the database.
- Removes associated versions and scheduled actions.
- Clears the step buffer.

#### Usage Example
```php
$content->delete(123);
```
Deletes content with index 123.

---

### `test_flag_set`
Tests if content flags can be set.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if flag setting is permitted  |

#### Usage Example
```php
if ($content->test_flag_set(123)) {
    $content->flag_set(123, CMS_CONTENT_FLAG_META_ROBOTS_NOINDEX);
}
```
Tests if flags can be set for content with index 123.

---

### `flag_set`
Sets content flags.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |
| `$flag` | `int` | Flag value                           |
| `$test` | `bool`| Test mode flag                       |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Validates permissions and updates the content flags in the database.

#### Usage Example
```php
$content->flag_set(123, CMS_CONTENT_FLAG_META_ROBOTS_NOINDEX);
```
Sets the noindex flag for content with index 123.

---

### `test_channel_set`
Tests if a content channel can be set.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if channel setting is permitted |

#### Usage Example
```php
if ($content->test_channel_set(123)) {
    $content->channel_set(123, "news");
}
```
Tests if a channel can be set for content with index 123.

---

### `channel_set`
Sets the content channel.

#### Parameters
| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$index`  | `int`    | Content index                        |
| `$channel`| `string` | Channel identifier                   |
| `$test`   | `bool`   | Test mode flag                       |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Validates permissions and updates the content channel in the database.

#### Usage Example
```php
$content->channel_set(123, "news");
```
Sets the channel for content with index 123 to "news".

---

### `step_store`
Stores the current state of content for undo functionality.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Stores the current buffered text in a circular buffer for undo/redo functionality.

#### Usage Example
```php
$content->step_store(123);
```
Stores the current state of content with index 123.

---

### `test_step_undo`
Tests if an undo operation is possible.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if undo is possible           |

#### Usage Example
```php
if ($content->test_step_undo(123)) {
    $content->step_undo(123);
}
```
Tests if an undo operation is possible for content with index 123.

---

### `step_undo`
Reverts to the previous state of content.

#### Parameters
| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$index`| `int`  | Content index                        |
| `$test` | `bool` | Test mode flag                       |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Reverts the content to its previous state using the circular buffer.

#### Usage Example
```php
$content->step_undo(123);
```
Reverts content with index 123 to its previous state.

---

### `step_undo_depth`
Returns the number of available undo steps.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type  | Description                          |
|-------|--------------------------------------|
| `int` | Number of available undo steps       |

#### Usage Example
```php
$depth = $content->step_undo_depth(123);
```
Retrieves the number of available undo steps for content with index 123.

---

### `test_step_redo`
Tests if a redo operation is possible.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if redo is possible           |

#### Usage Example
```php
if ($content->test_step_redo(123)) {
    $content->step_redo(123);
}
```
Tests if a redo operation is possible for content with index 123.

---

### `step_redo`
Redoes the next state of content.

#### Parameters
| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$index`| `int`  | Content index                        |
| `$test` | `bool` | Test mode flag                       |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Redoes the content to its next state using the circular buffer.

#### Usage Example
```php
$content->step_redo(123);
```
Redoes content with index 123 to its next state.

---

### `step_redo_depth`
Returns the number of available redo steps.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type  | Description                          |
|-------|--------------------------------------|
| `int` | Number of available redo steps       |

#### Usage Example
```php
$depth = $content->step_redo_depth(123);
```
Retrieves the number of available redo steps for content with index 123.

---

### `step_clear`
Clears the step buffer for content.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Usage Example
```php
$content->step_clear(123);
```
Clears the step buffer for content with index 123.

---

### `action`
Checks if a specific action is permitted based on content type, status, and user permissions.

#### Parameters
| Name      | Type  | Description                          |
|-----------|-------|--------------------------------------|
| `$type`   | `int` | Content type                         |
| `$status` | `int` | Content status                       |
| `$action` | `int` | Action to check                      |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if action is permitted        |

#### Usage Example
```php
if ($content->action(CMS_CONTENT_TYPE_ORIGINAL, CMS_CONTENT_STATUS_DRAFT, CMS_CONTENT_ACTION_UPDATE)) {
    // Update content
}
```
Checks if updating original draft content is permitted.

---

### `refresh_extra`
Refreshes extra data for content.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Extracts extra data from the content and updates the database.

#### Usage Example
```php
$content->refresh_extra(123);
```
Refreshes extra data for content with index 123.

---

### `test_set_extra`
Tests if extra data can be set for content.

#### Parameters
| Name    | Type  | Description                          |
|---------|-------|--------------------------------------|
| `$index`| `int` | Content index                        |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` if setting extra data is permitted |

#### Usage Example
```php
if ($content->test_set_extra(123)) {
    $content->set_extra(123, "value", "type", "#FF0000");
}
```
Tests if extra data can be set for content with index 123.

---

### `set_extra`
Sets extra data for content.

#### Parameters
| Name     | Type     | Description                          |
|----------|----------|--------------------------------------|
| `$index` | `int`    | Content index                        |
| `$value` | `string` | Extra value                          |
| `$type`  | `string` | Extra type                           |
| `$color` | `string` | Extra color                          |
| `$test`  | `bool`   | Test mode flag                       |

#### Return Values
| Type   | Description                          |
|--------|--------------------------------------|
| `bool` | `TRUE` on success, `FALSE` otherwise |

#### Inner Mechanisms
- Validates permissions and updates the extra data in the database.

#### Usage Example
```php
$content->set_extra(123, "value", "type", "#FF0000");
```
Sets extra data for content with index 123.


<!-- HASH:0fd65b88aa592bb83e7176823bcb63e3 -->
