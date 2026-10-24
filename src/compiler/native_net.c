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
    if (!body || length == SIZE_MAX) return NULL;
    char *out = (char *)malloc(length + 1);
    if (!out) return NULL;
    size_t r = 0, w = 0;
    for (;;) {
        /* Chunk-size lines and delimiters must use HTTP's CRLF framing. */
        size_t line_start = r;
        while (r + 1 < length && !(body[r] == '\r' && body[r + 1] == '\n')) {
            if (body[r] == '\r' || body[r] == '\n' || r - line_start > 8192) {
                free(out); return NULL;
            }
            r++;
        }
        if (r + 1 >= length) { free(out); return NULL; }
        size_t line_end = r;
        r += 2;
        size_t digits_end = line_start;
        while (digits_end < line_end && body[digits_end] != ';') digits_end++;
        if (digits_end == line_start) { free(out); return NULL; }
        size_t chunk = 0;
        for (size_t i = line_start; i < digits_end; ++i) {
            unsigned char c = (unsigned char)body[i];
            unsigned int v;
            if (c >= '0' && c <= '9') v = c - '0';
            else if (c >= 'a' && c <= 'f') v = c - 'a' + 10;
            else if (c >= 'A' && c <= 'F') v = c - 'A' + 10;
            else { free(out); return NULL; }
            if (chunk > (SIZE_MAX - v) / 16) { free(out); return NULL; }
            chunk = chunk * 16 + v;
        }
        if (chunk == 0) {
            /* Consume trailer fields through the mandatory empty CRLF line. */
            for (;;) {
                size_t trailer_start = r;
                while (r + 1 < length && !(body[r] == '\r' && body[r + 1] == '\n')) {
                    if (body[r] == '\r' || body[r] == '\n' || r - trailer_start > 8192) {
                        free(out); return NULL;
                    }
                    r++;
                }
                if (r + 1 >= length) { free(out); return NULL; }
                size_t trailer_len = r - trailer_start;
                r += 2;
                if (!trailer_len) {
                    out[w] = '\0';
                    if (decoded_length) *decoded_length = w;
                    return out;
                }
                const char *colon = (const char *)memchr(body + trailer_start, ':', trailer_len);
                if (!colon || colon == body + trailer_start) { free(out); return NULL; }
                for (const char *p = body + trailer_start; p < colon; ++p)
                    if (!http_token_char((unsigned char)*p)) { free(out); return NULL; }
                for (const char *p = colon + 1; p < body + trailer_start + trailer_len; ++p) {
                    unsigned char c = (unsigned char)*p;
                    if ((c < 0x20 && c != '\t') || c == 0x7f) { free(out); return NULL; }
                }
            }
        }
        if (chunk > length - r || length - r - chunk < 2) { free(out); return NULL; }
        memcpy(out + w, body + r, chunk);
        w += chunk;
        r += chunk;
        if (body[r] != '\r' || body[r + 1] != '\n') { free(out); return NULL; }
        r += 2;
    }
}

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>

/* A process-wide WinHTTP session enables its built-in connection pooling.
   Individual request and connection handles remain scoped to each call. */
