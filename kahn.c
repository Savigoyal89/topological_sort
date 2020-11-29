#include "kahn.h"

/** Run serially on the main thread. */
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

/** Thread safety needed for parallel processing. */
void fill_queue(vector* input_nodes, vector* unprocessed_nodes){
    // Fill unprocessed_nodes with the nodes without incoming edges
    for (int i = 0; i < vector_count(input_nodes); i++) {
        node_p v = (node_p)vector_get(input_nodes, i);
        if (v->Degree == 0 && v->Processed == 0) {
            v->Processed = 1;
            vector_add(unprocessed_nodes, v);
        }
    }
}

/** Thread safety needed for parallel processing. */
void process_nodes(vector* unprocessed_nodes, vector* output){
    while (vector_count(unprocessed_nodes)) {
        // Remove a node n from unprocessed_nodes
        node_p n = vector_get(unprocessed_nodes, 0);
        vector_delete(unprocessed_nodes, 0);
        vector_reinit(unprocessed_nodes);

        // Add node to the output
        vector_add(output, n);

        // For each node m with an edge e from n
        while (vector_count(&n->Vertices)) {
            node_p m = ((node_p)vector_get(&n->Vertices, 0));
            // Remove edge e from the graph
            removeEdge(n, vector_get(&n->Vertices, 0));
            // If m has no other incoming edges insert m into unprocessed_nodes
            if (m->Degree == 0) {
                m->Processed = 1;
                vector_add(unprocessed_nodes, m);
            }
        }
    }

}

int topological_sort_serial(vector* nodes, int num_nodes) {
    // Run the Kahn algorithm
    // Empty list that will contain the sorted elements
    printf("Topologically sorting the %d nodes serially\n", nodes->count);
    vector sorted_nodes;
    vector_init(&sorted_nodes);

    // Set of pointers to all nodes with no incoming edges
    vector unprocessed_nodes;
    vector_init(&unprocessed_nodes);

    // Fill the queue with nodes which have no incoming edges
    fill_queue(nodes, &unprocessed_nodes);

    // Process the unprocessed nodes in the queue
    process_nodes(&unprocessed_nodes,&sorted_nodes);

    // If graph still has edges
    for (int i = 0; i < num_nodes; i++) {
        if (!((node_p)vector_get(nodes, i))->Processed) {
            printf("Could not run algorithm as graph has at least one cycle.");
            return 128;
        }
    }

    printf("\nPrinting the topologically sorted order\n");
    for (int i = 0; i < vector_count(&sorted_nodes); i++) {
        printNode(vector_get(&sorted_nodes, i));
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