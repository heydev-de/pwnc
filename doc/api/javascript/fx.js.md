# PWNC API Documentation

[← Index](../README.md) | [`javascript/fx.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/fx.js)

- **Version:** `26.6.9.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## FX.js - Frontend Animation and Interaction Utilities

**Overview**
`fx.js` provides a comprehensive set of utilities for DOM manipulation, animation, event handling, and touch interaction within the PWNC Web Platform. It follows a zero-dependency, high-performance approach with a focus on smooth animations, precise element positioning, and robust event management. The module includes:

- **Animation control** (requestAnimationFrame scheduling)
- **Element manipulation** (positioning, visibility, styling)
- **Window management** (scrolling, resizing, focus control)
- **Element positioning and dimension utilities** (relative/absolute coordinates, cropping-aware measurements)
- **Touch and gesture support** (swipe, pinch-to-zoom, drag)
- **Advanced event system** (debouncing, ghost event prevention, multi-callback registration)
- **Global state tracking** (mouse/touch/keyboard positions, document/window dimensions)

---

## Animation

### `fx_animation_frame(callback, delay = 0)`

**Purpose**
Schedules a callback function to execute on the next animation frame, optionally after a delay. Supports both function objects and string-based function definitions.

| Parameter | Type | Description |
|-----------|------|-------------|
| `callback` | `function` or `string` | Function to execute or string to be converted to a function. |
| `delay` | `number` | Milliseconds to wait before scheduling the animation frame. |

**Return Value**
- `number`: ID of the scheduled timeout or animation frame (for cancellation).

**Inner Mechanisms**
- If `callback` is a string, it is converted to a function using `new Function()`.
- If `delay > 0`, the animation frame is scheduled after the delay using `setTimeout`.
- Otherwise, `requestAnimationFrame` is called directly.

**Usage Context**
Use for smooth, performance-optimized animations. Ideal for UI transitions, scroll effects, or any visual change that should be synchronized with the browser's repaint cycle.

**Example**
```javascript
// Fade in an element after 500ms
fx_animation_frame(() => {
    document.getElementById("modal").style.opacity = 1;
}, 500);
```

---

## Element Manipulation

### `fx_move(object, left, top)`

**Purpose**
Moves a DOM element to the specified pixel coordinates. Ensures integer values and only updates styles if the position has changed.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | Element ID or DOM object. |
| `left` | `number` | Target left position in pixels. |
| `top` | `number` | Target top position in pixels. |

**Return Value**
- `void`

**Inner Mechanisms**
- Resolves `object` to a DOM element if a string is provided.
- Floors `left` and `top` to integers.
- Compares current computed style with target values and updates only if different.

**Usage Context**
Use for repositioning elements dynamically, such as draggable UI components, tooltips, or custom dropdowns.

**Example**
```javascript
// Move a popup to 100px from the left and 200px from the top
fx_move("popup", 100, 200);
```

---

### `fx_style(object, property, value = null, priority = false)`

**Purpose**
Gets or sets a CSS style property on a DOM element. Supports removal of properties and `!important` priority.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | Element ID or DOM object. |
| `property` | `string` | CSS property name (e.g., `"color"`, `"margin-left"`). |
| `value` | `string` or `boolean` | Value to set. If `""` or `false`, the property is removed. |
| `priority` | `boolean` | If `true`, adds `!important` to the style. |

**Return Value**
- `string` or `boolean`: Current property value (when getting), or `true` on successful set.

**Inner Mechanisms**
- Resolves `object` to a DOM element.
- If `value` is `null`, returns the computed style value.
- If `value` is `""` or `false`, removes the property using `removeProperty()`.
- Otherwise, sets the property with optional `!important`.

**Usage Context**
Use for dynamic styling, theming, or responsive UI adjustments.

**Example**
```javascript
// Set background color with !important
fx_style("header", "background-color", "#333", true);

// Remove a style
fx_style("footer", "border", false);

// Get current font size
const size = fx_style("content", "font-size");
```

---

### `fx_visible(object, set = null)`

**Purpose**
Gets or sets the visibility of a DOM element.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | Element ID or DOM object. |
| `set` | `boolean` | If `true`, makes element visible; if `false`, hides it. |

**Return Value**
- `boolean`: `true` if element is visible, `false` otherwise.

**Inner Mechanisms**
- Uses `fx_style()` to get or set the `visibility` property.

**Usage Context**
Use for toggling UI elements without affecting layout (unlike `display: none`).

**Example**
```javascript
// Hide a tooltip
fx_visible("tooltip", false);

// Check if a modal is visible
if (fx_visible("modal")) {
    console.log("Modal is visible");
}
```

---

### `fx_change_image(object, image_url)`

**Purpose**
Changes the `src` attribute of an `<img>` element.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | Element ID or DOM object. |
| `image_url` | `string` | New image URL. |

**Return Value**
- `void`

**Inner Mechanisms**
- Only applies to elements with `tagName === "IMG"`.

**Usage Context**
Use for dynamic image loading, galleries, or lazy-loaded content.

**Example**
```javascript
// Change a product image
fx_change_image("product-img", "/images/product2.jpg");
```

---

## Window Manipulation

### `fx_scrollto(object)`

**Purpose**
Smoothly scrolls the window or scrollable container to bring a DOM element into view, with a slight offset for better UX.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | Element ID or DOM object. |

**Return Value**
- `void`

**Inner Mechanisms**
- Uses a custom easing animation with 200 frames and damping for smoothness.
- Calculates target position with a 20% vertical offset from the top of the viewport.
- Uses `fx_animation_frame` to schedule scroll updates.

**Usage Context**
Use for navigation links, "back to top" buttons, or any in-page navigation.

**Example**
```javascript
// Scroll to a section
fx_scrollto("section-3");
```

---

### `fx_adjust_window(object = window)`

**Purpose**
Adjusts the size and position of a popup window to fit its content and screen.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `Window` | Window object (default: current window). |

**Return Value**
- `void`

**Inner Mechanisms**
- Only works on child windows with an `opener`.
- Uses `fx_document_size()` to measure content dimensions.
- Resizes and repositions the window to fit within screen bounds.
- Focuses the window after adjustment.

**Usage Context**
Use for popup windows, modals, or external tool windows to ensure they are usable and visible.

**Example**
```javascript
// Open and adjust a popup
const popup = window.open("/help", "Help", "width=800,height=600");
popup.onload = () => fx_adjust_window(popup);
```

---

## Element Positioning

### `fx_left(object, relative = false)`

**Purpose**
Gets the left position of an element, either relative to its offset parent or the document.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | Element ID or DOM object. |
| `relative` | `boolean` | If `true`, returns `offsetLeft`; otherwise, returns document-relative position. |

**Return Value**
- `number`: Left position in pixels.

**Inner Mechanisms**
- If `relative`, returns `offsetLeft`.
- Otherwise, uses `fx_offset_left()`.

**Usage Context**
Use for positioning tooltips, context menus, or drag-and-drop logic.

**Example**
```javascript
// Get document-relative left position
const left = fx_left("sidebar");
```

---

### `fx_top(object, relative = false)`

**Purpose**
Gets the top position of an element, either relative to its offset parent or the document.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | Element ID or DOM object. |
| `relative` | `boolean` | If `true`, returns `offsetTop`; otherwise, returns document-relative position. |

**Return Value**
- `number`: Top position in pixels.

**Inner Mechanisms**
- If `relative`, returns `offsetTop`.
- Otherwise, uses `fx_offset_top()`.

**Usage Context**
Use alongside `fx_left()` for precise element positioning.

**Example**
```javascript
const top = fx_top("header");
```

---

### `fx_offset_left(object, no_cropping = false)`

**Purpose**
Gets the left position of an element relative to the document, optionally ignoring parent clipping.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | Element ID or DOM object. |
| `no_cropping` | `boolean` | If `true`, ignores parent overflow clipping. |

**Return Value**
- `number`: Left position in pixels.

**Inner Mechanisms**
- Uses `getBoundingClientRect()` to get viewport-relative position.
- If `no_cropping`, adds current scroll position.
- Otherwise, walks up the DOM tree to find the maximum left edge (to handle `overflow: hidden`).

**Usage Context**
Use for accurate positioning in complex layouts with nested scrollable containers.

**Example**
```javascript
const left = fx_offset_left("menu", true);
```

---

### `fx_offset_top(object, no_cropping = false)`

**Purpose**
Gets the top position of an element relative to the document, optionally ignoring parent clipping.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | Element ID or DOM object. |
| `no_cropping` | `boolean` | If `true`, ignores parent overflow clipping. |

**Return Value**
- `number`: Top position in pixels.

**Inner Mechanisms**
- Similar to `fx_offset_left()`, but for vertical positioning.

**Usage Context**
Use for scroll-spy, parallax effects, or sticky elements.

**Example**
```javascript
const top = fx_offset_top("banner");
```

---

## Element Dimensions

### `fx_width(object, no_cropping = false)`

**Purpose**
Gets the width of an element, optionally ignoring parent clipping.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | Element ID or DOM object. |
| `no_cropping` | `boolean` | If `true`, returns full width; otherwise, clips to visible area. |

**Return Value**
- `number`: Width in pixels.

**Inner Mechanisms**
- Uses `getBoundingClientRect()`.
- If `no_cropping`, returns `rect.width`.
- Otherwise, walks up the DOM to find the minimum right edge and maximum left edge.

**Usage Context**
Use for responsive design, drag handles, or layout calculations.

**Example**
```javascript
const width = fx_width("content");
```

---

### `fx_height(object, no_cropping = false)`

**Purpose**
Gets the height of an element, optionally ignoring parent clipping.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | Element ID or DOM object. |
| `no_cropping` | `boolean` | If `true`, returns full height; otherwise, clips to visible area. |

**Return Value**
- `number`: Height in pixels.

**Inner Mechanisms**
- Similar to `fx_width()`, but for height.

**Usage Context**
Use for dynamic resizing, scrollable containers, or aspect ratio calculations.

**Example**
```javascript
const height = fx_height("sidebar");
```

---

## Window Positioning

### `fx_position_left()`

**Purpose**
Gets the horizontal scroll position as a percentage of the total document width.

**Return Value**
- `number`: Percentage (0–100).

**Inner Mechanisms**
- Uses `fx_window_left` and `fx_document_width`.

**Usage Context**
Use for progress indicators, scroll-linked animations, or analytics.

**Example**
```javascript
const scrollPercent = fx_position_left();
console.log(`Scrolled ${scrollPercent}% horizontally`);
```

---

### `fx_position_top()`

**Purpose**
Gets the vertical scroll position as a percentage of the total document height.

**Return Value**
- `number`: Percentage (0–100).

**Inner Mechanisms**
- Uses `fx_window_top` and `fx_document_height`.

**Usage Context**
Use for scroll progress bars or lazy loading.

**Example**
```javascript
const scrollPercent = fx_position_top();
```

---

## Document Dimensions

### `fx_document_size(object = window)`

**Purpose**
Calculates the total dimensions of the document content, including all elements.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `Window` | Window object (default: current window). |

**Return Value**
- `object` or `null`: `{ width: number, height: number }` or `null` if no content.

**Inner Mechanisms**
- Walks the DOM tree using a stack-based approach.
- Skips elements with `transform` or `auto` dimensions.
- Accumulates maximum right and bottom edges.

**Usage Context**
Use for full-page layouts, print previews, or content measurement.

**Example**
```javascript
const size = fx_document_size();
console.log(`Document size: ${size.width}x${size.height}`);
```

---

## Swipe Functionality

### `fx_swipe(object, callback)`

**Purpose**
Enables swipe detection (left, right, up, down) on a DOM element.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | Element ID or DOM object. |
| `callback` | `function` | Called with `(object, direction)` when a swipe is detected. |

**Return Value**
- `void`

**Inner Mechanisms**
- Tracks touch/mouse start, move, and end events.
- Calculates direction based on displacement (minimum 20px).
- Supports both mouse and touch input.
- Prevents text selection and drag during swipe.

**Usage Context**
Use for carousels, image galleries, or mobile-friendly navigation.

**Example**
```javascript
fx_swipe("gallery", (el, dir) => {
    if (dir === "l") showNextImage();
    if (dir === "r") showPrevImage();
});
```

---

## Move & Zoom (Pinch) Functionality

### `fx_move_zoom(object, callback)`

**Purpose**
Enables drag-to-move and pinch-to-zoom on a DOM element.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string` or `HTMLElement` | Element ID or DOM object. |
| `callback` | `function` | Called with `(object, vx, vy, z, zx, zy)` on interaction. |

**Parameters in Callback**
| Parameter | Type | Description |
|-----------|------|-------------|
| `vx` | `number` | Horizontal movement (pixels). |
| `vy` | `number` | Vertical movement (pixels). |
| `z` | `number` | Zoom delta (positive = zoom in). |
| `zx` | `number` | X-coordinate of zoom center. |
| `zy` | `number` | Y-coordinate of zoom center. |

**Return Value**
- `void`

**Inner Mechanisms**
- Supports mouse drag, touch drag, mouse wheel, and pinch gestures.
- Implements inertia/flick effect after release.
- Prevents default behavior to avoid scrolling.

**Usage Context**
Use for interactive maps, image viewers, or custom canvas-based editors.

**Example**
```javascript
fx_move_zoom("map", (el, vx, vy, z, zx, zy) => {
    if (z !== 0) zoomMap(z, zx, zy);
    else moveMap(vx, vy);
});
```

---

## Miscellaneous

### `fx_pointer_block(set = true)`

**Purpose**
Globally disables pointer events on all elements (for modal overlays or loading states).

| Parameter | Type | Description |
|-----------|------|-------------|
| `set` | `boolean` | If `true`, blocks all pointer events; if `false`, restores them. |

**Return Value**
- `void`

**Inner Mechanisms**
- Injects or removes a `<style>` element with `pointer-events: none` on all elements.

**Usage Context**
Use during AJAX loading, modals, or critical operations to prevent user interaction.

**Example**
```javascript
// Block interaction during save
fx_pointer_block(true);
saveData().finally(() => fx_pointer_block(false));
```

---

### `fx_pointer_object()`

**Purpose**
Gets the DOM element under the mouse pointer, even if pointer events are blocked.

**Return Value**
- `HTMLElement` or `null`: The element at the current mouse position.

**Inner Mechanisms**
- Temporarily disables the pointer-blocking style.
- Uses `document.elementFromPoint()`.

**Usage Context**
Use for tooltips, hover effects, or debugging.

**Example**
```javascript
const element = fx_pointer_object();
if (element) console.log("Hovering over:", element.id);
```

---

## Global Event Values

| Variable | Type | Description |
|----------|------|-------------|
| `fx_document_width` | `number` | Total document width. |
| `fx_document_height` | `number` | Total document height. |
| `fx_window_left` | `number` | Current horizontal scroll position. |
| `fx_window_top` | `number` | Current vertical scroll position. |
| `fx_window_width` | `number` | Viewport width. |
| `fx_window_height` | `number` | Viewport height. |
| `fx_mouse_key` | `number` | Current mouse button (1=left, 2=middle, 3=right). |
| `fx_mouse_x` | `number` | Mouse X position relative to document. |
| `fx_mouse_y` | `number` | Mouse Y position relative to document. |
| `fx_mouse_window_x` | `number` | Mouse X position relative to viewport. |
| `fx_mouse_window_y` | `number` | Mouse Y position relative to viewport. |
| `fx_touch1_x`, `fx_touch1_y` | `number` | First touch position (document-relative). |
| `fx_touch2_x`, `fx_touch2_y` | `number` | Second touch position (document-relative). |
| `fx_keyboard_key` | `number` | Last key code pressed. |
| `fx_event_object` | `Event` | Current event object. |
| `fx_scroll_container` | `HTMLElement` or `Window` | Scrollable container (window or body). |

---

## Event Management

### `fx_event_listen(object, event, _function = null, passive = true, capture = false)`

**Purpose**
Registers an event listener on one or more objects or events. Supports both DOM events and custom FX events.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `string`, `HTMLElement`, or `array` | Element ID, DOM object, or array of objects. |
| `event` | `string` or `array` | Event name(s) (e.g., `"click"`, `"mousemove"`). |
| `_function` | `function` | Callback function. |
| `passive` | `boolean` | If `true`, marks listener as passive. |
| `capture` | `boolean` | If `true`, uses capture phase. |

**Return Value**
- `void`

**Inner Mechanisms**
- Supports arrays of objects or events.
- Maps standard DOM events to FX event names (e.g., `"load"` → `"window_load"`).
- For FX events, registers the function in the callback system.

**Usage Context**
Use for consistent event handling across the platform.

**Example**
```javascript
// Listen to multiple events
fx_event_listen("button", ["click", "mouseenter"], (e) => {
    console.log("Button event:", e.type);
});

// Listen on multiple elements
fx_event_listen(["btn1", "btn2"], "click", handleClick);
```

---

### `fx_event_remove(object, event = "", _function = null)`

**Purpose**
Removes an event listener or unregisters a callback from FX events.

| Parameter | Type | Description |
|-----------|------|-------------|
| `object` | `HTMLElement` or `function` | DOM object or callback function. |
| `event` | `string` | Event name (optional for FX callbacks). |
| `_function` | `function` | Callback function (for DOM events). |

**Return Value**
- `void`

**Inner Mechanisms**
- For FX callbacks, removes the event from the function's `fx_event` list and unregisters if no events remain.
- For DOM events, calls `removeEventListener()`.

**Usage Context**
Use for cleanup or dynamic event management.

**Example**
```javascript
// Remove FX event
fx_event_remove(handleResize, "window_resize");

// Remove DOM event
fx_event_remove(document, "click", handleClick);
```

---

### `fx_event_debounce(event, e)`

**Purpose**
Debounces high-frequency events (e.g., `resize`, `scroll`) to improve performance.

| Parameter | Type | Description |
|-----------|------|-------------|
| `event` | `string` | Event name. |
| `e` | `Event` | Event object. |

**Return Value**
- `void`

**Inner Mechanisms**
- Limits event processing to 30 FPS.
- Uses a global debounce list to track event state.

**Usage Context**
Use for performance-critical event handlers.

**Example**
```javascript
// Debounced scroll handler
fx_event_listen(window, "scroll", (e) => {
    fx_event_debounce("window_scroll", e);
});
```

---

### `fx_ghost_buster()`

**Purpose**
Prevents "ghost clicks" on touch devices by blocking mouse events that follow touch events.

**Return Value**
- `void`

**Inner Mechanisms**
- Uses an `AbortController` to manage event listeners.
- Blocks `mouseover`, `mousemove`, `mousedown`, `mouseup`, and `click` events unless they originate from a synthetic touch event.

**Usage Context**
Use on touch-enabled applications to avoid double-triggering of events.

**Example**
```javascript
// Enable ghost click prevention
fx_ghost_buster();
```

---

## Event Initialization

The module automatically initializes global event listeners for:
- Window: `load`, `resize`, `beforeunload`
- Document: `DOMContentLoaded`, `mousedown`, `mousemove`, `mouseup`, `mouseleave`, `dblclick`, `touchstart`, `touchmove`, `touchend`, `touchcancel`, `keydown`, `keypress`, `keyup`

These are mapped to FX event names and processed through the event system.


<!-- HASH:2aa429bbc24acd7efb47c7465b928185 -->
