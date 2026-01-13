#include <stdlib.h>
#include "../include/bt.h"

bt *createNode(unsigned char data, int frequency, bt *left, bt *right) {
    bt *node = (bt *)malloc(sizeof(bt));
    if (!node) return NULL;
    node->data = data;
    node->frequency = frequency;
    node->left = left;
    node->right = right;
    return node;
}

void freeTree(bt *root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}