---
name: PWNC Site Structure Manual
description: Use this skill whenever you create, move, rename, hide or delete pages, or work with the navigation tree, the home page, readable URLs, canonical addresses or the sitemap.
---
# PWNC Site Structure Manual

A site's structure is one tree of navigation nodes. It feeds the menus (`<CMS:menu/>`, see the [Template Manual](../Template%20manual/SKILLS.md)), the readable URLs, the canonical addresses and `/sitemap.xml`. Page content lives in content documents; editing it is the [Content Editing Manual](../Content%20editing/SKILLS.md)'s job. Posting to Backend apps: [Forms](../Forms/SKILLS.md).

## Mental Model

- **A node is a link, not a page.** It holds a name and a `url`. Only `content://<index>` makes it a page. Any number of nodes may link the same document.
- **The node id is the identity.** Moves and renames keep it; copies get new ids. `directory://<id>` references survive moves.
- **URLs are derived.** The readable path comes from the node names along the tree. Rename or move a node and its URL changes; nothing redirects the old one.
- **Every tree save rebuilds the whole URL file system**: all generated files are deleted and written anew.
- **Tree order matters.** The first node is the home page; a canonical base URL applies to everything after it.
- **Hidden means not in menus.** A hidden node keeps its URL and sitemap entry. Hide empty (placeholder) is what suppresses a URL.

## Apps

| `ifc_page` | Use |
|------------|-----|
| `easypage` | page in one step (document + publication + node), rename, hide, move, delete with documents |
| `directory` | every node field, any target, copy, sort, clean up |
| `content` | document lifecycle; publishing links a document into the tree |

The tree lists node ids in `data-fv-hir-index`; `ifc_message=select&ifc_param=<id>` opens a node and, in `directory`, shows its canonical URL. Legend: `(name)` hidden, green = hide-empty node in use, red = hide-empty node without any URL in its subtree (no stub).

**Selection.** `save`, `sort`, `clean` and the easypage `save` act on the posted `object`. Without it the last selection is used, which `directory` and `easypage` share per user. Always post `object`.

## Node Fields

`directory` settings form:

| Field | Name | Meaning |
|-------|------|---------|
| Link title | `ifc_param1` | multilingual; source of the URL segment; a language left empty gives the node no stub and removes it with its subtree from that language's menus; its children keep their stubs |
| Hidden | `ifc_param2` | left out of menus, with its subtree |
| Hide empty | `ifc_param3` | placeholder: no stub and no menu entry while neither it nor a descendant has a `url` |
| Delete automatically | `ifc_param4` | removable by a withdrawal once it has no `url` |
| Link description | `ifc_param5` | multilingual, menus may show it |
| URL | `ifc_param6` | the target |
| Set path | `ifc_param7` | multilingual path override |
| Canonical base URL | `ifc_param8` | multilingual |
| Icon | `ifc_param9` | icon id; `ifc_message=type` manages the set |
| Images | `ifc_param10`–`12` | normal, hover, active menu image |

`save` semantics: an omitted text field keeps its value, an empty one clears it, an omitted checkbox turns off.

## What a Node Points To

| `url` | Visiting its stub |
|-------|-------------------|
| `content://<index>` | renders the document; a `?query` in the URL is passed to it |
| `directory://<id>` | 301 to the end of the chain: a document's canonical URL or an external URL; a loop answers 508; a missing node ends the chain at the site root |
| other URL, any scheme | 301 there; relative URLs resolve against the site root |
| empty, missing document | 404 |

For a document: no read right → 302 to login; status not *publication* → 410, but only for accessors without update right on it. An editor sees the unpublished page; check visibility anonymously.

Menus and `[directory://<id> …]` links point a `directory://` node straight at its target's stub; an id without node gives the site root. `[content://<index> …]` links resolve to the document's canonical URL, falling back to `content.php?content_index=<index>` when no node links it. Prefer `content://` for pages: it survives moving or replacing nodes. A page's content index is in its node's URL field.

**Home page.** The root `index.php` redirects to the first node's address: 302 with `Vary: Accept-Language` in multilingual mode, 301 otherwise. A hidden first node with `directory://<id>` decouples the home page from menu order.

**Error page.** The root `.htaccess` sends 404s and missing `.php` files to `/404.php`. That file exists only while a node's path override is `404`.

