---
name: PWNC Layout Manual
description: Use this skill whenever you arrange page content with the shipped layout components (rows, columns, grids, tiles, slider, accordion, wrappers) in the Frontend's layout mode, or choose which component fits a design.
---
# PWNC Layout Manual

Scope: building a page layout from the layout components that ship with PWNC (template category **Layout**). Editing mechanics (modes, addresses, writes, buffer, publication): [Content editing](<pwnc:SKILLS/Content editing/SKILLS.md>). Template syntax, IDs, repeats: [Template manual](<pwnc:SKILLS/Template manual/SKILLS.md>). Tokens and CSS: [CSS guideline](<pwnc:SKILLS/CSS guideline/SKILLS.md>).

## Concept

- **A page is a tree of components.** The page template is the frame (head, header, footer) with one main slot (`tp_content` in the shipped page templates). The slot holds a component, the component has slots of its own, those hold components, and so on. That tree is the layout.
- **Layout components are the branches, content components the leaves.** A layout component arranges slots (rows, columns, tiles, slides) or wraps one slot and adds styling or behaviour (frame, width, colours, visibility). Content components (Text, Image, Table ...) fill the slots. The engine draws no line between them: both are templates, "Layout" is a category label in the Template app.
- **The layout lives in the page's document, not in template code.** A layout is chosen and tuned by values: which component sits in a slot (`template`), how many blocks there are (`repeat`), and the component's settings (`value`, `switch`). One component serves any number of pages with different settings.
- **Settings show only while editing.** Each component starts with a settings table (`div.module-settings`, an `edit` block). A row appears when the editing mode enables its element type: `repeat`, `value`, `switch`, `template`, `group` belong to Layout mode (`44808`), `text`, `href`, `image` to Content mode (`8439`). Use Full (`45055`) while composing, Output (`0`) to judge the result.
- **Empty slots.** Most components wrap each slot in a conditional block, so an empty slot leaves no markup (no empty box, no gap). The catalogue names the exceptions. While editing, conditional blocks always render, so empty slots stay reachable.
- **Counts are `repeat` values.** The number of rows, columns, slides ... is the repeat of the component, default 3. Unset is automatic: at least the default, growing to the highest filled slot. A stored `0` is automatic without the minimum: only up to the highest filled slot. `N` is exactly N; lowering it below filled slots hides the surplus and the next write deletes it ([Content editing](<pwnc:SKILLS/Content editing/SKILLS.md>), section Repeats and Component Templates). Some components compute with the count and need it set explicitly (section Components That Need an Explicit Count).
- **One phone breakpoint: 640px.** Components switch to their phone layout at `max-width: 640px` (desktop-only rules start at 641px). Sizes scale with `--cms-factor`, gaps follow the design tokens `--cms-hspacing` / `--cms-vspacing`.
- **Pixel settings scale too.** Settings labelled "in px" (maximum width, minimum block width, gaps, the Banner height) are multiplied by `--cms-factor`: 1 px per unit up to a 1600 px wide viewport in the Simple design, more beyond ([CSS guideline](<pwnc:SKILLS/CSS guideline/SKILLS.md>), section Scaling: `--cms-factor`). Exception: the minimum block width of Multi-column layout is plain px.
- Components nest up to 50 levels.

## Shipped Components and Your Site

The components in this manual are the ones in the installation's data: code in `data/#template`, names and categories in `data/#system/template.dat`, CSS and JS in `data/template/`. They are **site data, not program code**. Updates replace `pwnc/` only, so an older site keeps the versions it was installed or built with, and any site may have renamed, modified, deleted or added components. Treat every id, address and behaviour below as a statement about the shipped set and check it on the site first:

