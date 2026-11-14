---
name: PWNC Forms Manual
description: Use this skill whenever you operate a Backend app or a Desktop dialog — addressing an app, reading its form, sending an action and reading the result.
---
# PWNC Forms Manual

## Concept

Every Backend app (`pwnc/module/interface.php`) and every Desktop dialog is rendered by one PHP class, `ifc`. A page is one form named `ifc`: a few fixed hidden fields, the app's state fields, a status box and the app's input fields.

The server holds no copy of the form. Every request variable becomes a PHP variable of the same name (POST over GET over cookie), and the handler reads them by name. The action is the value of `ifc_message`: the handler runs it, writes a status into `#ifc-response` and renders the next page. What the page renders is what your account may do.

Consequences that every rule below derives from:

- A field you omit is not "unchanged": it arrives as NULL, or as the value of the same name in the URL query.
- The page you receive is the contract for the next request: field names, preset action and selection come from it, not from memory.
- An action is a value, not a method. A `get` whose URL carries `ifc_message` runs that action just like a `post`.

App workflows have their own skills (Desktop, Agent communication, Filemanager, Triggers, Site structure, Media, Diagnostics, Permissions). Frontend editing is in [Content editing](<pwnc:SKILLS/Content editing/SKILLS.md>).

---

## Addressing an App

```
pwnc/module/interface.php?ifc_page={app}&ifc_option=external
```

| Parameter | Meaning |
|-----------|---------|
| `ifc_page` | The app, matching `#interface/ifc.{app}.inc`; in the URL or in `form_data`, which wins. Without it, or unknown: the start page with one link per app you may open |
| `ifc_option=external` | Omits the app navigation; a few apps adapt their menu (e.g. add a Close button). Popups use it |
| `ifc_message` | The action to run |
| `ifc_param` | General-purpose parameter of an action |
| `ifc_select`, `ifc_select_action` | Picker mode, see below |

**App list.** An app missing from the start page does not exist or is not permitted to you. Requesting an app without the right yields no tool result: PWNC answers with a bare HTTP 302 to `identification.php` ([Permissions](<pwnc:SKILLS/Permissions/SKILLS.md>), section Model).

**Desktop dialogs** use the same class: `pwnc/module/desktop.php?desktop_display=interface&desktop_interface={type}`. Their `ifc_page` field is empty and their state fields are `desktop_interface`, `user` and `object`. Workflows: [Desktop](<pwnc:SKILLS/Desktop/SKILLS.md>).

**Pickers.** With `ifc_select` the page hands the value of the field named by `ifc_select` to a calling window. You have none: read that field and use the value in the original form yourself.

---

## Anatomy of a Form

The form has `method="post"`, `enctype="multipart/form-data"` and an `action` URL that repeats the current query string. Its first fields are always:

| Field | Use |
|-------|-----|
| `ifc_page` | Send |
| `ifc_message` | Send; preset to the action the confirm button commits |
| `ifc_prev`, `ifc_left`, `ifc_top` | Omit; scroll position |
| `ifc_param` + app state fields (`object`, `desktop_interface`, …) | Send unchanged; they carry the selection |

Then `#ifc-response`, the menu `#ifc-command`, and the app's fields. `data-agent-info` marks fields for you:

