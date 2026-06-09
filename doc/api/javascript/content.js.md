# PWNC API Documentation

[← Index](../README.md) | [`javascript/content.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/content.js)

- **Version:** `26.5.30.4`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Content Editing Module (`content.js`)

This module provides client-side JavaScript functions for managing content editing operations in the PWNC Web Platform. It handles common content manipulation tasks such as copying, pasting, swapping, deleting, and repositioning content elements via AJAX or page reloads. The module integrates with the platform's localization system (`CMS_L_*` constants) and coordinates with backend URLs for state changes.

---

### Global Variables

| Name              | Value/Default               | Description                                                                 |
|-------------------|-----------------------------|-----------------------------------------------------------------------------|
| `this.name`       | `"content_edit_" + random`  | Unique identifier for the current content editing context.                  |
| `content_buffer`  | `null`                      | Temporary storage for copied content ranges (used in copy/paste operations).|

---

### Functions

---

#### `content_edit_open(url)`
Opens a content editing interface or loads a new page in the current context.

**Parameters**

| Name  | Type     | Description                                      |
|-------|----------|--------------------------------------------------|
| `url` | `string` | URL to load for content editing.                 |

**Return Value**
- `void`

**Inner Mechanisms**
- Delegates to `load_page(url)` to handle the actual page load.

**Usage Context**
- Used when initiating a content editing session or navigating to a new editing context.

**Example**
```javascript
// Open the article editing interface
content_edit_open("/edit/article/123");
```

---

#### `content_edit_command(url, text = "")`
Executes a content editing command after optional user confirmation.

**Parameters**

| Name    | Type      | Description                                      |
|---------|-----------|--------------------------------------------------|
| `url`   | `string`  | URL to execute the command (may contain placeholders). |
| `text`  | `string`  | Confirmation message (optional). If empty, no confirmation is shown. |

**Return Value**
- `void`

**Inner Mechanisms**
- If `text` is provided, shows a confirmation dialog.
- Replaces `%left%` and `%top%` placeholders in the URL with the current window position using `fx_position_left()` and `fx_position_top()`.
- Navigates to the processed URL using `location.replace(url)`.

**Usage Context**
- Core function for executing backend commands (e.g., delete, paste, apply).
- Used by higher-level functions like `content_edit_clear()` or `content_edit_apply()`.

**Example**
```javascript
// Delete content without confirmation
content_edit_command("/delete/block/456");

// Delete content with confirmation
content_edit_command("/delete/block/456", "Delete this block?");
```

---

#### `content_edit_copy(url, range)`
Copies a content range to the clipboard buffer and notifies the server.

**Parameters**

| Name    | Type      | Description                                      |
|---------|-----------|--------------------------------------------------|
| `url`   | `string`  | URL to notify the server of the copy operation.  |
| `range` | `any`     | Content range or identifier to copy.             |

**Return Value**
- `void`

**Inner Mechanisms**
- Sends an asynchronous request to `url` using `asr_send(url)`.
- Stores the `range` in `content_buffer` for later paste/swap operations.

**Usage Context**
- Used in conjunction with `content_edit_paste()` or `content_edit_swap()`.

**Example**
```javascript
// Copy a text block to the clipboard
content_edit_copy("/copy/block/789", "block_789_content");
```

---

#### `content_edit_paste(url)`
Pastes the content from the clipboard buffer.

**Parameters**

| Name  | Type     | Description                                      |
|-------|----------|--------------------------------------------------|
| `url` | `string` | URL to execute the paste command.                |

**Return Value**
- `void`

**Inner Mechanisms**
- Delegates to `content_edit_command()` with a localized confirmation message (`CMS_L_COMMAND_PASTE`).

**Usage Context**
- Used after `content_edit_copy()` to insert copied content.

**Example**
```javascript
// Paste the copied content into a new location
content_edit_paste("/paste/block/101");
```

---

#### `content_edit_swap(url)`
Swaps the current content with the content in the clipboard buffer.

**Parameters**

| Name  | Type     | Description                                      |
|-------|----------|--------------------------------------------------|
| `url` | `string` | URL to execute the swap command.                 |

**Return Value**
- `void`

**Inner Mechanisms**
- Checks if `content_buffer` is non-empty.
- If empty, shows an alert (`CMS_L_MOD_CONTENT_002`).
- If not empty, delegates to `content_edit_command()` with a localized confirmation message (`CMS_L_MOD_CONTENT_005`).

**Usage Context**
- Used to exchange content between two locations.

**Example**
```javascript
// Swap the current block with the copied content
content_edit_swap("/swap/block/202");
```

---

#### `content_edit_kick1(url)`
Moves content forward by a user-specified number of positions.

**Parameters**

| Name  | Type     | Description                                      |
|-------|----------|--------------------------------------------------|
| `url` | `string` | URL to execute the move command (contains `%return%` placeholder). |

**Return Value**
- `void`

**Inner Mechanisms**
- Prompts the user for a positive integer (`CMS_L_MOD_CONTENT_011`).
- Validates the input using a regex (`/^[0-9]+$/`).
- Replaces `%return%` in the URL with the user-provided value.
- Delegates to `content_edit_command()`.

**Usage Context**
- Used to reorder content elements (e.g., moving a block up in a list).

**Example**
```javascript
// Move the current block forward by 2 positions
content_edit_kick1("/move/block/303/forward");
```

---

#### `content_edit_kick2(url)`
Moves content backward by a user-specified number of positions.

**Parameters**

| Name  | Type     | Description                                      |
|-------|----------|--------------------------------------------------|
| `url` | `string` | URL to execute the move command (contains `%return%` placeholder). |

**Return Value**
- `void`

**Inner Mechanisms**
- Prompts the user for a positive integer (`CMS_L_MOD_CONTENT_012`).
- Validates the input using a regex (`/^[0-9]+$/`).
- Replaces `%return%` in the URL with the negated user-provided value.
- Delegates to `content_edit_command()`.

**Usage Context**
- Used to reorder content elements (e.g., moving a block down in a list).

**Example**
```javascript
// Move the current block backward by 1 position
content_edit_kick2("/move/block/404/backward");
```

---

#### `content_edit_clear(url)`
Deletes the current content after confirmation.

**Parameters**

| Name  | Type     | Description                                      |
|-------|----------|--------------------------------------------------|
| `url` | `string` | URL to execute the delete command.               |

**Return Value**
- `void`

**Inner Mechanisms**
- Delegates to `content_edit_command()` with a localized confirmation message (`CMS_L_COMMAND_DELETE`).

**Usage Context**
- Used to remove content elements.

**Example**
```javascript
// Delete the current block
content_edit_clear("/delete/block/505");
```

---

#### `content_edit_repeat(url, value)`
Repeats the current content a user-specified number of times.

**Parameters**

| Name    | Type      | Description                                      |
|---------|-----------|--------------------------------------------------|
| `url`   | `string`  | URL to execute the repeat command (contains `%return%` placeholder). |
| `value` | `string`  | Default value for the repeat count.              |

**Return Value**
- `void`

**Inner Mechanisms**
- Prompts the user for a positive integer (`CMS_L_MOD_CONTENT_009`).
- Validates the input using a regex (`/^[0-9]+$/`).
- Replaces `%return%` in the URL with the user-provided value.
- Delegates to `content_edit_command()`.

**Usage Context**
- Used to duplicate content (e.g., repeating a block for testing).

**Example**
```javascript
// Repeat the current block 3 times
content_edit_repeat("/repeat/block/606", "3");
```

---

#### `content_edit_shift(url, value)`
Shifts the current content by a user-specified offset.

**Parameters**

| Name    | Type      | Description                                      |
|---------|-----------|--------------------------------------------------|
| `url`   | `string`  | URL to execute the shift command (contains `%return%` placeholder). |
| `value` | `string`  | Default value for the shift offset.              |

**Return Value**
- `void`

**Inner Mechanisms**
- Prompts the user for an integer (`CMS_L_MOD_CONTENT_010`).
- Validates the input using a regex (`/^-?[0-9]+$/`).
- Replaces `%return%` in the URL with the user-provided value.
- Delegates to `content_edit_command()`.

**Usage Context**
- Used to adjust content positioning (e.g., shifting a block's display order).

**Example**
```javascript
// Shift the current block by -2 positions
content_edit_shift("/shift/block/707", "-2");
```

---

#### `content_edit_switch(url, value)`
Toggles a binary state (e.g., visibility) for the current content.

**Parameters**

| Name    | Type      | Description                                      |
|---------|-----------|--------------------------------------------------|
| `url`   | `string`  | URL to execute the toggle command (contains `%return%` placeholder). |
| `value` | `string`  | Current state (`""` or `"1"`).                   |

**Return Value**
- `void`

**Inner Mechanisms**
- Toggles `value` between `""` and `"1"`.
- Replaces `%return%` in the URL with the new value.
- Delegates to `content_edit_command()`.

**Usage Context**
- Used to enable/disable content (e.g., toggling a block's visibility).

**Example**
```javascript
// Toggle the visibility of the current block
content_edit_switch("/toggle/block/808", "1");
```

---

#### `content_edit_apply(url)`
Applies pending changes to the current content.

**Parameters**

| Name  | Type     | Description                                      |
|-------|----------|--------------------------------------------------|
| `url` | `string` | URL to execute the apply command.                |

**Return Value**
- `void`

**Inner Mechanisms**
- Delegates to `content_edit_command()` with a localized confirmation message (`CMS_L_MOD_CONTENT_003`).

**Usage Context**
- Used to save edits made to content.

**Example**
```javascript
// Apply changes to the current block
content_edit_apply("/apply/block/909");
```

---

#### `content_edit_revert(url)`
Reverts pending changes to the current content.

**Parameters**

| Name  | Type     | Description                                      |
|-------|----------|--------------------------------------------------|
| `url` | `string` | URL to execute the revert command.               |

**Return Value**
- `void`

**Inner Mechanisms**
- Delegates to `content_edit_command()` with a localized confirmation message (`CMS_L_MOD_CONTENT_004`).

**Usage Context**
- Used to discard unsaved edits.

**Example**
```javascript
// Revert changes to the current block
content_edit_revert("/revert/block/1010");
```

---

#### `content_load(url)`
Loads a new page in the parent context.

**Parameters**

| Name  | Type     | Description                                      |
|-------|----------|--------------------------------------------------|
| `url` | `string` | URL to load in the parent frame.                 |

**Return Value**
- `void`

**Inner Mechanisms**
- Uses `parent.location.replace(url)` to navigate the parent frame.

**Usage Context**
- Used to exit the editing context or load a new page after an operation.

**Example**
```javascript
// Return to the content overview page
content_load("/content/overview");
```


<!-- HASH:523f2619721ab462282e1810c889aff3 -->
