#include "kahn.h"
#include <time.h>
#include <string.h>

#define MAXTHREADS 100
pthread_mutex_t mutex_unprocessed_queue;
pthread_mutex_t mutex_output_list;


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

/** Run serially on the main thread. */
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

/** Run serially on the main thread. */
void process_nodes_serial(vector *unprocessed_nodes, vector *output) {
    printf("Processing the nodes serially\n");
    for (int unprocessed_node_index = 0;
         unprocessed_node_index < vector_count(unprocessed_nodes); unprocessed_node_index++) {
        // Remove a node n from unprocessed_nodes
        node_p next_node = vector_get(unprocessed_nodes, unprocessed_node_index);
        if (next_node == NULL) continue;
        vector_delete(unprocessed_nodes, unprocessed_node_index);
        // Add node to the output
        vector_add(output, next_node);
        // For each node m with an edge e from n
        for (int neighbor_index = 0; neighbor_index < vector_count(&next_node->Vertices); neighbor_index++) {
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

struct args {
    vector *unprocessed_nodes;
    vector *output;
    int index;
};

/** Thread safety needed for parallel processing. This is done by using mutexes for output queue and unprocessed queue.*/
void *process_node_parallel(void *input) {
    vector *unprocessed_nodes = ((struct args *) input)->unprocessed_nodes;
    vector *output = ((struct args *) input)->output;
    int index = ((struct args *) input)->index;
    node_p next_node = vector_get(unprocessed_nodes, index);
    if (next_node == 0) return 0;
    vector_delete(unprocessed_nodes, index);
    // Add node to the output
    printf("Adding node %d to the output list\n", next_node->ID);
    pthread_mutex_lock(&mutex_output_list);
    vector_add(output, next_node);
    pthread_mutex_unlock(&mutex_output_list);
    // For each node m with an edge e from n
    for (int neighbor_index = 0; neighbor_index < vector_count(&next_node->Vertices); neighbor_index++) {
        node_p m = ((node_p) vector_get(&next_node->Vertices, neighbor_index));
        // Remove edge e from the graph
        removeEdge(next_node, vector_get(&next_node->Vertices, neighbor_index));
        // If m has no other incoming edges insert m into unprocessed_nodes
        if (m->Degree == 0) {
            m->Processed = 1;
            pthread_mutex_lock(&mutex_unprocessed_queue);
            printf("Adding node %d to the unprocessed queue\n", m->ID);
            vector_add(unprocessed_nodes, m);
            pthread_mutex_unlock(&mutex_unprocessed_queue);
        }
    }
    return 0;
}

/** Runs a new thread for each unprocessed node.*/
void process_nodes_parallel(vector *unprocessed_nodes, vector *output, int iteration) {
    printf("Processing the nodes in parallel for iteration: %d\n", iteration);
    pthread_t *thread_handles;
    thread_handles = malloc(MAXTHREADS * sizeof(pthread_t));
    int unprocessed_node_count = vector_count(unprocessed_nodes);
    for (int unprocessed_node_index = 0;
         unprocessed_node_index < unprocessed_node_count; unprocessed_node_index++) {
        struct args *input = (struct args *) malloc(sizeof(struct args));
        input->index = unprocessed_node_index;
        input->unprocessed_nodes = unprocessed_nodes;
        input->output = output;
        pthread_create(&thread_handles[unprocessed_node_index], NULL, process_node_parallel, (void *) input);
    }
    for (int unprocessed_node_index = 0;
         unprocessed_node_index < unprocessed_node_count; unprocessed_node_index++) {
        pthread_join(thread_handles[unprocessed_node_index], NULL);
    }
    free(thread_handles);
    vector_reinit(unprocessed_nodes);
    if (vector_count(unprocessed_nodes) > 0) process_nodes_parallel(unprocessed_nodes, output, iteration + 1);
}

/** Sort the input nodes based on their dependency relationship.*/
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
    if (process_parallel) {
        process_nodes_parallel(&unprocessed_nodes, &sorted_nodes, 1);
    } else {
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


int main(int argc, char *argv[]) {
    int source[10000];
    int destination[10000];
    int num_nodes = 0;
    int num_edges = parse_input_file(&num_nodes, source, destination);
    printf("Number of nodes: %d\n", num_nodes);
    printf("Number of edges: %d\n", num_edges);
    vector nodes;
    vector_init(&nodes);
    init_graph(&nodes, num_nodes, num_edges, source, destination);
    bool process_parallel = false;
    if ((argc >= 2) && (strcmp(argv[1],"parallel"))==0) {
        printf("Processing the topological sort in parallel\n");
        process_parallel = true;
    } else {
        printf("Processing the topological sort serially\n");
    }
    pthread_mutex_init(&mutex_unprocessed_queue, NULL);
    pthread_mutex_init(&mutex_output_list, NULL);
    clock_t begin = clock();
    topological_sort(&nodes, num_nodes, process_parallel);
    clock_t end = clock();
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("Time taken to perform topological sort: %f secs\n", time_spent);
    pthread_mutex_destroy(&mutex_unprocessed_queue);
    pthread_mutex_destroy(&mutex_output_list);
    return 0;
}