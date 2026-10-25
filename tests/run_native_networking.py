#!/usr/bin/env python3
"""Offline integration tests for VNT's native HTTP runtime."""
import http.server
import os
import pathlib
import subprocess
import ssl
import sys
import tempfile
import threading
import time

ROOT = pathlib.Path(__file__).resolve().parents[1]
HOST = "127.0.0.1"
PORT = 18765
REDIRECT_PORT = 18766

class ProxyHandler(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        if not self.path.startswith("http://proxy-test.invalid:80/"):
            self.send_error(400, "expected absolute-form proxy request")
            return
        payload = b"PROXY_OK"
        self.send_response(200)
        self.send_header("Content-Length", str(len(payload)))
        self.send_header("Connection", "close")
        self.end_headers()
        self.wfile.write(payload)

    def log_message(self, *_args):
        pass

LAST_KEEPALIVE_CONNECTION = None

TLS_LAST_CONNECTION = None

class TLSHandler(http.server.BaseHTTPRequestHandler):
    protocol_version = "HTTP/1.1"

    def do_GET(self):
        global TLS_LAST_CONNECTION
        if self.path == "/tls-keepalive":
            TLS_LAST_CONNECTION = self.connection
            payload = b"TLS_READY"
        elif self.path == "/tls-pool-check":
            payload = b"TLS_POOL_REUSED" if self.connection is TLS_LAST_CONNECTION else b"TLS_POOL_NEW"
        else:
            payload = b"TLS_UNKNOWN_PATH"
        self.close_connection = False
        self.send_response(200)
        self.send_header("Content-Length", str(len(payload)))
        self.send_header("Connection", "keep-alive")
        self.end_headers()
        self.wfile.write(payload)
        self.wfile.flush()

    def log_message(self, *_args):
        pass

class Handler(http.server.BaseHTTPRequestHandler):
    protocol_version = "HTTP/1.1"

    def _send(self, status, body):
        payload = body.encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Length", str(len(payload)))
        self.send_header("Connection", "close")
        self.end_headers()
        if payload:
            self.wfile.write(payload)

    def do_GET(self):
        if self.path == "/hello":
            self._send(200, "VNT_NATIVE_HTTP_OK")
        elif self.path == "/query?x=1":
            self._send(200, "FRAGMENT_NOT_SENT")
        elif self.path == "/keepalive":
            global LAST_KEEPALIVE_CONNECTION
            self.close_connection = False
            LAST_KEEPALIVE_CONNECTION = self.connection
            payload = b"CONTENT_LENGTH_OK"
            self.send_response(200)
            self.send_header("Content-Length", str(len(payload)))
            self.send_header("Connection", "keep-alive")
            self.end_headers()
            self.wfile.write(payload)
        elif self.path == "/pool-check":
            self._send(200, "POOL_REUSED" if self.connection is LAST_KEEPALIVE_CONNECTION else "POOL_NEW")
        elif self.path == "/redirect":
            self.send_response(302)
            self.send_header("Location", "/hello")
            self.send_header("Content-Length", "0")
            self.send_header("Connection", "close")
            self.end_headers()
        elif self.path == "/redirect-auth":
            self.send_response(302)
            self.send_header("Location", "http://127.0.0.1:18766/check-auth")
            self.send_header("Content-Length", "0")
            self.send_header("Connection", "close")
            self.end_headers()
        elif self.path == "/check-auth":
            self._send(200, "AUTH_STRIPPED" if not self.headers.get("Authorization") and not self.headers.get("Cookie") else "AUTH_LEAKED")
        elif self.path == "/chunked":
            self.send_response(200)
            self.send_header("Transfer-Encoding", "chunked")
            self.send_header("Connection", "close")
            self.end_headers()
            for chunk in (b"CHUNK", b"ED_OK"):
                self.wfile.write(f"{len(chunk):X}\r\n".encode("ascii") + chunk + b"\r\n")
            self.wfile.write(b"0\r\n\r\n")
        elif self.path == "/conflicting-length":
            self.send_response(200)
            self.send_header("Content-Length", "3")
            self.send_header("Content-Length", "4")
            self.send_header("Connection", "close")
            self.end_headers()
            self.wfile.write(b"test")
        elif self.path == "/mentions-framing":
            self._send(200, "Transfer-Encoding: chunked is body text")
        elif self.path == "/bad-chunked":
            self.send_response(200)
            self.send_header("Transfer-Encoding", "chunked")
            self.send_header("Connection", "close")
            self.end_headers()
            self.wfile.write(b"3\r\nabcX\n0\r\n\r\n")
        elif self.path == "/status":
            self._send(204, "")
        elif self.path == "/header":
            self._send(200, "HEADER_OK" if self.headers.get("X-VNT-Test") == "present" else "HEADER_MISSING")
        elif self.path == "/slow":
            time.sleep(2)
            try:
                self._send(200, "SLOW_OK")
            except (BrokenPipeError, ConnectionResetError):
                pass
        elif self.path == "/oversized":
            # Buffered requests must reject bodies beyond the documented 64 MiB ceiling.
            self.send_response(200)
            self.send_header("Content-Length", str(64 * 1024 * 1024 + 1))
            self.send_header("Connection", "close")
            self.end_headers()
            block = b"x" * 65536
            try:
                for _ in range(1024):
                    self.wfile.write(block)
                self.wfile.write(b"x")
            except (BrokenPipeError, ConnectionResetError):
                pass
        elif self.path == "/binary":
            payload = bytes((0, 1, 255, 10, 13, 128))
            self.send_response(200)
            self.send_header("Content-Length", str(len(payload)))
            self.send_header("Connection", "close")
            self.end_headers()
            self.wfile.write(payload)
        else:
            self._send(404, "NOT_FOUND")

    def do_POST(self):
        length = int(self.headers.get("Content-Length", "0"))
        body = self.rfile.read(length)
        if self.path == "/echo-bytes":
            self.send_response(200)
            self.send_header("Content-Length", str(len(body)))
            self.send_header("Connection", "close")
            self.end_headers()
            self.wfile.write(body)
            return
        self._send(201, "POST_OK:" + body.decode("utf-8"))

    def log_message(self, *_args):
        pass

def main():
    server = http.server.ThreadingHTTPServer((HOST, PORT), Handler)
    redirect_server = http.server.ThreadingHTTPServer((HOST, REDIRECT_PORT), Handler)
    thread = threading.Thread(target=server.serve_forever, daemon=True)
    redirect_thread = threading.Thread(target=redirect_server.serve_forever, daemon=True)
    thread.start()
    redirect_thread.start()
    try:
        exe = str(pathlib.Path(tempfile.gettempdir()) / ("vnt-native-networking-test.exe" if os.name == "nt" else "vnt-native-networking-test"))
        compiler = ROOT / ("vnt.exe" if os.name == "nt" else "vnt")
        compile_result = subprocess.run(
            [str(compiler), "--compile", "tests/native_networking.vnt", "-o", exe],
            cwd=ROOT, text=True, capture_output=True, timeout=60)
        if compile_result.returncode:
            sys.stderr.write(f"VNT compile failed with exit code {compile_result.returncode}.\n")
            sys.stderr.write(compile_result.stdout + compile_result.stderr)
            return 1
        result = subprocess.run([exe], cwd=ROOT, text=True, capture_output=True, timeout=40)
        if result.returncode:
            sys.stderr.write(f"Native networking program failed with exit code {result.returncode}.\n")
            sys.stderr.write(result.stdout + result.stderr)
            asm_path = pathlib.Path(exe + ".s")
            if asm_path.exists():
                lines = asm_path.read_text(errors="replace").splitlines()
                for marker in ("main:", "vnt_fn_get:", "call vnt_net_request", "vnt_net_request:"):
                    positions = [i for i, line in enumerate(lines) if marker in line]
                    for pos in positions[:2]:
                        sys.stderr.write(f"\n--- assembly context: {marker} line {pos + 1} ---\n")
                        sys.stderr.write("\n".join(lines[max(0, pos - 8):pos + 22]) + "\n")
            return 1
        # WinHTTP validates/normalizes transfer framing internally; unlike the
        # raw-socket transport it exposes the decoded partial body for this
        # intentionally malformed test response. Both outcomes are platform API
        # behavior, while the Unix decoder must reject the malformed CRLF.
        malformed_chunk_result = "abc\n" if os.name == "nt" else "\n"
        # WinHTTP normalizes conflicting Content-Length fields; the Unix parser
        # rejects them before creating the destination file.
        conflicting_length_result = "true\n" if os.name == "nt" else "false\n"
        expected = ('VNT_NATIVE_HTTP_OK\nFRAGMENT_NOT_SENT\nCONTENT_LENGTH_OK\nPOOL_REUSED\n'
                    'VNT_NATIVE_HTTP_OK\nCHUNKED_OK\nTransfer-Encoding: chunked is body text\n' + malformed_chunk_result +
                    '[0, 1, 255, 10, 13, 128]\n6\n255\ntrue\n' + conflicting_length_result +
                    '[0, 1, 255, 10, 13, 128]\n'
                    'AUTH_STRIPPED\n204\nHEADER_OK\n\nPOST_OK:{"native":true}\n'
                    'POST_OK:SAFE_BODY\n404\ntrue\n\ntrue\nVNT_NATIVE_HTTP_OK\n200\ntrue\ntrue\nfalse\ntrue\n')
        if result.stdout != expected:
            sys.stderr.write("Unexpected native networking output.\nExpected:\n" + expected +
                             "Actual:\n" + result.stdout + result.stderr)
            return 1
        binary_path = ROOT / "vnt-network-binary.test"
        binary = binary_path.read_bytes()
        binary_path.unlink(missing_ok=True)
        if binary != bytes((0, 1, 255, 10, 13, 128)):
            sys.stderr.write(f"Binary download was corrupted: {binary!r}\n")
            return 1
        chunked_path = ROOT / "vnt-network-chunked.test"
        chunked = chunked_path.read_bytes()
        chunked_path.unlink(missing_ok=True)
        if chunked != b"CHUNKED_OK":
            sys.stderr.write(f"Chunked streaming download was corrupted: {chunked!r}\n")
            return 1
        redirect_path = ROOT / "vnt-network-redirect.test"
        redirected = redirect_path.read_bytes() if redirect_path.exists() else b""
        redirect_path.unlink(missing_ok=True)
        if redirected != b"VNT_NATIVE_HTTP_OK":
            sys.stderr.write(f"Redirected streaming download was corrupted: {redirected!r}\n")
            return 1
        if os.name != "nt":
            proxy_server = http.server.ThreadingHTTPServer((HOST, 18767), ProxyHandler)
            proxy_thread = threading.Thread(target=proxy_server.serve_forever, daemon=True)
            proxy_thread.start()
            try:
                proxy_source = ROOT / "tests/native_proxy_networking.vnt"
                proxy_compile = subprocess.run(
                    [str(compiler), "--compile", str(proxy_source.relative_to(ROOT)), "-o", exe],
                    cwd=ROOT, text=True, capture_output=True, timeout=60)
                if proxy_compile.returncode:
                    sys.stderr.write(proxy_compile.stdout + proxy_compile.stderr)
                    return 1
                proxy_env = os.environ.copy()
                proxy_env.update({
                    "HTTP_PROXY": "http://127.0.0.1:18767",
                    "http_proxy": "http://127.0.0.1:18767",
                    "NO_PROXY": "",
                    "no_proxy": "",
                })
                proxy_result = subprocess.run([exe], cwd=ROOT, env=proxy_env,
                    text=True, capture_output=True, timeout=20)
                if proxy_result.returncode or proxy_result.stdout != "PROXY_OK\n":
                    sys.stderr.write("Unix HTTP proxy test failed.\n" + proxy_result.stdout + proxy_result.stderr)
                    return 1
                proxy_download_path = ROOT / "vnt-network-proxy.test"
                proxy_download = proxy_download_path.read_bytes() if proxy_download_path.exists() else b""
                proxy_download_path.unlink(missing_ok=True)
                if proxy_download != b"PROXY_OK":
                    sys.stderr.write(f"Unix proxy streaming download failed: {proxy_download!r}\n")
                    return 1
            finally:
                proxy_server.shutdown()
                proxy_server.server_close()
        if os.name != "nt":
            # Use a locally generated SAN certificate trusted only by this test process.
            with tempfile.TemporaryDirectory(prefix="vnt-tls-pool-") as tls_dir:
                tls_dir = pathlib.Path(tls_dir)
                cert_path = tls_dir / "localhost.crt"
                key_path = tls_dir / "localhost.key"
                cert_result = subprocess.run([
                    "openssl", "req", "-x509", "-newkey", "rsa:2048", "-nodes",
                    "-keyout", str(key_path), "-out", str(cert_path), "-days", "1",
                    "-subj", "/CN=localhost",
                    "-addext", "subjectAltName=DNS:localhost,IP:127.0.0.1",
                ], text=True, capture_output=True, timeout=30)
                if cert_result.returncode:
                    sys.stderr.write("Could not generate local TLS test certificate.\n" +
                                     cert_result.stdout + cert_result.stderr)
                    return 1
                tls_server = http.server.ThreadingHTTPServer((HOST, 18768), TLSHandler)
                tls_context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
                tls_context.load_cert_chain(str(cert_path), str(key_path))
                tls_server.socket = tls_context.wrap_socket(tls_server.socket, server_side=True)
                tls_thread = threading.Thread(target=tls_server.serve_forever, daemon=True)
                tls_thread.start()
                try:
                    tls_compile = subprocess.run(
                        [str(compiler), "--compile", "tests/native_tls_pool_networking.vnt", "-o", exe],
                        cwd=ROOT, text=True, capture_output=True, timeout=60)
                    if tls_compile.returncode:
                        sys.stderr.write(tls_compile.stdout + tls_compile.stderr)
                        return 1
                    tls_env = os.environ.copy()
                    for name in ("HTTPS_PROXY", "https_proxy", "ALL_PROXY", "all_proxy"):
                        tls_env[name] = ""
                    tls_env["NO_PROXY"] = "localhost,127.0.0.1"
                    tls_env["no_proxy"] = "localhost,127.0.0.1"
                    tls_env["SSL_CERT_FILE"] = str(cert_path)
                    tls_result = subprocess.run([exe], cwd=ROOT, env=tls_env,
                        text=True, capture_output=True, timeout=20)
                    if tls_result.returncode or tls_result.stdout != "TLS_READY\nTLS_POOL_REUSED\n":
                        sys.stderr.write("Unix TLS connection-pool test failed.\n" +
                                         tls_result.stdout + tls_result.stderr)
                        return 1
                finally:
                    tls_server.shutdown()
                    tls_server.server_close()
        print("Native HTTP integration tests passed.")
        return 0
    finally:
        (ROOT / "vnt-network-binary.test").unlink(missing_ok=True)
        (ROOT / "vnt-network-chunked.test").unlink(missing_ok=True)
        (ROOT / "vnt-network-redirect.test").unlink(missing_ok=True)
        (ROOT / "vnt-network-conflicting.test").unlink(missing_ok=True)
        (ROOT / "vnt-network-proxy.test").unlink(missing_ok=True)
        server.shutdown()
        server.server_close()
        redirect_server.shutdown()
        redirect_server.server_close()

if __name__ == "__main__":
    raise SystemExit(main())
