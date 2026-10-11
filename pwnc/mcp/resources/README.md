---
name: PWNC Instructions
description: Platform-wide instructions and guidelines for interacting with PWNC as an agent.
---
# PWNC Instructions

PWNC is a PHP-based web platform and self-contained runtime and management system that bundles its applications, tools and libraries into a single installation.

It provides a web endpoint, IDE, CMS, communication hub and management environment for AI agents and human users.

## Interaction Model

PWNC uses **HATEOAS (Hypermedia as the Engine of Application State)** as its primary interaction model.

PWNC provides a server-generated interface, primarily HTML forms and links with JavaScript augmentations. The interface presented by PWNC is authoritative for the current request, state and accessor.

When working with PWNC, always reason in terms of:

- **What can I see?**
- **What am I allowed to do?**
- **How can I discover what I can do?**
- **How do I execute the required action?**

Do not assume that an operation exists merely because PWNC is capable of performing it. Use the exposed interface to discover the actions required to fulfill the user's request.

Do not execute an exposed action merely because it is available. Execute an action only when it is required to fulfill the user's request or is necessary to discover or prepare such an action.

Work through the exposed HATEOAS interface.

## MCP

PWNC provides a fixed MCP entry point at:

`pwnc/module/mcp.php`

The actual URL depends on where PWNC is installed, e.g. `https://example.com/pwnc/module/mcp.php`.

The available tools, their inputs and their result structures are exposed through the standard MCP `tools/list` operation.

PWNC does not expose individual application functions as MCP tools. Its MCP tools are general-purpose operations for interacting with the same HATEOAS-driven environment exposed through the web interface.

When using the `get` or `post` tools via the fixed endpoint, `target_url` selects the context to operate on:

- If `target_url` identifies a local active PWNC endpoint (a URL ending in `.php`), that endpoint is executed directly and becomes the target context.
- If `target_url` identifies any other URL, the URL is retrieved through PWNC's built-in HTTP proxy.
- If `target_url` is omitted, the Desktop (`pwnc/module/desktop.php`) context is used.

PWNC also supports MCP directly via any active PWNC endpoint. In that case, the endpoint is the context and `target_url` is ignored.

Responses are returned either in full, with numbered lines, or as a unified diff. The diff is taken against the last response for the same URL (kept for one hour) or, if there is none, against the previously returned response, which may be from a different URL; its header names both URLs. Removed lines can be collapsed to `~~~ n lines removed ~~~`, the hunk header still counting them. `get_response` returns the last response in full, or with `url` the stored response of an earlier URL. `tools/list` erases the stored responses.

In HTML, `¬` at the end of a line indicates a continuation. It is not part of the source; remove it and the following line break to reconstruct the original HTML.

## Memory

PWNC provides persistent, shared memory for knowledge and continuity across requests, sessions and accessors.

Memory is a core part of the working environment. Use it to retrieve information that guides your current task, and preserve knowledge that helps future work: decisions, conventions, discoveries, working state, references and lessons learned.

**Recall relevant memory** when prior knowledge may affect the current task, especially for ongoing or unfamiliar work. **Store durable knowledge** when something learned is likely to help future agents or future requests.

Memory is not a replacement for the current HATEOAS interface. Use remembered information as context, but always follow the currently exposed interface and available actions.

## HTTP

Direct HTTP access using `GET` and `POST` is available as an alternative to MCP.

When using HTTP, follow the same HATEOAS principles: retrieve the current interface, inspect its links and forms, submit the exposed forms, and inspect each resulting response before continuing.

Use the form's declared `method`, `action` and `enctype`.

`multipart/form-data` MUST be sent as raw bytes via binary-safe transport layers only. Multipart request bodies may contain binary data and must not be treated as UTF-8 text or passed through text-only transformations.

### Authentication

For authenticated HTTP requests, send the 64-character hexadecimal API key via the `Authorization` header:

`Authorization: Bearer <api_key>`

Omit the `Authorization` header when intentionally accessing PWNC anonymously, for example to inspect what an unauthenticated visitor can see or access.

Never guess, reconstruct or disclose an API key.

PWNC permissions determine the capabilities available to the current accessor. Do not assume capabilities that are not exposed by the interface.

## JavaScript

PWNC has extensive JavaScript functionality. Most of it is not relevant when interacting with PWNC and should not be analyzed unnecessarily.

Do not attempt to execute browser-side JavaScript. Inspect JavaScript only when it is relevant to understanding an action exposed by the current interface.

JavaScript may:

- manipulate form fields before submission,
- provide browser-side convenience functionality,
- derive or transform values required for submission, or
- encapsulate HTTP actions, sometimes to reduce redundant data transfers.

When relevant JavaScript implements or encapsulates an action, analyze only the relevant code to determine the underlying HTTP interaction and perform that interaction directly through HTTP.

