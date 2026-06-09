# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.document.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.document.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Document Class

The `document` class is a core component of the PWNC Web Platform for managing structured document data. It provides functionality to import, manipulate, and export hierarchical document structures with support for references, templates, and type-safe operations. Documents are stored as key-value pairs with associated types and can reference external content via the `content_pool` system.

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_DOCUMENT_SEPARATOR` | `"\x1E"` | ASCII record separator used to delimit document entries during import/export. |
| `CMS_DOCUMENT_TYPE` | `"0"` | Array index for storing the type of a document entry. |
| `CMS_DOCUMENT_VALUE` | `"1"` | Array index for storing the value of a document entry. |
| `CMS_DOCUMENT_REFERENCE` | `"2"` | Array index for storing the reference identifier of a document entry. |

### Properties

| Name | Default | Description |
|------|---------|-------------|
| `data` | `[]` | Associative array storing document entries as `[id => [CMS_DOCUMENT_TYPE, CMS_DOCUMENT_VALUE, CMS_DOCUMENT_REFERENCE]]`. |
| `default` | `[]` | Associative array storing default values for document entries, following the same structure as `data`. |
| `template_index` | `NULL` | Identifier of the template used to structure the document. |
| `structure` | `NULL` | Parsed template structure defining the document's hierarchy and element types. |

---

### `__construct($text = NULL, $template_index = NULL)`

**Purpose:**
Initializes a new document instance, optionally importing data and setting a template structure.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Serialized document data to import. If `NULL`, an empty document is created. |
| `$template_index` | `string` | Template identifier to structure the document. If `NULL`, no structure is applied. |

**Return Values:**
- None (constructor).

**Inner Mechanisms:**
- Calls `import($text)` if `$text` is provided.
- Calls `set_structure($template_index)` if `$template_index` is provided.

**Usage Context:**
- Used to create a new document from serialized data or an empty document with a predefined structure.

**Example:**
```php
// Create an empty document with a template
$doc = new document(NULL, "article_template");

// Create a document from serialized data
$serialized_data = "title:text:Hello World\x1Econtent:html:<p>Content</p>";
$doc = new document($serialized_data, "article_template");
```

---

### `import($text)`

**Purpose:**
Parses serialized document data and populates the `data` property.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Serialized document data using `CMS_DOCUMENT_SEPARATOR` as delimiter. |

**Return Values:**
- None.

**Inner Mechanisms:**
- Splits the input string by `CMS_DOCUMENT_SEPARATOR`.
- Parses each entry using regex to extract `id`, `type`, and `value`.
- If the type is `#reference`, resolves the reference using `resolve_reference($id)`.
- Unmatched entries are stored with `NULL` type.

**Usage Context:**
- Used to load document data from storage or external sources.

**Example:**
```php
$serialized_data = "title:text:Hello World\x1Econtent:html:<p>Content</p>";
$doc = new document();
$doc->import($serialized_data);
echo $doc->get("title"); // Output: "Hello World"
```

---

### `resolve_reference($id)`

**Purpose:**
Resolves a reference entry (`#reference` type) by fetching the referenced content from the `content_pool` and merging it into the document.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$id` | `string` | Document entry identifier to resolve. |

**Return Values:**
- None.

**Inner Mechanisms:**
- Checks if the entry exists and is a reference.
- Loads the `content_pool` library and fetches the referenced content.
- Merges the referenced content into the document, preserving hierarchy and avoiding overwrites unless the existing entry is also a reference.
- Handles relative paths (e.g., `..key`) and nested keys (e.g., `parent.child`).

**Usage Context:**
- Used internally when importing documents with references or when explicitly setting a reference via `set($id, "#reference", $value)`.

**Example:**
```php
$doc = new document();
$doc->set("content", "#reference", "article_123");
$doc->resolve_reference("content"); // Fetches and merges content from content_pool
```

---

### `set_structure($template_index)`

**Purpose:**
Applies a template structure to the document, defining its hierarchy and element types.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$template_index` | `string` | Template identifier to structure the document. |

**Return Values:**
- `TRUE` on success.
- `FALSE` if the template library is unavailable or parsing fails.

**Inner Mechanisms:**
- Loads the `template` library and parses the template structure.
- Stores the parsed structure in the `structure` property and the template identifier in `template_index`.

