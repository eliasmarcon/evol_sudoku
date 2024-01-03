#!/bin/bash

# Set the name of the C++ source file
cpp_file="./src/sudoku_creator.cpp"

# Set the name of the output executable file
output_executable="./out/sudoku_creator"

# Set the output file
output_file="./sudoku_creator_output.txt"

# Set the saving files
sudoku_solutions_file="../sudoku_solutions.txt"

# clear the output file
> "$output_file"

# Compile the C++ program
g++ -o "$output_executable" "$cpp_file" -lga -fpermissive -Ofast

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation of Sudoku creator was successful. Running the program..."

    # Run the compiled program and append the output to the file
    "$output_executable" "$sudoku_solutions_file" >> "$output_file"

    echo "Results saved into $output_file."
else
    echo "Compilation failed. Please check for errors in your C++ code."
fi