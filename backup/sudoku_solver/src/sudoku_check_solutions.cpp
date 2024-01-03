#include <iostream>
using namespace std;

const int N = 9;

int grid[N][N] = {{5, 0, 0, 0, 7, 0, 0, 0, 0},
                  {6, 0, 0, 1, 0, 5, 0, 0, 0},
                  {0, 0, 8, 0, 0, 0, 0, 6, 0},
                  {8, 0, 0, 0, 0, 0, 0, 0, 3},
                  {4, 0, 0, 8, 0, 0, 0, 0, 1},
                  {7, 0, 0, 0, 0, 0, 0, 0, 0},
                  {0, 6, 0, 0, 0, 0, 0, 0, 0},
                  {0, 0, 0, 4, 0, 0, 0, 0, 0},
                  {0, 0, 0, 0, 0, 0, 0, 0, 0}};

/*
int grid[N][N] = {{5, 3, 0, 0, 7, 0, 0, 0, 0},
                  {6, 0, 0, 1, 9, 5, 0, 0, 0},
                  {0, 9, 8, 0, 0, 0, 0, 6, 0},
                  {8, 0, 0, 0, 6, 0, 0, 0, 3},
                  {4, 0, 0, 8, 0, 3, 0, 0, 1},
                  {7, 0, 0, 0, 2, 0, 0, 0, 6},
                  {0, 6, 0, 0, 0, 0, 2, 8, 0},
                  {0, 0, 0, 4, 1, 9, 0, 0, 5},
                  {0, 0, 0, 0, 8, 0, 0, 7, 9}};      
*/

void sudokuGrid() {
    for (int row = 0; row < N; row++) {
        for (int col = 0; col < N; col++) {
            if (col == 3 || col == 6)
                cout << " | ";
            cout << grid[row][col] << " ";
        }
        if (row == 2 || row == 5) {
            cout << endl;
            for (int i = 0; i < N; i++)
                cout << "---";
        }
        cout << endl;
    }
}

bool isPresentInCol(int col, int num) {
    for (int row = 0; row < N; row++)
        if (grid[row][col] == num)
            return true;
    return false;
}

bool isPresentInRow(int row, int num) {
    for (int col = 0; col < N; col++)
        if (grid[row][col] == num)
            return true;
    return false;
}

bool isPresentInBox(int boxStartRow, int boxStartCol, int num) {
    for (int row = 0; row < 3; row++)
        for (int col = 0; col < 3; col++)
            if (grid[row + boxStartRow][col + boxStartCol] == num)
                return true;
    return false;
}

bool findEmptyPlace(int &row, int &col) {
    for (row = 0; row < N; row++)
        for (col = 0; col < N; col++)
            if (grid[row][col] == 0)
                return true;
    return false;
}

bool isValidPlace(int row, int col, int num) {
    return !isPresentInRow(row, num) && !isPresentInCol(col, num) && !isPresentInBox(row - row % 3, col - col % 3, num);
}

bool solveSudoku(int &solutionsCount, int maxSolutions) {
    int row, col;
    if (!findEmptyPlace(row, col)) {
        solutionsCount++;
        if (solutionsCount >= maxSolutions)
            return true; // Break if the desired number of solutions is reached
        return false;    // Continue searching for more solutions
    }

    for (int num = 1; num <= 9; num++) {
        if (isValidPlace(row, col, num)) {
            grid[row][col] = num;
            if (solveSudoku(solutionsCount, maxSolutions))
                return true;
            grid[row][col] = 0; // turn to unassigned space when conditions are not satisfied
        }
    }
    return false;
}

int main() {
    int solutionsCount = 0;
    int maxSolutions = 2; // Set the desired number of solutions

    solveSudoku(solutionsCount, maxSolutions);

    if (solutionsCount == 1)
        cout << "Unique solution found:" << endl;
    else if (solutionsCount > 1)
        cout << "Multiple solutions found:" << endl;
    else
        cout << "No solution exists" << endl;

    sudokuGrid();

    return 0;
}
