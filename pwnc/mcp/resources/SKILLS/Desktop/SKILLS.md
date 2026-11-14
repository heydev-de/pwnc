---
name: PWNC Desktop Manual
description: Use this skill whenever you work on a Desktop (your own or another account's), including its objects, files, mailboxes, messenger, calendar, contacts, links and notes, or when you act on behalf of an account holder in assistant mode.
---
# PWNC Desktop Manual

## Concept

Every account with the `desktop` permission has a Desktop at `pwnc/module/desktop.php`: a personal tree of objects plus the messenger. It is the default MCP context: `get`/`post` without `url` lands here.

The Desktop shows one account's desktop, the **desktop user**. Everything done there acts as that user: messenger posts carry its name, mail goes out through its mailbox accounts, sent objects come from its tree. In assistant mode the account is the holder's, so you work in the holder's desktop, mailboxes and messenger.

All actions are ifc form posts (`ifc_message`, `ifc_param`, `ifc_paramN`); generic mechanics, including "send the whole form" and file fields, are in [Forms](<pwnc:SKILLS/Forms/SKILLS.md>). Labels follow the UI language; act on `ifc_message` values and ids, never on visible text.

### Object Model

A desktop is one ordered tree. Containers hold objects; everything else is a leaf. Each object has an 8-character id, a `name`, optional icon position `x`/`y`, a `color` computed from the name at creation, and a `quickaccess` (Favorites) flag.

| Type | Bit | Holds | Opens |
|------|-----|-------|-------|
| `link` | 1 | `url` | Link interface |
| `note` | 2 | `text` | Note interface |
| `appointment` | 4 | `time`, `content`, `place`, `participant` | Calendar |
| `address` | 8 | `company`, `phone`, `fax`, `email`, `address`, `comment` | Contacts |
| `container` | 16 | other objects | itself (navigation) |
| `mailbox` | 32 | one POP3/SMTP account and its mail store | Mailbox |
| `file` | 64 | `extension`; bytes stored outside the tree | download |

### Remembered State

The Desktop keeps three values permanently, across requests and sessions:

| Value | Scope | Set by |
|-------|-------|--------|
| `user` (desktop user) | per **accessor** account | `user` field or query parameter |
| `parent` (open container, empty = top level) | per **desktop**, shared by everyone using it | `activate`, `move` |
| `object` (selected object) | per **desktop**, shared | `select`, `activate`, `create`, `upload`, `move`, `drop`, `send`; `delete` of the selected object clears it |

Consequences:
- A later `get` without `url` returns to the same desktop and container. Switch back when done.
- On another account's desktop you move the owner's open folder and selection too. In assistant mode the holder shares even `user` with you.
- `rename` and `quickaccess` act on the remembered selection, which someone else may have changed. `select` right before them.

## Reading the View

- Only the direct children of the open container are rendered: `<e class="icon" id="dd-{id}">` with the name; the selected one is in `<strong>`.
- The script block registers each icon as `dd_register("dd-{id}", {type bit}, {accepted bits})`, the reliable way to read an object's type.
- `desktop-path` is the breadcrumb, and inside a container an extra icon `dd-{parent id}` (`dd-` for top level) leads up.
- `desktop-link` (Favorites) lists every `quickaccess` object of the whole tree with `desktop_activate('{id}',{bit})`: a quick way to find ids outside the open container.
- The sidebar lists up to three appointments from one hour ago onward.

**Opening an object.** `activate` on a leaf selects it, and the response ends with `load_page('…', '…desktop.php?desktop_display=interface&user={user}&object={id}')`. `get` that URL to work in the object's interface. Interfaces without an object:

```
desktop.php?desktop_display=interface&desktop_interface=ims&user={user}
desktop.php?desktop_display=interface&desktop_interface=appointment&user={user}
desktop.php?desktop_display=interface&desktop_interface=address&user={user}
```

Interface forms post to their own action URL and carry `desktop_interface`, `user` and `object` as hidden fields.

## Desktop Actions

Post to `desktop.php`:

| `ifc_message` | `ifc_param` | Effect |
|---------------|-------------|--------|
| `activate` | container id, or empty | Open that container; empty opens the top level |
| `activate` | leaf id | Select and open it (a file only gets a browser download script) |
| `select` | id | Select without opening |
| `create` | `{type};{name}` | Insert as first item of the open container and select it; a non-container is also opened. The name ends at the first `;` |
| `rename` | new name | Rename the **selected** object |
| `quickaccess` | — | Toggle Favorites for the **selected** object |
| `move` | `{id},{container id}` | Insert as first child of that container, or as first item of the top level if the target is empty. Clears `x`/`y`, keeps the id, and **opens the target** as `parent`. An unknown id, the object itself or a target inside its own subtree fails with an error and changes nothing |
| `drop` | `{id},{x},{y}` | Set the icon position in pixels; rendering snaps it to a grid and shifts collisions |
| `delete` | id | Delete the object with its whole subtree, files and mail stores |
| `send` | id | Recipient form, see [Sending Objects](#sending-objects) |
| `upload` | `{x},{y}` or `,` | See [Files](#files) |
| `config` | — | Account settings, see [Settings](#settings) |

`create` types: `link`, `note`, `appointment`, `address`, `container`, `mailbox`. Files come only from `upload`.

**Destructive edges:**
- `delete` is immediate and permanent: no trash, no confirmation.
- Deleting the open container (or one of its ancestors) leaves `parent` pointing at nothing: the view shows only a "Home" icon, and `create` and `upload` fail with an error. `activate` with empty `ifc_param`.

### System Containers

Three containers carry an internal `#subtype` and are recreated whenever one is missing:

| `#subtype` | Purpose |
|------------|---------|
| `mailbox` | Desktop In-box: objects other users send you land here |
| `appointment` | Where the calendar's `add` creates appointments |
| `address` | Where the contact book's `add` creates contacts |

They are created with localized names and in Favorites. `#subtype` is not rendered; recognize them by name and Favorites entry. Deleting one deletes its contents permanently; the replacement is a new empty container (new id, top of the top level). The desktop In-box is unrelated to a mailbox's `#inbox` folder.

## Files

Upload with the files in `ifc_file1` **as an array**, even for one file (the field is `ifc_file1[]`; a single object fails):

```
{"ifc_message": "upload", "ifc_param": ",",
 "ifc_file1": [{"filename": "report.pdf", "type": "application/pdf", "data": "<base64>"}]}
```

- Each file becomes a `file` object named after its full filename (extension included), inserted first in the open container, so the last file of a request ends on top; the first upload is selected.
- Several files are all-or-nothing: any failure removes every object and file of the request.
- Bytes live at `data/#desktop/<user>/file/<id>.<extension>`; the tree keeps only `extension`, taken lowercased from the upload name and never changed afterwards.

Download with `get` (the content comes back directly):

```
desktop.php?desktop_display=download&user={user}&object={id}
```

The download name is the object name without its last extension, slugged (`Report Ä 1.pdf` → `report-ae-1.pdf`), plus the stored extension. Renaming the object renames the download, not the type. Unknown id or missing bytes: 404.

## Sending Objects

`send` with an object id shows a multiselect of recipients (`ifc_param1`, an array of usernames; [Forms](<pwnc:SKILLS/Forms/SKILLS.md>), section Sending Fields); submitting posts `_send` with the hidden `object`.

- Recipients: every account holding `desktop`, except the desktop user. Posted names outside that list are dropped.
- Each recipient gets a full copy (subtree, file bytes, mail stores, every property including a mailbox's server credentials) inside a new container `{date time} {your name}`, inserted first in their desktop In-box. Copies get new ids.
- All-or-nothing across recipients.
- Recipients are **not** notified. Tell them through the [Messenger](#messenger) if they need to know.

## Other Desktops

| Permission | Allows |
|------------|--------|
| `desktop` | Own desktop; makes the account a recipient for objects and messages |
| `desktop.{user}` | Viewing and operating that user's desktop |

The user selector in `desktop-user` lists accounts holding `desktop` for which you hold `desktop.{user}`. Switch with the `user` field (or `?user={username}`). Without the permission the request silently falls back to your own desktop; check the selected option or the hidden `user` field.

A viewed desktop is fully operable: every action, interface, messenger post and mail acts as its owner, and changes the owner's remembered folder and selection. Only read there unless the task requires acting as the owner. Granting access: [Permissions](<pwnc:SKILLS/Permissions/SKILLS.md>).

## Settings

`config` shows the **accessor's own** account settings (password, email, timezone, interface language) plus the background image of the **displayed** desktop. `_config` writes email, timezone and language from the post, so an omitted field is cleared. Change the password only on explicit instruction; it requires the current one.

## Mailbox

A `mailbox` object is one mail account. Its interface needs the server's POP3, SMTP and MIME libraries; it is inactive without them.

**Account.** `configure` / `_configure`: `ifc_param1` … `ifc_param9` = sender name, address, account name, password, POP3 host, SMTP host, signature, spam threshold (`95`/`90`/`85`/`80`, `0` = off), delete after receipt. `_configure` overwrites all nine: an omitted password is erased.

### Folders

The current folder is the `container` field; post it with an empty `ifc_message` to switch.

| Folder | Content |
|--------|---------|
| `#inbox` | Retrieved mail |
| `#draft` | Drafts; every compose creates one |
| `#outbox` | Sent mail |
| `#trashbin` | Deleted mail, until emptied |
| `#spam` | Mail at or above the spam threshold, or trained as spam |

Custom folders: `create_container` (`ifc_param` = name, then opens it), `rename_container` (renames the current folder), `delete_container` (moves its messages to `#trashbin`, then removes it). System folders cannot be renamed or deleted. `order`: `1` date, `2` sender, `3` subject, `5` spam probability, `6` size.

### Reading

The list shows one checkbox `list[]` per message (value = message id). Subjects call `d('{id}')` (`display`) for received and sent mail, `c('{id}')` (`compose_edit`) for drafts.

- `display` with `ifc_param` = id and the right `container` shows the headers and every part, with text parts inline as plain text (HTML converted), and marks the message read.
- Each part links to `…ifc_message=_display&ifc_param={id}&…&container={folder}&part={n}`. `get` it for the raw part: an attachment's bytes, or an HTML body with `cid:` images rewritten to part URLs.
- Addresses link to `a('{group}','{name}','{email}')`, which opens [Contacts](#contacts) with `ifc_message=address`.

### Composing and Sending

| `ifc_message` | `ifc_param` | Creates a draft with |
|---------------|-------------|----------------------|
| `compose` | — | signature |
| `mail` | — (`to` = address) | recipient prefilled; used by the contact book |
| `compose_reply` | message id | `to` = first Reply-To or From address, `Re:` subject, signature, then the original's first text part (unquoted) |
| `compose_relay` | message id | `Fw:` subject, original attached as `message/rfc822` |
| `compose_edit` | draft id (`container` = `#draft`) | the stored draft |

The draft exists in `#draft` as soon as the form appears; the form carries hidden `message` (draft id) and `container`. Fields:

| Field | Content |
|-------|---------|
| `to`, `cc`, `bcc` | Comma-separated addresses |
| `ifc_param1` | Subject |
| `ifc_param2` | Plain-text body, not PWNC text format |
| `ifc_file1` | One attachment to add, a single file object (not an array) |
| `remove[{part}]` | Remove that attachment |

Submit with `_compose` (store and stay, e.g. after adding an attachment), `compose_save` (store, back to the list) or `compose_send`. Each of them first writes every field from the post into the draft: an omitted `to` or body is emptied. A forward cannot change attachments.

`compose_send` sends via the account's SMTP host and moves the message to `#outbox`. On failure the draft stays and the form returns with the server response. **Sending mail is irreversible; send only when the task asks for it.** Abandoned drafts stay in `#draft` until moved to the trash.

### Retrieving, Moving, Deleting

- `retrieve` fetches new POP3 messages into `#inbox` (or `#spam`) and reports counts. Messages already in the index are skipped by their server id. With "delete after receipt" each stored message is deleted on the server. Nothing retrieves automatically.
- `move` with `list` (array of message ids) and `target` moves messages between folders. Deleting a message = moving it to `#trashbin`.
- `empty_trashbin` deletes every message in `#trashbin` locally and deletes the matching ones on the POP3 server. A message whose server deletion fails is kept. If a POP3 host is configured but unreachable, nothing is deleted; without a POP3 host, deletion is local only.

### Spam Filter

Active only when the server's spam classifier is available. Every retrieved message gets a spam probability; one not classified as spam is trained as normal automatically. `train_bad` / `train_good` with a message id undo the opposite training, retrain, and move the message to `#spam` / `#inbox`.
## Messenger

The messenger exchanges short texts between accounts. Each message is stored once per owner (receiver copy, plus a "sent" copy if the sender has `desktop`), so deleting your copy leaves the other side untouched.

| `ifc_message` | `ifc_param` | Effect |
|---------------|-------------|--------|
| `send` | — | New-message form, new thread |
| `reply` | message id | Reply form in the same thread, sender preselected |
| `_send` / `_reply` | as in the form | Submit: recipients `ifc_param1` (array), text `ifc_param2`; the hidden `thread` decides the thread |
| `filter_user` | username | Correspondence with one account |
| `filter_thread` | thread id | One thread |
| `filter_reset` | — | Clear both filters |
| `delete` | — | Delete your copies checked in `list` (array of message hashes; one sent row covers all its recipients) |

- Text over 1500 characters is cut silently.
- Recipient list: every account holding `desktop`, except the desktop user. A `reply` link appears only when the sender is in that list; system senders such as `trigger` cannot be answered.
- The list shows the 50 newest messages; older ones load with `ifc_message=load&before={data-ims-key of the last row}`.

**Delivery and wake-up.** A message sets the receiver's unread flag; opening the messenger clears the desktop user's flag. Then:
- Receiver with web activity in the last 15 minutes: nothing more.
- Agent receiver: woken with the text. Every message wakes it, so do not message agents casually. Assistant mode: [Agent communication](<pwnc:SKILLS/Agent communication/SKILLS.md>), section Assistant Mode.
- Human receiver: one email per unread period, if an address is set.

A reply in a thread that came from a `call` neither wakes nor emails the caller; it is relayed as the call's reply, see [Agent communication](<pwnc:SKILLS/Agent communication/SKILLS.md>).

## Calendar

Shows years, months and days, and one day from 06:00 to 05:59 in 15-minute slots; counts in brackets mark days with appointments. It lists every appointment in the tree, not only those in its system container.

| `ifc_message` | `ifc_param` | Effect |
|---------------|-------------|--------|
| `time` | Unix timestamp | Show that day |
| `select` | appointment id | Show and edit it |
| `add` | Unix timestamp of the slot | Create an appointment there, in the system container |
| `save` | — | Store the edit fields of the selected appointment |
| `delete` | — | Delete the selected appointment |

Edit fields: `ifc_param1` subject, `ifc_param2` date and time (`YYYY-MM-DDTHH:MM`, accessor's timezone; unparsable keeps the old time), `ifc_param3` remove after expiry, `ifc_param4` contents, `ifc_param5` location, `ifc_param6` participants. `save` writes all six.

- With "remove after expiry" the appointment disappears one day after its time. `add` sets it; `save` without `ifc_param3` clears it.
- An appointment from the desktop's `create` has no time until saved; meanwhile the calendar shows it at the current time, and the sidebar ignores it.

## Contacts

Groups contacts by the first letter of organization + name (`initial`: `A`…`Z`, `-` for anything else), then by organization. Lists every contact in the tree.

| `ifc_message` | `ifc_param` | Effect |
|---------------|-------------|--------|
| `initial` | letter or `-` | Show that letter |
| `select` | contact id | Show and edit it |
| `add` | organization, or empty | Create a contact in the system container: with an organization it has no name, otherwise it is named after the shown letter (none for `-`) |
| `save` | — | Store `ifc_param1` … `ifc_param7`: name, organization, phone, fax, email, postal address, memo (all seven are written) |
| `delete` | — | Delete the selected contact |
| `address` | — | Select the contact with `email`, or create it from `name` (default: the email), `email` and `group` (organization) |

With a mailbox on the desktop, each email address links to `m('{email}')`: that mailbox with `ifc_message=mail&to={email}`, i.e. a new draft. With several mailboxes the last one in tree order is used. The compose form offers all contact addresses as suggestions.

## Links and Notes

**Note.** One plain-text field `ifc_param1`; `save` stores it.

**Link.** The URL is the text field `ifc_param1`; `save` stores it, `reload` discards an unsaved change. The interface frames the target for humans; to read the target, `get` the URL itself.

## Troubleshooting

| Symptom | Cause | Fix |
|---------|-------|-----|
| `get` without `url` shows another desktop or a subfolder | `user` and `parent` persist | Post `user` = your username; `activate` with empty `ifc_param` |
| Switching `user` shows your own desktop | No `desktop.{user}` | Request it, or stay on your own desktop |
| `rename`/`quickaccess` hit the wrong object | They act on the shared selection | `select` first |
| Only a "Home" icon visible; `create`/`upload` fail | Open container was deleted | `activate` with empty `ifc_param` |
| `upload` fails for one file | `ifc_file1` sent as one object | Send an array |
| Appointment gone a day after its time | "Remove after expiry" was set | Save without `ifc_param3` |
| Mailbox password or draft fields empty | Partial `_configure` / compose post | Send the complete form |
| No `reply` link on a messenger message | Sender is not a possible recipient | Answer through another channel |
| Mail error after `compose_send` | SMTP host or credentials | Check `configure`; the draft is kept |
| `empty_trashbin` errors, trash unchanged | POP3 server unreachable | Check `configure`, retry |

---

[PWNC Website](https://pwnc.it)