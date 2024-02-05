/** @file
 * @brief Implémentation des fonctions de manipulation des cases
 * @author 5cover, Matteo-K
 * @date 16/01/2024
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "const.h"
#include "tTile.h"

int tile_candidateAt(tTile const *tile, int n)
{
    assert(1 <= n && n <= SIZE);

    int candidate = 0;

    while (n > 0 && candidate <= SIZE) {
        candidate++;
        n -= tile->hasCandidate[candidate];
    }

    return candidate > SIZE ? 0 : candidate;
}
