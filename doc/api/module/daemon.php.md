# PWNC API Documentation

[← Index](../README.md) | [`module/daemon.php`](https://github.com/heydev-de/pwnc/blob/main/nuos/module/daemon.php)

- **Version:** `26.6.3.3`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Daemon Module (`module/daemon.php`)

The **Daemon Module** is a background task processor for the PWNC Web Platform. It manages and executes deferred or scheduled tasks stored as files in a designated directory (`CMS_DATA_PATH . "#daemon/"`). The module ensures tasks are processed sequentially based on their modification time (oldest first) and handles locking to prevent concurrent execution.

### Core Responsibilities
- **Task Discovery**: Scans the daemon directory for valid task files.
- **Task Prioritization**: Orders tasks by modification time (FIFO).
- **Concurrency Control**: Uses file locking to prevent overlapping daemon runs.
- **Task Execution**: Loads and executes each task script in isolation.
- **Cleanup**: Removes or truncates completed tasks and releases resources.
- **Status Reporting**: Logs daemon activity via `cms_daemon_status()`.

---

### Key Mechanisms

#### 1. **File-Based Task Queue**
- Tasks are stored as files in `CMS_DATA_PATH . "#daemon/"`.
- Filenames act as unique task identifiers.
- File modification time (`filemtime`) determines execution order.
- Empty files or files with `.tmp` extension are ignored.

#### 2. **Advisory Locking**
- A lock file (`daemon.lock`) ensures only one daemon instance runs at a time.
- Lock is acquired via `flock($hfile, LOCK_EX | LOCK_NB)` (non-blocking).
- Lock is released on shutdown or script termination.

#### 3. **Task Execution Flow**
1. **Discovery**: Scan directory for valid task files.
2. **Prioritization**: Sort tasks by modification time (ascending).
3. **Execution**: Load each task script via `require()`.
4. **Cleanup**: Remove or truncate the task file post-execution.
5. **Shutdown**: Release lock and log completion.

#### 4. **Error Handling**
- Uncaught exceptions in tasks are forwarded to the platform’s error handler.
- OPCache invalidation ensures fresh task script execution.

#### 5. **Resource Management**
- `set_time_limit(600)` resets the execution timeout per task.
- `gc_collect_cycles()` triggers garbage collection to free memory.

---

### Constants and Variables

| Name       | Value/Default               | Description                                                                 |
|------------|-----------------------------|-----------------------------------------------------------------------------|
| `$path`    | `CMS_DATA_PATH . "#daemon/"`| Absolute path to the daemon task directory.                                 |
| `$lock`    | `$path . "daemon.lock"`     | Path to the lock file.                                                      |
| `$flag`    | `$path . "daemon.flag"`     | Path to the "task available" flag file (removed on daemon start).           |
| `$list`    | `scandir($path)`            | Raw list of files in the daemon directory.                                  |
| `$task`    | `[]`                        | Associative array of tasks (`filename => modification_time`).              |

---

### Usage Context

#### When to Use
- **Deferred Processing**: Offload non-critical operations (e.g., sending emails, generating reports).
- **Scheduled Tasks**: Execute tasks at a later time (e.g., cleanup, backups).
- **High-Latency Operations**: Avoid blocking user requests (e.g., image processing, API calls).

#### How to Enqueue a Task
1. **Create a Task File**:
   Write a PHP script to `$path` with a unique filename (e.g., `send_email_123.php`).
   ```php
   <?php
   // Example: Send an email
   mail("user@example.com", "Subject", "Message body");
   ```
2. **Trigger the Daemon**:
   The daemon runs automatically when the next request checks for pending tasks (via `daemon.flag`).

---

### Example: Enqueuing a Task

#### Scenario
Send a welcome email to a new user without blocking the registration response.

#### Code
```php
// In your registration handler (e.g., module/user/register.php)
$taskPath = CMS_DATA_PATH . "#daemon/send_welcome_email_" . $userId . ".php";
$taskContent = <<<PHP
<?php
mail(
    "{$userEmail}",
    "Welcome to PWNC!",
    "Thank you for registering, {$userName}!"
);
PHP;
file_put_contents($taskPath, $taskContent);

// Notify the daemon (optional, if not using daemon.flag)
touch(CMS_DATA_PATH . "#daemon/daemon.flag");
```

#### Explanation
1. **Task Creation**: A file named `send_welcome_email_123.php` is created in the daemon directory.
2. **Content**: The file contains the PHP code to send the email.
3. **Execution**: The daemon processes the task in the background, ordered by its creation time.

---

### Error Handling and Edge Cases

| Scenario                     | Behavior                                                                 |
|------------------------------|--------------------------------------------------------------------------|
| Lock file exists             | Daemon exits immediately (another instance is running).                 |
| No tasks available           | Daemon exits without processing.                                         |
| Task file is empty           | Task is skipped.                                                         |
| Task throws an exception     | Exception is forwarded to the platform’s error handler.                  |
| Task exceeds time limit      | `set_time_limit(600)` resets the timer; task continues.                  |
| Daemon directory is missing  | `mkpath($path)` creates it; daemon exits if creation fails.              |

---

### Integration with PWNC Utilities

| Utility               | Role in Daemon Module                                                                 |
|-----------------------|---------------------------------------------------------------------------------------|
| `cms_daemon_status()` | Logs daemon activity (e.g., "Daemon run started. Processing 5 tasks.").               |
| `mkpath()`            | Ensures the daemon directory exists.                                                  |
| `opcache_invalidate()`| Clears OPCache for task scripts to ensure fresh execution.                            |
| `gc_collect_cycles()` | Frees memory after task execution.                                                    |


<!-- HASH:8e772e8738f19e4e6b5050fe927dd49e -->
