# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/math.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/math.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Math Utilities (`math.inc`)

Core mathematical and formatting utilities for the PWNC Web Platform. Provides functions for percentage calculations, number formatting, byte size conversion, bitmask operations, sign determination, CSS unit conversion, base62 encoding, and string distance measurement.

---

### `diffpercent`

Calculates the percentage difference between two values, returning a formatted string with a percentage sign.

#### Parameters

| Name    | Type    | Description                          |
|---------|---------|--------------------------------------|
| `$value1` | float   | Reference value (must not be zero).  |
| `$value2` | float   | Value to compare against reference.  |

#### Return Values

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| string  | Formatted percentage difference (e.g., `"5.2%"` or `"-3.7%"`).              |
| FALSE   | If `$value1` is zero (invalid reference).                                  |

#### Inner Mechanisms

- Computes relative difference: `(100 / $value1 * $value2)`.
- Normalizes to percentage scale: `100 - (relative difference)`.
- Applies absolute value, then negates if `$value2 < $value1`.
- Formats result with one decimal place using `CMS_L_DECIMAL_SEPARATOR`.

#### Usage Context

Use when displaying growth or decline metrics (e.g., traffic, sales, performance).

#### Example

```php
$lastMonthVisits = 1200;
$thisMonthVisits = 1500;
echo diffpercent($lastMonthVisits, $thisMonthVisits); // Output: "25.0%"
```

---

### `format_number`

Formats a number with locale-aware decimal and thousand separators, omitting decimals for whole numbers.

#### Parameters

| Name    | Type    | Description                          |
|---------|---------|--------------------------------------|
| `$value` | float   | Number to format.                    |

#### Return Values

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| string  | Formatted number (e.g., `"1,234"` or `"1,234.56"`).                        |

#### Inner Mechanisms

- Checks fractional part via `fmod($value, 1)`.
- Uses `number_format` with dynamic decimal count (0 or 2).
- Respects `CMS_L_DECIMAL_SEPARATOR` and `CMS_L_THOUSAND_SEPARATOR`.

#### Usage Context

Use for user-facing numeric displays (e.g., statistics, reports).

#### Example

```php
echo format_number(1234.56); // Output: "1,234.56" (en_US locale)
echo format_number(1234);    // Output: "1,234"
```

---

### `format_bytesize`

Converts a byte count into a human-readable string with appropriate unit (Byte, KB, MB, GB).

#### Parameters

| Name    | Type    | Description                          |
|---------|---------|--------------------------------------|
| `$value` | int     | Byte count.                          |

#### Return Values

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| string  | Formatted size (e.g., `"1.23 MB"`).                                        |

#### Inner Mechanisms

- Checks thresholds: 1024 (KB), 1048576 (MB), 1073741824 (GB).
- Divides by unit factor and formats with 2 decimal places.

#### Usage Context

Use for file sizes, storage metrics, or bandwidth reports.

#### Example

```php
echo format_bytesize(1572864); // Output: "1.50 MB"
```

---

### `flag`

Checks if a specific bit flag is set in a bitmask.

#### Parameters

| Name      | Type    | Description                          |
|-----------|---------|--------------------------------------|
| `$bitmask` | int     | Bitmask to test.                     |
| `$flag`    | int     | Flag to check.                       |

#### Return Values

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| bool    | `TRUE` if flag is set, `FALSE` otherwise.                                  |

#### Inner Mechanisms

- Uses bitwise AND (`&`) to test flag presence.

#### Usage Context

Use for permission checks, feature toggles, or state flags.

#### Example

```php
const PERM_READ = 1;
const PERM_WRITE = 2;
$userPerms = 3; // Read + Write
if (flag($userPerms, PERM_WRITE)) {
    echo "User can write.";
}
```

---

### `sgn`

Determines the sign of a numeric value.

#### Parameters

| Name    | Type    | Description                          |
|---------|---------|--------------------------------------|
| `$value` | float   | Value to evaluate.                   |

#### Return Values

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| int     | `1` (positive), `-1` (negative), or `0` (zero).                            |

#### Inner Mechanisms

- Simple conditional checks.

#### Usage Context

Use for directional logic (e.g., sorting, vector math).

#### Example

```php
$delta = $newValue - $oldValue;
if (sgn($delta) === 1) {
    echo "Increased.";
}
```

---

### `dimension_to_px`

Converts a CSS dimension string (e.g., `"12em"`) into pixels, using static unit conversion factors.

#### Parameters

| Name    | Type    | Description                          |
|---------|---------|--------------------------------------|
| `$value` | string  | CSS dimension (e.g., `"12em"`).      |

#### Return Values

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| int     | Pixel value (rounded).                                                      |
| FALSE   | If unit is invalid or parsing fails.                                       |

#### Inner Mechanisms

- Uses regex to split numeric value and unit.
- Falls back to static unit map (e.g., `1em = 16px`).
- Returns `FALSE` for unrecognized units.

#### Usage Context

Use for dynamic CSS calculations or layout adjustments.

#### Example

```php
echo dimension_to_px("2em"); // Output: 32
```

---

### `base62`

Encodes an integer into a base62 string using alphanumeric characters.

#### Parameters

| Name    | Type    | Description                          |
|---------|---------|--------------------------------------|
| `$value` | int     | Integer to encode.                   |

#### Return Values

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| string  | Base62-encoded string (e.g., `"a3f"`).                                     |

#### Inner Mechanisms

- Uses modulo division to extract digits.
- Maps digits to characters (`0-9`, `a-z`, `A-Z`).

#### Usage Context

Use for compact URL tokens or short hashes.

#### Example

```php
echo base62(123456); // Output: "w7e"
```

---

### `hamming_distance`

Calculates the Hamming distance between two strings (number of differing characters at each position).

#### Parameters

| Name      | Type    | Description                          |
|-----------|---------|--------------------------------------|
| `$string1` | string  | First string.                        |
| `$string2` | string  | Second string.                       |

#### Return Values

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| int     | Number of differing characters.                                            |

#### Inner Mechanisms

- Iterates through each character position.
- Counts mismatches, handling strings of unequal length via null coalescing.

#### Usage Context

Use for fuzzy string matching or error detection.

#### Example

```php
echo hamming_distance("karolin", "kathrin"); // Output: 3
```


<!-- HASH:5ecacee67dd41071fd6c31211bc4952f -->
