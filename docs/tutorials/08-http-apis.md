# Tutorial 8: HTTP APIs and External Tools

VNT's `lib/std/net.vnt` provides HTTP helpers backed by the system `curl` executable. This is useful for learning API integration, but it is not a built-in HTTP stack.

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

This sends a network request when you run the program. It requires `curl.exe` to be available on `PATH`. The helper follows redirects and uses a timeout; TLS certificate verification remains enabled.

## 2. Transport success is not the same as HTTP success

An HTTP server can return a response such as `404 Not Found` even though the network request itself worked. Use `net.status(url)` to inspect the HTTP status rather than assuming every non-empty body means success.

The current helper reports an empty string on some transport failures. That means an empty result is ambiguous: it may be a failed request or an empty response. Inspect the module implementation if your program needs more precise diagnostics.

## 3. Send a request with a header

For APIs that expect a particular media type or a user-agent header, use the header helper:

```vnt
import "lib/std/net.vnt" as net

response = net.get_with_header(
    "https://api.github.com/repos/ItsDropper/VNT",
    "Accept: application/vnd.github+json"
)
print(response)
```

The API takes the URL and header as separate arguments. The helper invokes `curl` with an argument array rather than constructing a shell command.

## 4. Work with JSON carefully

VNT includes `lib/std/json.vnt`, but its current API is intentionally small. `json.value_at(json_text, key, occurrence)` retrieves a matching field value; it is not a complete JSON document model, and it does not provide a general JSON serializer.

For production API clients, do not assume this helper handles every nested or repeated JSON shape. Read `lib/std/json.vnt` and the native implementation before relying on it for complex responses. For this lesson, print the raw response first so you can inspect the actual payload.

## 5. Keep network behavior explicit

A program should make network requests at a clear point in its control flow. Avoid hiding requests inside unrelated initialization or UI rendering code. For API clients, also consider:

- Handling non-success HTTP status codes.
- Distinguishing transport errors from valid empty responses.
- Avoiding printing or logging access tokens.
- Not sending secrets to URLs or public logs.
- Applying timeouts and validating the returned data before using it.

The standard library has helpers such as `get`, `get_json`, `get_with_header`, `post_json`, and `status`. Check `lib/std/net.vnt` for the current signatures and behavior.

## Exercises

1. Change the URL to another public endpoint that returns a small response.
2. Print the status before printing the response body.
3. Display a friendly message when the status is not `200`.
4. Add `log.info` to record that a request started, but do not log secrets or full private responses.
5. Explain why HTTP status and process exit code are different kinds of result.

Next: [Return to the tutorial index](README.md).
