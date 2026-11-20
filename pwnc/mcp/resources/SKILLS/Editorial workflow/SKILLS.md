---
name: PWNC Editorial Workflow Manual
description: Use this skill whenever you take part in the editorial workflow of the Backend content app — writing, reviewing, authorizing, publishing, withdrawing or forwarding documents, handing work to another account, working in another account's user box, or finding out which editorial role or combination of roles allows which step.
---
# PWNC Editorial Workflow Manual

Scope: the life cycle of a document in the content app (`interface.php?ifc_page=content`) and how work is split between accounts. Changing a document's content: [Content editing](<pwnc:SKILLS/Content editing/SKILLS.md>). Navigation nodes, publish targets and the sitemap: [Site structure](<pwnc:SKILLS/Site structure/SKILLS.md>). Operating Backend forms: [Forms](<pwnc:SKILLS/Forms/SKILLS.md>). Access strings: [Permissions](<pwnc:SKILLS/Permissions/SKILLS.md>).

## Mental Model

**Every document has a status.** *Draft* → *Document* (authorized, locked) → *Publication* (live). Visitors see only publications. In the Frontend a draft or document answers `410` to every account without the edit right on it, a Reviewer or Publisher without the Writer role included.

**Every document has two states.** Edits go to a buffer; *Apply* copies the buffer into the applied state. *Display*, *Info* and the published page show the applied state. A change that is not applied does not exist for the reviewer, the publisher or the visitor.

**Three roles move it forward.** Each role is a separate right. A step needs one role or a combination, and a combination counts only when **one** account holds all of it in the box.

| Role | Access string (own box) | UI label EN / DE | Main job |
|------|-------------------------|------------------|----------|
| Writer | `writer` | Writer / Editor | create and edit drafts |
| Reviewer | `editor` | Reviewer / Reviewer | authorize drafts into documents, or send them back |
| Publisher | `publisher` | Publisher / Herausgeber | publish, withdraw, flags, RSS |

The German *Editor* is the `writer` string, not `editor`. Rely on access strings, not on labels.

**Every document lives in one user box.** The box is its owner account. You act on a document only while its box is selected, and your roles are the ones you hold **in that box**. Publication is site-wide: a page published from any box is live for every visitor.

**The interface already applied the role table.** In the document list, a command shown as a link is allowed for you now; a command shown as an image whose name ends in `_disabled` is not. Read the row before acting; do not try a step to find out.

## Your Roles

What one account can do in one box, by the roles it holds there:

| You hold | You can | Someone else must |
|----------|---------|-------------------|
| W | create drafts; edit, apply and discard drafts; editable copies of drafts; delete drafts | authorize (R); every other step on a document or publication |
| R | authorize drafts; remove an authorization; delete documents | edit or apply anything (W); publish (P) |
| P | publish documents; withdraw, republish and delete publications; flags and RSS on any status | edit (W); authorize (R) |
| W+R | everything of W and R; edit a document (it returns to draft); editable copies of documents | publish (P); edit a publication (W+R+P) |
| W+P | everything of W and P | authorize (R); edit a document or publication |
| R+P | everything of R and P; a draft is authorized, then published in a second step | edit (W) |
| W+R+P | every step; publish a draft directly; edit a publication in place | — |

Any role allows read-only copies. No role is needed to forward an original, open *Display* or *Info*, or view versions. The *Function* column of the forwarding form shows which roles each account holds in its own box: that is how you find a Reviewer or Publisher.

## User Boxes

| Box | Visible when | Your roles there |
|-----|--------------|------------------|
| Your own | always, inside the app (`interface.content`) | `interface.content.writer`, `.editor`, `.publisher` |
| Account `<u>` | you hold `interface.content.<u>` and `<u>` holds `interface.content` | `interface.content.<u>.writer`, `.editor`, `.publisher` |
| Any account holding `interface.content` | `interface.content.operator` (or a wildcard above it, such as `*`) | all roles |

