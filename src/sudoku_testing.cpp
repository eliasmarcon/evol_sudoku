#include <ga/GA2DArrayGenome.h>
#include <ga/GASimpleGA.h>
#include <ga/std_stream.h>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>

using namespace std;

const int SUDOKU_SIZE = 9;
const int POPULATION_SIZE = 3000;
const int MAX_GENERATIONS = 30000;

// Objective function for Sudoku
float objective(GAGenome& g) {

    GA2DArrayGenome<int>& genome = (GA2DArrayGenome<int>&)g;
    int fitness = SUDOKU_SIZE * SUDOKU_SIZE * SUDOKU_SIZE * SUDOKU_SIZE; //int fitness = (CHESSBOARD_SIZE * (CHESSBOARD_SIZE - 1)) / 2;

    // Check each row for duplicates
    for (int i = 0; i < SUDOKU_SIZE; i++) {
        for (int j = 0; j < SUDOKU_SIZE; j++) {
            for (int k = j + 1; k < SUDOKU_SIZE; k++) {
                if (genome.gene(i, j) == genome.gene(i, k)) {
                    fitness--;
                }
            }
        }
    }

    // Check each column for duplicates
    for (int j = 0; j < SUDOKU_SIZE; j++) {
        for (int i = 0; i < SUDOKU_SIZE; i++) {
            for (int k = i + 1; k < SUDOKU_SIZE; k++) {
                if (genome.gene(i, j) == genome.gene(k, j)) {
                    fitness--;
                }
            }
        }
    }

    // Check each 3x3 subgrid for duplicates
    for (int blockRow = 0; blockRow < 3; blockRow++) {
        for (int blockCol = 0; blockCol < 3; blockCol++) {
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    int row = blockRow * 3 + i;
                    int col = blockCol * 3 + j;
                    for (int k = i + 1; k < 3; k++) {
                        for (int l = 0; l < 3; l++) {
                            int nextRow = blockRow * 3 + k;
                            int nextCol = blockCol * 3 + l;
                            if (genome.gene(row, col) == genome.gene(nextRow, nextCol)) {
                                fitness--;
                            }
                        }
                    }
                }
            }
        }
    }

    return (float)fitness;
}


//  Initializer
void initializer(GAGenome& g) {
    GA2DArrayGenome<int>& genome = (GA2DArrayGenome<int>&)g;

    // Create an array with values 1 to 9
    int values[SUDOKU_SIZE];
    for (int i = 0; i < SUDOKU_SIZE; ++i) {
        values[i] = i + 1;
    }

    // Seed the random number generator
    std::random_device rd;
    std::mt19937 g_rand(rd());

    // Swap the values
    for (int i = 0; i < SUDOKU_SIZE; ++i) {
        int randomIndex1 = std::uniform_int_distribution<int>(0, SUDOKU_SIZE - 1)(g_rand);
        int randomIndex2 = std::uniform_int_distribution<int>(0, SUDOKU_SIZE - 1)(g_rand);
        std::swap(values[randomIndex1], values[randomIndex2]);
    }

    // Print the shuffled and swapped values
    /*
    cout << "Swapped" << endl;
    for (int i = 0; i < SUDOKU_SIZE; ++i) {
        std::cout << values[i] << " ";
    }
    
    cout << endl;
    cout << endl;
    */

    // Shuffle the values array
    std::shuffle(values, values + SUDOKU_SIZE, g_rand);

    /*
    // Print the shuffled and swapped values
    cout << "Shuffeld" << endl;
    for (int i = 0; i < SUDOKU_SIZE; ++i) {
        std::cout << values[i] << " ";
    }

    cout << endl;
    cout << endl;
    */

    // Store values in a 2D vector
    //std::vector<std::vector<int>> sudokuValues(SUDOKU_SIZE, std::vector<int>(SUDOKU_SIZE, 0));

    // Fill the grid with shuffled and swapped values
    for (int i = 0; i < SUDOKU_SIZE; ++i) {

        // Shuffle the indices of the values array
        std::vector<int> indices(SUDOKU_SIZE);
        std::iota(indices.begin(), indices.end(), 0);  // Fill indices with 0 to SUDOKU_SIZE-1
        std::shuffle(indices.begin(), indices.end(), g_rand);

        for (int j = 0; j < SUDOKU_SIZE; ++j) {
            //cout << indices[j];
            //sudokuValues[i][j] = values[indices[j]];

            genome.gene(i, j, values[indices[j]]);

        }
        //cout << endl;
    }

    /*
    cout << "Sudoku" << endl;
    printSudoku(sudokuValues);
    cout << endl;
    */
}