## Generated Stubs

Stub format and language handling: [One Endpoint, Many URLs](../Content%20editing/SKILLS.md#one-endpoint-many-urls).

**Trigger.** Every tree save, every publication or withdrawal that touches the tree, and changing the enabled languages. There is no separate rebuild.

**Process.** `data/#system/directory.log` lists every generated file and folder. Regeneration locks it, deletes the listed entries in reverse order, writes the new set and logs it. The root `index.php` is rewritten each time; other files not in the log are never touched. Files in `data/#directory/auxiliary/` are copied into every generated folder.

| Rule | Effect |
|------|--------|
| segment | name in that language, transliterated, non-alphanumeric runs → `-`, trimmed, lowercased |
| missing translation | default name |
| nesting | a node with children also gets a folder of its segment: `parent.php`, `parent/child.php` |
| multilingual | every language gets its own set under `<lang>/`; no unprefixed stubs |
| collision | numeric suffix `name-1.php`, `folder-1/` — against siblings and foreign files alike |
| unused hide-empty node | no stub for it or its subtree |

**Path override.** Replaces the node's own path, relative to the site root, without `.php`; segments are normalised like names. The language folder is not added: in multilingual mode give each language its own value (`<lang>/legal`), since a lone default applies to every language and the second one collides into `-1`. Descendants do not inherit it; their paths still follow the names. Use it to keep one address stable across moves and renames.

## Canonical Addresses

Each node gets one canonical URL per language: canonical base + stub path.

- **Base.** Defaults to the site root URL. A node's base applies to it and every node after it in tree order, not only its subtree, until another node sets one. Give it absolute with scheme and trailing slash: a value without scheme or host is completed from the current request, and without the trailing slash the last path segment is dropped.
- **Shared target.** Nodes with an identical `url` string share one canonical URL: the first in tree order, replaced by a later node only if its URL is not deeper (count of `/`) and strictly shorter. A different `?query` or `#fragment` makes a different `url`.
- **Use.** A page reached through a node emits that node's canonical as `<link rel="canonical">`, plus `hreflang` alternates for the languages in which the node has a stub, when the request came through the canonical address. `content://` links and the sitemap use the document's canonical URL. A direct call by content index emits `content.php?content_index=<index>` as canonical.
- **Host restriction.** Once any base names a host, PWNC writes a host list: all base hosts plus the host of the request that regenerated. Every other `Host` — a `www.` alias, a second domain — gets 400 everywhere, Backend and MCP included. The list is rebuilt on each regeneration, so the previous saver's host can drop out.

## Sitemap

`/sitemap.xml` lists every document in status *publication*, not flagged sitemap-exclude or noindex, that a `content://` node links (hidden nodes count). One `<url>` per language with `hreflang` alternates and `x-default` for the default language. Read permissions are not checked: restricted pages appear. Cached 60 seconds.

## Creating a Page

**Placement.** `*_insert` makes the new node the first child of the target, `*_append` the next sibling after the target's subtree. An empty target puts it at the top of the tree for both — it becomes the home page.

### One Step: `easypage`

```
ifc_message=add_append   ifc_param=<target id>
ifc_param1=<name>        ifc_param3=<template id>   [ifc_param2=1 → hidden]
```

Creates a document titled with the name, **publishes it immediately** and links it from a new node (delete automatically on, hide empty off). The page is public at once, showing the template's defaults. The response selects the new node.

### Editorial Route: `content`

1. `ifc_message=_create`: `ifc_param1` title, `ifc_param2` template, `ifc_param3` memo → a draft.
2. Edit it via `content.php?content_index=<index>`.
3. `ifc_message=publish&ifc_param=<index>` opens the publication form:
   - `publish_insert` / `publish_append`, `ifc_param=<target id>`: publish and create a node named `ifc_param1` (hide empty and delete automatically on, description from the document)
   - `publish_replace`, `ifc_param=<node id>`: point an existing node at the document; sets its name and description
   - `___publish`: publish without a node
   - `ifc_param2=1` + `ifc_param3` date: scheduled publication; `ifc_param4=1` + `ifc_param5`: scheduled withdrawal

Scheduled into the tree, the node is created at once without a URL: an unused placeholder with no stub and no menu entry until the schedule fills in `content://`. If the node is gone when the schedule falls due, the document is not published and the schedule is dropped. This route honours editorial roles.

### Bare Node: `directory`

`add_insert` / `add_append` with `ifc_param=<target id>` create a node only. The add form numbers differently: name 1, hidden 2, hide empty 3, delete automatically 4, description 5, URL 6, icon 7. Path, canonical base and images need a `save` afterwards.

## Changing the Tree

| Task | `directory` | `easypage` |
|------|-------------|------------|
| move, `ifc_param=<source>,<target>` | `cut_insert`, `cut_append` | `insert`, `append` |
| copy subtree, new ids, same targets | `copy_insert`, `copy_append` | — |
| rename, hide | `save` | `save` |
| sort subtree below `object` by name, all levels | `sort`; no-op on root | — |
| remove dead nodes | `clean` | — |
| delete subtree, `ifc_param=<id>` | `del` | `delete` |

**`clean`** deletes every node in the selected subtree (whole tree without selection) that has no URL, is not hide-empty and has no descendant with either.

**`easypage`/`save`** writes name and hidden; on a `content://` node also title (`ifc_param3`), description (`4`) and keywords (`5`) of the document, live and buffer alike, bypassing editorial roles. Every field is written from the post: an omitted title, description or keyword is emptied, an empty default-language name becomes the placeholder name.

**Delete differs.** `directory`/`del` removes nodes only; their documents stay published, reachable by index, and leave the sitemap. `easypage`/`delete` also **deletes every document linked from the subtree that no remaining node links**, with versions and schedules, and drops the publish and withdraw schedules aimed at any node of the subtree. A document not in *publication* needs your editorial role for that; otherwise the response is `msg_error` and it remains.

## Withdrawal

`content`/`withdraw` with `ifc_param=<index>` removes `content://<index>` from every linking node and sets the document back to *document*. The content app's delete does the same before deleting.

Then the whole tree is swept, not only those nodes: every delete-automatically node without URL, pending scheduled publication or surviving descendant is removed. Other nodes stay without a target and answer 404.

A scheduled withdrawal releases only the node it was scheduled with; the document stays published while other nodes still link it.

## Permissions

| Permission | Allows |
|------------|--------|
| `interface.directory` | all node operations |
| `interface.easypage` | creating, publishing and deleting published documents, writing their metadata — **regardless of editorial roles** |
| `interface.content` + roles | the editorial route |

Writing into a page still needs `content.writer.<index>`; see [Permissions](../Permissions/SKILLS.md).

## Traps

**Foreign files in generated folders.** A folder holding an unlogged file cannot be removed. Regeneration then aborts after deleting the other stubs: the site answers 404 and every tree change returns `msg_error` until the file is gone. Put shared files in the auxiliary folder.

**Foreign names.** A foreign file or folder at a generated path pushes the stub or folder to `-1`. A foreign folder named like a parent node (or like a language) moves it to `name-1/`; all its children and descendants share that one folder.

**Same-named siblings** get `name.php` and `name-1.php`, but their children share the folder `name/`.

**`easypage` publishes on creation.** Hidden does not keep the URL private; use the editorial route for a page that must not go live empty.

**Moving a node into its own subtree** copies it in `directory`; `easypage` refuses with `msg_error`.

**Deleting a node** leaves `directory://` references to it dangling: such nodes and links resolve to the site root.

**`directory.log` is load-bearing.** Never edit a stub or the log. Without the log, old stubs stay and block their names.

## Troubleshooting

| Symptom | Cause |
|---------|-------|
| URL ends in `-1.php` | collision with a sibling or a foreign file |
| Old URL still answers after a move | the file is not in `directory.log` |
| Node answers 404 | no URL, withdrawn or deleted document, or failed regeneration |
| Node or link lands on the site root | its `directory://` target node was deleted |
| Page answers 410 | document not published; editors still see it |
| Hidden page in the sitemap | hidden only affects menus; flag the document sitemap-exclude |
| Page missing from the sitemap | not published, no `content://` node links it, or flagged exclude/noindex |
| Node gone after a withdrawal | it was set to delete automatically |
| Override path ignored for a child | overrides are not inherited |
| `msg_error` on a tree change | tree save or regeneration failed; see [Diagnostics](../Diagnostics/SKILLS.md) |
| 400 on every URL | host missing from the canonical host list |

---

[PWNC Website](https://pwnc.it)