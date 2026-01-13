#ifndef HUFFMAN_H
#define HUFFMAN_H

/* 
 * Le "./" précise explicitement que bt.h se trouve dans le dossier courant.
 * C'est équivalent à #include "bt.h" dans notre structure de projet.
 */
#include "./bt.h" 
#include <stdio.h>

typedef struct queue {
    bt *pointer;
    struct queue *nxt;
} queue;

// File de priorité
queue *createEleQueue(unsigned char data, int freq, bt *left, bt *right);
queue *insertQueue(queue *root, queue *ele);
queue *createQueue(int *freq);
queue *deleteEleQueue(queue *root, unsigned char data);
int sizequeue(queue *root);
void freeQueue(queue *root);

// Arbre Huffman
bt *createHuffmanTree(queue *root);
void generateCodes(bt *root, char *codes[], char *buffer, int depth);

// Compression
void countFrequencies(unsigned char *data, int size, int *freq);
char *compressData(unsigned char *data, int size, char *codes[]);
void saveTree(FILE *file, bt *root);
void writeCompressed(FILE *file, char *compressed);

// Décompression
bt *loadTree(FILE *file);
unsigned char *decompressData(FILE *file, bt *tree, int original_size);

// Utilitaires
unsigned char *readFile(FILE *file, int *size);
void writeFile(FILE *file, unsigned char *data, int size);

#endif // HUFFMAN_H