# PWNC API Documentation

[← Index](../README.md) | [`javascript/ifc.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/ifc.js)

- **Version:** `26.5.30.4`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## IFC (Interface Control) JavaScript Module

The `ifc.js` file provides a comprehensive set of utilities for managing user interface interactions within the PWNC Web Platform. It handles form submissions, input field manipulation, multilingual content management, syntax highlighting, and undo/redo functionality. This module is designed to work seamlessly with PWNC's backend PHP/MySQL infrastructure while maintaining a zero-dependency, high-performance frontend.

---

## Command Functions

Functions for controlling page navigation, form submissions, and interface state management.

### `ifc_select_page(index, popup = false)`

Switches the current page to a predefined IFC page.

| Parameter | Type    | Description                                                                 |
|-----------|---------|-----------------------------------------------------------------------------|
| index     | number  | Index of the target page in the `ifc_page` array.                          |
| popup     | boolean | If `true`, loads the page in a new context without resetting the interface. |

**Return Value:** None.

**Mechanism:**
- Constructs a URL using the base IFC page and appts the `ifc_message` and `ifc_param` parameters.
- If `popup` is `false`, resets the current interface state before loading the new page.
- Uses `load_page()` to handle the actual page transition.

**Usage:**
- Navigating between different sections of the PWNC interface.
- Opening a page in a popup for temporary interactions.

**Example:**
```javascript
// Navigate to the second page in the ifc_page array
ifc_select_page(1);

// Open the third page in a popup
ifc_select_page(2, true);
```

---

### `ifc_select_menu(index)`

Executes a predefined menu action.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| index     | number | Index of the menu item in the `ifc_menu` array.                            |

**Return Value:** None.

**Mechanism:**
- Checks if the menu item has a confirmation message. If so, prompts the user for confirmation.
- Executes the associated JavaScript code from `ifc_menu[1][index]` using `new Function()`.

**Usage:**
- Triggering actions from a dynamic menu system.
- Handling user confirmations before executing sensitive operations.

**Example:**
```javascript
// Execute the first menu item's action
ifc_select_menu(0);
```

---

### `ifc_submit()`

Submits the IFC form after memorizing the current scroll position.

**Return Value:** None.

**Mechanism:**
- Calls `ifc_memorize_position()` to store the current scroll position.
- Dispatches a `submit` event on the IFC form.
- Submits the form if the event is not canceled.

**Usage:**
- Standard form submission with scroll position preservation.
- Ensuring consistent user experience after form submission.

**Example:**
```javascript
// Submit the IFC form
ifc_submit();
```

---

### `ifc_post(message = "", param = "")`

Sets IFC form parameters and submits the form.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| message   | string | Value for the `ifc_message` form field.                                    |
| param     | string | Value for the `ifc_param` form field.                                      |

**Return Value:** None.

**Mechanism:**
- Sets the `ifc_message` and `ifc_param` form fields if provided.
- Calls `ifc_submit()` to submit the form.

**Usage:**
- Sending specific messages or parameters to the backend.
- Triggering backend actions with custom parameters.

**Example:**
```javascript
// Post a message with a parameter
ifc_post("save_data", "user_profile");
```

---

### `ifc_cancel(offset = 0)`

Resets the IFC form and posts a cancellation message.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| offset    | number | Index offset for resetting form elements.                                  |

**Return Value:** None.

**Mechanism:**
- Resets form elements starting from the given offset.
- Posts an `ifc_cancel` message to the backend.

**Usage:**
- Canceling ongoing operations and resetting the interface.
- Providing a clean state for new operations.

**Example:**
```javascript
// Cancel and reset the form
ifc_cancel();
```

---

### `ifc_autopost(object, message = "")`

Automatically posts a message when a form element changes.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| object    | object/string   | DOM element or name of the element to monitor.                             |
| message   | string          | Message to post when the element changes.                                  |

**Return Value:** None.

**Mechanism:**
- Converts the object parameter to a DOM element if it is a string.
- Listens for `change` events on the object and posts the specified message.

**Usage:**
- Automatically saving changes when a form field is modified.
- Reducing manual submission requirements for dynamic forms.

**Example:**
```javascript
// Auto-post "update_preferences" when the theme selector changes
ifc_autopost("theme_selector", "update_preferences");
```

---

### `ifc_response(value)`

Displays a response message in the IFC response area.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| value     | string | HTML content to display in the response area.                              |

**Return Value:** None.

**Mechanism:**
- Locates the `ifc-response` element.
- Temporarily hides the element, updates its content, and then reveals it with a fade-in effect.

**Usage:**
- Displaying feedback messages to the user.
- Showing operation results or status updates.

**Example:**
```javascript
// Display a success message
ifc_response("<div class='success'>Data saved successfully!</div>");
```

---

## Value Functions

Functions for retrieving, setting, and manipulating form field values.

### `ifc_get(object, index = 0)`

Retrieves the value of a form element.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| object    | object/string   | DOM element or name of the element.                                        |
| index     | number          | Index of the element if multiple elements share the same name.             |

**Return Value:** Mixed (string, boolean, FileList)

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| string    | Value of the form element.                                                 |
| boolean   | `false` if the element is unchecked or invalid.                            |
| FileList  | List of selected files for file inputs.                                    |

**Mechanism:**
- Converts the object parameter to a DOM element if it is a string.
- Handles different input types (checkbox, radio, text, file, etc.) appropriately.

**Usage:**
- Retrieving user input from form fields.
- Checking the state of checkboxes or radio buttons.

**Example:**
```javascript
// Get the value of a text input
const username = ifc_get("username");

// Get the value of a checked checkbox
const isSubscribed = ifc_get("newsletter_subscription");
```

---

### `ifc_get_selection(object)`

Retrieves the currently selected text from a textarea or the document.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| object    | object          | DOM element (textarea).                                                    |

**Return Value:** string

| Value     | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| string    | The selected text.                                                         |

**Mechanism:**
- For textareas, returns the selected substring.
- For other elements, returns the current document selection.

**Usage:**
- Retrieving user-selected text for manipulation or processing.
- Implementing custom text editing features.

**Example:**
```javascript
// Get selected text from a textarea
const selectedText = ifc_get_selection(document.getElementById("editor"));
```

---

### `ifc_title(object, index = 0)`

Retrieves the title or label of a form element.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| object    | object/string   | DOM element or name of the element.                                        |
| index     | number          | Index of the element if multiple elements share the same name.             |

**Return Value:** string

| Value     | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| string    | The title or label of the element.                                         |

**Mechanism:**
- Converts the object parameter to a DOM element if it is a string.
- Handles different input types to extract appropriate labels or titles.

**Usage:**
- Displaying element labels in tooltips or help text.
- Generating descriptive text for form elements.

**Example:**
```javascript
// Get the title of a button
const buttonTitle = ifc_title("submit_button");
```

---

### `ifc_reset(offset = 0)`

Resets all form elements starting from the given offset.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| offset    | number | Index offset for resetting form elements.                                  |

**Return Value:** None.

**Mechanism:**
- Iterates through form elements starting from the given offset.
- Calls `ifc_del()` on each element to reset its value.

**Usage:**
- Clearing form data for a fresh start.
- Resetting specific sections of a form.

**Example:**
```javascript
// Reset all form elements
ifc_reset();

// Reset form elements starting from the third element
ifc_reset(2);
```

---

### `ifc_del(object, index, focus = true)`

Clears the value of a form element.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| object    | object/string   | DOM element or name of the element.                                        |
| index     | number          | Index of the element if multiple elements share the same name.             |
| focus     | boolean         | If `true`, focuses the element after clearing.                             |

**Return Value:** None.

**Mechanism:**
- Converts the object parameter to a DOM element if it is a string.
- Handles different input types to clear their values appropriately.
- Focuses the element if specified.

**Usage:**
- Clearing individual form fields.
- Resetting specific elements without affecting the entire form.

**Example:**
```javascript
// Clear a text input and focus it
ifc_del("username", 0, true);

// Clear a radio button group
ifc_del("gender", 0);
```

---

### `ifc_set(object, value = "", index = 0)`

Sets the value of a form element.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| object    | object/string   | DOM element or name of the element.                                        |
| value     | string          | Value to set.                                                              |
| index     | number          | Index of the element if multiple elements share the same name.             |

**Return Value:** None.

**Mechanism:**
- Converts the object parameter to a DOM element if it is a string.
- Handles different input types to set their values appropriately.
- Triggers language reload if the element has a `data-l` attribute.

**Usage:**
- Setting form field values programmatically.
- Updating form fields based on user actions or backend responses.

**Example:**
```javascript
// Set the value of a text input
ifc_set("username", "john_doe");

// Set the value of a radio button group
ifc_set("gender", "male");
```

---

### `ifc_copy(source, target)`

Copies the value from one form element to another.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| source    | object/string   | Source DOM element or name.                                                |
| target    | object/string   | Target DOM element or name.                                                |

**Return Value:** None.

**Mechanism:**
- Retrieves the value from the source element using `ifc_get()`.
- Sets the value of the target element using `ifc_set()`.

**Usage:**
- Copying values between form fields.
- Synchronizing related form elements.

**Example:**
```javascript
// Copy the value from "source_field" to "target_field"
ifc_copy("source_field", "target_field");
```

---

### `ifc_select(object, index = 0)`

Selects or focuses a form element.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| object    | object/string   | DOM element or name of the element.                                        |
| index     | number          | Index of the element if multiple elements share the same name.             |

**Return Value:** None.

**Mechanism:**
- Converts the object parameter to a DOM element if it is a string.
- Handles different input types to select or focus them appropriately.

**Usage:**
- Focusing form fields for user input.
- Selecting text in text inputs or textareas.

**Example:**
```javascript
// Focus and select a text input
ifc_select("username");

// Focus a checkbox
ifc_select("newsletter_subscription");
```

---

### `ifc_limit(object, limit)`

Limits the length of the value in a form element.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| object    | object/string   | DOM element or name of the element.                                        |
| limit     | number          | Maximum allowed length of the value.                                       |

**Return Value:** None.

**Mechanism:**
- Retrieves the current value using `ifc_get()`.
- Truncates the value if it exceeds the specified limit and updates the element using `ifc_set()`.

**Usage:**
- Enforcing character limits on form fields.
- Preventing buffer overflows or excessive input.

**Example:**
```javascript
// Limit the "bio" textarea to 500 characters
ifc_limit("bio", 500);
```

---

## List Functions

Functions for managing groups of checkboxes or radio buttons.

### `ifc_list_activate(name = "list")`

Activates all checkboxes in a list.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| name      | string | Base name of the list elements.                                            |

**Return Value:** None.

**Mechanism:**
- Selects all checkboxes with names starting with `name[` and ending with `]`.
- Clicks each unchecked checkbox to activate it.

**Usage:**
- Selecting all items in a list for bulk operations.
- Activating multiple options at once.

**Example:**
```javascript
// Activate all items in the "permissions" list
ifc_list_activate("permissions");
```

---

### `ifc_list_invert(name = "list")`

Inverts the selection state of all checkboxes in a list.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| name      | string | Base name of the list elements.                                            |

**Return Value:** None.

**Mechanism:**
- Selects all checkboxes with names starting with `name[` and ending with `]`.
- Clicks each checkbox to invert its state.

**Usage:**
- Toggling the selection state of list items.
- Inverting bulk selections.

**Example:**
```javascript
// Invert the selection of items in the "permissions" list
ifc_list_invert("permissions");
```

---

### `ifc_list_deactivate(name = "list")`

Deactivates all checkboxes in a list.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| name      | string | Base name of the list elements.                                            |

**Return Value:** None.

**Mechanism:**
- Selects all checkboxes with names starting with `name[` and ending with `]`.
- Clicks each checked checkbox to deactivate it.

**Usage:**
- Clearing all selections in a list.
- Resetting list selections to a default state.

**Example:**
```javascript
// Deactivate all items in the "permissions" list
ifc_list_deactivate("permissions");
```

---

## Textarea Functions

Functions for managing and formatting textarea content.

### `ifc_format(object, index = 0)`

Formats the content of a textarea.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| object    | object/string   | DOM element or name of the textarea.                                       |
| index     | number          | Index of the textarea if multiple elements share the same name.            |

**Return Value:** None.

**Mechanism:**
- Calls `ifc_clean()` with the `format` parameter set to `true`.

**Usage:**
- Applying consistent formatting to textarea content.
- Cleaning and structuring user input.

**Example:**
```javascript
// Format the content of a textarea
ifc_format("editor");
```

---

### `ifc_clean(object, index, format = false)`

Cleans the content of a textarea.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| object    | object/string   | DOM element or name of the textarea.                                       |
| index     | number          | Index of the textarea if multiple elements share the same name.            |
| format    | boolean         | If `true`, applies additional formatting rules.                            |

**Return Value:** None.

**Mechanism:**
- Converts the object parameter to a DOM element if it is a string.
- Removes unwanted characters (carriage returns, zero-width characters, etc.).
- Replaces special whitespace characters and line separators.
- Applies formatting rules if specified.

**Usage:**
- Sanitizing textarea content for storage or processing.
- Ensuring consistent formatting across different input methods.

**Example:**
```javascript
// Clean the content of a textarea
ifc_clean("editor");

// Clean and format the content of a textarea
ifc_clean("editor", 0, true);
```

---

### `ifc_keydown(event)`

Handles keydown events for textareas to provide advanced editing features.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| event     | Event  | The keydown event.                                                         |

**Return Value:** boolean

| Value     | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| true      | Allows the default action to proceed.                                      |
| false     | Prevents the default action.                                               |

**Mechanism:**
- Handles various key combinations for text manipulation (e.g., indentation, line breaks).
- Supports undo/redo functionality with `Ctrl+Z` and `Ctrl+Y`.
- Manages cursor positioning and text selection.

**Usage:**
- Enhancing textarea editing with custom keybindings.
- Providing a richer editing experience for users.

**Example:**
```javascript
// Add keydown event listener to a textarea
document.getElementById("editor").addEventListener("keydown", ifc_keydown);
```

---

## Language Functions

Functions for managing multilingual content in form fields.

### Constants

| Name                     | Value/Default | Description                                                                 |
|--------------------------|---------------|-----------------------------------------------------------------------------|
| `ifc_language_separator` | `\u001F`      | Unit separator character used to delimit language sections in text.        |
| `ifc_language_flag`      | `false`       | Flag to indicate if a language selection operation is in progress.         |

---

### `ifc_language_select_all(language)`

Selects all language links for a specific language.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| language  | string | Language code to select.                                                   |

**Return Value:** boolean (`false`)

**Mechanism:**
- Sets `ifc_language_flag` to `true` to prevent recursive calls.
- Iterates through all links with class `language-` and triggers their `onclick` event if they match the specified language.
- Resets `ifc_language_flag` to `false` after completion.

**Usage:**
- Switching the entire interface to a different language.
- Ensuring all language-dependent elements are updated consistently.

**Example:**
```javascript
// Switch all language elements to German
ifc_language_select_all("de");
```

---

### `ifc_language_select(source, target, language)`

Selects a specific language for a target form element.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| source    | object/string   | Source DOM element or name.                                                |
| target    | object/string   | Target DOM element or name.                                                |
| language  | string          | Language code to select.                                                   |

**Return Value:** boolean (`false`)

**Mechanism:**
- Converts source and target parameters to DOM elements if they are strings.
- Checks if the target already has the specified language selected.
- Highlights the language link and loads the language content.

**Usage:**
- Switching individual form fields to different languages.
- Managing multilingual content in a single form.

**Example:**
```javascript
// Switch the "description" field to French
ifc_language_select("description_source", "description_target", "fr");
```

---

### `ifc_language_highlight(id, language)`

Highlights language links for a specific form element.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| id        | string | ID prefix of the language links.                                           |
| language  | string | Language code to highlight.                                                |

**Return Value:** None.

**Mechanism:**
- Iterates through all links with IDs starting with the specified prefix.
- Updates the class of each link to reflect the selected language.

**Usage:**
- Visually indicating the current language selection.
- Providing feedback on language switching.

**Example:**
```javascript
// Highlight the French language link for the "description" field
ifc_language_highlight("description", "fr");
```

---

### `ifc_language_load(source, target, language, select = true)`

Loads language-specific content into a target form element.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| source    | object/string   | Source DOM element or name.                                                |
| target    | object/string   | Target DOM element or name.                                                |
| language  | string          | Language code to load.                                                     |
| select    | boolean         | If `true`, focuses the target element after loading.                       |

**Return Value:** None.

**Mechanism:**
- Converts source and target parameters to DOM elements if they are strings.
- Retrieves the language-specific content using `ifc_language_get()`.
- Updates the target element's value and language property.

**Usage:**
- Loading multilingual content into form fields.
- Switching between different language versions of content.

**Example:**
```javascript
// Load French content into the "description" field
ifc_language_load("description_source", "description_target", "fr");
```

---

### `ifc_language_reload(source, target)`

Reloads the current language content into a target form element.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| source    | object/string   | Source DOM element or name.                                                |
| target    | object/string   | Target DOM element or name.                                                |

**Return Value:** None.

**Mechanism:**
- Converts source and target parameters to DOM elements if they are strings.
- Retrieves the current language from the target element.
- Calls `ifc_language_load()` to reload the content.

**Usage:**
- Refreshing language content after external changes.
- Ensuring content consistency after modifications.

**Example:**
```javascript
// Reload the current language content for the "description" field
ifc_language_reload("description_source", "description_target");
```

---

### `ifc_language_save(source, target)`

Saves the content of a source form element into a target element's multilingual data.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| source    | object/string   | Source DOM element or name.                                                |
| target    | object/string   | Target DOM element or name.                                                |

**Return Value:** None.

**Mechanism:**
- Converts source and target parameters to DOM elements if they are strings.
- Retrieves the current language from the source element.
- Updates the target element's value using `ifc_language_set()`.

**Usage:**
- Saving multilingual content from a form field.
- Updating the multilingual data store with new content.

**Example:**
```javascript
// Save the content of the "description" field to the multilingual data store
ifc_language_save("description_source", "description_target");
```

---

### `ifc_language_get(text, language)`

Retrieves language-specific content from a multilingual text string.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| text      | string | Multilingual text string.                                                  |
| language  | string | Language code to retrieve.                                                 |

**Return Value:** string

| Value     | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| string    | The language-specific content.                                             |

**Mechanism:**
- Parses the multilingual text string to extract the content for the specified language.
- Uses `ifc_language_separator` to delimit language sections.

**Usage:**
- Extracting specific language content from a multilingual data store.
- Displaying content in the user's preferred language.

**Example:**
```javascript
// Retrieve French content from a multilingual string
const frenchContent = ifc_language_get(multilingualText, "fr");
```

---

### `ifc_language_set(text, value, language)`

Sets language-specific content in a multilingual text string.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| text      | string | Multilingual text string.                                                  |
| value     | string | Content to set for the specified language.                                 |
| language  | string | Language code to set.                                                      |

**Return Value:** string

| Value     | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| string    | The updated multilingual text string.                                      |

**Mechanism:**
- Updates the content for the specified language in the multilingual text string.
- Uses `ifc_language_separator` to delimit language sections.

**Usage:**
- Updating multilingual content in a data store.
- Adding new language versions to existing content.

**Example:**
```javascript
// Set French content in a multilingual string
const updatedText = ifc_language_set(multilingualText, "Bonjour le monde", "fr");
```

---

## Common Functions

General utility functions for DOM manipulation and interface management.

### `ifc_object(name, index = 0, window = this)`

Retrieves a DOM element by name or ID.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| name      | string          | Name or ID of the element.                                                 |
| index     | number          | Index of the element if multiple elements share the same name.             |
| window    | object          | Window context to search within.                                           |

**Return Value:** object/Null

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| object    | The DOM element.                                                           |
| Null      | If the element is not found.                                               |

**Mechanism:**
- Searches for elements by name if the index is non-negative.
- Falls back to searching by ID if no elements are found by name.

**Usage:**
- Retrieving DOM elements for manipulation.
- Handling elements with dynamic or repeated names.

**Example:**
```javascript
// Get the first element with the name "username"
const usernameField = ifc_object("username");

// Get the element with the ID "submit_button"
const submitButton = ifc_object("submit_button", -1);
```

---

### `ifc_focus(object, index = 0)`

Focuses a form element.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| object    | object/string   | DOM element or name of the element.                                        |
| index     | number          | Index of the element if multiple elements share the same name.             |

**Return Value:** None.

**Mechanism:**
- Converts the object parameter to a DOM element if it is a string.
- Focuses the element if it is not disabled or hidden.

**Usage:**
- Directing user attention to specific form fields.
- Improving accessibility and user experience.

**Example:**
```javascript
// Focus a text input
ifc_focus("username");
```

---

### `ifc_autofocus()`

Automatically focuses the first visible and editable form element.

**Return Value:** None.

**Mechanism:**
- Selects all editable elements (inputs, textareas, contenteditable).
- Focuses the first element that is visible within the viewport.

**Usage:**
- Automatically focusing the first input field on page load.
- Enhancing user experience by reducing manual focus requirements.

**Example:**
```javascript
// Call on page load to autofocus the first editable element
window.addEventListener("load", ifc_autofocus);
```

---

### `ifc_scroll(object, top, left)`

Scrolls a textarea to the specified position.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| object    | object/string   | DOM element or name of the textarea.                                       |
| top       | number          | Vertical scroll position.                                                  |
| left      | number          | Horizontal scroll position.                                                |

**Return Value:** None.

**Mechanism:**
- Converts the object parameter to a DOM element if it is a string.
- Uses a timeout to ensure the scroll position is set after rendering.

**Usage:**
- Restoring scroll positions after content updates.
- Navigating to specific positions in large textareas.

**Example:**
```javascript
// Scroll a textarea to position (100, 50)
ifc_scroll("editor", 100, 50);
```

---

### `_ifc_scroll(object, top, left)`

Internal function to set the scroll position of a textarea.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| object    | object | DOM element (textarea).                                                    |
| top       | number | Vertical scroll position.                                                  |
| left      | number | Horizontal scroll position.                                                |

**Return Value:** None.

**Mechanism:**
- Directly sets the `scrollTop` and `scrollLeft` properties of the textarea.

**Usage:**
- Internal use by `ifc_scroll()`.
- Setting scroll positions without delay.

**Example:**
```javascript
// Internal use only
_ifc_scroll(textareaElement, 100, 50);
```

---

### `ifc_memorize_position()`

Memorizes the current scroll position in hidden form fields.

**Return Value:** None.

**Mechanism:**
- Stores the current scroll position in `ifc_left` and `ifc_top` hidden fields.

**Usage:**
- Preserving scroll positions across page reloads.
- Restoring user view after form submissions.

**Example:**
```javascript
// Memorize scroll position before form submission
ifc_memorize_position();
ifc_submit();
```

---

## Loading Animation Functions

Functions for managing the loading animation.

### `ifc_loading_event(event)`

Displays a loading animation on specific events.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| event     | string | Event name (e.g., `"window_unload"`).                                      |

**Return Value:** None.

**Mechanism:**
- Creates and appends a loading animation element if the event is `window_unload` and no loading element exists.

**Usage:**
- Indicating loading states during page transitions.
- Providing visual feedback during asynchronous operations.

**Example:**
```javascript
// Show loading animation on window unload
window.addEventListener("unload", () => ifc_loading_event("window_unload"));
```

---

### `_ifc_loading_event()`

Hides the loading animation.

**Return Value:** None.

**Mechanism:**
- Hides the loading animation element.

**Usage:**
- Internal use by the platform to hide the loading animation.
- Cleaning up after loading operations complete.

**Example:**
```javascript
// Internal use only
_ifc_loading_event();
```

---

## Download Function

### `ifc_download(url)`

Initiates a file download from the specified URL.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| url       | string | URL of the file to download.                                               |

**Return Value:** None.

**Mechanism:**
- Creates an invisible anchor element.
- Sets the `href` attribute to the specified URL and triggers a click event.
- Removes the anchor element after a delay.

**Usage:**
- Downloading files from the server.
- Providing direct download links to users.

**Example:**
```javascript
// Download a file
ifc_download("/downloads/report.pdf");
```

---

## Custom Select Function

### `ifc_custom_select(object)`

Enhances a custom select element with additional functionality.

| Parameter | Type            | Description                                                                 |
|-----------|-----------------|-----------------------------------------------------------------------------|
| object    | object/string   | DOM element or name of the custom select container.                        |

**Return Value:** None.

**Mechanism:**
- Defines custom properties (`type`, `value`, `selectedIndex`, `options`) for the select element.
- Adds event listeners for change events and click interactions.
- Supports custom styling and behavior for select elements.

**Usage:**
- Creating accessible and customizable select elements.
- Enhancing the default select element with additional features.

**Example:**
```javascript
// Enhance a custom select element
ifc_custom_select("custom_select");
```

---

## Syntax Highlighting Functions

Functions for syntax highlighting in textareas and contenteditable elements.

### Constants

| Name                     | Value/Default | Description                                                                 |
|--------------------------|---------------|-----------------------------------------------------------------------------|
| `ifc_highlight_php`      | Array         | Regex and settings for PHP syntax highlighting.                            |
| `ifc_highlight_token`    | Array         | Regex and settings for token syntax highlighting.                          |
| `ifc_highlight_detect`   | Array         | Array of syntax detection rules for different languages and contexts.      |

---

### `ifc_highlight_init()`

Initializes the syntax highlighting system.

**Return Value:** None.

**Mechanism:**
- Maps regex patterns to unique keys for efficient lookup during highlighting.

**Usage:**
- Internal initialization of the syntax highlighting system.
- Ensuring efficient pattern matching during highlighting.

**Example:**
```javascript
// Internal use only
ifc_highlight_init();
```

---

### `ifc_highlight(object, mode = 0, no_insert = false, bounce = false)`

Applies syntax highlighting to a textarea or contenteditable element.

| Parameter  | Type            | Description                                                                 |
|------------|-----------------|-----------------------------------------------------------------------------|
| object     | object          | DOM element to highlight.                                                  |
| mode       | number          | Initial highlighting mode (e.g., HTML, CSS, JavaScript).                   |
| no_insert  | boolean/number  | Flags to skip certain highlighting rules.                                  |
| bounce     | boolean         | If `true`, applies debouncing to the highlighting process.                 |

**Return Value:** None.

**Mechanism:**
- Checks for content changes to avoid unnecessary highlighting.
- Uses a state machine to apply syntax rules based on the current mode.
- Supports debouncing to improve performance on rapid content changes.

**Usage:**
- Applying syntax highlighting to code editors.
- Enhancing readability of code snippets.

**Example:**
```javascript
// Highlight a textarea as HTML
ifc_highlight(document.getElementById("html_editor"), 0);

// Highlight a contenteditable element as CSS
ifc_highlight(document.getElementById("css_editor"), 10);
```

---

### `ifc_highlight_bracket(object)`

Highlights matching brackets in a syntax-highlighted element.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| object    | object | DOM element containing highlighted content.                               |

**Return Value:** None.

**Mechanism:**
- Identifies the current cursor position and finds matching brackets.
- Applies the `active` class to matching bracket pairs.

**Usage:**
- Assisting users in identifying matching brackets in code.
- Improving code readability and debugging.

**Example:**
```javascript
// Highlight matching brackets in a code editor
ifc_highlight_bracket(document.getElementById("code_editor"));
```

---

### `ifc_save_selection(context)`

Saves the current selection in a contenteditable element.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| context   | object | DOM element (contenteditable).                                             |

**Return Value:** function

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| function  | A function to restore the saved selection.                                 |

**Mechanism:**
- Captures the current selection range and stores it for later restoration.

**Usage:**
- Preserving cursor positions during content updates.
- Ensuring consistent user experience after dynamic content changes.

**Example:**
```javascript
// Save the current selection in a contenteditable element
const restoreSelection = ifc_save_selection(document.getElementById("editor"));

// Restore the selection after content updates
restoreSelection();
```

---

### `ifc_get_position_in_context(context, position)`

Converts a text position to a DOM node and offset within a contenteditable element.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| context   | object | DOM element (contenteditable).                                             |
| position  | number | Text position to convert.                                                  |

**Return Value:** object

| Property  | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| node      | object | The DOM node at the specified position.                                    |
| position  | number | The offset within the node.                                                |

**Mechanism:**
- Uses a `TreeWalker` to navigate the DOM and find the node at the specified position.

**Usage:**
- Internal use by selection management functions.
- Converting text positions to DOM coordinates.

**Example:**
```javascript
// Internal use only
const position = ifc_get_position_in_context(editor, 100);
```

---

### `ifc_contenteditable_init(id)`

Initializes a contenteditable element with textarea-like properties.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| id        | string | ID of the contenteditable element.                                         |

**Return Value:** None.

**Mechanism:**
- Adds a placeholder element.
- Defines custom properties (`type`, `value`, `select`) to mimic textarea behavior.
- Sets up event listeners for syntax highlighting.

**Usage:**
- Creating rich text editors with textarea-like properties.
- Enhancing contenteditable elements for code editing.

**Example:**
```javascript
// Initialize a contenteditable element as a code editor
ifc_contenteditable_init("code_editor");
```

---

## Undo/Redo Functions

Functions for managing undo and redo functionality in textareas and contenteditable elements.

### `ifc_state_save(object)`

Saves the current state of a textarea or contenteditable element.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| object    | object | DOM element (textarea or contenteditable).                                 |

**Return Value:** None.

**Mechanism:**
- Stores the current value and selection range in the undo stack.
- Limits the undo stack to 100 states.

**Usage:**
- Enabling undo functionality for text editing.
- Preserving edit history for user recovery.

**Example:**
```javascript
// Save the current state of a textarea
ifc_state_save(document.getElementById("editor"));
```

---

### `ifc_state_undo(object, redo = false)`

Undoes or redoes the last action in a textarea or contenteditable element.

| Parameter | Type    | Description                                                                 |
|-----------|---------|-----------------------------------------------------------------------------|
| object    | object  | DOM element (textarea or contenteditable).                                 |
| redo      | boolean | If `true`, performs a redo operation.                                      |

**Return Value:** None.

**Mechanism:**
- Moves the current state between the undo and redo stacks.
- Restores the previous value and selection range.

**Usage:**
- Implementing undo/redo functionality in text editors.
- Allowing users to revert or reapply changes.

**Example:**
```javascript
// Undo the last action in a textarea
ifc_state_undo(document.getElementById("editor"));

// Redo the last undone action
ifc_state_undo(document.getElementById("editor"), true);
```

---

### `ifc_state_redo(object)`

Redoes the last undone action in a textarea or contenteditable element.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| object    | object | DOM element (textarea or contenteditable).                                 |

**Return Value:** None.

**Mechanism:**
- Calls `ifc_state_undo()` with `redo` set to `true`.

**Usage:**
- Implementing redo functionality in text editors.
- Allowing users to reapply undone changes.

**Example:**
```javascript
// Redo the last undone action in a textarea
ifc_state_redo(document.getElementById("editor"));
```

---

### `ifc_state_get_range(object)`

Retrieves the current selection range in a textarea or contenteditable element.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| object    | object | DOM element (textarea or contenteditable).                                 |

**Return Value:** Array

| Index | Type   | Description                                                                 |
|-------|--------|-----------------------------------------------------------------------------|
| 0     | number | Start position of the selection.                                           |
| 1     | number | End position of the selection.                                             |

**Mechanism:**
- For textareas, uses `selectionStart` and `selectionEnd`.
- For contenteditable elements, calculates the selection range based on the DOM.

**Usage:**
- Internal use by undo/redo functions.
- Capturing the current selection state.

**Example:**
```javascript
// Internal use only
const range = ifc_state_get_range(document.getElementById("editor"));
```

---

### `ifc_state_set_range(object, start, end)`

Sets the selection range in a textarea or contenteditable element.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| object    | object | DOM element (textarea or contenteditable).                                 |
| start     | number | Start position of the selection.                                           |
| end       | number | End position of the selection.                                             |

**Return Value:** None.

**Mechanism:**
- For textareas, sets `selectionStart` and `selectionEnd`.
- For contenteditable elements, converts text positions to DOM coordinates and sets the selection.

**Usage:**
- Internal use by undo/redo functions.
- Restoring the selection state.

**Example:**
```javascript
// Internal use only
ifc_state_set_range(document.getElementById("editor"), 10, 20);
```

---

### `ifc_state_purge(object)`

Clears the undo and redo stacks for a textarea or contenteditable element.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| object    | object | DOM element (textarea or contenteditable).                                 |

**Return Value:** None.

**Mechanism:**
- Removes all stored states from the undo and redo stacks.

**Usage:**
- Resetting the edit history for a text element.
- Clearing memory after significant content changes.

**Example:**
```javascript
// Clear the undo/redo history for a textarea
ifc_state_purge(document.getElementById("editor"));
```

---

## File Upload Progress Function

### `ifc_show_upload_progress(form)`

Displays upload progress for file inputs in a form.

| Parameter | Type   | Description                                                                 |
|-----------|--------|-----------------------------------------------------------------------------|
| form      | object | DOM form element.                                                          |

**Return Value:** None.

**Mechanism:**
- Checks for file inputs in the form.
- Overrides the form submission to display upload progress using `XMLHttpRequest`.
- Shows a progress bar for each file input.

**Usage:**
- Providing visual feedback during file uploads.
- Enhancing user experience for large file uploads.

**Example:**
```javascript
// Display upload progress for a form
ifc_show_upload_progress(document.getElementById("upload_form"));
```


<!-- HASH:4ce402f13233e39e13b6813ebaa5cb52 -->
