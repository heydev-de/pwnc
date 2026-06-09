# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.search.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.search.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Search Class

The `search` class provides a comprehensive search engine for the PWNC Web Platform. It handles indexing, searching, and managing web content, including URL processing, text analysis, and relevance scoring. The class supports multilingual content, link analysis, and periodic updates to maintain search accuracy.

---

### Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| **Permission** | | |
| `CMS_SEARCH_PERMISSION_SUBMIT` | `"submit"` | Permission required to submit URLs for indexing. |
| **Database Tables** | | |
| `CMS_DB_SEARCH_ENTRY` | `CMS_DB_PREFIX . "search_entry"` | Table storing indexed entries. |
| `CMS_DB_SEARCH_ENTRY_INDEX` | `"id"` | Primary key for entries. |
| `CMS_DB_SEARCH_ENTRY_ADDRESS` | `"address"` | URL of the indexed entry. |
| `CMS_DB_SEARCH_ENTRY_ADDRESS_HASH` | `"address_hash"` | 32-bit hash of the URL for indexing. |
| `CMS_DB_SEARCH_ENTRY_TITLE` | `"title"` | Title of the indexed entry. |
| `CMS_DB_SEARCH_ENTRY_TEXT` | `"text"` | Extracted text content of the entry. |
| `CMS_DB_SEARCH_ENTRY_TEXT_HASH` | `"text_hash"` | 64-bit simhash for content similarity detection. |
| `CMS_DB_SEARCH_ENTRY_TIME` | `"time"` | Timestamp of the last update. |
| `CMS_DB_SEARCH_ENTRY_UPDATE_INTERVAL` | `"update_interval"` | Time interval before the next update. |
| `CMS_DB_SEARCH_ENTRY_UPDATE_TIME` | `"update_time"` | Timestamp for the next scheduled update. |
| `CMS_DB_SEARCH_ENTRY_SCORE` | `"score"` | Relevance score of the entry. |
| `CMS_DB_SEARCH_ENTRY_LINK_COUNT` | `"link_count"` | Number of outbound links. |
| `CMS_DB_SEARCH_ENTRY_ERROR` | `"error"` | Error count for failed scans. |
| `CMS_DB_SEARCH_ENTRY_CANONICAL` | `"canonical"` | Flag indicating if the entry is the canonical version of similar content. |
| `CMS_DB_SEARCH_WORD` | `CMS_DB_PREFIX . "search_word"` | Table storing indexed words. |
| `CMS_DB_SEARCH_WORD_INDEX` | `"id"` | Primary key for words. |
| `CMS_DB_SEARCH_WORD_TEXT` | `"text"` | The indexed word. |
| `CMS_DB_SEARCH_WORD_LANGUAGE` | `"language"` | Language of the word. |
| `CMS_DB_SEARCH_WEIGHT` | `CMS_DB_PREFIX . "search_weight"` | Table storing word weights for entries. |
| `CMS_DB_SEARCH_WEIGHT_WORD` | `"word"` | Foreign key to `CMS_DB_SEARCH_WORD`. |
| `CMS_DB_SEARCH_WEIGHT_SOURCE` | `"source"` | Foreign key to the source entry. |
| `CMS_DB_SEARCH_WEIGHT_TARGET` | `"target"` | Foreign key to the target entry. |
| `CMS_DB_SEARCH_WEIGHT_VALUE` | `"value"` | Weight of the word for the entry. |
| `CMS_DB_SEARCH_LINK` | `CMS_DB_PREFIX . "search_link"` | Table storing links between entries. |
| `CMS_DB_SEARCH_LINK_SOURCE` | `"source"` | Foreign key to the source entry. |
| `CMS_DB_SEARCH_LINK_TARGET_HASH` | `"target_hash"` | Hash of the target URL. |
| `CMS_DB_SEARCH_LINK_TARGET` | `"target"` | Foreign key to the target entry. |
| `CMS_DB_SEARCH_LINK_TEXT` | `"text"` | Anchor text of the link. |
| `CMS_DB_SEARCH_LINK_LEVEL` | `"level"` | Level of the link in the link graph. |
| `CMS_DB_SEARCH_CLUSTER` | `CMS_DB_PREFIX . "search_cluster"` | Table storing clusters of similar entries. |
| `CMS_DB_SEARCH_CLUSTER_SOURCE` | `"source"` | Foreign key to the source entry. |
| `CMS_DB_SEARCH_CLUSTER_TARGET` | `"target"` | Foreign key to the target entry. |
| `CMS_DB_SEARCH_QUEUE` | `CMS_DB_PREFIX . "search_queue"` | Table storing URLs to be processed. |
| `CMS_DB_SEARCH_QUEUE_ADDRESS` | `"address"` | URL to be processed. |
| `CMS_DB_SEARCH_QUEUE_TYPE` | `"type"` | Type of queue entry. |
| `CMS_DB_SEARCH_QUEUE_TIME` | `"time"` | Timestamp for processing. |
| `CMS_DB_SEARCH_QUEUE_CODE` | `"code"` | Unique code for processing lock. |
| `CMS_DB_SEARCH_QUEUE_ERROR` | `"error"` | Error count for failed processing. |
| `CMS_DB_SEARCH_QUEUE_DONE` | `"done"` | Flag indicating if processing is complete. |
| **Scan Result Codes** | | |
| `CMS_SEARCH_SCAN_UNKNOWN_ERROR` | `1` | Unknown error during scanning. |
| `CMS_SEARCH_SCAN_REDIRECTION_LIMIT_EXCEEDED` | `2` | Redirection limit exceeded. |
| `CMS_SEARCH_SCAN_INVALID_ADDRESS` | `4` | Invalid URL. |
| `CMS_SEARCH_SCAN_ADDRESS_REJECTED` | `8` | URL rejected by blacklist/whitelist. |
| `CMS_SEARCH_SCAN_NO_CONNECTION` | `16` | Connection to the URL failed. |
| `CMS_SEARCH_SCAN_UNSUPPORTED_RESOURCE_FORMAT` | `32` | Unsupported content type. |
| `CMS_SEARCH_SCAN_NO_MODIFICATION` | `64` | Content not modified since last scan. |
| `CMS_SEARCH_SCAN_DATA_FETCH_FAILED` | `128` | Failed to fetch content. |
| `CMS_SEARCH_SCAN_NO_CONTENT` | `256` | No content found. |
| `CMS_SEARCH_SCAN_INDEXING_UNDESIRED` | `512` | Indexing blocked by `noindex` directive. |
| `CMS_SEARCH_SCAN_INDEXED` | `1024` | Content successfully indexed. |
| `CMS_SEARCH_SCAN_INDEXING_FAILED` | `2048` | Indexing failed. |
| `CMS_SEARCH_SCAN_DATABASE_ERROR` | `4096` | Database error during indexing. |
| `CMS_SEARCH_SCAN_FATAL_ERROR` | Combination of fatal errors | Fatal errors preventing indexing. |
| `CMS_SEARCH_SCAN_ERROR` | Combination of all errors | All errors during scanning. |
| **Queue Types** | | |
| `CMS_SEARCH_QUEUE_TYPE_NONE` | `0` | No specific type. |
| `CMS_SEARCH_QUEUE_TYPE_INTERNAL` | `1` | Internal URL. |
| `CMS_SEARCH_QUEUE_TYPE_SELECTION` | `2` | Selected URL. |
| `CMS_SEARCH_QUEUE_TYPE_SUBMISSION` | `4` | Submitted URL. |
| `CMS_SEARCH_QUEUE_TYPE_REFERENCE` | `8` | Discovered link. |
| `CMS_SEARCH_QUEUE_TYPE_UPDATE` | `16` | Scheduled update. |
| `CMS_SEARCH_QUEUE_TYPE_ALL` | `255` | All queue types. |

