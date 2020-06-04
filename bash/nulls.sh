#!/usr/bin/env bash

for i in "$@"; do
  printf "%s\0" "$i"
done | base64
