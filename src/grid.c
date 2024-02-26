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
    tInt32Grid gridValues;

    if (fread(gridValues, sizeof(uint32_t), SIZE * SIZE, inStream) == SIZE * SIZE) {
        grid_initialize(grid, gridValues);
        result = 0;
    } else {
        result = ERROR_INVALID_DATA;
    }
    return result;
}

void grid_initialize(tGrid *grid, tInt32Grid gridValues)
{
    // Initialize all cells to 0 (no value, no candidates)
    memset(grid->cells, 0, sizeof grid->cells);

    // Initialize all rows, columns and blocks to free
    memset(grid->_isColumnFree, true, sizeof(grid->_isColumnFree) + sizeof(grid->_isRowFree) + sizeof(grid->_isBlockFree));

    // Initialize values and mark them as not free
    for (tIndex r = 0; r < SIZE; r++) {
        for (tIndex c = 0; c < SIZE; c++) {
            tValue value = gridValues[r][c];
            if (value != 0) {
                grid->cells[r][c]._value = value;
                GRID_MARK_VALUE_FREE(grid, r, c, value, false);
            }
        }
    }

    // Add candidates
    for (tIndex r = 0; r < SIZE; r++) {
        for (tIndex c = 0; c < SIZE; c++) {
            tCell *cell = &grid->cells[r][c];
            // No need to compute the candidates of a cell that already has a value.
            if (!CELL_HAS_VALUE(*cell)) {
                // compute the cell's candidates
                for (tValue candidate = 1; candidate <= SIZE; candidate++) {
                    bool possible = POSSIBLE(grid, r, c, candidate);
                    // add the candidate to the structure
                    cell->hasCandidate[candidate] = possible;
                    cell->_candidateCount += possible;
                }
            }
        }
    }
}

bool grid_cell_removeCandidate(tGrid *grid, tIndex row, tIndex column, tValue candidate)
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

void grid_cell_provideValue(tGrid *grid, tIndex row, tIndex column, tValue value)
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

void grid_write(tGrid const *grid, FILE *outStream)
{
    for (tIndex r = 0; r < SIZE; r++) {
        for (tIndex c = 0; c < SIZE; c++) {
            uint32_t value32 = grid->cells[r][c]._value;
            fwrite(&value32, sizeof(uint32_t), 1, outStream);
        }
    }
}


void grid_print(tGrid const *grid, FILE *outStream)
{
    // Print grid body
    for (tIndex block = 0; block < N; block++) {
        printBlockSeparationLine(outStream);

        // For eahc line in the block:
        for (tIndex blockRow = 0; blockRow < N; blockRow++) {
            // Find the actual line and print it
            grid_printRow(grid, block * N + blockRow, outStream);
        }
    }

    // Print the last separation line all the way down
    printBlockSeparationLine(outStream);
}

void printBlockSeparationLine(FILE *outStream)
{
    putc(DISPLAY_INTERSECTION, outStream);

    for (int block = 0; block < N; block++) {
        // Add 2 to account for horizontal value margin (1 space left and right)
        printMultipleTimes(DISPLAY_HORIZONTAL_LINE, N * (DISPLAY_PADDING + 2), outStream);
        putc(DISPLAY_INTERSECTION, outStream);
    }

    putc('\n', outStream);
}

void grid_printRow(tGrid const *grid, tIndex row, FILE *outStream)
{
    putc(DISPLAY_VERTICAL_LINE, outStream);

    // Print line content
    for (tIndex block = 0; block < N; block++) {
        for (tIndex blockCol = 0; blockCol < N; blockCol++) {
            printValue(grid->cells[row][block * N + blockCol]._value, outStream);
        }
        putc(DISPLAY_VERTICAL_LINE, outStream);
    }

    putc('\n', outStream);
}

void printValue(tValue value, FILE *outStream)
{
    putc(DISPLAY_SPACE, outStream);

    if (value == 0) {
        fprintf(outStream, "%*c", DISPLAY_PADDING, DISPLAY_EMPTY_VALUE);
    } else {
        fprintf(outStream, "%*d", DISPLAY_PADDING, value);
    }

    putc(DISPLAY_SPACE, outStream);
}

void printMultipleTimes(char character, unsigned times, FILE *outStream)
{
    for (unsigned time = 0; time < times; time++) {
        putc(character, outStream);
    }
}