- The box is the form field `user` (*User box*). It appears only when you can see a box other than your own.
- **The selection is remembered for your account** across requests and sessions. Post `user` explicitly with every request and check which option is `selected` in the response.
- Selecting a navigation node (`directory_select`) switches to the box that owns the linked page, if you can see it.
- Editing a document in another box through the Frontend needs `content_user=<u>` in the page URL; the list's *Edit* link carries it.
- **Two ways to share work.** Forward documents between the participants' own boxes: each acts in their own box and the document moves. Or give several accounts roles in one box: the document stays and its status change is the handover. Roles in your own box do not count in another box, and forwarding checks only the recipient's roles in its own box.
- `content.writer.<index>` is the exception to boxes and roles: it lets you edit and apply that one page from anywhere, but only while it is a publication. Applying puts your change live at once, without review. Broad grants such as `content.*` or `*` include it for every page, so such an account sees *Edit* on every publication.

## Steps by Status

Originals. `W`, `R`, `P` = Writer, Reviewer, Publisher; `W+R` = one account holding both; *any* = any one of the three; *none* = no role needed.

| Step | Draft | Document | Publication |
|------|-------|----------|-------------|
| Create draft | W | | |
| Edit content or metadata | W | W+R, returns it to draft | W+R+P |
| Apply, discard, undo, save or activate a version | W | W+R, stays a document | W+R+P |
| Authorize and lock → document | R | | |
| Remove authorization → draft | | R | |
| Publish | W+R+P | P | P (links another node) |
| Withdraw → document | | | P |
| Create editable copy | W | W+R | W+R+P |
| Create read-only copy | any | any | any |
| Delete | W | R | P |
| Flags, RSS feed | P | P | P |
| Forward the original, Display, Info, view versions | none | none | none |
| Receive the original | W or R | R or P | P |
| Receive an editable copy | W | W | W |
| Receive a read-only copy | any | any | any |

Consequences:

- **A Reviewer alone cannot edit or even open a draft in the Frontend.** Read it with *Display* and *Info*. To get changes made, send the draft back with a memo, or remove the authorization of a document so a Writer can edit again.
- **Editing a document needs W+R and returns it to draft.** The authorization has to be given again. Applying, discarding, undoing or activating a version leaves it a document: authorization does not freeze the applied text.
- **Changing a live page with split roles is not possible in place.** Editing a publication needs W+R+P. The alternatives: an account holding all three roles; `content.writer.<index>` (skips review); or withdraw → remove authorization → edit → authorize → publish. During the last route the page answers `410`, and withdrawing releases every navigation node linking it; the sweep that follows can remove nodes set to delete automatically, so publishing again must link a node again ([Site structure](<pwnc:SKILLS/Site structure/SKILLS.md>)). Choosing between them is the user's decision.
- **Publishing a draft directly needs all three roles.** It is the shortcut for single-person sites. R+P publishes in two steps: authorize, then publish.

### Copies

| Type (UI) | Created as | Behaviour |
|-----------|------------|-----------|
| Original | | the document itself |
| Editable copy | a draft in the same box, versions included | a Writer edits it; it becomes an original draft on its first edit, until then it cannot be authorized or published; any role deletes it |
| Read-only copy | same status, a publication becomes a document (listed under *Documents*); no versions | never edited, authorized or published; can be copied, forwarded to anyone with a role and deleted with any role |

## Forwarding

*Forward* (`send`) moves documents between boxes. Transferring the original needs no role, only the document in the selected box. Sending a copy needs the right to create that copy (table above).

The form lists every account except the selected box's owner that can receive the original or a copy at the current status, judged by its roles **in its own box**; options it cannot receive are `disabled`. Per recipient:

- **Transfer** (`original=<user>`): the original moves to that box. It disappears from yours; you keep access only if you can see the recipient's box. `original=` (empty, *Keep*) leaves it with you.
- **Editable copy** (`duplicate[i]=<user>`) and **read-only copy** (`copy[i]=<user>`): a copy is created and delivered; you keep the original. `i` is the row number; take the field names from the form.
- `ifc_param1`: memo for the recipient, prefilled with the last dispatch memo.

No recipient qualifies → `There are no authorized recipients.`