---

### Properties

| Name | Default | Description |
|------|---------|-------------|
| `enabled` | `FALSE` | Flag indicating if the search engine is enabled. |
| `address_accepted_blacklist` | `[]` | List of regex patterns for URL exclusion. |
| `address_accepted_whitelist` | `[]` | List of regex patterns for URL inclusion (applied after blacklist). |
| `scan_routing_limit` | `5` | Maximum number of permitted redirects. |
| `update_interval` | `86400` (1 day) | Initial requeue interval in seconds. |
| `update_interval_min` | `3600` (1 hour) | Minimum requeue interval in seconds. |
| `update_interval_max` | `604800` (1 week) | Maximum requeue interval in seconds. |
| `update_interval_decrease_factor` | `0.5` | Factor to decrease the requeue interval. |
| `update_interval_increase_factor` | `1.5` | Factor to increase the requeue interval. |
| `entry_set_maximum_bit_difference` | `2` | Maximum bit difference for simhash similarity. |
| `entry_set_weight_factor_title` | `0.1` | Weight factor for titles. |
| `entry_set_weight_factor_h1` | `1.3` | Weight factor for `<h1>` headings. |
| `entry_set_weight_factor_h2` | `1.2` | Weight factor for `<h2>` headings. |
| `entry_set_weight_factor_h3` | `1.1` | Weight factor for `<h3>` headings. |
| `entry_set_weight_factor_copy` | `1.0` | Weight factor for body text. |
| `entry_set_weight_factor_side` | `0.1` | Weight factor for side content. |
| `entry_set_weight_factor_address` | `0.1` | Weight factor for URL components. |
| `entry_set_weight_factor_link` | `0.1` | Weight factor for link text. |
| `entry_remove_error_limit` | `3` | Number of failed scans before permanent removal. |
| `queue_process_retry_time` | `3600` (1 hour) | Time to retry failed scans. |
| `queue_process_error_limit` | `3` | Maximum retries for failed scans. |
| `find_results_per_page` | `10` | Number of results per page. |
| `score_compute_iteration_number` | `10` | Number of score refinement iterations. |
| `score_compute_dampening_factor` | `0.85` | Portion of inherited score in relevance calculation. |

