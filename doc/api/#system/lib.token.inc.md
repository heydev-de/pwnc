# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.token.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.token.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Token Management System

The `lib.token.inc` file provides a **Token Management System** for the PWNC Web Platform. This system allows developers to define, store, retrieve, and apply dynamic tokens (placeholders) within text content. Tokens are replaced with their corresponding values or formatted text at runtime, enabling dynamic content generation without modifying the original text structure.

Tokens follow the syntax `%%token_name param1,param2%%` and can be overridden or processed with user-provided values. This system is particularly useful for:
- Multilingual content management
- Dynamic configuration messages
- Templating with reusable text snippets
- User-customizable notifications or alerts

---

## Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_TOKEN_PERMISSION_OPERATOR` | `"operator"` | Permission identifier required to manage tokens. |
| `CMS_DB_TOKEN` | `CMS_DB_PREFIX . "token"` | Database table name for storing tokens. |
| `CMS_DB_TOKEN_INDEX` | `"id"` | Column name for the token identifier (primary key). |
| `CMS_DB_TOKEN_VALUE` | `"value"` | Column name for the token's allowed value list (comma-separated). |
| `CMS_DB_TOKEN_CATEGORY` | `"category"` | Column name for the token's category (used for grouping). |
| `CMS_DB_TOKEN_TITLE` | `"title"` | Column name for the token's display title (supports multilingual values). |
| `CMS_DB_TOKEN_TEXT` | `"text"` | Column name for the token's replacement text (supports placeholders like `%value%`). |

---

## Standalone Functions

### `token_get_index(&$token, $category)`

**Purpose:**
Retrieves the first token index (ID) from the database that belongs to a given category.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$token` | `object` (by reference) | Instance of the `token` class. Must be enabled. |
| `$category` | `string` | Category name to filter tokens. |

**Return Values:**
- `string` – The token index (ID) if found.
- `FALSE` – If no token exists in the category, the token system is disabled, or a database error occurs.

**Inner Mechanisms:**
- Executes a SQL query to fetch the first token index in the specified category.
- Uses `sqlesc()` for SQL injection protection.
- Relies on `mysql_query()` and `mysql_num_rows()` for result validation.

**Usage Context:**
Used to quickly access a representative token from a category, e.g., for validation or default value retrieval.

**Example:**
```php
$token = new token();
$index = token_get_index($token, "notifications");
if ($index) {
    echo "First notification token: " . $index;
}
```

---

### `token_get_category(&$token, $index)`

**Purpose:**
Retrieves the category of a token given its index.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$token` | `object` (by reference) | Instance of the `token` class. Must be enabled. |
| `$index` | `string` | Token index (ID) to look up. |

**Return Values:**
- `string` – The category name if the token exists.
- `FALSE` – If the token does not exist, the system is disabled, or an error occurs.

**Inner Mechanisms:**
- Validates that `$index` is not empty using `nstre()`.
- Executes a SQL query to fetch the category of the specified token.

**Usage Context:**
Useful for categorizing or filtering tokens dynamically.

**Example:**
```php
$token = new token();
$category = token_get_category($token, "welcome_message");
if ($category) {
    echo "Token belongs to category: " . $category;
}
```

---

### `token_get_select(&$token)`

