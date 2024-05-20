# Sudone

Sudone - an optimized Sudoku solver

The input grid is read from standard input and the result is printed to standard output.

Created by [Matteo-K](https://github.com/Matteo-K) and [Scover](https://github.com/5cover) (see [license](LICENSE)).

## Usage

`sudone N`

N : grid size factor (usually 3 for regular Sudoku grids that have 9 rows and columns).

### Options

Option|Description
-|-
`-s`|*Solve* the grid before printing it.
`-b`|*Binary* (Sud format) grid output
`--help`|Print *help* and exit.

### Examples

Solve a grid then view it:

`sudone -s < grid.sud`

Solve a grid and save the solved grid in the Sud format:

`sudone -sb < grid.sud > solved.sud`

View a grid:

`sudone < grid.sud`

### Remarks

The maximum value of $N$ is only the theoretical limit of the Sud format, and does not account for memory or time limitations.

Even a grid of size $N=100$ would consume 933 gigabytes of memory assuming `sizeof(bool) == 1`.

## Sud file format

Binary format for a Sudoku grid.

Value are stored as unsigned little-endian 32-bit integers.

File size is $4N^4$ bytes.

Empty values are indicated by 0.
