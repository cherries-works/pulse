---
name: edge-case-testing
description: Comprehensive edge case testing methodology for system monitoring tools, focusing on boundary conditions, error handling, and unusual scenarios that can reveal bugs and vulnerabilities.
tags:
  - testing
  - edge-cases
  - boundary
  - error-handling
  - robustness
  - c
  - monitoring
when: >
  - User requests thorough testing beyond basic functionality
  - User wants to find subtle bugs or edge cases
  - Robustness testing is needed
  - Error handling needs to be validated
priority: 10
---

# Edge Case Testing Skill for Pulse

## Philosophy

Edge case testing focuses on inputs, conditions, and scenarios that occur at the boundaries of expected operation. These tests often reveal:
- **Boundary violations** (off-by-one errors, buffer overflows)
- **Error handling issues** (crashes, memory leaks, resource leaks)
- **Race conditions** (concurrency issues, timing-dependent bugs)
- **Resource exhaustion** (memory, CPU, file descriptors)
- **Logic errors** (incorrect assumptions, unexpected state transitions)

For a C-based system monitoring tool like Pulse, edge cases are particularly important due to:
- Manual memory management
- Fixed-size buffers
- Direct system file access
- Network I/O
- Real-time data collection

## Testing Strategy

### 1. Input Boundary Testing

Test at the limits of what the system can handle.

#### A. HTTP Request Boundaries

**Buffer Size Limits:**
```bash
# Identify buffer sizes from code
# From utils.h: BUFFER_ONE_KB = 1024, BUFFER_ONE_MB = 1024*1024

# Test requests at exact buffer boundaries
python3 << 'EOF'
import socket

def test_request_size(size, description):
    s = socket.socket()
    s.settimeout(5)
    try:
        s.connect(('localhost', 8383))
        # GET /<path> HTTP/1.1\r\n\r\n
        path = 'A' * (size - len('GET / HTTP/1.1\r\n\r\n'))
        request = f'GET /{path} HTTP/1.1\r\n\r\n'.encode()
        s.send(request)
        response = s.recv(4096)
        print(f"{description} ({size} bytes): {'PASS' if response else 'FAIL/Crash'}")
        return True
    except Exception as e:
        print(f"{description} ({size} bytes): FAIL - {e}")
        return False
    finally:
        try:
            s.close()
        except:
            pass

# Test at various boundaries
for size in [1023, 1024, 1025, 2047, 2048, 2049, 4095, 4096, 4097, 8191, 8192, 8193]:
    test_request_size(size, f"Request size")

# Test path component specifically
for path_len in [1023, 1024, 1025, 2047, 2048, 2049]:
    s = socket.socket()
    s.settimeout(5)
    try:
        s.connect(('localhost', 8383))
        path = 'A' * path_len
        request = f'GET /{path} HTTP/1.1\r\n\r\n'.encode()
        s.send(request)
        response = s.recv(4096)
        print(f"Path length {path_len}: {'PASS' if response else 'FAIL'}")
    except Exception as e:
        print(f"Path length {path_len}: FAIL - {e}")
    finally:
        try:
            s.close()
        except:
            pass
EOF
```

**Method Name Boundaries:**
```bash
python3 << 'EOF'
import socket

def test_method_size(size):
    s = socket.socket()
    s.settimeout(5)
    try:
        s.connect(('localhost', 8383))
        method = 'A' * size
        request = f'{method} / HTTP/1.1\r\n\r\n'.encode()
        s.send(request)
        response = s.recv(4096)
        return response is not None
    except:
        return False
    finally:
        try:
            s.close()
        except:
            pass

# Test method names of various lengths
for size in [1, 3, 4, 5, 6, 7, 8, 15, 16, 20, 100, 255, 256, 1000]:
    result = test_method_size(size)
    print(f"Method length {size}: {'PASS' if result else 'FAIL/Crash'}")
EOF
```

#### B. String Content Edge Cases

