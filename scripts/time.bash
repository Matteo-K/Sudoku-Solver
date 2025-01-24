#!/bin/env bash
set -euo pipefail

if [[ $# -lt 3 ]]; then
    echo >&2 "Usage : $0 <exe:file> <grid:file> <count:int>"
    exit 1
fi

# Resolve arguments
file_exe=$(realpath -e "$1")
file_grid=$(realpath -e "$2")
int_count="$3"

sum=0
n=$(bc <<<"sqrt(sqrt($(stat -c %s "$file_grid") / 4))")

for i in $(seq "$int_count"); do
    start=$(date +%s.%N)
    $file_exe $n <"$file_grid" >/dev/null
    end=$(date +%s.%N)
    time=$(bc <<<"($end - $start) * 1000")
    LC_NUMERIC="en_US.UTF-8" printf "$i/$int_count: %.6f ms\n" "$time" >&2
    sum=$(bc <<<"$sum + $time")
done

echo "$(bc <<<"scale = 6; $sum / $int_count") ms"
