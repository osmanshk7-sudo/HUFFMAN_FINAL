/*
 * PROTECTION CONTRE LES INCLUSIONS MULTIPLES
 * ------------------------------------------
 * En C, quand un fichier .h est inclus plusieurs fois dans la même unité
 * de compilation, cela cause des erreurs de redéfinition.
 * ------------------------------------------
 * Exemple :
 * - main.c inclut bt.h et utils.h
 * - utils.h inclut aussi bt.h
 * → bt.h est inclus 2 fois → erreur!
 * 
 * Solution : #ifndef (if not defined)
 * 1. Si BT_H n'est pas défini, on le définit et on compile le contenu
 * 2. Si BT_H est déjà défini, on saute tout jusqu'au #endif
 * -------------------------------------------
 * (Appris via recherche personnelle pour les besoins du projet)
 */
#ifndef BT_H
#define BT_H

typedef struct bt {
    unsigned char data;
    int frequency;
    struct bt *left;
    struct bt *right;
} bt;

bt *createNode(unsigned char data, int frequency, bt *left, bt *right);
void freeTree(bt *root);

#endif // BT_H