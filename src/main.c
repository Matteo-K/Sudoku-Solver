/** @file
 * @brief Sudoku Solver main
 * @author 5cover, Matteo-K
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resolution.h"
#include "grid.h"

int main()
{
    tGrid grid;

    // Load the grid
    switch (grid_load(stdin, &grid)) {
    case ERROR_INVALID_DATA:
        fprintf(stderr, "The input is not a Sudoku grid of size N=%d.\n", N);
        return EXIT_FAILURE;
    }

    // Solve the grid
    {
        bool progress; // if progress has been made since the last iteration

        do {
            progress = perform_simpleTechniques(&grid);
        } while (progress);

        progress = technique_x_wing(&grid);
        while (progress) {
            // Alternate betweeen the X-Wing technique and simple techniques
            // The X-Wing technique could allow for more progress with simple techniques, and vice versa.
            // The loop continues until no further progress can be made.
            progress = technique_x_wing(&grid) || perform_simpleTechniques(&grid);
        }

        // Collect the positions of the remaining empty cells for backtracking
        tPositionArray emptyCellPositions;
        int emptyCellCount = 0;
        for (int r = 0; r < SIZE; r++) {
            for (int c = 0; c < SIZE; c++) {
                if (!CELL_HAS_VALUE(grid.cells[r][c])) {
                    emptyCellPositions[emptyCellCount++] = (tPosition){.row = r, .column = c};
                }
            }
        }

        // Wrap up with backtracking which will always solve the grid.
        technique_backtracking(&grid, emptyCellPositions, emptyCellCount, 0);
    }

    // Output the solved grid
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            if (fwrite(&grid.cells[r][c]._value, sizeof(int), 1, stdout) != 1) {
                fprintf(stderr, "Error writing the solved grid to output.\n");
                return EXIT_FAILURE;
            };
        }
    }

    return EXIT_SUCCESS;
}
