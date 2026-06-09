# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.html.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.html.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## HTML Class and Related Functions

The `lib.html.inc` file provides utilities for parsing and analyzing HTML documents. It includes:

1. **`html_page_info()`** – A standalone function that extracts metadata, content structure, and links from a given HTML page.
2. **`html` class** – A low-level HTML parser that navigates and extracts elements, attributes, and text content from HTML documents.

---

## Constants

| Name               | Value | Description                                                                 |
|--------------------|-------|-----------------------------------------------------------------------------|
| `CMS_HTML_GET_FIRST` | `1`   | Flag to start parsing from the beginning of the document.                  |
| `CMS_HTML_GET_NEXT`  | `2`   | Flag to continue parsing from the current position in the document.        |

---

## `html_page_info($url)`

### Purpose
Extracts structured information from an HTML page, including metadata, title, headings, body text, side content, and links. Useful for content analysis, SEO, or indexing.

### Parameters

| Name  | Type   | Description                                                                 |
|-------|--------|-----------------------------------------------------------------------------|
| `$url` | string | URL of the HTML page to analyze.                                            |

### Return Values
- **`array`** – Structured data with the following keys:
  - `meta`: Associative array of meta tags (e.g., `["description" => "..."]`).
  - `title`: Page title.
  - `h1`, `h2`, `h3`: Concatenated text of all headings of the respective level.
  - `copy`: Main content text (excluding headings and side content).
  - `text`: Combined headings and main content (separated by " — ").
  - `side`: Side content (from `<aside>`, `<footer>`, `<nav>`).
  - `links`: Array of links (`[ ["url" => "...", "text" => "..."], ... ]`).
- **`FALSE`** – If the URL cannot be fetched, is not valid HTML, or lacks basic structure.

### Inner Mechanisms
1. **Fetching**: Uses `http_fopen()` and `http_fetch_data()` to retrieve the HTML.
2. **Validation**: Checks for basic HTML structure (`<html>`, `<head>`, `<body>`).
3. **Metadata Extraction**: Parses `<meta>` tags and normalizes keys/values.
4. **Content Segmentation**: Uses the `html` class to traverse the document, extracting headings, body text, and side content.
5. **Link Processing**: Resolves relative URLs and filters out `nofollow` links.

### Usage Example
```php
$info = html_page_info("https://example.com");
if ($info !== FALSE) {
    echo "Title: " . q($info["title"]) . "\n";
    echo "Description: " . q($info["meta"]["description"] ?? "") . "\n";
    echo "H1: " . q($info["h1"]) . "\n";
    foreach ($info["links"] as $link) {
        echo "Link: " . q($link["text"]) . " -> " . q($link["url"]) . "\n";
    }
}
```
**Explanation**: Fetches and displays the title, description, top-level headings, and all links from a webpage.

---

## `html` Class

### Purpose
A lightweight, multibyte-safe HTML parser for extracting elements, attributes, and text content from HTML documents. Designed for performance and simplicity.

### Properties

| Name       | Type   | Description                                                                 |
|------------|--------|-----------------------------------------------------------------------------|
| `$file`    | string | The HTML document content. `NULL` if loading failed.                       |
| `$position`| int    | Current parsing position in the document.                                  |

---

### `__construct($url)`

#### Purpose
Initializes the parser by fetching and preprocessing an HTML document from a given URL.

#### Parameters

| Name  | Type   | Description                                                                 |
|-------|--------|-----------------------------------------------------------------------------|
| `$url` | string | URL of the HTML document to load.                                           |

#### Inner Mechanisms
1. **Fetching**: Uses `http_fopen()` and `http_fetch_data()` to retrieve the HTML.
2. **Character Set Conversion**: Detects and converts non-UTF-8 content to UTF-8 using meta tags or HTTP headers.
3. **Normalization**: Applies `utf8_normalize()` to ensure consistent encoding.
4. **Noise Removal**: Strips comments (`<!-- -->`), scripts (`<script>`), and styles (`<style>`).

#### Usage Example
```php
$html = new html("https://example.com");
if ($html->file !== NULL) {
    echo "Document loaded successfully.\n";
}
```
**Explanation**: Loads an HTML document and prepares it for parsing.

---

### `get_attributes($string)`

#### Purpose
Parses HTML element attributes from a string into an associative array.

#### Parameters

| Name      | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| `$string` | string | The attribute string (e.g., `class="example" id='main'`).                  |

#### Return Values
- **`array`** – Associative array of attributes (e.g., `["class" => "example", "id" => "main"]`).
  - Boolean attributes (e.g., `disabled`) are set to `TRUE`.
- **`FALSE`** – If no attributes are found.

#### Inner Mechanisms
Uses regex to match:
1. `attribute="value"`
2. `attribute='value'`
3. `attribute=value`
4. `attribute` (boolean attributes)

#### Usage Example
```php
$attributes = $html->get_attributes('class="header" disabled data-id=123');
if ($attributes !== FALSE) {
    echo "Class: " . q($attributes["class"]) . "\n"; // Output: Class: header
    echo "Disabled: " . ($attributes["disabled"] ? "Yes" : "No") . "\n"; // Output: Disabled: Yes
}
```
**Explanation**: Parses an attribute string and extracts values for further use.

---

### `get($option = CMS_HTML_GET_NEXT, $element = NULL, $get_pcdata = TRUE, $ignore_nested = TRUE)`

#### Purpose
Extracts the next or first occurrence of an HTML element, including its attributes and text content.

#### Parameters

| Name             | Type    | Description                                                                 |
|------------------|---------|-----------------------------------------------------------------------------|
| `$option`        | int     | `CMS_HTML_GET_FIRST` (start from beginning) or `CMS_HTML_GET_NEXT` (continue). |
| `$element`       | string  | Element name or regex pattern (e.g., `"div"`, `"h[1-3]"`). `NULL` matches any element. |
| `$get_pcdata`    | bool    | If `TRUE`, includes text content between opening/closing tags.             |
| `$ignore_nested` | bool    | If `TRUE`, skips nested elements of the same type.                         |

#### Return Values
- **`array`** – Structured data with keys:
  - `#element`: Element name (lowercase).
  - `#attribute`: Associative array of attributes.
  - `#pcdata`: Text content (if `$get_pcdata = TRUE`).
  - `#offset`: Starting position of the element in the document.
- **`NULL`** – If no matching element is found.

#### Inner Mechanisms
1. **Element Matching**: Uses regex to find the opening tag.
2. **Attribute Parsing**: Delegates to `get_attributes()`.
3. **Text Extraction**: Captures content between opening/closing tags, handling nested elements if `$ignore_nested = FALSE`.

#### Usage Example
```php
$html = new html("https://example.com");
while ($div = $html->get(CMS_HTML_GET_NEXT, "div")) {
    if (isset($div["#attribute"]["class"]) && streq($div["#attribute"]["class"], "content")) {
        echo "Content: " . q($div["#pcdata"]) . "\n";
        break;
    }
}
```
**Explanation**: Iterates through `<div>` elements until one with `class="content"` is found, then prints its text.

---

### `reset()`

#### Purpose
Resets the parser's position to the start of the document.

#### Return Values
- **`TRUE`** – On success.
- **`FALSE`** – If the document is not loaded (`$file = NULL`).

#### Usage Example
```php
$html->reset();
$title = $html->get(CMS_HTML_GET_FIRST, "title");
echo "Title: " . q($title["#pcdata"]) . "\n";
```
**Explanation**: Resets the parser to extract the `<title>` from the beginning of the document.


<!-- HASH:aec30a13b9603d32e9ff3f8b3efe287c -->
