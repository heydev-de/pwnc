# PWNC API Documentation

[← Index](../README.md) | [`javascript/defer.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/javascript/defer.js)

- **Version:** `26.5.30.4`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Deferred Media Loading Module (`defer.js`)

Core JavaScript module for lazy-loading media elements (images, videos, iframes, audio) based on viewport visibility. Implements a **deferral pattern** where media sources are initially stored in `data-defer-*` attributes and swapped into `src`/`srcset` attributes only when the page is fully loaded. Prioritizes visible elements with `fetchpriority="high"` and `loading="eager"`, while off-screen elements receive `loading="lazy"`.

---

### Global Variables

| Name         | Value/Default | Description                                                                 |
|--------------|---------------|-----------------------------------------------------------------------------|
| `defer_done` | `false`       | Boolean flag preventing duplicate execution of `defer_process()`.           |

---

### `defer_process()`

#### **Purpose**
Orchestrates the deferred loading of media elements by:
1. Calculating viewport boundaries.
2. Determining visibility and dimensions of each deferred element.
3. Swapping `data-defer-*` attributes into active `src`/`srcset` attributes.
4. Assigning `fetchpriority` and `loading` attributes based on visibility.

#### **Parameters**
None.

#### **Return Values**
None.

#### **Inner Mechanisms**
1. **Viewport Calculation**:
   - Uses `fx_window_left`, `fx_window_top`, `fx_window_width`, and `fx_window_height` to define the visible area.
2. **Element Visibility Check**:
   - For each element, computes its bounding box (`object_x1`, `object_y1`, `object_x2`, `object_y2`).
   - Compares against viewport boundaries to determine visibility.
3. **Dimension Handling**:
   - If `data-defer-sizes` is absent, calculates width from aspect ratio (if `height` is specified).
4. **Attribute Swapping**:
   - Moves `data-defer-src` → `src`, `data-defer-srcset` → `srcset`.
   - Removes `data-defer-*` attributes post-swap.
5. **Priority Assignment**:
   - Visible elements: `fetchpriority="high"`, `loading="eager"`.
   - Off-screen elements: `loading="lazy"`.

#### **Usage Context**
- **When**: Triggered on `document_load` (if DOM is still loading) or `window_load` (if DOM is ready).
- **Why**: Reduces initial page load time by deferring non-critical media. Improves Core Web Vitals (LCP, CLS).

#### **Example**
```html
<!-- Before deferral -->
<img
    data-defer-src="hero.jpg"
    data-defer-srcset="hero-480w.jpg 480w, hero-800w.jpg 800w"
    data-defer-sizes="(max-width: 600px) 480px, 800px"
    width="800"
    height="450"
    alt="Hero Image">

<!-- After deferral (visible element) -->
<img
    src="hero.jpg"
    srcset="hero-480w.jpg 480w, hero-800w.jpg 800w"
    sizes="(max-width: 600px) 480px, 800px"
    fetchpriority="high"
    loading="eager"
    width="800"
    height="450"
    alt="Hero Image">
```

---

### Event Listeners

| Event            | Trigger Condition                     | Action                     |
|------------------|---------------------------------------|----------------------------|
| `document_load`  | `document.readyState === "loading"`   | Calls `defer_process()`.   |
| `window_load`    | Default (DOM ready)                   | Calls `defer_process()`.   |

---

### Dependencies
- **`fx_*` Functions**:
  - `fx_update_window_position()`/`fx_update_window_size()`: Update viewport dimensions.
  - `fx_offset_left()`/`fx_offset_top()`: Calculate element offsets.
  - `fx_width()`/`fx_height()`: Retrieve element dimensions.
  - `fx_event_listen()`: Attach event listeners.

---

### Key Features
1. **Viewport-Aware Loading**:
   - Only loads media within or near the viewport immediately.
2. **Responsive Support**:
   - Handles `srcset` and `sizes` attributes for responsive images.
3. **Performance Optimized**:
   - Minimizes layout shifts (CLS) by pre-calculating dimensions.
4. **Zero Dependencies**:
   - Uses vanilla JS; no external libraries required.


<!-- HASH:1d8f565981dcfc96c41fdfcb7770417e -->
