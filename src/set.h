#ifndef SET_H
#define SET_H

#include <stdbool.h>
#include <stdlib.h>

#include "solver.h"

#define SET_MAX_ELEMENTS MAX_WORD_COUNT
#define MAX_STRING_LENGTH WORD_LENGTH

typedef enum {
    CHAR_TYPE,
    STRING_TYPE
} ElementType;

typedef union {
    char char_element;
    char string_element[MAX_STRING_LENGTH];
} Element;

typedef struct {
    Element elements[SET_MAX_ELEMENTS];
    ElementType type;
    int size;
} Set;

#define set_add(set, T) _Generic((T), \
    char: add_char_to_set, \
    char*: add_string_to_set \
)(set, T)

#define set_remove(set, T) _Generic((T), \
    char: remove_char_from_set, \
    char*: remove_char_from_set \
)(set, T)

#define is_in_set(set, T) _Generic((T), \
    char: is_char_in_set, \
    char*: is_string_in_set \
)(set, T)

// Generic functions
void set_init(Set *set, ElementType type);
void set_print(Set *set);

// Char set functions
bool is_char_in_set(Set *set, char char_element);
void add_char_to_set(Set *set, char char_element);
void remove_char_from_set(Set *set, char char_element);

// String set functions
bool is_string_in_set(Set *set, const char *string_element);
void add_string_to_set(Set *set, const char *string_element);
void remove_string_from_set(Set *set, const char *string_element);

#endif