**Special Characters:**
```bash
# Test various special characters in path
special_chars = [
    '/', '//', '///',  # Multiple slashes
    '.', '..', '../',  # Dots and traversal
    '%2e', '%2E', '%2f', '%2F',  # URL encoded
    '\x00', '%00',  # Null byte
    '\n', '%0a', '%0d',  # Newlines
    ' ', '%20', '+',  # Spaces
    '\t', '%09',  # Tabs
    ';', '&', '|', '`', '$', '>', '<', '"', "'",  # Shell metacharacters
    '{', '}', '[', ']', '(', ')',  # Brackets
    '*', '?', '\\',  # Wildcards and backslash
    '#', '~', '^',  # Other special
]

for char in special_chars:
    try:
        # URL encode if needed
        import urllib.parse
        encoded = urllib.parse.quote(char, safe='')
        url = f"http://localhost:8383/test{encoded}char"
        response = requests.get(url, timeout=2)
        print(f"Special char '{char}': PASS")
    except Exception as e:
        print(f"Special char '{char}': FAIL - {e}")
```

**Unicode and UTF-8:**
```bash
# Test various Unicode characters
unicode_tests = [
    '/test/café',  # Simple Unicode
    '/test/日本語',  # Japanese
    '/test/🎉',  # Emoji
    '/test/\u0000',  # Null
    '/test/\uffff',  # Invalid Unicode
    '/test/' + ''.join(chr(i) for i in range(0, 256)),  # All bytes
]

for test in unicode_tests:
    try:
        url = f"http://localhost:8383{test}"
        response = requests.get(url, timeout=2)
        print(f"Unicode test: PASS")
    except Exception as e:
        print(f"Unicode test '{test}': FAIL - {e}")
```

**Very Long Strings:**
```bash
# Test strings at various lengths
for length in [255, 256, 257, 511, 512, 513, 1023, 1024, 1025, 2047, 2048, 4095, 4096]:
    test_str = 'A' * length
    try:
        url = f"http://localhost:8383/{test_str}"
        response = requests.get(url, timeout=2)
        print(f"Length {length}: PASS")
    except Exception as e:
        print(f"Length {length}: FAIL - {e}")
```

### 2. HTTP Protocol Edge Cases

#### A. Malformed Requests

**Missing Components:**
```bash
# No method
python3 -c "
import socket
s = socket.socket()
s.connect(('localhost', 8383))
s.send(b'/ HTTP/1.1\r\n\r\n')
print(s.recv(1024).decode())
s.close()
"

# No path
python3 -c "
import socket
s = socket.socket()
s.connect(('localhost', 8383))
s.send(b'GET HTTP/1.1\r\n\r\n')
print(s.recv(1024).decode())
s.close()
"

# No HTTP version
python3 -c "
import socket
s = socket.socket()
s.connect(('localhost', 8383))
s.send(b'GET /\r\n\r\n')
print(s.recv(1024).decode())
s.close()
"

# Just CRLF
python3 -c "
import socket
s = socket.socket()
s.connect(('localhost', 8383))
s.send(b'\r\n')
print(s.recv(1024).decode())
s.close()
"

# Empty request
python3 -c "
import socket
s = socket.socket()
s.connect(('localhost', 8383))
s.send(b'')
print(s.recv(1024).decode())
s.close()
"
```

**Whitespace Variations:**
```bash
# Multiple spaces
python3 -c "
import socket
s = socket.socket()
s.connect(('localhost', 8383))
s.send(b'GET    /    HTTP/1.1\r\n\r\n')
print(s.recv(1024).decode())
s.close()
"

# Tabs instead of spaces
python3 -c "
import socket
s = socket.socket()
s.connect(('localhost', 8383))
s.send(b'GET\t/\tHTTP/1.1\r\n\r\n')
print(s.recv(1024).decode())
s.close()
"

# No spaces, just CRLF
python3 -c "
import socket
s = socket.socket()
s.connect(('localhost', 8383))
s.send(b'GET\r\n/\r\nHTTP/1.1\r\n\r\n')
print(s.recv(1024).decode())
s.close()
"
```

#### B. HTTP Version Edge Cases

**Different HTTP Versions:**
```bash
# HTTP/0.9
python3 -c "
import socket
s = socket.socket()
s.connect(('localhost', 8383))
s.send(b'GET / HTTP/0.9\r\n\r\n')
print(s.recv(1024).decode())
s.close()
"

