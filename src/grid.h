/** @file
 * @brief Sudoku grid functions header
 * @author 5cover, Matteo-K
 */

#ifndef SUDOKU_H
#define SUDOKU_H

#include <stdio.h>

#include "tCell.h"

// Using macros to maximize the performance of these simple functions and procedures called very frequently in the program.

#define grid_cellAt(grid, row, column) (grid).cells[at2d((grid).SIZE, row, column)]
#define grid_cellAtPos(grid, pos) grid_cellAt(grid, pos.row, pos.column)
/// @brief Defines whether a value is free or not at a position on the grid.
#define grid_markValueFree(isFree, grid, row, column, value)                       \
    (grid)._isColumnFree[at2d((grid).N * (grid).N, (column), (value))] = (isFree); \
    (grid)._isRowFree[at2d((grid).N * (grid).N, (row), (value))] = (isFree);       \
    (grid)._isBlockFree[at3d((grid).N, (grid).N, (row) / (grid).N, (column) / (grid).N, value)] = (isFree)

/// @brief Gets the axis index (a row or column number) of the start of the block containing the given index.
/// @param index in: an axis index (row or column number)
/// @return The axis index of the start of the block containing the given index.
#define grid_blockIndex(grid, index) (index - (index % (grid).N))

/// @brief Check if a value can be added to the grid. Does not take candidates into account.
/// @param grid in: the grid
/// @param row in: the cell's row
/// @param column in: the cell's column
/// @param value in: value to check
/// @return Whether the value can be added to the grid at the given position.
/// @remark According to the rules of Sudoku, a value can only be added to the grid when it is not already present in the row, in the column and in the block.
// After running tests, it seems that the fastest access order is column, row, block.
// This solutions is considerably faster than the naive alternative (iterating over cells)
// But it comes at a price : we must make sure that the state of candidates in the grid and the "_is*Free" arrays are synchronized from the start of the resolution to the backtracking call.
// For this we use the grid_markValueFree macro
#define grid_possible(grid, row, column, value)                     \
    ((grid)._isColumnFree[at2d((grid).N * (grid).N, column, value)] \
        && (grid)._isRowFree[at2d((grid).N * (grid).N, row, value)] \
        && (grid)._isBlockFree[at3d((grid).N, (grid).N, grid_blockIndex(grid, row) / (grid).N, grid_blockIndex(grid, column) / (grid).N, value)])

/// @brief Counts the number of possible values for a cell.
/// @param grid in: the grid
/// @param row in: the cell's row
/// @param column in: the cell's column
/// @param outVarName Name of the variable to declare and assign the result to.
/// @return The amount of values for which @ref grid_possible returns @c true.
#define grid_cellPossibleValuesCount(grid, row, column, outVarName) \
    tIntSize outVarName = 0;                                        \
    for (tIntSize val = 1; val <= (grid).SIZE; val++) {             \
        outVarName += grid_possible((grid), (row), (column), val);  \
    }

tGrid grid_create(tIntN const N);

/// @brief Loads a grid from a file in the Sud format.
/// @param inStream in: the file to read
/// @param N in: grid size factor
/// @param grid out: the loaded grid.
/// @return 0 if everything went well, or @ref ERROR_INVALID_DATA if the file contains invalid data.
int grid_load(FILE *inStream, tGrid *g);

/// @brief Writes a grid to a file in the Sud format.
/// @param grid in: the grid to write
/// @param outStream in: the file to write to
void grid_write(tGrid const *grid, FILE *outStream);

/// @brief Frees a grid.
/// @param grid in/out: the grid to free
void grid_free(tGrid *grid);

// The functions that affect the value of the cells are not defined in tCell because they need the coordinates of the cell on the grid.
// These coordinates allow for grid_possible to have a complexity of O(1), which represents a substantial performance gain.

/// @brief Removes a candidate from a cell of the grid and sets it as the cell's value if it's the last one.
/// @param grid in/out: the grid
/// @param row in: the cell's row
/// @param column in: the cell's column
/// @param candidate in: the candidate to remove
/// @return Whether the candidate has been removed.
bool grid_cell_removeCandidate(tGrid *grid, tIntSize row, tIntSize column, tIntSize candidate);

/// @brief Defines the value of a cell and removes all its candidates.
/// @param grid in/out: the grid
/// @param row in: the cell's row
/// @param column in: the cell's column
/// @param candidate in: the value to provide to the cell
void grid_cell_provideValue(tGrid *grid, tIntSize row, tIntSize column, tIntSize value);

/// @brief Removes a candidate from all cells of a row.
/// @param grid in/out: the grid
/// @param row in: the row
/// @param candidate in: the candidate to remove
/// @return Whether progress has been made.
bool grid_removeCandidateFromRow(tGrid *grid, tIntSize row, tIntSize candidate);

/// @brief Removes a candidate from all cells of a column.
/// @param grid in/out: the grid
/// @param column in: the column
/// @param candidate in: the candidate to remove
/// @return Whether progress has been made.
bool grid_removeCandidateFromColumn(tGrid *grid, tIntSize column, tIntSize candidate);

/// @brief Removes a candidate from all cells of a block.
/// @param grid in/out: the grid
/// @param row in: the row of a cell in the block
/// @param column in: the column of a cell in the block
/// @param candidate in: the candidate to remove
/// @return Whether progress has been made.
bool grid_removeCandidateFromBlock(tGrid *grid, tIntSize row, tIntSize column, tIntSize candidate);

// Grid display functions

/// @brief Prints a grid.
/// @param grid in: the grid to print
/// @param outStream in: the file to write to
void grid_print(tGrid const *grid, FILE *outStream);

/// @brief Prints a block separation line.
/// @param grid in: the grid
/// @param outStream in: the file to write to
void printBlockSeparationLine(tGrid const *grid, FILE *outStream);

/// @brief Prints a grid row.
/// @param grid in: @ref tGrid : the grid
/// @param row in: index of the row to print (between 0 and @ref SIZE - 1)
/// @param outStream in: the file to write to
void grid_printRow(tGrid const *grid, tIntSize row, FILE *outStream);

/// @brief Prints a grid value as a number or @ref DISPLAY_EMPTY_VALUE for empty values.
/// @param value in: the value to print
/// @param outStream in: the file to write to
void printValue(tIntSize value, FILE *outStream);

/// @brief Prints a character a specific amount of times.
/// @param character in: the character to print
/// @param times in: amount of times
/// @param outStream in: the file to write to
void printMultipleTimes(char character, unsigned times, FILE *outStream);

#endif // SUDOKU_H
