# PWNC API Documentation

[← Index](../../README.md) | [`module/#interface/ifc.search.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23interface/ifc.search.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Search Interface Module (`ifc.search.inc`)

This file implements the **Search** module interface for the PWNC Web Platform. It provides a comprehensive administrative UI for managing search functionality, including:
- **Indexing** (scanning, queue management, and processing)
- **Data processing** (score computation, canonical entry detection)
- **Filtering and configuration** (blacklists, whitelists, stopwords, daemon settings)
- **Entry management** (removal, filtering, and display)

The interface interacts with the `search` class, which handles core search operations, and integrates with the PWNC IFC (Interface Controller) system for UI rendering and user interaction.

---

## Constants and Initialization

### Constants
| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_DB_SEARCH_ENTRY` | `cms_search_entry` | Database table storing search entries. |
| `CMS_DB_SEARCH_ENTRY_INDEX` | `index` | Primary key for search entries. |
| `CMS_DB_SEARCH_ENTRY_ADDRESS` | `address` | URL of the indexed page. |
| `CMS_DB_SEARCH_ENTRY_TITLE` | `title` | Title of the indexed page. |
| `CMS_DB_SEARCH_ENTRY_TEXT` | `text` | Extracted text content of the page. |
| `CMS_DB_SEARCH_ENTRY_SCORE` | `score` | Relevance score of the entry. |
| `CMS_DB_SEARCH_ENTRY_TIME` | `time` | Timestamp of initial indexing. |
| `CMS_DB_SEARCH_ENTRY_UPDATE_TIME` | `update_time` | Timestamp of last update. |
| `CMS_DB_SEARCH_ENTRY_ERROR` | `error` | Error code (if any). |
| `CMS_DB_SEARCH_ENTRY_CANONICAL` | `canonical` | Boolean flag for canonical entries. |
| `CMS_SEARCH_QUEUE_TYPE_*` | Various | Queue types (e.g., `INTERNAL`, `SELECTION`, `SUBMISSION`). |
| `CMS_SEARCH_SCAN_*` | Various | Scan result codes (e.g., `INDEXED`, `NO_CONNECTION`). |

### Initialization
The interface initializes default values for filtering, sorting, and pagination:
- **`$sql_table`**: Defaults to `CMS_DB_SEARCH_ENTRY`.
- **`$sql_filter_field`**: Defaults to `CMS_DB_SEARCH_ENTRY_ADDRESS`.
- **`$sql_filter_option`**: Defaults to `LIKE '%#value#%'`.
- **`$sql_order`**: Defaults to `CMS_DB_SEARCH_ENTRY_TIME DESC`.
- **`$page`**: Defaults to `0`.
- **`$limit`**: Defaults to `25`.

---

## Message Handling

The interface processes user actions via `CMS_IFC_MESSAGE` cases. Each case corresponds to a specific operation (e.g., scanning, queue management).

---

### `scan`
**Purpose**:
Scans a URL for indexing and adds it to the search queue.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | URL to scan. |
| `$ifc_param2` | `bool` | Whether to follow links (optional). |

**Return Values**:
- `CMS_MSG_ERROR` or `CMS_MSG_DONE` based on the scan result.
- Human-readable message from `$array` mapping scan codes to localized strings.

**Inner Mechanisms**:
1. Translates the URL using `translate_url()`.
2. Calls `$search->scan()` to process the URL.
3. Determines the response type (error or success) based on the result.

**Usage Example**:
```php
// Scan a URL and follow links
$ifc_param1 = "https://example.com";
$ifc_param2 = true;
$ifc_response = ($search->scan($ifc_param1, $ifc_param2) & CMS_SEARCH_SCAN_ERROR) ? CMS_MSG_ERROR : CMS_MSG_DONE;
```

---

### `queue_add`
**Purpose**:
Adds selected entries to the search queue.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$list` | `array` | Array of entry indices to add. |

**Return Values**:
- `CMS_MSG_ERROR` if any addition fails.
- `CMS_MSG_DONE` if all additions succeed.

**Inner Mechanisms**:
1. Queries the database for addresses of selected entries.
2. Calls `$search->queue_add()` for each address.
3. Aggregates errors to determine the final response.

**Usage Example**:
```php
// Add entries with indices [1, 2, 3] to the queue
$list = [1, 2, 3];
$error = false;
foreach ($list as $index) {
    $error |= !$search->queue_add($index, CMS_SEARCH_QUEUE_TYPE_SELECTION);
}
$ifc_response = $error ? CMS_MSG_ERROR : CMS_MSG_DONE;
```

---

### `queue_add_all`
**Purpose**:
Adds all entries in the search table to the queue.

**Return Values**:
- `CMS_MSG_DONE` if successful.
- `CMS_MSG_ERROR` if failed.

**Usage Example**:
```php
// Add all entries to the queue
$ifc_response = $search->queue_add_all() ? CMS_MSG_DONE : CMS_MSG_ERROR;
```

---

### `queue_add_internal`
**Purpose**:
Adds all internal content pages to the queue.

**Inner Mechanisms**:
1. Iterates over enabled languages.
2. Uses the `map` class to retrieve content addresses.
3. Calls `$search->queue_add()` for each address.

**Usage Example**:
```php
// Add all internal content to the queue
$count = 0;
foreach (explode(",", CMS_LANGUAGE_ENABLED) as $language) {
    $map = new map("#system/" . ($language ? "$language." : "") . "directory.content");
    foreach ($map->get_value_list() as $address) {
        $count += $search->queue_add($address, CMS_SEARCH_QUEUE_TYPE_INTERNAL);
    }
}
$ifc_response = $count ? CMS_MSG_DONE : CMS_MSG_ERROR;
```

---

### `queue_remove`
**Purpose**:
Removes an entry from the queue.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Queue type to remove. |

**Return Values**:
- `CMS_MSG_DONE` if successful.
- `CMS_MSG_ERROR` if failed.

**Usage Example**:
```php
// Remove a queue entry
$ifc_response = $search->queue_remove(CMS_SEARCH_QUEUE_TYPE_SELECTION) ? CMS_MSG_DONE : CMS_MSG_ERROR;
```

---

### `queue_process` and `_queue_process`
**Purpose**:
Processes the search queue (UI and backend logic).

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Queue type to process. |
| `$follow_links` | `bool` | Whether to follow links (optional). |

**Inner Mechanisms**:
1. **`queue_process`**: Renders a UI for selecting queue type and link-following option.
2. **`_queue_process`**: Displays the queue length and spawns 4 threads for parallel processing.

**Usage Example**:
```php
// Start processing the queue (UI)
$ifc = new ifc();
$ifc->set([
    CMS_L_IFC_SEARCH_019 => CMS_SEARCH_QUEUE_TYPE_ALL,
    CMS_L_IFC_SEARCH_020 => CMS_SEARCH_QUEUE_TYPE_INTERNAL,
], "select b", $ifc_param);
$ifc->set(CMS_L_IFC_SEARCH_016, "checkbox b", true, false);
$ifc->close();
```

---

### `queue_process_thread`
**Purpose**:
Handles individual queue processing in a thread.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param` | `string` | Queue type to process. |
| `$follow_links` | `bool` | Whether to follow links. |

**Return Values**:
- HTML output for the thread, including status updates.
- JavaScript to reload the thread or update the parent UI.

**Inner Mechanisms**:
1. Calls `$search->queue_process()` to process a single queue item.
2. Determines the status (error, success, or completion) and updates the UI.
3. Uses `force_flush()` to ensure real-time updates.

**Usage Example**:
```php
// Process a queue item in a thread
$value = $search->queue_process(CMS_SEARCH_QUEUE_TYPE_ALL, false);
if ($value !== false) {
    $class = (CMS_SEARCH_SCAN_ERROR & $value) ? "error" : "ok";
    $text = ($class === "error") ? CMS_L_IFC_SEARCH_030 : CMS_L_IFC_SEARCH_031;
    echo(jscript("document.getElementById('loader').className = 'search-thread-$class';"));
}
```

---

### `entry_remove`
**Purpose**:
Removes selected entries from the search index.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$list` | `array` | Array of entry indices to remove. |

**Return Values**:
- `CMS_MSG_DONE` if all removals succeed.
- `CMS_MSG_ERROR` if any removal fails.

**Usage Example**:
```php
// Remove entries with indices [1, 2, 3]
$list = [1, 2, 3];
$error = false;
foreach ($list as $index) {
    $error |= !$search->entry_remove($index);
}
$ifc_response = $error ? CMS_MSG_ERROR : CMS_MSG_DONE;
```

---

### `data_process` and `data_process_thread`
**Purpose**:
Processes search data (e.g., score computation, canonical entry detection).

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$action` | `int` | Current processing step (0-5). |
| `$iteration` | `int` | Current iteration (for multi-step processes). |

**Inner Mechanisms**:
1. **`data_process`**: Renders a UI with a thread for processing.
2. **`data_process_thread`**: Handles each step of data processing:
   - **0**: Initialize page scores.
   - **1**: Compute page scores iteratively.
   - **2**: Finalize page scores.
   - **3**: Integrate dangling links.
   - **4**: Find canonical entries.
   - **5**: Completion.

**Usage Example**:
```php
// Start data processing (UI)
$ifc = new ifc();
echo("<object data=\"" . x(cms_url(["ifc_page" => CMS_IFC_PAGE, "ifc_message" => "data_process_thread"])) . "\" type=\"text/html\"></object>");
$ifc->close();
```

---

### `filter` and `_filter`
**Purpose**:
Manages URL filtering (blacklists and whitelists).

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Blacklist (newline-separated URLs). |
| `$ifc_param2` | `string` | Whitelist (newline-separated URLs). |

**Inner Mechanisms**:
1. **`filter`**: Renders a UI for editing blacklists and whitelists.
2. **`_filter`**: Saves the updated lists to the system configuration.

**Usage Example**:
```php
// Save blacklist and whitelist
$system = new system();
$system->setval($ifc_param1, "search", "blacklist");
$system->setval($ifc_param2, "search", "whitelist");
$ifc_response = $system->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
```

---

### `stopword` and `_stopword`
**Purpose**:
Manages stopwords for search indexing.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `string` | Stopwords (newline-separated, per language). |

**Inner Mechanisms**:
1. **`stopword`**: Renders a UI for editing stopwords.
2. **`_stopword`**: Saves stopwords to the language data store.

**Usage Example**:
```php
// Save stopwords for all languages
$data = new data("#system/language");
$array = language_get_array($ifc_param1);
foreach ($array as $language => $stopwords) {
    $data->set($stopwords, $language, "stopword");
}
$ifc_response = $data->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
```

---

### `clean`
**Purpose**:
Cleans the search index (removes outdated or invalid entries).

**Return Values**:
- `CMS_MSG_DONE` if successful.
- `CMS_MSG_ERROR` if failed.

**Usage Example**:
```php
// Clean the search index
$ifc_response = $search->clean() ? CMS_MSG_DONE : CMS_MSG_ERROR;
```

---

### `configuration` and `_configuration`
**Purpose**:
Configures search daemon settings and entry processing parameters.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| `$ifc_param1` | `bool` | Enable/disable daemon. |
| `$ifc_param2-$ifc_param6` | `bool` | Queue types to process. |
| `$ifc_param7` | `bool` | Follow links. |
| `$ifc_param8` | `int` | Maximum bit difference for entries. |

**Inner Mechanisms**:
1. **`configuration`**: Renders a UI for daemon settings and bit difference configuration.
2. **`_configuration`**: Saves settings and updates the search daemon.

**Usage Example**:
```php
// Save daemon configuration
$type = (isset($ifc_param2) ? CMS_SEARCH_QUEUE_TYPE_INTERNAL : 0) |
        (isset($ifc_param3) ? CMS_SEARCH_QUEUE_TYPE_SELECTION : 0);
$search->daemon_status(isset($ifc_param1), $type, isset($ifc_param7));
$system = new system();
$system->setval($ifc_param8, "search", "difference");
$ifc_response = $system->save() ? CMS_MSG_DONE : CMS_MSG_ERROR;
```

---

## Main Display

### Menu
The interface provides a menu for common actions:
- **Add to Queue**: Selected entries, internal content, or all entries.
- **Delete Selected**: Remove entries from the index.
- **Process Data**: Compute scores and canonical entries.
- **Filter**: Manage blacklists/whitelists.
- **Stopwords**: Edit stopwords.
- **Clean**: Remove outdated entries.
- **Configuration**: Adjust daemon settings.

### Search Filter
The filter UI allows users to:
- Select a field (`address`, `title`, `text`, etc.).
- Choose a condition (`LIKE`, `=`, `>`, etc.).
- Enter a value.
- Sort results.
- Set pagination limits.

### Data Display
Search entries are displayed in a table with columns for:
- **Selection** (checkboxes for bulk actions).
- **Address** (URL with icon indicating status).
- **Title**.
- **Score**.
- **Time** (initial indexing).
- **Update Time**.

**Example Output**:
```html
<tr>
    <td class="select"><input type="checkbox" name="list[]" value="1"></td>
    <td><a href="https://example.com">📄 https://example.com</a></td>
    <td>Example Page</td>
    <td>42.50</td>
    <td>2023-01-01 12:00</td>
    <td>2023-01-02 12:00</td>
</tr>
```

### Pagination
If the result count exceeds the limit, pagination controls are displayed:
```php
pagination("javascript:p(%page%);", $page, $count, CMS_L_COMMAND_NEXT, "pagination");
```

---


<!-- HASH:a34283df8147fa3d4da53d7bcc1246d9 -->
