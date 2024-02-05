/** @file
 * @brief Sudoku grid functions header
 * @author 5cover, Matteo-K
 */

#ifndef SUDOKU_H
#define SUDOKU_H

#include <stdio.h>

#include "tCell.h"

// Using macros to maximize the performance of these simple functions and procedures called very frequently in the program.

/// @brief Defines whether a value is free or not at a position on the grid.
#define GRID_MARK_VALUE_FREE(grid, row, column, value, isFree) \
    grid->_isColumnFree[column][value] = (isFree);             \
    grid->_isRowFree[row][value] = (isFree);                   \
    grid->_isBlockFree[(row) / N][(column) / N][value] = (isFree)

/// @brief Gets the axis index (a row or column number) of the start of the block containing the given index.
/// @param index in: an axis index (row or column number)
/// @return The axis index of the start of the block containing the given index.
#define BLOCK_INDEX(index) (index - (index % N))

/// @brief Check if a value can be added to the grid. Does not take candidates into account.
/// @param grid in: the grid
/// @param row in: the cell's row
/// @param column in: the cell's column
/// @param value in: value to check
/// @return Whether the value can be added to the grid at the given position.
/// @remark According to the rules of Sudoku, a value can only be added to the grid when it is not already present in the row, in the column and in the block.
// After running tests, it seems that the fastest access order is column, row, block.
#define POSSIBLE(grid, row, column, value) \
    (grid->_isColumnFree[column][value]    \
     && grid->_isRowFree[row][value]       \
     && grid->_isBlockFree[BLOCK_INDEX(row) / N][BLOCK_INDEX(column) / N][value])

/// @brief Counts the number of possible values for a cell.
/// @param grid in: the grid
/// @param row in: the cell's row
/// @param column in: the cell's column
/// @param outVarName Name of the variable to declare and assign the result to.
/// @return The amount of values for which @ref POSSIBLE returns @c true.
#define GRID_CELL_POSSIBLE_VALUES_COUNT(grid, row, column, outVarName) \
    int outVarName = 0;                                                \
    for (int val = 1; val <= SIZE; val++) {                            \
        outVarName += POSSIBLE(grid, row, column, val);                \
    }

/// @brief Initializes a grid from a file.
/// @param inStream in: the *.sud file to read
/// @param grid out: the grid to initialize
/// @return 0 if everything went well, or @ref ERROR_INVALID_DATA if the file contains invalid data.
int grid_load(FILE *inStream, tGrid *grid);

/// @brief Initializes the grid: adds the candidates and marks the values as possible or not.
/// @param grid in/out: the grid
/// @param gridValues in: the grid's values
void grid_initialize(tGrid *grid, tIntegerGrid gridValues);

// The functions that affect the value of the cells are not defined in tCell because they need the coordinates of the cell on the grid.
// These coordinates allow for POSSIBLE to have a complexity of O(1), which represents a substantial performance gain.

/// @brief Removes a candidate from a cell of the grid and sets it as the cell's value if it's the last one.
/// @param grid in/out: the grid
/// @param row in: the cell's row
/// @param column in: the cell's column
/// @param candidate in: the candidate to remove
/// @return Whether the candidate has been removed.
bool grid_cell_removeCandidate(tGrid *grid, int row, int column, int candidate);

/// @brief Defines the value of a cell and removes all its candidates.
/// @param grid in/out: the grid
/// @param row in: the cell's row
/// @param column in: the cell's column
/// @param candidate in: the value to provide to the cell
void grid_cell_provideValue(tGrid *grid, int row, int column, int value);

#endif // SUDOKU_H
