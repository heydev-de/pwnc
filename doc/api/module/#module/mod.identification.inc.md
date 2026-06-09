# PWNC API Documentation

[← Index](../../README.md) | [`module/#module/mod.identification.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23module/mod.identification.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Module: Identification (`mod.identification.inc`)

This module handles user authentication and password recovery for the PWNC Web Platform. It provides:
- **Login functionality** with secure password hashing (SHA-256 with optional MD5 fallback)
- **Password recovery** via email with CAPTCHA verification
- **Brute-force protection** via login attempt limiting
- **Dual authentication systems** (system permissions and user profiles)

---

### Global Variables

| Name                          | Default/Value                     | Description                                                                 |
|-------------------------------|-----------------------------------|-----------------------------------------------------------------------------|
| `$location`                   | `NULL`                            | Redirect URL after successful login                                         |
| `$identification_message`     | `NULL`                            | Controls module behavior (`"__recover"`, `"_recover"`, `"recover"`)         |
| `$identification_user`        | `NULL`                            | Username for login/recovery                                                 |
| `$identification_email`       | `NULL`                            | User email (unused in current implementation)                               |
| `$identification_captcha_key` | `NULL`                            | CAPTCHA user input                                                          |
| `$identification_captcha_code`| `NULL`                            | CAPTCHA verification code                                                   |
| `$identification_code`        | `NULL`                            | Recovery code from email link                                               |
| `$recover_message`            | Localized string                  | Default recovery instruction message                                        |

---

### Core Logic Flow

1. **Password Recovery (`__recover`)**
   - Validates recovery code
   - Resets password for system permissions or user profiles
   - Sends new password via email

2. **Recovery Request (`_recover`)**
   - Validates CAPTCHA
   - Verifies user existence in permissions or profiles
   - Generates recovery code and sends email

3. **Login Form**
   - Displays login interface with brute-force protection
   - Handles password hashing client-side
   - Provides "continue as guest" and logout options

---

### Key Functions/Methods

#### `identification_submit()` (JavaScript)
**Purpose**: Handles client-side password hashing before form submission.

**Parameters**: None (uses DOM elements)

**Return Values**: `false` (prevents default form submission)

**Mechanisms**:
1. Checks if legacy MD5 hashing is enabled
2. Loads MD5 library if needed
3. Hashes password with SHA-256 (or MD5 → SHA-256 for legacy)
4. Submits form with hashed password

**Usage Context**:
```javascript
// Example: Triggered by form onsubmit
<form onsubmit="return identification_submit();">
```

---

#### `_identification_submit(legacy)` (JavaScript)
**Purpose**: Core password hashing logic.

**Parameters**:

| Name    | Type    | Description                          |
|---------|---------|--------------------------------------|
| `legacy`| boolean | Use MD5 fallback (default: `true`)   |

**Mechanisms**:
1. Combines password with system salt
2. Applies SHA-256 (or MD5 → SHA-256)
3. Updates password field with hashed value

**Example**:
```javascript
// Force modern hashing
_identification_submit(false);
```

---

### Security Features

1. **Brute-Force Protection**
   - Tracks failed attempts per IP
   - Blocks after `CMS_LOGIN_ATTEMPT_MAX` attempts
   - Lockout duration: `CMS_LOGIN_BLOCK_TIME`

2. **Password Hashing**
   - Client-side SHA-256 (with optional MD5 fallback)
   - System salt (`cms_cache("cms.salt_password")`)

3. **Recovery Security**
   - CAPTCHA verification
   - Time-limited recovery codes (1 hour)
   - Email confirmation

---

### Usage Examples

#### 1. Password Recovery Flow
```php
// Trigger recovery form
cms_url(["identification_message" => "recover"]);

// Process recovery request
if ($identification_message === "_recover") {
    // Validates CAPTCHA and sends recovery email
}
```

#### 2. Login Form Integration
```php
// Display login form with redirect
$location = cms_url("dashboard");
include "module/#module/mod.identification.inc";
```

#### 3. Custom Recovery Message
```php
// Override default message
$recover_message = "<div class='custom-alert'>Enter your username</div>";
$identification_message = "recover";
```

---

### Error Handling

| Scenario                     | Message Constant                          | Display Location       |
|------------------------------|-------------------------------------------|------------------------|
| Invalid recovery code        | `CMS_L_MOD_IDENTIFICATION_017`            | Response div           |
| CAPTCHA mismatch             | `CMS_L_MOD_IDENTIFICATION_013`            | Recovery form          |
| Email sending failed         | `CMS_L_MOD_IDENTIFICATION_020`            | Response div           |
| Account locked               | `CMS_L_MOD_IDENTIFICATION_024`            | Section content        |

---

### Dependencies

| Module       | Purpose                          | Loaded Via          |
|--------------|----------------------------------|---------------------|
| `captcha`    | CAPTCHA generation/verification  | `cms_load("captcha")`|
| `smtp`       | Email delivery                   | `cms_load("smtp")`   |
| `profile`    | User profile management          | `cms_load("profile")`|
| `data`       | Key-value storage                | `new data()`         |

---

### Constants Used

| Constant                     | Typical Value               | Purpose                          |
|------------------------------|-----------------------------|----------------------------------|
| `CMS_L_MOD_IDENTIFICATION_*` | Localized strings           | UI text                          |
| `CMS_LOGIN_ATTEMPT_MAX`      | `5`                         | Max failed attempts              |
| `CMS_LOGIN_BLOCK_TIME`       | `3600` (1 hour)             | Lockout duration                 |
| `CMS_DB_PROFILE_USER`        | `"user"`                    | Profile field identifier         |


<!-- HASH:cd7e60296126a4068ba43e6638f686ef -->
