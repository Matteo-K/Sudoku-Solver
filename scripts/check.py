#!/bin/env python3

import sys
from typing import Iterable

from grid import grid_parse


def chain(*iterables: Iterable) -> Iterable:
    for iterable in iterables:
        yield from iterable


def grid_get_errors(grid: tuple[tuple[int]], N: int) -> list[str]:
    SIZE = N * N
    EXPECTED_VALUE_SET = set(range(1, SIZE + 1))

    def get_grid_size_error():
        rowCount = len(grid)
        columnCounts = map(lambda row: len(row), grid)
        if rowCount == SIZE and all(map(lambda count: count == SIZE, columnCounts)):
            return None
        return (f"Grid has invalid size ({rowCount} rows and {set(columnCounts) or 0} columns)",)

    def get_blocks():
        for blockRow in range(0, SIZE, N):
            for blockCol in range(0, SIZE, N):
                block = []
                for r in range(N):
                    for c in range(N):
                        block.append(grid[blockRow + r][blockCol + c])
                yield (block, f'Block at row {blockRow + 1}, column {blockCol + 1}')

    def get_error(sliceValues, slicePosition):
        valueSet = set(sliceValues)
        if valueSet == EXPECTED_VALUE_SET:
            return None

        msgParts = []

        # Values that should be there but aren't
        missing = EXPECTED_VALUE_SET.difference(valueSet)
        if missing:
            msgParts.append(f'missing {missing}')

        # Values that shouldn't be there but are
        unexpected = valueSet.difference(EXPECTED_VALUE_SET)
        if unexpected:
            msgParts.append(f'unexpected {unexpected}')

        duplicates = set(
            val for val in sliceValues if sliceValues.count(val) > 1)
        if duplicates:
            msgParts.append(f'duplicate {duplicates}')

        return f'{slicePosition}: {", ".join(msgParts)}'

    return get_grid_size_error() or map(lambda gridSlice: get_error(*gridSlice), chain(
        ((row, f"row {i+1}") for i, row in enumerate(grid)),
        ((column, f"column {i+1}")
         for i, column in enumerate(zip(*grid))),
        get_blocks()))


def printUsage():
    print(f"Usage : {sys.argv[0]} <N>")


if __name__ == "__main__":

    if len(sys.argv) not in (2, 3):
        printUsage()
        exit(1)

    quiet = len(sys.argv) == 3 and 'q' in sys.argv[1]

    try:
        n = int(sys.argv[2 if len(sys.argv) == 3 else 1])
    except ValueError:
        printUsage()
        exit(1)

    gridParseResult = grid_parse(sys.stdin)
    if not quiet:
        print(gridParseResult.text, end="")

    errors = list(filter(None, grid_get_errors(gridParseResult.grid, n)))
    if errors:
        print("\n".join(errors))
        print(f"This Sudoku grid is not valid ({len(errors)} errors).")
    else:
        print("This Sudoku grid is valid.")
