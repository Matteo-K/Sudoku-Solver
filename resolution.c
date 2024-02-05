/** @file
 * @brief Implémentation des fonctions de résolution
 * @author 5cover, Matteo-K
 * @date 16/01/2024
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resolution.h"
#include "sudoku.h"

bool perform_simpleTechniques(tGrid *grid)
{
    bool progress = false;
    tTile *tile;

    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            // Exécution des techniques dans l'ordre de complexité croissante.
            // Dès que la valeur de la case est définie, on passe à la suivante.

            // Permet de gagner du temps en évitant de recalculer l'adresse de la case à chaque fois.
            tile = &grid->tiles[r][c];

            if (TILE_HAS_VALUE(*tile)) continue;

            progress |= technique_nakedSingleton(grid, r, c);
            if (TILE_HAS_VALUE(*tile)) continue;

            progress |= technique_hiddenSingleton(grid, r, c);
            if (TILE_HAS_VALUE(*tile)) continue;

            progress |= technique_nakedPair(grid, r, c);
            if (TILE_HAS_VALUE(*tile)) continue;

            progress |= technique_hiddenPair(grid, r, c);
        }
    }

    return progress;
}

bool technique_backtracking(tGrid *grid, tPositionArray emptyTilePositions, int emptyTileCount, int iTilePosition)
{
    // Note : cette fonction, pouvant être appelée de nombreuses fois, doit être optimisée au maximum. Même un gain minimal peut avoir un impact substantiel.
    // On utilisera donc plusieurs return car cela offre un gain de performances ici.

    // Cette technique n'utilise pas les candidats mais les tableaux de présence de valeur.
    // La raison est que synchroniser les candidats entre les appels récursifs requiert des tours de boucles.
    // Alors que pour les tableaux de valeurs c'est un simple booléen qui indique si une valeur est présente dans un groupe (ligne bloc ou colonne)

    // on a traité toutes les cases, la grille est résolue
    if (iTilePosition == emptyTileCount) {
        return true;
    }

    assert(0 <= iTilePosition && iTilePosition < emptyTileCount);

    // On seléctionne la case à résoudre
    technique_backtracking_swap_tiles(grid, emptyTilePositions, emptyTileCount, iTilePosition);

    tPosition pos = emptyTilePositions[iTilePosition];

    for (int value = 1; value <= SIZE; value++) {
        if (POSSIBLE(grid, pos.row, pos.column, value)) {
            // assumant que la case contient cette valeur,
            GRID_MARK_VALUE_FREE(grid, pos.row, pos.column, value, false);

            // passer à la case suivante : appel récursif pour voir si la valeur est bonne par la suite
            if (technique_backtracking(grid, emptyTilePositions, emptyTileCount, iTilePosition + 1)) {
                // La valeur est bonne, on la met et on sort.
                grid->tiles[pos.row][pos.column]._value = value;
                return true;
            }

            // La résolution des cases suivantes assumant cette valeur a échoué, on a donc pas la bonne valeur.
            GRID_MARK_VALUE_FREE(grid, pos.row, pos.column, value, true);
        }
    }

    // On a échoué pour toutes les valeurs.
    return false;
}

void technique_backtracking_swap_tiles(tGrid const *grid, tPositionArray emptyTilePositions, int emptyTileCount, int iHere)
{
    assert(0 <= iHere && iHere < emptyTileCount);

    tPosition pos = emptyTilePositions[iHere];
    int iMin = iHere;
    GRID_TILE_POSSIBLE_VALUES_COUNT(grid, pos.row, pos.column, possibleValCountMin);

    // trouver la case après qui a le moins de valeurs possibles
    for (int i = iHere + 1; i < emptyTileCount; i++) {
        pos = emptyTilePositions[i];
        GRID_TILE_POSSIBLE_VALUES_COUNT(grid, pos.row, pos.column, possibleValCountI);

        if (possibleValCountI < possibleValCountMin) {
            iMin = i;
            possibleValCountMin = possibleValCountI;
        }
    }

    // on échange les cases
    tPosition tmp = emptyTilePositions[iHere];
    emptyTilePositions[iHere] = emptyTilePositions[iMin];
    emptyTilePositions[iMin] = tmp;
}

bool technique_nakedSingleton(tGrid *grid, int row, int column)
{
    bool progress = false;

    tTile *tile = &grid->tiles[row][column];
    if (TILE_CANDIDATE_COUNT(*tile) == 1) {
        TILE_GET_FIRST_CANDIDATE(*tile, candidate);
        // retirer tous les candidats correspondants
        // il y aura forcément au moins un retrait, le candidat unique de laCase.
        progress |= grid_removeCandidateFromRow(grid, row, candidate);
        progress |= grid_removeCandidateFromColumn(grid, column, candidate);
        progress |= grid_removeCandidateFromBlock(grid, row, column, candidate);
    }

    return progress;
}

bool technique_hiddenSingleton(tGrid *grid, int row, int column)
{
    tPosition candidatePos;
    int candidate;
    bool progress = false;

    int const BLOCK_ROW = BLOCK_INDEX(row);
    int const BLOCK_COL = BLOCK_INDEX(column);

    // Bloc
    if ((candidate = technique_hiddenSingleton_findUniqueCandidate(
             grid,
             BLOCK_ROW, BLOCK_ROW + N,
             BLOCK_COL, BLOCK_COL + N,
             &candidatePos))
        != 0) {
        grid_tile_provideValue(grid, candidatePos.row, candidatePos.column, candidate);
        grid_removeCandidateFromRow(grid, candidatePos.row, candidate);
        grid_removeCandidateFromColumn(grid, candidatePos.column, candidate);
        progress = true;
    }
    // Ligne
    if ((candidate = technique_hiddenSingleton_findUniqueCandidate(
             grid,
             row, row + 1,
             0, SIZE,
             &candidatePos))
        != 0) {
        grid_tile_provideValue(grid, candidatePos.row, candidatePos.column, candidate);
        grid_removeCandidateFromBlock(grid, candidatePos.row, candidatePos.column, candidate);
        grid_removeCandidateFromColumn(grid, candidatePos.column, candidate);
        progress = true;
    }
    // Colonne
    if ((candidate = technique_hiddenSingleton_findUniqueCandidate(
             grid,
             0, SIZE,
             column, column + 1,
             &candidatePos))
        != 0) {
        grid_tile_provideValue(grid, candidatePos.row, candidatePos.column, candidate);
        grid_removeCandidateFromBlock(grid, candidatePos.row, candidatePos.column, candidate);
        grid_removeCandidateFromRow(grid, candidatePos.row, candidate);
        progress = true;
    }

    return progress;
}

int technique_hiddenSingleton_findUniqueCandidate(
    tGrid const *grid,
    int rStart, int rEnd,
    int cStart, int cEnd,
    tPosition *candidatePosition)
{
    tCandidateCounts candidateCounts = {0};

    for (int r = rStart; r < rEnd; r++) {
        for (int c = cStart; c < cEnd; c++) {
            tTile tile = grid->tiles[r][c];
            for (int candidate = 1; candidate <= SIZE; candidate++) {
                candidateCounts[candidate] += TILE_HAS_CANDIDATE(tile, candidate);
            }
        }
    }

    // Rechercher le candidat unique
    int candidate = 1;
    while (candidate <= SIZE && candidateCounts[candidate] != 1) {
        candidate++;
    }

    // Si il n'pas été trouvé
    if (candidate == SIZE + 1) {
        return 0;
    }

    for (int r = rStart; r < rEnd; r++) {
        for (int c = cStart; c < cEnd; c++) {
            if (TILE_HAS_CANDIDATE(grid->tiles[r][c], candidate)) {
                *candidatePosition = (tPosition){.row = r, .column = c};
                return candidate;
            }
        }
    }

    // Impossible : candidat unique non trouvé alors qu'il a été trouvé plus haut
    abort();
}

bool technique_nakedPair(tGrid *grid, int row, int column)
{
    bool progress = false;

    tTile *tileRowColumn = &grid->tiles[row][column];

    if (TILE_CANDIDATE_COUNT(*tileRowColumn) == 2) {
        int const BLOCK_ROW = BLOCK_INDEX(row);
        int const BLOCK_COL = BLOCK_INDEX(column);

        tPair2 pair = (tPair2){
            .candidates = {
                tile_candidateAt(tileRowColumn, 1),
                tile_candidateAt(tileRowColumn, 2)},
            .count = 1,
        };

        // incrémentation des candidats dans une structure
        for (int r = BLOCK_ROW; r < BLOCK_ROW + N && pair.count < 2; r++) {
            for (int c = BLOCK_COL; c < BLOCK_COL + N && pair.count < 2; c++) {
                // si la case contient les candidats on incrémente le compteur de paire
                pair.count += (r != row || c != column) && TECHNIQUE_NAKED_PAIR_IS_PAIR_TILE(grid->tiles[r][c], pair);
            }
        }

        if (pair.count == 2) {
            // Supprimer tous les candidats du bloc sauf sur les cases contenant uniquement les candidats de la paire
            // On ne peut donc pas utiliser grid_removeCandidateFromBlock
            for (int r = BLOCK_ROW; r < BLOCK_ROW + N; r++) {
                for (int c = BLOCK_COL; c < BLOCK_COL + N; c++) {
                    bool isNotPairTile = !TECHNIQUE_NAKED_PAIR_IS_PAIR_TILE(grid->tiles[r][c], pair);
                    progress |= isNotPairTile && grid_tile_removeCandidate(grid, r, c, pair.candidates[0]);
                    progress |= isNotPairTile && grid_tile_removeCandidate(grid, r, c, pair.candidates[1]);
                }
            }
        }
    }

    return progress;
}

bool technique_hiddenPair(tGrid *grid, int row, int column)
{
    tPosition pairTilesPositions[PAIR_SIZE] = {(tPosition){.row = row, .column = column}};
    int candidates[PAIR_SIZE];
    bool progress = false;

    int const BLOCK_ROW = BLOCK_INDEX(row);
    int const BLOCK_COL = BLOCK_INDEX(column);

    tTile *tileRowColumn = &grid->tiles[row][column];

    // Bloc
    progress |= (TILE_CANDIDATE_COUNT(*tileRowColumn) >= 2
                 && technique_hiddenPair_findPair(grid, BLOCK_ROW, BLOCK_ROW + N, BLOCK_COL, BLOCK_COL + N, pairTilesPositions, candidates))
             && technique_hiddenPair_removePairTiles(grid, pairTilesPositions, candidates);
    // Ligne
    progress |= (TILE_CANDIDATE_COUNT(*tileRowColumn) >= 2
                 && technique_hiddenPair_findPair(grid, row, row + 1, 0, SIZE, pairTilesPositions, candidates))
             && technique_hiddenPair_removePairTiles(grid, pairTilesPositions, candidates);
    // Colonne
    progress |= (TILE_CANDIDATE_COUNT(*tileRowColumn) >= 2
                 && technique_hiddenPair_findPair(grid, 0, SIZE, column, column + 1, pairTilesPositions, candidates))
             && technique_hiddenPair_removePairTiles(grid, pairTilesPositions, candidates);

    return progress;
}

bool technique_hiddenPair_findPair(
    tGrid const *grid,
    int rStart, int rEnd,
    int cStart, int cEnd,
    tPosition pairTilesPositions[PAIR_SIZE], int candidates[PAIR_SIZE])
{
    tTile firstPairTile = grid->tiles[pairTilesPositions[0].row][pairTilesPositions[0].column];

    assert(TILE_CANDIDATE_COUNT(firstPairTile) >= 2);

    for (candidates[0] = 1; candidates[0] <= SIZE; candidates[0]++) {
        if (!TILE_HAS_CANDIDATE(firstPairTile, candidates[0])) {
            continue;
        }

        for (candidates[1] = candidates[0] + 1; candidates[1] <= SIZE; candidates[1]++) {
            if (!TILE_HAS_CANDIDATE(firstPairTile, candidates[1])) {
                continue;
            }

            // On recommence la recherche d'une paire avec de nouveaux candidats.
            if (technique_hiddenPair_findPairTiles(
                    grid, candidates,
                    rStart, rEnd,
                    cStart, cEnd,
                    pairTilesPositions)) {
                return true;
            }
        }
    }

    return false;
}

bool technique_hiddenPair_findPairTiles(
    tGrid const *grid, int const candidates[PAIR_SIZE],
    int rStart, int rEnd,
    int cStart, int cEnd,
    tPosition pairTilesPositions[PAIR_SIZE])
{
    int nbPairTiles = 1; // Nombre de cases contenant la paire (candidate1, candidate2) trouvées.
    int nbPairTilesContainingOtherCandidates = 0;

    // recherche des paires cachés
    // On ajoute chaque case qui contient les teux candidats
    for (int r = rStart; r < rEnd && nbPairTiles <= PAIR_SIZE + 1; r++) {
        for (int c = cStart; c < cEnd && nbPairTiles <= PAIR_SIZE + 1; c++) {
            if (r == pairTilesPositions[0].row && c == pairTilesPositions[0].column) {
                continue;
            }
            tTile tile = grid->tiles[r][c];

            if (TILE_HAS_CANDIDATE(tile, candidates[0])
                && TILE_HAS_CANDIDATE(tile, candidates[1])) {
                nbPairTilesContainingOtherCandidates += TILE_CANDIDATE_COUNT(tile) > PAIR_SIZE;
                if (nbPairTiles < PAIR_SIZE) {
                    pairTilesPositions[nbPairTiles] = (tPosition){.row = r, .column = c};
                }
                nbPairTiles++;
            }
            // rechercher les candidats tout seuls qui invalideraient la paire
            else if (TILE_HAS_CANDIDATE(tile, candidates[0])
                     || TILE_HAS_CANDIDATE(tile, candidates[1])) {
                return false;
            }
        }
    }

    // Pour que la paire cachée servent à quelquechose, il faut qu'au moins une case de la paire contienne d'autres candidats.
    // Sinon, on ne pourra supprimer aucun candidat.
    return nbPairTiles == PAIR_SIZE && nbPairTilesContainingOtherCandidates > 0;
}

bool technique_hiddenPair_removePairTiles(
    tGrid *grid,
    tPosition const pairTilesPositions[PAIR_SIZE],
    int const candidates[PAIR_SIZE])
{
    bool progress = false;
    // Pour chaque cellule contenant la paire :
    for (int iPos = 0; iPos < PAIR_SIZE; ++iPos) {
        tPosition pos = pairTilesPositions[iPos];
        // supprimer tous ses candidats
        for (int candidate = 1; candidate <= SIZE; ++candidate) {
            // sauf ceux formant la paire
            progress |= candidate != candidates[0]
                     && candidate != candidates[1]
                     && grid_tile_removeCandidate(grid, pos.row, pos.column, candidate);
        }
    }
    return progress;
}

bool technique_x_wing(tGrid *grid)
{
    bool progress = false;

    // X-WING VERTICAL
    // Identifier 2 colonnes qui ont chacune, sur les mêmes lignes, exactement deux cellules où un candidat apparaît.
    /* On obtient un rectangle avec pour sommets ces quatre cellules :
    _   _
    A---B
    |   |
    C---D
    ‾   ‾*/
    for (int colAC = 0; colAC < SIZE; colAC++) {
        for (int colBD = colAC + 1; colBD < SIZE; colBD++) {
            for (int candidate = 1; candidate <= SIZE; candidate++) {
                int rows[2];
                int candidateInBothCount = 0;
                int candidateCounts[2] = {0};
                for (int row = 0; row < SIZE; row++) {
                    bool colACHasCandidate = TILE_HAS_CANDIDATE(grid->tiles[row][colAC], candidate);
                    bool colBDHasCandidate = TILE_HAS_CANDIDATE(grid->tiles[row][colBD], candidate);
                    candidateCounts[0] += colACHasCandidate;
                    candidateCounts[1] += colBDHasCandidate;
                    if (colACHasCandidate && colBDHasCandidate) {
                        if (candidateInBothCount < 2) {
                            rows[candidateInBothCount] = row;
                        }
                        candidateInBothCount++;
                    }
                }
                if (candidateInBothCount == 2 && candidateCounts[0] == 2 && candidateCounts[1] == 2) {
                    // eliminer le candidat des lignes (AB) et (CD).
                    for (int c = 0; c < SIZE; c++) {
                        // on élimine pas le candidat des cases qui forment le X.
                        progress |= c != colAC && c != colBD && grid_tile_removeCandidate(grid, rows[0], c, candidate);
                        progress |= c != colAC && c != colBD && grid_tile_removeCandidate(grid, rows[1], c, candidate);
                    }
                }
            }
        }
    }

    // X-WING HORIZONTAL
    // Identifier 2 lignes qui ont chacune, sur les mêmes colonnes, exactement deux cellules où un candidat apparaît.
    /* On obtient un rectangle avec pour sommets ces quatre cellules :
    (A---B)
     |   |
    (C---D)*/
    for (int rowAB = 0; rowAB < SIZE; rowAB++) {
        for (int rowCD = rowAB + 1; rowCD < SIZE; rowCD++) {
            for (int candidate = 1; candidate <= SIZE; candidate++) {
                int columns[2];
                int candidateInBothCount = 0;
                int candidateCounts[2] = {0};
                for (int col = 0; col < SIZE; col++) {
                    bool rowABHasCandidate = TILE_HAS_CANDIDATE(grid->tiles[rowAB][col], candidate);
                    bool rowCDHasCandidate = TILE_HAS_CANDIDATE(grid->tiles[rowCD][col], candidate);
                    candidateCounts[0] += rowABHasCandidate;
                    candidateCounts[1] += rowCDHasCandidate;
                    if (rowABHasCandidate && rowCDHasCandidate) {
                        if (candidateInBothCount < 2) {
                            columns[candidateInBothCount] = col;
                        }
                        candidateInBothCount++;
                    }
                }
                if (candidateInBothCount == 2 && candidateCounts[0] == 2 && candidateCounts[1] == 2) {
                    // eliminer le candidat des colonnes (AC) et (BD).
                    for (int r = 0; r < SIZE; r++) {
                        // on élimine pas le candidat des cases qui forment le X.
                        progress |= r != rowAB && r != rowCD && grid_tile_removeCandidate(grid, r, columns[0], candidate);
                        progress |= r != rowAB && r != rowCD && grid_tile_removeCandidate(grid, r, columns[1], candidate);
                    }
                }
            }
        }
    }

    return progress;
}

bool grid_removeCandidateFromRow(tGrid *grid, int row, int candidate)
{
    bool progress = false;
    for (int c = 0; c < SIZE; c++) {
        progress |= grid_tile_removeCandidate(grid, row, c, candidate);
    }
    return progress;
}

bool grid_removeCandidateFromColumn(tGrid *grid, int column, int candidate)
{
    bool progress = false;
    for (int r = 0; r < SIZE; r++) {
        progress |= grid_tile_removeCandidate(grid, r, column, candidate);
    }
    return progress;
}

bool grid_removeCandidateFromBlock(tGrid *grid, int row, int column, int candidate)
{
    bool progress = false;

    int const BLOCK_ROW = BLOCK_INDEX(row);
    int const BLOCK_COL = BLOCK_INDEX(column);

    for (int r = BLOCK_ROW; r < BLOCK_ROW + N; r++) {
        for (int c = BLOCK_COL; c < BLOCK_COL + N; c++) {
            progress |= grid_tile_removeCandidate(grid, r, c, candidate);
        }
    }

    return progress;
}