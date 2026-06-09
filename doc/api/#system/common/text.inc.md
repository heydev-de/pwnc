# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/text.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/text.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Text Processing Utilities (`text.inc`)

This file provides core text manipulation utilities for the PWNC Web Platform. It includes functions for parsing formatted text, removing formatting, extracting images, highlighting keywords, converting HTML to plain text, and comparing text similarity. These utilities are designed to handle multibyte characters safely and support a custom lightweight markup syntax.

---

## `parse_text()`

Parses custom-formatted text into HTML, supporting inline formatting, links, images, and tables.

### Parameters

| Name       | Value/Default | Description                                                                 |
|------------|---------------|-----------------------------------------------------------------------------|
| `$text`    |               | Input text with custom formatting.                                          |
| `$token`   | `TRUE`        | Whether to parse tokens (`%token%`).                                        |
| `$base_url`| `CMS_HOST`    | Base URL for determining internal links.                                    |

### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| string | Parsed HTML output.                                                         |

### Inner Mechanisms

1. **Formatting Markers**: Uses a static map of formatting markers (e.g., `+`, `/`, `_`) to HTML tags.
2. **Token Parsing**: Supports `%token%` syntax if `$token` is `TRUE`.
3. **Link Handling**: Converts `[url]` into `<a>` tags, with special handling for internal links.
4. **Image Handling**: Converts `[IMG url]` into `<img>` tags, with alignment options (`<-IMG`, `IMG->`).
5. **Table Handling**: Converts `[# ... | ...]` into HTML tables with inline styles.
6. **Whitespace Handling**: Preserves and converts whitespace into HTML entities or tags (`&nbsp;`, `<br>`).

### Usage Example

```php
$formattedText = "This is [+bold+] and [/italic/] text with a [https://pwnc.it link].";
echo parse_text($formattedText);
```
**Output**:
```html
This is <b>bold</b> and <i>italic</i> text with a <a href="https://pwnc.it">link</a>.
```

---

## `remove_format()`

Removes custom formatting from text, converting it to plain text while preserving structure.

### Parameters

| Name             | Value/Default | Description                                                                 |
|------------------|---------------|-----------------------------------------------------------------------------|
| `$text`          |               | Input text with custom formatting.                                          |
| `$token`         | `TRUE`        | Whether to parse tokens (`%token%`).                                        |
| `$discard_links` | `FALSE`       | Whether to discard link URLs.                                               |

### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| string | Plain text with formatting removed.                                         |

### Inner Mechanisms

1. **Formatting Markers**: Strips formatting markers (e.g., `[+bold+]` → `bold`).
2. **Link Handling**: Converts `[url]` into plain text (e.g., `url` or `url (description)`).
3. **Table Handling**: Converts tables into tab-separated plain text.
4. **Whitespace Handling**: Preserves line breaks and indentation.

### Usage Example

```php
$formattedText = "Check out [+this+] [https://pwnc.it link].";
echo remove_format($formattedText);
```
**Output**:
```
Check out this link (https://pwnc.it).
```

---

## `get_first_image()`

Extracts the URL of the first image in formatted text.

### Parameters

| Name   | Value/Default | Description                                                                 |
|--------|---------------|-----------------------------------------------------------------------------|
| `$text`|               | Input text with custom formatting.                                          |

### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| string | URL of the first image, or `NULL` if no image is found.                    |

### Inner Mechanisms

1. **Regex Matching**: Uses regex to find the first occurrence of `[IMG ...]`, `[<-IMG ...]`, or `[IMG-> ...]`.
2. **URL Extraction**: Captures the URL from the matched pattern.

### Usage Example

```php
$text = "Here is an [IMG image://logo.png] and another [IMG image://banner.jpg].";
$imageUrl = get_first_image($text);
echo $imageUrl; // Output: "image://logo.png"
```

---

## `parse_token()`

Parses tokens (`%token%`) in text using the `token` module.

### Parameters

| Name   | Value/Default | Description                                                                 |
|--------|---------------|-----------------------------------------------------------------------------|
| `$text`|               | Input text containing tokens.                                               |

### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| string | Text with tokens replaced by their resolved values.                        |

### Inner Mechanisms

1. **Lazy Loading**: Loads the `token` module only when needed.
2. **Token Resolution**: Delegates token parsing to the `token` module's `apply()` method.

### Usage Example

```php
$text = "Hello, %username%!";
echo parse_token($text); // Output: "Hello, John!" (if %username% resolves to "John")
```

---

## `quote_text()`

Highlights keywords in text and extracts relevant passages.

