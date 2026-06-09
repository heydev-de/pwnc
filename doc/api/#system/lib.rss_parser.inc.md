# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.rss_parser.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.rss_parser.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## RSS Parser Module

This module provides a lightweight, zero-dependency RSS 2.0 parser and renderer for the PWNC Web Platform. It handles XML parsing, data extraction, and HTML output generation with configurable display options. The parser supports caching, multibyte-safe text processing, and proper escaping for web output.

---

## `rss_parser_attribute_list` Class

Manages a collection of RSS attributes as key-value pairs. Each attribute is stored as an `rss_parser_attribute` object.

### Properties

| Name | Value/Default | Description |
|------|---------------|-------------|
| *(dynamic)* | `rss_parser_attribute` | Attributes are added dynamically via `add()` method. |

### Methods

#### `add($name, $value)`
Adds a new attribute to the list.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Attribute name. |
| `$value` | `mixed` | Attribute value. |

**Inner Mechanisms:**
- Creates a new `rss_parser_attribute` object and assigns it to a dynamic property named `$name`.

**Usage Example:**
```php
$attributes = new rss_parser_attribute_list();
$attributes->add('domain', 'https://example.com');
echo $attributes->domain; // Outputs: https://example.com
```

---

#### `__get($name)`
Retrieves an attribute by name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Attribute name. |

**Return Values:**
- `rss_parser_attribute` if attribute exists.
- `NULL` if attribute does not exist.

**Usage Example:**
```php
$attributes = new rss_parser_attribute_list();
$attributes->add('version', '2.0');
echo $attributes->version; // Outputs: 2.0
```

---

#### `__toString()`
Returns an empty string. Implemented for type safety.

**Return Values:**
- `string`: Always empty.

---

## `rss_parser_attribute` Class

Represents a single RSS attribute with a value.

### Properties

| Name | Value/Default | Description |
|------|---------------|-------------|
| `value` | `""` | The attribute's value. |

### Constructor

#### `__construct($value)`
Initializes the attribute with a value.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | `mixed` | The attribute value. |

---

### Methods

#### `__get($name)`
Returns the attribute's value regardless of the property name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Ignored. |

**Return Values:**
- `mixed`: The stored value.

**Usage Example:**
```php
$attr = new rss_parser_attribute('2.0');
echo $attr->value; // Outputs: 2.0
echo $attr->anything; // Outputs: 2.0
```

---

#### `__toString()`
Returns the attribute's value as a string.

**Return Values:**
- `string`: The stored value.

---

## `rss_parser_node` Class

Represents a node in the parsed RSS XML tree. Supports hierarchical structure, attributes, and dynamic property access.

### Properties

| Name | Value/Default | Description |
|------|---------------|-------------|
| `_parent` | `NULL` | Parent node reference. |
| `_name` | `""` | Node name (e.g., `title`, `item`). |
| `_data` | `""` | Node's text content. |
| `_attrib` | `rss_parser_attribute_list` | Node's attributes. |

### Constructor

#### `__construct($name = "")`
Initializes a node with a name and empty attribute list.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Node name. |

---

### Methods

#### `add_attribute($name, $value)`
Adds an attribute to the node.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Attribute name. |
| `$value` | `mixed` | Attribute value. |

**Usage Example:**
```php
$node = new rss_parser_node('enclosure');
$node->add_attribute('url', 'https://example.com/podcast.mp3');
```

---

#### `add_node($name)`
Creates and adds a child node.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Child node name. |

**Return Values:**
- `rss_parser_node`: The newly created child node.

**Inner Mechanisms:**
- If a child with the same name already exists, it converts the child into an array and appends the new node.
- Sets the parent reference of the child node.

**Usage Example:**
```php
$channel = new rss_parser_node('channel');
$item = $channel->add_node('item');
$item->add_node('title')->_data = 'Sample Title';
```

---

#### `get_parent()`
Returns the parent node.

**Return Values:**
- `rss_parser_node` or `NULL`: Parent node or `NULL` if root.

---

#### `get_path()`
Returns the full path from root to this node (e.g., `rss:channel:item`).

**Return Values:**
- `string`: Colon-separated path.

**Usage Example:**
```php
$item = $channel->add_node('item');
echo $item->get_path(); // Outputs: rss:channel:item
```

---

