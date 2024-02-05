# Optimisations

## À faire

### Implémenter N-uplet nu et N-uplet caché

Maybe one day...

## Terminé

### Arrêter de comptabiliser les résultats de technique

Le type `tResultatResolution`, hérité de la partie 1, aggrège une information qui nous est inutile pour la partie 2 : le nombre de cases remplies et de candidats éliminés.

On a plus besoin d'afficher les résultats de résolution. La seule information utile fournie par cette structure est si une technique a réussi à faire un progrès ou non.

On peut donc la remplacer par un simple booléen.

Cela simplifiera le code et permettera de gagner un peu en performances (gain comptabilisé dans [Résultats.md](Résultats.md))

### Fusion de `tile_removeCandidate` et `tile_removeCandidateSafely`

Ce sera une fonction en moins appelée.

### Ajouter la technique `x-wing` de Mattéo

### Passer `possible` en $O(1)$

Avec la grille B, `possible` est appelée plus de 60 millions de fois, il est donc vital de l'optimiser le plus possible.

`possible` est actuellement `O(SIZE + N²)`. Il est possible de passer en `O(1)` ainsi :

One potential optimization is to maintain separate arrays for the rows, columns, and blocks that store the values already present in them. This way, you can check if a value is possible in constant time by looking up these arrays, instead of iterating over the row, column, and block every time.

```c
// rowHasValue[rowIndex][value]
bool rowHasValue[SIZE][SIZE+1] = {false};
// rowHasValue[columnIndex][value]
bool columnHasValue[SIZE][SIZE+1] = {false};
// rowHasValue[blockRowIndex][blockColumnIndex][value]
bool blockHasValue[N][N][SIZE+1] = {false};

void setCell(tGrid grid, int row, int column, int value) {
    int oldValue = grid[row][column].value;
    grid[row][column].value = value;

    // Update the row, column, and block arrays
    if (oldValue != 0) {
        rows[row][oldValue] = false;
        columns[column][oldValue] = false;
        blocks[row / N][column / N][oldValue] = false;
    }
    if (value != 0) {
        rows[row][value] = true;
        columns[column][value] = true;
        blocks[row / N][column / N][value] = true;
    }
}

bool possible(int row, int column, int value) {
    return !rowHasValue[row][value]
        && !columnHasValue[column][value]
        && !blockHasValue[row / N][column / N][value];
}

```