# HTTP/1.0
curl -v --http1.0 http://localhost:8383/

# HTTP/2 (should fail or downgrade)
curl -v --http2 http://localhost:8383/

# HTTP/3 (QUIC)
curl -v --http3 http://localhost:8383/

# Invalid version
python3 -c "
import socket
s = socket.socket()
s.connect(('localhost', 8383))
s.send(b'GET / HTTP/99.99\r\n\r\n')
print(s.recv(1024).decode())
s.close()
"
```

#### C. Header Edge Cases

**Empty Headers:**
```bash
# Empty header name
python3 -c "
import socket
s = socket.socket()
s.connect(('localhost', 8383))
s.send(b'GET / HTTP/1.1\r\n: value\r\n\r\n')
print(s.recv(1024).decode())
s.close()
"

# Empty header value
python3 -c "
import socket
s = socket.socket()
s.connect(('localhost', 8383))
s.send(b'GET / HTTP/1.1\r\nX-Test:\r\n\r\n')
print(s.recv(1024).decode())
s.close()
"

# Header with no colon
python3 -c "
import socket
s = socket.socket()
s.connect(('localhost', 8383))
s.send(b'GET / HTTP/1.1\r\nX-Test value\r\n\r\n')
print(s.recv(1024).decode())
s.close()
"
```

**Duplicate Headers:**
```bash
# Same header multiple times
python3 -c "
import socket
s = socket.socket()
s.connect(('localhost', 8383))
s.send(b'GET / HTTP/1.1\r\nHost: localhost\r\nHost: evil.com\r\n\r\n')
print(s.recv(1024).decode())
s.close()
"
```

**Case Sensitivity:**
```bash
# Different case headers
python3 -c "
import socket
s = socket.socket()
s.connect(('localhost', 8383))
s.send(b'GET / HTTP/1.1\r\nhost: localhost\r\n\r\n')
print(s.recv(1024).decode())
s.close()
"

# Mixed case
python3 -c "
import socket
s = socket.socket()
s.connect(('localhost', 8383))
s.send(b'gEt / hTtP/1.1\r\n\r\n')
print(s.recv(1024).decode())
s.close()
"
```

### 3. File System Edge Cases

#### A. Path Edge Cases

**Empty and Root Paths:**
```bash
# Empty path
curl -v http://localhost:8383/

# Just slash
curl -v http://localhost:8383//

# Multiple slashes
curl -v http://localhost:8383////
curl -v http://localhost:8383///test

# Trailing slash
curl -v http://localhost:8383/test/
```

**Dot Files:**
```bash
# Hidden files
curl -v http://localhost:8383/.hidden
curl -v http://localhost:8383/.git
curl -v http://localhost:8383/.git/config
curl -v http://localhost:8383/.gitignore
curl -v http://localhost:8383/.env
```

**Special Files:**
```bash
# Device files
curl -v http://localhost:8383/dev/null
curl -v http://localhost:8383/dev/random
curl -v http://localhost:8383/dev/zero

# Proc files
curl -v http://localhost:8383/proc/self
curl -v http://localhost:8383/proc/self/environ
curl -v http://localhost:8383/proc/cpuinfo
curl -v http://localhost:8383/proc/meminfo

# Sys files
curl -v http://localhost:8383/sys/kernel/hostname
```

**Symlinks:**
```bash
# Create test symlinks
ln -s /etc/passwd /tmp/test_passwd_link
ln -s /root /tmp/test_root_link

# Try to access via symlink
curl -v http://localhost:8383/tmp/test_passwd_link
curl -v http://localhost:8383/tmp/test_root_link
```

**Non-existent Files:**
```bash
# Files that don't exist
curl -v http://localhost:8383/nonexistent
curl -v http://localhost:8383/nonexistent.html
curl -v http://localhost:8383/nonexistent.css

# Directories without index
curl -v http://localhost:8383/src
curl -v http://localhost:8383/include
```

#### B. File Content Edge Cases

**Empty Files:**
```bash
# Create empty files
 mkdir -p /tmp/test_empty
touch /tmp/test_empty/empty.txt
touch /tmp/test_empty/empty.html

