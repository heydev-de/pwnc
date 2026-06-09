# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.content_pool.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.content_pool.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Content Pool Module

The `content_pool` module provides a system for managing reusable content fragments extracted from full documents. It allows storing, retrieving, and synchronizing content snippets (e.g., text blocks, HTML fragments) referenced by their original document, range, and type. This enables efficient reuse of content across the platform without duplication.

### Key Features
- **Content Fragmentation**: Extract and store parts of documents (e.g., a specific section or element).
- **Synchronization**: Automatically update fragments when their source documents change.
- **Caching**: Temporary in-memory caching for performance.
- **Permission Control**: Restricts operations to users with `pool.operator` permissions.

---

## Functions

### `content_pool_get_array`
Generates a categorized, nested array of all content pool entries for use in selection interfaces (e.g., dropdowns).

#### Parameters
| Name | Type   | Description                                                                 |
|------|--------|-----------------------------------------------------------------------------|
| type | string | (Optional) Filter entries by type. If `NULL`, all entries are included.     |

#### Return Values
| Type  | Description                                                                 |
|-------|-----------------------------------------------------------------------------|
| array | Nested associative array: `["Category" => ["Name" => "Index", ...], ...]`. |

#### Inner Mechanisms
1. Loads data from `#system/content.pool`.
2. Iterates through entries, skipping those that don’t match the `type` filter.
3. Groups entries by `category`, ensuring unique names by appending `(1)`, `(2)`, etc., if duplicates exist.
4. Sorts categories and names naturally (case-insensitive).

#### Usage Example
```php
$pool_array = content_pool_get_array("html");
/*
Example Output:
[
    "Headers" => [
        "Main Title" => "123",
        "Subtitle"   => "456"
    ],
    "Footers" => [
        "Copyright" => "789"
    ]
]
*/
```
**Use Case**: Populate a dropdown for selecting a content fragment in a CMS form.

---

### `content_pool_get_select`
Generates a flat array of unique categories for use in selection interfaces.

#### Return Values
| Type  | Description                                      |
|-------|--------------------------------------------------|
| array | Associative array: `["Category" => "Category"]`. |

#### Inner Mechanisms
1. Loads data from `#system/content.pool`.
2. Extracts unique `category` values and sorts them naturally.

#### Usage Example
```php
$categories = content_pool_get_select();
/*
Example Output:
[
    ""       => "",
    "Headers" => "Headers",
    "Footers" => "Footers"
]
*/
```
**Use Case**: Populate a category filter dropdown in a content pool management UI.

---

## Class: `content_pool`

Manages the lifecycle of content fragments, including creation, modification, synchronization, and deletion.

### Properties
| Name      | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| data      | object | Instance of `data` class for interacting with `#system/content.pool`.      |
| operator  | bool   | `TRUE` if the current user has `pool.operator` permissions, else `FALSE`.  |

### Constructor
Initializes the content pool system.

#### Inner Mechanisms
1. Loads the `#system/content.pool` data file.
2. Checks user permissions.
3. Creates the `#content/pool` directory if it doesn’t exist.
4. Establishes a MySQL connection.

---

### `add`
Creates a new content fragment entry and synchronizes it with its source document.

#### Parameters
| Name           | Type   | Description                                                                 |
|----------------|--------|-----------------------------------------------------------------------------|
| name           | string | Display name of the fragment. Defaults to `CMS_L_CONTENT_POOL_001` if empty.|
| category       | string | Category for grouping fragments.                                           |
| content_index  | string | Index of the source document in the `content` table.                       |
| range          | string | XPath or selector defining the fragment’s location in the source document. |
| type           | string | Type of content (e.g., `html`, `text`).                                    |

#### Return Values
| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| string  | Index of the new fragment on success.                                      |
| bool    | `FALSE` on failure (e.g., permission denied, invalid input).               |

#### Inner Mechanisms
1. Validates permissions and trims input strings.
2. Inserts a new record into `#system/content.pool`.
3. Calls `synchronize` to extract and store the fragment from the source document.
4. Saves the data file.

#### Usage Example
```php
$pool = new content_pool();
$index = $pool->add(
    "Welcome Message",
    "Headers",
    "homepage",
    "//div[@id='welcome']",
    "html"
);
```
**Use Case**: Add a reusable header fragment from a homepage document.

---

### `set`
Updates properties of an existing content fragment.

#### Parameters
| Name           | Type   | Description                                                                 |
|----------------|--------|-----------------------------------------------------------------------------|
| index          | string | Index of the fragment to update.                                           |
| name           | string | (Optional) New display name.                                                |
| category       | string | (Optional) New category.                                                    |
| content_index  | string | (Optional) New source document index.                                       |
| range          | string | (Optional) New XPath/selector.                                              |
| type           | string | (Optional) New content type.                                                |

