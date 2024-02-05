/** @file
 * @brief Types header
 * @author 5cover, Matteo-K
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

#include "const.h"

/// @brief A cell of a Sudoku grid
typedef struct {
    /// @brief Value of the cell.
    /// @remark In range [1 ; @ref SIZE]
    int _value;

    /// @brief Boolean array representing for each candidate whether it is present or not.
    /// @remark The first case of the array, at index 0 is unused. This is to allow direct indexation with the candidate value.
    bool hasCandidate[SIZE + 1];

    /// @brief Number of candidates.
    /// @remark In range [0 ; @ref SIZE]
    int _candidateCount;
} tCell;

/// @brief Square matrix of integers of side @ref SIZE representing a Sudoku grid's values
typedef int tIntegerGrid[SIZE][SIZE];

/// @brief A Sudoku grid
typedef struct {
    /// @brief Square matrix of cells of side @ref SIZE representing a Sudoku grid
    /// @remark Dimensions: [rowIndex][columnIndex]
    tCell cells[SIZE][SIZE];

    /// @brief Boolean matrix representing for each column whether the value is present or not.
    /// @remark Dimensions: [columnIndex][value]
    bool _isColumnFree[SIZE][SIZE + 1];

    /// @brief Boolean matrix representing for each row whether the value is present or not.
    /// @remark Dimensions: [rowIndex][value]
    bool _isRowFree[SIZE][SIZE + 1];

    /// @brief Boolean 3D array representing for each block whether the value is present or not.
    /// @remark Dimensions: [blockRowIndex][blockColumnIndex][value]
    bool _isBlockFree[N][N][SIZE + 1];
} tGrid;

/// @brief A position on the grid
typedef struct {
    /// @brief Row index.
    /// @remark In range [0 ; @ref SIZE - 1]
    int row;
    /// @brief Column index.
    /// @remark In range [0 ; @ref SIZE - 1]
    int column;
} tPosition;

/// @brief An array of a maximum of @ref SIZE * @ref SIZE positions on the grid
typedef tPosition tPositionArray[SIZE * SIZE];

/// @brief Pair of 2 identical candidates with their positions.
typedef struct {
    /// @brief Candidates
    int candidates[PAIR_SIZE];
    /// @brief Count of candidates
    /// @remark In range [0 ; @ref PAIR_SIZE]
    int count;
} tPair2;

/// @brief Integer array representing the number of occurences for each candidate.
/// @remark The first case of the array, at index 0 is unused. This is to allow direct indexation with the candidate value.
typedef int tCandidateCounts[SIZE + 1];

#endif // TYPES_H
