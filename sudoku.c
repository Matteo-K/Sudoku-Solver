/** @file
 * @brief Implémentation des fonctions issues du jeu de Sudoku
 * @author 5cover, Matteo-K
 * @date 16/01/2024
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "const.h"
#include "sudoku.h"

int grid_load(char const *filename, tGrid *grid)
{
    int result;

    // Comme les fichiers de grilles contiennent uniquement les valeurs entières, on ne peut pas lire directement dans grille.
    // Il faut passer par un tableau contenant les valeurs uniquement.
    tIntegerGrid valuesGrid;

    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        result = ERROR_FOPEN_FAILED;
    } else {
        if (fread(valuesGrid, sizeof(int), SIZE * SIZE, file) == SIZE * SIZE) {
            grid_initialize(grid, valuesGrid);
            result = 0;
        } else {
            result = ERROR_INVALID_DATA;
        }

        fclose(file);
    }

    return result;
}

void grid_initialize(tGrid *grid, tIntegerGrid valuesGrid)
{
    // Initialiser toutes les cases à 0. (aucun candidat, aucune valeur)
    memset(grid->tiles, 0, sizeof grid->tiles);

    // Initialiser toutes les valeurs comme libres sur toute la grille
    memset(grid->_isColumnFree, true, sizeof(grid->_isColumnFree) + sizeof(grid->_isRowFree) + sizeof(grid->_isBlockFree));

    // Initialiser les valeurs et les marquer comme impossibles
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            int value = valuesGrid[r][c];
            if (value != 0) {
                grid->tiles[r][c]._value = value;
                GRID_MARK_VALUE_FREE(grid, r, c, value, false);
            }
        }
    }

    // Ajouter les candidats
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            tTile *tile = &grid->tiles[r][c];
            // inutile de déterminer les candidats d'une case dont la valeur existe déjà.
            if (!TILE_HAS_VALUE(*tile)) {
                // calculer les candidats
                for (int candidate = 1; candidate <= SIZE; candidate++) {
                    bool possible = POSSIBLE(grid, r, c, candidate);
                    // Ajouter le candidat à la structure
                    tile->hasCandidate[candidate] = possible;
                    tile->_candidateCount += possible;
                }
            }
        }
    }
}

bool grid_tile_removeCandidate(tGrid *grid, int row, int column, int candidate)
{
    tTile *tile = &grid->tiles[row][column];

    assert(1 <= candidate && candidate <= SIZE);

    // Définir le candidat comme la valeur de la case avant de le retirer si c'est le dernier.
    if (TILE_CANDIDATE_COUNT(*tile) == 1) {
        TILE_GET_FIRST_CANDIDATE(*tile, lastCandidate);
        if (lastCandidate == candidate) {
            tile->_value = lastCandidate;
            GRID_MARK_VALUE_FREE(grid, row, column, candidate, false);
        }
    }

    bool possible = TILE_CANDIDATE_COUNT(*tile) > 0 && TILE_HAS_CANDIDATE(*tile, candidate);
    // Retire le candidat de la structure si il est présent.
    if (possible) {
        tile->hasCandidate[candidate] = false;
        tile->_candidateCount--;
    }

    return possible;
}

void grid_tile_provideValue(tGrid *grid, int row, int column, int value)
{
    assert(POSSIBLE(grid, row, column, value));

    tTile *tile = &grid->tiles[row][column];

    assert(1 <= value && value <= SIZE);
    assert(!TILE_HAS_VALUE(*tile));

    tile->_value = value;
    GRID_MARK_VALUE_FREE(grid, row, column, value, false);

    tile->_candidateCount = 0;
    memset(tile->hasCandidate, false, sizeof(tile->hasCandidate));
}

/* Procédures d'affichage */

void grid_print(tGrid const *grid)
{
    // Affichage du corps de la grille
    for (int block = 0; block < N; block++) {
        printBlockSeparationLine();

        // Pour chaque ligne du bloc
        for (int blockRow = 0; blockRow < N; blockRow++) {
            // Faire un calcul pour obtenir le véritable numéro de ligne, puis afficher la ligne.
            grid_printRow(grid, block * N + blockRow);
        }
    }

    // Afficher la dernière ligne de séparation tout en bas
    printBlockSeparationLine();
}

void printBlockSeparationLine(void)
{
    putchar(DISPLAY_INTERSECTION);

    for (int block = 0; block < N; block++) {
        // Ajouter 2 pour prendre en compte la marge horizontale des valeurs (1 espace à gauche et à droite).
        printMultipleTimes(DISPLAY_HORIZONTAL_LINE, N * (DISPLAY_PADDING + 2));
        putchar(DISPLAY_INTERSECTION);
    }

    putchar('\n');
}

void grid_printRow(tGrid const *grid, int row)
{
    putchar(DISPLAY_VERTICAL_LINE);

    // Affichage du contenu de la ligne
    for (int block = 0; block < N; block++) {
        for (int blockCol = 0; blockCol < N; blockCol++) {
            printValue(grid->tiles[row][block * N + blockCol]._value);
        }
        putchar(DISPLAY_VERTICAL_LINE);
    }

    putchar('\n');
}

void printValue(int value)
{
    putchar(DISPLAY_SPACE);

    if (value == 0) {
        printf("%*c", DISPLAY_PADDING, DISPLAY_EMPTY_VALUE);
    } else {
        printf("%*d", DISPLAY_PADDING, value);
    }

    putchar(DISPLAY_SPACE);
}

void printMultipleTimes(char character, int times)
{
    for (int time = 0; time < times; time++) {
        putchar(character);
    }
}
