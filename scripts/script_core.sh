#!/usr/bin/bash

# Contains some variables and functions for reuse in scripts

extension=".msh"
dir="./test"
daemon="./daemon/mash_d"
interp="./interpreter/build/mash"

daemon_prog=$(basename $daemon)
files=$(find "$dir" -name "*$extension" -type f)