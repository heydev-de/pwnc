# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.token.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.token.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Token Interface Module (`ifc.token.inc`)

This file implements the **Token Management Interface** for the PWNC Web Platform. It provides a user interface for creating, editing, deleting, and organizing **tokens**—dynamic placeholders that can be inserted into content (e.g., `%%token_name%%`). Tokens are stored in the database and can hold static values or dynamic content (HTML, text, etc.), making them reusable across the platform.

The interface supports:
- **Token CRUD operations** (Create, Read, Update, Delete)
- **Categorization** of tokens for better organization
- **Real-time preview** of token content
- **Bulk operations** (e.g., mass deletion)
- **Category renaming**
- **Permission-based access control** (operators only)

---

### **Constants & Dependencies**
| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_DB_TOKEN` | Database table name | Stores all token data. |
| `CMS_DB_TOKEN_INDEX` | Column name | Unique identifier for the token (e.g., `my_token`). |
| `CMS_DB_TOKEN_VALUE` | Column name | Optional value appended to the token (e.g., `param1,param2`). |
| `CMS_DB_TOKEN_TITLE` | Column name | Human-readable name for the token. |
| `CMS_DB_TOKEN_TEXT` | Column name | Content (HTML/text) associated with the token. |
| `CMS_DB_TOKEN_CATEGORY` | Column name | Category under which the token is grouped. |
| `CMS_TOKEN_PERMISSION_OPERATOR` | Permission key | Required permission to manage tokens. |
| `CMS_L_*` | Localized strings | UI labels (e.g., `CMS_L_COMMAND_ADD` = "Add"). |

**Dependencies:**
- `token` class (loaded via `cms_load("token")`)
- `ifc` class (PWNC’s interface framework)
- Database access via `mysql_*` wrappers

---

### **Core Workflow**
1. **Initialization**
   - Loads the `token` class and checks user permissions.
   - Restores the last selected token/category from cache or URL parameters.

2. **Message Handling**
   - Processes interface actions (`select`, `display`, `add`, `edit`, `delete`, `category_rename`).
   - Delegates operations to the `token` class or direct SQL queries.

3. **Main Display**
   - Renders a two-pane interface:
     - **Left pane**: Category selector + token list (with checkboxes for bulk actions).
     - **Right pane**: Live preview of the selected token’s content in an iframe.

---

### **Helper Functions**

#### `token_get_index(token $token, string $category): string|false`
**Purpose:**
Retrieves the first token index (name) in a given category. Used to default-select a token when switching categories.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$token` | `token` | Instance of the `token` class. |
| `$category` | `string` | Category name. Empty string for uncategorized tokens. |

**Return Value:**
- `string`: Token index (e.g., `my_token`).
- `false`: If no tokens exist in the category.

**Inner Mechanisms:**
- Executes a SQL query to fetch the first token in the category, ordered by index.
- Returns `false` if the query fails or no results are found.

**Usage Example:**
```php
$first_token = token_get_index($token, "newsletters");
// Returns e.g., "header_logo" if it's the first token in the "newsletters" category.
```

---

#### `token_get_category(token $token, string $object): string|false`
**Purpose:**
Fetches the category of a given token.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$token` | `token` | Instance of the `token` class. |
| `$object` | `string` | Token index (e.g., `my_token`). |

**Return Value:**
- `string`: Category name.
- `false`: If the token does not exist.

**Inner Mechanisms:**
- Queries the database for the token’s category.
- Returns `false` if the token is not found.

**Usage Example:**
```php
$category = token_get_category($token, "footer_links");
// Returns e.g., "navigation" if the token belongs to that category.
```

---

#### `token_get_select(token $token): string`
**Purpose:**
Generates an HTML `<select>` element for category selection, pre-populated with all existing categories.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$token` | `token` | Instance of the `token` class. |

**Return Value:**
- `string`: HTML `<select>` element with categories as `<option>`s.

**Inner Mechanisms:**
- Fetches distinct categories from the database.
- Escapes output with `x()` for XSS protection.
- Uses `strabridge()` to truncate long category names.

**Usage Example:**
```php
echo token_get_select($token);
// Renders:
// <select name="category">
//   <option value="navigation">Navigation</option>
//   <option value="newsletters">Newsletters</option>
// </select>
```

---

### **Interface Actions**
The module processes actions via `CMS_IFC_MESSAGE`. Below are the key actions:

#### **`select`**
**Purpose:**
Updates the selected token when a category is chosen from the dropdown.

**Parameters:**
- `$ifc_param`: Category name (from the `<select>` element).

**Mechanism:**
- Calls `token_get_index()` to fetch the first token in the category.
- Updates the cached `object` (token index) for the user.

---

#### **`display`**
**Purpose:**
Renders the content of a token in the preview iframe.

**Parameters:**
- `$object`: Token index (from URL or cache).

**Mechanism:**
1. Caches the selected token for the user.
2. Fetches the token’s data via `$token->get($object)`.
3. Formats the token placeholder (e.g., `%%token_name%%` or `%%token_name param1,param2%%`).
4. Applies the token (replaces it with its content) and displays it in the iframe.

**Example Output:**
```html
<iframe src=".../display?object=header_logo">
  <!-- Renders the HTML content of the "header_logo" token -->
</iframe>
```

---

#### **`add` / `edit`**
**Purpose:**
Displays a form to add or edit a token.

**Parameters (for `edit`):**
- `$object`: Token index to edit.

