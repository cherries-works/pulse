#!/usr/bin/env bash
set -euo pipefail

binary=${1:-./dist/pulse}
pid=2147483647
expected="Error: process with pid \"${pid}\" was not found."

if output=$("$binary" process --process "$pid" 2>&1); then
    printf 'expected process lookup to fail for pid %s\n' "$pid" >&2
    exit 1
fi

if [[ "$output" != *"$expected"* ]]; then
    printf 'expected error %q; got:\n%s\n' "$expected" "$output" >&2
    exit 1
fi

printf 'process-not-found CLI regression passed\n'
