/** @file
 * @brief En-tête des fonctions issues du jeu de Sudoku
 * @author 5cover, Matteo-K
 * @date 16/01/2024
 */

#ifndef SUDOKU_H
#define SUDOKU_H

#include "tTile.h"

// Utilisation de macros pour maximiser les performances de ces fonctions et procédures simples appelées très fréquemment dans le programme.

/// @brief Définit si une valeur est libre ou non à un emplacement sur la grille
#define GRID_MARK_VALUE_FREE(grid, row, column, value, isFree) \
    grid->_isColumnFree[column][value] = (isFree);             \
    grid->_isRowFree[row][value] = (isFree);                   \
    grid->_isBlockFree[(row) / N][(column) / N][value] = (isFree)

/// @brief Obtient l'indice (un numéro de ligne ou de colonne) de début du bloc contenant l'indice donné.
/// @param index E : un indice de case
/// @return L'indice du bloc contenant l'indice de case donné.
#define BLOCK_INDEX(index) (index - (index % N))

/// @brief Vérifie qu'une valeur peut être ajoutée à la grille. Ne prend pas en compte les candidats.
/// @param grid E : @ref tGrid : la grille
/// @param row E : entier : numéro de ligne de la case visée
/// @param column E : entier : numéro de colonne de la case visée
/// @param value E : entier : la valeur à insérer dans la grille
/// @return booléen : @c true si la valeur peut être ajoutée à la grille à l'emplacement indiqué ; @c false sinon.
/// @remark Affiche un message d'erreur explicite si la valeur ne peut pas être ajoutée à la grille.
/// @remark D'après les règles du Sudoku, une valeur ne peut être ajoutée à la grille que quand elle n'est pas déjà présente
/// dans la ligne, dans la colonne et dans le bloc.
// Après avoir mené des tests, il semblerait que l'ordre d'accès le plus rapide soit colonne, ligne, bloc.
#define POSSIBLE(grid, row, column, value) \
    (grid->_isColumnFree[column][value]    \
     && grid->_isRowFree[row][value]       \
     && grid->_isBlockFree[BLOCK_INDEX(row) / N][BLOCK_INDEX(column) / N][value])

/// @brief Compte le nombre de valeurs possibles d'une case.
/// @param grid E : la grille
/// @param row E : ligne de la case
/// @param column E : colonne de la case
/// @param outVarName Nom de la variable à créer contenant le résultat
/// @return Le nombre de valeurs pour lesquelles @ref POSSIBLE retourna @c true.
#define GRID_TILE_POSSIBLE_VALUES_COUNT(grid, row, column, outVarName) \
    int outVarName = 0;                                                \
    for (int val = 1; val <= SIZE; val++) {                            \
        outVarName += POSSIBLE(grid, row, column, val);                \
    }

/// @brief Initialise une grille à partir d'un fichier.
/// @param filename E : le nom du fichier *.sud à utiliser
/// @param grid S : la grille à initialiser
/// @return 0 si tout va bien, @ref ERROR_FOPEN_FAILED si la le fichier ne peut pas être ouvert, ou @ref ERROR_INVALID_DATA si le fichier contient des données invalides.
int grid_load(char const *filename, tGrid *grid);

/// @brief Initialize la grille : ajoute les candidats et marque les valeurs comme possibles ou non.
/// @param grid E/S : la grille
/// @param valuesGrid E : valeurs de la grille
void grid_initialize(tGrid *grid, tIntegerGrid valuesGrid);

// Les fonctions qui touchent à la valeur des cases ne sont pas définies dans tTile car elles ont besoin des coordonnées de la case sur la grille.
// Ces coordonnées sont pour que POSSIBLE ait une complexité de O(1), ce qui représente un gain substantiel en performances.

/// @brief Retire un candidat d'une case de la grille en le définissant comme valeur de la case si c'est le dernier.
/// @param grid E/S : la grille
/// @param row E : la ligne de la case
/// @param column E : la colonne de la case
/// @param candidate E : le candidat à retirer
/// @return Si le candidat a été retiré.
bool grid_tile_removeCandidate(tGrid *grid, int row, int column, int candidate);

/// @brief Définit la valeur d'une case et retire tous ses candidats.
/// @param grid E/S : la grille
/// @param row E : la ligne de la case
/// @param column E : la colonne de la case
/// @param candidate E : la valeur à définir
void grid_tile_provideValue(tGrid *grid, int row, int column, int value);

/* Procédures d'affichage */

/// @brief Affiche une grille et son contenu à l’écran.
/// @param grid E : @ref tGrid : la grille à afficher
void grid_print(tGrid const *grid);

/// @brief Affiche une ligne de séparation de blocs.
void printBlockSeparationLine(void);

/// @brief Affiche une ligne de la grille.
/// @param grid E : @ref tGrid : la grille
/// @param row E : entier : numéro de la ligne à afficher (entre 0 et @ref TAILLE - 1)
void grid_printRow(tGrid const *grid, int row);

/// @brief Affiche un même caractère un certain nombre de fois.
/// @param character E : caractère : le caractère à afficher
/// @param times E : entier : nombre de fois
void printMultipleTimes(char character, int times);

/// @brief Affiche une valeur de la grille sous la forme d'un nombre ou de @ref AFFICHAGE_VALEUR_VIDE pour représenter
/// @ref VALEUR_VIDE, avec un espace de marge de chaque côté.
/// @param value E : entier : la valeur à afficher
void printValue(int value);

#endif // SUDOKU_H