Do not treat JavaScript functions as an independent API or assume that an operation is available merely because a corresponding function exists. The currently exposed interface remains authoritative.

### Command

- `ifc_post(message = "", param = "")` — submits the current form, optionally setting `ifc_message` and the general-purpose parameter `ifc_param` (`""` = keep the current value)
- `ifc_cancel(offset = 0)` — submits the current form after clearing all fields starting at the `offset`th form element
- `ifc_autopost(object, message = "")` — submits the current form when the specified object's value changes, optionally setting `ifc_message` (`""` = keep the current value)

### Value

- `ifc_get(object, index = 0)` — retrieves the value of a form element; `index` is used for arrays such as radio buttons
- `ifc_reset(offset = 0)` — clears all fields starting at the `offset`th form element
- `ifc_set(object, value = "", index = 0)` — sets the value of a form element
- `ifc_copy(source, target)` — copies the value of one form element to another
- `ifc_del(object, index = 0, focus = true)` — clears the value of a form element

### Common

- `ifc_object(name, index = 0, window = this)` — retrieves a form element by name; `index` is used for arrays such as radio buttons

## Frontend, Backend, Desktop

PWNC has three interrelated main areas:

- `pwnc/module/content.php` — Frontend
- `pwnc/module/interface.php` — Backend
- `pwnc/module/desktop.php` — Desktop

### Frontend

Unified environment serving as public website and visual live compositing workspace.

The Frontend is usually accessed through human-readable paths derived from page names and the page hierarchy.

### Backend

Full-featured content and asset management and administrative applications, including data access, configuration, analytics and related functionality.

### Desktop

Utilities for communication (email, messaging, contacts, object exchange), planning (calendar, notes), and links.

`domain.tld/pwnc` always opens the Desktop, which links to Frontend and Backend.

## PWNC Text Format

PWNC does not use Markdown for text input.

Text input fields targeted by the `textcontrol` JS function use a simple proprietary format where literal `[` and `]` delimit formatted areas.

### Text

```text
[+ <bold text>]
[/ <italic text>]
[_ <underlined text>]
[< <big text>]
[> <small text>]
```

### Blocks

```text
[<- <left-aligned content>]
[<-> <centered content>]
[-> <right-aligned content>]
```

### Images

Basic usage:

```text
[IMG <any encoded url, e.g. https://example.com/image.png>]
```

or:

```text
[IMG image://<image_id>]
```

Left-floating image:

```text
[<-IMG <url>]
```

or:

```text
[<-IMG image://<image_id>]
```

Right-floating image:

```text
[IMG-> <url>]
```

or:

```text
[IMG-> image://<image_id>]
```

Limiting dimensions:

```text
[IMG <url> <max width in px (number only) or * to skip> <max height in px (number only), optional>]
```

Examples:

```text
[IMG https://example.com/image.png 250 150]
[IMG https://example.com/image.png 250]
[IMG https://example.com/image.png * 150]
```

### Links

```text
[<any encoded url> <visible link content>]
[content://<content_id> <visible link content>]
[directory://<directory_id> <visible link content>]
[mailto:info@example.com <visible link content>]
```

`content://<content_id>` and `directory://<directory_id>` support query strings and anchors as well.

### Tables

```text
[# row 1 / col 1 | row 1 / col 2 |
 | row 2 / col 1 | row 2 / col 2 ]
```

Formatting can be nested, e.g.:

```text
[+ <bold text> [/ <bold italic text>]]
```

Placeholders extend formatting; see [Template Manual](/pwnc/mcp/resources/SKILLS/Template%20manual/SKILLS.md).

Keep formatting basic; use templates for extensive formatting.

## Multilingual Values

PWNC uses a simple string format to define multilingual values delimited by the ASCII Unit Separator (`0x1F`):

```text
<default/fallback value>[0x1F]<IETF language code>:<language-specific value>[0x1F]<IETF language code>:<language-specific value> etc.
```

`[0x1F]` denotes the single raw byte `0x1F`, not the literal characters `[0x1F]`.

If an input field allows multilingual input, its `name` attribute is prefixed with `l_`, e.g.:

```text
name="l_ifc_param1"
```

This field is usually an editing UI element and can be omitted when sending form data. The combined value that is processed by PWNC is stored in a hidden field without the `l_` prefix, e.g.:

```text
name="ifc_param1"
```

Conceptual example of the unencoded string structure used inside a multipart form field:

```text
default/fallback text[0x1F]en:English text[0x1F]de:Deutscher Text
```

The fallback value is used when no specific language value exists for the active language.

In multilingual mode, one enabled language is the configured default, so align the fallback with it.

When values match across languages, which is common for links, images and template IDs, set the fallback and specify only the differing language values.

Meow! 🐱‍👤