| Value | Meaning |
|-------|---------|
| `active module`, `active action` | `ifc_page`, `ifc_message` |
| `actual value` | The hidden field that is submitted for an editor; edit this one |
| `ignore`, `ignore/omit`, `omit/ignore` | Editing UI or browser state; never send |
| `system response` | The result of the previous action |
| `danger zone` | Destructive action, see [Permissions](#permissions) |
| `permission settings` | Rights of this app, shown to holders of `interface.permission` |

**Field names are positional.** Unnamed fields are numbered in render order: `ifc_param1`, `ifc_param2`, … for all value fields, `ifc_file1`, … for uploads (separate counter). A radio group takes one number. Labels, titles and buttons take none. Conditional fields may leave gaps on purpose to keep the numbers stable. Fields the app writes itself keep their own names (`object`, `_object[]`, `list`, …). The same number means something else on another page or in another mode of the same app: read names from the current form every time.

**Cancel offset.** `ifc_cancel(n)` keeps the first `n` form elements (the five fixed fields plus `ifc_param` and the state fields), clears the rest and posts `ifc_message=ifc_cancel`. No app handles that message, so the app renders its main page with the selection kept. A plain `get` of the app URL does the same.

---

## The Action Cycle

The menu buttons carry the action in `data-ifc-command`:

```html
<button data-ifc-command="ifc_post('edit');">…Edit</button>
<button data-ifc-command="ifc_post();">…OK</button>
<button data-ifc-command="ifc_cancel(7);">…Cancel</button>
```

`ifc_post('edit')` sends `ifc_message=edit`; `ifc_post()` sends whatever `ifc_message` holds. Most apps work in two steps:

1. A plain message (`edit`, `add`) renders a form whose `ifc_message` is preset to the commit action, usually the same name with a leading underscore (`_edit`).
2. Posting that form runs the change, sets `#ifc-response` and renders the main page.

Never guess a message. Take it from a button, a select handler or the preset field. An unknown message runs nothing and renders the main page with an empty response.

**Reading the result.** `#ifc-response` holds a short status such as `<strong>OK</strong>` or the localised word for error, sometimes with details. Labels and responses follow your account's interface language, so judge by meaning and act on `ifc_message` values and field names, never on visible text. Empty means no action ran or it reports nothing. Then check the effect itself.

---

## Sending Fields

**Send the whole form.** Copy every field of the current form with its current value and change only what you mean to change. Exceptions: fields marked `ignore…`, the scroll fields, `disabled` fields (a browser does not submit them), unchecked checkboxes and `ifc_tab_{n}` radios (UI only).

| Field | Submitted as |
|-------|--------------|
| text, password, textarea, select | `name=value` |
| date / datetime | `Y-m-d` / `Y-m-dTH:i`; an empty date field is prefilled with today |
| text or code editor | the hidden field marked `actual value`; the `<code>` element is UI |
| checkbox | its `value` when checked; **omit the field when unchecked** |
| radio | the chosen value under the group's name |
| multiselect, checkbox list | `name[]`, one entry per chosen value |
| text with suggestions | free text in `name`; suggestions are the `<option>`s of `<e id="{name}">` |

**Unchecked means absent.** Many handlers test checkboxes with `isset()`, so `""` or `0` counts as checked.

**`[]` names in MCP `form_data`.** Drop the brackets and pass a JSON array: field `ifc_param1[]` → `"ifc_param1": ["a", "b"]`. The tool appends the index itself. A key written as `"ifc_param1[]"` with an array value nests the values one level too deep. An empty array sends nothing, which equals "none selected". Nested names work either way: `"opt[x]": "1"` or `"opt": {"x": "1"}`.

**JavaScript as form data.**

| In the page | Send |
|-------------|------|
| `ifc_post('x', 'v')` | the form with `ifc_message=x`, `ifc_param=v` |
| `ifc_autopost(field, 'x')` | the changed field with the form and `ifc_message=x`, in one request |
| `onchange="ifc_post('select', this.value)"` on a nameless select | `ifc_message=select`, `ifc_param={option value}` |
| `data-onchange` on a list (`<e class="select">`) | whatever its code sets, usually the state field (`object`) |
| `ifc_set(field, v)`, `ifc_copy(a, b)` | the target field with that value |
| `load_page(url)` | `get` that URL; a popup in the browser |

Read the change listeners of a field before sending it: page scripts normalise values (strip characters, derive an id from a name) and the server expects the normalised form. Length limits of a textarea show only as `ifc_limit(this, n)`.

### Multilingual Fields

The visible `l_{name}` field is UI. Send the hidden `{name}` marked `actual value` (it carries `data-l`). Its value shows the separator as `&#31;`. With MCP write it as an object, `{"": "Default", "en": "English"}`. The assembly drops empty entries, so a language you omit or leave empty is deleted: send every language. The links `id="{name}:{code}"` beside the field list the slots that field supports (empty code = default).

### File Uploads

Upload fields are `ifc_file{n}` (one file) or `ifc_file{n}[]` (several). The `post` tool takes file objects with `data` (base64) or `url`.

- One file field: one object, `"ifc_file1": {…}`. A list field: an array under the name without brackets, `"ifc_file1": [{…}, …]`. An array for a single field turns the upload into a list the handler does not expect.
- `url` is fetched by the server without your credentials, follows redirects, times out after 60 seconds and must be remote. A failed fetch or error status arrives as "no file", not as a tool error. A URL that needs a login uploads the login page.
- Upload fields often submit on change: `ifc_autopost("ifc_file1[]", "upload")` means send the files together with `ifc_message=upload`.

---

## State

Apps keep the selection and list settings per account in the server cache. Consequences:

- A plain `get` may open where an earlier request left off, including one from another session of the same account (a browser, another agent).
- An omitted state field falls back to that stored value: an `_edit` without `object` can hit the object selected last, not the one you read. Always send the state fields.
- List settings you send once (filter, sort, page, limit) become the account's default for later requests.

---

## Requests and Responses

- A `form_data` field overrides the same name in the URL. The form's `action` repeats your last query: if it holds `ifc_message=…` or `object=…` and `form_data` omits them, the URL values apply again.
- Treat a URL with `ifc_message` as an action, not a view. Do not `get` a destructive message to look at it.
- MCP does not follow redirects. A `Location` header comes back as `Redirects to: {url}`; `get` it yourself when needed.
- Stored responses are per account (per run for platform agents). Parallel sessions of the account overwrite "last", so pass `url` to `get_response`.
- `get_response` with `filter: "data-agent-info"` shows the routing fields, the response and the danger zones of a long page at a glance.
- Not every answer is a page: some actions return a bare status fragment or a file download.
- A page titled `{domain}: Window is closing …` (localised) with an empty body is a dialog that closed itself: the object or user does not exist or is not accessible to you.
- A page whose response says the application is not active: the app's library is disabled on this installation.

**Session token.** Browser URLs carry an anti-forgery token as their first query parameter (`?{token}&…`). API-key accessors, every MCP client included, are exempt: your URLs have none and you never send one. Do not copy a token from a browser URL.

---

## The llm View and What It Drops

The llm view keeps names, values, types, `checked`, `disabled`, `id`, `class`, `data-*`, `aria-*` and event handlers. Unknown elements become `<e>`. It drops, among others:

| Dropped | Effect | Fix |
|---------|--------|-----|
| `title` (kept on links) | Icon-only buttons show only an `<img>`; its file name and `data-ifc-command` tell the action | `view: raw` |
| `readonly`, `maxlength`, `required`, `pattern` | A field looks freely editable | `view: raw` before relying on limits |
| `multiple` on file inputs | Only the `[]` suffix shows it | Read the name |
| `style`, `hidden` | Hidden elements look visible | `view: raw` |

A label split by `<strong>` (`<strong>E</strong>dit`) marks the access key; it is one word.

---

## Permissions

**What you see is what you may do.** The server renders only what your account is permitted: the app list, menu buttons and fields depend on your rights. A missing button means the action is not available to you. Several apps show add, edit and delete only to holders of their operator right.

`danger zone` buttons (`data-ifc-confirm`) ask for confirmation in the browser only. Nothing on the server asks you. Run them only when the user's request clearly calls for it.

`#ifc-permission` lists the rights of the current app and links to the Permission app; it is not part of the app's form. See [Permissions](<pwnc:SKILLS/Permissions/SKILLS.md>).

---

## Worked Example

The pattern is the same for every app. The Placeholders app (`token`) serves as illustration; names and numbers come from the page you are on. `U` = `pwnc/module/interface.php?ifc_page=token&ifc_option=external`.

```
1. get   pwnc/module/interface.php
         → app list; take the app's link

2. get   U
         → fixed fields + ifc_param + object
         → buttons ifc_post('add'), …; category select: ifc_post('select', this.value)

3. post  U  {ifc_message: "select", ifc_param: "<category>"}
         → list _object[]; its data-onchange sets object

4. post  U  {ifc_message: "edit", object: "<id>"}
         → ifc_message preset to _edit; fields ifc_param1…5; Cancel is ifc_cancel(7)

5. post  U  {ifc_message: "_edit", ifc_param: "", object: "<id>",
             ifc_param1: {"": "…", "en": "…"}, ifc_param2: "…", … every field}
         → #ifc-response: OK or error, then the main page

6. post  step 4 again and compare the stored values
```

Step 5 is the only one that changes data.

---

## Troubleshooting

| Symptom | Likely Cause | Fix |
|---------|--------------|-----|
| Start page instead of the app | `ifc_page` missing or misspelled | Take the link from the start page |
| HTTP 302 to `identification.php`, no tool result | No right on the requested app | Request access; do not retry |
| Another field is now empty | A field was omitted | Send every field with its current value |
| The wrong object changed | State field omitted, stored selection used | Send `object` and the other state fields |
| A checkbox stays on | Sent as `""` or `0` | Omit unchecked checkboxes |
| Multi-value field stored wrongly | Key written with `[]` | Base name + JSON array |
| Wrong field changed | Field numbers from another page or mode | Re-read the names from the current form |
| Error with the right fields | Value not normalised like the page script does | Read the field's change listener |
| The previous action runs again | `ifc_message` in the posted URL, missing in `form_data` | Send `ifc_message` explicitly |
| Empty response after a post | The message is not an action of this page | Take it from a button or the preset |
| `get_response` shows an unrelated page | Another session of the account answered last | Pass `url` |

---

[PWNC Website](https://pwnc.it)