#### Return Values
| Type | Description                                                                 |
|------|-----------------------------------------------------------------------------|
| bool | `TRUE` on success, `FALSE` on failure (e.g., permission denied).           |

#### Inner Mechanisms
1. Validates permissions and updates the specified fields.
2. Triggers resynchronization if `content_index`, `range`, or `type` are modified.
3. Saves the data file.

#### Usage Example
```php
$pool = new content_pool();
$pool->set(
    "123",
    "Updated Welcome Message",
    "Banners",
    NULL,
    "//div[@class='banner']"
);
```
**Use Case**: Update the XPath of a fragment to point to a different section of the source document.

---

### `get`
Retrieves all properties of a content fragment.

#### Parameters
| Name  | Type   | Description                     |
|-------|--------|---------------------------------|
| index | string | Index of the fragment to fetch. |

#### Return Values
| Type  | Description                                                                 |
|-------|-----------------------------------------------------------------------------|
| array | Associative array of fragment properties (e.g., `name`, `category`, etc.). |

#### Usage Example
```php
$pool = new content_pool();
$fragment = $pool->get("123");
/*
Example Output:
[
    "name"  => "Welcome Message",
    "category" => "Headers",
    "index" => "homepage",
    "range" => "//div[@id='welcome']",
    "type"  => "html"
]
*/
```
**Use Case**: Display fragment details in a management UI.

---

### `get_text`
Retrieves the actual content of a fragment from cache or file.

#### Parameters
| Name  | Type   | Description                     |
|-------|--------|---------------------------------|
| index | string | Index of the fragment to fetch. |

#### Return Values
| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| string | The fragment’s content (e.g., HTML, text).                                 |
| NULL   | If the fragment doesn’t exist or is empty.                                 |

#### Inner Mechanisms
1. Checks the temporary cache (`cms_cache`) for the fragment.
2. Falls back to reading the file from `#content/pool/{index}` if not cached.
3. Caches the content for future requests.

#### Usage Example
```php
$pool = new content_pool();
$html = $pool->get_text("123");
// Output: "<div id='welcome'>Hello, World!</div>"
```
**Use Case**: Render a fragment in a template or page.

---

### `delete`
Removes a content fragment and its associated file.

#### Parameters
| Name  | Type   | Description                     |
|-------|--------|---------------------------------|
| index | string | Index of the fragment to delete.|

#### Return Values
| Type | Description                                                                 |
|------|-----------------------------------------------------------------------------|
| bool | `TRUE` on success, `FALSE` on failure (e.g., permission denied, file error).|

#### Inner Mechanisms
1. Validates permissions and checks if the fragment exists.
2. Deletes the fragment’s file from `#content/pool/{index}`.
3. Removes the fragment from the data file and cache.

#### Usage Example
```php
$pool = new content_pool();
$pool->delete("123");
```
**Use Case**: Remove an outdated or unused fragment.

---

### `synchronize`
Updates a fragment’s content by re-extracting it from its source document.

#### Parameters
| Name  | Type   | Description                     |
|-------|--------|---------------------------------|
| index | string | Index of the fragment to update.|

#### Return Values
| Type | Description                                                                 |
|------|-----------------------------------------------------------------------------|
| bool | `TRUE` on success, `FALSE` on failure (e.g., source document not found).    |

#### Inner Mechanisms
1. Loads the source document using the `document` class.
2. Extracts the fragment using the `range` and `type` properties.
3. Writes the extracted content to the fragment’s file and clears the cache.

#### Usage Example
```php
$pool = new content_pool();
$pool->synchronize("123");
```
**Use Case**: Manually refresh a fragment after its source document is updated.

---

### `synchronize_content`
Updates all fragments referencing a specific source document.

#### Parameters
| Name           | Type   | Description                     |
|----------------|--------|---------------------------------|
| content_index  | string | Index of the source document.   |

#### Inner Mechanisms
1. Finds all fragments referencing the `content_index`.
2. Re-extracts and updates each fragment using the `document` class.

#### Usage Example
```php
$pool = new content_pool();
$pool->synchronize_content("homepage");
```
**Use Case**: Bulk-update fragments after editing a document.

---

### `synchronize_all`
Updates all fragments in the content pool.

#### Inner Mechanisms
1. Groups fragments by their source document.
2. Retrieves all source documents in a single query.
3. Re-extracts and updates each fragment.

#### Usage Example
```php
$pool = new content_pool();
$pool->synchronize_all();
```
**Use Case**: Full system synchronization after a major content update.


<!-- HASH:a338a83d54ea2e98b317d8f1bc45a662 -->
