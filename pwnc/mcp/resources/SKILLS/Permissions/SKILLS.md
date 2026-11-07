---
name: PWNC Permission Manual
description: Use this skill whenever you need to understand who you are on a PWNC installation, why something is or is not available to you, or when you manage users, groups, access rights or agent accounts in the Permission app.
---
# PWNC Permission Manual

## Model

Every request runs as one account. Every application, sub-function and object is guarded by an **access string**, e.g. `interface.image` or `memory.scope.docs.writer`. Each user and group record holds a list of granted strings and a list of excluded strings; a user also inherits from its groups. Agents are ordinary accounts: the same rules govern a human in a browser and you over MCP.

Consequences for you:

- **The interface is rendered for your rights.** A missing app, command or field means a missing right, not a glitch.
- **Do not probe.** No guessed URLs, replayed forms or alternative `ifc_message` values to test access.
- **Ask.** Name the missing capability and, if known, the exact access string.

How a denial shows:

| Where | Form |
|-------|------|
| Whole application | HTTP 302 to `identification.php`, sent before the MCP layer runs. An MCP request gets that bare 302: no JSON-RPC body, no tool result, no `Redirects to:` line. How it surfaces depends on the MCP client. |
| Inside an app | Commands or fields are missing; some selectors fall back silently, e.g. another user's Desktop shows your own ([Desktop](../Desktop/SKILLS.md)). |
| MCP authentication | HTTP 401: key unknown or expired, or account disabled. |

## Access Strings

Dot-separated, general to specific. The last segment usually names a right (`reader`, `writer`, `operator`; content adds `editor`, `publisher`), but each application defines its own segments. There are no levels: each right is a separate string, so `x.writer` does not include `x.reader`.

### Matching

A list entry matches access string `s` when it is:

- `s` itself,
- `*` or `operator`: everything,
- `<p>.*` or `<p>.operator`, where `<p>` is `s` or any leading part of it.

As a last segment, `operator` is the wildcard `*`.

| Entry | Covers | Does not cover |
|-------|--------|----------------|
| `interface.*` | `interface`, `interface.image`, `interface.image.operator` | `desktop` |
| `interface.image` | the Image app | `interface.image.operator` |
| `interface.image.operator` | the Image app and everything below it | `interface.media` |
| `interface` | the Backend start page | any Backend app |

Exclusions match the same way. Traps that follow:

- Excluding a plain string (`interface`) excludes only that string; to exclude a subtree, exclude `interface.*`.
- Excluding `x.operator` equals excluding `x.*` and locks out `x` itself. To withhold operator rights, do not grant them.
- Within one record, any matching exclusion beats any matching grant, wildcard or exact.

### Resolution

1. `identification`, `index`, `check`, `mcp`, `security` are always allowed. The MCP endpoint still applies the check of its target endpoint (below).
2. Guest rights: if the `anonymous` account or one of its enabled groups grants the string, everyone gets it. Skipped while `anonymous` is disabled.
3. Unknown, disabled or password-less accounts are denied.
4. The user's own lists: a matching exclusion denies, a matching grant permits. Either decision is final.
5. The merged lists of all enabled groups: exclusion denies, grant permits.
6. Otherwise denied.

So a user grant overrides a group exclusion, a user exclusion overrides a group grant, and no exclusion removes a guest grant.

### Where Strings Come From

An endpoint's own access string is its script name: `desktop`, `content`, `interface`, `chat`, `download`, `rss`, `sitemap`. For the Backend, `ifc_page` is appended: `interface.<app>`. Sub-rights and object rights are checked by the code inside.

Every MCP request passes the check of its context endpoint: `url`, or the Desktop when there is none. Tools without `url` (`whois`, `memory_*`, `call`, `readme_first`, …) always run in the Desktop context, so every agent account needs `desktop`. A non-local or static `url` runs through the proxy endpoint `http`.

