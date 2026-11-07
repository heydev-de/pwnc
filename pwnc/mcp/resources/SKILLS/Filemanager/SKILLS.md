---
name: PWNC Filemanager Manual
description: Use this skill whenever you browse, read, edit, create, upload, fetch, rename, copy, move, delete, zip, search or replace files of a PWNC installation through the Backend Filemanager.
---
# PWNC Filemanager Manual

The Filemanager (`interface.php?ifc_page=filemanager`, menu **Files**) works on the raw files below the installation root: designs, templates, stylesheets, data stores, logs, uploads and the PWNC core. It has one app right without sub-rights: read and write on everything, code and configuration included. Where an app owns the files (templates, pages, images, `.dat` stores), prefer that app; the Filemanager bypasses its logic.

## Model

- **`object`**: the selected entry, an absolute filesystem path below `<root>/` (the installation root, visible in the `object` field). A folder always carries exactly one trailing `/`: `object` and `ifc_param` naming an existing folder are normalised, so both spellings work. A file never gets one.
- **Working folder**: the folder of `object`, or the search scope `filter_path` while a search is active. Zip and copy/move sources resolve against it; create, upload and fetch use the folder of `object`.
- **`list`**: the marked entries (checkboxes `list[]`, absolute paths as rendered). Over MCP send a JSON array under the key `list` ([Forms](../Forms/SKILLS.md#sending-fields)). Only a request without an action message pre-marks `object`; action posts use `list` as sent, and an empty `list` makes delete, copy and move a silent no-op and `_compress` an Error.
- **Messages**: `mkdir`, `mkfile`, `transfer`, `compress` and `config` only render a dialog; the `_`-prefixed variant executes. `copy`/`cut` only open the target picker. All other messages execute directly. A successful action selects its result (`object` and marks move to the new entries).
- **Persistence**: `object` (stored by every request that carries a message), sort order, the regex/snippet switches and the search cache are stored per account and persist. A posted `object` overrides the stored one. Parallel sessions on one account share this state, so send `object` with every post.
- **Sanitising**: a missing path falls back to its nearest existing parent folder, a path outside the root to the root.

Navigate with `ifc_message=select`, `ifc_param=<path>` (empty = root). Every page renders the folder tree and both recent lists before the file table; read the table with `get_response` and `filter: "list[]"`. Folder-tree links carry the path percent-encoded inside `javascript:`; post it decoded (the tree's `data-fv-hir-index` attribute holds it plain). **System** holds code-defined shortcuts (design, data, database, logs, libraries, 3rd party, localization, backup, PWNC core).

## Reading and Editing

```
GET  interface.php?ifc_page=filemanager&ifc_message=edit&ifc_option=external&object=<urlencoded path>
POST ifc_page=filemanager, ifc_message=_edit, object=<path>, ifc_param1=<complete new content>
```

- The edit view carries the file in `ifc_param1`: a textarea, or for html, svg, php, css and js files a code editor whose hidden `actual value` field is `ifc_param1`. `_edit` replaces the whole file and answers with a bare `OK`/`Error` fragment instead of a page. No partial edit, no file creation: a missing `object` falls back to its folder and the save fails. Create files with `_mkfile`.
- Every request field is normalised on input: CRLF and CR become LF, invalid UTF-8 is repaired. `_edit`, `_mkfile` and replace therefore write LF line breaks and cannot write binary content. Uploads keep their bytes.
- For `.dat` files the **Edit** command opens the System app's structured editor when that app is available and permitted. Prefer it; `_edit` writes verbatim.
- **Download** (`ifc_message=download`) delivers the file as an attachment.

## Operations

| Action | `ifc_message` | Fields | Behaviour |
|---|---|---|---|
| Create folder | `_mkdir` | `ifc_param1` name | An existing folder succeeds and gets selected |
| Create file | `_mkfile` | `ifc_param1` name, `ifc_param2` content | Fails if the name exists |
| Upload | `upload` | `ifc_file1`: one file or an array of files | Into the folder of `object`, named by the file's `filename`. **Overwrites** same-named files, which get default permissions |
| Fetch URL | `_transfer` | `ifc_param1` remote URL, `ifc_param2` name | Server-side download, remote URLs only, follows redirects, 60 s timeout, **overwrites**. Default name: last URL segment, query string included |
| Rename, permissions | `save` | `ifc_param1` new name, `ifc_param2`…`ifc_param10` | See below |
| Copy / move | `copy_insert` / `cut_insert` | `list`, `ifc_param` absolute target folder inside the root, created if missing (empty = root; a target outside answers `Error`) | Never overwrite: `name-1.ext`, `name-1/` … |
| Delete | `delete` | `list` | Recursive, permanent, no trash. Only the UI asks for confirmation |
| Zip | `_compress` | `list`, `ifc_param1` name without `.zip` | Archive in the working folder, entries relative to it; never overwrites |
| Unzip | `decompress` | `object` = the archive | Into a new sibling folder named after it (`-1` suffix if taken) |

- **Names** in `ifc_param1`/`ifc_param2` are reduced to their basename.
- **`save` runs `chmod` before renaming**, from nine checkboxes `ifc_param2`…`ifc_param10` = owner, group, other × read, write, execute. An omitted checkbox clears its bit, so re-send the currently checked ones; a bare rename sets mode `000`. On Windows only owner write has an effect. Renaming onto an existing file name **replaces that file**. Rename stays in the folder; move with `cut_insert`.
- **Copy/move sources** must lie inside the working folder; others are skipped and the response is `Error`. Their path relative to it is recreated under the target. Copy keeps permissions and skips symlinked folders.
- **Unzip** skips entries with drive letters, `//`, stream wrappers or `..`, reports `Error` and keeps what it extracted.

## Search and Replace

Search: post `ifc_message=filter` with `filter` (the term), `filter_regex=1` for a PCRE pattern with delimiters and flags (`/foo\d+/i`), `filter_snippet=1` for up to three snippets per file with line numbers.

- Scope: the folder of `object`, recursive, kept in the hidden `filter_path`. Matches entry names and the content of text files; binary files and files over 16 MiB match by name only. The table lists only matching entries, with a match count.
- Literal mode is case-sensitive, and a line break in the term matches LF, CRLF and CR. In regex mode a line break in the pattern is a plain LF; use `\R` to match CRLF files.
- A search lives only while every post carries `filter`. A post without it ends the search, `reset` also clears the cache, `select` moves the scope to the selected folder.
- Each `filter` and `replace` post re-stores both switches from the post; an omitted checkbox turns its switch off for later searches.
- Results are capped at `max_input_vars` − 100 (at least 100) so the marks stay postable; a capped result says so.
- **Settings** (`config`) holds excluded folders, one per line relative to the root, installation-wide. They are skipped unless the search starts inside them.

Replace writes into **marked files** only; a new `filter` post clears the marks.

1. Search.
2. Post `ifc_message=replace` with `filter`, `filter_regex`, `filter_replace`, `filter_path`, `list` and `filter_preview=1`. The page shows deleted/inserted text for the first three matches of every search result, marked or not. Nothing is written.
3. Repeat without `filter_preview`. Response: *n replacements in m files, e errors*.

- Every match in each marked file is replaced. Marked folders and files outside `filter_path` are ignored; symlinks and files over 16 MiB count as errors; binary files are skipped.
- Literal mode inserts the replacement literally; regex mode expands `$1` and `\1`.
- Line breaks in `filter_replace` arrive as LF, so replacing across line breaks in a CRLF file mixes line endings.

## Protection

- **Web access.** The shipped root `.htaccess` (Apache) answers 404 for `*.dat`, for everything inside a folder whose name starts with `#`, and for dot entries at the web root. Folders starting with `#` or `!` that PWNC creates also get their own deny `.htaccess` (`!` adds CGI execution); renaming a folder to such a name adds none. Everything else, including non-`#` folders under `data/`, is publicly served. **Show** (the file's URL) therefore returns 404 for protected files; read them through the edit view.
- **Path guard.** PWNC's file functions (open, list, write, rename, copy, delete, chmod) reject a path with a `.` or `..` segment and end the request with the plain text `Invalid path.`. Send normalised absolute paths.

## Concurrency

There is no edit lock. Saves (`_edit`, `_mkfile`, replace) write a temporary file under an exclusive lock and rename it into place, keeping the file's permissions; reads take a shared lock. Nobody reads a half-written file, but concurrent edits are last-save-wins: re-open the edit view right before saving. Mutex lock files of running processes live in `data/#lock/`; leave them alone.

## Recent Lists

**Last edited** (filled by opening the edit view) and **Last selected** (every Filemanager page, and the System app's `.dat` editor) are installation-wide, shared by all accessors, stored in `data/#system/filemanager.edited.lst` and `filemanager.recent.lst`. Entries survive rename and delete; a stale entry selects its nearest existing parent folder.

## Traps

| Symptom | Cause | Fix |
|---|---|---|
| Copy/move reports `Error` although items were placed | Part of `list` lies outside the working folder and was skipped | Send `object` = a folder containing all sources; check the target listing |
| Delete, copy or move does nothing, no message | `list` empty | Send the paths in `list` |
| Fatal `TypeError` on delete, copy, move or zip | `list` is not a flat array: key `list[]` with an array value, or a bare string (delete still runs for a string) | JSON array under `list`, also for one path |
| Every page shows `Invalid path.` | A path with `.`/`..` is stored as `object` | Post with a valid `object` |
| File unreadable after rename | `save` without permission checkboxes | Re-post `save` with the intended checkboxes |
| Another file vanished after rename | Renamed onto an existing name | Check the folder before renaming; there is no undo |
| Search results vanished | Post without `filter` | Repeat `filter` on every post while searching |
| Replace reports *No files marked* | A new search cleared the marks | Send the files in `list` |
| Replace ran in literal mode | `filter_regex` omitted on `replace` | Send `filter_regex=1` with the replace post |
| CRLF file turned LF, binary bytes mangled | Written through `_edit` or `_mkfile` | Upload the file instead |
| Show returns 404 | `#` folder, `.dat` file or web-root dot entry | Read it through the edit view |

## Related Skills

- [CSS guideline](../CSS%20guideline/SKILLS.md), [Template manual](../Template%20manual/SKILLS.md): what to change in stylesheets and templates.
- [Agent communication](../Agent%20communication/SKILLS.md): conversation transcripts are JSON files under `data/#agent/<owner>/<agent>/`; being a `#` folder, it is not web-fetchable but readable here.
- [Site structure](../Site%20structure/SKILLS.md): **Add link to Navigation** opens the Navigation app with the selected file's URL prefilled.
- [Diagnostics](../Diagnostics/SKILLS.md): logs under `data/#log/`. [Permissions](../Permissions/SKILLS.md): the app right. [Forms](../Forms/SKILLS.md): upload encoding.

---

[PWNC Website](https://pwnc.it)