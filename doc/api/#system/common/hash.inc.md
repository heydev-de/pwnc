# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/hash.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/hash.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Hash Utilities (`hash.inc`)

Core hashing utilities for the PWNC Web Platform. This file provides specialized hashing algorithms for fingerprinting, similarity detection, checksum generation, and cryptographic hashing. All functions are namespaced under `cms\`.

---

### `fingerprint`

Generates a **winnowing fingerprint** for content similarity detection. Uses a rolling hash algorithm to extract a sequence of minimal hash values from overlapping k-grams, ensuring that similar strings produce similar fingerprints even with minor modifications.

#### Parameters

| Name                  | Value/Default | Description                                                                                     |
|-----------------------|---------------|-------------------------------------------------------------------------------------------------|
| `$string`             | (string)      | Input string to fingerprint.                                                                    |
| `$guarantee_threshold`| 8             | Minimum number of consecutive characters that must match to guarantee a fingerprint collision. |
| `$noise_threshold`    | 5             | Maximum number of consecutive differing characters tolerated before fingerprint diverges.       |

#### Return Value

- **Type**: `array`
- **Description**: Array of minimal hash values representing the fingerprint. Empty if input is too short.

#### Inner Mechanisms

1. **Parameter Adjustment**: Ensures `$noise_threshold` ≤ `$guarantee_threshold`.
2. **Window & K-gram Sizing**: Computes sliding window size and k-gram size based on thresholds.
3. **Rolling Hash**: Uses polynomial rolling hash (base 257, mod 65537) to compute hash values for each k-gram.
4. **Winnowing**: Maintains a circular buffer of hash values; extracts the rightmost minimum in each window to form the fingerprint.

#### Usage Context

- **Plagiarism Detection**: Compare fingerprints of documents to detect partial or modified copies.
- **Deduplication**: Identify near-duplicate content in large datasets (e.g., user-generated content).
- **Version Tracking**: Detect incremental changes in text-based assets.

#### Example

```php
$text = "The quick brown fox jumps over the lazy dog.";
$fp = \cms\fingerprint($text, 10, 6);
echo "Fingerprint hashes: " . implode(", ", $fp);
// Output: Fingerprint hashes: 12345, 67890, 23456, ... (example values)
```

---

### `hmac_md5`

Generates an **HMAC-MD5** signature for message authentication using a secret key. Follows RFC 2104.

#### Parameters

| Name      | Value/Default | Description                     |
|-----------|---------------|---------------------------------|
| `$string` | (string)      | Message to authenticate.        |
| `$key`    | (string)      | Secret key.                     |

#### Return Value

- **Type**: `string`
- **Description**: 32-character hexadecimal HMAC-MD5 digest.

#### Inner Mechanisms

1. **Key Normalization**: Truncates keys longer than 64 bytes using MD5.
2. **Padding**: Pads key to 64 bytes with null bytes.
3. **Inner/Outer Pads**: Computes `ipad` (0x36) and `opad` (0x5C) XOR masks.
4. **Hashing**: Computes `MD5(opad + MD5(ipad + message))`.

#### Usage Context

- **API Authentication**: Sign requests to prevent tampering.
- **Session Validation**: Verify integrity of session tokens.
- **Secure Cookies**: Ensure cookies have not been altered.

#### Example

```php
$message = "user_id=123&action=update";
$secret = "s3cr3t_k3y";
$signature = \cms\hmac_md5($message, $secret);
echo "HMAC-MD5: " . $signature;
// Output: HMAC-MD5: 9e107d9d372bb6826bd81d3542a419d6
```

---

### `simhash`

Generates a **64-bit SimHash** for text similarity detection. Converts text into a compact binary fingerprint where similar texts have small Hamming distances.

#### Parameters

| Name      | Value/Default | Description                     |
|-----------|---------------|---------------------------------|
| `$string` | (string)      | Input text to hash.             |

#### Return Value

- **Type**: `string`
- **Description**: 64-character binary string (e.g., `"010101...1010"`).

#### Inner Mechanisms

1. **Shingling**: Generates overlapping 16-byte shingles from the input.
2. **MD5 Hashing**: Computes MD5 hash for each shingle.
3. **Bit Histogram**: Maintains a 64-bit histogram; increments/decrements bits based on MD5 hash bits.
4. **Thresholding**: Converts histogram into a 64-bit binary string.

#### Usage Context

- **Near-Duplicate Detection**: Cluster similar documents (e.g., news articles, product descriptions).
- **Spam Filtering**: Identify slightly modified spam messages.
- **Recommendation Systems**: Group similar user-generated content.

#### Example

```php
$text1 = "The cat sat on the mat.";
$text2 = "The cat sat on a mat.";
$hash1 = \cms\simhash($text1);
$hash2 = \cms\simhash($text2);
$distance = substr_count($hash1 ^ $hash2, '1');
echo "Hamming distance: " . $distance;
// Output: Hamming distance: 3 (example)
```

---

### `crc32_base62`

Computes a **CRC32 checksum** and encodes it in **Base62** for URL-safe, compact representation.

#### Parameters

| Name      | Value/Default | Description                     |
|-----------|---------------|---------------------------------|
| `$string` | (string)      | Input string to checksum.       |

#### Return Value

- **Type**: `string`
- **Description**: 6–7 character Base62-encoded CRC32 checksum.

#### Inner Mechanisms

1. **CRC32**: Computes unsigned 32-bit CRC32 checksum.
2. **Base62 Encoding**: Converts integer to Base62 (0-9, A-Z, a-z) for compactness.

#### Usage Context

- **Asset Versioning**: Append checksums to filenames for cache busting.
- **Data Integrity**: Verify file or message integrity in transit.
- **Short Identifiers**: Generate unique, collision-resistant IDs.

#### Example

```php
$file = "logo.png";
$checksum = \cms\crc32_base62($file);
echo "Versioned filename: logo_{$checksum}.png";
// Output: Versioned filename: logo_3fG7h9.png
```

---

### `hash32`

Generates a **128-bit RIPEMD-128** hash. Suitable for non-cryptographic hashing where collision resistance is required.

#### Parameters

| Name      | Value/Default | Description                     |
|-----------|---------------|---------------------------------|
| `$string` | (string)      | Input string to hash.           |
| `$binary` | `FALSE`       | If `TRUE`, returns raw binary output. |

#### Return Value

- **Type**: `string`
- **Description**: 32-character hexadecimal hash (or 16-byte binary if `$binary=TRUE`).

#### Usage Context

- **Database Keys**: Generate unique identifiers for records.
- **Checksums**: Verify data integrity in storage or transmission.
- **Bloom Filters**: Use as a hash function for probabilistic data structures.

#### Example

```php
$data = "user@example.com";
$hash = \cms\hash32($data);
echo "RIPEMD-128: " . $hash;
// Output: RIPEMD-128: 1a79a4d60de6718e8e5b326e338ae533
```

---

### `hash64`

Generates a **256-bit SHA-256** hash. Cryptographically secure for password hashing, digital signatures, and data integrity.

#### Parameters

| Name      | Value/Default | Description                     |
|-----------|---------------|---------------------------------|
| `$string` | (string)      | Input string to hash.           |
| `$binary` | `FALSE`       | If `TRUE`, returns raw binary output. |

#### Return Value

- **Type**: `string`
- **Description**: 64-character hexadecimal hash (or 32-byte binary if `$binary=TRUE`).

#### Usage Context

- **Password Storage**: Hash passwords before storage (use with salt).
- **Digital Signatures**: Verify authenticity of messages.
- **Blockchain**: Generate transaction hashes.

#### Example

```php
$password = "s3cur3_p@ss";
$salt = "random_salt_123";
$hash = \cms\hash64($password . $salt);
echo "SHA-256: " . $hash;
// Output: SHA-256: 5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8
```

---

### `djb2`

Generates a **32-bit DJB2** hash. Fast, non-cryptographic hash function for general-purpose use.

#### Parameters

| Name      | Value/Default | Description                     |
|-----------|---------------|---------------------------------|
| `$string` | (string)      | Input string to hash.           |

#### Return Value

- **Type**: `int`
- **Description**: 32-bit unsigned integer hash value.

#### Inner Mechanisms

1. **Initialization**: Starts with magic number `5381`.
2. **Iteration**: For each character, updates hash using `(hash << 5) + hash + char`.
3. **Masking**: Applies `0xFFFFFFFF` mask to ensure 32-bit result.

#### Usage Context

- **Hash Tables**: Use as a fast hash function for associative arrays.
- **Caching**: Generate cache keys for objects or strings.
- **Load Balancing**: Distribute requests across servers.

#### Example

```php
$key = "session_12345";
$hash = \cms\djb2($key);
$server = $hash % 10;
echo "Route to server: " . $server;
// Output: Route to server: 7 (example)
```


<!-- HASH:88bb6fe3697a619ebb14306d5f0c82e3 -->
