# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/url.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/url.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## URL Management Utilities (`url.inc`)

This file provides core URL manipulation utilities for the PWNC Web Platform. It handles:
- **URL translation** – Converting logical identifiers (`content://`, `image://`, etc.) into physical URLs.
- **URL analysis** – Parsing and dissecting URLs into structured components.
- **Path resolution** – Converting relative paths into absolute paths based on a source URL.
- **Query string management** – Merging, altering, and importing query parameters.
- **URL generation** – Building complete URLs with query strings and fragments.

These utilities are context-aware, supporting multilingual content, asset management, and seamless integration with the platform’s routing and state management systems.

---

## Functions

### `translate_url($address, $param = NULL, $language = CMS_LANGUAGE, $omit_cms_param = FALSE)`

#### **Purpose**
Resolves logical URL identifiers (e.g., `content://about`, `image://logo`) into fully qualified, platform-compatible URLs. Supports external HTTP/HTTPS links, content directories, images, media, and downloadable files.

#### **Parameters**

| Name               | Type          | Default          | Description                                                                                     |
|--------------------|---------------|------------------|-------------------------------------------------------------------------------------------------|
| `$address`         | `string`      | —                | Logical or physical URL. Supports schemes: `http`, `https`, `directory`, `content`, `image`, `media`, `download`. |
| `$param`           | `array\|null` | `NULL`           | Additional query parameters to merge into the resolved URL.                                     |
| `$language`        | `string`      | `CMS_LANGUAGE`   | Language code (e.g., `en`, `de`) for content resolution.                                        |
| `$omit_cms_param`  | `bool\|null`  | `FALSE`          | If `TRUE`, omits global CMS parameters (e.g., CSRF tokens) from the generated query string.     |

#### **Return Values**
| Type      | Description                                                                                     |
|-----------|-------------------------------------------------------------------------------------------------|
| `string`  | Fully resolved, absolute URL. Returns the input unchanged if resolution fails or is unnecessary. |

#### **Inner Mechanisms**
1. **Early Exit for Non-Resolvable Schemes**: Returns immediately for `javascript:` or `mailto:` links.
2. **Absolute Path Conversion**: Uses `absolute_path()` to ensure the input is absolute.
3. **URL Analysis**: Uses `analyze_url()` to parse the URL into components (scheme, host, path, query, fragment).
4. **Scheme-Specific Logic**:
   - **HTTP/HTTPS**: Preserves external links; merges parameters with global state via `cms_param()`.
   - **Directory/Content**: Resolves via language-specific `map` objects (`#system/{lang}.directory` or `#system/{lang}.directory.content`). Falls back to `content.php` with parameters if resolution fails.
   - **Image/Media**: Uses `image` or `media` modules to resolve internal filenames or external URLs. Falls back to `no_image` if unavailable.
   - **Download**: Resolves via `download` module; falls back to direct index if needed.
5. **Query String Handling**: Merges input parameters with existing query strings using `array_replace_recursive()`.

#### **Usage Context**
- **Content Management**: Resolving content pages (`content://about`) or directories (`directory://blog`).
- **Asset Delivery**: Generating URLs for images (`image://logo`), media (`media://video`), or downloads (`download://report`).
- **Multilingual Support**: Dynamically resolving content in the user’s language.
- **External Links**: Preserving and parameterizing external URLs while maintaining platform state.

#### **Example**
```php
// Resolve a content page in German
$url = translate_url("content://about", ["highlight" => "team"], "de");
echo $url;
// Output: https://pwnc.it/modules/content.php?content_index=about&highlight=team&cms_language=de

// Resolve an image with fallback
$url = translate_url("image://logo");
echo $url;
// Output: https://pwnc.it/data/image/logo.png (or no_image if missing)
```

---

### `analyze_url($address)`

#### **Purpose**
Parses a URL into a structured associative array, including standard `parse_url()` components and additional path metadata (dirname, basename, filename, extension).

#### **Parameters**

| Name        | Type     | Default | Description                     |
|-------------|----------|---------|---------------------------------|
| `$address`  | `string` | —       | The URL to analyze.             |

#### **Return Values**
| Type               | Description                                                                                     |
|--------------------|-------------------------------------------------------------------------------------------------|
| `array\|false`     | Associative array with keys: `url`, `scheme`, `user`, `pass`, `host`, `port`, `path`, `query`, `fragment`, `relative`, `dirname`, `basename`, `filename`, `extension`. Returns `FALSE` on failure. |