static HINTERNET vnt_shared_http_session;

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
                      const char *headers, const char *body, size_t body_length,
                      int *status, size_t *response_length, unsigned int redirect_count) {
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
    HINTERNET session = (HINTERNET)InterlockedCompareExchangePointer(
        (PVOID volatile *)&vnt_shared_http_session, NULL, NULL);
    if (!session) {
        HINTERNET created = WinHttpOpen(L"VNT/0.1", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!created) { free(wide_url); free(wide_method); free(hostname); free(path); return NULL; }
        HINTERNET existing = (HINTERNET)InterlockedCompareExchangePointer(
            (PVOID volatile *)&vnt_shared_http_session, created, NULL);
        if (existing) {
            WinHttpCloseHandle(created);
            session = existing;
        } else {
            session = created;
            WinHttpSetTimeouts(session, 10000, 10000, 30000, 30000);
        }
    }
    HINTERNET connection = session ? WinHttpConnect(session, hostname, parts.nPort, 0) : NULL;
    DWORD flags = parts.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0;
    HINTERNET request = connection ? WinHttpOpenRequest(connection, wide_method,
        path, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags) : NULL;
    free(path); free(hostname); free(wide_url); free(wide_method);
    if (!request) {
        if (connection) WinHttpCloseHandle(connection);
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
    size_t body_size = body ? body_length : 0;
    if (body_size > MAXDWORD) {
        WinHttpCloseHandle(request);
        if (connection) WinHttpCloseHandle(connection);
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
                    next_body, next_body == body ? body_length : 0, status, response_length, redirect_count + 1);
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
    if (!ok) { free(result); return NULL; }
    if (!result) result = net_copy("", 0);
    return result;
}

char *vnt_native_http_request(const char *method, const char *url,
                      const char *headers, const char *body, int *status, size_t *response_length) {
    return vnt_native_http_request_impl(method, url, headers, body, body ? strlen(body) : 0, status, response_length, 0);
}

char *vnt_native_http_request_bytes(const char *method, const char *url,
                      const char *headers, const char *body, size_t body_length,
                      int *status, size_t *response_length) {
    return vnt_native_http_request_impl(method, url, headers, body, body_length, status, response_length, 0);
}

/* Stream successful downloads directly to disk. Redirects fall back to the
   regular request path so its redirect and credential rules remain identical. */
int vnt_native_http_download_stream(const char *url, const char *destination,
                                    const char *headers, int *status) {
    if (status) *status = 0;
    if (!valid_http_url_chars(url) || !valid_request_headers(headers) ||
        !destination || !*destination) return 0;
    wchar_t *wide_url = to_wide(url);
    if (!wide_url) return 0;
    URL_COMPONENTS parts;
    memset(&parts, 0, sizeof(parts));
    parts.dwStructSize = sizeof(parts);
    parts.dwSchemeLength = parts.dwHostNameLength = parts.dwUrlPathLength =
        parts.dwExtraInfoLength = (DWORD)-1;
    if (!WinHttpCrackUrl(wide_url, 0, 0, &parts)) { free(wide_url); return 0; }
    wchar_t *host = (wchar_t *)calloc((size_t)parts.dwHostNameLength + 1, sizeof(wchar_t));
    size_t plen = (size_t)parts.dwUrlPathLength + (size_t)parts.dwExtraInfoLength;
    wchar_t *path = (wchar_t *)calloc(plen + 2, sizeof(wchar_t));
    if (!host || !path) { free(host); free(path); free(wide_url); return 0; }
    memcpy(host, parts.lpszHostName, (size_t)parts.dwHostNameLength * sizeof(wchar_t));
    size_t at = 0;
    if (parts.dwUrlPathLength) {
        memcpy(path, parts.lpszUrlPath, (size_t)parts.dwUrlPathLength * sizeof(wchar_t));
        at = (size_t)parts.dwUrlPathLength;
    } else path[at++] = L'/';
    if (parts.dwExtraInfoLength) {
        memcpy(path + at, parts.lpszExtraInfo, (size_t)parts.dwExtraInfoLength * sizeof(wchar_t));
        at += (size_t)parts.dwExtraInfoLength;
    }
    path[at] = L'\0';
    wchar_t *fragment = wcschr(path, L'#');
    if (fragment) *fragment = L'\0';
    HINTERNET session = (HINTERNET)InterlockedCompareExchangePointer(
        (PVOID volatile *)&vnt_shared_http_session, NULL, NULL);
    if (!session) {
        HINTERNET created = WinHttpOpen(L"VNT/0.1", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!created) { free(host); free(path); free(wide_url); return 0; }
        HINTERNET existing = (HINTERNET)InterlockedCompareExchangePointer(
            (PVOID volatile *)&vnt_shared_http_session, created, NULL);
        if (existing) { WinHttpCloseHandle(created); session = existing; }
        else {
            session = created;
            WinHttpSetTimeouts(session, 10000, 10000, 30000, 30000);
        }
    }
    HINTERNET connection = WinHttpConnect(session, host, parts.nPort, 0);
    HINTERNET request = connection ? WinHttpOpenRequest(connection, L"GET", path, NULL,
        WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
        parts.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0) : NULL;
    free(host); free(path); free(wide_url);
    if (!request) { if (connection) WinHttpCloseHandle(connection); return 0; }
    DWORD no_redirect = WINHTTP_OPTION_REDIRECT_POLICY_NEVER;
    WinHttpSetOption(request, WINHTTP_OPTION_REDIRECT_POLICY, &no_redirect, sizeof(no_redirect));
    char *copy = headers ? _strdup(headers) : NULL, *save = NULL;
    if (copy) {
        for (char *line = strtok_s(copy, "\n", &save); line; line = strtok_s(NULL, "\n", &save)) {
            while (*line == ' ') line++;
            size_t len = strlen(line);
            while (len && line[len-1] == '\r') line[--len] = '\0';
            if (!len || reserved_request_header(line, len)) continue;
            wchar_t *wh = to_wide(line);
            if (wh) { WinHttpAddRequestHeaders(request, wh, (DWORD)-1, WINHTTP_ADDREQ_FLAG_ADD); free(wh); }
        }
        free(copy);
    }
    BOOL ok = WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        WINHTTP_NO_REQUEST_DATA, 0, 0, 0) && WinHttpReceiveResponse(request, NULL);
    DWORD code = 0, code_size = sizeof(code);
    if (ok) WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX, &code, &code_size, WINHTTP_NO_HEADER_INDEX);
    if (status) *status = (int)code;
    if (ok && (code == 301 || code == 302 || code == 303 || code == 307 || code == 308)) {
        WinHttpCloseHandle(request);
        if (connection) WinHttpCloseHandle(connection);
        size_t response_length = 0; int redirected_status = 0;
        char *response = vnt_native_http_request("GET", url, headers, "", &redirected_status, &response_length);
        if (status) *status = redirected_status;
        if (!response || redirected_status < 200 || redirected_status >= 300) { free(response); return 0; }
        FILE *f = fopen(destination, "wb");
        int wrote = f && fwrite(response, 1, response_length, f) == response_length;
        if (f && fclose(f) != 0) wrote = 0;
        free(response);
        return wrote;
    }
    FILE *out = ok && code >= 200 && code < 300 ? fopen(destination, "wb") : NULL;
    if (!out) ok = FALSE;
    char buffer[65536];
    while (ok) {
        DWORD available = 0;
        if (!WinHttpQueryDataAvailable(request, &available)) { ok = FALSE; break; }
        if (!available) break;
        DWORD received = 0;
        if (!WinHttpReadData(request, buffer, (DWORD)(available > sizeof(buffer) ? sizeof(buffer) : available), &received)) {
            ok = FALSE; break;
        }
        if (received && fwrite(buffer, 1, received, out) != received) { ok = FALSE; break; }
    }
    if (out && fclose(out) != 0) ok = FALSE;
    WinHttpCloseHandle(request);
    if (connection) WinHttpCloseHandle(connection);
    return ok ? 1 : 0;
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

