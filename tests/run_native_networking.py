#!/usr/bin/env python3
"""Offline integration tests for VNT's native HTTP runtime."""
import http.server
import pathlib
import subprocess
import sys
import threading

ROOT = pathlib.Path(__file__).resolve().parents[1]
HOST = "127.0.0.1"
PORT = 18765

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
        elif self.path == "/status":
            self._send(204, "")
        elif self.path == "/header":
            self._send(200, "HEADER_OK" if self.headers.get("X-VNT-Test") == "present" else "HEADER_MISSING")
        else:
            self._send(404, "NOT_FOUND")

    def do_POST(self):
        length = int(self.headers.get("Content-Length", "0"))
        body = self.rfile.read(length).decode("utf-8")
        self._send(201, "POST_OK:" + body)

    def log_message(self, *_args):
        pass

def main():
    server = http.server.ThreadingHTTPServer((HOST, PORT), Handler)
    thread = threading.Thread(target=server.serve_forever, daemon=True)
    thread.start()
    try:
        exe = "/tmp/vnt-native-networking-test"
        compile_result = subprocess.run(
            [str(ROOT / "vnt"), "--compile", "tests/native_networking.vnt", "-o", exe],
            cwd=ROOT, text=True, capture_output=True, timeout=60)
        if compile_result.returncode:
            sys.stderr.write(compile_result.stdout + compile_result.stderr)
            return 1
        result = subprocess.run([exe], cwd=ROOT, text=True, capture_output=True, timeout=40)
        if result.returncode:
            sys.stderr.write(result.stdout + result.stderr)
            return 1
        expected = 'VNT_NATIVE_HTTP_OK\n204\nHEADER_OK\nPOST_OK:{"native":true}\n404\n'
        if result.stdout != expected:
            sys.stderr.write("Unexpected native networking output.\nExpected:\n" + expected +
                             "Actual:\n" + result.stdout + result.stderr)
            return 1
        print("Native HTTP integration tests passed.")
        return 0
    finally:
        server.shutdown()
        server.server_close()

if __name__ == "__main__":
    raise SystemExit(main())
