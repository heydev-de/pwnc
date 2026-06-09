# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.rss.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.rss.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## RSS Feed Management (`lib.rss.inc`)

This file provides functionality for managing RSS feed channels within the PWNC Web Platform. It includes a utility function for retrieving default RSS channels and a class for creating, modifying, and deleting RSS feed channels.

---

### Utility Function

#### `rss_get_default()`
Retrieves the default RSS channel(s) from the system configuration.

**Parameters:**
None

**Return Values:**
| Type   | Description                                                                 |
|--------|-----------------------------------------------------------------------------|
| string | A string containing the path(s) of the default RSS channel(s), e.g., `/key/` |

**Inner Mechanisms:**
- Initializes a `data` object targeting the `#system/rss` storage.
- Iterates through all stored RSS channels.
- Concatenates the keys of channels marked as default into a string.

**Usage Context:**
- Used to fetch default RSS channels for inclusion in site-wide syndication feeds.
- Typically called when generating a global RSS feed or determining which channels to display by default.

**Example:**
```php
$defaultChannels = rss_get_default();
// Outputs: "/news/sports/" if those channels are marked as default
echo $defaultChannels;
```

---

## `rss` Class

The `rss` class provides an interface for managing RSS feed channels, including creation, modification, deletion, and persistence.

### Properties

| Name   | Type   | Description                                      |
|--------|--------|--------------------------------------------------|
| `data` | object | Instance of the `data` class for storage access. |

---

### Constructor

#### `__construct()`
Initializes a new `rss` object with a `data` instance targeting the `#system/rss` storage.

**Parameters:**
None

**Return Values:**
None

**Inner Mechanisms:**
- Instantiates a `data` object to interact with the `#system/rss` storage.

**Usage Context:**
- Called when a new `rss` object is created to manage RSS channels.

**Example:**
```php
$rssManager = new \cms\rss();
```

---

### Methods

#### `add_channel($name, $description, $link, $image = NULL, $category = NULL, $default = NULL)`
Adds a new RSS channel to the system.

**Parameters:**

| Name          | Type    | Description                                      |
|---------------|---------|--------------------------------------------------|
| `$name`       | string  | The name/title of the RSS channel.               |
| `$description`| string  | A brief description of the channel.              |
| `$link`       | string  | The URL associated with the channel.             |
| `$image`      | string  | (Optional) URL of an image/logo for the channel. |
| `$category`   | string  | (Optional) Category or tag for the channel.      |
| `$default`    | bool    | (Optional) Whether the channel is default.       |

**Return Values:**
| Type   | Description                                      |
|--------|--------------------------------------------------|
| mixed  | The index/key of the newly inserted channel.     |

**Inner Mechanisms:**
- Buffers the channel data as an associative array.
- Uses the `data` object to insert the buffered data into storage.

**Usage Context:**
- Used when creating a new RSS channel, such as for a blog, news section, or podcast.

**Example:**
```php
$rssManager = new \cms\rss();
$channelId = $rssManager->add_channel(
    "Tech News",
    "Latest updates in technology",
    "https://example.com/tech-news",
    "https://example.com/images/tech-logo.png",
    "Technology",
    true
);
// $channelId now holds the key for the new channel
```

---

#### `set_channel($index, $name, $description, $link, $image = NULL, $category = NULL, $default = NULL)`
Updates an existing RSS channel.

**Parameters:**

| Name          | Type    | Description                                      |
|---------------|---------|--------------------------------------------------|
| `$index`      | mixed   | The key/index of the channel to update.          |
| `$name`       | string  | The updated name/title of the RSS channel.       |
| `$description`| string  | The updated description of the channel.          |
| `$link`       | string  | The updated URL associated with the channel.     |
| `$image`      | string  | (Optional) Updated URL of an image/logo.         |
| `$category`   | string  | (Optional) Updated category or tag.              |
| `$default`    | bool    | (Optional) Updated default status.               |

**Return Values:**
| Type   | Description                                      |
|--------|--------------------------------------------------|
| mixed  | The index/key of the updated channel.            |

**Inner Mechanisms:**
- Updates each field of the specified channel individually using the `data` object.

**Usage Context:**
- Used to modify existing RSS channels, such as updating a channel's description or URL.

**Example:**
```php
$rssManager = new \cms\rss();
$rssManager->set_channel(
    $channelId,
    "Updated Tech News",
    "Latest updates in technology and innovation",
    "https://example.com/updated-tech-news",
    "https://example.com/images/updated-tech-logo.png",
    "Technology, Innovation"
);
```

---

#### `del_channel($index)`
Deletes an RSS channel from the system.

**Parameters:**

| Name     | Type  | Description                              |
|----------|-------|------------------------------------------|
| `$index` | mixed | The key/index of the channel to delete.  |

**Return Values:**
| Type    | Description                                      |
|---------|--------------------------------------------------|
| bool    | `true` if deletion was successful, `false` otherwise. |

**Inner Mechanisms:**
- Uses the `data` object to delete the specified channel.

**Usage Context:**
- Used to remove obsolete or unused RSS channels.

**Example:**
```php
$rssManager = new \cms\rss();
$success = $rssManager->del_channel($channelId);
if ($success) {
    echo "Channel deleted successfully.";
}
```

---

#### `save()`
Persists all changes made to RSS channels to permanent storage.

**Parameters:**
None

**Return Values:**
| Type    | Description                                      |
|---------|--------------------------------------------------|
| bool    | `true` if the save operation was successful, `false` otherwise. |

**Inner Mechanisms:**
- Delegates the save operation to the `data` object.

**Usage Context:**
- Called after making multiple changes to ensure they are saved to disk.

**Example:**
```php
$rssManager = new \cms\rss();
$rssManager->add_channel("New Channel", "Description", "https://example.com");
$rssManager->set_channel($channelId, "Updated Name", "Updated Description", "https://example.com/updated");
$success = $rssManager->save();
if ($success) {
    echo "Changes saved successfully.";
}
```


<!-- HASH:f2285d93f7504374b112858fba5263bc -->
