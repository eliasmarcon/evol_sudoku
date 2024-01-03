#!/bin/bash

# Set the name of the C++ source file
cpp_file="./src/sudoku_creator.cpp"

# Set the name of the output executable file
output_executable="./out/sudoku_creator"

# Set the output file
output_file="./sudoku_creator_output.txt"

# clear the output file
> "$output_file"

# Compile the C++ program
g++ -o "$output_executable" "$cpp_file" -lga -fpermissive -Ofast

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Running the program..."

    # Run the compiled program and append the output to the file
    "$output_executable" >> "$output_file"

    echo "Sudoku was saved into $output_file and the sudoku_solutions.txt file."

else
    echo "Compilation failed. Please check for errors in your C++ code."
fi