---

### Constructor

#### `__construct()`

**Purpose:**
Initializes the search engine, verifies database tables, and loads configuration settings.

**Parameters:**
None.

**Return Values:**
None.

**Inner Mechanisms:**
1. Establishes a database connection.
2. Verifies the existence and structure of all required database tables.
3. Loads blacklist/whitelist patterns and other settings from the system configuration.
4. Sets `enabled` to `TRUE` if initialization succeeds.

**Usage Context:**
Called automatically when the `search` class is instantiated. Ensures the search engine is ready for use.

**Example:**
```php
$search = new \cms\search();
if ($search->enabled) {
    // Search engine is ready for use
}
```

---

### Methods

#### `scan($address, $follow_links = FALSE)`

**Purpose:**
Scans a URL, checks for updates, and indexes its content if modified.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$address` | `string` | URL to scan. |
| `$follow_links` | `bool` | If `TRUE`, discovered links are added to the queue. |

**Return Values:**
- `int`: One of the `CMS_SEARCH_SCAN_*` constants indicating the scan result.

**Inner Mechanisms:**
1. Standardizes the URL and checks if it is accepted by the blacklist/whitelist.
2. Checks if the URL is already indexed and if it has been modified since the last scan.
3. Delegates scanning to `_scan()` if the URL is new or modified.
4. Removes the entry from the index if a fatal error occurs.

**Usage Context:**
Used to manually trigger a scan of a URL. Typically called by `queue_process()` for scheduled updates.

**Example:**
```php
$search = new \cms\search();
$result = $search->scan("https://example.com", TRUE);
if ($result === CMS_SEARCH_SCAN_INDEXED) {
    echo "URL indexed successfully.";
}
```

---

#### `_scan($address, $follow_links, $entry_time = 0)`

**Purpose:**
Internal method to scan a URL, fetch its content, and index it.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$address` | `string` | URL to scan. |
| `$follow_links` | `bool` | If `TRUE`, discovered links are added to the queue. |
| `$entry_time` | `int` | Timestamp of the last scan (for modification checks). |

