#!/bin/env bash
set -euo pipefail

if [[ $# -ne 2 ]]; then
    >&2 echo "Usage : $0 <exe:file> <grid:file>"
    exit 1
fi

# Resolve arguments
file_exe=$(realpath -e $1)
file_grid=$(realpath -e $2)

# File size : 4N⁴ bytes
# N = (file size / 4)^(1/4)
size=$(stat -c %s $file_grid)
n=$(bc <<<  "sqrt(sqrt($size / 4))")

$file_exe < $file_grid | scripts/check.py $n
echo N=$n
