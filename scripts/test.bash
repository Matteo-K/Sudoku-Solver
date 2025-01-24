#!/bin/env bash
set -euo pipefail

if [[ $# -lt 2 ]]; then
    echo >&2 "Usage : $0 <exe:file> <grid:file>"
    exit 1
fi

# Resolve arguments
file_exe=$(realpath -e $1)
file_grid=$(realpath -e $2)

# File size : 4N⁴ bytes
# N = (file size / 4)^(1/4)
n=$(bc <<<"sqrt(sqrt($(stat -c %s $file_grid) / 4))")

$file_exe $n -sb <$file_grid | scripts/check.py $n
echo N=$n
