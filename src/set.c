#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "set.h"

// Initialize the set
void set_init(Set *set, ElementType type) {
    set->type = type;
    set->size = 0;
}

// Print the set
void set_print(Set *set) {
    fprintf(stderr, "{");
    for (int i = 0; i < set->size; i++) {
        if (set->type == CHAR_TYPE) {
            if (i < set->size - 1) {
                fprintf(stderr, "%c, ", set->elements[i].char_element);
            } else {
                fprintf(stderr, "c");
            }
        } else if (set->type == STRING_TYPE) {
            if (i < set->size - 1) {
                fprintf(stderr, "%s, ", set->elements[i].string_element);
            } else {
                fprintf(stderr, "%s", set->elements[i].string_element);
            }
        }
    }
    fprintf(stderr, "}\n");
}

// Check if a character is in the set
bool is_char_in_set(Set *set, char char_element) {
    for (int i = 0; i < set->size; i++) {
        if (set->type == CHAR_TYPE && set->elements[i].char_element == char_element) {
            return true;
        }
    }
    return false;
}

// Check if a string is in the set
bool is_string_in_set(Set *set, const char *string_element) {
    for (int i = 0; i < set->size; i++) {
        if (set->type == STRING_TYPE && strcmp(set->elements[i].string_element, string_element) == 0) {
            return true;
        }
    }
    return false;
}

// Add a character to the set
void add_char_to_set(Set *set, char char_element) {
    if (set->type == CHAR_TYPE && !is_char_in_set(set, char_element) && set->size < SET_MAX_ELEMENTS) {
        set->elements[set->size++].char_element = char_element;
    }
}

// Add a string to the set
void add_string_to_set(Set *set, const char *string_element) {
    if (set->type == STRING_TYPE && !is_string_in_set(set, string_element) && set->size < SET_MAX_ELEMENTS) {
        strncpy(set->elements[set->size].string_element, string_element, MAX_STRING_LENGTH);
    }
}
