# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.smtp.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.smtp.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## SMTP Email Handling in PWNC

This file provides SMTP email functionality for the PWNC Web Platform. It includes:
- A high-level `smtp_send()` function for quick email dispatch
- A low-level `smtp` class for full SMTP protocol control

The implementation supports:
- Plaintext and HTML emails (with automatic plaintext fallback)
- Multiple authentication methods (CRAM-MD5, LOGIN, PLAIN, ANONYMOUS)
- STARTTLS encryption
- Both direct SMTP and PHP's `mail()` function
- MIME message construction via the `mime` library

---

## Functions

### `smtp_send()`

Sends an email via SMTP with minimal configuration.

#### Parameters

| Name       | Type    | Description                                                                 |
|------------|---------|-----------------------------------------------------------------------------|
| `$to`      | string  | Recipient email address(es), comma-separated                                |
| `$subject` | string  | Email subject                                                               |
| `$body`    | string  | Email body content                                                          |
| `$html`    | bool    | Whether the body is HTML (default: `FALSE`)                                 |
| `$reply_to`| string\|`NULL`\|`FALSE` | Reply-to address. `NULL` uses system default, `FALSE` omits the header |

#### Return Values

| Type    | Description                          |
|---------|--------------------------------------|
| bool    | `TRUE` on success, `FALSE` on failure |

#### Inner Mechanisms
1. Loads the `mime` library for message construction
2. Creates either:
   - A multipart/alternative MIME message (HTML + plaintext fallback) if `$html=TRUE`
   - A simple plaintext MIME message if `$html=FALSE`
3. Initializes SMTP connection using system configuration
4. Handles `reply_to` address resolution:
   - `NULL`: Uses system default from `system()->getval("email", "reply_to")`
   - `FALSE`: Omits the Reply-To header
5. Delegates sending to the `smtp` class
6. Ensures proper connection cleanup

#### Usage Example
```php
// Send a simple plaintext email
$smtp_send(
    "user@example.com",
    "Welcome to PWNC",
    "Your account has been created successfully."
);

// Send an HTML email with custom reply-to
$smtp_send(
    "user@example.com",
    "Your Weekly Update",
    "<h1>Newsletter</h1><p>Here's what's new this week...</p>",
    TRUE,
    "newsletter@pwnc.it"
);
```

---

## Class: `smtp`

Handles direct SMTP protocol communication.

### Properties

| Name         | Default | Description                                  |
|--------------|---------|----------------------------------------------|
| `$hfile`     | `NULL`  | File handle for the SMTP socket connection   |
| `$enabled`   | `NULL`  | Whether the SMTP connection is active        |
| `$mail`      | `NULL`  | Whether to use PHP's `mail()` function       |
| `$username`  | `NULL`  | SMTP username                                |
| `$password`  | `NULL`  | SMTP password                                |
| `$response`  | `NULL`  | Last SMTP server response                    |

---

### `__construct()`

Initializes an SMTP connection.

#### Parameters

| Name         | Type    | Description                                  |
|--------------|---------|----------------------------------------------|
| `$host`      | string\|`NULL` | SMTP server host[:port]. `NULL` uses system config |
| `$username`  | string\|`NULL` | SMTP username. `NULL` uses system config     |
| `$password`  | string\|`NULL` | SMTP password. `NULL` uses system config     |

#### Return Values
None (constructor)

#### Inner Mechanisms
1. Checks if the system is configured to use PHP's `mail()` function
2. Falls back to system SMTP configuration if no parameters are provided
3. Validates the host format (supports optional port via `:port` syntax)
4. Establishes a socket connection to the SMTP server
5. Performs initial handshake and authentication via `authenticate()`
6. Disables the connection if any step fails

#### Usage Example
```php
// Initialize with system configuration
$smtp = new smtp();

// Initialize with custom server
$smtp = new smtp("smtp.example.com:587", "user", "password");
if (!$smtp->enabled) {
    error_log("SMTP connection failed: " . $smtp->response);
}
```

---

### `authenticate()`

Handles SMTP authentication.

#### Parameters
None

#### Return Values

| Type    | Description                          |
|---------|--------------------------------------|
| bool    | `TRUE` if authentication succeeded, `FALSE` otherwise |

