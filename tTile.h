/** @file
 * @brief En-tête des fonctions de manipulation des cases
 * @author 5cover, Matteo-K
 * @date 16/01/2024
 */

#ifndef T_TILE_H
#define T_TILE_H

#include "const.h"
#include "types.h"

// Utilisation de macros pour maximiser les performances de ces fonctions et procédures simples appelées très fréquemment dans le programme.

/// @brief Retourne le nombre de candidats d'une case.
/// @param tile E : la case
/// @return Le nombre de candidats de la case.
#define TILE_CANDIDATE_COUNT(tile) ((tile)._candidateCount)

/// @brief Détermine si une case a une valeur.
/// @param tile E : la case
/// @return Un booléen indiquant si la case a une valeur.
#define TILE_HAS_VALUE(tile) ((tile)._value != 0)

/// @brief Détermine si une case a un candidat.
/// @param tile E : la case
/// @param candidate E : le candidat à rechercher
/// @return Un booléen indiquant si la valeur est candidate dans la case.
#define TILE_HAS_CANDIDATE(tile, candidate) ((tile).hasCandidate[candidate])

/// @brief Obtient le premier candidat d'une case
/// @param tile E : la case
/// @param outVarName Nom de la variable à créer contenant le résultat
/// @return Le premieère valeur [1 ; @ref SIZE] candidate de @p tile.
#define TILE_GET_FIRST_CANDIDATE(tile, outVarName)  \
    int outVarName = 1;                             \
    while (!TILE_HAS_CANDIDATE(tile, outVarName)) { \
        outVarName++;                               \
    }

/// @brief Retourne le n-ième candidat d'une case.
/// @param tile E : la case
/// @param n E : l'indice basé à 1 du candidat à retourner
/// @return Le @p n -ième candidat de @p tile, ou 0 si il n'existe pas.
int tile_candidateAt(tTile const *tile, int n);

#endif // T_TILE_H
