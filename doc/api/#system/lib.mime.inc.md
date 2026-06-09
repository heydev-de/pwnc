# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.mime.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.mime.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## MIME Handling Utilities

This file provides utilities for parsing and constructing MIME (Multipurpose Internet Mail Extensions) messages. It includes functions for extracting email addresses and headers from RFC 2822 formatted strings, encoding headers, and a `mime` class for building complex MIME messages with support for multipart structures, attachments, and text content.

---

## Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_MIME_TYPE_TEXT` | `"text"` | MIME type for text content. |
| `CMS_MIME_TYPE_IMAGE` | `"image"` | MIME type for image content. |
| `CMS_MIME_TYPE_AUDIO` | `"audio"` | MIME type for audio content. |
| `CMS_MIME_TYPE_VIDEO` | `"video"` | MIME type for video content. |
| `CMS_MIME_TYPE_APPLICATION` | `"application"` | MIME type for application-specific data. |
| `CMS_MIME_TYPE_MESSAGE_RFC822` | `"message/rfc822"` | MIME type for embedded RFC 822 messages. |
| `CMS_MIME_TYPE_MESSAGE_PARTIAL` | `"message/partial"` | MIME type for partial messages. |
| `CMS_MIME_TYPE_MESSAGE_EXTERNAL_BODY` | `"message/external-body"` | MIME type for messages with external body references. |
| `CMS_MIME_CHARSET_UTF_8` | `"utf-8"` | UTF-8 character set. |
| `CMS_MIME_CHARSET_ISO_8859_1` | `"iso-8859-1"` | ISO-8859-1 (Latin-1) character set. |
| `CMS_MIME_CHARSET_ISO_8859_2` | `"iso-8859-2"` | ISO-8859-2 (Latin-2) character set. |
| `CMS_MIME_CHARSET_ISO_8859_3` | `"iso-8859-3"` | ISO-8859-3 (Latin-3) character set. |
| `CMS_MIME_CHARSET_ISO_8859_4` | `"iso-8859-4"` | ISO-8859-4 (Latin-4) character set. |
| `CMS_MIME_CHARSET_ISO_8859_5` | `"iso-8859-5"` | ISO-8859-5 (Cyrillic) character set. |
| `CMS_MIME_CHARSET_ISO_8859_6` | `"iso-8859-6"` | ISO-8859-6 (Arabic) character set. |
| `CMS_MIME_CHARSET_ISO_8859_7` | `"iso-8859-7"` | ISO-8859-7 (Greek) character set. |
| `CMS_MIME_CHARSET_ISO_8859_8` | `"iso-8859-8"` | ISO-8859-8 (Hebrew) character set. |
| `CMS_MIME_CHARSET_ISO_8859_9` | `"iso-8859-9"` | ISO-8859-9 (Latin-5) character set. |
| `CMS_MIME_CHARSET_ISO_8859_10` | `"iso-8859-10"` | ISO-8859-10 (Latin-6) character set. |
| `CMS_MIME_CHARSET_ISO_8859_11` | `"iso-8859-11"` | ISO-8859-11 (Thai) character set. |
| `CMS_MIME_CHARSET_ISO_8859_13` | `"iso-8859-13"` | ISO-8859-13 (Latin-7) character set. |
| `CMS_MIME_CHARSET_ISO_8859_14` | `"iso-8859-14"` | ISO-8859-14 (Latin-8) character set. |
| `CMS_MIME_CHARSET_ISO_8859_15` | `"iso-8859-15"` | ISO-8859-15 (Latin-9) character set. |
| `CMS_MIME_CHARSET_ISO_8859_16` | `"iso-8859-16"` | ISO-8859-16 (Latin-10) character set. |
| `CMS_MIME_CHARSET_US_ASCII` | `"us-ascii"` | US-ASCII character set. |
| `CMS_MIME_ENCODING_7BIT` | `"7bit"` | 7-bit encoding. |
| `CMS_MIME_ENCODING_8BIT` | `"8bit"` | 8-bit encoding. |
| `CMS_MIME_ENCODING_BASE64` | `"base64"` | Base64 encoding. |
| `CMS_MIME_ENCODING_BINARY` | `"binary"` | Binary encoding. |
| `CMS_MIME_ENCODING_QUOTED_PRINTABLE` | `"quoted-printable"` | Quoted-printable encoding. |
| `CMS_MIME_DISPOSITION_INLINE` | `"inline"` | Inline content disposition. |
| `CMS_MIME_DISPOSITION_ATTACHMENT` | `"attachment"` | Attachment content disposition. |

