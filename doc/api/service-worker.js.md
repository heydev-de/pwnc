# PWNC API Documentation

[← Index](README.md) | [`service-worker.js`](https://github.com/heydev-de/pwnc/blob/main/nuos/service-worker.js)

- **Version:** `26.5.30.4`
- **Website:** [pwnc.it](https://pwnc.it)
- **Repository:** [GitHub](https://github.com/heydev-de/pwnc)

---

## Service Worker (`service-worker.js`)

This file implements a minimal **service worker** for the PWNC Web Platform. Its primary role is to intercept network requests (`fetch` events) and handle them programmatically. Currently, it acts as a **pass-through proxy**, forwarding all requests to the network without modification. This establishes a foundation for future offline capabilities, caching strategies, or request/response manipulation.

---

### `fetch` Event Listener

#### **Purpose**
Intercepts all outgoing `fetch` requests initiated by the web application. The current implementation forwards each request to the network unchanged, but the structure allows for future enhancements such as:
- Caching responses for offline use
- Modifying request/response headers
- Serving fallback content when offline
- Implementing custom routing logic

#### **Parameters**
| Name    | Type               | Description                                                                 |
|---------|--------------------|-----------------------------------------------------------------------------|
| `event` | `FetchEvent`       | The fetch event object provided by the browser, containing the request data. |

#### **Return Values**
- **None** (The function uses `event.respondWith()` to provide a response asynchronously.)

#### **Inner Mechanisms**
1. The event listener is registered on the global `self` object (the service worker scope).
2. When a `fetch` event occurs, `event.respondWith()` is called to take control of the response.
3. The current implementation simply forwards the original request using `fetch(event.request)`, ensuring no functional change to the application's behavior.
4. The service worker runs in a separate thread and remains active even when the page is closed, enabling background processing.

#### **Usage Context**
- **When to Use**: This service worker is automatically registered by the PWNC platform during page load (if supported by the browser). No manual intervention is required for basic operation.
- **Typical Scenarios**:
  - **Future-Proofing**: Provides a hook for later implementing offline-first strategies.
  - **Performance Optimization**: Can be extended to cache static assets (e.g., CSS, JS, images) for faster repeat loads.
  - **Security**: Can intercept and modify requests/responses to enforce security policies (e.g., CSP headers).

---

#### **Usage Example**
**Extending the Service Worker to Cache Static Assets**
```javascript
const CACHE_NAME = "pwnc-static-v1";
const ASSETS_TO_CACHE = [
    "/styles/main.css",
    "/scripts/app.js",
    "/images/logo.png"
];

self.addEventListener("install", (event) => {
    event.waitUntil(
        caches.open(CACHE_NAME)
            .then((cache) => cache.addAll(ASSETS_TO_CACHE))
    );
});

self.addEventListener("fetch", (event) => {
    event.respondWith(
        caches.match(event.request)
            .then((cachedResponse) => {
                // Return cached response if found, otherwise fetch from network
                return cachedResponse || fetch(event.request);
            })
    );
});
```

**Explanation**:
1. The `install` event pre-caches specified static assets during service worker installation.
2. The `fetch` event listener checks the cache first for a matching request. If found, it serves the cached response; otherwise, it falls back to the network.
3. This reduces network dependency for static assets, improving load times and offline resilience.


<!-- HASH:e79ec2bd5ff762c37eaf8ca9e468a3f7 -->