Forwarding sends no notification. Tell the recipient yourself ([Agent communication](<pwnc:SKILLS/Agent communication/SKILLS.md>)). The recipient finds the document under the filter *Inbox*, which lists every document in the box that carries a dispatch record, not only new ones; copies keep their source's record. In the recipient list, the box the text was written in is marked *Author*: that is where to send a document back.

## Handover Routes

- **Writer → Reviewer.** Apply. Write the writer memo (at creation, or `_meta` with `ifc_param5`). Forward the original to an account offered for drafts whose *Function* shows *Reviewer*. Tell them.
- **Reviewer → Publisher.** Read with *Display* and *Info*. Authorize with a memo. Forward the document to an account whose *Function* shows *Publisher*.
- **Reviewer → Writer (changes needed).** Forward the draft back to the box marked *Author*, with a memo naming what to change. A document needs its authorization removed first; otherwise the Writer cannot edit it.
- **Publisher.** Read *Display* (it is exactly what goes live) and *Info*, then publish with a memo, or forward back with the reason.
- **Shared box.** No forwarding: change the status, write the step's memo, tell the next person.

## Reading the State

**The list** (select the box, then filter):

- Folder filter `filter`: *All*, *Inbox*, *All drafts*, *My drafts*, *Others' drafts*, *Documents*, *Publications*, each with a count for the selected box. The values are the labels in your interface language; take them from the options. Status filters appear only if your roles there can receive that status. *Documents* includes read-only copies; *Publications* lists originals only. *My drafts* were last written in this box; *Others' drafts* come from another box and nobody has edited them here yet.
- The selected document (`object`) and the pages linked under the selected navigation node are always listed, at the top, whatever the filter. The selection is remembered per box; an empty `object` keeps it, only another document replaces it.
- Two fields name who wrote: the **writer** is the user box the current text was written in (creating and editing in a box record that box, also when you work there for its owner; forwarding and copying keep it). The **author** (column *Author*, hidden under *My drafts*) is the person who last edited. *Sender* (under *Inbox*) is the display name of the last forwarder.
- The title link's `title` attribute reads `<status> <type>`, e.g. `Publication Original`. A title in italics with *(edited)* has buffer changes that are not applied.
- The command links of each row show what you may do now (Mental Model). *Authorize* and *Remove authorization* share one slot. *Schedule* is a link only when schedules exist, *Versions* only when versions exist.

**Display** (`GET interface.php?ifc_page=content&ifc_message=display&ifc_option=external&object=<index>`) renders the applied state at any status. It is how an account without the edit right reads a draft or document.

**Info** (`GET interface.php?ifc_page=content&ifc_message=info&ifc_option=external&object=<index>`) shows status (*in progress* = unapplied changes), type, the applied title, the writer with time and memo, and one section per step taken: dispatch (sender, date, memo), authorization (reviewer, date, memo), publication (publisher, date, memo). A section stays when the status moves back: an authorization section on a draft is history. Read Info before acting on a document you received; the memos are how the participants hand over. Each memo field holds only the last memo of its step.

## Forms

Every POST goes to `interface.php?ifc_page=content` and carries `user=<box>`. A row command is `ifc_message=<step>` with `ifc_param=<index>`; it selects the document as `object`, and the follow-up form acts on `object`, not on `ifc_param`. Responses: `msg_done` or `msg_error` in the system response, in your interface language. An error gives no reason; usually the document is not in the selected box or your roles there do not allow the step at its current status.