**Form Fields:**
| Field | Label | Type | Description |
|-------|-------|------|-------------|
| `ifc_param1` | Name | `text` | Human-readable title (e.g., "Header Logo"). |
| `ifc_param2` | Index | `text` | Token identifier (e.g., `header_logo`). Auto-sanitized to replace spaces with underscores. |
| `ifc_param3` | Value | `text` | Comma-separated parameters (e.g., `width=100,height=50`). |
| `ifc_param4` | Text | `code_html` | HTML/text content of the token. |
| `ifc_param5` | Category | `list` | Dropdown of existing categories. |

**JavaScript Helpers:**
- `token_index()`: Sanitizes the token index (replaces spaces with underscores).
- `token_value()`: Sanitizes the value field (removes invalid characters) and generates a placeholder list.
- `token_insert()`: Inserts a placeholder (e.g., `[ %token_name% ]`) into the text field.

**Example Form Submission:**
```javascript
// After filling the form:
ifc_post("_add", {
  ifc_param1: "Footer Copyright",
  ifc_param2: "footer_copyright",
  ifc_param3: "year=2026",
  ifc_param4: "<p>© {year} My Company</p>",
  ifc_param5: "footer"
});
```

---

#### **`_add` / `_edit`**
**Purpose:**
Processes the form submission to create or update a token.

**Parameters:**
| Parameter | Source | Description |
|-----------|--------|-------------|
| `$ifc_param1` | Form | Token title. |
| `$ifc_param2` | Form | Token index. |
| `$ifc_param3` | Form | Token value (parameters). |
| `$ifc_param4` | Form | Token content (HTML/text). |
| `$ifc_param5` | Form | Token category. |
| `$object` | URL/Cache | For `_edit`: Current token index. |

**Mechanism:**
- Calls `$token->add()` or `$token->update()`.
- Updates the response status (`CMS_MSG_DONE` or `CMS_MSG_ERROR`).

**Example:**
```php
// Adding a token:
$token->add(
  "footer_copyright",  // index
  "year=2026",         // value
  "footer",            // category
  "Footer Copyright",  // title
  "<p>© {year} My Company</p>"  // text
);
```

---

#### **`delete`**
**Purpose:**
Deletes one or more tokens selected via checkboxes.

**Parameters:**
- `$_object`: Array of token indices to delete.

**Mechanism:**
- Iterates over `$_object` and calls `$token->delete()` for each token.
- Updates the response status based on success/failure.

**Example:**
```php
// Deleting tokens "old_header" and "old_footer":
$token->delete("old_header");
$token->delete("old_footer");
```

---

#### **`category_rename` / `_category_rename`**
**Purpose:**
Renames a category and updates all tokens under it.

**Parameters:**
- `$object`: Token index (used to fetch the current category).
- `$ifc_param1`: New category name (from form).

**Mechanism:**
- For `category_rename`: Displays a form with the current category name.
- For `_category_rename`: Executes a SQL `UPDATE` to rename the category.

**Example SQL:**
```sql
UPDATE cms_token
SET category = 'new_category_name'
WHERE category = 'old_category_name';
```

---

### **Main Display Logic**
#### **Category & Token Selection**
1. **Category Handling:**
   - If `$object` (token index) is provided, fetches its category.
   - If no `$object` is provided, restores the last selected category from cache.
   - Defaults to uncategorized tokens if the category is empty.

2. **Token List:**
   - Fetches all tokens in the selected category.
   - Renders them as checkboxes in a scrollable list.
   - Supports bulk actions (select all/invert/none).

3. **Token Preview:**
   - Displays the selected token’s content in an iframe.
   - Updates dynamically via JavaScript when a token is selected.

**JavaScript Helper:**
- `token_select(value)`: Updates the iframe URL to display the selected token.

---

### **Usage Example**
#### **Scenario: Adding a Token for a Reusable Banner**
1. **Navigate to the Token Interface:**
   - Access `/ifc/token` (or via the PWNC admin menu).

2. **Fill the Add Form:**
   - **Name:** "Promo Banner"
   - **Index:** `promo_banner`
   - **Value:** `color=blue,link=/sale`
   - **Text:**
     ```html
     <div style="background:{color}; padding:10px;">
       <a href="{link}">Summer Sale!</a>
     </div>
     ```
   - **Category:** "banners"

3. **Submit the Form:**
   - The token is saved to the database.
   - The preview iframe updates to show the rendered banner.

4. **Use the Token in Content:**
   - Insert `%%promo_banner%%` or `%%promo_banner color=red%%` into any HTML/text field.
   - The token will be replaced with the banner HTML at runtime.

---

### **Security Considerations**
1. **Permissions:**
   - Only users with `CMS_TOKEN_PERMISSION_OPERATOR` can access the interface.
   - The `token` class enforces additional checks (e.g., `$token->enabled`).

2. **Input Sanitization:**
   - All user inputs are escaped with `sqlesc()` (SQL) and `x()` (HTML).
   - Token indices are sanitized to remove spaces and special characters.

3. **CSRF Protection:**
   - The `ifc` class handles CSRF tokens automatically.

4. **Iframe Sandboxing:**
   - The preview iframe uses `sandbox="allow-same-origin"` to restrict interactions.

---

### **Error Handling**
- **Missing Token:** Displays an error if a token is not found during edit/delete.
- **Database Errors:** Returns `CMS_MSG_ERROR` if SQL queries fail.
- **Empty Inputs:** Skips operations if required fields (e.g., `$object`) are empty.


<!-- HASH:d477117068f7c5b590b781d58bbdf1c2 -->
