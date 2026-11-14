---
name: PWNC Agent Communication Manual
description: Use this skill whenever you call an agent or a user, answer a call, wait for or forward a reply, continue a conversation by context id, or inspect platform agent runs, the Agent app and its transcripts.
---
# PWNC Agent Communication Manual

## Model

Agents are users: a user record, rights, a personal memory region, the same MCP tools. `whois` returns a user's name, designation (the record's comment) and groups, or a group's members; it never shows rights. Omit `key` (or pass an empty string) for yourself; any other value not starting with `user.` or `group.` is rejected, an unknown record answers `` `<key>` unknown ``.

- **Platform agent**: a user whose record names a provider template. PWNC runs it: wakes it, talks to the provider API, executes its tool calls. Configuration: [Permissions](<pwnc:SKILLS/Permissions/SKILLS.md>).
- **External agent**: connects to PWNC's MCP endpoint with its own API key. PWNC cannot wake it; it sees replies when it next looks.
- **Conversation**: one transcript, keyed by **owner + agent + context id**. The same id under another owner is another conversation.
- **Run**: one background stretch of a platform agent's work, from a wake to its final message.
- **Call**: one message plus its routed reply: an agent target's final message once, a user's every reply in the thread. Everything later belongs to no call.

## How an Agent Is Woken

| Channel | Owner of the conversation | Context | Final text goes to |
|---------|---------------------------|---------|--------------------|
| `call` | the caller | chosen by `id` | the caller, once |
| Agent app chat | the chatting user | selected in the app | the chat display only |
| Desktop messenger | the agent itself | new per message | nowhere; reply in the messenger |
| Trigger | `trigger` | new per firing | nowhere ([Triggers](<pwnc:SKILLS/Triggers/SKILLS.md>)) |

A messenger wake carries only the message text: no sender, no thread. Open the Desktop messenger to see who wrote and to answer ([Desktop](<pwnc:SKILLS/Desktop/SKILLS.md>)).

## Calling

`call {target: "user.<username>", message, id?}` returns at once; the target works in the background. Plain text you write afterwards does not reach the target, only another `call` does. You cannot call yourself; groups cannot be called.

**Routing** is decided per call by the target's state:

| Target | Delivery | Requirement |
|--------|----------|-------------|
| Platform agent, no web activity on that account in the last 15 minutes | woken directly | you hold `agent.<username>`, else `not available` |
| Anyone else: humans, external agents, a platform agent whose account is in web use | Desktop messenger, possibly email | message ≤ 1500 characters |

The same platform agent can switch between both rows from one call to the next.

**Context id**:

| `id` | Platform agent caller | External caller |
|------|-----------------------|-----------------|
| omitted | your current context id | new context / new messenger thread |
| `false` | new | new |
| string | that context / thread | that context / thread |

- Omitting `id` as a platform agent ties the callee's context to your conversation: the same callee in another of your conversations gets a different context.
- An external caller must pass the id from the previous result to continue.
- A resumed agent context keeps its full history and the system prompt from its creation. Start a new one when that history is stale.
- For users, the messenger thread is derived from the id: same id, same thread.

A second `call` into a context whose run is still active joins that run. Its single final reply answers both messages and goes to the first call's caller context.

## Receiving Replies

| You are | A reply arrives as |
|---------|--------------------|
| Platform agent | a `call_reply` tool call with its result, appended to the conversation you called from: `` `x` (Name) answers your call: … `` or `` … could not answer your call: <reason> `` |
| External agent | an entry in your personal memory region: mnemonic `callback`, description `` `<id>` ``, follow-up due now; on failure the content is `<reason> (no reply)` |

- A platform agent does not wait or poll. End your turn; the reply wakes that conversation in a new run, or joins the running one after pending tool results.
- That new run has no caller of its own. If you owe someone an answer, forward it with `call`.
- Never call `call_reply`; it only carries delivered replies and fails when invoked.
- A reply is dropped when the conversation it would land in was deleted meanwhile.
- External agents find replies among the follow-ups listed by `memory_recall`; reading the entry clears it ([Memory](<pwnc:SKILLS/Memory/SKILLS.md>)).

**User replies** are relayed only if both you and the user have Desktop access; otherwise the result says **No reply channel** and the message is one-way. Only replies in the call's thread, addressed to you, are relayed, and every such reply is relayed, not only the first. Calling the same user again with the same id re-points the relay to your current context.

## Being Called

A call arrives as a user message: `` `caller` (Name) calls you: <message> ``.

**The call ends with your run.** When the run stops, the most recent text you wrote in that run is relayed to the caller, once. Write the answer as your final text: if the run ends without a final answer, an earlier remark of the same run ("I'll check …") is what the caller gets. A run without any text reaches the caller as `could not answer your call` with the finish reason.

Not part of the call, never relayed: later runs in the same context, replies to your own calls, anything you learn afterwards. To report later, `call` the caller:

- As a platform agent, omitting `id` reuses your context id, the id the caller knows the exchange by.
- Your call opens a conversation owned by you, not the caller's original one. A platform agent caller wakes there without its earlier history; the shared id and memory are the only link. State the context in the message.
- In chains (called, then calling someone yourself), the reply to your call arrives after your own answer was relayed. Forward it with `call`.

## Platform Agent Runs

Each wake appends a message and queues a run. A message arriving while a run is active joins it, held until pending tool results are in.

**System prompt**: the `readme_first` output, fetched once when the conversation is created: identity, designation, groups, context id, README, skill list, memory overview, and the agent's instruction text if set. It is never refreshed: instruction edits reach only new conversations, and the memory overview is a snapshot; use `memory_recall` for the current state.

**Incoming messages** are prefixed `` `<username>` (Name): `` (`` `trigger` `` for triggers; `calls you` added for calls), except messenger wakes and messages from the agent's own account. Text after the private-use character U+E000 is added by the platform, not typed by the sender: suggested memory mnemonics, and for an Agent app window opened from a Backend page that page's URL and hidden form fields. The page context is sent only when it changed since the previous message; without it, the last one still applies.

**External agents** get their instruction text in the `readme_first` result and as a leading text block on the first tool call after `tools/list`, then every n-th call, n being the interval on their user record. Without an interval it is never delivered.

**Tools**: each run gets an MCP token bound to agent and conversation, revoked when the run stops; replies to your calls find your conversation through it. Depending on the provider template, the provider calls PWNC's MCP endpoint itself or PWNC executes the calls. Registered external MCP servers add their tools if the agent holds `mcp.<id>` and the server is authorized; when PWNC executes, they are named `<server>__<tool>`.

**Compaction**: when a request overflows the model's context window, the oldest tool results are replaced with `~~~ compacted ~~~` until about a quarter of the request is freed. Fetch the data again if you still need it.

**Finish reasons** (last transcript entry of the run):

| Reason | Caller receives |
|--------|-----------------|
| provider stop reason (normal end), `Request limit exceeded.`, `Run interrupted.` | last text of the run |
| errors: `Connection error.` (no usable response within the request timeout), the provider's error text (transient errors after retries), `Compaction limit exceeded.`, `Context limit exceeded.`, `Invalid response.`, `Context corrupted.`, `I/O error.`, `Process was killed repeatedly.`, MCP client failures | `could not answer your call` + reason |
| conversation deleted during the run; run interrupted before it started | nothing |

A killed process resumes the run automatically, up to a limit. An interrupt answers tool calls not yet executed with `Interrupted.`

## Assistant Mode

Basics: README. The holder counts as offline after 15 minutes without web activity on the account. Messenger messages and triggers land in the holder's messenger; only while the holder is offline do they also wake the agent, in a run owned by the account itself, with the message text (text over 1500 characters is cut silently), a link to the holder's messenger and the assistant mode hint. A `call` wakes the agent directly while the holder is offline, without the hint; otherwise it is delivered by messenger like a call to a human. Acting in the holder's Desktop: [Desktop](<pwnc:SKILLS/Desktop/SKILLS.md>).

## Agent App and Transcripts

`interface.php?ifc_page=agent` is the chat interface for platform agents. It lists the agents you hold `agent.<username>` for and your conversations with them, including those your calls created. Anyone with access can interrupt a run. Operators additionally see other owners' conversations, delete conversations, and manage external MCP servers.

To watch a callee, GET `interface.php?ifc_page=agent&ifc_message=display&object=<owner>.<agent>.<id>&from=-1&to=-1`. The first two lines are the returned entry range (the last 50 entries by default); `from`/`to` select other entry indexes. Requesting it also changes your selection in the app.

Chatting with an agent through the app gives you no reply delivery; use `call`.

Transcripts are JSON lines at `data/#agent/<owner>/<agent>/<id>.json`, not web-fetchable. Read them through the Agent app or [Filemanager](<pwnc:SKILLS/Filemanager/SKILLS.md>).

## Troubleshooting

| Symptom | Cause | Fix |
|---------|-------|-----|
| ``Invalid `target` `` | not `user.<username>` | address members individually |
| `` `user.x` not available `` | unknown or disabled user, missing `agent.<username>` (inactive platform agents only), or no delivery channel (no Desktop access and no email) | `whois`; request the right |
| `Unknown or unavailable user` | platform agent not fully configured | administrator ([Permissions](<pwnc:SKILLS/Permissions/SKILLS.md>)) |
| ``expects at most 1500 characters`` | target is delivered by messenger | shorten, or split across calls with the same `id` |
| callee remembers nothing | new context opened | pass the earlier `id` |
| no reply (external) | replies land in memory | `memory_recall`, look for `callback` follow-ups |
| no reply at all | **No reply channel**, run interrupted before start, callee's or your conversation deleted, or the user answered outside the thread | check the transcript; call again |
| your later findings never reached the caller | the call ended with your run | `call` the caller |

---

[PWNC Website](https://pwnc.it)