# Try to serve them
# Note: Pulse only serves from its own directory
# So we need to test with files in the project
curl -v http://localhost:8383/$(ls pulse/ | grep -E '\.(txt|md)$' | head -1)
```

**Very Large Files:**
```bash
# Create a large file
dd if=/dev/zero of=/tmp/large_file.bin bs=1M count=100

# Try to serve it (if in the right location)
# For Pulse, we need to place it in the project directory
```

**Binary Files:**
```bash
# Try to serve binary files
curl -v http://localhost:8383/dist/pulse
curl -v http://localhost:8383/Makefile
```

### 4. Network Edge Cases

#### A. Connection Edge Cases

**Rapid Connections:**
```bash
# Open and close connections rapidly
python3 << 'EOF'
import socket, time

start = time.time()
count = 0
while time.time() - start < 10:
    try:
        s = socket.socket()
        s.settimeout(1)
        s.connect(('localhost', 8383))
        s.send(b'GET / HTTP/1.1\r\n\r\n')
        s.recv(1)
        s.close()
        count += 1
    except:
        pass

print(f"Connections per second: {count / 10}")
EOF
```

**Slow Reading:**
```bash
# Read response very slowly
python3 << 'EOF'
import socket, time

s = socket.socket()
s.connect(('localhost', 8383))
s.send(b'GET / HTTP/1.1\r\n\r\n')

# Read byte by byte with delay
timeout = 5
start = time.time()
while time.time() - start < timeout:
    try:
        byte = s.recv(1)
        if not byte:
            break
        time.sleep(0.1)
    except:
        break

s.close()
EOF
```

**Slow Writing:**
```bash
# Send request very slowly
python3 << 'EOF'
import socket, time

s = socket.socket()
s.connect(('localhost', 8383))

request = b'GET / HTTP/1.1\r\n\r\n'
for byte in request:
    s.send(bytes([byte]))
    time.sleep(0.1)

response = s.recv(4096)
print(response.decode())
s.close()
EOF
```

**Partial Requests:**
```bash
# Send only part of request
python3 << 'EOF'
import socket

s = socket.socket()
s.connect(('localhost', 8383))
s.send(b'GET ')
# Don't send the rest
try:
    response = s.recv(1024)
    print(response.decode())
except:
    pass
s.close()
EOF
```

#### B. IP Address Edge Cases

**Different Source IPs:**
```bash
# If testing on a system with multiple IPs
# Bind to specific source IP
python3 << 'EOF'
import socket

# Try different source ports
for port in [12345, 12346, 12347, 50000, 60000]:
    s = socket.socket()
    s.bind(('0.0.0.0', port))
    s.connect(('localhost', 8383))
    s.send(b'GET / HTTP/1.1\r\n\r\n')
    response = s.recv(1024)
    print(f"Source port {port}: {len(response)} bytes")
    s.close()
EOF
```

### 5. System Edge Cases

#### A. System State Edge Cases

**High Load:**
```bash
# Run CPU-intensive tasks while testing
python3 << 'EOF'
import os, time

# Fork bomb (be careful!)
# os.system(":(){ :|:& };:")

# CPU load
for i in range(4):
    os.system(f"dd if=/dev/zero of=/dev/null bs=1M count=100000 &")

time.sleep(5)
EOF

# Then test Pulse response times
curl -v http://localhost:8383/api/metrics
```

**Low Memory:**
```bash
# Consume memory
python3 << 'EOF'
import os, time

# Allocate large arrays
arrays = []
try:
    while True:
        arrays.append(bytearray(1024 * 1024))  # 1MB at a time
        time.sleep(0.1)
except MemoryError:
    print("Out of memory")

# Keep allocated
time.sleep(60)
EOF

# Then test Pulse
```

**Many Processes:**
```bash
# Create many processes
python3 << 'EOF'
import os, time

for i in range(1000):
    try:
        os.system(f"sleep 1000 &")
    except:
        pass

time.sleep(5)

# Then test process listing
curl -v http://localhost:8383/api/metrics
EOF
```

**No Processes:**
```bash
# This is hard to achieve, but test with minimal processes
# Run Pulse in a container with minimal environment
```

#### B. File System Edge Cases

**Full Disk:**
```bash
# Fill up disk space
python3 << 'EOF'
import os

