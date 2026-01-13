#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/huffman.h"
#include "../include/bt.h"

// ========== PRIORITY QUEUE ==========

queue *createEleQueue(unsigned char data, int freq, bt *left, bt *right) {
    queue *q = (queue *)malloc(sizeof(queue));
    if (!q) return NULL;
    q->pointer = createNode(data, freq, left, right);
    q->nxt = NULL;
    return q;
}

queue *insertQueue(queue *root, queue *ele) {
    if (!root) return ele;
    if (root->pointer->frequency <= ele->pointer->frequency) {
        ele->nxt = root;
        return ele;
    }
    root->nxt = insertQueue(root->nxt, ele);
    return root;
}

queue *createQueue(int *freq) {
    queue *root = NULL;
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            queue *ele = createEleQueue((unsigned char)i, freq[i], NULL, NULL);
            if (ele) root = insertQueue(root, ele);
        }
    }
    return root;
}

queue *deleteEleQueue(queue *root, unsigned char data) {
    if (!root) return NULL;
    if (root->pointer->data == data) {
        queue *temp = root->nxt;
        // DON'T free root here, we'll free it later with freeQueue
        return temp;
    }
    root->nxt = deleteEleQueue(root->nxt, data);
    return root;
}

int sizequeue(queue *root) {
    if (!root) return 0;
    return 1 + sizequeue(root->nxt);
}

void freeQueue(queue *root) {
    queue *current = root;
    while (current != NULL) {
        queue *next = current->nxt;
        free(current);
        current = next;
    }
}

// ========== HUFFMAN TREE ==========

bt *createHuffmanTree(queue *root) {
    // Special case: empty queue (shouldn't happen with our checks)
    if (!root) return NULL;
    
    // Special case: single element (file with only one unique character)
    if (!root->nxt) {
        // Return tree with single leaf directly
        return root->pointer;
    }
    
    // Normal case: at least 2 elements
    while (root && root->nxt) {
        // Extract two nodes with lowest frequency
        bt *left = root->pointer;
        root = deleteEleQueue(root, left->data);
        
        bt *right = root->pointer;
        root = deleteEleQueue(root, right->data);
        
        // Create unique parent with left and right as children
        bt *parent = createNode(0, left->frequency + right->frequency, left, right);
        
        // Create queue element containing this parent
        queue *ele = (queue *)malloc(sizeof(queue));
        if (!ele) {
            freeTree(parent); // Cleanup on failure
            return NULL;
        }
        ele->pointer = parent;
        ele->nxt = NULL;
        
        // Insert into queue
        root = insertQueue(root, ele);
    }
    return root ? root->pointer : NULL;
}

void generateCodes(bt *root, char *codes[], char *buffer, int depth) {
    if (!root) return;
    if (!root->left && !root->right) {
        buffer[depth] = '\0';
        codes[root->data] = strdup(buffer);
        return;
    }
    buffer[depth] = '0';
    generateCodes(root->left, codes, buffer, depth + 1);
    buffer[depth] = '1';
    generateCodes(root->right, codes, buffer, depth + 1);
}

// ========== COMPRESSION ==========

void countFrequencies(unsigned char *data, int size, int *freq) {
    for (int i = 0; i < size; i++) {
        freq[data[i]]++;
    }
}

char *compressData(unsigned char *data, int size, char *codes[]) {
    // Calculate total bits needed
    int total_bits = 0;
    for (int i = 0; i < size; i++) {
        if (codes[data[i]]) {
            total_bits += (int)strlen(codes[data[i]]);
        }
    }
    
    // Allocate memory for compressed string
    char *compressed = (char *)malloc((size_t)total_bits + 1);
    if (!compressed) return NULL;
    
    // Build compressed string
    compressed[0] = '\0';
    for (int i = 0; i < size; i++) {
        if (codes[data[i]]) {
            strcat(compressed, codes[data[i]]);
        }
    }
    return compressed;
}