---

## Functions

### `mime_extract_rfc2822_address($string)`

Extracts email addresses and associated names from an RFC 2822 formatted string.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | RFC 2822 formatted string containing email addresses. |

#### Return Values

| Type | Description |
|------|-------------|
| `array` | Associative array where keys are group names (or empty string for no group) and values are associative arrays mapping email addresses to display names. |

#### Inner Mechanisms

1. **Lexer**: Tokenizes the input string, handling comments, quoted strings, address blocks, and delimiters.
2. **Parser**: Processes tokens to extract email addresses, display names, and groups.

#### Usage Context

Used to parse email headers (e.g., `To`, `From`, `Cc`) into structured data for further processing.

#### Example

```php
$header = 'John Doe <john@example.com>, "Jane, Smith" <jane@example.com>';
$addresses = mime_extract_rfc2822_address($header);
// $addresses = [
//     "" => [
//         "john@example.com" => "John Doe",
//         "jane@example.com" => "Jane, Smith"
//     ]
// ];
```

---

### `mime_extract_rfc2822_header($string)`

Extracts key-value pairs from an RFC 2822 header string (e.g., `Content-Type`).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$string` | `string` | RFC 2822 header string. |

#### Return Values

| Type | Description |
|------|-------------|
| `array` | Associative array where keys are parameter names (or `#value` for the main value) and values are the corresponding values. |

#### Inner Mechanisms

1. **Lexer**: Tokenizes the input string, handling quoted strings, parameter delimiters (`;`), and value delimiters (`=`).
2. **Parser**: Processes tokens to extract the main value and parameters.

#### Usage Context

Used to parse MIME headers (e.g., `Content-Type: text/plain; charset="utf-8"`) into structured data.

#### Example

```php
$header = 'text/plain; charset="utf-8"; format=flowed';
$params = mime_extract_rfc2822_header($header);
// $params = [
//     "#value" => "text/plain",
//     "charset" => "utf-8",
//     "format" => "flowed"
// ];
```

---

### `mime_encode_header($value)`

Encodes a header value for safe transmission in MIME messages, handling non-ASCII characters via quoted-printable encoding.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$value` | `string` | Header value to encode. |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | Encoded header value, wrapped at 76 characters. |

#### Inner Mechanisms

1. **ASCII Check**: If the value contains only ASCII characters, it is returned as-is (with word wrapping).
2. **Quoted-Printable Encoding**: Non-ASCII characters are encoded as `=XX` (hexadecimal).
3. **Folding**: The encoded value is folded at 76 characters, breaking at spaces where possible.

#### Usage Context

Used to encode email headers (e.g., `Subject`) containing non-ASCII characters.

#### Example

```php
$subject = 'Hello, 世界!';
$encoded = mime_encode_header($subject);
// $encoded = "=?utf-8?q?Hello=2C_=E4=B8=96=E7=95=8C!?=";
```

---

## Class: `mime`

Handles the construction of MIME messages, including multipart structures, attachments, and text content.

### Properties

| Name | Type | Description |
|------|------|-------------|
| `$data` | `data` | Internal `data` object for storing MIME parts. |

---

### `__construct($name = NULL)`

Initializes a new MIME message.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Optional name for the `data` object. |

#### Inner Mechanisms

Creates a new `data` object to store MIME parts.

---

### `add_text($subject = NULL, $body = NULL, $subtype = "plain", $charset = CMS_MIME_CHARSET_UTF_8, $parent_key = NULL, $content_id = NULL)`

Adds a text part to the MIME message.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$subject` | `string` | Optional subject for the text part. |
| `$body` | `string` | Text content. |
| `$subtype` | `string` | MIME subtype (e.g., `plain`, `html`). Default: `"plain"`. |
| `$charset` | `string` | Character set. Default: `CMS_MIME_CHARSET_UTF_8`. |
| `$parent_key` | `mixed` | Key of the parent part (for nested structures). |
| `$content_id` | `string` | Optional content ID. |

