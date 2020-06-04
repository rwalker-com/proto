#!/usr/bin/env bash

set -x

readarray -d '' ARGS < <(base64 -d)

echo "${ARGS[@]}"
