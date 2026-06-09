# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/string.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/string.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## String Utilities (`string.inc`)

Core string manipulation and validation utilities for the PWNC Web Platform. These functions provide multibyte-safe, locale-aware, and context-sensitive string operations for content processing, URL generation, and data sanitization.

---

## Functions

### `strtoalphanum`

Converts a string to alphanumeric characters only, replacing non-alphanumeric characters with a specified replacement string. Trims leading/trailing replacements and collapses multiple replacements into one.

| Parameter     | Type     | Description                                                                 |
|---------------|----------|-----------------------------------------------------------------------------|
| `$string`     | `string` | Input string to process.                                                    |
| `$replacement`| `string` | Replacement string for non-alphanumeric characters. Default: `" "` (space). |

**Return Value:**
`string` – Processed string containing only alphanumeric characters and the replacement string.

**Inner Mechanisms:**
1. Decodes HTML entities to their plain-text equivalents.
2. Uses Unicode-aware regex (`\p{L}` for letters, `\p{N}` for numbers) to replace non-alphanumeric characters.
3. Trims leading/trailing replacements and collapses multiple replacements into one.

**Usage Context:**
Sanitizing user input for URLs, filenames, or identifiers where only alphanumeric characters are allowed.

**Example:**
```php
$slug = strtoalphanum("Hello, World! 123", "-");
// Returns: "Hello-World-123"
```

---

### `strtonum`

Extracts and converts the first numeric value found in a string into a float, respecting locale-specific thousand and decimal separators.

| Parameter | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$string` | `string` | Input string containing numbers.|

**Return Value:**
`float|FALSE` – Extracted numeric value as float, or `FALSE` if no number is found.

**Inner Mechanisms:**
1. Uses regex to find the first numeric substring, including locale-specific separators.
2. Replaces locale-specific separators with standard `.` for float conversion.

**Usage Context:**
Parsing user input (e.g., form fields) where numeric values are embedded in text.

**Example:**
```php
$price = strtonum("Price: 1.234,56 €");
// Returns: 1234.56 (assuming CMS_L_DECIMAL_SEPARATOR = ",")
```

---

### `stripspaces`

Normalizes whitespace in a string, optionally preserving newlines and limiting empty lines.

| Parameter            | Type      | Description                                                                 |
|----------------------|-----------|-----------------------------------------------------------------------------|
| `$string`            | `string`  | Input string to process.                                                    |
| `$preserve_newlines` | `bool`    | If `TRUE`, preserves newlines. Default: `FALSE`.                            |
| `$limit_empty_lines` | `bool`    | If `TRUE`, limits consecutive empty lines to 2. Default: `TRUE`.            |

**Return Value:**
`string` – Processed string with normalized whitespace.

**Inner Mechanisms:**
- If `$preserve_newlines` is `TRUE`, collapses horizontal whitespace but preserves newlines.
- If `$limit_empty_lines` is `TRUE`, limits consecutive newlines to 2.

**Usage Context:**
Cleaning up user-generated content (e.g., comments, articles) before display or storage.

**Example:**
```php
$text = "  Hello   \n\n\nWorld  ";
echo stripspaces($text, TRUE, TRUE);
// Output: "Hello\n\nWorld"
```

---

### `nl2br`

Converts newlines to `<br>` tags, marking consecutive newlines with a special class for styling.

| Parameter | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$string` | `string` | Input string with newlines.     |

**Return Value:**
`string` – String with newlines replaced by `<br>` tags.

**Inner Mechanisms:**
1. Normalizes `\r\n` to `\n`.
2. Replaces consecutive newlines with `<br class="multiple">`.
3. Replaces single newlines with `<br>`.

**Usage Context:**
Displaying plain-text content in HTML while preserving line breaks.

**Example:**
```php
$text = "Hello\n\nWorld";
echo nl2br($text);
// Output: "Hello<br class="multiple"><br>World"
```

---

### `limitstr`

Truncates a string to a specified length, prioritizing whole segments separated by a delimiter.

