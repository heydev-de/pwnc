# PWNC API Documentation

[← Index](../README.md) | [`module/sitemap.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/sitemap.php)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Sitemap Module (`module/sitemap.php`)

Generates an XML sitemap for search engines following the **Sitemap Protocol 0.9** (`http://www.sitemaps.org/schemas/sitemap/0.9`). The module dynamically retrieves all published content entries from the database, resolves their canonical URLs across all enabled languages, and outputs a well-formed XML document with `<url>`, `<loc>`, `<lastmod>`, and multilingual `<xhtml:link>` elements.

---

### **Purpose**
- **SEO Optimization**: Provides search engines with a structured list of all publicly accessible pages.
- **Multilingual Support**: Generates language-specific URLs and alternate links for each content entry.
- **Performance**: Uses caching to avoid regenerating the sitemap on every request.

---

### **Workflow**
1. **Caching Check**: Attempts to serve a cached version if available and not expired (60-second delay).
2. **Language Mapping**: Loads language-specific directory mappings for URL resolution.
3. **Database Query**: Retrieves all published content entries, excluding those flagged for sitemap exclusion or `noindex`.
4. **URL Generation**: Resolves canonical URLs for each content entry in all enabled languages.
5. **XML Construction**: Builds the sitemap XML with `<url>` entries, including last modification dates and multilingual alternates.
6. **Caching & Output**: Caches the generated XML and outputs it with the correct `Content-Type` header.

---

### **Key Components**

#### **Constants & Variables**
| Name               | Value/Default                     | Description                                                                 |
|--------------------|-----------------------------------|-----------------------------------------------------------------------------|
| `$cache_key`       | `"sitemap"`                       | Cache key for storing/retrieving the sitemap.                              |
| `$cache_time`      | `cms_cache_time($cache_key)`      | Timestamp of the last cache update.                                         |
| `$time`            | `time()`                          | Current Unix timestamp.                                                     |
| `$map`             | `[]`                              | Associative array mapping language IDs to `map` objects for URL resolution.|
| `$language`        | `[0]` or `explode(",", CMS_LANGUAGE_ENABLED)` | Array of enabled language IDs (0 = default/language-agnostic).             |
| `$buffer`          | `""`                              | Accumulates the XML output.                                                 |

---

### **Functions & Logic**

#### **Caching Mechanism**
- **Check**: If a cached sitemap exists and is less than 60 seconds old, it is served directly via `cms_cache_notouch()`.
- **Update**: After generating the sitemap, it is cached permanently (`$p = TRUE`) via `cms_cache()`.

**Example**:
```php
// Serve cached sitemap if available and recent
if (($cache_time !== FALSE) && ($cache_time > ($time - 60))) {
    echo(cms_cache_notouch($cache_key));
    exit();
}
```

---

#### **Language-Specific URL Resolution**
- **Directory Mapping**: For each enabled language, a `map` object is instantiated to resolve content indices to canonical URLs.
  - Path: `#system/{language_prefix}directory.content` (e.g., `#system/en.directory.content` for English).
  - Language prefix is omitted for the default language (`0`).

**Example**:
```php
foreach ($language AS $value) {
    $_value = ($value !== 0) ? "$value." : "";
    $map[$value] = new map("#system/" . $_value . "directory.content");
}
```

---

#### **Database Query**
- **Fields**: Retrieves `CMS_DB_CONTENT_INDEX` (content ID) and `CMS_DB_CONTENT_TIME` (last modification timestamp).
- **Filters**:
  - Status: `CMS_CONTENT_STATUS_PUBLICATION` (published content only).
  - Flags: Excludes entries with `CMS_CONTENT_FLAG_SITEMAP_EXCLUDE` or `CMS_CONTENT_FLAG_META_ROBOTS_NOINDEX`.
- **Sorting**: Orders by `CMS_DB_CONTENT_TIME` (newest first).

**Example**:
```php
$result = mysql_query(
    "SELECT " . CMS_DB_CONTENT_INDEX . ", " . CMS_DB_CONTENT_TIME . " " .
    "FROM " . CMS_DB_CONTENT . " " .
    "WHERE " . CMS_DB_CONTENT_STATUS . "='" . CMS_CONTENT_STATUS_PUBLICATION . "' " .
    "AND NOT " . CMS_DB_CONTENT_FLAG . " & " . (CMS_CONTENT_FLAG_SITEMAP_EXCLUDE | CMS_CONTENT_FLAG_META_ROBOTS_NOINDEX) . " " .
    "ORDER BY " . CMS_DB_CONTENT_TIME . " DESC"
);
```

---

#### **XML Construction**
- **Root Element**: `<urlset>` with XML namespaces for sitemaps and XHTML.
- **URL Entries**: For each content entry:
  - `<loc>`: Canonical URL (escaped with `x()` for XML).
  - `<lastmod>`: Last modification date (formatted as `Y-m-d`).
  - **Multilingual Alternates**: For non-default languages, `<xhtml:link>` elements are added for all enabled languages, including a fallback `x-default` link if applicable.

**Example**:
```php
$buffer .= "<url>\n" .
    "<loc>" . x($url) . "</loc>\n" .
    "<lastmod>$date</lastmod>\n" .
    $_buffer . // Multilingual alternates
    "</url>\n";
```

---

### **Usage Scenarios**

#### **1. Generating a Sitemap**
- **Trigger**: Access the module directly via its URL (e.g., `https://example.com/module/sitemap.php`).
- **Output**: XML sitemap with all published content entries.
- **Caching**: Subsequent requests within 60 seconds will serve the cached version.

**Example Output**:
```xml
<?xml version="1.0" encoding="utf-8"?>
<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9" xmlns:xhtml="http://www.w3.org/1999/xhtml">
    <url>
        <loc>https://example.com/about</loc>
        <lastmod>2023-10-01</lastmod>
        <xhtml:link rel="alternate" href="https://example.com/en/about" hreflang="en"/>
        <xhtml:link rel="alternate" href="https://example.com/es/about" hreflang="es"/>
        <xhtml:link rel="alternate" href="https://example.com/en/about" hreflang="x-default"/>
    </url>
</urlset>
```

---

#### **2. Integration with Search Engines**
- **Submission**: Submit the sitemap URL to search engines (e.g., Google Search Console, Bing Webmaster Tools).
- **Automation**: Schedule a cron job to ping the sitemap URL periodically (e.g., daily) to ensure freshness.

**Example (Cron Job)**:
```bash
0 3 * * * curl -s https://example.com/module/sitemap.php > /dev/null
```

---

#### **3. Customization**
- **Excluding Content**: Flag content entries with `CMS_CONTENT_FLAG_SITEMAP_EXCLUDE` to omit them from the sitemap.
- **Language Support**: Enable additional languages via `CMS_LANGUAGE_ENABLED` (e.g., `"en,es,fr"`).

**Example (Excluding Content)**:
```php
// In content management interface, set the flag:
$content_flags |= CMS_CONTENT_FLAG_SITEMAP_EXCLUDE;
```

---

### **Dependencies**
- **Libraries**:
  - `content`: Required for content management logic (loaded via `cms_load("content", TRUE)`).
- **Database**: Relies on the `CMS_DB_CONTENT` table and its fields (`CMS_DB_CONTENT_INDEX`, `CMS_DB_CONTENT_TIME`, etc.).
- **Caching**: Uses `cms_cache()` and `cms_cache_time()` for performance optimization.

---

### **Error Handling**
- **Missing URLs**: Skips content entries without a canonical URL in the directory mapping (`if ($url === NULL) continue`).
- **Database Errors**: Assumes `mysql_query()` succeeds; errors would propagate as PHP warnings/errors (no explicit handling).


<!-- HASH:f2bfe6aaaeb38aecee97d02545c3314c -->
