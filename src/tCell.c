/** @file
 * @brief Cell functions implementation
 * @author 5cover, Matteo-K
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "const.h"
#include "tCell.h"

int cell_candidateAt(tCell const *cell, tIntSize n) {
    assert(n <= cell_candidate_count(*cell));

    tIntSize candidate = 0;

    while (n > 0) {
        candidate++;
        n -= cell->hasCandidate[candidate];
    }

    return candidate;
}
