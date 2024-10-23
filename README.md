# Squareword Solver

This project is a solver for the [Squareword](https://squareword.org/) game. The
solver uses a backtracking algorithm to find all possible solutions given a
starting point.

## Setup

To setup the solver open `src/main.c`. A black space is represented by a `'.'`. In `board[][]`, place the green letters as they appear on the game board. In `unplaced[][]`, place the yellow letters for each column (the order does not matter). In `unused[]`, place any gray letters (if there are no gray letters, leave it empty by putting a `'.'`). Once this is done, you can build and run the solver.

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