**Purpose:**
Generates an associative array of all distinct, non-empty token categories for use in HTML `<select>` elements.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$token` | `object` (by reference) | Instance of the `token` class. Must be enabled. |

**Return Values:**
- `array` – Associative array in the form `["" => "", "category1" => "category1", ...]`.
- `FALSE` – If the token system is disabled or no categories exist.

**Inner Mechanisms:**
- Uses `DISTINCT` in SQL to fetch unique categories.
- Builds an array suitable for form dropdowns, with an empty default option.

**Usage Context:**
Used in admin interfaces to allow users to filter or assign tokens by category.

**Example:**
```php
$token = new token();
$categories = token_get_select($token);
if ($categories) {
    echo '<select name="category">';
    foreach ($categories as $value => $label) {
        echo '<option value="' . x($value) . '">' . x($label) . '</option>';
    }
    echo '</select>';
}
```

---

### `token_override($index, $text = NULL)`

**Purpose:**
Temporarily overrides the replacement text of a token for the current request.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Token index to override. |
| `$text` | `string|null` | New replacement text. If `NULL`, removes the override. |

**Return Values:**
- None (void).

**Inner Mechanisms:**
- Modifies the static property `token::$override`.
- Overrides are stored in memory and do not persist across requests.

**Usage Context:**
Useful for testing, debugging, or context-specific token behavior without modifying the database.

**Example:**
```php
token_override("maintenance_notice", "System is down for maintenance until 5 PM.");
$token = new token();
echo $token->apply("The site is currently: %%maintenance_notice%%");
// Output: "The site is currently: System is down for maintenance until 5 PM."
```

---

## Class: `token`

### Properties

| Name | Type | Description |
|------|------|-------------|
| `operator` | `bool` | Indicates whether the current user has operator permissions. |
| `enabled` | `bool` | Indicates whether the token system is enabled and the database table exists. |
| `$override` | `static array` | Stores temporary token text overrides. |

---

### `__construct()`

**Purpose:**
Initializes the token system and verifies the database table.

**Parameters:**
- None.

**Return Values:**
- None (constructor).

**Inner Mechanisms:**
- Creates a `mysql` instance.
- Uses `verify_table()` to ensure the token table exists with the correct schema.
- Sets `operator` based on `cms_permission(CMS_TOKEN_PERMISSION_OPERATOR)`.
- Sets `enabled` to `TRUE` only if the table is valid.

**Usage Context:**
Instantiate the class to begin using the token system.

**Example:**
```php
$token = new token();
if ($token->enabled) {
    echo "Token system is ready.";
}
```

---

### `add($index, $value, $category, $title, $text)`

**Purpose:**
Adds a new token to the database.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Token identifier (e.g., `welcome_message`). If empty, derived from `$title`. |
| `$value` | `string` | Comma-separated list of allowed placeholder values (e.g., `name,date`). |
| `$category` | `string` | Category for grouping (e.g., `notifications`). |
| `$title` | `string` | Human-readable title. If empty, defaults to `$index`. Supports multilingual keys. |
| `$text` | `string` | Replacement text with placeholders (e.g., `Hello %name%, today is %date%`). |

**Return Values:**
- `string` – The normalized token index on success.
- `FALSE` – If the system is disabled, user lacks permissions, or database insertion fails.

**Inner Mechanisms:**
- Validates system and permission state.
- Normalizes `$index` and `$value` using regex to ensure valid format (alphanumeric, underscores, commas).
- Uses `language_get()` and `language_set()` for multilingual title support.
- Escapes all inputs with `sqlesc()` before SQL insertion.

**Usage Context:**
Used in admin interfaces to create new tokens.

**Example:**
```php
$token = new token();
$result = $token->add(
    "greeting",
    "user,time",
    "messages",
    "Welcome Message",
    "Welcome back, %user%! It's %time%."
);
if ($result) {
    echo "Token created: " . $result;
}
```

---

### `get($index)`

**Purpose:**
Retrieves all data for a specific token.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Token index to retrieve. |

**Return Values:**
- `array` – Associative array with keys: `id`, `value`, `category`, `title`, `text`.
- `FALSE` – If the token does not exist or the system is disabled.

**Inner Mechanisms:**
- Executes a SQL query to fetch all fields for the given token index.
- Returns the first matching row as an associative array.

**Usage Context:**
Used to inspect or edit token properties.

**Example:**
```php
$token = new token();
$data = $token->get("greeting");
if ($data) {
    echo "Title: " . $data['title'] . "<br>";
    echo "Text: " . $data['text'];
}
```

---

### `update($index, $_index, $value, $category, $title, $text)`

**Purpose:**
Updates an existing token.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Current token index (used for lookup). |
| `$_index` | `string` | New token index (if empty, derived from `$title` or kept as `$index`). |
| `$value` | `string` | Updated value list. |
| `$category` | `string` | Updated category. |
| `$title` | `string` | Updated title. |
| `$text` | `string` | Updated replacement text. |

**Return Values:**
- `string` – The new token index on success.
- `FALSE` – If update fails, system is disabled, or user lacks permissions.

**Inner Mechanisms:**
- Similar to `add()`, but performs an `UPDATE` query.
- Uses the current `$index` to locate the token.
- Normalizes and escapes all inputs.

**Usage Context:**
Used in admin interfaces to modify existing tokens.

**Example:**
```php
$token = new token();
$result = $token->update(
    "greeting",
    "welcome_greeting",
    "user,time,location",
    "messages",
    "Welcome Message Updated",
    "Hello %user%! It's %time% in %location%."
);
if ($result) {
    echo "Token updated to: " . $result;
}
```

---

### `delete($index)`

**Purpose:**
Deletes a token from the database.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `string` | Token index to delete. |

**Return Values:**
- `bool` – `TRUE` if deletion was successful.
- `FALSE` – If system is disabled or user lacks permissions.

**Inner Mechanisms:**
- Executes a `DELETE` SQL query with a `LIMIT 1` clause.
- Returns the result of `mysql_query()`.

**Usage Context:**
Used in admin interfaces to remove obsolete tokens.

**Example:**
```php
$token = new token();
if ($token->delete("old_token")) {
    echo "Token deleted successfully.";
}
```

---

### `apply($text)`

**Purpose:**
Processes a text string and replaces all token placeholders with their corresponding values.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Input text containing token placeholders (e.g., `%%greeting user=John%%`). |

**Return Values:**
- `string` – The processed text with tokens replaced.
- `FALSE` – If the system is disabled.

**Inner Mechanisms:**
1. **Token Detection:**
   - Scans for `%%` delimiters, skipping escaped sequences (`\%%`).
   - Extracts token index and user-provided parameters.
2. **Database Lookup:**
   - Fetches all detected tokens in a single query using `IN` clause.
   - Applies overrides from `token::$override` if present.
   - Uses `l()` to localize the replacement text.
3. **Replacement:**
   - For each token, splits user parameters and matches them against the token's value list.
   - Replaces placeholders (`%value%`) with user values or removes conditional blocks if no value is provided.
   - Handles escaped commas in user input (e.g., `value\,with\,commas`).
   - Uses `x()` to escape output for HTML safety.

**Usage Context:**
Used to render dynamic content in templates, emails, or user-facing messages.

**Example:**
```php
$token = new token();
$output = $token->apply("
    %%greeting user=Alice,time=3 PM%%
    %%maintenance_notice%%
    This is a %%status type=warning%% alert.
");
echo $output;
// Output (if tokens exist):
// Hello Alice! It's 3 PM.
// System is operational.
// This is a <strong>warning</strong> alert.
```

**Note:**
- Tokens can be used with or without parameters.
- Conditional blocks: `[prefix%value%suffix]` are removed if the value is empty.
- Supports multilingual text via `l()`.
- Overrides via `token_override()` take precedence.


<!-- HASH:1323fc68463316b7b9f319deabad4359 -->
