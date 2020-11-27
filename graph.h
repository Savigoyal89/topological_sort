//
// Created by Savi Goyal on 11/25/20.
//

#ifndef KAHN_GRAPH_H
#define KAHN_GRAPH_H

#include "vector.h"

#include <stdio.h>
#include <stdlib.h>

// Exit function to handle fatal errors
static inline void err_exit(char *msg) {
    printf("[Fatal Error]: %s \nExiting...\n", msg);
    exit(1);
}

typedef struct Node {
    int ID;
    int Degree;
    vector Vertices;
    int Processed;
} node_t, *node_p;

// Function declarations
node_p createNode(int ID);
void printNode(node_p node);
void addEdge(node_t *src, node_t *dst);
void removeEdge(node_p src, node_t *dst);

#endif //KAHN_GRAPH_H
