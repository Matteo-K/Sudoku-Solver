#!/bin/env bash
set -euo pipefail

if [[ $# -lt 4 ]]; then
    echo >&2 "Usage : $0 <exe:file> <grid:file> <gridName:str> <out:dir>"
    exit 1
fi

# Resolve arguments
file_exe=$(realpath -e "$1")
file_grid=$(realpath -e "$2")
str_gridName=$(tr '/' '-' <<<"$3")
dir_out=$(realpath -m "$4")

n=$(bc <<<"sqrt(sqrt($(stat -c %s "$file_grid") / 4))")

pushd "$(dirname "$file_exe")"

# If the wildcard finds no match
if ! compgen -G "*.c" >/dev/null; then
    echo >&2 "In order for gcov to work, the exe ($file_exe) must be in the same directory as the C source files."
    exit 2
fi

# Run program
$file_exe "$n" -s <"$file_grid"

# Run gcov
dir_out=$dir_out/gcov_${str_gridName}_$(date +'%H-%M-%S')
mkdir -p "$dir_out"

gcov -kmq *.c >"$dir_out"/report.txt

# Recover result
mv *.gcov "$dir_out"

# Cleanup
rm *.gc{no,da}

popd
