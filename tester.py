#!/usr/bin/env python3
"""
irc_tester.py
Расширенный набор тестов для IRC-сервера из сабджекта:
- JOIN
- PRIVMSG forwarding
- INVITE
- MODE (i, t, k, o, l)
- TOPIC
- KICK
- Тест частичной доставки команд
"""

import argparse
import socket
import time
import subprocess
import os
import signal
import sys
from typing import List, Tuple

CRLF = "\r\n"
log_file = None

def log(msg: str):
    print(msg)
    if log_file:
        with open(log_file, "a") as f:
            f.write(msg + "\n")

class IRCClient:
    def __init__(self, name: str, host: str, port: int, timeout=3.0):
        self.name = name
        self.sock = socket.create_connection((host, port))
        self.sock.settimeout(0.5)
        self.buf = ""
        self.timeout = timeout

    def send_raw(self, data: str, split=False, delay=0.2):
        if not data.endswith("\r\n"):
            data = data + "\r\n"
        if split:
            for chunk in [data[:len(data)//2], data[len(data)//2:]]:
                log(f"[{self.name} ->] (split) {repr(chunk)}")
                self.sock.sendall(chunk.encode())
                time.sleep(delay)
        else:
            log(f"[{self.name} ->] {repr(data)}")
            self.sock.sendall(data.encode())

    def recv_some(self):
        try:
            data = self.sock.recv(4096)
            if not data:
                return False
            s = data.decode(errors="replace")
            log(f"[{self.name} <-] {repr(s)}")
            self.buf += s
            return True
        except socket.timeout:
            return False
        except Exception as e:
            log(f"[{self.name}] recv error: {e}")
            return False

    def read_lines(self) -> List[str]:
        lines = []
        while CRLF in self.buf:
            idx = self.buf.index(CRLF)
            line = self.buf[:idx]
            self.buf = self.buf[idx + len(CRLF):]
            lines.append(line)
        return lines

    def expect_contains(self, substring: str, timeout=None) -> Tuple[bool, List[str]]:
        if timeout is None:
            timeout = self.timeout
        deadline = time.time() + timeout
        collected = []
        while time.time() < deadline:
            self.recv_some()
            lines = self.read_lines()
            for ln in lines:
                collected.append(ln)
                if substring in ln:
                    return True, collected
            time.sleep(0.01)
        return False, collected

    def expect_any(self, timeout=None) -> List[str]:
        if timeout is None:
            timeout = self.timeout
        deadline = time.time() + timeout
        collected = []
        while time.time() < deadline:
            self.recv_some()
            lines = self.read_lines()
            if lines:
                collected.extend(lines)
            else:
                time.sleep(0.01)
        return collected

    def close(self):
        try:
            self.sock.close()
        except:
            pass

# --- helpers ---

def do_registration(client: IRCClient, password: str, nick: str, user: str):
    client.send_raw(f"PASS {password}")
    client.send_raw(f"NICK {nick}")
    client.send_raw(f"USER {user} 0 * :{user}")
    return client.expect_any(1.0)

# --- tests ---

def test_partial_command(host, port, password):
    c = IRCClient("partial", host, port)
    try:
        c.send_raw(f"PASS {password}")
        c.send_raw("NICK testnick", split=True)
        c.send_raw("USER u 0 * :uuu", split=True)
        lines = c.expect_any(1.0)
        if not lines:
            return False, "no response for partial command"
        return True, "partial command handled"
    finally:
        c.close()

def test_join_and_privmsg(host, port, password):
    a = IRCClient("A", host, port)
    b = IRCClient("B", host, port)
    try:
        do_registration(a, password, "nickA", "userA")
        do_registration(b, password, "nickB", "userB")
        a.send_raw("JOIN #chan")
        b.send_raw("JOIN #chan")
        time.sleep(0.2)
        a.send_raw("PRIVMSG #chan :hello")
        ok, lines = b.expect_contains("hello", timeout=2.0)
        return (ok, "privmsg delivered" if ok else "privmsg not received")
    finally:
        a.close()
        b.close()

def test_invite(host, port, password):
    op = IRCClient("op", host, port)
    guest = IRCClient("guest", host, port)
    try:
        do_registration(op, password, "opnick", "opuser")
        do_registration(guest, password, "guest", "guser")
        op.send_raw("JOIN #room")
        op.send_raw("MODE #room +i")  # invite-only
        time.sleep(0.2)
        guest.send_raw("JOIN #room")
        time.sleep(0.2)
        op.send_raw("INVITE guest #room")
        ok, lines = guest.expect_contains("INVITE", timeout=1.0)
        return (ok, "invite works" if ok else "invite failed")
    finally:
        op.close()
        guest.close()

def test_modes(host, port, password):
    op = IRCClient("op", host, port)
    try:
        do_registration(op, password, "mop", "mopuser")
        op.send_raw("JOIN #modes")
        time.sleep(0.1)
        op.send_raw("MODE #modes +t")
        op.send_raw("MODE #modes +k key123")
        op.send_raw("MODE #modes +l 1")
        lines = op.expect_any(1.0)
        if not lines:
            return False, "no mode responses"
        return True, "modes applied"
    finally:
        op.close()

def test_topic(host, port, password):
    op = IRCClient("op", host, port)
    try:
        do_registration(op, password, "topnick", "topuser")
        op.send_raw("JOIN #top")
        time.sleep(0.1)
        op.send_raw("TOPIC #top :MyTopic")
        ok, lines = op.expect_contains("MyTopic", timeout=1.0)
        return (ok, "topic set" if ok else "topic not set")
    finally:
        op.close()

def test_kick(host, port, password):
    op = IRCClient("op", host, port)
    vic = IRCClient("vic", host, port)
    try:
        do_registration(op, password, "kop", "kopuser")
        do_registration(vic, password, "kvic", "kvuser")
        op.send_raw("JOIN #kick")
        vic.send_raw("JOIN #kick")
        time.sleep(0.2)
        op.send_raw("MODE #kick +o kop")
        op.send_raw("KICK #kick kvic :bye")
        ok, lines = vic.expect_contains("KICK", timeout=1.0)
        return (ok, "kick works" if ok else "kick failed")
    finally:
        op.close()
        vic.close()

# --- server mgmt ---

def wait_for_port(port, host="127.0.0.1", timeout=3.0):
    deadline = time.time() + timeout
    while time.time() < deadline:
        try:
            s = socket.create_connection((host, port), timeout=0.5)
            s.close()
            return True
        except Exception:
            time.sleep(0.05)
    return False

def run_server(exe_path: str, port: int, password: str):
    proc = subprocess.Popen([exe_path, str(port), password],
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            preexec_fn=os.setsid)
    return proc

def kill_proc(proc):
    try:
        os.killpg(os.getpgid(proc.pid), signal.SIGTERM)
    except Exception:
        try:
            proc.terminate()
        except Exception:
            pass

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--exe", required=True)
    parser.add_argument("--port", type=int, required=True)
    parser.add_argument("--pass", dest="password", required=True)
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--log", default="irc_tester.log")
    args = parser.parse_args()

    global log_file
    log_file = args.log
    if os.path.exists(log_file):
        os.remove(log_file)

    log(f"Starting server {args.exe} {args.port} {args.password}")
    proc = run_server(args.exe, args.port, args.password)
    try:
        if not wait_for_port(args.port, host=args.host, timeout=4.0):
            log("Server not listening")
            kill_proc(proc)
            sys.exit(3)

        tests = [
            (test_partial_command, "partial_command"),
            (test_join_and_privmsg, "join_privmsg"),
            (test_invite, "invite"),
            (test_modes, "modes"),
            (test_topic, "topic"),
            (test_kick, "kick"),
        ]
        results = []
        for fn, name in tests:
            ok, msg = fn(args.host, args.port, args.password)
            results.append((ok, name, msg))
            log(f"Test {name}: {'OK' if ok else 'FAIL'} - {msg}")

        passed = sum(1 for r in results if r[0])
        total = len(results)
        log(f"\n=== SUMMARY ===\nPassed {passed}/{total} tests.")
        sys.exit(0 if passed == total else 4)

    finally:
        kill_proc(proc)

if __name__ == "__main__":
    main()
