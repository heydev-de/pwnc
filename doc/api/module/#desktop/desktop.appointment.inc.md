# PWNC API Documentation

[← Index](../../README.md) | [`module/#desktop/desktop.appointment.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/%23desktop/desktop.appointment.inc)

- **Version:** `26.6.9.0`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Desktop Appointment Module

**Overview**
This file implements the appointment management interface for the PWNC Web Platform's desktop environment. It handles creation, modification, deletion, and display of calendar appointments through an interactive UI. The module processes interface messages (`CMS_IFC_MESSAGE`) to manipulate appointment data and renders a multi-level calendar view (year, month, day, hour) with appointment listings.

---

### Message Handling

The module responds to the following interface messages:

| Message | Purpose | Parameters |
|---------|---------|------------|
| `select` | Selects an existing appointment for editing | `$ifc_param`: Appointment object ID |
| `time` | Sets the current view time | `$ifc_param`: Unix timestamp |
| `add` | Creates a new appointment | `$ifc_param`: Unix timestamp (start time) |
| `save` | Saves changes to an appointment | `$ifc_param1`: Name<br>`$ifc_param2`: Time (string)<br>`$ifc_param3`: Expire flag<br>`$ifc_param4`: Content<br>`$ifc_param5`: Place<br>`$ifc_param6`: Participants |
| `delete` | Deletes an appointment | None (uses `$object`) |

**Inner Mechanisms**
- Uses `$desktop->data` (a structured data store) to manage appointments.
- Appointments are stored as objects with `#subtype = "appointment"`.
- Time normalization ensures all timestamps are aligned to 15-minute intervals (`$time % 900`).
- Expiry is set to 24 hours after the appointment time if enabled.

**Usage Example**
```php
// Add a new appointment for tomorrow at 14:00
$tomorrow = strtotime("+1 day 14:00");
ifc_post("add", $tomorrow); // Triggers the "add" message
```

---

### Main Display Logic

#### Time Normalization
Normalizes the current view time to the nearest 15-minute interval and adjusts for edge cases (e.g., hours < 6 are treated as the previous day).

**Key Variables**
| Variable | Type | Description |
|----------|------|-------------|
| `$time_current` | `int` | Current Unix timestamp |
| `$time` | `int` | Normalized view timestamp (15-minute aligned) |
| `$hour`, `$minute`, `$day`, `$month`, `$year` | `int` | Extracted from `$time` |
| `$days_per_month` | `int` | Days in the current month |

---

#### Appointment Listing
Scans all desktop objects to find appointments matching the current day/month/year. Appointments are grouped by hour and minute in `$list`.

**Inner Mechanisms**
- Skips non-appointment objects using `nstreq($desktop->object_type($key), "appointment")`.
- Adjusts timestamps for appointments spanning midnight (e.g., 01:00 becomes 25:00 of the previous day).
- Counts appointments per day/month/year in `$count` for summary displays.

---

#### Calendar Rendering
Generates HTML tables for year, month, day, and hour views. Each level highlights the current selection and shows appointment counts.

**Buffer Arrays**
| Array | Purpose |
|-------|---------|
| `$buffer_year` | Year-level navigation (current year in bold) |
| `$buffer_month` | Month-level navigation (current month in bold) |
| `$buffer_day` | Day-of-week headers (e.g., "Mo", "Tu") |
| `$_buffer_day` | Day-level navigation (current day in bold) |
| `$buffer_hour` | Hour-by-hour grid with appointment links |

**Helper Function**
```php
$sum_recursive = function(&$value) { ... };
```
Sums all values in a nested array (e.g., `$count[$year][$month][$day]`).

**Usage Example**
```php
// Render a day view for March 15, 2023
$time = mktime(0, 0, 0, 3, 15, 2023);
ifc_post("time", $time); // Updates the view
```

---

#### Appointment Editor
Displays a form for editing the selected appointment (`$object`). Uses the `ifc` class to render input fields.

**Fields**
| Field | Label | Type | Data Source |
|-------|-------|------|-------------|
| Name | `CMS_L_DESKTOP_APPOINTMENT_005` | Text (40 chars) | `$desktop->object_get($object, "name")` |
| Time | `CMS_L_DESKTOP_APPOINTMENT_007` | Datetime | `$time` |
| Expire | `CMS_L_DESKTOP_APPOINTMENT_011` | Checkbox | `#expire` flag |
| Content | `CMS_L_DESKTOP_APPOINTMENT_012` | Textarea (40x8) | `content` field |
| Place | `CMS_L_DESKTOP_APPOINTMENT_006` | Textarea (40x4) | `place` field |
| Participants | `CMS_L_DESKTOP_APPOINTMENT_013` | Textarea (40x4) | `participant` field |

**Inner Mechanisms**
- Time strings (e.g., "2023-03-15 14:00") are parsed using `strtotime()`.
- The `#expire` flag is set to `NULL` if unchecked, or to `time + 86400` if checked.

---

### UI Integration

#### Menu
The module dynamically generates a context menu:
```php
$menu = stre($object) ? NULL :
    [CMS_L_COMMAND_SAVE . "|desktop/command_save" => "save",
     CMS_L_COMMAND_DELETE . "|desktop/command_delete" => "#delete"];
```
- Hidden if no appointment is selected (`stre($object)`).
- Includes "Save" and "Delete" commands.

#### IFC Initialization
```php
$ifc = new ifc(
    NULL,
    $ifc_page,
    $menu,
    array_merge($param, ["object" => $object, "time" => $time]),
    NULL,
    CMS_L_DESKTOP_APPOINTMENT_002
);
```
- Passes the current `$object` and `$time` to the interface context.

---

### Helper Functions Used

| Function | Purpose |
|----------|---------|
| `ifc_post()` | Sends interface messages (e.g., `select`, `add`) |
| `ifc_table_open()` / `ifc_table_close()` | Wraps HTML tables for consistent styling |
| `ifc_varied()` | Applies alternating row styles |
| `month()` / `weekday()` | Localized month/day names |
| `friendly_date()` | Formats timestamps for display (e.g., "Today, 14:00") |
| `image()` | Renders icons (e.g., `desktop/icon_appointment`) |
| `qx()` | Escapes strings for JavaScript (XML-safe) |
| `x()` | Escapes strings for HTML |

---

### Typical Workflow

1. **Navigation**
   - User clicks a day/month/year link → `ifc_post("time", $timestamp)` updates the view.
2. **Appointment Creation**
   - User clicks the "+" icon → `ifc_post("add", $timestamp)` creates a new appointment.
3. **Editing**
   - User selects an appointment → `ifc_post("select", $object_id)` loads it into the editor.
   - Changes are saved via `ifc_post("save", ...)`.
4. **Deletion**
   - User clicks "Delete" → `ifc_post("delete")` removes the appointment.


<!-- HASH:1cd6ba8966f0b34f583457e9c6fb4102 -->
