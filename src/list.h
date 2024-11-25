#ifndef LIST_H
#define LIST_H

#include "solver.h"

#define LIST_SIZE GRID_SIZE

typedef struct {
    char elements[LIST_SIZE];
    int size;
} List;

void list_init(List *list);
void list_add(List *list, char element);
void list_print(List *list);
int is_in_list(List *list, char element);

#endif