#### Return Values

| Type | Description |
|------|-------------|
| `mixed` | Key of the added part or `FALSE` on failure. |

#### Inner Mechanisms

1. Validates the parent part (if provided).
2. Constructs the `Content-Type` header with the subtype and charset.
3. Sets the `Content-Transfer-Encoding` to `quoted-printable`.
4. Adds the part to the `data` object.

#### Usage Context

Used to add plain text or HTML content to an email.

#### Example

```php
$mime = new mime();
$mime->add_text("Hello", "This is a test email.", "plain");
// Adds a plain text part with the given subject and body.
```

---

### `add_file($file, $filename = NULL, $attachment = TRUE, $parent_key = NULL, $content_id = NULL)`

Adds a file as an attachment or inline part to the MIME message.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$file` | `string` | Path to the file. |
| `$filename` | `string` | Optional filename (defaults to the basename of `$file`). |
| `$attachment` | `bool` | Whether to treat the file as an attachment (`TRUE`) or inline (`FALSE`). Default: `TRUE`. |
| `$parent_key` | `mixed` | Key of the parent part (for nested structures). |
| `$content_id` | `string` | Optional content ID. |

#### Return Values

| Type | Description |
|------|-------------|
| `mixed` | Key of the added part or `FALSE` on failure. |

#### Inner Mechanisms

1. Determines the MIME type of the file.
2. Reads the file content.
3. Sets the `Content-Transfer-Encoding` to `quoted-printable` for text files or `base64` for others.
4. Adds the part to the `data` object.

#### Usage Context

Used to attach files (e.g., PDFs, images) to an email.

#### Example

```php
$mime = new mime();
$mime->add_file("/path/to/document.pdf");
// Attaches the file as an attachment.
```

---

### `add_part($content_type = CMS_MIME_TYPE_TEXT, $subtype = "plain", $subject = NULL, $body = NULL, $content_transfer_encoding = CMS_MIME_ENCODING_QUOTED_PRINTABLE, $charset = CMS_MIME_CHARSET_UTF_8, $content_disposition = CMS_MIME_DISPOSITION_INLINE, $name = NULL, $parent_key = NULL, $content_id = NULL)`

Adds a custom MIME part to the message.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$content_type` | `string` | MIME type (e.g., `text`, `image`). Default: `CMS_MIME_TYPE_TEXT`. |
| `$subtype` | `string` | MIME subtype (e.g., `plain`, `png`). Default: `"plain"`. |
| `$subject` | `string` | Optional subject. |
| `$body` | `string` | Part content. |
| `$content_transfer_encoding` | `string` | Encoding (e.g., `base64`, `quoted-printable`). Default: `CMS_MIME_ENCODING_QUOTED_PRINTABLE`. |
| `$charset` | `string` | Character set. Default: `CMS_MIME_CHARSET_UTF_8`. |
| `$content_disposition` | `string` | Disposition (`inline` or `attachment`). Default: `CMS_MIME_DISPOSITION_INLINE`. |
| `$name` | `string` | Optional name for the part. |
| `$parent_key` | `mixed` | Key of the parent part (for nested structures). |
| `$content_id` | `string` | Optional content ID. |

#### Return Values

| Type | Description |
|------|-------------|
| `mixed` | Key of the added part or `FALSE` on failure. |

#### Inner Mechanisms

1. Validates the parent part (if provided).
2. Constructs the `Content-Type` header with the type, subtype, charset, and name.
3. Sets the `Content-Disposition` header.
4. Generates a content ID if not provided.
5. Adds the part to the `data` object.

