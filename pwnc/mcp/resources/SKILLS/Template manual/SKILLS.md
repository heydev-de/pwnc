---
name: PWNC Template Manual
description: Use this skill whenever you create, modify, review, debug or explain a PWNC template or component — CMS elements, element IDs, placeholders, embedded PHP and page caching.
---
# PWNC Template Manual

## Concept

A template is HTML with `<CMS:…>` elements, embedded PHP and `%%placeholders%%`. Each editable element is a **slot**: its ID addresses a typed value in the page's **document**, a flat map `data id → (type, value)`. The engine renders in one linear pass and resolves every slot against the document, outputting the value (view) or an editing control (edit).

- **Page template**: top-level HTML frame, selected per page. Flag "page template" in the template's properties.
- **Component**: embedded through a `<CMS:template/>` slot. The slot's value is the component's template id, chosen per page by editors, so pages are composed from components at runtime and nest freely (up to 50 levels).
- **IDs are the contract.** Content stays attached to an ID, not to a template. Swapping a component that uses the same IDs keeps the content; renaming an ID orphans it.
- Editing modes, writing values and publication are covered in [Content editing](../Content%20editing/SKILLS.md); stylesheet design in [CSS guideline](../CSS%20guideline/SKILLS.md).

### Storage and Backend

Template app (`interface.php?ifc_page=template`); writing needs its operator right.

| Part | File |
|------|------|
| Name, category, page flag | `data/#system/template.dat` |
| Code | `data/#template/[<lang>.]<template-id>.htm` |
| CSS | `data/template/stylesheet/[<lang>.]<template-id>.css` |
| JS | `data/template/javascript/[<lang>.]<template-id>.js` |

- Code, CSS and JS are **multilingual fields** (README format): the fallback goes to the unprefixed file, each language part to its own file. Saving a field without a language part deletes that language's file. Saving an empty field deletes the file.
- The source editor saves code, CSS and JS from one post. A field left out of the post counts as empty and deletes its files.
- Reference templates by id (`default="<template-id>"`), never by name.
- Preview: `ifc_page=template&ifc_message=_display&object=<template-id>` renders the template in edit mode with an empty document. Each editing button's `title` shows the element's resolved path (components render under the base `preview.`). It is the fastest way to check IDs.

## Render Pipeline

