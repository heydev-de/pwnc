# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.pop.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.pop.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## POP Class

The `pop` class provides a lightweight, no-dependency implementation for interacting with POP3 (Post Office Protocol version 3) mail servers. It handles connection establishment, authentication, message retrieval, and server communication following the POP3 protocol. The class supports both plaintext and TLS-encrypted connections, APOP and standard USER/PASS authentication, and MIME message parsing through the `mime` library.

### Properties

| Name       | Value/Default | Description                                                                 |
|------------|---------------|-----------------------------------------------------------------------------|
| `hfile`    | `NULL`        | File handle for the socket connection to the POP3 server.                  |
| `response` | `NULL`        | Last raw response received from the POP3 server.                           |
| `enabled`  | `NULL`        | Boolean indicating whether the connection is active and authenticated.     |

---

### `__construct($host, $username, $password)`

#### Purpose
Establishes a connection to a POP3 server, negotiates TLS if available, and authenticates the user. Supports both APOP and standard USER/PASS authentication.

#### Parameters

| Name       | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$host`    | `string` | Hostname or IP address of the POP3 server. If empty, constructor returns early. |
| `$username`| `string` | Username for authentication.                                                |
| `$password`| `string` | Password for authentication.                                                |

#### Return Values
- **`void`**: No explicit return. On success, sets `enabled = TRUE`. On failure, `enabled` remains `FALSE`, and `response` contains an error message.

#### Inner Mechanisms
1. Opens a socket to the server on port 110.
2. Checks for a valid `+OK` server greeting.
3. Attempts TLS upgrade if OpenSSL is available and `STLS` command succeeds.
4. Tries APOP authentication first (using MD5 digest of server challenge + password), falls back to USER/PASS if APOP fails.
5. On failure, closes the connection and sets `enabled = FALSE`.

#### Usage Context
Used to initialize a POP3 session. Typically called once per mailbox access.

#### Example
```php
$pop = new \cms\pop("mail.example.com", "user@example.com", "password");
if (!$pop->enabled) {
    die("Connection failed: " . $pop->response);
}
```

---

### `get_statistics()`

#### Purpose
Retrieves the total number of messages and their combined size in bytes from the mailbox.

#### Parameters
None.

#### Return Values
- **`array|FALSE`**:
  - On success: `["count" => int, "size" => int]` (message count and total size in bytes).
  - On failure: `FALSE`.

#### Inner Mechanisms
Sends the `STAT` command and parses the server response into an associative array.

#### Usage Context
Used to quickly assess mailbox status before fetching messages.

#### Example
```php
$stats = $pop->get_statistics();
if ($stats) {
    echo "Messages: {$stats['count']}, Size: {$stats['size']} bytes";
}
```

---

### `get_list()`

#### Purpose
Retrieves a list of all messages in the mailbox, indexed by message number and containing their individual sizes in bytes.

#### Parameters
None.

#### Return Values
- **`array|FALSE`**:
  - On success: Associative array where keys are message numbers and values are sizes in bytes.
  - On failure: `FALSE`.

#### Inner Mechanisms
Sends the `LIST` command and reads multi-line responses until a termination line (`.\r\n`) is received.

#### Usage Context
Used to enumerate messages and their sizes for batch operations.

#### Example
```php
$list = $pop->get_list();
if ($list) {
    foreach ($list as $index => $size) {
        echo "Message #$index: $size bytes\n";
    }
}
```

---

### `get_unique_id_list()`

#### Purpose
Retrieves a list of all messages with their unique identifiers (UIDs) as assigned by the server.

#### Parameters
None.

#### Return Values
- **`array|FALSE`**:
  - On success: Associative array where keys are message numbers and values are UIDs.
  - On failure: `FALSE`.

#### Inner Mechanisms
Sends the `UIDL` command and parses multi-line responses similar to `get_list()`.

#### Usage Context
Used for tracking messages across sessions (UIDs persist between connections).

#### Example
```php
$uids = $pop->get_unique_id_list();
if ($uids) {
    foreach ($uids as $index => $uid) {
        echo "Message #$index: UID $uid\n";
    }
}
```

---

### `get_header($index)`

#### Purpose
Retrieves the RFC 822 headers of a specific message without downloading the entire body.

#### Parameters

| Name    | Type  | Description                     |
|---------|-------|---------------------------------|
| `$index`| `int` | Message number (1-based index). |

#### Return Values
- **`array|FALSE`**:
  - On success: Associative array of header fields (lowercase keys), with MIME-encoded values decoded.
  - On failure: `FALSE`.

#### Inner Mechanisms
1. Sends `TOP $index 0` to fetch headers only.
2. Uses `receive_header()` to parse and decode headers.
3. Discards any remaining body data.

#### Usage Context
Used to inspect message metadata (subject, from, to, etc.) before downloading.

#### Example
```php
$headers = $pop->get_header(1);
if ($headers) {
    echo "Subject: " . ($headers['subject'] ?? 'No subject');
}
```

---

### `receive_header()`

#### Purpose
Parses raw POP3 header data into a structured associative array, handling MIME encoding (base64, quoted-printable) and header folding.

#### Parameters
None. Operates on the last received response.

#### Return Values
- **`array|NULL`**:
  - On success: Associative array of header fields (lowercase keys), with values decoded and unfolded.
  - On failure or empty: `NULL`.

#### Inner Mechanisms
1. Reads lines until an empty line (`\r\n`) is encountered.
2. Unfolds continued headers (lines starting with whitespace).
3. Decodes MIME-encoded words (e.g., `=?UTF-8?B?...?=`).
4. Converts keys to lowercase for consistency.

#### Usage Context
Internal helper used by `get_header()` and `get_message()`.

---

### `get_message($index)`

#### Purpose
Retrieves a full MIME message, including all parts, and parses it into a structured `data` object.

#### Parameters

| Name    | Type  | Description                     |
|---------|-------|---------------------------------|
| `$index`| `int` | Message number (1-based index). |

#### Return Values
- **`data|FALSE`**:
  - On success: A `data` object containing parsed MIME structure, headers, and decoded bodies.
  - On failure: `FALSE`.

#### Inner Mechanisms
1. Loads the `mime` library for parsing.
2. Sends `RETR $index` to download the full message.
3. Parses MIME boundaries, content types, and transfer encodings.
4. Decodes base64/quoted-printable bodies.
5. Handles nested multipart and message containers.
6. Returns a hierarchical `data` object with `#type`, `#body`, `#size`, and `#boundary` metadata.