**Usage Context:**
- Used to enforce a predefined structure on the document, enabling type-safe operations and hierarchical manipulations.

**Example:**
```php
$doc = new document();
$success = $doc->set_structure("article_template");
if ($success) {
    echo "Document structured with article_template";
}
```

---

### `update_structure()`

**Purpose:**
Reapplies the current template structure to the document.

**Parameters:**
- None.

**Return Values:**
- `TRUE` on success.
- `FALSE` if no template is set.

**Inner Mechanisms:**
- Calls `set_structure($this->template_index)` if a template is set.

**Usage Context:**
- Used to refresh the document structure after modifications that may affect it (e.g., `inject`, `del`).

**Example:**
```php
$doc = new document(NULL, "article_template");
$doc->set("title", "text", "New Title");
$doc->update_structure(); // Reapplies the template structure
```

---

### `export($resolve_references = TRUE, $cleanup = TRUE)`

**Purpose:**
Serializes the document data into a string for storage or transmission.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$resolve_references` | `bool` | `TRUE` | If `TRUE`, exports resolved values; if `FALSE`, exports unresolved references. |
| `$cleanup` | `bool` | `TRUE` | If `TRUE`, removes entries not defined in the template structure or with mismatched types. |

**Return Values:**
- `string`: Serialized document data.

**Inner Mechanisms:**
- Iterates over `data` and constructs a string using `CMS_DOCUMENT_SEPARATOR`.
- Skips entries not listed in the structure or with mismatched types if `$cleanup` is `TRUE`.
- Exports references as `#reference:type:value` if `$resolve_references` is `FALSE`.

**Usage Context:**
- Used to save document data to storage or transmit it to other systems.

**Example:**
```php
$doc = new document();
$doc->set("title", "text", "Hello World");
$serialized = $doc->export();
echo $serialized; // Output: "title:text:Hello World"
```

---

### `get($id, $type = NULL, $use_default = TRUE)`

**Purpose:**
Retrieves the value of a document entry, optionally validating its type and falling back to default values.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$id` | `string` | | Entry identifier. |
| `$type` | `string` | `NULL` | Expected type of the entry. If `NULL`, type is not validated. |
| `$use_default` | `bool` | `TRUE` | If `TRUE`, falls back to the default value if the entry is not found or empty. |

**Return Values:**
- `string`: The entry value if found and valid.
- `NULL`: If the entry is not found, empty, or has an invalid type.

**Inner Mechanisms:**
- Checks if the entry exists, is non-empty, and matches the expected type (if provided).
- Falls back to the default value if `$use_default` is `TRUE` and the entry is invalid.

**Usage Context:**
- Used to safely retrieve document data with type validation.

**Example:**
```php
$doc = new document();
$doc->set("title", "text", "Hello World");
$doc->set_default("title", "text", "Default Title");
echo $doc->get("title", "text"); // Output: "Hello World"
echo $doc->get("nonexistent", "text"); // Output: "Default Title"
```

---

### `get_reference($id, $type = NULL)`

**Purpose:**
Retrieves the reference identifier of a document entry.

**Parameters:**

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$id` | `string` | | Entry identifier. |
| `$type` | `string` | `NULL` | Expected type of the entry. If `NULL`, type is not validated. |

**Return Values:**
- `string`: The reference identifier if the entry is a reference and valid.
- `NULL`: If the entry is not a reference or invalid.

**Inner Mechanisms:**
- Checks if the entry exists and matches the expected type (if provided).
- Returns the reference identifier stored in `CMS_DOCUMENT_REFERENCE`.

**Usage Context:**
- Used to inspect reference entries without resolving them.

**Example:**
```php
$doc = new document();
$doc->set("content", "#reference", "article_123");
echo $doc->get_reference("content"); // Output: "article_123"
```

---

### `get_parent_template($path)`

