/** @file
 * @brief En-tête des fonctions de résolution
 * @author 5cover, Matteo-K
 * @date 16/01/2024
 */

#ifndef RESOLUTION_H
#define RESOLUTION_H

#include <stdbool.h>

#include "types.h"

/// @brief Exécute les techniques simples.
/// @param grid E/S : la grille
/// @return Si un progrès a été fait.
bool perform_simpleTechniques(tGrid *grid);

/// Effectue la technique de backtracking.
/// @param grid E/S : la grille
/// @param emptyTilePositions E/S : les positions des cases vides
/// @param emptyTileCount E : le nombre de cases vides (longueur de @p emptyTilePositions)
/// @param iTilePositions E : indice de la position par laquelle commencer
/// @return Si la technique a effectué un progrès.
/// @remark Cette technique doit être exécutée en dernier.
/// @remark Après l'appel de cette fonction, il est possible que les candidats de la grille aient un état incohérent. Ce choix a été fait car cela offre un gain en performances et on a plus de besoin des candidats une fois la grille résolue.
bool technique_backtracking(tGrid *grid, tPositionArray emptyTilePositions, int emptyTileCount, int iTilePosition);

/// @brief Échange la case à @p iHere avec la case ayant le moins de valeurs possibles dans @p emptyTilePositions.
/// @param grid E : la grille
/// @param emptyTilePositions E/S : les positions des cases vides
/// @param emptyTileCount E : le nombre de cases vides (longueur de @p emptyTilePositions)
/// @param iHere E : l'indice de la case à échanger
/// @remark Utilisée pour la technique de backtracking.
void technique_backtracking_swap_tiles(tGrid const *grid, tPositionArray emptyTilePositions, int emptyTileCount, int iHere);

/// @brief Effectue la technique du singleton nu.
/// @param grid E/S : la grille
/// @param row E : la ligne de la case visée
/// @param column E : la colonne de la case visée
/// @return Si la technique a effectué un progrès.
bool technique_nakedSingleton(tGrid *grid, int row, int column);

/// @brief Effectue la technique du singleton caché.
/// @param grid E/S : la grille
/// @param row E : la ligne de la case visée
/// @param column E : la colonne de la case visée
/// @return Si la technique a effectué un progrès.
bool technique_hiddenSingleton(tGrid *grid, int row, int column);

/// @brief Trouve le candidat unique d'un groupe.
/// @param grid E : la grille
/// @param rStart E : ligne de début de recherche
/// @param rEnd E : ligne de fin de recherche (excluse)
/// @param cStart E : colonne de début de recherche
/// @param cEnd E : colonne de fin de recherche (excluse)
/// @param candidatePosition S : position du candidat unique trouvé
/// @return Le candidat unique, ou 0 si il n'y en a pas.
/// @remark Utilisée pour la technique du singleton caché.
int technique_hiddenSingleton_findUniqueCandidate(
    tGrid const *grid,
    int rStart, int rEnd,
    int cStart, int cEnd,
    tPosition *candidatePosition);

/// @brief Effectue la technique de la paire nue.
/// @param grid E/S : la grille
/// @param row E : la ligne de la case visée
/// @param column E : la colonne de la case visée
/// @return Si la technique a effectué un progrès.
bool technique_nakedPair(tGrid *grid, int row, int column);

/// @brief Vérifie si une case est une paire nue (elle contient uniquement les candidats de @p pair).
/// @param grid E : la grille
/// @param tile E : la case à vérifier
/// @return Si la case est une paire nue.
/// @remark Utilisée pour la technique de la paire nue.
#define TECHNIQUE_NAKED_PAIR_IS_PAIR_TILE(tile, pair) \
    (TILE_CANDIDATE_COUNT(tile) == 2                  \
     && TILE_HAS_CANDIDATE(tile, pair.candidates[0])  \
     && TILE_HAS_CANDIDATE(tile, pair.candidates[1]))

/// @brief Effectue la technique de la paire cachée.
/// @param grid E/S : la grille
/// @param row E : la ligne de la case visée
/// @param column E : la colonne de la case visée
/// @return Si la technique a effectué un progrès.
bool technique_hiddenPair(tGrid *grid, int row, int column);

/// @brief Trouve une paire exactement deux fois dans un groupe.
/// @param grid E : la grille
/// @param rStart E : ligne de début de recherche
/// @param rEnd E : ligne de fin de recherche (excluse)
/// @param cStart E : colonne de début de recherche
/// @param cEnd E : colonne de fin de recherche (excluse)
/// @param pairTilesPositions E/S : positions des cases contenant la paire trouvées. Le premier élément contient la position de la première case contenant la paire.
/// @param candidates S : candidats de la paire
/// @return Si une paire a été trouvée.
/// @remark Utilisée pour la technique du singleton caché.
bool technique_hiddenPair_findPair(
    tGrid const *grid,
    int rStart, int rEnd,
    int cStart, int cEnd,
    tPosition pairTilesPositions[PAIR_SIZE], int candidates[PAIR_SIZE]);

/// @brief Supprime les cases contenant une paire de candidats
/// @param grid E/S : la grille
/// @param pairTilesPositions E : les positions des cases contenant la paire
/// @param candidates E : les candidats de la paire
bool technique_hiddenPair_removePairTiles(tGrid *grid,
                                          tPosition const pairTilesPositions[PAIR_SIZE], int const candidates[PAIR_SIZE]);

bool technique_hiddenPair_findPairTiles(
    tGrid const *grid, int const candidates[PAIR_SIZE],
    int rStart, int rEnd,
    int cStart, int cEnd,
    tPosition pairTilesPositions[PAIR_SIZE]);

/// @brief Effectue la technique X-Wing.
/// @param grid E/S : la grille
/// @return Si la technique a effectué un progrès.
bool technique_x_wing(tGrid *grid);

/// @brief Retire un candidat de toutes les cases d'une ligne.
/// @param grid E/S : la grille où effectuer les retraits
/// @param row E : la ligne
/// @param candidate E : la valeur candidate à retirer
/// @return Si un progrès a été fait.
bool grid_removeCandidateFromRow(tGrid *grid, int row, int candidate);

/// @brief Retire un candidat de toutes les cases d'une colonne.
/// @param grid E/S : la grille où effectuer les retraits
/// @param column E : la colonne
/// @param candidate E : la valeur candidate à retirer
/// @return Si un progrès a été fait.
bool grid_removeCandidateFromColumn(tGrid *grid, int column, int candidate);

/// @brief Retire un candidat de toutes les cases d'un bloc.
/// @param grid E/S : la grille où effectuer les retraits
/// @param row E : la ligne d'une case du bloc
/// @param column E : la colonne d'une case du bloc
/// @param candidate E : la valeur candidate à retirer
/// @return Si un progrès a été fait.
bool grid_removeCandidateFromBlock(tGrid *grid, int row, int column, int candidate);

#endif // RESOLUTION_H
