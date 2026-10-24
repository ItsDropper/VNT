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

static char *net_copy(const char *s, size_t n) {
    char *p = (char *)malloc(n + 1);
    if (!p) return NULL;
    memcpy(p, s, n);
    p[n] = '\0';
    return p;
}

static int http_token_char(unsigned char c) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9')) return 1;
    return strchr("!#$%&'*+-.^_`|~", c) != NULL;
}

static int header_name_equals(const char *line, size_t name_length, const char *name) {
    if (strlen(name) != name_length) return 0;
    for (size_t i = 0; i < name_length; ++i)
        if (tolower((unsigned char)line[i]) != tolower((unsigned char)name[i])) return 0;
    return 1;
}

static int reserved_request_header(const char *line, size_t length) {
    const char *colon = (const char *)memchr(line, ':', length);
    if (!colon) return 0;
    size_t n = (size_t)(colon-line);
    return header_name_equals(line,n,"host") ||
           header_name_equals(line,n,"content-length") ||
           header_name_equals(line,n,"transfer-encoding") ||
           header_name_equals(line,n,"connection") ||
           header_name_equals(line,n,"proxy-connection");
}

static int valid_http_method(const char *method) {
    if (!method || !*method) return 0;
    for (const unsigned char *p = (const unsigned char *)method; *p; ++p)
        if (!http_token_char(*p)) return 0;
    return 1;
}

static int valid_http_url_chars(const char *url) {
    if (!url || !*url) return 0;
    for (const unsigned char *p = (const unsigned char *)url; *p; ++p)
        if (*p <= 0x20 || *p == 0x7f) return 0;
    return 1;
}

static int valid_request_headers(const char *headers) {
    const char *p = headers ? headers : "";
    while (*p) {
        const char *end = strchr(p, '\n');
        size_t n = end ? (size_t)(end-p) : strlen(p);
        if (n && p[n-1] == '\r') n--;
        const char *colon = (const char *)memchr(p, ':', n);
        if (!colon || colon == p) return 0;
        for (const char *q = p; q < colon; ++q)
            if (!http_token_char((unsigned char)*q)) return 0;
        for (const char *q = colon+1; q < p+n; ++q) {
            unsigned char ch = (unsigned char)*q;
            if ((ch < 0x20 && ch != '\t') || ch == 0x7f) return 0;
        }
        if (!end) break;
        p = end+1;
    }
    return 1;
}

static int has_request_header(const char *headers, const char *name) {
    const char *p = headers ? headers : "";
    while (*p) {
        const char *end = strchr(p, '\n');
        size_t n = end ? (size_t)(end-p) : strlen(p);
        if (n && p[n-1] == '\r') n--;
        if (header_name_equals(p, (size_t)((const char *)memchr(p, ':', n)-p), name))
            return 1;
        if (!end) break;
        p = end+1;
    }
    return 0;
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

static char *decode_chunked(char *body, size_t length, size_t *decoded_length) {
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
        if (!chunk) { out[w] = '\0'; if (decoded_length) *decoded_length = w; return out; }
        memcpy(out + w, body + r, chunk);
        w += chunk; r += chunk;
        if (r + 1 < length && body[r] == '\r' && body[r + 1] == '\n') r += 2;
    }
    out[w] = '\0';
    if (decoded_length) *decoded_length = w;
    return out;
}

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>

static char *to_utf8(const wchar_t *s) {
    int n = WideCharToMultiByte(CP_UTF8, 0, s, -1, NULL, 0, NULL, NULL);
    if (n <= 0) return NULL;
    char *out = (char *)malloc((size_t)n);
    if (!out || !WideCharToMultiByte(CP_UTF8, 0, s, -1, out, n, NULL, NULL)) {
        free(out); return NULL;
    }
    return out;
}

static int win_same_origin(const char *a, const char *b) {
    const char *as = strstr(a ? a : "", "://"), *bs = strstr(b ? b : "", "://");
    if (!as || !bs || (size_t)(as-a) != (size_t)(bs-b) ||
        _strnicmp(a, b, (size_t)(as-a) + 3) != 0) return 0;
    const char *ah = as+3, *bh = bs+3;
    size_t an = strcspn(ah, "/?#"), bn = strcspn(bh, "/?#");
    return an == bn && _strnicmp(ah, bh, an) == 0;
}

