#include "kahn.h"


void init_graph(vector* nodes, int num_nodes, int num_edges, int* source, int* destination){
    // Create the graph
    for (int i = 0; i < num_nodes; i++) {
        node_p node = createNode(i + 1);
        vector_add(nodes, node);
    }
    // Add the edges to the graph
    for (int i = 0; i < num_edges; i++) {
        addEdge(vector_get(nodes, source[i]-1), vector_get(nodes, destination[i]-1));
    }
}

int topological_sort_serial(vector* nodes, int num_nodes) {
    // Run the Kahn algorithm
    // Empty list that will contain the sorted elements
    printf("Topologically sorting the %d nodes serially\n", nodes->count);
    vector L;
    vector_init(&L);
    // Set of pointers to all nodes with no incoming edges
    vector S;
    vector_init(&S);
    // Fill S with the nodes without incoming edges
    for (int i = 0; i < vector_count(nodes); i++) {
        node_p v = (node_p)vector_get(nodes, i);
        if (v->Degree == 0 && v->Processed == 0) {
            v->Processed = 1;
            vector_add(&S, v);
        }
    }

    while (vector_count(&S)) {
        // Remove a node n from S
        node_p n = vector_get(&S, 0);
        vector_delete(&S, 0);
        // Add n to tail of L
        vector_add(&L, n);

        // For each node m with an edge e from n
        while (vector_count(&n->Vertices)) {
            node_p m = ((node_p)vector_get(&n->Vertices, 0));
            // Remove edge e from the graph
            removeEdge(n, vector_get(&n->Vertices, 0));
            // If m has no other incoming edges insert m into S
            if (m->Degree == 0) {
                m->Processed = 1;
                vector_add(&S, m);
            }
        }
    }

    // If graph still has edges
    for (int i = 0; i < num_nodes; i++) {
        if (!((node_p)vector_get(nodes, i))->Processed) {
            printf("Could not run algorithm as graph has at least one cycle.");
            return 128;
        }
    }

    printf("\nPrinting the topologically sorted order\n");
    for (int i = 0; i < vector_count(&L); i++) {
        printNode(vector_get(&L, i));
    }
    return 0;
}

int main() {
    int source[10];
    int destination[10];
    int num_nodes = 0;
    int num_edges  = parse_input_file(&num_nodes, source, destination);
    printf("Number of nodes: %d\n", num_nodes);
    printf("Number of edges: %d\n", num_edges);
    vector nodes;
    vector_init(&nodes);
    init_graph(&nodes,num_nodes,num_edges,source, destination);
    topological_sort_serial(&nodes, num_nodes);
    return 0;
}