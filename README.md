# Squareword Solver

This project is a solver for the [Squareword](https://squareword.org/) game. The
solver uses a backtracking algorithm to find all possible solutions given a
starting point.

## Setup

To setup the solver open `src/main.c`. A blank space is represented by a `'.'`. In `board[][]`, place the **green** letters as they appear on the game board. In `unplaced[][]`, place the **yellow** letters for each column (the order does not matter). In `unused[]`, place any **gray** letters (if there are no gray letters, leave it empty by putting a `'.'`). Once this is done, you can build and run the solver.

## Build

Build the project with...

```bash
$ make
```

## Run

Run the binary from the project root with...

```bash
$ ./bin/main
```

## Test

Run the tests with...

```bash
$ make test
```

## Clean

Clean old build files with...

```bash
$ make clean
```

## Example Boards with Intended Solutions

### Solution 1

```c
char board[GRID_SIZE][GRID_SIZE] = {
{'.','.','.','.','.'},
{'.','.','.','.','t'},
{'.','.','.','.','e'},
{'.','r','.','.','e'},
{'.','o','.','.','.'},
};

char unplaced[GRID_SIZE][GRID_SIZE] = {
{'a','i','s','t','.'},
{'i','u','n','.','.'},
{'s','o','t','.','.'},
{'e','o','.','.','.'},
{'e','t','.','.','.'},
};

char unused[] = {'.'};

// This is the intended answer
final_solution = {
{'f','i','a','t','s'},
{'u','n','f','i','t'},
{'s','t','o','l','e'},
{'e','r','o','d','e'},
{'d','o','t','e','d'},
};
```

### Solution 2

```c
char board[GRID_SIZE][GRID_SIZE] = {
{'.','.','.','.','.'},
{'.','.','.','.','.'},
{'.','.','u','.','e'},
{'a','.','.','.','e'},
{'.','r','.','.','.'},
};

char unplaced[GRID_SIZE][GRID_SIZE] = {
{'a','s','m','.','.'},
{'a','e','.','.','.'},
{'e','.','.','.','.'},
{'e','i','r','.','.'},
{'a','e','.','.','.'},
};

char unused[] = {'t', 'o', 'n'};

// This is the intended answer
final_solution = {
{'s','p','a','m','s'},
{'h','a','z','e','l'},
{'e','l','u','d','e'},
{'a','e','r','i','e'},
{'f','r','e','a','k'},
};
```