void saveTree(FILE *file, bt *root) {
    if (!root) return;
    if (!root->left && !root->right) {
        unsigned char flag = 1; // Leaf
        fwrite(&flag, 1, 1, file);
        fwrite(&root->data, 1, 1, file);
    } else {
        unsigned char flag = 0; // Internal node
        fwrite(&flag, 1, 1, file);
        saveTree(file, root->left);
        saveTree(file, root->right);
    }
}

void writeCompressed(FILE *file, char *compressed) {
    if (!compressed) return;
    
    int bit_count = 0;
    unsigned char byte = 0;
    size_t length = strlen(compressed);
    
    for (size_t i = 0; i < length; i++) {
        byte = (byte << 1) | (compressed[i] - '0');
        bit_count++;
        if (bit_count == 8) {
            fwrite(&byte, 1, 1, file);
            byte = 0;
            bit_count = 0;
        }
    }
    // Handle last incomplete byte
    if (bit_count > 0) {
        byte <<= (8 - bit_count);
        fwrite(&byte, 1, 1, file);
    }
}

// ========== DECOMPRESSION ==========

bt *loadTree(FILE *file) {
    unsigned char flag;
    if (fread(&flag, 1, 1, file) != 1) return NULL;
    
    if (flag == 1) {
        unsigned char data;
        if (fread(&data, 1, 1, file) != 1) return NULL;
        return createNode(data, 0, NULL, NULL);
    } else {
        bt *left = loadTree(file);
        bt *right = loadTree(file);
        if (!left || !right) {
            if (left) freeTree(left);
            if (right) freeTree(right);
            return NULL;
        }
        return createNode(0, 0, left, right);
    }
}

unsigned char *decompressData(FILE *file, bt *tree, int original_size) {
    if (original_size == 0 || !tree) return NULL;
    
    unsigned char *data = (unsigned char *)malloc((size_t)original_size);
    if (!data) return NULL;
    
    bt *current = tree;
    unsigned char byte;
    int data_index = 0;
    
    while (data_index < original_size) {
        if (fread(&byte, 1, 1, file) != 1) {
            free(data);
            return NULL;
        }
        for (int bit = 7; bit >= 0 && data_index < original_size; bit--) {
            if ((byte >> bit) & 1) {
                current = current->right;
            } else {
                current = current->left;
            }
            
            if (!current) {
                free(data);
                return NULL; // Arbre corrompu
            }
            
            if (!current->left && !current->right) {
                data[data_index++] = current->data;
                current = tree;
            }
        }
    }
    return data;
}

// ========== UTILITIES ==========

unsigned char *readFile(FILE *file, int *size) {
    if (!file) {
        *size = 0;
        return NULL;
    }
    
    // Sauvegarder la position actuelle
    long current_pos = ftell(file);
    if (current_pos < 0) {
        *size = 0;
        return NULL;
    }
    
    // Aller à la fin pour obtenir la taille
    if (fseek(file, 0, SEEK_END) != 0) {
        *size = 0;
        return NULL;
    }
    
    long file_size = ftell(file);
    if (file_size < 0) {
        *size = 0;
        return NULL;
    }
    
    // Retourner au début
    if (fseek(file, 0, SEEK_SET) != 0) {
        *size = 0;
        return NULL;
    }
    
    // Si fichier vide, retourner NULL
    if (file_size == 0) {
        *size = 0;
        return NULL;
    }
    
    // Allouer mémoire
    unsigned char *data = (unsigned char *)malloc((size_t)file_size);
    if (!data) {
        *size = 0;
        return NULL;
    }
    
    // Lire les données
    size_t bytes_read = fread(data, 1, (size_t)file_size, file);
    
    // Vérifier si on a tout lu
    if (bytes_read != (size_t)file_size) {
        free(data);
        *size = 0;
        return NULL;
    }
    
    // Restaurer position originale si nécessaire
    fseek(file, current_pos, SEEK_SET);
    
    *size = (int)file_size;
    return data;
}

void writeFile(FILE *file, unsigned char *data, int size) {
    if (data && size > 0 && file) {
        fwrite(data, 1, (size_t)size, file);
    }
}