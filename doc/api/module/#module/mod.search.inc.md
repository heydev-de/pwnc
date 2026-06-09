# PWNC API Documentation

[← Index](../../README.md) | [`module/#module/mod.search.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23module/mod.search.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Search Module (`mod.search.inc`)

The Search Module provides frontend functionality for searching content within the PWNC Web Platform. It handles user input, query execution, result display, pagination, and tag cloud generation. Additionally, it includes an interface for submitting new URLs to the search index.

---

### Global Variables

| Name | Value/Default | Description |
|------|---------------|-------------|
| `$search_message` | `NULL` | Determines the current action mode (e.g., `"submit"` for URL submission). |
| `$search_term` | `NULL` | The search query string entered by the user. |
| `$search_language` | `NULL` | Language filter for search results. Defaults to the current CMS language or `#any`. |
| `$search_page` | `0` | Current result page number (0-based). |
| `$search_submit_url` | `NULL` | URL submitted via the submission form. |
| `$search_submit_message` | `NULL` | Action command from the submission form (e.g., cancel or submit). |

---

### Module Workflow

1. **Initialization**: The module loads the `search` library and instantiates the `search` class. If the search feature is disabled, it outputs an "unavailable" message and exits.
2. **Action Handling**: Based on `$search_message`, it either:
   - Renders a URL submission form (`"submit"`).
   - Displays the standard search interface with results, pagination, and tag cloud.
3. **Search Execution**: If a search term is provided, it executes the query, displays results, and generates navigation controls.
4. **Tag Cloud**: If no search term is provided, it displays a tag cloud of the most frequent search terms.

---

### Key Functions and Logic

#### `cms_load("search")` and `new search()`

- **Purpose**: Loads the search library and initializes the `search` class.
- **Parameters**: None.
- **Return Values**: `TRUE` if the library is loaded and the search feature is enabled; otherwise, `FALSE`.
- **Inner Mechanisms**: Uses the platform’s library loader to include the search functionality. The `search` class must define an `enabled` property.
- **Usage Context**: Required for all search-related operations. If unavailable, the module exits early.

**Example**:
```php
if (!cms_load("search") || !($search = new search())->enabled) {
    echo(CMS_MSG_UNAVAILABLE);
    return;
}
```

---

#### `$search->find($term, $page, $language)`

- **Purpose**: Executes a search query for the given term, page, and language.
- **Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$term` | `string` | The search query. |
| `$page` | `int` | Page number (0-based). |
| `$language` | `string\|NULL` | Language code or `NULL` for all languages. |

- **Return Values**:
  - `array`: Search results (each entry contains `address`, `title`, `text`, `time`, `score`, `supplemental`).
  - `FALSE`: No results found.
- **Inner Mechanisms**: Queries the database for matching entries, applies pagination, and calculates relevance scores.
- **Usage Context**: Used to retrieve and display search results.

**Example**:
```php
$result = $search->find($search_term, $search_page, $_search_language);
if ($result === FALSE) {
    echo(CMS_L_MOD_SEARCH_005); // "No results found."
} else {
    foreach ($result AS $value) {
        extract($value);
        echo("<li><a href=\"" . x(cms_url($address)) . "\">" . x($title) . "</a></li>");
    }
}
```

---

#### `$search->tag($term, $limit, $language)`

- **Purpose**: Generates a tag cloud of the most frequent search terms.
- **Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$term` | `string\|NULL` | Optional term to filter tags. If `NULL`, returns all tags. |
| `$limit` | `int` | Maximum number of tags to return. |
| `$language` | `string\|NULL` | Language code or `NULL` for all languages. |

- **Return Values**:
  - `array`: Associative array of tags (keys) and their frequencies (values).
  - `FALSE`: No tags found.
- **Inner Mechanisms**: Queries the database for tag frequencies, applies language filtering, and limits results.
- **Usage Context**: Used to display a tag cloud when no search term is provided.

**Example**:
```php
$tag = $search->tag(NULL, 100, $_search_language);
if (is_array($tag)) {
    $score_max = max($tag);
    foreach ($tag AS $value => $score) {
        $size = 15 + round(25 * $score / $score_max);
        echo("<a href=\"" . x(cms_url(["search_term" => $value])) . "\" style=\"font-size:{$size}px\">" . x($value) . "</a> ");
    }
}
```

---

#### `$search->queue_add($url, $type)`

- **Purpose**: Adds a URL to the search index submission queue.
- **Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$url` | `string` | URL to be indexed. |
| `$type` | `int` | Type of submission (e.g., `CMS_SEARCH_QUEUE_TYPE_SUBMISSION`). |

- **Return Values**: None.
- **Inner Mechanisms**: Inserts the URL into the database queue for later processing by the search indexer.
- **Usage Context**: Used in the URL submission form to queue new URLs for indexing.

**Example**:
```php
if ($search_submit_message !== CMS_L_COMMAND_CANCEL) {
    $search->queue_add($search_submit_url, CMS_SEARCH_QUEUE_TYPE_SUBMISSION);
}
```

---

#### `cms_url([...])` and `x($string)`

- **Purpose**:
  - `cms_url()`: Generates a URL with query parameters, merging global and local state.
  - `x()`: Escapes strings for safe HTML output.
- **Parameters**:

| Function | Name | Type | Description |
|----------|------|------|-------------|
| `cms_url` | `$addr` | `string\|array` | Path or associative array of query parameters. |
| `cms_url` | `$param` | `array` | Additional query parameters. |
| `cms_url` | `$omit` | `bool` | If `TRUE`, omits global parameters. |
| `x` | `$string` | `string` | String to escape. |

- **Return Values**:
  - `cms_url()`: `string` (generated URL).
  - `x()`: `string` (escaped HTML).
- **Usage Context**: Used throughout the module to generate safe URLs and escape dynamic content.

**Example**:
```php
echo("<form action=\"" . x(cms_url(["search_message" => "submit"])) . "\" method=\"post\">");
```

---

#### `permission([...])`

- **Purpose**: Displays permission information for the current module.
- **Parameters**:

| Name | Type | Description |
|------|------|-------------|
| `$permissions` | `array` | Associative array of permissions (e.g., `["" => CMS_L_READ]`). |

- **Return Values**: None.
- **Inner Mechanisms**: Outputs a permission summary (e.g., "Read: All, Write: Admins").
- **Usage Context**: Used at the end of the module to display access rights.

**Example**:
```php
permission(["" => CMS_L_READ, CMS_SEARCH_PERMISSION_SUBMIT => CMS_L_WRITE]);
```

---

### Usage Scenarios

#### 1. Basic Search
A user enters a search term and submits the form. The module:
1. Executes the search query.
2. Displays results with titles, excerpts, and relevance scores.
3. Generates pagination links for additional results.

**Example Output**:
```html
<ol start="1">
    <li>
        <a href="/page1"><strong>Page 1</strong></a><br>
        This is an excerpt from the page...<br>
        <small>/page1, 2023-10-01, 0.95</small>
    </li>
</ol>
<nav class="search-pagination">
    <a href="?search_term=example&search_page=1#search">11 - 20</a> |
    <a href="?search_term=example&search_page=2#search">Next</a>
</nav>
```

---

#### 2. Tag Cloud
When no search term is provided, the module displays a tag cloud of the most frequent search terms, sized by frequency.

**Example Output**:
```html
<div class="search-cloud">
    <a href="?search_term=php" style="font-size:2em;opacity:0.8">php</a>
    <a href="?search_term=javascript" style="font-size:1.5em;opacity:0.6">javascript</a>
</div>
```

---

#### 3. URL Submission
Users with `CMS_SEARCH_PERMISSION_SUBMIT` can submit new URLs for indexing via a dedicated form.

**Example Output**:
```html
<form action="/search?search_message=submit" method="post">
    <h2>Submit URL</h2>
    <div class="p">
        <label for="search-submit-url">URL</label><br>
        <input id="search-submit-url" name="search_submit_url" type="text" value="https://">
    </div>
    <div class="p">
        <input name="search_submit_message" type="submit" value="Cancel">
        <input name="search_submit_message" type="submit" value="Submit">
    </div>
</form>
```


<!-- HASH:6fee4cc784fc23c179edb8edf4ac0273 -->
