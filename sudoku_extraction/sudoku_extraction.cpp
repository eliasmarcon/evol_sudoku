#include <iostream>
#include <fstream>
#include <vector>
#include <cstring> // Include the header for memset

const int SUDOKU_SIZE = 9;

// Load Sudoku into a 2D vector
void loadSudoku(std::vector<std::vector<int>>& sudoku, int puzzleNumber) {
    // Open the file
    std::ifstream file("sudoku_solutions.txt");

    // Check if the file is open
    if (!file.is_open()) {
        std::cerr << "Error opening the file.\n";
        return;
    }

    std::string line;

    // Move to the desired puzzle number
    while (std::getline(file, line) && puzzleNumber > 1) {
        --puzzleNumber;
    }

    std::cout << line << std::endl;

    // Resize the 2D vector
    sudoku.resize(SUDOKU_SIZE, std::vector<int>(SUDOKU_SIZE, 0));

    // Read the puzzle line
    for (int i = 0; i < SUDOKU_SIZE; ++i) {
        for (int j = 0; j < SUDOKU_SIZE; ++j) {
            sudoku[i][j] = line[i * SUDOKU_SIZE + j] - '0'; // Convert char to int
        }
    }

    // Close the file
    file.close();
}

// Validate Sudoku
std::string validSudoku(const std::vector<std::vector<int>>& sudoku) {
    int row = 0, col = 0, i = 0, block = 0;
    int count[9];

    for (row = 0; row < 9; ++row) {
        memset(count, 0, 9 * sizeof(int));
        for (col = 0; col < 9; ++col) {
            if (sudoku[row][col] != 0) // Check for non-empty cell
                ++count[sudoku[row][col] - 1];
        }
        for (i = 0; i < 9; ++i)
            if (count[i] > 1)
                return "false";
    }

    for (col = 0; col < 9; ++col) {
        memset(count, 0, 9 * sizeof(int));
        for (row = 0; row < 9; ++row) {
            if (sudoku[row][col] != 0) // Check for non-empty cell
                ++count[sudoku[row][col] - 1];
        }
        for (i = 0; i < 9; ++i)
            if (count[i] > 1)
                return "false";
    }

    int block_row = 0, block_col = 0;
    for (block = 0; block < 9; ++block) {
        block_row = (block / 3) * 3, block_col = (block % 3) * 3;
        memset(count, 0, 9 * sizeof(int));
        for (row = block_row; row < (block_row + 3); ++row)
            for (col = block_col; col < (block_col + 3); ++col)
                if (sudoku[row][col] != 0) // Check for non-empty cell
                    ++count[sudoku[row][col] - 1];
        for (i = 0; i < 9; ++i)
            if (count[i] > 1)
                return "false";
    }

    return "true";
}

int main() {
    // Example: Load Sudoku puzzle number 1 into a vector<vector<int>>
    std::vector<std::vector<int>> sudoku;
    loadSudoku(sudoku, 10);

    // Display the loaded Sudoku puzzle (for testing purposes)
    for (int i = 0; i < SUDOKU_SIZE; ++i) {
        for (int j = 0; j < SUDOKU_SIZE; ++j) {
            std::cout << sudoku[i][j] << ' ';
        }
        std::cout << '\n';
    }

    std::string valid = validSudoku(sudoku);
    std::cout << "Answer to valid Sudoku: " << valid << std::endl;

    return 0;
}
