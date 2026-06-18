This file is used as a sort of diary.

# Pulse

The core functionallity of [Cherries] Pulse is a simple monitoring program. CPU, and RAM usage,
json outputs, API hosting, anomaly detection, monitor services, view leaks, get warned, export
to other tools, and use little to not memory.

Added network support, so fetching the **/** route returns the JSON output of the current system
metrics.

Changed it up, now network requests for metrics work through the path **/api/metrics**, and Pulse can
now host static files and return JSON. It utilizes macros, to de-dupe code. C does not have
closures unfortunately, meaning that each route has his own function definition. Hoping to find
a quick alternative to that.

Displaying the top 3 processes that take up the most amount of RAM. Terminal and the Web UI both
display the same metrics. No graphs have yet been implemented (10th June 2026).

The website styling is finished. No media queries added, will be added later. As of now, v0.1.0 seems to be ready.