1. **Placeholders** are replaced in the template's source before parsing, so they may yield CMS elements or PHP.
2. **Linear scan.** Plain HTML is copied. CMS elements are processed and each `<?php … ?>` block runs where it stands. A `<CMS:template/>` slot parses its component recursively at that point.
3. **Head insertion.** At the end, meta data, links and scripts are inserted at the position of `<CMS:head/>`.
4. **Caching** for visitors: see [Caching](#caching).

## Syntax

- An element is recognized only as `<CMS:` + lowercase name. Unknown names pass through as text. Attribute names and the toggle values `on|yes|true|off|no|false` are case-insensitive.
- **A tag ends at the first `>`.** Never write a raw `>` or PHP inside a CMS tag. Entities in attribute values are decoded, so write `&gt;` and `&quot;`.
- `/>` versus `>` does not matter to the parser. Wrapping elements (`href download group repeat shift namespace base cblock calt edit noedit nocache`) need their closing tag. A missing one leaves the scope open to the end of the template.
- Elements output complete markup (`<img>`, `<a>`, formatted text), never a bare URL. Do not put them inside HTML attributes. Build URLs in PHP (see [PHP](#php)).
- Common attributes: `id`, `default` (value used while nothing is stored), `edit="off"` (not editable, stored/default value still renders), `title` (editor label). These four and `idref` are consumed. To emit an HTML attribute with the same name, prefix it with `_` (`_id`, `_title`).
- Other attributes pass through to the HTML only on `href`, `download`, `image`, `thumbnail` (onto its `<a>`) and `media` (`class`, `style`, `_id` only). `text`, `value`, `switch`, `plugin` and `template` output no attributes, so a `class` there has no effect. Wrap them in HTML instead.

## IDs

### Resolution

*base* = the *path for children* of the enclosing `<CMS:template/>` slot or `<CMS:base>` (see the table); at page level it is empty. *path* = base plus the segments of the enclosing `group`, `repeat`, `namespace` and `shift`.

| `id` | Data id | Path for children |
|------|---------|-------------------|
| omitted or invalid | path + auto-ID | same |
| `name`, `a.b` | path + `name` | same |
| `.name` | base + `name` | path + `name` |
| `.name.` | base + `name` | base + `name` |
| `..name` | `name` (document root) | path + `name` |
| `..name.` | `name` | `name` |

- A segment `+N` / `-N` becomes auto-ID ± N, e.g. `idref="+0.i1"`.
- Allowed characters are `0-9 A-Z a-z _ .`. Anything else (`my-id`) makes the ID invalid, and it **silently falls back to the auto-ID**.
- `.name` lets elements at different nesting levels share one value (e.g. a column count used in a header row and in every body row). `..name` shares a value across all components of the page.

### Auto-ID

A counter per template, starting at 0. **Every editable element advances it, with or without an explicit ID, shown or suppressed**: `text value switch href download image thumbnail media plugin template group repeat shift`. `cblock`, `calt`, `edit`, `noedit`, `nocache` and `menu` do not advance it; `namespace` does only in modern mode. `<CMS:base>` without `id` takes one.

Per installation the engine runs in one of two modes: the system data key `template`, field `version`; empty means legacy.

| | Legacy (default) | Modern |
|---|---|---|
| Counter in `group`, `repeat`, `shift`, `namespace`, `base` | continues through children | children count from the start of the scope (first child 1 in `group`, `repeat`, `shift`, which take an index themselves; 0 in `namespace`, `base`); the scope occupies one index outside |
| Counter in `<CMS:template/>` | own counter | own counter |
| `<CMS:namespace auto-id="contain">` | children start at 0, outer counter restored | same |

```html
<CMS:text/>                                          <!-- legacy 0      | modern 0 -->
<CMS:repeat id="r1" default="2"><CMS:image/></CMS:repeat>
                                                     <!-- r1.2, r1.3    | r1.1, r1.2 -->
<CMS:text/>                                          <!-- 4             | 2 -->
<CMS:namespace id="ns" auto-id="contain"><CMS:text/></CMS:namespace>
                                                     <!-- ns.0          | ns.0 -->
<CMS:text/>                                          <!-- 5             | 4 -->
```

- Inside a repeat the auto-ID differs per iteration; an explicit ID is the same in every iteration.
- **Inserting an editable element before auto-ID elements re-addresses them**, and existing content then appears in the wrong slot. Use explicit IDs except for per-iteration content inside repeats.
- Switching the mode on an installation with content re-addresses every auto-ID element. Do not switch it on a populated site. Check which mode is active with the preview or debug output.
- **The same element in an `edit` and a `noedit` branch needs the same explicit ID.** With auto-IDs the two copies get different IDs: editors fill one, visitors see the other.

### Naming conventions

Prefix plus number or word: `t1`/`t_title` (text), `i1` (image, thumbnail), `h1` (href), `v_` (value, switch), `r1` (repeat), `g_` (group), `tp1` (template slot). Consistent names across components make them exchangeable. A placeholder can hold a shared ID centrally.

## Elements

| Element | Stored type | View output | Notes |
|---------|------------|-------------|-------|
| `text` | `text` | PWNC text format as HTML | Placeholders in the value are replaced, not parsed further. |
| `value` | `value` | nothing | Setting; read it via `cblock` or PHP. |
| `switch` | `value` | nothing | On = non-empty value. |
| `href` … `</CMS:href>` | `href` | `<a href>` around the content | Value: URL, `content://<id>`, `directory://<id>`. Empty: content renders without a link. Link `title` defaults to the target's description. |
| `download` … `</CMS:download>` | `download` | `<a href>` around the content | Value: download id. Empty: like `href`. |
| `image` | `image` | `<img>` with `srcset` | See below. |
| `thumbnail` | `image` | `<a href=zoom><img></a>` | See below. |
| `media` | `media` | player markup | See [Media](../Media/SKILLS.md). |
| `plugin` | `plugin` | raw response of the URL | Fetched server-side at render time; HTTP status ≥ 300 gives nothing. |
| `template` | `template` | the component | `default` = template id. Empty slot: nothing. |
| `menu` | — | navigation list | See below. |

Empty `image`, `thumbnail` and `media` output nothing in view and a placeholder while editing. `title` labels the slot for editors.

### Images

- `width` / `height` are a bounding box in px. Units are not parsed (`50%` means 50). The image is never upscaled and is capped by the system's maximum image resolution. Output: processed cached copies, a `srcset` of scaled versions, `sizes` (default `100vw`, override with a `sizes` attribute on `image`), deferred loading, `alt` from the image library name (an `alt` attribute overrides it).
- `thumbnail`: the visible `<img>` and the zoom target both use `width` / `height`. `twidth` / `theight` are accepted and ignored. Size the visible image with CSS. Pass-through attributes land on the `<a>`.
- A fixed image: `<CMS:image default="image://<id>" edit="off"/>`.

### Repeat

```html
<CMS:repeat id="r1" default="3"> … </CMS:repeat>
```

- **Stored value > 0**: exactly that many iterations.
- **Empty or 0 (automatic)**: at least `default` iterations, and as many as needed to reach the highest used numeric child slot (`r1.<n>…`). While repeats are being edited, one extra empty iteration follows the last used slot.
- Automatic mode detects used slots only through numeric child segments, so per-iteration content must use auto-IDs. Wrap explicit IDs in an unnamed `group` or `namespace` (`r1.<n>.i1`).
- **Repeats with the same data id share their count.** During a render, the default grows to the iterations already rendered. That is the controller pattern: a count control in the settings area plus an output repeat with `edit="off"`.

```html
<CMS:edit option="repeat"><CMS:repeat id="r1" default="3"></CMS:repeat></CMS:edit>
<CMS:repeat id="r1" edit="off">
  <CMS:cblock idref="+0.i1"><CMS:namespace>
    <CMS:noedit><CMS:thumbnail id="i1" width="1600"/></CMS:noedit>
    <CMS:edit><CMS:image id="i1" height="100"/></CMS:edit>
  </CMS:namespace></CMS:cblock>
</CMS:repeat>
```

Here `+0` resolves to the auto-ID that the following unnamed `namespace` takes. Both `i1` copies share `r1.<n>.i1`.

### Scopes

| Element | Effect |
|---------|--------|
| `<CMS:template/>` | New base = the slot's data id. Own counter. Links the component's CSS/JS once. Editors get a component control. |
| `group` | Adds a path segment. Editors get a block control (move, copy, sync). |
| `namespace` | Adds a path segment, no editing control. `auto-id="contain"` isolates the counter. |
| `base` | New base and counter, like an inline component. `scope="global"` keeps the template's `$cms_template_temp_id`. Produced by template export. |
| `shift` | Legacy reordering group; reorder with the editing commands instead. |

### Conditions

| Element | Content shown when |
|---------|--------------------|
| `cblock idref="…"` | the referenced value matches `regex` (PCRE with delimiters) if given, else is non-empty |
| `calt idref="…"` | the referenced value is empty |
| `edit [option="text image …"]` | an editing mode is active that enables at least one listed type (without `option`: any type) |
| `noedit` | no element type is being edited (visitors, Output mode) |

- `idref` resolves like `id`. If omitted, it is the auto-ID that the next editable element takes, so it works only if that element has no explicit ID.
- The check uses the current-language value of any type: a text, an image, a template slot (non-empty once a component is chosen), a repeat count.
- **Defaults count only for elements already rendered in this pass.** A `cblock` placed before its element does not see that element's `default`; on an empty page the block stays hidden although the element would show its default. Put the element first, or condition on stored content only.
- While conditional blocks are being edited, every `cblock` shows and every `calt` is hidden; while alternative blocks are being edited, every `calt` shows (details: [Content editing](../Content%20editing/SKILLS.md)). Judge the visitor view with an Output render.
- `edit` / `noedit` follow the request's editing mode, which is limited by the editor's rights (no Image app access, no `image` editing). They ignore `edit="off"` on contained elements.

## Page Template

```html
<!DOCTYPE html>
<html lang="<?=x(CMS_LANGUAGE)?>">
<head>
<CMS:head/>
<meta name="robots" content="<?=x(CMS_CONTENT_META_ROBOTS)?>">
<meta name="viewport" content="width=device-width, initial-scale=1">
</head>
<body><CMS:control/>
<main><CMS:template id="tp1"/></main>
<footer><CMS:backlink/></footer>
</body>
</html>
```

- `<CMS:head/>` is **required**, once, in the page template. It inserts charset, `<title>`, description, keywords, generator, `<base>`, canonical/alternate links, the stylesheet, preload links of used components, the core scripts and the editing scripts. Without it, editing does not work.
  - `title="Site – %s"`: `%s` = the page title.
  - `stylesheet`: omitted or `on` links `data/stylesheet.css`; a URL replaces it; `off` links none. Login, chat, download and other system pages always use `data/stylesheet.css`, so a custom path leaves them on the default design.
  - `base`: omitted or `on` = site root URL; a URL replaces it; `off` = no `<base>`.
- `<CMS:control/>`: editing toolbar (mode switch, commands, language, page permissions). It renders only for editors. Place it directly after `<body>`.
- `<CMS:backlink/>`: PWNC credit link; it renders only in the top-level template.
- Give the main content slot the same ID in all page templates, so switching a page's template keeps its content. The shipped page templates use `tp1`.
- The Backend's insert list offers a complete page frame for page templates.

### Menu

`<CMS:menu/>` renders the navigation tree ([Site structure](../Site%20structure/SKILLS.md)) as nested lists, in any template.

| Attribute | Meaning |
|-----------|---------|
| `base` | node id whose branch is shown (default: root) |
| `level` | first depth shown; `+N` / `-N` relative to the current page's depth |
| `depth` | number of levels |
| `filter` | `normal` all; `open` children only under the current path; `active` only the current top-level branch; `active-open` both |
| `exclude` | space-separated node ids, each with its subtree |
| `show-hidden`, `show-icons`, `show-images`, `show-description` | toggles, default off |

### Assets

- A template's own CSS/JS is linked automatically wherever the template is used (page template: in the head; component: at its first use, plus a preload in the head).
- `<CMS:stylesheet default="<template-id>"/>` / `<CMS:javascript default="<template-id>"/>` link another template's CSS/JS without using that template.

## Placeholders

`%%name%%` or `%%name param1, param2%%`, managed in the Backend Placeholders app (`ifc_page=token`).

- The name ends at the first space or newline. Parameters are comma-separated, with whitespace around commas trimmed; `\,` is a literal comma. `%%name,a%%` is wrong.
- A definition has a multilingual text (the current language is used) and an ordered list of parameter names, referenced as `%param%` in the text.
- **Parameter values are HTML-escaped.** Pass text, not markup.
- `[…%param%…]` in the text is removed when the parameter is empty; otherwise only the brackets are removed: `<span[ title="%hint%"]>%text%</span>`.
- An unknown placeholder stays as literal text. `\%%` escapes one delimiter; escape both to show `%%x%%` literally.
- In template source they are replaced before parsing, so they work inside attributes and PHP strings (`echo($ok ? $v : "%%l_invalid%%");`). In `text` values they are replaced at output and not parsed further.
- Visible template text belongs in a placeholder (convention `l_*`) so languages can be added later. Reuse existing ones before creating new ones.

## PHP

- **Each `<?php … ?>` block is compiled on its own.** A control structure cannot span blocks: in `<?php if ($x) { ?>…<?php } ?>` both blocks fail as parse errors, and the HTML between them is printed unconditionally. Build conditional markup inside one block with `echo` or a heredoc, or use `cblock`.
- `<?php` and `<?=` work. A bare `<?` depends on the server's `short_open_tag`.
- Code runs in namespace `cms`. PWNC functions are called unqualified; global classes need a backslash (`new \DateTime()`).
- Variables persist across the blocks of one component instance. They do not cross into other components or into replayed `nocache` sections.
- `return FALSE;` in a block ends the rest of the current template; the embedding template continues.

| Variable | Content |
|----------|---------|
| `$cms_template_document` | the page document |
| `$cms_template_base_id` | base with trailing dot (`tp1.`), empty at page level |
| `$cms_template_path_id` | current path with trailing dot |
| `$cms_template_temp_id` | short hash of page + base: stable across requests, unique per component instance; use it to prefix HTML/JS ids |

Constants: `CMS_TEMPLATE_EDIT` (editor render), `CMS_TEMPLATE_OPTION` (bitmask of the types being edited), `CMS_CONTENT_INDEX`, `CMS_CONTENT_DIRECTORY_INDEX`, `CMS_LANGUAGE`.

### Document access

```php
<?php
$title = l($cms_template_document->get($cms_template_base_id . "t1", "text"));
$src   = translate_url(l($cms_template_document->get($cms_template_base_id . "i1", "image")));
$on    = nstre($cms_template_document->get($cms_template_base_id . "v_show", "value"));
$n     = (int)$cms_template_document->get($cms_template_base_id . "r1", "repeat");
$cfg   = l($cms_template_document->get("global.category", "value")); // what id="..global.category" stores
?>
```

- `get($id, $type)` takes the final data id; the dot syntax is not resolved there.
- The type must match the stored type: `switch` is stored as `value`, `thumbnail` as `image`, a slot as `template`. With no type, any type matches.
- `get()` returns the **raw value**: multilingual values need `l()`, references (`image://…`, `content://…`) need `translate_url()`, PWNC text needs `parse_text()`. Escape output with `x()`.
- A `default` is visible only after its element has rendered.
- `set($id, $type, $value)` changes the document for the rest of this render only; nothing is saved.

### Debugging

- `debug($a, …)` prints a `<script>` with console output into the page, **for every viewer** (and into the cache). The script is readable in an MCP response. Remove it after use.
- PHP errors go to the error log with template name, file and line ([Diagnostics](../Diagnostics/SKILLS.md)). The browser console shows them only in edit renders and to developers, never in MCP responses. A block with a parse error outputs nothing; a runtime error ends its block. Rendering continues.
- Element IDs and condition references: debug mode in [Content editing](../Content%20editing/SKILLS.md).

## Caching

- Only accessors **without update rights** on a page get the cached render; to see caching effects, view the page with an account that cannot edit it. Saving template code or properties in the Template app invalidates every cached page, as does creating or deleting a CSS/JS file there. Editing an existing CSS/JS file does not (it is linked, not embedded); neither do placeholders, plugin targets, databases, external data, or CSS/JS files created or deleted outside the app. Keys and invalidation: [Diagnostics](../Diagnostics/SKILLS.md#caches).
- `<CMS:nocache>…</CMS:nocache>` re-renders its content on every request: time, randomness, session or cookie state, external data. A page with a `nocache` part gets no ETag / 304.
- **A replayed `nocache` section loses its context.** It runs with the template base as path, the counter at 0, a freshly loaded document and no variables from cached blocks. The first (cache-building) request renders it in context, so the results differ. Inside `nocache`, use explicit IDs that resolve from the template base, do not nest it inside `group`/`repeat` when it contains elements, and recompute variables inside it.

## Template Export

The editing command "export" on a page or a component slot creates a new template from the current composition. Subcomponents become `<CMS:base>` blocks and repeats are unrolled into `<CMS:namespace>` blocks.

---

[PWNC Website](https://pwnc.it)