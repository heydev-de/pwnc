# PWNC API Documentation

[← Index](../../README.md) | [`#system/common/date.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/common/date.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Date Utilities (`date.inc`)

Core date-handling utilities for the PWNC Web Platform. This file provides localized, human-readable representations of weekdays, months, and timestamps. All functions are multibyte-safe and locale-aware via CMS language constants.

---

### Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_L_SUNDAY` – `CMS_L_SATURDAY` | Language-specific full weekday names | Full localized weekday names. |
| `CMS_L_SUNDAY_ABBR` – `CMS_L_SATURDAY_ABBR` | Language-specific abbreviated weekday names | Abbreviated localized weekday names (e.g., "Sun"). |
| `CMS_L_JANUARY` – `CMS_L_DECEMBER` | Language-specific full month names | Full localized month names. |
| `CMS_L_JANUARY_ABBR` – `CMS_L_DECEMBER_ABBR` | Language-specific abbreviated month names | Abbreviated localized month names (e.g., "Jan"). |
| `CMS_L_SHORT_TIME_FORMAT` | `"H:i"` | Default time format for short display (hours:minutes). |
| `CMS_L_DATE_FORMAT` | `"d.m.Y"` | Default date format for full display (day.month.year). |
| `CMS_L_COMMON_002` – `CMS_L_COMMON_027` | Language-specific date/time phrases | Localized phrases for relative time intervals (e.g., "5 minutes ago"). |

---

### `weekday`

#### Purpose
Returns the localized name of a weekday (0–6) in either full or abbreviated form.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$number` | `int` | — | Weekday number (0 = Sunday, 6 = Saturday). |
| `$abbr` | `bool` | `TRUE` | If `TRUE`, returns abbreviated name; otherwise, full name. |

#### Return Values
| Type | Description |
|------|-------------|
| `string` | Localized weekday name. |
| `FALSE` | Invalid weekday number. |

#### Inner Mechanisms
- Normalizes `$number` modulo 7 to handle overflow.
- Uses a `switch` statement to map numbers to language constants.
- Falls back to `FALSE` for invalid inputs.

#### Usage Context
- Displaying calendar headers.
- Formatting timestamps in logs or user interfaces.

#### Example
```php
echo weekday(0);       // "Sun" (if locale is English)
echo weekday(1, FALSE); // "Monday"
```

---

### `month`

#### Purpose
Returns the localized name of a month (1–12) in either full or abbreviated form.

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$number` | `int` | — | Month number (1 = January, 12 = December). |
| `$abbr` | `bool` | `TRUE` | If `TRUE`, returns abbreviated name; otherwise, full name. |

#### Return Values
| Type | Description |
|------|-------------|
| `string` | Localized month name. |
| `FALSE` | Invalid month number. |

#### Inner Mechanisms
- Uses a `switch` statement to map numbers to language constants.
- Falls back to `FALSE` for invalid inputs.

#### Usage Context
- Date pickers.
- Event listings.

#### Example
```php
echo month(1);       // "Jan" (if locale is English)
echo month(2, FALSE); // "February"
```

---

### `friendly_date`

#### Purpose
Converts a Unix timestamp into a human-readable, localized relative date string (e.g., "2 hours ago" or "Yesterday at 14:30").

#### Parameters

| Name | Type | Default | Description |
|------|------|---------|-------------|
| `$time` | `int\|NULL` | `NULL` | Unix timestamp. If `NULL`, uses current time. |
| `$date_only` | `bool` | `FALSE` | If `TRUE`, omits time and returns only date phrases (e.g., "Today"). |

#### Return Values
| Type | Description |
|------|-------------|
| `string` | Localized relative date string. |

#### Inner Mechanisms
1. **Time Normalization**: Uses `time()` if `$time` is `NULL`.
2. **Interval Calculation**: Computes seconds, minutes, hours, days, and weeks between `$time` and now.
3. **Contextual Phrasing**:
   - **0 seconds**: "Just now".
   - **<1 minute**: "X seconds ago" / "in X seconds".
   - **<1 hour**: "X minutes ago" / "in X minutes".
   - **<6 hours**: "X hours and Y minutes ago" / "in X hours and Y minutes".
   - **Today**: "Today at HH:MM" (or "Today" if `$date_only`).
   - **Yesterday/Tomorrow**: "Yesterday at HH:MM" (or "Yesterday" if `$date_only`).
   - **This week**: "Weekday at HH:MM" (or "Weekday" if `$date_only`).
   - **This year**: "Weekday, X days ago" or "Day Month at HH:MM".
   - **Other years**: Full date (e.g., "01.01.2020 at 12:00").

#### Usage Context
- Activity feeds.
- Comment timestamps.
- Event reminders.

#### Example
```php
echo friendly_date(time() - 3600); // "1 hour ago"
echo friendly_date(time() + 86400, TRUE); // "Tomorrow"
```


<!-- HASH:d3c2febffe3da7bf20b12241db968055 -->
