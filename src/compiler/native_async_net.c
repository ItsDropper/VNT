/*
 * Copyright (c) 2026 ItsDropper
 *
 * This file is part of VNT and is licensed under the MIT License.
 * See the LICENSE file in the project root for the full license text.
 *
 * Bounded asynchronous HTTP request queue with active transport cancellation.
 * Responses are still buffered under the existing 64 MiB cap; caller-consumed
 * streaming with backpressure is a separate capability not implemented here.
 */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <winhttp.h>
#else
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/socket.h>
#endif

#define VNT_NET_ASYNC_WORKERS 4
#define VNT_NET_ASYNC_CAPACITY 64
#define VNT_NET_STREAM_CAPACITY (64u * 1024u)

typedef int (*VntAsyncStreamSink)(const unsigned char *, size_t, void *);
extern char *vnt_native_http_request_async(const char *, const char *,
                                          const char *, const char *,
                                          int *, size_t *);
extern char *vnt_native_http_request_stream_async(const char *, const char *,
                                          const char *, const char *,
                                          VntAsyncStreamSink, void *,
                                          int *, size_t *);

enum { TASK_FREE, TASK_QUEUED, TASK_RUNNING, TASK_DONE, TASK_CANCELLED };

typedef struct {
    int id;
    int state;
    char *method;
    char *url;
    char *headers;
    char *body;
    char *response;
    size_t response_length;
    int status;
    intptr_t active_transport;
    int worker_finished;
    int release_requested;
    int mode; /* 0=buffered request, 1=caller-consumed stream */
    unsigned char stream_buffer[VNT_NET_STREAM_CAPACITY];
    size_t stream_head;
    size_t stream_count;
} VntAsyncTask;

static VntAsyncTask tasks[VNT_NET_ASYNC_CAPACITY];
static int queue[VNT_NET_ASYNC_CAPACITY];
static unsigned queue_head, queue_tail, queue_count;
static int next_handle = 1;
static int worker_count;
#ifdef _WIN32
static __declspec(thread) int async_current_handle;
#else
static __thread int async_current_handle;
#endif

#ifdef _WIN32
static SRWLOCK async_lock = SRWLOCK_INIT;
static CONDITION_VARIABLE async_condition = CONDITION_VARIABLE_INIT;
static HANDLE async_workers[VNT_NET_ASYNC_WORKERS];
#define async_lock_acquire() AcquireSRWLockExclusive(&async_lock)
#define async_lock_release() ReleaseSRWLockExclusive(&async_lock)
#define async_wake_all() WakeAllConditionVariable(&async_condition)
static void async_wait_forever(void) {
    SleepConditionVariableSRW(&async_condition, &async_lock, INFINITE, 0);
}
#else
static pthread_mutex_t async_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t async_condition = PTHREAD_COND_INITIALIZER;
static pthread_t async_workers[VNT_NET_ASYNC_WORKERS];
#define async_lock_acquire() pthread_mutex_lock(&async_lock)
#define async_lock_release() pthread_mutex_unlock(&async_lock)
#define async_wake_all() pthread_cond_broadcast(&async_condition)
static void async_wait_forever(void) {
    pthread_cond_wait(&async_condition, &async_lock);
}
#endif

static char *async_strdup(const char *s) {
    size_t n;
    char *p;
    if (!s) s = "";
    n = strlen(s);
    p = (char *)malloc(n + 1);
    if (p) memcpy(p, s, n + 1);
    return p;
}

static void async_task_clear(VntAsyncTask *t) {
    free(t->method); free(t->url); free(t->headers); free(t->body);
    free(t->response);
    memset(t, 0, sizeof(*t));
    t->state = TASK_FREE;
}

static VntAsyncTask *async_find_locked(int id) {
    if (id <= 0) return NULL;
    for (unsigned i = 0; i < VNT_NET_ASYNC_CAPACITY; ++i)
        if (tasks[i].state != TASK_FREE && tasks[i].id == id)
            return &tasks[i];
    return NULL;
}