static int proxy_bypasses_host(const char *host, const char *list) {
    if (!host || !list) return 0;
    size_t host_len = strlen(host);
    const char *p = list;
    while (*p) {
        while (*p == ' ' || *p == '\t' || *p == ',') p++;
        const char *end = p;
        while (*end && *end != ',') end++;
        const char *trim = end;
        while (trim > p && (trim[-1] == ' ' || trim[-1] == '\t')) trim--;
        size_t n = (size_t)(trim - p);
        if (n == 1 && p[0] == '*') return 1;
        if (n && p[0] == '.') {
            if (host_len >= n && !strncasecmp(host + host_len - n, p, n)) return 1;
        } else if (n == host_len && !strncasecmp(host, p, n)) return 1;
        p = *end ? end + 1 : end;
    }
    return 0;
}

static const char *proxy_environment_for(int tls) {
    const char *value = getenv(tls ? "HTTPS_PROXY" : "HTTP_PROXY");
    if (!value || !*value) value = getenv(tls ? "https_proxy" : "http_proxy");
    if (!value || !*value) value = getenv("ALL_PROXY");
    if (!value || !*value) value = getenv("all_proxy");
    return value && *value ? value : NULL;
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
                      const char *headers, const char *body, size_t body_length,
                      int *status, size_t *response_length, unsigned int redirect_count) {
    if (status) *status = 0;
    if (response_length) *response_length = 0;
    if (!valid_request_headers(headers)) return NULL;
    ParsedUrl u, proxy;
    memset(&proxy, 0, sizeof(proxy));
    if (!parse_url(url ? url : "", &u)) return NULL;
    const char *proxy_text = proxy_environment_for(u.tls);
    const char *no_proxy = getenv("NO_PROXY");
    if (!no_proxy || !*no_proxy) no_proxy = getenv("no_proxy");
    int use_proxy = proxy_text && !proxy_bypasses_host(u.host, no_proxy);
    char *proxy_url = NULL;
    if (use_proxy) {
        if (strstr(proxy_text, "://")) proxy_url = strdup(proxy_text);
        else {
            size_t n = strlen(proxy_text) + sizeof("http://");
            proxy_url = (char *)malloc(n);
            if (proxy_url) snprintf(proxy_url, n, "http://%s", proxy_text);
        }
        if (!proxy_url || !parse_url(proxy_url, &proxy) || proxy.tls) {
            free(proxy_url); parsed_url_free(&proxy); parsed_url_free(&u); return NULL;
        }
    }
    struct addrinfo hints, *addresses = NULL;
    memset(&hints, 0, sizeof(hints)); hints.ai_socktype = SOCK_STREAM; hints.ai_family = AF_UNSPEC;
    const char *connect_host = use_proxy ? proxy.host : u.host;
    const char *connect_port = use_proxy ? proxy.port : u.port;
    if (getaddrinfo(connect_host, connect_port, &hints, &addresses) != 0) {
        free(proxy_url); parsed_url_free(&proxy); parsed_url_free(&u); return NULL;
    }
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
    if (fd < 0) { free(proxy_url); parsed_url_free(&proxy); parsed_url_free(&u); return NULL; }
    if (use_proxy && u.tls) {
        size_t need = strlen(u.host) + strlen(u.port) + 128;
        char *connect_request = (char *)malloc(need);
        if (!connect_request) { close(fd); free(proxy_url); parsed_url_free(&proxy); parsed_url_free(&u); return NULL; }
        int cn = snprintf(connect_request, need, "CONNECT %s:%s HTTP/1.1\r\nHost: %s:%s\r\nProxy-Connection: keep-alive\r\n\r\n",
                          u.host, u.port, u.host, u.port);
        int sent = cn > 0 && (size_t)cn < need && write_all(fd, NULL, connect_request, (size_t)cn);
        free(connect_request);
        char response_headers[16385];
        response_headers[0] = '\0';
        size_t hn = 0;
        while (sent && hn < sizeof(response_headers) - 1) {
            char ch;
            ssize_t got = recv(fd, &ch, 1, 0);
            if (got != 1) { sent = 0; break; }
            response_headers[hn++] = ch;
            response_headers[hn] = '\0';
            if (hn >= 4 && !memcmp(response_headers + hn - 4, "\r\n\r\n", 4)) break;
        }
        int proxy_status = 0;
        if (sscanf(response_headers, "HTTP/%*u.%*u %d", &proxy_status) != 1 ||
            proxy_status != 200 || hn < 4 ||
            memcmp(response_headers + hn - 4, "\r\n\r\n", 4) != 0) {
            close(fd); free(proxy_url); parsed_url_free(&proxy); parsed_url_free(&u); return NULL;
        }
    }
    free(proxy_url);
    parsed_url_free(&proxy);
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
                       headers ? strlen(headers) : 0, body ? body_length : 0 };
    for (size_t i = 0; i < sizeof(parts)/sizeof(parts[0]); ++i) {
        if (parts[i] > SIZE_MAX - reqcap) goto fail;
        reqcap += parts[i];
    }
    char *request = (char *)malloc(reqcap);
    if (!request) goto fail;
    int n;
    if (use_proxy && !u.tls) {
        n = snprintf(request, reqcap, "%s http://%s:%s%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n",
                     method ? method : "GET", u.host, u.port, u.target, u.host);
    } else {
        n = snprintf(request, reqcap, "%s %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n",
                     method ? method : "GET", u.target, u.host);
    }
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
    size_t bodylen = body ? body_length : 0;
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
    return vnt_native_http_request_impl(method, url, headers, body, body ? strlen(body) : 0, status, response_length, 0);
}