**Return Values:**
- `int`: One of the `CMS_SEARCH_SCAN_*` constants indicating the scan result.

**Inner Mechanisms:**
1. Follows redirects up to `scan_routing_limit`.
2. Checks for `noindex` directives in HTTP headers or HTML meta tags.
3. Verifies the content type is supported (e.g., `text/html`).
4. Fetches and parses the HTML content using the `html` library.
5. Adds discovered links to the queue if `$follow_links` is `TRUE`.
6. Indexes the content using `entry_set()`.

**Usage Context:**
Called internally by `scan()`. Not intended for direct use.

---

#### `entry_set($address, $data, $language, $links = NULL)`

**Purpose:**
Indexes or updates an entry in the search database.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$address` | `string` | URL of the entry. |
| `$data` | `array` | Parsed HTML data (e.g., title, text, links). |
| `$language` | `string` | Language of the content. |
| `$links` | `array\|NULL` | Optional array of outbound links. |

**Return Values:**
- `bool`: `TRUE` if indexing succeeded.
- `string`: MySQL error message if indexing failed.

**Inner Mechanisms:**
1. Standardizes the URL and checks if it is accepted.
2. Prepares the content for indexing (e.g., hashing, truncation).
3. Checks if the entry already exists in the database.
4. Updates the entry if it exists and has changed, or creates a new entry.
5. Indexes the content by calling `_entry_set()` for each content type (e.g., title, body text).
6. Processes outbound and inbound links, adding them to the link graph.
7. Clusters similar entries based on simhash similarity.

**Usage Context:**
Called by `_scan()` to index or update content. Can also be used to manually index content.

**Example:**
```php
$search = new \cms\search();
$data = [
    "title" => "Example Page",
    "text" => "This is the main content of the page.",
    "links" => [["url" => "https://example.com/link", "text" => "Link Text"]]
];
$result = $search->entry_set("https://example.com", $data, "en");
if ($result === TRUE) {
    echo "Content indexed successfully.";
}
```

---

#### `_entry_set($source_index, $target_index, $text, $language, $weight_factor = 1.0)`

**Purpose:**
Indexes text content for a specific source-target pair.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$source_index` | `int` | Index of the source entry. |
| `$target_index` | `int` | Index of the target entry. |
| `$text` | `string` | Text to index. |
| `$language` | `string` | Language of the text. |
| `$weight_factor` | `float` | Weight multiplier for the text. |

**Return Values:**
- `bool`: `TRUE` if indexing succeeded.
- `string`: MySQL error message if indexing failed.

**Inner Mechanisms:**
1. Strips stop words from the text.
2. Tokenizes the text into words.
3. Counts word occurrences and inserts new words into the `CMS_DB_SEARCH_WORD` table.
4. Computes word weights and stores them in the `CMS_DB_SEARCH_WEIGHT` table.

**Usage Context:**
Called internally by `entry_set()` to index individual content types (e.g., titles, headings).

---

#### `entry_remove($index, $force = TRUE)`

**Purpose:**
Removes an entry from the search index.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$index` | `int` | Index of the entry to remove. |
| `$force` | `bool` | If `FALSE`, increments the error count instead of removing the entry. |

**Return Values:**
- `bool`: `TRUE` if the entry was removed or the error count was incremented.

**Inner Mechanisms:**
1. If `$force` is `FALSE`, increments the error count and removes canonical status.
2. If `$force` is `TRUE` or the error count exceeds `entry_remove_error_limit`, removes the entry and all associated data (e.g., weights, links, clusters).

**Usage Context:**
Used to remove outdated or invalid entries from the index.

**Example:**
```php
$search = new \cms\search();
$search->entry_remove(123, TRUE); // Force remove entry with index 123
```

---

#### `queue_add($address, $type = CMS_SEARCH_QUEUE_TYPE_NONE)`

**Purpose:**
Adds a URL to the processing queue.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$address` | `string` | URL to add to the queue. |
| `$type` | `int` | One of the `CMS_SEARCH_QUEUE_TYPE_*` constants. |

