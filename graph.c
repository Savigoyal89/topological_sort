#include "graph.h"

node_p createNode(int ID) {
    node_p newNode = (node_p)malloc(sizeof(node_t));
    if (!newNode) { err_exit("Unable to allocate memory for new node"); }
    newNode->ID = ID;
    newNode->Degree = 0;
    newNode->Processed = 0;
    vector V;
    vector_init(&V);
    newNode->Vertices = V;
    return newNode;
}

void printNode(node_p node) {
    printf("ID: %d, Degree: %d, Processed: %d, Vertices: ", node->ID, node->Degree, node->Processed);
    for (int i = 0; i < vector_count(&node->Vertices); i++) {
        printf("%d ", ((node_p)vector_get(&node->Vertices, i))->ID);
    }
    printf("\n");
}

void addEdge(node_t *src, node_t *dst) {
    printf("Adding edge in the graph from : %d to %d\n",src->ID, dst->ID);
    dst->Degree++;
    vector_add(&src->Vertices, dst);
}

// Remove from src the vertex with ID = dst->ID;
void removeEdge(node_p src, node_t *dst) {
    for (int i = 0; i < vector_count(&src->Vertices); i++) {
        if (((node_p)vector_get(&src->Vertices, i))->ID == dst->ID) {
            dst->Degree--;
            vector_delete(&src->Vertices, i);
            break;
        }
    }
}

