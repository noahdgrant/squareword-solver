#ifndef JSON_H
#define JSON_H

#include "solver.h"

int json_parse(const char *filename, char game_board[GRID_SIZE][GRID_SIZE],
                char unplaced_letters[GRID_SIZE][GRID_SIZE],
                char unused_letters[NUM_LETTERS], int* unused_count);

#endif