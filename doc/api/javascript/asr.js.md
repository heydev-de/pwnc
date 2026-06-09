# PWNC API Documentation

[← Index](../README.md) | [`javascript/asr.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/asr.js)

- **Version:** `26.5.30.4`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## ASR (AJAX Submit & Request) Module

Core JavaScript utility for handling asynchronous form submissions and HTTP requests without page reloads. Implements a lightweight, dependency-free AJAX layer for PWNC's frontend interactions.

---

## Functions

### `asr_send(url, callback = null)`

Handles asynchronous GET requests with cache-busting and optional callback execution.

#### Parameters

| Name       | Type       | Default | Description                                                                 |
|------------|------------|---------|-----------------------------------------------------------------------------|
| `url`      | string     | -       | Target URL. May include existing query parameters and/or fragment identifiers. |
| `callback` | function   | `null`  | Optional callback function receiving the server response or `false` on error. |

#### Return Value
`void` – Executes asynchronously.

#### Inner Mechanisms
1. **Cache-Busting**: Appends a random 8-character alphanumeric query parameter (`_XXXXXXXX=1`) to the URL to prevent browser caching.
2. **Fragment Preservation**: Splits the URL at `#` to preserve any fragment identifier, reattaching it after cache-busting.
3. **Fetch API**: Uses `fetch()` with `cache: "no-store"` to enforce fresh requests.
4. **Error Handling**: Silently fails (callback receives `false`) on network errors or non-200 responses.

#### Usage Context
- Polling endpoints for live updates (e.g., notifications, chat messages).
- Fetching dynamic content without page reloads.
- Bypassing aggressive browser caching for critical requests.

#### Example
```javascript
asr_send("/api/notifications", function(response) {
    if (response === false) {
        console.error("Failed to fetch notifications");
        return;
    }
    document.getElementById("notifications").innerHTML = response;
});
// Requests: /api/notifications?_aB3dE7fG=1
```

---

### `asr_form_bind(object, callback = null)`

Binds a form to asynchronous submission via AJAX, overriding the default synchronous behavior.

#### Parameters

| Name       | Type       | Default | Description                                                                 |
|------------|------------|---------|-----------------------------------------------------------------------------|
| `object`   | HTMLFormElement | -   | The `<form>` element to bind.                                               |
| `callback` | function   | `null`  | Optional callback function receiving the server response.                   |

#### Return Value
`void`

#### Inner Mechanisms
1. **Event Override**: Replaces the form's native `submit` event with a custom handler (`asr_submit_function1`) that prevents default submission and triggers `asr_form_post()`.
2. **Method Preservation**: Stores the original `submit()` method in `asr_submit_function2` to allow later restoration.
3. **Synthetic Events**: Overrides the form's `submit()` method to dispatch a synthetic `submit` event, ensuring consistency with native behavior.

#### Usage Context
- Enhancing forms with AJAX submission while maintaining progressive degradation.
- Preventing full-page reloads on form submissions (e.g., login forms, comments).
- Integrating with PWNC's backend modules that expect POST data.

#### Example
```javascript
const loginForm = document.getElementById("login-form");
asr_form_bind(loginForm, function(response) {
    if (response.includes("Success")) {
        window.location.href = "/dashboard";
    } else {
        document.getElementById("error-message").textContent = "Invalid credentials";
    }
});
```

---

### `asr_form_unbind(object)`

Restores a form's original synchronous submission behavior after `asr_form_bind()`.

#### Parameters

| Name     | Type            | Default | Description                     |
|----------|-----------------|---------|---------------------------------|
| `object` | HTMLFormElement | -       | The `<form>` element to unbind. |

#### Return Value
`void`

#### Inner Mechanisms
1. **Event Cleanup**: Removes the custom `submit` event listener added by `asr_form_bind()`.
2. **Method Restoration**: Reverts the form's `submit()` method to its original implementation (`asr_submit_function2`).

#### Usage Context
- Temporarily disabling AJAX submission (e.g., for debugging or fallback scenarios).
- Cleaning up event listeners to prevent memory leaks.

#### Example
```javascript
const commentForm = document.getElementById("comment-form");
asr_form_bind(commentForm, handleCommentSubmit);

// Later, restore native behavior
asr_form_unbind(commentForm);
```

---

### `asr_form_post(object, callback = "")`

Submits a form asynchronously via POST and processes the response.

#### Parameters

| Name       | Type            | Default | Description                                                                 |
|------------|-----------------|---------|-----------------------------------------------------------------------------|
| `object`   | HTMLFormElement | -       | The `<form>` element to submit.                                             |
| `callback` | function        | `""`    | Optional callback function receiving the server response or `false` on error. |

#### Return Value
`void` – Executes asynchronously.

#### Inner Mechanisms
1. **FormData**: Serializes the form's inputs into a `FormData` object for multipart/form-data submission.
2. **Fetch API**: Uses `fetch()` with `method: "POST"` to submit the form data to the form's `action` URL.
3. **Response Handling**: Passes the raw response text to the callback if the request succeeds.

#### Usage Context
- Core submission logic for AJAX-bound forms.
- Handling file uploads or complex form data without page reloads.

#### Example
```javascript
function handleContactForm(response) {
    if (response === false) {
        alert("Submission failed. Please try again.");
        return;
    }
    document.getElementById("contact-form").reset();
    alert("Thank you for your message!");
}

const contactForm = document.getElementById("contact-form");
contactForm.addEventListener("submit", function(e) {
    e.preventDefault();
    asr_form_post(contactForm, handleContactForm);
});
```


<!-- HASH:73a10b8b7b9a3fcebb52bcb2bf59b901 -->
