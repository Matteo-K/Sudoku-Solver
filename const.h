/** @file
 * @brief Constantes symboliques
 * @author 5cover, Matteo-K
 * @date 16/01/2024
 */

#ifndef CONST_H
#define CONST_H

/// @brief Entier : facteur de taille de la grille
#define N 4

/// @brief Entier : côté de la grille
#define SIZE (N * N)

/// @brief Entier : code d'erreur retourné par @ref grid_load quand le fichier ne peut pas être ouvert.
#define ERROR_FOPEN_FAILED -1
/// @brief Entier : code d'erreur retourné par @ref grid_load quand le fichier contient des données invalides.
#define ERROR_INVALID_DATA -2

/// @brief Entier : taille d'une paire de candidats.
#define PAIR_SIZE 2

/* Macros de configuration de l'affichage */

/// @brief Entier : marge intérieure de l'affichage de la grille
#define DISPLAY_PADDING 2 // Nombre de chiffres de SIZE

/* Macros des caractères de l'affichage */

/// @brief Caractère : représente un espace dans l'affichage de la grille
#define DISPLAY_SPACE ' '
/// @brief Caractère : représente une valeur vide dans l'affichage de la grille
#define DISPLAY_EMPTY_VALUE '.'
/// @brief Caractère : représente une intersection dans l'affichage de la grille
#define DISPLAY_INTERSECTION '+'
/// @brief Caractère : représente un trait vertical dans l'affichage de la grille
#define DISPLAY_VERTICAL_LINE '|'
/// @brief Caractère : représente un trait horizontal dans l'affichage de la grille
#define DISPLAY_HORIZONTAL_LINE '-'

#endif // CONST_H