static char *win_headers_without_credentials(const char *headers) {
    size_t cap = headers ? strlen(headers) + 2 : 2;
    char *out = (char *)malloc(cap);
    if (!out) return NULL;
    size_t used = 0;
    const char *p = headers ? headers : "";
    while (*p) {
        const char *end = strchr(p, '\n');
        size_t len = end ? (size_t)(end-p) : strlen(p);
        while (len && p[len-1] == '\r') len--;
        int sensitive = (len >= 14 && !_strnicmp(p, "Authorization:", 14)) ||
                        (len >= 19 && !_strnicmp(p, "Proxy-Authorization:", 19)) ||
                        (len >= 7 && !_strnicmp(p, "Cookie:", 7));
        if (!sensitive && len) {
            memcpy(out+used, p, len); used += len; out[used++] = '\n';
        }
        if (!end) break;
        p = end+1;
    }
    out[used] = '\0';
    return out;
}

static char *win_resolve_redirect(const char *base, const char *location) {
    if (!location || !*location) return NULL;
    if (!_strnicmp(location, "http://", 7) || !_strnicmp(location, "https://", 8))
        return _strdup(location);
    const char *scheme = strstr(base, "://");
    if (!scheme) return NULL;
    const char *authority_end = strpbrk(scheme+3, "/?#");
    size_t origin_len = authority_end ? (size_t)(authority_end-base) : strlen(base);
    size_t prefix_len = origin_len;
    if (location[0] == '/') {
        /* Root-relative reference. */
    } else if (location[0] == '?') {
        const char *fragment = strchr(base, '#');
        size_t base_len = fragment ? (size_t)(fragment-base) : strlen(base);
        const char *query = strchr(base, '?');
        if (query && (size_t)(query-base) < base_len) base_len = (size_t)(query-base);
        char *out = (char *)malloc(base_len + strlen(location) + 1);
        if (!out) return NULL;
        memcpy(out, base, base_len); strcpy(out+base_len, location);
        return out;
    } else {
        const char *fragment = strchr(base, '#');
        size_t base_len = fragment ? (size_t)(fragment-base) : strlen(base);
        const char *query = strchr(base, '?');
        if (query && (size_t)(query-base) < base_len) base_len = (size_t)(query-base);
        const char *slash = NULL;
        for (const char *p = scheme+3; p < base+base_len; ++p) if (*p == '/') slash = p;
        prefix_len = slash ? (size_t)(slash-base+1) : origin_len;
        if (!slash) {
            size_t n = prefix_len + 1 + strlen(location) + 1;
            char *out = (char *)malloc(n);
            if (!out) return NULL;
            snprintf(out, n, "%.*s/%s", (int)prefix_len, base, location);
            return out;
        }
    }
    size_t n = prefix_len + strlen(location) + 1;
    char *out = (char *)malloc(n);
    if (!out) return NULL;
    memcpy(out, base, prefix_len);
    strcpy(out+prefix_len, location);
    return out;
}

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

