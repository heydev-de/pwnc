# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.http.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.http.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## HTTP Functions

This file provides low-level HTTP client functionality for the PWNC Web Platform. It includes functions for making HTTP requests (GET, POST, HEAD), handling timeouts, chunked data transfer, and response parsing. These utilities are designed for high performance and reliability in server-to-server communication.

---

### Constants

| Name                     | Value/Default | Description                                                                 |
|--------------------------|---------------|-----------------------------------------------------------------------------|
| `CMS_HTTP_TIMEOUT_TOTAL` | `10`          | Total timeout in seconds for HTTP operations.                              |
| `CMS_HTTP_TIMEOUT_CHUNK` | `3`           | Timeout in seconds for individual data chunks during transfer.             |
| `CMS_HTTP_SIZE_CHUNK`    | `524289`      | Maximum size in bytes for a single data chunk (512 KB + 1 byte).           |
| `CMS_HTTP_LIMIT`         | `1048576`     | Maximum allowed response size in bytes (1 MB).                             |

---

### `http_fopen`

#### Purpose
Establishes an HTTP connection to a remote server and sends a request (GET, POST, or HEAD). Returns a file handle for reading the response or an array of headers if `get_header_only` is `TRUE`.

#### Parameters

| Name               | Type      | Description                                                                 |
|--------------------|-----------|-----------------------------------------------------------------------------|
| `$url`             | `string`  | The full URL to connect to (e.g., `https://example.com/path?query=value`).  |
| `$get_header_only` | `bool`    | If `TRUE`, returns only the HTTP headers as an associative array.          |
| `$post_data`       | `array`   | Associative array of POST data (name-value pairs). Only used if method is POST. |

#### Return Values
- `resource|array|FALSE`:
  - On success: A file handle (for GET/POST) or an associative array of headers (for HEAD).
  - On failure: `FALSE`.

#### Inner Mechanisms
1. Parses the URL using `analyze_url()` to extract components (scheme, host, path, query, etc.).
2. Validates the scheme (`http` or `https` only).
3. Opens a socket connection to the remote host with appropriate transport (`tcp://` or `ssl://`).
4. Constructs the HTTP request with headers (Host, User-Agent, Authorization if credentials are provided).
5. Sends the request using `http_send()`.
6. Reads the response headers using `http_fetch_header()`.
7. Returns either the headers (for HEAD requests) or the file handle for further reading.

#### Usage Context
- Used internally by `http_header()` and `http_post()`.
- Suitable for custom HTTP clients requiring fine-grained control over requests and responses.

#### Example
```php
// Fetch headers from a remote URL
$headers = http_fopen("https://api.example.com/data", TRUE);
if ($headers !== FALSE) {
    echo "HTTP Status: " . $headers["#status"] . "\n";
    echo "Content-Type: " . ($headers["content-type"] ?? "unknown") . "\n";
}
```

---

### `http_send`

#### Purpose
Sends data over an open socket connection in chunks, respecting timeouts and blocking modes.

#### Parameters

| Name     | Type       | Description                          |
|----------|------------|--------------------------------------|
| `$hfile` | `resource` | Open socket file handle.             |
| `$data`  | `string`   | Data to send.                        |

#### Return Values
- `bool`: `TRUE` on success, `FALSE` on failure (timeout or write error).

#### Inner Mechanisms
1. Sets the socket to non-blocking mode.
2. Sends data in chunks, retrying on partial writes.
3. Enforces a per-chunk timeout (`CMS_HTTP_TIMEOUT_CHUNK`).
4. Restores blocking mode after completion.

#### Usage Context
- Used internally by `http_fopen()` to send the HTTP request.

---

### `http_fetch_header`

#### Purpose
Reads a single HTTP header line from an open socket connection.

#### Parameters

| Name     | Type       | Description                          |
|----------|------------|--------------------------------------|
| `$hfile` | `resource` | Open socket file handle.             |

#### Return Values
- `string|FALSE`:
  - On success: A trimmed header line (e.g., `Content-Type: text/html`).
  - On empty line: `""` (end of headers).
  - On failure: `FALSE`.

#### Inner Mechanisms
1. Sets the socket to non-blocking mode.
2. Reads lines using `stream_get_line()`.
3. Enforces a per-chunk timeout (`CMS_HTTP_TIMEOUT_CHUNK`).
4. Returns `""` for empty lines (end of headers).

#### Usage Context
- Used internally by `http_fopen()` to parse response headers.

---

### `http_fetch_data`

#### Purpose
Reads the response body from an open socket connection, enforcing size limits and timeouts.

#### Parameters

| Name     | Type       | Description                          |
|----------|------------|--------------------------------------|
| `$hfile` | `resource` | Open socket file handle.             |

#### Return Values
- `string|FALSE`:
  - On success: The response body (up to `CMS_HTTP_LIMIT` bytes).
  - On failure: `FALSE`.

#### Inner Mechanisms
1. Sets the socket to non-blocking mode.
2. Reads data in chunks (`CMS_HTTP_SIZE_CHUNK`).
3. Enforces total and per-chunk timeouts.
4. Closes the socket and returns the accumulated data.

#### Usage Context
- Used internally by `http_post()` to fetch the response body.

#### Example
```php
// Fetch raw data from a URL
$hfile = http_fopen("https://example.com/data", FALSE);
if ($hfile !== FALSE) {
    $data = http_fetch_data($hfile);
    if ($data !== FALSE) {
        echo "Received " . strlen($data) . " bytes.\n";
    }
}
```

---

### `http_header`

#### Purpose
Fetches HTTP headers from a remote URL as an associative array.

#### Parameters

| Name   | Type     | Description                          |
|--------|----------|--------------------------------------|
| `$url` | `string` | The URL to fetch headers from.       |

#### Return Values
- `array|FALSE`:
  - On success: Associative array of headers (e.g., `["content-type" => "text/html"]`).
  - On failure: `FALSE`.

#### Inner Mechanisms
- Wrapper for `http_fopen($url, TRUE)`.

#### Usage Context
- Useful for checking resource metadata (e.g., Content-Type, Last-Modified) without downloading the body.

#### Example
```php
$headers = http_header("https://example.com");
if ($headers !== FALSE && $headers["#status"] == 200) {
    echo "Resource is available.\n";
}
```

---

### `http_post`

#### Purpose
Sends a POST request with form data to a remote URL and returns the response body.

#### Parameters

| Name    | Type     | Description                          |
|---------|----------|--------------------------------------|
| `$url`  | `string` | The URL to send the POST request to. |
| `$data` | `array`  | Associative array of POST data.      |

#### Return Values
- `string|FALSE`:
  - On success: The response body.
  - On failure: `FALSE`.

#### Inner Mechanisms
1. Validates `$data` is an array.
2. Calls `http_fopen()` with `$post_data` to establish the connection.
3. Uses `http_fetch_data()` to read the response.

#### Usage Context
- Ideal for submitting form data to APIs or remote endpoints.

#### Example
```php
$response = http_post("https://api.example.com/submit", [
    "name" => "John Doe",
    "email" => "john@example.com"
]);
if ($response !== FALSE) {
    echo "Server replied: " . $response . "\n";
}
```


<!-- HASH:d699f505db4c4a6271cef6b230353ec3 -->