**Return Values:**
- `bool`: `TRUE` if the URL was added to the queue.

**Inner Mechanisms:**
1. Standardizes the URL and checks if it is accepted.
2. Adds the URL to the queue, updating the type and timestamp if it already exists.
3. For `CMS_SEARCH_QUEUE_TYPE_REFERENCE`, uses `INSERT IGNORE` to avoid duplicates.

**Usage Context:**
Used to schedule URLs for scanning or updates.

**Example:**
```php
$search = new \cms\search();
$search->queue_add("https://example.com", CMS_SEARCH_QUEUE_TYPE_SUBMISSION);
```

---

#### `queue_add_update()`

**Purpose:**
Adds all entries due for an update to the queue.

**Parameters:**
None.

**Return Values:**
- `bool`: `TRUE` if entries were added to the queue.

**Inner Mechanisms:**
Calls `queue_add_all(TRUE)` to add entries with `update_time` in the past.

**Usage Context:**
Used to schedule periodic updates for all indexed entries.

---

#### `queue_add_all($update = FALSE)`

**Purpose:**
Adds all entries or only those due for an update to the queue.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$update` | `bool` | If `TRUE`, only adds entries due for an update. |

**Return Values:**
- `bool`: `TRUE` if entries were added to the queue.

**Inner Mechanisms:**
1. Queries the `CMS_DB_SEARCH_ENTRY` table for entries.
2. Adds entries to the queue with `CMS_SEARCH_QUEUE_TYPE_UPDATE`.
3. If `$update` is `TRUE`, only adds entries with `update_time` in the past.

**Usage Context:**
Used to bulk-schedule updates for indexed entries.

---

#### `queue_remove($type = CMS_SEARCH_QUEUE_TYPE_ALL)`

**Purpose:**
Marks queue entries as done.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$type` | `int` | One of the `CMS_SEARCH_QUEUE_TYPE_*` constants. |

**Return Values:**
- `bool`: `TRUE` if entries were marked as done.

**Inner Mechanisms:**
Updates the `done` flag for queue entries matching the specified type.

**Usage Context:**
Used to clean up the queue after processing.

---

#### `queue_length($type = CMS_SEARCH_QUEUE_TYPE_ALL)`

**Purpose:**
Returns the number of pending queue entries.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$type` | `int` | One of the `CMS_SEARCH_QUEUE_TYPE_*` constants. |

**Return Values:**
- `int\|bool`: Number of pending entries or `FALSE` on failure.

**Inner Mechanisms:**
Counts entries in the queue with `done = 0` and matching the specified type.

**Usage Context:**
Used to monitor the queue length.

**Example:**
```php
$search = new \cms\search();
$pending = $search->queue_length();
if ($pending > 0) {
    echo "$pending URLs pending in the queue.";
}
```

---

#### `queue_process($type = CMS_SEARCH_QUEUE_TYPE_ALL, $follow_links = FALSE)`

**Purpose:**
Processes the next entry in the queue.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$type` | `int` | One of the `CMS_SEARCH_QUEUE_TYPE_*` constants. |
| `$follow_links` | `bool` | If `TRUE`, discovered links are added to the queue. |

**Return Values:**
- `int\|bool`: One of the `CMS_SEARCH_SCAN_*` constants or `FALSE` on failure.

**Inner Mechanisms:**
1. Locks the next queue entry for processing.
2. Calls `scan()` to process the URL.
3. Marks the entry as done or increments the error count if processing fails.
4. Retries failed entries up to `queue_process_error_limit`.

**Usage Context:**
Used to process the queue, typically in a daemon or cron job.

---

#### `daemon_status($enabled = TRUE, $type = CMS_SEARCH_QUEUE_TYPE_ALL, $follow_links = FALSE)`

**Purpose:**
Sets the status of the search daemon.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$enabled` | `bool` | If `TRUE`, enables the daemon. |
| `$type` | `int` | One of the `CMS_SEARCH_QUEUE_TYPE_*` constants. |
| `$follow_links` | `bool` | If `TRUE`, discovered links are added to the queue. |

**Return Values:**
- `bool`: `TRUE` if the status was updated.

**Inner Mechanisms:**
Writes the daemon status to a file (`#system/search.daemon.status`).

