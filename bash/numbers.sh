#!/usr/bin/env bash

die() {
  echo "$me: Error $*"
  exit 1
}

image_size=$1

# single bracket required here...
[ "$image_size" -eq "$image_size" ] 2>/dev/null || die not number

image_size=$((image_size))
