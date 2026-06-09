# PWNC API Documentation

[← Index](../README.md) | [`javascript/dragdrop.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/dragdrop.js)

- **Version:** `26.5.30.4`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Drag & Drop Module (`dragdrop.js`)

This module provides a lightweight, custom drag-and-drop (DnD) system for the PWNC Web Platform. It enables interactive dragging of DOM elements with support for:
- **Type-based acceptance** (bitmask matching between draggable and droppable elements)
- **Visual feedback** (cloning, styling, and cursor changes)
- **Event callbacks** (activation, dragging, dropping, and alternative actions)
- **Edge scrolling** (auto-scroll when dragging near viewport edges)
- **Modifier key support** (Shift/Ctrl/Alt for alternative drop actions)

The module integrates with PWNC’s core `fx_*` utilities (e.g., `fx_pointer_object()`, `fx_style()`) and avoids dependencies on native HTML5 DnD APIs to maintain full control over behavior and styling.

---

### Global Variables

| Name            | Default | Description                                                                 |
|-----------------|---------|-----------------------------------------------------------------------------|
| `dd_object`     | `null`  | Currently dragged source element.                                           |
| `dd_vehicle`    | `null`  | Cloned visual representation of the dragged element (floating overlay).    |
| `dd_touched`    | `null`  | Current target element under the cursor (potential drop zone).              |
| `dd_callback`   | `null`  | User-defined callback function for drag events.                             |
| `dd_left`/`dd_top` | `0`   | Initial mouse coordinates when dragging started.                            |
| `dd_nofx`       | `false` | Flag to disable visual effects (e.g., `dd-active` class) for the target.   |
| `dd_flag`       | `false` | Modifier key state (Shift/Ctrl/Alt) during drag.                            |
| `dd_scroll_flag`| `false` | Flag to track active auto-scrolling.                                       |
| `dd_scroll_x`/`dd_scroll_y` | `0` | Auto-scroll direction and speed.                                   |

---

### Functions

---

#### `dd_register(object, type, accept, fixed = false, nofx = false)`

Registers a DOM element for drag-and-drop operations.

##### Parameters

| Name     | Type               | Description                                                                 |
|----------|--------------------|-----------------------------------------------------------------------------|
| `object` | `string`\|`object` | Element ID or DOM object to register.                                      |
| `type`   | `number`           | Bitmask representing the element’s drag type (e.g., `0b0001` for "file").  |
| `accept` | `number`           | Bitmask of types the element can accept as a drop target.                  |
| `fixed`  | `boolean`          | If `true`, the element cannot be dragged (acts only as a drop target).     |
| `nofx`   | `boolean`          | If `true`, disables visual feedback (e.g., `dd-active` class) for the target. |

##### Return Value
- **`boolean`**: `true` if registration succeeded, `false` if the element was invalid.

##### Inner Mechanisms
1. **Validation**: Converts string IDs to DOM objects and checks for `null`.
2. **Properties**: Attaches `dd_*` properties to the element (e.g., `dd_enabled`, `dd_type`).
3. **Events**: Disables native `dragstart` and `selectstart` to prevent conflicts.
4. **Tooltips**: Sets a tooltip (✥) indicating the element’s role (drag, drop, or both).

##### Usage Example
```javascript
// Register a draggable file element (type 1) that accepts files (type 1) and folders (type 2)
dd_register("file1", 0b0001, 0b0011);

// Register a fixed drop zone (type 2) that only accepts folders
dd_register("folder-zone", 0b0010, 0b0010, true);
```

---

#### `dd_set_callback(callback)`

Sets the user-defined callback function for drag-and-drop events.

##### Parameters

| Name       | Type       | Description                                                                 |
|------------|------------|-----------------------------------------------------------------------------|
| `callback` | `function` | Function to handle events. Signature: `(event, source, target) => void`.   |

##### Event Types
| Event            | Description                                                                 |
|------------------|-----------------------------------------------------------------------------|
| `"activate"`     | Single-click on a draggable element (no drag).                              |
| `"select"`       | Single-click with a modifier key (Shift/Ctrl/Alt).                         |
| `"beforedragstart"` | Before dragging starts (cancelable).                                    |
| `"dragstart"`    | Dragging has started.                                                       |
| `"dragover"`     | Cursor is over a valid drop target.                                        |
| `"drag"`         | Cursor is over an invalid area.                                             |
| `"dropon"`       | Element dropped on a valid target.                                          |
| `"dropon_alt"`   | Element dropped on a valid target with a modifier key.                      |
| `"drop"`         | Element dropped on an invalid area.                                         |
| `"drop_alt"`     | Element dropped on an invalid area with a modifier key.                     |
| `"dblclick"`     | Double-click on an element.                                                 |

##### Usage Example
```javascript
function handleDragEvent(event, source, target) {
    switch (event) {
        case "dropon":
            console.log(`Moved ${source.id} to ${target.id}`);
            target.appendChild(source);
            break;
        case "drop_alt":
            console.log(`Copied ${source.id} to new location`);
            const clone = source.cloneNode(true);
            document.body.appendChild(clone);
            break;
    }
}
dd_set_callback(handleDragEvent);
```

---

#### `dd_get_object()`

Retrieves the topmost registered drag-and-drop element under the cursor.

##### Return Value
- **`object`\|`null`**: The DOM element or `null` if none is found.

##### Inner Mechanisms
1. Uses `fx_pointer_object()` to get the element under the cursor.
2. Traverses up the DOM tree to find the nearest parent with `dd_enabled: true`.
3. Excludes the currently dragged source (`dd_object`).

##### Usage Example
```javascript
// Check if the cursor is over a drop target
const target = dd_get_object();
if (target && (target.dd_accept & 0b0001)) {
    console.log("Hovering over a file drop zone");
}
```

---

#### `dd_move_vehicle()`

Updates the position of the drag vehicle (cloned overlay) to follow the cursor.

##### Inner Mechanisms
- Uses `fx_move()` to position the vehicle at `(mouse_x + 10, mouse_y + 5)` for visual offset.

---

#### `dd_scroll()`

Handles auto-scrolling when the cursor is near the viewport edges during dragging.

##### Inner Mechanisms
1. **Edge Detection**: Checks if the cursor is within 100px of the viewport edges.
2. **Scrolling**: Uses `scrollBy()` with `behavior: "instant"` for smooth movement.
3. **Reflow**: Forces a layout recalculation to ensure accurate positioning.
4. **Recursion**: Uses `fx_animation_frame()` to repeat scrolling every 25ms.

---

#### `dd_event(event)`

Core event handler for mouse interactions. Manages the drag-and-drop lifecycle.

##### Parameters

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `event` | `string` | Event type: `"mousedown"`, `"mousemove"`, `"mouseup"`, `"mouseleave"`, `"dblclick"`. |

##### Inner Mechanisms
- **`mousedown`**:
  - Validates the source element and mouse button (left-click only).
  - Stores initial mouse position and modifier key state.
  - Disables native scrolling via `fx_noscroll()`.
- **`mousemove`**:
  - **Drag Initiation**: Creates the vehicle clone if the cursor moves >10px from the start.
  - **Edge Scrolling**: Triggers `dd_scroll()` if near viewport edges.
  - **Target Detection**: Updates `dd_touched` and applies visual feedback (`dd-active` class).
  - **Callbacks**: Fires `dragover` or `drag` events based on target validity.
- **`mouseup`**:
  - **Drop Handling**: Fires `dropon`/`drop` or `dropon_alt`/`drop_alt` based on target and modifier keys.
  - **Cleanup**: Removes the vehicle, resets cursors, and re-enables scrolling.
- **`mouseleave`**:
  - Aborts the drag operation and performs cleanup.
- **`dblclick`**:
  - Fires the `dblclick` callback if a registered element is double-clicked.

##### Usage Example
```javascript
// The function is automatically registered via fx_register_callback(dd_event).
// No direct usage required.
```

---

### Integration Notes

1. **Dependencies**:
   - Requires `fx_*` utilities (e.g., `fx_pointer_object()`, `fx_style()`, `fx_noscroll()`).
   - Assumes `fx_register_callback()` is available to hook into mouse events.

2. **Styling**:
   - The vehicle uses the `.dd-vehicle` class for styling. Example CSS:
     ```css
     .dd-vehicle {
         background: white;
         border: 1px solid #ccc;
         box-shadow: 0 0 10px rgba(0, 0, 0, 0.2);
     }
     .dd-active {
         border: 2px dashed #007bff;
     }
     ```

3. **Performance**:
   - Clones are lightweight (scripts and IDs are removed).
   - Auto-scrolling uses `requestAnimationFrame` for smoothness.

4. **Limitations**:
   - No touch support (mouse-only).
   - No nested drag-and-drop (only top-level elements are considered).


<!-- HASH:60e105817f617386a4c51aa71005d91b -->
