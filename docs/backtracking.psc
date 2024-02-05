fonction technique_backtracking(tGrille grille, int numeroCase)
délivre booléen c'est
  ligne : entier;
  colonne : entier;
  resultat : booléen;
début
  resultat := FAUX;
  si numeroCase = TAILLE * TAILLE alors
    // on a traité toutes les cases, la grille est résolue
    resultat := VRAI;
  sinon
    // On récupère les "coordonnées" de la case
    ligne := numeroCase / TAILLE;
    colonne := numeroCase % TAILLE;
    si grille[ligne][colonne]!=0 alors
      // Si la case n’est pas vide, on passe à la suivante
      // (appel récursif)
      resultat := technique_backtracking(grille, numeroCase+1);
    sinon
      pour valeur de 1 à TAILLE faire
        si absentSurLigne(valeur,grille,ligne)
           ET absentSurColonne(valeur,grille,colonne)
           ET absentSurBloc(valeur,grille,ligne,colonne) alors
           // Si la valeur est autorisée on  l'inscrit dans la case...
           grille[ligne][colonne] := valeur;
           // ... et on passe à la suivante : appel récursif
           // pour voir si ce choix est bon par la suite
            si technique_backtracking(grille,numeroCase+1) = VRAI alors
                resultat := VRAI;
            sinon
                grille[ligne][colonne] := 0;
            finSi
         finSi
      finFaire
    finSi
  finSi
  retourne resultat;
fin