#### **Inner Mechanisms**
1. **Standard Parsing**: Uses PHP’s `parse_url()` to extract base components.
2. **Path Analysis**: Uses `pathinfo()` to split the path into directory, basename, filename, and extension.
3. **Normalization**: Ensures `dirname` uses forward slashes; handles edge cases (e.g., `.` or empty paths).
4. **Merging**: Combines `parse_url()` and `pathinfo()` results into a unified structure.

#### **Usage Context**
- **URL Manipulation**: Required for `translate_url()` and `absolute_path()` to dissect and rebuild URLs.
- **Path Extraction**: Extracting filenames or extensions from asset URLs (e.g., `image://logo.png` → `logo.png`).

#### **Example**
```php
$components = analyze_url("https://pwnc.it/blog/post?page=2#section");
print_r($components);
/*
Output:
[
    "url" => "https://pwnc.it/blog/post?page=2#section",
    "scheme" => "https",
    "host" => "pwnc.it",
    "path" => "/blog/post",
    "query" => "page=2",
    "fragment" => "section",
    "relative" => false,
    "dirname" => "/blog",
    "basename" => "post",
    "filename" => "post",
    "extension" => ""
]
*/
```

---

### `absolute_path($source, $target)`

#### **Purpose**
Converts a relative path (e.g., `../about`) into an absolute path based on a source URL (e.g., `https://pwnc.it/blog/post`).

#### **Parameters**

| Name      | Type     | Default | Description                                                                                     |
|-----------|----------|---------|-------------------------------------------------------------------------------------------------|
| `$source` | `string` | —       | The base URL (must be absolute).                                                                |
| `$target` | `string` | —       | The relative or absolute path to resolve.                                                       |

#### **Return Values**
| Type      | Description                                                                                     |
|-----------|-------------------------------------------------------------------------------------------------|
| `string`  | Absolute URL. Returns `FALSE` if resolution fails (e.g., invalid input or relative source).     |

#### **Inner Mechanisms**
1. **Input Validation**: Uses `analyze_url()` to parse both `$source` and `$target`. Returns `FALSE` if either is invalid.
2. **Absolute Target Handling**: Returns `$target` immediately if it is already absolute.
3. **Path Resolution**:
   - **Root-Relative Paths**: Replaces the source path entirely if `$target` starts with `/`.
   - **Relative Paths**: Splits source and target paths into segments; processes `.` (current directory) and `..` (parent directory) to compute the final path.
4. **Query/Fragments**: Preserves the target’s query string and fragment.

#### **Usage Context**
- **Link Generation**: Resolving relative links in templates or content (e.g., `<a href="../about">`).
- **Asset Paths**: Converting relative asset paths (e.g., `../images/logo.png`) into absolute URLs.

#### **Example**
```php
$base = "https://pwnc.it/blog/post";
$relative = "../about";
$absolute = absolute_path($base, $relative);
echo $absolute;
// Output: https://pwnc.it/about
```

---

### `querystring($param, $alter = NULL)`

#### **Purpose**
Generates a query string from an associative array, optionally merging it with another array. Uses `cms_param()` for state-aware parameter management.

#### **Parameters**

| Name    | Type          | Default | Description                                                                                     |
|---------|---------------|---------|-------------------------------------------------------------------------------------------------|
| `$param`| `array`       | —       | Base parameter array.                                                                           |
| `$alter`| `array\|null` | `NULL`  | Additional parameters to merge into `$param` (recursively).                                     |

#### **Return Values**
| Type      | Description                                                                                     |
|-----------|-------------------------------------------------------------------------------------------------|
| `string`  | Query string (e.g., `?key1=value1&key2=value2`). Returns `FALSE` if `$param` is not an array.   |

#### **Inner Mechanisms**
1. **Input Validation**: Returns `FALSE` if `$param` is not an array.
2. **Parameter Merging**: Uses `array_replace_recursive()` to merge `$alter` into `$param`.
3. **Query String Generation**: Delegates to `cms_param($param, TRUE, TRUE)` to generate the query string.

#### **Usage Context**
- **Dynamic Links**: Generating query strings for pagination, filters, or search parameters.
- **State Management**: Merging local parameters with global state (e.g., language, CSRF tokens).

#### **Example**
```php
$params = ["page" => 2, "filter" => "recent"];
$alter = ["filter" => "popular", "sort" => "asc"];
$query = querystring($params, $alter);
echo $query;
// Output: ?page=2&filter=popular&sort=asc
```

---

### `u($address = NULL, $param = NULL)`

