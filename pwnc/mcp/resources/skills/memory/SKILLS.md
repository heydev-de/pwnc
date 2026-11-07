---
name: PWNC Memory Manual
description: Use this skill whenever you store, retrieve, rate or organise knowledge in PWNC's shared memory, or when an entry, follow-up or region does not behave as expected.
---
# PWNC Memory Manual

## Model

- A **region** is one namespace: its own database table plus a mnemonic index, and the unit of permission. `scope.<token>` is shared by topic, `user.<username>` is one accessor's personal region. Identifier: `[-0-9a-z_]{1,40}`; anything else is rejected, never sanitised.
- An **entry** is a numbered record: description (the listing line), mnemonics (indexed retrieval cues), content (shown only when read), optional attachment, flags, rating, follow-up. Its owner is the account that created it.
- **Mnemonics carry findability.** They gate cross-region search, rank highest in fulltext, form the clouds you navigate by, and train the classifier behind the README's suggested mnemonics (entry text labelled with its mnemonics).
- **Entries fade unless used.** Rating scales two expiry clocks; reading and editing reset them.
- **State is per entry, not per accessor.** Read count, follow-up and rating are shared by everyone who can see the region.

## Addressing

`<region>/<number>` everywhere: in output, as `id` of all three tools, and as attachment URI `memory:<region>/<number>`. Output can be passed back as input.

| `id` | Means |
|------|-------|
| omitted or `""` | `memory_recall`: discovery. `memory_remember`: create in your personal region |
| `<number>`, `/<number>` | entry in your personal region |
| `/` | your personal region |
| `<region>`, `<region>/` | the region |
| `<region>/<number>` | the entry |

A number counts as an entry only alone or after the slash: `scope.docs2` is a region. A second slash or a non-numeric entry part fails with ``Invalid `id` ``.

## Recalling

| Parameters | Returns |
|------------|---------|
| none | Every existing region you can read, your rights, its 10 most frequent mnemonics |
| `id: <region>` | Rights, 100 most frequent mnemonics, 20 entries ranked by relevance |
| `query` | Cross-region search |
| `id: <region>` + `query` | Region search, up to 200 hits |
| `id: <region>/<number>` | The entry with content and attachment info; wins over `query` |

- **Overview ranking** = `rating / 5 + 2 / (1 + days since last write)`. Fresh writes dominate for days; older entries drop out of the 20 and are reached by search.
- **Search** matches any word, word beginnings included, over mnemonics (weight 4), description (2) and content (1). Operators are stripped. The database's fulltext rules apply: very short words and stopwords may not match.
- **Cross-region search** first skips every region where no mnemonic contains a query word as a substring. Surviving regions are searched in full, up to 20 hits each, scores normalised so each region's best hit is 1.0. A content-only match is found only if its region passes that gate; search the region directly when you expect it.
- Both searches drop hits scoring below a quarter of the best one.
- **Clouds** under search results list the hits' mnemonics, each counted across the whole region (cross-region: summed over all readable regions), not within the result set.
- **Listings** show per entry: description, mnemonics, created/edited by whom, read count and last read, `read-only`, `safe`, `★<average> (<ratings>)` once rated, `📎<type>`. Content appears only in the read view.
- **Due follow-ups** head every response except the read view. `readme_first` embeds the discovery output, follow-ups included.
- `Not available.` covers a missing right, a region that does not exist yet and a missing entry alike.

## Writing

`memory_remember` with a region creates (`Remembered in <region>/<number>.`); with an entry, updates.

On update, per field: omitted keeps the value, a value replaces it, `false` clears. An empty string also clears mnemonic, description and content. **Every update**, even one that only sets a flag or follow-up, records you as editor and resets the write clock.

### Normalisation

- **description**: all whitespace, newlines included, collapses to single spaces; over 500 characters it is cut at a word boundary and ends in ` …`.
- **mnemonic**: split on commas and newlines only; `·`, `;` or `/` stay inside one term. Each term is trimmed and lowercased; empty terms and duplicates are dropped, the list is natural-sorted. Terms are kept in input order while the total stays within 500 characters, the rest are dropped whole. A term over 100 characters is then cut at a word boundary and ends in ` …`, which becomes part of the key.
- **content**: stored verbatim, limit 65,535 bytes. Longer content is truncated without an error; put large material into an attachment.

### Attachments

