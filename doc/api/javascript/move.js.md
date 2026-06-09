# PWNC API Documentation

[← Index](../README.md) | [`javascript/move.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/move.js)

- **Version:** `26.5.30.4`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Move Module (`move.js`)

Core drag-and-drop utility for moving DOM elements via mouse interaction. Designed for lightweight, dependency-free repositioning of any registered element with optional fixed positioning and automatic scroll-at-border behavior.

---

### Global Variables

| Name            | Default | Description                                                                 |
|-----------------|---------|-----------------------------------------------------------------------------|
| `mv_object`     | `null`  | Currently dragged element or `null` if none.                               |
| `mv_left`       | `0`     | Initial mouse X position at drag start.                                    |
| `mv_top`        | `0`     | Initial mouse Y position at drag start.                                    |
| `mv_offset_left`| `0`     | Horizontal offset between mouse and element’s left edge.                   |
| `mv_offset_top` | `0`     | Vertical offset between mouse and element’s top edge.                      |
| `mv_fixed`      | `false` | `true` if element uses `position: fixed`.                                  |
| `mv_flag`       | `false` | `true` once element has been moved >10px from start (prevents accidental drags). |
| `mv_scroll_flag`| `false` | `true` while auto-scroll is active.                                        |
| `mv_scroll_x`   | `0`     | Horizontal scroll speed (pixels per frame).                                |
| `mv_scroll_y`   | `0`     | Vertical scroll speed (pixels per frame).                                  |

---

### `mv_register(object)`

Registers a DOM element for drag-and-drop movement.

#### Parameters

| Name    | Type               | Description                                                                 |
|---------|--------------------|-----------------------------------------------------------------------------|
| `object`| `string` \| `HTMLElement` | Element ID or DOM object. If string, `document.getElementById()` is used. |

#### Return Value

| Type    | Description                                                                 |
|---------|-----------------------------------------------------------------------------|
| `boolean` | `true` on success, `false` if element is invalid or not found.             |

#### Inner Mechanisms

1. Converts string ID to DOM object.
2. Blocks native drag (`ondragstart`) and text selection (`onselectstart`).
3. Marks element with `mv_enabled = true`.
4. Sets default title “✥ Move” if none exists.

#### Usage Example

```javascript
// Register a modal dialog for movement
mv_register("modal-dialog");
```

---

### `mv_get_object()`

Finds the nearest registered ancestor of the current pointer target.

#### Return Value

| Type      | Description                                                                 |
|-----------|-----------------------------------------------------------------------------|
| `HTMLElement` \| `null` | Registered element or `null` if none found or pointer is over `mv_object`. |

#### Inner Mechanisms

1. Uses `fx_pointer_object()` to get element under pointer.
2. Walks up the DOM tree until a registered element (`mv_enabled`) is found.
3. Returns `null` if the found element is the currently dragged one (`mv_object`).

#### Usage Example

```javascript
// Check if pointer is over a movable element
const target = mv_get_object();
if (target) console.log("Hovering over movable element:", target.id);
```

---

### `mv_move_object()`

Repositions `mv_object` to the current mouse coordinates, adjusted for offsets and fixed positioning.

#### Inner Mechanisms

1. Calculates new coordinates: `mouseX - mv_offset_left`, `mouseY - mv_offset_top`.
2. If `mv_fixed`, subtracts window scroll position (`fx_window_left`, `fx_window_top`).
3. Delegates actual movement to `fx_move()`.

#### Usage Example

```javascript
// Manually trigger a move (e.g., after programmatic offset change)
mv_offset_left = 50;
mv_move_object();
```

---

### `mv_scroll()`

Auto-scrolls the container when the mouse is near the viewport edge, then updates the dragged element’s position.

#### Inner Mechanisms

1. Sets `mv_scroll_flag = true`.
2. Stores current scroll position.
3. Uses `scrollBy()` with `behavior: "instant"`.
4. Temporarily hides `mv_object` to measure new scroll position.
5. If scroll position changed, calls `mv_move_object()` and schedules next frame via `fx_animation_frame()`.

#### Usage Example

```javascript
// Simulate mouse near bottom edge
mv_scroll_y = 10;
mv_scroll();
```

---

### `mv_event(event)`

Central event dispatcher for mouse interactions.

#### Parameters

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `event` | `string` | Event type: `"mousedown"`, `"mousemove"`, `"mouseup"`, `"mouseleave"`.     |

#### Inner Mechanisms

| Event         | Logic                                                                       |
|---------------|-----------------------------------------------------------------------------|
| `mousedown`   | Validates left mouse button, finds target, sets offsets, disables scrolling. |
| `mousemove`   | Starts move after 10px threshold, enables scroll-at-border, updates position. |
| `mouseup`/`mouseleave` | Cleans up: resets flags, re-enables scrolling, restores cursor.          |

#### Usage Example

```javascript
// Manually trigger a mouseup to cancel drag
mv_event("mouseup");
```

---

### Integration Notes

- **Dependencies**: Relies on `fx_*` utility functions (`fx_pointer_object`, `fx_mouse_*`, `fx_move`, `fx_style`, `fx_noscroll`, `fx_pointer_block`, `fx_animation_frame`).
- **Initialization**: Automatically hooks into the global event loop via `fx_register_callback(mv_event)`.
- **Styling**: Registered elements should have `position: absolute` or `position: fixed` for movement to work.


<!-- HASH:47dc22be6ceff1629d5572cab1c33d99 -->