**Usage Context:**
Used to control the search daemon.

---

#### `daemon_get_status()`

**Purpose:**
Retrieves the status of the search daemon.

**Parameters:**
None.

**Return Values:**
- `array\|bool`: Associative array with keys `enabled`, `type`, and `follow_links`, or `FALSE` on failure.

**Inner Mechanisms:**
Reads the daemon status from the file (`#system/search.daemon.status`).

**Usage Context:**
Used to check the daemon status.

---

#### `daemon($time_limit = 60)`

**Purpose:**
Runs the search daemon for a specified time limit.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$time_limit` | `int` | Time limit in seconds. |

**Return Values:**
- `bool`: `TRUE` if the daemon completed successfully, `FALSE` on failure.

**Inner Mechanisms:**
1. Retrieves the daemon status.
2. Processes the queue until the time limit is reached or the queue is empty.
3. Sleeps for 1 second between iterations to avoid overloading the server.

**Usage Context:**
Used to run the search daemon as a background process.

**Example:**
```php
$search = new \cms\search();
$search->daemon(300); // Run for 5 minutes
```

---

#### `find($term, $page = 0, $language = CMS_LANGUAGE)`

**Purpose:**
Searches the index for a given term and returns paginated results.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$term` | `string` | Search term. |
| `$page` | `int` | Page number (0-based). |
| `$language` | `string` | Language of the search term. |

**Return Values:**
- `array\|bool`: Associative array of search results or `FALSE` on failure.

**Inner Mechanisms:**
1. Tokenizes the search term and preprocesses the tokens.
2. Retrieves word indices matching the search tokens.
3. Queries the database for entries matching the search tokens, ordered by relevance.
4. Extracts a snippet of text containing the search term for each result.
5. Returns an array of results with keys `address`, `title`, `text`, `time`, `score`, and `supplemental`.

**Usage Context:**
Used to perform searches and display results.

**Example:**
```php
$search = new \cms\search();
$results = $search->find("example", 0, "en");
if ($results !== FALSE) {
    foreach ($results as $result) {
        echo "<h3>{$result['title']}</h3>";
        echo "<p>{$result['text']}</p>";
    }
}
```

---

#### `tag($entry_index = NULL, $limit = 10, $language = CMS_LANGUAGE)`

**Purpose:**
Retrieves the most frequent words (tags) for an entry or the entire index.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$entry_index` | `int\|NULL` | Index of the entry. If `NULL`, retrieves tags for the entire index. |
| `$limit` | `int` | Maximum number of tags to return. |
| `$language` | `string` | Language of the tags. |

**Return Values:**
- `array\|bool`: Associative array of tags and their weights or `FALSE` on failure.

**Inner Mechanisms:**
1. If `$entry_index` is `NULL`, retrieves the most frequent words across all entries.
2. If `$entry_index` is specified, retrieves the most frequent words for the entry.
3. Filters words by length and language.

**Usage Context:**
Used to display tags or keywords for entries.

**Example:**
```php
$search = new \cms\search();
$tags = $search->tag(123, 5, "en");
if ($tags !== FALSE) {
    foreach ($tags as $tag => $weight) {
        echo "<span>$tag</span>";
    }
}
```

---

#### `score_compute()`

**Purpose:**
Computes relevance scores for all entries.

**Parameters:**
None.

**Return Values:**
- `bool`: `TRUE` if score computation succeeded.

**Inner Mechanisms:**
1. Checks for new or updated entries.
2. Initializes the score computation by calling `score_compute_initialize()`.
3. Iteratively refines scores by calling `score_compute_iterate()`.
4. Finalizes the computation by calling `score_compute_finalize()`.
5. Computes scores for dangling links (links to non-indexed URLs).
6. Updates canonical entries by calling `score_compute_canonical()`.

**Usage Context:**
Used to update relevance scores, typically after indexing new content.

---

#### `score_compute_initialize()`

**Purpose:**
Initializes the score computation process.

**Parameters:**
None.

**Return Values:**
- `bool`: `TRUE` if initialization succeeded.
- `string`: MySQL error message if initialization failed.

**Inner Mechanisms:**
1. Resets link targets and levels.
2. Identifies and marks dangling links.
3. Resets entry scores and link counts.

**Usage Context:**
Called internally by `score_compute()`.

---

#### `score_compute_iterate($link_level = 0)`

**Purpose:**
Performs one iteration of score refinement.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$link_level` | `int` | Level of links to process (0 for all links). |

