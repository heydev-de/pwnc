# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/codec.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/codec.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Codec Utilities (`codec.inc`)

This file provides core encoding, decoding, and cryptographic functions for the PWNC Web Platform. It includes:

- **Modern encryption** (via libsodium)
- **Legacy RC4 encryption** (for compatibility)
- **Character escaping** for SQL, JavaScript, URLs, and XML
- **Special character encoding/decoding** for safe string transmission
- **HTML entity handling**

All functions are namespaced under `cms\`.

---

## Encryption Functions

### `encrypt($string, $password)`

Encrypts a string using libsodium's authenticated encryption.

| Parameter  | Type     | Description                          |
|------------|----------|--------------------------------------|
| `$string`  | string   | Plaintext to encrypt                 |
| `$password`| string   | Encryption key/password              |

**Return Value:**
- `string`: Base64-encoded ciphertext (salt + nonce + HMAC + encrypted data)
- `NULL`: On failure

**Mechanism:**
1. Generates a random salt and derives a key using Argon2id (interactive parameters)
2. Creates a random nonce
3. Encrypts using XSalsa20-Poly1305
4. Generates HMAC for integrity
5. Combines all components in a structured format

**Usage:**
```php
$encrypted = \cms\encrypt("Sensitive data", "my-secret-key");
```

---

### `decrypt($string, $password)`

Decrypts a string encrypted with `encrypt()`.

| Parameter  | Type     | Description                          |
|------------|----------|--------------------------------------|
| `$string`  | string   | Base64-encoded ciphertext            |
| `$password`| string   | Encryption key/password              |

**Return Value:**
- `string`: Decrypted plaintext
- `NULL`: On failure (invalid password, tampered data, or malformed input)

**Mechanism:**
1. Decodes base64 and splits into components
2. Derives key using the same parameters
3. Verifies HMAC for integrity
4. Decrypts using XSalsa20-Poly1305

**Usage:**
```php
$decrypted = \cms\decrypt($encrypted, "my-secret-key");
if ($decrypted === NULL) {
    // Handle decryption failure
}
```

---

## Legacy Encryption

### `rc4encrypt($string, $password)`

Encrypts/decrypts using the RC4 stream cipher (insecure - use only for legacy compatibility).

| Parameter  | Type     | Description                          |
|------------|----------|--------------------------------------|
| `$string`  | string   | Data to encrypt/decrypt              |
| `$password`| string   | Encryption key                       |

**Return Value:**
- `string`: Encrypted/decrypted data (same function for both operations)
- `NULL`: If password is empty

**Mechanism:**
1. Initializes RC4 state with key scheduling algorithm
2. Generates keystream via pseudo-random generation algorithm
3. XORs input with keystream

**Usage:**
```php
$encrypted = \cms\rc4encrypt("Legacy data", "weak-key");
$decrypted = \cms\rc4decrypt($encrypted, "weak-key");
```

---

### `rc4decrypt($string, $password)`

Alias of `rc4encrypt()` (RC4 is symmetric).

---

## Character Encoding

### `encchr($string)`

Encodes special characters for safe transmission in text-based protocols.

| Parameter | Type   | Description          |
|-----------|--------|----------------------|
| `$string` | string | Input string         |

**Return Value:**
- `string`: String with special characters replaced by `[chrXX]` tokens

**Encoded Characters:**

| Original | Encoded   |
|----------|-----------|
| `\0`     | `[chr0]`  |
| `\n`     | `[chr10]` |
| `\r`     | `[chr13]` |
| `;`      | `[chr59]` |
| `=`      | `[chr61]` |
| `[`      | `[chr91]` |
| `\`      | `[chr92]` |

**Usage:**
```php
$safe = \cms\encchr("key=value;data[0]");
```

---

### `decchr($string)`

Decodes strings encoded with `encchr()`.

| Parameter | Type   | Description          |
|-----------|--------|----------------------|
| `$string` | string | Encoded string       |

**Return Value:**
- `string`: Original string with special characters restored

**Usage:**
```php
$original = \cms\decchr($safe);
```

---

## SQL Escaping

### `sqlesc($value, $escape_backticks = FALSE)`

Recursively escapes values for SQL queries.

| Parameter           | Type    | Description                          |
|---------------------|---------|--------------------------------------|
| `$value`            | mixed   | Value to escape                      |
| `$escape_backticks` | bool    | Whether to escape backticks (for MySQL identifiers) |

**Return Value:**
- `string|array`: Escaped value (same type as input)
- `string`: Empty string for unsupported types

**Handling by Type:**

| Type      | Handling                                                                 |
|-----------|--------------------------------------------------------------------------|
| bool      | Converts to `1` or empty string                                          |
| int/double| Returns as-is                                                            |
| string    | Escapes special characters (see below)                                   |
| array     | Recursively processes each element                                       |
| other     | Returns empty string                                                     |

**String Escaping:**

| Character | Escaped To |
|-----------|------------|
| `\0`      | `\\0`      |
| `\n`      | `\\n`      |
| `\r`      | `\\r`      |
| `\`       | `\\\\`     |
| `'`       | `\\'`      |
| `"`       | `\\"`      |
| `\x1A`    | `\\Z`      |
| `` ` ``   | `` `` ``   | (if `$escape_backticks` is `TRUE`)

**Usage:**
```php
$escaped = \cms\sqlesc("O'Reilly");
$query = "SELECT * FROM books WHERE title = '$escaped'";

$params = \cms\sqlesc(["id" => 1, "name" => "Test"]);
```

---

## JavaScript/JSON Encoding

### `q($string, $escape_closing_tag = TRUE, $binary = FALSE)`

Encodes strings for JavaScript/JSON with UTF-16 support.

| Parameter             | Type    | Description                          |
|-----------------------|---------|--------------------------------------|
| `$string`             | string  | Input string                         |
| `$escape_closing_tag` | bool    | Whether to escape `</` as `<\/`      |
| `$binary`             | bool    | Use binary-safe encoding (no UTF-16) |

**Return Value:**
- `string`: Encoded string

**Encoding Modes:**

1. **Binary Mode** (`$binary = TRUE`):
   - Escapes all non-ASCII characters as `\xXX`
   - Preserves binary data integrity

2. **UTF-16 Mode** (default):
   - Converts UTF-8 to UTF-16 escape sequences (`\uXXXX`)
   - Handles surrogate pairs for characters outside BMP
   - Preserves ASCII characters (except those in escape table)

**Special Escapes:**

| Character | Escape   |
|-----------|----------|
| `\0`      | `\0`     |
| `\t`      | `\t`     |
| `\n`      | `\n`     |
| `\r`      | `\r`     |
| `"`       | `\"`     |
| `'`       | `\'`     |
| `\`       | `\\`     |
| `</`      | `<\/`    | (if `$escape_closing_tag` is `TRUE`)

**Usage:**
```php
$js = \cms\q("Line 1\nLine 2");
echo "<script>var text = \"$js\";</script>";

$binary = \cms\q("\x00\xFF", FALSE, TRUE);
```

---

### `qb($string, $escape_closing_tag = TRUE)`

Alias of `q()` with `$binary = TRUE`.

---

## URL Encoding

### `r($string)`

Alias of PHP's `rawurlencode()`.

| Parameter | Type   | Description          |
|-----------|--------|----------------------|
| `$string` | string | String to encode     |

**Return Value:**
- `string`: URL-encoded string

**Usage:**
```php
$url = "/path/" . \cms\r("file name.txt");
```

---

## XML Encoding

### `x($string)`

Alias of `xmlspecialchars()` for XML escaping.

| Parameter | Type   | Description          |
|-----------|--------|----------------------|
| `$string` | string | String to escape     |

**Return Value:**
- `string`: XML-escaped string

**Usage:**
```php
$xml = "<tag>" . \cms\x("value & data") . "</tag>";
```

---

## Combined Encoders

### `rq($string)`

Combines URL encoding and JavaScript encoding.

**Process:** `rawurlencode()` → `q($string, FALSE)`

**Usage:**
```php
$param = \cms\rq("value with spaces & special chars");
echo "<a href='?q=$param'>Link</a>";
```

---

### `qr($string)`

Combines JavaScript encoding and URL encoding.

**Process:** `q($string, FALSE)` → `rawurlencode()`

**Usage:**
```php
$js = \cms\qr("alert('test')");
echo "<a href='javascript:$js'>Click</a>";
```

---

### `qx($string)`

Combines JavaScript encoding and XML escaping.

**Process:** `q($string, FALSE)` → `xmlspecialchars()`

**Usage:**
```php
$attr = \cms\qx("value with <tags> & 'quotes'");
echo "<div data-value=\"$attr\"></div>";
```

---

### `rx($string)`

Combines URL encoding and XML escaping.

**Process:** `rawurlencode()` → `xmlspecialchars()`

**Usage:**
```php
$url = \cms\rx("file name.txt");
echo "<a href=\"/download/$url\">Download</a>";
```

---

### `qrx($string)`

Combines JavaScript encoding, URL encoding, and XML escaping.

**Process:** `q($string, FALSE)` → `rawurlencode()` → `xmlspecialchars()`

**Usage:**
```php
$param = \cms\qrx("complex & value");
echo "<input value=\"$param\">";
```

---

## XML Utilities

### `xmlspecialchars($string, $decode = NULL)`

Escapes or unescapes XML special characters.

| Parameter | Type    | Description                          |
|-----------|---------|--------------------------------------|
| `$string` | string  | Input string                         |
| `$decode` | bool    | `TRUE` to decode, `FALSE` to encode  |

**Return Value:**
- `string`: Processed string

**Character Mapping:**

| Original | Encoded  |
|----------|----------|
| `"`      | `&quot;` |
| `'`      | `&apos;` |
| `&`      | `&amp;`  |
| `<`      | `&lt;`   |
| `>`      | `&gt;`   |

**Usage:**
```php
$escaped = \cms\xmlspecialchars("<tag>value</tag>");
$original = \cms\xmlspecialchars($escaped, TRUE);
```

---

### `htmlentities_decode($string)`

Decodes HTML entities to UTF-8 characters.

| Parameter | Type   | Description          |
|-----------|--------|----------------------|
| `$string` | string | String with entities |

**Return Value:**
- `string`: Decoded string

**Handling:**
1. Converts numeric entities (`&#xHHHH;` and `&#DDDD;`) to UTF-8
2. Converts named entities to UTF-8 using HTML translation table

**Usage:**
```php
$text = \cms\htmlentities_decode("&lt;tag&gt; &amp; &copy;");
```

---

### `escape($string)`

Percent-encodes all characters in a string.

| Parameter | Type   | Description          |
|-----------|--------|----------------------|
| `$string` | string | Input string         |

**Return Value:**
- `string`: Percent-encoded string (e.g., `a` → `%61`)

**Usage:**
```php
$encoded = \cms\escape("data");
```


<!-- HASH:53db896c9cc83bbe035acc29c1566943 -->
