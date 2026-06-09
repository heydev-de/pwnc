# PWNC API Documentation

[← Index](../README.md) | [`#system/sys.utf8.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/sys.utf8.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## UTF-8 Utility Module

This file provides a comprehensive set of UTF-8 string manipulation utilities for the PWNC Web Platform. It includes functions for character encoding conversion, detection, normalization, case transformation, and substring operations. The module ensures multibyte-safe operations with fallback mechanisms when the `mbstring` extension is unavailable.

---

### Constants

| Name | Value | Description |
|------|-------|-------------|
| `CMS_UTF8_CHARSET_UTF_8` | `"utf-8"` | UTF-8 character encoding identifier. |
| `CMS_UTF8_CHARSET_ISO_8859_1` | `"iso-8859-1"` | ISO-8859-1 (Latin-1) character encoding identifier. |
| `CMS_UTF8_CHARSET_ISO_8859_2` | `"iso-8859-2"` | ISO-8859-2 (Latin-2) character encoding identifier. |
| `CMS_UTF8_CHARSET_ISO_8859_3` | `"iso-8859-3"` | ISO-8859-3 (Latin-3) character encoding identifier. |
| `CMS_UTF8_CHARSET_ISO_8859_4` | `"iso-8859-4"` | ISO-8859-4 (Latin-4) character encoding identifier. |
| `CMS_UTF8_CHARSET_ISO_8859_5` | `"iso-8859-5"` | ISO-8859-5 (Cyrillic) character encoding identifier. |
| `CMS_UTF8_CHARSET_ISO_8859_6` | `"iso-8859-6"` | ISO-8859-6 (Arabic) character encoding identifier. |
| `CMS_UTF8_CHARSET_ISO_8859_7` | `"iso-8859-7"` | ISO-8859-7 (Greek) character encoding identifier. |
| `CMS_UTF8_CHARSET_ISO_8859_8` | `"iso-8859-8"` | ISO-8859-8 (Hebrew) character encoding identifier. |
| `CMS_UTF8_CHARSET_ISO_8859_9` | `"iso-8859-9"` | ISO-8859-9 (Latin-5) character encoding identifier. |
| `CMS_UTF8_CHARSET_ISO_8859_10` | `"iso-8859-10"` | ISO-8859-10 (Latin-6) character encoding identifier. |
| `CMS_UTF8_CHARSET_ISO_8859_11` | `"iso-8859-11"` | ISO-8859-11 (Thai) character encoding identifier. |
| `CMS_UTF8_CHARSET_ISO_8859_13` | `"iso-8859-13"` | ISO-8859-13 (Latin-7) character encoding identifier. |
| `CMS_UTF8_CHARSET_ISO_8859_14` | `"iso-8859-14"` | ISO-8859-14 (Latin-8) character encoding identifier. |
| `CMS_UTF8_CHARSET_ISO_8859_15` | `"iso-8859-15"` | ISO-8859-15 (Latin-9) character encoding identifier. |
| `CMS_UTF8_CHARSET_ISO_8859_16` | `"iso-8859-16"` | ISO-8859-16 (Latin-10) character encoding identifier. |

---

### Functions

---

#### `utf8_convert`

**Purpose:**
Converts a string from a specified character encoding to UTF-8.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string to convert. |
| `$charset` | `string` | The source character encoding. Defaults to `CMS_UTF8_CHARSET_ISO_8859_1`. |

**Return Values:**
- `string`: The converted UTF-8 string.

**Inner Mechanisms:**
- Prefers `mb_convert_encoding` if available.
- Falls back to a static mapping system for ISO-8859-* encodings.
- Loads encoding maps from included files on demand.

**Usage Context:**
- Used when processing user input or external data in non-UTF-8 encodings.
- Essential for ensuring consistent UTF-8 handling across the platform.

**Example:**
```php
$isoString = "\xE9"; // "é" in ISO-8859-1
$utf8String = utf8_convert($isoString, CMS_UTF8_CHARSET_ISO_8859_1);
// $utf8String now contains the UTF-8 encoded "é"
```

---

#### `utf8_detect`

**Purpose:**
Detects whether a string is valid UTF-8 encoded.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The string to check. |

**Return Values:**
- `bool`: `TRUE` if the string is valid UTF-8, `FALSE` otherwise.

**Inner Mechanisms:**
- Checks for a UTF-8 Byte Order Mark (BOM).
- Prefers `mb_check_encoding` if available.
- Uses regex for shorter strings and bit-level checks for longer strings.

**Usage Context:**
- Validates user input or external data before processing.
- Ensures data integrity when handling multibyte strings.

**Example:**
```php
$userInput = $_POST['text'];
if (!utf8_detect($userInput)) {
    $userInput = utf8_convert($userInput, CMS_UTF8_CHARSET_ISO_8859_1);
}
```

---

#### `utf8_chr`

**Purpose:**
Generates a UTF-8 character from a Unicode code point.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `int` | The Unicode code point. |

**Return Values:**
- `string|bool`: The UTF-8 character or `FALSE` if the code point is invalid.

**Inner Mechanisms:**
- Prefers `mb_chr` if available.
- Manually constructs UTF-8 bytes for 1-4 byte characters.

**Usage Context:**
- Used when generating special characters or emojis dynamically.
- Essential for constructing strings from Unicode code points.

**Example:**
```php
$copyrightSymbol = utf8_chr(0x00A9); // "©"
$emoji = utf8_chr(0x1F600); // "😀"
```

---

#### `utf8_ord`

**Purpose:**
Returns the Unicode code point of the first character in a UTF-8 string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The UTF-8 string. |

**Return Values:**
- `int|bool`: The Unicode code point or `FALSE` if the string is empty or invalid.

**Inner Mechanisms:**
- Prefers `mb_ord` if available.
- Manually decodes UTF-8 bytes for 1-4 byte characters.

**Usage Context:**
- Used when analyzing or processing individual characters in a string.
- Essential for character-level operations.

**Example:**
```php
$char = "é";
$codePoint = utf8_ord($char); // 233
```

---

#### `utf8_substr`

**Purpose:**
Extracts a substring from a UTF-8 string, handling multibyte characters correctly.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string. |
| `$offset` | `int` | The starting position. |
| `$count` | `int|null` | The number of characters to extract. `NULL` extracts to the end. |

**Return Values:**
- `string`: The extracted substring.

**Inner Mechanisms:**
- Prefers `mb_substr` if available.
- Manually calculates byte offsets for multibyte characters.
- Handles negative offsets and counts.

**Usage Context:**
- Used when truncating or extracting parts of multibyte strings.
- Essential for safe substring operations in multilingual applications.

**Example:**
```php
$text = "こんにちは世界";
$substring = utf8_substr($text, 2, 3); // "は世"
```

---

#### `utf8_clean_edges`

**Purpose:**
Removes invalid UTF-8 byte sequences from the start and end of a string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string. |

**Return Values:**
- `string`: The cleaned string.

**Inner Mechanisms:**
- Uses regex to strip invalid leading and trailing bytes.

**Usage Context:**
- Used when processing binary data or strings with potential encoding issues.
- Ensures strings are valid UTF-8 before further processing.

**Example:**
```php
$dirtyString = "\x80Hello\xC2";
$cleanString = utf8_clean_edges($dirtyString); // "Hello"
```

---

#### `utf8_strcut`

**Purpose:**
Cuts a UTF-8 string to a specified byte length while preserving character integrity.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string. |
| `$offset` | `int` | The starting byte position. |
| `$count` | `int|null` | The number of bytes to extract. `NULL` extracts to the end. |

**Return Values:**
- `string`: The cut string with invalid edges cleaned.

**Inner Mechanisms:**
- Uses `substr` to cut the string at the byte level.
- Cleans invalid edges with `utf8_clean_edges`.

**Usage Context:**
- Used when working with byte-limited storage or display constraints.
- Ensures strings remain valid UTF-8 after byte-level truncation.

**Example:**
```php
$text = "こんにちは世界";
$cutText = utf8_strcut($text, 0, 6); // "こん"
```

---

#### `utf8_strlen`

**Purpose:**
Returns the number of characters in a UTF-8 string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string. |

**Return Values:**
- `int`: The number of characters.

**Inner Mechanisms:**
- Prefers `mb_strlen` if available.
- Falls back to regex-based counting.

**Usage Context:**
- Used when determining the length of multibyte strings.
- Essential for validation and display logic.

**Example:**
```php
$text = "こんにちは";
$length = utf8_strlen($text); // 5
```

---

#### `utf8_strtoupper`

**Purpose:**
Converts a UTF-8 string to uppercase.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string. |

**Return Values:**
- `string`: The uppercase string.

**Inner Mechanisms:**
- Prefers `mb_strtoupper` if available.
- Falls back to a static mapping system for case conversion.

**Usage Context:**
- Used for consistent case handling in multilingual applications.
- Essential for user input normalization.

**Example:**
```php
$text = "héllò";
$upperText = utf8_strtoupper($text); // "HÉLLÒ"
```

---

#### `utf8_strtolower`

**Purpose:**
Converts a UTF-8 string to lowercase.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string. |

**Return Values:**
- `string`: The lowercase string.

**Inner Mechanisms:**
- Prefers `mb_strtolower` if available.
- Falls back to a static mapping system for case conversion.

**Usage Context:**
- Used for consistent case handling in multilingual applications.
- Essential for user input normalization.

**Example:**
```php
$text = "HÉLLÒ";
$lowerText = utf8_strtolower($text); // "héllò"
```

---

#### `utf8_ucfirst`

**Purpose:**
Converts the first character of a UTF-8 string to uppercase.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string. |

**Return Values:**
- `string`: The string with the first character uppercase.

**Inner Mechanisms:**
- Uses `utf8_substr` and `utf8_strtoupper` to transform the first character.

**Usage Context:**
- Used for capitalizing names or titles.
- Essential for consistent formatting.

**Example:**
```php
$text = "héllò";
$capitalizedText = utf8_ucfirst($text); // "Héllò"
```

---

#### `utf8_ucwords`

**Purpose:**
Converts the first character of each word in a UTF-8 string to uppercase.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string. |

**Return Values:**
- `string`: The string with each word capitalized.

**Inner Mechanisms:**
- Uses regex to identify word boundaries.
- Applies `utf8_ucfirst` to each word.

**Usage Context:**
- Used for formatting names, titles, or headings.
- Essential for consistent multilingual text formatting.

**Example:**
```php
$text = "héllò wörld";
$capitalizedText = utf8_ucwords($text); // "Héllò Wörld"
```

---

#### `utf8_ltrim`, `utf8_rtrim`, `utf8_trim`

**Purpose:**
Trims whitespace and Unicode space characters from the left, right, or both ends of a UTF-8 string.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string. |

**Return Values:**
- `string`: The trimmed string.

**Inner Mechanisms:**
- Uses regex to match Unicode whitespace characters.

**Usage Context:**
- Used for cleaning user input or formatted text.
- Essential for consistent string handling.

**Example:**
```php
$text = "  héllò  ";
$trimmedText = utf8_trim($text); // "héllò"
```

---

#### `utf8_strcasecmp`

**Purpose:**
Performs a case-insensitive comparison of two UTF-8 strings.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value1` | `string` | The first string. |
| `$value2` | `string` | The second string. |

**Return Values:**
- `int`: Negative if `$value1` is less than `$value2`, positive if greater, and `0` if equal.

**Inner Mechanisms:**
- Converts strings to lowercase using `utf8_strtolower`.
- Uses `strcmp` for comparison.

**Usage Context:**
- Used for case-insensitive string comparisons.
- Essential for sorting and searching multilingual text.

**Example:**
```php
$result = utf8_strcasecmp("Héllò", "héllò"); // 0
```

---

#### `utf8_strnatcasecmp`

**Purpose:**
Performs a case-insensitive natural order comparison of two UTF-8 strings.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value1` | `string` | The first string. |
| `$value2` | `string` | The second string. |

**Return Values:**
- `int`: Negative if `$value1` is less than `$value2`, positive if greater, and `0` if equal.

**Inner Mechanisms:**
- Converts strings to lowercase using `utf8_strtolower`.
- Uses `strnatcmp` for natural order comparison.

**Usage Context:**
- Used for case-insensitive natural order sorting.
- Essential for human-readable sorting of multilingual text.

**Example:**
```php
$result = utf8_strnatcasecmp("file2.txt", "file10.txt"); // -1
```

---

#### `utf8_strspn`

**Purpose:**
Finds the length of the initial segment of a UTF-8 string containing only characters from a specified mask.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string. |
| `$mask` | `string` | The mask of allowed characters. |
| `$start` | `int|null` | The starting position. |
| `$length` | `int|null` | The maximum length to check. |

**Return Values:**
- `int`: The length of the initial segment.

**Inner Mechanisms:**
- Uses `utf8_substr` to extract the relevant segment.
- Uses regex to match the mask.

**Usage Context:**
- Used for validating or parsing strings with specific character sets.
- Essential for input validation.

**Example:**
```php
$text = "123abc";
$length = utf8_strspn($text, "1234567890"); // 3
```

---

#### `utf8_wordwrap`

**Purpose:**
Wraps a UTF-8 string to a specified number of characters per line.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string. |
| `$length` | `int` | The maximum line length. Defaults to `75`. |
| `$break` | `string` | The line break character. Defaults to `"\n"`. |
| `$cut` | `bool` | Whether to cut words longer than `$length`. Defaults to `FALSE`. |

**Return Values:**
- `string`: The wrapped string.

**Inner Mechanisms:**
- Uses regex to split the string at word boundaries or punctuation.
- Handles hard breaks if `$cut` is `TRUE`.

**Usage Context:**
- Used for formatting text for display or storage.
- Essential for multilingual text wrapping.

**Example:**
```php
$text = "This is a long sentence that needs wrapping.";
$wrappedText = utf8_wordwrap($text, 20, "\n", false);
// "This is a long\nsentence that needs\nwrapping."
```

---

#### `utf8_normalize`

**Purpose:**
Normalizes a UTF-8 string to NFC (Normalization Form C) or NFD (Normalization Form D).

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string. |
| `$compose` | `bool` | Whether to compose characters (NFC). Defaults to `TRUE`. |

**Return Values:**
- `string`: The normalized string.

**Inner Mechanisms:**
- Prefers the `Normalizer` class if available.
- Falls back to a custom implementation using decomposition and composition maps.

**Usage Context:**
- Used for consistent string comparison and storage.
- Essential for handling Unicode equivalence.

**Example:**
```php
$text = "é"; // May be "é" (precomposed) or "é" (decomposed)
$normalizedText = utf8_normalize($text); // "é" (NFC)
```

---

## `utf8` Class

The `utf8` class provides advanced Unicode utilities, including normalization, decomposition, and composition of characters. It also includes methods for building and testing Unicode data files.

---

### Methods

---

#### `utf8::build`

**Purpose:**
Builds the necessary data files for UTF-8 normalization, decomposition, and composition from Unicode data files.

**Parameters:**
None.

**Return Values:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Parses Unicode data files (`UnicodeData.txt`, `DerivedNormalizationProps.txt`, etc.).
- Generates PHP include files for canonical combining classes, decomposition/composition mappings, and quick checks.

**Usage Context:**
- Used during platform setup or updates to generate Unicode data files.
- Essential for maintaining up-to-date Unicode support.

**Example:**
```php
if (utf8::build()) {
    echo "Unicode data files built successfully.";
} else {
    echo "Failed to build Unicode data files.";
}
```

---

#### `utf8::test`

**Purpose:**
Tests the normalization implementation against the Unicode Normalization Test file.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$limit` | `int` | The maximum number of test lines to process. `0` for no limit. Defaults to `20000`. |

**Return Values:**
- `bool`: `TRUE` if errors were found, `FALSE` otherwise.

**Inner Mechanisms:**
- Reads the `NormalizationTest.txt` file.
- Compares the platform's normalization results with expected values.

**Usage Context:**
- Used for verifying the correctness of the normalization implementation.
- Essential for quality assurance.

**Example:**
```php
if (utf8::test(1000)) {
    echo "Normalization test failed.";
} else {
    echo "Normalization test passed.";
}
```

---

#### `utf8::get_canonical_combining_class`

**Purpose:**
Returns the canonical combining class of a Unicode character.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The Unicode character. |

**Return Values:**
- `int`: The canonical combining class.

**Inner Mechanisms:**
- Loads the canonical combining class data from an include file.

**Usage Context:**
- Used during normalization and composition processes.
- Essential for handling combining characters.

**Example:**
```php
$char = "́"; // Combining acute accent
$class = utf8::get_canonical_combining_class($char); // 230
```

---

#### `utf8::decompose`

**Purpose:**
Decomposes a Unicode character into its canonical decomposition.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string (passed by reference). |

**Return Values:**
- `string`: The decomposed string.

**Inner Mechanisms:**
- Uses recursive decomposition to handle nested decompositions.

**Usage Context:**
- Used during normalization to prepare strings for composition.
- Essential for handling Unicode equivalence.

**Example:**
```php
$char = "é";
utf8::decompose($char, $decomposed); // "é"
```

---

#### `utf8::sort`

**Purpose:**
Sorts combining characters in a UTF-8 string according to their canonical combining class.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string. |

**Return Values:**
- `string`: The sorted string.

**Inner Mechanisms:**
- Separates starters and non-starters.
- Sorts non-starters by their combining class.

**Usage Context:**
- Used during normalization to ensure canonical order.
- Essential for handling combining characters.

**Example:**
```php
$text = "é"; // "e" + combining acute accent
$sortedText = utf8::sort($text); // "é" (already sorted)
```

---

#### `utf8::compose`

**Purpose:**
Composes a decomposed UTF-8 string into its canonical composition.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string. |

**Return Values:**
- `string`: The composed string.

**Inner Mechanisms:**
- Processes the string character by character.
- Composes characters where possible, handling Hangul syllables and combining marks.

**Usage Context:**
- Used during normalization to produce NFC strings.
- Essential for handling Unicode equivalence.

**Example:**
```php
$text = "é"; // "e" + combining acute accent
$composedText = utf8::compose($text); // "é"
```

---

#### `utf8::get_character`

**Purpose:**
Extracts a single UTF-8 character from a string at a specified offset.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The input string (passed by reference). |
| `$offset` | `int` | The byte offset (passed by reference). |

**Return Values:**
- `string|bool`: The extracted character or `FALSE` if invalid.

**Inner Mechanisms:**
- Uses regex to match valid UTF-8 character patterns.
- Updates the offset to the next character.

**Usage Context:**
- Used internally for character-level processing.
- Essential for handling multibyte strings.

**Example:**
```php
$text = "こんにちは";
$offset = 0;
$char = utf8::get_character($text, $offset); // "こ"
```

---

#### `utf8::ucd_get_record`

**Purpose:**
Reads a record from a Unicode data file.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$hfile` | `resource` | The file handle. |

**Return Values:**
- `array|bool`: The record as an array of fields or `FALSE` on failure.

**Inner Mechanisms:**
- Reads lines from the file.
- Strips comments and splits fields by semicolons.

**Usage Context:**
- Used internally for parsing Unicode data files.
- Essential for building Unicode data files.

**Example:**
```php
$hfile = fopen(CMS_PATH . "unicode/UnicodeData.txt", "rb");
$record = utf8::ucd_get_record($hfile);
fclose($hfile);
```

---

#### `utf8::ucd_extract_code_point`

**Purpose:**
Extracts Unicode code points from a UCD (Unicode Character Database) field.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | The UCD field. |

**Return Values:**
- `array`: An array of UTF-8 characters.

**Inner Mechanisms:**
- Handles both single code points and ranges.
- Converts hexadecimal values to UTF-8 characters.

**Usage Context:**
- Used internally for parsing Unicode data files.
- Essential for building Unicode data files.

**Example:**
```php
$field = "0041..0043";
$chars = utf8::ucd_extract_code_point($field); // ["A", "B", "C"]
```


<!-- HASH:bf442c79bd2a04e728a58b8ecf343d81 -->