#### **Purpose**
Smart wrapper for `cms_url()` that generates a complete URL from an address and parameters. Handles both path+parameters and parameters-only inputs.

#### **Parameters**

| Name        | Type          | Default          | Description                                                                                     |
|-------------|---------------|------------------|-------------------------------------------------------------------------------------------------|
| `$address`  | `string\|array\|null` | `NULL`      | Target URL or parameter array. If an array, treated as `$param` and `$address` defaults to `CMS_ACTIVE_URL`. |
| `$param`    | `array`       | `NULL`           | Query parameters to append.                                                                     |

#### **Return Values**
| Type      | Description                                                                                     |
|-----------|-------------------------------------------------------------------------------------------------|
| `string`  | Complete URL (e.g., `https://pwnc.it/about?page=2`).                                            |

#### **Inner Mechanisms**
1. **Overload Handling**: If `$address` is an array, treats it as `$param` and sets `$address` to `CMS_ACTIVE_URL`.
2. **Parameter Normalization**: Ensures `$param` is an array; defaults to empty array if `NULL`.
3. **URL Construction**: Concatenates `$address` with the query string from `cms_param($param, TRUE, TRUE)`.

#### **Usage Context**
- **Template Links**: Generating links in HTML templates (e.g., `<a href="<?= u('about', ['ref' => 'nav']) ?>">`).
- **Redirects**: Building redirect URLs with parameters.

#### **Example**
```php
// Link to "about" page with a reference parameter
$url = u("about", ["ref" => "footer"]);
echo $url;
// Output: https://pwnc.it/about?ref=footer

// Current URL with additional parameters
$url = u(["highlight" => "team"]);
echo $url;
// Output: https://pwnc.it/current/page?highlight=team
```

---

### `qu($address = NULL, $param = NULL)`

#### **Purpose**
Generates a URL-encoded string (via `q()`) from the output of `u()`. Useful for embedding URLs in JavaScript or JSON.

#### **Parameters**

| Name        | Type          | Default | Description                     |
|-------------|---------------|---------|---------------------------------|
| `$address`  | `string\|array\|null` | `NULL`  | Passed to `u()`.                |
| `$param`    | `array\|null` | `NULL`  | Passed to `u()`.                |

#### **Return Values**
| Type      | Description                                                                                     |
|-----------|-------------------------------------------------------------------------------------------------|
| `string`  | URL-encoded string (e.g., `"https:\/\/pwnc.it\/about?ref=footer"`).                            |

#### **Inner Mechanisms**
1. **URL Generation**: Calls `u($address, $param)` to generate the URL.
2. **Encoding**: Passes the result to `q()` for JSON/JS-style encoding.

#### **Usage Context**
- **JavaScript Integration**: Embedding URLs in inline scripts or `data-*` attributes.
- **JSON APIs**: Returning URLs in API responses.

#### **Example**
```php
$jsUrl = qu("about", ["ref" => "nav"]);
echo "<a href='#' data-url='$jsUrl'>About</a>";
// Output: <a href='#' data-url='"https:\/\/pwnc.it\/about?ref=nav"'>About</a>
```

---

### `import_querystring($address)`

#### **Purpose**
Parses a URL’s query string and imports its parameters into the global `$_GET` and `$GLOBALS` arrays. Normalizes UTF-8 values using `cms_utf8_normalize()`.

#### **Parameters**

| Name        | Type     | Default | Description                     |
|-------------|----------|---------|---------------------------------|
| `$address`  | `string` | —       | URL containing a query string.  |

#### **Return Values**
| Type      | Description                                                                                     |
|-----------|-------------------------------------------------------------------------------------------------|
| `bool`    | `TRUE` if parameters were imported; `FALSE` if no query string was found.                       |

#### **Inner Mechanisms**
1. **URL Parsing**: Uses `parse_url()` to extract the query string.
2. **Parameter Extraction**: Uses `parse_str()` to convert the query string into an associative array.
3. **Global Injection**: Copies each parameter into `$_GET` and `$GLOBALS`, normalizing values with `cms_utf8_normalize()`.

#### **Usage Context**
- **Legacy Systems**: Importing query parameters from external URLs into the platform’s state.
- **URL Rewriting**: Processing query strings from rewritten URLs (e.g., `/page?param=value`).

#### **Example**
```php
import_querystring("https://pwnc.it/search?q=test&lang=en");
echo $_GET["q"]; // Output: test
echo $lang;      // Output: en (via $GLOBALS)
```


<!-- HASH:f5eed7c2053185f033ae0c34d43fd55d -->
