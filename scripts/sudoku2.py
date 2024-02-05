#!/bin/env python3

from grid import grid_print
from timeit import timeit
import sys

N: int = 4
SIZE: int = N*N


def sudoku2(grille: tuple[list[int]]) -> bool:
    # C'est l'équivalent de ce que j'ai fait en C, sauf que moi j'utilise des tableaux de booléens.
    freeValsRows, freeValsCols, freeValsBlocks = [
        tuple(set(range(1, SIZE+1)) for _ in range(SIZE)) for _ in range(3)]

    casesVides: list[tuple[int]] = []

    def essaye_case(i: int) -> bool:
        if i == len(casesVides):
            return True

        row, col, block = casesVides[i]

        freeValsRow = freeValsRows[row]
        freeValsCol = freeValsCols[col]
        freeValsBlock = freeValsBlocks[block]

        # Pour chaque valeur dispo dans la ligne, dans la colonne et dans le bloc(équivalent de la fonction possible)
        for val in freeValsRow & freeValsCol & freeValsBlock:
            # On considère que val n'est plus une valeur possible
            freeValsRow.remove(val)
            freeValsCol.remove(val)
            freeValsBlock.remove(val)

            # On essaye de continuer la résolution en assumant donc que la case actuelle a été remplie avec val
            if essaye_case(i + 1):
                # Si c'est un succès, on termine en donnant la valeur
                grille[row][col] = val
                return True

            # Sinon, val est une valeur possible à nouveau
            freeValsRow.add(val)
            freeValsCol.add(val)
            freeValsBlock.add(val)

        return False

    # préparation
    for row in range(SIZE):
        for col in range(SIZE):
            block = (col // N) + N * (row // N)
            if grille[row][col] == 0:
                casesVides.append((row, col, block))
            else:
                freeValsRows[row].remove(grille[row][col])
                freeValsCols[col].remove(grille[row][col])
                freeValsBlocks[block].remove(grille[row][col])

    # résolution
    return essaye_case(0)


def sudoku3(grille):
    freeValsRows, freeValsCols, freeValsBlocks = [
        tuple(set(range(1, SIZE+1)) for _ in range(SIZE)) for _ in range(3)]

    casesVides: list[tuple[int]] = []

    def selectionne_case(iHere):
        # on regarde la case actuelle
        l, c, b = casesVides[iHere]
        iMin = iHere
        nbCandidatsMin = len(
            freeValsRows[l] & freeValsCols[c] & freeValsBlocks[b])
        # trouve la case du minimum du nombre de cases possibles
        for i in range(iHere + 1, len(casesVides)):
            l, c, b = casesVides[i]
            nbCandidats_i = len(
                freeValsRows[l] & freeValsCols[c] & freeValsBlocks[b])
            if nbCandidats_i < nbCandidatsMin:
                iMin = i
                nbCandidatsMin = nbCandidats_i

        # si le minimum n'est pas cette case
        if iMin != iHere:
            # on échange les cases
            casesVides[iMin], casesVides[iHere] = casesVides[iHere], casesVides[iMin]

    def essaye_case(pos) -> bool:
        if pos == len(casesVides):
            return True

        selectionne_case(pos)

        row, col, block = casesVides[pos]

        freeValsRow = freeValsRows[row]
        freeValsCol = freeValsCols[col]
        freeValBlock = freeValsBlocks[block]

        for val in sorted(freeValsRow & freeValsCol & freeValBlock):
            freeValsRow.remove(val)
            freeValsCol.remove(val)
            freeValBlock.remove(val)

            if essaye_case(pos + 1):
                grille[row][col] = val
                return True

            freeValsRow.add(val)
            freeValsCol.add(val)
            freeValBlock.add(val)

        return False

    # préparation
    for r in range(SIZE):
        for c in range(SIZE):
            b = (c // N) + N * (r // N)
            if grille[r][c] == 0:
                casesVides.append((r, c, b))
            else:
                freeValsRows[r].remove(grille[r][c])
                freeValsCols[c].remove(grille[r][c])
                freeValsBlocks[b].remove(grille[r][c])

    # résolution
    return essaye_case(0)


def grilleA(): return ([15, 0, 6, 0, 0, 3, 0, 16, 0, 11, 13, 0, 0, 1, 0, 0],
                       [0, 12, 4, 0, 14, 0, 0, 0, 0, 6, 0, 5, 15, 0, 11, 3],
                       [3, 2, 7, 0, 11, 0, 0, 0, 4, 14, 12, 15, 0, 8, 0, 0],
                       [0, 0, 0, 0, 6, 8, 12, 15, 0, 0, 0, 7, 2, 0, 13, 0],
                       [0, 13, 14, 15, 7, 5, 0, 9, 3, 0, 10, 12, 0, 11, 0, 1],
                       [10, 0, 0, 16, 8, 14, 0, 0, 5, 0, 0, 0, 4, 6, 0, 0],
                       [0, 0, 0, 5, 0, 0, 3, 6, 0, 8, 9, 0, 12, 10, 0, 0],
                       [2, 0, 0, 7, 4, 0, 10, 1, 0, 13, 0, 6, 0, 14, 3, 0],
                       [0, 0, 13, 0, 15, 4, 0, 0, 0, 0, 0, 2, 1, 7, 9, 16],
                       [16, 3, 15, 0, 0, 0, 7, 13, 0, 0, 0, 0, 14, 12, 0, 0],
                       [4, 0, 10, 0, 1, 0, 8, 0, 0, 0, 14, 13, 11, 2, 6, 15],
                       [0, 1, 11, 2, 9, 0, 0, 14, 12, 0, 7, 4, 0, 0, 0, 0],
                       [0, 11, 0, 3, 0, 7, 13, 0, 6, 12, 15, 0, 0, 5, 0, 0],
                       [13, 0, 5, 0, 12, 9, 6, 2, 8, 16, 4, 0, 7, 3, 1, 0],
                       [0, 10, 0, 6, 0, 0, 0, 4, 7, 0, 5, 0, 0, 16, 14, 8],
                       [0, 16, 0, 0, 5, 0, 0, 0, 13, 0, 1, 0, 0, 0, 12, 2])


def grilleB(): return ([12, 7, 0, 9, 0, 0, 0, 10, 0, 0, 0, 0, 0, 15, 0, 5],
                       [0, 1, 5, 2, 0, 12, 0, 0, 0, 0, 0, 7, 0, 13, 14, 6],
                       [10, 0, 0, 11, 0, 0, 15, 0, 0, 13, 0, 14, 3, 4, 0, 2],
                       [3, 0, 15, 0, 16, 0, 0, 0, 4, 0, 0, 0, 12, 0, 9, 0],
                       [0, 12, 0, 8, 0, 3, 10, 6, 0, 0, 15, 0, 1, 0, 16, 0],
                       [0, 0, 11, 3, 0, 15, 0, 0, 7, 0, 0, 0, 0, 5, 0, 13],
                       [0, 0, 0, 15, 7, 5, 0, 0, 3, 1, 2, 4, 0, 11, 0, 14],
                       [0, 13, 0, 7, 0, 0, 0, 0, 0, 0, 6, 0, 8, 0, 2, 0],
                       [0, 0, 0, 12, 0, 14, 0, 1, 8, 9, 0, 0, 2, 16, 4, 0],
                       [0, 0, 0, 0, 0, 0, 5, 9, 0, 0, 0, 2, 13, 1, 0, 0],
                       [0, 0, 1, 0, 0, 0, 4, 0, 16, 0, 5, 15, 0, 8, 0, 3],
                       [0, 0, 7, 14, 0, 0, 6, 8, 1, 0, 0, 0, 5, 0, 0, 0],
                       [0, 8, 6, 0, 5, 10, 0, 0, 0, 4, 16, 0, 0, 14, 0, 12],
                       [0, 9, 2, 10, 4, 0, 0, 16, 12, 0, 3, 0, 0, 0, 0, 0],
                       [0, 0, 0, 0, 6, 8, 0, 7, 14, 0, 0, 0, 0, 0, 0, 0],
                       [11, 0, 12, 0, 9, 0, 14, 0, 0, 0, 0, 13, 0, 0, 0, 0])


def test(alg, gridName, grid):
    grid_print(grid, sys.stdout)
    print(f'{alg.__name__} Grille {gridName} : {timeit(lambda: alg(grid), number=1)*5:.5} ms')
    grid_print(grid, sys.stdout)


if __name__ == '__main__':
    test(sudoku2, 'A', grilleA())
    test(sudoku3, 'B', grilleB())
    test(sudoku2, 'A', grilleA())
    test(sudoku3, 'B', grilleB())
