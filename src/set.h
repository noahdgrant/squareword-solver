#ifndef SET_H
#define SET_H

#include <stdbool.h>
#include <stdlib.h>

#include "solver.h"

#define SET_MAX_ELEMENTS GRID_SIZE
#define MAX_STRING_LENGTH WORD_LENGTH

typedef enum {
    CHAR_TYPE,
    STRING_TYPE
} ElementType;

typedef union {
    char char_element;
    char* string_element;
} Element;

typedef struct {
    Element elements[SET_MAX_ELEMENTS];
    ElementType type;
    int size;
} Set;

#define set_add(set, T) _Generic((T), \
    char: add_char_to_set, \
    const char*: add_string_to_set \
)(set, T)

#define is_in_set(set, T) _Generic((T), \
    char: is_char_in_set, \
    const char*: is_string_in_set \
)(set, T)

void set_init(Set *set, ElementType type);
void set_print(Set *set);

bool is_char_in_set(Set *set, char char_element);
void add_char_to_set(Set *set, char char_element);

bool is_string_in_set(Set *set, const char *string_element);
void add_string_to_set(Set *set, const char *string_element);

#endif
