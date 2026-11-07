---
name: PWNC Style Manual
description: Use this skill whenever you create or modify CSS for a PWNC site — the design stylesheet, its `--cms-*` tokens, or a template's stylesheet.
---
# PWNC Style Manual

PWNC styling is plain CSS in files, edited directly. No style GUI, no preprocessor, no minifier, no version suffix on URLs: a file is served exactly as saved. The shipped design uses native CSS nesting.

## Model

One cascade, three layers. Later layers win at equal specificity:

1. **Design stylesheet**: tokens (`--cms-*` in `:root`), global reset, element styles, styles for module pages.
2. **Page template stylesheet**: linked at the end of the `<CMS:head/>` output, after the design stylesheet and the core scripts.
3. **Component template stylesheets**: linked inline in `<body>` before the first instance of each employed template, once per page, in order of first use.

A component's CSS therefore beats the page template's CSS at equal specificity. Template CSS has no scoping; every rule is global.

Not part of the design: the Backend and Desktop (`pwnc/stylesheet/ifc.css`), the editing controls on pages (`pwnc/stylesheet/template.css`, `--tp-*` tokens), mailform e-mails (`pwnc/stylesheet/mail.css`). These are core files. Never edit anything under `pwnc/`.

## Linking the design

- `<CMS:head/>` without `stylesheet`, or with `on|yes|true`, links `data/stylesheet.css`. `off|no|false` links nothing. Any other value is linked verbatim; a relative URL resolves against `<base href>` (installation root unless `base` overrides it).
- `data/stylesheet.css` is also the stylesheet of every page without a page template: identification (login), chat, download, the MCP authorization callback, Backend previews (image, media, insert, token). It ships with a single `@import url("../design/simple/stylesheet.css");`.
- To switch designs, change that `@import`. A page template with an explicit `stylesheet="design/<name>/stylesheet.css"` skips the import hop but no longer follows `data/stylesheet.css`; keep both pointing at the same design.
- `<CMS:head/>` emits no viewport meta. The page template must contain `<meta name="viewport" content="width=device-width, initial-scale=1">`, or phones lay out at desktop width. The Template app's page skeleton includes it.

## Design package

`design/simple/` holds `stylesheet.css`, `font/` (`inter.css`: Inter weights 100–900, normal and italic; `entypo.css`: Entypo icon font and `icon-<name>` classes) and `#include/` (PHP snippets for menus and language links; `#` folders are not web-served).

Custom design: copy the whole folder to `design/<name>/`, edit the copy, repoint `data/stylesheet.css`. The stylesheet imports `font/*.css` relative to itself, so copying `stylesheet.css` alone loses fonts and icons. Its `url()` into `data/` is `../../data/…`; keep the copy exactly two levels below the root.

Updates replace only `pwnc/`. `design/` and `data/` are never overwritten, so changes there survive, and fixes to the shipped design do not arrive by update.

Edit files through the Filemanager ([Filemanager](../Filemanager/SKILLS.md)); template CSS through the Template app ([Template manual](../Template%20manual/SKILLS.md)).

## Scaling: `--cms-factor`

```css
--cms-factor-raw: 0.0625vw;                     /* 1/1600 of the viewport width */
--cms-factor: max(1px, var(--cms-factor-raw));
--cms-font-size: calc(17 * var(--cms-factor));  /* every size is N factors */
```

- One factor is 1px up to a 1600px viewport and grows linearly above it (1920px → 1.2px, 2560px → 1.6px). No upper cap.
- The factor scales up only. Below 1600px all factor-based sizes are fixed pixels; narrow screens need `@media`, percentages or `min()`/`clamp()`.
- Move the threshold to width `W`: `--cms-factor-raw: calc(100vw / W)`.
- `--cms-hspacing` = `clamp(2 × --cms-hpadding, 5vw, --cms-hspacing-raw)`; `--cms-vspacing` likewise with `--cms-vpadding`. Between those bounds they follow 5vw, so they shrink on narrow screens although the factor does not.
- Write sizes as `calc(N * var(--cms-factor))`. Inside `min()`, `max()` and `clamp()` the math needs no `calc`: `WIDTH: min(600 * var(--cms-factor), 100%)` caps a column that still scales.
- `var()` is not allowed in media queries; breakpoints are literal pixels.

## Tokens

Read the `:root` block of the active design for the values; a site's design may define more. Suffixes: `-alt` is the alternate state or inverted counterpart, `-raw` an unclamped input.

