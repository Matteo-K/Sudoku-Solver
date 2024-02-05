# Implémentation technique X-Wing

## Vertical

Vertical car on supprime les candidats des colonnes.

1. Identifier 2 lignes qui ont chacune, sur les mêmes colonnes, exactement deux cellules où un candidat apparaît.
2. On obtient un rectangle avec pour sommets ces quatre cellules :

$$
\begin{CD}
   A @= B \\
 @VVV @VVV \\
   C @= D
\end{CD}
$$

3. On peut supprimer le candidat des colonnes $(AC)$ et $(BD)$ sauf sur $A$, $B$, $C$, et $D$.

## Horizontal

Horizontal car on supprime les candidats des lignes.

1. Identifier 2 colonnes qui ont chacune, sur les mêmes lignes, exactement deux cellules où un candidat apparaît.
2. On obtient un rectangle avec pour sommets ces quatre cellules :

$$
\begin{CD}
   A @>>> B \\
  @|     @| \\
   C @>>> D
\end{CD}
$$

3. On peut supprimer le candidat des lignes $(AB)$ et $(CD)$ sauf sur $A$, $B$, $C$, et $D$.
