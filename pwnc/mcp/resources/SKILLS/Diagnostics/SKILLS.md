---
name: PWNC Diagnostics Manual
description: Use this skill whenever you check the health or state of a PWNC installation or trace a fault — version, PHP error log, access log and bot status, background tasks, updates, backups or caches.
---
# PWNC Diagnostics Manual

## Model

Diagnosis is reading four independent records. Each has its own scope and clock; correlate them by time.

| Record | Where | Scope | Clock |
|--------|-------|-------|-------|
| Error log | `data/#log/error.txt` (Filemanager) | PHP errors of every request, user and background task | UTC |
| Access log | Database, Log app `ifc_page=log` | Selected events only, per visitor (client address), not per account | Accessor's time zone |
| Daemon status | System app `ifc_page=setup&ifc_message=_daemon` | Last 25 background-task lines | UTC |
| Update log | System app `ifc_page=setup&ifc_message=update_status`, file `#update/update.log` | Last update or backup run | — |

The start page `interface.php?ifc_page=pwnc` shows PWNC version (`pwnc/version.txt`), PHP version, SAPI and OS. With System app access it also shows an "update available" link from a cached hourly check; no link means up to date or source unreachable.

Reading has no side effects, with these exceptions:

- **System app main page** (`ifc_page=setup` without message): forces a remote version check and sends a test mail on every load (recipient: configured test receiver, else reply-to, else sender address). Missing or failing mail settings replace the status page with the e-mail form. Open it only for its update/backup controls; use the start page and the subviews above otherwise.
- Filemanager **Edit** adds the file to "recently edited"; **Download** does not.

Updates, backups, cache clears, log settings and visitor-record saves are changes — only on request. Log settings and visitor-record saves need `operator`; without it the post is ignored.

## Error Log

`cms_error()` is the PHP error handler. It records every error that passes `error_reporting` (not `@`-suppressed ones), fatal errors and uncaught exceptions at shutdown, exceptions thrown by daemon tasks, and errors in embedded template PHP (with template name and file). Code wrapped in `cms_error_silent()` records nothing — the System app's update check and mail test fail silently.

```
==== 2026-01-01 12:00:00 UTC ======…

Warning (2): <message> in <file> on line <n>

0: file <file>, line <n>
  <function>
    (
      0: (string[12])"…"
    )
```

- Entries are prepended: newest first. `(n)` counts errors within one request, so a request's first error is the lowest of its group.
- On each write the newest 512000 bytes of the old file are kept; the oldest entry is cut mid-text.
- The stack trace is written only when the failing request ran in debug context: local client address, `admin`, or `debug` permission. Template errors also get one in template edit mode. Otherwise the entry is the message line alone.
- Frames reveal the origin: `module/mcp.php` = agent request, `data/#daemon/<hash>` = background task (same hash as the daemon status).
- MCP responses suppress the browser-console error output. The error log is the only place an agent sees errors of its own requests.

**Reading:** GET `interface.php?ifc_page=filemanager&ifc_message=download&object=<absolute path>/data/%23log/error.txt` (path as the Filemanager shows it; `#` encoded). It returns plain text, truncated by MCP; continue with `get_response`: `filter: "===="` lists entry headers with their message lines, `filter: "mcp.php"` finds agent requests. `data/#log` is not web-fetchable. See [Filemanager](../Filemanager/SKILLS.md).

## Access Log

### What Gets Logged

Events, not requests. Each row's Action is `{application}[.{instance}]:{event}`, e.g. `content:viewed`, `desktop:identified`.

| Event | When |
|-------|------|
| `viewed` | Frontend page view by someone without edit rights on it. 404 pages served through the server's error document: Error page = 1, URL = the missing one |
| `downloaded`, `sent`, `registered`, `connected`, `created`, `search` | Download, form submission, profile registration, chat, comment or forum post, forum search |
| `identified` | Successful login |
| `unauthorized` | Login attempt after which the target application is still not permitted (wrong credentials or missing right) |
| `suspended` | Failed logins from one client address reached `CMS_LOGIN_ATTEMPT_MAX`. Further logins from it, correct ones included, are treated as anonymous until `CMS_LOGIN_BLOCK_TIME` has passed since the last failure |
| `token_mismatch` | Logged-in browser request without valid CSRF token, redirected to `security.php`. Agents and anonymous visitors are exempt |

Logging follows the event, not the channel: an agent viewing a published page without edit rights is logged too. Backend views are not logged. An entry identical (URL, action, info) to the visitor's previous entry within 10 s is a reload and not logged.