| Group | Tokens | Role |
|---|---|---|
| Base | `--cms-color`, `--cms-background`, `--cms-color-highlight` | text, page background, accent (bullets, list numbers, `accent-color`, focus borders) |
| Inverted | `--cms-color-alt`, `--cms-background-alt` | inverted pair for templates |
| Type | `--cms-font-family`, `-font-size`, `-font-weight`, `-line-height` | body text |
| Link | `--cms-link-color`, `--cms-link-color-alt` | normal, hover/focus |
| Spacing | `--cms-hpadding`, `--cms-vpadding` | inner padding |
| | `--cms-hspacing`, `--cms-vspacing`, `*-raw` | gaps between blocks, body margin |
| | `--cms-minwidth` | for templates |
| Headings | `--cms-h{1,2,3}-{color,background,font-family,font-size,font-weight,line-height}` | H1–H3 and `.like-h1`–`.like-h3` |
| Controls | `--cms-{input,checkbox,button}-{color,background,border-width,border-style,border-color}[-alt]` | `-alt`: focus (input), hover/focus (button), checked (checkbox background and border) |
| | `--cms-{input,checkbox,button}-border[-alt]` | composite shorthands of the three border parts |
| | `--cms-input-height`, `--cms-button-height` | minimum heights |
| | `--cms-checkbox-color` | checkmark |
| Separator | `--cms-separator-{width,style,color}`, `--cms-separator-border` | `HR`; table cell borders use the color |
| Feedback | `--cms-{response,success,error}-{color,background}` | response boxes, field errors |
| Overlay | `--cms-overlay-color`, `--cms-overlay-background` | dark overlays; the background frames the login box |
| Misc | `--cms-scrollbar-color`, `--cms-scrollbar-bgcolor`, `--cms-box-shadow` | |

`--cms-color-alt`, `--cms-background-alt`, `--cms-overlay-color`, `--cms-box-shadow`, `--cms-minwidth` and `--cms-checkbox-color-alt` are not used by the shipped design itself; templates use them.

### Token traps

- **Dependents resolve where declared.** `var()` inside a custom property is substituted on the element that declares it, and descendants inherit the result. Overriding `--cms-factor` or `--cms-input-border-color` on a section does not change `--cms-font-size` or `--cms-input-border` there. Redeclare the dependents in the same rule.
- **Inverted section pattern:** on the section, swap the base pairs (`--cms-color` ↔ `--cms-color-alt`, `--cms-background` ↔ `--cms-background-alt`) via helper tokens set in `:root`, then redeclare link, input, checkbox, button and separator tokens, including the composite borders.
- **Undefined token:** the declaration becomes invalid at computed-value time; the property falls back to its inherited or initial value, not to an earlier declaration. Give optional tokens a fallback: `var(--cms-x, #000)`.
- The design declares `COLOR-SCHEME: only light`. There is no built-in dark mode; dark areas come from token swaps.
- House style writes standard properties and element selectors in upper case (`COLOR:`, `DIV.x`). Custom property names and class names are case-sensitive: `--cms-*` and classes stay lower case.

## What the design does to every element

- **Reset on `*`:** margin and padding 0, no border, transparent background, no background image, fonts and colors inherited, `POSITION: relative`, `OVERFLOW: visible`, aggressive word breaking.
  - Every element is a containing block: an absolutely positioned child anchors to its direct parent, and `top`/`left` on any element shift it.
  - The reset has zero specificity: any element or class rule, third-party CSS included, overrides it. Widgets that rely on browser defaults (borders, bullets, static positioning) lose them.
