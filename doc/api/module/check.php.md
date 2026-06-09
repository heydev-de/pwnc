# PWNC API Documentation

[← Index](../README.md) | [`module/check.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/check.php)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Module: `check.php`

### Overview
This module serves as a **bot verification endpoint** for the PWNC Web Platform. Its primary purpose is to:
1. **Detect provisional bots** (identified by `CMS_IPHASH` with status `CMS_LOG_STATUS_BOT_PROVISIONAL` in the database).
2. **Upgrade their status** to `CMS_LOG_STATUS_USER_PROVISIONAL` (human-like) if verified.
3. **Return a 1x1 transparent GIF pixel** with `noindex` headers to avoid search engine indexing.

This is typically used in **honeypot mechanisms** or **bot mitigation strategies** where automated traffic needs to be silently validated or redirected.

---

### Key Components

#### Database Interaction
| Name          | Type       | Description                                                                 |
|---------------|------------|-----------------------------------------------------------------------------|
| `$mysql`      | `mysql`    | PWNC’s MySQL wrapper class instance. Handles connection and query execution. |
| `$query`      | `string`   | SQL query to fetch bot status for the current IP hash (`CMS_IPHASH`).       |
| `$result`     | `resource` | MySQL query result resource.                                                |
| `$resultrow`  | `array`    | Single row from the query result (if found).                                |

#### Constants
| Name                          | Value/Default | Description                                                                 |
|-------------------------------|---------------|-----------------------------------------------------------------------------|
| `CMS_DB_LOG_USER_BOT`         | (DB column)   | Database column storing bot status flags.                                   |
| `CMS_DB_LOG_USER`             | (DB table)    | Database table logging user/bot activity.                                   |
| `CMS_DB_LOG_USER_USERID`      | (DB column)   | Database column storing the user/bot identifier (IP hash).                  |
| `CMS_IPHASH`                  | (Dynamic)     | Hashed IP address of the current requester.                                 |
| `CMS_LOG_STATUS_BOT_PROVISIONAL` | (Integer)  | Status flag for provisional bots (awaiting verification).                   |
| `CMS_LOG_STATUS_USER_PROVISIONAL` | (Integer) | Status flag for verified users (or upgraded bots).                          |

#### Core Logic Flow
1. **Error Suppression**: Temporarily disables PHP error reporting to avoid exposing sensitive details.
2. **Database Connection**: Establishes a connection via `$mysql->connection()`.
3. **Bot Check**:
   - Queries the database for the current IP hash’s bot status.
   - If the status is `CMS_LOG_STATUS_BOT_PROVISIONAL`, upgrades it to `CMS_LOG_STATUS_USER_PROVISIONAL` using the `log` class.
4. **Response**:
   - Clears output buffers to ensure no prior content is sent.
   - Returns a **1x1 transparent GIF** with `noindex` headers to avoid SEO impact.

---

### Functions/Methods

#### Anonymous Function (IIFE)
- **Purpose**: Encapsulates the entire module logic to avoid polluting the global namespace.
- **Parameters**: None.
- **Return Values**: None (exits with a GIF response).
- **Inner Mechanisms**:
  - Uses an **immediately-invoked function expression (IIFE)** to isolate scope.
  - Relies on `set_error_handler` to suppress errors during execution.
  - Employs a `while` loop to ensure the database connection is retried if needed (though the loop breaks after one iteration).
- **Usage Context**:
  - Deployed as a **silent endpoint** (e.g., `/check.gif`) in forms or links to validate bots.
  - Often embedded in `<img>` tags with `display: none` to trigger verification without user interaction.

---

### Usage Example

#### Scenario: Bot Honeypot in a Contact Form
```html
<!-- Contact form with hidden bot trap -->
<form action="/submit" method="POST">
  <input type="text" name="name" placeholder="Your Name" required>
  <input type="email" name="email" placeholder="Your Email" required>
  <!-- Hidden field to trigger bot verification -->
  <img src="/module/check.php" style="display: none;" alt="">
  <button type="submit">Submit</button>
</form>
```

**Explanation**:
1. **Bot Behavior**: Automated scripts may follow the `src` of the hidden `<img>` tag, triggering `/module/check.php`.
2. **Verification**: If the bot’s IP hash is flagged as `CMS_LOG_STATUS_BOT_PROVISIONAL`, its status is upgraded, and the GIF is returned.
3. **Human Behavior**: Humans ignore the hidden image, and the form submits normally.

---

### Error Handling
- **Database Errors**: If the query fails, the script exits silently (no GIF is returned).
- **No Bot Entry**: If no database entry exists for the IP hash, the script exits without action.
- **Output Buffering**: Ensures no prior content (e.g., PHP warnings) interferes with the GIF response.

---

### Dependencies
- **`pwnc.inc`**: Core PWNC include file (provides database wrappers, constants, and utility functions).
- **`mysql` Class**: Handles MySQL connections and queries.
- **`log` Class**: Manages user/bot status updates in the database.


<!-- HASH:0e0da05941df133da475d444b655613f -->