#### Inner Mechanisms
1. Skips authentication if using PHP's `mail()` function
2. Attempts EHLO/HELO handshake
3. Negotiates STARTTLS if supported and OpenSSL is available
4. Attempts authentication methods in order of preference:
   - CRAM-MD5 (challenge-response)
   - LOGIN (base64-encoded username/password)
   - PLAIN (null-byte separated credentials)
   - ANONYMOUS (no authentication)
5. Returns `TRUE` if any method succeeds or if no authentication is required

---

### `send()`

Sends a MIME-formatted email via SMTP.

#### Parameters

| Name       | Type    | Description                                                                 |
|------------|---------|-----------------------------------------------------------------------------|
| `&$mime`   | `mime`  | MIME message object (from the `mime` library)                              |
| `$from`    | string\|`NULL` | Sender address. `NULL` uses MIME header value                              |
| `$to`      | string\|`NULL` | Recipient address(es). `NULL` uses MIME header value                       |
| `$cc`      | string\|`NULL` | CC address(es). `NULL` uses MIME header value                              |
| `$bcc`     | string\|`NULL` | BCC address(es). `NULL` uses MIME header value                             |
| `$reply_to`| string\|`NULL` | Reply-to address. `NULL` uses MIME header value                            |

#### Return Values

| Type    | Description                          |
|---------|--------------------------------------|
| bool    | `TRUE` on success, `FALSE` on failure |

#### Inner Mechanisms
1. **PHP `mail()` Fallback**:
   - Extracts subject, recipient, and sender from MIME headers
   - Splits message into headers and body
   - Uses `mail()` with `-f` parameter for sender enforcement
2. **Direct SMTP**:
   - Initiates SMTP transaction with `MAIL FROM`
   - Adds recipients via `RCPT TO` (handles multiple recipients)
   - Sends message data via `DATA` command
   - Escapes lines starting with `.` to prevent premature termination
3. Handles both single and multiple recipients (comma-separated)

#### Usage Example
```php
// Create a MIME message
$mime = new mime();
$key = $mime->add_multipart("mixed", "Test Email");
$mime->add_text("Plaintext body", NULL, "plain", CMS_MIME_CHARSET_UTF_8, $key);
$mime->add_attachment("/path/to/file.pdf", "application/pdf", $key);

// Send via SMTP
$smtp = new smtp();
if ($smtp->send($mime, "sender@pwnc.it", "recipient@example.com")) {
    echo "Email sent successfully!";
} else {
    echo "Failed to send email: " . $smtp->response;
}
```

---

### `receive_line()`

Reads a line from the SMTP server.

#### Parameters
None

#### Return Values

| Type       | Description                          |
|------------|--------------------------------------|
| array\|`FALSE` | `[code, message]` on success, `FALSE` on failure |

#### Inner Mechanisms
1. Reads data from the socket in chunks of 1024 bytes
2. Parses SMTP response format: `###-Message` (continuation) or `### Message` (final)
3. Aggregates multi-line responses
4. Returns the SMTP status code and full message

---

### `send_line()`

Sends a command to the SMTP server and verifies the response.

#### Parameters

| Name       | Type    | Description                                  |
|------------|---------|----------------------------------------------|
| `$code`    | int     | Expected SMTP status code                    |
| `$message` | string  | Command to send                              |

#### Return Values

| Type       | Description                          |
|------------|--------------------------------------|
| string\|bool | Server response message on success, `FALSE` on failure |

#### Inner Mechanisms
1. Sends the command followed by CRLF
2. Reads the server response via `receive_line()`
3. Verifies the status code matches the expected value
4. Returns the response message or `TRUE` if no message body

---

### `quit()`

Closes the SMTP connection gracefully.

#### Parameters
None

#### Return Values

| Type    | Description                          |
|---------|--------------------------------------|
| bool    | Always returns `TRUE`                |

#### Inner Mechanisms
1. Sends `QUIT` command if the connection is active
2. Closes the socket handle
3. Marks the connection as disabled
4. Safe to call multiple times


<!-- HASH:121c6e75e302ad769085d8c1834a97b4 -->
