#!/bin/sh

echo "$(find "$@" -type f | wc -l) $(find "$@" -type d | wc -l)"