| Step | Request | Submit |
|------|---------|--------|
| Create draft | `create` | `_create`: `ifc_param1` title, `ifc_param2` template, `ifc_param3` writer memo; the new draft becomes `object` |
| Metadata, writer memo | `meta` | `_meta`: `ifc_param1` title, `2` summary, `3` keywords, `4` image, `5` writer memo, `6` template; an omitted field stays unchanged; counts as an edit |
| Apply changes | `apply` | `_apply`: `ifc_param1=0` now, or `1` with `ifc_param2` date and time |
| Discard changes | `revert` | acts at once |
| Authorize and lock | `authorize` | `_authorize`: `ifc_param1` memo |
| Remove authorization | `derive_draft` | acts at once |
| Publish | `publish` | publish targets and scheduling: [Site structure](<pwnc:SKILLS/Site structure/SKILLS.md>), section Editorial Route; `ifc_param6` memo |
| Withdraw | `withdraw` | acts at once; releases every navigation node linking the page |
| Editable / read-only copy | `duplicate` / `copy` | acts at once; the copy becomes `object` |
| Forward | `send` | `_send`: fields in Forwarding |
| Delete | `delete` with `list[]` | acts at once; unlinks the page from navigation, removes versions and schedules |
| Save version | `version_store` | acts at once; stores the buffer |
| Versions | `version` | `version_retrieve` with the chosen `version`, then `_version_retrieve`: `ifc_param1=0` into the buffer now, or `1` with `ifc_param2` |
| Schedule | `schedule` | `schedule_save` with `time[<hash>]`; `schedule_delete` with `list[]` of hashes; entries you may not change are `disabled` |
| Flags | `flag` | `_flag`: `ifc_param1` sitemap exclude, `ifc_param2` noindex, `ifc_param3` nofollow; omit unchecked ones |

- **`list[]` is pre-checked** for the selected `object`. Send only the indexes you mean to delete.
- **Memo fields are prefilled** with the last memo of their step. Write the new memo; an untouched field keeps the old text, which then reads as current.
- Discard, withdraw, delete and *Apply all changes* ask for confirmation in the browser. That dialog does not exist for you: the POST is the decision.

## Working in a Role

- **Your role comes from the user's instruction, not from your rights.** If you hold more roles than assigned, take only the assigned steps. Authorizing, publishing and withdrawing are the user's decisions unless delegated to you.
- **Apply before handing over.** Reviewer and Publisher see only the applied state. Authorize, publish and forward do not apply the buffer. Publishing makes the last applied state live and leaves pending changes pending; after authorization only W+R can still apply them.
- **Write a memo at every handover**: what changed, what is open, what the next role should check.
- **Read Display before publishing.** It is exactly what goes live; a document's applied text can have changed after its authorization.
- **Check schedules before you pull a document back.** A scheduled apply, version activation, publication or withdrawal runs at its due time with every role, whatever the status is then. A scheduled version activation writes straight into the applied state, bypassing the buffer. Delete a schedule in *Schedule* when it no longer fits: an apply or version entry needs the right to apply the document now, a publication or withdrawal entry the right to publish it.
- **Apply per document.** *Apply all changes* applies every pending change in the selected box that you are allowed to, without showing you what they are.
- **Do not route around the workflow.** The Webseiten app (`easypage`) creates, publishes and deletes pages regardless of roles ([Site structure](<pwnc:SKILLS/Site structure/SKILLS.md>)), and `content.writer.<index>` changes a live page without review. Use them only when the user asks for that route.

## Troubleshooting

| Symptom | Cause |
|---------|-------|
| Document missing from the list | other box selected, or a filter or search is active |
| Command shown disabled | your roles in this box do not allow it at this status |
| Filter option missing | your roles in this box cannot receive that status |
| `msg_error` on a step | document not in the selected box, status changed meanwhile, or missing role |
| Box selector missing | you can see no box but your own |
| Unexpected box selected | the selection is remembered, or a node selection switched to the page's owner |
| Draft or document answers `410` in the Frontend | no edit right on it in the box `content_user` names (your own when omitted); use *Display* |
| *Edit* offered on a publication your roles do not cover | you hold `content.writer.<index>`, also through `content.*` or `*`; applying goes live at once |
| Recipient not offered, or an option `disabled` | the recipient's roles in its own box do not allow receiving this status or copy type |
| A document stays selected and pre-checked | `object` is remembered per box; select another document |
| Published page shows old content | changes not applied before publishing |
| Document went back to draft | it was edited as W+R while it was a document |
| Document published or withdrawn by itself | a schedule fell due |
| Schedule entry `disabled` | your roles in this box do not allow its step (apply or publish) at the document's current status |

---

[PWNC Website](https://pwnc.it)