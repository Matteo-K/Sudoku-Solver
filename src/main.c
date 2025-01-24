/** @file
 * @brief Sudone main
 * @author 5cover, Matteo-K
 */

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "grid.h"
#include "memdbg.h"
#include "resolution.h"
#include "tCell.h"

static tGrid gs_grid; // Automatically zero-initialized

void perform_emergencyMemoryCleanup(void) {
    // It's always safe to call grid_free since the pointers inside tGrid and tCell are always either NULL or valid, thanks to static member auto initialization and grid_create.
    grid_free(&gs_grid);
}

static void print_help(void) {
    puts("Sudone - an optimized Sudoku solver");
    puts("The input grid is read from standard input and the result is printed to standard output.");
    puts("");
    puts("Usage: " PROGRAM_NAME " N");
    puts("");
    puts("N\tGrid size integer constant between 1 and 255");
    puts("");
    puts("Options:");
    puts("");
    puts("-s\t solve the grid");
    puts("-b\t binary (.sud) output");
    puts("--help\t print this help and exit");
    puts("");
    puts("This is public domain software. Compiled on " __DATE__ ".");
}

int main(int argc, char **argv) {
    bool opt_solve = false, opt_binary = false;

    // Parse command-line options
    {
        struct option longOptions[] = {
            (struct option) {
                .name = "help",
                .has_arg = 0,
                .flag = NULL,
                .val = 'h',
            },
            { 0 }
        };

        int opt;
        while ((opt = getopt_long(argc, argv, "sb", longOptions, NULL)) != -1) {
            switch (opt) {
            case 's':
                opt_solve = true;
                break;
            case 'b':
                opt_binary = true;
                break;
            case 'h':
                print_help();
                return EXIT_SUCCESS;
            case '?':
                return EXIT_INVALID_ARG;
            default:
                abort();
            }
        }
    }

    // parse n argument

    if (optind >= argc) {
        fprintf(stderr, PROGRAM_NAME ": N argument missing\n");
        return EXIT_INVALID_ARG;
    }

    int const N = atoi(argv[optind]);
    if (N <= 0 || N > MAX_N) {
        fprintf(stderr, PROGRAM_NAME ": the N argument must be an integer between 1 and %d\n", MAX_N);
        return EXIT_INVALID_ARG;
    }

    gs_grid = grid_create(N);

    // Load the grid
    if (grid_load(stdin, &gs_grid) == ERROR_INVALID_DATA) {
        fprintf(stderr, PROGRAM_NAME ": the input is not a Sudoku grid of size N=%d.\n", gs_grid.N);
        return EXIT_INVALID_DATA;
    }

    // Solve the grid
    if (opt_solve) {
        bool progress; // if progress has been made since the last iteration

        do {
            progress = perform_simpleTechniques(&gs_grid);
        } while (progress);

        progress = technique_x_wing(&gs_grid);
        while (progress) {
            // Alternate betweeen the X-Wing technique and simple techniques
            // The X-Wing technique could allow for more progress with simple techniques, and vice versa.
            // The loop continues until no further progress can be made.
            progress = technique_x_wing(&gs_grid) || perform_simpleTechniques(&gs_grid);
        }

        // Collect the positions of the remaining empty cells for backtracking
        tPosition *emptyCellPositions = check_alloc(array_malloc(emptyCellPositions, gs_grid.SIZE * gs_grid.SIZE), "empty cell positions");

        int emptyCellCount = 0;

        for (tIntSize r = 0; r < gs_grid.SIZE; r++) {
            for (tIntSize c = 0; c < gs_grid.SIZE; c++) {
                if (!cell_hasValue(grid_cellAt(gs_grid, r, c))) {
                    emptyCellPositions[emptyCellCount++] = (tPosition) { .row = r, .column = c };
                }
            }
        }

        // Wrap up with backtracking which will always solve the grid.
        technique_backtracking(&gs_grid, emptyCellPositions, emptyCellCount, 0);

        free(emptyCellPositions);
    }

    // Output the grid
    if (opt_binary) {
        grid_write(&gs_grid, stdout);
    } else {
        grid_print(&gs_grid, stdout);
    }

    grid_free(&gs_grid);

    return EXIT_SUCCESS;
}