| String | Guards | Details |
|--------|--------|---------|
| `desktop` | The Desktop, and every MCP tool call without `url` | [Desktop](../Desktop/SKILLS.md) |
| `desktop.<user>` | Viewing and acting in that user's Desktop | [Desktop](../Desktop/SKILLS.md) |
| `interface.<app>`, `interface.<app>.operator` | One Backend app; full control inside it, where the app distinguishes | |
| `http` | MCP `get`/`post` to external URLs or static files | |
| `agent.<user>` | Waking, calling and viewing that platform agent | [Agent communication](../Agent%20communication/SKILLS.md) |
| `mcp.<id>` | A registered external MCP server for platform agents | [Agent communication](../Agent%20communication/SKILLS.md) |
| `content…`, `interface.content…` | Page reading and editing | [Content editing](../Content%20editing/SKILLS.md) |
| `memory`, `memory…` | Own memory region; other regions | [Memory](../Memory/SKILLS.md) |

`interface.permission` has no sub-rights: its holder can edit every record, its own rights included. Treat it as full control.

### Permission Overlay

Apps, the Desktop and other pages declare the strings they check. Accounts holding `interface.permission` see them in the element `data-agent-info="permission settings"`: one button per string; the absolute string is its `title` (raw view) and the `ifc_param1` of its `onclick` URL (default view). Each opens the rights management view for that string; this is the reliable way to learn a string before asking for it or granting it.

## Accounts

Records are keyed `user.<username>` and `group.<group id>`. Identifiers: up to 40 characters, trimmed, fixed after creation (the display name can change). The username becomes a segment of strings like `agent.<username>`; avoid `.` and spaces. Users may belong to any number of groups; groups do not nest.

| Field | User | Group | Meaning |
|-------|:----:|:-----:|---------|
| name | ✓ | ✓ | Display name, required |
| comment | ✓ | ✓ | Free note; for an agent its **designation** |
| disabled | ✓ | ✓ | Disabled user: cannot sign in, keys rejected. Disabled group: contributes nothing |
| permission, exclusion | ✓ | ✓ | Granted and excluded strings, whitespace-separated |
| group | ✓ | | Memberships |
| password | ✓ | | Required for every account except `anonymous`, also for key-only agents |
| email, timezone, language | ✓ | | Personal settings |
| expire | ✓ | | Suspension date. The first request after it disables the account permanently; removing the date does not re-enable it |

Frontend profiles (Profile app) are not accounts: each inherits the rights of one account, `profile` for self-registration.

### Built-in Accounts

Recreated and maintained by code:

| Account | Role |
|---------|------|
| `admin` | Always `*`; cannot be disabled, grouped, restricted or suspended |
| `anonymous` | The guest; no password. Its rights apply to every accessor. Disabling it removes them for everyone |
| `profile` | Rights template for self-registered Frontend profiles |
| `daemon` | Background tasks; always holds `daemon` |
| `trigger` | Sender and conversation owner for trigger deliveries ([Triggers](../Triggers/SKILLS.md)) |

### Agent Configuration

Know what exists; never quote the values.

| Field | Purpose |
|-------|---------|
| Instructions | Per-agent instruction text, stored as a file beside the record; delivery: [Agent communication](../Agent%20communication/SKILLS.md) |
| Reminder interval | Tool calls between instruction deliveries to an external agent; `0` or empty: never delivered to it |
| Assistant mode | Account shared with its human holder (README); takes effect only with an API selected |
| API | Provider template. Selecting one makes the account a **platform agent** that PWNC runs itself |
| Endpoint, model, provider API key | Override the template's settings |
| Options | `key = value` lines; numbers and `true`/`false` are typed. Named fields beat options, options beat template defaults. A field the template requires but nobody sets prevents every run: the Agent app answers `<field> not configured.`, a `call` answers `Unknown or unavailable user`. |
| MCP API key, key expiry | Authentication of external clients |

- **External agent**: needs an MCP API key only.
- **Platform agent**: needs API and the provider fields; no MCP API key. Each run gets its own temporary token.

### MCP API Keys

- **Create new API key** generates a key, displays it once and replaces the account's previous key; PWNC stores only a hash. Platform-agent run tokens are not affected.
- Key expiry covers every token of the account, run tokens included. The first use after expiry deletes all of them and clears the date. A date already in the past rejects a freshly created key: move or clear it first.
- Clients: `.mcpb` bundles from the Backend start page ask for the key at install; OAuth clients enter it on PWNC's authorization page, and the issued access token is the key itself.
- Never ask for, guess, reconstruct or reveal a key.