// Mutator
int mutator(GAGenome& g, float p) {
    GA2DArrayGenome<int>& genome = (GA2DArrayGenome<int>&)g;
    /*
    int nMutations = 0;

    for (int blockRow = 0; blockRow < 3; blockRow++) {
        for (int blockCol = 0; blockCol < 3; blockCol++) {
            // Mutate within each 3x3 subgrid
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (GAFlipCoin(p)) {
                        int row1 = blockRow * 3 + i;
                        int col1 = blockCol * 3 + j;
                        int row2 = blockRow * 3 + rand() % 3;
                        int col2 = blockCol * 3 + rand() % 3;

                        // Swap values within the same 3x3 subgrid
                        int temp = genome.gene(row1, col1);
                        genome.gene(row1, col1, genome.gene(row2, col2));
                        genome.gene(row2, col2, temp);

                        nMutations++;
                    }
                }
            }
        }
    }

    return nMutations;
    */
    
    int nMutations = 0;

    for (int i = 0; i < genome.width(); i++) {
        for (int j = 0; j < genome.height(); j++) {
            if (GAFlipCoin(p)) {
                int row1 = i;
                int col1 = j;
                int row2 = rand() % genome.width();
                int col2 = rand() % genome.height();

                // Swap values between random cells in the entire Sudoku grid
                int temp = genome.gene(row1, col1);
                genome.gene(row1, col1, genome.gene(row2, col2));
                genome.gene(row2, col2, temp);

                nMutations++;
            }
        }
    }

    return nMutations;

}

int crossover(const GAGenome& p1, const GAGenome& p2, GAGenome* c1, GAGenome* c2) {
    GA2DArrayGenome<int>& parent1 = (GA2DArrayGenome<int>&)p1;
    GA2DArrayGenome<int>& parent2 = (GA2DArrayGenome<int>&)p2;

    if (c1 && c2) {
        GA2DArrayGenome<int>& child1 = (GA2DArrayGenome<int>&)*c1;
        GA2DArrayGenome<int>& child2 = (GA2DArrayGenome<int>&)*c2;

        int cut1 = rand() % 3;  // Randomly choose subgrid row to start merging
        int cut2 = rand() % 3;  // Randomly choose subgrid column to start merging

        // Copy values from parents to children
        for (int i = 0; i < SUDOKU_SIZE; ++i) {
            for (int j = 0; j < SUDOKU_SIZE; ++j) {
                int row1 = (i + cut1) % 3;
                int col1 = (j + cut2) % 3;

                int row2 = (i + cut2) % 3;
                int col2 = (j + cut1) % 3;

                child1.gene(i, j, parent1.gene(row1 + cut1 * 3, col1 + cut2 * 3));
                child2.gene(i, j, parent2.gene(row2 + cut2 * 3, col2 + cut1 * 3));
            }
        }

        // Fix the validity of children
        //mutator(*c1, 1.0);
        //mutator(*c2, 1.0);

        return 2;
    } else if (c1) {
        GA2DArrayGenome<int>& child = (GA2DArrayGenome<int>&)*c1;
        int cut = rand() % 3;  // Randomly choose subgrid row to start merging

        // Copy values from parents to child
        for (int i = 0; i < SUDOKU_SIZE; ++i) {
            for (int j = 0; j < SUDOKU_SIZE; ++j) {
                int row = (i + cut) % 3;
                int col = (j + cut) % 3;

                child.gene(i, j, parent1.gene(row + cut * 3, col + cut * 3));
            }
        }

        // Fix the validity of the child
        //mutator(*c1, 1.0);

        return 1;
    } else {
        return 0;
    }
}

