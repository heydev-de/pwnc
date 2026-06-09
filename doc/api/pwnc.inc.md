# PWNC API Documentation

[← Index](README.md) | [`pwnc.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/pwnc.inc)

- **Version:** `26.6.6.1`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## PWNC Core Initialization (`pwnc.inc`)

This file serves as the central initialization and configuration hub for the PWNC Web Platform. It establishes core system settings, handles error management, input preprocessing, user identification, language selection, library loading, URL generation, caching mechanisms, and background task management.

---

## Namespace Isolation and Overloading

### `constant()`
Overloads PHP's native `constant()` to automatically include the `cms` namespace.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$name`   | `string` | Name of the constant to retrieve.    |

**Return Value:**
- `mixed`: The value of the constant, or `NULL` if it does not exist.

**Inner Mechanisms:**
- Prepends the `cms` namespace to the constant name before resolution.

**Usage Example:**
```php
define("TEST_CONSTANT", 123);
echo constant("TEST_CONSTANT"); // Outputs: 123
```

---

### `define()`
Overloads PHP's native `define()` to automatically include the `cms` namespace.

| Parameter            | Type      | Description                          |
|----------------------|-----------|--------------------------------------|
| `$name`              | `string`  | Name of the constant to define.      |
| `$value`             | `mixed`   | Value of the constant.               |
| `$case_insensitive`  | `bool`    | Whether the constant is case-insensitive. |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Inner Mechanisms:**
- Prepends the `cms` namespace to the constant name before definition.

**Usage Example:**
```php
define("TEST_CONSTANT", 456);
echo TEST_CONSTANT; // Outputs: 456
```

---

### `defined()`
Overloads PHP's native `defined()` to automatically include the `cms` namespace.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$name`   | `string` | Name of the constant to check.       |

**Return Value:**
- `bool`: `TRUE` if the constant exists, `FALSE` otherwise.

**Inner Mechanisms:**
- Prepends the `cms` namespace to the constant name before checking.

**Usage Example:**
```php
if (defined("TEST_CONSTANT")) {
    echo "Constant is defined.";
}
```

---

## Software Information

### Constants

| Name               | Value/Default                          | Description                          |
|--------------------|----------------------------------------|--------------------------------------|
| `CMS_SOFTWARE`     | `"PWNC"`                               | Name of the software.                |
| `CMS_VERSION`      | Content of `version.txt` or `"?"`      | Current version of the software.     |
| `CMS_COPYRIGHT`    | `"© 2026 Patrick Heyer"`               | Copyright notice.                    |
| `CMS_HOMEPAGE`     | `"https://pwnc.it"`                    | Official homepage of the software.   |
| `CMS_IDENTIFIER`   | `"PWNC/1.0 (+https://pwnc.it)"`        | Full software identifier.            |
| `CMS`              | Combination of above constants         | Full software description.           |

---

## Requirements and Resources

### System Requirements
The following checks are performed at runtime:

- **PHP Version:** Requires PHP 7.4.0 or higher.
- **Extensions:** `mysqli`, `pcre`, and `sodium` must be loaded.
- **PCRE UTF-8 Support:** Must be available.

If any requirement is not met, the script terminates with an error message.

### Resource Limits
The following PHP configuration settings are adjusted to minimum values if possible:

| Setting                | Minimum Value |
|------------------------|---------------|
| `max_execution_time`   | `600`         |
| `max_file_uploads`     | `500`         |
| `memory_limit`         | `256M`        |
| `post_max_size`        | `505M`        |
| `upload_max_filesize`  | `500M`        |

---

### `cms_ini_set_minimum()`
Sets a PHP configuration value to a minimum threshold if the current value is lower.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$key`    | `string` | PHP configuration setting name.      |
| `$value`  | `string` | Minimum value to enforce.            |

**Return Value:**
- `mixed`: The new value if successful, `FALSE` on failure.

**Inner Mechanisms:**
- Parses values with optional suffixes (`K`, `M`, `G`).
- Compares the current value with the requested minimum and updates if necessary.

**Usage Example:**
```php
cms_ini_set_minimum("memory_limit", "512M");
```

---

## Initial Settings

### Configuration
- **User Abort:** `ignore_user_abort(TRUE)` prevents script termination on user abort.
- **Output Buffering:** Enabled with `ob_start()`.
- **Timezone:** Defaults to `UTC` unless overridden.
- **File Permissions:** `umask(0002)` sets default permissions to `664` for files and `775` for directories.

---

## Error Handling

### `cms_error()`
Handles and logs PHP errors, warnings, and notices.

| Parameter   | Type      | Description                          |
|-------------|-----------|--------------------------------------|
| `$code`     | `int`     | Error code.                          |
| `$message`  | `string`  | Error message.                       |
| `$path`     | `string`  | File path where the error occurred.  |
| `$line`     | `int`     | Line number where the error occurred.|
| `$display`  | `bool`    | Force display of the error.          |

**Return Value:**
- `bool`: `TRUE` if the error was handled, `FALSE` otherwise.

**Inner Mechanisms:**
- **Silent Mode:** Errors are suppressed if `cms.error.silent` is set.
- **Error Buffering:** Errors are stored in a static array for later output.
- **Error Output:** Errors are displayed in the browser console using JavaScript.
- **Error Logging:** Errors are logged to `data/#log/error.txt` with a maximum size of 500 KB.

**Usage Example:**
```php
cms_error(E_USER_NOTICE, "This is a test notice.");
```

---

### `cms_error_silent()`
Enables or disables silent error handling mode.

| Parameter | Type      | Description                          |
|-----------|-----------|--------------------------------------|
| `$flag`   | `bool`    | `TRUE` to enable silent mode, `FALSE` to disable. |

**Return Value:**
- `bool`: The previous state of silent mode.

**Usage Example:**
```php
cms_error_silent(TRUE); // Suppress error output
```

---

### `cms_shutdown()`
Captures fatal errors and outputs the error buffer on script termination.

**Inner Mechanisms:**
- Checks for fatal errors using `error_get_last()`.
- Calls `cms_error()` to handle fatal errors.
- Outputs the error buffer using `cms_error(-1, "")`.

---

## Input Preprocessing

### `cms_initialize_globals()`
Sanitizes and loads superglobal data (`$_COOKIE`, `$_GET`, `$_POST`, `$_FILES`) into the global scope.

**Inner Mechanisms:**
- Normalizes line breaks and repairs invalid UTF-8 using `cms_utf8_normalize()`.
- Prevents multiple executions by defining `CMS_INITIALIZE_GLOBALS_EXECUTED`.

**Usage Example:**
```php
cms_initialize_globals();
echo $GLOBALS["test"]; // Access sanitized input
```

---

### `cms_utf8_normalize()`
Normalizes line breaks and repairs invalid UTF-8 strings.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$value`  | `mixed`  | Input value (string or array).       |

**Return Value:**
- `mixed`: Normalized string or array.

**Inner Mechanisms:**
- Recursively processes arrays.
- Replaces `\r\n` and `\r` with `\n`.
- Uses `utf8_normalize()` to repair invalid UTF-8.

**Usage Example:**
```php
$normalized = cms_utf8_normalize("Line 1\r\nLine 2");
```

---

## Identification

### `cms_identification()`
Handles user authentication, logout, account expiration, CSRF protection, and brute-force attack prevention.

**Inner Mechanisms:**
- **Logout:** Clears authentication cookies and redirects to the root URL.
- **Login Attempts:** Limits failed login attempts to `CMS_LOGIN_ATTEMPT_MAX` within `CMS_LOGIN_BLOCK_TIME`.
- **CSRF Protection:** Generates and verifies security tokens for non-anonymous users.
- **User Verification:** Validates user credentials using the `permission` class.
- **Account Expiration:** Sets expiration for user accounts if configured.

**Usage Example:**
```php
cms_identification(); // Initialize user authentication
```

---

### `cms_generate_id()`
Generates anonymous fingerprints and IP hashes for the current client.

**Inner Mechanisms:**
- Uses a salted combination of HTTP headers and remote address to generate a unique identifier.
- Defines `CMS_USERID` and `CMS_IPHASH` constants.

**Usage Example:**
```php
cms_generate_id();
echo CMS_USERID; // Outputs: Anonymous user fingerprint
```

---

## Language

### `cms_language()`
Initializes language settings based on user preferences and browser headers.

**Inner Mechanisms:**
- Retrieves the default language from the system configuration.
- Extracts the user's preferred language from `HTTP_ACCEPT_LANGUAGE` if not set.
- Loads the appropriate language file from `data/#language/`.

**Usage Example:**
```php
cms_language(); // Initialize language settings
```

---

### `cms_language_extract()`
Finds the best match for a requested language from a list of supported languages.

| Parameter    | Type     | Description                          |
|--------------|----------|--------------------------------------|
| `$requested` | `string` | RFC 9110 / BCP 47 language string.   |
| `$supported` | `string` | Comma-separated list of supported languages. |

**Return Value:**
- `string|null`: Best matching language or `NULL` if no match is found.

**Inner Mechanisms:**
- Parses the requested language string using `cms_language_parse()`.
- Compares language codes, scripts, regions, and variants to find the best match.

**Usage Example:**
```php
$language = cms_language_extract("en-US,en;q=0.9", "en,fr,de");
echo $language; // Outputs: en
```

---

### `cms_language_parse()`
Parses an RFC 9110 / BCP 47 language string into its components.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$string` | `string` | Language string to parse.            |

**Return Value:**
- `array`: Parsed language components with priorities.

**Component Mapping:**

| Key      | Description                          |
|----------|--------------------------------------|
| `code`   | ISO 639 language code.               |
| `script` | ISO 15924 script subtag.             |
| `region` | ISO 3166-1 alpha-2 or UN M.49 code.  |
| `variant`| Registered language variants.        |
| `q`      | Priority value (0.0 to 1.0).         |

**Usage Example:**
```php
$parsed = cms_language_parse("en-US;q=0.9");
print_r($parsed);
```

---

## Libraries

### `cms_load()`
Loads a library file from the `#system/` directory.

| Parameter         | Type      | Description                          |
|-------------------|-----------|--------------------------------------|
| `$library`        | `string`  | Name of the library to load.         |
| `$exit_on_error`  | `bool`    | Terminate script on failure.         |
| `$test`           | `bool`    | Test availability without loading.   |

**Return Value:**
- `bool`: `TRUE` if the library is loaded or available, `FALSE` otherwise.

**Usage Example:**
```php
if (cms_load("database")) {
    // Library is loaded
}
```

---

### `cms_available()`
Checks if a library is available without loading it.

| Parameter  | Type     | Description                          |
|------------|----------|--------------------------------------|
| `$library` | `string` | Name of the library to check.        |

**Return Value:**
- `bool`: `TRUE` if the library is available, `FALSE` otherwise.

**Usage Example:**
```php
if (cms_available("database")) {
    // Library is available
}
```

---

### `cms_load_system()`
Loads all system libraries from the `#system/` directory.

**Inner Mechanisms:**
- Scans the `#system/` directory for files matching `sys.*.inc`.
- Loads each matching file.

**Usage Example:**
```php
cms_load_system(); // Load all system libraries
```

---

## Applications

### `cms_application()`
Loads an application module or checks permissions.

| Parameter     | Type      | Description                          |
|---------------|-----------|--------------------------------------|
| `$application`| `string`  | Application name to load.            |
| `$instance`   | `string`  | Instance name to use.                |
| `$permission` | `string`  | Permission to check.                 |
| `$user`       | `string`  | User to check permission for.        |

**Return Value:**
- `mixed`: Depends on the context (see below).

**Usage Contexts:**

1. **Application Loading:**
   ```php
   cms_application("blog"); // Loads the blog application
   ```

2. **Permission Checks:**
   ```php
   if (cms_application(NULL, NULL, "edit")) {
       // User has edit permission
   }
   ```

3. **Context Information:**
   ```php
   $app = cms_application(); // Returns current application.instance
   ```

---

### `cms_instance()`
Returns the current application instance.

**Return Value:**
- `string`: Current instance name.

**Usage Example:**
```php
$instance = cms_instance();
```

---

### `cms_permission()`
Checks permissions for the current or specified user.

| Parameter     | Type      | Description                          |
|---------------|-----------|--------------------------------------|
| `$permission` | `string`  | Permission to check.                 |
| `$application`| `bool`    | Check application-level permission.  |
| `$instance`   | `bool`    | Check instance-level permission.     |
| `$user`       | `string`  | User to check permission for.        |

**Return Value:**
- `bool`: `TRUE` if the permission is granted, `FALSE` otherwise.

**Usage Example:**
```php
if (cms_permission("edit")) {
    // User has edit permission
}
```

---

## URL Functions

### `cms_url()`
Generates a URL for the current or specified address with optional parameters.

| Parameter      | Type      | Description                          |
|----------------|-----------|--------------------------------------|
| `$address`     | `mixed`   | URL or parameters to use.            |
| `$param`       | `array`   | Parameters to add/overwrite.         |
| `$omit_param`  | `bool`    | Omit stored parameters.              |

**Return Value:**
- `string|bool`: Generated URL or `FALSE` on failure.

**Usage Example:**
```php
$url = cms_url("blog/post", ["id" => 123]);
echo $url; // Outputs: https://example.com/blog/post?id=123
```

---

### `cms_param()`
Generates a querystring or manages stored parameter values.

| Parameter | Type      | Description                          |
|-----------|-----------|--------------------------------------|
| `$value`  | `mixed`   | Value to set or retrieve.            |
| `$key`    | `mixed`   | Key for value management.            |
| `$omit_token` | `bool` | Omit security token.                 |

**Return Value:**
- `mixed`: Depends on the context (see below).

**Usage Contexts:**

1. **Querystring Generation:**
   ```php
   $querystring = cms_param(["id" => 123]);
   echo $querystring; // Outputs: ?token&id=123
   ```

2. **Value Management:**
   ```php
   cms_param("value", "key"); // Sets key to "value"
   $value = cms_param(NULL, "key"); // Retrieves "value"
   ```

3. **Value Retrieval:**
   ```php
   $all = cms_param(TRUE); // Returns all stored values
   ```

---

### `cms_build_url()`
Builds a valid URL from its components.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$parts`  | `array`  | URL components (e.g., `parse_url()` output). |

**Return Value:**
- `string`: Constructed URL.

**Usage Example:**
```php
$url = cms_build_url([
    "scheme" => "https",
    "host" => "example.com",
    "path" => "/blog/post",
    "query" => "id=123"
]);
echo $url; // Outputs: https://example.com/blog/post?id=123
```

---

## Cache Data Storage

### `cms_cache()`
Manages temporary and permanent cache values.

| Parameter   | Type      | Description                          |
|-------------|-----------|--------------------------------------|
| `$key`      | `string`  | Cache key.                           |
| `$value`    | `mixed`   | Value to store or `""` to delete.    |
| `$permanent`| `bool`    | Store permanently.                   |
| `$notouch`  | `bool`    | Do not update access time.           |

**Return Value:**
- `mixed`: Cached value or `NULL` if not found.

**Usage Example:**
```php
cms_cache("test", "value"); // Store temporarily
$value = cms_cache("test"); // Retrieve "value"
```

---

### `cms_cache_delete()`
Deletes a cache value, optionally from permanent storage.

| Parameter   | Type      | Description                          |
|-------------|-----------|--------------------------------------|
| `$key`      | `mixed`   | Cache key or array of keys.          |
| `$permanent`| `bool`    | Delete from permanent storage.       |

**Return Value:**
- `bool`: `TRUE` if all deletions succeeded, `FALSE` otherwise.

**Usage Example:**
```php
cms_cache_delete("test", TRUE); // Delete permanently
```

---

### `cms_cache_sync()`
Initializes a variable with a cache value or updates the cache with the variable's value.

| Parameter       | Type      | Description                          |
|-----------------|-----------|--------------------------------------|
| `&$variable`    | `mixed`   | Variable to synchronize.             |
| `$key`          | `string`  | Cache key.                           |
| `$default`      | `mixed`   | Default value if cache is empty.     |
| `$load_on_empty`| `bool`    | Load cache if variable is empty.     |

**Usage Example:**
```php
$value = NULL;
cms_cache_sync($value, "test", "default");
echo $value; // Outputs: "default" if cache is empty
```

---

### `cms_cache_init()`
Initializes a variable with a cache value if it is undefined or empty.

| Parameter       | Type      | Description                          |
|-----------------|-----------|--------------------------------------|
| `&$variable`    | `mixed`   | Variable to initialize.              |
| `$key`          | `string`  | Cache key.                           |
| `$load_on_empty`| `bool`    | Load cache if variable is empty.     |

**Usage Example:**
```php
$value = "";
cms_cache_init($value, "test");
echo $value; // Outputs cached value if available
```

---

### `cms_cache_notouch()`
Retrieves a cache value without updating its access time.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$key`    | `string` | Cache key.                           |

**Return Value:**
- `mixed`: Cached value or `NULL` if not found.

**Usage Example:**
```php
$value = cms_cache_notouch("test");
```

---

### `cms_cache_time()`
Returns the last access time of a cache value.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$key`    | `string` | Cache key.                           |

**Return Value:**
- `int|bool`: Unix timestamp or `FALSE` if the cache does not exist.

**Usage Example:**
```php
$time = cms_cache_time("test");
```

---

### `cms_cache_touch()`
Updates the access time of a cache value.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$key`    | `string` | Cache key.                           |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Usage Example:**
```php
cms_cache_touch("test");
```

---

### `cms_cache_clean()`
Removes expired cache files from a directory.

| Parameter | Type      | Description                          |
|-----------|-----------|--------------------------------------|
| `$path`   | `string`  | Path to the cache directory.         |
| `$force`  | `bool`    | Force removal of all files.          |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Usage Example:**
```php
cms_cache_clean(CMS_DATA_PATH . "#cache/");
```

---

## Daemon

### `cms_daemon()`
Adds a background task to the daemon queue.

| Parameter  | Type      | Description                          |
|------------|-----------|--------------------------------------|
| `$code`    | `string`  | PHP code to execute.                 |
| `$id`      | `string`  | Unique task identifier.              |
| `$interval`| `int`     | Minimum interval between executions. |
| `$status`  | `string`  | Status message for the task.         |

**Return Value:**
- `bool`: `TRUE` if the task was queued, `FALSE` otherwise.

**Usage Example:**
```php
cms_daemon(
    "echo 'Task executed';",
    "test_task",
    3600,
    "Test task"
);
```

---

### `cms_daemon_status()`
Sets or retrieves the status of the background worker.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$value`  | `string` | Status message to set.               |

**Return Value:**
- `string|bool`: Current status or `TRUE`/`FALSE` on success/failure.

**Usage Example:**
```php
cms_daemon_status("Processing task...");
$status = cms_daemon_status();
```

---

### `cms_daemon_exists()`
Checks if a task with the given ID is queued.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$id`     | `string` | Task identifier.                     |

**Return Value:**
- `bool`: `TRUE` if the task is queued, `FALSE` otherwise.

**Usage Example:**
```php
if (cms_daemon_exists("test_task")) {
    // Task is queued
}
```

---

### `cms_daemon_run()`
Starts a discrete asynchronous background worker.

**Return Value:**
- `bool`: `TRUE` if the daemon was started, `FALSE` otherwise.

**Usage Example:**
```php
cms_daemon_run();
```

---

## Flag

### `cms_flag_set()`
Sets, retrieves, or deletes a flag.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$key`    | `string` | Flag key.                            |
| `$mode`   | `int`    | `0` to set, `1` to get, `2` to delete. |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Usage Example:**
```php
cms_flag_set("test_flag"); // Set flag
$exists = cms_flag_get("test_flag"); // Check flag
cms_flag_del("test_flag"); // Delete flag
```

---

### `cms_flag_get()`
Checks if a flag exists.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$key`    | `string` | Flag key.                            |

**Return Value:**
- `bool`: `TRUE` if the flag exists, `FALSE` otherwise.

**Usage Example:**
```php
if (cms_flag_get("test_flag")) {
    // Flag exists
}
```

---

### `cms_flag_del()`
Deletes a flag.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$key`    | `string` | Flag key.                            |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Usage Example:**
```php
cms_flag_del("test_flag");
```

---

## Miscellaneous

### `cms_set_cookie()`
Stores or deletes session cookies.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$array`  | `array`  | Associative array of cookie names and values. |

**Return Value:**
- `bool`: `TRUE` on success, `FALSE` on failure.

**Usage Example:**
```php
cms_set_cookie(["test_cookie" => "value"]);
```

---

### `cms_email_agent()`
Initializes the system email address.

**Inner Mechanisms:**
- Retrieves the email address from the system configuration or defaults to `mailagent@domain`.

**Usage Example:**
```php
cms_email_agent();
echo CMS_EMAIL_AGENT; // Outputs: mailagent@example.com
```

---

### `cms_path_urlencode()`
Encodes non-alphanumeric characters in a string for use in file paths.

| Parameter | Type     | Description                          |
|-----------|----------|--------------------------------------|
| `$string` | `string` | String to encode.                    |

**Return Value:**
- `string`: Encoded string.

**Usage Example:**
```php
$encoded = cms_path_urlencode("file name.txt");
echo $encoded; // Outputs: file%20name.txt
```

---

## System Constants

### Paths

| Name                  | Description                          |
|-----------------------|--------------------------------------|
| `CMS_PATH`            | Base path of the PWNC installation.  |
| `CMS_SYSTEM_PATH`     | Path to system libraries.            |
| `CMS_MODULES_PATH`    | Path to application modules.         |
| `CMS_INTERFACE_PATH`  | Path to the interface module.        |
| `CMS_DESKTOP_PATH`    | Path to the desktop module.          |
| `CMS_IMAGES_PATH`     | Path to the images directory.        |
| `CMS_ROOT_PATH`       | Root path of the web server.         |
| `CMS_DATA_PATH`       | Path to the data directory.          |

### URLs

| Name                  | Description                          |
|-----------------------|--------------------------------------|
| `CMS_PROTOCOL`        | Protocol (`http` or `https`).        |
| `CMS_HOST`            | Full host URL.                       |
| `CMS_DOMAIN`          | Domain name.                         |
| `CMS_ACTIVE_URL`      | Current URL.                         |
| `CMS_URL`             | Base URL of the PWNC installation.   |
| `CMS_MODULES_URL`     | URL to application modules.          |
| `CMS_IMAGES_URL`      | URL to the images directory.         |
| `CMS_JAVA_URL`        | URL to the Java directory.           |
| `CMS_JAVASCRIPT_URL`  | URL to the JavaScript directory.     |
| `CMS_SOUNDS_URL`      | URL to the sounds directory.         |
| `CMS_ROOT_URL`        | Root URL of the web server.          |
| `CMS_RELATIVE_URL`    | Relative URL of the PWNC installation. |
| `CMS_DATA_URL`        | URL to the data directory.           |

### Configuration

| Name                  | Description                          |
|-----------------------|--------------------------------------|
| `CMS_APACHE`          | `TRUE` if running on Apache.         |
| `CMS_CACHE_TTL`       | Cache time-to-live in seconds.       |
| `CMS_USER_AGENT`      | User agent string for HTTP requests. |
| `CMS_LOGIN_ATTEMPT_MAX` | Maximum allowed login attempts.    |
| `CMS_LOGIN_BLOCK_TIME` | Block time after failed attempts.   |
| `CMS_PERMISSION_ALWAYS` | Applications always accessible.    |

### Regex Patterns

| Name                  | Description                          |
|-----------------------|--------------------------------------|
| `CMS_REGEX_MATTER`    | Matches valid characters in content. |
| `CMS_REGEX_JOINT`     | Matches valid joint characters.      |
| `CMS_REGEX_SEPARATOR` | Matches valid separators.            |
| `CMS_REGEX_WORD`      | Matches valid words.                 |
| `CMS_REGEX_BORDER`    | Matches valid word borders.          |

### Security Event Types

| Name                  | Description                          |
|-----------------------|--------------------------------------|
| `CMS_SECURITY_EVENT_CSRF` | CSRF security event type.         |

---

## Initialization

### Steps
1. **Load System Libraries:** `cms_load_system()`.
2. **Generate User ID:** `cms_generate_id()`.
3. **Initialize Globals:** `cms_initialize_globals()`.
4. **Initialize Language:** `cms_language()`.
5. **Initialize Email Agent:** `cms_email_agent()`.
6. **User Identification:** `cms_identification()`.
7. **Add Instance to Parameters:** `cms_param(CMS_INSTANCE, "cms_instance")`.

---

## HTML Constants

| Name               | Description                          |
|--------------------|--------------------------------------|
| `CMS_DOCTYPE_HTML` | HTML5 doctype declaration.           |
| `CMS_BOT_CHECK`    | Preload link for bot checking.       |
| `CMS_HTML_HEADER`  | Standard HTML header meta tags.      |
| `CMS_JAVASCRIPT`   | JavaScript file inclusions.          |
| `CMS_STYLESHEET`   | Stylesheet inclusion.                |
| `CMS_CLASS`        | CSS class for the current application. |

---

## HTTP Headers

- **Cache-Control:** Set based on user authentication status.
- **Content-Type:** `text/html; charset=utf-8`.
- **Date:** Current GMT date.
- **Last-Modified:** Current GMT date.
- **X-Generator:** Software identifier.

---

## Migration

### NuOS to PWNC
- Detects if the installation is in a `nuos/` directory.
- Renames `nuos/` to `pwnc/` and regenerates the directory structure.

### Directory Regeneration
- Regenerates the filesystem directory if the `cms.directory.regenerate` flag is set.

---

## Daemon Tasks

### Cache Cleaning
- Runs every hour to clean expired cache files.

### Log Cleaning
- Runs every 5 minutes to maintain log files.

### Search Update Processing
- Runs every 5 minutes to process search updates.

### Search Score Processing
- Runs every hour to compute search scores.

### Daemon Invocation
- `cms_daemon_run()` is called to start background tasks.


<!-- HASH:99ddc5f1898955d66bccdb2b18eeeb0d -->