# Create a large file
with open('/tmp/fill_disk', 'wb') as f:
    try:
        while True:
            f.write(os.urandom(1024 * 1024))
    except Exception as e:
        print(f"Stopped: {e}")
EOF

# Then test file operations in Pulse
```

**Read-only File System:**
```bash
# Mount file system as read-only (requires root)
# Then test if Pulse can still read /proc and /sys
```

**Missing Files:**
```bash
# Check what happens if /proc or /sys is missing
# This can be tested in a container
```

### 6. API Edge Cases

#### A. JSON Response Edge Cases

**Empty Data:**
```bash
# Check what happens with no history
curl -v http://localhost:8383/api/history/cpu
```

**Fresh Start:**
```bash
# Restart Pulse and immediately request history
# Check if it handles empty history gracefully
```

**Process Name Edge Cases:**
```bash
# Process names with special characters
# Create a process with special name
python3 << 'EOF'
import os, time

# Create process with special name
os.system("exec -a 'test\"process' sleep 100 &")
time.sleep(1)

# Request metrics
import requests
response = requests.get('http://localhost:8383/api/metrics')
print(response.json()['processes'])
EOF
```

**Very Long Process Names:**
```bash
# Create process with very long name
python3 << 'EOF'
import os

long_name = 'A' * 500
# Note: Linux has a limit on process name length (typically 15-16 chars)
# So this might not work, but we can try
os.system(f"exec -a '{long_name}' sleep 100 &")
EOF

# Then check metrics
curl -v http://localhost:8383/api/metrics
```

#### B. Metric Edge Cases

**Zero Values:**
```bash
# Check if Pulse handles zero values correctly
# This would require specific system conditions
curl -v http://localhost:8383/api/metrics | jq '.cpu, .memory, .disk'
```

**Maximum Values:**
```bash
# Check behavior with maximum uint64 values
# This would be hard to achieve naturally
```

**Overflow Values:**
```bash
# Check if counters wrap around correctly
# This would require running Pulse for a very long time
```

### 7. Time Edge Cases

#### A. Timestamp Edge Cases

**Epoch:**
```bash
# Set system time to epoch (1970)
# Then check timestamps in responses
```

**Far Future:**
```bash
# Set system time to far future
# Then check if timestamps are handled correctly
```

**Negative Time:**
```bash
# Set system time before epoch (if possible)
# Check how negative timestamps are handled
```

**Time Zone Changes:**
```bash
# Change time zone rapidly
while true; do
    sudo timedatectl set-timezone UTC
    sleep 1
    sudo timedatectl set-timezone America/New_York
    sleep 1
    curl -s http://localhost:8383/api/metrics | jq '.timestamp'
done
```

#### B. History Collection Edge Cases

**Rapid Sampling:**
```bash
# Start Pulse with very fast sampling
./dist/pulse --web --history-snapshot-interval 1

# Check if it handles rapid collection
```

**Very Slow Sampling:**
```bash
# Start Pulse with very slow sampling
./dist/pulse --web --history-snapshot-interval 3600

# Check if history is still collected
```

**Stop and Start:**
```bash
# Stop Pulse, wait, start again
# Check if history persists or resets
```

### 8. Concurrency Edge Cases

#### A. Race Conditions

**Rapid Requests to Same Endpoint:**
```bash
# Send many rapid requests to /api/metrics
python3 << 'EOF'
import requests, threading, time

results = []
errors = []

def make_request():
    try:
        response = requests.get('http://localhost:8383/api/metrics', timeout=2)
        results.append(response.json())
    except Exception as e:
        errors.append(str(e))

threads = []
for i in range(100):
    t = threading.Thread(target=make_request)
    threads.append(t)
    t.start()

for t in threads:
    t.join()

print(f"Success: {len(results)}, Errors: {len(errors)}")

# Check for inconsistencies
if results:
    timestamps = [r['timestamp'] for r in results]
    print(f"Timestamp range: {min(timestamps)} - {max(timestamps)}")