### Parameters

| Name                      | Value/Default | Description                                                                 |
|---------------------------|---------------|-----------------------------------------------------------------------------|
| `$text`                   |               | Input text to search.                                                       |
| `$keyword`                |               | Keyword(s) to highlight.                                                    |
| `$minimum_keyword_length` | `3`           | Minimum length of keywords to consider.                                     |

### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| string | Highlighted text with relevant passages, or an empty string if no matches. |

### Inner Mechanisms

1. **Keyword Processing**: Splits keywords into individual terms and sorts them by length.
2. **Passage Extraction**: Finds passages containing keywords, with context (50 chars before, 150 chars after).
3. **Highlighting**: Wraps keywords in `<strong>` tags.
4. **Merging Passages**: Combines overlapping or adjacent passages into a single excerpt.

### Usage Example

```php
$text = "The quick brown fox jumps over the lazy dog.";
$keyword = "fox jumps";
echo quote_text($text, $keyword);
```
**Output**:
```html
The quick brown <strong>fox</strong> <strong>jumps</strong> over the lazy dog.
```

---

## `htmltoplain()`

Converts HTML to plain text, optionally preserving formatting.

### Parameters

| Name               | Value/Default | Description                                                                 |
|--------------------|---------------|-----------------------------------------------------------------------------|
| `$string`          |               | Input HTML string.                                                          |
| `$format`          | `FALSE`       | Whether to preserve basic formatting (line breaks, lists, etc.).            |
| `$discard_links`   | `FALSE`       | Whether to discard link URLs.                                               |
| `$discard_images`  | `FALSE`       | Whether to discard image alt text.                                          |

### Return Values

| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| string | Plain text representation of the HTML.                                      |

### Inner Mechanisms

1. **HTML Cleanup**: Removes scripts, styles, comments, and head sections.
2. **Tag Conversion**: Converts HTML tags to plain text equivalents (e.g., `<br>` → `\n`).
3. **Link/Image Handling**: Preserves or discards links and images based on parameters.
4. **Whitespace Normalization**: Trims and normalizes whitespace.

### Usage Example

```php
$html = "<p>Hello, <strong>world</strong>!</p><br><a href='https://pwnc.it'>PWNC</a>";
echo htmltoplain($html, TRUE);
```
**Output**:
```
Hello, world!

PWNC <https://pwnc.it>
```

---

## `text_similarity()`

Calculates the similarity between two texts as a percentage.

### Parameters

| Name    | Value/Default | Description                                                                 |
|---------|---------------|-----------------------------------------------------------------------------|
| `$text1`|               | First input text.                                                           |
| `$text2`|               | Second input text.                                                          |

### Return Values

| Type  | Description                                                                 |
|-------|-----------------------------------------------------------------------------|
| float | Similarity percentage (0-100).                                              |

### Inner Mechanisms

1. **Fingerprinting**: Uses `fingerprint()` to generate token sets for each text.
2. **Intersection Calculation**: Compares token sets to determine overlap.
3. **Normalization**: Returns a percentage based on the total number of unique tokens.

### Usage Example

```php
$text1 = "The quick brown fox jumps over the lazy dog.";
$text2 = "A quick brown fox leaps over a sleepy dog.";
echo text_similarity($text1, $text2); // Output: ~70 (example value)
```

---

## `tokenize_text()`

Splits text into an array of tokens (words, ideograms, etc.).

### Parameters

| Name                 | Value/Default | Description                                                                 |
|----------------------|---------------|-----------------------------------------------------------------------------|
| `$text`              |               | Input text to tokenize.                                                     |
| `$cleanup_repeats`   | `FALSE`       | Whether to reduce repeated characters (e.g., `aaa` → `aa`).                 |

### Return Values

| Type  | Description                                                                 |
|-------|-----------------------------------------------------------------------------|
| array | Array of tokens.                                                            |

### Inner Mechanisms

1. **Cleanup**: Removes control characters, normalizes whitespace, and isolates ideograms.
2. **Token Separation**: Splits text at word boundaries using `CMS_REGEX_BORDER`.
3. **Repeat Handling**: Optionally reduces repeated characters (e.g., for informal text).

### Usage Example

```php
$text = "Hello, 世界! This is a test.";
$tokens = tokenize_text($text);
print_r($tokens);
```
**Output**:
```
Array
(
    [0] => Hello
    [1] => 世
    [2] => 界
    [3] => This
    [4] => is
    [5] => a
    [6] => test
)
```


<!-- HASH:a3a6e8f35edc4e9e64361a2fc86c7055 -->
