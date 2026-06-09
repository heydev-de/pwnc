# PWNC API Documentation

[← Index](../README.md) | [`module/identification.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/identification.php)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Module: Identification (`module/identification.php`)

**Overview**
This module handles the initial identification workflow for the PWNC Web Platform. It ensures cookie support is available in the user's browser before proceeding with authentication. If cookies are disabled, it displays an error message. If cookies are enabled, it renders the identification prompt (login form).

---

### **Workflow Logic**

| Step | Condition | Action |
|------|-----------|--------|
| 1 | No `cms_check_cookie` cookie present | Attempt to set a test cookie and reload the page with a `cms_check_cookie` parameter. |
| 2 | `cms_check_cookie` parameter present but cookie still missing | Display an error message indicating cookies are required. |
| 3 | `cms_check_cookie` cookie present | Render the identification prompt (login form). |

---

### **Key Mechanisms**

#### **1. Cookie Availability Check**
- The module checks for the presence of the `cms_check_cookie` cookie.
- If absent, it attempts to set a test cookie and reloads the page with a `cms_check_cookie` parameter to verify cookie support.
- If the cookie is still missing after reload, it assumes cookies are disabled and displays an error.

#### **2. URL Redirection**
- Uses `cms_url()` to generate a self-referential URL with the `cms_check_cookie` parameter.
- Forces a page refresh via `header("Refresh: 0; url=$location")` to ensure the cookie is set before further processing.

#### **3. HTML Output**
- Renders minimal HTML5 documents with platform-standard headers, stylesheets, and body classes.
- Uses `x()` for XML escaping of dynamic values (e.g., `CMS_CLASS`).
- Localized strings (e.g., `CMS_L_MOD_IDENTIFICATION_007`, `CMS_L_NOCOOKIE`) are used for user-facing messages.

#### **4. Identification Prompt**
- If cookies are available, the module dynamically includes the login form via `mod.identification.inc`.

---

### **Dependencies**

| Dependency | Purpose |
|------------|---------|
| `pwnc.inc` | Core platform utilities (e.g., `cms_url()`, `cms_set_cookie()`, `x()`). |
| `CMS_MODULES_PATH . "#module/mod.identification.inc` | Login form UI and logic. |
| `CMS_DOCTYPE_HTML`, `CMS_HTML_HEADER`, `CMS_STYLESHEET`, `CMS_JAVASCRIPT` | Platform-standard HTML templates. |

---

### **Usage Example**

#### **Scenario: User Accesses a Protected Page**
1. The user navigates to a protected URL (e.g., `/admin`).
2. The platform routes the request to `module/identification.php`.
3. The module checks for cookie support:
   - If cookies are disabled, the user sees:
     ```html
     <div>Cookies are required to use this platform.</div>
     ```
   - If cookies are enabled, the login form is displayed:
     ```html
     <form method="POST" action="...">
       <input type="text" name="username" />
       <input type="password" name="password" />
       <button type="submit">Login</button>
     </form>
     ```

---

### **Error Handling**

| Error | User-Facing Message | Technical Resolution |
|-------|---------------------|----------------------|
| Cookies disabled | `CMS_L_NOCOOKIE` (e.g., "Cookies are required.") | No further action; user must enable cookies. |
| Failed cookie set | Implicit (redirect loop) | Platform logs the event for debugging. |

---

### **Security Considerations**
- **CSRF Protection**: The `cms_url()` function includes CSRF tokens in generated URLs.
- **Escaping**: All dynamic values (e.g., `$location`) are escaped using `x()` to prevent XSS.
- **Cookie Validation**: The test cookie (`cms_check_cookie`) is a minimal, non-sensitive value.

---

### **Customization Points**
- **Localization**: Modify `CMS_L_MOD_IDENTIFICATION_007` and `CMS_L_NOCOOKIE` in language files.
- **Styling**: Override `CMS_STYLESHEET` or `CMS_CLASS` in platform configuration.
- **Redirect Logic**: Adjust `$location` handling to redirect to a custom post-login page.


<!-- HASH:a8fd0f5846bc4b94f0cda39efdd757a6 -->
