---
name: PWNC Trigger Manual
description: Use this skill whenever you create, inspect, test, edit, disable or delete a trigger, or when you were woken by one and need to know why it fired and what it implies.
---
# PWNC Trigger Manual

## Concept

A trigger is a stored rule set in the Backend app **Trigger** (`pwnc/module/interface.php?ifc_page=trigger`). When it fires, it sends its message to its target users: an agent is woken with a new run, a human gets a messenger message or an email.

PWNC has no clock. Triggers are evaluated **at the end of every request** PWNC handles, after the response has been generated. Timing therefore depends on traffic, and every request is a potential firing.

Per request, each trigger that is enabled and has a target is checked once:

1. **Schedule due** → fire. Conditions are not consulted.
2. Otherwise, if at least one condition field is filled and **all filled fields match** → fire.

Schedule and conditions are alternatives (OR), never filters on each other. One request fires a trigger at most once, but can fire several triggers.

Firing freezes the message (placeholders, language of the firing request) and queues a background task; the background worker delivers it shortly after. Delivery reads the targets anew; a trigger deleted before delivery delivers nothing.

There is no dry run, rate limit or cooldown. Each firing with an agent target starts a new, paid agent run.

### Which requests are evaluated

| Request | Evaluated |
|---------|-----------|
| Browser requests to Frontend, Backend, Desktop | yes |
| MCP `get` and `post`, including those of the woken agent | yes |
| The background worker (`daemon.php`), where agent runs execute | no |
| MCP tools other than `get`/`post`, and `get`/`post` that end in an MCP error | no |
| Requests that end during identification (login redirect, security-token mismatch, logout, MCP authentication failure) | no |
| Files the web server delivers without PHP (images, CSS, …) | no |

A woken agent's own `get`/`post` requests can fire the trigger that woke it, which wakes a new run. This is intended behaviour; the rules must exclude the agent's traffic.

MCP `get`/`post` with a URL that is not a `.php` endpoint runs through the HTTP proxy, which is itself an evaluated request: user = you, application `http`, URL = this site's host with the target's path and query, output = the fetched body. If the target is a page of this site, the proxied fetch is a second evaluated request, as anonymous visitor. One `get` can thus fire a URL rule twice.

## Schedule

`schedule`: one rule per line. The next run is the earliest upcoming time over all valid lines. Invalid lines are ignored silently — check **Next run** after saving.

| Rule | Format | Example | Next run |
|------|--------|---------|----------|
| Interval | `#d#h#m`, parts optional, in this order, no spaces | `1d12h`, `90m` | Evaluation time + interval |
| Cron | 5 fields: minute hour day month weekday | `0 7 * * 1-5` | Next matching minute |
| Point in time | `YYYY-MM-DD hh:mm` | `2026-12-24 18:00` | That minute, if still in the future |

**All rule times are UTC**, independent of any account timezone. The **Next run** column displays in the viewer's account timezone.

Cron fields: `*`, `n`, `n-m`, `*/s`, `n-m/s`, comma lists. `n/s` means only `n` — write `n-max/s` (`5-59/15`). Weekday `0`–`7`, `0` and `7` are Sunday. Names (`MON`), `?`, `L`, `W` are invalid. Day and weekday both restricted → either match suffices; one starting with `*` → both must match.

Timing:

- Fires on the **first request after** the due time.
- Missed runs collapse into one firing; the next run is computed from that moment, so intervals drift by the traffic gap.
- **Every save recomputes the next run from now**: it restarts intervals and discards a pending due run.
- A point in time fires once; afterwards, with no other valid line, Next run shows `–`.
- Concurrent requests cannot fire the same due schedule twice (one lock spans check and update). Conditions have no such guard; every matching request fires.

The app's schedule builder is JavaScript that appends such a line to `schedule`; write the line directly.

## Conditions

| Field | Matched against |
|-------|-----------------|
| `url` | Normalized absolute request URL with query string |
| `param` | `name=value` rules against request parameters |
| `inclusion` | Response output; fires only if a line matches |
| `exclusion` | Response output; blocked if a line matches |

One rule per line; any matching line satisfies its field. AND within one field is not possible — split the patterns across `inclusion` and `exclusion`, or across fields.

Rule syntax:

