---
name: PWNC Media Manual
description: Use this skill whenever you upload, find, replace, rename, categorise or delete images, media files or downloads, or reference them from content and templates.
---
# PWNC Media Manual

## Concept

Three Backend apps manage the site's assets with one model. An asset is a record (multilingual name, category, and either a stored file or an external URL) keyed by an id such as `image://<uid>`. The id is the whole string, scheme included: it is the record key, the app's `object` value and what content stores.

Content and templates store only the id. It is resolved to a URL on every render. Edit, rename and replace keep the id, so id references survive them. Direct file URLs (`data/image/<file>`) do not: a filename edit or an extension-changing replace moves the file. Always reference by id.

| App | `ifc_page` | Holds | Files in | Web access to the file |
|-----|-----------|-------|----------|------------------------|
| Image | `image` | GIF, JPG/JPEG, PNG, SVG, WEBP, or an external URL | `data/image/` | public, no rights checked |
| Media | `media` | any file, or an external URL | `data/media/` | public, no rights checked |
| Download | `download` | any file, plus a description | `data/#download/` | blocked; only through `download.php` |

Opening an app needs `interface.<app>`, every change `interface.<app>.operator`. Without the operator right the command menu is missing (see [Permissions](../Permissions/SKILLS.md)). Use Download for files that need access control or logging.

---

## Finding an Id

- The category `<select>` has no name. Each option's value is the first id in that category, so `ifc_message=select`, `ifc_param=<option value>` lists that category. The option with the empty label holds the uncategorised assets.
- Sending `object=<id>` opens the app on that asset's category.
- The list is the checkbox group `_object[]`: value = id, label = name in the interface language. Labels are not unique (duplicates get ` (1)`, ` (2)`); use the values.
- `ifc_message=display&object=<id>` shows one asset: image size in px and MIME type; media size and MIME type; download name, format, size and description.

---

## Operations

Commands act on `object`; `delete` acts on every checked `_object[]` (send `_object` as an array). Each message except `delete` opens a form that posts the `_`-prefixed variant. Form mechanics and file uploads: [Forms](../Forms/SKILLS.md).

| `ifc_message` | Effect |
|---------------|--------|
| `upload` | One file with name, category; filename (image, media), format (media), description (download) |
| `upload_multi` | Several files into one category (and one format for media); names from the file names |
| `add` | Register an external URL (image, media) |
| `edit` | Name, category; filename for file assets, URL for URL assets; format (media); description (download) |
| `replace` | New file under the same id; file assets only |
| `delete` | Removes record and stored file |
| `category_rename` | Renames the selected asset's category on every asset in it |

**Result.** After `_upload` and `_add` the response's `object` holds the new id (bulk upload: the last stored file). A failed file in a bulk upload makes the response `Error`, but the other files are stored. Failures answer a bare `Error`: wrong image extension, no file, missing operator right, `_replace` on a URL asset (`replace` on one answers that the source is external). `add` and URL edits store the URL unchecked, nothing is fetched.

**Names.** The name is multilingual and is used on the site: default alt text of `<CMS:image>`, `<CMS:thumbnail>` and `<CMS:media>`, and, for downloads, the file name a visitor receives (slug of the name plus the stored extension). Set it per language where it matters. An empty name falls back to the filename field, then to the uploaded file name without extension; for `add`, to the URL's file name. On `edit`, an empty name keeps the old one.

**Filenames.** Slugged (`Logo Ü 2` → `logo-ue-2`) and made unique with a suffix; default is the name. A filename edit renames the file and keeps the extension. Downloads always get a random storage name and have no filename field.

**Image formats.** Upload and replace check the extension of the uploaded file name only: `gif`, `jpg`, `jpeg` (stored as `jpg`), `png`, `svg`, `webp`. The content is not checked; a mislabelled file is stored and then shown unprocessed.

**Replace.** Same extension: the file is overwritten, its URL stays. Different extension: a new file `<stem>.<ext>` replaces the old one, the file URL changes. A URL asset cannot be replaced or turned into a file asset; add a new asset and update its references.

**Categories.** Free text with suggestions, compared exactly (case-sensitive). A new name creates a category; it disappears with its last asset. Upload forms preset the selected asset's category. `category_rename` to an existing name merges the two; to an empty name, the assets become uncategorised.

