/*
 * Copyright (c) 2026 ItsDropper
 *
 * This file is part of VNT and is licensed under the MIT License.
 * See the LICENSE file in the project root for the full license text.
 *
 * Native HTTP transport for compiled VNT programs. No external curl process.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define VNT_NET_MAX_RESPONSE (16u * 1024u * 1024u)

static char *net_copy(const char *s, size_t n) {
    char *p = (char *)malloc(n + 1);
    if (!p) return NULL;
    memcpy(p, s, n);
    p[n] = '\0';
    return p;
}

char *vnt_native_url_encode(const char *text) {
    static const char hex[] = "0123456789ABCDEF";
    if (!text) text = "";
    size_t n = strlen(text);
    if (n > (SIZE_MAX - 1) / 3) return NULL;
    char *out = (char *)malloc(n * 3 + 1);
    if (!out) return NULL;
    char *w = out;
    for (const unsigned char *p = (const unsigned char *)text; *p; ++p) {
        unsigned char c = *p;
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_' ||
            c == '.' || c == '~') {
            *w++ = (char)c;
        } else {
            *w++ = '%'; *w++ = hex[c >> 4]; *w++ = hex[c & 15];
        }
    }
    *w = '\0';
    return out;
}

static char *decode_chunked(char *body, size_t length) {
    char *out = (char *)malloc(length + 1);
    if (!out) return NULL;
    size_t r = 0, w = 0;
    while (r < length) {
        size_t chunk = 0;
        int digits = 0;
        while (r < length && body[r] != '\r' && body[r] != ';') {
            unsigned char c = (unsigned char)body[r++];
            unsigned int v;
            if (c >= '0' && c <= '9') v = c - '0';
            else if (c >= 'a' && c <= 'f') v = c - 'a' + 10;
            else if (c >= 'A' && c <= 'F') v = c - 'A' + 10;
            else { free(out); return NULL; }
            if (chunk > (SIZE_MAX - v) / 16) { free(out); return NULL; }
            chunk = chunk * 16 + v;
            digits++;
        }
        while (r < length && body[r] != '\n') r++;
        if (r < length) r++;
        if (!digits || chunk > length - r) { free(out); return NULL; }
        if (!chunk) { out[w] = '\0'; return out; }
        memcpy(out + w, body + r, chunk);
        w += chunk; r += chunk;
        if (r + 1 < length && body[r] == '\r' && body[r + 1] == '\n') r += 2;
    }
    out[w] = '\0';
    return out;
}

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>

static wchar_t *to_wide(const char *s) {
    if (!s) s = "";
    int n = MultiByteToWideChar(CP_UTF8, 0, s, -1, NULL, 0);
    if (n <= 0) return NULL;
    wchar_t *w = (wchar_t *)calloc((size_t)n, sizeof(wchar_t));
    if (!w || !MultiByteToWideChar(CP_UTF8, 0, s, -1, w, n)) {
        free(w); return NULL;
    }
    return w;
}

char *vnt_native_http_request(const char *method, const char *url,
                      const char *headers, const char *body, int *status) {
    if (status) *status = 0;
    wchar_t *wide_url = to_wide(url), *wide_method = to_wide(method);
    if (!wide_url || !wide_method) { free(wide_url); free(wide_method); return NULL; }
    URL_COMPONENTS parts;
    memset(&parts, 0, sizeof(parts));
    parts.dwStructSize = sizeof(parts);
    parts.dwSchemeLength = parts.dwHostNameLength = parts.dwUrlPathLength = parts.dwExtraInfoLength = (DWORD)-1;
    if (!WinHttpCrackUrl(wide_url, 0, 0, &parts)) {
        free(wide_url); free(wide_method); return NULL;
    }
    size_t host_bytes = ((size_t)parts.dwHostNameLength + 1) * sizeof(wchar_t);
    wchar_t *hostname = (wchar_t *)calloc(1, host_bytes);
    if (!hostname) { free(wide_url); free(wide_method); return NULL; }
    memcpy(hostname, parts.lpszHostName, parts.dwHostNameLength * sizeof(wchar_t));
    size_t path_len = (size_t)parts.dwUrlPathLength + (size_t)parts.dwExtraInfoLength;
    int has_path = parts.dwUrlPathLength != 0;
    wchar_t *path = (wchar_t *)calloc(path_len + 2, sizeof(wchar_t));
    if (!path) { free(hostname); free(wide_url); free(wide_method); return NULL; }
    size_t path_at = 0;
    if (has_path) {
        memcpy(path, parts.lpszUrlPath, parts.dwUrlPathLength * sizeof(wchar_t));
        path_at = parts.dwUrlPathLength;
    } else {
        path[path_at++] = L'/';
    }
    if (parts.dwExtraInfoLength) {
        memcpy(path + path_at, parts.lpszExtraInfo, parts.dwExtraInfoLength * sizeof(wchar_t));
        path_at += parts.dwExtraInfoLength;
    }
    path[path_at] = L'\\0';
    HINTERNET session = WinHttpOpen(L"VNT/0.1", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                    WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    HINTERNET connection = session ? WinHttpConnect(session, hostname, parts.nPort, 0) : NULL;
    DWORD flags = parts.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0;
    HINTERNET request = connection ? WinHttpOpenRequest(connection, wide_method,
        path, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags) : NULL;
    free(path); free(hostname); free(wide_url); free(wide_method);
    if (!request) {
        if (connection) WinHttpCloseHandle(connection);
        if (session) WinHttpCloseHandle(session);
        return NULL;
    }
    WinHttpSetTimeouts(request, 10000, 10000, 30000, 30000);
    if (headers && *headers) {
        char *copy = _strdup(headers), *save = NULL;
        if (copy) {
            for (char *line = strtok_s(copy, "\n", &save); line; line = strtok_s(NULL, "\n", &save)) {
                while (*line == '\r' || *line == ' ') line++;
                wchar_t *wh = to_wide(line);
                if (wh) { WinHttpAddRequestHeaders(request, wh, (DWORD)-1, WINHTTP_ADDREQ_FLAG_ADD); free(wh); }
            }
            free(copy);
        }
    }
    DWORD body_len = body ? (DWORD)strlen(body) : 0;
    BOOL ok = WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        body_len ? (LPVOID)body : WINHTTP_NO_REQUEST_DATA, body_len, body_len, 0) &&
        WinHttpReceiveResponse(request, NULL);
    DWORD code = 0, code_size = sizeof(code);
    if (ok) WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                                WINHTTP_HEADER_NAME_BY_INDEX, &code, &code_size, WINHTTP_NO_HEADER_INDEX);
    if (status) *status = (int)code;
    char *result = NULL;
    size_t used = 0, capacity = 0;
    if (ok) {
        for (;;) {
            DWORD available = 0;
            if (!WinHttpQueryDataAvailable(request, &available) || !available) break;
            if ((size_t)available > VNT_NET_MAX_RESPONSE - used) { ok = FALSE; break; }
            if (used + available + 1 > capacity) {
                size_t next = used + available + 1;
                char *grown = (char *)realloc(result, next);
                if (!grown) { ok = FALSE; break; }
                result = grown; capacity = next;
            }
            DWORD received = 0;
            if (!WinHttpReadData(request, result + used, available, &received)) { ok = FALSE; break; }
            used += received;
        }
    }
    if (result) result[used] = '\0';
    if (request) WinHttpCloseHandle(request);
    if (connection) WinHttpCloseHandle(connection);
    if (session) WinHttpCloseHandle(session);
    if (!ok) { free(result); return NULL; }
    if (!result) result = net_copy("", 0);
    return result;
}

#else
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509v3.h>

typedef struct {
    char *host, *port, *target;
    int tls;
} ParsedUrl;

static void parsed_url_free(ParsedUrl *u) {
    free(u->host); free(u->port); free(u->target);
    memset(u, 0, sizeof(*u));
}

static int parse_url(const char *url, ParsedUrl *u) {
    memset(u, 0, sizeof(*u));
    const char *p;
    if (!strncmp(url, "http://", 7)) { p = url + 7; u->tls = 0; }
    else if (!strncmp(url, "https://", 8)) { p = url + 8; u->tls = 1; }
    else return 0;
    const char *end = p + strcspn(p, "/?#");
    if (p == end || memchr(p, '@', (size_t)(end-p))) return 0;
    const char *colon = NULL;
    if (*p == '[') {
        const char *close = memchr(p, ']', (size_t)(end-p));
        if (!close) return 0;
        if (close + 1 < end && close[1] == ':') colon = close + 1;
    } else {
        for (const char *q = p; q < end; ++q) if (*q == ':') colon = q;
    }
    const char *host_end = colon ? colon : end;
    if (*p == '[') { p++; host_end--; }
    u->host = net_copy(p, (size_t)(host_end-p));
    u->port = colon ? net_copy(colon+1, (size_t)(end-colon-1)) : net_copy(u->tls ? "443" : "80", 2 + (u->tls ? 1 : 0));
    const char *path = *end ? end : "/";
    if (*path == '?') u->target = NULL;
    if (*path == '#') return 0;
    if (*path == '?') {
        size_t n = strlen(path); u->target = (char *)malloc(n + 2);
        if (u->target) { u->target[0] = '/'; memcpy(u->target+1, path, n+1); }
    } else u->target = strdup(path);
    if (!u->host || !u->port || !u->target || !*u->port) { parsed_url_free(u); return 0; }
    return 1;
}

static int write_all(int fd, SSL *ssl, const char *data, size_t length) {
    size_t sent = 0;
    while (sent < length) {
        int n = ssl ? SSL_write(ssl, data + sent, (int)((length-sent)>INT32_MAX?INT32_MAX:length-sent))
                    : (int)send(fd, data + sent, length-sent, 0);
        if (n <= 0) return 0;
        sent += (size_t)n;
    }
    return 1;
}

char *vnt_native_http_request(const char *method, const char *url,
                      const char *headers, const char *body, int *status) {
    if (status) *status = 0;
    ParsedUrl u;
    if (!parse_url(url ? url : "", &u)) return NULL;
    struct addrinfo hints, *addresses = NULL;
    memset(&hints, 0, sizeof(hints)); hints.ai_socktype = SOCK_STREAM; hints.ai_family = AF_UNSPEC;
    if (getaddrinfo(u.host, u.port, &hints, &addresses) != 0) { parsed_url_free(&u); return NULL; }
    int fd = -1;
    for (struct addrinfo *a = addresses; a; a = a->ai_next) {
        fd = (int)socket(a->ai_family, a->ai_socktype, a->ai_protocol);
        if (fd < 0) continue;
        struct timeval timeout = {30, 0};
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
        if (connect(fd, a->ai_addr, a->ai_addrlen) == 0) break;
        close(fd); fd = -1;
    }
    freeaddrinfo(addresses);
    if (fd < 0) { parsed_url_free(&u); return NULL; }
    SSL_CTX *ctx = NULL; SSL *ssl = NULL;
    if (u.tls) {
        ctx = SSL_CTX_new(TLS_client_method());
        if (!ctx || SSL_CTX_set_default_verify_paths(ctx) != 1) goto fail;
        SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
        ssl = SSL_new(ctx);
        if (!ssl || SSL_set_tlsext_host_name(ssl, u.host) != 1 ||
            SSL_set1_host(ssl, u.host) != 1) goto fail;
        SSL_set_fd(ssl, fd);
        if (SSL_connect(ssl) != 1) goto fail;
    }
    size_t reqcap = strlen(method ? method : "GET") + strlen(u.target) + strlen(u.host) +
                    (headers ? strlen(headers) : 0) + (body ? strlen(body) : 0) + 512;
    char *request = (char *)malloc(reqcap);
    if (!request) goto fail;
    int n = snprintf(request, reqcap, "%s %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n",
                     method ? method : "GET", u.target, u.host);
    if (n < 0 || (size_t)n >= reqcap) { free(request); goto fail; }
    size_t at = (size_t)n;
    if (headers && *headers) {
        const char *p = headers;
        while (*p) {
            const char *e = strchr(p, '\n');
            size_t len = e ? (size_t)(e-p) : strlen(p);
            while (len && p[len-1] == '\r') len--;
            if (len && at + len + 2 < reqcap) { memcpy(request+at,p,len); at+=len; request[at++]='\r'; request[at++]='\n'; }
            if (!e) break; p=e+1;
        }
    }
    size_t bodylen = body ? strlen(body) : 0;
    if (bodylen) at += (size_t)snprintf(request+at, reqcap-at, "Content-Length: %zu\r\n", bodylen);
    if (bodylen && !strstr(headers ? headers : "", "Content-Type:") && !strstr(headers ? headers : "", "content-type:"))
        at += (size_t)snprintf(request+at, reqcap-at, "Content-Type: application/octet-stream\r\n");
    at += (size_t)snprintf(request+at, reqcap-at, "\r\n");
    if (bodylen && at + bodylen < reqcap) { memcpy(request+at, body, bodylen); at+=bodylen; }
    int sentok = write_all(fd, ssl, request, at);
    free(request);
    if (!sentok) goto fail;
    size_t cap = 8192, used = 0;
    char *raw = (char *)malloc(cap + 1);
    if (!raw) goto fail;
    for (;;) {
        if (used == cap) {
            if (cap >= VNT_NET_MAX_RESPONSE + 65536u) { free(raw); goto fail; }
            size_t next = cap * 2;
            if (next > VNT_NET_MAX_RESPONSE + 65536u) next = VNT_NET_MAX_RESPONSE + 65536u;
            char *grown = (char *)realloc(raw, next + 1);
            if (!grown) { free(raw); goto fail; }
            raw = grown; cap = next;
        }
        int got = ssl ? SSL_read(ssl, raw+used, (int)((cap-used)>INT32_MAX?INT32_MAX:cap-used))
                      : (int)recv(fd, raw+used, cap-used, 0);
        if (got <= 0) {
            if (ssl && SSL_get_error(ssl, got) == SSL_ERROR_ZERO_RETURN) break;
            if (!ssl && got == 0) break;
            if (ssl && SSL_get_error(ssl, got) == SSL_ERROR_SYSCALL && errno == 0) break;
            if (got < 0 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)) continue;
            if (ssl && SSL_get_error(ssl, got) == SSL_ERROR_WANT_READ) continue;
            if (got < 0) { free(raw); goto fail; }
            break;
        }
        used += (size_t)got;
        if (used > VNT_NET_MAX_RESPONSE + 65536u) { free(raw); goto fail; }
    }
    raw[used] = '\0';
    char *sep = strstr(raw, "\r\n\r\n");
    if (!sep) { free(raw); goto fail; }
    int code = 0;
    if (sscanf(raw, "HTTP/%*u.%*u %d", &code) != 1) { free(raw); goto fail; }
    if (status) *status = code;
    size_t headerlen = (size_t)(sep + 4 - raw), bodylen_read = used - headerlen;
    char *result = NULL;
    if (strcasestr(raw, "Transfer-Encoding: chunked") && bodylen_read) {
        result = decode_chunked(sep+4, bodylen_read);
    } else {
        if (bodylen_read > VNT_NET_MAX_RESPONSE) { free(raw); goto fail; }
        result = net_copy(sep+4, bodylen_read);
    }
    free(raw);
    if (ssl) { SSL_shutdown(ssl); SSL_free(ssl); }
    if (ctx) SSL_CTX_free(ctx);
    close(fd); parsed_url_free(&u);
    return result;
fail:
    if (ssl) SSL_free(ssl);
    if (ctx) SSL_CTX_free(ctx);
    close(fd); parsed_url_free(&u);
    return NULL;
}
#endif