#### Usage Context
Used to fetch and parse complete messages, including attachments and nested parts.

#### Example
```php
$message = $pop->get_message(1);
if ($message) {
    echo "From: " . $message->get(1, "from") . "\n";
    echo "Body: " . $message->get(1, "#body");
}
```

---

### `delete($index)`

#### Purpose
Marks a message for deletion from the server. Deletion occurs only after `QUIT` is called.

#### Parameters

| Name    | Type  | Description                     |
|---------|-------|---------------------------------|
| `$index`| `int` | Message number (1-based index). |

#### Return Values
- **`bool`**: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
Sends the `DELE $index` command and checks for a `+OK` response.

#### Usage Context
Used to remove messages from the server after processing.

#### Example
```php
if ($pop->delete(1)) {
    echo "Message marked for deletion";
}
```

---

### `execute($command)`

#### Purpose
Sends a raw POP3 command to the server and reads the response.

#### Parameters

| Name      | Type     | Description                     |
|-----------|----------|---------------------------------|
| `$command`| `string` | POP3 command (e.g., `STAT`, `RETR 1`). |

#### Return Values
- **`bool`**: `TRUE` if the server responds with `+OK`, `FALSE` otherwise.

#### Inner Mechanisms
1. Writes the command to the socket.
2. Calls `receive()` to read the response.
3. Sets `response` to the raw server reply.

#### Usage Context
Internal helper used by all command methods.

---

### `receive($boundary = NULL)`

#### Purpose
Reads a single line from the server and validates it against expected patterns.

#### Parameters

| Name       | Type     | Description                                                                 |
|------------|----------|-----------------------------------------------------------------------------|
| `$boundary`| `string` | Optional MIME boundary string. If provided, checks for boundary termination. |

#### Return Values
- **`bool`**: `TRUE` if the response is valid, `FALSE` on error or termination.

#### Inner Mechanisms
1. Reads a line from the socket.
2. Checks for:
   - End-of-data marker (`.\r\n`).
   - MIME boundary termination (if `$boundary` is provided).
   - Error response (`-ERR`).
3. Sets `response` to the raw line.

#### Usage Context
Internal helper used by `execute()` and message parsing loops.

---

### `quit()`

#### Purpose
Gracefully terminates the POP3 session, committing any pending deletions.

#### Parameters
None.

#### Return Values
- **`bool`**: `TRUE` if the server acknowledges `QUIT`, `FALSE` otherwise.

#### Inner Mechanisms
1. Sends `QUIT` command.
2. Closes the socket.
3. Sets `enabled = FALSE`.

#### Usage Context
Should be called when done with the session to ensure proper cleanup.

#### Example
```php
$pop->quit();
```


<!-- HASH:725a6dfa278a75b5a6368ad10ddf6806 -->