EOF
```

**Concurrent Connections:**
```bash
# Open many connections simultaneously
python3 << 'EOF'
import socket, threading, time

sockets = []
errors = []

def open_connection():
    try:
        s = socket.socket()
        s.settimeout(5)
        s.connect(('localhost', 8383))
        s.send(b'GET / HTTP/1.1\r\n\r\n')
        sockets.append(s)
    except Exception as e:
        errors.append(str(e))

threads = []
for i in range(100):
    t = threading.Thread(target=open_connection)
    threads.append(t)
    t.start()

for t in threads:
    t.join()

print(f"Open sockets: {len(sockets)}, Errors: {len(errors)}")

# Read responses
for s in sockets:
    try:
        response = s.recv(1024)
        print(f"Response length: {len(response)}")
    except:
        pass
    finally:
        s.close()
EOF
```

#### B. Resource Exhaustion

**File Descriptor Exhaustion:**
```bash
# Open many files to exhaust file descriptors
python3 << 'EOF'
import os, time

files = []
try:
    while True:
        f = open('/tmp/test_file', 'w')
        files.append(f)
        time.sleep(0.1)
except Exception as e:
    print(f"Stopped: {e}")

# Keep files open and test Pulse
time.sleep(60)

for f in files:
    f.close()
EOF

# Then try to connect to Pulse
curl -v http://localhost:8383/
```

**Memory Exhaustion:**
```bash
# Allocate lots of memory
python3 << 'EOF'
import time

arrays = []
try:
    while True:
        arrays.append(bytearray(1024 * 1024))
        time.sleep(0.1)
except:
    pass

time.sleep(60)
EOF

# Then test Pulse
```

### 9. Error Handling Edge Cases

#### A. Error Response Testing

**Induce Parsing Errors:**
```bash
# Send malformed requests and check error responses
python3 << 'EOF'
import socket

malformed_requests = [
    b'',  # Empty
    b'\r\n',  # Just CRLF
    b'GET',  # Incomplete
    b'GET /',  # Missing version
    b'GET / HTTP',  # Incomplete version
    b'\x00\x00\x00',  # Null bytes
    b'A' * 100000,  # Very long
    b'GET /\x00 HTTP/1.1\r\n\r\n',  # Null in path
]

for req in malformed_requests:
    try:
        s = socket.socket()
        s.settimeout(2)
        s.connect(('localhost', 8383))
        s.send(req)
        response = s.recv(1024)
        print(f"Request: {req[:50]}... -> {response[:100]}")
        s.close()
    except Exception as e:
        print(f"Request: {req[:50]}... -> ERROR: {e}")
EOF
```

**Check for Crash vs Graceful Error:**
```bash
# Monitor Pulse process while sending bad requests
pulse_pid=$(pgrep -f 'pulse --web')
echo "Pulse PID: $pulse_pid"

# Send bad request
python3 -c "
import socket
s = socket.socket()
s.connect(('localhost', 8383))
s.send(b'A' * 100000)
try:
    s.recv(1024)
except:
    pass
s.close()
"

# Check if Pulse is still running
if ps -p $pulse_pid > /dev/null; then
    echo "Pulse still running"
else
    echo "Pulse crashed!"
fi
```

#### B. Logging Edge Cases

**Check Logs for Errors:**
```bash
# Check if Pulse has any logging
# From the code, it uses _log() function
# But it's unclear where logs go

# Try to trigger log messages
python3 << 'EOF'
import socket

# Send requests that should trigger errors
for i in range(10):
    try:
        s = socket.socket()
        s.settimeout(1)
        s.connect(('localhost', 8383))
        s.send(b'A' * 100000)
        s.close()
    except:
        pass
EOF
```

## Testing Tools and Scripts

### Automated Edge Case Generator

```bash
#!/bin/bash
# edge_case_tester.sh

TARGET="http://localhost:8383"
TIMEOUT=2
LOG_FILE="edge_case_results.log"

> "$LOG_FILE"

echo "Starting edge case testing..."

