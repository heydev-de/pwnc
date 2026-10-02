---
name: PWNC Memory Manual
description: Use this skill whenever you store, retrieve, rate or organise knowledge in PWNC's shared memory.
---
# PWNC Memory Manual

## Table of Contents

- [Introduction](#introduction)
- [Regions](#regions)
  - [Addressing](#addressing)
  - [Auto-Creation](#auto-creation)
- [The Three Tools](#the-three-tools)
- [Recalling](#recalling)
  - [Mode Table](#mode-table)
  - [Discovery](#discovery)
  - [Region Overview](#region-overview)
  - [Search](#search)
  - [Reading an Entry](#reading-an-entry)
- [Mnemonics](#mnemonics)
  - [How They Are Normalised](#how-they-are-normalised)
  - [Navigating by Mnemonic](#navigating-by-mnemonic)
- [Remembering](#remembering)
  - [Overwrite Semantics](#overwrite-semantics)
  - [Field Limits](#field-limits)
- [Attachments](#attachments)
- [Follow-Ups](#follow-ups)
- [Flags](#flags)
- [Rating and Lifetime](#rating-and-lifetime)
- [Permissions](#permissions)
- [Troubleshooting](#troubleshooting)

---

## Introduction

Memory is PWNC's shared, searchable store for anything worth keeping — notes, decisions, references, drafts, instructions, working state, files. There is no fixed subject matter and no schema beyond a description, some retrieval cues, an optional body and an optional attachment.

Whatever is stored can be found again by anyone holding rights to its region: you on a later request, or another accessor entirely.

**Key concepts:**
- **Regions** — every memory lives in exactly one region, addressed as `user.{username}` or `scope.{token}`
- **Mnemonics** — short retrieval cues attached to each entry; the primary way to navigate
- **Ratings** — entries are rated 1 to 5, and low ratings make them expire sooner
- **Permissions** — reader, writer and operator rights are granted per region

What deserves an entry is your judgement. The one thing that pays off consistently is choosing good mnemonics — an entry nobody can find again might as well not exist.

---

## Regions

A region is one namespace of memories. It maps to its own database table plus a mnemonic index, and permissions are granted per region.

| Form | Meaning |
|------|---------|
| `scope.{token}` | A shared, topic-scoped memory |
| `user.{username}` | The personal memory of one accessor |

The identifier after the dot must match `[-0-9a-z_]{1,40}` — lowercase letters, digits, hyphen and underscore only. Invalid identifiers are rejected, never sanitised.

### Addressing

A single memory is addressed as `region/id`, for example `scope.docs/17`. That is the form shown throughout the output, the form used by the resource URI (`memory:scope.docs/17`), and the form all three tools take as their `id` parameter. Output can be handed straight back as input.

The slash separates region from entry number, and either side may be left out:

| `id` | Means |
|------|-------|
| *(omitted)* | your personal region, no entry |
| `17` | entry 17 in your personal region |
| `/17` | the same |
| `scope.docs` | the region, no entry |
| `scope.docs/` | the same |
| `scope.docs/17` | entry 17 in that region |

An absent region always resolves to your personal region. A number counts as an entry only when it stands alone or follows a slash, so a region whose name ends in digits stays a single region name — `scope.test2` is that region, never `scope.test` entry 2.

### Auto-Creation

Regions are created on first write. There is no separate create step — `memory_remember` with a new region name creates it, provided you hold writer rights on it. A region that loses its last entry is removed again by garbage collection; writing to the name recreates it, and the permission grants are unaffected either way.

---

## The Three Tools

| Tool | Purpose |
|------|---------|
| `memory_recall` | Discover regions, browse, search, read |
| `memory_remember` | Create or update an entry |
| `memory_rate` | Rate 1 to 5, or delete with 0 |

`memory_remember` and `memory_rate` resolve an absent region to your personal region, so `memory_rate {id: "17"}` rates entry 17 of your own memory. `memory_rate` still needs an entry number and rejects a region on its own. `memory_recall` does not default at all — an omitted `id` selects discovery mode rather than a region.

---

## Recalling

### Mode Table

| Parameters | Mode | Returns |
|------------|------|---------|
| none | Discovery | Every readable region, its rights and top mnemonics |
| `query` | Cross-region search | Matches across all readable regions |
| `id: region` | Overview | The region's mnemonic cloud and most relevant entries |
| `id: region` + `query` | Region search | Matches within that region |
| `id: region/number` | Read | The full entry including content |

An `id` naming an entry wins over `query`. Only the read mode returns `content`.

### Discovery

Start here when you do not know what exists. Each region is listed with your rights and its ten most common mnemonics, which is usually enough to decide where to look.

### Region Overview

Returns up to 100 mnemonics and the 20 most relevant entries. Relevance combines the rating with recency of the last write.

### Search

Full-text over mnemonics, description and content, weighted so a mnemonic match ranks highest. A region search returns up to 200 entries; a cross-region search takes up to 20 per region and keeps only results scoring within a quarter of the best.

Both modes append a mnemonic cloud of the results' cues. **The counts in that cloud are region-wide totals, not counts within the result set** — `oauth (12)` means twelve entries in readable regions carry that mnemonic, however many the search returned. Use them to judge how much material sits behind a term.

> **Cross-region search only finds mnemonic matches.** For speed, a region is skipped unless one of the query words appears in its mnemonic index. An entry whose match is only in its description or content will be found by a region search but not by a cross-region one. If you expect something and it does not appear, search the region directly.

### Reading an Entry

Returns the description, mnemonics with counts, metadata, the full content, and an attachment block if one exists. **Reading clears a due follow-up on that entry.**

---

## Mnemonics

Mnemonics are the retrieval cues of an entry — the words you would search for later. Choose terms you would plausibly arrive at from a different angle, not a restatement of the description.

### How They Are Normalised

Input is split on commas and newlines, then:

- lowercased
- trimmed, with empty segments dropped
- de-duplicated
- naturally sorted
- each term truncated to 100 characters at a word boundary
- the whole list truncated to 500 characters

A truncated term ends in `…`, which becomes part of the stored key — so an over-long mnemonic can no longer be matched by typing the full phrase. Keep them short.

### Navigating by Mnemonic

The intended path is **discovery → refine → read**:

1. `memory_recall` with no parameters to see which regions exist and what they are about
2. `memory_recall {query}` or `{id: region}` to get a cloud of cues
3. Search again using a cue from the cloud to narrow
4. `memory_recall {id: "region/number"}` to read what you found

---

## Remembering

```
memory_remember {id, mnemonic, description, content}
```

An `id` naming a region creates a new entry in it; an `id` naming an entry updates that entry. Omit `id` entirely to create in your personal region. A create returns the new address.

### Overwrite Semantics

On update, each field behaves independently:

| You send | Result |
|----------|--------|
| the field omitted | previous value preserved |
| a value | replaces the previous value |
| `false` | clears the field back to its default |

`false` is the only way to clear a field. Sending an empty string stores an empty string.

### Field Limits

| Field | Limit |
|-------|-------|
| `description` | 500 characters, truncated at a word boundary |
| `mnemonic` | 100 characters per term, 500 total |
| `content` | no practical limit |

Whitespace in the description is normalised when stored, so the 500-character budget is spent on words rather than formatting.

---

## Attachments

```
memory_remember {id: "scope.docs/17", attachment: {type: "image/png", data: "<base64>"}}
```

`type` is the MIME type and `data` is base64-encoded file content. `attachment: false` deletes the file.

Attachment bytes are **never** included in a recall. The read view shows the type, size and a resource URI instead:

```
**Resource URI:** `memory:scope.docs/17`
```

Fetch it with `resources_read {uri}` when you actually need the content. This keeps a read cheap whether the entry carries a note or a megabyte.

---

## Follow-Ups

```
memory_remember {id: "scope.docs/17", followup: "2026-09-14 09:00:00"}
```

A follow-up surfaces at the top of **every** `memory_recall` response until the entry is read, which clears it. Use it for something you must return to, not as a general scheduling mechanism — there is no notification, only the reminder line.

- The date is read in your timezone and stored as UTC.
- An unparseable date is **rejected** — the whole call fails with `Invalid follow-up date` and nothing is written. It is never silently ignored.
- `followup: false` cancels a pending reminder.

---

## Flags

| Flag | Effect |
|------|--------|
| `read_only` | Only the owner and operators may edit the entry |
| `safe` | The entry can never be deleted, by rating or by garbage collection |

`read_only` restricts *editing*. Rating still works, and it does not hide the entry — a writer who is neither the owner nor an operator can read and rate it but not change it.

`safe` is absolute. Even an operator cannot delete a `safe` entry; clear the flag first. That two-step is deliberate.

---

## Rating and Lifetime

```
memory_rate {id: "scope.docs/17", rating: 4}
```

Ratings are 1 to 5 and averaged. `rating: 0` deletes the entry, and is accepted only from the owner or an operator, never on a `safe` entry.

Unrated entries sit at 3.00. Garbage collection deletes an entry only when **both** clocks have run out:

```
time since last write > (180 / 5) × rating  days
time since last read  > ( 90 / 5) × rating  days
```

So a 1-star entry expires after 36 days unedited and 18 unread, a 5-star one after 180 and 90. Reading an entry resets the read clock, editing resets the write clock — anything in active use survives indefinitely.

Rate honestly. A low rating is not a deletion; it lets something fade if nobody returns to it, which is the intended way for stale knowledge to disappear.

---

## Permissions

Rights are granted per region as permission strings:

```
memory.user.{username}.reader
memory.scope.{token}.writer
memory.scope.{token}.operator
```

| Right | Allows |
|-------|--------|
| `reader` | See the region in discovery, browse, search, read |
| `writer` | Create and update entries, rate them |
| `operator` | Also edit `read_only` entries and delete entries owned by others |

`operator` implies `reader` and `writer`. A bare `memory` grant gives you operator rights on your own `user.{yourname}` region.

Two things that surprise people:

- **The wildcard binds to the right, not the region.** `memory.scope.test.*` grants all three rights on `scope.test` and grants nothing on `scope.test2`.
- **`writer` does not imply `reader`.** With writer alone you can create entries in a region that never appears in your discovery listing and that you cannot read back.

Your current rights are shown in every discovery, overview and read response.

---

## Troubleshooting

### Nothing Found

| Symptom | Likely Cause | Fix |
|---------|--------------|-----|
| Cross-region search finds nothing, region search finds it | The match is in content only, and cross-region search pre-filters on the mnemonic index | Search the region directly, and add the term as a mnemonic |
| A region is missing from discovery | No `reader` right on it | Request `memory.{type}.{identifier}.reader` |
| `Not available` on a region you can write to | You hold `writer` without `reader` | Request the reader right as well |

### Writing

| Symptom | Likely Cause | Fix |
|---------|--------------|-----|
| ``Invalid `id` `` | `id` has more than one slash, or a non-numeric entry number | Use `{region}`, `{region}/{number}`, `/{number}` or `{number}` |
| `Invalid follow-up date` | The date could not be parsed | Use `Y-m-d H:i:s`; send `false` to cancel instead |
| `Remembering failed` on a new region | Invalid identifier, or no writer right | Identifiers must match `[-0-9a-z_]{1,40}` |
| `Update failed` on someone else's entry | The entry is `read_only` and you are not the owner or an operator | Ask the owner, or request operator rights |
| An update wiped a field you did not send | You sent `false` rather than omitting it | Omit a field to preserve it |

### Rating

| Symptom | Likely Cause | Fix |
|---------|--------------|-----|
| `Rating failed` with `rating: 0` | The entry is `safe`, or you are neither owner nor operator | Clear `safe` first, or ask the owner |
| `Rating failed` with a number | Out of range | Ratings are 1 to 5; 0 deletes |

### Attachments

| Symptom | Likely Cause | Fix |
|---------|--------------|-----|
| Type comes back as `application/octet-stream` | `type` was not sent in the attachment object | Send `{type, data}`, not just `data` |
| `Resource not found` on a `memory:` URI | The entry has no attachment, or you lack reader rights | Check the read view for an attachment block |

---

[PWNC Website](https://pwnc.it)