char *vnt_native_http_request_bytes(const char *method, const char *url,
                      const char *headers, const char *body, size_t body_length,
                      int *status, size_t *response_length) {
    return vnt_native_http_request_impl(method, url, headers, body, body_length, status, response_length, 0);
}

static int stream_read(int fd, SSL *ssl, char *buffer, size_t capacity, size_t *received) {
    int n;
    do {
        n = ssl ? SSL_read(ssl, buffer, (int)(capacity > INT32_MAX ? INT32_MAX : capacity))
                : (int)recv(fd, buffer, capacity > INT32_MAX ? INT32_MAX : capacity, 0);
    } while (n < 0 && errno == EINTR);
    if (n <= 0) return n == 0 ? 0 : -1;
    *received = (size_t)n;
    return 1;
}

static int stream_read_exact(int fd, SSL *ssl, FILE *out, size_t length) {
    char buffer[65536];
    while (length) {
        size_t got = 0, want = length < sizeof(buffer) ? length : sizeof(buffer);
        int r = stream_read(fd, ssl, buffer, want, &got);
        if (r != 1 || !got || fwrite(buffer, 1, got, out) != got) return 0;
        length -= got;
    }
    return 1;
}

static int stream_read_line(int fd, SSL *ssl, char *line, size_t capacity) {
    size_t used = 0;
    while (used + 1 < capacity) {
        char ch; size_t got = 0;
        int r = stream_read(fd, ssl, &ch, 1, &got);
        if (r != 1 || got != 1) return 0;
        line[used++] = ch;
        if (used >= 2 && line[used-2] == '\r' && line[used-1] == '\n') {
            line[used-2] = '\0';
            return 1;
        }
    }
    return 0;
}

