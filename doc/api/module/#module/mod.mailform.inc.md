# PWNC API Documentation

[← Index](../../README.md) | [`module/#module/mod.mailform.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23module/mod.mailform.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Mail Form Module (`mod.mailform.inc`)

The Mail Form module provides a flexible, multi-page form system for collecting and processing user input. It supports various field types (text, textarea, checkbox, radio, select, hidden, and custom code), input validation, CAPTCHA verification, and multiple submission methods (email, HTTP POST). Forms are defined in a structured data file (`#system/mailform`) and rendered dynamically based on configuration.

---

### **Global Variables**

| Name | Type | Description |
|------|------|-------------|
| `$mailform_form` | `string` | Currently selected form identifier. |
| `$mailform_form_active` | `string` | Form identifier during submission. |
| `$mailform_page` | `int` | Current page index (0-based). |
| `$mailform_message` | `string` | Submission command (`CMS_L_COMMAND_PREVIOUS` or `CMS_L_COMMAND_NEXT`). |
| `$mailform_captcha_code` | `string` | User-provided CAPTCHA code. |
| `$mailform_captcha_key` | `string` | CAPTCHA session key. |

---

### **Core Workflow**

1. **Form Selection**
   - If no form is selected, lists all available forms (containers with `email` or `url` fields).
   - If exactly one form exists, it is auto-selected.

2. **Form Submission**
   - Processes multi-page navigation (`PREVIOUS`/`NEXT`).
   - Validates input against regex patterns (`match`) and required fields.
   - Handles CAPTCHA verification on the final page.

3. **Submission Methods**
   - **Email**: Sends form data to the configured `email` address.
   - **HTTP POST**: Forwards data to the configured `url` (supports multiple endpoints).
   - **Confirmation**: Sends a copy to users (if `confirm` fields are present).

4. **Output**
   - Renders the form or a success/error message.

---

### **Key Functions & Logic**

#### **Form Rendering**
The module dynamically generates HTML for each field type based on the form configuration. Field types are processed in a `switch` statement:

| Field Type | Description |
|------------|-------------|
| `checkbox`/`radio` | Multi-option inputs with column layout support. |
| `select` | Dropdown list with optional preselection. |
| `text` | Single-line text input with `width`/`length` constraints. |
| `textarea` | Multi-line text input with `width`/`height` constraints. |
| `code` | Custom HTML/JS (supports placeholders like `%label%`). |
| `hidden` | Hidden input field. |
| `pagebreak` | Splits the form into multiple pages. |

**Example (Text Field):**
```php
case "text":
    echo("<div class='mailform-text p'>" .
         "<label for='$_id'>$name</label><br>" .
         "<input id='$_id' name='$id' type='text' value='" . x($value) . "'>" .
         "</div>");
```

---

#### **Input Validation**
- **Required Fields**: Checks if `required` is set and the field is empty.
- **Regex Matching**: Validates input against the `match` pattern.
- **CAPTCHA**: Verifies the user-provided code against the session key.

**Example (Validation Logic):**
```php
if (nstre($match = $data->get($key, "match")) && preg_match($match, $value) !== 1) {
    $flag_mismatch = TRUE; // Mark as invalid
}
```

---

#### **Submission Handling**
- **Email**: Uses the `smtp_send()` function to deliver HTML-formatted content.
- **HTTP POST**: Forwards data to external URLs via `http_post()`.
- **Confirmation**: Sends a copy to users if `confirm` fields are present.

**Example (Email Submission):**
```php
$flag_success = smtp_send(
    $email,          // Recipient
    $subject,        // Subject
    $_buffer,        // HTML body
    TRUE,            // Is HTML?
    $confirm ? implode(", ", $confirm) : NULL  // Reply-to
);
```

---

### **Usage Example**

#### **1. Define a Form in `#system/mailform`**
```ini
[contact_form]
name = "Contact Us"
email = "support@example.com"
captcha = true

[contact_form/field1]
#type = text
name = "Your Name"
required = true
match = "/^[a-zA-Z ]+$/"

[contact_form/field2]
#type = textarea
name = "Message"
required = true
```

#### **2. Render the Form in a Template**
```php
// Auto-selects the form if only one exists
insert("mailform");
```

#### **3. Process Submission**
- The module automatically handles:
  - Multi-page navigation.
  - Input validation.
  - CAPTCHA verification.
  - Email/HTTP POST submission.

---

### **Error Handling**
- **No Forms Available**: Displays `CMS_MSG_UNAVAILABLE`.
- **Validation Errors**: Highlights mismatched fields and shows `CMS_L_MOD_MAILFORM_008`.
- **CAPTCHA Failure**: Marks the CAPTCHA field as invalid.
- **Submission Failure**: Displays `CMS_L_MOD_MAILFORM_012`.

---

### **Dependencies**
- **SMTP Module**: Required for email submissions (`cms_load("smtp")`).
- **CAPTCHA Module**: Optional for CAPTCHA verification (`cms_load("captcha")`).
- **Data Module**: Uses `data` class to read form configurations.

---

### **Security Considerations**
- **Input Sanitization**: Uses `x()` for HTML escaping and `sqlesc()` for SQL safety.
- **CSRF Protection**: Relies on `cms_url()` for form action URLs.
- **CAPTCHA**: Prevents automated submissions.


<!-- HASH:9d9c187c1fee12a1f4f6b022657aaa6d -->