- Substring match anywhere in the value, case-sensitive.
- `*` matches any run of characters (newlines included); any whitespace matches one or more whitespace characters; `\` escapes the next character. Leading and trailing whitespace of a line is dropped.
- A leading `!` negates the line: it matches when the pattern is absent. Two negated lines in one field mean "a absent OR b absent".

`url` sees the normalized form: default port removed, duplicate slashes collapsed, query parameters **sorted by name**, values percent-encoded. Write `*ifc_page=trigger*`, never a fixed parameter order. POST data is not part of the URL; use `param`.

`param`:

- The value is that of the global variable `name` at request end: normally the request parameter (POST before GET before cookie), but code may have changed it during the request.
- An absent parameter is the empty string. `name=` and `name=*` always match; presence cannot be tested.
- Array values are skipped. Lines without `=` or with an empty name are dropped.
- Negation prefixes the whole line: `!name=value`. `name=!value` matches the literal text `!value`.

Output rules (`inclusion`, `exclusion`):

- They see the page body still buffered at request end: no headers, not the MCP `llm` view — inspect with `view: raw` before writing one. Output already flushed is not seen; with nothing buffered no output rule can match, an `exclusion` included.
- For proxied MCP fetches the output is the fetched body.
- Output larger than 1 MiB: a trigger with any output rule does not fire, even with `exclusion` only. Binary output counts as empty.

`url`, `param`, `inclusion`, `exclusion` and `message` are multilingual fields (see README): the value for the request's active language applies. Set only the fallback value unless rules must differ per language.

## Targets and Delivery

`target` lists usernames. The app offers every enabled user who is a platform agent, has an email address or has Desktop access. Disabled users are skipped at delivery.

| Target | Delivery |
|--------|----------|
| Agent | New agent run per firing, owned by user `trigger`; full message |
| Agent in assistant mode | Messenger message (if Desktop access); wakes the agent only while the holder is offline ([Assistant mode](<pwnc:SKILLS/Agent communication/SKILLS.md>)) |
| Human | Messenger message (if Desktop access); email (if address set) only after 15 minutes without web activity and with no earlier unread messenger message pending; the email holds only a Desktop link when the messenger got the message |

For humans and assistant-mode agents, text over 1500 characters is cut silently. A human without Desktop access who had web activity within 15 minutes receives nothing.

**Woken by a trigger** (non-assistant agent): the first prompt is the message, prefixed with `` `trigger` `` and the trigger user's name, possibly followed by suggested memory mnemonics. No earlier firing's context is carried over, and nobody receives your final reply. Put results where they are needed: messenger, memory or content. Prompt format, assistant mode and transcripts: [Agent communication](<pwnc:SKILLS/Agent communication/SKILLS.md>).

## Message and Placeholders

The message is the instruction. Write it for a fresh agent with no other context. Empty message → a default text naming trigger, URL, application, user and time.

| Placeholder | Schedule firing | Condition firing |
|-------------|-----------------|------------------|
| `%name%` | Trigger description | Trigger description |
| `%time%` | Firing time, UTC | Firing time, UTC |
| `%url%` | empty | Request URL as requested (not normalized) |
| `%application%` | empty | Application, e.g. `content`, `interface.<page>`, `http` |
| `%user%` | empty | Requesting user, `anonymous` for visitors |

`[text%x%text]` is conditional: with a value the brackets vanish and the text stays, with an empty value the whole bracket disappears. One placeholder per bracket, no nested brackets.

## Managing Triggers

The list shows description, **Next run** (`–` = disabled, no valid line, or no future time) and **Executions/day**: a smoothed daily average, the current day's count until the first day has passed. It is neither a live counter nor a limit.

| Action | `ifc_message` | Fields |
|--------|---------------|--------|
| Select | `select` | `ifc_param`: trigger id |
| Create | `add` | creates a trigger with a default message and selects it |
| Save | `set` | all settings fields of the selected trigger |
| Delete | `del` | `list`: array of trigger ids |

Trigger ids are in the list rows (`s('<id>')`, `list[]` values). The selection is remembered per user; posting `object: "<id>"` selects explicitly. A `set` for an unknown id does nothing.

**`set` overwrites every field with what is posted.** An omitted field is cleared; an omitted `disabled` means enabled, any posted `disabled` value (even `0`) disables. `target` must be an array, also for one user. Read the selected trigger first, copy every value, change what you need, post all of it:

```
post {url: "…/pwnc/module/interface.php?ifc_page=trigger", form_data: {
  ifc_page: "trigger", ifc_message: "set", object: "<id>",
  name: "…", url: "", param: "", inclusion: "", exclusion: "",
  schedule: "0 6 * * *", target: ["<username>"], message: "…"}}
```

Disabling clears the next run; saving enabled recomputes it from now. A trigger never fires without a target and either a schedule or a condition.

### Testing

Every test firing is real; with agent targets it costs a run.

- Schedule syntax: save, check **Next run**. Proves parsing, not delivery.
- Delivery: target a human first (messenger), or a point in time a few minutes ahead (UTC); switch to the real target afterwards.
- Conditions: narrow rules, one matching request of your own, then check the target's messenger or agent runs. Every request of yours is evaluated as well.
- Disable or delete test triggers afterwards.

## Troubleshooting

| Symptom | Likely cause | Fix |
|---------|--------------|-----|
| Next run `–` | Disabled, no valid line, or only past points in time | Check syntax; cron needs exactly 5 fields |
| Next run at an unexpected hour | Rules are UTC, the column shows your timezone | Convert to UTC |
| Fired late | No traffic between due time and firing | Expected |
| Interval never comes round | Trigger saved repeatedly; each save restarts it | Save less often or use cron |
| URL condition never fires | Query order or encoding differs from the normalized URL | Use `*` between parameters |
| Condition never fires | A filled field does not match; language-specific value set; output over 1 MiB | Check every filled field; set the fallback value |
| Agent woken over and over | Rules match common requests, including the woken agent's own `get`/`post` | Narrow rules; exclude the agent's URLs or parameters |
| Two firings per MCP `get` of a page | Proxy request and proxied page request both match | Expected; see *Which requests are evaluated* |
| Settings vanished or trigger re-enabled | `set` posted with fields omitted, or without `disabled` | Always post every field |
| Target receives nothing | Target disabled; human without Desktop access who is active or has no email | Pick another target |

---

[PWNC Website](https://pwnc.it)