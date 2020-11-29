#include "kahn.h"

/** Run serially on the main thread. */
void init_graph(vector *nodes, int num_nodes, int num_edges, int *source, int *destination) {
    // Create the graph
    for (int i = 0; i < num_nodes; i++) {
        node_p node = createNode(i + 1);
        vector_add(nodes, node);
    }
    // Add the edges to the graph
    for (int i = 0; i < num_edges; i++) {
        addEdge(vector_get(nodes, source[i] - 1), vector_get(nodes, destination[i] - 1));
    }
}

/** Thread safety needed for parallel processing. */
void fill_queue(vector *input_nodes, vector *unprocessed_nodes) {
    // Fill unprocessed_nodes with the nodes without incoming edges
    for (int i = 0; i < vector_count(input_nodes); i++) {
        node_p v = (node_p) vector_get(input_nodes, i);
        if (v->Degree == 0 && v->Processed == 0) {
            v->Processed = 1;
            vector_add(unprocessed_nodes, v);
        }
    }
}

/** Thread safety needed for parallel processing. */
void process_nodes_serial(vector *unprocessed_nodes, vector *output) {
    printf("Processing the nodes serially\n");
    for (int unprocessed_node_index = 0;
         unprocessed_node_index < vector_count(unprocessed_nodes); unprocessed_node_index++) {
        // Remove a node n from unprocessed_nodes
        node_p next_node = vector_get(unprocessed_nodes, unprocessed_node_index);
        if (next_node == NULL) continue;
        vector_delete(unprocessed_nodes, 0);
        // Add node to the output
        vector_add(output, next_node);
        // For each node m with an edge e from n
        for (int neighbor_index = 0; neighbor_index < vector_count(&next_node->Vertices); neighbor_index++){
            node_p m = ((node_p) vector_get(&next_node->Vertices, neighbor_index));
            // Remove edge e from the graph
            removeEdge(next_node, vector_get(&next_node->Vertices, neighbor_index));
            // If m has no other incoming edges insert m into unprocessed_nodes
            if (m->Degree == 0) {
                m->Processed = 1;
                vector_add(unprocessed_nodes, m);
            }
        }
    }
}

/** Thread safety needed for parallel processing. */
void process_nodes_parallel(vector *unprocessed_nodes, vector *output) {
    printf("Processing the nodes in parallel\n");
    for (int unprocessed_node_index = 0;
         unprocessed_node_index < vector_count(unprocessed_nodes); unprocessed_node_index++) {
        // Remove a node n from unprocessed_nodes
        node_p next_node = vector_get(unprocessed_nodes, unprocessed_node_index);
        if (next_node == NULL) continue;
        vector_delete(unprocessed_nodes, 0);
        // Add node to the output
        vector_add(output, next_node);
        // For each node m with an edge e from n
        for (int neighbor_index = 0; neighbor_index < vector_count(&next_node->Vertices); neighbor_index++){
            node_p m = ((node_p) vector_get(&next_node->Vertices, neighbor_index));
            // Remove edge e from the graph
            removeEdge(next_node, vector_get(&next_node->Vertices, neighbor_index));
            // If m has no other incoming edges insert m into unprocessed_nodes
            if (m->Degree == 0) {
                m->Processed = 1;
                vector_add(unprocessed_nodes, m);
            }
        }
    }
}

int topological_sort(vector *nodes, int num_nodes, bool process_parallel) {
    // Run the Kahn algorithm
    // Empty list that will contain the sorted elements
    printf("Topologically sorting the %d nodes\n", nodes->count);
    vector sorted_nodes;
    vector_init(&sorted_nodes);

    // Set of pointers to all nodes with no incoming edges
    vector unprocessed_nodes;
    vector_init(&unprocessed_nodes);

    // Fill the queue with nodes which have no incoming edges
    fill_queue(nodes, &unprocessed_nodes);

    // Process the unprocessed nodes in the queue
    if (process_parallel){
        process_nodes_parallel(&unprocessed_nodes, &sorted_nodes);
    }
    else{
        process_nodes_serial(&unprocessed_nodes, &sorted_nodes);
    }
    // If graph still has edges
    for (int i = 0; i < num_nodes; i++) {
        if (!((node_p) vector_get(nodes, i))->Processed) {
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
    int num_edges = parse_input_file(&num_nodes, source, destination);
    printf("Number of nodes: %d\n", num_nodes);
    printf("Number of edges: %d\n", num_edges);
    vector nodes;
    vector_init(&nodes);
    init_graph(&nodes, num_nodes, num_edges, source, destination);

    bool process_parallel = false;
    topological_sort(&nodes, num_nodes, process_parallel);
    return 0;
}