#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

typedef struct {
    bool solve;
    bool find_minimum;
} Args;

void print_usage(const char *program_name);
Args parse_args(int argc, char* argv[]);

#endif
