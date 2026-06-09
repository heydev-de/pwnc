# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/language.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/language.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Language Management Module (`language.inc`)

This file provides core language management utilities for the PWNC Web Platform. It enables multi-language support through string-based language tags, automatic translation extraction, and language detection based on stopwords. The module is designed to be lightweight, flexible, and context-aware.

---

## Constants

| Name                  | Value/Default | Description                                                                 |
|-----------------------|---------------|-----------------------------------------------------------------------------|
| `CMS_LANGUAGE`        | (config)      | Default language code (e.g., `"en"`).                                       |
| `CMS_LANGUAGE_ENABLED`| (config)      | Comma-separated list of enabled languages (e.g., `"en,de,fr"`).             |
| `CMS_LANGUAGE_SEPARATOR` | `"\x1E"`   | Internal separator for multi-language strings.                              |
| `CMS_REGEX_BORDER`    | `"\b"`        | Word boundary regex pattern for stopword matching.                          |

---

## Functions

### `l($text)`

**Purpose:**
Shortcut for `language_get()` to retrieve the current language translation of a multi-language string.

**Parameters:**

| Name  | Type     | Description                          |
|-------|----------|--------------------------------------|
| `$text` | `string` | Multi-language string to translate.  |

**Return Values:**
- `string`: Translated string for the current language, or the default if no translation exists.

**Inner Mechanisms:**
- Delegates to `language_get()` with default parameters.

**Usage Context:**
- Used for quick inline translations in templates or code.

**Example:**
```php
echo l("Hello|de:Hallo|fr:Bonjour"); // Outputs "Hello" (default), "Hallo" if CMS_LANGUAGE="de"
```

---

### `language_get($text, $language = NULL, $explicit = NULL)`

**Purpose:**
Extracts the translation for a specific language from a multi-language string.

**Parameters:**

| Name        | Type      | Description                                                                 |
|-------------|-----------|-----------------------------------------------------------------------------|
| `$text`     | `string`  | Multi-language string (e.g., `"Hello|de:Hallo|fr:Bonjour"`).              |
| `$language` | `string`  | Target language code (e.g., `"de"`). If `NULL`, uses `CMS_LANGUAGE`.       |
| `$explicit` | `bool`    | If `TRUE`, returns `NULL` for missing translations instead of the default. |

**Return Values:**
- `string`: Translated string or default if not found.
- `NULL`: Only if `$explicit=TRUE` and no translation exists.

**Inner Mechanisms:**
1. Extracts the default string (before the first `CMS_LANGUAGE_SEPARATOR`).
2. If `$language` is empty, uses `CMS_LANGUAGE` unless `$explicit=TRUE`.
3. Searches for the pattern `|{language}:{translation}` in the string.
4. Returns the translation if found, otherwise the default (or `NULL` if `$explicit`).

**Usage Context:**
- Core translation logic for multi-language strings.

**Example:**
```php
$text = "Welcome|de:Willkommen|fr:Bienvenue";
echo language_get($text, "fr"); // Outputs "Bienvenue"
echo language_get($text, "es", TRUE); // Outputs NULL
```

---

### `language_get_array($text)`

**Purpose:**
Converts a multi-language string into an associative array of translations.

**Parameters:**

| Name    | Type     | Description                          |
|---------|----------|--------------------------------------|
| `$text` | `string` | Multi-language string.               |

**Return Values:**
- `array`: Keys are language codes, values are translations. The default string is under the empty key `""`.

**Inner Mechanisms:**
1. Splits the string by `CMS_LANGUAGE_SEPARATOR`.
2. Initializes the return array with the default string.
3. Populates enabled languages (from `CMS_LANGUAGE_ENABLED`) with `NULL`.
4. Parses each segment for `{language}:{translation}` pairs.

**Usage Context:**
- Debugging or bulk processing of multi-language strings.

**Example:**
```php
$text = "Submit|de:Senden|fr:Envoyer";
print_r(language_get_array($text));
// Output: ["" => "Submit", "de" => "Senden", "fr" => "Envoyer"]
```

---

### `language_set($text, $value = NULL, $language = NULL)`

**Purpose:**
Updates or adds a translation to a multi-language string.

**Parameters:**

| Name        | Type      | Description                                                                 |
|-------------|-----------|-----------------------------------------------------------------------------|
| `$text`     | `string`  | Original multi-language string.                                             |
| `$value`    | `string`  | New translation value.                                                      |
| `$language` | `string`  | Target language code. If `NULL`, replaces the default string.               |