/* Unix-like streaming download: response bytes are written incrementally,
   including chunked decoding, so large files do not need a response-sized heap buffer. */
int vnt_native_http_download_stream(const char *url, const char *destination,
                                    const char *headers, int *status) {
    if (status) *status = 0;
    if (!destination || !*destination || !valid_request_headers(headers)) return 0;
    /* Proxying and redirects use the fully featured request path as a safe fallback. */
    const char *proxy = proxy_environment_for(url && !strncasecmp(url, "https://", 8));
    const char *no_proxy = getenv("NO_PROXY");
    if (!no_proxy || !*no_proxy) no_proxy = getenv("no_proxy");
    ParsedUrl parsed;
    if (!parse_url(url ? url : "", &parsed)) return 0;
    int fallback = proxy && !proxy_bypasses_host(parsed.host, no_proxy);
    parsed_url_free(&parsed);
    if (fallback) {
        size_t length = 0; int code = 0;
        char *body = vnt_native_http_request("GET", url, headers, "", &code, &length);
        if (status) *status = code;
        if (!body || code < 200 || code >= 300) { free(body); return 0; }
        FILE *f = fopen(destination, "wb");
        int ok = f && fwrite(body, 1, length, f) == length;
        if (f && fclose(f) != 0) ok = 0;
        free(body); return ok;
    }
    ParsedUrl u;
    if (!parse_url(url ? url : "", &u)) return 0;
    struct addrinfo hints, *addresses = NULL;
    memset(&hints, 0, sizeof(hints)); hints.ai_socktype = SOCK_STREAM; hints.ai_family = AF_UNSPEC;
    if (getaddrinfo(u.host, u.port, &hints, &addresses) != 0) { parsed_url_free(&u); return 0; }
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
    if (fd < 0) { parsed_url_free(&u); return 0; }
    SSL_CTX *ctx = NULL; SSL *ssl = NULL;
    if (u.tls) {
        ctx = SSL_CTX_new(TLS_client_method());
        if (!ctx || SSL_CTX_set_default_verify_paths(ctx) != 1) goto stream_fail;
        SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
        ssl = SSL_new(ctx);
        if (!ssl || SSL_set_tlsext_host_name(ssl, u.host) != 1 ||
            SSL_set1_host(ssl, u.host) != 1) goto stream_fail;
        SSL_set_fd(ssl, fd);
        if (SSL_connect(ssl) != 1) goto stream_fail;
    }
    size_t cap = 1024 + (headers ? strlen(headers) : 0);
    char *request = (char *)malloc(cap);
    if (!request) goto stream_fail;
    int n = snprintf(request, cap, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n",
                     u.target, u.host);
    if (n < 0 || (size_t)n >= cap) { free(request); goto stream_fail; }
    size_t used = (size_t)n;
    const char *hp = headers ? headers : "";
    while (*hp) {
        const char *end = strchr(hp, '\n');
        size_t len = end ? (size_t)(end-hp) : strlen(hp);
        while (len && hp[len-1] == '\r') len--;
        if (len && !reserved_request_header(hp, len)) {
            if (len > cap - used - 4) { free(request); goto stream_fail; }
            memcpy(request+used, hp, len); used += len;
            memcpy(request+used, "\r\n", 2); used += 2;
        }
        if (!end) break;
        hp = end + 1;
    }
    if (used + 2 >= cap) { free(request); goto stream_fail; }
    memcpy(request+used, "\r\n", 2); used += 2;
    int sent = write_all(fd, ssl, request, used);
    free(request);
    if (!sent) goto stream_fail;
    char header[65537]; size_t hlen = 0;
    while (hlen + 1 < sizeof(header)) {
        char ch; size_t got = 0;
        int r = stream_read(fd, ssl, &ch, 1, &got);
        if (r != 1 || got != 1) goto stream_fail;
        header[hlen++] = ch; header[hlen] = '\0';
        if (hlen >= 4 && !memcmp(header + hlen - 4, "\r\n\r\n", 4)) break;
    }
    if (hlen < 4 || memcmp(header + hlen - 4, "\r\n\r\n", 4)) goto stream_fail;
    int code = 0, chunked = 0, has_length = 0, connection_close = 0;
    unsigned long long content_length = 0;
    if (sscanf(header, "HTTP/%*u.%*u %d", &code) != 1) goto stream_fail;
    char *line = strstr(header, "\r\n");
    while (line && *line) {
        line += 2;
        if (!strncmp(line, "\r\n", 2)) break;
        char *end = strstr(line, "\r\n");
        if (!end) break;
        if ((size_t)(end-line) >= 15 && !strncasecmp(line, "Content-Length:", 15)) {
            char *v = line+15; while (*v == ' ' || *v == '\t') v++;
            char *tail = NULL; content_length = strtoull(v, &tail, 10);
            while (tail && tail < end && (*tail == ' ' || *tail == '\t')) tail++;
            if (!tail || tail != end) goto stream_fail;
            has_length = 1;
        } else if ((size_t)(end-line) >= 18 && !strncasecmp(line, "Transfer-Encoding:", 18) &&
                   strcasestr(line+18, "chunked")) chunked = 1;
        else if ((size_t)(end-line) >= 11 && !strncasecmp(line, "Connection:", 11) &&
                 strcasestr(line+11, "close")) connection_close = 1;
        line = end;
    }
    if (has_length && chunked) goto stream_fail;
    if (status) *status = code;
    if (code >= 300 && code < 400) {
        if (ssl) { SSL_shutdown(ssl); SSL_free(ssl); }
        if (ctx) SSL_CTX_free(ctx);
        close(fd); parsed_url_free(&u);
        size_t length = 0; int redirected_status = 0;
        char *body = vnt_native_http_request("GET", url, headers, "", &redirected_status, &length);
        if (status) *status = redirected_status;
        if (!body || redirected_status < 200 || redirected_status >= 300) { free(body); return 0; }
        FILE *f = fopen(destination, "wb");
        int ok = f && fwrite(body, 1, length, f) == length;
        if (f && fclose(f) != 0) ok = 0;
        free(body); return ok;
    }
    if (code < 200 || code >= 300) goto stream_fail;
    FILE *out = fopen(destination, "wb");
    if (!out) goto stream_fail;
    int ok = 1;
    if (chunked) {
        char chunk_line[8193];
        for (;;) {
            if (!stream_read_line(fd, ssl, chunk_line, sizeof(chunk_line))) { ok = 0; break; }
            char *semi = strchr(chunk_line, ';'); if (semi) *semi = '\0';
            if (!*chunk_line) { ok = 0; break; }
            char *tail = NULL; unsigned long long amount = strtoull(chunk_line, &tail, 16);
            if (!tail || *tail || amount > SIZE_MAX) { ok = 0; break; }
            if (!amount) {
                for (;;) {
                    if (!stream_read_line(fd, ssl, chunk_line, sizeof(chunk_line))) { ok = 0; break; }
                    if (!*chunk_line) break;
                    if (!strchr(chunk_line, ':')) { ok = 0; break; }
                }
                break;
            }
            if (!stream_read_exact(fd, ssl, out, (size_t)amount)) { ok = 0; break; }
            char ending[2]; size_t got = 0;
            if (stream_read(fd, ssl, ending, 1, &got) != 1 || got != 1 || ending[0] != '\r' ||
                stream_read(fd, ssl, ending+1, 1, &got) != 1 || got != 1 || ending[1] != '\n') { ok = 0; break; }
        }
    } else if (has_length) {
        if (content_length > SIZE_MAX) ok = 0;
        else ok = stream_read_exact(fd, ssl, out, (size_t)content_length);
    } else {
        char buffer[65536];
        for (;;) {
            size_t got = 0; int r = stream_read(fd, ssl, buffer, sizeof(buffer), &got);
            if (r == 0) break;
            if (r < 0 || fwrite(buffer, 1, got, out) != got) { ok = 0; break; }
        }
    }
    if (fclose(out) != 0) ok = 0;
    if (ssl) { SSL_shutdown(ssl); SSL_free(ssl); }
    if (ctx) SSL_CTX_free(ctx);
    close(fd); parsed_url_free(&u);
    return ok;
stream_fail:
    if (ssl) SSL_free(ssl);
    if (ctx) SSL_CTX_free(ctx);
    close(fd); parsed_url_free(&u);
    return 0;
}
#endif
