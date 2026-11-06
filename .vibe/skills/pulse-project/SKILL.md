---
name: pulse-project
description: Comprehensive knowledge and understanding of the Pulse system monitoring project - its architecture, codebase structure, components, and design philosophy.
tags:
  - pulse
  - monitoring
  - c
  - system
  - architecture
when: >
  - User requests work on or understanding of the Pulse project
  - User needs to modify, extend, or debug Pulse
  - Security testing or code analysis of Pulse is requested
  - User asks about Pulse's architecture, components, or design
priority: 10
---

# Pulse Project Skill

## Overview

Pulse is a lightweight Linux system monitoring tool written in C. It collects system metrics directly from `/proc` and `/sys`, exposing them through a minimal web dashboard and JSON API. The project emphasizes:

- **Simplicity**: No accounts, no complex setup
- **Lightweight**: Minimal resource usage
- **Speed**: Fast, efficient metrics collection
- **Self-contained**: No external dependencies beyond a C compiler

## Project Structure

```
pulse/
├── include/                    # Header files
│   ├── app.h                  # Application headers
│   ├── config.h               # Configuration structures
│   ├── daemon.h               # Daemon/monitoring logic
│   ├── http.h                 # HTTP server definitions
│   ├── log.h                  # Logging functionality
│   ├── os.h                   # OS-specific utilities
│   ├── parse.h                # Data parsing utilities
│   ├── render.h               # Rendering/HTML generation
│   ├── setup.h                # Setup and initialization
│   └── utils.h                # Utility functions
├── src/                       # Source files
│   ├── app/                   # Application logic
│   │   ├── routes.c           # API and web routes
│   │   └── start.c            # Application startup
│   ├── config/                # Configuration handling
│   ├── daemon/                # Daemon processes
│   ├── http/                  # HTTP server implementation
│   │   ├── request.c          # HTTP request parsing
│   │   ├── routes.c           # Route handling logic
│   │   └── server.c           # HTTP server core
│   ├── log/                   # Logging implementation
│   ├── parse/                 # Data parsing
│   ├── render/                # HTML/response rendering
│   ├── setup/                 # Setup utilities
│   └── utils/                 # Utility implementations
├── contrib/                   # Contributed packaging and scripts
├── test/                      # Test files
├── docs/                      # Documentation
└── Makefile                   # Build configuration
```

## Core Components

### 1. HTTP Server (`src/http/`)

**Architecture:**
- Custom HTTP server written from scratch in C
- Single-threaded, blocking I/O model
- Uses `socket()`, `bind()`, `listen()`, `accept()` for TCP connections
- Handles each request synchronously

**Key Files:**
- `server.c`: Socket creation, binding, listening, and request handling loop
- `request.c`: HTTP request parsing (method, path extraction)
- `routes.c`: Route registration and handling, static file serving, JSON responses

**Security Considerations:**
- Buffer sizes defined as constants (BUFFER_ONE_MB, BUFFER_ONE_KB)
- No request size limits enforced beyond buffer sizes
- No authentication or authorization
- No rate limiting
- Path-based routing with exact string matching

**Endpoints:**
```
GET /                           - Dashboard HTML
GET /css/style.css              - Stylesheet
GET /js/script.js               - JavaScript
GET /assets/favicon.png         - Favicon
GET /api/style                  - Current style setting
GET /api/metrics                - Current system metrics (JSON)
GET /api/history/cpu            - CPU usage history
GET /api/history/ram            - RAM usage history
GET /api/history/disk           - Disk usage history
GET /api/history/network/rx     - Network RX history
GET /api/history/network/tx     - Network TX history
```

### 2. Metrics Collection (`src/daemon/`)

**Data Sources:**
- `/proc/stat` - CPU statistics
- `/proc/meminfo` - Memory information
- `/proc/diskstats` - Disk I/O statistics
- `/proc/net/dev` - Network statistics
- `/proc/loadavg` - Load averages
- `/proc/uptime` - System uptime
- `/sys/class/thermal/` - Temperature
- `/proc/[pid]/` - Process information

**Shared Memory:**
- Uses `mmap()` with shared memory for inter-process communication
- Data structure: `CWData` contains snapshots of system metrics

### 3. Request Parsing (`src/http/request.c`)

**Function:** `parseRequest(char *buffer)`

**Behavior:**
1. Splits request by newline to get first line
2. Splits by first space to extract method
3. Splits by next space to extract path
4. Supports methods: GET, POST, PUT, DELETE, PATCH
5. Defaults to GET with path "/" on parse failure