**Return Values:**
- `bool`: `TRUE` if the iteration succeeded.
- `string`: MySQL error message if the iteration failed.

**Inner Mechanisms:**
1. Computes the inherited score portion for each entry.
2. Updates entry scores based on the dampening factor.

**Usage Context:**
Called internally by `score_compute()`.

---

#### `score_compute_finalize()`

**Purpose:**
Finalizes the score computation process.

**Parameters:**
None.

**Return Values:**
- `bool`: `TRUE` if finalization succeeded.
- `string`: MySQL error message if finalization failed.

**Inner Mechanisms:**
Updates the link count for each entry to include all links (not just non-dangling ones).

**Usage Context:**
Called internally by `score_compute()`.

---

#### `score_compute_link_level()`

**Purpose:**
Retrieves the maximum link level in the link graph.

**Parameters:**
None.

**Return Values:**
- `int\|bool`: Maximum link level or `FALSE` on failure.

**Inner Mechanisms:**
Queries the `CMS_DB_SEARCH_LINK` table for the maximum `level`.

**Usage Context:**
Called internally by `score_compute()` to process dangling links.

---

#### `score_compute_canonical()`

**Purpose:**
Updates the canonical status of entries.

**Parameters:**
None.

**Return Values:**
- `bool`: `TRUE` if the update succeeded.
- `string`: MySQL error message if the update failed.

**Inner Mechanisms:**
1. Resets all entries to canonical status.
2. Sets entries with a higher-scoring alternative in the same cluster to non-canonical.

**Usage Context:**
Called internally by `score_compute()`.

---

#### `clean()`

**Purpose:**
Cleans up orphaned data in the search database.

**Parameters:**
None.

**Return Values:**
- `bool`: `TRUE` if cleanup succeeded.

**Inner Mechanisms:**
1. Removes orphaned weights, words, links, and clusters.
2. Optimizes database tables.

**Usage Context:**
Used to maintain database integrity, typically in a maintenance script.

---

#### `address_standardize($address)`

**Purpose:**
Standardizes a URL for consistent indexing.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$address` | `string` | URL to standardize. |

**Return Values:**
- `string\|bool`: Standardized URL or `FALSE` on failure.

**Inner Mechanisms:**
1. Parses the URL and validates its scheme.
2. Sorts the query string parameters alphabetically.
3. Reassembles the URL without fragments or unsupported components.

**Usage Context:**
Called internally by `scan()` and `queue_add()` to ensure URL consistency.

---

#### `address_accepted($address)`

**Purpose:**
Checks if a URL is accepted for indexing based on blacklist/whitelist rules.

**Parameters:**

| Name | Type | Description |
|------|------|-------------|
| `$address` | `string` | URL to check. |

**Return Values:**
- `bool`: `TRUE` if the URL is accepted, `FALSE` otherwise.

**Inner Mechanisms:**
1. Checks the URL against the blacklist. If matched, checks the whitelist for an override.
2. Supports regex patterns with optional inversion (e.g., `!^https://`).

**Usage Context:**
Called internally by `scan()` and `queue_add()` to filter URLs.

**Example:**
```php
$search = new \cms\search();
$search->address_accepted_blacklist = ["!^https://"];
$search->address_accepted_whitelist = ["^https://example.com"];
if ($search->address_accepted("https://example.com")) {
    echo "URL accepted.";
}
```


<!-- HASH:ddcd5892ab6e17e3ad57568812dea1f3 -->