1. **Does it exist?** Template app (`interface.php?ifc_page=template`), category **Layout**. Names show in the active language; ids differ if a component was recreated.
2. **Is it the same code?** Read its source (`ifc_message=source`, `object=<id>`) and compare element ids and structure with this manual. The preview (`ifc_message=_display&object=<id>`) renders it in edit mode with an empty document, placed in a slot named `preview`: each button's `title` is the element's address prefixed with `preview.` (`preview.r1.2` is `r1.2` in the component), and the containers carry no `data-tp-index` ([Template manual](<pwnc:SKILLS/Template manual/SKILLS.md>)).
3. **Auto-ID mode.** Columns (all three), Flexible layout and Anchor links compute data addresses in PHP and need the legacy mode, which is the default (system data key `template`, field `version` empty). In modern mode their weights, spans and anchor texts are read from the wrong addresses, so they are ignored or taken from another element, without any error.
4. **Placeholders.** Labels (`%%l_…%%`) and the defaults `%%default_column_min_width%%` (400) and `%%default_iframe_sandbox_attribute%%` come from the Placeholders app. A missing one shows as literal text; a missing default is used as that literal text, so the column minimum becomes 0 and the sandbox list holds no valid token (the frame is sandboxed completely).
5. **Design tokens and reset.** Component CSS uses `--cms-*` tokens (spacing, separator, overlay, colours). A custom design without them loses spacing and colours. The overlays of Slider, Carousel, Fan and Marquee (arrows, dots, links) are positioned absolutely and rely on the design giving every element `position: relative`, as the Simple design's reset does; without it they land in the wrong place.
6. **Page template script.** Parallax needs a loader in the page template's JS (see Wrappers). The Simple design has one, the Minimal page template has no script at all.

Where the site differs, the site's code wins. For a component the site lacks, see the section Building a Missing Component below.

## Workflow

Addresses below are examples. Copy the real ones from the markup (`data-tp-index`), see [Content editing](<pwnc:SKILLS/Content editing/SKILLS.md>), section Finding What to Edit.

