#!/bin/bash

# Check for the number of tasks and diameter arguments
if [ "$#" -gt 2 ]; then
    echo "Usage: $0 [<sudokuNumber>] [<numbersToRemove>]"
    exit 1
fi

# Set default values if arguments are not provided
sudokuNumber=${1:-1}
numbersToRemove=${2:-4}

# set variables
sudokuSolutions="sudoku_solutions.txt"
executable="./out/sudoku_solver"
cpp_file="./src/sudoku_solver.cpp"

# read the amount of lines in the file
lineCount=$(wc -l < "$sudokuSolutions")

if [ "$sudokuNumber" -lt 1 ] || [ "$sudokuNumber" -gt "$lineCount" ]; then
    echo "The sudoku number has to be between 1 and $lineCount"
    exit 1
fi

# check if the numbers to remove are valid has to be between 1 and 15
if [ "$numbersToRemove" -lt 1 ] || [ "$numbersToRemove" -gt 15 ]; then
    echo "The numbers to remove has to be between 1 and 15"
    exit 1
fi


# Compile the C++ file
g++ -o $executable $cpp_file -lga -fpermissive

# Running the executable
echo "Running the sudoku solver..."
$executable $sudokuNumber $numbersToRemove

