#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WORD_LENGTH 6 // 1 space for null terminator
#define WORD_COUNT 14856
#define NUM_CHARS 5

void test();

int get_wordle_words(char words[WORD_COUNT][WORD_LENGTH]) {
    FILE *file;
    int count = 0;
    char line[WORD_LENGTH + 1]; // +1 for \n
    printf("Getting wordle words...\n");

    // Open the file in read mode
    file = fopen("wordle-words.txt", "r");
    printf("Opened file\n");
    if (file == NULL) {
        printf("Could not open file\n");
        return 1;
    }

    printf("Reading words...\n");
    // Read each line from the file and print it
    while (fgets(line, sizeof(line), file)) {
        line[WORD_LENGTH - 1] = '\0'; // Remove newline character
        printf("Line (%d): %s\n", count + 1, line);
        count++;
    }

    printf("Got words\n");

    // Close the file
    fclose(file);

    return 0;
}

int solve(char board[NUM_CHARS][NUM_CHARS],
          char unplaced[NUM_CHARS][NUM_CHARS],
          char solution[NUM_CHARS][NUM_CHARS],
          char wordle_words[WORD_COUNT][WORD_LENGTH]) {

    printf("%s", wordle_words[WORD_COUNT - 1]);

    return 0;
}

int main() {
    int err_code = 0;
    char wordle_words[WORD_COUNT][WORD_LENGTH];

    // the game board (green)
    char board[NUM_CHARS][NUM_CHARS] = {
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    };

    // the characters that go in the words but you don't know where (yellow)
    char unplaced[NUM_CHARS][NUM_CHARS] = {
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    };

    // the final solution
    char solution[NUM_CHARS][NUM_CHARS] = {
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    };

    printf("Hello from main\n");

    err_code = get_wordle_words(wordle_words);
    if (err_code == 1) {
        return 1;
    }

    err_code = solve(board, unplaced, solution, wordle_words);
    if (err_code == 1) {
        return 1;
    }

    return 0;
}

void test() {
    char board[NUM_CHARS][NUM_CHARS] = {
    {'.','.','.','.','.'},
    {'.','.','.','.','t'},
    {'.','.','.','.','e'},
    {'.','r','.','.','e'},
    {'.','o','.','.','.'},
    };

    char unplaced[NUM_CHARS][NUM_CHARS] = {
    {'a','i','s','t','.'},
    {'i','u','n','.','.'},
    {'s','o','t','.','.'},
    {'e','o','.','.','.'},
    {'e','t','.','.','.'},
    };

    char solution[NUM_CHARS][NUM_CHARS] = {
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    {'.','.','.','.','.'},
    };

    char final_solution[NUM_CHARS][NUM_CHARS] = {
    {'f','i','a','t','s'},
    {'u','n','f','i','t'},
    {'s','t','o','l','e'},
    {'e','r','o','d','e'},
    {'d','o','t','e','d'},
    };
}
