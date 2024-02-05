/** @file
 * @brief En-tête des types
 * @author 5cover, Matteo-K
 * @date 16/01/2024
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

#include "const.h"

/// @brief Case
typedef struct {
    /// @brief Valeur
    int _value;

    /// @brief Tableau de booléens représentant pour chaque candidats si il est présent ou non.
    bool hasCandidate[SIZE + 1];

    /// @brief Nombre de candidats
    int _candidateCount;
} tTile;

/// @brief Matrice carrée d'entiers de côté @ref SIZE représentant une grille de Sudoku
typedef int tIntegerGrid[SIZE][SIZE];

typedef struct {
    /// @brief Matrice carrée de cases de côté @ref SIZE représentant une grille de Sudoku
    tTile tiles[SIZE][SIZE];

    /// @brief Matrice de booléens représentant pour chaque colonne si la valeur est présente ou non.
    /// @remark Dimensions: [columnIndex][value]
    bool _isColumnFree[SIZE][SIZE + 1];

    /// @brief Matrice de booléens représentant pour chaque ligne si la valeur est présente ou non.
    /// @remark Dimensions: [rowIndex][value]
    bool _isRowFree[SIZE][SIZE + 1];

    /// @brief Tableau tridimensionnel de booléens représentant pour chaque bloc si la valeur est présente ou non.
    /// @remark Dimensions: [blockRowIndex][blockColumnIndex][value]
    bool _isBlockFree[N][N][SIZE + 1];
} tGrid;

/// @brief Une position sur la grille
typedef struct {
    int row;
    int column;
} tPosition;

/// @brief Un tableau d'un maximum de @ref SIZE * @ref SIZE positions sur la grille
typedef tPosition tPositionArray[SIZE * SIZE];

/// @brief Paire de 2 candidats identiques avec leurs positions.
typedef struct {
    int candidates[PAIR_SIZE];
    int count;
} tPair2;

/// @brief Tableau d'entiers représentant le nombre d'occurences pour chaque candidat.
/// @remark La première case du tableau, à l'indice 0 est inutilisée.
typedef int tCandidateCounts[SIZE + 1];

#endif // TYPES_H
