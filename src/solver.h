#ifndef SOLVER_H
#define SOLVER_H

#define GRID_SIZE 5
#define WORD_LENGTH 6
#define MAX_WORD_COUNT 12036 // number of words in words.txt

int compare_arrays(char arr1[GRID_SIZE][GRID_SIZE], char arr2[GRID_SIZE][GRID_SIZE]);
void print_grid(char grid[GRID_SIZE][GRID_SIZE]);
int fits_in_row(char solution[GRID_SIZE][GRID_SIZE], char unplaced[GRID_SIZE][GRID_SIZE],
                char word[], int row);
int fits_in_col(char solution[GRID_SIZE][GRID_SIZE], char word[], int col);
void place_word(char solution[GRID_SIZE][GRID_SIZE], char word[], int row);
void remove_word(char board[GRID_SIZE][GRID_SIZE], char solution[GRID_SIZE][GRID_SIZE], int row);

int solver(char board[GRID_SIZE][GRID_SIZE], char unplaced[GRID_SIZE][GRID_SIZE],
           char unused[], int unused_length, char solution[GRID_SIZE][GRID_SIZE],
           char words[MAX_WORD_COUNT][WORD_LENGTH]);

#endif