static char *vnt_native_http_request_impl(const char *method, const char *url,
                      const char *headers, const char *body, int *status, size_t *response_length,
                      unsigned int redirect_count) {
    if (status) *status = 0;
    if (response_length) *response_length = 0;
    if (!valid_http_method(method ? method : "GET") || !valid_http_url_chars(url) || !valid_request_headers(headers)) return NULL;
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
    path[path_at] = L'\0';
    wchar_t *fragment = wcschr(path, L'#');
    if (fragment) *fragment = L'\0';
    HINTERNET session = WinHttpOpen(L"VNT/0.1", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                    WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (session) WinHttpSetTimeouts(session, 10000, 10000, 30000, 30000);
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
    DWORD redirect_policy = WINHTTP_OPTION_REDIRECT_POLICY_NEVER;
    WinHttpSetOption(request, WINHTTP_OPTION_REDIRECT_POLICY, &redirect_policy, sizeof(redirect_policy));
    if (headers && *headers) {
        char *copy = _strdup(headers), *save = NULL;
        if (copy) {
            for (char *line = strtok_s(copy, "\n", &save); line; line = strtok_s(NULL, "\n", &save)) {
                while (*line == ' ') line++;
                size_t line_length = strlen(line);
                while (line_length && line[line_length-1] == '\r') line[--line_length] = '\0';
                if (!line_length || reserved_request_header(line, line_length)) continue;
                wchar_t *wh = to_wide(line);
                if (wh) { WinHttpAddRequestHeaders(request, wh, (DWORD)-1, WINHTTP_ADDREQ_FLAG_ADD); free(wh); }
            }
            free(copy);
        }
    }
    size_t body_size = body ? strlen(body) : 0;
    if (body_size > MAXDWORD) {
        WinHttpCloseHandle(request);
        if (connection) WinHttpCloseHandle(connection);
        if (session) WinHttpCloseHandle(session);
        return NULL;
    }
    DWORD body_len = (DWORD)body_size;
    BOOL ok = WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        body_len ? (LPVOID)body : WINHTTP_NO_REQUEST_DATA, body_len, body_len, 0) &&
        WinHttpReceiveResponse(request, NULL);
    DWORD code = 0, code_size = sizeof(code);
    if (ok) WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                                WINHTTP_HEADER_NAME_BY_INDEX, &code, &code_size, WINHTTP_NO_HEADER_INDEX);
    if (ok && (code == 301 || code == 302 || code == 303 || code == 307 || code == 308) &&
        redirect_count < 10) {
        DWORD location_size = 0;
        WinHttpQueryHeaders(request, WINHTTP_QUERY_LOCATION, WINHTTP_HEADER_NAME_BY_INDEX,
                            NULL, &location_size, WINHTTP_NO_HEADER_INDEX);
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER && location_size >= sizeof(wchar_t)) {
            wchar_t *wide_location = (wchar_t *)calloc(1, location_size + sizeof(wchar_t));
            if (wide_location && WinHttpQueryHeaders(request, WINHTTP_QUERY_LOCATION,
                WINHTTP_HEADER_NAME_BY_INDEX, wide_location, &location_size, WINHTTP_NO_HEADER_INDEX)) {
                char *location = to_utf8(wide_location);
                char *next_url = win_resolve_redirect(url, location);
                free(location);
                free(wide_location);
                WinHttpCloseHandle(request);
                if (connection) WinHttpCloseHandle(connection);
                if (session) WinHttpCloseHandle(session);
                if (!next_url) return NULL;
                const char *next_method = method ? method : "GET";
                const char *next_body = body ? body : "";
                if (code == 303 || ((code == 301 || code == 302) && !_stricmp(next_method, "POST"))) {
                    next_method = "GET";
                    next_body = "";
                }
                char *safe_headers = NULL;
                const char *next_headers = headers;
                if (!win_same_origin(url, next_url)) {
                    safe_headers = win_headers_without_credentials(headers);
                    if (!safe_headers) { free(next_url); return NULL; }
                    next_headers = safe_headers;
                }
                char *redirected = vnt_native_http_request_impl(next_method, next_url, next_headers,
                    next_body, status, response_length, redirect_count + 1);
                free(safe_headers);
                free(next_url);
                return redirected;
            }
            free(wide_location);
        }
    }
    if (status) *status = (int)code;
    char *result = NULL;
    size_t used = 0, capacity = 0;
    if (ok) {
        for (;;) {
            DWORD available = 0;
            if (!WinHttpQueryDataAvailable(request, &available) || !available) break;
            if ((size_t)available > SIZE_MAX - used - 1) { ok = FALSE; break; }
            if (used + (size_t)available + 1 > capacity) {
                size_t next = capacity ? capacity : 8192;
                while (next < used + (size_t)available + 1) {
                    if (next > SIZE_MAX / 2) { next = used + (size_t)available + 1; break; }
                    next *= 2;
                }
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
    if (response_length) *response_length = used;
    if (request) WinHttpCloseHandle(request);
    if (connection) WinHttpCloseHandle(connection);
    if (session) WinHttpCloseHandle(session);
    if (!ok) { free(result); return NULL; }
    if (!result) result = net_copy("", 0);
    return result;
}

char *vnt_native_http_request(const char *method, const char *url,
                      const char *headers, const char *body, int *status, size_t *response_length) {
    return vnt_native_http_request_impl(method, url, headers, body, status, response_length, 0);
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
    if (!valid_http_url_chars(url)) return 0;
    const char *p;
    if (!strncasecmp(url, "http://", 7)) { p = url + 7; u->tls = 0; }
    else if (!strncasecmp(url, "https://", 8)) { p = url + 8; u->tls = 1; }
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
    const char *path = (*end == '/' || *end == '?') ? end : "/";
    const char *fragment = strchr(path, '#');
    size_t path_length = fragment ? (size_t)(fragment-path) : strlen(path);
    if (*path == '?') {
        u->target = (char *)malloc(path_length + 2);
        if (u->target) {
            u->target[0] = '/';
            memcpy(u->target + 1, path, path_length);
            u->target[path_length + 1] = '\0';
        }
    } else {
        u->target = net_copy(path, path_length);
    }
    if (!u->host || !u->port || !u->target || !*u->port) { parsed_url_free(u); return 0; }
    return 1;
}

static int same_origin(const char *a, const char *b) {
    const char *as = strstr(a ? a : "", "://");
    const char *bs = strstr(b ? b : "", "://");
    if (!as || !bs || (size_t)(as-(a ? a : "")) != (size_t)(bs-(b ? b : "")) ||
        strncasecmp(a, b, (size_t)(as-(a ? a : "")) + 3) != 0) return 0;
    const char *ah = as + 3, *bh = bs + 3;
    size_t an = strcspn(ah, "/?#"), bn = strcspn(bh, "/?#");
    return an == bn && strncasecmp(ah, bh, an) == 0;
}

static char *headers_without_credentials(const char *headers) {
    size_t cap = headers ? strlen(headers) + 2 : 2;
    char *out = (char *)malloc(cap);
    if (!out) return NULL;
    size_t used = 0;
    const char *p = headers ? headers : "";
    while (*p) {
        const char *end = strchr(p, '\n');
        size_t len = end ? (size_t)(end-p) : strlen(p);
        while (len && p[len-1] == '\r') len--;
        int sensitive = (len >= 14 && !strncasecmp(p, "Authorization:", 14)) ||
                        (len >= 19 && !strncasecmp(p, "Proxy-Authorization:", 19)) ||
                        (len >= 7 && !strncasecmp(p, "Cookie:", 7));
        if (!sensitive && len) {
            memcpy(out + used, p, len); used += len;
            out[used++] = '\n';
        }
        if (!end) break;
        p = end + 1;
    }
    out[used] = '\0';
    return out;
}

static int parse_response_headers(const char *raw, size_t used, const char *method,
                                  size_t *header_length, size_t *content_length,
                                  int *has_content_length, int *status_code, int *no_body) {
    char *sep = strstr(raw, "\r\n\r\n");
    if (!sep || (size_t)(sep-raw) + 4 > used) return 0;
    size_t hlen = (size_t)(sep-raw) + 4;
    int code = 0;
    if (sscanf(raw, "HTTP/%*u.%*u %d", &code) != 1) return -1;
    size_t content_len = 0;
    int has_len = 0, transfer_chunked = 0;
    char *line = (char *)raw;
    while (line < sep) {
        char *end = strstr(line, "\r\n");
        if (!end || end > sep) break;
        if ((size_t)(end-line) > 15 && !strncasecmp(line, "Content-Length:", 15)) {
            char *value = line + 15;
            while (value < end && (*value == ' ' || *value == '\t')) value++;
            if (value == end) return -1;
            for (char *q = value; q < end && *q != ' ' && *q != '\t'; ++q)
                if (*q < '0' || *q > '9') return -1;
            char *tail = NULL;
            unsigned long long parsed = strtoull(value, &tail, 10);
            while (tail && tail < end && (*tail == ' ' || *tail == '\t')) tail++;
            if (!tail || tail != end || parsed > SIZE_MAX) return -1;
            if (has_len && content_len != (size_t)parsed) return -1;
            content_len = (size_t)parsed;
            has_len = 1;
        } else if ((size_t)(end-line) > 18 && !strncasecmp(line, "Transfer-Encoding:", 18) &&
                   strcasestr(line+18, "chunked")) {
            transfer_chunked = 1;
        }
        line = end + 2;
    }
    if (has_len && transfer_chunked) return -1;
    *header_length = hlen;
    *content_length = content_len;
    *has_content_length = has_len;
    *status_code = code;
    *no_body = (method && !strcasecmp(method, "HEAD")) ||
               (code >= 100 && code < 200) || code == 204 || code == 304;
    return 1;
}

static int write_all(int fd, SSL *ssl, const char *data, size_t length) {
    size_t sent = 0;
    while (sent < length) {
        size_t remaining = length - sent;
        int amount = (int)(remaining > INT32_MAX ? INT32_MAX : remaining);
        int n;
        if (ssl) {
            n = SSL_write(ssl, data + sent, amount);
        } else {
#ifdef MSG_NOSIGNAL
            n = (int)send(fd, data + sent, (size_t)amount, MSG_NOSIGNAL);
#else
            n = (int)send(fd, data + sent, (size_t)amount, 0);
#endif
        }
        if (n <= 0) return 0;
        sent += (size_t)n;
    }
    return 1;
}

static char *vnt_native_http_request_impl(const char *method, const char *url,
                      const char *headers, const char *body, int *status, size_t *response_length,
                      unsigned int redirect_count) {
    if (status) *status = 0;
    if (response_length) *response_length = 0;
    if (!valid_request_headers(headers)) return NULL;
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
    size_t reqcap = 512;
    size_t parts[] = { strlen(method ? method : "GET"), strlen(u.target), strlen(u.host),
                       headers ? strlen(headers) : 0, body ? strlen(body) : 0 };
    for (size_t i = 0; i < sizeof(parts)/sizeof(parts[0]); ++i) {
        if (parts[i] > SIZE_MAX - reqcap) goto fail;
        reqcap += parts[i];
    }
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
            if (len && !reserved_request_header(p, len) && at + len + 2 < reqcap) {
                memcpy(request+at,p,len); at+=len; request[at++]='\r'; request[at++]='\n';
            }
            if (!e) break; p=e+1;
        }
    }
    size_t bodylen = body ? strlen(body) : 0;
    if (bodylen) at += (size_t)snprintf(request+at, reqcap-at, "Content-Length: %zu\r\n", bodylen);
    if (bodylen && !has_request_header(headers, "Content-Type"))
        at += (size_t)snprintf(request+at, reqcap-at, "Content-Type: application/octet-stream\r\n");
    at += (size_t)snprintf(request+at, reqcap-at, "\r\n");
    if (bodylen && at + bodylen < reqcap) { memcpy(request+at, body, bodylen); at+=bodylen; }
    int sentok = write_all(fd, ssl, request, at);
    free(request);
    if (!sentok) goto fail;
    size_t cap = 8192, used = 0;
    size_t headerlen = 0, expected_body_length = 0;
    int has_expected_body_length = 0, code = 0, response_no_body = 0;
    char *raw = (char *)malloc(cap + 1);
    if (!raw) goto fail;
    raw[0] = '\0';
    for (;;) {
        if (used == cap) {
            if (cap > SIZE_MAX / 2) { free(raw); goto fail; }
            size_t next = cap * 2;
            if (next <= cap || next == SIZE_MAX) { free(raw); goto fail; }
            char *grown = (char *)realloc(raw, next + 1);
            if (!grown) { free(raw); goto fail; }
            raw = grown; cap = next;
        }
        int receive_size = (int)((cap-used)>INT32_MAX?INT32_MAX:cap-used);
        int got = ssl ? SSL_read(ssl, raw+used, receive_size)
                      : (int)recv(fd, raw+used, (size_t)receive_size, 0);
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
        raw[used] = '\0';
        int parsed = parse_response_headers(raw, used, method, &headerlen,
                                            &expected_body_length, &has_expected_body_length,
                                            &code, &response_no_body);
        if (parsed < 0) { free(raw); goto fail; }
        if (parsed > 0 && (response_no_body ||
            (has_expected_body_length && used - headerlen >= expected_body_length)))
            break;
    }
    raw[used] = '\0';
    int parsed = parse_response_headers(raw, used, method, &headerlen,
                                        &expected_body_length, &has_expected_body_length,
                                        &code, &response_no_body);
    if (parsed <= 0) { free(raw); goto fail; }
    if (has_expected_body_length && used - headerlen < expected_body_length) {
        free(raw); goto fail;
    }
    char *sep = strstr(raw, "\r\n\r\n");
    size_t bodylen_read = response_no_body ? 0 : used - headerlen;
    if (has_expected_body_length && bodylen_read > expected_body_length)
        bodylen_read = expected_body_length;

    /* Follow standard redirects, with a hard loop limit and relative URL support. */
    if ((code == 301 || code == 302 || code == 303 || code == 307 || code == 308) &&
        redirect_count < 10) {
        char *location = NULL;
        char *line = raw;
        while (line < sep) {
            char *line_end = strstr(line, "\r\n");
            if (!line_end || line_end > sep) break;
            if ((size_t)(line_end - line) > 9 && !strncasecmp(line, "Location:", 9)) {
                char *value = line + 9;
                while (value < line_end && (*value == ' ' || *value == '\t')) value++;
                location = net_copy(value, (size_t)(line_end - value));
                break;
            }
            line = line_end + 2;
        }
        if (location && *location) {
            char *next_url = NULL;
            if (!strncasecmp(location, "http://", 7) || !strncasecmp(location, "https://", 8)) {
                next_url = strdup(location);
            } else if (location[0] == '/') {
                size_t n = strlen(u.host) + strlen(u.port) + strlen(location) + 16;
                next_url = (char *)malloc(n);
                if (next_url) snprintf(next_url, n, "%s://%s:%s%s", u.tls ? "https" : "http", u.host, u.port, location);
            } else if (location[0] == '?') {
                const char *slash = strrchr(u.target, '?');
                size_t path_len = slash ? (size_t)(slash-u.target) : strlen(u.target);
                size_t n = strlen(u.host) + strlen(u.port) + path_len + strlen(location) + 16;
                next_url = (char *)malloc(n);
                if (next_url) snprintf(next_url, n, "%s://%s:%s%.*s%s", u.tls ? "https" : "http", u.host, u.port, (int)path_len, u.target, location);
            } else {
                const char *slash = strrchr(u.target, '/');
                size_t path_len = slash ? (size_t)(slash-u.target+1) : 1;
                size_t n = strlen(u.host) + strlen(u.port) + path_len + strlen(location) + 16;
                next_url = (char *)malloc(n);
                if (next_url) snprintf(next_url, n, "%s://%s:%s%.*s%s", u.tls ? "https" : "http", u.host, u.port, (int)path_len, u.target, location);
            }
            free(location);
            free(raw);
            if (ssl) { SSL_shutdown(ssl); SSL_free(ssl); }
            if (ctx) SSL_CTX_free(ctx);
            close(fd); parsed_url_free(&u);
            if (!next_url) return NULL;
            const char *next_method = method ? method : "GET";
            const char *next_body = body ? body : "";
            if (code == 303 || ((code == 301 || code == 302) && !strcasecmp(next_method, "POST"))) {
                next_method = "GET";
                next_body = "";
            }
            char *safe_headers = NULL;
            const char *next_headers = headers;
            if (!same_origin(url, next_url)) {
                safe_headers = headers_without_credentials(headers);
                if (!safe_headers) { free(next_url); return NULL; }
                next_headers = safe_headers;
            }
            char *redirected = vnt_native_http_request_impl(next_method, next_url, next_headers, next_body,
                                                             status, response_length, redirect_count + 1);
            free(safe_headers);
            free(next_url);
            return redirected;
        }
        free(location);
    }
    if (status) *status = code;
    char *result = NULL;
    if (strcasestr(raw, "Transfer-Encoding: chunked") && bodylen_read) {
        result = decode_chunked(sep+4, bodylen_read, response_length);
    } else {
        result = net_copy(sep+4, bodylen_read);
        if (response_length) *response_length = bodylen_read;
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

char *vnt_native_http_request(const char *method, const char *url,
                      const char *headers, const char *body, int *status, size_t *response_length) {
    return vnt_native_http_request_impl(method, url, headers, body, status, response_length, 0);
}
#endif