**Purpose:**
Retrieves the path of the nearest parent template of a given element in the document structure.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$path` | `string` | Path of the element to inspect. |

**Return Values:**
- `string`: Path of the parent template if found.
- `NULL`: If no parent template exists.
- `FALSE`: If no structure is set.

**Inner Mechanisms:**
- Traverses the structure to find the element with the given path.
- Walks up the hierarchy to find the nearest parent with type `template`.

**Usage Context:**
- Used to determine the template context of an element for hierarchical operations.

**Example:**
```php
$doc = new document(NULL, "article_template");
$parent_template = $doc->get_parent_template("article.body");
echo $parent_template; // Output: "article" (or similar, depending on the template)
```

---

### `set($id, $type, $value)`

**Purpose:**
Sets the value and type of a document entry, optionally resolving references.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$id` | `string` | Entry identifier. |
| `$type` | `string` | Type of the entry. |
| `$value` | `string` | Value of the entry. |

**Return Values:**
- None.

**Inner Mechanisms:**
- Stores the entry in `data` with the given type and value.
- If the type is `#reference`, resolves the reference using `resolve_reference($id)`.

**Usage Context:**
- Used to add or update document entries.

**Example:**
```php
$doc = new document();
$doc->set("title", "text", "Hello World");
$doc->set("content", "#reference", "article_123");
```

---

### `set_default($id, $type, $value)`

**Purpose:**
Sets the default value and type of a document entry.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$id` | `string` | Entry identifier. |
| `$type` | `string` | Type of the entry. |
| `$value` | `string` | Default value of the entry. |

**Return Values:**
- None.

**Inner Mechanisms:**
- Stores the default value in the `default` property.

**Usage Context:**
- Used to define fallback values for document entries.

**Example:**
```php
$doc = new document();
$doc->set_default("title", "text", "Default Title");
echo $doc->get("title"); // Output: "Default Title" (if no value is set)
```

---

### `extract($path)`

**Purpose:**
Extracts a subdocument from the current document, preserving the hierarchy relative to the given path.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$path` | `string` | Path of the element to extract. |

**Return Values:**
- `document`: A new document instance containing the extracted data.
- `FALSE`: If no structure is set.

**Inner Mechanisms:**
- Creates a new `document` instance.
- Finds the element with the given path in the structure.
- Copies the element and its children into the new document, adjusting keys to be relative to the extracted path.

**Usage Context:**
- Used to isolate a portion of the document for manipulation or injection elsewhere.

**Example:**
```php
$doc = new document(NULL, "article_template");
$doc->set("article.title", "text", "Hello World");
$subdoc = $doc->extract("article");
echo $subdoc->get("title"); // Output: "Hello World"
```

---

### `inject($path, $document)`

**Purpose:**
Injects a subdocument into the current document at the given path, merging hierarchies and resolving conflicts.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$path` | `string` | Path to inject the subdocument. |
| `$document` | `document` | Subdocument to inject. |

**Return Values:**
- None.

**Inner Mechanisms:**
- Finds the target element in the structure.
- Merges the subdocument data into the current document, adjusting keys to match the target path.
- Recomputes the structure and resolves conflicts by fitting unassigned elements into compatible positions.

**Usage Context:**
- Used to merge documents or insert subdocuments into a specific position.

**Example:**
```php
$doc = new document(NULL, "article_template");
$subdoc = new document();
$subdoc->set("title", "text", "Injected Title");
$doc->inject("article", $subdoc);
echo $doc->get("article.title"); // Output: "Injected Title"
```

---

### `copy($path_source, $path_target)`

**Purpose:**
Copies an element and its children from one path to another within the document.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$path_source` | `string` | Path of the element to copy. |
| `$path_target` | `string` | Path to copy the element to. |

**Return Values:**
- None.

**Inner Mechanisms:**
- Extracts the source element using `extract($path_source)`.
- Injects the extracted data into the target path using `inject($path_target, $subdoc)`.

**Usage Context:**
- Used to duplicate document sections.

**Example:**
```php
$doc = new document(NULL, "article_template");
$doc->set("article.title", "text", "Hello World");
$doc->copy("article.title", "article.subtitle");
echo $doc->get("article.subtitle"); // Output: "Hello World"
```

---

### `swap($path_source, $path_target)`

