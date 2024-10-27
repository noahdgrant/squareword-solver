#include <stdio.h>
#include <string.h>

#include "json.h"
#include "logger.h"
#include "solver.h"

#define BUFFER_SIZE 1024 // number of characters to read from the file

int json_parse(char game_board[GRID_SIZE][GRID_SIZE],
               char unplaced_letters[GRID_SIZE][GRID_SIZE],
               char unused_letters[NUM_LETTERS], int* unused_count) {

    FILE *file = fopen("input.json", "r");
    if (!file) {
        logger(ERROR, __func__, "Failed to open input.json");
        return 1;
    }

    char buffer[BUFFER_SIZE];
    fread(buffer, sizeof(char), BUFFER_SIZE, file);
    fclose(file);

    logger(DEBUG, __func__, "File contents...\n%s", buffer);

    // Parsing the game board
    char *ptr = strstr(buffer, "\"game_board\"");
    ptr = strchr(ptr, '[');
    for (int i = 0; i < GRID_SIZE; i++) {
        ptr = strchr(ptr, '[');
        for (int j = 0; j < GRID_SIZE; j++) {
            ptr = strchr(ptr, '\"') + 1;
            game_board[i][j] = *ptr;
            ptr = strchr(ptr, '\"') + 1;
        }
        ptr = strchr(ptr, ']');
    }

    // Parsing the unplaced letters
    ptr = strstr(buffer, "\"unplaced_letters\"");
    ptr = strchr(ptr, '[');
    for (int i = 0; i < GRID_SIZE; i++) {
        ptr = strchr(ptr, '[');
        for (int j = 0; j < GRID_SIZE; j++) {
            ptr = strchr(ptr, '\"') + 1;
            unplaced_letters[i][j] = *ptr;
            ptr = strchr(ptr, '\"') + 1;
        }
        ptr = strchr(ptr, ']');
    }

    // Parsing the unused letters
    ptr = strstr(buffer, "\"unused_letters\"");
    ptr = strchr(ptr, '[');
    *unused_count = 0;
    while ((ptr = strchr(ptr, '\"')) != NULL && *ptr != ']') {
        ptr++;
        unused_letters[*unused_count] = *ptr;
        (*unused_count)++;
        ptr = strchr(ptr, '\"') + 1;
    }

    return 0;
}
