---
name: PWNC Style Manual
description: Use this skill whenever you create or modify a PWNC template or CSS stylesheet.
---
# PWNC Style Manual

## Table of Contents

- [Introduction](#introduction)
- [Who This Manual Is For](#who-this-manual-is-for)
- [The Fluid-Responsive Design System](#the-fluid-responsive-design-system)
  - [How `--cms-factor` Works](#how---cms-factor-works)
  - [Why `max(1px, …)`?](#why-max1px-)
  - [Practical Examples](#practical-examples)
- [CSS Variables Reference](#css-variables-reference)
  - [Core Scaling Variables](#core-scaling-variables)
  - [Color Variables](#color-variables)
  - [Typography Variables](#typography-variables)
  - [Spacing Variables](#spacing-variables)
  - [Heading Variables](#heading-variables)
  - [Input Variables](#input-variables)
  - [Checkbox Variables](#checkbox-variables)
  - [Button Variables](#button-variables)
  - [Separator Variables](#separator-variables)
  - [Overlay Variables](#overlay-variables)
  - [Scrollbar Variables](#scrollbar-variables)
  - [Shadow Variables](#shadow-variables)
- [The `design/` Folder](#the-design-folder)
  - [Default Design Package](#default-design-package)
  - [Creating a Custom Design](#creating-a-custom-design)
- [Editing the Stylesheet via the Filemanager](#editing-the-stylesheet-via-the-filemanager)
- [Template Stylesheets](#template-stylesheets)
- [General CSS Classes](#general-css-classes)
  - [Layout Classes](#layout-classes)
  - [Response Classes](#response-classes)
- [Including the Stylesheet in Templates](#including-the-stylesheet-in-templates)
- [PWNC Is Developer-Focused](#pwnc-is-developer-focused)
- [Troubleshooting](#troubleshooting)

---

## Introduction

PWNC ships with a default stylesheet at `design/simple/stylesheet.css` that provides a fluid-responsive design system. This stylesheet defines the visual foundation for all shipped components (forum, blog, chat, etc.) and templates. It uses CSS custom properties (`--cms-*` variables) for resolution-adjusted scaling, so layouts and typography adapt smoothly to any screen size and pixel density.

**Key concepts:**
- **Fluid-responsive design** — sizes scale with viewport width via `vw` units, with a floor to prevent illegibly small text on tiny screens
- **CSS custom properties** — all dimensions, colors, fonts, and spacing are defined as variables in `:root`, making global changes trivial
- **Convention over configuration** — the `design/` folder is the conventional location for design assets, but PWNC imposes no structural requirements
- **Developer-first** — no GUI style editors; you edit CSS directly

---

## Who This Manual Is For

- **Developers** building or customizing PWNC sites
- **Coding-trained AI agents** generating or modifying PWNC stylesheets
- **Designers** who need to understand the variable system to create custom designs

---

## The Fluid-Responsive Design System

### How `--cms-factor` Works

The entire stylesheet is built on two core variables defined in the `:root` block:

```css
:root
{
    --cms-factor-raw: 0.0625vw;
    --cms-factor: max(1px, var(--cms-factor-raw));
}
```

**`--cms-factor-raw`** is `0.0625vw` — that is, 0.0625% of the viewport width. On a 1920px-wide screen, this equals 1.2px. On a 360px-wide phone, it equals 0.225px.

**`--cms-factor`** wraps `--cms-factor-raw` in `max(1px, …)`, ensuring the scaling factor never drops below 1px. This prevents text and UI elements from becoming illegibly small on very narrow screens.

Every other dimension in the stylesheet is expressed as `calc(N * var(--cms-factor))`, where `N` is a multiplier. For example:

```css
--cms-font-size: calc(17 * var(--cms-factor));
--cms-hpadding: calc(15 * var(--cms-factor));
--cms-h1-font-size: calc(43 * var(--cms-factor));
```

This means:
- On a **1920px screen**: `--cms-factor` = 1.2px, so `--cms-font-size` = 20.4px
- On a **360px screen**: `--cms-factor` = 1px (floored), so `--cms-font-size` = 17px
- On a **1200px screen**: `--cms-factor` = 1px (floored), so `--cms-font-size` = 17px
- On a **2560px screen**: `--cms-factor` = 1.6px, so `--cms-font-size` = 27.2px

> **Why `0.0625vw`?** This is 1/1600 of the viewport width. At 1600px viewport width, one `--cms-factor` unit equals exactly 1px. This provides a natural baseline where the design looks "as intended" at 1600px and scales proportionally above and below that.

### Why `max(1px, …)`?

Without the `max(1px, …)` floor, `--cms-factor` would become extremely small on mobile devices. At 360px viewport width, `0.0625vw` = 0.225px. A font size of `calc(17 * 0.225px)` = 3.825px would be unreadable. The `max(1px, …)` ensures a minimum of 1px per factor unit, so `calc(17 * 1px)` = 17px on small screens.

### Practical Examples

| Viewport Width | `--cms-factor-raw` | `--cms-factor` | `--cms-font-size` (17×) | `--cms-h1-font-size` (43×) |
|---|---|---|---|---|
| 360px (phone) | 0.225px | 1px | 17px | 43px |
| 1024px (tablet) | 0.64px | 1px | 17px | 43px |
| 1600px (desktop) | 1px | 1px | 17px | 43px |
| 1920px (desktop) | 1.2px | 1.2px | 20.4px | 51.6px |
| 2560px (large) | 1.6px | 1.6px | 27.2px | 68.8px |

> **Note:** With the default `--cms-factor-raw` of `0.0625vw`, the `max(1px, …)` floor means that on screens narrower than ~1600px, the design renders at its base size. Scaling up only kicks in on larger viewports. This is intentional — it ensures readability on mobile while taking advantage of extra screen real estate on large displays. If you change `--cms-factor-raw`, the breakpoint shifts accordingly: use `--cms-factor-raw: (100 / W)vw` to set the floor at width `W` (e.g., `0.0833vw` for 1200px).

---

## CSS Variables Reference

All variables are defined in the `:root` block of `design/simple/stylesheet.css`. They use the `--cms-` prefix (short for "CMS"). Template stylesheets in `data/template/stylesheet/` reference these same variables, ensuring visual consistency across all components.

### Core Scaling Variables

| Variable | Value | Description |
|----------|-------|-------------|
| `--cms-factor-raw` | `0.0625vw` | Raw scaling factor: 1/1600 of viewport width |
| `--cms-factor` | `max(1px, var(--cms-factor-raw))` | Scaling factor with 1px floor; used by all `calc()` expressions |

### Color Variables

| Variable | Value | Description |
|----------|-------|-------------|
| `--cms-color` | `#000000` | Default text color |
| `--cms-color-alt` | `#ffffff` | Alternate text color (used on dark backgrounds) |
| `--cms-color-highlight` | `#d4004b` | Accent/highlight color (pink-red) |
| `--cms-background` | `#ffffff` | Default page background |
| `--cms-background-alt` | `#000000` | Alternate background (used for inverted elements) |
| `--cms-link-color` | `#000000` | Link color (default state) |
| `--cms-link-color-alt` | `#d4004b` | Link color (hover/focus state) |
| `--cms-response-color` | `#808040` | Response message text color (olive) |
| `--cms-response-background` | `#f2f2da` | Response message background |
| `--cms-success-color` | `#408040` | Success message text color (green) |
| `--cms-success-background` | `#daf2da` | Success message background |
| `--cms-error-color` | `#800000` | Error message text color (red) |
| `--cms-error-background` | `#ffd9d9` | Error message background |
| `--cms-overlay-color` | `#ffffff` | Overlay text color |
| `--cms-overlay-background` | `rgba(15, 15, 15, 0.95)` | Overlay background (dark, near-opaque) |
| `--cms-scrollbar-color` | `#000000` | Scrollbar thumb color |
| `--cms-scrollbar-bgcolor` | `transparent` | Scrollbar track color |

### Typography Variables

| Variable | Value | Description |
|----------|-------|-------------|
| `--cms-font-family` | `"Inter", "Open Sans", "Arial", sans-serif` | Base font family |
| `--cms-font-size` | `calc(17 * var(--cms-factor))` | Base font size |
| `--cms-font-weight` | `500` | Base font weight (medium) |
| `--cms-line-height` | `155%` | Base line height |

### Spacing Variables

| Variable | Value | Description |
|----------|-------|-------------|
| `--cms-hpadding` | `calc(15 * var(--cms-factor))` | Horizontal padding (base unit) |
| `--cms-vpadding` | `calc(7.5 * var(--cms-factor))` | Vertical padding (half of hpadding) |
| `--cms-hspacing-raw` | `calc(50 * var(--cms-factor))` | Raw horizontal spacing (before clamping) |
| `--cms-vspacing-raw` | `calc(50 * var(--cms-factor))` | Raw vertical spacing (before clamping) |
| `--cms-hspacing` | `clamp(var(--cms-hpadding) * 2, 5vw, var(--cms-hspacing-raw))` | Horizontal spacing with clamp: min 30×factor, preferred 5vw, max 50×factor |
| `--cms-vspacing` | `clamp(var(--cms-vpadding) * 2, 5vw, var(--cms-vspacing-raw))` | Vertical spacing with clamp: min 15×factor, preferred 5vw, max 50×factor |
| `--cms-minwidth` | `calc(250 * var(--cms-factor))` | Minimum element width |

> **Spacing clamp explained:** `--cms-hspacing` uses `clamp()` to ensure spacing never gets too small or too large. On narrow screens, it floors at `2 * --cms-hpadding` (30×factor). On very wide screens, it caps at `--cms-hspacing-raw` (50×factor). The preferred value `5vw` provides a middle ground that scales with the viewport.

### Heading Variables

| Variable | Value | Description |
|----------|-------|-------------|
| `--cms-h1-color` | `inherit` | H1 text color |
| `--cms-h1-background` | `transparent` | H1 background |
| `--cms-h1-font-family` | `inherit` | H1 font family |
| `--cms-h1-font-size` | `calc(43 * var(--cms-factor))` | H1 font size |
| `--cms-h1-font-weight` | `900` | H1 font weight (black) |
| `--cms-h1-line-height` | `105%` | H1 line height |
| `--cms-h2-color` | `inherit` | H2 text color |
| `--cms-h2-background` | `transparent` | H2 background |
| `--cms-h2-font-family` | `inherit` | H2 font family |
| `--cms-h2-font-size` | `calc(26 * var(--cms-factor))` | H2 font size |
| `--cms-h2-font-weight` | `700` | H2 font weight (bold) |
| `--cms-h2-line-height` | `117.5%` | H2 line height |
| `--cms-h3-color` | `inherit` | H3 text color |
| `--cms-h3-background` | `transparent` | H3 background |
| `--cms-h3-font-family` | `inherit` | H3 font family |
| `--cms-h3-font-size` | `calc(24 * var(--cms-factor))` | H3 font size |
| `--cms-h3-font-weight` | `600` | H3 font weight (semi-bold) |
| `--cms-h3-line-height` | `117.5%` | H3 line height |

### Input Variables

| Variable | Value | Description |
|----------|-------|-------------|
| `--cms-input-color` | `#000000` | Input text color (default) |
| `--cms-input-color-alt` | `#000000` | Input text color (focus) |
| `--cms-input-background` | `#f6f6f6` | Input background (default) |
| `--cms-input-background-alt` | `#f6f6f6` | Input background (focus) |
| `--cms-input-height` | `calc(40 * var(--cms-factor))` | Input height |
| `--cms-input-border-width` | `calc(2 * var(--cms-factor))` | Input border width (default) |
| `--cms-input-border-width-alt` | `calc(2 * var(--cms-factor))` | Input border width (focus) |
| `--cms-input-border-style` | `dotted` | Input border style (default) |
| `--cms-input-border-style-alt` | `solid` | Input border style (focus) |
| `--cms-input-border-color` | `#c7c7c7` | Input border color (default) |
| `--cms-input-border-color-alt` | `#d4004b` | Input border color (focus) |
| `--cms-input-border` | *(composite)* | Shorthand: `var(--cms-input-border-width) var(--cms-input-border-style) var(--cms-input-border-color)` |
| `--cms-input-border-alt` | *(composite)* | Shorthand: `var(--cms-input-border-width-alt) var(--cms-input-border-style-alt) var(--cms-input-border-color-alt)` |

### Checkbox Variables

| Variable | Value | Description |
|----------|-------|-------------|
| `--cms-checkbox-color` | `#000000` | Checkmark color |
| `--cms-checkbox-color-alt` | `#d4004b` | Checkmark color (alt) |
| `--cms-checkbox-background` | `#f6f6f6` | Checkbox background (default) |
| `--cms-checkbox-background-alt` | `#f6f6f6` | Checkbox background (checked) |
| `--cms-checkbox-border-width` | `calc(2 * var(--cms-factor))` | Checkbox border width (default) |
| `--cms-checkbox-border-width-alt` | `calc(2 * var(--cms-factor))` | Checkbox border width (checked) |
| `--cms-checkbox-border-style` | `dotted` | Checkbox border style (default) |
| `--cms-checkbox-border-style-alt` | `dotted` | Checkbox border style (checked) |
| `--cms-checkbox-border-color` | `#c7c7c7` | Checkbox border color (default) |
| `--cms-checkbox-border-color-alt` | `#c7c7c7` | Checkbox border color (checked) |
| `--cms-checkbox-border` | *(composite)* | Shorthand: `var(--cms-checkbox-border-width) var(--cms-checkbox-border-style) var(--cms-checkbox-border-color)` |
| `--cms-checkbox-border-alt` | *(composite)* | Shorthand: `var(--cms-checkbox-border-width-alt) var(--cms-checkbox-border-style-alt) var(--cms-checkbox-border-color-alt)` |

### Button Variables

| Variable | Value | Description |
|----------|-------|-------------|
| `--cms-button-color` | `#000000` | Button text color (default) |
| `--cms-button-color-alt` | `#ffffff` | Button text color (hover/focus) |
| `--cms-button-background` | `#ffffff` | Button background (default) |
| `--cms-button-background-alt` | `#d4004b` | Button background (hover/focus) |
| `--cms-button-height` | `calc(40 * var(--cms-factor))` | Button minimum height |
| `--cms-button-border-width` | `calc(2 * var(--cms-factor))` | Button border width (default) |
| `--cms-button-border-width-alt` | `calc(2 * var(--cms-factor))` | Button border width (hover/focus) |
| `--cms-button-border-style` | `dotted` | Button border style (default) |
| `--cms-button-border-style-alt` | `solid` | Button border style (hover/focus) |
| `--cms-button-border-color` | `#000000` | Button border color (default) |
| `--cms-button-border-color-alt` | `#d4004b` | Button border color (hover/focus) |
| `--cms-button-border` | *(composite)* | Shorthand: `var(--cms-button-border-width) var(--cms-button-border-style) var(--cms-button-border-color)` |
| `--cms-button-border-alt` | *(composite)* | Shorthand: `var(--cms-button-border-width-alt) var(--cms-button-border-style-alt) var(--cms-button-border-color-alt)` |

### Separator Variables

| Variable | Value | Description |
|----------|-------|-------------|
| `--cms-separator-width` | `calc(2 * var(--cms-factor))` | Separator (hr) border width |
| `--cms-separator-style` | `dotted` | Separator border style |
| `--cms-separator-color` | `#666666` | Separator border color |
| `--cms-separator-border` | *(composite)* | Shorthand: `var(--cms-separator-width) var(--cms-separator-style) var(--cms-separator-color)` |

### Overlay Variables

| Variable | Value | Description |
|----------|-------|-------------|
| `--cms-overlay-color` | `#ffffff` | Overlay text color |
| `--cms-overlay-background` | `rgba(15, 15, 15, 0.95)` | Overlay background (dark, near-opaque) |

### Scrollbar Variables

| Variable | Value | Description |
|----------|-------|-------------|
| `--cms-scrollbar-color` | `#000000` | Scrollbar thumb color |
| `--cms-scrollbar-bgcolor` | `transparent` | Scrollbar track color |

### Shadow Variables

| Variable | Value | Description |
|----------|-------|-------------|
| `--cms-box-shadow` | `var(--cms-factor) var(--cms-factor) calc(10 * var(--cms-factor)) rgba(0, 0, 0, 0.5)` | Box shadow: horizontal offset, vertical offset, blur radius, color |

---

## The `design/` Folder

The `design/` folder is the **conventional** location for design assets in PWNC. It is not mandatory — PWNC is open to any structure the developer prefers. However, using this convention allows you to get the most out of PWNC instantly, as the shipped components and templates are designed to work with it.

### Default Design Package

The default design package lives at `design/simple/` and contains:

```
design/
└── simple/
    ├── stylesheet.css       ← The main global stylesheet
    └── font/
        ├── inter.css        ← Inter font definitions
        ├── inter-*.woff2    ← Inter font files (various weights)
        ├── entypo.css       ← Entypo icon font definitions
        └── entypo.woff2     ← Entypo font file
```

The `stylesheet.css` file is the **main global stylesheet**. It defines:
- All `--cms-*` CSS custom properties in `:root`
- A global reset (normalizing all HTML elements to a consistent baseline)
- Semantic element styling (`<h1>`–`<h3>`, `<p>`, `<table>`, `<ol>`, `<ul>`, `<blockquote>`, `<code>`, etc.)
- Form element styling (`<input>`, `<select>`, `<textarea>`, `<label>`, `<fieldset>`, `<legend>`)
- Button styling (`.button`, `<button>`, `<input type="button/submit">`)
- Response message styling (`.response`, `.response-success`, `.response-error`)
- Chat interface styling (`.cms-chat-interface`, `.chat-message`, etc.)
- Identification page styling (`body.cms-identification`)
- System-critical sections (layout tables, error messages, text controls, permissions)

> **Important:** The section below `/* -------- DO NOT CHANGE BELOW -------- */` (line 1125) contains system-critical CSS that supports PWNC's internal functionality (layout tables, error message overlays, text controls, permission indicators). These should not be modified unless you know exactly what you're doing.

### Creating a Custom Design

To create your own design:

1. **Create a new folder** under `design/` — e.g., `design/my_site/`
2. **Copy the default stylesheet** into it: `design/my_site/stylesheet.css`
3. **Adjust the variables** in the `:root` block to match your brand colors, fonts, and spacing. You can also change `--cms-factor-raw` and `--cms-factor` to alter the overall scaling behavior — increasing `--cms-factor-raw` makes the design scale up faster with viewport width
4. **Reference it in your page template** via the `<CMS:head/>` element:

```html
<CMS:head
    stylesheet="design/my_site/stylesheet.css"/>
```

You can also copy the `font/` folder if you want to use the same fonts, or replace it with your own font files.

---

## Editing the Stylesheet via the Filemanager

PWNC provides a simple backend interface under **Files** for browsing and editing the project's codebase. To edit the stylesheet:

1. Go to **Backend** → **Files**
2. Choose the folder `Design` from the **System** menu, open your project design folder and select `stylesheet.css`.
3. Click **Edit** — the file opens in a code editor with CSS syntax highlighting
4. Make your changes to the `:root` variables or any other section
5. Click **Save** to write the changes to disk

> **Tip:** Before making extensive changes, copy the file to a new location (e.g., `design/my_site/stylesheet.css`) using the filemanager's **Copy** function, then edit the copy. This preserves the original as a fallback. You can also create the new folder using the filemanager's **New directory** function.

---

## Template Stylesheets

In addition to the global stylesheet, PWNC templates can have their own component-specific stylesheets. These live in `data/template/stylesheet/` and are named with short random identifiers (e.g., `5Q8RIJpC.css`, `DEETEaYt.css`).

**How they are created:** When you write CSS into a component's stylesheet codebox in the template editor, PWNC automatically creates a file in `data/template/stylesheet/` with a random ID as the filename. The template-to-stylesheet mapping is tracked in `data/#system/template.dat`, which is maintained by the template module. This index file can also be directly edited via the filemanager if needed.

**Key characteristics of template stylesheets:**

1. **They reference `--cms-*` variables** — Template stylesheets use the same CSS custom properties defined in the global stylesheet. This ensures visual consistency. For example, a template stylesheet might use `var(--cms-vspacing)` for margins or `var(--cms-factor)` for sizing:

```css
SECTION.expand-block
{
    MARGIN: 0 0 var(--cms-vspacing);
}

SECTION.expand-block > LABEL
{
    PADDING-RIGHT: calc(40 * var(--cms-factor));
}
```

2. **They define component-specific layout** — Template stylesheets handle the structural CSS for individual components (sliders, galleries, dropdowns, dialogs, etc.), not global design tokens.

3. **They are automatically linked** — When a template is embedded via `<CMS:template/>`, its stylesheet is automatically included. Manual linking is only needed when including assets of another template without employing the template itself (see `<CMS:stylesheet/>` in the [Template Manual](../Template%20manual/SKILLS.md)).

4. **They use the same fluid-responsive system** — All dimensions in template stylesheets use `calc(N * var(--cms-factor))` for consistency with the global design.

---

## General CSS Classes

The stylesheet defines several general-purpose classes that can be used in templates and content. These are the classes available across the platform (excluding mailform, profile, and chat-specific classes, which are documented separately).

### Layout Classes

| Class | Description |
|-------|-------------|
| `.like-h1` | Applies H1 styling (font size, weight, etc.) to any element |
| `.like-h2` | Applies H2 styling to any element |
| `.like-h3` | Applies H3 styling to any element |
| `.p` | Applies paragraph styling (margin, display) to any element |
| `.button` | Applies button styling to `<a>`, `<button>`, `<label>`, or `<input>` elements |

**Example usage:**

```html
<!-- Use H1 styling on a span -->
<span class="like-h1">Custom Heading</span>

<!-- Use paragraph styling on a div -->
<div class="p">This div behaves like a paragraph.</div>

<!-- Use button styling on a link -->
<a href="#" class="button">Click me</a>
```

### Response Classes

| Class | Description |
|-------|-------------|
| `.response` | Generic response message (olive background) |
| `.response-success` | Success message (green background) |
| `.response-error` | Error message (red background) |

These classes are used for system feedback messages. They automatically inherit the `--cms-response-*`, `--cms-success-*`, and `--cms-error-*` color variables. Empty response elements are automatically hidden (`display: none`).

**Example:**

```html
<div class="response-success">Your changes have been saved.</div>
<div class="response-error">Please correct the errors below.</div>
```

---

## Including the Stylesheet in Templates

The default stylesheet should be included in page templates. The recommended way is via the `stylesheet` attribute of the `<CMS:head/>` element:

```html
<head>
    <CMS:head
        stylesheet="design/simple/stylesheet.css"/>
</head>
```

**When to use the default stylesheet:**

- ✅ **Default case** — Always include it. The shipped components (forum, blog, chat, etc.) are designed to work with it.
- ✅ **Custom design** — Copy it to a new folder under `design/`, adjust the variables, and reference the new path.
- ⚠️ **Existing codebase** — You may skip it if you're integrating PWNC into an already-styled site. In that case, you'll need to modify the shipped components or create your own to match your existing CSS.

> **Bottom line:** Unless you have a specific reason not to (e.g., integrating with an existing design system), use the default stylesheet. It provides a solid, fluid-responsive foundation that all PWNC components expect.

---

## PWNC Is Developer-Focused

PWNC is a professional web platform for building and managing websites. While it ships with ready-to-use applications (forum, blog, chat, etc.) for end users, the **styling system** is designed for developers and coding-trained AI agents who need full control over the visual presentation.

It does **not** offer UI adjustment tools for styles — there are no color pickers, font selectors, or spacing sliders in the backend.

**Why?** Modern CSS is too diverse and has almost crossed the threshold from declarative to functional. A GUI that tries to cover all CSS capabilities would either be overwhelming or severely limited. Instead, PWNC gives you:

- A well-structured, well-commented stylesheet with all design tokens as CSS custom properties
- A filemanager with a syntax-highlighted code editor for direct CSS editing
- A fluid-responsive design system that works out of the box
- Template stylesheets that follow the same variable conventions

**To customize styles:**
1. Copy `design/simple/stylesheet.css` to `design/your_design/stylesheet.css`
2. Edit the `--cms-*` variables in the `:root` block
3. Reference your stylesheet in `<CMS:head stylesheet="design/your_design/stylesheet.css"/>`

That's it. No GUI, no limitations — just CSS. *Meow* 🐱‍👤

---

## Troubleshooting

### Styles Not Applying

| Symptom | Likely Cause | Fix |
|---------|-------------|-----|
| Default styles appear unstyled | Stylesheet not linked | Add `<CMS:head stylesheet="design/simple/stylesheet.css"/>` to page template |

### Fluid Scaling Not Working

| Symptom | Likely Cause | Fix |
|---------|-------------|-----|
| Text too small on mobile | `--cms-factor` floor not working | Check that `--cms-factor: max(1px, var(--cms-factor-raw))` is intact in `:root` |

### Filemanager Editing Issues

| Symptom | Likely Cause | Fix |
|---------|-------------|-----|
| Syntax highlighting missing | MIME type not detected as `text/css` | Ensure the file has a `.css` extension |
| Changes not saved | File permissions | Check that the web server has write permissions to the file |

---

[PWNC Website](https://pwnc.it)