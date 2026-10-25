# Tutorial 8: Native HTTP APIs

VNT's `lib/std/net.vnt` uses the compiler's native C networking runtime. It does not start `curl` or another external process.

## 1. Make a GET request

Create `api_demo.vnt` in the repository root:

```vnt
import "lib/std/net.vnt" as net

url = "https://api.github.com/repos/ItsDropper/VNT"
code = net.status(url)
print("HTTP status: " + code)

response = net.get(url)
if response == "" {
    print("No response body was returned.")
} else {
    print(response)
}
```

Compile and run:

```powershell
.\vnt.exe --compile api_demo.vnt -o api_demo.exe
.\api_demo.exe
```

This sends a real network request when the program runs. Windows uses WinHTTP; Unix-like targets use native sockets with OpenSSL for HTTPS. TLS certificate and hostname verification are enabled.

## 2. Send headers and JSON

```vnt
import "lib/std/net.vnt" as net

response = net.get_with_header(
    "https://api.github.com/repos/ItsDropper/VNT",
    "Accept: application/vnd.github+json"
)
print(response)

created = net.post_json(
    "https://example.com/api",
    "{\"ready\":true}"
)
print(created)
```

Bearer-token variants are available for GET, POST, PUT, PATCH and DELETE. Keep credentials out of source control and logs.

## 3. Use a custom method or header set

`net.request(method, url, headers, body)` exposes the native request path for methods and headers not covered by the convenience wrappers. Supply headers as newline-separated `Name: value` lines. VNT filters caller-supplied framing headers (`Host`, `Content-Length`, `Transfer-Encoding`, and connection headers) so they cannot override the transport's framing.

```vnt
response = net.request("PATCH", "https://example.com/api/item", "Content-Type: application/json", "{\"enabled\":true}")
print(response)
```

## 4. Send and receive arbitrary bytes

VNT uses a compact byte-buffer value for binary responses, preserving embedded NUL bytes and invalid UTF-8. You can index it like an array, call `len()`, and pass it directly to another binary request. `request_bytes` also accepts an ordinary array of integer bytes (`0` through `255`):

```vnt
import "lib/std/net.vnt" as net

response_bytes = net.get_bytes("https://example.com/image.bin")
print(len(response_bytes))

echoed = net.request_bytes(
    "POST",
    "https://example.com/byte-endpoint",
    "Content-Type: application/octet-stream",
    [0, 1, 255, 10, 13, 128]
)
print(echoed)
```

`request_bytes` accepts either an integer array or a compact byte buffer as its body and returns a compact byte buffer. Use ordinary `get`/`request` for text-oriented responses.

## 5. Understand status codes and failures

An HTTP server can return `404 Not Found` even though the connection worked. Use `net.status(url)` to inspect the HTTP status. The helper returns a three-digit string; `000` indicates that the transport failed before an HTTP status was received.

Body helpers return an empty string on transport failure, so an empty response is ambiguous. `net.download` preserves binary bytes and writes successful 2xx bodies to disk. Direct download responses stream in bounded chunks, but redirect/proxy fallback paths may buffer the response. Use it instead of `get`/`request` for large files. The ordinary `get`/`request` APIs return the complete response in memory, so response size is limited by available memory.

## 6. Behavior and remaining gaps

The native transport supports HTTP and HTTPS, standard request methods, newline-separated headers, text request bodies, status codes, redirects, and binary-safe downloads. Redirect loops are capped at ten hops; cross-origin redirects strip `Authorization`, `Proxy-Authorization`, and `Cookie` headers. The in-memory `get`/`request` path is buffered because it returns a complete response. Direct downloads stream in bounded chunks, while redirect/proxy fallback paths may buffer; do not assume every download route has constant memory use. Binary helpers return compact byte buffers that support `len()`, indexing, and reuse as a subsequent binary request body. Unix-like requests honor HTTP_PROXY/HTTPS_PROXY (and lowercase equivalents), ALL_PROXY, and NO_PROXY. Windows delegates proxy selection to WinHTTP.

The Unix-like HTTPS implementation links against OpenSSL and verifies the peer certificate and hostname. Windows uses the platform WinHTTP stack and its certificate validation.

## Exercises

1. Call a public JSON endpoint and print its status before its response.
2. Add an `Accept` header using `get_with_header`.
3. Send a JSON body to a local test endpoint with `post_json`.
4. Explain why a 404 response is different from a DNS or TLS failure.

Next: [Return to the tutorial index](README.md).
