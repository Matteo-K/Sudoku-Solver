#!/bin/env bash
set -euo pipefail

if [[ $# -ne 4 ]]; then
    >&2 echo "Usage : $0 <exe:file> <grid:file> <gridName:str> <out:dir>"
    exit 1
fi

# Resolve arguments
file_exe=$(realpath -e $1)
file_grid=$(realpath -e $2)
str_gridName=$3
dir_out=$(realpath -m $4)

pushd $(dirname $file_exe)

# Run program
$file_exe < $file_grid

# Run gprof
mkdir -p $dir_out
gprof $file_exe gmon.out > $dir_out/gprof_report_${str_gridName}_$(date +"%H-%M-%S").txt

# Cleanup
rm gmon.out

popd
