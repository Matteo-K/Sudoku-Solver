# Sudoku Solve

Fast Linux Sudoku solver

Created by [Matteo-K](https://github.com/Matteo-K) and [Scover](https://github.com/5cover) (see [license](LICENSE)).

## Usage

`sudosolve [OPTION]... [N:integer]`

$N$ represents the grid size factor (usually 3 for regular Sudoku grids that have 9 rows and columns). Must be in range $[1 ; `INT_MAX_`]$.

The grid is read from standard input.

The solved grid is printed to standard ouptut in the Sud format.

### Options reference

Option|Description
-|-
`-s`|**See** the grid: Don't solve the grid before printing it (default is to solve the grid)
`-h`|Print the grid in a **human-readable** format (default is Sud format)

### Examples

Solve a grid then view it:

`sudosolve -h < grid.sud`

Solve a grid and save the solved grid in the Sud format:

`sudosolve < grid.sud > solved.sud`

View a grid:

`sudosolve -sh < grid.sud`

### Remarks

The maximum value of $N$ is only the theoretical limit of the Sud format, and does not account for memory or time limitations.

Even a grid of size $N=100$ would consume 933 gigabytes of memory assuming `sizeof(bool) == 1`.

## Sud file format

Binary format for a Sudoku grid.

Value are stored as unsigned little-endian 4-byte integers.

File size is $4N^4$ bytes.

Empty values are indicated by 0.

Since the values are stored as 4-bytes integers the maximum is $2^{31}-1$, hence the maximum value of N, $\lfloor\sqrt{2^{32}-1}\rfloor = 65535$.