## whois

`whois {}` or an empty `key` for yourself, `whois {key: "user.<username>"}` or `whois {key: "group.<group id>"}` for others. A user returns username, name, comment and groups; a group returns id, name, comment and members. Disabled records are listed like enabled ones. Rights are never shown, by design: learn them from the interface.

## The Permission App

`interface.php?ifc_page=permission`. The list shows users or groups; selecting one opens its properties beside the list. User tabs: Data, Permissions, Extended, Agent, AI, MCP. Group tabs: Basic data, Permissions, Extended.

### Saving and Deleting

- **Save replaces the whole record.** Post every field as rendered: an omitted field is cleared. For users this includes the AI configuration, reminder interval, assistant mode, key expiry and the disabled checkbox (omitted = enabled); for groups, the member list (omitted = no members). Exceptions: an empty password keeps the current one, an omitted instruction text is kept (a posted empty one deletes it), an omitted name rejects the save.
- The user properties contain the provider API key in clear text. Never quote, log or store it.
- Access lists are normalised on save: entries covered by a wildcard in the same list are dropped, and `*` or `operator` collapses the list to `*`.
- New users and groups start **disabled**; enable them from the list menu.
- Deleting a user removes its keys and run tokens, instruction text, own messenger messages and Desktop data; its pages pass to `admin`. Its memory entries, agent transcripts and the strings naming it in other records (`agent.<user>`, `desktop.<user>`) remain and apply again if the username is reused. Deleting a group removes it from all users.

### Creating an Agent Account

Only on the user's instruction; grant the narrowest strings that fulfil the task.

1. **Create account**: name, username, password.
2. Select it. Set the note (designation), groups and grants, at least `desktop`.
3. Platform agent: fill the Agent and AI tabs. External agent: Agent tab optional; set a reminder interval if it should receive instructions.
4. External agent: on the MCP tab, **Create new API key** and hand it to the client configuration.
5. **Enable** the account.

### Rights Management View

GET `interface.php?ifc_page=permission&ifc_message=_set&ifc_param1=<access string>` (also via the menu or the overlay). Read-only until one of its buttons is posted. It lists every enabled user and group as granted or denied:

- Top-level user: effective result of its own and its groups' lists. Guest rights are ignored.
- Group: its own lists only; undecided groups appear as denied.
- Users indented below a group: listed in the group's column, whatever their own result.
- `⁺`/`⁻`: the record's own lists grant or exclude (exact or wildcard); no marker: inherited.

Buttons act on the selected entries of both lists, writing the exact string into each selected record:

| Button | Effect |
|--------|--------|
| `‹` | Remove from exclusions, add to grants |
| `›` | Remove from grants, add to exclusions |
| Remove explicit permission | Remove from grants |
| Remove explicit exclusion | Remove from exclusions |

Only the exact string is added or removed. A wildcard entry such as `interface.*` stays and keeps deciding; edit it in the Permissions tab.

## Troubleshooting

| Symptom | Cause | Fix |
|---------|-------|-----|
| App missing from the Backend menu | No `interface.<app>`, or the app is hidden by design | Ask for the right; do not guess URLs |
| Redirect to `identification.php` | No right for that endpoint | Ask; do not retry with other URLs |
| `whois`, memory tools and `get` without `url` fail | Account lacks `desktop` | Grant `desktop` |
| MCP 401 | Key replaced or expired, or account disabled | New key, later expiry date, or enable |
| A grant has no effect | Matching exclusion on the user, or on a group without a user grant | Rights view; look for `⁻` |
| `‹` does not grant | A wildcard exclusion in the same record | Edit the exclusion list |
| Excluding `x.operator` blocked `x` | `operator` is a wildcard | Remove the exclusion; withhold operator by not granting it |
| `interface` granted, no Backend apps | A plain string covers nothing below it | Grant `interface.<app>` or `interface.*` |
| An exclusion does not take a right away | The guest grants it | Change the guest's rights |
| Account cannot sign in after a suspension date | Expiry disabled it | Clear the date and enable |
| New account cannot do anything | Created disabled | Enable it |
| Agent config or members vanished after a save | Fields omitted from the post | Re-enter; post the complete form |

---

[PWNC Website](https://pwnc.it)