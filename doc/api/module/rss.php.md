# PWNC API Documentation

[← Index](../README.md) | [`module/rss.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/rss.php)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## RSS Module (`module/rss.php`)

The RSS module generates RSS 2.0 feeds for content channels in the PWNC Web Platform. It dynamically constructs an XML feed based on content items from a specified channel, with configurable sorting, limiting, and caching mechanisms.

---

### Overview

This module:
1. Loads required libraries (`content`, `directory`, `rss`).
2. Validates the requested RSS channel.
3. Fetches and formats content items into an RSS 2.0-compliant XML document.
4. Implements caching to reduce database load (60-second delay).
5. Supports custom sorting (`published` or `modified`).
6. Automatically resolves URLs for content, images, and directories.
7. Escapes all output for XML compliance.

---

### Constants and Global Variables

| Name            | Value/Default | Description                                                                 |
|-----------------|---------------|-----------------------------------------------------------------------------|
| `$rss_channel`  | (Required)    | Identifier of the content channel to generate the RSS feed for.             |
| `$rss_limit`    | `NULL`        | Maximum number of items to include in the feed. Defaults to all.           |
| `$rss_order`    | `NULL`        | Sorting order: `"published"` (default) or `"modified"`.                     |
| `$cache_key`    | (Generated)   | Cache key for the RSS feed, based on channel, limit, order, and language.   |
| `$cache_time`   | (Dynamic)     | Timestamp of the last cache update.                                         |

---

### Core Logic Flow

1. **Initialization**:
   - Loads required libraries.
   - Sets the `Content-Type` header to `application/rss+xml`.
   - Validates the `$rss_channel` parameter.

2. **Caching**:
   - Checks if a valid cache exists (updated within the last 60 seconds).
   - If cached, outputs the cached feed and exits.

3. **Channel Metadata**:
   - Constructs the RSS `<channel>` element with title, link, description, language, and other metadata.
   - Resolves the channel’s image URL (fallback to a default RSS SVG).

4. **Content Items**:
   - Queries the database for content items in the specified channel.
   - Sorts items by `$rss_order` (default: publication time).
   - Limits results if `$rss_limit` is set.
   - For each item:
     - Generates `<item>` elements with title, link, description, categories, enclosure (for images), GUID, and publication date.
     - Resolves URLs for content and images.
     - Processes images to a maximum dimension of 500x500 pixels.
     - Escapes all output for XML compliance.

5. **Output**:
   - Caches the generated feed permanently.
   - Outputs the XML document.

---

### Key Functions and Methods

#### `cms_cache($k, $v, $p=FALSE, $n=FALSE)`
**Purpose**:
Manages dual-layer caching (RAM and permanent storage) for the RSS feed.

**Parameters**:

| Name | Type      | Description                                                                 |
|------|-----------|-----------------------------------------------------------------------------|
| `$k` | `string`  | Cache key (e.g., `"rss.$rss_channel.$rss_limit.$rss_order." . CMS_LANGUAGE`). |
| `$v` | `mixed`   | Value to cache (the generated RSS XML).                                    |
| `$p` | `bool`    | If `TRUE`, caches permanently (default: `FALSE`).                           |
| `$n` | `bool`    | If `TRUE`, bypasses RAM cache (default: `FALSE`).                           |

**Return Values**:
- `void`

**Inner Mechanisms**:
- Stores the RSS feed in both RAM and permanent storage (if `$p=TRUE`).
- Reduces database queries and processing time for repeated requests.

**Usage Context**:
- Called after generating the RSS feed to cache the output.

---

#### `cms_cache_time($cache_key)`
**Purpose**:
Retrieves the timestamp of the last cache update for a given key.

**Parameters**:

| Name        | Type     | Description                     |
|-------------|----------|---------------------------------|
| `$cache_key`| `string` | Key used to identify the cache. |

**Return Values**:
- `int|FALSE`: Timestamp of the last update, or `FALSE` if the cache does not exist.

**Inner Mechanisms**:
- Checks both RAM and permanent storage for the cache entry.

**Usage Context**:
- Used to determine if the cached RSS feed is still valid (updated within the last 60 seconds).

---

#### `cms_cache_notouch($cache_key)`
**Purpose**:
Retrieves a cached value without updating its timestamp.

**Parameters**:

| Name        | Type     | Description                     |
|-------------|----------|---------------------------------|
| `$cache_key`| `string` | Key used to identify the cache. |

**Return Values**:
- `mixed|FALSE`: The cached value, or `FALSE` if the cache does not exist.

**Inner Mechanisms**:
- Retrieves the cached RSS feed from storage without modifying its metadata.

**Usage Context**:
- Used to output a cached RSS feed if it exists and is still valid.

---

#### `translate_url($addr, ...)`
**Purpose**:
Resolves logical identifiers (e.g., `content://`, `image://`, `directory://`) into physical URLs.

**Parameters**:

| Name    | Type     | Description                                                                 |
|---------|----------|-----------------------------------------------------------------------------|
| `$addr` | `string` | Logical URL (e.g., `content://123`, `image://logo.png`).                    |
| `...`   | `mixed`  | Additional parameters (e.g., language, query parameters).                   |

**Return Values**:
- `string`: Resolved physical URL.

**Inner Mechanisms**:
- Parses the logical identifier and maps it to the corresponding physical path or URL.
- Supports dynamic resolution for content, images, and directories.

**Usage Context**:
- Used to generate links for content items, channel images, and directory categories.

**Example**:
```php
// Resolve a content URL
$url = translate_url("content://123", NULL, CMS_LANGUAGE, TRUE);
// Output: "https://example.com/content/123?lang=en"
```

---

#### `image_process($url, $width, $height)`
**Purpose**:
Processes an image URL to ensure it fits within specified dimensions.

**Parameters**:

| Name      | Type     | Description                              |
|-----------|----------|------------------------------------------|
| `$url`    | `string` | URL of the image to process.             |
| `$width`  | `int`    | Maximum width of the processed image.    |
| `$height` | `int`    | Maximum height of the processed image.   |

**Return Values**:
- `string`: URL of the processed image.

**Inner Mechanisms**:
- Checks if the image exists locally or remotely.
- Applies resizing if necessary (e.g., for enclosures in RSS feeds).

**Usage Context**:
- Used to generate image URLs for RSS enclosures.

**Example**:
```php
// Process an image to fit within 500x500 pixels
$processed_url = image_process("image://example.jpg", 500, 500);
// Output: "https://example.com/images/processed/example_500x500.jpg"
```

---

#### `x($s)`
**Purpose**:
Escapes a string for XML output by replacing special characters (`"`, `'`, `&`, `<`, `>`) with their XML entities.

**Parameters**:

| Name | Type     | Description               |
|------|----------|---------------------------|
| `$s` | `string` | String to escape.         |

**Return Values**:
- `string`: XML-escaped string.

**Inner Mechanisms**:
- Uses `htmlspecialchars` with `ENT_QUOTES` and `UTF-8` encoding.

**Usage Context**:
- Ensures all dynamic content in the RSS feed is XML-compliant.

**Example**:
```php
$title = "Example & Test";
$escaped_title = x($title);
// Output: "Example &amp; Test"
```

---

### Usage Example

#### Generating an RSS Feed for a Content Channel
To generate an RSS feed for a channel named `"news"` with a limit of 10 items, sorted by publication date:

1. **Request the RSS Feed**:
   Access the RSS module with the following parameters:
   ```
   https://example.com/module/rss.php?rss_channel=news&rss_limit=10&rss_order=published
   ```

2. **Expected Output**:
   The module will:
   - Validate the `news` channel.
   - Query the database for the 10 most recently published content items.
   - Generate an RSS 2.0 XML document with:
     - Channel metadata (title, link, description, etc.).
     - 10 `<item>` elements, each with a title, link, description, and publication date.
   - Cache the output for 60 seconds to optimize performance.

3. **Example RSS Output**:
   ```xml
   <?xml version="1.0" encoding="utf-8"?>
   <rss version="2.0" xmlns:atom="http://www.w3.org/2005/Atom">
     <channel>
       <atom:link href="https://example.com/module/rss.php?rss_channel=news&amp;rss_limit=10&amp;rss_order=published" rel="self" type="application/rss+xml"/>
       <title>News</title>
       <link>https://example.com/news</link>
       <description>Latest news updates</description>
       <language>en</language>
       <pubDate>Mon, 01 Jan 2023 12:00:00 +0000</pubDate>
       <lastBuildDate>Mon, 01 Jan 2023 12:00:00 +0000</lastBuildDate>
       <generator>PWNC Web Platform</generator>
       <image>
         <url>https://example.com/images/news_rss.svg</url>
         <title>News</title>
         <link>https://example.com/news</link>
       </image>
       <item>
         <title>Breaking News: Example Event</title>
         <link>https://example.com/content/123</link>
         <description>This is an example news item.</description>
         <category domain="https://example.com/news">News</category>
         <guid isPermaLink="false">123</guid>
         <pubDate>Mon, 01 Jan 2023 12:00:00 +0000</pubDate>
       </item>
       <!-- Additional items -->
     </channel>
   </rss>
   ```

---

### Error Handling

- **Invalid Channel**: If `$rss_channel` is not provided or does not exist, the module exits silently.
- **Database Errors**: If the database query fails, the module continues execution but may omit dynamic data (e.g., publication dates).
- **Image Processing**: If an image cannot be processed (e.g., invalid URL), the enclosure is omitted from the RSS item.

---

### Best Practices

1. **Caching**:
   - Leverage the built-in caching to reduce database load. The 60-second delay is configurable by modifying the cache time check.
   - For high-traffic sites, consider increasing the cache delay or implementing a CDN.

2. **URL Resolution**:
   - Use logical identifiers (e.g., `content://`, `image://`) for URLs to ensure portability across environments.

3. **Escaping**:
   - Always use `x()` to escape dynamic content in XML output to prevent malformed feeds.

4. **Performance**:
   - Limit the number of items (`$rss_limit`) to improve performance and reduce feed size.
   - Use `$rss_order` to prioritize sorting by publication or modification time.


<!-- HASH:b7dd2abb915e17a6b2ff246dd545d241 -->
