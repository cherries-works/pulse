#!/bin/sh

docker build -t cw_pulse -f ./contrib/packaging/docker/Dockerfile --no-cache .