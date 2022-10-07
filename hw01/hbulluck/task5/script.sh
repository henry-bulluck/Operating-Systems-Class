#!/bin/bash
stdout=$1
stderr=$2
cwd=$3
subcmd="${@:4}"

(
cd $cwd

$subcmd
)>$stdout 2>$stderr