### Querying

The form accepts GET:

| Field | Values |
|-------|--------|
| `ifc_param1` | End date `Y-m-d` |
| `ifc_param2` | Range: 0 day, 1 week, 2 month (default), 3 quarter, 4 half year, 5 year, 6–9 two to five years, 10 decade |
| `ifc_param3` | Filter field: 0 User ID, 1 User, 2 IP, 3 Action, 4 URL, 5 Path, 6 Ref. URL, 7 Ref. domain, 8 User agent, 9 Client, 10 Mobile, 11 Language, 12 Region, 13 Error page |
| `ifc_param4` | 0 contains, 1 not (both SQL `LIKE`: `%` and `_` are wildcards), 2 is |
| `ifc_param5` | Filter value; empty = no filter. Mobile and Error page are `0`/`1` |
| `ifc_param6` | Bots: 0 humans only (default), 1 only, 2 all |

The main page is large. Its Graph tab is inline: totals and `const log = {<period>: [accesses, users, mobile]}`; "users" is distinct visitors per period, summed. All other tabs load lazily from URLs embedded in the response (`log_url`, `log_load_stats`):

```
interface.php?ifc_page=log&ifc_message=<load_…>&ts_start=<t>&ts_end=<t>&bot=<0|1|2>&_filter_field=<n>&_filter_operation=<n>&filter_value=<v>[&offset=<n>]
```

GET them directly. `ifc_message`: `load_raw` (Details), `load_origin`, `load_content`, `load_activity`, `load_region`, `load_language`, `load_technology`, `load_identity`, `load_time`. Aggregates return the top 100. `load_raw` returns 100 rows per call, newest first, about 60 lines per row; a trailing `data-log-load` element carries the next `offset`. Filter narrowly before using it. `ts_start`/`ts_end` are the accessor's wall-clock time encoded as if UTC; copy them from a main-page response instead of computing them.

Table links `javascript:f(field, 2, 'value')` mean "filter field is value": set `ifc_param3`–`ifc_param5` accordingly.

### Visitors and Bot Status

The User ID is a hash of the client address with a salt that rotates after 60 minutes without requests from it. One visitor gets a new ID after a pause; clients behind one address share an ID. Filtering User ID is `<id>` adds a User tab with the visitor record: pseudonym (generated; with the privacy setting off, names and e-mails the visitor submitted), info, status.

| Status | Set when |
|--------|----------|
| Bot (provisional) | First request of an anonymous visitor; automatic statuses fall back to it after the configured inactivity interval |
| User (provisional) | First request of a logged-in user, or a Bot (provisional) whose browser preloaded `module/check.php` |
| Bot (limit exceeded) | Provisional visitor reached the bot threshold of logged entries within 10 s |
| Bad bot | Over the limit and its 404 entries reached the bad-bot threshold within 10 s |
| User (fixed), Bot (fixed) | Set manually; never changed automatically |

- A switch between user and bot out of a provisional status rewrites the bot flag of that visitor's earlier rows since its last non-provisional status was set: past human/bot counts change retroactively.
- With the countermeasure enabled, a bad bot receives empty responses once the countermeasure delay has passed (never `admin`). It acts only on logged events.
- Thresholds, delay, anonymisation, privacy and retention are operator settings (`config`). Retention is enforced by the "Log maintenance" daemon; data retention "disabled" stops logging.

## Background Tasks

`cms_daemon()` queues a task as a file in `data/#daemon/`. On every request `pwnc.inc` queues the recurring tasks while loading and, when a task is pending, starts `daemon.php` by an HTTP request to the installation's own URL; triggers and agent runs start it too. Up to `CMS_DAEMON_PROC_LIMIT` workers run in parallel.

- No traffic, no tasks. MCP `get`/`post` count as traffic; memory and resource tools do not. A recurring task's interval is the minimum gap; it runs on the first request after that.
- If the server cannot reach its own URL, no task ever runs and nothing is logged: the start request suppresses its errors.
- Order: one-shot tasks first, then recurring tasks by queue time. A task id that is still queued is not queued again.

Daemon status lines (`_daemon`, plain text; the "Daemon monitor" polls it):

```
<UTC time> — Daemon <n> started.
<UTC time> — Task <hash> started: <label> …
<UTC time> — Task <hash> completed.
<UTC time> — Task <hash> failed: <message>
<UTC time> — Daemon <n> finished.
```

