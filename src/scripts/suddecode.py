#!/bin/env python3

import sys

from grid import grid_decode, grid_print

if __name__ == "__main__":
    if (len(sys.argv) != 2 or not sys.argv[1].isdecimal()):
        print(f"Usage: {sys.argv[0]} N", file=sys.stderr)
        sys.exit(1)
    n = int(sys.argv[1])
    g = grid_decode(n, sys.stdin.buffer)
    grid_print(g, sys.stdout)
