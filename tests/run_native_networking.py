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
        elif self.path == "/redirect":
            self.send_response(302)
            self.send_header("Location", "/hello")
            self.send_header("Content-Length", "0")
            self.send_header("Connection", "close")
            self.end_headers()
        elif self.path == "/chunked":
            self.send_response(200)
            self.send_header("Transfer-Encoding", "chunked")
            self.send_header("Connection", "close")
            self.end_headers()
            for chunk in (b"CHUNK", b"ED_OK"):
                self.wfile.write(f"{len(chunk):X}\r\n".encode("ascii") + chunk + b"\r\n")
            self.wfile.write(b"0\r\n\r\n")
        elif self.path == "/status":
            self._send(204, "")
        elif self.path == "/header":
            self._send(200, "HEADER_OK" if self.headers.get("X-VNT-Test") == "present" else "HEADER_MISSING")
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
        expected = 'VNT_NATIVE_HTTP_OK\nVNT_NATIVE_HTTP_OK\nCHUNKED_OK\n204\nHEADER_OK\nPOST_OK:{"native":true}\n404\ntrue\n'
        if result.stdout != expected:
            sys.stderr.write("Unexpected native networking output.\nExpected:\n" + expected +
                             "Actual:\n" + result.stdout + result.stderr)
            return 1
        binary = pathlib.Path("/tmp/vnt-network-binary").read_bytes()
        if binary != bytes((0, 1, 255, 10, 13, 128)):
            sys.stderr.write(f"Binary download was corrupted: {binary!r}\n")
            return 1
        print("Native HTTP integration tests passed.")
        return 0
    finally:
        server.shutdown()
        server.server_close()

if __name__ == "__main__":
    raise SystemExit(main())