#### `__get($name)`
Retrieves a child node by name.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$name` | `string` | Child node name. |

**Return Values:**
- `rss_parser_node`, `array`, or `NULL`: Child node(s) or `NULL` if not found.

---

#### `__toString()`
Returns the node's text content (`_data`).

**Return Values:**
- `string`: Node content.

---

## `rss_parser` Class

Main RSS parser and renderer. Handles XML parsing, caching, and HTML output.

### Properties

| Name | Value/Default | Description |
|------|---------------|-------------|
| `enable_html_filter` | `FALSE` | If `TRUE`, strips HTML from descriptions. |
| `max_text_length` | `100` | Maximum length for text fields (e.g., titles). |
| `max_description_length` | `400` | Maximum length for descriptions (HTML filter only). |
| `max_item_number` | `20` | Maximum number of items to display. |
| `show_channel` | `TRUE` | Whether to display channel info. |
| *(Numerous `show_*` properties)* | `TRUE` | Toggle display of specific RSS elements. |
| `parser` | `NULL` | Internal XML parser. |
| `data` | `NULL` | Root node of parsed RSS data. |
| `node` | `NULL` | Current node during parsing. |
| `stack` | `[]` | Tag stack for XML parsing. |
| `structure` | *(array)* | Valid RSS 2.0 paths. |

---

### Constructor

#### `__construct()`
Initializes the XML parser and sets up event handlers.

**Inner Mechanisms:**
- Creates a UTF-8 XML parser.
- Registers `_start`, `_end`, and `_data` as event handlers.
- Disables case folding for tag names.

---

### Methods

#### `parse($source)`
Parses an RSS feed from a URL.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$source` | `string` | URL of the RSS feed. |

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Uses `http_fopen()` to fetch the feed.
- Streams data in chunks (`CMS_HTTP_SIZE_CHUNK`).
- Caches results for 60 seconds.
- Validates against RSS 2.0 structure.

**Usage Example:**
```php
$parser = new rss_parser();
if ($parser->parse('https://example.com/feed.rss')) {
    $parser->display();
}
```

---

#### `display()`
Renders the parsed RSS feed as HTML.

**Return Values:**
- `bool`: `TRUE` if rendered, `FALSE` if invalid or not RSS 2.0.

**Inner Mechanisms:**
- Outputs a `<section class="rss">` with nested `<div>` and `<article>` elements.
- Respects all `show_*` properties.
- Uses `cms_url()` for link generation and `x()` for escaping.
- Truncates long text/description fields.

**Usage Example:**
```php
$parser = new rss_parser();
$parser->show_channel_image = FALSE;
$parser->max_item_number = 5;
$parser->parse('https://example.com/feed.rss');
$parser->display();
```

---

#### `fd($value)`
Filters and escapes description text.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | `string` | Raw description text. |

**Return Values:**
- `string`: Filtered and escaped text.

**Inner Mechanisms:**
- Strips HTML if `enable_html_filter` is `TRUE`.
- Truncates to `max_description_length`.
- Escapes XML special characters.

**Usage Example:**
```php
$parser = new rss_parser();
$parser->enable_html_filter = TRUE;
echo $parser->fd('<p>Hello & World</p>'); // Outputs: Hello &amp; World …
```

---

#### `ft($value)`
Filters and escapes plain text.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$value` | `string` | Raw text. |

**Return Values:**
- `string`: Filtered and escaped text.

**Inner Mechanisms:**
- Decodes HTML entities.
- Truncates to `max_text_length`.
- Escapes XML special characters.

**Usage Example:**
```php
$parser = new rss_parser();
echo $parser->ft('Hello &amp; World'); // Outputs: Hello &amp; World …
```

---

#### `_start($parser, $tag, $attribute)`
XML parser callback for opening tags.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$parser` | `resource` | XML parser. |
| `$tag` | `string` | Tag name. |
| `$attribute` | `array` | Tag attributes. |

**Inner Mechanisms:**
- Validates tag path against `structure`.
- Creates a new node and adds attributes.
- Pushes tag onto stack.

---

#### `_data($parser, $data)`
XML parser callback for character data.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$parser` | `resource` | XML parser. |
| `$data` | `string` | Text content. |

**Inner Mechanisms:**
- Appends data to current node's `_data`.

---

#### `_end($parser, $tag)`
XML parser callback for closing tags.

| Parameter | Type | Description |
|-----------|------|-------------|
| `$parser` | `resource` | XML parser. |
| `$tag` | `string` | Tag name. |

**Inner Mechanisms:**
- Validates tag against stack.
- Moves back to parent node.
- Pops tag from stack.


<!-- HASH:e6a7d82decd21273ceeaef418c471901 -->
