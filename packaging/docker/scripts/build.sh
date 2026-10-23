#!/bin/sh

docker build -t cw_pulse -f ./packaging/docker/Dockerfile --no-cache .