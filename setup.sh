#!/bin/bash

echo "Setting up project..."

# Installed requirements
echo "Installing depencencies..."

sudo apt install build-essential

# Setup input file
echo "Creating input file..."

echo '{
    "game_board": [
        [".", ".", ".", ".", "."],
        [".", ".", ".", ".", "."],
        [".", ".", ".", ".", "."],
        [".", ".", ".", ".", "."],
        [".", ".", ".", ".", "."]
    ],
    "unplaced_letters": [
        [".", ".", ".", ".", "."],
        [".", ".", ".", ".", "."],
        [".", ".", ".", ".", "."],
        [".", ".", ".", ".", "."],
        [".", ".", ".", ".", "."]
    ],
    "unused_letters": ["."]
}' > "input.json"

echo "Setup finished"