static int async_stream_push_task(VntAsyncTask *t,
                                  const unsigned char *data, size_t length) {
    while (length) {
        async_lock_acquire();
        while (t->state == TASK_RUNNING && t->stream_count == VNT_NET_STREAM_CAPACITY)
            async_wait_forever();
        if (t->state != TASK_RUNNING) {
            async_lock_release();
            return 0;
        }
        size_t free_space = VNT_NET_STREAM_CAPACITY - t->stream_count;
        size_t amount = length < free_space ? length : free_space;
        size_t tail = (t->stream_head + t->stream_count) % VNT_NET_STREAM_CAPACITY;
        size_t first = amount < VNT_NET_STREAM_CAPACITY - tail
            ? amount : VNT_NET_STREAM_CAPACITY - tail;
        memcpy(t->stream_buffer + tail, data, first);
        if (amount > first) memcpy(t->stream_buffer, data + first, amount - first);
        t->stream_count += amount;
        async_wake_all();
        async_lock_release();
        data += amount;
        length -= amount;
    }
    return 1;
}

static int async_stream_sink(const unsigned char *data, size_t length, void *context) {
    return async_stream_push_task((VntAsyncTask *)context, data, length);
}

#ifdef _WIN32
static DWORD WINAPI async_worker_main(LPVOID unused)
#else
static void *async_worker_main(void *unused)
#endif
{
    (void)unused;
    for (;;) {
        int slot;
        VntAsyncTask *task;
        char *response;
        int status = 0;
        size_t length = 0;

        async_lock_acquire();
        while (!queue_count) async_wait_forever();
        slot = queue[queue_head];
        queue_head = (queue_head + 1u) % VNT_NET_ASYNC_CAPACITY;
        --queue_count;
        task = &tasks[slot];
        if (task->state == TASK_CANCELLED) {
            async_task_clear(task);
            async_wake_all();
            async_lock_release();
            continue;
        }
        if (task->state != TASK_QUEUED) {
            async_lock_release();
            continue;
        }
        task->state = TASK_RUNNING;
        async_current_handle = task->id;
        async_lock_release();

        if (task->mode == 1) {
            response = vnt_native_http_request_stream_async(task->method, task->url,
                task->headers, task->body, async_stream_sink, task, &status, &length);
        } else {
            response = vnt_native_http_request_async(task->method, task->url,
                task->headers, task->body, &status, &length);
        }
        async_current_handle = 0;

        async_lock_acquire();
        /* The task cannot be released while running, so this slot is stable. */
        task->worker_finished = 1;
        if (task->state == TASK_RUNNING) {
            task->response = response;
            task->response_length = response ? length : 0;
            task->status = status;
            task->state = TASK_DONE;
        } else {
            free(response);
            if (task->release_requested) async_task_clear(task);
        }
        async_wake_all();
        async_lock_release();
    }
#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

static int async_ensure_workers_locked(void) {
    while (worker_count < VNT_NET_ASYNC_WORKERS) {
#ifdef _WIN32
        HANDLE h = CreateThread(NULL, 0, async_worker_main, NULL, 0, NULL);
        if (!h) return 0;
        async_workers[worker_count] = h;
#else
        if (pthread_create(&async_workers[worker_count], NULL,
                           async_worker_main, NULL) != 0) return 0;
        pthread_detach(async_workers[worker_count]);
#endif
        ++worker_count;
    }
    return 1;
}

static int async_start_impl(const char *method, const char *url,
                            const char *headers, const char *body, int mode) {
    char *m = async_strdup(method), *u = async_strdup(url);
    char *h = async_strdup(headers), *b = async_strdup(body);
    if (!m || !u || !h || !b) {
        free(m); free(u); free(h); free(b);
        return 0;
    }

    async_lock_acquire();
    if (!async_ensure_workers_locked() || queue_count == VNT_NET_ASYNC_CAPACITY) {
        async_lock_release();
        free(m); free(u); free(h); free(b);
        return 0;
    }
    int slot = -1;
    for (int i = 0; i < VNT_NET_ASYNC_CAPACITY; ++i)
        if (tasks[i].state == TASK_FREE) { slot = i; break; }
    if (slot < 0) {
        async_lock_release();
        free(m); free(u); free(h); free(b);
        return 0;
    }

    VntAsyncTask *t = &tasks[slot];
    memset(t, 0, sizeof(*t));
    int candidate;
    for (;;) {
        candidate = next_handle++;
        if (next_handle <= 0) next_handle = 1;
        if (candidate <= 0) continue;
        int duplicate = 0;
        for (int i = 0; i < VNT_NET_ASYNC_CAPACITY; ++i)
            if (tasks[i].state != TASK_FREE && tasks[i].id == candidate)
                duplicate = 1;
        if (!duplicate) break;
    }
    t->id = candidate;
    t->method = m; t->url = u; t->headers = h; t->body = b;
    t->mode = mode;
    t->state = TASK_QUEUED;
    queue[queue_tail] = slot;
    queue_tail = (queue_tail + 1u) % VNT_NET_ASYNC_CAPACITY;
    ++queue_count;
    int id = t->id;
    async_wake_all();
    async_lock_release();
    return id;
}

int vnt_native_http_async_start(const char *method, const char *url,
                                const char *headers, const char *body) {
    return async_start_impl(method, url, headers, body, 0);
}

int vnt_native_http_async_stream_start(const char *method, const char *url,
                                       const char *headers, const char *body) {
    return async_start_impl(method, url, headers, body, 1);
}

int vnt_native_http_async_poll(int id) {
    int result = -1;
    async_lock_acquire();
    VntAsyncTask *t = async_find_locked(id);
    if (t) result = t->state == TASK_DONE ? 1 :
                    (t->state == TASK_CANCELLED ? -1 : 0);
    async_lock_release();
    return result;
}

int vnt_native_http_async_wait(int id, unsigned int timeout_ms) {
    int result = -1;
    async_lock_acquire();
    VntAsyncTask *t = async_find_locked(id);
    if (!t) { async_lock_release(); return -1; }
    if (timeout_ms == 0) {
        result = t->state == TASK_DONE ? 1 :
                 (t->state == TASK_CANCELLED ? -1 : 0);
        async_lock_release();
        return result;
    }
#ifdef _WIN32
    DWORD start = GetTickCount();
    while (t->state == TASK_QUEUED || t->state == TASK_RUNNING) {
        DWORD elapsed = GetTickCount() - start;
        if (elapsed >= timeout_ms) break;
        if (!SleepConditionVariableSRW(&async_condition, &async_lock,
                                       timeout_ms - elapsed, 0)) {
            if (GetLastError() == ERROR_TIMEOUT) break;
            break;
        }
    }
#else
    struct timespec deadline;
    clock_gettime(CLOCK_REALTIME, &deadline);
    deadline.tv_sec += timeout_ms / 1000u;
    deadline.tv_nsec += (long)(timeout_ms % 1000u) * 1000000L;
    if (deadline.tv_nsec >= 1000000000L) {
        deadline.tv_sec++;
        deadline.tv_nsec -= 1000000000L;
    }
    while (t->state == TASK_QUEUED || t->state == TASK_RUNNING) {
        int rc = pthread_cond_timedwait(&async_condition, &async_lock, &deadline);
        if (rc == ETIMEDOUT) break;
        if (rc != 0) break;
    }
#endif
    result = t->state == TASK_DONE ? 1 :
             (t->state == TASK_CANCELLED ? -1 : 0);
    async_lock_release();
    return result;
}

int vnt_native_http_async_cancel(int id) {
    int result = 0;
#ifdef _WIN32
    HINTERNET abort_request = NULL;
#endif
    async_lock_acquire();
    VntAsyncTask *t = async_find_locked(id);
    if (t && t->state == TASK_QUEUED) {
        t->state = TASK_CANCELLED;
        result = 1;
    } else if (t && t->state == TASK_RUNNING) {
        t->state = TASK_CANCELLED;
        result = 1;
        if (t->active_transport) {
#ifdef _WIN32
            abort_request = (HINTERNET)t->active_transport;
            t->active_transport = 0;
#else
            int fd = (int)(t->active_transport - 1);
            /* shutdown() interrupts a blocked socket read/write; worker owns close(). */
            shutdown(fd, SHUT_RDWR);
#endif
        }
    }
    async_wake_all();
    async_lock_release();
#ifdef _WIN32
    /* Do not hold the queue lock while aborting a synchronous WinHTTP request. */
    if (abort_request) WinHttpCloseHandle(abort_request);
#endif
    return result;
}

char *vnt_native_http_async_result(int id, size_t *length) {
    char *copy = NULL;
    if (length) *length = 0;
    async_lock_acquire();
    VntAsyncTask *t = async_find_locked(id);
    if (t && t->state == TASK_DONE && t->response) {
        copy = async_strdup(t->response);
        if (copy && length) *length = t->response_length;
    } else if (t && t->state == TASK_DONE && !t->response) {
        copy = async_strdup("");
    }
    async_lock_release();
    return copy;
}

int vnt_native_http_async_status(int id) {
    int status = 0;
    async_lock_acquire();
    VntAsyncTask *t = async_find_locked(id);
    if (t && t->state == TASK_DONE) status = t->status;
    async_lock_release();
    return status;
}

int vnt_native_http_async_release(int id) {
    int result = 0;
    async_lock_acquire();
    VntAsyncTask *t = async_find_locked(id);
    if (t && t->state == TASK_DONE &&
        (t->mode != 1 || t->stream_count == 0)) {
        async_task_clear(t);
        result = 1;
    } else if (t && t->state == TASK_CANCELLED) {
        t->release_requested = 1;
        if (t->worker_finished) async_task_clear(t);
        /* Queued cancellation is reclaimed when its queue entry is popped. */
        result = 1;
    }
    async_lock_release();
    return result;
}

/* Transport hooks used by native_net.c to interrupt active requests safely. */
int vnt_native_http_async_current_handle(void) {
    return async_current_handle;
}

void vnt_native_http_async_set_transport(int id, intptr_t transport) {
    if (!id || !transport) return;
#ifdef _WIN32
    HINTERNET abort_request = NULL;
#endif
    int abort_socket = -1;
    async_lock_acquire();
    VntAsyncTask *t = async_find_locked(id);
    if (t && t->state == TASK_RUNNING) {
        t->active_transport = transport;
    } else if (t && t->state == TASK_CANCELLED) {
#ifdef _WIN32
        abort_request = (HINTERNET)transport;
#else
        abort_socket = (int)(transport - 1);
#endif
    }
    async_lock_release();
#ifdef _WIN32
    if (abort_request) WinHttpCloseHandle(abort_request);
#else
    if (abort_socket >= 0) shutdown(abort_socket, SHUT_RDWR);
#endif
}

int vnt_native_http_async_should_close_transport(int id, intptr_t transport) {
    int should_close = 1;
    if (!id || !transport) return 1;
    async_lock_acquire();
    VntAsyncTask *t = async_find_locked(id);
    if (t && t->active_transport == transport) {
        t->active_transport = 0;
    } else if (t && t->state == TASK_CANCELLED) {
        /* Cancellation already closed this WinHTTP handle. */
#ifdef _WIN32
        should_close = 0;
#endif
    }
    async_lock_release();
    return should_close;
}

unsigned char *vnt_native_http_async_stream_read(int id, size_t maximum, size_t *out_length) {
    if (out_length) *out_length = 0;
    if (!maximum || maximum > VNT_NET_STREAM_CAPACITY) maximum = VNT_NET_STREAM_CAPACITY;
    unsigned char *out = (unsigned char *)malloc(maximum ? maximum : 1);
    if (!out) return NULL;
    async_lock_acquire();
    VntAsyncTask *t = async_find_locked(id);
    if (!t || t->mode != 1) {
        async_lock_release();
        free(out);
        return NULL;
    }
    while (!t->stream_count && (t->state == TASK_QUEUED || t->state == TASK_RUNNING))
        async_wait_forever();
    size_t amount = t->stream_count < maximum ? t->stream_count : maximum;
    size_t first = amount < VNT_NET_STREAM_CAPACITY - t->stream_head
        ? amount : VNT_NET_STREAM_CAPACITY - t->stream_head;
    if (first) memcpy(out, t->stream_buffer + t->stream_head, first);
    if (amount > first) memcpy(out + first, t->stream_buffer, amount - first);
    t->stream_head = (t->stream_head + amount) % VNT_NET_STREAM_CAPACITY;
    t->stream_count -= amount;
    if (out_length) *out_length = amount;
    async_wake_all();
    async_lock_release();
    return out;
}

int vnt_native_http_async_stream_done(int id) {
    int done = 1;
    async_lock_acquire();
    VntAsyncTask *t = async_find_locked(id);
    if (t && t->mode == 1)
        done = (t->state == TASK_DONE || t->state == TASK_CANCELLED) && t->stream_count == 0;
    async_lock_release();
    return done;
}
