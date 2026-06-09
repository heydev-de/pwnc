# PWNC API Documentation

[← Index](../../README.md) | [`module/#desktop/desktop.mailbox.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23desktop/desktop.mailbox.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Desktop Mailbox Module

The `desktop.mailbox.inc` file provides a comprehensive email management system within the PWNC Web Platform. It handles email retrieval, composition, organization, and spam filtering through a desktop-like interface. The module integrates with POP3/SMTP protocols and supports MIME message parsing, character set conversion, and Bayesian spam filtering.

---

## Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_MAILBOX_PROPERTY_DATE` | `1` | Property identifier for message date. |
| `CMS_MAILBOX_PROPERTY_FROM` | `2` | Property identifier for sender information. |
| `CMS_MAILBOX_PROPERTY_SUBJECT` | `3` | Property identifier for message subject. |
| `CMS_MAILBOX_PROPERTY_STATUS` | `4` | Property identifier for message status flags. |
| `CMS_MAILBOX_PROPERTY_SPAM_INDICATOR` | `5` | Property identifier for spam probability. |
| `CMS_MAILBOX_PROPERTY_SIZE` | `6` | Property identifier for message size in bytes. |
| `CMS_MAILBOX_STATUS_NONE` | `0` | No status flags set. |
| `CMS_MAILBOX_STATUS_IMPORTANCE_HIGH` | `1` | Message marked as high importance. |
| `CMS_MAILBOX_STATUS_READ` | `2` | Message has been read. |
| `CMS_MAILBOX_STATUS_SENT` | `4` | Message has been sent. |
| `CMS_MAILBOX_STATUS_BAD` | `8` | Message marked as spam. |
| `CMS_MAILBOX_STATUS_GOOD` | `16` | Message marked as non-spam. |
| `CMS_MAILBOX_STATUS_DRAFT` | `32` | Message is a draft. |
| `CMS_MAILBOX_STATUS_ATTACHMENT` | `64` | Message contains attachments. |
| `CMS_MAILBOX_DRAFT_TYPE_MESSAGE` | `1` | Draft type: new message. |
| `CMS_MAILBOX_DRAFT_TYPE_REPLY` | `2` | Draft type: reply to a message. |
| `CMS_MAILBOX_DRAFT_TYPE_RELAY` | `3` | Draft type: forward a message. |
| `CMS_MAILBOX_DRAFT_OPTION_TEXT` | `1` | Draft option: text body. |
| `CMS_MAILBOX_DRAFT_OPTION_ATTACHMENT` | `2` | Draft option: file attachment. |

---

## Functions

### `mailbox_directory()`

**Purpose:**
Retrieves a list of available mailbox directories (containers) for the current user/object.

**Parameters:**
None.

**Return Values:**
- `array`: Associative array mapping display names to directory names.

**Inner Mechanisms:**
- Scans the desktop object directory for subdirectories.
- Uses predefined container names if available; otherwise, URL-decodes directory names for display.
- Excludes `.` and `..` directories.

**Usage Context:**
Used to populate dropdown menus for container selection (e.g., Inbox, Drafts, Sent).

**Example:**
```php
$containers = mailbox_directory();
foreach ($containers as $name => $dir) {
    echo "<option value=\"$dir\">$name</option>";
}
```

---

### `mailbox_convert($value, $charset = NULL)`

**Purpose:**
Converts a string to UTF-8 and normalizes it for consistent processing.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Input string to convert. |
| `$charset` | `string\|NULL` | Source character set. If `NULL`, assumes ISO-8859-1. |

**Return Values:**
- `string`: Normalized UTF-8 string.

**Inner Mechanisms:**
- Checks if the input is already UTF-8 using `utf8_detect()`.
- Converts from the specified charset (or ISO-8859-1) to UTF-8 if needed.
- Normalizes the UTF-8 string using `utf8_normalize()`.

**Usage Context:**
Used to ensure all email text (subjects, bodies, headers) is in UTF-8 before display or processing.

**Example:**
```php
$subject = mailbox_convert($raw_subject, "ISO-8859-1");
echo x($subject); // Safely output in HTML
```

---

### `mailbox_text(&$data)`

**Purpose:**
Extracts and concatenates text content from a MIME message for spam analysis or indexing.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `&$data` | `data` | MIME message data object (passed by reference). |

**Return Values:**
- `string`: Concatenated plain text from all text parts of the message.

**Inner Mechanisms:**
- Iterates through all MIME parts.
- Extracts the subject and body of each text part (plain or HTML).
- Converts HTML to plain text if necessary.
- Concatenates all text for analysis.

**Usage Context:**
Used by the spam filter to evaluate message content.

**Example:**
```php
$text = mailbox_text($data_message);
$spam_score = $category->evaluate_spam($text);
```

---

### `mailbox_attachment(&$data)`

**Purpose:**
Determines if a MIME message contains attachments.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `&$data` | `data` | MIME message data object (passed by reference). |

**Return Values:**
- `bool`: `TRUE` if the message contains attachments, `FALSE` otherwise.

**Inner Mechanisms:**
- Iterates through MIME parts.
- Returns `TRUE` if a non-multipart, non-text part is found.
- Also returns `TRUE` for embedded messages (`message/rfc822`).

**Usage Context:**
Used to set the `CMS_MAILBOX_STATUS_ATTACHMENT` flag in the message index.

**Example:**
```php
if (mailbox_attachment($data_message)) {
    $status |= CMS_MAILBOX_STATUS_ATTACHMENT;
}
```

---

## Message Handling

The module processes messages based on the `CMS_IFC_MESSAGE` parameter, which triggers different actions such as retrieving, displaying, composing, or moving messages.

---

### `retrieve`

**Purpose:**
Retrieves emails from a POP3 server, processes them, and stores them in the appropriate container (Inbox or Spam).

**Inner Mechanisms:**
- Establishes a POP3 connection using the user's credentials.
- Retrieves the UIDL list to identify new messages.
- For each new message:
  - Downloads the message.
  - Parses headers and body.
  - Applies spam filtering if enabled.
  - Stores the message in the Inbox or Spam container.
  - Updates the message index with metadata.
- Deletes messages from the server if configured.

**Usage Context:**
Triggered by the "Refresh" button in the mailbox interface.

**Example:**
```javascript
ifc_post("retrieve", "");
```

---

### `display` / `_display`

**Purpose:**
Displays a message in the interface (`display`) or serves a MIME part as a download (`_display`).

**Parameters (via `CMS_IFC_PARAM`):**
- Message ID to display.

**Inner Mechanisms (`display`):**
- Opens the message data file.
- Displays headers (From, To, Date, Subject).
- Renders each MIME part with appropriate icons and links.
- Marks the message as read.

**Inner Mechanisms (`_display`):**
- Serves a specific MIME part as a download or inline display.
- Sets HTTP headers for content type, disposition, and caching.
- Supports ETag-based conditional requests.

**Usage Context:**
- `display`: Triggered when a user clicks on a message.
- `_display`: Used for downloading attachments or viewing parts in the browser.

**Example:**
```javascript
// Display a message
ifc_post("display", message_id);

// Download an attachment
window.location = cms_url({
    desktop_display: "interface",
    ifc_message: "_display",
    ifc_param: message_id,
    part: part_key
});
```

---

### `move`

**Purpose:**
Moves selected messages from one container to another.

**Parameters (via `CMS_IFC_PARAM`):**
- `list[]`: Array of message IDs to move.
- `target`: Destination container.

**Inner Mechanisms:**
- Renames message files from the source to the target directory.
- Does nothing if the source and target are the same.

**Usage Context:**
Triggered by the "Move" button after selecting messages.

**Example:**
```javascript
ifc_post("move", "", { list: [message_id1, message_id2], target: "#trashbin" });
```

---

### Message Composition (`compose`, `compose_reply`, `compose_relay`, `compose_edit`, `compose_send`, `compose_save`)

**Purpose:**
Handles the creation, editing, and sending of email messages.

#### `compose`

**Purpose:**
Creates a new draft message.

**Inner Mechanisms:**
- Initializes a new MIME message with a multipart frame.
- Sets the sender's address and name.
- Creates a text body part.
- Saves the draft in the `#draft` container.

**Usage Context:**
Triggered by the "Compose" button.

---

#### `compose_reply`

**Purpose:**
Creates a reply draft to an existing message.

**Inner Mechanisms:**
- Extracts the reply-to address from the original message.
- Copies the original message's text into the reply body.
- Prepends a reply prefix to the subject.
- Creates a new draft with the reply content.

**Usage Context:**
Triggered by the "Reply" button in the message display.

---

#### `compose_relay`

**Purpose:**
Creates a forward draft of an existing message.

**Inner Mechanisms:**
- Attaches the original message as an embedded `message/rfc822` part.
- Creates a new draft with the forward content.

**Usage Context:**
Triggered by the "Forward" button in the message display.

---

#### `compose_edit`

**Purpose:**
Edits an existing draft message.

**Inner Mechanisms:**
- Loads the draft message.
- Populates the composition form with the draft's content.

**Usage Context:**
Triggered when a user clicks on a draft message.

---

#### `compose_send`

**Purpose:**
Sends a draft message via SMTP.

**Inner Mechanisms:**
- Updates the draft with the latest form data (To, Cc, Bcc, Subject, Body, Attachments).
- Establishes an SMTP connection.
- Sends the message.
- Moves the message to the `#outbox` container.
- Updates the message status to `CMS_MAILBOX_STATUS_SENT`.

**Usage Context:**
Triggered by the "Send" button in the composition form.

**Example:**
```javascript
ifc_post("compose_send", message_id, {
    to: "recipient@example.com",
    subject: "Hello",
    ifc_param2: "Message body"
});
```

---

#### `compose_save`

**Purpose:**
Saves a draft message without sending it.

**Inner Mechanisms:**
- Updates the draft with the latest form data.
- Leaves the message in the `#draft` container.

**Usage Context:**
Triggered by the "Save" button in the composition form.

---

### Container Management (`create_container`, `rename_container`, `delete_container`)

**Purpose:**
Manages mailbox containers (folders).

#### `create_container`

**Purpose:**
Creates a new container.

**Parameters (via `CMS_IFC_PARAM`):**
- Name of the new container.

**Inner Mechanisms:**
- Creates a new directory under the object's mailbox path.

**Usage Context:**
Triggered by the "Create Folder" button.

**Example:**
```javascript
ifc_post("create_container", "Projects");
```

---

#### `rename_container`

**Purpose:**
Renames an existing container.

**Parameters (via `CMS_IFC_PARAM`):**
- New name for the container.

**Inner Mechanisms:**
- Renames the container directory.

**Usage Context:**
Triggered by the "Rename" button in the container menu.

---

#### `delete_container`

**Purpose:**
Deletes a container and its contents.

**Inner Mechanisms:**
- Moves message files to the `#trashbin` container.
- Deletes non-message files using `filemanager_delete()`.
- Removes the container directory.

**Usage Context:**
Triggered by the "Delete Folder" button.

---

### Spam Training (`train_bad`, `train_good`)

**Purpose:**
Trains the spam filter by marking messages as spam or non-spam.

#### `train_bad`

**Purpose:**
Marks a message as spam and trains the filter.

**Parameters (via `CMS_IFC_PARAM`):**
- Message ID to mark as spam.

**Inner Mechanisms:**
- Extracts the message text using `mailbox_text()`.
- Undoes any previous "good" training.
- Moves the message to the `#spam` container if not already there.
- Trains the spam filter with the message text.

**Usage Context:**
Triggered by the "Mark as Spam" button.

---

#### `train_good`

**Purpose:**
Marks a message as non-spam and trains the filter.

**Parameters (via `CMS_IFC_PARAM`):**
- Message ID to mark as non-spam.

**Inner Mechanisms:**
- Extracts the message text using `mailbox_text()`.
- Undoes any previous "spam" training.
- Moves the message from the `#spam` container to the `#inbox` if applicable.
- Trains the spam filter with the message text.

**Usage Context:**
Triggered by the "Mark as Good" button.

---

### `empty_trashbin`

**Purpose:**
Empties the trashbin by deleting all messages.

**Inner Mechanisms:**
- Deletes local message files.
- Attempts to delete remote messages from the POP3 server if the connection is successful.
- Updates the message index.

**Usage Context:**
Triggered by the "Empty Trashbin" button.

---

### Configuration (`configure`, `_configure`)

**Purpose:**
Displays and saves mailbox configuration settings.

#### `configure`

**Purpose:**
Displays the configuration form.

**Inner Mechanisms:**
- Creates an `ifc` form with fields for email settings (name, address, POP3/SMTP hosts, etc.).
- Populates fields with current values from the desktop object.

**Usage Context:**
Triggered by the "Configuration" button.

---

#### `_configure`

**Purpose:**
Saves the configuration settings.

**Parameters (via `CMS_IFC_PARAM`):**
- Form fields: name, address, username, password, POP3/SMTP hosts, signature, spam threshold, delete flag.

**Inner Mechanisms:**
- Updates the desktop object with the new settings.
- Saves the desktop object.

**Usage Context:**
Triggered when the configuration form is submitted.

---

## Main Display

The main display renders the mailbox interface, including:
- A container selection dropdown.
- A message list with sortable columns (Date, From, Subject, Spam Indicator).
- Action buttons for message management.
- A menu for mailbox operations (Refresh, Compose, Create Folder, etc.).

**Sorting:**
- Messages are sorted by the selected property (`CMS_MAILBOX_PROPERTY_DATE`, `CMS_MAILBOX_PROPERTY_FROM`, etc.).
- Sort order is indicated by arrows (↘ for descending, ↗ for ascending).

**Message Status Icons:**
- High importance: Alert icon.
- Spam: Spam icon.
- Draft: Draft icon.
- Sent: Sent icon.
- Unread: Bold subject.

**Example:**
```php
// The main display is automatically rendered when the module is loaded.
// No direct function call is needed.
```


<!-- HASH:e453357af47ac2135e395b83843d4d3b -->