| Parameter   | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$string`   | `string` | Input string to truncate.                                                   |
| `$length`   | `int`    | Maximum length of the output string. Default: `255`.                        |
| `$separator`| `string` | Delimiter for segmenting the string. Default: `","`.                        |

**Return Value:**
`string` – Truncated string, preserving whole segments.

**Inner Mechanisms:**
1. Splits the string by `$separator`.
2. Reconstructs the string segment by segment until the length limit is reached.

**Usage Context:**
Displaying truncated text (e.g., previews, summaries) where segment integrity is important.

**Example:**
```php
$tags = "php, javascript, html, css, sql";
echo limitstr($tags, 20);
// Output: "php, javascript, html"
```

---

### `first_paragraph`

Extracts the first paragraph from a string, defined as the initial non-whitespace sequence.

| Parameter | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$string` | `string` | Input string with paragraphs.   |

**Return Value:**
`string` – First paragraph of the string.

**Inner Mechanisms:**
Uses regex (`^\V+`) to match the first non-vertical-whitespace sequence.

**Usage Context:**
Extracting previews or summaries from multi-paragraph content.

**Example:**
```php
$text = "Hello World.\n\nThis is a test.";
echo first_paragraph($text);
// Output: "Hello World."
```

---

### `first_words`

Truncates a string to a specified word count, appending an ellipsis if truncated.

| Parameter  | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$string`  | `string` | Input string to truncate.                                                   |
| `$length`  | `int`    | Maximum number of characters. Default: `250`.                               |
| `$ellipsis`| `string` | Ellipsis to append if truncated. Default: `" …"`.                           |

**Return Value:**
`string` – Truncated string with ellipsis if applicable.

**Inner Mechanisms:**
1. Uses regex to match the first `$length` characters, stopping at word boundaries.
2. Appends ellipsis if the string is truncated.

**Usage Context:**
Generating previews or excerpts from longer text.

**Example:**
```php
$text = "This is a long string that needs truncation.";
echo first_words($text, 20);
// Output: "This is a long …"
```

---

### `zerofill`

Pads a string with null bytes (`\0`) to a specified length.

| Parameter | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$string` | `string` | Input string to pad.            |
| `$length` | `int`    | Desired length of the output.   |

**Return Value:**
`string` – Padded string.

**Usage Context:**
Binary data manipulation or fixed-length field generation.

**Example:**
```php
$binary = zerofill("data", 8);
// Returns: "data\0\0\0\0"
```

---

### `stre`, `nstre`, `streq`, `nstreq`

String emptiness and equality checks.

| Function  | Parameters               | Return Type | Description                                                                 |
|-----------|--------------------------|-------------|-----------------------------------------------------------------------------|
| `stre`    | `$string` (`string`)     | `bool`      | Returns `TRUE` if the string is empty.                                      |
| `nstre`   | `$string` (`string`)     | `bool`      | Returns `TRUE` if the string is not empty.                                  |
| `streq`   | `$string1`, `$string2`   | `bool`      | Returns `TRUE` if the strings are equal.                                    |
| `nstreq`  | `$string1`, `$string2`   | `bool`      | Returns `TRUE` if the strings are not equal.                                |

**Usage Context:**
Input validation, conditional logic, or comparisons.

**Example:**
```php
if (nstre($username) && streq($username, "admin")) {
    // Grant access
}
```

---

### `strieq`

Case-insensitive string equality check using regex.

| Parameter | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$string1`| `string` | First string to compare.        |
| `$string2`| `string` | Second string to compare.       |

**Return Value:**
`bool` – `TRUE` if `$string2` contains `$string1` (case-insensitive).

**Usage Context:**
Case-insensitive comparisons (e.g., user input matching).

**Example:**
```php
if (strieq("admin", $username)) {
    // Grant access
}
```

---

### `substri_count`

Counts the number of occurrences of a substring in a string (case-insensitive).

| Parameter | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$source` | `string` | String to search in.            |
| `$string` | `string` | Substring to count.             |

**Return Value:**
`int` – Number of occurrences.

**Usage Context:**
Analyzing text for keyword density or pattern matching.

**Example:**
```php
$count = substri_count("Hello hello world", "hello");
// Returns: 2
```

---

### `verify_email`

Validates an email address format.

