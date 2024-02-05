/** @file
 * @brief Sudoku grid functions implementation
 * @author 5cover, Matteo-K
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "const.h"
#include "grid.h"

int grid_load(FILE *inStream, tGrid *grid)
{
    int result;

    // As the .sud files only contain the grid values, we need a temporary integer grid to store them.
    tIntegerGrid gridValues;

    if (fread(gridValues, sizeof(int), SIZE * SIZE, inStream) == SIZE * SIZE) {
        grid_initialize(grid, gridValues);
        result = 0;
    } else {
        result = ERROR_INVALID_DATA;
    }
    return result;
}

void grid_initialize(tGrid *grid, tIntegerGrid gridValues)
{
    // Initialize all cells to 0 (no value, no candidates)
    memset(grid->cells, 0, sizeof grid->cells);

    // Initialize all rows, columns and blocks to free
    memset(grid->_isColumnFree, true, sizeof(grid->_isColumnFree) + sizeof(grid->_isRowFree) + sizeof(grid->_isBlockFree));

    // Initialize values and mark them as not free
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            int value = gridValues[r][c];
            if (value != 0) {
                grid->cells[r][c]._value = value;
                GRID_MARK_VALUE_FREE(grid, r, c, value, false);
            }
        }
    }

    // Add candidates
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            tCell *cell = &grid->cells[r][c];
            // No need to compute the candidates of a cell that already has a value.
            if (!CELL_HAS_VALUE(*cell)) {
                // compute the cell's candidates
                for (int candidate = 1; candidate <= SIZE; candidate++) {
                    bool possible = POSSIBLE(grid, r, c, candidate);
                    // add the candidate to the structure
                    cell->hasCandidate[candidate] = possible;
                    cell->_candidateCount += possible;
                }
            }
        }
    }
}

bool grid_cell_removeCandidate(tGrid *grid, int row, int column, int candidate)
{
    tCell *cell = &grid->cells[row][column];

    assert(1 <= candidate && candidate <= SIZE);

    // Define the candidate as the cell's value before removing it if it's the last.
    if (CELL_CANDIDATE_COUNT(*cell) == 1) {
        CELL_GET_FIRST_CANDIDATE(*cell, lastCandidate);
        if (lastCandidate == candidate) {
            cell->_value = lastCandidate;
            GRID_MARK_VALUE_FREE(grid, row, column, candidate, false);
        }
    }

    bool possible = CELL_CANDIDATE_COUNT(*cell) > 0 && CELL_HAS_CANDIDATE(*cell, candidate);
    // Remove the candidate from the structure if it's present.
    if (possible) {
        cell->hasCandidate[candidate] = false;
        cell->_candidateCount--;
    }

    return possible;
}

void grid_cell_provideValue(tGrid *grid, int row, int column, int value)
{
    assert(POSSIBLE(grid, row, column, value));

    tCell *cell = &grid->cells[row][column];

    assert(1 <= value && value <= SIZE);
    assert(!CELL_HAS_VALUE(*cell));

    cell->_value = value;
    GRID_MARK_VALUE_FREE(grid, row, column, value, false);

    cell->_candidateCount = 0;
    memset(cell->hasCandidate, false, sizeof(cell->hasCandidate));
}
