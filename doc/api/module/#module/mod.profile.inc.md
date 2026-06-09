# PWNC API Documentation

[← Index](../../README.md) | [`module/#module/mod.profile.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23module/mod.profile.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Profile Module (`mod.profile.inc`)

The **Profile Module** provides user registration, activation, and profile management functionality within the PWNC Web Platform. It handles:
- User registration with email verification and CAPTCHA protection
- Account activation via confirmation links
- Profile editing with configurable fields
- Data validation and error handling
- Secure password management

This module integrates with the platform's core systems for:
- Database operations (`data` class)
- Authentication (`profile` class)
- Logging (`log` class)
- Email delivery (`smtp` module)
- CAPTCHA verification (`captcha` module)

---

### Global Variables

| Name | Type | Description |
|------|------|-------------|
| `$profile_message` | `string` | Controls module behavior: `"register"`, `"_register"`, `"activate"`, `"edit"`, `"_edit"`, `"_activate"` |
| `$profile_param` | `array` | Form submission data for profile fields |
| `$profile_user` | `string` | Username input |
| `$profile_email` | `string` | Email address input |
| `$profile_captcha_key` | `string` | CAPTCHA user input |
| `$profile_captcha_code` | `string` | CAPTCHA verification code |
| `$profile_code` | `string` | Activation code for account confirmation |

---

### Module Workflow

1. **Initialization**
   - Loads the `profile` library
   - Instantiates the `profile` class and checks if the module is enabled
   - Sets operator privileges for administrative operations

2. **Activation Flow**
   - Validates activation codes from confirmation emails
   - Creates user accounts from registration data
   - Logs successful registrations
   - Sets authentication cookies and redirects

3. **Profile Retrieval**
   - Loads existing user profiles for editing
   - Creates new profile objects for registration

4. **Field Configuration**
   - Defines profile fields across 7 categories (login, personal, contact, etc.)
   - Loads custom field configurations from system data
   - Determines field visibility, editability, and required status

5. **Form Processing**
   - Validates input data (emails, passwords, required fields)
   - Handles registration with CAPTCHA verification
   - Processes profile updates
   - Displays appropriate error messages

6. **UI Rendering**
   - Generates form fields based on configuration
   - Handles special field types (passwords, credit card dates, comments)
   - Displays success/error messages
   - Includes CAPTCHA for registration

---

### Key Methods and Logic

#### Registration Process
```php
if (in_array($profile_message, ["_register", "_edit"])) {
    // Field validation and data assignment
    // ...
    if (count($mismatch) === 0) {
        // Save profile data
        if ($profile->set($profile_data)) {
            echo("<div class=\"response-success\">" . CMS_L_MOD_PROFILE_034 . "</div>");
        }
    }
}
```

**Purpose**: Processes submitted registration or profile update forms.

**Parameters**:
- Uses global `$profile_param` for form data
- Uses `$profile_data` object to store validated values

**Return Values**: None (outputs HTML responses)

**Mechanisms**:
1. Iterates through editable fields
2. Validates each field based on type (email, password, etc.)
3. Checks for required fields
4. Stores valid data in `$profile_data` object
5. Saves data if no validation errors exist

**Usage Context**:
- Triggered when form is submitted with `_register` or `_edit` message
- Used for both new registrations and profile updates

**Example**:
```php
// After form submission with profile data
$profile_message = "_register";
$profile_param = [
    "email" => "user@example.com",
    "password" => ["secure123", "secure123"],
    "pre_name" => "John"
];
// Module processes this data and creates/updates the profile
```

---

#### Activation Flow
```php
if (streq($profile_message, "activate")) {
    $data = new data("#system/profile.registration");
    if (stre($profile_code) || ($data->get($profile_code) === NULL)) {
        // Invalid code handling
    }
    // Create profile from registration data
    if ($profile->add($profile_data)) {
        // Log and redirect
    }
}
```

**Purpose**: Handles account activation via confirmation links.

**Parameters**:
- `$profile_code`: Activation code from URL
- Uses global `$profile_message` = "activate"

**Return Values**: None (outputs HTML or redirects)

**Mechanisms**:
1. Verifies activation code exists in registration data
2. Creates new profile from registration data
3. Logs successful registration
4. Sets authentication cookies
5. Redirects to success page

**Usage Context**:
- Triggered when user clicks activation link in confirmation email
- Final step in registration process

**Example**:
```php
// In confirmation email:
$activation_url = cms_url([
    "profile_message" => "activate",
    "profile_code" => $generated_code
]);
// When user visits this URL, their account is activated
```

---

#### Field Configuration
```php
$field = [
    1 => [CMS_DB_PROFILE_CODE => CMS_L_MOD_PROFILE_008, ...],
    2 => [CMS_DB_PROFILE_COMPANY => CMS_L_MOD_PROFILE_012, ...],
    // ...
];
for ($i = 1; $i <= 20; $i++) {
    $field[7][CMS_DB_PROFILE_CUSTOM_FIELD . $i] = $_title;
}
```

**Purpose**: Defines the structure of profile fields and their display labels.

**Parameters**:
- `$data`: System configuration data
- `$i`: Loop counter for custom fields

**Return Values**: Populates `$field` array with field definitions

**Mechanisms**:
1. Defines 6 standard field categories (login, personal, contact, etc.)
2. Adds up to 20 custom fields from system configuration
3. Each field has a database column name and display label

**Usage Context**:
- Used to generate the profile form
- Determines which fields are shown and how they're labeled

**Example**:
```php
// System configuration might define:
$data->set(CMS_DB_PROFILE_CUSTOM_FIELD . "1", "title", "Department");
// This would add a "Department" field to the custom fields section
```

---

#### Form Generation
```php
foreach ($visible AS $key => $value) {
    echo("<h2>" . $title[$key] . "</h2>");
    foreach ($value AS $_key => $_) {
        // Generate appropriate input field based on $_key
        switch ($_key) {
            case CMS_DB_PROFILE_PASSWORD:
                // Special password field handling
                break;
            case CMS_DB_PROFILE_CREDIT_CARD_VALIDITY:
                // Month/year select and input
                break;
            // ...
        }
    }
}
```

**Purpose**: Dynamically generates the profile form based on configuration.

**Parameters**:
- `$visible`: Array of fields to display
- `$editable`: Array of fields that can be edited
- `$required`: Array of required fields
- `$mismatch`: Array of validation errors

**Return Values**: None (outputs HTML form)

**Mechanisms**:
1. Iterates through field categories
2. For each field, determines appropriate input type
3. Handles special cases (passwords, credit card dates, textareas)
4. Applies error styling for invalid fields
5. Marks required fields with asterisks

**Usage Context**:
- Called during page rendering
- Generates the complete profile form

**Example**:
```php
// For a password field:
echo("<input id=\"profile-password\" name=\"profile_param[password][0]\" type=\"password\">");
echo("<input id=\"profile-password-confirmation\" name=\"profile_param[password][1]\" type=\"password\">");
// For a credit card validity field:
select($months, $selected_month, "profile_param[credit_card_validity][0]");
echo(" / <input name=\"profile_param[credit_card_validity][1]\" type=\"text\" value=\"2023\">");
```

---

### Error Handling

The module uses several error handling mechanisms:

1. **Validation Errors** (`$mismatch` array):
   - Email format validation
   - Password confirmation matching
   - Required field checks
   - Username availability
   - CAPTCHA verification

2. **System Errors**:
   - Database operation failures
   - Email sending failures
   - Registration data storage failures

3. **User Feedback**:
   - Error messages displayed in `<div class="response-error">`
   - Success messages displayed in `<div class="response-success">`
   - Field-specific errors shown next to invalid fields

**Example Error Display**:
```php
if (isset($mismatch[0]["email"])) {
    echo("<div>" . x($mismatch[0]["email"]) . "</div>");
    echo("<input class=\"profile-mismatch\" ...>");
}
```

---

### Security Features

1. **Password Handling**:
   - Passwords are hashed with `hash64()` using platform salt
   - Password confirmation fields require matching values
   - Minimum length requirement (4 characters)

2. **Activation Codes**:
   - One-time use codes with 24-hour expiration
   - Stored in separate registration data store

3. **CAPTCHA**:
   - Integrated CAPTCHA verification for registration
   - Prevents automated registration attempts

4. **Data Validation**:
   - Email format verification
   - Username uniqueness checks
   - Required field validation

5. **Secure Output**:
   - All output is properly escaped with `x()` function
   - Form submissions use POST method

---

### Integration Points

1. **Database**:
   - Uses `data` class for system configuration
   - Uses `profile` class for user data operations
   - Stores registration data in `#system/profile.registration`

2. **Authentication**:
   - Sets authentication cookies on successful activation
   - Uses `cms_set_cookie()` for secure cookie handling

3. **Email**:
   - Uses `smtp` module for sending confirmation emails
   - Email templates use platform localization strings

4. **Logging**:
   - Logs successful registrations with `log` class
   - Tracks both access and user-specific events

5. **Permissions**:
   - Checks user permissions with `permission` class
   - Verifies username availability

---

### Usage Example: Complete Registration Flow

**1. Registration Form Display**:
```php
// User visits registration page
$profile_message = "register";
// Module displays registration form with:
// - Username/email fields
// - Password fields
// - CAPTCHA
// - Submit button
```

**2. Form Submission**:
```php
// User submits form with:
$profile_message = "_register";
$profile_param = [
    "email" => "user@example.com",
    "password" => ["secure123", "secure123"],
    "pre_name" => "John",
    "sur_name" => "Doe"
];
$profile_captcha_key = "user_input";
$profile_captcha_code = "generated_code";
// Module:
// - Validates all fields
// - Verifies CAPTCHA
// - Stores registration data
// - Sends confirmation email
```

**3. Email Confirmation**:
```php
// User receives email with link:
$activation_url = cms_url([
    "profile_message" => "activate",
    "profile_code" => "unique_code_here"
]);
// When clicked, module:
// - Verifies activation code
// - Creates user account
// - Sets authentication cookies
// - Redirects to success page
```

**4. Profile Editing**:
```php
// Authenticated user visits profile page
$profile_message = "edit";
// Module:
// - Loads user profile data
// - Displays edit form
// - Processes updates on submission
```

---

### Configuration Options

The module respects several configuration options stored in `#system/profile`:

| Configuration Key | Type | Description |
|-------------------|------|-------------|
| `[field_name].visible` | `bool` | Whether field should be displayed |
| `[field_name].editable` | `bool` | Whether field can be edited |
| `[field_name].required` | `bool` | Whether field is required |
| `CMS_DB_PROFILE_USER.required` | `bool` | Whether username must differ from email |
| `CMS_DB_PROFILE_CUSTOM_FIELD[1-20].title` | `string` | Custom field display labels |

**Example Configuration**:
```php
$data = new data("#system/profile");
$data->set(CMS_DB_PROFILE_PHONE_1, "visible", true);
$data->set(CMS_DB_PROFILE_PHONE_1, "editable", true);
$data->set(CMS_DB_PROFILE_PHONE_1, "required", false);
$data->set(CMS_DB_PROFILE_CUSTOM_FIELD . "1", "title", "Department");
$data->save();
```

---

### Localization

The module uses platform localization strings for all user-facing text:

| Localization Key | Typical Value | Usage |
|------------------|---------------|-------|
| `CMS_L_MOD_PROFILE_001` | "Not specified" | Default value display |
| `CMS_L_MOD_PROFILE_002` | "Login" | Category title |
| `CMS_L_MOD_PROFILE_009` | "Username" | Field label |
| `CMS_L_MOD_PROFILE_032` | "Passwords do not match" | Error message |
| `CMS_L_MOD_PROFILE_033` | "Password is too short" | Error message |
| `CMS_L_MOD_PROFILE_034` | "Profile saved" | Success message |
| `CMS_L_MOD_PROFILE_040` | "Register" | Button label |
| `CMS_L_MOD_PROFILE_041` | "Registration Confirmation" | Email subject |
| `CMS_L_MOD_PROFILE_042` | "Welcome %s! ..." | Email body template |
| `CMS_L_MOD_PROFILE_043` | "Registration successful" | Success message |
| `CMS_L_MOD_PROFILE_044` | "Invalid email format" | Error message |
| `CMS_L_MOD_PROFILE_045` | "Username already exists" | Error message |
| `CMS_L_MOD_PROFILE_046` | "CAPTCHA verification failed" | Error message |
| `CMS_L_MOD_PROFILE_047` | "Invalid activation code" | Error message |
| `CMS_L_MOD_PROFILE_048` | "Account activated" | Success message |
| `CMS_L_MOD_PROFILE_049` | "Leave empty to keep current password" | Password hint |
| `CMS_L_MOD_PROFILE_051` | "This field is required" | Error message |
| `CMS_L_MOD_PROFILE_052` | "Email already registered" | Error message |

---

### Dependencies

| Dependency | Purpose |
|------------|---------|
| `profile` library | Core profile management functionality |
| `data` class | Database operations and configuration |
| `log` class | User activity logging |
| `smtp` module | Email delivery |
| `captcha` module | CAPTCHA generation and verification |
| `cms_cache()` | Password salt retrieval |
| `cms_url()` | URL generation |
| `cms_set_cookie()` | Authentication cookie management |
| `unique_id()` | Activation code generation |
| `verify_email()` | Email format validation |
| `hash64()` | Password hashing |
| `utf8_*` functions | Multibyte string handling |
| `x()` | XML/HTML escaping |
| `insert()` | Template inclusion |
| `permission()` | Access control |

---

### Best Practices

1. **Customization**:
   - Configure visible/editable fields via `#system/profile` data
   - Add custom fields by setting their titles in configuration
   - Adjust required fields based on business needs

2. **Security**:
   - Always use HTTPS for profile-related pages
   - Consider increasing password length requirement
   - Regularly rotate password salt

3. **User Experience**:
   - Provide clear error messages
   - Mark required fields clearly
   - Consider adding password strength meter

4. **Integration**:
   - Use the `profile` class for programmatic profile management
   - Leverage the `log` class to track profile-related events
   - Customize email templates for branding

5. **Performance**:
   - Cache field configurations when possible
   - Consider lazy-loading profile data for large user bases
   - Optimize database indexes for profile queries


<!-- HASH:5f40c3ec741d11bf1686fe232ba84157 -->