**Vulnerabilities to Consider:**
- No validation of method strings (buffer overflow if method is very long)
- No URL decoding of paths
- No path normalization (../ traversal possible?)
- No length checking on path

### 4. Route Handling (`src/http/routes.c`)

**Key Functions:**
- `routeHandle()`: Iterates through registered routes, matches method and path exactly
- `route()`: Registers a new route with handler
- `routeStatic()`: Serves static files based on extension
- `routeJSON()`: Sends JSON responses

**Static File Serving:**
- Uses file extension to determine Content-Type
- No path sanitization
- No directory traversal protection
- Opens files with `fopen()` using path directly from request

**Potential Issues:**
- Path traversal: `../../../etc/passwd` could be requested
- No Content-Length validation
- No MIME type verification

### 5. JSON API (`src/app/routes.c`)

**API Endpoints:**

1. **`/api/metrics`** - Full system snapshot
   - CPU: idle, total, processes (cumulative since boot)
   - Memory: available, total (in KiB)
   - Disk: available, total, reads, writes
   - Network: rx, tx (bytes)
   - Load: load1, load5, load15
   - Processes: array of top processes (configurable limit)
   - Hostname, uptime, temperature

2. **`/api/history/*`** - Historical metrics
   - Stored in shared memory
   - Read from history files in `~/.local/share/pulse/history/`

**JSON Generation:**
- Uses `snprintf()` to build JSON strings
- Manual escaping of process names with `escapeJson()`
- Fixed buffer sizes (BUFFER_ONE_KB * 32)

**Potential Issues:**
- Buffer overflow in JSON construction
- No escaping validation
- Information disclosure (process names, paths, IPs)

## Configuration

**Default:** `src/config/default.toml`

**Runtime Configuration:**
- Port: `--port` flag (default: 8383)
- Web interface: `--web` flag
- History snapshot interval: `--history-snapshot-interval`
- Process limit: `--processes`
- Style: `--style` (1, 2, or 3)

## Build System

**Simple Makefile:**
```bash
make        # Build
make clean  # Clean build artifacts
```

**Dependencies:**
- C compiler (GCC, Clang)
- make
- Standard C library

## Security Architecture

### What Pulse Does:
- Binds to all interfaces by default (0.0.0.0)
- Runs with privileges of the executing user
- Reads system files directly
- Opens socket for incoming connections
- Serves files from working directory

### What Pulse Does NOT Do:
- Authentication
- Authorization
- Rate limiting
- Request validation
- Input sanitization
- HTTPS/SSL/TLS
- CORS headers
- Security headers

## Known Attack Surface

1. **Network Attack Surface:**
   - Open TCP port (default 8383)
   - HTTP server accepts any request
   - No authentication required
   - Information disclosure through API

2. **File System Attack Surface:**
   - Static file serving without path validation
   - Potential path traversal vulnerabilities
   - Reads system files directly

3. **Memory Safety:**
   - Written in C with manual memory management
   - Uses fixed-size buffers
   - Uses `snprintf()` for buffer safety
   - Potential for buffer overflows in string operations

4. **Information Disclosure:**
   - Process list with names and PIDs
   - System hostname
   - IP addresses (IPv4 and IPv6)
   - System uptime
   - CPU, memory, disk, network statistics
   - Temperature readings

## Design Philosophy

From `docs/PHILOSOPHY.md`:
- "Fast, simple, and efficient monitoring that just works"
- "No accounts, or no setup is not a design flaw, it's the goal"
- "Reads everything from /proc and /sys"
- "HTTP server written from scratch"
- "Serves a single lightweight page"
- "Consumes little to no RAM"

## Testing Approach

When pentesting or analyzing Pulse, focus on:

1. **Input Validation:**
   - HTTP request parsing (method, path, headers)
   - Path traversal in static file serving
   - Buffer overflows in request handling

2. **API Security:**
   - Information disclosure in `/api/metrics`
   - Directory traversal in history endpoints
   - JSON injection

3. **Resource Exhaustion:**
   - Large request handling
   - Many concurrent connections
   - Memory exhaustion through large responses

4. **Code Quality:**
   - Memory safety in C code
   - Error handling
   - Race conditions in shared memory
   - File descriptor leaks

## Useful Commands

```bash
# Build
cd pulse && make

# Run with web interface
./dist/pulse --web

# Run on custom port
./dist/pulse --web --port 8080

# View metrics
curl http://localhost:8383/api/metrics | jq

# Check running processes
ps aux | grep pulse
```
