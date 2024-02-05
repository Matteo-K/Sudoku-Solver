/** @file
 * @brief Cell functions header
 * @author 5cover, Matteo-K
 */

#ifndef T_CELL_H
#define T_CELL_H

#include "const.h"
#include "types.h"

// Using macros to maximize the performance of these simple functions and procedures called very frequently in the program.

/// @brief Returns the number of candidates of a cell.
/// @param cell in: the cell
/// @return The number of candidates of @p cell.
#define CELL_CANDIDATE_COUNT(cell) ((cell)._candidateCount)

/// @brief Determines whether a cell has a value.
/// @param cell in: the cell
/// @return A boolean indicating whether @p cell has a value.
#define CELL_HAS_VALUE(cell) ((cell)._value != 0)

/// @brief Determines whether a cell has a specific value as a candidate.
/// @param cell in: the cell
/// @param candidate in: the candidate to check
/// @return A boolean indicating whether @p cell has @p candidate as a candidate.
#define CELL_HAS_CANDIDATE(cell, candidate) ((cell).hasCandidate[candidate])

/// @brief Gets the first candidate of a cell in the range [1 ; @ref SIZE].
/// @param cell in: the cell
/// @param outVarName Name of the variable to declare and assign the result to.
/// @return The first value in the range [1 ; @ref SIZE] that is a candidate of @p cell.
/// @remark This macro is equivalent to calling @c cell_candidateAt(cell,1) but offers better performance as it does not require the overhead of a function call.
#define CELL_GET_FIRST_CANDIDATE(cell, outVarName)  \
    int outVarName = 1;                             \
    while (!CELL_HAS_CANDIDATE(cell, outVarName)) { \
        outVarName++;                               \
    }

/// @brief Returns the nth candidate of a cell in the range [1 ; @ref SIZE].
/// @param cell in: the cell
/// @param n in: the one-based index of the candidate to get
/// @return The nth candidate of @p cell.
int cell_candidateAt(tCell const *cell, int n);

#endif // T_CELL_H
