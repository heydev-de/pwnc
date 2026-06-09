# PWNC API Documentation

[← Index](../README.md) | [`javascript/md5.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/md5.js)

- **Version:** `26.5.30.4`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## MD5 Hashing Utility (`md5.js`)

This file provides a pure JavaScript implementation of the MD5 hashing algorithm. It is used for generating 128-bit (16-byte) hash values, typically rendered as 32-character hexadecimal strings. The implementation is self-contained and does not rely on external libraries, aligning with PWNC's zero-dependency philosophy.

---

### **Functions**

#### ### `md5_hex(num)`
Converts a 32-bit number into an 8-character hexadecimal string.

| Parameter | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `num`     | Number | A 32-bit integer to be converted.    |

**Return Value:**
- `String`: 8-character lowercase hexadecimal representation of `num`.

**Inner Mechanisms:**
- Iterates over each of the 4 bytes in the 32-bit number.
- Extracts the high and low nibbles (4 bits) of each byte using bitwise operations.
- Maps each nibble to its corresponding hexadecimal character via a lookup table.

**Usage Context:**
- Used internally by `md5()` to convert final hash state variables (`a`, `b`, `c`, `d`) into hexadecimal strings.
- Not typically called directly by application code.

**Example:**
```javascript
console.log(md5_hex(255)); // Output: "000000ff"
```

---

#### ### `md5_convert(str)`
Converts an input string into an array of 32-bit integers, padding it according to the MD5 specification.

| Parameter | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `str`     | String | The input string to be converted.    |

**Return Value:**
- `Array[Number]`: An array of 32-bit integers representing the padded message in 512-bit blocks.

**Inner Mechanisms:**
- Calculates the number of 512-bit blocks required to hold the message plus padding.
- Initializes an array of zeros with length equal to the number of 32-bit words in the padded message.
- Fills the array by placing each character's ASCII code into the appropriate 32-bit word, using little-endian byte ordering.
- Appends the mandatory `0x80` bit (1 in the most significant bit of the next byte) and sets the last two words to the message length in bits (64-bit integer, little-endian).

**Usage Context:**
- First step in the MD5 algorithm: prepares the input for processing.
- Called internally by `md5()`.

**Example:**
```javascript
const blocks = md5_convert("abc");
// blocks is an array of 16 32-bit integers representing the padded message
```

---

#### ### `md5_add(a, b)`
Performs 32-bit unsigned addition with wrap-around (modulo 2³²).

| Parameter | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `a`       | Number | First 32-bit operand.                |
| `b`       | Number | Second 32-bit operand.               |

**Return Value:**
- `Number`: The sum of `a` and `b` as a 32-bit unsigned integer.

**Inner Mechanisms:**
- Splits both operands into low and high 16-bit words.
- Adds the low words; if overflow occurs, carries over to the high words.
- Combines the result back into a 32-bit integer.

**Usage Context:**
- Used throughout the MD5 algorithm to update hash state variables.
- Ensures arithmetic remains within 32-bit bounds.

**Example:**
```javascript
const sum = md5_add(0xFFFFFFFF, 1); // Returns 0 (32-bit wrap-around)
```

---

#### ### `md5_shift_bit(num, cnt)`
Performs a circular left shift (rotate) of a 32-bit number by a specified number of bits.

| Parameter | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `num`     | Number | The 32-bit number to rotate.         |
| `cnt`     | Number | Number of bits to shift left.        |

**Return Value:**
- `Number`: The rotated 32-bit number.

**Inner Mechanisms:**
- Uses bitwise left shift (`<<`) and unsigned right shift (`>>>`) to achieve circular rotation.
- Ensures the result remains a 32-bit integer.

**Usage Context:**
- Core operation in MD5 round functions to mix bits during hashing.

**Example:**
```javascript
const rotated = md5_shift_bit(0x80000000, 1); // Returns 0x00000001
```

---

#### ### `md5_cmn(q, a, b, x, s, t)`
Core MD5 round function used by all four main round types (`ff`, `gg`, `hh`, `ii`).

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| `q`       | Number | A function-specific bitwise operation result.    |
| `a`       | Number | Current hash state variable.                     |
| `b`       | Number | Next hash state variable.                        |
| `x`       | Number | Message word from the current block.             |
| `s`       | Number | Number of bits to rotate.                        |
| `t`       | Number | Constant used in the round.                      |

**Return Value:**
- `Number`: Updated value for the `a` state variable.

**Inner Mechanisms:**
- Combines `a`, `q`, `x`, and `t` using `md5_add`.
- Applies a circular left shift by `s` bits.
- Adds the result to `b` and returns.

**Usage Context:**
- Abstracts the common pattern used in all MD5 round functions.
- Not called directly; used internally by `md5_ff`, `md5_gg`, `md5_hh`, and `md5_ii`.

---

#### ### `md5_ff(a, b, c, d, x, s, t)`
First round function of MD5 (Round 1).

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| `a`, `b`, `c`, `d` | Number | Current hash state variables.                    |
| `x`       | Number | Message word from the current block.             |
| `s`       | Number | Number of bits to rotate.                        |
| `t`       | Number | Constant used in the round.                      |

**Return Value:**
- `Number`: Updated value for the `a` state variable.

**Inner Mechanisms:**
- Computes `(b & c) | ((~b) & d)` — a bitwise choice function.
- Passes the result to `md5_cmn` along with other parameters.

**Usage Context:**
- Used in the first 16 steps of each 512-bit block in the MD5 algorithm.

---

#### ### `md5_gg(a, b, c, d, x, s, t)`
Second round function of MD5 (Round 2).

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| `a`, `b`, `c`, `d` | Number | Current hash state variables.                    |
| `x`       | Number | Message word from the current block.             |
| `s`       | Number | Number of bits to rotate.                        |
| `t`       | Number | Constant used in the round.                      |

**Return Value:**
- `Number`: Updated value for the `a` state variable.

**Inner Mechanisms:**
- Computes `(b & d) | (c & (~d))` — a different bitwise choice function.
- Passes the result to `md5_cmn`.

**Usage Context:**
- Used in steps 16–31 of each 512-bit block.

---

#### ### `md5_hh(a, b, c, d, x, s, t)`
Third round function of MD5 (Round 3).

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| `a`, `b`, `c`, `d` | Number | Current hash state variables.                    |
| `x`       | Number | Message word from the current block.             |
| `s`       | Number | Number of bits to rotate.                        |
| `t`       | Number | Constant used in the round.                      |

**Return Value:**
- `Number`: Updated value for the `a` state variable.

**Inner Mechanisms:**
- Computes `b ^ c ^ d` — a bitwise XOR function.
- Passes the result to `md5_cmn`.

**Usage Context:**
- Used in steps 32–47 of each 512-bit block.

---

#### ### `md5_ii(a, b, c, d, x, s, t)`
Fourth round function of MD5 (Round 4).

| Parameter | Type   | Description                                      |
|-----------|--------|--------------------------------------------------|
| `a`, `b`, `c`, `d` | Number | Current hash state variables.                    |
| `x`       | Number | Message word from the current block.             |
| `s`       | Number | Number of bits to rotate.                        |
| `t`       | Number | Constant used in the round.                      |

**Return Value:**
- `Number`: Updated value for the `a` state variable.

**Inner Mechanisms:**
- Computes `c ^ (b | (~d))` — a more complex bitwise function.
- Passes the result to `md5_cmn`.

**Usage Context:**
- Used in steps 48–63 of each 512-bit block.

---

#### ### `md5(str)`
Main function: computes the MD5 hash of a string.

| Parameter | Type   | Description                          |
|-----------|--------|--------------------------------------|
| `str`     | String | The input string to hash.            |

**Return Value:**
- `String`: 32-character lowercase hexadecimal MD5 hash of `str`.

**Inner Mechanisms:**
1. Converts the input string into a padded array of 32-bit integers using `md5_convert`.
2. Initializes the four 32-bit hash state variables (`a`, `b`, `c`, `d`) with standard MD5 constants.
3. Processes each 512-bit block in 16-word chunks:
   - Saves the current state.
   - Applies 64 rounds of transformations using `md5_ff`, `md5_gg`, `md5_hh`, and `md5_ii`.
   - Adds the saved state back to the current state (modulo 2³²).
4. Converts the final state variables into hexadecimal strings using `md5_hex`.
5. Concatenates the four hexadecimal strings to form the final 128-bit hash.

**Usage Context:**
- Primary entry point for generating MD5 hashes in the PWNC platform.
- Used for checksums, password hashing (with salt), data integrity checks, and token generation.

**Example:**
```javascript
const hash = md5("The quick brown fox jumps over the lazy dog");
console.log(hash); // Output: "9e107d9d372bb6826bd81d3542a419d6"
```

**Security Note:**
- MD5 is considered cryptographically broken and unsuitable for security-sensitive applications (e.g., password storage, digital signatures).
- In PWNC, this function is primarily used for non-security purposes (e.g., cache keys, content fingerprinting) or in legacy contexts.
- For security, prefer modern algorithms like SHA-256 or Argon2, which are available in PWNC's server-side utilities.


<!-- HASH:8efacfc686dc9ed4aa5db42e0dc0dd5b -->