**Purpose:**
Swaps two elements and their children within the document.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$path_source` | `string` | Path of the first element. |
| `$path_target` | `string` | Path of the second element. |

**Return Values:**
- None.

**Inner Mechanisms:**
- Extracts both elements using `extract()`.
- Injects the source into the target and vice versa.

**Usage Context:**
- Used to reorder document sections.

**Example:**
```php
$doc = new document(NULL, "article_template");
$doc->set("article.title", "text", "Title");
$doc->set("article.subtitle", "text", "Subtitle");
$doc->swap("article.title", "article.subtitle");
echo $doc->get("article.title"); // Output: "Subtitle"
```

---

### `kick($path, $value)`

**Purpose:**
Moves an element and its children forward or backward by `$value` positions among compatible elements (same type and level).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$path` | `string` | Path of the element to move. |
| `$value` | `int` | Number of positions to move (positive for forward, negative for backward). |

**Return Values:**
- None.

**Inner Mechanisms:**
- Finds the element and its relevant parent (nearest `template` or `group`).
- Determines the element's level in the hierarchy.
- Moves the element to the nth compatible position in the specified direction.
- Reconstructs the document with the new order.

**Usage Context:**
- Used to reorder elements within a group or template.

**Example:**
```php
$doc = new document(NULL, "article_template");
$doc->set("article.section1", "group", "");
$doc->set("article.section1.item1", "text", "Item 1");
$doc->set("article.section1.item2", "text", "Item 2");
$doc->kick("article.section1.item1", 1); // Moves "Item 1" after "Item 2"
```

---

### `drop($path, $value)`

**Purpose:**
Removes an element and reinserts it at a new position determined by `$value` among compatible elements.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$path` | `string` | Path of the element to move. |
| `$value` | `int` | Direction and magnitude of movement (positive for forward, negative for backward). |

**Return Values:**
- None.

**Inner Mechanisms:**
- Removes the element from its current position.
- Finds the next compatible position in the specified direction and reinserts the element.

**Usage Context:**
- Used to reorder elements while maintaining compatibility with the template structure.

**Example:**
```php
$doc = new document(NULL, "article_template");
$doc->set("article.section1", "group", "");
$doc->set("article.section1.item1", "text", "Item 1");
$doc->set("article.section1.item2", "text", "Item 2");
$doc->drop("article.section1.item1", 1); // Moves "Item 1" after "Item 2"
```

---

### `shift($id, $value)`

**Purpose:**
Moves all first-level children of a `shift` element forward or backward by `$value` positions among compatible elements.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$id` | `string` | Identifier of the `shift` element. |
| `$value` | `int` | Number of positions to move (positive for forward, negative for backward). |

**Return Values:**
- None.

**Inner Mechanisms:**
- Finds the `shift` element and its first-level children.
- Moves each child to the nth compatible position in the specified direction.

**Usage Context:**
- Used to reorder dynamic lists or collections within a `shift` container.

**Example:**
```php
$doc = new document(NULL, "article_template");
$doc->set("article.items", "shift", "");
$doc->set("article.items.item1", "text", "Item 1");
$doc->set("article.items.item2", "text", "Item 2");
$doc->shift("article.items", 1); // Moves "Item 1" after "Item 2"
```

---

### `del($path)`

**Purpose:**
Removes an element and its children from the document.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$path` | `string` | Path of the element to remove. |

**Return Values:**
- None.

**Inner Mechanisms:**
- Finds the element in the structure and removes its data.
- If the element is a `template` or `group`, removes all its children as well.

**Usage Context:**
- Used to delete document sections.

**Example:**
```php
$doc = new document(NULL, "article_template");
$doc->set("article.title", "text", "Hello World");
$doc->del("article.title");
echo $doc->get("article.title"); // Output: NULL
```

---

### `cleanup()`

**Purpose:**
Removes document entries that are not defined in the template structure or have mismatched types.

**Parameters:**
- None.

**Return Values:**
- None.

**Inner Mechanisms:**
- Collects all valid entry identifiers and types from the structure.
- Rebuilds the `data` property, retaining only valid entries.

**Usage Context:**
- Used to enforce consistency with the template structure before export or validation.

**Example:**
```php
$doc = new document(NULL, "article_template");
$doc->set("invalid_key", "text", "Invalid Data");
$doc->cleanup();
echo $doc->get("invalid_key"); // Output: NULL
```


<!-- HASH:87d2dfb5758cba08a177b808c867ce9c -->