**Return Values:**
- `string`: Updated multi-language string.

**Inner Mechanisms:**
1. If `$language` is `NULL`, replaces the default string.
2. If `$value` is empty, removes the translation for `$language`.
3. Otherwise, updates or appends the translation for `$language`.

**Usage Context:**
- Dynamic modification of multi-language strings (e.g., user-generated content).

**Example:**
```php
$text = "Save|de:Speichern";
echo language_set($text, "Guardar", "es"); // Output: "Save|de:Speichern|es:Guardar"
echo language_set($text, "Sauvegarder", "fr"); // Output: "Save|de:Speichern|fr:Sauvegarder"
```

---

### `language_set_array($array)`

**Purpose:**
Converts an associative array of translations into a multi-language string.

**Parameters:**

| Name    | Type    | Description                                                                 |
|---------|---------|-----------------------------------------------------------------------------|
| `$array` | `array` | Keys are language codes (empty key for default), values are translations.   |

**Return Values:**
- `string`: Multi-language string.

**Inner Mechanisms:**
1. Iterates over the array, skipping empty values.
2. Prepends `CMS_LANGUAGE_SEPARATOR` and `{language}:` to non-default translations.

**Usage Context:**
- Serializing translations for storage or transmission.

**Example:**
```php
$array = ["" => "Delete", "de" => "Löschen", "fr" => "Supprimer"];
echo language_set_array($array); // Output: "Delete|de:Löschen|fr:Supprimer"
```

---

### `language_detect($text)`

**Purpose:**
Detects the most likely language of a text using stopword matching.

**Parameters:**

| Name    | Type     | Description                          |
|---------|----------|--------------------------------------|
| `$text` | `string` | Input text to analyze.               |

**Return Values:**
- `string`: Detected language code (e.g., `"en"`), or empty string if no match.

**Inner Mechanisms:**
1. Tokenizes the text into lowercase words.
2. Compares words against stopword lists for each language (stored in `#system/language`).
3. Returns the language with the highest stopword match count.

**Usage Context:**
- Automatic language detection for user-generated content.

**Example:**
```php
$text = "This is a sample text with some common English words.";
echo language_detect($text); // Output: "en"
```

---

### `language_strip_stopword($text, $language)`

**Purpose:**
Removes stopwords from a text for a specific language.

**Parameters:**

| Name        | Type     | Description                          |
|-------------|----------|--------------------------------------|
| `$text`     | `string` | Input text.                          |
| `$language` | `string` | Language code (e.g., `"en"`).        |

**Return Values:**
- `string`: Text with stopwords removed.

**Inner Mechanisms:**
1. Fetches stopwords for the language from `#system/language`.
2. Escapes stopwords for regex and constructs a pattern.
3. Uses `preg_replace` to remove stopwords, preserving word boundaries.

**Usage Context:**
- Text preprocessing for search or NLP tasks.

**Example:**
```php
$text = "This is a simple example text.";
echo language_strip_stopword($text, "en"); // Output: "simple example text."
```

---

### `language_name($string)`

**Purpose:**
Retrieves the human-readable name of a language.

**Parameters:**

| Name      | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$string` | `string` | Language code (e.g., `"en"`).        |

**Return Values:**
- `string`: Language name (e.g., `"English"`), or empty string if not found.

**Inner Mechanisms:**
- Queries the `#system/language` data file for the `"name"` field.

**Usage Context:**
- Displaying language names in UIs.

**Example:**
```php
echo language_name("de"); // Output: "German"
```

---

### `t($text)`

**Purpose:**
**Future Feature:** Automatically extracts and manages translatable strings from source code.

**Parameters:**

| Name    | Type     | Description                          |
|---------|----------|--------------------------------------|
| `$text` | `string` | String to translate (e.g., `"Hello"`). |

**Return Values:**
- `string`: Translated string if available, otherwise the original text.

**Inner Mechanisms:**
1. **Static Analysis:** Parses the calling script for `t("...")` calls using `token_get_all()`.
2. **Caching:** Stores extracted strings in `#language/test.language.inc` and per-language files.
3. **Update Detection:** Compares file modification times to detect changes.
4. **Shutdown Handler:** Writes updated translation templates on script termination.

**Usage Context:**
- Planned for automatic translation extraction in development.

**Example:**
```php
// In a template or PHP file:
echo t("Welcome to the platform!"); // Initially outputs "Welcome to the platform!"
// After translation files are populated, outputs the translated string.
```


<!-- HASH:2168c27f921624f72bb0fc2195b4a72e -->