# Test 1: Request length boundaries
echo "=== Request Length Tests ===" >> "$LOG_FILE"
for length in 1023 1024 1025 2047 2048 2049 4095 4096 4097 8191 8192 8193; do
    path=$(printf 'A%.0s' $(seq 1 $((length - 10))))
    if curl -s -m $TIMEOUT "${TARGET}/${path}" > /dev/null 2>&1; then
        echo "Length $length: PASS" >> "$LOG_FILE"
    else
        echo "Length $length: FAIL" >> "$LOG_FILE"
    fi
done

# Test 2: Special characters
echo -e "\n=== Special Character Tests ===" >> "$LOG_FILE"
special_chars=("/" ".." "%2e%2e%2f" "%2f" "%00" "%20" "%0a" "%0d" "\"" "'" "\\" ";`" "|" "&" ";" "<")
for char in "${special_chars[@]}"; do
    if curl -s -m $TIMEOUT "${TARGET}/test${char}file" > /dev/null 2>&1; then
        echo "Char '$char': PASS" >> "$LOG_FILE"
    else
        echo "Char '$char': FAIL" >> "$LOG_FILE"
    fi
done

# Test 3: Path traversal
echo -e "\n=== Path Traversal Tests ===" >> "$LOG_FILE"
traversal_tests=(""/.." "/../.." "/../../etc/passwd" "%2e%2e%2f" "%2e%2e%2f%2e%2e%2fetc%2fpasswd")
for test in "${traversal_tests[@]}"; do
    if curl -s -m $TIMEOUT "${TARGET}${test}" > /dev/null 2>&1; then
        echo "Traversal '$test': PASS" >> "$LOG_FILE"
    else
        echo "Traversal '$test': FAIL" >> "$LOG_FILE"
    fi
done

# Test 4: HTTP methods
echo -e "\n=== HTTP Method Tests ===" >> "$LOG_FILE"
methods=("GET" "POST" "PUT" "DELETE" "PATCH" "HEAD" "OPTIONS" "TRACE" "CONNECT" "FOOBAR" "")
for method in "${methods[@]}"; do
    if curl -s -X "$method" -m $TIMEOUT "$TARGET/" > /dev/null 2>&1; then
        echo "Method '$method': PASS" >> "$LOG_FILE"
    else
        echo "Method '$method': FAIL" >> "$LOG_FILE"
    fi
done

# Test 5: Rapid requests
echo -e "\n=== Rapid Request Test ===" >> "$LOG_FILE"
start=$(date +%s)
count=0
for i in {1..100}; do
    if curl -s -m 1 "$TARGET/" > /dev/null 2>&1; then
        count=$((count + 1))
    fi
done
end=$(date +%s)
elapsed=$((end - start))
echo "Rapid requests: $count in ${elapsed}s (rate: $((count / (elapsed > 0 ? elapsed : 1))) req/s)" >> "$LOG_FILE"

echo "Edge case testing complete. Results in $LOG_FILE"
```

### Fuzzing Script

```python
#!/usr/bin/env python3
# pulse_fuzzer.py

import socket
import random
import string
import time
import sys

def generate_random_string(length):
    """Generate random string of given length"""
    return ''.join(random.choice(string.ascii_letters + string.digits + '/._-~%') 
                   for _ in range(length))

def generate_malformed_request():
    """Generate a malformed HTTP request"""
    methods = ['GET', 'POST', 'PUT', 'DELETE', 'PATCH', 'FOOBAR', '']
    paths = [
        '/',
        '/test',
        '/../',
        '/../../etc/passwd',
        '/%2e%2e%2f',
        generate_random_string(random.randint(1, 2048)),
        ''.join(chr(i) for i in range(0, 256)),
        '\x00' * 10,
    ]
    
    method = random.choice(methods)
    path = random.choice(paths)
    
    # Randomly decide to include HTTP version
    if random.random() > 0.3:
        version = f'HTTP/{random.randint(0, 2)}.{random.randint(0, 99)}'
    else:
        version = ''
    
    request_parts = [method, path]
    if version:
        request_parts.append(version)
    
    request_line = ' '.join(request_parts)
    
    # Randomly add headers
    headers = []
    num_headers = random.randint(0, 5)
    for _ in range(num_headers):
        header_name = generate_random_string(random.randint(1, 20))
        header_value = generate_random_string(random.randint(0, 100))
        headers.append(f'{header_name}: {header_value}')
    
    # Build request
    request = request_line + '\r\n'
    for header in headers:
        request += header + '\r\n'
    request += '\r\n'
    
    # Randomly corrupt the request
    if random.random() > 0.7:
        # Insert random bytes
        corrupt_pos = random.randint(0, len(request))
        corrupt_bytes = bytes([random.randint(0, 255) for _ in range(random.randint(1, 10))])
        request = request[:corrupt_pos] + corrupt_bytes.decode('latin-1', errors='ignore') + request[corrupt_pos:]
    
    return request.encode('latin-1')

def fuzz(target='localhost', port=8383, iterations=1000, timeout=2):
    """Fuzz the target server"""
    crashes = 0
    errors = 0
    successes = 0
    
    for i in range(iterations):
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.settimeout(timeout)
            s.connect((target, port))
            
            request = generate_malformed_request()
            s.send(request)
            
            try:
                response = s.recv(4096)
                successes += 1
                
                # Check for error responses
                if b'error' in response.lower() or b'404' in response or b'400' in response:
                    errors += 1
                    if i % 100 == 0:
                        print(f"Iteration {i}: Error response - {request[:50]}")
                
            except socket.timeout:
                print(f"Iteration {i}: Timeout - {request[:50]}")
                errors += 1
            
            s.close()
            
        except ConnectionRefusedError:
            print(f"Iteration {i}: Server crashed! Reconnecting...")
            crashes += 1
            time.sleep(1)
        except Exception as e:
            print(f"Iteration {i}: Exception - {e}")
            errors += 1
        
        # Check if server is still running
        try:
            test_s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            test_s.settimeout(1)
            test_s.connect((target, port))
            test_s.close()
        except:
            print(f"Iteration {i}: Server appears to have crashed!")
            crashes += 1
            time.sleep(5)
        
        if i % 100 == 0:
            print(f"Progress: {i}/{iterations} (successes: {successes}, errors: {errors}, crashes: {crashes})")
    
    print(f"\nFuzzing complete:")
    print(f"  Total requests: {iterations}")
    print(f"  Successes: {successes}")
    print(f"  Errors: {errors}")
    print(f"  Crashes: {crashes}")
    
    return crashes > 0

if __name__ == '__main__':
    target = sys.argv[1] if len(sys.argv) > 1 else 'localhost'
    port = int(sys.argv[2]) if len(sys.argv) > 2 else 8383
    iterations = int(sys.argv[3]) if len(sys.argv) > 3 else 1000
    
    fuzz(target, port, iterations)
```

## Best Practices for Edge Case Testing

1. **Start Small:** Begin with simple, non-destructive tests
2. **Isolate Tests:** Test one variable at a time
3. **Monitor System:** Watch system resources during testing
4. **Document Everything:** Record all test cases and outcomes
5. **Automate:** Create scripts for repetitive tests
6. **Be Patient:** Some edge cases require specific timing or conditions
7. **Verify Fixes:** After fixing an issue, re-test to ensure it's resolved
8. **Test Regressions:** Ensure new fixes don't break existing functionality

## Pulse-Specific Edge Cases to Prioritize

Based on the code review, these are the most likely areas for edge cases:

1. **Request Parsing (`parseRequest` in `src/http/request.c`):**
   - Long method names
   - Long paths
   - Missing spaces
   - Multiple spaces
   - Non-ASCII characters
   - Null bytes

2. **Static File Serving (`routeStatic` in `src/http/routes.c`):**
   - Path traversal
   - Symlink following
   - Non-existent files
   - Very large files
   - Binary files
   - Device files
   - Proc/Sys files

3. **JSON Generation (`APIMetrics` in `src/app/routes.c`):**
   - Very long process names
   - Process names with special characters
   - Many processes (array bounds)
   - Buffer overflows in snprintf

4. **History Endpoints:**
   - Missing history files
   - Empty history
   - Corrupted history files

5. **Shared Memory Access:**
   - Concurrent access
   - Memory mapping failures
   - Invalid data in shared memory

6. **Socket Handling:**
   - Connection drops
   - Partial reads
   - Large requests
   - Many concurrent connections
