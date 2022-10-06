#!/bin/sh

echo "$(find "$1" -type f | wc -l) $(find "$1" -type d | wc -l)"
