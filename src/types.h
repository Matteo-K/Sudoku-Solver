/** @file
 * @brief Types header
 * @author 5cover, Matteo-K
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdint.h>

#include "const.h"

/// @brief Type of a grid value.
/// @remark Range : [0; @ref MAX_N]
typedef uint_least8_t tValue;
/// @brief Type of a grid index.
/// @remark Range : [0; @ref MAX_SIZE]
typedef uint_least16_t tIndex;

/// @brief Maximum value of the grid size factor.
#define MAX_N UINT_LEAST8_MAX
/// @brief Maximum number of tiles in the grid. Equivalent to @ref MAX_N²
#define MAX_SIZE UINT_LEAST16_MAX

/// @brief A cell of a Sudoku grid
typedef struct {
    /// @brief Value of the cell.
    /// @remark In range [1 ; @ref SIZE]
    tValue _value;

    /// @brief Boolean array representing for each candidate whether it is present or not.
    /// @remark The first case of the array, at index 0 is unused. This is to allow direct indexation with the candidate value.
    bool hasCandidate[SIZE + 1];

    /// @brief Number of candidates.
    /// @remark In range [0 ; @ref SIZE]
    int _candidateCount;
} tCell;

/// @brief Square matrix of 32-bit integers of side @ref SIZE representing a Sud file.
typedef uint32_t tInt32Grid[SIZE][SIZE];

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
    tIndex row;
    /// @brief Column index.
    /// @remark In range [0 ; @ref SIZE - 1]
    tIndex column;
} tPosition;

/// @brief An array of a maximum of @ref SIZE * @ref SIZE positions on the grid
typedef tPosition tPositionArray[SIZE * SIZE];

/// @brief Pair of 2 identical candidates with their positions.
typedef struct {
    /// @brief Candidates
    tValue candidates[PAIR_SIZE];
    /// @brief Count of candidates
    /// @remark In range [0 ; @ref PAIR_SIZE]
    tIndex count;
} tPair2;

/// @brief Integer array representing the number of occurences for each candidate.
/// @remark The first case of the array, at index 0 is unused. This is to allow direct indexation with the candidate value.
typedef tIndex tCandidateCounts[SIZE + 1];

#endif // TYPES_H
