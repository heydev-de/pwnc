# PWNC API Documentation

[← Index](../README.md) | [`#system/lib.category.inc`](https://github.com/heydev-de/pwnc/blob/main/nuos/%23system/lib.category.inc)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Category Class

The `category` class implements a Bayesian text classification system for categorizing content (e.g., spam detection). It tokenizes text into trigrams, trains the classifier with labeled examples, and evaluates new text against learned patterns.

### Constants

| Name | Value/Default | Description |
|------|---------------|-------------|
| `CMS_CATEGORY_CLASS_SPAM` | `"#spam"` | Default class identifier for spam content. |
| `CMS_CATEGORY_LEARNING_THRESHOLD` | `25` | Minimum number of positive/negative samples required for evaluation. |
| `CMS_CATEGORY_TOKEN_LIMIT` | `15` | Maximum number of tokens used for evaluation. |
| `CMS_CATEGORY_DEVIATION_THRESHOLD` | `10` | Minimum deviation from 50% to consider a token significant. |
| `CMS_CATEGORY_TRAINING_THRESHOLD` | `10` | Minimum combined count (yes + no) for a token to be retained. |
| `CMS_CATEGORY_PROBABILITY_DEFAULT` | `40` | Default probability assigned to untrained tokens. |
| `CMS_DB_CATEGORY_META` | `CMS_DB_PREFIX . "category_meta"` | Table storing class metadata. |
| `CMS_DB_CATEGORY_META_INDEX` | `"id"` | Primary key for `category_meta`. |
| `CMS_DB_CATEGORY_META_CLASS` | `"class"` | Class name column. |
| `CMS_DB_CATEGORY_META_COUNT_YES` | `"count_yes"` | Count of valid samples for a class. |
| `CMS_DB_CATEGORY_META_COUNT_NO` | `"count_no"` | Count of invalid samples for a class. |
| `CMS_DB_CATEGORY` | `CMS_DB_PREFIX . "category"` | Table storing token statistics. |
| `CMS_DB_CATEGORY_INDEX` | `"id"` | Primary key for `category`. |
| `CMS_DB_CATEGORY_TOKEN` | `"token"` | Trigram token (3-character UTF-8 string). |
| `CMS_DB_CATEGORY_CLASS` | `"class"` | Foreign key to `category_meta`. |
| `CMS_DB_CATEGORY_COUNT_YES` | `"count_yes"` | Count of valid samples for a token. |
| `CMS_DB_CATEGORY_COUNT_NO` | `"count_no"` | Count of invalid samples for a token. |
| `CMS_DB_CATEGORY_PROBABILITY_YES` | `"probability_yes"` | Relative probability of token being valid (0-100). |
| `CMS_DB_CATEGORY_PROBABILITY_NO` | `"probability_no"` | Relative probability of token being invalid (0-100). |
| `CMS_DB_CATEGORY_RATING` | `"rating"` | Bayesian rating (0-100) for the token. |

---

### `category_tokenize_text($text)`

#### Purpose
Converts input text into an array of trigram tokens (3-character sequences) for classification. Handles UTF-8 multibyte characters.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Input text to tokenize. |

#### Return Values
- `array`: List of unique trigram tokens.

#### Inner Mechanisms
1. Uses `tokenize_text()` to split text into words.
2. Iterates through characters, grouping them into 3-character sequences (trigrams).
3. Skips incomplete trigrams at the end of the text.
4. Returns unique tokens as array keys.

#### Usage Example
```php
$tokens = category_tokenize_text("Hello world!");
// Returns: ["Hel", "ell", "llo", "lo ", "o w", " wo", "wor", "orl", "rld", "ld!"]
```

---

### `category::__construct()`

#### Purpose
Initializes the classifier by verifying database tables and creating them if necessary.

#### Parameters
None.

#### Return Values
None (sets `$this->enabled` to `TRUE` if tables exist or are created successfully).

#### Inner Mechanisms
1. Uses `mysql->verify_table()` to check/create:
   - `category_meta`: Stores class metadata (e.g., spam/ham counts).
   - `category`: Stores token statistics (e.g., trigram counts and probabilities).
2. Sets `$this->enabled` based on success.

#### Usage Example
```php
$classifier = new category();
// Tables are created if they don't exist.
```

---

### `category::train($text, $class = "", $valid = TRUE, $undo = FALSE)`

#### Purpose
Trains the classifier by adjusting token counts for a given class (e.g., spam/ham).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Text to train on. |
| `$class` | `string` | Class identifier (e.g., `"#spam"`). Default: `""`. |
| `$valid` | `bool` | Whether the text is valid (`TRUE`) or invalid (`FALSE`). Default: `TRUE`. |
| `$undo` | `bool` | If `TRUE`, decrements counts (undoes training). Default: `FALSE`. |

#### Return Values
- `bool`: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. Tokenizes the text into trigrams.
2. Inserts the class into `category_meta` if it doesn’t exist.
3. Retrieves the class index.
4. Updates counts in `category_meta` and `category` tables:
   - Increments/decrements `count_yes` or `count_no` based on `$valid` and `$undo`.
   - Adds new token-class combinations if they don’t exist.

#### Usage Example
```php
$classifier->train("Buy cheap pills now!", "#spam", FALSE);
// Marks the text as invalid (spam) for the "#spam" class.
```

---

### `category::update()`

#### Purpose
Recalculates token probabilities and removes insignificant tokens.

#### Parameters
None.

#### Return Values
- `bool`: `TRUE` on success, `FALSE` on failure.

#### Inner Mechanisms
1. Updates `probability_yes` and `probability_no` in `category`:
   - Computes relative probabilities (0-100) for each token.
2. Applies Bayesian formula to calculate `rating` (0-100).
3. Deletes tokens with:
   - Combined count (`count_yes + count_no`) > `CMS_CATEGORY_TRAINING_THRESHOLD`.
   - Ratings within `±CMS_CATEGORY_DEVIATION_THRESHOLD` of 50% (insignificant).

#### Usage Example
```php
$classifier->update();
// Recalculates all token probabilities after training.
```

---

### `category::evaluate($text, $class = "")`

#### Purpose
Evaluates text against a trained class and returns a probability score (0-100).

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `$text` | `string` | Text to evaluate. |
| `$class` | `string` | Class identifier (e.g., `"#spam"`). Default: `""`. |

#### Return Values
- `int|bool`: Probability score (0-100) or `FALSE` on error. Returns `CMS_CATEGORY_PROBABILITY_DEFAULT` if the class is untrained.

#### Inner Mechanisms
1. Tokenizes the text into trigrams.
2. Checks if the class has sufficient training data (`count_yes` and `count_no` > `CMS_CATEGORY_LEARNING_THRESHOLD`).
3. Retrieves the most significant tokens (highest deviation from 50%).
4. Computes the average rating across tokens, defaulting to `CMS_CATEGORY_PROBABILITY_DEFAULT` for untrained tokens.

#### Usage Example
```php
$score = $classifier->evaluate("Win a free iPhone!", "#spam");
// Returns: 85 (high probability of spam).
```

---

### Convenience Methods

| Method | Purpose | Example |
|--------|---------|---------|
| `train_valid($text, $class)` | Trains text as valid for a class. | `$classifier->train_valid("Legitimate email", "ham");` |
| `undo_valid($text, $class)` | Undoes training for valid text. | `$classifier->undo_valid("Legitimate email", "ham");` |
| `train_invalid($text, $class)` | Trains text as invalid for a class. | `$classifier->train_invalid("Scam email", "spam");` |
| `undo_invalid($text, $class)` | Undoes training for invalid text. | `$classifier->undo_invalid("Scam email", "spam");` |
| `train_spam($text)` | Trains text as spam. | `$classifier->train_spam("Free lottery!");` |
| `undo_spam($text)` | Undoes spam training. | `$classifier->undo_spam("Free lottery!");` |
| `train_nospam($text)` | Trains text as non-spam. | `$classifier->train_nospam("Meeting notes");` |
| `undo_nospam($text)` | Undoes non-spam training. | `$classifier->undo_nospam("Meeting notes");` |
| `evaluate_spam($text)` | Evaluates text for spam. | `$score = $classifier->evaluate_spam("Click here!");` |

#### Usage Example
```php
// Train and evaluate spam:
$classifier->train_spam("Get rich quick!");
$classifier->train_nospam("Project update");
$classifier->update();
$score = $classifier->evaluate_spam("Limited time offer!");
// $score: 90 (likely spam).
```


<!-- HASH:fe12cb59b47d865e2951bdbfe2502578 -->
