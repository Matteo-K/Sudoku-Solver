/** @file
 * @brief Solver functions implementation
 * @author 5cover, Matteo-K
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resolution.h"
#include "grid.h"

bool perform_simpleTechniques(tGrid *grid)
{
    bool progress = false;
    tCell *cell;

    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            // Executing the techniques in order of increasing complexity.
            // As soon as the value of the cell is defined, we move on to the next one.

            // Save time by avoiding to recalculate the address of the cell each time.
            cell = &grid->cells[r][c];

            if (CELL_HAS_VALUE(*cell)) continue;

            progress |= technique_nakedSingleton(grid, r, c);
            if (CELL_HAS_VALUE(*cell)) continue;

            progress |= technique_hiddenSingleton(grid, r, c);
            if (CELL_HAS_VALUE(*cell)) continue;

            progress |= technique_nakedPair(grid, r, c);
            if (CELL_HAS_VALUE(*cell)) continue;

            progress |= technique_hiddenPair(grid, r, c);
        }
    }

    return progress;
}

bool technique_backtracking(tGrid *grid, tPositionArray emptyCellPositions, int emptyCellCount, int iCellPosition)
{
    // This technique does not use candidates but value presence arrays.
    // The reason is that synchronizing the candidates between recursive calls requires loops.
    // While for the value arrays it is a simple boolean that indicates whether a value is present in a group (row, block or column).

    // we have processed all the cells, the grid is solved
    if (iCellPosition == emptyCellCount) {
        return true;
    }

    assert(0 <= iCellPosition && iCellPosition < emptyCellCount);

    // Select the cell to solve
    technique_backtracking_swap_cells(grid, emptyCellPositions, emptyCellCount, iCellPosition);

    tPosition pos = emptyCellPositions[iCellPosition];

    for (int value = 1; value <= SIZE; value++) {
        if (POSSIBLE(grid, pos.row, pos.column, value)) {
            // assuming that the cell contains this value,
            GRID_MARK_VALUE_FREE(grid, pos.row, pos.column, value, false);

            // move on to the next cell: recursive call to see if the value is good afterwards
            if (technique_backtracking(grid, emptyCellPositions, emptyCellCount, iCellPosition + 1)) {
                // the value is good, put it and return.
                grid->cells[pos.row][pos.column]._value = value;
                return true;
            }

            // Solving the following cells assuming this value has failed, so we don't have the right value.
            GRID_MARK_VALUE_FREE(grid, pos.row, pos.column, value, true);
        }
    }

    // We failed for all values
    return false;
}

void technique_backtracking_swap_cells(tGrid const *grid, tPositionArray emptyCellPositions, int emptyCellCount, int iHere)
{
    assert(0 <= iHere && iHere < emptyCellCount);

    tPosition pos = emptyCellPositions[iHere];
    int iMin = iHere;
    GRID_CELL_POSSIBLE_VALUES_COUNT(grid, pos.row, pos.column, possibleValCountMin);

    // find the cell after which has the least possible values
    for (int i = iHere + 1; i < emptyCellCount; i++) {
        pos = emptyCellPositions[i];
        GRID_CELL_POSSIBLE_VALUES_COUNT(grid, pos.row, pos.column, possibleValCountI);

        if (possibleValCountI < possibleValCountMin) {
            iMin = i;
            possibleValCountMin = possibleValCountI;
        }
    }

    // swap the cells
    tPosition tmp = emptyCellPositions[iHere];
    emptyCellPositions[iHere] = emptyCellPositions[iMin];
    emptyCellPositions[iMin] = tmp;
}

bool technique_nakedSingleton(tGrid *grid, int row, int column)
{
    bool progress = false;

    tCell *cell = &grid->cells[row][column];
    if (CELL_CANDIDATE_COUNT(*cell) == 1) {
        CELL_GET_FIRST_CANDIDATE(*cell, candidate);
        // remove all corresponding candidates
        // there will be at least one removal, the unique candidate of the cell.
        progress |= grid_removeCandidateFromRow(grid, row, candidate);
        progress |= grid_removeCandidateFromColumn(grid, column, candidate);
        progress |= grid_removeCandidateFromBlock(grid, row, column, candidate);
    }

    return progress;
}

bool technique_hiddenSingleton(tGrid *grid, int row, int column)
{
    tPosition candidatePos;
    int candidate;
    bool progress = false;

    int const BLOCK_ROW = BLOCK_INDEX(row);
    int const BLOCK_COL = BLOCK_INDEX(column);

    // Block
    if ((candidate = technique_hiddenSingleton_findUniqueCandidate(
             grid,
             BLOCK_ROW, BLOCK_ROW + N,
             BLOCK_COL, BLOCK_COL + N,
             &candidatePos))
        != 0) {
        grid_cell_provideValue(grid, candidatePos.row, candidatePos.column, candidate);
        grid_removeCandidateFromRow(grid, candidatePos.row, candidate);
        grid_removeCandidateFromColumn(grid, candidatePos.column, candidate);
        progress = true;
    }
    // Row
    if ((candidate = technique_hiddenSingleton_findUniqueCandidate(
             grid,
             row, row + 1,
             0, SIZE,
             &candidatePos))
        != 0) {
        grid_cell_provideValue(grid, candidatePos.row, candidatePos.column, candidate);
        grid_removeCandidateFromBlock(grid, candidatePos.row, candidatePos.column, candidate);
        grid_removeCandidateFromColumn(grid, candidatePos.column, candidate);
        progress = true;
    }
    // Column
    if ((candidate = technique_hiddenSingleton_findUniqueCandidate(
             grid,
             0, SIZE,
             column, column + 1,
             &candidatePos))
        != 0) {
        grid_cell_provideValue(grid, candidatePos.row, candidatePos.column, candidate);
        grid_removeCandidateFromBlock(grid, candidatePos.row, candidatePos.column, candidate);
        grid_removeCandidateFromRow(grid, candidatePos.row, candidate);
        progress = true;
    }

    return progress;
}

int technique_hiddenSingleton_findUniqueCandidate(
    tGrid const *grid,
    int rStart, int rEnd,
    int cStart, int cEnd,
    tPosition *candidatePosition)
{
    tCandidateCounts candidateCounts = {0};

    for (int r = rStart; r < rEnd; r++) {
        for (int c = cStart; c < cEnd; c++) {
            tCell cell = grid->cells[r][c];
            for (int candidate = 1; candidate <= SIZE; candidate++) {
                candidateCounts[candidate] += CELL_HAS_CANDIDATE(cell, candidate);
            }
        }
    }

    // Search for the unique candidate
    int candidate = 1;
    while (candidate <= SIZE && candidateCounts[candidate] != 1) {
        candidate++;
    }

    // If it hasn't been found
    if (candidate == SIZE + 1) {
        return 0;
    }

    for (int r = rStart; r < rEnd; r++) {
        for (int c = cStart; c < cEnd; c++) {
            if (CELL_HAS_CANDIDATE(grid->cells[r][c], candidate)) {
                *candidatePosition = (tPosition){.row = r, .column = c};
                return candidate;
            }
        }
    }

    // Impossible : unique candidate not found even though it was found earlier
    abort();
}

bool technique_nakedPair(tGrid *grid, int row, int column)
{
    bool progress = false;

    tCell *cellRowColumn = &grid->cells[row][column];

    if (CELL_CANDIDATE_COUNT(*cellRowColumn) == 2) {
        int const BLOCK_ROW = BLOCK_INDEX(row);
        int const BLOCK_COL = BLOCK_INDEX(column);

        tPair2 pair = (tPair2){
            .candidates = {
                cell_candidateAt(cellRowColumn, 1),
                cell_candidateAt(cellRowColumn, 2)},
            .count = 1,
        };

        for (int r = BLOCK_ROW; r < BLOCK_ROW + N && pair.count < 2; r++) {
            for (int c = BLOCK_COL; c < BLOCK_COL + N && pair.count < 2; c++) {
                pair.count += (r != row || c != column) && TECHNIQUE_NAKED_PAIR_IS_PAIR_CELL(grid->cells[r][c], pair);
            }
        }

        if (pair.count == 2) {
            // Remove all candidates from the block except on the cells containing only the candidates of the pair
            // So we cannot use grid_removeCandidateFromBlock
            for (int r = BLOCK_ROW; r < BLOCK_ROW + N; r++) {
                for (int c = BLOCK_COL; c < BLOCK_COL + N; c++) {
                    bool isNotPairCell = !TECHNIQUE_NAKED_PAIR_IS_PAIR_CELL(grid->cells[r][c], pair);
                    progress |= isNotPairCell && grid_cell_removeCandidate(grid, r, c, pair.candidates[0]);
                    progress |= isNotPairCell && grid_cell_removeCandidate(grid, r, c, pair.candidates[1]);
                }
            }
        }
    }

    return progress;
}

bool technique_hiddenPair(tGrid *grid, int row, int column)
{
    tPosition pairCellPositions[PAIR_SIZE] = {(tPosition){.row = row, .column = column}};
    int candidates[PAIR_SIZE];
    bool progress = false;

    int const BLOCK_ROW = BLOCK_INDEX(row);
    int const BLOCK_COL = BLOCK_INDEX(column);

    tCell *cellRowColumn = &grid->cells[row][column];

    // Block
    progress |= (CELL_CANDIDATE_COUNT(*cellRowColumn) >= 2
                 && technique_hiddenPair_findPair(grid, BLOCK_ROW, BLOCK_ROW + N, BLOCK_COL, BLOCK_COL + N, pairCellPositions, candidates))
             && technique_hiddenPair_removePairCells(grid, pairCellPositions, candidates);
    // Row
    progress |= (CELL_CANDIDATE_COUNT(*cellRowColumn) >= 2
                 && technique_hiddenPair_findPair(grid, row, row + 1, 0, SIZE, pairCellPositions, candidates))
             && technique_hiddenPair_removePairCells(grid, pairCellPositions, candidates);
    // Column
    progress |= (CELL_CANDIDATE_COUNT(*cellRowColumn) >= 2
                 && technique_hiddenPair_findPair(grid, 0, SIZE, column, column + 1, pairCellPositions, candidates))
             && technique_hiddenPair_removePairCells(grid, pairCellPositions, candidates);

    return progress;
}

bool technique_hiddenPair_findPair(
    tGrid const *grid,
    int rStart, int rEnd,
    int cStart, int cEnd,
    tPosition pairCellPositions[PAIR_SIZE], int candidates[PAIR_SIZE])
{
    tCell firstPairCell = grid->cells[pairCellPositions[0].row][pairCellPositions[0].column];

    assert(CELL_CANDIDATE_COUNT(firstPairCell) >= 2);

    for (candidates[0] = 1; candidates[0] <= SIZE; candidates[0]++) {
        if (!CELL_HAS_CANDIDATE(firstPairCell, candidates[0])) {
            continue;
        }

        for (candidates[1] = candidates[0] + 1; candidates[1] <= SIZE; candidates[1]++) {
            if (!CELL_HAS_CANDIDATE(firstPairCell, candidates[1])) {
                continue;
            }

            // Start the search for a pair with new candidates.
            if (technique_hiddenPair_findPairCells(
                    grid, candidates,
                    rStart, rEnd,
                    cStart, cEnd,
                    pairCellPositions)) {
                return true;
            }
        }
    }

    return false;
}

bool technique_hiddenPair_findPairCells(
    tGrid const *grid, int const candidates[PAIR_SIZE],
    int rStart, int rEnd,
    int cStart, int cEnd,
    tPosition pairCellPositions[PAIR_SIZE])
{
    int nbPairCells = 1; // Count of cells found containing the pair (candidate1, candidate2).
    int nbPairCellsContainingOtherCandidates = 0;

    // Searching for hidden pairs
    // Add each cell that contain both candidates
    for (int r = rStart; r < rEnd && nbPairCells <= PAIR_SIZE + 1; r++) {
        for (int c = cStart; c < cEnd && nbPairCells <= PAIR_SIZE + 1; c++) {
            if (r == pairCellPositions[0].row && c == pairCellPositions[0].column) {
                continue;
            }

            tCell cell = grid->cells[r][c];

            if (CELL_HAS_CANDIDATE(cell, candidates[0])
                && CELL_HAS_CANDIDATE(cell, candidates[1])) {
                nbPairCellsContainingOtherCandidates += CELL_CANDIDATE_COUNT(cell) > PAIR_SIZE;
                if (nbPairCells < PAIR_SIZE) {
                    pairCellPositions[nbPairCells] = (tPosition){.row = r, .column = c};
                }
                nbPairCells++;
            }
            // search for single candidates that would invalidate the pair
            else if (CELL_HAS_CANDIDATE(cell, candidates[0])
                     || CELL_HAS_CANDIDATE(cell, candidates[1])) {
                return false;
            }
        }
    }

    // For the hidden pair to be useful, at least one cell of the pair must contain other candidates.
    // Otherwise, we won't be able to remove any candidates.
    return nbPairCells == PAIR_SIZE && nbPairCellsContainingOtherCandidates > 0;
}

bool technique_hiddenPair_removePairCells(
    tGrid *grid,
    tPosition const pairCellPositions[PAIR_SIZE],
    int const candidates[PAIR_SIZE])
{
    bool progress = false;
    // For each cell containing the pair:
    for (int iPos = 0; iPos < PAIR_SIZE; ++iPos) {
        tPosition pos = pairCellPositions[iPos];
        // remove all its candidates
        for (int candidate = 1; candidate <= SIZE; ++candidate) {
            // except those forming the pair
            progress |= candidate != candidates[0]
                     && candidate != candidates[1]
                     && grid_cell_removeCandidate(grid, pos.row, pos.column, candidate);
        }
    }
    return progress;
}

bool technique_x_wing(tGrid *grid)
{
    bool progress = false;

    // VERTICAL X-WING
    // identify 2 columns that each have, on the same rows, exactly two cells where a candidate appears.
    /* We get a rectangle with these four cells as vertices:
    _   _
    A---B
    |   |
    C---D
    ‾   ‾*/
    for (int colAC = 0; colAC < SIZE; colAC++) {
        for (int colBD = colAC + 1; colBD < SIZE; colBD++) {
            for (int candidate = 1; candidate <= SIZE; candidate++) {
                int rows[2];
                int candidateInBothCount = 0;
                int candidateCounts[2] = {0};
                for (int row = 0; row < SIZE; row++) {
                    bool colACHasCandidate = CELL_HAS_CANDIDATE(grid->cells[row][colAC], candidate);
                    bool colBDHasCandidate = CELL_HAS_CANDIDATE(grid->cells[row][colBD], candidate);
                    candidateCounts[0] += colACHasCandidate;
                    candidateCounts[1] += colBDHasCandidate;
                    if (colACHasCandidate && colBDHasCandidate) {
                        if (candidateInBothCount < 2) {
                            rows[candidateInBothCount] = row;
                        }
                        candidateInBothCount++;
                    }
                }
                if (candidateInBothCount == 2 && candidateCounts[0] == 2 && candidateCounts[1] == 2) {
                    // eliminate the candidate from the rows (AB) and (CD).
                    for (int c = 0; c < SIZE; c++) {
                        // do not remove the candidate from the cells that form the X.
                        progress |= c != colAC && c != colBD && grid_cell_removeCandidate(grid, rows[0], c, candidate);
                        progress |= c != colAC && c != colBD && grid_cell_removeCandidate(grid, rows[1], c, candidate);
                    }
                }
            }
        }
    }

    // HORIZONTAL X-WING
    // identify 2 rows that each have, on the same columns, exactly two cells where a candidate appears.
    /* We get a rectangle with these four cells as vertices:
    (A---B)
     |   |
    (C---D)*/
    for (int rowAB = 0; rowAB < SIZE; rowAB++) {
        for (int rowCD = rowAB + 1; rowCD < SIZE; rowCD++) {
            for (int candidate = 1; candidate <= SIZE; candidate++) {
                int columns[2];
                int candidateInBothCount = 0;
                int candidateCounts[2] = {0};
                for (int col = 0; col < SIZE; col++) {
                    bool rowABHasCandidate = CELL_HAS_CANDIDATE(grid->cells[rowAB][col], candidate);
                    bool rowCDHasCandidate = CELL_HAS_CANDIDATE(grid->cells[rowCD][col], candidate);
                    candidateCounts[0] += rowABHasCandidate;
                    candidateCounts[1] += rowCDHasCandidate;
                    if (rowABHasCandidate && rowCDHasCandidate) {
                        if (candidateInBothCount < 2) {
                            columns[candidateInBothCount] = col;
                        }
                        candidateInBothCount++;
                    }
                }
                if (candidateInBothCount == 2 && candidateCounts[0] == 2 && candidateCounts[1] == 2) {
                    // elmininate the candidate from the columns (AC) and (BD).
                    for (int r = 0; r < SIZE; r++) {
                        // do not remove the candidate from the cells that form the X.
                        progress |= r != rowAB && r != rowCD && grid_cell_removeCandidate(grid, r, columns[0], candidate);
                        progress |= r != rowAB && r != rowCD && grid_cell_removeCandidate(grid, r, columns[1], candidate);
                    }
                }
            }
        }
    }

    return progress;
}

bool grid_removeCandidateFromRow(tGrid *grid, int row, int candidate)
{
    bool progress = false;
    for (int c = 0; c < SIZE; c++) {
        progress |= grid_cell_removeCandidate(grid, row, c, candidate);
    }
    return progress;
}

bool grid_removeCandidateFromColumn(tGrid *grid, int column, int candidate)
{
    bool progress = false;
    for (int r = 0; r < SIZE; r++) {
        progress |= grid_cell_removeCandidate(grid, r, column, candidate);
    }
    return progress;
}

bool grid_removeCandidateFromBlock(tGrid *grid, int row, int column, int candidate)
{
    bool progress = false;

    int const BLOCK_ROW = BLOCK_INDEX(row);
    int const BLOCK_COL = BLOCK_INDEX(column);

    for (int r = BLOCK_ROW; r < BLOCK_ROW + N; r++) {
        for (int c = BLOCK_COL; c < BLOCK_COL + N; c++) {
            progress |= grid_cell_removeCandidate(grid, r, c, candidate);
        }
    }

    return progress;
}