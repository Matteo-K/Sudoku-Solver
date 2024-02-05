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

# If the wildcard finds no match
if ! compgen -G "*.c" > /dev/null; then
    >&2 echo "In order for gcov to work, the exe ($file_exe) must be in the same directory as the C source files."
    exit 2
fi

# Run program
$file_exe < $file_grid

# Run gcov
dir_out=$dir_out/gcov_${str_gridName}_$(date +'%H-%M-%S')
mkdir -p $dir_out

gcov -kmq *.c > $dir_out/report.txt

# Recover result
mv *.gcov $dir_out

# Cleanup
rm *.gc{no,da}

popd