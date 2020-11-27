#include "file.h"

int get_num_nodes(FILE *fp){
    if (fp == NULL){
        printf("Could not read from invalid file pointer");
        return 0;
    }
    char line[16];
    fscanf(fp, "%[^\n]", line);
    return atoi(line);
}

int parse_input_file(int* num_nodes, int* source , int* destination) {
    FILE *fp;
    fp = fopen("/Users/savigoyal/CLionProjects/Kahn/graph.txt", "r");
    if (fp == NULL){
        printf("Could not open file %s", "graph.txt");
        return 0;
    }
    *num_nodes = get_num_nodes(fp);
    printf("Number of nodes in the file: %d\n", *num_nodes);
    int index = 0;
    while (fscanf(fp,"%d,%d\n",&source[index], &destination[index])!=EOF){
        printf("Source Node: %d\tDestination Node: %d\n", source[index], destination[index]);
        index++;
    }
    fclose(fp);
    return index;
}
