//
// Created by Savi Goyal on 11/25/20.
//

#ifndef KAHN_VECTOR_H
#define KAHN_VECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct vector_ {
    void **data;
    int size;
    int count;
} vector;

void vector_init(vector *);
int vector_count(vector *);
void vector_add(vector *, void *);
void vector_set(vector *, int, void *);
void *vector_get(vector *, int);
void vector_delete(vector *, int);
void vector_free(vector *);


#endif //KAHN_VECTOR_H
