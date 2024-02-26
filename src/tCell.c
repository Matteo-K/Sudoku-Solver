/** @file
 * @brief Cell functions implementation
 * @author 5cover, Matteo-K
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "const.h"
#include "tCell.h"

int cell_candidateAt(tCell const *cell, tIndex n)
{
    assert(n <= SIZE);
    assert(1 <= n && n <= SIZE);

    tValue candidate = 0;

    while (n > 0 && candidate <= SIZE) {
        candidate++;
        n -= cell->hasCandidate[candidate];
    }

    return candidate > SIZE ? 0 : candidate;
}
