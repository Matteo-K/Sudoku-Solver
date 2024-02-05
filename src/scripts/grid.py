import math

from typing import List, BinaryIO, TextIO


class GridParseResult:
    def __init__(self, text: str, grid: List[List[int]]):
        self._text = text
        self._grid = grid

    @property
    def text(self) -> str:
        return self._text

    @property
    def grid(self) -> List[List[int]]:
        return self._grid


def grid_parse(txtFile: TextIO) -> GridParseResult:
    """Parses a grid from txtFile in text format."""
    grid = []
    gridTxt = ""
    for line in txtFile:
        gridTxt += line
        if line.startswith("+"):
            continue
        row = []
        number = ""
        for char in line:
            if char.isdigit():
                number += char
            else:
                if char == '.':
                    row.append(0)
                if number != "":
                    row.append(int(number))
                    number = ""
        if row:
            grid.append(row)
    return GridParseResult(gridTxt, grid)


def grid_print(grid: List[List[int]], txtFile: TextIO):
    """Writes the grid to txtFile in text format."""
    size = len(grid)
    n = int(size ** 0.5)
    padding = int(math.log10(size)) + 1

    def print_separation_row():
        print((n * (padding + 2) * '-').join(['+' for _ in range(n + 1)]))

    print_separation_row()
    for i, row in enumerate(grid):
        print('|', end='')
        for j, value in enumerate(row):
            print(f" {'.' if value == 0 else value:>{padding}} ", end='')
            if (j + 1) % n == 0:
                print('|', end='')
        print()
        if (i + 1) % n == 0:
            print_separation_row()


def grid_encode(grid: List[List[int]], binFile: BinaryIO):
    """Writes the grid to stdout in binary format."""
    for row in grid:
        for value in row:
            binFile.write(value.to_bytes(4, byteorder='little', signed=False))


def grid_decode(n: int, binFile: BinaryIO) -> List[List[int]]:
    """Reads a grid of size n⁴ from binFile in binary format."""
    grid = []
    for _ in range(n * n):
        row = []
        for _ in range(n * n):
            row.append(int.from_bytes(binFile.read(
                4), byteorder='little', signed=True))
        grid.append(row)
    return grid
