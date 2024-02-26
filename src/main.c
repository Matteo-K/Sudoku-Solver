/** @file
 * @brief Sudoku Solver main
 * @author 5cover, Matteo-K
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "grid.h"
#include "resolution.h"

#if _POSIX_C_SOURCE < 2 && !_XOPEN_SOURCE
#error "POSIX.2 or X/Open 4.2 required"
#endif

int main(int argc, char **argv)
{
    bool opt_see = false, opt_human = false;

    // Parse command-line options
    int opt;
    while ((opt = getopt(argc, argv, "sh")) != -1) {
        switch (opt) {
        case 's':
            opt_see = true;
            break;
        case 'h':
            opt_human = true;
            break;
        case '?':
            return EXIT_INVALID_OPTION;
        default:
            abort();
        }
    }

    tGrid grid;

    // Load the grid
    switch (grid_load(stdin, &grid)) {
    case ERROR_INVALID_DATA:
        fprintf(stderr, "The input is not a Sudoku grid of size N=%d.\n", N);
        return EXIT_INVALID_DATA;
    }

    // Solve the grid
    if (!opt_see) {
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
        for (tIndex r = 0; r < SIZE; r++) {
            for (tIndex c = 0; c < SIZE; c++) {
                if (!CELL_HAS_VALUE(grid.cells[r][c])) {
                    emptyCellPositions[emptyCellCount++] = (tPosition){.row = r, .column = c};
                }
            }
        }

        // Wrap up with backtracking which will always solve the grid.
        technique_backtracking(&grid, emptyCellPositions, emptyCellCount, 0);
    }

    // Output the solved grid
    if (opt_human) {
        grid_print(&grid, stdout);
    } else {
        grid_write(&grid, stdout);
    }

    return EXIT_SUCCESS;
}
