#include <stdio.h>
#include <string.h>

#include "json.h"
#include "logger.h"
#include "solver.h"

#define BUFFER_SIZE 4096 // number of characters to read from the file

int json_parse(char game_board[GRID_SIZE][GRID_SIZE],
               char unplaced_letters[GRID_SIZE][GRID_SIZE],
               char unused_letters[NUM_LETTERS], int* unused_count,
               char columns[GRID_SIZE][GRID_SIZE]) {

    FILE *file = fopen("input.json", "r");
    if (!file) {
        logger(ERROR, "Failed to open input.json");
        return 1;
    }

    char buffer[BUFFER_SIZE];
    fread(buffer, sizeof(char), BUFFER_SIZE, file);
    fclose(file);

    logger(DEBUG, "File contents...\n%s", buffer);

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

    if (logger_get_level() == DEBUG) {
        logger(DEBUG, "Game board...");
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                fprintf(stderr, "%c", game_board[i][j]);
            }
            fprintf(stderr, "\n");
        }
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

    if (logger_get_level() == DEBUG) {
        logger(DEBUG, "Unplaced letters...");
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                fprintf(stderr, "%c", unplaced_letters[i][j]);
            }
            fprintf(stderr, "\n");
        }
    }

    // Parsing the unused letters
    ptr = strstr(buffer, "\"unused_letters\"");
    ptr = strchr(ptr, '[');
    *unused_count = 0;
    while (*ptr != ']' && (ptr = strchr(ptr, '\"')) != NULL) {
        ptr++;
        unused_letters[*unused_count] = *ptr;
        (*unused_count)++;
        ptr = strchr(ptr, '\"') + 1;
    }

    if (logger_get_level() == DEBUG) {
        logger(DEBUG, "Unused letters...");
        for (int i = 0; i < *unused_count; i++) {
            fprintf(stderr, "%c", unused_letters[i]);
        }
        fprintf(stderr, "\n");
    }

    // Parsing the columns
    ptr = strstr(buffer, "\"columns\"");
    ptr = strchr(ptr, '[');
    for (int i = 0; i < GRID_SIZE; i++) {
        ptr = strchr(ptr, '[');
        for (int j = 0; j < GRID_SIZE; j++) {
            ptr = strchr(ptr, '\"') + 1;
            columns[i][j] = *ptr;
            ptr = strchr(ptr, '\"') + 1;
        }
        ptr = strchr(ptr, ']');
    }

    if (logger_get_level() == DEBUG) {
        logger(DEBUG, "Columns...");
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                fprintf(stderr, "%c", columns[i][j]);
            }
            fprintf(stderr, "\n");
        }
    }

    return 0;
}
