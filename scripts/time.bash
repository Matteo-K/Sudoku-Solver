#!/bin/env bash
set -euo pipefail

if [[ $# -ne 3 ]]; then
    >&2 echo "Usage : $0 <exe:file> <grid:file> <count:int>"
    exit 1
fi

# Resolve arguments
file_exe=$(realpath -e $1)
file_grid=$(realpath -e $2)
int_count=$3

sum=0

for i in $(seq $int_count); do
    time=$($file_exe <<< $file_grid | tail -n 1 | colrm 1 27 | head -c -5)
    echo "$i/$int_count: $time" >&2
    sum=$(bc <<< "$sum + $time * 1000")
done

echo $(bc <<< "scale = 3; $sum / $int_count") ms