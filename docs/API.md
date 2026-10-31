# Pulse API

Pulse serves a JSON API when the web server is enabled:

```bash
pulse monitor --web
```

The default address is `http://localhost:8383`. Use `--port NUMBER` to choose
another port. The API routes use `GET`.

## Responses

Successful API responses are JSON objects with these common fields:

| Field | Type | Description |
| --- | --- | --- |
| `success` | boolean | `true` when the request succeeds. |
| `error` | string or null | `null` on success. |
| `timestamp` | integer | Unix timestamp in seconds when the response was created. |

An unknown path returns HTTP `404` and a JSON response like this:

```json
{
  "error": "Not Found",
  "success": false,
  "timestamp": 1781131285
}
```

## Current metrics

```http
GET /api/metrics
```

Returns the current system snapshot:

```json
{
  "error": null,
  "success": true,
  "timestamp": 1781131285,
  "cpu": {
    "idle": 9096832,
    "total": 9973308,
    "processes": 30307
  },
  "ip": {
    "ipv4": "192.168.1.20",
    "ipv6": "::1"
  },
  "process_count": 250,
  "disk": {
    "available": 70075166720,
    "total": 97574191104,
    "reads": 283625,
    "writes": 120
  },
  "memory": {
    "available": 9560580,
    "total": 16141972
  },
  "network": {
    "rx": 195424940,
    "tx": 80206860
  },
  "load": {
    "load1": 1.87,
    "load5": 1.41,
    "load15": 1.17
  },
  "hostname": "cherries",
  "processes": [
    {
      "pid": 4503,
      "ram": 795660,
      "cpu": 10834,
      "name": "pulse"
    }
  ],
  "uptime": 14625,
  "temp": 42000
}
```

| Field | Type | Description |
| --- | --- | --- |
| `cpu.idle` | integer | Cumulative idle CPU time from Linux. |
| `cpu.total` | integer | Cumulative CPU time from Linux. |
| `cpu.processes` | integer | Number of processes created since boot. This is not the current process count. |
| `ip.ipv4`, `ip.ipv6` | string | IPv4 and IPv6 addresses. |
| `process_count` | integer | Number of running processes found. |
| `disk.available`, `disk.total` | integer | Available and total bytes on `/`. |
| `disk.reads`, `disk.writes` | integer | Read and written sector counters from Linux disk statistics. |
| `memory.available`, `memory.total` | integer | Available and total memory in KiB. |
| `network.rx`, `network.tx` | integer | Received and transmitted bytes. |
| `load.load1`, `load.load5`, `load.load15` | number | Load averages over 1, 5, and 15 minutes. |
| `hostname` | string | System hostname. |
| `processes` | array | Process entries, limited by the monitor's `--processes` option. |
| `processes[].pid` | integer | Process ID. |
| `processes[].ram` | integer | Resident memory in KiB. |
| `processes[].cpu` | integer | Cumulative CPU time counter for the process. |
| `processes[].name` | string | Process name. |
| `uptime` | integer | System uptime in seconds. |
| `temp` | integer | Temperature in millidegrees Celsius. A value of `0` means no supported sensor reading was found. |

`cpu.idle`, `cpu.total`, and `cpu.processes` are cumulative counters, not
percentages. The top-level `process_count` is the current process count.

## Historical metrics

```http
GET /api/history/cpu
GET /api/history/ram
GET /api/history/disk
```

Each route returns saved samples for the requested resource:

```json
{
  "error": null,
  "success": true,
  "timestamp": 1783110731,
  "data": [
    {
      "timestamp": 1783110719,
      "usage": 13.801922
    },
    {
      "timestamp": 1783110721,
      "usage": 14.735517
    }
  ]
}
```

`data` is an array of samples. Each sample has a Unix timestamp in seconds and
a `usage` value in percent. History is collected while Pulse is running. The
snapshot frequency can be changed with `--history-snapshot-interval`.

## Dashboard style

```http
GET /api/style
```

Returns the selected dashboard style as a number:

```json
{
  "error": null,
  "success": true,
  "style": 1,
  "timestamp": 1781131285
}
```