`attachment: {type: "<mime>", data: "<base64>"}` stores or replaces the file; without `type` it becomes `application/octet-stream`. `attachment: false` deletes it. Recall never includes the bytes: the read view lists URI, MIME type and size, `resources_read {uri: "memory:<region>/<number>"}` returns the file. Fetching an attachment needs reader rights and does not count as reading the entry.

### Follow-ups

`followup` takes any date expression PHP's `strtotime` understands (`2026-12-01 09:00`, `tomorrow 9:00`, `+3 days`), read in your account's timezone. An unparseable value fails the whole call with ``Invalid `followup` ``; no entry is created or changed. On update, `false` cancels.

- Due means the date has passed. A past date is due at once.
- A due follow-up is shown to **every** reader of the region, not only to whoever set it.
- **Reading the entry clears it** for everyone, due or not yet due. Re-set it if you still need the reminder.
- No view shows a pending date. There is no notification beyond the follow-up line.

Replies to an external agent's `call` arrive this way; see [Agent communication](../Agent%20communication/SKILLS.md).

### Flags

| Flag | Effect |
|------|--------|
| `read_only` | Only owner and operators may update the entry. Reading and rating are unaffected |
| `safe` | Exempt from expiry and from deletion, even by owner or operator. Clear it before deleting |

## Rating and Expiry

`memory_rate` with `1` (wrong/obsolete) to `5` (important/useful) needs writer rights and works on `read_only` entries.

- The rating is the average of all ratings given. Unrated entries count as `3.00`, but that default is not averaged in: the first rating replaces it.
- **Every call adds a rating**, repeated calls by the same accessor included. Rate an entry once.
- Rating resets neither clock.
- `rating: 0` deletes the entry with its attachment: owner or operator only, never a `safe` entry.

Garbage collection runs as a background task and deletes an entry that is not `safe` once **both** hold:

```
days since last write (edit, else creation) > 36 × rating
days since last read  (read, else creation) > 18 × rating
```

Rating 1 → 36/18 days, 3 → 108/54, 5 → 180/90. Only the read view counts as a read; appearing in an overview or search result does not. A low rating shortens both clocks; an entry that keeps being read or edited survives.

## Regions and Permissions

A region comes into existence with its first entry, written by anyone with writer rights on the name. Memory never removes a region: an emptied one stays in discovery without mnemonics.

| Access string | Covers |
|---------------|--------|
| `memory` | Reader, writer and operator on your own `user.<you>` region, which is checked against this string, not `memory.user.<you>.…` |
| `memory.<scope\|user>.<identifier>.<right>` | `reader`, `writer` or `operator` on that region |
| `memory.operator`, `memory.*` | Operator on every region, other accounts' personal regions included |

| Right | Allows |
|-------|--------|
| `reader` | Discovery listing, overview, search, read, attachments |
| `writer` | Create; update entries that are not `read_only`; rate; delete own entries |
| `operator` | Also update others' `read_only` entries and delete others' entries |

Matching follows the [Permissions](../Permissions/SKILLS.md) rules: `operator` or `*` as last segment covers the rights below it, so `memory.scope.docs.operator` includes reader and writer, and `memory.scope.docs.*` does not reach `scope.docs2`. `writer` does not include `reader`: you can create entries in a region you cannot list or read back. Your rights appear in the discovery, overview and read headers.

## Practice

- Search before creating; update or extend an existing entry instead of duplicating it.
- Reuse terms from the clouds as mnemonics, then add synonyms, ids and the angle a later reader will come from.
- Write the description as a self-contained one-line summary; listings show nothing else.
- Personal region for your own working state, a scope region for knowledge others need.

## Troubleshooting

| Symptom | Cause | Fix |
|---------|-------|-----|
| Cross-region search misses an entry | No mnemonic in its region contains a query word | Search the region; add the term as a mnemonic |
| Region missing from discovery | No reader right, or no entry was ever written | Request `memory.<type>.<identifier>.reader` |
| `Remembering failed.` | Invalid identifier or no writer right | Check `[-0-9a-z_]{1,40}` and rights |
| ``Updating `<number>` failed.`` | Entry missing, or `read_only` and you are neither owner nor operator | Ask the owner or an operator |
| ``Rating of memory `<number>` failed.`` | Out of range, entry missing, no writer right; with `0`: `safe`, or not owner/operator | Clear `safe` first, or ask the owner |
| A follow-up vanished | Someone read the entry | Set it again |
| Long content cut off | Over 65,535 bytes | Use an attachment |
| `Resource not found` on a `memory:` URI | No attachment, or no reader right | Check the read view for an attachment block |

---

[PWNC Website](https://pwnc.it)