/** @file
 * @brief Version 2 de l'algorithme de solveur de Sudoku améliorer
 * @author 5cover, Matteo-K
 * @date 16/01/2024
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "resolution.h"
#include "sudoku.h"

int main()
{
    tGrid grid;
    char filename[BUFSIZ];
    printf("Nom du fichier : ");
    if (fgets(filename, BUFSIZ, stdin)) {
        filename[strlen(filename) - 1] = '\0';
    } else {
        filename[0] = '\0'; // Erreur de lecture? Chaîne vide.
    }

    switch (grid_load(filename, &grid)) {
    case ERROR_FOPEN_FAILED:
        fprintf(stderr, "Ouverture impossible : ");
        perror(filename);
        return EXIT_FAILURE;
    case ERROR_INVALID_DATA:
        fprintf(stderr, "Le fichier '%s' n'est pas une grille de Sudoku.\n", filename);
        return EXIT_FAILURE;
    }

    printf("\nGrille initiale\n");
    grid_print(&grid);

    // Partie chronométrée de l'algorithme
    clock_t cStart = clock();
    {
        bool progress; // si une progression a été faite par rapport à la dernière itération

        do {
            progress = perform_simpleTechniques(&grid);
        } while (progress);

        progress = technique_x_wing(&grid);
        while (progress) {
            // Alterner la technique X-Wing et les techniques simples.
            // La technique X-Wing pourrait permettre de réaliser d'autres progrès avec les techniques simples, et vice versa.
            // La boucle continue jusqu'à ce qu'aucun autre progrès ne puisse être réalisé.
            progress = technique_x_wing(&grid) || perform_simpleTechniques(&grid);
        }

        // Collecter les positions des cases vides restantes pour le backtracking
        tPositionArray emptyTilePositions;
        int emptyTileCount = 0;
        for (int r = 0; r < SIZE; r++) {
            for (int c = 0; c < SIZE; c++) {
                if (!TILE_HAS_VALUE(grid.tiles[r][c])) {
                    emptyTilePositions[emptyTileCount++] = (tPosition){.row = r, .column = c};
                }
            }
        }

        // Terminer avec le backtracking qui résoudra forcément la grille.
        technique_backtracking(&grid, emptyTilePositions, emptyTileCount, 0);
    }
    clock_t cEnd = clock();

    printf("\nGrille finale\n");
    grid_print(&grid);

    printf("\nResolution de la grille en %.6f sec\n",
           // On utilise le type double au lieu de float pour bénéficier d'une plus grande précision.
           (cEnd - cStart) / (double)CLOCKS_PER_SEC);

    return EXIT_SUCCESS;
}