---

## References

| Id | Resolves to | Unknown id |
|----|-------------|------------|
| `image://<uid>` | `data/image/<file>` or the external URL | `pwnc/image/no_image.svg` placeholder |
| `media://<uid>` | `data/media/<file>` or the external URL | the id itself, unresolved |
| `download://<uid>` | `pwnc/module/download.php?download_index=<id>&download_start=1` | the id itself, unresolved |

Template elements (`<CMS:image/>`, `<CMS:thumbnail/>`, `<CMS:media/>`, `<CMS:download>`) take ids as values, see [Template manual](../Template%20manual/SKILLS.md); writing them into pages: [Content editing](../Content%20editing/SKILLS.md). In the text format any id works as a link target: `[download://<id> label]`.

**Nothing records where an id is used.** Deleting leaves every reference in place: a dead image shows the placeholder, a dead media or download id breaks the output.

**Asset changes do not reach cached pages.** After an edit, rename, replace or delete, cached renders keep the old URLs, image variants, alt text and embed code; renders by accessors with update rights on the page are fresh. A deleted image keeps showing on cached pages because its variants still exist. Clear the caches when a change must show ([Diagnostics](../Diagnostics/SKILLS.md#caches)).

---

## Images: Variants

- The original stays as uploaded, publicly reachable at its file URL.
- On render, templates request resized copies at standard widths, limited by the requested size (×2 for high-density screens), the source width (never upscaled) and the configured maximum resolution. Output format is the preferred one (WebP or JPEG); transparent images become WebP, or PNG without WebP support.
- Variants live in `data/image/cache/` and are rebuilt when the source is newer, on the next uncached render.
- Passed through unprocessed: GIF, SVG, animated images, files that cannot be decoded.
- External images are fetched and processed only for `http`, `https` or `ftp` URLs whose path ends in an image extension (`gif`, `jpg`, `jpeg`, `png`, `svg`, `webp`); `<CMS:image>` and `<CMS:thumbnail>` show the placeholder for any other URL. The copy is rechecked at most every 10 minutes; a 401, 403, 404 or 410 drops it and the placeholder appears.
- With background processing on, the first render serves blurred previews scaled up from a tiny copy; a daemon task writes the full variants later. Blurred images right after an upload are expected.
- Delete and replace never remove variants. They stay public in the cache until `clear_cache`. To remove an image completely, delete it and clear the cache.

Operator extras: `config` (preferred format, maximum resolution, background processing) and `clear_cache`, which empties the image variants and the page cache; everything is regenerated on demand afterwards.

---

## Media: Formats

A media asset is embedded through a **media format**: name, assigned file extensions (comma-separated, case-insensitive) and a multilingual HTML embed code. The asset stores a format id or *Automatic*.

- *Automatic*, or a format with empty code: the format is chosen by the extension of the file or URL. When several formats claim an extension, the first by name wins. No match: a generic `<object>` element.
- Placeholders: `%id%` (template `_id`, else a random `media_<uid>`), `%src%` (resolved URL), `%type%` (MIME type from the extension), `%width%`, `%height%`, `%alt%`, `%title%` (both from the template's `alt` or the asset name), `%class%`, `%style%`. Values are HTML-escaped.
- A bracketed segment such as `[ width="%width%"]` loses its brackets when the placeholder has a value and is dropped when it is empty. Brackets around a placeholder are always consumed this way, so the code cannot contain them literally.
- Managed under `type` (operator): `type_select` with `ifc_param=<format id>`, `type_add`, `type_set` (name, extensions and code of `type_object`), `type_delete` on the checked `list` (array). The selected format arrives pre-checked.
- A code change affects every asset using that format.

---

## Downloads: Delivery

- `download.php?download_index=<id>` without `download_start` shows an info page (name, format, size, description) that starts the download after 2.5 seconds. References add `download_start=1` and deliver directly.
- Delivery requires the `download` right; grant it to anonymous for public files. Without it the request redirects to the login page.
- The description is multilingual PWNC text format (see README).
- Each delivery through `download.php` is written to the access log as `downloaded` with the asset name. There is no per-asset counter.

---

[PWNC Website](https://pwnc.it)