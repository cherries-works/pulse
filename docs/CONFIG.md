# Pulse configuration

Pulse creates its configuration file the first time it runs:

```text
~/.cherries-works/pulse/config.toml
```

Print the current file or open it in an editor with:

```bash
pulse config --current
pulse config
```

Reset it to the built-in defaults with `pulse config --reset`.

## Example

```toml
[alerts]
enabled = true

[alerts.cpu]
threshold = 99
operator = "<"
duration = "1s"
cooldown = "2s"

[alerts.ram]
threshold = 10
operator = "<"
duration = "5s"
cooldown = "10s"

[alerts.disk]
threshold = 90
operator = "<"
duration = "15s"
cooldown = "30s"

[notifications.desktop]
enabled = true
title = "Pulse Alert"
message = "{resource} is above {value}%"

[notifications.discord]
enabled = false
webhook = ""
message = "**Pulse:** {resource} reached {value}%"

[notifications.command]
enabled = false
command = ""
```

## Alerts

`[alerts]` controls whether resource alerts are checked. Each resource has its
own section: `[alerts.cpu]`, `[alerts.ram]`, and `[alerts.disk]`.

| Setting | Type | Description |
| --- | --- | --- |
| `enabled` | boolean | Turns alert checking on or off. |
| `threshold` | integer | Percentage value to compare against. |
| `operator` | string | Comparison used with the threshold: `>`, `<`, `>=`, `<=`, `==`, or `!=`. |
| `duration` | duration | How long the condition must hold before sending an alert. |
| `cooldown` | duration | Minimum time between alerts while the condition continues. |

Durations use seconds (`s`), minutes (`m`), hours (`h`), or days (`d`). For
example, `"90s"`, `"5m"`, and `"1h30m"` are duration values.

The built-in thresholds are 99 for CPU, 10 for RAM, and 90 for disk. The
comparison direction is set by `operator`.

The current config parser does not remove quotes from `operator` values.
Because of that, quoted operators such as the defaults shown above currently
fall back to `>`. The choices in the table describe the intended operators,
but the configured comparison may not match until this parser behavior is
fixed.

## Notifications

Notifications are configured separately, and more than one type can be enabled
at the same time.

### Desktop

`[notifications.desktop]` sends a desktop notification using `notify-send`.

| Setting | Type | Description |
| --- | --- | --- |
| `enabled` | boolean | Turns desktop notifications on or off. |
| `title` | string | Notification title. |
| `message` | string | Notification text. |

### Discord

`[notifications.discord]` posts a message to a Discord webhook.

| Setting | Type | Description |
| --- | --- | --- |
| `enabled` | boolean | Turns Discord notifications on or off. |
| `webhook` | string | Discord webhook URL. |
| `message` | string | Message sent to the webhook. |

### Command

`[notifications.command]` runs a command when an alert fires.

| Setting | Type | Description |
| --- | --- | --- |
| `enabled` | boolean | Turns command notifications on or off. |
| `command` | string | Command to run. |

Only enable commands that you trust.

### Message placeholders

Desktop and Discord messages can include these placeholders:

| Placeholder | Description |
| --- | --- |
| `{resource}` | Resource name: `CPU`, `RAM`, or `Disk`. |
| `{value}` | Current resource usage, formatted to one decimal place. |

For example, `{resource} is at {value}%` could become `CPU is at 92.4%`.