#### Usage Context

Used to add custom MIME parts (e.g., embedded images, custom data).

#### Example

```php
$mime = new mime();
$mime->add_part("image", "png", NULL, $image_data, "base64", NULL, "inline", "logo.png");
// Adds an inline PNG image.
```

---

### `add_multipart($subtype = "mixed", $subject = NULL, $parent_key = NULL)`

Adds a multipart container to the MIME message.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$subtype` | `string` | Multipart subtype (e.g., `mixed`, `alternative`). Default: `"mixed"`. |
| `$subject` | `string` | Optional subject. |
| `$parent_key` | `mixed` | Key of the parent part (for nested structures). |

#### Return Values

| Type | Description |
|------|-------------|
| `mixed` | Key of the added container or `FALSE` on failure. |

#### Inner Mechanisms

1. Validates the parent part (if provided).
2. Generates a unique boundary string.
3. Adds the container to the `data` object.

#### Usage Context

Used to create nested MIME structures (e.g., emails with both HTML and plain text parts).

#### Example

```php
$mime = new mime();
$container = $mime->add_multipart("alternative");
// Adds a multipart/alternative container.
```

---

### `add_message($subtype = "rfc822", $data = NULL, $parent_key = NULL)`

Adds an embedded message (e.g., forwarded email) to the MIME message.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$subtype` | `string` | Message subtype (e.g., `rfc822`). Default: `"rfc822"`. |
| `$data` | `mime` | `mime` object representing the embedded message. |
| `$parent_key` | `mixed` | Key of the parent part (for nested structures). |

#### Return Values

| Type | Description |
|------|-------------|
| `mixed` | Key of the added message or `FALSE` on failure. |

#### Inner Mechanisms

1. Validates the parent part (if provided).
2. Merges the embedded message data into the current message.
3. Adds the message to the `data` object.

#### Usage Context

Used to embed forwarded emails or other messages.

#### Example

```php
$forwarded = new mime();
$forwarded->add_text("Original Subject", "Original Body");
$mime = new mime();
$mime->add_message("rfc822", $forwarded);
// Embeds the forwarded message.
```

---

### `build($from = NULL, $to = NULL, $cc = NULL, $bcc = NULL, $reply_to = NULL)`

Constructs the final MIME message as a string.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$from` | `string` | Sender address. |
| `$to` | `string` | Recipient address. |
| `$cc` | `string` | Carbon copy recipients. |
| `$bcc` | `string` | Blind carbon copy recipients. |
| `$reply_to` | `string` | Reply-to address. |

#### Return Values

| Type | Description |
|------|-------------|
| `string` | The constructed MIME message. |

#### Inner Mechanisms

1. Generates a unique `Message-ID`.
2. Sets standard headers (`From`, `To`, `Cc`, `Bcc`, `Reply-To`, `Date`, `MIME-Version`, `X-Mailer`).
3. Processes each part, encoding the body according to its `Content-Transfer-Encoding`.
4. Handles multipart boundaries and nesting.
5. Increments the message count for the current process.

#### Usage Context

Used to finalize the MIME message before sending or saving.

#### Example

```php
$mime = new mime();
$mime->add_text("Hello", "This is a test email.");
$message = $mime->build("sender@example.com", "recipient@example.com");
// $message contains the full MIME message.
```

---

### `save($name = NULL)`

Saves the MIME message data to a file.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$name` | `string` | Optional filename. |

#### Inner Mechanisms

Delegates to the `data` object's `save` method.

---

### `content_id()`

Generates a unique content ID for MIME parts.

#### Return Values

| Type | Description |
|------|-------------|
| `string` | A unique content ID in the format `<part.message.pid.timestamp@domain>`. |

#### Inner Mechanisms

1. Counts the number of parts in the current message.
2. Uses the message count, process ID, timestamp, and domain to generate a unique ID.

#### Usage Context

Used internally to generate `Content-ID` headers for MIME parts.


<!-- HASH:abb65657c0a91fe2ae65e533ea2bd003 -->
