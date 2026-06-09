# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/image.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/image.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Image Handling Module (`image.inc`)

This module provides comprehensive image handling capabilities for the PWNC Web Platform, including:
- **Image output generation** with responsive features (srcset, deferred loading)
- **Image processing** (scaling, format conversion, caching)
- **Image existence and type verification**
- **Image dimension retrieval** with SVG support
- **Quality optimization** based on image resolution

The module follows PWNC's zero-dependency approach, using GD library when available and falling back to CGI binaries when necessary.

---

## Functions

### `image()`

Generates HTML `<img>` tag with responsive image support and optional deferred loading.

#### Parameters

| Name      | Type    | Default | Description                                                                 |
|-----------|---------|---------|-----------------------------------------------------------------------------|
| `$url`    | string  | -       | Image URL (local path or remote URL)                                       |
| `$width`  | int     | NULL    | Target width in pixels (0 = auto)                                          |
| `$height` | int     | NULL    | Target height in pixels (0 = auto)                                         |
| `$alt`    | string  | NULL    | Alternative text for accessibility                                         |
| `$style`  | string  | NULL    | Inline CSS styles                                                          |
| `$defer`  | bool    | FALSE   | Enable deferred loading (lazy loading)                                     |
| `$preview`| bool    | TRUE    | Enable preview generation during processing                                |

#### Return Value
- **string**: Complete `<img>` HTML tag with responsive attributes

#### Inner Mechanisms
1. Processes image through `image_process()` to handle scaling/caching
2. Generates srcset with multiple resolutions for responsive images
3. Creates both noscript fallback and deferred loading versions when `$defer=TRUE`
4. Maintains aspect ratio when only one dimension is specified
5. Uses SVG placeholder for deferred loading

#### Usage Example
```php
// Basic responsive image
echo image('content/uploads/photo.jpg', 800, 0, 'Company logo');

// Deferred loading with custom style
echo image('content/uploads/hero.jpg', 1200, 600, 'Hero image',
           'border-radius: 8px; box-shadow: 0 4px 8px rgba(0,0,0,0.1);',
           TRUE);
```

---

### `image_exists()`

Checks if an image file exists in the local images directory.

#### Parameters

| Name    | Type   | Description                          |
|---------|--------|--------------------------------------|
| `$value`| string | Image filename without extension     |

#### Return Value
- **bool**: TRUE if image exists in any supported format, FALSE otherwise

#### Inner Mechanisms
1. Checks for existence of `.webp`, `.jpg`, `.png`, `.gif`, or `.svg` versions
2. Looks in `CMS_IMAGES_PATH` directory

#### Usage Example
```php
if (image_exists('profile/user123')) {
    echo image('profile/user123', 200, 200, 'User avatar');
} else {
    echo image('default/avatar', 200, 200, 'Default avatar');
}
```

---

### `image_quality()`

Calculates optimal image quality based on resolution using quadratic Bézier interpolation.

#### Parameters

| Name        | Type   | Description                          |
|-------------|--------|--------------------------------------|
| `$width`    | int    | Image width in pixels                |
| `$height`   | int    | Image height in pixels               |
| `$extension`| string | Image format ('webp' or 'jpg')       |

#### Return Value
- **int**: Quality value between 0-100

#### Inner Mechanisms
1. Uses predefined quality ranges for each format
2. Calculates image diagonal as resolution metric
3. Applies quadratic Bézier interpolation between min/mid/max quality points
4. Returns rounded integer quality value

#### Usage Example
```php
$quality = image_quality(1920, 1080, 'webp');
// Returns 95 for high-resolution webp images
```

---

### `image_process()`

Processes images with scaling, format conversion, and caching.

#### Parameters

