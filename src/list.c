#include <stdio.h>

#include "list.h"

// Initialize the list
void init_list(List *list) {
    list->size = 0;
}

// Add an element to the list
void list_add(List *list, char element) {
    if (list->size < LIST_SIZE) {
        list->elements[list->size++] = element;
    }
}

// Print the list
void list_print(List *list) {
    fprintf(stderr, "[");
    for (int i = 0; i < list->size; i++) {
        if (i < list->size - 1) {
            fprintf(stderr, "%c, ", list->elements[i]);
        } else {
            fprintf(stderr, "%c", list->elements[i]);
        }
    }
    fprintf(stderr, "]\n");
}

// Check if an element is in the list
int is_in_list(List *list, char element) {
    for (int i = 0; i < list->size; i++) {
        if (list->elements[i] == element) {
            return 1;
        }
    }
    return 0;
}
