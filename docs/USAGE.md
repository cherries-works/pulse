# Using Pulse

Pulse is a lightweight Linux system monitor. It starts the terminal dashboard
by default and can also serve a website and JSON API.

## Build from source

```bash
git clone https://github.com/cherries-works/pulse.git
cd pulse
make
./dist/pulse
```

The examples below use `pulse` from your `PATH`. If you built from source without
installing it, replace `pulse` with `./dist/pulse`.

## Commands

```bash
pulse help
```

Put the command before its options. With no command, Pulse runs `monitor`.

| Command | Purpose | Options |
| --- | --- | --- |
| `monitor` | Run the terminal dashboard and optionally the website/API. | See the monitoring options below. |
| `info` | Display system information. | `--json`, `--no-color` |
| `top` | Print the top processes. | `--processes NUMBER` (1–100, default 3), `--sort cpu\|ram` (default `ram`), `--json`, `--no-color` |
| `snapshot` | Print the current system snapshot. | `--json`, `--no-color` |
| `process` | Print details for a process. | `--process PID` (required), `--json`, `--no-color` |
| `stop` | Stop Pulse's running processes. | None. |
| `help` | Print the command reference. | `--no-color` |
| `prune` | Delete stored logs, history, or both. | `--prune history\|logs\|all`, `--keep NUMBER`, `--until YYYY-MM-DD`, `--json` |
| `config` | Open the configuration in nano. | `--current` prints it instead, `--json` formats `--current` as JSON, `--reset` resets it to defaults. |
| `version` | Print the version. | `--hash` prints the commit hash instead, `--json` prints both. |

`-nc` is an alias for `--no-color`. `config --reset` overwrites the current
configuration, and `prune` deletes stored data.

## Monitoring options

| Option | Meaning |
| --- | --- |
| `--web` | Serve the website and API on port 8383. |
| `--port NUMBER` | Choose the web port; this also enables `--web`. |
| `--headless` | Run without the terminal dashboard, for example with `--web`. |
| `--sleep SECONDS` | Set the update interval (1–60 seconds, default 1). |
| `--history-snapshot-interval NUMBER` | Save history every NUMBER update cycles (1–3600, default 2). |
| `--processes NUMBER` | Display 1–10 processes (default 3). |
| `--sort cpu\|ram` | Sort processes by CPU or RAM usage (default `ram`). |
| `--no-color`, `-nc` | Disable ANSI colors and styling. |

### Website and API

```bash
pulse monitor --web --port 12345
```

Open `http://localhost:12345` for the dashboard or
`http://localhost:12345/api/metrics` for JSON metrics. Omit `--port` to use 8383.
To run without the terminal dashboard:

```bash
pulse monitor --web --headless
```

Stop the running Pulse processes with `pulse stop`.

### Terminal dashboard

Show up to ten processes without colors:

```bash
pulse monitor --no-color --processes 10
```

Update every ten seconds and sort two processes by CPU usage:

```bash
pulse monitor --sleep 10 --processes 2 --sort cpu
```

## One-shot output

```bash
pulse info --json
pulse top --processes 10 --sort cpu --json
pulse process --process 1234 --json
pulse config --current --json
pulse version --json
```

Replace `1234` with a running process ID. To print the latest snapshot, start
monitoring first, then run `pulse snapshot --json` in another terminal.
