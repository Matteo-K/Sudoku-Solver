/** @file
 * @brief Types header
 * @author 5cover, Matteo-K
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "const.h"

#define array_malloc(name, length) malloc(sizeof *(name) * (length))
#define array_calloc(name, length) calloc(sizeof *(name), (length))

#define array2d_malloc(name, rowCount, colCount) malloc(sizeof *(name) * (rowCount) * (colCount))
#define array2d_calloc(name, rowCount, colCount) calloc(sizeof *(name), (rowCount) * (colCount))
#define at2d(rowCount, row, col) ((rowCount) * (row) + (col))

#define array3d_malloc(name, xSize, ySize, zSize) malloc(sizeof *(name) * (xSize) * (ySize) * (zSize))
#define array3d_calloc(name, xSize, ySize, zSize) calloc(sizeof *(name), (xSize) * (ySize) * (zSize))
#define at3d(xSize, ySize, x, y, z) ((xSize) * (x) + (ySize) * (y) + (z))

/// @brief Type for a N-majored integer.
/// @remark Range : [0; @ref MAX_N]
/// @remark May be used for grid values.
typedef uint_least8_t tIntN;

/// @brief Type for a SIZE-majored integer.
/// @remark Range : [0; @ref MAX_SIZE]
/// @remark May be used for grid axis indexes, cell indexes, block indexes, value/candodate counts.
typedef uint_least16_t tIntSize;

/// @brief Maximum value of the grid size factor.
#define MAX_N UINT_LEAST8_MAX
/// @brief Maximum number of tiles in the grid. Equivalent to @ref MAX_N².
#define MAX_SIZE UINT_LEAST16_MAX

/// @brief A cell of a Sudoku grid
typedef struct {
    /// @brief Value of the cell.
    /// @remark In range [1 ; SIZE]
    tIntSize _value;

    /// @brief Boolean dynamic array of length SIZE + 1 representing for each candidate whether it is present or not.
    /// @remark The first ekement of the array at index 0 is unused. This is to allow direct indexation with the candidate value.
    bool *hasCandidate;

    /// @brief Number of candidates.
    /// @remark In range [0 ; SIZE]
    tIntSize _candidateCount;
} tCell;

/// @brief A Sudoku grid
typedef struct {
    /// @brief Square dynamic matrix of cells of side SIZE representing a Sudoku grid
    /// @remark Dimensions: [rowIndex][columnIndex]
    tCell *cells;

    /// @brief Grid side.
    tIntSize const SIZE;

    /// @brief Grid size factor.
    tIntN const N;

    /// @brief Boolean dynamic matrix representing for each column whether the value is present or not.
    /// @remark Dimensions: [columnIndex][value]
    bool *_isColumnFree;

    /// @brief Boolean dynamic matrix representing for each row whether the value is present or not.
    /// @remark Dimensions: [rowIndex][value]
    bool *_isRowFree;

    /// @brief Boolean dynamic 3D array representing for each block whether the value is present or not.
    /// @remark Dimensions: [blockRowIndex][blockColumnIndex][value]
    bool *_isBlockFree;
} tGrid;

/// @brief A position on the grid
typedef struct {
    /// @brief Row index.
    /// @remark In range [0 ; SIZE[
    tIntSize row;
    /// @brief Column index.
    /// @remark In range [0 ; SIZE[
    tIntSize column;
} tPosition;

/// @brief Pair of 2 identical candidates with their positions.
typedef struct {
    /// @brief Candidates.
    tIntN candidates[PAIR_SIZE];
    /// @brief Candidate count.
    /// @remark In range [0 ; @ref PAIR_SIZE]
    unsigned count;
} tPair2;

#endif // TYPES_H