| Name                  | Type    | Default | Description                                                                 |
|-----------------------|---------|---------|-----------------------------------------------------------------------------|
| `$url`                | string  | -       | Source image URL                                                           |
| `$width`              | int     | NULL    | Target width (0 = original)                                                |
| `$height`             | int     | NULL    | Target height (0 = original)                                               |
| `$preference_override`| string  | NULL    | Force specific output format ('webp', 'jpg', 'png')                        |
| `$ignore_cache`       | bool    | FALSE   | Bypass cache and regenerate image                                          |
| `$preview`            | bool    | TRUE    | Generate preview during processing                                         |

#### Return Value
- **string**: URL of processed image

#### Inner Mechanisms
1. **Format Handling**:
   - Supports webp, jpg, png, gif, svg
   - Uses system preference or `$preference_override`
   - Falls back to other formats when preferred format isn't supported

2. **Caching System**:
   - Creates hash-based directory structure
   - Stores processed images in `CMS_DATA_PATH/image/cache/`
   - Checks cache expiration based on source file modification time

3. **Processing Pipeline**:
   - Downloads remote images to local cache
   - Uses GD library when available
   - Falls back to CGI binaries for JPEG processing
   - Handles transparency and alpha channels
   - Generates previews for background processing

4. **Resolution Handling**:
   - Maintains aspect ratio
   - Respects system-wide resolution limits
   - Generates multiple resolutions for responsive images

#### Usage Example
```php
// Process and cache a remote image
$processed_url = image_process(
    'https://example.com/photo.jpg',
    800,
    600,
    'webp',
    FALSE,
    TRUE
);

// Force regeneration of cached image
$fresh_url = image_process(
    'content/uploads/photo.jpg',
    800,
    600,
    NULL,
    TRUE
);
```

---

### `image_path()`

Converts image URL to local filesystem path.

#### Parameters

| Name   | Type   | Description          |
|--------|--------|----------------------|
| `$url` | string | Image URL            |

#### Return Value
- **string|FALSE**: Local filesystem path or FALSE if not found

#### Inner Mechanisms
1. Checks if URL starts with `CMS_ROOT_URL`
2. Converts URL to filesystem path
3. Verifies file existence

#### Usage Example
```php
$path = image_path(CMS_ROOT_URL . 'content/uploads/photo.jpg');
if ($path !== FALSE) {
    $filesize = filesize($path);
    echo "Image size: $filesize bytes";
}
```

---

### `image_type()`

Determines image type from file.

#### Parameters

| Name   | Type   | Description          |
|--------|--------|----------------------|
| `$path`| string | Filesystem path      |

#### Return Value
- **string|NULL**: Image type ('gif', 'jpg', 'png', 'webp', 'svg') or NULL if unknown

#### Inner Mechanisms
1. Checks file extension for SVG
2. Uses `exif_imagetype()` when available
3. Falls back to `getimagesize()`

#### Usage Example
```php
$type = image_type(CMS_IMAGES_PATH . 'logo.png');
if ($type === 'png') {
    // Handle PNG-specific processing
}
```

---

### `getimagesize()`

Enhanced version of PHP's `getimagesize()` with SVG support and preview handling.

#### Parameters

| Name        | Type    | Description                          |
|-------------|---------|--------------------------------------|
| `$filename` | string  | Image file path or URL               |
| `$imageinfo`| array   | Reference for additional info        |
| `$preview`  | mixed   | Preview size cache key               |

#### Return Value
- **array**: Image dimensions [width, height]

#### Inner Mechanisms
1. **Preview Handling**:
   - Checks cache for preview dimensions
   - Returns cached values when available

2. **SVG Support**:
   - Parses SVG files using SimpleXML
   - Extracts width/height attributes
   - Falls back to viewBox dimensions
   - Returns default [300, 250] for invalid SVGs

3. **Fallback**:
   - Uses native `getimagesize()` for raster images
   - Returns [300, 250] for invalid images

#### Usage Example
```php
list($width, $height) = getimagesize('content/uploads/logo.svg');
echo "SVG dimensions: {$width}x{$height}px";

// With additional info
$size = getimagesize('photo.jpg', $info);
echo "Image type: {$info['mime']}";
```


<!-- HASH:b66551351165c732d3b26c7929498ea3 -->
