/** @file
 * @brief Solver functions header
 * @author 5cover, Matteo-K
 */

#ifndef RESOLUTION_H
#define RESOLUTION_H

#include <stdbool.h>

#include "types.h"

/// @brief Performs the simple techniques on the grid.
/// @param grid in/out: the grid
/// @return Whether progress has been made.
bool perform_simpleTechniques(tGrid *grid);

/// @brief Performs the backtracking technique.
/// @param grid in/out: the grid
/// @param emptyCellPositions in/out: the empty cell positions
/// @param emptyCellCount in: the amount of empty cells (length of @p emptyCellPositions)
/// @param iEmptyCellPositions in: index of the empty cell to start the backtracking from
/// @return Whether progress has been made.
/// @remark This technique must be performed last, as it will always solve the grid completely.
/// @remark After calling this function, it is possible that the candidates of the grid have an inconsistent state. This choice was made because it offers a performance gain and we no longer need the candidates once the grid is solved.
bool technique_backtracking(tGrid *grid, tPositionArray emptyCellPositions, tCount emptyCellCount, tIndex iEmptyCellPosition);

/// @brief Swaps the cell at @p iHere with the cell after @p iHere having the least possible values in @p emptyCellPositions.
/// @param grid in: the grid
/// @param emptyCellPositions in/out: the empty cell positions
/// @param emptyCellCount in: the amount of empty cells (length of @p emptyCellPositions)
/// @param iHere in: the index of the cell to swap
/// @remark Used in the backtracking technique.
void technique_backtracking_swap_cells(tGrid const *grid, tPositionArray emptyCellPositions, tCount emptyCellCount, tIndex iHere);

/// @brief Performs the naked singleton technique.
/// @param grid in/out: the grid
/// @param row in: the row of the targeted cell
/// @param column in: the column of the targeted cell
/// @return Whether progress has been made.
bool technique_nakedSingleton(tGrid *grid, tIndex row, tIndex column);

/// @brief Performs the hidden singleton technique.
/// @param grid in/out: the grid
/// @param row in: the row of the targeted cell
/// @param column in: the column of the targeted cell
/// @return Whether progress has been made.
bool technique_hiddenSingleton(tGrid *grid, tIndex row, tIndex column);

/// @brief Finds the unique candidate in a group.
/// @param grid in: the grid
/// @param rStart in: search start row
/// @param rEnd in: search end row (excluded)
/// @param cStart in: search start column
/// @param cEnd in: search end column (excluded)
/// @param candidatePosition out: assigned to the position of the unique candidate found
/// @return The unique candidate found, or 0 if none was found.
/// @remark Used in the hidden singleton technique.
int technique_hiddenSingleton_findUniqueCandidate(
    tGrid const *grid,
    tIndex rStart, tIndex rEnd,
    tIndex cStart, tIndex cEnd,
    tPosition *candidatePosition);

/// @brief Performs the naked pair technique.
/// @param grid in/out: the grid
/// @param row in: the row of the targeted cell
/// @param column in: the column of the targeted cell
/// @return Whether progress has been made.
bool technique_nakedPair(tGrid *grid, tIndex row, tIndex column);

/// @brief Checks if a cell is a naked pair cell (it only contains the candidates of @p pair).
/// @param grid in: the grid
/// @param cell in: the cell to check
/// @return Whether the cell is a naked pair cell
/// @remark Used in the naked pair technique.
#define TECHNIQUE_NAKED_PAIR_IS_PAIR_CELL(cell, pair) \
    (CELL_CANDIDATE_COUNT(cell) == 2                  \
     && CELL_HAS_CANDIDATE(cell, pair.candidates[0])  \
     && CELL_HAS_CANDIDATE(cell, pair.candidates[1]))

/// @brief Performs the hidden pair technique.
/// @param grid in/out: the grid
/// @param row in: the row of the targeted cell
/// @param column in: the column of the targeted cell
/// @return Whether progress has been made.
bool technique_hiddenPair(tGrid *grid, tIndex row, tIndex column);

/// @brief Finds a pair present exactly twice in a group.
/// @param grid in: the grid
/// @param rStart in: search start row
/// @param rEnd in: search end row (excluded)
/// @param cStart in: search start column
/// @param cEnd in: search end column (excluded)
/// @param pairCellPositions in/out: positions of the cells containing the found pair. The first element must contain the position of the first cell containing the pair.
/// @param candidates out: filled with the pair's candididates
/// @return Whether a pair has been found.
/// @remark Used in the hidden singleton technique.
bool technique_hiddenPair_findPair(
    tGrid const *grid,
    tIndex rStart, tIndex rEnd,
    tIndex cStart, tIndex cEnd,
    tPosition pairCellPositions[PAIR_SIZE], tValue candidates[PAIR_SIZE]);

/// @brief Removes the cells containing a pair of candidates.
/// @param grid in/out: the grid
/// @param pairCellPositions in: positions of the pair cells
/// @param candidates in: the pair's candidates
/// @return Whether progress has been made.
bool technique_hiddenPair_removePairCells(tGrid *grid,
                                          tPosition const pairCellPositions[PAIR_SIZE], tValue const candidates[PAIR_SIZE]);

/// @brief Finds the cells containing a pair of candidates.
/// @param grid in: the grid
/// @param candidates in: the pair's candidates
/// @param rStart in: search start row
/// @param rEnd in: search end row (excluded)
/// @param cStart in: search start column
/// @param cEnd in: search end column (excluded)
/// @param pairCellPositions out: assigned to the positions of the cells containing the pair
/// @return Whether a pair has been found.
bool technique_hiddenPair_findPairCells(
    tGrid const *grid, tValue const candidates[PAIR_SIZE],
    tIndex rStart, tIndex rEnd,
    tIndex cStart, tIndex cEnd,
    tPosition pairCellPositions[PAIR_SIZE]);

/// @brief Performs the X-Wing technique
/// @param grid in/out: the grid
/// @return Whether progress has been made.
bool technique_x_wing(tGrid *grid);

/// @brief Removes a candidate from all cells of a row.
/// @param grid in/out: the grid
/// @param row in: the row
/// @param candidate in: the candidate to remove
/// @return Whether progress has been made.
bool grid_removeCandidateFromRow(tGrid *grid, tIndex row, tValue candidate);

/// @brief Removes a candidate from all cells of a column.
/// @param grid in/out: the grid
/// @param column in: the column
/// @param candidate in: the candidate to remove
/// @return Whether progress has been made.
bool grid_removeCandidateFromColumn(tGrid *grid, tIndex column, tValue candidate);

/// @brief Removes a candidate from all cells of a block.
/// @param grid in/out: the grid
/// @param row in: the row of a cell in the block
/// @param column in: the column of a cell in the block
/// @param candidate in: the candidate to remove
/// @return Whether progress has been made.
bool grid_removeCandidateFromBlock(tGrid *grid, tIndex row, tIndex column, tValue candidate);

#endif // RESOLUTION_H
