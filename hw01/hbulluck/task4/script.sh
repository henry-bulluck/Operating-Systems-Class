#!/bin/sh

tar -czvf "$(basename $1)_$(date +%s).tar" -C "$1/.." "$(basename $1)"