| Parameter | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$email`  | `string` | Email address to validate.      |

**Return Value:**
`bool` – `TRUE` if the email format is valid.

**Inner Mechanisms:**
Uses regex to validate:
- Local part (before `@`).
- Domain part (after `@`), including Unicode characters.
- Top-level domain (2-4 characters).

**Usage Context:**
Email input validation in forms.

**Example:**
```php
if (verify_email("user@example.com")) {
    // Proceed with registration
}
```

---

### `unique_id`

Generates a random alphanumeric string of a specified length.

| Parameter | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$count`  | `int`    | Length of the ID. Default: `8`. |

**Return Value:**
`string` – Random alphanumeric ID.

**Inner Mechanisms:**
1. Uses a static character set (`0-9a-zA-Z`).
2. Generates random characters using `mt_rand`.

**Usage Context:**
Generating unique identifiers for sessions, tokens, or temporary files.

**Example:**
```php
$token = unique_id(16);
// Example output: "aB3x9Y7pL2qR4sT8"
```

---

### `strabridge`

Truncates a string to a specified length, adding an ellipsis in the middle or at the end.

| Parameter   | Type      | Description                                                                 |
|-------------|-----------|-----------------------------------------------------------------------------|
| `$string`   | `string`  | Input string to truncate.                                                   |
| `$length`   | `int`     | Maximum length of the output string. Default: `50`.                         |
| `$cut_end`  | `bool`    | If `TRUE`, truncates at the end. If `FALSE`, truncates in the middle.       |

**Return Value:**
`string` – Truncated string with ellipsis.

**Inner Mechanisms:**
- If `$cut_end` is `TRUE`, truncates the end and appends `" …"`.
- If `FALSE`, splits the string into 65% start and 35% end, joining with `" … "`.

**Usage Context:**
Displaying long strings (e.g., filenames, URLs) in limited space.

**Example:**
```php
$longString = "This is a very long string that needs truncation.";
echo strabridge($longString, 20, FALSE);
// Output: "This is a … truncation."
```

---

### `generate_pseudonym`

Generates a pseudorandom name from a predefined set of syllables, optionally seeded for reproducibility.

| Parameter   | Type     | Description                                                                 |
|-------------|----------|-----------------------------------------------------------------------------|
| `$seed`     | `string` | Seed for reproducible generation. Default: `NULL`.                          |
| `$syllables`| `int`    | Number of syllables in the name. Default: `3`.                              |

**Return Value:**
`string` – Generated pseudonym with the first letter capitalized.

**Inner Mechanisms:**
1. Uses a predefined array of syllables (Japanese-inspired).
2. If `$seed` is provided, seeds the random number generator for reproducibility.
3. Combines syllables randomly, ending with a syllable that may include `"n"`.

**Usage Context:**
Generating usernames, placeholders, or test data.

**Example:**
```php
$name = generate_pseudonym("user123", 4);
// Example output: "Kirino"
```

---

### `strtocolor`

Converts a string into a deterministic HSL color, optionally wrapped in a `<span>` tag. Ensures adjacent colors have a minimum hue difference.

| Parameter    | Type      | Description                                                                 |
|--------------|-----------|-----------------------------------------------------------------------------|
| `$string`    | `string`  | Input string to convert.                                                    |
| `$lightness` | `int`     | Lightness percentage for the HSL color. Default: `75`.                      |
| `$span`      | `bool`    | If `TRUE`, wraps the string in a `<span>` with the color. Default: `TRUE`.  |
| `$diff_min`  | `int`     | Minimum hue difference (in degrees) from the previous color. Default: `0`.  |

**Return Value:**
`string` – HSL color string or `<span>`-wrapped input string.

**Inner Mechanisms:**
1. Uses the `djb2` hash function to generate a hue value from the string.
2. Adjusts the hue to ensure a minimum difference from the previous color.
3. Formats the color as HSL and optionally wraps the string in a `<span>`.

**Usage Context:**
Generating consistent colors for tags, categories, or user avatars.

**Example:**
```php
echo strtocolor("admin", 75, TRUE);
// Output: <span style="COLOR:hsl(123,75%,75%)">admin</span>
```


<!-- HASH:75454808701cd4b938c7078a465c1d24 -->
