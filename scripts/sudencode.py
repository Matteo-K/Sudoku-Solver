#!/bin/env python3

import sys

from grid import grid_parse, grid_encode

if __name__ == "__main__":
    g = grid_parse(sys.stdin).grid
    grid_encode(g, sys.stdout.buffer)
