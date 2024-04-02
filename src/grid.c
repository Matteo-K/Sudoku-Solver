/** @file
 * @brief Sudoku grid functions implementation
 * @author 5cover, Matteo-K
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "const.h"
#include "grid.h"
#include "memdbg.h"
#include "utils.h"

tGrid grid_create(tIntN const N)
{
    return (tGrid){
        .N = N,
        .SIZE = N * N,
        .cells = NULL,
        ._isBlockFree = NULL,
        ._isColumnFree = NULL,
        ._isRowFree = NULL,
    };
}

int grid_load(FILE *inStream, tGrid *g)
{
    // As the .sud files only contain the grid values, we need a temporary integer grid to store them.
    uint32_t *gridValues = check_alloc(array2d_malloc(gridValues, g->SIZE, g->SIZE), "gridValues");

    if (fread(gridValues, sizeof *gridValues, g->SIZE * g->SIZE, inStream) != g->SIZE * g->SIZE) {
        free(gridValues);
        return ERROR_INVALID_DATA;
    }

    // Allocate and initialize all cells to 0 (candidates array is NULL, no value, 0 candidates)
    g->cells = check_alloc(array2d_calloc(g->cells, g->SIZE, g->SIZE), "grid cells array");

    // Allocate row, column and block arrays
    g->_isColumnFree = check_alloc(array2d_malloc(g->_isColumnFree, g->SIZE, g->SIZE + 1), "grid _isColumnFree array");
    g->_isRowFree = check_alloc(array2d_malloc(g->_isRowFree, g->SIZE, g->SIZE + 1), "grid _isRowFree array");
    g->_isBlockFree = check_alloc(array3d_malloc(g->_isBlockFree, g->N, g->N, g->SIZE + 1), "grid _isBlockFree array");

    // Initialize all rows, columns and blocks to free
    memset(g->_isRowFree, true, sizeof(bool) * g->SIZE * (g->SIZE + 1));
    memset(g->_isColumnFree, true, sizeof(bool) * g->SIZE * (g->SIZE + 1));
    memset(g->_isBlockFree, true, sizeof(bool) * g->N * g->N * (g->SIZE + 1));

    // Initialize cells and mark them as not free
    for (tIntSize r = 0; r < g->SIZE; r++) {
        for (tIntSize c = 0; c < g->SIZE; c++) {
            tIntSize value = gridValues[at2d(g->SIZE, r, c)];
            tCell *cell = &grid_cellAt(*g, r, c);

            cell->hasCandidate = check_alloc(array_calloc(cell->hasCandidate, g->SIZE + 1),
                "grid cell %d,%d hasCandidate array", r, c);

            if (value != 0) {
                cell->_value = value;
                grid_markValueFree(false, *g, r, c, value);
            }
        }
    }

    // Add candidates
    for (tIntSize r = 0; r < g->SIZE; r++) {
        for (tIntSize c = 0; c < g->SIZE; c++) {
            tCell *cell = &grid_cellAt(*g, r, c);
            // No need to compute the candidates of a cell that already has a value.
            if (!cell_hasValue(*cell)) {
                // compute the cell's candidates
                for (tIntSize candidate = 1; candidate <= g->SIZE; candidate++) {
                    bool possible = grid_possible(*g, r, c, candidate);
                    // add the candidate
                    cell->hasCandidate[candidate] = possible;
                    cell->_candidateCount += possible;
                }
            }
        }
    }

    free(gridValues);

    return 0;
}

void grid_free(tGrid *grid)
{
    for (tIntSize r = 0; r < grid->SIZE; r++) {
        for (tIntSize c = 0; c < grid->SIZE; c++) {
            free(grid_cellAt(*grid, r, c).hasCandidate);
        }
    }
    free(grid->cells);
    free(grid->_isBlockFree);
    free(grid->_isColumnFree);
    free(grid->_isRowFree);
}

bool grid_cell_removeCandidate(tGrid *grid, tIntSize row, tIntSize column, tIntSize candidate)
{
    tCell *cell = &grid_cellAt(*grid, row, column);

    assert(1 <= candidate && candidate <= grid->SIZE);

    // If the cell has only one candidate remaining and it's the one we want to remove, set it as the cell's value and remove it.
    if (cell_candidate_count(*cell) == 1) {
        cell_get_first_candidate(*cell, onlyCandidate);
        if (onlyCandidate == candidate) {
            cell->_value = onlyCandidate;
            cell->hasCandidate[candidate] = false;
            cell->_candidateCount = 0;
            grid_markValueFree(false, *grid, row, column, candidate);
            return true;
        }
    }

    // Otherwise proceed as usual
    bool possible = cell_hasCandidate(*cell, candidate);
    if (possible) {
        cell->hasCandidate[candidate] = false;
        cell->_candidateCount--;
    }

    return possible;
}

void grid_cell_provideValue(tGrid *grid, tIntSize row, tIntSize column, tIntSize value)
{
    assert(grid_possible(*grid, row, column, value));

    tCell *cell = &grid_cellAt(*grid, row, column);

    assert(1 <= value && value <= grid->SIZE);
    assert(!cell_hasValue(*cell));

    cell->_value = value;
    cell->_candidateCount = 0;
    memset(cell->hasCandidate, false, sizeof(bool) * grid->SIZE + 1);
    grid_markValueFree(false, *grid, row, column, value);
}

bool grid_removeCandidateFromRow(tGrid *grid, tIntSize row, tIntSize candidate)
{
    bool progress = false;
    for (tIntSize c = 0; c < grid->SIZE; c++) {
        progress |= grid_cell_removeCandidate(grid, row, c, candidate);
    }
    return progress;
}

bool grid_removeCandidateFromColumn(tGrid *grid, tIntSize column, tIntSize candidate)
{
    bool progress = false;
    for (tIntSize r = 0; r < grid->SIZE; r++) {
        progress |= grid_cell_removeCandidate(grid, r, column, candidate);
    }
    return progress;
}

bool grid_removeCandidateFromBlock(tGrid *grid, tIntSize row, tIntSize column, tIntSize candidate)
{
    bool progress = false;

    tIntSize const blockRow = grid_blockIndex(*grid, row);
    tIntSize const blockCol = grid_blockIndex(*grid, column);

    for (tIntSize r = blockRow; r < blockRow + grid->N; r++) {
        for (tIntSize c = blockCol; c < blockCol + grid->N; c++) {
            progress |= grid_cell_removeCandidate(grid, r, c, candidate);
        }
    }

    return progress;
}

void grid_write(tGrid const *grid, FILE *outStream)
{
    for (tIntSize r = 0; r < grid->SIZE; r++) {
        for (tIntSize c = 0; c < grid->SIZE; c++) {
            uint32_t value32 = grid_cellAt(*grid, r, c)._value;
            fwrite(&value32, sizeof(uint32_t), 1, outStream);
        }
    }
}

void grid_print(tGrid const *grid, FILE *outStream)
{
    // Print grid body
    int padding = digitCount(grid->SIZE, 10);

    for (tIntSize block = 0; block < grid->N; block++) {
        printBlockSeparationLine(grid, padding, outStream);

        // For eahc line in the block:
        for (tIntSize blockRow = 0; blockRow < grid->N; blockRow++) {
            // Find the actual line and print it
            grid_printRow(grid, block * grid->N + blockRow, padding, outStream);
        }
    }

    // Print the last separation line all the way down
    printBlockSeparationLine(grid, padding, outStream);
}

void printBlockSeparationLine(tGrid const *grid, int padding, FILE *outStream)
{
    putc(DISPLAY_INTERSECTION, outStream);

    for (tIntSize block = 0; block < grid->N; block++) {
        // Add 2 to account for horizontal value margin (1 space left and right)
        printMultipleTimes(DISPLAY_HORIZONTAL_LINE, grid->N * (padding + 2), outStream);
        putc(DISPLAY_INTERSECTION, outStream);
    }

    putc('\n', outStream);
}

void grid_printRow(tGrid const *grid, tIntSize row, int padding, FILE *outStream)
{
    putc(DISPLAY_VERTICAL_LINE, outStream);

    // Print line content
    for (tIntSize block = 0; block < grid->N; block++) {
        for (tIntSize blockCol = 0; blockCol < grid->N; blockCol++) {
            printValue(grid_cellAt(*grid, row, block * grid->N + blockCol)._value, padding, outStream);
        }
        putc(DISPLAY_VERTICAL_LINE, outStream);
    }

    putc('\n', outStream);
}

void printValue(tIntSize value, int padding, FILE *outStream)
{
    putc(DISPLAY_SPACE, outStream);

    if (value == 0) {
        fprintf(outStream, "%*c", padding, DISPLAY_EMPTY_VALUE);
    } else {
        fprintf(outStream, "%*d", padding, value);
    }

    putc(DISPLAY_SPACE, outStream);
}

void printMultipleTimes(char character, unsigned times, FILE *outStream)
{
    for (unsigned time = 0; time < times; time++) {
        putc(character, outStream);
    }
}