- **`BODY`** has margin `--cms-vspacing --cms-hspacing`. Full-bleed page templates set `MARGIN: 0`.
- **Headings:** H1 is `TEXT-TRANSFORM: uppercase`. H1–H3 use balanced wrapping and a bottom margin of half `--cms-vspacing`. `.like-h1`–`.like-h3` and `.p` give any element the heading or paragraph look. H1–H3, `P`, `.p`, `OL`, `UL` and `LI` lose their bottom margin as last child.
- **Links:** weight 600, dotted underline, `--cms-link-color-alt` on hover and focus.
- **Lists outside `NAV`:** `UL` gets accent-colored dots, `OL` accent-colored counter numbers. A list item without `<br>` and `<b>`, and any `<b>` inside a list item, renders at 1.1× font size, weight 500. Lists inside `NAV` get neither and need template CSS.
- **Buttons:** every `BUTTON` (PhotoSwipe's `.pswp__button` excepted) and every `INPUT[type=button|submit]` is styled as a button, including `MIN-WIDTH: 100px` and `MIN-HEIGHT: var(--cms-button-height)`. The class `button` is needed only on `A` and `LABEL`. Icon or toggle buttons in components must reset these properties.
- **Inputs:** styled are `INPUT` with an explicit `type` of `text`, `email`, `password`, `date*`, plus `SELECT` and `TEXTAREA`. An `INPUT` without `type` attribute keeps only the reset. Other types (`search`, `number`, `tel`, `url`, `file`, `range`, `color`) keep only the reset. Text-like inputs and textareas are 100% wide unless `size`/`cols` is set; a textarea without `rows` is 15em high. `INPUT[type=email]:valid` turns green, including empty optional fields.
- **Checkbox and radio** are drawn with `::before`/`::after` (`APPEARANCE: none`); their pseudo-elements are taken. A `LABEL` containing one is styled like a link.
- **Disabled controls:** grayscale, half opacity, no pointer events.
- **Media** (`IMG`, `SVG`, `VIDEO`, `CANVAS`, `OBJECT`): `MAX-WIDTH: 100%`, `HEIGHT: auto`, clipped to their box.
- **Response boxes** work on `DIV` only: `DIV.response`, `DIV.response-success`, `DIV.response-error`. Empty ones are hidden.
- **Icons:** `<span class="icon-<name>" aria-hidden="true"></span>`, names in `font/entypo.css`. In CSS: `FONT-FAMILY: "Entypo"; CONTENT: "\e8f3";`.

## Module styles a design must keep

Core generates this markup, and no template stylesheet covers it; chat and login pages have no page template at all. Dropping a section leaves it unstyled.

- Mailform and profile: `SECTION.mailform`, `SECTION.profile`, `*-mismatch` error marking, `DIV.mailform-multicolumn` (columns stack at ≤ 640px).
- Chat: `.cms-chat-interface`, `#chat-*`, `.chat-message*`, `.core-control-*`.
- Standalone bodies carry `class="cms-<application>"`; the design centers `BODY.cms-identification` and `BODY.cms-mcp`.
- Below `/* -------- DO NOT CHANGE BELOW -------- */`: `TABLE.layout`, `DIV.error` (error overlay), `DIV.textcontrol`, `.permission`.

## Template stylesheets

- **Storage:** `data/template/stylesheet/<template-id>.css`, plus `<lang>.<template-id>.css` per language variant. The filename is the template id; there is no separate index.
- **Editing:** CSS tab of the Template app's source editor (`ifc_param2`). The field is multilingual (format: see README). A language variant replaces the neutral CSS for that language; it is not added to it.
- **Saving** writes code, CSS and JavaScript in one post. An omitted or empty CSS field deletes the stylesheet; CSS without language segments deletes the language variants. A template without CSS gets no `<link>`.
- **Linking** is automatic for the page template and every employed component. `<CMS:stylesheet default="<id>"/>` links another template's CSS without its output; see [Template manual](../Template%20manual/SKILLS.md).
- **Scope** by prefixing every selector with the component's root class (`SECTION.expand-block > LABEL`). Use tokens instead of literal colors and sizes so the component follows the design.
- **Edit views** wrap editable elements in `DIV.tp-dd` / `DIV.tp-dd100` (`inline-block` / `block`, `!important`). Child combinators, `:first-child`, and flex or grid item rules then hit the wrappers. Check layouts in the Output view (`content_option=0`) and never style `tp-*` classes. Templates switch classes for edit mode with `<CMS:edit>`/`<CMS:noedit>`.
- **Page cache:** rendered pages are cached together with their `<link>` set. Saving template code, and creating or deleting a template CSS/JS file in the Template app, invalidate every cached page. Editing an existing CSS file takes effect at once. Creating or deleting one outside the app (Filemanager) leaves cached pages with the old link set ([Diagnostics](../Diagnostics/SKILLS.md#caches)).

## Verifying

- The `llm` view drops `<link rel="stylesheet">`, `<style>` and `style` attributes; classes and ids remain. Use `view: raw` to see which stylesheets a page loads, their order, and inline styles.
- `get` on a stylesheet URL (`<root>/design/<name>/stylesheet.css`, `<root>/data/template/stylesheet/<id>.css`) returns the served file.
- The shipped root `.htaccess` gives CSS a one-month `Expires`, and stylesheet URLs carry no version. Browsers keep the old file after a change until a hard reload.

---

[PWNC Website](https://pwnc.it)