- `<hash>` is stable per task id: the same hash recurs for "Cache cleanup", "Log maintenance" and other recurring tasks. Workers interleave lines.
- `failed:` only for a thrown exception, also written to the error log. PHP warnings inside a task go to the error log only; the task still reports `completed`.
- `started` without a result line: still running or killed (fatal error, time limit). Check the error log.
- Only 25 lines are kept: on a busy installation, failures scroll out within minutes; the error log keeps them.

## Updates and Backups

Both run as daemon tasks ("System update", "System backup"). `update_status` returns `[status, log]`: -1 error, 0 none ("waiting for execution", log hidden), 1 done, 2 backup, 3 download, 4 install, 5 cleanup.

- A failed step leaves status -1 and its error as the last log line; the log ends with "Update completed" and status 1 only when every step succeeded. The daemon reports `completed` either way (no exception): judge an update or backup by `update_status`, not by the daemon line.
- Opening the System app main page while nothing runs resets the status to 0, hiding the log. Read `update_status` first; `#update/update.log` keeps the text.
- A backup holds the database export (`data/#database/backup/`) and the installation, without caches, the daemon queue, locks, captchas, generated MCP bundles, `#update` and git folders. An update creates one first unless skipped.
- The System app lists the current backup and its predecessor under "Download Backup" (files `#update/backup/backup.zip`, `_backup.zip`). No such section: no backup exists.

`ifc_page=system` (title "#System", not in the menu) is the raw `.dat` editor, opened on `data/#system/system.dat` (settings: `log`, `email`, `mysql`, `setup`, …). Filemanager Edit on any `.dat` opens it too. Read only.

## Caches

The content cache stores the pre-rendered part of each published page per page, user, language, URL and request parameters; dynamic parts (`nocache`) render on every request. Accessors with update rights on the page render the buffer live and never use it. An entry is reused only while all three are unchanged:

- the page's publication time,
- `data/#system/directory.dat` (site structure),
- `data/#system/template.dat` (template properties; touched by every code save and by creating or deleting a CSS or JS file, not by editing an existing one).

Nothing else invalidates it. Changes to images, media and downloads (URLs, variants, names, embed code), placeholders, plugin targets and other files a page pulls in can stay stale. A template CSS or JS file created or deleted outside the source editor leaves cached pages with the old `<link>` set; an existing file edited in place takes effect at once, since it is linked, not embedded. A change still in the editing buffer is not on the public page at all — see [Content editing](../Content%20editing/SKILLS.md#buffer-and-publication).

| Cache | Cleared by | Also clears |
|-------|------------|-------------|
| Content (`data/#content/cache/`) | Editor app `ifc_page=content` → "Clear content cache" (publisher) | — |
| Images (`data/image/cache/`, scaled variants) | Image app `ifc_page=image` → "Clear image cache" (operator) | Content cache |
| Placeholders (`data/#token/cache/`) | Placeholders app `ifc_page=token` → "Clear placeholder cache" (operator) | Content cache |

- The hourly "Cache cleanup" deletes files in `#cache`, `#content/cache`, `#agent` and `#lock` untouched for `CMS_CACHE_TTL` (30 days). Image and placeholder caches never expire on their own.
- A successful update clears the content cache; a failed one does not.
- Clear a cache only when output stays stale after a change the cache does not track, never as a routine step: every entry is rebuilt on the next request.
- For template-level inspection of one page, use debug mode — see [Content editing](../Content%20editing/SKILLS.md#debug-mode).

## Troubleshooting

| Symptom | Look at | Likely cause |
|---------|---------|--------------|
| Page blank or incomplete | Error log, newest entries | PHP error in a template or module |
| Change not on the public page | Editor app status, then content cache | Still buffered, or a dependency the cache does not track |
| Scheduled work never happens | Daemon status | Task failing, no traffic, or server cannot reach its own URL |
| Users locked out | Log, Action contains `suspended` | Failed-login limit reached for their address |
| Users see "Security validation" | Log, Action contains `token_mismatch` | Stale browser session, link without token |
| Traffic spike | Log Graph, then `load_identity` and `load_technology` with Bots = 1 | Crawler or bad bot |
| Many 404s | Log, Error page is `1`, `load_content` | Broken links or probing |
| Log app inactive | Error log | Database unavailable |
| Log app empty | Log settings (operator) | Data retention set to disabled |
| "Version could not be determined" | System app | Update source unreachable |
| Error entries without stack trace | — | Failing request ran outside debug context |
| System app shows the e-mail form | Mail settings | Mail not configured or test failed |

---

[PWNC Website](https://pwnc.it)