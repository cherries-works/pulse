#!/bin/sh

docker run -d -it --name pulse \
  --pid=host \
  -v /proc:/host/proc:ro \
  -p 8383:8383 \
  cw_pulse:latest monitor --web