1. **Open the page in Full mode:** `content.php?content_index=<index>&content_option=45055` (or its stub with the same query). Check `tp-marker` for a pending foreign draft first.
2. **Find the slot.** A slot is a container with `data-tp-type="template"`. `data-tp-value` holds the id of the selected component. `data-tp-name` shows its name in the UI language; an empty slot shows its title there if the template gives it one (Pages "Page content", Split, Slider), a component id that does not exist shows "missing". `data-tp-empty="true"` marks a slot without a component (or with a missing one); a component that outputs nothing, such as an Iframe without URL, is not marked.
3. **Pick the component** by name from the Template app and use its id (catalogue below). The picker lists every category; page templates are not offered for slots.
4. **Select it:** post `range=<slot data-tp-index>`, `type=template`, `value=<template id>` to the page URL.
5. **Read the page again.** The new component brings its own containers: the repeat, settings values and its slots. A write to an address that the structure does not contain stores nothing, so work from the outside in: component first, then its count and settings, then the components inside its slots.
6. **Set the count:** `type=repeat`, `range=<repeat data-tp-index>`, `value=<N>`. Always set it for the components that compute with it (section Components That Need an Explicit Count), and never store `0` for them.
7. **Set settings:** `type=value` for values and switches (`1` on, empty off). A write whose `type` does not match the element stores nothing. A setting that is not rendered as a container (the Slider's transition drop-down) is written to its address directly (see its entry).
8. **Fill the slots** with components, down to the leaves. Text, images and links of leaf components are Content mode work.
9. **Check in Output mode** (`content_option=0`) with `view: raw`: the llm view drops `style` attributes, where widths, columns and gaps are. Judge phone behaviour from the 640px rules in the catalogue.
10. **Publication is the user's decision.** Do not apply because the layout is finished.

Example, Rows with a two-column row (addresses of the shipped components, legacy mode):

| Step | `range` | `type` | `value` |
|------|---------|--------|---------|
| Rows into the main slot | `tp_content` | `template` | `ivVEV74Y` |
| Columns into row 1 | `tp_content.r1.2` | `template` | `50IGerbz` |
| two columns | `tp_content.r1.2.r1` | `repeat` | `2` |
| weight of column 1 | `tp_content.r1.2.r1.2` | `value` | `1` |
| weight of column 2 | `tp_content.r1.2.r1.4` | `value` | `2` |
| Text into column 1 | `tp_content.r1.2.r1.3` | `template` | `dd6ce3cf4ed61f84346a67cd0806b170` |
| its text | `tp_content.r1.2.r1.3.t1` | `text` | PWNC text |

What Layout mode shows for Columns with two columns (attributes shortened):

```html
<div class="module-settings"><table><tr>
  <td>Number of columns:</td>
  <td><div class="tp-dd" data-tp-index="tp_content.r1.2.r1" data-tp-type="repeat">…2</div></td>
</tr></table></div>
<div class="layout-col">
  <div class="layout-col-block" style="WIDTH:50%">
    <div class="layout-col-weight"><div class="tp-dd" data-tp-index="…r1.2" data-tp-type="value">…Weighting empty</div></div>
    <div class="tp-dd100" data-tp-index="…r1.3" data-tp-type="template" data-tp-name="Text">…</div>
  </div>
  …
</div>
```

### Components That Need an Explicit Count

Some components read their count in PHP. Columns, Fan and Anchor links read it before their blocks are rendered and see only the stored value or the default 3, not the automatic growth. Carousel, Pages and Tiles read it after their blocks and see the grown value, but a stored `0` is read as 0. Effects with more used slots than the default:

| Component | Count unset | Count `0` stored |
|-----------|-------------|------------------|
| Columns (all three) | widths are computed for 3 columns: five columns get 33.3 % each and wrap | every column 100 % wide |
| Fan | an opened slat widens less than intended | opening a slat widens nothing |
| Anchor links | the menu lists only the first 3 sections | the menu is empty |
| Carousel | correct | auto-rotate stops |
| Pages | correct | no pagination |
| Tiles | correct | no dot pager on phones |

Set the count to the number of blocks you use, and raise it before filling another block.

### Swapping Components Without Losing Content

Content hangs on addresses and types ([Template manual](<pwnc:SKILLS/Template manual/SKILLS.md>)). Components with identical structure can replace each other in a slot and keep everything below it; others orphan the old values (deleted by the next write).

| Family | Members | Shared structure |
|--------|---------|------------------|
| Row-like | Rows, Rows (dividing line), Rows (no gap), List, Sequence, Horizontal container, Pages | `r1`, slots `r1.2`, `r1.3` … |
| Column-like | Columns, Columns (dividing line), Columns (no gap) | `r1`, weight `r1.2`, slot `r1.3`, weight `r1.4` … |
| Single-slot wrappers | Frame, Limited width, Centered, Full width, Alternate colors, the four Parallax, Hide on desktop, Hide on mobile, Multi-column: full width, Multi-column: no break | `tp1` (the ones with settings add their own ids) |

Settings of the target component start empty; settings the target does not have are deleted. Rows ↔ Columns is not safe: `r1.2` is a slot in Rows and a weight in Columns.

## Which Component for Which Need

| Need | Component |
|------|-----------|
| Sections stacked on a page | Rows |
| Side by side, proportional widths | Columns |
| Side by side, equal or spanning blocks that wrap | Flexible layout |
| Card grid, swipeable on phones | Tiles |
| Running text in newspaper columns | Multi-column layout |
| Full-bleed colour band | Full width + Alternate colors |
| Narrow readable column, centred | Centered |
| Image background with text on top | Banner, Split |
| One visible item at a time | Slider, Carousel |
| Collapsible sections | Accordion |
| Long page with an in-page menu | Anchor links |
| Content split over several URLs | Pages |
| Box around content | Frame |
| Show on desktop or on phones only | Hide on mobile / Hide on desktop |
| Content that opens over the page | Dialog |
| Embedded third-party page | Iframe |
| Scrolling ticker | Marquee |

## Catalogue

Names are given as English / German (the Template app lists the active language; a leading `*` keeps a name at the top of the list). Setting labels are the English texts of the editing UI. `L` = Layout mode, `C` = Content mode. Slot addresses are relative to the slot that holds the component; *k* counts blocks (1, 2, …). The formulas hold for the shipped components in legacy mode.

### Rows, Lists and Strips

Structure: repeat `r1` ("Number of rows" / "entries" / "blocks" / "pages", default 3) `L`; slot *k* = `r1.<k+1>`. Empty slots are skipped (except Pages).

| Component | Id | Output |
|-----------|----|--------|
| * Rows / * Zeilen | `ivVEV74Y` | blocks stacked, `--cms-vspacing` between them, none after the last |
| * Rows (dividing line) / * Zeilen (Trennstrich) | `nuQKjDIY` | like Rows with a rule (`--cms-separator-border`) between the blocks |
| * Rows (no gap) / * Zeilen (kein Abstand) | `r9Hqor48` | stacked without spacing, no stylesheet |
| List / Liste | `Be1AR47Y` | `<ul>`, one `<li>` per filled slot; the look comes from the design's list styles |
| Sequence / Aneinanderreihung | `FwKLnBbU` | blocks as `inline-block`: side by side at their content width, wrapping, with `--cms-hspacing` / `--cms-vspacing` between |
| Horizontal container / Horizontaler Container | `w0v1qcE6` | one non-wrapping row of `inline-block` blocks, centred, scrolling sideways with snapping when wider than its container. `white-space: nowrap` is inherited, so text inside does not wrap: use it for short items and images |
| Pages / Seiten | `RbOfmcoA` | see below |

**Pages.** Slot *k* is page *k* ("Page content"). A request renders only the page named by the query parameter `page_<id>` (default 1), followed by a pagination `nav.layout-page-pagination` when there are two or more pages. A value outside the pages or not a number renders no page, while the pagination still shows. The count is the repeat value, filled or not. While editing, all pages render. Each page is its own URL (`?page_<id>=2#page-<id>`); the page cache keys on the query string. `<id>` is a hash per component instance, not the slot address: read it from `section id="page-<id>"` or the pagination links.

### Columns

`50IGerbz` * Columns / * Spalten · `5J6BhotJ` * Columns (dividing line) / * Spalten (Trennstrich) · `7OGTyAH7` * Columns (no gap) / * Spalten (kein Abstand)

- Repeat `r1` ("Number of columns", 3) `L`. Column *k*: weight `r1.<2k>` ("Weighting", `value`) `L`, slot `r1.<2k+1>`.
- Width = weight ÷ sum of the weights of the first *count* columns; set the count explicitly (section Components That Need an Explicit Count). A weight is a whole number ≥ 1; empty, 0, negative or non-numeric text counts as 1, decimals are cut. The weight cell shows while editing only.
- Columns are `inline-block`s with `--cms-hspacing` as gap and stack at ≤ 640px. **An empty column keeps its width.** **Columns of unequal height line up on the baseline of their last text line (bottom), not at the top.** Flexible layout aligns at the top.
- Dividing line: a vertical rule between columns (horizontal between stacked columns). No gap: no spacing, no rule.

### Flexible Layout

`nTp6JIjw` * Flexible layout / * Flexibles Layout. Wrapping grid with spans.

| Id | Label | Default | Meaning |
|----|-------|---------|---------|
| `layout_flex_columns` | Number of columns | 3 | grid the spans refer to, 1–100 |
| `layout_flex_blocks` | Number of blocks (repeat) | 3 | |
| `layout_flex_minwidth` | Minimum block width in px | 400 (placeholder) | negative uses `--cms-minwidth` |
| `layout_flex_hspacing`, `layout_flex_vspacing` | gaps in px | -1 | negative uses the design's spacing |
| `layout_flex_blocks.<3k+3>.v1` | Column span `L` | empty | 1 … columns |

Slot *k* = `layout_flex_blocks.<3k+3>.tp1` (inside a group). **An empty span is the full width of all columns, so a new layout shows one block per row; set the span of each block.** Blocks grow to fill their row and wrap when `minimum width + gap` no longer fits: with the default 400 px, three blocks of span 1 wrap to two plus one in a 1180 px wide area (viewport up to 1600 px, where `--cms-factor` is 1 px); lower the minimum to 250 to keep three in a row. Phones: every block full width. Empty blocks render empty (with their margin).

### Multi-Column Layout

`47uDGzSb` * Multi-column layout / * Mehrspaltenlayout. CSS multi-column flow.

| Id | Label | Default | Meaning |
|----|-------|---------|---------|
| `layout_autocol_count` | Number of columns | -1 | below 1 = as many as fit the minimum width; with a negative minimum width too, one column |
| `layout_autocol_blocks` | Number of blocks (repeat) | 1 | |
| `layout_autocol_minwidth` | Minimum block width in px | 400 (placeholder) | negative = automatic; plain px, not scaled |
| `layout_autocol_hspacing` | Horizontal gap in px | -1 | negative = design gap |

Block *k* = group `layout_autocol_blocks.<2k+3>`, slot `….tp1`; empty slots are skipped. **With one filled block the content flows from column to column (running text). With two or more filled blocks, every block stays whole inside one column** (and gets `--cms-vspacing` below it). The rule counts rendered blocks: a count of 3 with one filled block still flows, and while editing, when all blocks render, it always shows the "whole" behaviour. Columns are balanced. Two wrappers exist for use inside a block: `SegL3nvK` * Multi-column layout: full width (spans all columns) and `SwyYDFcP` * Multi-column layout: no break (never split across columns). Both have the single slot `tp1` and do nothing outside a multi-column container.

### Tiles

`h5eI4e5D` Tiles / Kacheln. Repeat `layout_tile_blocks` (3) `L`; slot *k* = `layout_tile_blocks.<k+1>`. Empty tiles render empty. Desktop: grid of equal cells, as many columns as fit with each cell at least 250 px and 25 % wide (so at most four). Phones: a horizontally scrolling, snapping strip, two rows high, with a dot pager per four tiles (a single dot with up to four tiles). The cells have no gap between them.

### Slider

`biYNuUu3` Slider. Slot *k* = `r1.<k+3>`; empty slides are skipped.

| Id | Label | Default | Meaning |
|----|-------|---------|---------|
| `r1` | Number of slides (repeat) `L` | 3 | |
| `slider_transition` | Transition `L` | empty | `slide`, `fade`, `zoom`, `flip`, `wipe`; empty = none |
| `slider_interval` | Interval in ms `L` | 7500 | `0` = no autoplay |

The transition is a drop-down without element container: its script sends `range=<slot>.slider_transition`, `type=value`; write that. One slide is visible, previous/next arrows and a dot per slide show on hover, swiping works, autoplay pauses while the slider is hovered or focused, `prefers-reduced-motion` turns the animation off. All slides share one grid cell, so the height follows the tallest slide.

### Carousel

`GGTCvwaH` Carousel / Karussell.

| Id | Label | Default |
|----|-------|---------|
| `r1` | Number of slides (repeat) `L` | 3 |
| `carousel_spin` | Auto-rotate (switch) `L` | off |
| `carousel_speed` | Rotation speed in ms `L` | 5000, used only with Auto-rotate |
| `i1` | Background `C` (image) | |
| `t1` | Heading `C` (text, shown as `h2`) | |

Slide *k* = group `r1.<2k+5>` ("Slide"), slot `r1.<2k+5>.tp1`. The current slide shows at full size with one neighbour on each side, smaller and blurred; all other slides are invisible. The first slide has no left neighbour and the last none on the right; only auto-rotate wraps from the last slide to the first. A click on a neighbour or a dot selects it; hover or focus pauses the rotation. Empty slides render.

### Accordion

`5Q8RIJpC` Accordion / Akkordeon. Repeat `r1` ("Number of blocks", 3) `L`, switch `expand_collapse` ("Collapse inactive blocks") `L`. Block *k* = group `r1.<3k>` with heading text `t1` `C` and slot `tp1`. **A block renders only when its slot is filled; a heading alone shows nothing.** Visitors see all blocks closed; while editing all are open. With the switch on, opening one block closes the others.

### Anchor Links

`GRqEgxF9` Anchor links / Sprungmarken. Repeat `r1` ("Number of sections", 3) `L`. Section *k* = group `r1.<4k-2>` with `t_anchor_label` ("Anchor link", empty = automatic) `L`, `t_anchor_text` ("Heading", text) `C` and slot `tp1`. A `nav` with one link per section comes first, then the sections (`article` with `h2`, content, "Overview" link back to the menu). The `h2` keeps the heading's formatting, the menu entry shows it as plain text. **A section and its menu entry exist only when the heading text is set**; the `nav` itself is always output. An empty label gives the anchor `anchor-<id>-<n>`: `<id>` is a hash per component instance, `n` is the repeat position from 0, so sections without heading leave gaps (`-0`, `-2`). Read real anchors from the `h2` ids or the menu links. A label you set replaces the anchor.

### Fan

`zQ7TL4DI` Fan / Fächer. Repeat `r1` (3) `L`. Slat *k* = group `r1.<7k-5>` with image `i1`, text `t1`, link `h1` (all `C`). Slats share the width equally; hovering or activating a slat widens it to the full width. The link works once the slat is open (on touch the first tap opens, the next follows the link). Height is `max(180px, 35 %)` of the width. Empty slats render.

### Marquee

`wD7IT8X0` Marquee. Repeat `r1` (3) `L`, `marquee_minwidth` ("Minimum block width in px", 350) `L`, `marquee_duration` ("Duration in s", 20, at least 1) `L`, `h1` ("Link") `C`, `t1` ("Link text") `C`. Slot *k* = `r1.<k+5>`. Blocks run as an endless strip (a script appends a clone); hover pauses. With a link set, the whole marquee is clickable (`t1` is its invisible link text). Empty slots are skipped.

### Single-Slot Wrappers

Slot `tp1`. Nothing renders around an empty slot.

| Component | Id | Settings | Output |
|-----------|----|----------|--------|
| Frame / Rahmen | `jGmyuJ0u` | | border `--cms-separator-border`, padding `--cms-vpadding` / `--cms-hpadding` |
| Limited width / Begrenzte Breite | `EGiap5sm` | `limited_width_value` "Maximum width in px" `L` | width `min(N × --cms-factor, 100 %)`; **left-aligned, not centred**; empty = no limit |
| Centered / Zentriert | `ExONGXpx` | `layout_center_width` "Maximum width in px" `L`, `layout_center_text` "Center text" `L` | block centred horizontally (and vertically where the parent has height), width `min(N × --cms-factor, 100 %)`; optional centred text |
| Full width / Volle Breite | `pKIV97L0` | | breaks out of the container to the viewport width (`100vw`); a plain `div` while editing. `100vw` includes the scrollbar, so it can add a horizontal scrollbar unless the page clips its overflow (the Simple design's `BODY` does) |
| Alternate colors / Alternativfarben | `n0dZ8YRp` | `alternate_nobackground` "No background" `L`, `alternate_nopadding` "No padding" `L` | inverted colour section: swaps `--cms-color`/`--cms-background` with their `-alt` tokens (links, inputs, buttons, checkboxes, separators included); background and `--cms-vspacing` / `--cms-hspacing` padding unless switched off |
| Hide on desktop displays / Auf Desktop-Displays ausblenden | `43XuVb5O` | | `display: none` from 641px |
| Hide on mobile displays / Auf Mobil-Displays ausblenden | `SfLW8gq3` | | `display: none` up to 640px |
| Parallax (close / medium / far / fixed) / Parallaxe (nah / mittel / weit / fest) | `kdjiW4r8` / `UhJ6WZdK` / `1pPMFQ96` / `TICCPw3n` | | clipped wrapper around an element with `data-parallax-distance` 25 / 50 / 75 / 100. The parallax script shifts that element against the scroll position by the percentage of its distance to the viewport centre (100 keeps it fixed in the viewport), scaled up and clipped to fill the wrapper |

The hidden content of the two Hide components stays in the HTML; only CSS hides it. Parallax needs `3rdparty/parallax/parallax.js`, which the Simple design's page template script loads when the page has such an element; the script does nothing without WebGL. A page template without that loader (Minimal) shows the content static. While editing, Parallax and Full width render as plain `div`s; their effect shows only in Output.

### Banner, Split, Dialog, Iframe

**Banner** `U3JsGudh`. `i1` ("Background", image `C`), `tp1` (content). Each part renders only when set. The background sits on black at 65 % opacity with minimum height `max(50vh, 500 × --cms-factor)`, the content is centred with **white text and shadow**. Without a background image the Banner has no minimum height and its text is white on the page's background.

**Split** `ZvToM2XN`. Two halves side by side, each at least the viewport high; at ≤ 640px they stack and a half with a background image keeps at least half the viewport height. With "Entire background" on, the left image sits outside the halves, so on phones only a right half with its own image keeps that height. Left: `i1` ("Background left", image `C`), `v1` ("Entire background", switch `L`: the left image spans both halves), `tp1` ("Content left"). Right: `i2` ("Background right", image `C`), `tp2` ("Content right"). A half with a background image gets a dark translucent, blurred overlay behind its content. In Output, a half's content renders only when its slot is filled (the image and switch controls sit inside it).

**Dialog (modal)** `vIWcUkLk` (same name in German). `dialog_button_text` ("Button text", default "Open") `C`, slot `tp1`, **preset to the Iframe component**. Output is an opener button and a `<dialog>` with the slot and a close button. Set another component in `tp1` for ordinary content. **Its settings and slot show only in Full mode** (`45055`): the whole edit block belongs to the text type, so Layout mode hides it and Content mode cannot change the component. The slot is never empty: without a stored value it falls back to the Iframe, so the button always renders. To remove the dialog, remove the Dialog component.

**Iframe (opt. Consent Gate)** `l9acZemy`. `iframe_url` ("URL", `href` `C`), `iframe_info` ("Info", text `C`), `iframe_button_text` ("Button text", default "Accept") `C`, `iframe_code` ("Source code", value `L`), `iframe_sandbox_disabled` ("Disabled", switch `L`), `iframe_sandbox_attribute` ("Sandbox attribute", default `%%default_iframe_sandbox_attribute%%`) `L`. **Nothing renders without a URL.** With an info text the frame loads only after the visitor presses the button; without it the frame loads at once. If source code is set, the frame shows that code (`srcdoc`) instead of the URL, which must still be set for the component to render. The sandbox attribute is omitted when "Disabled" is on.

## Patterns

- **Page sections.** Main slot → Rows; each row holds one section (a wrapper, Columns, Tiles ...).
- **Full-bleed colour band with a readable column.** Full width → Alternate colors → Centered (maximum width, optional centred text) → content. The band spans the viewport, the content sits centred inside it.
- **Narrow centred text.** Centered with a maximum width. Limited width keeps its block at the left.
- **Proportional columns.** Columns with weights and an explicit count. Put equal-height content in them, or accept bottom-aligned blocks.
- **Card grid.** Tiles for equal cards (swipes on phones); Flexible layout for spans and mixed widths (set spans and a suitable minimum width). A Frame in each slot gives the cards a border; in Tiles the frames touch, since the cells have no gap.
- **Newspaper text.** Multi-column layout with one block.
- **Hero.** Banner with a background image and a content component; Split for two-sided heroes.
- **Device-specific content.** Put the component in Hide on mobile or Hide on desktop. Both versions stay in the HTML.

## Traps

- **Settings are invisible in the wrong mode.** Content mode hides `repeat`, `value`, `switch` rows. Use Layout or Full. The Dialog shows only in Full.
- **Counts that are not set.** Columns, Fan and Anchor links compute with the count and break beyond 3 blocks when it is unset; a stored `0` also breaks Carousel, Pages and Tiles. Set it explicitly.
- **Flexible layout without spans is one block per row.** Set each block's span.
- **Columns hide nothing and align at the bottom.** Empty columns keep their width; unequal heights line up on the last text line.
- **Accordion, Iframe** render nothing without their key content (slot, URL). **Anchor links** render no section without a heading, but always the (then empty) menu. A **Banner** without background image shows its white text on the page background.
- **The Slider transition has no container.** Write `<slot>.slider_transition`.
- **Order of writes.** Set the component, then the count and settings, then children; a child written before its parent component exists is never stored.
- **Wrong type.** A write whose `type` does not match the element is dropped; switches are `type=value`.
- **Repeat controller.** Lowering the count below filled slots hides content, and the next write deletes it.
- **Modern Auto-ID mode** breaks Columns, Flexible layout and Anchor links.
- **Swapping to a different structure** orphans content: only swap within a family.
- **Component CSS is global.** The classes (`layout-*`, `slider`, `carousel` ...) are not scoped; do not reuse them in your own components.
- **Text in the Horizontal container does not wrap.**
- **Parallax** does nothing without the page template's loader.

## Building a Missing Component

When no shipped component fits, create one in the Template app ([Template manual](<pwnc:SKILLS/Template manual/SKILLS.md>)) after the pattern of the shipped ones:

- One `<CMS:edit option="…">` block with `div.module-settings` and a table of settings at the top. Use an `option` list so each row shows in the mode that edits its type.
- The count as a repeat controller: a repeat with a default in the settings block, and the output repeat with `edit="off"` and the same id.
- Each slot wrapped in `<CMS:cblock>` so empty slots leave no markup. Without `idref` the block depends on the next element, the slot.
- Explicit ids with a component prefix for settings (`layout_<name>_…`); auto ids for per-block slots.
- CSS with a class prefix, tokens (`--cms-*`) instead of literals, `calc(N * var(--cms-factor))` for sizes, and the 640px breakpoint.
- Do not compute data addresses in PHP when a `group` with explicit ids does the job: hard-coded numbers break in the other Auto-ID mode.
- Do not read the count in PHP before the output repeat: it does not yet include the automatic growth. Read it after the repeat, or leave the arithmetic to CSS (for example `flex-grow` weights instead of percentages from a sum).

## Troubleshooting

| Symptom | Cause |
|---------|-------|
| Component id unknown or code differs | older or modified site: check the Template app, read the source |
| Settings table missing | mode lacks the element types; use Full (`45055`) |
| Columns wrap or are too wide, Anchor menu incomplete or empty, Fan slat does not open fully | count not set or stored as `0`; set it explicitly |
| Write had no effect | wrong `type`, or the address is not in the current structure; read the page again |
| Slots missing after selecting a component | page not read again; addresses come from the new markup |
| Child content gone after a swap | different structure; only swap within a family |
| Weights or spans ignored | modern Auto-ID mode, or the component's addresses were changed |
| Blocks wrap earlier than expected | minimum width (default 400) larger than the share of the row |
| Columns look bottom-aligned | inline-block baseline alignment; see Columns |
| Banner text invisible | no background image; the text is white |
| Parallax static | page template without the parallax loader |
| `%%l_…%%` shown as text | placeholders missing on this site |
| Spacing or colours off | design lacks the `--cms-*` tokens the component uses |

---

[PWNC Website](https://pwnc.it)