/*
// Crossover
int crossover(const GAGenome& p1, const GAGenome& p2, GAGenome* c1, GAGenome* c2) {
    GA2DArrayGenome<int>& parent1 = (GA2DArrayGenome<int>&)p1;
    GA2DArrayGenome<int>& parent2 = (GA2DArrayGenome<int>&)p2;

    if (c1 && c2) {
        GA2DArrayGenome<int>& child1 = (GA2DArrayGenome<int>&)*c1;
        GA2DArrayGenome<int>& child2 = (GA2DArrayGenome<int>&)*c2;

        int cut1 = rand() % SUDOKU_SIZE;
        int cut2 = rand() % SUDOKU_SIZE;

        if (cut1 > cut2) {
            std::swap(cut1, cut2);
        }

        // Copy values from parents to children
        for (int i = 0; i < SUDOKU_SIZE; i++) {
            for (int j = 0; j < SUDOKU_SIZE; j++) {
                if (i < cut1 || i >= cut2) {
                    child1.gene(i, j, parent1.gene(i, j));
                    child2.gene(i, j, parent2.gene(i, j));
                } else {
                    child1.gene(i, j, parent2.gene(i, j));
                    child2.gene(i, j, parent1.gene(i, j));
                }
            }
        }

        // Fix the validity of children
        mutator(*c1, 1.0);
        mutator(*c2, 1.0);

        return 2;
    } else if (c1) {
        // Similar changes as in the two-children case
        GA2DArrayGenome<int>& child = (GA2DArrayGenome<int>&)*c1;
        int cut = rand() % SUDOKU_SIZE;

        // Copy values from parents to child
        for (int i = 0; i < SUDOKU_SIZE; i++) {
            for (int j = 0; j < SUDOKU_SIZE; j++) {
                if (i < cut) {
                    child.gene(i, j, parent1.gene(i, j));
                } else {
                    child.gene(i, j, parent2.gene(i, j));
                }
            }
        }

        // Fix the validity of the child
        mutator(*c1, 1.0);

        return 1;

    } else {
        return 0;
    }
}
*/

// Function to print the Sudoku field
void printSudoku(const std::vector<std::vector<int>>& sudoku) {
    for (int i = 0; i < SUDOKU_SIZE; ++i) {
        if (i > 0 && i % 3 == 0) {
            std::cout << "---------------------" << std::endl;
        }

        for (int j = 0; j < SUDOKU_SIZE; ++j) {
            if (j > 0 && j % 3 == 0) {
                std::cout << "| ";
            }

            std::cout << sudoku[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

string validSudoku(const std::vector<std::vector<int>>& sudoku) {
    int row = 0, col = 0, i = 0, block = 0;
    int count[9];

    for (row = 0; row < 9; ++row) {
        memset(count, 0, 9 * sizeof(int));
        for (col = 0; col < 9; ++col) {
            if (sudoku[row][col] != '.')
                ++count[sudoku[row][col] - 1];
        }
        for (i = 0; i < 9; ++i)
            if (count[i] > 1)
                return "false";
    }

    for (col = 0; col < 9; ++col) {
        memset(count, 0, 9 * sizeof(int));
        for (row = 0; row < 9; ++row) {
            if (sudoku[row][col] != '.')
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
                if (sudoku[row][col] != '.')
                    ++count[sudoku[row][col] - 1];
        for (i = 0; i < 9; ++i)
            if (count[i] > 1)
                return "false";
    }

    return "true";
}

int main() {
    
    srand(static_cast<unsigned int>(time(nullptr)));

    // Initialize a Sudoku field with zeros
    std::vector<std::vector<int>> sudoku(SUDOKU_SIZE, std::vector<int>(SUDOKU_SIZE, 0));

    GA2DArrayGenome<int> genome(SUDOKU_SIZE, SUDOKU_SIZE, objective);
    genome.initializer(initializer);
    genome.mutator(mutator);
    genome.crossover(crossover);

    GASimpleGA ga(genome);
    ga.populationSize(POPULATION_SIZE);
    ga.nGenerations(MAX_GENERATIONS);
    ga.pMutation(0.05); //0.05
    ga.pCrossover(0.9); //0.9
    ga.evolve();

    const GA2DArrayGenome<int>& bestGenome = (GA2DArrayGenome<int>&)ga.statistics().bestIndividual();
    
    cout << "Best solution found: " << endl;
    for (int i = 0; i < SUDOKU_SIZE; i++) {
        for (int j = 0; j < SUDOKU_SIZE; j++){
            cout << bestGenome.gene(i, j) << " ";
            sudoku[i][j] = bestGenome.gene(i, j);
        }
        cout << endl;
    }
    cout << endl;

    cout << "Sudoku Field" << endl;
    printSudoku(sudoku);
    cout << endl;

    string valid = validSudoku(sudoku);
    cout << "Answer to valid Sudoku: " << valid << endl;    


    return 0;
}